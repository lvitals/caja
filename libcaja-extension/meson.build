libcaja_extension_headers = [
  'caja-column-provider.h',
  'caja-column.h',
  'caja-extension-types.h',
  'caja-widget-view-provider.h',
  'caja-file-info.h',
  'caja-info-provider.h',
  'caja-location-widget-provider.h',
  'caja-menu-item.h',
  'caja-menu-provider.h',
  'caja-property-page-provider.h',
  'caja-property-page.h',
  'caja-menu.h',
  'caja-configurable.h',
]

libcaja_extension_sources = [
  'caja-column-provider.c',
  'caja-column.c',
  'caja-widget-view-provider.c',
  'caja-extension-types.c',
  'caja-file-info.c',
  'caja-info-provider.c',
  'caja-location-widget-provider.c',
  'caja-menu-item.c',
  'caja-menu-provider.c',
  'caja-property-page-provider.c',
  'caja-property-page.c',
  'caja-menu.c',
  'caja-configurable.c',
]

libcaja_extension = shared_library('caja-extension',
  libcaja_extension_sources,
  version: '5.0.0',
  soversion: '5',
  include_directories: include_dirs,
  dependencies: [glib_dep, gio_dep, gtk_dep],
  install: true
)

libcaja_extension_dep = declare_dependency(
  link_with: libcaja_extension,
  include_directories: include_directories('.')
)

install_headers(libcaja_extension_headers, subdir: 'caja/libcaja-extension')

# GObject Introspection
introspection_opt = get_option('introspection')
if not introspection_opt.disabled()
  introspection_dep = dependency('gobject-introspection-1.0', required: introspection_opt.enabled())
  g_ir_scanner = find_program('g-ir-scanner', required: introspection_opt.enabled())
  if introspection_dep.found() and g_ir_scanner.found()
    gnome.generate_gir(libcaja_extension,
      sources: [libcaja_extension_sources, libcaja_extension_headers],
      namespace: 'Caja',
      nsversion: '2.0',
      identifier_prefix: 'Caja',
      symbol_prefix: 'caja',
      export_packages: 'libcaja-extension',
      includes: ['Gtk-3.0', 'Gio-2.0', 'GLib-2.0'],
      install: true,
      extra_args: ['--warn-all']
    )
  endif
endif

# Pkgconfig file
pc_conf = configuration_data()
pc_conf.set('prefix', prefix)
pc_conf.set('exec_prefix', prefix)
pc_conf.set('libdir', libdir)
pc_conf.set('includedir', includedir)
pc_conf.set('VERSION', meson.project_version())
configure_file(
  input: 'libcaja-extension.pc.in',
  output: 'libcaja-extension.pc',
  configuration: pc_conf,
  install: true,
  install_dir: join_paths(get_option('libdir'), 'pkgconfig')
)
