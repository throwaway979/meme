#include "Autowall.h"
#include "..\..\Utils\Utils.h"
#include "..\..\SDK\IVEngineClient.h"
#include "..\..\SDK\PlayerInfo.h"
#include "..\..\SDK\ISurfaceData.h"
#include "..\..\SDK\Hitboxes.h"
#include "..\..\SDK\bspflags.h"
#include "..\..\SDK\ICvar.h"
#include "..\..\Utils\Math.h"
#include "..\..\SDK\ClientClass.h"
#include <algorithm>

// esoterik ftw

Autowall g_Autowall;

void TraceLine( Vector& vecAbsStart, Vector& vecAbsEnd, unsigned int mask, C_BaseEntity* ignore, C_Trace* ptr )
{
	C_TraceFilter filter( ignore );
	g_pTrace->TraceRay( C_Ray( vecAbsStart, vecAbsEnd ), mask, &filter, ptr );
}

bool VectortoVectorVisible( Vector src, Vector point )
{
	C_Trace Trace;
	TraceLine( src, point, mask_solid, g::pLocalEntity, &Trace );

	if ( Trace.flFraction == 1.0f )
	{
		return true;
	}

	return false;

	//legacy

	/*
	C_Trace TraceInit;
	TraceLine(src, point, mask_solid, g::pLocalEntity, &TraceInit);
	C_Trace Trace;
	TraceLine(src, point, mask_solid, TraceInit.m_pEnt, &Trace);

	if (Trace.flFraction == 1.0f || TraceInit.flFraction == 1.0f)
		return true;

	return false; */
}

float GetHitgroupDamageMult( int iHitGroup )
{
	switch ( iHitGroup )
	{
	case HITGROUP_HEAD:
	return 4.f;
	case HITGROUP_STOMACH:
	return 1.25f;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
	return 0.75f;
	}

	return 1.0f;
}

bool HandleBulletPenetration( WeaponInfo_t* wpn_data, FireBulletData& data, bool extracheck, Vector point );

void ScaleDamage( int hitgroup, C_BaseEntity* enemy, float weapon_armor_ratio, float& current_damage )
{
	if (!enemy)
		return;

	current_damage *= GetHitgroupDamageMult( hitgroup );

	if ( enemy->ArmorValue( ) > 0.0f && hitgroup < HITGROUP_LEFTLEG )
	{
		if ( hitgroup == HITGROUP_HEAD && !enemy->HasHelmet( ) )
			return;

		float armorscaled = ( weapon_armor_ratio * 0.5f ) * current_damage;
		if ( ( current_damage - armorscaled ) * 0.5f > enemy->ArmorValue( ) )
			armorscaled = current_damage - ( enemy->ArmorValue( ) * 2.0f );
		current_damage = armorscaled;
	}
}

void UTIL_ClipTraceToPlayers( const Vector & vecAbsStart, const Vector & vecAbsEnd, unsigned int mask, ITraceFilter * filter, C_Trace * tr )
{
	static DWORD dwAddress = Utils::FindPattern( "client_panorama.dll", ( BYTE* )"\x53\x8B\xDC\x83\xEC\x08\x83\xE4\xF0\x83\xC4\x04\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x81\xEC\x00\x00\x00\x00\x8B\x43\x10", "xxxxxxxxxxxxxxxxxxxxxxxx????xxx" );

	if ( !dwAddress )
		return;

	_asm
	{
		MOV		EAX, filter
		LEA		ECX, tr
		PUSH	ECX
		PUSH	EAX
		PUSH	mask
		LEA		EDX, vecAbsEnd
		LEA		ECX, vecAbsStart
		CALL	dwAddress
		ADD		ESP, 0xC
	}
}

bool Autowall::SimulateFireBullet( C_BaseEntity * local, C_BaseCombatWeapon * weapon, FireBulletData & data )
{
	data.penetrate_count = 4;
	data.trace_length = 0.0f;
	auto* wpn_data = weapon->GetCSWpnData( );
	if ( !wpn_data ) return false;
	if ( wpn_data )
	{
		data.current_damage = ( float )wpn_data->damage;
		while ( ( data.penetrate_count > 0 ) && ( data.current_damage >= 1.0f ) )
		{
			data.trace_length_remaining = wpn_data->range - data.trace_length;
			Vector End_Point = data.src + data.direction * data.trace_length_remaining;
			TraceLine( data.src, End_Point, 0x4600400B, local, &data.enter_trace );
			UTIL_ClipTraceToPlayers( data.src, End_Point * 40.f, 0x4600400B, &data.filter, &data.enter_trace );
			if ( data.enter_trace.flFraction == 1.0f )
				break;
			if ( ( data.enter_trace.hitGroup <= 7 ) && ( data.enter_trace.hitGroup > 0 ) && ( local->GetTeam( ) != data.enter_trace.m_pEnt->GetTeam( ) && !g_Menu.Config.FriendlyFire || g_Menu.Config.FriendlyFire) )
			{
				data.trace_length += data.enter_trace.flFraction * data.trace_length_remaining;
				data.current_damage *= pow( wpn_data->range_modifier, data.trace_length * 0.002 );
				ScaleDamage( data.enter_trace.hitGroup, data.enter_trace.m_pEnt, wpn_data->armor_ratio, data.current_damage );
				return true;
			}
			if ( !HandleBulletPenetration( wpn_data, data, false, Vector( 0, 0, 0 ) ) )
				break;
		}
		return false;
	}
}

