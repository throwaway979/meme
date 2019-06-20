#include <thread>
#include "Hooks.h"
#include "Features\Features.h"
#include "Utils\Utils.h"
#include "Utils\GlobalVars.h"
#include "Console\Console.hpp"

HINSTANCE HThisModule;

DWORD WINAPI OnDllAttach( PVOID base )
{
	Hooks::Init( );
	g_Anim.init( );

	while ( true ) {
		using namespace std::literals::chrono_literals;
		std::this_thread::sleep_for( 1s );
	}

	FreeLibraryAndExitThread( static_cast< HMODULE >( base ), 1 );
}

BOOL WINAPI DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved )
{
	if ( dwReason == DLL_PROCESS_ATTACH )
	{
		DisableThreadLibraryCalls( hModule );
		auto h = CreateThread( nullptr, NULL, OnDllAttach, hModule, NULL, nullptr );
		CloseHandle( h );
	}
	else if ( dwReason == DLL_PROCESS_DETACH )
	{
		if ( !lpReserved )
		{
			using namespace std::literals::chrono_literals;
			std::this_thread::sleep_for( 1s );
		}
	}

	return TRUE;
}