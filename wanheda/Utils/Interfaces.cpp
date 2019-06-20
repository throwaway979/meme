#include "Interfaces.h"
#include "Utils.h"

#include "..\SDK\IClientMode.h"
#include "..\SDK\IBaseClientDll.h"
#include "..\SDK\IClientEntityList.h"
#include "..\SDK\IVEngineClient.h"
#include "..\SDK\CPrediction.h"
#include "..\SDK\IGameEvent.h"
#include "..\SDK\ISurface.h"
#include "..\SDK\IEngineTrace.h"
#include "..\SDK\ISurfaceData.h"
#include "..\SDK\ICvar.h"
#include "..\SDK\IVModelInfo.h"
#include "..\SDK\CModelRender.h"
#include "..\SDK\IMaterial.h"
#include "..\SDK\IVRenderView.h"
#include "..\SDK\IInputSystem.h"
#include "..\SDK\CClientState.h"
#include "..\SDK\IMemAlloc.h"
#include "..\SDK\IVDebugOverlay.h"
#include "..\SDK\IViewRenderBeams.h"

#include <fstream>

#define enc_str(s) std::string(s)
#define enc_char(s) enc_str(s).c_str()
#define enc_wstr(s) std::wstring(enc_str(s).begin(), enc_str(s).end())
#define enc_wchar(s) enc_wstr(s).c_str()

// Initializing global interfaces
IMemAlloc* g_pMemAlloc = nullptr;
IBaseClientDLL* g_pClientDll = nullptr;
IClientMode* g_pClientMode = nullptr;
IClientEntityList* g_pEntityList = nullptr;
IVEngineClient* g_pEngine = nullptr;
CPrediction* g_pPrediction = nullptr;
IGameMovement* g_pMovement = nullptr;
IMoveHelper* g_pMoveHelper = nullptr;
CGlobalVarsBase* g_pGlobalVars = nullptr;
IGameEventManager* g_pEventManager = nullptr;
ISurface* g_pSurface = nullptr;
IEngineTrace* g_pTrace = nullptr;
IPhysicsSurfaceProps* g_pSurfaceData = nullptr;
ICVar* g_pCvar = nullptr;
IPanel* g_pPanel = nullptr;
IVModelInfo* g_pModelInfo = nullptr;
CModelRender* g_pModelRender = nullptr;
CGlowObjectManager* g_pGlow = nullptr;
IMaterialSystem* g_pMaterialSys = nullptr;
IVRenderView* g_pRenderView = nullptr;
i_inputsytem* g_pInputSystem = nullptr;
IClientState* g_pClientState = nullptr;
IVDebugOverlay* g_pDebugOverlay = nullptr;
IViewRenderBeams* g_pBeam = nullptr;

namespace interfaces
{
	template< class T >
	T* FindClass( std::string szModuleName, std::string szInterfaceName, bool bSkip = false )
	{
		if ( szModuleName.empty( ) || szInterfaceName.empty( ) )
			return nullptr;
		typedef PVOID( *CreateInterfaceFn )( const char* pszName, int* piReturnCode );
		CreateInterfaceFn hInterface = nullptr;
		while ( !hInterface )
		{
			hInterface = ( CreateInterfaceFn )GetProcAddress( GetModuleHandleA( szModuleName.c_str( ) ), "CreateInterface" );
			Sleep( 5 );
		}

		char pszBuffer[ 256 ];
		for ( int i = 0; i < 100; i++ )
		{
			sprintf_s( pszBuffer, "%s%0.3d", szInterfaceName.c_str( ), i );
			PVOID pInterface = hInterface( pszBuffer, nullptr );

			if ( pInterface && pInterface != NULL )
			{
				if ( bSkip )
					sprintf_s( pszBuffer, "%s%0.3d", szInterfaceName.c_str( ), i + 1 );

				Sleep( 5 );
				break;
			}
		}

		return ( T* )hInterface( pszBuffer, nullptr );
	}

