project('sqtest', 'vala')

dependencies = [
  dependency('glib-2.0'),
  dependency('gobject-2.0'),
  dependency('libsquirrel3'),
  meson.get_compiler('c').find_library('m', required: false)
  ]

source = files(
  'main.vala'
  )

executable('sqtest', source, dependencies: dependencies)

