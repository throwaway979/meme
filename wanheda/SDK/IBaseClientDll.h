#pragma once
#include "CGlobalVarsBase.h"
#include "ClientClass.h"

enum cstrike15_user_message_t {

	cs_um_vguimenu = 1,
	cs_um_geiger = 2,
	cs_um_train = 3,
	cs_um_hudtext = 4,
	cs_um_saytext = 5,
	cs_um_saytext2 = 6,
	cs_um_textmsg = 7,
	cs_um_hudmsg = 8,
	cs_um_resethud = 9,
	cs_um_gametitle = 10,
	cs_um_shake = 12,
	cs_um_fade = 13,
	cs_um_rumble = 14,
	cs_um_closecaption = 15,
	cs_um_closecaptiondirect = 16,
	cs_um_sendaudio = 17,
	cs_um_rawaudio = 18,
	cs_um_voicemask = 19,
	cs_um_requeststate = 20,
	cs_um_damage = 21,
	cs_um_radiotext = 22,
	cs_um_hinttext = 23,
	cs_um_keyhinttext = 24,
	cs_um_processspottedentityupdate = 25,
	cs_um_reloadeffect = 26,
	cs_um_adjustmoney = 27,
	cs_um_updateteammoney = 28,
	cs_um_stopspectatormode = 29,
	cs_um_killcam = 30,
	cs_um_desiredtimescale = 31,
	cs_um_currenttimescale = 32,
	cs_um_achievementevent = 33,
	cs_um_matchendconditions = 34,
	cs_um_disconnecttolobby = 35,
	cs_um_playerstatsupdate = 36,
	cs_um_displayinventory = 37,
	cs_um_warmuphasended = 38,
	cs_um_clientinfo = 39,
	cs_um_xrankget = 40,
	cs_um_xrankupd = 41,
	cs_um_callvotefailed = 45,
	cs_um_votestart = 46,
	cs_um_votepass = 47,
	cs_um_votefailed = 48,
	cs_um_votesetup = 49,
	cs_um_serverrankrevealall = 50,
	cs_um_sendlastkillerdamagetoclient = 51,
	cs_um_serverrankupdate = 52,
	cs_um_itempickup = 53,
	cs_um_showmenu = 54,
	cs_um_bartime = 55,
	cs_um_ammodenied = 56,
	cs_um_markachievement = 57,
	cs_um_matchstatsupdate = 58,
	cs_um_itemdrop = 59,
	cs_um_glowpropturnoff = 60,
	cs_um_sendplayeritemdrops = 61
};

class IBaseClientDLL {
public:

	// Connect appsystem components, get global interfaces, don't run any other init code
	virtual int              Connect( CreateInterfaceFn appSystemFactory, CGlobalVarsBase* pGlobals ) = 0;
	virtual int              Disconnect( void ) = 0;
	virtual int              Init( CreateInterfaceFn appSystemFactory, CGlobalVarsBase* pGlobals ) = 0;
	virtual void             PostInit( ) = 0;
	virtual void             Shutdown( void ) = 0;
	virtual void             LevelInitPreEntity( char const* pMapName ) = 0;
	virtual void             LevelInitPostEntity( ) = 0;
	virtual void             LevelShutdown( void ) = 0;
	virtual ClientClass* GetAllClasses( void ) = 0;


	bool DispatchUserMessage( int msg_type, unsigned int arg1, unsigned int length, const void* data = nullptr ) {
		using original_fn = bool( __thiscall* )( void*, int, unsigned int, unsigned int, const void* );
		return ( *( original_fn * * )this )[ 38 ]( this, msg_type, arg1, length, data );
	}
};
extern IBaseClientDLL* g_pClientDll;