	template<typename T>
	T* CaptureInterface( const char* szModuleName, const char* szInterfaceVersion )
	{
		HMODULE moduleHandle = GetModuleHandleA( szModuleName );
		if ( moduleHandle )   /* In case of not finding module handle, throw an error. */
		{
			CreateInterfaceFn pfnFactory = reinterpret_cast< CreateInterfaceFn >( GetProcAddress( moduleHandle, "CreateInterface" ) );
			return reinterpret_cast< T* >( pfnFactory( szInterfaceVersion, nullptr ) );
		}
		Utils::Log( "[purpura sdk]: couldn't get interface %", szInterfaceVersion );
		return nullptr;
	}

	void Init( )
	{
		g_pMemAlloc = *reinterpret_cast< IMemAlloc * * >( GetProcAddress( GetModuleHandle( "tier0.dll" ), "g_pMemAlloc" ) );
		g_pClientDll = FindClass<IBaseClientDLL>( "client_panorama.dll", "VClient" );
		g_pClientMode = **reinterpret_cast< IClientMode * ** >    ( ( *reinterpret_cast< uintptr_t * * >( g_pClientDll ) )[ 10 ] + 0x5u );
		g_pGlobalVars = **reinterpret_cast< CGlobalVarsBase * ** >( ( *reinterpret_cast< uintptr_t * * >( g_pClientDll ) )[ 0 ] + 0x1Bu );
		g_pEntityList = FindClass<IClientEntityList>( "client_panorama.dll", "VClientEntityList" );
		g_pEngine = FindClass<IVEngineClient>( "engine.dll", "VEngineClient" );
		g_pPrediction = FindClass<CPrediction>( "client_panorama.dll", "VClientPrediction" );
		g_pMovement = FindClass<IGameMovement>( "client_panorama.dll", "GameMovement" );
		g_pMoveHelper = **reinterpret_cast< IMoveHelper * ** >( ( Utils::FindSignature( "client_panorama.dll", "8B 0D ? ? ? ? 8B 46 08 68" ) + 0x2 ) );
		g_pEventManager = CaptureInterface<IGameEventManager>( "engine.dll", "GAMEEVENTSMANAGER002" );
		g_pSurface = FindClass<ISurface>( "vguimatsurface.dll", "VGUI_Surface" );
		g_pTrace = FindClass<IEngineTrace>( "engine.dll", "EngineTraceClient" );
		g_pSurfaceData = FindClass<IPhysicsSurfaceProps>( "vphysics.dll", "VPhysicsSurfaceProps" );
		g_pCvar = FindClass<ICVar>( "vstdlib.dll", "VEngineCvar" );
		g_pPanel = FindClass<IPanel>( "vgui2.dll", "VGUI_Panel" );
		g_pModelInfo = FindClass<IVModelInfo>( "engine.dll", "VModelInfoClient" );
		g_pModelRender = FindClass<CModelRender>( "engine.dll", "VEngineModel" );
		g_pMaterialSys = FindClass<IMaterialSystem>( "materialsystem.dll", "VMaterialSystem" );
		g_pGlow = *reinterpret_cast< CGlowObjectManager * * >( Utils::FindSignature( "client_panorama.dll", "0F 11 05 ? ? ? ? 83 C8 01" ) + 3 );
		g_pRenderView = FindClass<IVRenderView>( "engine.dll", "VEngineRenderView" );
		g_pInputSystem = FindClass<i_inputsytem>( "inputsystem.dll", "InputSystemVersion" );
		g_pClientState = **reinterpret_cast< IClientState * ** >( ( Utils::FindSignature( "engine.dll", "A1 ? ? ? ? 33 D2 6A 00 6A 00 33 C9 89 B0" ) + 0x1 ) );
		g_pDebugOverlay = CaptureInterface<IVDebugOverlay>( "engine.dll", "VDebugOverlay004" );
		g_pBeam = *( IViewRenderBeams * * )( Utils::FindSignature( "client_panorama.dll", "8D 43 FC B9 ? ? ? ? 50 A1" ) + 4 );

		//thanks monarch (from yeti)
		std::ofstream( "csgo\\materials\\FlatChams.vmt" ) << R"#("UnlitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
  "$reflectivity" "[1 1 1]"
}
)#";
		std::ofstream( "csgo\\materials\\simple_regular.vmt" ) << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
	}
}