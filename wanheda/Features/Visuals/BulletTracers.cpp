#include "BulletTracers.h"
#include "../../SDK/IViewRenderBeams.h"

bullettracers g_pBulletTracers;

void bullettracers::draw_beam( Vector src, Vector end, Color color )
{
	BeamInfo_t info;
	info.m_nType = TE_BEAMPOINTS;
	info.m_pszModelName = "sprites/purplelaser1.vmt";
	//info.m_pszHaloName = -1;
	info.m_nHaloIndex = -1;
	info.m_flHaloScale = 0.f;
	info.m_flLife = 2;
	info.m_flWidth = 4;
	info.m_flEndWidth = 6;
	info.m_flFadeLength = 0;
	info.m_flAmplitude = 0.f;
	info.m_flBrightness = color.alpha;
	info.m_flSpeed = 1.f;
	info.m_nStartFrame = 0;
	info.m_flFrameRate = 0;
	info.m_flRed = color.red;
	info.m_flGreen = color.green;
	info.m_flBlue = color.blue;
	info.m_nSegments = -1;
	info.m_bRenderable = true;
	info.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE;
	info.m_vecStart = src; // Vector(0, 0, 5);
	info.m_vecEnd = end;

	Beam_t* beam = g_pBeam->CreateBeamPoints( info );

	if ( beam )
		g_pBeam->DrawBeam( beam );
}

void bullettracers::events( IGameEvent* event )
{
	if ( !strcmp( event->GetName( ), "bullet_impact" ) )
	{
		auto index = g_pEngine->GetPlayerForUserID( event->GetInt( "userid" ) );

		auto shooter = g_pEntityList->GetClientEntity( g_pEngine->GetPlayerForUserID( event->GetInt( "userid" ) ) );
		if ( !shooter )
			return;

		Vector position( event->GetFloat( "x" ), event->GetFloat( "y" ), event->GetFloat( "z" ) );

		if ( position == Vector( 0, 0, 0 ) )
			return;

		if ( index == g_pEngine->GetLocalPlayer( ) )
		{
			if ( g_Menu.Config.BulletTracers_Local )
			{
				logs.push_back( trace_info( g::pLocalEntity->GetEyePosition( ), position, g_pGlobalVars->realtime ) );
				draw_beam( g::pLocalEntity->GetEyePosition( ) - Vector( 0, 0, 1 ), position, g_Menu.Config.BulletTracers_Local_Color );
				g_pDebugOverlay->AddBoxOverlay( position, Vector( -2, -2, -2 ), Vector( 2, 2, 2 ), QAngle( 0, 0, 0 ), 0, 0, 255, 127, 4 );
			}
		}
		else
		{
			if ( g_Menu.Config.BulletTracers_Enemy && shooter->GetTeam( ) != g::pLocalEntity->GetTeam( ) || g_Menu.Config.BulletTracers_Team )
			{
				logs.push_back( trace_info( shooter->GetEyePosition( ), position, g_pGlobalVars->realtime ) );
				if ( shooter->GetTeam( ) != g::pLocalEntity->GetTeam( ) )
					draw_beam( shooter->GetEyePosition( ), position, g_Menu.Config.BulletTracers_Enemy_Color );
				else
					draw_beam( shooter->GetEyePosition( ), position, g_Menu.Config.BulletTracers_Team_Color );
			}
		}
	}
}