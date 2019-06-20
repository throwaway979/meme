#include "awall.h"
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

bool awall::is_breakable(C_BaseEntity* ent) {
	if (!ent || !ent->EntIndex())
		return false;

	int backup_value = *reinterpret_cast<byte*>(uintptr_t(ent) + 0x27C);
	ClientClass* p_class = ent->GetClientClass();

	if (!p_class)
		return false;

	// strcmp kills framerate so don't try to argue this
	if ((p_class->pNetworkName[1] == 'B' &&
		p_class->pNetworkName[9] == 'e' &&
		p_class->pNetworkName[10] == 'S' &&
		p_class->pNetworkName[16] == 'e') ||
		(p_class->pNetworkName[1] != 'B' ||
			p_class->pNetworkName[5] != 'D'))
		*reinterpret_cast<byte*>(uintptr_t(ent) + 0x27C) = 2;

	typedef bool(__thiscall* is_breakable_ent_fn)(C_BaseEntity*);
	static is_breakable_ent_fn is_breakable_ent = (is_breakable_ent_fn)Utils::FindPattern("client_panorama.dll", (PBYTE)"\x55\x8B\xEC\x51\x56\x8B\xF1\x85\xF6\x74\x68", "xxxxxxxxxxx");

	bool breakable = is_breakable_ent(ent);
	*reinterpret_cast<byte*>(uintptr_t(ent) + 0x27C) = backup_value;

	return breakable;
}

float awall::get_thickness(Vector start, Vector end) {
	float current_thickness = 0.f;

	Vector direction;

	g_Math.AngleVectors(g_Math.CalcAngle(start, end), &direction);

	CTraceWorldOnly filter = CTraceWorldOnly(nullptr);
	C_Trace enter_trace, exit_trace;

	for (int i = 0; i < 4; i++) {
		C_Ray ray = C_Ray(start, end);

		g_pTrace->TraceRay(ray, mask_all, &filter, &enter_trace);

		if (enter_trace.flFraction == 1.f) return current_thickness;

		start = enter_trace.end;

		if (!trace_to_exit(enter_trace, exit_trace, start, direction)) return current_thickness + 90.f;

		start = exit_trace.end;
		current_thickness += (start - exit_trace.end).Length();
	}

	return current_thickness;
}

bool awall::trace_to_exit(C_Trace &enter_trace, C_Trace &exit_trace, Vector start_pos, Vector direction) {
	Vector start, end;
	float max_dist = 90.f, current_dist = 0.f;
	int first_contents = 0;

	while (current_dist <= max_dist) {
		current_dist += 4.f;
		start = start_pos + direction * current_dist;
		if (!first_contents)
			first_contents = g_pTrace->GetPointContents(start, mask_shot_hull | contents_hitbox, nullptr);
		int point_contents = g_pTrace->GetPointContents(start, mask_shot_hull | contents_hitbox, nullptr);

		if (!(point_contents & mask_shot_hull) || (point_contents & contents_hitbox) && point_contents != first_contents) {
			end = start - (direction * 4.f);

			_util_trace_line(start, end, mask_shot_hull | contents_hitbox, nullptr, &exit_trace);

			if (exit_trace.startSolid && exit_trace.surface.flags & surf_hitbox) {
				_util_trace_line(start, start_pos, mask_shot_hull, exit_trace.m_pEnt, &exit_trace);

				if (exit_trace.DidHit() && !exit_trace.startSolid) {
					start = exit_trace.end;
					return true;
				}

				continue;
			}

			//hey can we hit?
			if (exit_trace.DidHit() && !exit_trace.startSolid) {
				if (is_breakable(enter_trace.m_pEnt) && is_breakable(enter_trace.m_pEnt)) return true;

				if (enter_trace.surface.flags & surf_nodraw || !(exit_trace.surface.flags & surf_nodraw) && (exit_trace.plane.normal.Dot(direction) <= 1.f)) {
					start -= direction * (exit_trace.flFraction * 4.f);
					return true;
				}

				continue;
			}

			if (!exit_trace.DidHit() || exit_trace.startSolid) {
				if (enter_trace.DidHitNonWorldEntity() && is_breakable(enter_trace.m_pEnt)) {
					exit_trace = enter_trace;
					exit_trace.end = start + direction;
					return true;
				}
			}
		}
	}

	return false;
}

