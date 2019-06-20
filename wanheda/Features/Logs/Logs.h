#pragma once

class Logs {
public:
	void DrawLogs( );
	void OnFireEvent( int entity_index, int damage_dealt, std::string hitgroup, int remaining_health, bool spreadmiss, bool resolvemiss );
};

extern Logs* g_Logs;