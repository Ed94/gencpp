## Navigation

[Top](../Readme.md)

* [docs](../docs/Readme.md)

# C Library Generation

`c_library.cpp` generates both *segemnted* and *singleheader* variants of the library compliant with C11.

The output will be in the `gen_segmented/gen` directory (if the directory does not exist, it will create it).

If using the library's provided build scripts:

```ps1
.\build.ps1 <compiler> <debug or omit> c_lib
```

To generate a static or dynamic library:

```ps1
.\build.ps1 <compiler> <debug or omit> c_lib_static c_lib_dyn
```
.
All free from tag identifiers will be prefixed with `gen_` or `GEN_` as the namespace. This can either be changed after generation with a `.refactor` script (or your preferred subst method), OR by modifying [c_library.refactor](./c_library.refactor).

**If c_library.refactor is modified you may need to modify c_library.cpp and its [components](./components/). As some of the container generation relies on that prefix.**

## Generation structure

1. Files are scanned in or parsed.
    * If they are parsed, its due to requiring some changes to either naming, or adding additonal definitions (container generation, typedefs, etc).
2. All scanned or parsed code is refactored (identifiers substs) and/or formatted.
3. Singleheader generated.
4. Segmented headers and source generated.

## Templated container generation

The array and hashtable containers used across this library are generated using the following implementatioon:

* [containers.array.hpp](./components/containers.array.hpp)
* [containers.hashtable.hpp](./components/containers.hashtable.hpp)

These are functionally (and interface wise) equivalent to the library's `Array<Type>` `HashTable<Type>` within [containers.hpp](../base/dependencies/containers.hpp)

Both files follow the same pattern of providing three procedures:

* `gen_<container>_base` : Intended to be called once, defines universal "base" definitions.
* `gen_<container>` : Called per instatiation of the container for a given set of dependent args.
* `gen_<container>_generic_selection_interface` : Intended to be called once after all of the instantiated containers have finished generating. It will generate a set of generic selection macros as described by Macro Usage section below.

A simple `<container>_DefinitionCounter` is used to know how many instantiations of the template have occured. This is used to determine how to define `GENERIC_SLOT_<ID>_<functionID>` for the generic interface along with how many slots the `_Generic` macro will need to have generated.

## Macro Usage

For the most part macros are kept minimal with exception to `_Generic`...  

The `_Generic` macro plays a key role in reducing direct need of the user to wrangle with mangled definition identifiers of 'templated' containers or for type coercion to map distinct data types to a common code path.

Many C11 libraries don't use it.. and, of those that do. they usually end up obfuscate it with excessive preprocessor abuse; Effort was put into minimizing how much of these macros are handled by the preprocessor vs gencpp itself.

*(I will be explaining this thing for the rest of this seciton along with gencpp c library's usage of it)*


The usual presentation (done bare) is the following:

```c
#define macro_that_selects_typeof_arg(arg, y) \
_Generic( (arg),                              \
    int            : some expression,         \
    double         : some other expression,   \
    struct Whatnot : something else again,    \
    default        : fallback expression      \
)
```

Where `_Generic` can be considered the follwoing (psuedo-C):

```c
#define type_expr_pair(type, expr) type: expr

C_Expression _Generic( selector_arg, a_type_expr_pair, ... ) {
    switch( typeof(selector_arg)) {
        case a_type_expr_pair:
            return a_type_expr_pari.expr;
        ...
        default:
            return default.expr;
    }
}
```

The first `arg` of _Generic behaves as the "controlling expression" or the expression that resolves to a type which will dictate which of the following expressions provided after to `_Generic` will be resolved as the one used inline for the implemenation.


For this library's purposes we'll be using the functional macro equivalent *(if there is an exception I'll link it at the end of this section)*:

```c
#define macro_that_uses_selector_arg_for_resolving_a_fucntion( selecting_exp) \
_Generic( (selecting_exp),                                                    \
    int            : func_use_int,                                            \
    double         : func_use_double,                                         \
    struct Whatnot : func_use_Whatnot,                                        \
    default        : struct SIGNALS_FAILURE                                   \
) (selecting_exp)
```

In this case, we directly added `(selecting_exp)` to the end there.. as we want to directly have the macro resolve to calling a resolved procedure. A default has been set to a struct as that leads to a neat compiler message that would otherwise be impossible beause static_assert is a statement and thus cannot be used within a slot.

Now, even with gencpp generating this type-expression table, we still need wrapper macros to achieve function 'overloading' for the templated containers as _Generic has a [significant drawback](https://www.chiark.greenend.org.uk/~sgtatham/quasiblog/c11-generic/):  

> Discarded expressions still have to be semantically valid.

The only way to absolve this issue [(without resorting to nasty preprocessor hacks)](https://github.com/JacksonAllan/CC/blob/main/articles/Better_C_Generics_Part_1_The_Extendible_Generic.md) is with wrapping expressions in 'slot' resolving macros that do not expand if the slot is not defined:

```c
GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( GENERIC_SLOT_1__function_sig )
```

`GENERIC_SLOT_1__function_sig` is our warpper of a "`int, func_use_int`" pair. The `GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT` is a verbse named macro to indicate that that pair will be expanded ***ONLY IF*** its defined.

So for any given templated container interface. Expect the follwoing (taken straight from generation, and just cleaned up formatting):

```c
#define gen_array_append( selector_arg, ... ) _Generic(                         \
(selector_arg ),                                                                \
    GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( GEN_GENERIC_SLOT_1__array_append )  \
    GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( GEN_GENERIC_SLOT_2__array_append )  \
    GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( GEN_GENERIC_SLOT_3__array_append )  \
    GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( GEN_GENERIC_SLOT_4__array_append )  \
    GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( GEN_GENERIC_SLOT_5__array_append )  \
    GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( GEN_GENERIC_SLOT_6__array_append )  \
    GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( GEN_GENERIC_SLOT_7__array_append )  \
    GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( GEN_GENERIC_SLOT_8__array_append )  \
    GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( GEN_GENERIC_SLOT_9__array_append )  \
    GEN_IF_MACRO_DEFINED_INCLUDE_THIS_SLOT( GEN_GENERIC_SLOT_10__array_append ) \
    default: gen_generic_selection_fail                                         \
) GEN_RESOLVED_FUNCTION_CALL( &selector_arg, __VA_ARGS__ )

```

*Note(Ed): Unfortunately I cannot get clang-format to output these macros sanely like the above..*  
*Eventually I'll add some basic builtin formatting but if the user has suggestions for something better I'm open ears...*

`GEN_RESOLVED_FUNCTION_CALL` is an empty define, its just to indicate that its intended to expand to a function call.

To see the the actual macro definitions used - see: [generic_macros.h](./components/generic_macros.h). They'll be injected right after the usual macros in the generated header file.
