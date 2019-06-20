#pragma once
#include <Psapi.h>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include "..\SDK\IVEngineClient.h"
#include "..\SDK\datamap.hpp"

#define INRANGE(x,a,b)   (x >= a && x <= b)
#define GET_BYTE( x )    (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))
#define GET_BITS( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))

class Utils {
public:

	template<unsigned int IIdx, typename TRet, typename ... TArgs>
	static auto CallVFunc( void* thisptr, TArgs ... argList ) -> TRet
	{
		using Fn = TRet( __thiscall* )( void*, decltype( argList )... );
		return ( *static_cast< Fn * * >( thisptr ) )[ IIdx ]( thisptr, argList... );
	}

	template <typename t>
	static t GetVFunc( void* class_pointer, size_t index ) {
		return ( *( t * * )class_pointer )[ index ];
	}

	static unsigned int FindInDataMap(datamap_t *pMap, const char *name)
	{
		while (pMap)
		{
			for (int i = 0; i < pMap->dataNumFields; i++)
			{
				if (pMap->dataDesc[i].fieldName == NULL)
					continue;

				if (strcmp(name, pMap->dataDesc[i].fieldName) == 0)
					return pMap->dataDesc[i].fieldOffset[0];

				if (pMap->dataDesc[i].fieldType == 10)
				{
					if (pMap->dataDesc[i].td)
					{
						unsigned int offset;

						if ((offset = FindInDataMap(pMap->dataDesc[i].td, name)) != 0)
							return offset;
					}
				}
			}
			pMap = pMap->baseMap;
		}

		return 0;
	}

	static uintptr_t FindSignature( const char* szModule, const char* szSignature )
	{
		const char* pat = szSignature;
		DWORD firstMatch = 0;
		DWORD rangeStart = ( DWORD )GetModuleHandleA( szModule );
		MODULEINFO miModInfo;
		GetModuleInformation( GetCurrentProcess( ), ( HMODULE )rangeStart, &miModInfo, sizeof( MODULEINFO ) );
		DWORD rangeEnd = rangeStart + miModInfo.SizeOfImage;
		for ( DWORD pCur = rangeStart; pCur < rangeEnd; pCur++ )
		{
			if ( !*pat )
				return firstMatch;

			if ( *( PBYTE )pat == '\?' || *( BYTE* )pCur == GET_BYTE( pat ) )
			{
				if ( !firstMatch )
					firstMatch = pCur;

				if ( !pat[ 2 ] )
					return firstMatch;

				if ( *( PWORD )pat == '\?\?' || *( PBYTE )pat != '\?' )
					pat += 3;

				else
					pat += 2;
			}
			else
			{
				pat = szSignature;
				firstMatch = 0;
			}
		}
		return NULL;
	}

	template < typename t = uintptr_t >
	static t FindSignature2( const char* szModule, const char* szSignature )
	{
		const char* pat = szSignature;
		DWORD firstMatch = 0;
		DWORD rangeStart = ( DWORD )GetModuleHandleA( szModule );
		MODULEINFO miModInfo;
		GetModuleInformation( GetCurrentProcess( ), ( HMODULE )rangeStart, &miModInfo, sizeof( MODULEINFO ) );
		DWORD rangeEnd = rangeStart + miModInfo.SizeOfImage;
		for ( DWORD pCur = rangeStart; pCur < rangeEnd; pCur++ )
		{
			if ( !*pat )
				return ( t )firstMatch;

			if ( *( PBYTE )pat == '\?' || *( BYTE* )pCur == GET_BYTE( pat ) )
			{
				if ( !firstMatch )
					firstMatch = pCur;

				if ( !pat[ 2 ] )
					return ( t )firstMatch;

				if ( *( PWORD )pat == '\?\?' || *( PBYTE )pat != '\?' )
					pat += 3;

				else
					pat += 2;
			}
			else
			{
				pat = szSignature;
				firstMatch = 0;
			}
		}
		return ( t )NULL;
	}

