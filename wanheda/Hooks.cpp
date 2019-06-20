#include <thread>
#include "Hooks.h"
#include "Utils\Utils.h"
#include "Features/Visuals/BulletTracers.h"
#include "Features\Features.h"
#include "Features\Sounds.h"
#include "Menu\Menu.h"
#include "SDK\Hitboxes.h"
#include "SDK\Definitions.h"
#include "libloaderapi.h"
#include "Console\Console.hpp"
#include <array>
#include <mmsystem.h>
#include <fstream>
#include <intrin.h>
#include "Features/Visuals/GrenadePrediction.h"

#define TEXTURE_GROUP_MODEL							          "Model textures"

Misc     g_Misc;
Hooks    g_Hooks;
Event    g_Event;

void Warning( const char* msg, ... ) // wintergang https://www.unknowncheats.me/forum/1923881-post1.html
{
	if ( msg == nullptr )
		return;
	typedef void( __cdecl * MsgFn )( const char* msg, va_list );
	static MsgFn fn = ( MsgFn )GetProcAddress( GetModuleHandle( "tier0.dll" ), "Warning" );
	char buffer[ 989 ];
	va_list list;
	va_start( list, msg );
	vsprintf( buffer, msg, list );
	va_end( list );
	fn( buffer, list );
}

static bool fuck_you_tommy = false;

void SetMoveChokeClampLimit() {
	auto ClMoveChokeClamp = Utils::FindSignature("engine.dll", "B8 ? ? ? ? 3B F0 0F 4F F0 89 5D FC") + 1;

	if (fuck_you_tommy)
		return;

	unsigned long o_prot = 0;

	VirtualProtect((void*)ClMoveChokeClamp, 4, PAGE_EXECUTE_READWRITE, &o_prot);
	*(std::uint32_t*)ClMoveChokeClamp = 62; //serverside max: value of sv_maxusrcmdprocessticks + 1 (sv_maxusrcmdprocessticks isn't networked)
	VirtualProtect((void*)ClMoveChokeClamp, 4, o_prot, &o_prot);

	fuck_you_tommy = true;
}

void ResetMoveChokeClampLimit() {
	auto ClMoveChokeClamp = Utils::FindSignature("engine.dll", "B8 ? ? ? ? 3B F0 0F 4F F0 89 5D FC") + 1;

	if (!fuck_you_tommy)
		return;

	unsigned long o_prot = 0;

	VirtualProtect((void*)ClMoveChokeClamp, 4, PAGE_EXECUTE_READWRITE, &o_prot);
	*(std::uint32_t*)ClMoveChokeClamp = 15;
	VirtualProtect((void*)ClMoveChokeClamp, 4, o_prot, &o_prot);

	fuck_you_tommy = false;
}

C_BaseEntity* UTIL_PlayerByIndex(int index)
{
	typedef C_BaseEntity* (__fastcall * PlayerByIndex)(int);
	static PlayerByIndex UTIL_PlayerByIndex = (PlayerByIndex)Utils::FindSignature("server.dll", "85 C9 7E 2A A1");

	if (!UTIL_PlayerByIndex)
		return false;

	return UTIL_PlayerByIndex(index);
}

