#include "AntiAim.h"
#include "..\Aimbot\Autowall.h"
#include "..\..\Utils\Utils.h"
#include "..\..\SDK\IVEngineClient.h"
#include "..\..\SDK\PlayerInfo.h"
#include "..\..\Utils\Math.h"
#include "..\..\Menu\Menu.h"

AntiAim g_AntiAim;

bool Swtich = false;
bool local_update;

bool next_lby_update( CUserCmd* cmd )
{
	auto local_player = g_pEntityList->GetClientEntity( g_pEngine->GetLocalPlayer( ) );

	if ( !local_player )
		return false;

	static float next_lby_update_time = 0;
	float curtime = g_pGlobalVars->curtime;
	local_update = next_lby_update_time;

	auto animstate = local_player->AnimState( );

	if ( !animstate )
		return false;

	if ( !( local_player->GetFlags( ) & FL_ONGROUND ) )
		return false;

	if ( animstate->speed_2d > 0.1f )
		next_lby_update_time = curtime + 0.22f;

	if ( next_lby_update_time < curtime )
	{
		next_lby_update_time = curtime + 1.1f;
		return true;
	}

	return false;
}

void hide_your_head_nigger( ) // cancer v1
{
	static float FinalAngle;
	bool bside1 = false;
	bool bside2 = false;
	bool autowalld = false;
	for ( int i = 1; i <= g_pEngine->GetMaxClients( ); ++i )
	{
		C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity( i );

		if ( !pPlayerEntity
			|| !pPlayerEntity->IsAlive( )
			|| pPlayerEntity->IsDormant( )
			|| pPlayerEntity == g::pLocalEntity
			|| pPlayerEntity->GetTeam( ) == g::pLocalEntity->GetTeam( ) && !g_Menu.Config.FriendlyFire )
			continue;

		float angToLocal = g_Math.CalcAngle( g::pLocalEntity->GetOrigin( ), pPlayerEntity->GetOrigin( ) ).y;
		Vector ViewPoint = pPlayerEntity->GetOrigin( ) + Vector( 0, 0, 90 );

		Vector2D Side1 = { ( 45 * sin( g_Math.GRD_TO_BOG( angToLocal ) ) ),( 45 * cos( g_Math.GRD_TO_BOG( angToLocal ) ) ) };
		Vector2D Side2 = { ( 45 * sin( g_Math.GRD_TO_BOG( angToLocal + 180 ) ) ) ,( 45 * cos( g_Math.GRD_TO_BOG( angToLocal + 180 ) ) ) };

		Vector2D Side3 = { ( 50 * sin( g_Math.GRD_TO_BOG( angToLocal ) ) ),( 50 * cos( g_Math.GRD_TO_BOG( angToLocal ) ) ) };
		Vector2D Side4 = { ( 50 * sin( g_Math.GRD_TO_BOG( angToLocal + 180 ) ) ) ,( 50 * cos( g_Math.GRD_TO_BOG( angToLocal + 180 ) ) ) };

		Vector Origin = g::pLocalEntity->GetOrigin( );

		Vector2D OriginLeftRight[ ] = { Vector2D( Side1.x, Side1.y ), Vector2D( Side2.x, Side2.y ) };

		Vector2D OriginLeftRightLocal[ ] = { Vector2D( Side3.x, Side3.y ), Vector2D( Side4.x, Side4.y ) };

		for ( int side = 0; side < 2; side++ )
		{
			Vector OriginAutowall = { Origin.x + OriginLeftRight[ side ].x,  Origin.y - OriginLeftRight[ side ].y , Origin.z + 80 };
			Vector OriginAutowall2 = { ViewPoint.x + OriginLeftRightLocal[ side ].x,  ViewPoint.y - OriginLeftRightLocal[ side ].y , ViewPoint.z };

			if ( g_Autowall.CanHitFloatingPoint( OriginAutowall, ViewPoint ) )
			{
				if ( side == 0 )
				{
					bside1 = true;
					FinalAngle = angToLocal + 90;
				}
				else if ( side == 1 )
				{
					bside2 = true;
					FinalAngle = angToLocal - 90;
				}
				autowalld = true;
			}
			else
			{
				for ( int side222 = 0; side222 < 2; side222++ )
				{
					Vector OriginAutowall222 = { Origin.x + OriginLeftRight[ side222 ].x,  Origin.y - OriginLeftRight[ side222 ].y , Origin.z + 80 };

					if ( g_Autowall.CanHitFloatingPoint( OriginAutowall222, OriginAutowall2 ) )
					{
						if ( side222 == 0 )
						{
							bside1 = true;
							FinalAngle = angToLocal + 90;
						}
						else if ( side222 == 1 )
						{
							bside2 = true;
							FinalAngle = angToLocal - 90;
						}
						autowalld = true;
					}
				}
			}
		}
	}

	if ( !autowalld || ( bside1 && bside2 ) )
		g::pCmd->viewangles.y += 180;
	else
		g::pCmd->viewangles.y = FinalAngle;
}

