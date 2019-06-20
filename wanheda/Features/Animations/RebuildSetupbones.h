#pragma once
#include "../../SDK/CEntity.h"

class SetupBonesRebuild {
public:
	bool HandleBoneSetup( C_BaseEntity* target, matrix3x4_t* boneOut, int boneMask, float curtime );
};

extern SetupBonesRebuild g_SetupBones;