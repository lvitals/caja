/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */

/*
 * Caja
 *
 * Copyright (C) 2000 Eazel, Inc.
 *
 * Caja is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * Caja is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Authors: Darin Adler <darin@bentspoon.com>
 */

#include <config.h>

#include <X11/Xatom.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <glib/gi18n.h>
#ifdef HAVE_WAYLAND
#include <gdk/gdkwayland.h>
#include <gtk-layer-shell/gtk-layer-shell.h>
#endif
#include <eel/eel-background.h>
#include <eel/eel-vfs-extensions.h>

#include <libcaja-private/caja-file-utilities.h>
#include <libcaja-private/caja-icon-names.h>

#include "caja-desktop-window.h"
#include "caja-window-private.h"
#include "caja-actions.h"

/* Tell screen readers that this is a desktop window */

static GType caja_desktop_window_accessible_get_type (void);

G_DEFINE_TYPE (CajaDesktopWindowAccessible, caja_desktop_window_accessible,
               GTK_TYPE_WINDOW_ACCESSIBLE);

static AtkAttributeSet *
desktop_get_attributes (AtkObject *accessible)
{
    AtkAttributeSet *attributes;
    AtkAttribute *is_desktop;

    attributes = ATK_OBJECT_CLASS (caja_desktop_window_accessible_parent_class)->get_attributes (accessible);

    is_desktop = g_malloc (sizeof (AtkAttribute));
    is_desktop->name = g_strdup ("is-desktop");
    is_desktop->value = g_strdup ("true");

    attributes = g_slist_append (attributes, is_desktop);

    return attributes;
}

static void
caja_desktop_window_accessible_init (CajaDesktopWindowAccessible *window)
{
}

static void
caja_desktop_window_accessible_class_init (CajaDesktopWindowAccessibleClass *klass)
{
    AtkObjectClass *aclass = ATK_OBJECT_CLASS (klass);

    aclass->get_attributes = desktop_get_attributes;
}

struct _CajaDesktopWindowPrivate
{
    gulong size_changed_id;
    gulong monitor_added_id;
    gulong monitor_removed_id;
    guint geometry_update_id;
    GList *monitors;

    gboolean loaded;
    GdkMonitor *monitor;
};

G_DEFINE_TYPE_WITH_PRIVATE (CajaDesktopWindow, caja_desktop_window,
               CAJA_TYPE_SPATIAL_WINDOW);

static GdkMonitor *
get_fallback_monitor (GdkDisplay *display)
{
    GdkMonitor *monitor;
    GdkMonitor *largest = NULL;
    int i, n_monitors, max_pixels;

    n_monitors = gdk_display_get_n_monitors (display);
    if (n_monitors == 0) {
        return NULL;
    }

    monitor = gdk_display_get_primary_monitor (display);
    if (monitor != NULL) {
        return monitor;
    }

    /* Prefer the monitor at (0,0) */
    for (i = 0; i < n_monitors; i++) {
        GdkRectangle geometry = {0};

        monitor = gdk_display_get_monitor (display, i);
        gdk_monitor_get_geometry (monitor, &geometry);
        if (geometry.x == 0 && geometry.y == 0) {
            return monitor;
        }
    }

    /* Then prefer the largest monitor */
    max_pixels = 0;
    for (i = 0; i < n_monitors; i++) {
        GdkRectangle geometry = {0};
        int pixels;

        monitor = gdk_display_get_monitor (display, i);
        gdk_monitor_get_geometry (monitor, &geometry);
        pixels = geometry.width * geometry.height;
        if (pixels > max_pixels) {
            max_pixels = pixels;
            largest = monitor;
        }
    }

    if (largest != NULL) {
        return largest;
    }

    /* Last resort */
    return gdk_display_get_monitor (display, 0);
}

