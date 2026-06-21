desktop_names = [
  'caja',
  'caja-home',
  'caja-computer',
  'caja-folder-handler',
  'caja-file-management-properties',
  'caja-browser',
  'caja-autorun-software',
  'mate-network-scheme'
]

desktop_conf = configuration_data()
desktop_conf.set('bindir', join_paths(prefix, get_option('bindir')))

foreach name : desktop_names
  desktop_in = configure_file(
    input: name + '.desktop.in.in',
    output: name + '.desktop.in',
    configuration: desktop_conf
  )
  
  i18n.merge_file(
    input: desktop_in,
    output: name + '.desktop',
    type: 'desktop',
    po_dir: join_paths(meson.project_source_root(), 'po'),
    install: true,
    install_dir: join_paths(datadir, 'applications')
  )
endforeach

# XML Mime Database
i18n.merge_file(
  input: 'caja.xml.in',
  output: 'caja.xml',
  type: 'xml',
  po_dir: join_paths(meson.project_source_root(), 'po'),
  install: true,
  install_dir: join_paths(datadir, 'mime', 'packages')
)

# DBus Service file
service_conf = configuration_data()
service_conf.set('bindir', join_paths(prefix, get_option('bindir')))
configure_file(
  input: 'org.mate.freedesktop.FileManager1.service.in',
  output: 'org.mate.freedesktop.FileManager1.service',
  configuration: service_conf,
  install: true,
  install_dir: join_paths(datadir, 'dbus-1', 'services')
)

# Caja DATA files
cajadata_DATA = [
  'browser.xml',
  'caja-extras.placeholder',
  'caja-suggested.placeholder',
  'caja.css',
  'caja-desktop.css',
  'a11y-caja-desktop-base.css',
  'caja-desktop-ContrastHigh.css',
  'caja-desktop-ContrastHighInverse.css',
  'caja-desktop-HighContrast.css',
  'caja-desktop-HighContrastInverse.css',
]
install_data(cajadata_DATA,
  install_dir: join_paths(datadir, 'caja')
)

# App Data XML file
appdata_conf = configuration_data()
appdata_conf.set('PACKAGE_URL', 'https://mate-desktop.org')
appdata_in = configure_file(
  input: 'caja.appdata.xml.in.in',
  output: 'caja.appdata.xml.in',
  configuration: appdata_conf
)

i18n.merge_file(
  input: appdata_in,
  output: 'caja.appdata.xml',
  type: 'xml',
  po_dir: join_paths(meson.project_source_root(), 'po'),
  install: true,
  install_dir: join_paths(datadir, 'metainfo')
)

subdir('icons')
subdir('patterns')
