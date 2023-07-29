struct Builder
{
	FileInfo File;
	String   Buffer;

	void print( Code );
	void print_fmt( char const* fmt, ... );

	bool open( char const* path );
	void write();
};
