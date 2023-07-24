project( 'gencpp_bootstrap', 'c', 'cpp', default_options : ['buildtype=debug'] )

includes = include_directories(
[
'../project',
])

sources = [ 'gen.bootstrap.cpp' ]

if get_option('buildtype').sartswith('debug')

  add_project_arguments('-DBuild_Debug', language : ['c', 'cpp'])

endif

add_project_arguments('-DGEN_TIME', language : ['c', 'cpp'])

executable( 'gencpp_bootstrap', sources, include_directories : includes )
