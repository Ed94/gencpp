#if gen_time
// This undefines the macros used by the gen library but are not necessary for the user.
// TODO : This is incomplete until all dependencies are brough in from ZPL into bloat.

#undef bit
#undef bitfield_is_equal
#undef ccast
#undef scast
#undef rcast
#undef pcast
#undef do_once
#undef do_once_start
#undef do_once_end

#undef kilobytes
#undef megabytes
#undef gigabytes
#undef terabytes

// gen_time
#endif