void Hooks::Init( )
{
	// Get window handle
	while ( !( g_Hooks.hCSGOWindow = FindWindowA( "Valve001", nullptr ) ) )
	{
		using namespace std::literals::chrono_literals;
		std::this_thread::sleep_for( 50ms );
	}

	interfaces::Init( );                         // Get interfaces
	g_pNetvars = std::make_unique<NetvarTree>( );// Get netvars after getting interfaces as we use them

	if ( g_Hooks.hCSGOWindow )        // Hook WNDProc to capture mouse / keyboard input
		g_Hooks.pOriginalWNDProc = reinterpret_cast< WNDPROC >( SetWindowLongPtr( g_Hooks.hCSGOWindow, GWLP_WNDPROC,
		reinterpret_cast< LONG_PTR >( g_Hooks.WndProc ) ) );

	// VMTHooks
	g_Hooks.pClientHook = std::make_unique<VMTHook>( g_pClientDll );
	g_Hooks.pClientModeHook = std::make_unique<VMTHook>( g_pClientMode );
	g_Hooks.pSurfaceHook = std::make_unique<VMTHook>( g_pSurface );
	g_Hooks.pPanelHook = std::make_unique<VMTHook>( g_pPanel );
	g_Hooks.pModelHook = std::make_unique<VMTHook>( g_pModelRender );
	g_Hooks.pRenderViewHook = std::make_unique<VMTHook>( g_pRenderView );
	g_Hooks.pEngineHook = std::make_unique<VMTHook>( g_pEngine );
	//g_Hooks.pPredictionHook = std::make_unique<VMTHook>(g_pPrediction);

	// Hook the table functions
	g_Hooks.pClientHook->Hook( vtable_indexes::frameStage, Hooks::FrameStageNotify );
	g_Hooks.pClientModeHook->Hook( vtable_indexes::createMove, Hooks::CreateMove );
	g_Hooks.pClientModeHook->Hook( vtable_indexes::view, Hooks::OverrideView );
	g_Hooks.pClientModeHook->Hook( vtable_indexes::dopostscreeneffects, Hooks::PostScreenEffects );
	g_Hooks.pClientModeHook->Hook( vtable_indexes::viewmodelIndex, Hooks::ViewmodelNigga );
	g_Hooks.pSurfaceHook->Hook( vtable_indexes::lockCursor, Hooks::LockCursor );
	g_Hooks.pPanelHook->Hook( vtable_indexes::paint, Hooks::PaintTraverse );
	g_Hooks.pModelHook->Hook( vtable_indexes::dme, Hooks::DrawModelExecute );
	g_Hooks.pRenderViewHook->Hook( vtable_indexes::sceneEnd, Hooks::SceneEnd );
	g_Hooks.pEngineHook->Hook( vtable_indexes::ishltv, Hooks::IsHltv );
	//g_Hooks.pPredictionHook->Hook(vtable_indexes::inprediction, Hooks::InPrediction);

	g_Event.Init( );

	g::CourierNew = g_pSurface->FontCreate( );
	g_pSurface->SetFontGlyphSet( g::CourierNew, "Courier New", 14, 300, 0, 0, FONTFLAG_OUTLINE );
	g::Tahoma = g_pSurface->FontCreate( );
	g_pSurface->SetFontGlyphSet( g::Tahoma, "Open Sans", 12, 700, 0, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS );

	// render manager shit

	g::fonts::others_font = g_pSurface->FontCreate( );
	g::fonts::esp_font = g_pSurface->FontCreate( );
	g::fonts::menu_font = g_pSurface->FontCreate( );
	g::fonts::watermark_font = g_pSurface->FontCreate( );
	g::fonts::tab_font = g_pSurface->FontCreate( );
	g::fonts::flags_font = g_pSurface->FontCreate( );
	g::fonts::weapon_icons_font = g_pSurface->FontCreate( );
	g::fonts::manual_antiaim_font = g_pSurface->FontCreate( );

	g_pSurface->SetFontGlyphSet( g::fonts::others_font, "Small Fonts", 9, 400, 0, 0, FONTFLAG_OUTLINE );
	g_pSurface->SetFontGlyphSet( g::fonts::esp_font, "Open Sans", 16, 300, 0, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS );
	g_pSurface->SetFontGlyphSet( g::fonts::menu_font, "Open Sans", 19, 0, 0, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS );
	g_pSurface->SetFontGlyphSet( g::fonts::watermark_font, "Small Fonts", 9, 400, 0, 0, FONTFLAG_DROPSHADOW );
	g_pSurface->SetFontGlyphSet( g::fonts::watermark2_font, "Verdana", 12, 0, 0, 0, FONTFLAG_NONE );
	g_pSurface->SetFontGlyphSet( g::fonts::tab_font, "Wanheda Font", 34, 0, 0, 0, FONTFLAG_ANTIALIAS );
	g_pSurface->SetFontGlyphSet( g::fonts::flags_font, "Verdana", 32, 700, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW );
	g_pSurface->SetFontGlyphSet( g::fonts::weapon_icons_font, "Counter-Strike", 24, 0, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW );
	g_pSurface->SetFontGlyphSet( g::fonts::manual_antiaim_font, "wanheda.space || logo font", 42, 0, 0, 0, FONTFLAG_ANTIALIAS  );

	g_Skins.ApplyAAAHooks();

	g_pEngine->ExecuteClientCmd( "clear" );
}

