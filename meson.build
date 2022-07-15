project('libsquirrel3', 'c', 'cpp', version: '0.1', license: 'MIT')

gnome = import('gnome')
pkg = import('pkgconfig')

dependencies = [
  dependency('glib-2.0'),
  dependency('gobject-2.0'),
  dependency('gobject-introspection-1.0')
  ]

squirrel3_sources = [
  'Squirrel3/squirrel/sqapi.cpp',
  'Squirrel3/squirrel/sqbaselib.cpp',
  'Squirrel3/squirrel/sqclass.cpp',
  'Squirrel3/squirrel/sqcompiler.cpp',
  'Squirrel3/squirrel/sqdebug.cpp',
  'Squirrel3/squirrel/sqfuncstate.cpp',
  'Squirrel3/squirrel/sqlexer.cpp',
  'Squirrel3/squirrel/sqmem.cpp',
  'Squirrel3/squirrel/sqobject.cpp',
  'Squirrel3/squirrel/sqstate.cpp',
  'Squirrel3/squirrel/sqtable.cpp',
  'Squirrel3/squirrel/sqvm.cpp',
  'Squirrel3/sqstdlib/sqstdaux.cpp',
  'Squirrel3/sqstdlib/sqstdblob.cpp',
  'Squirrel3/sqstdlib/sqstdio.cpp',
  'Squirrel3/sqstdlib/sqstdmath.cpp',
  'Squirrel3/sqstdlib/sqstdrex.cpp',
  'Squirrel3/sqstdlib/sqstdstream.cpp',
  'Squirrel3/sqstdlib/sqstdstring.cpp',
  'Squirrel3/sqstdlib/sqstdsystem.cpp',
  'src/vm.c'
  ]

libsquirrel3lib = shared_library('squirrel3',
  squirrel3_sources,
  vala_header: 'inc/libsquirrel3-0.1.h',
  vala_vapi: 'libsquirrel3-0.1.vapi',
  vala_gir: 'libsquirrel3-0.1.gir',
  include_directories: ['inc', 'Squirrel3/include'],
  dependencies: dependencies,
  install: true
  )

pkg.generate(libsquirrel3lib,
  version: '0.1',
  name: 'libsquirrel3',
  filebase: 'libsquirrel3',
  description: 'Gobject wrapper for the Squirrel3 language',
  subdirs: 'libsquirrel3-0.1'
  )

headers = ['inc/squirrel-0.1.h',
  'Squirrel3/include/squirrel.h',
  'Squirrel3/include/sqconfig.h',
  'Squirrel3/include/sqstdaux.h',
  'Squirrel3/include/sqstdblob.h',
  'Squirrel3/include/sqstdio.h',
  'Squirrel3/include/sqstdmath.h',
  'Squirrel3/include/sqstdstring.h',
  'Squirrel3/include/sqstdsystem.h'
  ]

install_headers(headers, subdir: 'libsquirrel3-0.1')
introspection_sources = [
  'inc/squirrel-0.1.h',
  'src/vm.c'
  ]

g_ir_compiler = find_program('g-ir-compiler')
libsquirrel3_gir = gnome.generate_gir(libsquirrel3lib,
  sources: introspection_sources,
  nsversion: '0.1',
  namespace: 'squirrel',
  symbol_prefix: 'squirrel_',
  identifier_prefix: 'Squirrel',
  export_packages: 'libsquirrel3',
  includes: ['GObject-2.0'],
  install: true,
  install_dir_gir: get_option('libdir') / 'girepository-1.0',
  install_dir_typelib: get_option('libdir') / 'girepository-1.0'
  )[0]

gnome.generate_vapi('libsquirrel3',
  install: true,
  sources:libsquirrel3_gir 
  )


