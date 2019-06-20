#pragma once
#include "..\..\SDK\Definitions.h"
#include "..\Aimbot\Autowall.h"
#include "..\Aimbot\Aimbot.h"
#include "..\Aimbot\LagComp.h"
#include "..\..\Utils\GlobalVars.h"
#include "..\..\Utils\Math.h"
#include "..\..\Utils\XorStr.h"
#include "..\..\SDK\CClientState.h"
#include "..\..\SDK\ICvar.h"
#include "..\..\SDK\CPrediction.h"
#include "..\..\Menu\Menu.h"
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <random>
#include <fstream>
#include <Windows.h>

// kinda just throw shit in here

#define _SOLVEY(a, b, c, d, e, f) ((c * b - d * a) / (c * f - d * e))
#define SOLVEY(...) _SOLVEY(?, ?, ?, ?, ?, ?)
#define SOLVEX(y, world, forward, right) ((world.x - right.x * y) / forward.x)

static int direction = 0;

class Misc {
public:
	void OnCreateMove( )
	{
		this->pCmd = g::pCmd;
		this->pLocal = g::pLocalEntity;

		this->DoAutostrafe( );
		this->DoBhop( );
		this->DoFakeLag( );
		this->DoClantag( ); // L
		this->SlowWalk( pCmd );
	};

	void MovementFix( Vector& oldang )
	{
		Vector vMovements( g::pCmd->forwardmove, g::pCmd->sidemove, 0.f );

		if ( vMovements.Length2D( ) == 0 )
			return;

		Vector vRealF, vRealR;
		Vector aRealDir = g::pCmd->viewangles;
		aRealDir.Clamp( );

		g_Math.AngleVectors( aRealDir, &vRealF, &vRealR, nullptr );
		vRealF[ 2 ] = 0;
		vRealR[ 2 ] = 0;

		VectorNormalize( vRealF );
		VectorNormalize( vRealR );

		Vector aWishDir = oldang;
		aWishDir.Clamp( );

		Vector vWishF, vWishR;
		g_Math.AngleVectors( aWishDir, &vWishF, &vWishR, nullptr );

		vWishF[ 2 ] = 0;
		vWishR[ 2 ] = 0;

		VectorNormalize( vWishF );
		VectorNormalize( vWishR );

		Vector vWishVel;
		vWishVel[ 0 ] = vWishF[ 0 ] * g::pCmd->forwardmove + vWishR[ 0 ] * g::pCmd->sidemove;
		vWishVel[ 1 ] = vWishF[ 1 ] * g::pCmd->forwardmove + vWishR[ 1 ] * g::pCmd->sidemove;
		vWishVel[ 2 ] = 0;

		float a = vRealF[ 0 ], b = vRealR[ 0 ], c = vRealF[ 1 ], d = vRealR[ 1 ];
		float v = vWishVel[ 0 ], w = vWishVel[ 1 ];

		float flDivide = ( a * d - b * c );
		float x = ( d * v - b * w ) / flDivide;
		float y = ( a * w - c * v ) / flDivide;

		g::pCmd->forwardmove = x;
		g::pCmd->sidemove = y;
	}

	bool InThirdperson = false;

	void ThirdPerson( ClientFrameStage_t curStage )
	{
		if ( !g_pEngine->IsInGame( ) || !g_pEngine->IsConnected( ) || !g::pLocalEntity )
			return;

		if ( !g_Menu.Config.Thirdperson_NIGGA )
			return;

		static bool init = false;

		//TODO: Stop forcing sv_cheats to be on. Spoof it properly.

		if ( GetKeyState( g_Menu.Config.Thirdperson_NIGGA_Key ) && g::pLocalEntity->IsAlive( ) )
		{
			if ( init )
			{
				InThirdperson = !InThirdperson;
				ConVar* sv_cheats = g_pCvar->FindVar( "sv_cheats" );
				*( int* )( ( DWORD )& sv_cheats->fnChangeCallback + 0xC ) = 0; // ew
				sv_cheats->SetValue( 1 );
				g_pEngine->ExecuteClientCmd( "thirdperson" );
			}
			init = false;
		}
		else
		{
			if ( !init )
			{
				ConVar* sv_cheats = g_pCvar->FindVar( "sv_cheats" );
				*( int* )( ( DWORD )& sv_cheats->fnChangeCallback + 0xC ) = 0; // ew
				sv_cheats->SetValue( 1 );
				g_pEngine->ExecuteClientCmd( "firstperson" );
			}
			init = true;
		}

		if ( curStage == FRAME_RENDER_START && GetKeyState( g_Menu.Config.Thirdperson_NIGGA_Key ) && g::pLocalEntity->IsAlive( ) ) {
			g_pPrediction->SetLocalViewAngles( g::RealAngle );
			g::pLocalEntity->UpdateClientAnimation();
		}
	}

