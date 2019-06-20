#include "Skinchanger.h"

Skinchanger g_Skins;
#define SEQUENCE_DEFAULT_DRAW 0
#define SEQUENCE_DEFAULT_IDLE1 1
#define SEQUENCE_DEFAULT_IDLE2 2
#define SEQUENCE_DEFAULT_LIGHT_MISS1 3
#define SEQUENCE_DEFAULT_LIGHT_MISS2 4
#define SEQUENCE_DEFAULT_HEAVY_MISS1 9
#define SEQUENCE_DEFAULT_HEAVY_HIT1 10
#define SEQUENCE_DEFAULT_HEAVY_BACKSTAB 11
#define SEQUENCE_DEFAULT_LOOKAT01 12

#define SEQUENCE_BUTTERFLY_DRAW 0
#define SEQUENCE_BUTTERFLY_DRAW2 1
#define SEQUENCE_BUTTERFLY_LOOKAT01 13
#define SEQUENCE_BUTTERFLY_LOOKAT03 15

#define SEQUENCE_FALCHION_IDLE1 1
#define SEQUENCE_FALCHION_HEAVY_MISS1 8
#define SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP 9
#define SEQUENCE_FALCHION_LOOKAT01 12
#define SEQUENCE_FALCHION_LOOKAT02 13

#define SEQUENCE_DAGGERS_IDLE1 1
#define SEQUENCE_DAGGERS_LIGHT_MISS1 2
#define SEQUENCE_DAGGERS_LIGHT_MISS5 6
#define SEQUENCE_DAGGERS_HEAVY_MISS2 11
#define SEQUENCE_DAGGERS_HEAVY_MISS1 12

#define SEQUENCE_BOWIE_IDLE1 1

RecvVarProxyFn oRecvnModelIndex;
RecvVarProxyFn fnSequenceProxyFn = NULL;
RecvVarProxyFn ovecForce = nullptr;
#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);
void SetViewModelSequence(const CRecvProxyData *pDataConst, void *pStruct, void *pOut) {
	// Make the incoming data editable.
	CRecvProxyData* pData = const_cast<CRecvProxyData*>(pDataConst);

	// Confirm that we are replacing our view model and not someone elses.
	C_BaseViewModel* pViewModel = (C_BaseViewModel*)pStruct;

	if (pViewModel) {
		IClientEntity* pOwner = g_pEntityList->GetClientEntityFromHandle(CBaseHandle(pViewModel->GetOwner()));

		// Compare the owner entity of this view model to the local player entity.
		if (pOwner && pOwner->EntIndex() == g_pEngine->GetLocalPlayer()) {
			// Get the filename of the current view model.
			void* pModel = g_pModelInfo->GetModel(pViewModel->GetModelIndex());

			const char* szModel = g_pModelInfo->GetModelName((model_t*)pModel);

			// Store the current sequence.
			int m_nSequence = pData->m_Value.m_Int;

			if (!strcmp(szModel, "models/weapons/v_knife_butterfly.mdl")) {
				// Fix animations for the Butterfly Knife.
				switch (m_nSequence) {
				case SEQUENCE_DEFAULT_DRAW:
					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2); break;
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03); break;
				default:
					m_nSequence++;
				}
			}
			else if (!strcmp(szModel, "models/weapons/v_knife_falchion_advanced.mdl")) {
				// Fix animations for the Falchion Knife.
				switch (m_nSequence) {
				case SEQUENCE_DEFAULT_IDLE2:
					m_nSequence = SEQUENCE_FALCHION_IDLE1; break;
				case SEQUENCE_DEFAULT_HEAVY_MISS1:
					m_nSequence = RandomInt(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP); break;
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02); break;
				case SEQUENCE_DEFAULT_DRAW:
				case SEQUENCE_DEFAULT_IDLE1:
					break;
				default:
					m_nSequence--;
				}
			}
			else if (!strcmp(szModel, "models/weapons/v_knife_push.mdl")) {
				// Fix animations for the Shadow Daggers.
				switch (m_nSequence) {
				case SEQUENCE_DEFAULT_IDLE2:
					m_nSequence = SEQUENCE_DAGGERS_IDLE1; break;
				case SEQUENCE_DEFAULT_LIGHT_MISS1:
				case SEQUENCE_DEFAULT_LIGHT_MISS2:
					m_nSequence = RandomInt(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5); break;
				case SEQUENCE_DEFAULT_HEAVY_MISS1:
					m_nSequence = RandomInt(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1); break;
				case SEQUENCE_DEFAULT_HEAVY_HIT1:
				case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence += 3; break;
				case SEQUENCE_DEFAULT_DRAW:
				case SEQUENCE_DEFAULT_IDLE1:
					break;
				default:
					m_nSequence += 2;
				}
			}
			else if (!strcmp(szModel, "models/weapons/v_knife_survival_bowie.mdl")) {
				// Fix animations for the Bowie Knife.
				switch (m_nSequence) {
				case SEQUENCE_DEFAULT_DRAW:
				case SEQUENCE_DEFAULT_IDLE1:
					break;
				case SEQUENCE_DEFAULT_IDLE2:
					m_nSequence = SEQUENCE_BOWIE_IDLE1; break;
				default:
					m_nSequence--;
				}
			}
			else if (!strcmp(szModel, "models/weapons/v_knife_ursus.mdl")) {
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_DRAW:
					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
					break;
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
					break;
				default:
					m_nSequence++;
					break;
				}
			}
			else if (!strcmp(szModel, "models/weapons/v_knife_stiletto.mdl")) {
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt(12, 13);
					break;
				}
			}
			else if (!strcmp(szModel, "models/weapons/v_knife_widowmaker.mdl")) {
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt(14, 15);
					break;
				}
			}
			// Set the fixed sequence.
			pData->m_Value.m_Int = m_nSequence;
		}
	}

	// Call original function with the modified data.
	fnSequenceProxyFn(pData, pStruct, pOut);
}

