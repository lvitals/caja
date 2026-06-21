libcaja_file_manager_sources = [
  'fm-desktop-icon-view.c',
  'fm-desktop-wayland-bg-dialog.c',
  'fm-directory-view.c',
  'fm-widget-view.c',
  'fm-ditem-page.c',
  'fm-error-reporting.c',
  'fm-icon-container.c',
  'fm-icon-view.c',
  'fm-list-model.c',
  'fm-list-view.c',
  'fm-properties-window.c',
  'fm-tree-model.c',
  'fm-tree-view.c',
]

if enable_empty_view
  libcaja_file_manager_sources += 'fm-empty-view.c'
endif

python3 = find_program('python3')
generate_marshal_script = join_paths(meson.project_source_root(), 'generate-marshal-list.py')

fm_marshal_list = custom_target('fm-marshal.list',
  input: libcaja_file_manager_sources,
  output: 'fm-marshal.list',
  command: [
    python3, generate_marshal_script,
    'fm', '@OUTPUT@', '@INPUT@'
  ]
)

fm_marshal_h = custom_target('fm-marshal.h',
  input: fm_marshal_list,
  output: 'fm-marshal.h',
  command: [glib_genmarshal, '--header', '--prefix=fm_marshal', '@INPUT@'],
  capture: true
)

fm_marshal_c = custom_target('fm-marshal.c',
  input: fm_marshal_list,
  output: 'fm-marshal.c',
  command: [
    python3, '-c',
    'import sys, subprocess; sys.stdout.write("#include " + chr(34) + "fm-marshal.h" + chr(34) + chr(10)); subprocess.run([sys.argv[1], "--body", "--prefix=fm_marshal", sys.argv[2]])',
    glib_genmarshal, '@INPUT@'
  ],
  capture: true
)

libcaja_file_manager = static_library('caja-file-manager',
  [
    libcaja_file_manager_sources,
    fm_marshal_h,
    fm_marshal_c,
  ],
  c_args: [
    '-DCAJA_DATADIR="@0@/caja"'.format(datadir),
    '-DICONDIR="@0@/icons"'.format(datadir),
    '-DPIXMAPDIR="@0@/pixmaps"'.format(datadir),
  ],
  include_directories: include_dirs,
  dependencies: [
    glib_dep, gio_dep, gio_unix_dep, gdk_pixbuf_dep, mate_desktop_dep,
    gthread_dep, pango_dep, gtk_dep, libxml_dep, libnotify_dep, gail_dep,
    gmodule_dep, x11_dep, ice_dep, sm_dep, libm, selinux_dep,
    libcaja_extension_dep, libcaja_private_dep
  ]
)

libcaja_file_manager_dep = declare_dependency(
  link_with: libcaja_file_manager,
  include_directories: include_directories('.')
)

ui_files = [
  'caja-desktop-icon-view-ui.xml',
  'caja-directory-view-ui.xml',
  'caja-icon-view-ui.xml',
  'caja-list-view-ui.xml',
]

install_data(ui_files,
  install_dir: join_paths(datadir, 'caja/ui')
)
