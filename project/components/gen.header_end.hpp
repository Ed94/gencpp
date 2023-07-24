#pragma region Inlines

void AST::append( AST* other )
{
	if ( other->Parent )
		other = other->duplicate();

	other->Parent = this;

	if ( Front == nullptr )
	{
		Front = other;
		Back  = other;

		NumEntries++;
		return;
	}

	AST*
		Current       = Back;
	Current->Next = other;
	other->Prev   = Current;
	Back          = other;
	NumEntries++;
}

char const* AST::debug_str()
{
	char const* fmt = stringize(
	                  \nCode Debug:
	                  \nType    : %s
	                  \nParent  : %s
	                  \nName    : %s
	                  \nComment : %s
	                  );

	// These should be used immediately in a log.
	// Thus if its desired to keep the debug str
	// for multiple calls to bprintf,
	// allocate this to proper string.
	return str_fmt_buf( fmt
	                   ,	type_str()
	                   ,	Parent   ? Parent->Name : ""
	                   ,	Name     ? Name         : ""
	                   );
}

Code& AST::entry( u32 idx )
{
	AST** current = & Front;
	while ( idx >= 0 && current != nullptr )
	{
		if ( idx == 0 )
			return * rcast( Code*, current);

		current = & ( * current )->Next;
		idx--;
	}

	return * rcast( Code*, current);
}

bool AST::has_entries()
{
	return NumEntries;
}

char const* AST::type_str()
{
	return ECode::to_str( Type );
}

AST::operator Code()
{
	return { this };
}

Code& Code::operator ++()
{
	if ( ast )
		ast = ast->Next;

	return *this;
}

#pragma region AST & Code Gen Common
#define Define_CodeImpl( Typename )                                                  \
char const* Typename::debug_str()                                                    \
{                                                                                    \
	if ( ast == nullptr )                                                            \
		return "Code::debug_str: AST is null!";                                      \
	                                                                                 \
	return rcast(AST*, ast)->debug_str();                                            \
}                                                                                    \
Code Typename::duplicate()														     \
{                                                                                    \
	if ( ast == nullptr )                                                            \
	{                                                                                \
		log_failure("Code::duplicate: Cannot duplicate code, AST is null!");         \
		return Code::Invalid;                                                        \
	}                                                                                \
	                                                                                 \
	return { rcast(AST*, ast)->duplicate() };                                        \
}                                                                                    \
bool Typename::is_equal( Code other )                                                \
{                                                                                    \
	if ( ast == nullptr || other.ast == nullptr )                                    \
	{                                                                                \
		log_failure("Code::is_equal: Cannot compare code, AST is null!");            \
		return false;                                                                \
	}                                                                                \
                                                                                     \
	return rcast(AST*, ast)->is_equal( other.ast );                                  \
}                                                                                    \
bool Typename::is_valid()                                                            \
{                                                                                    \
	return (AST*) ast != nullptr && rcast( AST*, ast)->Type != CodeT::Invalid;       \
}                                                                                    \
void Typename::set_global()                                                          \
{                                                                                    \
	if ( ast == nullptr )                                                            \
	{                                                                                \
		log_failure("Code::set_global: Cannot set code as global, AST is null!");    \
		return;                                                                      \
	}                                                                                \
	                                                                                 \
	rcast(AST*, ast)->Parent = Code::Global.ast;                                     \
}                                                                                    \
String Typename::to_string()													     \
{                                                                                    \
	if ( ast == nullptr )                                                            \
	{                                                                                \
	log_failure("Code::to_string: Cannot convert code to string, AST is null!");     \
	return { nullptr };                                                              \
	}                                                                                \
	                                                                                 \
	return rcast(AST*, ast)->to_string();                                            \
}                                                                                    \
Typename& Typename::operator =( Code other )                                         \
{                                                                                    \
	if ( other.ast && other->Parent )                                                \
	{                                                                                \
		ast = rcast( decltype(ast), other.ast->duplicate() );                        \
		rcast( AST*, ast)->Parent = nullptr;                                         \
	}                                                                                \
                                                                                     \
	ast = rcast( decltype(ast), other.ast );                                         \
	return *this;                                                                    \
}                                                                                    \
bool Typename::operator ==( Code other )                                             \
{                                                                                    \
	return (AST*) ast == other.ast;                                                  \
}                                                                                    \
bool Typename::operator !=( Code other )                                             \
{                                                                                    \
	return (AST*) ast != other.ast;                                                  \
}

