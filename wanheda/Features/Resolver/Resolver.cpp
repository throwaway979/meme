#include "Resolver.h"
#include "..\Aimbot\Aimbot.h"
#include "..\Aimbot\Autowall.h"
#include "..\Aimbot\LagComp.h"
#include "..\..\Utils\Utils.h"
#include "..\..\SDK\IVEngineClient.h"
#include "..\..\SDK\Hitboxes.h"
#include "..\..\SDK\PlayerInfo.h"
#include "..\..\Utils\Math.h"
#include "..\..\Menu\Menu.h"
#include "..\Logs\Logs.h"

Resolver g_Resolver;

void StaticDesyncResolve(C_BaseEntity* entity)
{
	int angle1[64];
	int angle2[64];
	int angle3[64];
	static float old_simtime[65];
	bool using_fake_angles[65];

	if (old_simtime[entity->EntIndex()] != entity->GetSimulationTime())
		angle1[entity->EntIndex()] = entity->GetEyeAngles().y;
	else
		angle2[entity->EntIndex()] = entity->GetEyeAngles().y;

	if (angle1[entity->EntIndex()] != angle2[entity->EntIndex()])
		using_fake_angles[entity->EntIndex()] = true;
	else
		using_fake_angles[entity->EntIndex()] = false;

	if (using_fake_angles[entity->EntIndex()])
	{
		if (g_Math.YawDistance(angle1[entity->EntIndex()], angle2[entity->EntIndex()]) <= 58)
		{
			entity->GetEyeAnglesPointer()->y = entity->GetLowerBodyYaw() - g_Math.YawDistance(angle1[entity->EntIndex()], angle2[entity->EntIndex()]);
		}
	}

	old_simtime[entity->EntIndex()] = entity->GetSimulationTime();
}

void Guess(C_BaseEntity* ent)
{
	// 0,1,2 not 1,2,3. someone doesn't know how '%' works, eh? -swoopae
	switch (g::MissedShots[ent->EntIndex()] % 3)
	{
	case 0:
		StaticDesyncResolve(ent); break;
	case 1:
		ent->GetEyeAnglesPointer()->y += ent->GetMaxDesyncDelta(); break;
	case 2:
		ent->GetEyeAnglesPointer()->y -= ent->GetMaxDesyncDelta(); break;
	}
}

