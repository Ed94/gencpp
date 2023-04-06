/*
	Will generate a c99 compliant version of the gen library

	Note: This is done this way to test usage of library.
*/

#define GEN_DEFINE_DSL
#include "gen.cpp"

using namespace gen;

ct char const* Header_Comment = 
R"(/*
	genc: A simple staged metaprogramming library for C99.
	
	This library is intended for small-to midsize projects.

	AST type checking supports only a small subset of c++. 
	See the 'ECode' namespace and 'gen API' region to see what is supported.

	### *WHAT IS NOT PROVIDED*

	* Macro or template generation      : This library is to avoid those, adding support for them adds unnecessary complexity. 
	                                      If you desire define them outside the gen_time scopes. 
	* Expression validation             : Execution expressions are defined using the untyped string API. 
	                                      There is no parse API for validating expression (possibly will add in the future)
	* Complete file parser DSL          : This isn't like the unreal header tool. 
	                                      Code injection to file or based off a file contents is not supported by the api. However nothing is stopping you using the library for that purpose.
	* Modern c++ (STL library) features

	The AST is managed by the library and provided the user via its interface prodedures.

	Notes:

	* The allocator definitions used are exposed to the user incase they want to dictate memory usage
	* ASTs are wrapped for the user in a Code struct which essentially a warpper for a AST* type.  
	* Both AST and Code have member symbols but their data layout is enforced to be POD types.

	Data layout of AST struct:

	genc_CodeT             Type;    
	bool                   Readonly;
	genc_AST*              Parent;  
	genc_string            Name ;    
	genc_string            Comment; 
	union {                    
		array(genc_AST*)   Entries;
		genc_string        Content;
	};

	*`CodeT` is a typedef for `ECode::Type` which is the type of the enum.*

	ASTs can be set to readonly by calling Code's lock() member function.
	Adding comments is always available even if the AST is set to readonly.  

	### There are four sets of interfaces for Code AST generation the library provides

	* Upfront
	* Incremental
	* Parsing
	* Untyped

	### Upfront Construction

	All component ASTs must be previously constructed, and provided on creation of the code AST.
	The construction will fail and return InvalidCode otherwise.

	Interface :

	* genc_def_class
	* genc_def_class_body
	* genc_def_class_fwd
	* genc_def_enum
	* genc_def_enum_class
	* genc_def_enum_body
	* genc_def_global_body
	* genc_def_namespace
	* genc_def_namespace_body
	* genc_def_operator
	* genc_def_param
	* genc_def_params
	* genc_def_proc
	* genc_def_proc_body
	* genc_def_proc_fwd
	* genc_def_operator_fwd
	* genc_def_specifier
	* genc_def_specifiers
	* genc_def_struct
	* genc_def_struct_body
	* genc_def_struct_fwd
	* genc_def_variable
	* genc_def_type
	* genc_def_using
	* genc_def_using_namespace

	### Incremental construction

	A Code ast is provided but only completed upfront if all components are provided.
	Components are then added using the AST API for adding ASTs:

	* genc_code_add( AST*, AST* other )                     // Adds AST with validation.
	* genc_code_add_entry( AST*, AST* other )               // Adds AST entry without validation.

	Code ASTs may be explictly validated at anytime using Code's check() member function.

	Interface :

	* genc_make_class
	* genc_make_enum
	* genc_make_enum_class
	* genc_make_fwd
	* genc_make_global_body
	* genc_make_namespace
	* genc_make_operator
	* genc_make_params
	* genc_make_proc
	* genc_make_specifiers
	* genc_make_struct
	* genc_make_variable
	* genc_make_type
	* genc_make_using

	### Parse construction

	A string provided to the API is parsed for the intended language construct.

	Interface :

	* genc_parse_class
	* genc_parse_classes
	* genc_parse_class_fwd
	* genc_parse_classes_fwd
	* genc_parse_enum
	* genc_parse_enums
	* genc_parse_global_body
	* genc_parse_namespace
	* genc_parse_namespaces
	* genc_parse_params
	* genc_parse_proc
	* genc_parse_procs
	* genc_parse_operator
	* genc_parse_operators
	* genc_parse_specifiers
	* genc_parse_struct
	* genc_parse_strucs
	* genc_parse_variable
	* genc_parse_variables
	* genc_parse_type
	* genc_parse_types
	* genc_parse_using
	* genc_parse_usings

	The parse API treats any execution scope definitions with no validation and are turned into untyped Code ASTs.
	This includes the assignmetn of variables; due to the library not yet supporting c/c++ expression parsing.

	The plural variants provide an array of codes, its up to the user to add them to a body AST 
	(they are not auto-added to a body)

	### Untyped constructions

	Code ASTs are constructed using unvalidated strings.

	Interface :

	* genc_untyped_str
	* genc_untyped_fmt
	* genc_untyped_token_fmt

	During serialization any untyped Code AST is has its string value directly injected inline of 
	whatever context the content existed as an entry within.
	Even though thse are not validated from somewhat correct c/c++ syntax or components, it doesn't mean that
	Untyped code can be added as any component of a Code AST:

	* Untyped code cannot have children, thus there cannot be recursive injection this way.
	* Untyped code can only be a child of a parent of body AST, or for values of an assignment.

	These restrictions help prevent abuse of untyped code to some extent.