Define_CodeImpl( Code );
Define_CodeImpl( CodeBody );
Define_CodeImpl( CodeAttributes );
Define_CodeImpl( CodeComment );
Define_CodeImpl( CodeClass );
Define_CodeImpl( CodeEnum );
Define_CodeImpl( CodeExec );
Define_CodeImpl( CodeExtern );
Define_CodeImpl( CodeInclude );
Define_CodeImpl( CodeFriend );
Define_CodeImpl( CodeFn );
Define_CodeImpl( CodeModule );
Define_CodeImpl( CodeNamespace );
Define_CodeImpl( CodeOperator );
Define_CodeImpl( CodeOpCast );
Define_CodeImpl( CodeParam );
Define_CodeImpl( CodeSpecifiers );
Define_CodeImpl( CodeStruct );
Define_CodeImpl( CodeTemplate );
Define_CodeImpl( CodeType );
Define_CodeImpl( CodeTypedef );
Define_CodeImpl( CodeUnion );
Define_CodeImpl( CodeUsing );
Define_CodeImpl( CodeVar );
#undef Define_CodeImpl

#define Define_AST_Cast( typename )              \
AST::operator Code ## typename()                 \
{                                                \
	return { rcast( AST_ ## typename*, this ) }; \
}

Define_AST_Cast( Body );
Define_AST_Cast( Attributes );
Define_AST_Cast( Comment );
Define_AST_Cast( Class );
Define_AST_Cast( Enum );
Define_AST_Cast( Exec );
Define_AST_Cast( Extern );
Define_AST_Cast( Include );
Define_AST_Cast( Friend );
Define_AST_Cast( Fn );
Define_AST_Cast( Module );
Define_AST_Cast( Namespace );
Define_AST_Cast( Operator );
Define_AST_Cast( OpCast );
Define_AST_Cast( Param );
Define_AST_Cast( Struct );
Define_AST_Cast( Specifiers );
Define_AST_Cast( Template );
Define_AST_Cast( Type );
Define_AST_Cast( Typedef );
Define_AST_Cast( Union );
Define_AST_Cast( Using );
Define_AST_Cast( Var );
#undef Define_AST_Cast

#define Define_CodeCast( type )     \
Code::operator Code ## type() const \
{                                   \
	return { (AST_ ## type*) ast }; \
}

Define_CodeCast( Attributes );
Define_CodeCast( Comment );
Define_CodeCast( Class );
Define_CodeCast( Exec );
Define_CodeCast( Enum );
Define_CodeCast( Extern );
Define_CodeCast( Include );
Define_CodeCast( Friend );
Define_CodeCast( Fn );
Define_CodeCast( Module );
Define_CodeCast( Namespace );
Define_CodeCast( Operator );
Define_CodeCast( OpCast );
Define_CodeCast( Param );
Define_CodeCast( Specifiers );
Define_CodeCast( Struct );
Define_CodeCast( Template );
Define_CodeCast( Type );
Define_CodeCast( Typedef );
Define_CodeCast( Union );
Define_CodeCast( Using );
Define_CodeCast( Var );
Define_CodeCast( Body);
#undef Define_CodeCast
#pragma endregion AST & Code Gen Common

void CodeClass::add_interface( CodeType type )
{
	if ( ! ast->Next )
	{
		ast->Next = type;
		ast->Last = ast->Next;
		return;
	}

	ast->Next->Next = type;
	ast->Last       = ast->Next->Next;
}

void CodeParam::append( CodeParam other )
{
	AST* self  = (AST*) ast;
	AST* entry = (AST*) other.ast;

	if ( entry->Parent )
		entry = entry->duplicate();

	entry->Parent = self;

	if ( self->Last == nullptr )
	{
		self->Last = entry;
		self->Next = entry;
		self->NumEntries++;
		return;
	}

	self->Last->Next = entry;
	self->Last       = entry;
	self->NumEntries++;
}

CodeParam CodeParam::get( s32 idx )
{
	CodeParam param = *this;
	do
	{
		if ( ! ++ param )
			return { nullptr };

		return { (AST_Param*) param.raw()->Next };
	}
	while ( --idx );

	return { nullptr };
}

bool CodeParam::has_entries()
{
	return ast->NumEntries > 0;
}

CodeParam& CodeParam::operator ++()
{
	ast = ast->Next.ast;
	return * this;
}

void CodeStruct::add_interface( CodeType type )
{
	if ( ! ast->Next )
	{
		ast->Next = type;
		ast->Last = ast->Next;
	}

	ast->Next->Next = type;
	ast->Last       = ast->Next->Next;
}

CodeBody def_body( CodeT type )
{
	switch ( type )
	{
		using namespace ECode;
		case Class_Body:
		case Enum_Body:
		case Export_Body:
		case Extern_Linkage:
		case Function_Body:
		case Global_Body:
		case Namespace_Body:
		case Struct_Body:
		case Union_Body:
			break;

		default:
			log_failure( "def_body: Invalid type %s", (char const*)ECode::to_str(type) );
			return (CodeBody)Code::Invalid;
	}

	Code
		result       = make_code();
	result->Type = type;
	return (CodeBody)result;
}

//! Do not use directly. Use the token_fmt macro instead.
// Takes a format string (char const*) and a list of tokens (StrC) and returns a StrC of the formatted string.
StrC token_fmt_impl( sw num, ... )
{
	local_persist thread_local
		char buf[GEN_PRINTF_MAXLEN] = { 0 };
	mem_set( buf, 0, GEN_PRINTF_MAXLEN );

	va_list va;
	va_start(va, num );
	sw result = token_fmt_va(buf, GEN_PRINTF_MAXLEN, num, va);
	va_end(va);

	return { result, buf };
}
#pragma endregion Inlines

#pragma region Constants
#ifdef GEN_DEFINE_LIBRARY_CODE_CONSTANTS
	// Predefined typename codes. Are set to readonly and are setup during gen::init()

	extern CodeType t_b32;

	extern CodeType t_s8;
	extern CodeType t_s16;
	extern CodeType t_s32;
	extern CodeType t_s64;

	extern CodeType t_u8;
	extern CodeType t_u16;
	extern CodeType t_u32;
	extern CodeType t_u64;

	extern CodeType t_sw;
	extern CodeType t_uw;

	extern CodeType t_f32;
	extern CodeType t_f64;
#endif

#ifndef GEN_GLOBAL_BUCKET_SIZE
#	define GEN_GLOBAL_BUCKET_SIZE megabytes(10)
#endif
#ifndef GEN_CODEPOOL_NUM_BLOCKS
#	define GEN_CODEPOOL_NUM_BLOCKS kilobytes(64)
#endif
#ifndef GEN_SIZE_PER_STRING_ARENA
#	define GEN_SIZE_PER_STRING_ARENA megabytes(1)
#endif
#ifndef GEN_MAX_COMMENT_LINE_LENGTH
#	define GEN_MAX_COMMENT_LINE_LENGTH 1024
#endif
#ifndef GEN_MAX_NAME_LENGTH
#	define GEN_MAX_NAME_LENGTH 128
#endif
#ifndef GEN_MAX_UNTYPED_STR_LENGTH
#	define GEN_MAX_UNTYPED_STR_LENGTH kilobytes(640)
#endif
#ifndef GEN_TOKEN_FMT_TOKEN_MAP_MEM_SIZE
#	define GEN_TOKEN_FMT_TOKEN_MAP_MEM_SIZE kilobytes(4)
#endif
#ifndef GEN_LEX_ALLOCATOR_SIZE
#	define GEN_LEX_ALLOCATOR_SIZE megabytes(10)
#endif
#ifndef GEN_BUILDER_STR_BUFFER_RESERVE
#	define GEN_BUILDER_STR_BUFFER_RESERVE megabytes(1)
#endif

// These constexprs are used for allocation behavior of data structures
// or string handling while constructing or serializing.
// Change them to suit your needs.

constexpr s32 InitSize_DataArrays = 16;

// NOTE: This limits the maximum size of an allocation
// If you are generating a string larger than this, increase the size of the bucket here.
constexpr uw  Global_BucketSize         = GEN_GLOBAL_BUCKET_SIZE;
constexpr s32 CodePool_NumBlocks        = GEN_CODEPOOL_NUM_BLOCKS;
constexpr s32 SizePer_StringArena       = GEN_SIZE_PER_STRING_ARENA;

constexpr s32 MaxCommentLineLength      = GEN_MAX_COMMENT_LINE_LENGTH;
constexpr s32 MaxNameLength             = GEN_MAX_NAME_LENGTH;
constexpr s32 MaxUntypedStrLength       = GEN_MAX_UNTYPED_STR_LENGTH;
constexpr s32 TokenFmt_TokenMap_MemSize	= GEN_TOKEN_FMT_TOKEN_MAP_MEM_SIZE;
constexpr s32 LexAllocator_Size         = GEN_LEX_ALLOCATOR_SIZE;
constexpr s32 Builder_StrBufferReserve  = GEN_BUILDER_STR_BUFFER_RESERVE;

extern CodeType t_empty; // Used with varaidc parameters. (Exposing just in case its useful for another circumstance)
extern CodeType t_auto;
extern CodeType t_void;
extern CodeType t_int;
extern CodeType t_bool;
extern CodeType t_char;
extern CodeType t_wchar_t;
extern CodeType t_class;
extern CodeType t_typename;

extern CodeParam param_varadic;

extern CodeAttributes attrib_api_export;
extern CodeAttributes attrib_api_import;

extern Code access_public;
extern Code access_protected;
extern Code access_private;

extern Code module_global_fragment;
extern Code module_private_fragment;

extern Code pragma_once;

extern CodeSpecifiers spec_const;
extern CodeSpecifiers spec_consteval;
extern CodeSpecifiers spec_constexpr;
extern CodeSpecifiers spec_constinit;
extern CodeSpecifiers spec_extern_linkage;
extern CodeSpecifiers spec_final;
extern CodeSpecifiers spec_global;
extern CodeSpecifiers spec_inline;
extern CodeSpecifiers spec_internal_linkage;
extern CodeSpecifiers spec_local_persist;
extern CodeSpecifiers spec_mutable;
extern CodeSpecifiers spec_override;
extern CodeSpecifiers spec_ptr;
extern CodeSpecifiers spec_ref;
extern CodeSpecifiers spec_register;
extern CodeSpecifiers spec_rvalue;
extern CodeSpecifiers spec_static_member;
extern CodeSpecifiers spec_thread_local;
extern CodeSpecifiers spec_virtual;
extern CodeSpecifiers spec_volatile;
#pragma endregion Constants

#pragma region Macros
#	define gen_main main

#	define __ NoCode

	//	Convienence for defining any name used with the gen api.
	//  Lets you provide the length and string literal to the functions without the need for the DSL.
#	define name( Id_ )   { sizeof(stringize( Id_ )) - 1, stringize(Id_) }

	//  Same as name just used to indicate intention of literal for code instead of names.
#	define code( ... ) { sizeof(stringize(__VA_ARGS__)) - 1, stringize( __VA_ARGS__ ) }

#	define args( ... ) num_args( __VA_ARGS__ ), __VA_ARGS__

#	define code_str( ... ) gen::untyped_str( code( __VA_ARGS__ ) )
#	define code_fmt( ... ) gen::untyped_str( token_fmt( __VA_ARGS__ ) )

	// Takes a format string (char const*) and a list of tokens (StrC) and returns a StrC of the formatted string.
#	define token_fmt( ... ) gen::token_fmt_impl( (num_args( __VA_ARGS__ ) + 1) / 2, __VA_ARGS__ )
#pragma endregion Macros

#ifdef GEN_EXPOSE_BACKEND
	// Global allocator used for data with process lifetime.
	extern AllocatorInfo  GlobalAllocator;
	extern Array< Arena > Global_AllocatorBuckets;
	extern Array< Pool >  CodePools;
	extern Array< Arena > StringArenas;

	extern StringTable StringCache;

	extern Arena LexArena;

	extern AllocatorInfo Allocator_DataArrays;
	extern AllocatorInfo Allocator_CodePool;
	extern AllocatorInfo Allocator_Lexer;
	extern AllocatorInfo Allocator_StringArena;
	extern AllocatorInfo Allocator_StringTable;
	extern AllocatorInfo Allocator_TypeTable;
#endif