	void NormalWalk( ) // heh
	{
		g::pCmd->buttons &= ~IN_MOVERIGHT;
		g::pCmd->buttons &= ~IN_MOVELEFT;
		g::pCmd->buttons &= ~IN_FORWARD;
		g::pCmd->buttons &= ~IN_BACK;

		if ( g::pCmd->forwardmove > 0.f )
			g::pCmd->buttons |= IN_FORWARD;
		else if ( g::pCmd->forwardmove < 0.f )
			g::pCmd->buttons |= IN_BACK;
		if ( g::pCmd->sidemove > 0.f )
		{
			g::pCmd->buttons |= IN_MOVERIGHT;
		}
		else if ( g::pCmd->sidemove < 0.f )
		{
			g::pCmd->buttons |= IN_MOVELEFT;
		}
	}

	void FakeDuck( ) const
	{
		if ( !( g::pLocalEntity->GetFlags( ) & FL_ONGROUND ) )
			return;

		if ( !g_Menu.Config.FakeDuckHAH )
			return;

		if ( !GetAsyncKeyState( g_Menu.Config.FakeDuckHAH_Key ) )
			return;

		g::pCmd->buttons |= IN_BULLRUSH;

		auto MaxProcessTicks = g_Menu.Config.MaxProcessTicks == 0 ? 57 : g_Menu.Config.MaxProcessTicks;

		if ( g_pClientState->m_nChokedCommands <= (MaxProcessTicks - 8))
		{
			g::pCmd->buttons &= ~IN_DUCK;
		}
		else
		{
			g::pCmd->buttons |= IN_DUCK;
		}

		if ( g_pClientState->m_nChokedCommands < (MaxProcessTicks) )
		{
			g::bSendPacket = false;
		}
		else
		{
			g::bSendPacket = true;
		}
	}

	void RemoveSmoke( ) const
	{
		if ( !g_pEngine->IsConnected( ) || !g_pEngine->IsInGame( ) ) return;
		if ( !g::pLocalEntity ) return;

		static auto smoke_count = *reinterpret_cast< uint32_t * * >( Utils::FindSignature( "client_panorama.dll", "A3 ? ? ? ? 57 8B CB" ) + 1 );

		static std::vector<const char*> smoke_materials = {
			"effects/overlaysmoke",
			"particle/beam_smoke_01",
			"particle/particle_smokegrenade",
			"particle/particle_smokegrenade1",
			"particle/particle_smokegrenade2",
			"particle/particle_smokegrenade3",
			"particle/particle_smokegrenade_sc",
			"particle/smoke1/smoke1",
			"particle/smoke1/smoke1_ash",
			"particle/smoke1/smoke1_nearcull",
			"particle/smoke1/smoke1_nearcull2",
			"particle/smoke1/smoke1_snow",
			"particle/smokesprites_0001",
			"particle/smokestack",
			"particle/vistasmokev1/vistasmokev1",
			"particle/vistasmokev1/vistasmokev1_emods",
			"particle/vistasmokev1/vistasmokev1_emods_impactdust",
			"particle/vistasmokev1/vistasmokev1_nearcull",
			"particle/vistasmokev1/vistasmokev1_nearcull_fog",
			"particle/vistasmokev1/vistasmokev1_nearcull_nodepth",
			"particle/vistasmokev1/vistasmokev1_smokegrenade",
			"particle/vistasmokev1/vistasmokev4_emods_nocull",
			"particle/vistasmokev1/vistasmokev4_nearcull",
			"particle/vistasmokev1/vistasmokev4_nocull"
		};

		for ( auto material_name : smoke_materials ) {
			IMaterial* material = g_pMaterialSys->FindMaterial( material_name, "Other textures" );
			if ( material )
				material->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, g_Menu.Config.NoSmoke );
		}

