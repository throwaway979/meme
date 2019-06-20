#include "legitbot.h"
#include "..\..\Utils\GlobalVars.h"
#include "..\..\Menu\TGFCfg.h"
#include "..\..\Menu\Menu.h"
#include "..\..\Utils\Utils.h"
#include "..\..\SDK\IVEngineClient.h"
#include "..\..\SDK\PlayerInfo.h"
#include "..\..\SDK\ICvar.h"
#include "..\..\Utils\Math.h"
#include "..\..\SDK\Hitboxes.h"
#include "..\..\SDK\IEngineTrace.h"



Legitbot g_Legitbot;

mstudiobbox_t* GetHitbox( C_BaseEntity* entity, int hitbox_index )
{
	if ( entity->IsDormant( ) || entity->GetHealth( ) <= 0 )
		return NULL;

	const auto pModel = entity->GetModel( );
	if ( !pModel )
		return NULL;

	auto pStudioHdr = g_pModelInfo->GetStudiomodel( pModel );
	if ( !pStudioHdr )
		return NULL;

	auto pSet = pStudioHdr->GetHitboxSet( 0 );
	if ( !pSet )
		return NULL;

	if ( hitbox_index >= pSet->numhitboxes || hitbox_index < 0 )
		return NULL;

	return pSet->GetHitbox( hitbox_index );
}

Vector Legitbot::HitBoxPosition( C_BaseEntity * entity, int hitbox_id )
{
	auto pHitbox = GetHitbox( entity, hitbox_id );

	if ( !pHitbox )
		return Vector( 0, 0, 0 );

	auto pBoneMatrix = entity->GetBoneMatrix( pHitbox->bone );

	Vector bbmin, bbmax;
	g_Math.VectorTransform( pHitbox->min, pBoneMatrix, bbmin );
	g_Math.VectorTransform( pHitbox->max, pBoneMatrix, bbmax );

	return ( bbmin + bbmax ) * 0.5f;
}

template<typename T>
static T CubicInterpolate( T const& p1, T const& p2, T const& p3, T const& p4, float t )
{
	return p1 * ( 1 - t ) * ( 1 - t ) * ( 1 - t ) +
		p2 * 3 * t * ( 1 - t ) * ( 1 - t ) +
		p3 * 3 * t * t * ( 1 - t ) +
		p4 * t * t * t;
}

