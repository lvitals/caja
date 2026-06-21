eel_sources = [
  'eel-accessibility.c',
  'eel-art-extensions.c',
  'eel-art-gtk-extensions.c',
  'eel-background.c',
  'eel-background-box.c',
  'eel-canvas.c',
  'eel-canvas-rect-ellipse.c',
  'eel-debug.c',
  'eel-editable-label.c',
  'eel-gdk-extensions.c',
  'eel-gdk-pixbuf-extensions.c',
  'eel-glib-extensions.c',
  'eel-mate-extensions.c',
  'eel-graphic-effects.c',
  'eel-gtk-container.c',
  'eel-gtk-extensions.c',
  'eel-image-table.c',
  'eel-labeled-image.c',
  'eel-lib-self-check-functions.c',
  'eel-self-checks.c',
  'eel-stock-dialogs.c',
  'eel-string.c',
  'eel-vfs-extensions.c',
  'eel-wrap-table.c',
  'eel-xml-extensions.c',
]

python3 = find_program('python3')
generate_marshal_script = join_paths(meson.project_source_root(), 'generate-marshal-list.py')

eel_marshal_list = custom_target('eel-marshal.list',
  input: eel_sources,
  output: 'eel-marshal.list',
  command: [
    python3, generate_marshal_script,
    'eel', '@OUTPUT@', '@INPUT@'
  ]
)

eel_marshal_h = custom_target('eel-marshal.h',
  input: eel_marshal_list,
  output: 'eel-marshal.h',
  command: [glib_genmarshal, '--header', '--prefix=eel_marshal', '@INPUT@'],
  capture: true
)

eel_marshal_c = custom_target('eel-marshal.c',
  input: eel_marshal_list,
  output: 'eel-marshal.c',
  command: [
    python3, '-c',
    'import sys, subprocess; sys.stdout.write("#include " + chr(34) + "eel-marshal.h" + chr(34) + chr(10)); subprocess.run([sys.argv[1], "--body", "--prefix=eel_marshal", sys.argv[2]])',
    glib_genmarshal, '@INPUT@'
  ],
  capture: true
)

libeel = static_library('eel-2',
  [
    eel_sources,
    eel_marshal_h,
    eel_marshal_c,
  ],
  c_args: [
    '-DG_LOG_DOMAIN="Eel"',
    '-DSOURCE_DATADIR="@0@/data"'.format(meson.project_source_root()),
  ],
  include_directories: include_dirs,
  dependencies: [
    glib_dep, gio_dep, gio_unix_dep, gdk_pixbuf_dep, mate_desktop_dep,
    gthread_dep, pango_dep, gtk_dep, libxml_dep, libnotify_dep, gail_dep,
    gmodule_dep, x11_dep, ice_dep, sm_dep, libm
  ]
)