		*( int* )smoke_count = 0;
	}

	static void set_clantag( const char* tag, const char* name ) {
		static auto p_set_clantag = reinterpret_cast< void( __fastcall* )( const char*, const char* ) >( ( DWORD )( Utils::FindSignature( "engine.dll", "53 56 57 8B DA 8B F9 FF 15" ) ) );
		p_set_clantag( tag, name );
	}

	void DoClantag() const {
		if (!g_pEngine->IsConnected() || !g_pEngine->IsInGame()) return;
		if (!g_Menu.Config.Clantag) return;

		static std::string cur_clantag = XorStr(" wanheda ");
		static float oldtime;

		if (g_pGlobalVars->curtime - oldtime >= 0.5f)
		{
			std::string temp = (std::string&)cur_clantag;
			cur_clantag.erase(0, 1);
			cur_clantag += temp[0];
			set_clantag(cur_clantag.c_str(), XorStr("wanheda"));
			oldtime = g_pGlobalVars->curtime;
		}
	}

	void RemoveFlashbangs( ) const
	{
		if ( !g_pEngine->IsConnected( ) || !g_pEngine->IsInGame( ) ) return;
		if ( !g::pLocalEntity ) return;
		if ( !g_Menu.Config.NoFlash ) return;

		if ( g::pLocalEntity && g::pLocalEntity->m_flFlashDuration( ) > 0.0f )
			g::pLocalEntity->m_flFlashDuration( ) = 0.0f;
	}

	void LoadSky( const char* sky_name ) const
	{
		static auto fnLoadSky = ( void( __fastcall* )( const char* ) )Utils::FindSignature( "engine.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45" );
		fnLoadSky( sky_name );
	}

	void Nightmode( ) const
	{
		static bool nightmode_performed = false, nightmode_lastsetting;

		if ( !g_pEngine->IsConnected( ) || !g_pEngine->IsInGame( ) )
		{
			if ( nightmode_performed )
				nightmode_performed = false;
			return;
		}

		if ( !g::pLocalEntity )
		{
			if ( nightmode_performed )
				nightmode_performed = false;
			return;
		}

		if ( nightmode_lastsetting != g_Menu.Config.Nightmode )
		{
			nightmode_lastsetting = g_Menu.Config.Nightmode;
			nightmode_performed = false;
		}

		if ( !nightmode_performed )
		{
			for ( MaterialHandle_t i = g_pMaterialSys->FirstMaterial( ); i != g_pMaterialSys->InvalidMaterial( ); i = g_pMaterialSys->NextMaterial( i ) )
			{
				IMaterial* pMaterial = g_pMaterialSys->GetMaterial( i );

				if ( !pMaterial )
					continue;

				if ( g_Menu.Config.Nightmode )
					LoadSky( "sky_descent" );

				if ( strstr( pMaterial->GetTextureGroupName( ), "World" ) )
				{
					if ( g_Menu.Config.Nightmode )
						pMaterial->ColorModulate( g_Menu.Config.Nightval / 100.f, g_Menu.Config.Nightval / 100.f, g_Menu.Config.Nightval / 100.f );
					else
						pMaterial->ColorModulate( 1, 1, 1 );

					if ( g_Menu.Config.Nightmode )
					{
						pMaterial->SetMaterialVarFlag( MATERIAL_VAR_TRANSLUCENT, false );
						pMaterial->ColorModulate( g_Menu.Config.Nightval / 100.f, g_Menu.Config.Nightval / 100.f, g_Menu.Config.Nightval / 100.f );
					}
					else
					{
						pMaterial->ColorModulate( 1.00, 1.00, 1.00 );
					}
				}
				else if ( strstr( pMaterial->GetTextureGroupName( ), "StaticProp" ) )
				{
					if ( g_Menu.Config.Nightmode )
						pMaterial->ColorModulate( g_Menu.Config.Nightval / 100.f, g_Menu.Config.Nightval / 100.f, g_Menu.Config.Nightval / 100.f );
					else
						pMaterial->ColorModulate( 1, 1, 1 );
				}
				else if ( strstr( pMaterial->GetTextureGroupName( ), "SkyBox" ) )
				{
					if ( g_Menu.Config.Nightmode )
						pMaterial->ColorModulate( g_Menu.Config.Skybox.red / 100.f, g_Menu.Config.Skybox.green / 100.f, g_Menu.Config.Skybox.blue / 100.f );
					else
						pMaterial->ColorModulate( 1, 1, 1 );
				}
			}
			nightmode_performed = true;
		}
	} // Yeah bro, cam cringe bro.... Bag un blunt... M-a prajit M-a spart:)))) LOL!

	void Viewmodel_Shit( ) const {
		static auto view_x_backup = g_pCvar->FindVar( "viewmodel_offset_x" )->GetInt( );
		static auto view_y_backup = g_pCvar->FindVar( "viewmodel_offset_y" )->GetInt( );
		static auto view_z_backup = g_pCvar->FindVar( "viewmodel_offset_z" )->GetInt( );
		static ConVar* view_x = g_pCvar->FindVar( "viewmodel_offset_x" );
		static ConVar* view_y = g_pCvar->FindVar( "viewmodel_offset_y" );
		static ConVar* view_z = g_pCvar->FindVar( "viewmodel_offset_z" );

		static ConVar* bob = g_pCvar->FindVar( "cl_bobcycle" ); // sv_competitive_minspec 0

		ConVar* sv_minspec = g_pCvar->FindVar( "sv_competitive_minspec" );
		*( int* )( ( DWORD )& sv_minspec->fnChangeCallback + 0xC ) = 0; // ew
		sv_minspec->SetValue( 0 );

		bob->SetValue( 0.98f );
		if ( g_Menu.Config.ViewmodelAdjust ) {
			view_x->SetValue( g_Menu.Config.ViewmodelX - 10 );
			view_y->SetValue( g_Menu.Config.ViewmodelY - 10 );
			view_z->SetValue( g_Menu.Config.ViewmodelZ - 10 );
		}
		else {
			view_x->SetValue( 1 );
			view_y->SetValue( 2 );
			view_z->SetValue( -2 );
		}
	}

	void HitmarkerDraw( )
	{
		if ( !g_pEngine->IsInGame( ) || !g_pEngine->IsConnected( ) || !g::pLocalEntity )
			return;

		/* screen size */
		int screenWidth, screenHeight;
		g_pEngine->GetScreenSize( screenWidth, screenHeight );

		/* screen middle */
		int ScreenWidthMid = screenWidth / 2;
		int ScreenHeightMid = screenHeight / 2;

		static int lineSize = 5;

		/* draw the hitmarker */
		if ( g::hitmarkerTime > 0 )
		{
			/* set alpha to hitmarker time */
			float alpha = g::hitmarkerTime;

			Color color_hitmarker = Color( g_Menu.Config.Hitcolor.red, g_Menu.Config.Hitcolor.green, g_Menu.Config.Hitcolor.blue, alpha );

			if ( g_Menu.Config.Hitmarker )
			{
				g_pSurface->Line( ScreenWidthMid - lineSize * 2, ScreenHeightMid - lineSize * 2, ScreenWidthMid - ( lineSize ), ScreenHeightMid - ( lineSize ), color_hitmarker );
				g_pSurface->Line( ScreenWidthMid - lineSize * 2, ScreenHeightMid + lineSize * 2, ScreenWidthMid - ( lineSize ), ScreenHeightMid + ( lineSize ), color_hitmarker );
				g_pSurface->Line( ScreenWidthMid + lineSize * 2, ScreenHeightMid + lineSize * 2, ScreenWidthMid + ( lineSize ), ScreenHeightMid + ( lineSize ), color_hitmarker );
				g_pSurface->Line( ScreenWidthMid + lineSize * 2, ScreenHeightMid - lineSize * 2, ScreenWidthMid + ( lineSize ), ScreenHeightMid - ( lineSize ), color_hitmarker );
			}

			/* hitmarker fade animation */
			g::hitmarkerTime -= 4;
		}
	}

	void Watermark( )
	{
		if ( !g_Menu.Config.Watermark )
			return;

		int screenx, screeny;
		g_pEngine->GetScreenSize( screenx, screeny );

		if ( g_Menu.Config.Watermarks == 0 ) {
			g_pSurface->DrawT( 4, 2, Color( 255, 255, 255 ), g::fonts::manual_antiaim_font, false, "0" );
		}

		if ( g_Menu.Config.Watermarks == 1 ) {
			int x = 25, y = 25, w = 209,
				h = 20, _x = 25, _w = 100;

			// main
			// REMAKE THIS
		}
	}

	void AutoDerank( ) {

		if ( !g_Menu.Config.AutoDerank )
			return;

		if ( g_pEngine->IsInGame( ) && g_pEngine->IsConnected( ) ) {
			for ( int time; time > 9000; time++ ) {
				g_pEngine->ExecuteClientCmd( "disconnect" );
			}
		}
	}

	void AFKBot( ) {
		if ( !g_Menu.Config.AfkBot )
			return;

		if ( g_pEngine->IsInGame( ) && g_pEngine->IsConnected( ) ) {
			g_pEngine->ExecuteClientCmd( "+forward" );
			g_pEngine->ExecuteClientCmd( "+right" );
		}
	}

	void Crosshair( )
	{
		int x, y;
		g_pEngine->GetScreenSize( x, y );

		if ( !g_pEngine->IsConnected( ) || !g_pEngine->IsInGame( ) )
			return;

		if ( !g::pLocalEntity )
			return;

		if ( !g::pLocalEntity->GetLifeState( ) == 0 )
			return;

		if ( !g_Menu.Config.Crosshair )
			return;

		g_pSurface->FilledRect( x / 2, y / 2 - 10, 1, 20, Color( 0, 226, 255 ) );
		g_pSurface->FilledRect( x / 2 - 10, y / 2, 20, 1, Color( 0, 226, 255 ) );

		return;
	}

	void ScopeLines( )
	{
		if ( !g_pEngine->IsInGame( ) || !g_pEngine->IsConnected( ) || !g::pLocalEntity )
			return;
		if ( g_Menu.Config.NoScope && g::pLocalEntity )
		{
			int screen_x, screen_y, center_x, center_y;
			g_pEngine->GetScreenSize( screen_x, screen_y );
			center_x = screen_x / 2;
			center_y = screen_y / 2;

			if ( g::pLocalEntity->IsScoped( ) )
			{
				g_pSurface->Line( 0, center_y, screen_x, center_y, Color( 0, 0, 0, 220 ) );
				g_pSurface->Line( center_x, 0, center_x, screen_y, Color( 0, 0, 0, 220 ) );
			}
		}
	}

	double clamp( double x, double upper, double lower )
	{
		return min( upper, max( x, lower ) );
	}

	void DisablePostProcess( ) {
		static auto mat_postprocess_enable = g_pCvar->FindVar( "mat_postprocess_enable" );
		mat_postprocess_enable->SetValue( g_Menu.Config.DisablePostProcessing ? 0 : 1 );
	}

	void RecoilCrosshair( )  {
		if ( !g_Menu.Config.RecoilCrosshair )
			return;

		static auto cl_crosshair_recoil = g_pCvar->FindVar( "cl_crosshair_recoil" );
		cl_crosshair_recoil->SetValue( g_Menu.Config.RecoilCrosshair ? 1 : 0 );
	}

	void RankReveal( ) {
		if ( !g_Menu.Config.RevealRanks )
			return;

		if ( GetAsyncKeyState( VK_TAB ) )
			g_pClientDll->DispatchUserMessage( cs_um_serverrankrevealall, 0, 0, nullptr );
	}

	void Indicators( )
	{
		if ( !g_pEngine->IsInGame( ) || !g_pEngine->IsConnected( ) || !g::pLocalEntity )
			return;

		// not my comments
		int screen_width, screen_height;
		g_pEngine->GetScreenSize( screen_width, screen_height );

		if ( g_pEngine->IsConnected( ) && g_pEngine->IsInGame( ) )
		{
			if ( g::pLocalEntity )
			{
				auto NetChannel = g_pEngine->GetNetChannel( );

				if ( !g::pLocalEntity->IsAlive( ) )
					return;

				if ( g_Menu.Config.Arrows ) {
					auto client_viewangles = Vector( );
					g_pEngine->GetViewAngles( client_viewangles );
					constexpr auto radius = 120.f;
					const auto screen_center = Vector2D( screen_width / 2.f, screen_height / 2.f );
					auto draw_arrow = [ & ]( float rot, Color color ) -> void
					{
						Vector2D upperCorner, lowerCorner, farestCorner, midCornerUp, midCornerLow, mid;
						mid = Vector2D( screen_width / 2.f, screen_height / 2.f );
						farestCorner = ( Vector2D( screen_center.x + cosf( rot ) * radius, screen_center.y + sinf( rot ) * radius ) );
						lowerCorner = ( Vector2D( screen_center.x + cosf( rot + DEG2RAD( 20 ) ) * ( 80.f ), screen_center.y + sinf( rot + DEG2RAD( 12 ) ) * ( 80.f ) ) ); //25
						upperCorner = ( Vector2D( screen_center.x + cosf( rot - DEG2RAD( 20 ) ) * ( 80.f ), screen_center.y + sinf( rot - DEG2RAD( 12 ) ) * ( 80.f ) ) ); //25
						midCornerLow = ( Vector2D( screen_center.x + cosf( rot + DEG2RAD( 9 ) ) * ( 80.f ), screen_center.y + sinf( rot + DEG2RAD( 9 ) ) * ( 80.f ) ) ); //25
						midCornerUp = ( Vector2D( screen_center.x + cosf( rot - DEG2RAD( 9 ) ) * ( 80.f ), screen_center.y + sinf( rot - DEG2RAD( 9 ) ) * ( 80.f ) ) ); //25
						/* Step 1 */	g_pSurface->Line( upperCorner.x, upperCorner.y, farestCorner.x, farestCorner.y, color ); //from upper corner to top
						/* Step 2 */	g_pSurface->Line( farestCorner.x, farestCorner.y, lowerCorner.x, lowerCorner.y, color ); //top to lower corner

						/* Step 3 */	g_pSurface->Line( lowerCorner.x, lowerCorner.y, midCornerLow.x, midCornerLow.y, color ); //lower corner to mid lower corner

						/* Step 4 */	g_pSurface->Line( midCornerLow.x, midCornerLow.y, mid.x, mid.y, color ); //mid lower corner to mid
						/* Step 5 */	g_pSurface->Line( mid.x, mid.y, midCornerUp.x, midCornerUp.y, color ); //mid to upper mid corner

						/* Step 6 */	g_pSurface->Line( midCornerUp.x, midCornerUp.y, upperCorner.x, upperCorner.y, color ); //mid upper corner to upper corner
					};
					static const auto real_color = Color( 102, 205, 0, 200 );
					float real_rot = 0;
					real_rot = DEG2RAD( client_viewangles.y - g::RealAngle.y - 90 );
					draw_arrow( real_rot, real_color );
					static const auto fake_color = Color( 205, 0, 0, 200 );
					const auto fake_rot = DEG2RAD( client_viewangles.y - g::TpAngle.y - 90 );
					draw_arrow( fake_rot, fake_color );
				}

				if ( !g_Menu.Config.Indicators )
					return;

				/*if ( NetChannel && g_Menu.Config.Fakelag )
				{
					int choke = NetChannel->choked_packets;
					int _y;
					if ( !g_Menu.Config.DesyncAngle )
						_y = screen_height - 95;
					else if ( !( GetAsyncKeyState( g_Menu.Config.FakeDuckHAH_Key ) || !g_Menu.Config.FakeDuckHAH ) )
						_y = screen_height - 95 + 34;
					else
						_y = screen_height - 95 + 68;
					std::string packetts = std::to_string( choke );
					g_pSurface->DrawT( 5, _y, Color( 255, 255, 255, 255 ), g::fonts::flags_font, false, packetts.c_str( ) );
				}*/

				if ( GetAsyncKeyState( g_Menu.Config.FakeDuckHAH_Key ) && g_Menu.Config.FakeDuckHAH && g::pLocalEntity->GetFlags() & FL_ONGROUND)
					g_pSurface->DrawT( 5, screen_height - 95 + 34, Color( 21, 15, 220, 255 ), g::fonts::flags_font, false, "FD" );

				if (NetChannel && g_Menu.Config.Fakelag || NetChannel && GetAsyncKeyState(g_Menu.Config.FakeDuckHAH_Key) && g_Menu.Config.FakeDuckHAH && g::pLocalEntity->GetFlags() & FL_ONGROUND) {
					int x2, y2;
					x2 = 25;
					if (!g_Menu.Config.DesyncAngle)
						y2 = screen_height / 2;
					else
						y2 = screen_height / 2 - 37;

					int choked_packets = NetChannel->choked_packets;

					if (choked_packets < 1) //prevent dividing by 0
						choked_packets = 1;

					float choke = (g_Menu.Config.MaxProcessTicks + 1) / choked_packets;
					float width = 125 / choke;
					std::string text = "CHOKE : " + std::to_string(NetChannel->choked_packets);

					// -- tommy sorry for deleting the string here but it semt like the thing was screaming at me it was getting scary ok

					g_pSurface->DrawT(30 + 125 / 2, y2 - 14, Color(255, 255, 255, 255), g::fonts::menu_font, true, text.c_str());

					g_pSurface->FilledRect(25, y2, 125, 20, Color(50, 50, 50, 75));
					g_pSurface->FilledRect(25 + 2, y2 + 2, width - 4, 20 - 4, g_Menu.Config.AccentColour);
				}
				
				if ( !g_Menu.Config.Antiaim )
					return;

				if ( !g_Menu.Config.DesyncAngle )
					return;

				float delta = g::pLocalEntity->GetMaxDesyncDelta();

				if (delta < 1)
					delta = 1;

				float desync = 58 / delta;

				if (desync < 1)
					desync = 1;

				float width = 125 / desync;

				std::string text = "DESYNC : " + std::to_string((int)g::pLocalEntity->GetMaxDesyncDelta());
				g_pSurface->DrawT(30 + 125 / 2, screen_height / 2 - 14, Color(255, 255, 255, 255), g::fonts::menu_font, true, text.c_str());

				g_pSurface->FilledRect(25, screen_height / 2, 125, 20, Color(50, 50, 50, 75));
				g_pSurface->FilledRect(25 + 2, (screen_height / 2) + 2, width - 4, 20 - 4, g_Menu.Config.AccentColour);

				/*auto desync = g::pLocalEntity->GetMaxDesyncDelta() / 58;

				if (desync > 1) //clamp
					desync = 1;

				Color culoare( ( 1.f - desync ) * 255.f, desync * 255.f, 0 );
				Color Culoaredone = Color( culoare.red, culoare.green, culoare.blue );
				std::string desync_txt = "DESYNC";

				g_pSurface->DrawT( 5, screen_height - 95, Culoaredone, g::fonts::flags_font, false, desync_txt.c_str( ) );*/
			}
		}
	}