void Resolver::Resolve( C_BaseEntity* pEnt )
{
	auto fix_goal_feet_yaw = [ ]( float rotation, float invertedrotation, float yawfeetdelta, float yaw, C_AnimState * state ) {
		if ( yawfeetdelta < rotation ) {
			if ( invertedrotation > yawfeetdelta )
				* ( float* )( uintptr_t( state ) + 0x80 ) = invertedrotation + yaw;
		}
		else
			*( float* )( uintptr_t( state ) + 0x80 ) = yaw - rotation;
	};

	auto get_rotation = [ & ]( int rotation_type, C_AnimState * state ) {
		float v43 = *( float* )( ( uintptr_t )state + 0xA4 );
		float v54 = max( 0, min( *reinterpret_cast< float* >( ( uintptr_t )state + 0xF8 ), 1 ) );
		float v55 = max( 0, min( 1, *reinterpret_cast< float* >( ( uintptr_t )state + 0xFC ) ) );

		float v56;
		v56 = ( ( *reinterpret_cast< float* >( ( uintptr_t )state + 0x11C ) * -0.30000001 ) - 0.19999999 ) * v54;
		if ( v43 > 0 )
			v56 += ( ( v43 * v55 ) * ( 0.5 - v56 ) );

		v56 = *( float* )( ( uintptr_t )state + rotation_type ) * v56;
		return v56;
	};

	auto AnimState = pEnt->AnimState( );

	if ( !AnimState )
		return;

	float inverted = get_rotation( 0x2B4, AnimState );
	float max = get_rotation( 0x2B0, AnimState );
	float yawfeetdelta = AnimState->m_flEyeYaw - AnimState->m_flGoalFeetYaw;
	float yaw = pEnt->GetEyeAngles( ).y;

	if ( pEnt != g::pLocalEntity && (pEnt->GetTeam( ) != g::pLocalEntity->GetTeam( ) && !g_Menu.Config.FriendlyFire) && g_Menu.Config.Resolver /*&& !( GetAsyncKeyState( g_Menu.Config.override_resolver_key ) && g_Menu.Config.ResolverOverride && g::pLocalEntity && g::pLocalEntity->IsAlive())*/ || pEnt != g::pLocalEntity && g_Menu.Config.FriendlyFire && g_Menu.Config.Resolver /*&& !( GetAsyncKeyState( g_Menu.Config.override_resolver_key ) && g_Menu.Config.ResolverOverride && g::pLocalEntity && g::pLocalEntity->IsAlive())*/) {
		if (!g::MissedShots[pEnt->EntIndex()]) {
			fix_goal_feet_yaw(max, inverted, yawfeetdelta, yaw, AnimState);
			pEnt->AnimState()->m_flGoalFeetYaw = AnimState->m_flGoalFeetYaw;
		}
		// xy0's ""reversed"" resolver which is pretty much our static desync but ok
		else if (g::MissedShots[pEnt->EntIndex()] < 2) {
			float m_flLastClientSideAnimationUpdateTimeDelta = fabs(AnimState->m_iLastClientSideAnimationUpdateFramecount - AnimState->m_flLastClientSideAnimationUpdateTime);

			auto v48 = 0.f;

			if (AnimState->m_flFeetSpeedForwardsOrSideWays >= 0.0f)
			{
				v48 = fminf(AnimState->m_flFeetSpeedForwardsOrSideWays, 1.0f);
			}
			else
			{
				v48 = 0.0f;
			}

			float v49 = ((AnimState->m_flStopToFullRunningFraction * -0.30000001) - 0.19999999) * v48;

			float flYawModifier = v49 + 1.0;

			if (AnimState->m_fDuckAmount > 0.0)
			{
				float v53 = 0.0f;

				if (AnimState->m_flFeetSpeedUnknownForwardOrSideways >= 0.0)
				{
					v53 = fminf(AnimState->m_flFeetSpeedUnknownForwardOrSideways, 1.0);
				}
				else
				{
					v53 = 0.0f;
				}
			}

			float flMaxYawModifier = AnimState->pad10[516] * flYawModifier;
			float flMinYawModifier = AnimState->pad10[512] * flYawModifier;

			float newFeetYaw = 0.f;

			auto eyeYaw = AnimState->m_flEyeYaw;

			auto lbyYaw = AnimState->m_flGoalFeetYaw;

			float eye_feet_delta = fabs(eyeYaw - lbyYaw);

			if (eye_feet_delta <= flMaxYawModifier)
			{
				if (flMinYawModifier > eye_feet_delta)
				{
					newFeetYaw = fabs(flMinYawModifier) + eyeYaw;
				}
			}
			else
			{
				newFeetYaw = eyeYaw - fabs(flMaxYawModifier);
			}

			float v136 = fmod(newFeetYaw, 360.0);

			if (v136 > 180.0)
			{
				v136 = v136 - 360.0;
			}

			if (v136 < 180.0)
			{
				v136 = v136 + 360.0;
			}

			AnimState->m_flGoalFeetYaw = v136;
		}
		else {
			Guess(pEnt);
		}

		g::MissedShots[pEnt->EntIndex()] = g::MissedShots[pEnt->EntIndex()] % 6; // so it just doesn't brute all the fucking time, that's dumb.
	}
}

