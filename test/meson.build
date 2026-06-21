test_programs = [
  { 'name': 'test-caja-copy', 'sources': ['test-copy.c', 'test.c'] },
  { 'name': 'test-caja-wrap-table', 'sources': ['test-caja-wrap-table.c', 'test.c'] },
  { 'name': 'test-caja-search-engine', 'sources': ['test-caja-search-engine.c'] },
  { 'name': 'test-caja-directory-async', 'sources': ['test-caja-directory-async.c'] },
  { 'name': 'test-eel-background', 'sources': ['test-eel-background.c'] },
  { 'name': 'test-eel-editable-label', 'sources': ['test-eel-editable-label.c'] },
  { 'name': 'test-eel-image-table', 'sources': ['test-eel-image-table.c', 'test.c'] },
  { 'name': 'test-eel-labeled-image', 'sources': ['test-eel-labeled-image.c', 'test.c'] },
  { 'name': 'test-eel-pixbuf-scale', 'sources': ['test-eel-pixbuf-scale.c', 'test.c'] },
]

foreach t : test_programs
  executable(t['name'],
    t['sources'],
    c_args: [
      '-DPIXBUFDIR="@0@/pixmaps"'.format(datadir),
    ],
    include_directories: include_dirs,
    dependencies: [
      libcaja_private_dep,
      exif_dep, exempi_dep, wayland_deps
    ],
    install: false
  )
endforeach
