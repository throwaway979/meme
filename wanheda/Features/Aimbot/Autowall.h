#pragma once
#include "..\..\Utils\GlobalVars.h"
#include "..\..\SDK\CGlobalVarsBase.h"
#include "..\..\SDK\IEngineTrace.h"

struct FireBulletData {
	FireBulletData( const Vector& eyePos, C_BaseEntity* entity ) : src( eyePos ), filter( entity )
	{
	}

	Vector          src;
	C_Trace         enter_trace;
	Vector          direction;
	C_TraceFilter   filter;
	float           trace_length;
	float           trace_length_remaining;
	float           current_damage;
	int             penetrate_count;
};

class Autowall {
public:
	bool CanHitFloatingPoint( const Vector& point, const Vector& source );
	float Damage( const Vector& point );
	bool SimulateFireBullet( C_BaseEntity* local, C_BaseCombatWeapon* weapon, FireBulletData& data );
	bool TraceToExit2( Vector& vecEnd, C_Trace* pEnterTrace, Vector vecStart, Vector vecDir, C_Trace* pExitTrace );
private:
	bool IsBreakableEntity( C_BaseEntity* entity );
};
extern Autowall g_Autowall;