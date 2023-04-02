project( 'test', 'c', 'cpp', default_options : ['buildtype=debug'] )

# add_global_arguments('-E', language : 'cpp')

includes = include_directories( 
[ 
	'../test'
	'../project'
	, '../thirdparty' 
])

# get_sources = files('./get_sources.ps1')
# sources     = files(run_command('powershell', get_sources, check: true).stdout().strip().split('\n'))

sources = [ 'test.cpp' ]

if get_option('buildtype').startswith('debug')

  add_project_arguments('-DBuild_Debug', language : ['c', 'cpp'])

endif

  add_project_arguments('-Dgen_time', langauge : ['c', 'cpp'])

executable( '', sources, include_directories : includes )