void Legitbot::OnCreateMove( )
{
	static C_BaseEntity* preTarget = nullptr;
	static C_BaseEntity* curTarget = nullptr;
	static float t = 0.f;

	float bestFOV = (float)g_Menu.Config.legitbotFov; // fov
	Vector viewAngles, engineAngles, angles;

	g_pEngine->GetViewAngles( engineAngles );
	Vector punchAngles = g::pLocalEntity->GetPunchAngles( );
	static float recoil = g_pCvar->FindVar( "weapon_recoil_scale" )->GetFloat( );

	if ( g::pLocalEntity->IsKnifeorNade( ) )
		return;

	if (!g_Menu.Config.legitbotEnable)
		return;

	if (g_Menu.Config.legitbotOnKey && !GetAsyncKeyState(g_Menu.Config.legitbot__Key))
		return;

	{
		if ( g_Menu.Config.legitRecoilControlSystem ) {
			if (g::pCmd->buttons & IN_ATTACK) {
				Vector final_rcsd_shit = punchAngles * recoil;
				final_rcsd_shit.x *= (g_Menu.Config.LegitbotRCSX / 100.f);
				final_rcsd_shit.y *= (g_Menu.Config.LegitbotRCSY / 100.f);

				angles -= final_rcsd_shit;
			}
		}

		static bool was_firing = false;

		if ( g_Menu.Config.AutoPistol ) {

			if ( g::pLocalEntity->IsPistol( ) ) {
				if ( g::pCmd->buttons & IN_ATTACK && !g::pLocalEntity->IsKnifeorNade( ) ) {
					if ( was_firing ) {
						g::pCmd->buttons &= ~IN_ATTACK;
					}
				}

				was_firing = g::pCmd->buttons & IN_ATTACK ? true : false;
			}

		}
		for ( int it = 1; it <= g_pEngine->GetMaxClients( ); ++it )
		{
			C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity( it );

			if ( !pPlayerEntity
				|| !pPlayerEntity->IsAlive( )
				|| pPlayerEntity->IsDormant( )
				|| pPlayerEntity == g::pLocalEntity
				|| pPlayerEntity->GetTeam( ) == g::pLocalEntity->GetTeam( ) && !g_Menu.Config.FriendlyFireLegit)
				continue;

			if ( g_Menu.Config.legitHitbox == 0 )
				angles = g_Math.ClampAngle( g_Math.NormalizeAngle( g_Math.CalcAngle( g::pLocalEntity->GetEyePosition( ), HitBoxPosition( pPlayerEntity, HITBOX_HEAD ) ) ) );
			else if ( g_Menu.Config.legitHitbox == 1 )
				angles = g_Math.ClampAngle( g_Math.NormalizeAngle( g_Math.CalcAngle( g::pLocalEntity->GetEyePosition( ), HitBoxPosition( pPlayerEntity, HITBOX_NECK ) ) ) );
			else if ( g_Menu.Config.legitHitbox == 2 )
				angles = g_Math.ClampAngle( g_Math.NormalizeAngle( g_Math.CalcAngle( g::pLocalEntity->GetEyePosition( ), HitBoxPosition( pPlayerEntity, HITBOX_UPPER_CHEST ) ) ) );
			else if ( g_Menu.Config.legitHitbox == 3 )
				angles = g_Math.ClampAngle( g_Math.NormalizeAngle( g_Math.CalcAngle( g::pLocalEntity->GetEyePosition( ), HitBoxPosition( pPlayerEntity, HITBOX_LEFT_HAND ) ) ) );
			else if ( g_Menu.Config.legitHitbox == 4 )
				angles = g_Math.ClampAngle( g_Math.NormalizeAngle( g_Math.CalcAngle( g::pLocalEntity->GetEyePosition( ), HitBoxPosition( pPlayerEntity, HITBOX_RIGHT_HAND ) ) ) );
			else if ( g_Menu.Config.legitHitbox == 5 )
				angles = g_Math.ClampAngle( g_Math.NormalizeAngle( g_Math.CalcAngle( g::pLocalEntity->GetEyePosition( ), HitBoxPosition( pPlayerEntity, HITBOX_PELVIS ) ) ) );

			float fov = ( engineAngles - angles ).Length2D( );

			if ( fov < bestFOV ) {
				bestFOV = fov;
				viewAngles = angles;
				curTarget = pPlayerEntity;
			}
		}

		if ( preTarget != curTarget || preTarget == nullptr ) {
			t = 0.0f;
			preTarget = curTarget;
		}
		else if ( preTarget == curTarget ) {
			t += 0.03f;
		}

		if ( bestFOV != (float)g_Menu.Config.legitbotFov )
		{
			if ( t < 1.f && bestFOV > 1.f ) //not really necessary to add a curve if we are this close
			{
				Vector src = engineAngles;
				Vector dst = viewAngles;

				Vector delta = src - dst;
				g_Math.ClampAngle( delta );

				float randValPt1 = 15.0f + g_Math.RandomFloat( 0.0f, 15.0f );
				float finalRandValPt1 = 3.f / randValPt1;
				Vector point1 = src + ( delta * finalRandValPt1 );
				g_Math.ClampAngle( point1 );

				float randValPt2 = 40.0f + g_Math.RandomFloat( 0.0f, 15.0f );
				float finalRandValPt2 = 1.0f / randValPt2;
				Vector point2 = dst * ( 1.0f + finalRandValPt2 );
				g_Math.ClampAngle( point2 );

				Vector angle = CubicInterpolate( src, point1, point2, dst, t );
				g_Math.ClampAngle( angle );

				g::pCmd->viewangles = angle;
				g_pEngine->SetViewAngles( angle );
			}
			else
			{
				Vector smoothAngle = g_Math.NormalizeAngle( viewAngles - engineAngles );
				viewAngles = engineAngles + (g_Menu.Config.legitbotSmooth ? smoothAngle / (float)g_Menu.Config.legitbotSmooth * 2.f /* since smoothness barely does something*/ : smoothAngle); //don't divide by 0 el retardo
				g_Math.ClampAngle( viewAngles );
				g::pCmd->viewangles = viewAngles;

				if(!g_Menu.Config.LegitSilentAim)
					g_pEngine->SetViewAngles( viewAngles );
			}
		}
	}
	/*else
	{
		t = 0.f;
	}*/

	g::pCurrentFOV = t;
}