#define square( x ) ( x * x )
	inline float FastSqrt2( float x )
	{
		unsigned int i = *( unsigned int* )& x;
		i += 127 << 23;
		i >>= 1;
		return *( float* )& i;
	}

	void SlowWalk( CUserCmd * pCmd )
	{
		if ( !g_Menu.Config.SlowWalkHAH )
			return;

		if ( !GetAsyncKeyState( g_Menu.Config.SlowWalkHAH_Key ) )
			return;

		auto weapon = g::pLocalEntity->GetActiveWeapon( );

		if ( !weapon )
			return;

		float yes = 34.f;
		float fSpeed = ( float )( FastSqrt2( square( pCmd->forwardmove ) + square( pCmd->sidemove ) + square( pCmd->upmove ) ) );
		if ( fSpeed <= 0.f )
			return;
		if ( pCmd->buttons & IN_DUCK )
			yes *= 2.94117647f; // why not just 2.9f
		if ( fSpeed <= yes )
			return;
		float fRatio = yes / fSpeed;
		pCmd->forwardmove *= fRatio;
		pCmd->sidemove *= fRatio;
		pCmd->upmove *= fRatio;
	}

private:
	CUserCmd * pCmd;
	C_BaseEntity * pLocal;

	void DoBhop( ) const
	{
		if ( !g_Menu.Config.Bhop )
			return;

		if ( !g::pLocalEntity->IsAlive( ) )
			return;

		if (g::pCmd->buttons & IN_JUMP && !(g::pLocalEntity->GetFlags() & FL_ONGROUND)) {
			g::pCmd->buttons &= ~IN_JUMP;
		}
	}

	void DoAutostrafe( ) const
	{
		if ( !g_pEngine->IsConnected( ) || !g_pEngine->IsInGame( ) || !g_Menu.Config.AutoStrafe )
			return;

		if ( !g::pLocalEntity->IsAlive( ) )
			return;

		if ( g_Menu.Config.WASDMovement )
		{
			if ( GetAsyncKeyState( VK_W ) )
				return;
			if ( GetAsyncKeyState( VK_A ) )
				return;
			if ( GetAsyncKeyState( VK_S ) )
				return;
			if ( GetAsyncKeyState( VK_D ) )
				return;
		}

		if ( !( g::pLocalEntity->GetFlags( ) & FL_ONGROUND ) && GetAsyncKeyState( VK_SPACE ) )
		{
			pCmd->forwardmove = ( 10000.f / g::pLocalEntity->GetVelocity( ).Length2D( ) > 450.f ) ? 450.f : 10000.f / g::pLocalEntity->GetVelocity( ).Length2D( );
			pCmd->sidemove = ( pCmd->mousedx != 0 ) ? ( pCmd->mousedx < 0.0f ) ? -450.f : 450.f : ( pCmd->command_number % 2 ) == 0 ? -450.f : 450.f;
		}
	}

	void DoFakeLag( ) const
	{
		if ( !g_pEngine->IsConnected( ) || !g_pEngine->IsInGame( ) )
			return;

		//voice
		if ( g_pEngine->IsVoiceRecording( ) )
			return;

		if ( !g::pLocalEntity->IsAlive( ) )
			return;

		auto NetChannel = g_pEngine->GetNetChannel( );

		if ( !NetChannel )
			return;

		if (!g_Menu.Config.Fakelag && !(g::pCmd->buttons & IN_ATTACK)) {
			g::bSendPacket = (NetChannel->choked_packets >= 1);
		}
		else if ( ( g::pCmd->buttons & IN_ATTACK ) && !g_Menu.Config.FakelagWhileShooting ) {
			g::bSendPacket = true;
			return;
		}
		else {
				g::bSendPacket = (NetChannel->choked_packets >= g_Menu.Config.Fakelag);
		}
	}
};