void Hooked_RecvProxy_Viewmodel(CRecvProxyData *pData, void *pStruct, void *pOut)
{
	// Get the knife view model id's
	int default_t = g_pModelInfo->GetModelIndex("models/weapons/v_knife_default_t.mdl");
	int default_ct = g_pModelInfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
	int iBayonet = g_pModelInfo->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
	int iButterfly = g_pModelInfo->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
	int iFlip = g_pModelInfo->GetModelIndex("models/weapons/v_knife_flip.mdl");
	int iGut = g_pModelInfo->GetModelIndex("models/weapons/v_knife_gut.mdl");
	int iKarambit = g_pModelInfo->GetModelIndex("models/weapons/v_knife_karam.mdl");
	int iM9Bayonet = g_pModelInfo->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
	int iHuntsman = g_pModelInfo->GetModelIndex("models/weapons/v_knife_tactical.mdl");
	int iFalchion = g_pModelInfo->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
	int iDagger = g_pModelInfo->GetModelIndex("models/weapons/v_knife_push.mdl");
	int iBowie = g_pModelInfo->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");
	int iGunGame = g_pModelInfo->GetModelIndex("models/weapons/v_knife_gg.mdl");

	// Get local player (just to stop replacing spectators knifes)
	auto pLocal = g::pLocalEntity;
	if (pLocal && g_Menu.Config.KnifeModel > 0)
	{
		// If we are alive and holding a default knife(if we already have a knife don't worry about changing)
		if (pLocal->IsAlive() && (
			pData->m_Value.m_Int == default_t ||
			pData->m_Value.m_Int == default_ct ||
			pData->m_Value.m_Int == iBayonet ||
			pData->m_Value.m_Int == iButterfly ||
			pData->m_Value.m_Int == iFlip ||
			pData->m_Value.m_Int == iGunGame ||
			pData->m_Value.m_Int == iGut ||
			pData->m_Value.m_Int == iKarambit ||
			pData->m_Value.m_Int == iM9Bayonet ||
			pData->m_Value.m_Int == iHuntsman ||
			pData->m_Value.m_Int == iFalchion ||
			pData->m_Value.m_Int == iDagger ||
			pData->m_Value.m_Int == iBowie))
		{
			pData->m_Value.m_Int = g_pModelInfo->GetModelIndex(g_Skins.GetKnifeModel());
		}
	}

	// Carry on to the original proxy
	oRecvnModelIndex(pData, pStruct, pOut);
}

