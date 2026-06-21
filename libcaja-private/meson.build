libcaja_private_sources = [
  'caja-autorun.c',
  'caja-bookmark.c',
  'caja-cell-renderer-text-ellipsized.c',
  'caja-clipboard-monitor.c',
  'caja-clipboard.c',
  'caja-column-chooser.c',
  'caja-column-utilities.c',
  'caja-customization-data.c',
  'caja-debug-log.c',
  'caja-default-file-icon.c',
  'caja-desktop-directory-file.c',
  'caja-desktop-directory.c',
  'caja-desktop-icon-file.c',
  'caja-desktop-link-monitor.c',
  'caja-desktop-link.c',
  'caja-desktop-metadata.c',
  'caja-directory-async.c',
  'caja-directory-background.c',
  'caja-directory.c',
  'caja-dnd.c',
  'caja-emblem-utils.c',
  'caja-extensions.c',
  'caja-entry.c',
  'caja-file-changes-queue.c',
  'caja-file-conflict-dialog.c',
  'caja-file-dnd.c',
  'caja-file-operations.c',
  'caja-file-queue.c',
  'caja-file-utilities.c',
  'caja-file.c',
  'caja-global-preferences.c',
  'caja-icon-canvas-item.c',
  'caja-icon-container.c',
  'caja-icon-dnd.c',
  'caja-icon-info.c',
  'caja-keep-last-vertical-box.c',
  'caja-lib-self-check-functions.c',
  'caja-link.c',
  'caja-metadata.c',
  'caja-mime-actions.c',
  'caja-mime-application-chooser.c',
  'caja-module.c',
  'caja-monitor.c',
  'caja-open-with-dialog.c',
  'caja-progress-info.c',
  'caja-program-choosing.c',
  'caja-recent.c',
  'caja-saved-search-file.c',
  'caja-search-directory.c',
  'caja-search-directory-file.c',
  'caja-search-engine.c',
  'caja-search-engine-simple.c',
  'caja-search-engine-beagle.c',
  'caja-search-engine-tracker.c',
  'caja-sidebar-provider.c',
  'caja-sidebar.c',
  'caja-signaller.c',
  'caja-query.c',
  'caja-thumbnails.c',
  'caja-trash-monitor.c',
  'caja-tree-view-drag-dest.c',
  'caja-ui-utilities.c',
  'caja-vfs-directory.c',
  'caja-vfs-file.c',
  'caja-view-factory.c',
  'caja-view.c',
  'caja-window-info.c',
  'caja-window-slot-info.c',
  'caja-undostack-manager.c',
]

python3 = find_program('python3')
generate_marshal_script = join_paths(meson.project_source_root(), 'generate-marshal-list.py')

caja_marshal_list = custom_target('caja-marshal.list',
  input: libcaja_private_sources,
  output: 'caja-marshal.list',
  command: [
    python3, generate_marshal_script,
    'caja', '@OUTPUT@', '@INPUT@'
  ]
)

caja_marshal_h = custom_target('caja-marshal.h',
  input: caja_marshal_list,
  output: 'caja-marshal.h',
  command: [glib_genmarshal, '--header', '--prefix=caja_marshal', '@INPUT@'],
  capture: true
)

caja_marshal_c = custom_target('caja-marshal.c',
  input: caja_marshal_list,
  output: 'caja-marshal.c',
  command: [
    python3, '-c',
    'import sys, subprocess; sys.stdout.write("#include " + chr(34) + "caja-marshal.h" + chr(34) + chr(10)); subprocess.run([sys.argv[1], "--body", "--prefix=caja_marshal", sys.argv[2]])',
    glib_genmarshal, '@INPUT@'
  ],
  capture: true
)

libcaja_private = static_library('caja-private',
  [
    libcaja_private_sources,
    caja_marshal_h,
    caja_marshal_c,
  ],
  include_directories: include_dirs,
  dependencies: [
    glib_dep, gio_dep, gio_unix_dep, gdk_pixbuf_dep, mate_desktop_dep,
    gthread_dep, pango_dep, gtk_dep, libxml_dep, libnotify_dep, gail_dep,
    gmodule_dep, x11_dep, ice_dep, sm_dep, libm, selinux_dep,
    libcaja_extension_dep
  ],
  link_with: [libegg, libeel]
)

libcaja_private_dep = declare_dependency(
  link_with: libcaja_private,
  include_directories: include_directories('.'),
  dependencies: [
    glib_dep, gio_dep, gio_unix_dep, gdk_pixbuf_dep, mate_desktop_dep,
    gthread_dep, pango_dep, gtk_dep, libxml_dep, libnotify_dep, gail_dep,
    gmodule_dep, x11_dep, ice_dep, sm_dep, libm, selinux_dep,
    libcaja_extension_dep
  ]
)

install_data(
  'org.mate.caja.gschema.xml',
  'org.mate.media-handling.gschema.xml',
  install_dir: join_paths(get_option('datadir'), 'glib-2.0', 'schemas')
)

gnome.compile_schemas(
  depend_files: files('org.mate.caja.gschema.xml', 'org.mate.media-handling.gschema.xml')
)