Vector angle_vector(Vector to_convert) { //what the fuck was going on thru my head? these all should be math classes. sorry for pulling a psilent/uber/dumb paster, fellow gamer.
	auto y_sin = sin(to_convert.y / 180.f * static_cast<float>(M_PI));
	auto y_cos = cos(to_convert.y / 180.f * static_cast<float>(M_PI));

	auto x_sin = sin(to_convert.x / 180.f * static_cast<float>(M_PI));
	auto x_cos = cos(to_convert.x / 180.f * static_cast<float>(M_PI));

	return Vector(x_cos * y_cos, x_cos * y_sin, -x_sin);
}

float distance_point_to_line(Vector point, Vector origin, Vector direction) {
	auto delta = point - origin;

	auto temp = delta.Dot(direction) / (direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
	if (temp < 0.000001f)
		return FLT_MAX;

	auto temp_meme = origin + (direction * temp);
	return (point - temp_meme).Length();
}

struct backtrack_tick {
	float simulation_time;
	Vector hitbox_position;
	Vector origin;
	Vector angles;
	matrix3x4_t bt_matrix[128];
};

matrix3x4_t bone_matrix[128];
backtrack_tick backtrack[64][14];

void Legitbot::CM_Backtrack() {
	int best_target = -1;
	float best_fov = 90.f;

	if (!g_Menu.Config.LegitBacktrack || !g_Menu.Config.legitbotEnable)
		return;

	if (!g::pLocalEntity)
		return;

	if (g::pLocalEntity->GetHealth() <= 0)
		return;

	for (int i = 0; i <= 65; i++) {
		C_BaseEntity* ent = g_pEntityList->GetClientEntity(i);

		if (!ent) continue;
		if (ent->GetHealth() < 0) continue;
		if (ent->IsDormant()) continue;
		if (ent->GetTeam() == g::pLocalEntity->GetTeam()) continue;

		if (ent->GetHealth() > 0) { //useless check
			float sim_time = ent->GetSimulationTime();
			ent->FixSetupBones(bone_matrix);

			backtrack[i][g::pCmd->command_number % 14] = backtrack_tick
			{
				sim_time,
				ent->bone_pos(8),
				ent->GetAbsOrigin(),
				ent->GetAbsAngles(),
				bone_matrix[128]
			};

			Vector view_direction = angle_vector(g::pCmd->viewangles);
			float fov = distance_point_to_line(ent->bone_pos(8), g::pLocalEntity->bone_pos(8), view_direction);

			if (best_fov > fov) {
				best_fov = fov;
				best_target = i;
			}
		}
	}
	float best_target_simulation_time = 0.f;

	if (best_target != -1) {
		float temp = FLT_MAX;
		Vector view_direction = angle_vector(g::pCmd->viewangles);

		for (int t = 0; t < 14; ++t) {
			float fov = distance_point_to_line(backtrack[best_target][t].hitbox_position, g::pLocalEntity->bone_pos(8), view_direction);
			if (temp > fov && backtrack[best_target][t].simulation_time > g::pLocalEntity->GetSimulationTime() - 1) {
				temp = fov;
				best_target_simulation_time = backtrack[best_target][t].simulation_time;
			}
		}

		if (g::pCmd->buttons & IN_ATTACK) {
			g::pCmd->tick_count = (int)(0.5f + (float)(best_target_simulation_time) / g_pGlobalVars->intervalPerTick);
		}
	}
}