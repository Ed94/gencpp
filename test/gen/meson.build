project( 'test', 'c', 'cpp', default_options : ['buildtype=debug'] )

# add_global_arguments('-E', language : 'cpp')

includes = include_directories(
[
	'../../project',
	'../../thirdparty'
])

# get_sources = files('./get_sources.ps1')
# sources     = files(run_command('powershell', get_sources, check: true).stdout().strip().split('\n'))

sources = [ '../test.cpp' ]

if get_option('buildtype').startswith('debug')

  add_project_arguments('-DBuild_Debug', language : ['c', 'cpp'])
  # add_project_arguments('-E', language : ['c', 'cpp'])

endif
  # add_project_arguments('-E', language : ['c', 'cpp'])
  # add_global_arguments( '-E', language : ['cpp'])

add_project_arguments('-Dgentime', language : ['c', 'cpp'])

executable( 'gencpp', sources, include_directories : includes )