void Resolver::OnCreateMove() // cancer v2
{
	if (!g_Menu.Config.Resolver)
		return;

	if (!g_pEngine->IsInGame() && !g_pEngine->IsConnected())
		return;

	if (!g::pLocalEntity->IsAlive())
		return;

	if (!g::pLocalEntity->GetActiveWeapon())
		return; // why wouldnt u resolve people when u have a knife out retard

	for (int i = 1; i < g_pEngine->GetMaxClients(); ++i)
	{
		C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity(i);

		if (!pPlayerEntity
			|| !pPlayerEntity->IsAlive()
			|| pPlayerEntity->IsDormant()
			|| pPlayerEntity == g::pLocalEntity
			|| pPlayerEntity->GetTeam() == g::pLocalEntity->GetTeam())
		{
			UseFreestandAngle[i] = false;
			continue;
		}

		if (abs(pPlayerEntity->GetVelocity().Length2D()) > 29.f)
			UseFreestandAngle[pPlayerEntity->EntIndex()] = false;

		if (abs(pPlayerEntity->GetVelocity().Length2D()) <= 29.f && !UseFreestandAngle[pPlayerEntity->EntIndex()])
		{
			bool Autowalled = false, HitSide1 = false, HitSide2 = false;

			float angToLocal = g_Math.CalcAngle(g::pLocalEntity->GetOrigin(), pPlayerEntity->GetOrigin()).y;
			Vector ViewPoint = g::pLocalEntity->GetOrigin() + Vector(0, 0, 90);

			Vector2D Side1 = { (45 * sin(g_Math.GRD_TO_BOG(angToLocal))),(45 * cos(g_Math.GRD_TO_BOG(angToLocal))) };
			Vector2D Side2 = { (45 * sin(g_Math.GRD_TO_BOG(angToLocal + 180))) ,(45 * cos(g_Math.GRD_TO_BOG(angToLocal + 180))) };

			Vector2D Side3 = { (50 * sin(g_Math.GRD_TO_BOG(angToLocal))),(50 * cos(g_Math.GRD_TO_BOG(angToLocal))) };
			Vector2D Side4 = { (50 * sin(g_Math.GRD_TO_BOG(angToLocal + 180))) ,(50 * cos(g_Math.GRD_TO_BOG(angToLocal + 180))) };

			Vector Origin = pPlayerEntity->GetOrigin();

			Vector2D OriginLeftRight[] = { Vector2D(Side1.x, Side1.y), Vector2D(Side2.x, Side2.y) };

			Vector2D OriginLeftRightLocal[] = { Vector2D(Side3.x, Side3.y), Vector2D(Side4.x, Side4.y) };

			for (int side = 0; side < 2; side++)
			{
				Vector OriginAutowall = { Origin.x + OriginLeftRight[side].x,  Origin.y - OriginLeftRight[side].y , Origin.z + 90 };
				Vector OriginAutowall2 = { ViewPoint.x + OriginLeftRightLocal[side].x,  ViewPoint.y - OriginLeftRightLocal[side].y , ViewPoint.z };

				if (g_Autowall.CanHitFloatingPoint(OriginAutowall, ViewPoint))
				{
					if (side == 0)
					{
						HitSide1 = true;
						FreestandAngle[pPlayerEntity->EntIndex()] = 90;
					}
					else if (side == 1)
					{
						HitSide2 = true;
						FreestandAngle[pPlayerEntity->EntIndex()] = -90;
					}

					Autowalled = true;
				}
				else
				{
					for (int side222 = 0; side222 < 2; side222++)
					{
						Vector OriginAutowall222 = { Origin.x + OriginLeftRight[side222].x,  Origin.y - OriginLeftRight[side222].y , Origin.z + 90 };

						if (g_Autowall.CanHitFloatingPoint(OriginAutowall222, OriginAutowall2))
						{
							if (side222 == 0)
							{
								HitSide1 = true;
								FreestandAngle[pPlayerEntity->EntIndex()] = 90;
							}
							else if (side222 == 1)
							{
								HitSide2 = true;
								FreestandAngle[pPlayerEntity->EntIndex()] = -90;
							}

							Autowalled = true;
						}
					}
				}
			}

			if (Autowalled)
			{
				if (HitSide1 && HitSide2)
					UseFreestandAngle[pPlayerEntity->EntIndex()] = false;
				else
					UseFreestandAngle[pPlayerEntity->EntIndex()] = true;
			}
		}

		// bad idea but ill try this why not
		if (UseFreestandAngle[pPlayerEntity->EntIndex()] && g::MissedShots[pPlayerEntity->EntIndex()] < 4) {
			pPlayerEntity->SetEyeAngles(Vector(pPlayerEntity->GetEyeAngles().x, pPlayerEntity->GetEyeAngles().y + FreestandAngle[pPlayerEntity->EntIndex()], pPlayerEntity->GetEyeAngles().z));
		}
	}
}