void Hooks::Restore( )
{
	// Unhook every function we hooked and restore original one
	g_Hooks.pClientHook->Unhook( vtable_indexes::frameStage );
	g_Hooks.pClientModeHook->Unhook( vtable_indexes::createMove );
	g_Hooks.pClientModeHook->Unhook( vtable_indexes::view );
	g_Hooks.pClientModeHook->Unhook( vtable_indexes::dopostscreeneffects );
	g_Hooks.pSurfaceHook->Unhook( vtable_indexes::lockCursor );
	g_Hooks.pPanelHook->Unhook( vtable_indexes::paint );
	g_Hooks.pModelHook->Unhook( vtable_indexes::dme );
	g_Hooks.pEngineHook->Unhook( vtable_indexes::ishltv );
	//g_Hooks.pPredictionHook->Unhook(vtable_indexes::inprediction);

	ResetMoveChokeClampLimit(); //reset clientside fakelag limit, unneeded but still smth to-do

	SetWindowLongPtr( g_Hooks.hCSGOWindow, GWLP_WNDPROC, reinterpret_cast< LONG_PTR >( g_Hooks.pOriginalWNDProc ) );

	g_pNetvars.reset( );   /* Need to reset by-hand, global pointer so doesnt go out-of-scope */
}

float __fastcall Hooks::ViewmodelNigga( ) {
	return g_Menu.Config.ViewmodelFov == 0 ? 68.f : g_Menu.Config.ViewmodelFov;
}

bool __fastcall Hooks::InPrediction(void* ecx, void* edx) {
	auto ret = g_Hooks.pPredictionHook->GetOriginal< InPrediction_t >(vtable_indexes::inprediction)(ecx);

	if (!g_Menu.Config.PosBacktrack)
		return ret;
	
	for (auto i = 1; i <= g_pGlobalVars->maxClients; i++) {
		C_BaseEntity* entity = g_pEntityList->GetClientEntity(i);

		if (!entity || !entity->IsAlive())
			continue;

		//skip client setupbones call (more fps)
		entity->GetBoneAccessor()->SetReadableBones(0xFFFFFFFF);
		entity->GetBoneAccessor()->SetWritableBones(0xFFFFFFFF);
		*(float*)((uintptr_t)entity + 0x2914) = std::sqrt(-1.0f); //lastbonesetuptime (?)
		//entity->SetupBones(nullptr, -1, 0x7FF0, g_pGlobalVars->curtime);
	}

	return ret;
}

bool __fastcall Hooks::IsHltv( uintptr_t ecx, uintptr_t edx ) {
	//antipaste
	return ret;
}

bool __fastcall Hooks::CreateMove( IClientMode * thisptr, void* edx, float sample_frametime, CUserCmd * pCmd )
{
	// Call original createmove before we start screwing with it
	static auto oCreateMove = g_Hooks.pClientModeHook->GetOriginal<CreateMove_t>( vtable_indexes::createMove );

	oCreateMove( thisptr, edx, sample_frametime, pCmd );

	auto ofunc = oCreateMove( thisptr, edx, sample_frametime, pCmd );

	if ( !pCmd || !pCmd->command_number )
		return ofunc;

	//Sendpacket
	uintptr_t * fpointer; __asm { MOV fpointer, EBP }
	byte* bSendPacket = ( byte* )( *fpointer - 0x1C ); if ( !bSendPacket ) return false;

	g::bSendPacket = *bSendPacket;

	// Get globals
	g::pCmd = pCmd;
	g::pLocalEntity = g_pEntityList->GetClientEntity( g_pEngine->GetLocalPlayer( ) );

	if ( !g::pLocalEntity )
		return ofunc;

	g::OriginalView = g::pCmd->viewangles;

	if ( g_Menu.Config.InfinityDuck )
		g::pCmd->buttons |= IN_BULLRUSH;

	g_Misc.NormalWalk();
	g_Misc.DisablePostProcess();
	g_Misc.RecoilCrosshair();
	g_Misc.RankReveal();
	g_Misc.FakeDuck();
	g_Misc.OnCreateMove( );
	g_Resolver.OnCreateMove( );
	g_AntiAim.OnCreateMove( );

	engine_prediction::RunEnginePred( );
	g_Aimbot.OnCreateMove( );
	g_Legitbot.CM_Backtrack( );
	g_Legitbot.OnCreateMove( );
	engine_prediction::EndEnginePred();

	g_Misc.AFKBot( );
	g_Misc.AutoDerank( );

	g_Misc.MovementFix( g::OriginalView );
	g_Math.Clamp( g::pCmd->viewangles );

	if ( g::bSendPacket && !(g::pCmd->buttons & IN_ATTACK) ) {
		g::FakeAngle = pCmd->viewangles;
	}
	else {
		g::RealAngle = pCmd->viewangles;
	}

	*bSendPacket = g::bSendPacket;

	if (!g_Hooks.oTempEntities) {
		g_Hooks.pClientStateHook = std::make_unique<VMTHook>(((uintptr_t*)((uintptr_t)g_pClientState + 0x8)));
		g_Hooks.pClientStateHook->Hook(vtable_indexes::tempentities, Hooks::TempEntities_h);
		g_Hooks.oTempEntities = g_Hooks.pClientStateHook->GetOriginal<TempEntities_t>(vtable_indexes::tempentities);
	}

	if ( g::pLocalEntity->IsAlive( ) && g::pLocalEntity  && g_Menu.Config.GrenadePrediction )
		GrenadePrediction::get( ).Tick( pCmd->buttons );

	return false;
}