void awall::scale_damage(C_BaseEntity* ent, WeaponInfo_t* weapon_info, int hitgroup, float& current_dmg) {
	if (!ent || &current_dmg == nullptr)
		return;

	bool has_heavy_armour = ent->GetHeavyArmor();
	int armour_value = ent->ArmorValue();

	auto is_armoured = [&ent, &hitgroup]() -> bool {
		switch (hitgroup) {
		case HITGROUP_HEAD:
			return ent->HasHelmet();
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			return true;
		default:
			return false;
		}
	};

	switch (hitgroup) {
	case HITGROUP_HEAD:
		current_dmg *= has_heavy_armour ? 2.f : 4.f; break; // beacuse heavy armour is still a thing
	case HITGROUP_STOMACH:
		current_dmg *= 1.25f; break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		current_dmg *= 0.75f; break;
	default:
		break;
	}

	if (armour_value > 0 && is_armoured()) {
		float bonus_value = 1.f, armour_bonus_ratio = 0.5f, armour_ratio = weapon_info->armor_ratio / 2.f;

		if (has_heavy_armour) {
			armour_bonus_ratio = 0.33f;
			armour_ratio *= 0.5f;
			bonus_value = 0.25f;
		}

		auto new_damage = has_heavy_armour ? (current_dmg * armour_ratio) * 0.8f : current_dmg * armour_ratio;

		if (((current_dmg - (current_dmg * armour_ratio)) * (bonus_value * armour_bonus_ratio)) > armour_value)
			new_damage = current_dmg - (armour_value / armour_bonus_ratio);

		current_dmg = new_damage;
	}
}

// legacy function, just force return new sv_penetration_type values beacuse noone uses old penetration
void awall::_get_bullet_type_params(float& max_range, float& max_dist) {
	max_range = 35.f;
	max_dist = 3000.f;
}

// traces are cool
void awall::_util_trace_line(Vector &vec_abs_start, Vector &vec_abs_end, unsigned int mask, C_BaseEntity* ignore, C_Trace* tr) {
	C_Ray ray = C_Ray(vec_abs_start, vec_abs_end);
	C_TraceFilter filter = C_TraceFilter(ignore);

	g_pTrace->TraceRay(ray, mask, &filter, tr);
}

void awall::_util_clip_trace_to_players(const Vector& vec_abs_start, const Vector& vec_abs_end, unsigned int mask, ITraceFilter* filter, C_Trace* tr) {
	static DWORD dword_address = Utils::FindPattern("client_panorama.dll", (BYTE*)"\x53\x8B\xDC\x83\xEC\x08\x83\xE4\xF0\x83\xC4\x04\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x81\xEC\xD8\x00\x00\x00\x0F\x57\xC9", "xxxxxxxxxxxxxxxxxxxxxxxxx???xxx");

	if (!dword_address)
		return;

	_asm {
		MOV		EAX, filter
		LEA		ECX, tr
		PUSH	ECX
		PUSH	EAX
		PUSH	mask
		LEA		EDX, vec_abs_end
		LEA		ECX, vec_abs_start
		CALL	dword_address
		ADD		ESP, 0xC
	}
}


