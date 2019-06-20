#pragma once
#include <string>
#include <minwindef.h>

enum class console_colour {
	BLACK,
	DARKBLUE,
	DARKGREEN,
	DARKCYAN,
	DARKRED,
	DARKPINK,
	DARKYELLOW,
	LIGHTGREY,
	DARKGREY,
	BLUE,
	GREEN,
	CYAN,
	RED,
	PINK,
	YELLOW,
	WHITE
};

namespace console {
	bool allocate_debug( const char* window_name );
	void allocate( HMODULE hModule );
	void detach( );
	void set_text_colour( console_colour colour );
	void print( const char* str, ... );
	void error( const char* text, ... );

	template< typename T, typename... Targs >
	void print_debug( const char* format, T value, Targs ... f_args ) {
		for ( ; *format != '\0'; format++ ) {
			if ( *format == '%' ) {
				std::cout << value;
				print( format + 1, f_args... ); // recursive call
				return;
			}

			std::cout << *format;
		}
	}
}