void setup_chams( ) {
	std::ofstream( "csgo\\materials\\cherry_flat.vmt" ) << R"#("UnlitGeneric"
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
	std::ofstream( "csgo\\materials\\cherry_flat_ignorez.vmt" ) << R"#("UnlitGeneric"
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
		}
		)#";
}

bool __fastcall Hooks::TempEntities_h(void* ECX, void* EDX, void* msg)
{

	if (!g::pLocalEntity || !g_pEngine->IsInGame() || !g_pEngine->IsConnected())
		return g_Hooks.oTempEntities(ECX, msg);

	bool ret = g_Hooks.oTempEntities(ECX, msg);

	if (!g_Menu.Config.PosBacktrack || !g::pLocalEntity->IsAlive())
		return ret;

	CEventInfo *ei = g_pClientState->events;
	CEventInfo *next = NULL;

	if (!ei)
		return ret;

	do
	{
		next = *(CEventInfo**)((uintptr_t)ei + 0x38);

		uint16_t classID = ei->classID - 1;

		auto m_pCreateEventFn = ei->pClientClass->pCreateEventFn;
		if (!m_pCreateEventFn)
			continue;

		IClientNetworkable *pCE = m_pCreateEventFn();
		if (!pCE)
			continue;

		if (classID == 197 ) //ClassId_CTEFireBullets
			ei->fire_delay = 0.0f;

		ei = next;
	} while (next != NULL);

	return ret;
}


