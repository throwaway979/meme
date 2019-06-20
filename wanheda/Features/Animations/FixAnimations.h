#pragma once
#include "..\..\Hooks.h"
#include "..\..\Utils\GlobalVars.h"
#include "..\..\SDK\Definitions.h"
#include "RebuildSetupbones.h"
#include "VMT.h"
#include <array>
#include <map>

namespace animations {
	void __fastcall UpdateClientSideAnimations(uintptr_t ecx, uintptr_t edx);
	void __fastcall DoExtraBonesProcessing(uintptr_t ecx, uintptr_t edx, studiohdr_t* hdr, Vector* pos, Quaternion* q, matrix3x4_t* matrix, CBoneBitList& bone_list, CIKContext* context);
	bool __fastcall SetupBones(uintptr_t ecx, uintptr_t edx, matrix3x4_t* bone_to_world_out, int max_bones, int bone_bask, float current_time);
	namespace fn {
		using DoExtraBonesProcessing_t = void(__thiscall*)(uintptr_t, studiohdr_t*, Vector*, Quaternion*, matrix3x4_t*, CBoneBitList&, CIKContext*);
		using SetupBones_t = bool(__thiscall*)(uintptr_t, matrix3x4_t*, int, int, float);
		using UpdateClientSideAnimations_t = void(__thiscall *)(uintptr_t);
	}
}

struct container_t {
	container_t() { }
	container_t(C_BaseEntity* ent) {
		m_idx = ent->EntIndex();
		m_vmt = std::make_unique< c_vmt >(ent);
		m_renderable_vmt = std::make_unique< c_vmt >(ent->GetClientRenderable());
		m_vmt->hook_method(195, animations::DoExtraBonesProcessing);
		m_vmt->hook_method( 221, animations::UpdateClientSideAnimations);
		m_renderable_vmt->hook_method(13, animations::SetupBones);
		m_hooked = true;
	}

	std::unique_ptr< c_vmt > m_vmt;
	std::unique_ptr< c_vmt > m_renderable_vmt;

	int m_idx;
	bool m_hooked;
};

class c_animations : public IClientEntityListener {
public:
	void OnEntityCreated(C_BaseEntity* ent) override;
	void OnEntityDeleted(C_BaseEntity* ent) override;

	void init();
	void remove();

	std::array< container_t, 64 > m_track;
	std::map< int, int > m_players;
};

extern c_animations g_Anim;

struct animation_info {
	animation_info() {
		spawn_time = 0.f;
		ref_handle = nullptr;
		server_state = nullptr;
	}
	float spawn_time;
	CBaseHandle ref_handle;
	C_AnimState* server_state;
};

class fix_animations {
private:
	animation_info info[65];
public:
	Vector server_angles[65];
	void HandleAnimations() {
		for (int j = 1; j <= 64; j++) {
			auto fix_goal_feet_yaw = [](float rotation, float invertedrotation, float yawfeetdelta, float yaw, C_AnimState* state) {
				if (yawfeetdelta < rotation) {
					if (invertedrotation > yawfeetdelta)
						*(float*)(uintptr_t(state) + 0x80) = invertedrotation + yaw;
				}
				else
					*(float*)(uintptr_t(state) + 0x80) = yaw - rotation;
			};

			auto get_rotation = [&](int rotation_type, C_AnimState* state) {
				float v43 = *(float*)((uintptr_t)state + 0xA4);
				float v54 = max(0, min(*reinterpret_cast<float*>((uintptr_t)state + 0xF8), 1));
				float v55 = max(0, min(1, *reinterpret_cast<float*>((uintptr_t)state + 0xFC)));

				float v56;
				v56 = ((*reinterpret_cast<float*>((uintptr_t)state + 0x11C) * -0.30000001) - 0.19999999) * v54;
				if (v43 > 0)
					v56 += ((v43 * v55) * (0.5 - v56));

				v56 = *(float*)((uintptr_t)state + rotation_type) * v56;
				return v56;
			};
			auto target = g_pEntityList->GetClientEntity(j);
			auto& rec = info[j];

			if (!target || target->GetHealth() <= 0)
				continue;

			float spawn_time = *(float*)((uintptr_t)target + 0xA290);

			bool  allocate = (rec.server_state == nullptr),
				change = (!allocate) && (target->GetRefEHandle() != rec.ref_handle),
				reset = (!allocate && !change) && (spawn_time != rec.spawn_time);

			if (reset) {
				reset_state(rec.server_state);
				rec.spawn_time = spawn_time;
			}

			if (allocate || change) {
				auto state = (C_AnimState*)(g_pMemAlloc->Alloc(sizeof(C_AnimState)));
				if (state != nullptr)
					create_state(state, target);

				rec.ref_handle = target->GetRefEHandle();
				rec.spawn_time = spawn_time;

				rec.server_state = state;
			}
			else {

			}


		}
	}

	void create_state(C_AnimState * state, C_BaseEntity * player) {
		using fn = void(__thiscall*)(C_AnimState*, C_BaseEntity*);
		static auto ret = reinterpret_cast<fn>(Utils::FindSignature("client_panorama.dll", "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46"));

		ret(state, player);
	}
	void update_state(C_AnimState * state, Vector ang) {
		using fn = void(__vectorcall*)(void*, void*, float, float, float, void*);
		static auto ret = reinterpret_cast<fn>(Utils::FindSignature("client_panorama.dll", "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24"));

		if (!ret)
			return;

		ret(state, NULL, NULL, ang.y, ang.x, NULL);
	}
	void reset_state(C_AnimState * state) {
		using fn = void(__thiscall*)(C_AnimState*);
		static auto ret = reinterpret_cast<fn>(Utils::FindSignature("client_panorama.dll", "56 6A 01 68 ? ? ? ? 8B F1"));

		ret(state);
	}
};

extern fix_animations g_AnimFix;