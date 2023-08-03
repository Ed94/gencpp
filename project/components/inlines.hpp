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
	if ( Parent )
	{
		char const* fmt = stringize(
			\nType    : %s
			\nParent  : %s %s
			\nName    : %s
		);

		// These should be used immediately in a log.
		// Thus if its desired to keep the debug str
		// for multiple calls to bprintf,
		// allocate this to proper string.
		return str_fmt_buf( fmt
			,	type_str()
			,	Parent->Name
			,   Parent->type_str()
			,	Name     ? Name         : ""
		);
	}

	char const* fmt = stringize(
		\nType    : %s
		\nName    : %s
	);

	// These should be used immediately in a log.
	// Thus if its desired to keep the debug str
	// for multiple calls to bprintf,
	// allocate this to proper string.
	return str_fmt_buf( fmt
		,	type_str()
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