void __fastcall Hooks::SceneEnd( void* ecx, void* edx )
{
	static auto oSceneEnd = g_Hooks.pRenderViewHook->GetOriginal<SceneEnd_t>( vtable_indexes::sceneEnd );
	oSceneEnd( ecx, edx );

	static IMaterial* Material = nullptr;
	static IMaterial* Materialz = nullptr;
	static IMaterial* MaterialGlow = nullptr;
	static bool ResetMaterial = false;

	if ( !g::pLocalEntity || !g_pEngine->IsInGame( ) || !g_pEngine->IsConnected( ) )
	{
		Material = nullptr; Materialz = nullptr; MaterialGlow = nullptr;
		return;
	}

	if ( Material == nullptr || Materialz == nullptr )
	{
		Material = g_pMaterialSys->FindMaterial( "FlatChams", TEXTURE_GROUP_MODEL ); Materialz = g_pMaterialSys->FindMaterial( "FlatChams", TEXTURE_GROUP_MODEL );
		MaterialGlow = g_pMaterialSys->FindMaterial( "models/inventory_items/dogtags/dogtags_outline", TEXTURE_GROUP_MODEL );
		return;
	}

	ResetMaterial = true;

	/*static uintptr_t pCall = (uintptr_t)Utils::FindSignature("server.dll", "55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 57 8B CE");

	float fDuration = 1.f;

	PVOID pEntity = nullptr;
	pEntity = UTIL_PlayerByIndex(g::pLocalEntity->EntIndex());

	if (pEntity)
	{
		__asm
		{
			pushad
			movss xmm1, fDuration
			push 0 //bool monoColor
			mov ecx, pEntity
			call pCall
			popad
		}
	}*/

	for ( int i = 1; i < g_pEngine->GetMaxClients( ); ++i )
	{
		C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity( i );

		if ( !pPlayerEntity
			|| !pPlayerEntity->IsAlive( )
			|| pPlayerEntity->IsDormant( )
			|| pPlayerEntity->GetTeam( ) == g::pLocalEntity->GetTeam( ) && pPlayerEntity != g::pLocalEntity )
			continue;

		/*if ( g_Menu.Config.bChamsDesync && pPlayerEntity == g::pLocalEntity ) {
			AnimationLayer backup_layers[ 13 ];
			float backup_poses[ 20 ];
			Color ChamsColorDesync = g_Menu.Config.DesyncChams;

			std::memcpy( backup_poses, g::pLocalEntity->GetPoseParamsModifiable( ), sizeof( backup_poses ) );
			std::memcpy( backup_layers, g::pLocalEntity->AnimOverlays( ), sizeof( backup_layers ) );

			std::memcpy( g::pLocalEntity->GetPoseParamsModifiable( ), g_AnimFix.fake_poses, sizeof( g_AnimFix.fake_poses ) );
			std::memcpy( g::pLocalEntity->AnimOverlays( ), g_AnimFix.fake_layers, sizeof( g_AnimFix.fake_layers ) );

			g_pRenderView->SetColorModulation( ChamsColorDesync.red / 255.f, ChamsColorDesync.green / 255.f, ChamsColorDesync.blue / 255.f );
			g_pModelRender->ForcedMaterialOverride( MaterialGlow );
			MaterialGlow->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
			pPlayerEntity->DrawModel( 0x1, pPlayerEntity->GetModelInstance( ) );
			g_pModelRender->ForcedMaterialOverride( nullptr );

			std::memcpy( g::pLocalEntity->GetPoseParamsModifiable( ), backup_poses, sizeof( backup_poses ) );
			std::memcpy( g::pLocalEntity->AnimOverlays( ), backup_layers, sizeof( backup_layers ) );
		}*/

		if ( g_Menu.Config.bChamsZ && pPlayerEntity != g::pLocalEntity || g_Menu.Config.bChamsZ && g_Menu.Config.LocalChams ) {
			Color ChamsColorZ = g_Menu.Config.InVisChams;
			g_pRenderView->SetColorModulation( ChamsColorZ.red / 255.f, ChamsColorZ.green / 255.f, ChamsColorZ.blue / 255.f );
			g_pModelRender->ForcedMaterialOverride( Materialz );
			Material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, true );
			pPlayerEntity->DrawModel( 0x1, pPlayerEntity->GetModelInstance( ) );
			g_pModelRender->ForcedMaterialOverride( nullptr );
		}

		if ( g_Menu.Config.bChams && pPlayerEntity != g::pLocalEntity || g_Menu.Config.bChams && g_Menu.Config.LocalChams ) {
			Color ChamsColor = g_Menu.Config.VisChams;
			g_pRenderView->SetColorModulation( ChamsColor.red / 255.f, ChamsColor.green / 255.f, ChamsColor.blue / 255.f );
			g_pModelRender->ForcedMaterialOverride( Material );
			Material->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, false );
			pPlayerEntity->DrawModel( 0x1, pPlayerEntity->GetModelInstance( ) );
			g_pModelRender->ForcedMaterialOverride( nullptr );
		}
	}
}

void __fastcall Hooks::DrawModelExecute( void* ecx, void* edx, IMatRenderContext * context, const DrawModelState_t & state, const ModelRenderInfo_t & info, matrix3x4_t * matrix )
{
	static auto oDrawModelExecute = g_Hooks.pModelHook->GetOriginal<DrawModelExecute_t>( vtable_indexes::dme );
	C_BaseEntity* entity = ( C_BaseEntity* )g_pEntityList->GetClientEntity( info.index );

	oDrawModelExecute( ecx, context, state, info, matrix );
}