static void
caja_desktop_window_apply_geometry (CajaDesktopWindow *window)
{
    GdkDisplay *display;
    GdkRectangle geometry = {0};
    int width_request;
    int height_request;

    display = gtk_widget_get_display (GTK_WIDGET (window));
    if (GDK_IS_X11_DISPLAY (display)) {
        GdkWindow *root_window;
        GdkScreen *screen;

        screen = gtk_window_get_screen (GTK_WINDOW (window));
        root_window = gdk_screen_get_root_window (screen);
        gdk_window_get_geometry (root_window, NULL, NULL,
                                 &width_request, &height_request);
    } else {
        GdkMonitor *monitor = window->details->monitor;
        if (monitor == NULL) {
            monitor = get_fallback_monitor (display);
        }
        if (monitor != NULL) {
            gdk_monitor_get_geometry (monitor, &geometry);
        }

        width_request = MAX (geometry.width, 1);
        height_request = MAX (geometry.height, 1);
    }

    gtk_widget_set_size_request (GTK_WIDGET (window),
                                 width_request, height_request);
    gtk_window_resize (GTK_WINDOW (window), width_request, height_request);
    gtk_widget_queue_resize (GTK_WIDGET (window));
    gtk_widget_queue_draw (GTK_WIDGET (window));
}

static gboolean
caja_desktop_window_update_geometry_idle (gpointer data)
{
    CajaDesktopWindow *window = CAJA_DESKTOP_WINDOW (data);

    window->details->geometry_update_id = 0;
    caja_desktop_window_apply_geometry (window);

    return G_SOURCE_REMOVE;
}

static void
caja_desktop_window_queue_geometry_update (CajaDesktopWindow *window)
{
    if (window->details->geometry_update_id != 0) {
        return;
    }

    window->details->geometry_update_id =
        g_idle_add (caja_desktop_window_update_geometry_idle, window);
}

static void
caja_desktop_window_monitor_changed (GObject    *object,
                                     GParamSpec *pspec,
                                     gpointer    user_data)
{
    caja_desktop_window_queue_geometry_update (CAJA_DESKTOP_WINDOW (user_data));
}

static void
caja_desktop_window_disconnect_monitor_signals (CajaDesktopWindow *window)
{
    GList *l;

    for (l = window->details->monitors; l != NULL; l = l->next) {
        g_signal_handlers_disconnect_by_func (l->data,
                                              caja_desktop_window_monitor_changed,
                                              window);
        g_object_unref (l->data);
    }

    g_list_free (window->details->monitors);
    window->details->monitors = NULL;
}

static void
caja_desktop_window_connect_monitor_signals (CajaDesktopWindow *window)
{
    GdkDisplay *display;
    int i, n_monitors;

    caja_desktop_window_disconnect_monitor_signals (window);

    display = gtk_widget_get_display (GTK_WIDGET (window));
    n_monitors = gdk_display_get_n_monitors (display);
    for (i = 0; i < n_monitors; i++) {
        GdkMonitor *monitor = gdk_display_get_monitor (display, i);

        if (monitor == NULL) {
            continue;
        }

        window->details->monitors =
            g_list_prepend (window->details->monitors, g_object_ref (monitor));
        g_signal_connect (monitor, "notify",
                          G_CALLBACK (caja_desktop_window_monitor_changed),
                          window);
    }
}

static void
caja_desktop_window_init (CajaDesktopWindow *window)
{
    GtkAction *action;
    AtkObject *accessible;

    window->details = caja_desktop_window_get_instance_private (window);

    GtkStyleContext *context;

    context = gtk_widget_get_style_context (GTK_WIDGET (window));
    gtk_style_context_add_class (context, "caja-desktop-window");

    gtk_window_move (GTK_WINDOW (window), 0, 0);

    /* shouldn't really be needed given our semantic type
     * of _NET_WM_TYPE_DESKTOP, but why not
     */
    gtk_window_set_resizable (GTK_WINDOW (window),
                              FALSE);

    g_object_set_data (G_OBJECT (window), "is_desktop_window",
                       GINT_TO_POINTER (1));

    gtk_widget_hide (CAJA_WINDOW (window)->details->statusbar);
    gtk_widget_hide (CAJA_WINDOW (window)->details->menubar);

    /* Don't allow close action on desktop */
    G_GNUC_BEGIN_IGNORE_DEPRECATIONS;
    action = gtk_action_group_get_action (CAJA_WINDOW (window)->details->main_action_group,
                                          CAJA_ACTION_CLOSE);
    gtk_action_set_sensitive (action, FALSE);
    G_GNUC_END_IGNORE_DEPRECATIONS;

    /* Set the accessible name so that it doesn't inherit the cryptic desktop URI. */
    accessible = gtk_widget_get_accessible (GTK_WIDGET (window));

    if (accessible) {
        atk_object_set_name (accessible, _("Desktop"));
    }
}