*/
)";

Code IfDef_GENC_IMPLEMENTATION;
Code EndIf_GENC_IMPLEMENTATION;

Code make_log_failure()
{
	Code result = make_global_body();
	{
		result->add( untyped_str(
			R"(#ifdef GEN_USE_FATAL)"
		));

		result->add( function_code(
			inline
			sw genc_log_failure(char const *fmt, ...) 
			{
				if ( genc_global_ShouldShowDebug == false )
					return 0;

				sw res;
				va_list va;
				
				va_start(va, fmt);
				res = zpl_printf_va(fmt, va);
				va_end(va);
				
				return res;
			}
		));

		result->add( untyped_str(
			R"(#else)"
		));

		result->add( proc_code(
			inline
			sw genc_log_failure(char const *fmt, ...) 
			{
				local_persist thread_local 
				char buf[ZPL_PRINTF_MAXLEN] = { 0 };

				va_list va;

			#if Build_Debug
				va_start(va, fmt);
				zpl_snprintf_va(buf, ZPL_PRINTF_MAXLEN, fmt, va);
				va_end(va);

				assert_crash(buf);
				return -1;
			#else
				va_start(va, fmt);
				zpl_printf_err_va( fmt, va);
				va_end(va);

				zpl_exit(1);
				return -1;
			#endif
			}
		));

		result->add( untyped_str(
			R"(#endif)"
		));

		result->check();
	}

	return result;
}

Code make_ECode()
{
	Code ECode = make_global_body();
	{
		ECode->add( enum_code(
			enum genc_ECode
			{
				Invalid,
				Untyped,
				Enum,
				Enum_Body,
				Global_Body,
				Parameters,
				Proc,           
				Proc_Body,      
				Proc_Forward,   
				Specifiers,     
				Struct,         
				Struct_Body,    
				Variable,       
				Typedef,        
				Typename,  

				Num_Types
			};
			typedef u32 genc_CodeT;
		));

		ECode->add( function_code(
			inline
			char const* genc_ecode_to_str( Type type )
			{
				genc_local_persist
				char const* lookup[Num_Types] = {
					"Invalid",
					"Untyped",
					"Access_Public",
					"Access_Private",
					"Access_Protected",
					"Class",
					"Enum",
					"Enum_Body",
					"Global_Body",
					"Namespace",
					"Namespace_Body",
					"Parameters",
					"Proc",
					"Proc_Body",
					"Proc_Forward",
					"Specifiers",
					"Struct",
					"Struct_Body",
					"Variable",
					"Typedef",
					"Typename",
					"Using",
				};

				return lookup[ type ];
			}
		));

		ECode->check();
	}

	return ECode;
}

Code make_EOperator()
{
	Code eoperator = make_global_body();

	return eoperator;
}

Code make_ESpecifier()
{
	Code especifier = make_global_body();

	return especifier;
}