void DisableInterpolation() {
	for (int i = 1; i < g_pEngine->GetMaxClients(); ++i)
	{
		C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(i);

		if (!pPlayerEntity
			|| !pPlayerEntity->IsAlive())
			continue;
		if (pPlayerEntity->IsDormant())
			continue;

		auto VarMap = reinterpret_cast<uintptr_t>(pPlayerEntity) + 36;
		auto VarMapSize = *reinterpret_cast<int*>(VarMap + 20);

		for (auto index = 0; index < VarMapSize; index++)
			* reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(VarMap) + index * 12) = 0;
	}
}

void HandleAnimationUpdates() {
	for (int i = 1; i < g_pEngine->GetMaxClients(); ++i)
	{
		C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(i);

		if (!pPlayerEntity
			|| !pPlayerEntity->IsAlive())
			continue;
		if (pPlayerEntity->IsDormant())
			continue;
		if (pPlayerEntity == g::pLocalEntity)
			continue;

		static float old_simtime[65];

		if (old_simtime[pPlayerEntity->EntIndex()] != pPlayerEntity->GetSimulationTime()) {
			g::AllowAnimationUpdate[pPlayerEntity->EntIndex()] = true;

			old_simtime[pPlayerEntity->EntIndex()] = pPlayerEntity->GetSimulationTime();
		}
		else {
			g::AllowAnimationUpdate[pPlayerEntity->EntIndex()] = false;
		}

	}
}

void FixAnimations() {
	for (int i = 1; i < g_pEngine->GetMaxClients(); ++i)
	{
		C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(i);

		if (!pPlayerEntity
			|| !pPlayerEntity->IsAlive())
			continue;
		if (pPlayerEntity->IsDormant())
			continue;

		C_AnimState *state = pPlayerEntity->AnimState();
		if (state && pPlayerEntity != g::pLocalEntity)
		{
			// if state, we [REDACTED]
		}
	}
}

void __stdcall Hooks::FrameStageNotify( ClientFrameStage_t curStage )
{
	static auto oFrameStage = g_Hooks.pClientHook->GetOriginal<FrameStageNotify_t>( vtable_indexes::frameStage );

	g_Misc.ThirdPerson( curStage );

	g_Resolver.FrameStage( curStage );

	if (g_Menu.Config.ForceApply && g_pEngine->IsInGame() && g_pEngine->IsConnected() && g::pLocalEntity && g::pLocalEntity->IsAlive() && g::pLocalEntity->GetActiveWeapon())
		g_Skins.KnifeApplyCallbk();

	if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		g_Skins.Run();
	}

	if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_END && g_Menu.Config.PosBacktrack)
	{
		HandleAnimationUpdates();
	}

	if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_END && g_Menu.Config.PosBacktrack)
	{
		DisableInterpolation();
	}

	if (curStage == FRAME_NET_UPDATE_END && g_Menu.Config.PosBacktrack)
	{
		FixAnimations();
	}

	oFrameStage( curStage );
}

void __fastcall Hooks::PaintTraverse( PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce )
{
	static auto oPaintTraverse = g_Hooks.pPanelHook->GetOriginal<PaintTraverse_t>( vtable_indexes::paint );

	static unsigned int panelID, panelHudID;

	if ( !panelHudID )
		if ( !strcmp( "HudZoom", g_pPanel->GetName( vguiPanel ) ) )
		{
			panelHudID = vguiPanel;
		}

	if ( panelHudID == vguiPanel && g::pLocalEntity && g::pLocalEntity->IsAlive( ) && g_Menu.Config.NoScope )
	{
		if ( g::pLocalEntity->IsScoped( ) )
			return;
	}

	oPaintTraverse( pPanels, vguiPanel, forceRepaint, allowForce );

	if ( !panelID )
		if ( !strcmp( "MatSystemTopPanel", g_pPanel->GetName( vguiPanel ) ) )
		{
			panelID = vguiPanel;
			g_Hooks.bInitializedDrawManager = true;
		}

	if ( panelID == vguiPanel )
	{
		g_ESP.Render( );
		g_Misc.RemoveSmoke( );
		g_Misc.RemoveFlashbangs( );
		g_Misc.Nightmode( );
		g_Misc.HitmarkerDraw( );
		g_Misc.Viewmodel_Shit( );
		g_Misc.Watermark( );
		g_Misc.ScopeLines( );
		g_Misc.Indicators( );
		g_Misc.Crosshair( );
		//g_Menu.draw_hitgroups( );
		//g_Menu.draw_radar( ); //fuck your hitgroups HAH
		g_Menu.draw( );

		for ( int i = 0; i < 4; i++ ) {
			for ( int j = 0; j < 4; j++ )
			{
				g::w2s_matrix[ i ][ j ] = g_pEngine->WorldToScreenMatrix( )[ i ][ j ];
			}
		}

		if ( g_Menu.Config.GrenadePrediction )
			GrenadePrediction::get( ).Paint( );


		if(g_Menu.Config.Logs)
			g_Logs->DrawLogs( );

		if (g_Menu.Config.RemoveFakelagLimit)
			SetMoveChokeClampLimit(); //remove client side fakelag limit

		if (!g_Menu.Config.RemoveFakelagLimit && fuck_you_tommy)
			ResetMoveChokeClampLimit();

		if (!fuck_you_tommy)
			std::clamp(g_Menu.Config.MaxProcessTicks, 0, 15); // might cause problems.
	}
}

