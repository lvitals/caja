[![Translation status](https://translate.fedoraproject.org/widget/mate-desktop/svg-badge.svg)](https://translate.fedoraproject.org/engage/mate-desktop/)
[![Build Status](https://github.com/mate-desktop/caja/actions/workflows/builds.yml/badge.svg?branch=master)](https://github.com/mate-desktop/caja/actions/workflows/builds.yml)
[![Release](https://img.shields.io/github/v/release/mate-desktop/caja)](https://github.com/mate-desktop/caja/releases)

# Caja

Caja is the official file manager for the MATE Desktop Environment. It allows users to browse directories, preview files, launch applications, and manage the desktop background and icons. Caja is designed to be fast, stable, and highly customizable through extensions.

---

## Dependencies

To build Caja from source, you will need the following tools and libraries installed on your system.

### Build Tools & Utilities
* C Compiler (GCC or Clang)
* Autotools (Autoconf, Automake, Libtool, pkg-config)
* Meson Build System (when using Meson build)
* Ninja build backend (when using Meson build)
* Git
* Gettext
* GLib development tools (providing `glib-genmarshal`, `glib-compile-resources`)
* GObject Introspection

### Library Dependencies
* **GLib / GIO / GIO-Unix** (>= 2.58.1 / >= 2.50.0)
* **GTK+ 3** (>= 3.22.0)
* **MATE Desktop Library** (`mate-desktop-2.0` >= 1.17.3)
* **Pango** (>= 1.1.2)
* **Libxml-2.0** (>= 2.4.7)
* **X11 Libraries** (including `x11`, `ice`, `sm`)
* **Libnotify** (>= 0.7.0)
* **Libexif** (optional, >= 0.6.14, for EXIF tag display support)
* **Exempi-2.0** (optional, >= 1.99.5, for XMP metadata support)
* **Gtk Layer Shell & Wayland Client** (optional, >= 0.8.0, for Wayland support)
* **Libselinux** (optional, for SELinux context support)

---

## Building and Installing

### 1. Initialize and Update Submodules
The project relies on external submodules (such as `libegg` under `mate-submodules`) which must be initialized and updated before configuring the build:

```bash
git submodule update --init --recursive
```

### 2. Configure the Build (Meson)
Use Meson to configure the build. It is recommended to use the standard installation directories matching your system layout:

```bash
meson setup build \
  --prefix=/usr \
  --sysconfdir=/etc \
  --localstatedir=/var \
  --buildtype=plain
```

#### Custom Build Options
You can toggle optional features using `-D<option>=<value>`. Available options include:

| Option | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `xmp` | feature | `auto` | Enable XMP support using exempi |
| `exif` | feature | `auto` | Enable EXIF support using libexif |
| `wayland` | feature | `auto` | Enable Wayland support using gtk-layer-shell |
| `selinux` | feature | `auto` | Enable SELinux support |

*Example (disabling Wayland and enabling EXIF support):*
```bash
meson setup build -Dwayland=disabled -Dexif=enabled
```

### 3. Compile the Code
Compile the project using the configured build directory:

```bash
meson compile -C build
```

### 4. Install Caja
Install the compiled binaries and data files to the system:

```bash
sudo meson install -C build
```

---

## Legacy Autotools Build (Reference)

For environments still relying on the legacy Autotools build system:

```bash
./autogen.sh \
  --prefix=/usr \
  --sysconfdir=/etc \
  --localstatedir=/var
make
sudo make install
```

---

## How to Report Bugs

If you encounter any issues or want to request features, please report them to our GitHub issue tracker:

[Caja Issue Tracker](https://github.com/mate-desktop/caja/issues)

For details on contributing, including how to send patches or pull requests, please read the [HACKING](file:///home/leandro/workspaces/mate-desktop/caja/HACKING) file.