void vec_force(CRecvProxyData* data, void* pStruct, void* out) {
	Vector v;

	v.x = data->m_Value.m_Vector[0];
	v.y = data->m_Value.m_Vector[1];
	v.z = data->m_Value.m_Vector[2];
	if (g_Menu.Config.Ragdoll) {
		Vector pp;
		Vector angles{ -45, g_Math.RandomFloat(-180, 180), 0 };
		g_Math.AngleVectors(angles, &pp, nullptr, nullptr);

		v = pp * 9999999999.0f;
	}
	*(Vector*)out = v;
}

typedef void(*CL_FullUpdate_t) (void);
CL_FullUpdate_t CL_FullUpdate = nullptr;

std::uint8_t* pattern_scan(void* module, const char* signature) {
	static auto pattern_to_byte = [](const char* pattern) {
		auto bytes = std::vector<int>{};
		auto start = const_cast<char*>(pattern);
		auto end = const_cast<char*>(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current) {
			if (*current == '?') {
				++current;
				if (*current == '?')
					++current;
				bytes.push_back(-1);
			}
			else {
				bytes.push_back(strtoul(current, &current, 16));
			}
		}
		return bytes;
	};

	auto dos_headers = (PIMAGE_DOS_HEADER)module;
	auto nt_headers = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dos_headers->e_lfanew);

	auto size_of_image = nt_headers->OptionalHeader.SizeOfImage;
	auto pattern_bytes = pattern_to_byte(signature);
	auto scan_bytes = reinterpret_cast<std::uint8_t*>(module);

	auto s = pattern_bytes.size();
	auto d = pattern_bytes.data();

	for (auto i = 0ul; i < size_of_image - s; ++i) {
		bool found = true;
		for (auto j = 0ul; j < s; ++j) {
			if (scan_bytes[i + j] != d[j] && d[j] != -1) {
				found = false;
				break;
			}
		}
		if (found) {
			return &scan_bytes[i];
		}
	}
	return nullptr;
}

struct hud_weapons_t {
	std::int32_t* get_weapon_count() {
		return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
	}
};
template<class T>
static T* find_hud_element(const char* name) {
	static auto fn = *reinterpret_cast<DWORD**>(pattern_scan(GetModuleHandleA("client_panorama.dll"), ("B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08")) + 1);

	static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(pattern_scan(GetModuleHandleA("client_panorama.dll"), ("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28")));
	return (T*)find_hud_element(fn, name);
}

void Skinchanger::KnifeApplyCallbk()
{
	static auto fn = reinterpret_cast<std::int32_t(__thiscall*)(void*, std::int32_t)>(pattern_scan(GetModuleHandleA("client_panorama.dll"), ("55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 2C")));

	auto element = find_hud_element<std::uintptr_t*>(("CCSGO_HudWeaponSelection"));

	auto hud_weapons = reinterpret_cast<hud_weapons_t*>(std::uintptr_t(element) - 0xA0);
	if (hud_weapons == nullptr)
		return;

	if (!*hud_weapons->get_weapon_count())
		return;

	for (std::int32_t i = 0; i < *hud_weapons->get_weapon_count(); i++)
		i = fn(hud_weapons, i);

	static auto CL_FullUpdate = reinterpret_cast<CL_FullUpdate_t>(Utils::FindPattern("engine.dll", reinterpret_cast<PBYTE>("\xA1\x00\x00\x00\x00\xB9\x00\x00\x00\x00\x56\xFF\x50\x14\x8B\x34\x85"), "x????x????xxxxxxx"));
	CL_FullUpdate();
}