bool TraceToExitalt( Vector & end, C_Trace & tr, Vector start, Vector vEnd, C_Trace * trace )
{
	typedef bool( __fastcall * TraceToExitFn )( Vector&, C_Trace&, float, float, float, float, float, float, C_Trace* );
	static TraceToExitFn oTraceToExit = ( TraceToExitFn )Utils::FindSignature( "client_panorama.dll", "55 8B EC 83 EC 30 F3 0F 10 75" );

	if ( !oTraceToExit )
		return false;

	return oTraceToExit( end, tr, start.x, start.y, start.z, vEnd.x, vEnd.y, vEnd.z, trace );
}

bool Autowall::IsBreakableEntity( C_BaseEntity * entity )
{
	if ( !entity || !entity->EntIndex( ) )
		return false;

	// backup original take_damage value.
	const int take_damage = entity->getTakeDamage( );

	const ClientClass * clientClass = entity->GetClientClass( );

	if (!clientClass)
		return false;

	if ( clientClass->pNetworkName[ 1 ] == 'B' && clientClass->pNetworkName[ 9 ] == 'e' && clientClass->pNetworkName[ 10 ] == 'S' && clientClass->pNetworkName[ 16 ] == 'e' )
		entity->getTakeDamage( ) = 2;

	else if ( clientClass->pNetworkName[ 1 ] != 'B' && clientClass->pNetworkName[ 5 ] != 'D' )
		entity->getTakeDamage( ) = 2;

	else if ( clientClass->pNetworkName[ 1 ] != 'F' && clientClass->pNetworkName[ 4 ] != 'c' && clientClass->pNetworkName[ 5 ] != 'B' && clientClass->pNetworkName[ 9 ] != 'h' ) // CFuncBrush
		entity->getTakeDamage( ) = 2;

	using IsBreakableEntity_t = bool( __thiscall* )( C_BaseEntity* );
	static IsBreakableEntity_t IsBreakableEntityEx = nullptr;

	if ( !IsBreakableEntityEx )
		IsBreakableEntityEx = reinterpret_cast< IsBreakableEntity_t >( Utils::FindSignature( "client_panorama.dll", "55 8B EC 51 56 8B F1 85 F6 74 68" ) ); //MIGHT BE INCORRECT

	entity->getTakeDamage( ) = take_damage;

	return IsBreakableEntityEx( entity );
}

bool Autowall::TraceToExit2( Vector & vecEnd, C_Trace * pEnterTrace, Vector vecStart, Vector vecDir, C_Trace * pExitTrace )
{
	float dist = 0.f;

	while ( dist <= 90.f ) {
		dist += 4.f;
		vecEnd = vecStart + vecDir * dist;

		const int point_contents = g_pTrace->GetPointContents( vecEnd, mask_shot_hull | contents_hitbox, nullptr );
		if ( point_contents & mask_shot_hull && !( point_contents & contents_hitbox ) )
			continue;

		Vector a1 = vecEnd - vecDir * 4.f;
		C_Ray ray( vecEnd, a1 );
		ray.Init( vecEnd, a1 );

		g_pTrace->TraceRay( ray, mask_shot_hull | contents_hitbox, nullptr, pExitTrace );

		if ( pExitTrace->start != Vector( 0, 0, 0 ) && pExitTrace->surface.flags & surf_hitbox ) {
			CTraceFilterOneEntity filter( pExitTrace->m_pEnt );
			C_Ray ray2( vecEnd, vecStart );
			ray2.Init( vecEnd, vecStart );
			g_pTrace->TraceRay( ray2, mask_shot_hull, &filter, pExitTrace );

			if ( ( pExitTrace->flFraction < 1.f || pExitTrace->allsolid ) && !pExitTrace->startSolid ) {
				vecEnd = pExitTrace->end;
				return true;
			}

			continue;
		}

		if ( !pExitTrace->DidHit( ) || pExitTrace->startSolid ) {
			if ( pEnterTrace->m_pEnt && ( pEnterTrace->m_pEnt != nullptr && pEnterTrace->m_pEnt != g_pEntityList->GetClientEntity( 0 ) ) ) {
				*pExitTrace = *pEnterTrace;
				pExitTrace->end = vecStart + vecDir;
				return true;
			}

			continue;
		}

		if ( !pExitTrace->DidHit( ) || pExitTrace->startSolid ) {
			if ( pEnterTrace->m_pEnt != nullptr && !pEnterTrace->m_pEnt->EntIndex( ) == 0 && IsBreakableEntity( static_cast< C_BaseEntity* >( pEnterTrace->m_pEnt ) ) ) {
				*pExitTrace = *pEnterTrace;
				pExitTrace->end = vecStart + vecDir;
				return true;
			}

			continue;
		}

		if ( pExitTrace->surface.flags >> 7 & surf_light && !( pEnterTrace->surface.flags >> 7 & surf_light ) )
			continue;

		if ( pExitTrace->plane.normal.Dot( vecDir ) <= 1.f ) {
			vecEnd = vecEnd - vecDir * ( pExitTrace->flFraction * 4.f );
			return true;
		}
	}

	return false;
}

