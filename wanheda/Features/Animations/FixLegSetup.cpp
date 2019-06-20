#include "FixLegSetup.h"

c_animations g_anim;

void __fastcall animations::DoExtraBonesProcessing( uintptr_t ecx, uintptr_t edx, studiohdr_t *hdr, Vector *pos, Quaternion *q, matrix3x4_t *matrix, CBoneBitList &bone_list, CIKContext *context ) {
	auto e = ( C_BaseEntity* )ecx;
	auto original = g_anim.m_track[e->EntIndex()].m_vmt->get_old_method< fn::DoExtraBonesProcessing_t >(195);
	const auto animstate = e->AnimState();

	if (!animstate || !animstate->pBaseEntity)
		return original(ecx, hdr, pos, q, matrix, bone_list, context);

	const auto backup = animstate->m_flUnknownFraction;


	animstate->m_flUnknownFraction = 0.f;

	original(ecx, hdr, pos, q, matrix, bone_list, context);

	if (!(GetAsyncKeyState(VK_SHIFT) && e == g::pLocalEntity) ) { //we out here setting unkfraction on keypress :sunglasses:
		animstate->m_flUnknownFraction = backup;
	}
}

void c_animations::init( ) {
	g_pEntityList->AddListenerEntity( this );
}

void c_animations::remove( ) {
	g_pEntityList->RemoveListenerEntity(this);

	for (int i = 0; i < 64; i++) {
		if (m_track.at(i).m_hooked) {
			m_track.at(i).m_vmt->unhook_all();
		}
	}
}

void c_animations::OnEntityCreated( C_BaseEntity *ent ) {
	if (!ent)
		return;

	const int idx = ent->EntIndex();
	if (idx < 0)
		return;

	ClientClass *cc = ent->GetClientClass();
	if (!cc)
		return;

	switch (cc->ClassID) {
	case 38: {
		m_track.at(idx) = container_t(ent);
		m_players.emplace(idx, idx);

		break;
	}
	default:
		break;
	}
}

void c_animations::OnEntityDeleted( C_BaseEntity *ent ) {
	if (!ent)
		return;

	int idx = ent->EntIndex();
	if (idx < 0)
		return;

	m_players.erase(idx);

	const auto it = std::find_if(m_track.begin(), m_track.end(), [&](const container_t &data) {
		return data.m_idx == idx;
	});

	if (it == m_track.end())
		return;

	if (m_track.at(it->m_idx).m_hooked)
		m_track.at(it->m_idx).m_vmt->unhook_all();
}