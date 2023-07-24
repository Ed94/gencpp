project( 'gen_singleheader', 'c', 'cpp', default_options : ['buildtype=debug'] )

includes = include_directories(
[
'../project',
])

sources = [ 'gen.singleheader.cpp' ]

if get_option('buildtype').startswith('debug')

  add_project_arguments('-DBuild_Debug', language : ['c', 'cpp'])

endif

add_project_arguments('-DGEN_TIME', language : ['c', 'cpp'])

executable( 'gen_singlehader', sources, include_directories : includes )
