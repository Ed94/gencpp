/*
	gencpp: An attempt at "simple" staged metaprogramming for c/c++.

	See Readme.md for more information from the project repository.

	Public Address:
	https://github.com/Ed94/gencpp
*/
#pragma once

#include "helpers/push_ignores.inline.hpp"
#include "components/header_start.hpp"

GEN_NS_BEGIN

#include "components/types.hpp"
#include "components/gen/ecode.hpp"
#include "components/gen/eoperator.hpp"
#include "components/gen/especifier.hpp"

#include "components/ast.hpp"
#include "components/code_types.hpp"
#include "components/ast_types.hpp"

#include "components/interface.hpp"

#include "components/inlines.hpp"
#include "components/gen/ast_inlines.hpp"
#include "components/header_end.hpp"

GEN_NS_END

#include "helpers/pop_ignores.inline.hpp"

#undef array_init
#undef array_init_reserve
#undef array_append_array
#undef array_append_value
#undef array_append_items
#undef array_append_at
#undef array_append_at
#undef array_back
#undef array_clear
#undef array_fill
#undef array_free
#undef arary_grow
#undef array_num
#undef arary_pop
#undef arary_remove_at
#undef arary_reserve
#undef arary_resize
#undef arary_set_capacity
#undef arary_get_header

#undef hashtable_init
#undef hashtable_init_reserve
#undef hashtable_clear
#undef hashtable_destroy
#undef hashtable_get
#undef hashtable_grow
#undef hashtable_rehash
#undef hashtable_rehash_fast
#undef hashtable_remove
#undef hashtable_remove_entry
#undef hashtable_set
#undef hashtable_slot
#undef hashtable_add_entry
#undef hashtable_find
#undef hashtable_full
#undef hashtable_map
#undef hashtable_map_mut