auto get_hitgroup_to_string = [ ]( int hitgroup ) -> std::string {
	switch ( hitgroup ) {
	case 1: return "head"; break;
	case 2: return "chest"; break;
	case 3: return "stomach"; break;
	case 4: return "left arm"; break;
	case 5: return "right arm"; break;
	case 6: return "left leg"; break;
	case 7: return "right leg"; break;
	case 8: return "pelvis"; break;
	default: return "generic"; break;
	}
};

void Event::FireGameEvent( IGameEvent * event )
{
	if ( !event )
		return;

	g_pBulletTracers.events( event );

	if ( !g::pLocalEntity )
		return;

	auto attacker = g_pEntityList->GetClientEntity( g_pEngine->GetPlayerForUserID( event->GetInt( "attacker" ) ) );

	if ( !attacker )
		return;

	if ( attacker != g::pLocalEntity )
		return;

	int index = g_pEngine->GetPlayerForUserID( event->GetInt( "userid" ) );

	PlayerInfo_t pInfo;
	g_pEngine->GetPlayerInfo( index, &pInfo );

	g::Hit[ index ] = true;

	C_BaseEntity* hit_entity = g_pEntityList->GetClientEntity( index );

	if (hit_entity != g::pLocalEntity ) // no reason to do logs / hitmarker if we hurt ourselves
	{
		if (g_Menu.Config.Logs) //only log if logs are on
			g_Logs->OnFireEvent( index, event->GetInt( "dmg_health" ), get_hitgroup_to_string( event->GetInt( "hitgroup" ) ), event->GetInt( "health" ), false, false );
		if ( g_Menu.Config.HitmarkerSound )
			g_pEngine->ExecuteClientCmd( "play buttons\\light_power_on_switch_01.wav" );
		g::hitmarkerTime = 255;
	}
};

void __fastcall Hooks::OverrideView( void* ecx, void* edx, CViewSetup * pSetup )
{
	static auto oOverrideView = g_Hooks.pClientModeHook->GetOriginal<OverrideView_t>( vtable_indexes::view );

	if ( g_pEngine->IsConnected( ) && g_pEngine->IsInGame( ) )
	{
		if ( !g::pLocalEntity )
			return;

		if ( !g::pLocalEntity->IsAlive( ) )
			return;

		if ( g_Menu.Config.GrenadePrediction )
			GrenadePrediction::get( ).View( pSetup );

		if ( g_Menu.Config.NoRecoil )
		{
			Vector viewPunch = g::pLocalEntity->GetViewPunchAngle( );
			Vector aimPunch = g::pLocalEntity->GetAimPunchAngle( );

			pSetup->angles[ 0 ] -= ( viewPunch[ 0 ] + ( aimPunch[ 0 ] * 2 * 0.4499999f ) );
			pSetup->angles[ 1 ] -= ( viewPunch[ 1 ] + ( aimPunch[ 1 ] * 2 * 0.4499999f ) );
			pSetup->angles[ 2 ] -= ( viewPunch[ 2 ] + ( aimPunch[ 2 ] * 2 * 0.4499999f ) );
		}

		if ( g_Menu.Config.Fov != 0 && !g::pLocalEntity->IsScoped( ) )
			pSetup->fov = g_Menu.Config.Fov;

		if ( g_Menu.Config.NoZoom && g::pLocalEntity->IsScoped( ) )
			pSetup->fov = ( g_Menu.Config.Fov == 0 ) ? 90 : g_Menu.Config.Fov;
	}

	oOverrideView( ecx, edx, pSetup );
}