bool awall::handle_bullet_penetration(WeaponInfo_t * weapon_info, firebulletdata_t& _data) {
	C_Trace trace_exit;
	surfacedata_t* enter_surface_data = g_pSurfaceData->GetSurfaceData(_data.m_enter_trace.surface.surfaceProps);

	int enter_material = enter_surface_data->game.material;
	float enter_surface_penetration_modifier = enter_surface_data->game.flPenetrationModifier, final_dmg_modifier = 0.16f, compined_penetration_modifier = 0.f;
	bool solid_surf = ((_data.m_enter_trace.contents >> 3) & contents_solid), light_surf = ((_data.m_enter_trace.surface.flags >> 7) & surf_light);

	if (_data.m_penetration_count <= 0
		|| (_data.m_enter_trace.surface.name == (const char*)0x2227c261 && trace_exit.surface.name == (const char*)0x2227c868) //fix for stupid dumb poo-poo wall on cache
		|| (!_data.m_penetration_count && !light_surf && !solid_surf && enter_material != CHAR_TEX_GLASS && enter_material != CHAR_TEX_GRATE)
		|| weapon_info->penetration <= 0.f
		|| !trace_to_exit(_data.m_enter_trace, trace_exit, _data.m_enter_trace.end, _data.m_direction)
		&& !(g_pTrace->GetPointContents(_data.m_enter_trace.end, mask_shot_hull | contents_hitbox, NULL) & (mask_shot_hull | contents_hitbox)))
		return false;

	surfacedata_t *exit_surface_data = g_pSurfaceData->GetSurfaceData(trace_exit.surface.surfaceProps);
	int exit_material = exit_surface_data->game.material;
	float exit_surface_penetration_modifier = exit_surface_data->game.flPenetrationModifier;

	if (enter_material == CHAR_TEX_GLASS || enter_material == CHAR_TEX_GRATE) {
		compined_penetration_modifier = 3.f;
		final_dmg_modifier = 0.05f;
	}

	else if (light_surf || solid_surf) {
		compined_penetration_modifier = 1.f;
		final_dmg_modifier = 0.16f;
	}
	else {
		compined_penetration_modifier = (enter_surface_penetration_modifier + exit_surface_penetration_modifier) * 0.5f;
		final_dmg_modifier = 0.16f;
	}

	if (enter_material == exit_material) {
		if (exit_material == CHAR_TEX_CARDBOARD || exit_material == CHAR_TEX_WOOD)
			compined_penetration_modifier = 3.f;
		else if (exit_material == CHAR_TEX_PLASTIC)
			compined_penetration_modifier = 2.0f;
	}

	float thickness = (trace_exit.end - _data.m_enter_trace.end).LengthSqr();
	float modifier = fmaxf(0.f, 1.f / compined_penetration_modifier);

	float lost_damage = fmaxf(
		((modifier * thickness) / 24.f) //* 0.041666668
		+ ((_data.m_current_damage * final_dmg_modifier)
			+ (fmaxf(3.75 / weapon_info->penetration, 0.f) * 3.f * modifier)), 0.f);

	if (lost_damage > _data.m_current_damage)
		return false;

	if (lost_damage > 0.f)
		_data.m_current_damage -= lost_damage;

	if (_data.m_current_damage < 1.f)
		return false;

	_data.m_current_position = trace_exit.end;
	_data.m_penetration_count--;

	return true;
}