Code make_Code()
{
	Code code = make_global_body();

	code->add( struct_code(
		struct genc_AST
		{
			genc_CodeT             Type;    
			bool                   Readonly;
			genc_AST*              Parent;  
			genc_string            Name;    
			genc_string            Comment; 
			union {                    
				array(genc_AST*)   Entries;
				genc_string        Content;
			};
		};
		typedef struct genc_AST genc_AST;
		typedef genc_AST* Code;
	));

	code->add( function_code(
		bool genc_ast_add( genc_Code other );
	));

	code->add( function_code( 
		genc_forceinline
		void genc_ast_add_entry( genc_Code self,  genc_Code other )
		{
			genc_array_append( self->Entries, other );

			other->Parent = self;
		}
	));

	code->add( untyped_str(
		R"(#define genc_code_body( AST_ ) AST->Entries[0])", 47
	));

	code->add( function_code( 
		genc_forceinline
		bool genc_code_has_entries( Code self ) const
		{
			genc_local_persist 
			bool lookup[ genc_ECode::Num_Types] = {
				false, // Invalid
				false, // Untyped
				true,  // Global_Body
				true,  // Parameters
				true,  // Proc
				true,  // Proc_Body
				true,  // Proc_Foward
				false, // Specifies
				true,  // Struct
				true,  // Struct_Body
				true,  // Variable
				true,  // Typedef
				true,  // Typename
			};

			return lookup[self->Type];
		}
	));

	code->check();

	return code;
}

Code make_static_Data()
{
	Code 
	data = make_global_body();
	data->add( IfDef_GENC_IMPLEMENTATION );
	data->add( global_body_code(
		static genc_array(genc_AST) genc_CodePool = nullptr;

		static genc_array(genc_arena) genc_StringArenas =  nullptr;

		static genc_StringTable    genc_StringMap;
		static genc_TypeTable      genc_TypeMap;

		static sw genc_InitSize_CodePool       = genc_megabytes(64);
		static sw genc_InitSize_StringArena    = genc_megabytes(32);
		static sw genc_InitSize_StringTable    = genc_megabytes(4);
		static sw genc_InitSize_TypeTable      = genc_megabytes(4);

		static allocator genc_Allocator_CodePool    = zpl_heap();
		static allocator genc_Allocator_StringArena = zpl_heap();
		static allocator genc_Allocator_StringTable = zpl_heap();
		static allocator genc_Allocator_TypeTable   = zpl_heap();
	));
	data->add( untyped_str( R"(#ifdef GENC_DEFINE_LIBRARY_CODE_CONSTANTS)"));
	data->add( global_body_code(
		Code t_void;

		Code t_bool;
		Code t_char;
		Code t_char_wide;

		Code t_s8;
		Code t_s16;
		Code t_s32;
		Code t_s64;

		Code t_u8;
		Code t_u16;
		Code t_u32;
		Code t_u64;

		Code t_sw;
		Code t_uw;

		Code t_f32;
		Code t_f64;
	));
	data->add( untyped_str( R"(#endif)"));
	data->add( global_body_code(
		Code spec_inline;
		Code spec_const;
	));
	data->add( EndIf_GENC_IMPLEMENTATION );

	return data;
}

Code make_make_code()
{

}

Code make_init()
{

}

Code make_mem_config_interface()
{

}

Code make_internal_funcs()
{

}

Code make_upfront()
{

}

Code make_incremental()
{

}

Code make_parsing()
{

}

Code make_untyped()
{

}

Code make_interface()
{

}


int gen_main()
{
	Memory::setup();
	gen::init();
	
	IfDef_GENC_IMPLEMENTATION = untyped_str(
		R"(#ifdef GENC_IMPLEMENTATION)"
	);

	EndIf_GENC_IMPLEMENTATION = untyped_str(
		R"(#endif // GENC_IMPLEMENTATION)"
	);

	Code header_comment = untyped_str( Header_Comment, sizeof( Header_Comment ) );
	Code log_failure    = make_log_failure();

	Code ecode          = make_ECode();
	Code eoperator      = make_EOperator();
	Code especifier     = make_ESpecifier();
	Code code           = make_Code();
	
	Builder
	builder;
	builder.open( "genc.h" );

	builder.print( header_comment );
	builder.print( log_failure );
	builder.print( ecode );
	builder.print( eoperator );
	builder.print( especifier );
	builder.print( code );

	builder.write();

	Memory::cleanup();
	return 0;
}
