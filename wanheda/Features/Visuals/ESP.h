#pragma once
#include "..\..\Utils\GlobalVars.h"
#include "..\..\Utils\Utils.h"
#include "..\..\Utils\Math.h"
#include "..\..\SDK\IVEngineClient.h"
#include "..\..\SDK\PlayerInfo.h"
#include "..\..\SDK\Definitions.h"
#include "..\..\SDK\CGlobalVarsBase.h"
#include "..\..\SDK\ClientClass.h"
#include "..\..\SDK\IEngineTrace.h"

struct TestPos {
	Vector Pos;
	float SimulationTime;
};

class ESP {
public:
	void Render( );
private:
	struct {
		int left, top, right, bottom;
	}Box;

	Color color;
	Color textcolor;
	Color skelecolor;
	DWORD font;
	int offsetY;

	const bool calculate_box_position( C_BaseEntity* entity );
	void RenderBox( );
	void RenderWeaponName( C_BaseEntity* pEnt );
	void RenderName( C_BaseEntity* pEnt, int iterator );
	void RenderMoney( C_BaseEntity* pEnt );
	void RenderHealth( C_BaseEntity* pEnt );
	void RenderAmmo( C_BaseEntity* pEnt );
	void RenderHitboxPoints( C_BaseEntity* pEnt );
	void RenderSkeleton( C_BaseEntity* pEnt );
	void RenderArrows( C_BaseEntity* pEnt );
};
extern ESP g_ESP;