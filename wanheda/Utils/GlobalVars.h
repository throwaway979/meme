#pragma once
#include "..\SDK\CInput.h"
#include "..\SDK\CEntity.h"

#define VK_A 0x41
#define VK_B 0x42
#define VK_C 0x43
#define VK_D 0x44
#define VK_E 0x45
#define VK_F 0x46
#define VK_G 0x47
#define VK_H 0x48
#define VK_I 0x49
#define VK_J 0x4A
#define VK_K 0x4B
#define VK_L 0x4C
#define VK_M 0x4D
#define VK_N 0x4E
#define VK_O 0x4F
#define VK_P 0x50
#define VK_Q 0x51
#define VK_R 0x52
#define VK_S 0x53
#define VK_T 0x54
#define VK_U 0x55
#define VK_V 0x56
#define VK_W 0x57
#define VK_X 0x58
#define VK_Y 0x59
#define VK_Z 0x5A

namespace g
{
	extern bool           AllowAnimationUpdate[ 65 ];
	extern bool           AllowBoneSetup[ 65 ];
	extern CUserCmd* pCmd;
	extern C_BaseEntity* pLocalEntity;
	extern std::uintptr_t uRandomSeed;
	extern Vector         OriginalView;
	extern bool           bSendPacket;
	extern bool			  LagPeek;
	extern int            TargetIndex;
	extern Vector         EnemyEyeAngs[ 65 ];
	extern Vector         AimbotHitbox[ 65 ][ 28 ];
	extern Vector         RealAngle;
	extern Vector         FakeAngle;
	extern Vector         TpAngle;
	extern bool           Shot[ 65 ];
	extern bool           Hit[ 65 ];
	extern int            MissedShots[ 65 ];
	extern int			  hitmarkerTime;
	extern Vector		  LastOrigin;
	extern float          pCurrentFOV;
	extern int			  iEntityWeWereLastAimingAt;
	extern int			  iTotalEntShots[65];
	extern int			  iTotalHitShots[65];
	extern                float w2s_matrix[ 4 ][ 4 ];



	extern DWORD CourierNew;
	extern DWORD Tahoma;

	namespace fonts {
		extern DWORD others_font;
		extern DWORD esp_font;
		extern DWORD menu_font;
		extern DWORD watermark_font;
		extern DWORD watermark2_font;
		extern DWORD tab_font;
		extern DWORD flags_font;
		extern DWORD weapon_icons_font;
		extern DWORD manual_antiaim_font;
	}
}