	static DWORD FindPattern( const char* module_name, const BYTE * mask, const char* mask_string )
	{
		/// Get module address
		const unsigned int module_address = reinterpret_cast< unsigned int >( GetModuleHandle( module_name ) );

		/// Get module information to the size
		MODULEINFO module_info;
		GetModuleInformation( GetCurrentProcess( ), reinterpret_cast< HMODULE >( module_address ), &module_info, sizeof( MODULEINFO ) );

		auto IsCorrectMask = [ ]( const unsigned char* data, const unsigned char* mask, const char* mask_string ) -> bool
		{
			for ( ; *mask_string; ++mask_string, ++mask, ++data )
				if ( *mask_string == 'x' && *mask != *data )
					return false;

			return ( *mask_string ) == 0;
		};

		/// Iterate until we find a matching mask
		for ( unsigned int c = 0; c < module_info.SizeOfImage; c += 1 )
		{
			/// does it match?
			if ( IsCorrectMask( reinterpret_cast< unsigned char* >( module_address + c ), mask, mask_string ) )
				return ( module_address + c );
		}

		return 0;
	}

	/**
	*   GetCurrentSystemTime - Gets actual system time
	*   @timeInfo: Reference to your own tm variable, gets modified.
	*/
	static void GetCurrentSystemTime( tm & timeInfo )
	{
		const std::chrono::system_clock::time_point systemNow = std::chrono::system_clock::now( );
		std::time_t now_c = std::chrono::system_clock::to_time_t( systemNow );
		localtime_s( &timeInfo, &now_c ); // using localtime_s as std::localtime is not thread-safe.
	};

	/**
	*   Log - Logs specified message to debug console if in debug mode.
	*   Format: [HH:MM:SS] str
	*   @str: Debug message to be shown.
	*/
	static void Log( const std::string & str, ... )
	{
#ifdef _DEBUG
		tm timeInfo{ };
		Utils::GetCurrentSystemTime( timeInfo );

		std::stringstream ssTime; // Temp stringstream to keep things clean
		ssTime << "[" << std::put_time( &timeInfo, "%T" ) << "] " << str << std::endl;

		std::cout << ssTime.str( );
#endif // _DEBUG
	};

	/**
	*   Log(HRESULT) - Sets up an error message when you pass HRESULT
	*   Message contains error code only, no specific information
	*/
	static void Log( const HRESULT hr )
	{
#ifdef _DEBUG
		std::stringstream strFormatted;
		strFormatted << "something fucked up error = 0x" << std::hex << hr;

		Utils::Log( strFormatted.str( ) );
#endif // _DEBUG
	};

	/**
	*   WorldToScreen - Converts game coordinates to screen
	*   @origin: 3D coordinates to be converted
	*   @screen: Viewport coordinates to which we will convert
	*/
	static bool WorldToScreen( const Vector & origin, Vector2D & screen )
	{
		const auto screen_transform = [ &origin, &screen ]( ) -> bool
		{
			static std::uintptr_t p_viewmatrix;
			if ( !p_viewmatrix )
			{
				p_viewmatrix = static_cast < std::uintptr_t > ( Utils::FindSignature( "client_panorama.dll", "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9" ) );
				p_viewmatrix += 3;
				p_viewmatrix = *reinterpret_cast< std::uintptr_t* >( p_viewmatrix );
				p_viewmatrix += 176;
			}

			const VMatrix& w2s_matrix = *reinterpret_cast< VMatrix* >( p_viewmatrix );
			screen.x = w2s_matrix.m[ 0 ][ 0 ] * origin.x + w2s_matrix.m[ 0 ][ 1 ] * origin.y + w2s_matrix.m[ 0 ][ 2 ] * origin.z + w2s_matrix.m[ 0 ][ 3 ];
			screen.y = w2s_matrix.m[ 1 ][ 0 ] * origin.x + w2s_matrix.m[ 1 ][ 1 ] * origin.y + w2s_matrix.m[ 1 ][ 2 ] * origin.z + w2s_matrix.m[ 1 ][ 3 ];

			float w = w2s_matrix.m[ 3 ][ 0 ] * origin.x + w2s_matrix.m[ 3 ][ 1 ] * origin.y + w2s_matrix.m[ 3 ][ 2 ] * origin.z + w2s_matrix.m[ 3 ][ 3 ];

			if ( w < 0.001f )
			{
				screen.x *= 100000;
				screen.y *= 100000;
				return true;
			}

			float invw = 1.f / w;
			screen.x *= invw;
			screen.y *= invw;

			return false;
		};

		if ( !screen_transform( ) )
		{
			int width, height;
			g_pEngine->GetScreenSize( width, height );

			screen.x = ( width * 0.5f ) + ( screen.x * width ) * 0.5f;
			screen.y = ( height * 0.5f ) - ( screen.y * height ) * 0.5f;

			return true;
		}
		return false;
	}
};
