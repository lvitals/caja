project('caja', 'c',
  version: '1.29.1',
  meson_version: '>= 0.56.0',
  default_options: [
    'warning_level=1',
    'c_std=gnu99',
  ]
)

gnome = import('gnome')
i18n = import('i18n')

cc = meson.get_compiler('c')
glib_genmarshal = find_program('glib-genmarshal')

# Global compiler flags
add_project_arguments('-DHAVE_CONFIG_H', language: 'c')
add_project_arguments('-I' + meson.project_build_root(), language: 'c')

# Version requirements
gdkpixbuf_req = '>= 2.36.5'
glib_req = '>= 2.58.1'
gio_req = '>= 2.50.0'
mate_desktop_req = '>= 1.17.3'
pango_req = '>= 1.1.2'
gtk_req = '>= 3.22.0'
xml_req = '>= 2.4.7'
exif_req = '>= 0.6.14'
exempi_req = '>= 1.99.5'
gail_req = '>= 3.0.0'
notify_req = '>= 0.7.0'

# Standard dependencies
glib_dep = dependency('glib-2.0', version: glib_req)
gio_dep = dependency('gio-2.0', version: gio_req)
gio_unix_dep = dependency('gio-unix-2.0')
gdk_pixbuf_dep = dependency('gdk-pixbuf-2.0', version: gdkpixbuf_req)
mate_desktop_dep = dependency('mate-desktop-2.0', version: mate_desktop_req)
gthread_dep = dependency('gthread-2.0')
pango_dep = dependency('pango', version: pango_req)
gtk_dep = dependency('gtk+-3.0', version: gtk_req)
libxml_dep = dependency('libxml-2.0', version: xml_req)
libnotify_dep = dependency('libnotify', version: notify_req)
gail_dep = dependency('gail-3.0', version: gail_req)
gmodule_dep = dependency('gmodule-2.0')
x11_dep = dependency('x11')
ice_dep = dependency('ice')
sm_dep = dependency('sm')

# Math library
libm = cc.find_library('m', required: false)

# Optional dependencies
# exif
exif_opt = get_option('exif')
have_exif = false
exif_dep = []
if not exif_opt.disabled()
  exif_dep = dependency('libexif', version: exif_req, required: exif_opt.enabled())
  if exif_dep.found()
    have_exif = true
  endif
endif

# exempi / xmp
xmp_opt = get_option('xmp')
have_xmp = false
exempi_dep = []
if not xmp_opt.disabled()
  exempi_dep = dependency('exempi-2.0', version: exempi_req, required: xmp_opt.enabled())
  if exempi_dep.found()
    have_xmp = true
  endif
endif

# Wayland
wayland_opt = get_option('wayland')
have_wayland = false
wayland_deps = []
if not wayland_opt.disabled()
  gtk_layer_shell_dep = dependency('gtk-layer-shell-0', version: '>= 0.8.0', required: wayland_opt.enabled())
  wayland_client_dep = dependency('wayland-client', required: wayland_opt.enabled())
  if gtk_layer_shell_dep.found() and wayland_client_dep.found()
    have_wayland = true
    wayland_deps = [gtk_layer_shell_dep, wayland_client_dep]
  endif
endif

# SELinux
selinux_opt = get_option('selinux')
have_selinux = false
selinux_dep = []
if not selinux_opt.disabled()
  selinux_dep = cc.find_library('selinux', required: selinux_opt.enabled())
  if selinux_dep.found() and cc.has_header('selinux/selinux.h')
    have_selinux = true
  endif
endif

# Self check option
msg_self_check = get_option('self-check')

# Empty view option
enable_empty_view = get_option('empty-view')

# PackageKit option
enable_packagekit = get_option('packagekit')

# Configuration data
conf = configuration_data()
conf.set_quoted('PACKAGE', meson.project_name())
conf.set_quoted('VERSION', meson.project_version())
conf.set_quoted('PACKAGE_NAME', meson.project_name())
conf.set_quoted('PACKAGE_VERSION', meson.project_version())
conf.set_quoted('PACKAGE_BUGREPORT', 'https://github.com/mate-desktop/caja/issues')
conf.set_quoted('PACKAGE_URL', 'https://mate-desktop.org')
conf.set_quoted('PACKAGE_STRING', '@0@ @1@'.format(meson.project_name(), meson.project_version()))
conf.set_quoted('GETTEXT_PACKAGE', meson.project_name())

conf.set('HAVE_GTK_MULTIHEAD', 1)

if have_exif
  conf.set('HAVE_EXIF', 1)
endif
if have_xmp
  conf.set('HAVE_EXEMPI', 1)
endif
if have_wayland
  conf.set('HAVE_WAYLAND', 1)
endif
if have_selinux
  conf.set('HAVE_SELINUX', 1)
endif
if not msg_self_check
  conf.set('EEL_OMIT_SELF_CHECK', 1)
  conf.set('CAJA_OMIT_SELF_CHECK', 1)
endif
if enable_empty_view
  conf.set('ENABLE_EMPTY_VIEW', 1)
endif
if enable_packagekit
  conf.set('ENABLE_PACKAGEKIT', 1)
endif

# Check headers
foreach h : ['sys/mount.h', 'sys/vfs.h', 'sys/param.h', 'malloc.h', 'X11/XF86keysym.h']
  if cc.has_header(h)
    conf.set('HAVE_' + h.underscorify().to_upper(), 1)
  endif
endforeach

# Check functions
if cc.has_function('mallopt')
  conf.set('HAVE_MALLOPT', 1)
endif

# Configuration directories
prefix = get_option('prefix')
datadir = join_paths(prefix, get_option('datadir'))
libdir = join_paths(prefix, get_option('libdir'))
sysconfdir = join_paths(prefix, get_option('sysconfdir'))
includedir = join_paths(prefix, get_option('includedir'))

# Add global compiler options
add_project_arguments([
  '-DDATADIR="@0@"'.format(datadir),
  '-DSYSCONFDIR="@0@"'.format(sysconfdir),
  '-DCAJA_DATADIR="@0@/caja"'.format(datadir),
  '-DCAJA_EXTENSIONDIR="@0@/caja/extensions-2.0"'.format(libdir),
], language: 'c')

conf.set('ENABLE_NLS', 1)
conf.set_quoted('MATELOCALEDIR', join_paths(datadir, 'locale'))

configure_file(
  output: 'config.h',
  configuration: conf
)

include_dirs = [
  include_directories('.'),
  include_directories('mate-submodules'),
  include_directories('eel'),
  include_directories('libcaja-extension'),
  include_directories('libcaja-private'),
]

libegg_deps = [
  glib_dep,
  gtk_dep,
  ice_dep,
  sm_dep,
]

subdir('po')
subdir('mate-submodules/libegg')
libegg = egg
subdir('eel')
subdir('libcaja-extension')
subdir('libcaja-private')
subdir('src')
subdir('data')
subdir('icons')
subdir('docs')
subdir('test')

gnome.post_install(
  glib_compile_schemas: true
)
