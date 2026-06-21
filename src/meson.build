subdir('file-manager')

# Compile GResource XML
caja_resources = gnome.compile_resources('caja-resources',
  '../data/caja-resources.gresource.xml',
  source_dir: '../data',
  c_name: 'caja'
)

# GDBus Codegen for freedesktop DBus interfaces
dbus_freedesktop = gnome.gdbus_codegen('caja-freedesktop-generated',
  '../data/freedesktop-dbus-interfaces.xml',
  interface_prefix: 'org.freedesktop.',
  namespace: 'CajaFreedesktop',
  object_manager: true
)

caja_sources = [
  'caja-actions.h',
  'caja-application.c',
  'caja-bookmark-list.c',
  'caja-bookmarks-sidebar.c',
  'caja-bookmarks-window.c',
  'caja-connect-server-dialog.c',
  'caja-connect-server-dialog-nonmain.c',
  'caja-connect-server-operation.c',
  'caja-desktop-window.c',
  'caja-emblem-sidebar.c',
  'caja-file-management-properties.c',
  'caja-freedesktop-dbus.c',
  'caja-history-sidebar.c',
  'caja-image-properties-page.c',
  'caja-information-panel.c',
  'caja-location-bar.c',
  'caja-location-dialog.c',
  'caja-location-entry.c',
  'caja-main.c',
  'caja-navigation-action.c',
  'caja-navigation-window-menus.c',
  'caja-navigation-window.c',
  'caja-navigation-window-pane.c',
  'caja-navigation-window-slot.c',
  'caja-notebook.c',
  'caja-notes-viewer.c',
  'caja-pathbar.c',
  'caja-places-sidebar.c',
  'caja-property-browser.c',
  'caja-query-editor.c',
  'caja-search-bar.c',
  'caja-self-check-functions.c',
  'caja-side-pane.c',
  'caja-sidebar-title.c',
  'caja-spatial-window.c',
  'caja-trash-bar.c',
  'caja-view-as-action.c',
  'caja-window-bookmarks.c',
  'caja-window-manage-views.c',
  'caja-window-menus.c',
  'caja-window-pane.c',
  'caja-window-slot.c',
  'caja-window-toolbars.c',
  'caja-window.c',
  'caja-x-content-bar.c',
  'caja-zoom-action.c',
  'caja-zoom-control.c',
]

python3 = find_program('python3')
generate_marshal_script = join_paths(meson.project_source_root(), 'generate-marshal-list.py')

caja_src_marshal_list = custom_target('caja-src-marshal.list',
  input: caja_sources,
  output: 'caja-src-marshal.list',
  command: [
    python3, generate_marshal_script,
    'caja_src', '@OUTPUT@', '@INPUT@'
  ]
)

caja_src_marshal_h = custom_target('caja-src-marshal.h',
  input: caja_src_marshal_list,
  output: 'caja-src-marshal.h',
  command: [glib_genmarshal, '--header', '--prefix=caja_src_marshal', '@INPUT@'],
  capture: true
)

caja_src_marshal_c = custom_target('caja-src-marshal.c',
  input: caja_src_marshal_list,
  output: 'caja-src-marshal.c',
  command: [
    python3, '-c',
    'import sys, subprocess; sys.stdout.write("#include " + chr(34) + "caja-src-marshal.h" + chr(34) + chr(10)); subprocess.run([sys.argv[1], "--body", "--prefix=caja_src_marshal", sys.argv[2]])',
    glib_genmarshal, '@INPUT@'
  ],
  capture: true
)

# Common dependencies for src targets
src_deps = [
  glib_dep, gio_dep, gio_unix_dep, gdk_pixbuf_dep, mate_desktop_dep,
  gthread_dep, pango_dep, gtk_dep, libxml_dep, libnotify_dep, gail_dep,
  gmodule_dep, x11_dep, ice_dep, sm_dep, libm, selinux_dep,
  exif_dep, exempi_dep, wayland_deps
]

executable('caja',
  [
    caja_sources,
    caja_resources,
    dbus_freedesktop,
    caja_src_marshal_h,
    caja_src_marshal_c,
  ],
  c_args: [
    '-DLIBDIR="@0@"'.format(libdir),
    '-DCAJA_DATADIR="@0@/caja"'.format(datadir),
    '-DCAJA_PIXMAPDIR="@0@/pixmaps/caja"'.format(datadir),
    '-DPREFIX="@0@"'.format(prefix),
    '-DVERSION="@0@"'.format(meson.project_version()),
  ],
  include_directories: include_dirs,
  dependencies: src_deps,
  link_with: [libcaja_extension, libcaja_private, libcaja_file_manager],
  install: true
)

executable('caja-file-management-properties',
  [
    'caja-file-management-properties.c',
    'caja-file-management-properties.h',
    'caja-file-management-properties-main.c',
    caja_resources,
  ],
  c_args: [
    '-DVERSION="@0@"'.format(meson.project_version()),
  ],
  include_directories: include_dirs,
  dependencies: src_deps,
  link_with: [libcaja_extension, libcaja_private, libcaja_file_manager],
  install: true
)

executable('caja-autorun-software',
  'caja-autorun-software.c',
  c_args: [
    '-DVERSION="@0@"'.format(meson.project_version()),
  ],
  include_directories: include_dirs,
  dependencies: src_deps,
  link_with: [libcaja_extension, libcaja_private, libcaja_file_manager],
  install: true
)

executable('caja-connect-server',
  [
    'caja-bookmark-list.c',
    'caja-connect-server-dialog.c',
    'caja-connect-server-dialog-main.c',
    'caja-connect-server-operation.c',
  ],
  c_args: [
    '-DVERSION="@0@"'.format(meson.project_version()),
  ],
  include_directories: include_dirs,
  dependencies: src_deps,
  link_with: [libcaja_extension, libcaja_private, libcaja_file_manager],
  install: true
)

install_data('caja.about',
  install_dir: join_paths(datadir, 'caja')
)

install_data(
  [
    'caja-shell-ui.xml',
    'caja-navigation-window-ui.xml',
    'caja-spatial-window-ui.xml',
  ],
  install_dir: join_paths(datadir, 'caja/ui')
)