bool HandleBulletPenetration( WeaponInfo_t * wpn_data, FireBulletData & data, bool extracheck, Vector point )
{
	surfacedata_t* enter_surface_data = g_pSurfaceData->GetSurfaceData( data.enter_trace.surface.surfaceProps );
	int enter_material = enter_surface_data->game.material;
	float enter_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;
	data.trace_length += data.enter_trace.flFraction * data.trace_length_remaining;
	data.current_damage *= pow( wpn_data->range_modifier, ( data.trace_length * 0.002 ) );
	if ( ( data.trace_length > 3000.f ) || ( enter_surf_penetration_mod < 0.1f ) )
		data.penetrate_count = 0;
	if ( data.penetrate_count <= 0 )
		return false;
	Vector dummy;
	C_Trace trace_exit;
	if ( !g_Autowall.TraceToExit2( dummy, &data.enter_trace, data.enter_trace.end, data.direction, &trace_exit ) )
		return false;
	surfacedata_t * exit_surface_data = g_pSurfaceData->GetSurfaceData( trace_exit.surface.surfaceProps );
	int exit_material = exit_surface_data->game.material;
	float exit_surf_penetration_mod = exit_surface_data->game.flPenetrationModifier;
	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;
	if ( ( ( data.enter_trace.contents & contents_grate ) != 0 ) || ( enter_material == 89 ) || ( enter_material == 71 ) )
	{
		combined_penetration_modifier = 3.0f;
		final_damage_modifier = 0.05f;
	}
	else
		combined_penetration_modifier = ( enter_surf_penetration_mod + exit_surf_penetration_mod ) * 0.5f;
	if ( enter_material == exit_material )
	{
		if ( exit_material == 87 || exit_material == 85 )combined_penetration_modifier = 3.0f;
		else if ( exit_material == 76 )combined_penetration_modifier = 2.0f;
	}
	float v34 = fmaxf( 0.f, 1.0f / combined_penetration_modifier );
	float v35 = ( data.current_damage * final_damage_modifier ) + v34 * 3.0f * fmaxf( 0.0f, ( 3.0f / wpn_data->penetration ) * 1.25f );
	float thickness = VectorLength( trace_exit.end - data.enter_trace.end );
	if ( extracheck )
		if ( !VectortoVectorVisible( trace_exit.end, point ) )
			return false;
	thickness *= thickness;
	thickness *= v34;
	thickness /= 24.0f;
	float lost_damage = fmaxf( 0.0f, v35 + thickness );
	if ( lost_damage > data.current_damage )
		return false;
	if ( lost_damage >= 0.0f )
		data.current_damage -= lost_damage;
	if ( data.current_damage < 1.0f )
		return false;
	data.src = trace_exit.end;
	data.penetrate_count--;

	return true;
}

float Autowall::Damage( const Vector & point )
{
	auto data = FireBulletData( g::pLocalEntity->GetEyePosition( ), g::pLocalEntity );

	Vector angles;
	angles = g_Math.CalcAngle( data.src, point );
	g_Math.AngleVectors( angles, &data.direction );
	VectorNormalize( data.direction );

	if ( SimulateFireBullet( g::pLocalEntity, g::pLocalEntity->GetActiveWeapon( ), data ) )
		return data.current_damage;

	return 0.f;
}

bool Autowall::CanHitFloatingPoint( const Vector & point, const Vector & source ) // ez
{
	if ( !g::pLocalEntity )
		return false;

	FireBulletData data = FireBulletData( source, g::pLocalEntity );

	Vector angles = g_Math.CalcAngle( data.src, point );
	g_Math.AngleVectors( angles, &data.direction );
	VectorNormalize( data.direction );

	C_BaseCombatWeapon* pWeapon = ( C_BaseCombatWeapon* )g::pLocalEntity->GetActiveWeapon( );

	if ( !pWeapon )
		return false;

	data.penetrate_count = 1;
	data.trace_length = 0.0f;

	WeaponInfo_t* weaponData = pWeapon->GetCSWpnData( );

	if ( !weaponData )
		return false;

	data.current_damage = ( float )weaponData->damage;
	data.trace_length_remaining = weaponData->range - data.trace_length;
	Vector end = data.src + ( data.direction * data.trace_length_remaining );
	TraceLine( data.src, end, mask_shot | contents_hitbox, g::pLocalEntity, &data.enter_trace );
	if ( weaponData )
		if ( VectortoVectorVisible( data.src, point ) || HandleBulletPenetration( weaponData, data, true, point ) )
			return true;

	return false;
}