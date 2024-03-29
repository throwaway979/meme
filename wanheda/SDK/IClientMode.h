#pragma once
#include "Vector.h"
#include "CEntity.h"

class IPanel {
public:
	const char* GetName( unsigned int vguiPanel )
	{
		typedef const char* ( __thiscall * Fn )( IPanel*, unsigned int );
		return Utils::GetVFunc<Fn>( this, 36 )( this, vguiPanel );
	}
};
extern IPanel* g_pPanel;

enum ClientFrameStage_t {
	FRAME_UNDEFINED = -1,			// (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,

	// Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,

	// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,

	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,

	// We're about to start rendering the scene
	FRAME_RENDER_START,

	// We've finished rendering the scene.
	FRAME_RENDER_END
};

enum class ClearFlags_t {
	VIEW_CLEAR_COLOR = 0x1,
	VIEW_CLEAR_DEPTH = 0x2,
	VIEW_CLEAR_FULL_TARGET = 0x4,
	VIEW_NO_DRAW = 0x8,
	VIEW_CLEAR_OBEY_STENCIL = 0x10,
	VIEW_CLEAR_STENCIL = 0x20,
};

enum class MotionBlurMode_t {
	MOTION_BLUR_DISABLE = 1,
	MOTION_BLUR_GAME = 2,
	MOTION_BLUR_SFM = 3
};

class CViewSetup {
public:
	__int32   x;                  //0x0000
	__int32   x_old;              //0x0004
	__int32   y;                  //0x0008
	__int32   y_old;              //0x000C
	__int32   width;              //0x0010
	__int32   width_old;          //0x0014
	__int32   height;             //0x0018
	__int32   height_old;         //0x001C
	char      pad_0x0020[ 0x90 ];   //0x0020
	float     fov;                //0x00B0
	float     viewmodel_fov;      //0x00B4
	Vector    origin;             //0x00B8
	Vector    angles;             //0x00C4
	char      pad_0x00D0[ 0x7C ];   //0x00D0
};//Size=0x014C

class IClientMode {
public:
};
extern IClientMode* g_pClientMode;
/*
virtual             ~IClientMode() {}
virtual int         ClientModeCSNormal(void *) = 0;
virtual void        Init() = 0;
virtual void        InitViewport() = 0;
virtual void        Shutdown() = 0;
virtual void        Enable() = 0;
virtual void        Disable() = 0;
virtual void        Layout() = 0;
virtual IPanel*     GetViewport() = 0;
virtual void*       GetViewportAnimationController() = 0;
virtual void        ProcessInput(bool bActive) = 0;
virtual bool        ShouldDrawDetailObjects() = 0;
virtual bool        ShouldDrawEntity(C_BaseEntity *pEnt) = 0;
virtual bool        ShouldDrawLocalPlayer(C_BaseEntity *pPlayer) = 0;
virtual bool        ShouldDrawParticles() = 0;
virtual bool        ShouldDrawFog(void) = 0;
virtual void        OverrideView(CViewSetup *pSetup) = 0; // 19
virtual int         KeyInput(int down, int keynum, const char *pszCurrentBinding) = 0; // 20
virtual void        StartMessageMode(int iMessageModeType) = 0; //21
virtual IPanel*     GetMessagePanel() = 0; //22
virtual void        OverrideMouseInput(float *x, float *y) = 0; //23
virtual bool        CreateMove(float flSampleFrametime, void* pCmd) = 0; // 24
virtual void        LevelInit(const char *newmap) = 0;
virtual void        LevelShutdown(void) = 0; */