void Skinchanger::ApplyAAAHooks()
{
	ClientClass *pClass = g_pClientDll->GetAllClasses();
	while (pClass)
	{
		const char *pszName = pClass->pRecvTable->pNetTableName;

		if (!strcmp(pszName, "DT_BaseViewModel")) {
			// Search for the 'm_nModelIndex' property.
			RecvTable* pClassTable = pClass->pRecvTable;

			for (int nIndex = 0; nIndex < pClass->pRecvTable->nProps; nIndex++) {
				RecvProp *pProp = &(pClass->pRecvTable->pProps[nIndex]);

				if (!pProp || strcmp(pProp->pVarName, "m_nSequence"))
					continue;

				// Store the original proxy function.
				fnSequenceProxyFn = (RecvVarProxyFn)pProp->ProxyFn;

				// Replace the proxy function with our sequence changer.
				pProp->ProxyFn = SetViewModelSequence;
			}
		}

		if (!strcmp(pszName, "DT_BaseViewModel"))
		{
			for (int i = 0; i < pClass->pRecvTable->nProps; i++)
			{
				RecvProp *pProp = &(pClass->pRecvTable->pProps[i]);
				const char *name = pProp->pVarName;

				// Knives
				if (!strcmp(name, "m_nModelIndex"))
				{
					oRecvnModelIndex = (RecvVarProxyFn)pProp->ProxyFn;
					pProp->ProxyFn = (RecvVarProxyFn)Hooked_RecvProxy_Viewmodel;
				}
			}
		}

		if (!strcmp(pszName, "DT_CSRagdoll")) {
			for (int i = 0; i < pClass->pRecvTable->nProps; i++)
			{
				RecvProp* pProp = &(pClass->pRecvTable->pProps[i]);
				const char* name = pProp->pVarName;
				if (!strcmp(name, "m_vecForce")) {
					ovecForce = (RecvVarProxyFn)pProp->ProxyFn;
					pProp->ProxyFn = (RecvVarProxyFn)vec_force;
				}
			}
		}

		pClass = pClass->pNext;
	}
}

int Skinchanger::GetKnifeDefinitionIndex()
{

	if (!g::pLocalEntity)
		return 59;


	switch (g_Menu.Config.KnifeModel)
	{
	case 0:
		return g::pLocalEntity->GetTeam() == 1 ? 42 : 26;
		break;

	case 1:
		return 508;
		break;

	case 2:
		return 500;
		break;

	case 3:
		return 505;
		break;

	case 4:
		return 506;
		break;

	case 5:
		return 507;
		break;

	case 6:
		return 509;
		break;

	case 7:
		return 512;
		break;

	case 8:
		return 514;
		break;

	case 9:
		return 515;
		break;
	case 10:
		return 516;
		break;
	case 11:
		return 520;
		break;
	case 12:
		return 522;
		break;
	case 13:
		return 519;
		break;
	default:
		return 523;
		break;
	}
}

int Skinchanger::GetGloveDefinitionIndex( )
{

	if ( !g::pLocalEntity )
		return 5028;


	switch ( g_Menu.Config.KnifeModel )
	{
	case 0:
		return g::pLocalEntity->GetTeam( ) == 1 ? 5029 : 5028;
		break;
	case 1:
		return 5030;
		break;
	case 2:
		return 5031;
		break;
	case 3:
		return 5029;
		break;
	case 4:
		return 5033;
		break;
	
	}
}


