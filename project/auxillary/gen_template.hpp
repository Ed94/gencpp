#ifdef GEN_INTELLISENSE_DIRECTIVES
#	pragma once
#	include "../gen.hpp"
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
Code gen_template( StrC template, StrC instantiation );
