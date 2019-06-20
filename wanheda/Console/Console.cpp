#include <windows.h>
#include <cstdint>
#include <string>
#include <sstream>
#include <cctype>
#include <random>
#include <array>
#include <utility>
#include <intrin.h>
#include <chrono>
#include "console.hpp"

#pragma warning( disable : 4307 ) // warning C4307: '*': integral constant overflow
#pragma warning( disable : 4244 ) // warning C4244: '=': conversion possible loss of data
#pragma warning( disable : 4996 ) // stupid json error when you try and stream a file to json so we can use it

char g_logFile[ MAX_PATH ];
char g_debugLogFile[ MAX_PATH ];

std::string wide_to_multibyte( const std::wstring& str ) {
	if ( str.empty( ) )
		return { };

	int str_len = WideCharToMultiByte( CP_UTF8, 0, str.data( ), ( int )str.size( ), nullptr, 0, nullptr, nullptr );

	std::string out;
	out.resize( str_len );

	WideCharToMultiByte( CP_UTF8, 0, str.data( ), ( int )str.size( ), &out[ 0 ], str_len, nullptr, nullptr );

	return out;
}

void console::allocate( HMODULE hModule ) {
	memset( g_logFile, 0, sizeof( g_logFile ) );

	if ( GetModuleFileNameA( hModule, g_logFile, MAX_PATH ) != 0 ) {
		size_t slash = -1;

		for ( size_t i = 0; i < strlen( g_logFile ); i++ ) {
			if ( g_logFile[ i ] == '/' || g_logFile[ i ] == '\\' ) {
				slash = i;
			}
		}

		if ( slash != -1 ) {
			g_logFile[ slash + 1 ] = '\0';
			strcpy_s( g_debugLogFile, g_logFile );
			strcat_s( g_debugLogFile, "debug.log" );
			strcat_s( g_logFile, "debug.log" );
		}
		else {
			// Shitty manual mapper detected.
			MessageBoxA( NULL, "(1) Unable to allocate console - bad manual mapper. Try using another injection method", "ERROR", MB_OK );
			//ExitProcess(0);
		}
	}
	else {
		// Shitty manual mapper detected.
		MessageBoxA( NULL, "(2) Unable to allocate console - bad manual mapper. Try using another injection method", "ERROR", MB_OK );
		//ExitProcess(0);
	}
}

bool console::allocate_debug( const char* window_name ) {
	if ( !AllocConsole( ) ) {
		_RPTF1( _CRT_ERROR, "Failed to allocate debug console. Error code: %i", GetLastError( ) );
		return false;
	}

	_iobuf* data;
	const errno_t res = freopen_s( &data, "CONOUT$", "w", stdout );
	if ( res != 0 ) {
		_RPTF1( _CRT_ERROR, "Failed to open stdout filestream. Error code: %i", res );
		return false;
	}

	if ( !SetConsoleTitleA( window_name ) ) {
		_RPTF1( _CRT_WARN, "Failed to set console title. Error code: %i", GetLastError( ) );
		return false;
	}

	return true;
}

void console::detach( ) {
	HMENU menu = GetSystemMenu( GetConsoleWindow( ), true ); // use true to restore buttons.
	if ( !menu )
		return;

	DeleteMenu( menu, SC_CLOSE, MF_BYCOMMAND );

	FreeConsole( );
}

void console::set_text_colour( console_colour colour ) {
	SetConsoleTextAttribute( GetStdHandle( static_cast< uintptr_t >( -11 ) ), *reinterpret_cast< unsigned short* >( &colour ) );
}

void console::print( const char* fmt, ... ) {
	va_list va_alist;
	char szLogbuf[ 4096 ];
	char szParameters[ 4066 ];
	char szTimestamp[ 30 ];
	struct tm current_tm;
	time_t current_time = time( NULL );
	FILE* file;

	localtime_s( &current_tm, &current_time );
	sprintf_s( szTimestamp, "[%02d:%02d:%02d]{DEBUG} %%s\n", current_tm.tm_hour, current_tm.tm_min, current_tm.tm_sec );

	va_start( va_alist, fmt );
	_vsnprintf_s( szParameters, sizeof( szParameters ), fmt, va_alist );
	va_end( va_alist );
	sprintf_s( szLogbuf, szTimestamp, szParameters );
	if ( ( fopen_s( &file, g_debugLogFile, "a" ) ) == 0 )
	{
		fprintf_s( file, "%s", szLogbuf );
		fclose( file );
	}
}

void console::error( const char* text, ... ) {
	va_list va_alist;
	char szLogbuf[ 4096 ];
	char szParameters[ 4066 ];
	char szTimestamp[ 30 ];
	struct tm current_tm;
	time_t current_time = time( NULL );
	FILE* file;

	localtime_s( &current_tm, &current_time );
	sprintf_s( szTimestamp, "[%02d:%02d:%02d]{FATAL} %%s\n", current_tm.tm_hour, current_tm.tm_min, current_tm.tm_sec );

	va_start( va_alist, text );
	_vsnprintf_s( szParameters, sizeof( szParameters ), text, va_alist );
	va_end( va_alist );
	sprintf_s( szLogbuf, szTimestamp, szParameters );
	if ( ( fopen_s( &file, g_debugLogFile, "a" ) ) == 0 )
	{
		fprintf_s( file, "%s", szLogbuf );
		fclose( file );
	}

	MessageBoxA( NULL, szLogbuf, "FATAL ERROR", MB_ICONERROR );

	ExitProcess( 0 );
}