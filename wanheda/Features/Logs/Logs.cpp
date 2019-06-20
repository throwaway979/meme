#include "..\Aimbot\Autowall.h"
#include "..\Aimbot\Aimbot.h"
#include "..\Aimbot\LagComp.h"
#include "..\..\Utils\GlobalVars.h"
#include "..\..\Utils\Math.h"
#include "..\..\SDK\CClientState.h"
#include "..\..\SDK\ICvar.h"
#include "..\..\SDK\CPrediction.h"
#include "..\..\Menu\Menu.h"

#include "Logs.h"

#include <iostream>
#include <algorithm>

// ciudat copil

struct log_thing {
	std::string text;
	Color colour;
	int thing_until_fade_happens;

	log_thing(std::string outside_text, Color outside_colour, int outside_int){
		text = outside_text;
		colour = outside_colour;
		thing_until_fade_happens = outside_int;
	}
};

std::vector<log_thing> logs;

void Logs::DrawLogs( ) {
	if ( !g_pEngine->IsConnected( ) || !g_pEngine->IsInGame( ) )
		return;

	if ( !g_Menu.Config.Logs )
		return;

	int counter = 70;

	for (int i = 0; i < logs.size(); i++) {
		g_pSurface->DrawT(19, counter, logs[i].colour, g::fonts::menu_font, false, logs[i].text.c_str());
		g_pSurface->horizontal_gradient_rect(0, counter + 2, 17, 15, Color(logs[i].colour.red, logs[i].colour.green, logs[i].colour.blue, 0), logs[i].colour);
		if (logs[i].thing_until_fade_happens > 0) logs[i].thing_until_fade_happens--;
		else logs[i].colour.alpha -= 1;
		counter += 15;

		if (logs[i].colour.alpha < 5) logs.erase(logs.begin() + i); 
	}
}

void Logs::OnFireEvent( int entity_index, int damage_dealt, std::string hitgroup, int remaining_health, bool spreadmiss, bool resolvemiss ) {
	C_BaseEntity* entity = g_pEntityList->GetClientEntity( entity_index );
	PlayerInfo_s info;

	static auto set_max = [](char* name) -> std::string {
		name[255] = '\0';

		std::string tmp(name);

		if (tmp.length() > 30) {
			tmp.erase(30, (tmp.length() - 30));
			tmp.append("...");
		}

		return tmp;
	};

	if ( spreadmiss ) {
		//logs.push_back(log_thing("missed shot due to either poor resolve or spread", Color(0, 166, 255, 255), 50));
	}
	else if ( resolvemiss ) {
		//logs.push_back(log_thing("missed shot due to poor resolve", Color(0, 166, 255, 255), 50));
	}

	if (spreadmiss || resolvemiss)
		return;

	if (!g_pEngine->GetPlayerInfo(entity_index, &info))
		return;

	std::string sanitized_name = set_max(info.szName);

	// sanitize name
	std::transform(sanitized_name.begin(), sanitized_name.end(), sanitized_name.begin(), ::tolower);

	if ( remaining_health <= 0 ) {
		logs.push_back(log_thing("killed " + (info.fakeplayer ? "[bot] " + sanitized_name : sanitized_name) + ". shot dealt " + std::to_string(damage_dealt) + " damage. (hit in " + hitgroup + ")", Color(255, 35, 35, 255), 50));
		g_pEngine->ExecuteClientCmd(std::string("\n\necho [wanheda log]: killed " + (info.fakeplayer ? "[bot] " + sanitized_name : sanitized_name) + ". shot dealt " + std::to_string(damage_dealt) + " damage. (hit in " + hitgroup + ")\n").c_str());
	}
	else if ( remaining_health > 0 ) {
		logs.push_back(log_thing("shot " + (info.fakeplayer ? "[bot] " + sanitized_name : sanitized_name) + " for " + std::to_string(damage_dealt) + " damage. (hit in " + hitgroup + ")", Color(0, 166, 255, 255), 50));
		g_pEngine->ExecuteClientCmd(std::string("\n\necho [wanheda log]: shot " + (info.fakeplayer ? "[bot] " + sanitized_name : sanitized_name) + " for " + std::to_string(damage_dealt) + " damage. (hit in " + hitgroup + ")\n").c_str());
	}
}

Logs* g_Logs;