void HandleHits( C_BaseEntity * pEnt )
{
	auto NetChannel = g_pEngine->GetNetChannelInfo( );

	if ( !NetChannel )
		return;

	static float predTime[ 65 ];
	static bool init[ 65 ];

	if ( g::Shot[ pEnt->EntIndex( ) ] )
	{
		if ( init[ pEnt->EntIndex( ) ] )
		{
			predTime[ pEnt->EntIndex( ) ] = g_pGlobalVars->curtime + NetChannel->GetAvgLatency( FLOW_INCOMING ) + NetChannel->GetAvgLatency( FLOW_OUTGOING ) + TICKS_TO_TIME( 1 ) + TICKS_TO_TIME( g_pEngine->GetNetChannel( )->choked_packets );
			init[ pEnt->EntIndex( ) ] = false;
		}

		if ( g_pGlobalVars->curtime > predTime[ pEnt->EntIndex( ) ] && !g::Hit[ pEnt->EntIndex( ) ] )
		{
			g::MissedShots[ pEnt->EntIndex( ) ] += 1; 
			g::Shot[ pEnt->EntIndex( ) ] = false;
		}
		else if ( g_pGlobalVars->curtime <= predTime[ pEnt->EntIndex( ) ] && g::Hit[ pEnt->EntIndex( ) ] ) {
			g::Shot[ pEnt->EntIndex( ) ] = false; // missed shot due to spread (?) - no
		}
	}
	else
		init[ pEnt->EntIndex( ) ] = true;

	g::Hit[ pEnt->EntIndex( ) ] = false;
}

inline float ClampYaw( float yaw )
{
	while ( yaw > 180.f )
		yaw -= 360.f;
	while ( yaw < -180.f )
		yaw += 360.f;
	return yaw;
}

void Override( C_BaseEntity * pEnt )
{
	if ( !g_Menu.Config.Resolver || !g_Menu.Config.ResolverOverride )
		return;

	if ( !GetAsyncKeyState( g_Menu.Config.override_resolver_key ) )
		return;

	if (!g::pLocalEntity || !g::pLocalEntity->IsAlive())
		return;

	Vector viewangles;
	g_pEngine->GetViewAngles( viewangles );

	auto at_target_yaw = g_Math.CalcAngle( pEnt->GetOrigin( ),
		g::pLocalEntity->GetOrigin( ) ).y;
	auto delta = ClampYaw( viewangles.y - at_target_yaw );

	if ( delta > 0 )
		pEnt->SetEyeAngles( Vector( pEnt->GetEyeAngles( ).x, at_target_yaw + 90, 0 ) );
	else
		pEnt->SetEyeAngles( Vector( pEnt->GetEyeAngles( ).x, at_target_yaw - 90, 0 ) );
}

void Resolver::FrameStage( ClientFrameStage_t stage )
{
	if ( !g::pLocalEntity || !g_pEngine->IsInGame( ) || !g_pEngine->IsConnected( ) )
		return;

	if ( !g_Menu.Config.Resolver )
		return;

	for ( int i = 1; i < g_pEngine->GetMaxClients( ); ++i )
	{
		C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity( i );

		if ( !pPlayerEntity
			|| !pPlayerEntity->IsAlive( ) )
			continue;

		if ( pPlayerEntity->IsDormant( ) ) {
			continue;
		}

		if ( stage == FRAME_RENDER_START ) {
			HandleHits( pPlayerEntity );
			Resolve( pPlayerEntity );
		}
		/*else if ( stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START ) {
			Override( pPlayerEntity );
		}*/
	}
}