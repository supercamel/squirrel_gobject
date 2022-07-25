###############################################################################
## Project Options
###############################################################################

project('ValaSqBind', 'vala', 'c')
namespace = 'ValaSqBind'
version = '1.0'

dependencies = [
  dependency('glib-2.0'),
  dependency('gobject-2.0'),
  dependency('libsquirrel3')
  ]

sources = files(
  'object_bind.vala'
  )

executable('bind', sources, dependencies: dependencies)
