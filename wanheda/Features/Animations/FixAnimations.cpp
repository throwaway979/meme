#include "FixAnimations.h"
#include "../Aimbot/Aimbot.h"
#include "../../SDK/ICvar.h"

c_animations g_Anim;
fix_animations g_AnimFix;

void __fastcall animations::UpdateClientSideAnimations(uintptr_t ecx, uintptr_t edx) {
	auto e = (C_BaseEntity*)ecx;
	auto original = g_Anim.m_track[e->EntIndex()].m_vmt->get_old_method< fn::UpdateClientSideAnimations_t >(221);

//	if (g::AllowAnimationUpdate[e->EntIndex()] || !g_Menu.Config.PosBacktrack || e == g::pLocalEntity)
		original(ecx);

	return;
}

void __fastcall animations::DoExtraBonesProcessing(uintptr_t ecx, uintptr_t edx, studiohdr_t* hdr, Vector* pos, Quaternion* q, matrix3x4_t* matrix, CBoneBitList& bone_list, CIKContext* context) {
	auto e = (C_BaseEntity*)ecx;
	auto original = g_Anim.m_track[e->EntIndex()].m_vmt->get_old_method< fn::DoExtraBonesProcessing_t >(195);
	const auto animstate = e->AnimState();

	if (!animstate || !animstate->pBaseEntity || (!g_Menu.Config.PosBacktrack && e != g::pLocalEntity))
		return original(ecx, hdr, pos, q, matrix, bone_list, context);


	auto o_on_ground = animstate->m_bOnGround;

	animstate->m_bOnGround = false;
	original(ecx, hdr, pos, q, matrix, bone_list, context);
	animstate->m_bOnGround = o_on_ground;
}

bool __fastcall animations::SetupBones(uintptr_t ecx, uintptr_t edx, matrix3x4_t* bone_to_world_out, int max_bones, int bone_bask, float current_time) {
	auto entity = reinterpret_cast <C_BaseEntity*>(ecx - 4);
	auto original = g_Anim.m_track[entity->EntIndex()].m_renderable_vmt->get_old_method< fn::SetupBones_t >(13);

	if (!entity || entity->GetClientClass()->ClassID != (int)EClassIds::CCSPlayer|| !g_Menu.Config.PosBacktrack)
		return original(ecx, bone_to_world_out, max_bones, bone_bask, current_time);

	if (bone_bask == 0x00000100)
		bone_bask |= 0x200;
	if (current_time == g_pEngine->GetLastTimeStamp())
		current_time += 3.5f;
//readded cuz tommy wanted it in yet again
	//backup
	static auto host_timescale = g_pCvar->FindVar(("host_timescale"));
	float OldCurTime = g_pGlobalVars->curtime;
	float OldRealTime = g_pGlobalVars->realtime;
	float OldFrameTime = g_pGlobalVars->frametime;
	float OldAbsFrameTime = g_pGlobalVars->absoluteframetime;
	float OldAbsFrameTimeStart = g_pGlobalVars->absoluteframestarttimestddev;
	float OldInterpAmount = g_pGlobalVars->interpolationAmount;
	int OldFrameCount = g_pGlobalVars->framecount;
	int OldTickCount = g_pGlobalVars->tickcount;

	g_pGlobalVars->curtime = entity->GetSimulationTime();
	g_pGlobalVars->realtime = entity->GetSimulationTime();
	g_pGlobalVars->frametime = g_pGlobalVars->intervalPerTick * host_timescale->GetFloat();
	g_pGlobalVars->absoluteframetime = g_pGlobalVars->intervalPerTick * host_timescale->GetFloat();
	g_pGlobalVars->absoluteframestarttimestddev = entity->GetSimulationTime() - g_pGlobalVars->intervalPerTick * host_timescale->GetFloat();
	g_pGlobalVars->interpolationAmount = 0;
	g_pGlobalVars->framecount = TIME_TO_TICKS(entity->GetSimulationTime());
	g_pGlobalVars->tickcount = TIME_TO_TICKS(entity->GetSimulationTime());

	static int m_fFlags = g_pNetvars->GetOffset("DT_BasePlayer", "m_fFlags");
	auto backup = *(int*)(entity + 2600);


	*(int*)(entity + 2600) |= 0xA;
	*(int*)((int)entity + 236) |= 8; // IsNoInterpolationFrame
	*reinterpret_cast<int*>(uintptr_t(entity) + m_fFlags) |= 0xF0;
	bool ret_value = original(ecx, bone_to_world_out, max_bones, bone_bask, current_time);
	*reinterpret_cast<int*>(uintptr_t(entity) + m_fFlags) &= ~(0xF0);
	*reinterpret_cast<int*>(uintptr_t(entity) + m_fFlags) |= 0x98;
	*(int*)(entity + 2600) = backup;
	*(int*)((int)entity + 236) &= ~8; // (1 << 3)

	//restore old values
	g_pGlobalVars->curtime = OldCurTime;
	g_pGlobalVars->realtime = OldRealTime;
	g_pGlobalVars->frametime = OldFrameTime;
	g_pGlobalVars->absoluteframetime = OldAbsFrameTime;
	g_pGlobalVars->absoluteframestarttimestddev = OldAbsFrameTimeStart;
	g_pGlobalVars->interpolationAmount = OldInterpAmount;
	g_pGlobalVars->framecount = OldFrameCount;
	g_pGlobalVars->tickcount = OldTickCount;

	return ret_value;

	//return g_SetupBones.HandleBoneSetup(entity, bone_to_world_out, bone_bask, current_time);
}

void c_animations::init() {
	g_pEntityList->AddListenerEntity(this);
}

void c_animations::remove() {
	g_pEntityList->RemoveListenerEntity(this);

	for (int i = 0; i < 64; i++) {
		if (m_track.at(i).m_hooked) {
			m_track.at(i).m_vmt->unhook_all();
			m_track.at(i).m_renderable_vmt->unhook_all();
		}
	}
}

void c_animations::OnEntityCreated(C_BaseEntity * ent) {
	if (!ent)
		return;

	const int idx = ent->EntIndex();
	if (idx < 0)
		return;

	ClientClass* cc = ent->GetClientClass();
	if (!cc)
		return;

	switch (cc->ClassID) {
	case (int)EClassIds::CCSPlayer: {
		m_track.at(idx) = container_t(ent);
		m_players.emplace(idx, idx);

		break;
	}
	default:
		break;
	}
}

void c_animations::OnEntityDeleted(C_BaseEntity * ent) {
	if (!ent)
		return;

	int idx = ent->EntIndex();
	if (idx < 0)
		return;

	m_players.erase(idx);

	const auto it = std::find_if(m_track.begin(), m_track.end(), [&](const container_t & data) {
		return data.m_idx == idx;
	});

	if (it == m_track.end())
		return;

	if (m_track.at(it->m_idx).m_hooked) {
		m_track.at(it->m_idx).m_vmt->unhook_all();
		m_track.at(it->m_idx).m_renderable_vmt->unhook_all();
		m_track.at(it->m_idx).m_hooked = false;
	}
}
