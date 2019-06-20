#pragma once
#include "..\..\Hooks.h"
#include "..\..\Utils\GlobalVars.h"
#include "VMT.h"
#include <array>
#include <map>

namespace animations {
	void __fastcall DoExtraBonesProcessing(uintptr_t ecx, uintptr_t edx, studiohdr_t *hdr, Vector *pos, Quaternion *q, matrix3x4_t *matrix, CBoneBitList &bone_list, CIKContext *context);
	namespace fn {
		using DoExtraBonesProcessing_t = void(__thiscall *)(uintptr_t, studiohdr_t *, Vector *, Quaternion *, matrix3x4_t *, CBoneBitList &, CIKContext *);
	}
}

struct container_t {
	container_t() { }
	container_t(C_BaseEntity* ent) {
		m_idx = ent->EntIndex();
		m_vmt = std::make_unique< c_vmt >(ent);
		m_vmt->hook_method(195, animations::DoExtraBonesProcessing);
		m_hooked = true;
	}

	std::unique_ptr< c_vmt > m_vmt;
	int m_idx;
	bool m_hooked;
};

class c_animations : public IClientEntityListener {
public:
	void OnEntityCreated(C_BaseEntity *ent) override;
	void OnEntityDeleted(C_BaseEntity *ent) override;

	void init();
	void remove();

	std::array< container_t, 64 > m_track;
	std::map< int, int > m_players;
};

extern c_animations g_anim;