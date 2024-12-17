#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "helpers/push_ignores.inline.hpp"
#	include "components/header_start.hpp"
#	include "components/types.hpp"
#	include "components/gen/ecode.hpp"
#	include "components/gen/eoperator.hpp"
#	include "components/gen/especifier.hpp"
#	include "components/ast.hpp"
#	include "components/code_types.hpp"
#	include "components/ast_types.hpp"
#	include "components/interface.hpp"
#	include "components/inlines.hpp"
#	include "components/gen/ast_inlines.hpp"
#	include "components/header_end.hpp"
#endif

/*
	Explicitly generates a resolved definition of a cpp template definition.

	TODO(Ed): Needs implementing for the C-library variant.
	TODO(Ed): We need a non <token> syntax subst implemtnation for Strings for this to work. It must subst keywords directly based on template parameter names.

	This is only meant to be used on relatively trivial templates, where the type or numeric is mostly a 'duck' type.
	It cannot parse complex template parameters.

	The varadic args should correspond 1:1 with the type of objects the generator expects from the template's parameters.alignas.
*/

CodeOperator gen_operator_template( CodeTemplate template, ... );
CodeFn       gen_func_template( CodeTemplate template, ...  );
Code         gen_class_struct_template( CodeTemplate template, ... );

Code gen_template( CodeTemplate template, ... );
Code gen_template( Str template, Str instantiation );
