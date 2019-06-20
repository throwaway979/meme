#include "../../SDK/Vector.h"
#include "../../SDK/IGameEvent.h"
#include "../../SDK/IVEngineClient.h"
#include "../../SDK/IClientEntityList.h"
#include "../../SDK/CGlobalVarsBase.h"
#include "../../SDK/IVDebugOverlay.h"
#include "../../SDK/CEntity.h"
#include "../../Utils/GlobalVars.h"
#include "../../Utils/Interfaces.h"
#include "../../Utils/Color.h"
#include "../../Menu/menu.h"
#include <vector>

class bullettracers {
private:
	class trace_info {
	public:
		trace_info( Vector starts, Vector positions, float times ) {
			this->start = starts;
			this->position = positions;
			this->time = times;
		}

		Vector position;
		Vector start;
		float time;
	};

	std::vector<trace_info> logs;

	void draw_beam( Vector src, Vector end, Color color );
public:
	void events( IGameEvent* event );
};

extern bullettracers g_pBulletTracers;