static gint
caja_desktop_window_delete_event (CajaDesktopWindow *window)
{
    /* Returning true tells GTK+ not to delete the window. */
    return TRUE;
}

void
caja_desktop_window_update_directory (CajaDesktopWindow *window)
{
    GFile *location;

    g_assert (CAJA_IS_DESKTOP_WINDOW (window));

    location = g_file_new_for_uri (EEL_DESKTOP_URI);
    caja_window_go_to (CAJA_WINDOW (window), location);
    window->details->loaded = TRUE;

    g_object_unref (location);
}

static void
caja_desktop_window_screen_size_changed (GdkScreen             *screen,
        CajaDesktopWindow *window)
{
    caja_desktop_window_queue_geometry_update (window);
}

CajaDesktopWindow *
caja_desktop_window_new (CajaApplication *application,
                         GdkScreen           *screen)
{
    return caja_desktop_window_new_for_monitor (application, screen, NULL);
}

CajaDesktopWindow *
caja_desktop_window_new_for_monitor (CajaApplication *application,
                                     GdkScreen           *screen,
                                     GdkMonitor          *monitor)
{
    CajaDesktopWindow *window;
    GdkMonitor *target_monitor = NULL;
    int width_request, height_request;
    int scale;

    GdkDisplay *display = gdk_screen_get_display (screen);
    if (GDK_IS_X11_DISPLAY (display))
    {
        scale = gdk_window_get_scale_factor (gdk_screen_get_root_window (screen));
        width_request = WidthOfScreen (gdk_x11_screen_get_xscreen (screen)) / scale;
        height_request = HeightOfScreen (gdk_x11_screen_get_xscreen (screen)) / scale;
    }
    else
    {
        GdkRectangle geometry = {0};
        target_monitor = monitor ? monitor : get_fallback_monitor (display);
        if (target_monitor != NULL) {
            gdk_monitor_get_geometry (target_monitor, &geometry);
        }
        width_request = geometry.width;
        height_request = geometry.height;
    }

    window = CAJA_DESKTOP_WINDOW
             (gtk_widget_new (caja_desktop_window_get_type(),
                              "app", application,
                              "width_request", width_request,
                              "height_request", height_request,
                              "screen", screen,
                              "decorated", FALSE,
                              NULL));
    window->details->monitor = target_monitor;
    g_object_set_data (G_OBJECT (window), "caja-desktop-monitor", target_monitor);

    /* Stop wrong desktop window size in GTK 3.20*/
    /* We don't want to set a default size, which the parent does, since this */
    /* will cause the desktop window to open at the wrong size in gtk 3.20 */
    gtk_window_set_default_size (GTK_WINDOW (window), -1, -1);

    /*For wayland only
     *Code taken from gtk-layer-shell simple-example.c
     */
#ifdef HAVE_WAYLAND
    if (GDK_IS_WAYLAND_DISPLAY (display))
    {
        GtkWindow *gtkwin;
        gtkwin = (GTK_WINDOW(window));

        /* Before the window is first realized, set it up to be a layer surface */
        gtk_layer_init_for_window (gtkwin);

        if (target_monitor != NULL) {
            gtk_layer_set_monitor (gtkwin, target_monitor);
        }

        /* Order below normal windows */
        gtk_layer_set_layer (gtkwin, GTK_LAYER_SHELL_LAYER_BACKGROUND);

        gtk_layer_set_namespace (gtkwin, "desktop");

        /*Anchor the desktop to all four corners
         *This is much simpler than on x11 and
         *should always render the desktop across
         *all of the screen
         */
        gtk_layer_set_anchor (gtkwin, GTK_LAYER_SHELL_EDGE_TOP, TRUE);
        gtk_layer_set_anchor (gtkwin, GTK_LAYER_SHELL_EDGE_BOTTOM, TRUE);
        gtk_layer_set_anchor (gtkwin, GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
        gtk_layer_set_anchor (gtkwin, GTK_LAYER_SHELL_EDGE_RIGHT, TRUE);

        /*Enable keyboard use on the desktop*/
        gtk_layer_set_keyboard_mode (gtkwin, GTK_LAYER_SHELL_KEYBOARD_MODE_ON_DEMAND);
    }
#endif
    /* Connect signals before realize/show */
    g_signal_connect (window, "delete_event", G_CALLBACK (caja_desktop_window_delete_event), NULL);

    GdkWindow *gdkwin;
    if (GDK_IS_X11_DISPLAY (display)) {
        gtk_widget_realize (GTK_WIDGET (window));
        gdkwin = gtk_widget_get_window (GTK_WIDGET (window));
        if (gdkwin != NULL) {
            if (gdk_window_ensure_native (gdkwin)) {
                Display *disp = GDK_DISPLAY_XDISPLAY (gdk_window_get_display (gdkwin));
                XClassHint *xch = XAllocClassHint ();
                xch->res_name = "desktop_window";
                xch->res_class = "Caja";
                XSetClassHint (disp, GDK_WINDOW_XID (gdkwin), xch);
                XFree (xch);
            }
            gdk_window_set_title (gdkwin, _("Desktop"));
        }
    } else {
        /* Wayland: explicit realize to trigger the realize signal
         * and connect mate-bg before mapping the window */
        gtk_widget_realize (GTK_WIDGET (window));
    }

    /* Point window at the desktop folder.
     * Note that caja_desktop_window_init is too early to do this.
     */
    caja_desktop_window_update_directory (window);

    return window;
}

static void
map (GtkWidget *widget)
{
    /* Chain up to realize our children */
    GTK_WIDGET_CLASS (caja_desktop_window_parent_class)->map (widget);
    gdk_window_lower (gtk_widget_get_window (widget));
}

static void
unrealize (GtkWidget *widget)
{
    CajaDesktopWindow *window;
    CajaDesktopWindowPrivate *details;
    GdkWindow *root_window;
    GdkDisplay *display;

    window = CAJA_DESKTOP_WINDOW (widget);
    details = window->details;
    display = gtk_widget_get_display (widget);

    /*Avoid root window on wayland-it's not supposed to work*/
    if (GDK_IS_X11_DISPLAY (display))
    {
        root_window = gdk_screen_get_root_window (
                      gtk_window_get_screen (GTK_WINDOW (window)));

        gdk_property_delete (root_window,
                             gdk_atom_intern ("CAJA_DESKTOP_WINDOW_ID", TRUE));
    }

    if (details->size_changed_id != 0) {
        g_signal_handler_disconnect (gtk_window_get_screen (GTK_WINDOW (window)),
                         details->size_changed_id);
        details->size_changed_id = 0;
    }
    if (details->monitor_added_id != 0) {
        g_signal_handler_disconnect (display, details->monitor_added_id);
        details->monitor_added_id = 0;
    }
    if (details->monitor_removed_id != 0) {
        g_signal_handler_disconnect (display, details->monitor_removed_id);
        details->monitor_removed_id = 0;
    }
    if (details->geometry_update_id != 0) {
        g_source_remove (details->geometry_update_id);
        details->geometry_update_id = 0;
    }
    caja_desktop_window_disconnect_monitor_signals (window);

    GTK_WIDGET_CLASS (caja_desktop_window_parent_class)->unrealize (widget);
}

/*This should only be reached in x11*/
static void
set_wmspec_desktop_hint (GdkWindow *window)
{
    GdkAtom atom;

    atom = gdk_atom_intern ("_NET_WM_WINDOW_TYPE_DESKTOP", FALSE);

    gdk_property_change (window,
                         gdk_atom_intern ("_NET_WM_WINDOW_TYPE", FALSE),
                         gdk_x11_xatom_to_atom (XA_ATOM), 32,
                         GDK_PROP_MODE_REPLACE, (guchar *) &atom, 1);
}

/*This should only be reached in x11*/
static void
set_desktop_window_id (CajaDesktopWindow *window,
                       GdkWindow             *gdkwindow)
{
    /* Tuck the desktop windows xid in the root to indicate we own the desktop in on x11
     */
    Window window_xid;
    GdkWindow *root_window;

    root_window = gdk_screen_get_root_window (
                      gtk_window_get_screen (GTK_WINDOW (window)));
    window_xid = GDK_WINDOW_XID (gdkwindow);

    gdk_property_change (root_window,
                         gdk_atom_intern ("CAJA_DESKTOP_WINDOW_ID", FALSE),
                         gdk_x11_xatom_to_atom (XA_WINDOW), 32,
                         GDK_PROP_MODE_REPLACE, (guchar *) &window_xid, 1);
}

static void
realize (GtkWidget *widget)
{
    CajaDesktopWindow *window;
    CajaDesktopWindowPrivate *details;
    window = CAJA_DESKTOP_WINDOW (widget);
    details = window->details;
    GdkDisplay *display;

    /* Make sure we get keyboard events */
    display = gtk_widget_get_display (widget);
    if (GDK_IS_X11_DISPLAY (display))
        gtk_widget_set_events (widget, gtk_widget_get_events (widget)
                               | GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);

    /*Do the work of realizing. */
    GTK_WIDGET_CLASS (caja_desktop_window_parent_class)->realize (widget);

    /* This is the new way to set up the desktop window in x11 but not for wayland */
    display = gtk_widget_get_display (widget);
    if (GDK_IS_X11_DISPLAY (display))
    {
        set_wmspec_desktop_hint (gtk_widget_get_window (widget));
        set_desktop_window_id (window, gtk_widget_get_window (widget));
    }

    details->size_changed_id =
        g_signal_connect (gtk_window_get_screen (GTK_WINDOW (window)), "size_changed",
                          G_CALLBACK (caja_desktop_window_screen_size_changed), window);

    if (GDK_IS_WAYLAND_DISPLAY (display)) {
        caja_desktop_window_connect_monitor_signals (window);
    }
}

/* Should only reached in x11*/
static gboolean
draw (GtkWidget *widget,
      cairo_t   *cr)
{
    g_assert (GDK_IS_X11_DISPLAY (gdk_display_get_default()));
    eel_background_draw (widget, cr);
    return GTK_WIDGET_CLASS (caja_desktop_window_parent_class)->draw (widget, cr);
}

static CajaIconInfo *
real_get_icon (CajaWindow *window,
               CajaWindowSlot *slot)
{
    gint scale = gtk_widget_get_scale_factor (GTK_WIDGET (window));
    return caja_icon_info_lookup_from_name (CAJA_ICON_DESKTOP, 48, scale);
}

static void
caja_desktop_window_class_init (CajaDesktopWindowClass *klass)
{
    GtkWidgetClass *wclass = GTK_WIDGET_CLASS (klass);
    CajaWindowClass *nclass = CAJA_WINDOW_CLASS (klass);

    wclass->realize = realize;
    wclass->unrealize = unrealize;
    wclass->map = map;
   /*Drawing the desktop background from here gives a black background in wayland
    *So manage desktop background from the icon container as in navigation windows
    */
    if (GDK_IS_X11_DISPLAY (gdk_display_get_default()))
        wclass->draw = draw;

    gtk_widget_class_set_accessible_type (wclass, CAJA_TYPE_DESKTOP_WINDOW_ACCESSIBLE);

    nclass->window_type = CAJA_WINDOW_DESKTOP;
    nclass->get_icon = real_get_icon;
}

gboolean
caja_desktop_window_loaded (CajaDesktopWindow *window)
{
    return window->details->loaded;
}
