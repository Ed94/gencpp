#define BLOAT_IMPL
#include "Bloat.hpp"

namespace Global
{
	bool ShouldShowDebug = false;
}

namespace Memory
{
	arena Global_Arena {};
	
	void setup()
	{
		arena_init_from_allocator( & Global_Arena, heap(), Initial_Reserve );

		if ( Global_Arena.total_size == 0 )
		{
			assert_crash( "Failed to reserve memory for Tests:: Global_Arena" );
		}
	}
	
	void resize( uw new_size )
	{
		void* new_memory = resize( heap(), Global_Arena.physical_start, Global_Arena.total_size, new_size );		
		
		if ( new_memory == nullptr )
		{
			fatal("Failed to resize global arena!");
		}
		
		Global_Arena.physical_start = new_memory;
		Global_Arena.total_size     = new_size;
	}

	void cleanup()
	{
		arena_free( & Global_Arena);
	}	
}


bool opts_custom_add(opts* options, opts_entry *t, char* b)
{
	if (t->type != ZPL_OPTS_STRING)
	{
		return false;   
	}

	t->text = string_append_length(t->text, " ", 1);
	t->text = string_appendc( t->text, b );

	return true;
}

b32 opts_custom_compile(opts *options, int argc, char **argv) 
{
	b32 had_errors = false;

	for (int i = 1; i < argc; ++i) 
	{
		char* arg = argv[i];
	
		if (*arg) 
		{
			arg = (char*)str_trim(arg, false);

			if (*arg == '-') 
			{
				opts_entry* entry = 0;
				b32 checkln = false;
				if ( *(arg + 1) == '-') 
				{
					checkln = true;
					++arg;
				}

				char *b = arg + 1, *e = b;

				while (char_is_alphanumeric(*e) || *e == '-' || *e == '_') {
					++e;
				}

				entry = zpl__opts_find(options, b, (e - b), checkln);

				if (entry) 
				{
					char *ob = b;
					b = e;

					/**/ 
					if (*e == '=') 
					{
						if (entry->type == ZPL_OPTS_FLAG) 
						{
							*e = '\0';
							zpl__opts_push_error(options, ob, ZPL_OPTS_ERR_EXTRA_VALUE);
							had_errors = true;

							continue;
						}

						b = e = e + 1;
					} 
					else if (*e == '\0') 
					{
						char *sp = argv[i+1];

						if (sp && *sp != '-' && (array_count(options->positioned) < 1  || entry->type != ZPL_OPTS_FLAG)) 
						{
							if (entry->type == ZPL_OPTS_FLAG) 
							{
								zpl__opts_push_error(options, b, ZPL_OPTS_ERR_EXTRA_VALUE);
								had_errors = true;

								continue;
							}

							arg = sp;
							b = e = sp;
							++i;
						} 
						else 
						{
							if (entry->type != ZPL_OPTS_FLAG) 
							{
								zpl__opts_push_error(options, ob, ZPL_OPTS_ERR_MISSING_VALUE);
								had_errors = true;
								continue;
							}

							entry->met = true;

							continue;
						}
					}

					e = (char *)str_control_skip(e, '\0');
					zpl__opts_set_value(options, entry, b);

					if ( (i + 1) < argc )
					{
						for ( b = argv[i + 1]; i < argc && b[0] != '-'; i++, b = argv[i + 1] )
						{
							opts_custom_add(options, entry, b );
						}
					}
				} 
				else 
				{
					zpl__opts_push_error(options, b, ZPL_OPTS_ERR_OPTION);
					had_errors = true;
				}
			} 
			else if (array_count(options->positioned)) 
			{
				opts_entry *l = array_back(options->positioned);
				array_pop(options->positioned);
				zpl__opts_set_value(options, l, arg);
			} 
			else 
			{
				zpl__opts_push_error(options, arg, ZPL_OPTS_ERR_VALUE);
				had_errors = true;
			}
		}
	}

	return !had_errors;
}