const char* Skinchanger::GetGloveModel( bool viewmodel )
{
	if ( !g::pLocalEntity )
		return viewmodel ? "models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl" : "models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl";

	switch ( g_Menu.Config.GlovesModel )
	{
	case 0:
		return viewmodel ? g::pLocalEntity->GetTeam( ) == 2 ? "models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl" : "models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl" : g::pLocalEntity->GetTeam( ) == 2 ? "models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl" : "models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl";
		break;
	case 1:
		return viewmodel ? "models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl" : "models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl";
		break;
	case 2:
		return viewmodel ? "models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl" : "models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl";
		break;
	case 3:
		return viewmodel ? "models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl" : "models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl";
		break;
	case 4:
		return viewmodel ? "models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl" : "models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl";
		break;
	}
}

const char* Skinchanger::GetKnifeModel(bool viewmodel)
{

	if (!g::pLocalEntity)
		return "models/weapons/v_knife_default_t.mdl";


	switch (g_Menu.Config.KnifeModel)
	{
	case 0:
		return viewmodel ? g::pLocalEntity->GetTeam() == 2 ? "models/weapons/v_knife_default_t.mdl" : "models/weapons/v_knife_default_ct.mdl" : g::pLocalEntity->GetTeam() == 2 ? "models/weapons/w_knife_default_t.mdl" : "models/weapons/w_knife_default_ct.mdl";
		break;

	case 1:
		return viewmodel ? "models/weapons/v_knife_m9_bay.mdl" : "models/weapons/w_knife_m9_bay.mdl";
		break;

	case 2:
		return viewmodel ? "models/weapons/v_knife_bayonet.mdl" : "models/weapons/w_knife_bayonet.mdl";
		break;

	case 3:
		return viewmodel ? "models/weapons/v_knife_flip.mdl" : "models/weapons/w_knife_flip.mdl";
		break;

	case 4:
		return viewmodel ? "models/weapons/v_knife_gut.mdl" : "models/weapons/w_knife_gut.mdl";
		break;

	case 5:
		return viewmodel ? "models/weapons/v_knife_karam.mdl" : "models/weapons/w_knife_karam.mdl";
		break;

	case 6:
		return viewmodel ? "models/weapons/v_knife_tactical.mdl" : "models/weapons/w_knife_tactical.mdl";
		break;

	case 7:
		return viewmodel ? "models/weapons/v_knife_falchion_advanced.mdl" : "models/weapons/w_knife_falchion_advanced.mdl";
		break;

	case 8:
		return viewmodel ? "models/weapons/v_knife_survival_bowie.mdl" : "models/weapons/w_knife_survival_bowie.mdl";
		break;
	case 9:
		return viewmodel ? "models/weapons/v_knife_butterfly.mdl" : "models/weapons/w_knife_butterfly.mdl";
		break;
	case 10:
		return viewmodel ? "models/weapons/v_knife_push.mdl" : "models/weapons/w_knife_push.mdl";
		break;
	case 11:
		return viewmodel ? "models/weapons/v_knife_gypsy_jackknife.mdl" : "models/weapons/w_knife_gypsy_jackknife.mdl";
		break;
	case 12:
		return viewmodel ? "models/weapons/v_knife_stiletto.mdl" : "models/weapons/w_knife_stiletto.mdl";
		break;
	case 13:
		return viewmodel ? "models/weapons/v_knife_ursus.mdl" : "models/weapons/w_knife_ursus.mdl";
		break;
	case 14:
		return viewmodel ? "models/weapons/v_knife_widowmaker.mdl" : "models/weapons/2_knife_widowmaker.mdl";
		break;
	default:
		return "";
		break;
	}
}

