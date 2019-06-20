#include "GlobalVars.h"

namespace g
{
	bool           AllowAnimationUpdate[ 65 ];
	bool           AllowBoneSetup[ 65 ];
	CUserCmd* pCmd = nullptr;
	C_BaseEntity* pLocalEntity = nullptr;
	std::uintptr_t uRandomSeed = NULL;
	Vector         OriginalView;
	bool           bSendPacket;
	bool		   LagPeek = false;
	int            TargetIndex = -1;
	Vector         EnemyEyeAngs[ 65 ];
	Vector         AimbotHitbox[ 65 ][ 28 ];
	Vector         RealAngle;
	Vector         FakeAngle;
	Vector         TpAngle;
	bool           Shot[ 65 ];
	bool           Hit[ 65 ];
	int            MissedShots[ 65 ];
	int			   hitmarkerTime = 0;
	Vector		   LastOrigin;
	float          pCurrentFOV;
	int			   iEntityWeWereLastAimingAt = -1;
	int			   iTotalEntShots[65];
	int			   iTotalHitShots[65];
	float          w2s_matrix[ 4 ][ 4 ];

	DWORD CourierNew;
	DWORD Tahoma;

	namespace fonts {
		DWORD others_font;
		DWORD esp_font;
		DWORD menu_font;
		DWORD watermark_font;
		DWORD watermark2_font;
		DWORD tab_font;
		DWORD flags_font;
		DWORD weapon_icons_font;
		DWORD manual_antiaim_font;
	}
}