void __fastcall Hooks::LockCursor( ISurface * thisptr, void* edx )
{
	static auto oLockCursor = g_Hooks.pSurfaceHook->GetOriginal<LockCursor_t>( vtable_indexes::lockCursor );

	if ( !g_Menu.menu_opened )
		return oLockCursor( thisptr, edx );

	g_pSurface->UnLockCursor( );
}

void __fastcall Hooks::PostScreenEffects( void* thisptr, void* edx, CViewSetup * setup )
{
	static auto run_glow = [ ]( ) -> void
	{
		if ( !( g_pEngine->IsInGame( ) && g_pEngine->IsConnected( ) ) || !g::pLocalEntity ) {
			return;
		}

		for ( int i = 0; i < g_pGlow->m_GlowObjectDefinitions.Count( ); i++ )
		{
			if ( g_pGlow->m_GlowObjectDefinitions[ i ].IsUnused( ) || !g_pGlow->m_GlowObjectDefinitions[ i ].GetEnt( ) ) {
				continue;
			}

			auto* object = &g_pGlow->m_GlowObjectDefinitions[ i ];
			auto entity = reinterpret_cast< C_BaseEntity* >( object->GetEnt( ) );

			if ( !entity || entity->IsDormant( ) || !entity->IsAlive( ) ) {
				continue;
			}

			float color[ 3 ];
			float color_local[ 3 ];

			color[ 0 ] = g_Menu.Config.GlowColor.red / 255.0f;
			color[ 1 ] = g_Menu.Config.GlowColor.green / 255.0f;
			color[ 2 ] = g_Menu.Config.GlowColor.blue / 255.0f;

			color_local[ 0 ] = g_Menu.Config.LocalGlowColor.red / 255.0f;
			color_local[ 1 ] = g_Menu.Config.LocalGlowColor.green / 255.0f;
			color_local[ 2 ] = g_Menu.Config.LocalGlowColor.blue / 255.0f;

			if ( g_Menu.Config.Glow && entity->GetClientClass( )->ClassID == ( int )40 && !( entity->GetTeam( ) == g::pLocalEntity->GetTeam( ) ) || g_Menu.Config.LocalGlow && entity == g::pLocalEntity ) // ent
			{
				if ( entity == g::pLocalEntity )
					object->Set( color_local[ 0 ], color_local[ 1 ], color_local[ 2 ], 1.f, 1.f, g_Menu.Config.LocalGlowStyle );
				else
					object->Set( color[ 0 ], color[ 1 ], color[ 2 ], 1.f, 1.f, g_Menu.Config.GlowStyle );
			}
		}
	};

	static auto oDoPostScreenEffects = g_Hooks.pClientModeHook->GetOriginal<DoPostScreenEffects_t>( vtable_indexes::dopostscreeneffects );

	if ( g_Menu.Config.Glow || g_Menu.Config.LocalGlow )
		run_glow( );

	return oDoPostScreenEffects( thisptr, setup );
}

LRESULT Hooks::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// for now as a lambda, to be transfered somewhere
	// Thanks uc/WasserEsser for pointing out my mistake!
	// Working when you HOLD th button, not when you press it.
	const auto getButtonHeld = [ uMsg, wParam ]( bool& bButton, int vKey )
	{
		if ( wParam != vKey ) return;

		if ( uMsg == WM_KEYDOWN )
			bButton = true;
		else if ( uMsg == WM_KEYUP )
			bButton = false;
	};

	const auto getButtonToggle = [ uMsg, wParam ]( bool& bButton, int vKey )
	{
		if ( wParam != vKey ) return;

		if ( uMsg == WM_KEYUP )
			bButton = !bButton;
	};

	if ( g_Hooks.bInitializedDrawManager )
	{
		// our wndproc capture fn
		if ( g_Menu.menu_opened )
		{
			return true;
		}
	}

	return CallWindowProcA( g_Hooks.pOriginalWNDProc, hWnd, uMsg, wParam, lParam );
}