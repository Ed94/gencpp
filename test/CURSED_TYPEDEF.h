#include <functional>
#include <tuple>

class MyClass;

enum class MyEnum : short { VAL1, VAL2 };

struct OuterStruct {
    union NamedUnion {
        struct InnerStruct {
            double d;
            char c;
        } inner;
        int i;
    } unionInstance;
};

template<typename T, int N = alignof(double)>
struct TemplateStruct {
    T member[N];
};

template<>
struct TemplateStruct<int, 10> {
    int specialMember[10];
};

template<typename T>
struct AnotherTemplate {
    T value;
};

typedef decltype(nullptr) ( MyClass:: * InsaneComplexTypeDef )
(
    decltype( (MyEnum::VAL1 == MyEnum::VAL2) ? 1 : 2.0 )
	( TemplateStruct< decltype(OuterStruct().unionInstance.inner), 5> :: * ptr ) [5] [ alignof(double) ],
    	std::function<
			void *
			(
				TemplateStruct<int, 10> &&,
				void ( MyClass:: * memFnPtr )(
					TemplateStruct<
						decltype(OuterStruct().unionInstance.inner)
					>
				)
			)
		>,
    	int ( MyClass                :: * & refToMemFnPtr           )( TemplateStruct<int, 10> )    ,
    	int ( TemplateStruct<int, 10>:: *   memberPointer)                                      [10],
    	typename std::tuple_element<
			0,
			std::tuple< 
				AnotherTemplate<
					TemplateStruct< decltype(OuterStruct().unionInstance), 3 > 
				>
			>
		> :: type :: * complexMember,
    	template<typename U> typename AnotherTemplate<U> :: * templateMember
    char&&...,
)
volatile const && noexcept;
