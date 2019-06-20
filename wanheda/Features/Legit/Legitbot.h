#pragma once
#include "..\..\SDK\CEntity.h"

class Legitbot {
private:
	Vector HitBoxPosition( C_BaseEntity* entity, int hitbox_id );
	bool get_weapon_settings(C_BaseCombatWeapon * weapon);
	void auto_pistol();
	int find_target();
public:
	void OnCreateMove( );
	void CM_Backtrack( );
};

extern Legitbot g_Legitbot;