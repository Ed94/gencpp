#include "Bloat.cpp"


#ifdef gen_time
#include "gen.cpp"


void case_untyped()
{


}
















int gen_main()
{
	Memory::setup();

	log_fmt("\nPress any key after attaching to process\n");
	getchar();

	gen::init();

	case_untyped();

	Memory::cleanup();
	return 0;
}
#endif


#ifdef runtime
int main()
{
	return 0;
}
#endif
