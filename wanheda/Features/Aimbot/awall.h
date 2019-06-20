#pragma once
#include "..\..\Utils\GlobalVars.h"
#include "..\..\SDK\CGlobalVarsBase.h"
#include "..\..\SDK\IEngineTrace.h"

struct awall_return {
	int damage, hitgroup, penetration_count;
	bool did_penetrate_wall;
	float thickness;
	Vector end;
	C_BaseEntity* hit_entity;

	awall_return(int dmg, int hgroup, int penetrate_count, bool did_penetrate_w, float thicc, C_BaseEntity* ent) {
		damage = dmg; hitgroup = hgroup; penetration_count = penetrate_count; did_penetrate_wall = did_penetrate_w; thickness = thicc; hit_entity = ent;
	}
};

struct firebulletdata_t {
	Vector m_start, m_end, m_current_position, m_direction;

	ITraceFilter * m_filter;
	C_Trace m_enter_trace;

	float m_thickness, m_current_damage;
	int m_penetration_count;
};

class awall {
public:

	bool is_breakable(C_BaseEntity * ent);
	float get_thickness(Vector start, Vector end);
	bool trace_to_exit(C_Trace & enter_trace, C_Trace & exit_trace, Vector start_pos, Vector direction);
	void scale_damage(C_BaseEntity * ent, WeaponInfo_t * weapon_info, int hitgroup, float & current_dmg);
	void _get_bullet_type_params(float & max_range, float & max_dist);
	void _util_trace_line(Vector & vec_abs_start, Vector & vec_abs_end, unsigned int mask, C_BaseEntity * ignore, C_Trace * tr);
	void _util_clip_trace_to_players(const Vector & vec_abs_start, const Vector & vec_abs_end, unsigned int mask, ITraceFilter * filter, C_Trace * tr);
	bool handle_bullet_penetration(WeaponInfo_t * weapon_info, firebulletdata_t & _data);
	awall_return calc_return_info(Vector start, Vector end, C_BaseEntity * from_ent, C_BaseEntity * to_ent, int specific_hitgroup);

};
extern awall* g_Autowall;