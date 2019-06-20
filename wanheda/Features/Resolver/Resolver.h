#pragma once
#include "..\..\Utils\GlobalVars.h"
#include "..\..\SDK\CGlobalVarsBase.h"
#include "..\..\SDK\IClientMode.h"
#include <deque>

class Resolver {
public:
	bool UseFreestandAngle[65];
	float FreestandAngle[65];
	Vector absOriginBackup;
	float pitchHit[65];

	void FrameStage( ClientFrameStage_t stage );
	void OnCreateMove();
private:
	void Resolve( C_BaseEntity* pEnt );
};
extern Resolver g_Resolver;