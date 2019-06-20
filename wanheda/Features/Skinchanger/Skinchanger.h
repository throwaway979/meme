#pragma once
#include "../../Utils/GlobalVars.h"
#include "../../SDK/ISurface.h"
#include "../../Utils/Math.h"
#include "../../SDK/IVModelInfo.h"
#include "../../SDK/IClientMode.h"
#include "../../SDK/CGlobalVarsBase.h"
#include "../../SDK/IBaseClientDll.h"
#include "../../Menu/TGFCfg.h"
#include <deque>

class Skinchanger
{
public:
	void KnifeApplyCallbk();
	void ApplyAAAHooks();
	int GetKnifeDefinitionIndex();
	int GetGloveDefinitionIndex( );
	const char* GetKnifeModel(bool viewmodel = true);
	const char* GetGloveModel(bool viewmodel = true);
	void Run();
};

extern Skinchanger g_Skins;