awall_return awall::calc_return_info(Vector start, Vector end, C_BaseEntity* from_ent, C_BaseEntity* to_ent, int specific_hitgroup) {
	C_BaseEntity* local_nigger = g::pLocalEntity;
	awall_return return_info = awall_return(-1, -1, 4, false, 0.f, nullptr);

	firebulletdata_t fire_bullet_data;
	fire_bullet_data.m_start = start;
	fire_bullet_data.m_end = end;
	fire_bullet_data.m_current_position = start;
	fire_bullet_data.m_thickness = 0.f;
	fire_bullet_data.m_penetration_count = 4;

	g_Math.AngleVectors(g_Math.CalcAngle(start, end), &fire_bullet_data.m_direction);

	if (!from_ent) from_ent = local_nigger;
	if (!local_nigger) return return_info;

	CTraceFilterOneEntity filter_player = CTraceFilterOneEntity(to_ent);
	C_TraceFilter filter_local = C_TraceFilter(from_ent);

	fire_bullet_data.m_filter = &filter_player;

	if (from_ent->GetClientClass()->ClassID != EClassIds::CCSPlayer) return return_info;

	C_BaseCombatWeapon* weapon = from_ent->GetActiveWeapon();
	if (!weapon) return return_info;

	WeaponInfo_t* weapon_info = weapon->GetCSWpnData();
	if (!weapon_info)
		return return_info;

	float range = min(weapon_info->range, (start - end).Length());
	end = start + (fire_bullet_data.m_direction * range);
	fire_bullet_data.m_current_damage = weapon_info->damage;

	while (fire_bullet_data.m_current_damage && fire_bullet_data.m_penetration_count) {
		return_info.penetration_count = fire_bullet_data.m_penetration_count;

		_util_trace_line(fire_bullet_data.m_current_position, end, mask_shot | contents_grate, from_ent, &fire_bullet_data.m_enter_trace);

		//clip_trace_to_player( fire_bullet_data.m_current_position, fire_bullet_data.m_current_position + ( fire_bullet_data.m_direction * 40.f ), e_to, MASK_SHOT | CONTENTS_GRATE, fire_bullet_data.m_filter, &fire_bullet_data.m_enter_trace );
		_util_clip_trace_to_players(fire_bullet_data.m_current_position, fire_bullet_data.m_current_position + (fire_bullet_data.m_direction * 40.f), mask_shot | contents_grate, fire_bullet_data.m_filter, &fire_bullet_data.m_enter_trace);

		const float distance_traced = (fire_bullet_data.m_enter_trace.end - start).Length();
		fire_bullet_data.m_current_damage *= pow(weapon_info->range_modifier, (distance_traced / 500.f));

		// if reached the end
		if (fire_bullet_data.m_enter_trace.flFraction == 1.f) {
			if (to_ent && specific_hitgroup != -1) {
				scale_damage(to_ent, weapon_info, specific_hitgroup, fire_bullet_data.m_current_damage);

				return_info.damage = fire_bullet_data.m_current_damage;
				return_info.hitgroup = specific_hitgroup;
				return_info.end = fire_bullet_data.m_enter_trace.end;
				return_info.hit_entity = to_ent;
			}
			else {
				return_info.damage = fire_bullet_data.m_current_damage;
				return_info.hitgroup = -1;
				return_info.end = fire_bullet_data.m_enter_trace.end;
				return_info.hit_entity = nullptr;
			}

			break;
		}

		// if actually hit
		if (fire_bullet_data.m_enter_trace.hitGroup > 0 && fire_bullet_data.m_enter_trace.hitGroup <= 7) {
			if ((to_ent && fire_bullet_data.m_enter_trace.m_pEnt != to_ent) ||
				(reinterpret_cast<C_BaseEntity *>(fire_bullet_data.m_enter_trace.m_pEnt)->GetTeam() == from_ent->GetTeam())) {
				return_info.damage = -1;
				return return_info;
			}

			if (specific_hitgroup != -1)
				scale_damage(reinterpret_cast<C_BaseEntity *>(fire_bullet_data.m_enter_trace.m_pEnt), weapon_info, specific_hitgroup, fire_bullet_data.m_current_damage);
			else
				scale_damage(reinterpret_cast<C_BaseEntity *>(fire_bullet_data.m_enter_trace.m_pEnt), weapon_info, fire_bullet_data.m_enter_trace.hitGroup, fire_bullet_data.m_current_damage);

			return_info.damage = fire_bullet_data.m_current_damage;
			return_info.hitgroup = fire_bullet_data.m_enter_trace.hitGroup;
			return_info.end = fire_bullet_data.m_enter_trace.end;
			return_info.hit_entity = fire_bullet_data.m_enter_trace.m_pEnt;
			break;
		}

		if (!handle_bullet_penetration(weapon_info, fire_bullet_data))
			break;

		return_info.did_penetrate_wall = true;
	}

	return_info.penetration_count = fire_bullet_data.m_penetration_count;
	return return_info;
}