void Skinchanger::Run()
{
	if (!g::pLocalEntity)
		return;

	if (g::pLocalEntity->IsAlive())
	{
		int nLocalPlayerID = g_pEngine->GetLocalPlayer();
		C_BaseEntity* pLocal = (C_BaseEntity*)g_pEntityList->GetClientEntity(nLocalPlayerID);


		auto Weapons = g::pLocalEntity->GetWeapons();

		for (int i = 0; i < 64; i++) {
			if (Weapons[i] == -1)
				continue;

			C_BaseCombatWeapon* Weap = (C_BaseCombatWeapon*)g_pEntityList->GetClientEntityFromHandle(Weapons[i]);

			if (!Weap || Weap == nullptr)
				continue;


			ClientClass *pClass = g_pClientDll->GetAllClasses();

			auto world_model_handle = Weap->m_hWeaponWorldModel_c();
			auto world_model_handle2 = Weap->GetWearables();

			const auto world_model = (CBaseWeaponWorldModel*)(g_pEntityList->GetClientEntityFromHandle(world_model_handle));
			const auto world_model2 = (CBaseWeaponWorldModel*)(g_pEntityList->GetClientEntityFromHandle( world_model_handle2 ));

			if ( Weap->GetClientClass( )->ClassID == ( int )EClassIds::CEconWearable )
			{
				*Weap->ItemDefinitionIndex2( ) = GetGloveDefinitionIndex( );
				Weap->SetModelIndex( g_pModelInfo->GetModelIndex( GetGloveModel( ) ) );

				if ( world_model2 ) {
					*world_model2->GetModelIndex( ) = g_pModelInfo->GetModelIndex( GetGloveModel( false ) );
				}

				Weap->GetItemIDLow( ) = -1;
				Weap->GetItemIDHigh( ) = -1;
				*Weap->GetFallbackPaintKit( ) = 10021;
				*Weap->FallbackWear( ) = 0.00000000001;
			}
			if (Weap->GetClientClass()->ClassID == (int)EClassIds::CKnife && g_Menu.Config.KnifeModel > 0)
			{
				*Weap->ItemDefinitionIndex2() = GetKnifeDefinitionIndex();
				Weap->SetModelIndex(g_pModelInfo->GetModelIndex(GetKnifeModel()));

				if (world_model) {
					*world_model->GetModelIndex() = g_pModelInfo->GetModelIndex(GetKnifeModel(false));
				}

				Weap->GetItemIDLow() = -1;
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.KnifeSkin;
				*Weap->FallbackWear() = 0.00000000001;
			}

			switch (Weap->GetItemDefinitionIndex()) {
			case ItemDefinitionIndex::WEAPON_SSG08: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.Ssg08Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			}break;
			case ItemDefinitionIndex::WEAPON_SCAR20: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.Scar20Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_G3SG1: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.G3sg1Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_AK47: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.Ak47Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_M4A1: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.M4a4Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_M4A1_SILENCER: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.M4a1SSkin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_CZ75A: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.Cz75Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_DEAGLE: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.DeagleSkin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_REVOLVER: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.R8Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_AUG: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.AugSkin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_AWP: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.AwpSkin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_ELITE: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.DualsSkin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_FAMAS: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.FamasSkin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_FIVESEVEN: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.FiveSevenSkin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_GALILAR: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.GalilArSkin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_GLOCK: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.Glock18Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_M249: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.M249Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_MAC10: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.Mac10Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_MAG7: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.Mag7Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_MP5SD: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.Mp5SdSkin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_MP7: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.Mp7Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_MP9: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.Mp9Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_NEGEV: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.NegevSkin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_NOVA: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.NovaSkin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_HKP2000: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.P2000Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_P250: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.P250Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_P90: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.P90Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_BIZON: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.PPBizonSkin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_SG556: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.Sg553Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_SAWEDOFF: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.SawedOffSkin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_TEC9: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.Tec9Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_UMP45: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.Ump45Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_USP_SILENCER: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.UspSkin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			case ItemDefinitionIndex::WEAPON_XM1014: {
				Weap->GetItemIDHigh() = -1;
				*Weap->GetFallbackPaintKit() = g_Menu.Config.Xm1014Skin;
				*Weap->FallbackSeed() = 0;
				*Weap->OwnerXuidLow() = 0;
				*Weap->OwnerXuidHigh() = 0;
				*Weap->FallbackWear() = 0.00000000001;
			} break;
			}

		}
	}
}