version_xml = configure_file(
  input: 'version.xml.in',
  output: 'version.xml',
  configuration: conf
)

gnome.gtkdoc('libcaja-extension',
  main_xml: 'libcaja-extension-docs.xml',
  src_dir: join_paths(meson.project_source_root(), 'libcaja-extension'),
  dependencies: libcaja_extension_dep,
  gobject_typesfile: 'libcaja-extension.types',
  scan_args: '--deprecated-guards=GTK_DISABLE_DEPRECATED',
  install: true
)
