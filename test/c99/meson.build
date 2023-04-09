project( 'test', 'c', default_options : ['buildtype=debug'] )

# add_global_arguments('-E', language : 'cpp')

includes = include_directories(
  [
    '../gen',
    '../../singleheader'
  ])

# get_sources = files('./get_sources.ps1')
# sources     = files(run_command('powershell', get_sources, check: true).stdout().strip().split('\n'))

sources = [ 'test.c99.c' ]

if get_option('buildtype').startswith('debug')

  add_project_arguments('-DBuild_Debug', language : ['c' ])

endif

add_project_arguments('-Dgentime', language : ['c', 'cpp'])

executable( 'test_c99', sources, include_directories : includes )