static int direction = 0;

float get_yaw( ) {
	switch (g_Menu.Config.yaw_aa) { //"back", "freestanding", "key-based"
	case 0:
		return g::pCmd->viewangles.y;
		break;
	case 1:
		return g::pCmd->viewangles.y + 180;
		break;
	case 2:
		hide_your_head_nigger();
		break;
	case 3:
		switch (direction) {
		case 0:
			return g::pCmd->viewangles.y + 180;
			break; // back
		case 1:
			return g::pCmd->viewangles.y + 90.f;
			break; // i dont know
		case 2:
			return g::pCmd->viewangles.y - 90.f;
		}
		break;
	}
	return g::pCmd->viewangles.y;
}

void Real( )
{
	if ( GetAsyncKeyState( VK_DOWN ) )
		direction = 0;
	if ( GetAsyncKeyState( VK_LEFT ) )
		direction = 1;
	if ( GetAsyncKeyState( VK_RIGHT ) )
		direction = 2;

	if ( g::bSendPacket && g_Menu.Config.DesyncAngle ) {
		g::pCmd->viewangles.y = get_yaw( ) - 120;
	}
	else {
		g::pCmd->viewangles.y = get_yaw( );
	}

	if ( g_Menu.Config.DesyncAngle && next_lby_update( g::pCmd ) ) //we only want to break lby while faking our rotation
		g::pCmd->viewangles.y -= (g::pLocalEntity->GetMaxDesyncDelta() + 30.0f) * -1;
}

void AntiAim::OnCreateMove( )
{
	if ( !g_pEngine->IsInGame( ) || !g_Menu.Config.Antiaim || g_Menu.Config.LegitBacktrack )
		return;

	if ( !g::pLocalEntity->IsAlive( ) )
		return;

	if ( !g::pLocalEntity->GetActiveWeapon( ) || g::pLocalEntity->IsNade( ) )
		return;

	if ( g::pLocalEntity->GetMoveType( ) == MoveType_t::MOVETYPE_LADDER )
		return;

	float flServerTime = g::pLocalEntity->GetTickBase( ) * g_pGlobalVars->intervalPerTick;
	bool canShoot = ( g::pLocalEntity->GetActiveWeapon( )->GetNextPrimaryAttack( ) <= flServerTime );

	static bool jesus = false;

	if ( canShoot && ( g::pCmd->buttons & IN_ATTACK ) )
		return;

	if ( g::bSendPacket )
		Swtich = !Swtich;

	switch ( g_Menu.Config.pitch_aa ) {
	case 0:
	break;
	case 1:
	g::pCmd->viewangles.x = 89.9f;
	break;
	case 2:
	g::pCmd->viewangles.x = -89.9f;
	break;
	case 3:
	g::pCmd->viewangles.x = 0.01f; // so it's unresolvable
	break;
	case 4:
	g::pCmd->viewangles.x = -540.f;
	break;
	case 5:
	if ( jesus )
		g::pCmd->viewangles.x = 89.f;
	else
		g::pCmd->viewangles.x = -89.f;
	jesus = !jesus;
	break;
	}

	Real( );


	if ( g_Menu.Config.JitterRange && g_Menu.Config.RandJitterInRange)
	{
		float Offset = g_Menu.Config.JitterRange / 2.f;

		static bool jitter_flip = false;
		jitter_flip = !jitter_flip;

		g::pCmd->viewangles.y += jitter_flip ? Offset : -Offset;
	}
}