// Sanity check: def_comment test

// The following will show a series of base cases for the gen api.

class TestEmptyClass;
class TestEmptyClass
{
	// Empty class body

};

typedef unsigned char u8;

enum ETestEnum : u8;
enum ETestEnum : u8
{
	A,
	B,
	C,
};
enum class ETestEnumClass : u8;

extern "C"
{
	// Empty extern body

}

class TestFriend
{
	friend 	class TestFriendFwd;

};

void test_function(void);
void test_function(void)
{
	// Empty function body

}

#include "DummyInclude.hpp"


namespace TestNamespace
{
	// Empty namespace body

};


void test_function_wparam(u8 a);
void test_function_wparams(u8 a, u8 b)
{
	// Empty function body

}
void test_function_wparams2(u8 a, u8 b)
{
	// Empty function body

}


class TestEmptyStruct;
class TestEmptyStruct
{
	// Empty class body

};

union TestEmptyUnion
{
	// Empty union body

};

using TestUsing = u8;
using namespace TestNamespace;

u8 test_variable;
u8 test_variable2 = 0x12;

// End of base case tests.