extern Misc g_Misc;

/*
		if ( !g::pLocalEntity )
			return;

		if ( !g::pLocalEntity->IsAlive( ) )
			return;

		if ( g::pLocalEntity->IsScoped( ) && g_Menu.Config.NoScope )
		{
			int Height, Width;
			g_pEngine->GetScreenSize( Width, Height );

			Vector punchAngle = g::pLocalEntity->GetAimPunchAngle( );

			float x = Width / 2;
			float y = Height / 2;
			int dy = Height / 90;
			int dx = Width / 90;
			x -= ( dx*( punchAngle.y ) );
			y += ( dy*( punchAngle.x ) );

			Vector2D screenPunch = { x, y };

			g_pSurface->Line( 0, screenPunch.y, Width, screenPunch.y, Color( 0, 0, 0, 255 ) );
			g_pSurface->Line( screenPunch.x, 0, screenPunch.x, Height, Color( 0, 0, 0, 255 ) );
		}

		static bool init = false;
		static bool init2 = false;

		if ( g_Menu.Config.Crosshair )
		{
			if ( g::pLocalEntity->IsScoped( ) )
			{
				if ( init2 )
				{
					ConVar* sv_cheats = g_pCvar->FindVar( "sv_cheats" );
					*( int* )( ( DWORD )&sv_cheats->fnChangeCallback + 0xC ) = 0; // ew
					sv_cheats->SetValue( 1 );

					g_pEngine->ExecuteClientCmd( "weapon_debug_spread_show 0" );
					g_pEngine->ExecuteClientCmd( "crosshair 0" );
				}
				init2 = false;
			}
			else
			{
				if ( !init2 )
				{
					ConVar* sv_cheats = g_pCvar->FindVar( "sv_cheats" );
					*( int* )( ( DWORD )&sv_cheats->fnChangeCallback + 0xC ) = 0; // ew
					sv_cheats->SetValue( 1 );

					g_pEngine->ExecuteClientCmd( "weapon_debug_spread_show 3" );
					g_pEngine->ExecuteClientCmd( "crosshair 1" );
				}
				init2 = true;
			}

			init = false;
		}
		else
		{
			if ( !init )
			{
				ConVar* sv_cheats = g_pCvar->FindVar( "sv_cheats" );
				*( int* )( ( DWORD )&sv_cheats->fnChangeCallback + 0xC ) = 0; // ew
				sv_cheats->SetValue( 1 );

				g_pEngine->ExecuteClientCmd( "weapon_debug_spread_show 0" );
				g_pEngine->ExecuteClientCmd( "crosshair 1" );
			}
			init = true;
		}*/