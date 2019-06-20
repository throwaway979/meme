#include "Menu.h"
#include "TGFCfg.h"
#include "..\SDK\ISurface.h"
#include "..\SDK\CClientState.h"
#include "..\Utils\Color.h"
#include "..\Utils\GlobalVars.h"
#include "..\Utils\XorStr.h"
#include "..\SDK\IInputSystem.h"

// chestii todo
// fix antiaim when just pulling out gun it doesnt work it just looks at that nigga aaaaaaaaaah // asta este pentru ca iti da pull canshoot si cand tragi nu ai AA, se poate vedea cand e gata wm dar tu trebuia sa dai in cineva

std::string hitchances[ ] = { "off", "Complex", "Simple" };
std::string desync[ ] = { "Static", "Shuffle" };
std::string pitch_aas[ ] = { "Off", "Down", "Up", "Zero", "Fake Down", "Mixed" };
std::string config_nigga[ ] = { "Config 1", "Config 2", "Config 3", "Config 4", "Config 5", "Config 6" };
std::string yaw_aas[ ] = { "Viewangle", "Back", "Freestanding", "Key-Based" };
std::string watermarks[ ] = { "Simple", "Fancy" };
std::string material_mode[ ] = { "Normal", "Pulse" };
std::string materials[ ] = { "Flat", "Regular", "Metallic" };
std::string glow_styles[ ] = { "Regular", "Pulse", "Stencil", "Stencil Pulse" };
std::string legitHitbox[ ] = { "Head", "Neck", "Upper Chest", "Left Hand", "Right Hand", "Pelvis" };
std::string gloves_model[ ] = {
	"default",
	"idk1",
	"idk2",
	"idk3",
	"idk4"
};

std::string knife_options[] = {
"Default",
"M9 Bayonet",
"Bayonet",
"Flip",
"Gut",
"Karambit",
"Huntsman",
"Falchion",
"Bowie",
"Butterfly",
"Shadow Daggers",
"Navaja",
"Stiletto",
"Ursus",
"Talon"
};

std::string weapon_options[ ] = {
"Knife",
"AK-47",
"AUG",
"AWP",
"CZ75-Auto",
"Desert Eagle",
"Dual Berettas",
"FAMAS",
"Five-SeveN",
"G3SG1",
"Galil AR",
"Glock-18",
"M249",
"M4A1",
"M4A4",
"MAC-10",
"MAG-7",
"MP5-SD",
"MP7",
"MP9",
"Negev",
"Nova",
"P2000",
"P250",
"P90",
"PP-Bizon",
"R8 Revolver",
"SCAR-20",
"SG 553",
"SSG 08",
"Sawed-Off",
"Tec-9",
"UMP-45",
"USP",
"XM1014"
};
c_menu g_Menu;

LPCWSTR covert_char_array_to_lpcwstr( const char* char_array ) {
	wchar_t* w_string = new wchar_t[ 4096 ];
	MultiByteToWideChar( CP_ACP, 0, char_array, -1, w_string, 4096 );
	return w_string;
}

Vector2D c_menu::mouse_pos( ) {
	POINT mouse_position;
	GetCursorPos( &mouse_position );
	ScreenToClient( FindWindow( 0, ( "Counter-Strike: Global Offensive" ) ), &mouse_position );
	return { static_cast < float > ( mouse_position.x ), static_cast < float > ( mouse_position.y ) };
}

bool c_menu::mouse_in_params( int x, int y, int x2, int y2 ) {
	if ( mouse_pos( ).x > x && mouse_pos( ).y > y && mouse_pos( ).x < x2 + x && mouse_pos( ).y < y2 + y )
		return true;
	return false;
}

enum tabs {
	legit = 0,
	rage = 1,
	visuals = 2,
	misc = 3,
	skins = 4,
	customization = 5
};

bool save_config = false, load_config = false, full_update = false, force_apply = false;
int configs = 0;

void c_menu::draw_hitgroups( )
{
	static int _drag_x = 300;
	static int _drag_y = 300;
	static int _width = 300;
	static int _height = 340;

	static bool _dragging = false;
	bool _click = false;
	static bool _resizing = false;

	if ( GetAsyncKeyState( VK_LBUTTON ) )
		_click = true;

	Vector2D _mouse_pos = mouse_pos( );

	if ( _dragging && !_click )
		_dragging = false;

	if ( _resizing && !_click )
		_resizing = false;

	if ( _dragging && _click )
	{
		_pos_radar.x = _mouse_pos.x - _drag_x;
		_pos_radar.y = _mouse_pos.y - _drag_y;
	}

	if ( mouse_in_params( _pos_radar.x, _pos_radar.y, _width, _height ) && !( mouse_in_params( _pos_radar.x + 20, _pos_radar.y + 20, _width - 10, _height - 10 ) ) )
	{
		_dragging = true;
		_drag_x = _mouse_pos.x - _pos_radar.x;
		_drag_y = _mouse_pos.y - _pos_radar.y;
	}

	g_pSurface->FilledRect( _pos_radar.x, _pos_radar.y, _width, _height, Color( 62, 62, 62, 145 ) );
	g_pSurface->OutlinedRect( _pos_radar.x, _pos_radar.y, _width, _height, Color( 32, 32, 32, 145 / 2 ) );
	g_pSurface->FilledRect( _pos_radar.x + 10, _pos_radar.y + 20, _width - 20, _height - 30, Color( 42, 42, 42, 255 ) );
	g_pSurface->OutlinedRect( _pos_radar.x + 9, _pos_radar.y + 19, _width - 18, _height - 28, Color( 32, 32, 32, 255 / 2 ) );

	g_pSurface->OutlinedRect( _pos_radar.x + ( _width / 2 ) - 35, _pos_radar.y + 35, 70, 60, Color( 255, 255, 255 ) );
	g_pSurface->OutlinedRect( _pos_radar.x + ( _width / 2 ) - 35 / 2, _pos_radar.y + 35 + 1 + 60, 70 / 2, 40, Color( 255, 255, 255 ) );
	g_pSurface->OutlinedRect( _pos_radar.x + ( _width / 2 ) - 50, _pos_radar.y + 35 + 1 + 60 + 40 + 1, 100, 80, Color( 255, 255, 255 ) );
	g_pSurface->OutlinedRect( _pos_radar.x + ( _width / 2 ) - 91, _pos_radar.y + 35 + 1 + 60 + 40 + 1, 40, 60, Color( 255, 255, 255 ) );
	g_pSurface->OutlinedRect( _pos_radar.x + ( _width / 2 ) + 51, _pos_radar.y + 35 + 1 + 60 + 40 + 1, 40, 60, Color( 255, 255, 255 ) );
	g_pSurface->OutlinedRect( _pos_radar.x + ( _width / 2 ) - 50, _pos_radar.y + 35 + 1 + 60 + 40 + 1 + 81, 40, 80, Color( 255, 255, 255 ) );
	g_pSurface->OutlinedRect( _pos_radar.x + ( _width / 2 ) + 101 - 91, _pos_radar.y + 35 + 1 + 60 + 40 + 1 + 81, 40, 80, Color( 255, 255, 255 ) );
	g_pSurface->DrawT( _pos_radar.x + ( _width / 2 ), _pos_radar.y + 1, Color( 255, 255, 255 ), g::fonts::menu_font, true, "Hitgroups" );
}

void c_menu::draw_radar( )
{
	static int _drag_x = 300;
	static int _drag_y = 300;
	static int _width = 300;
	static int _height = 320;

	static bool _dragging = false;
	bool _click = false;
	static bool _resizing = false;

	if ( GetAsyncKeyState( VK_LBUTTON ) )
		_click = true;

	Vector2D _mouse_pos = mouse_pos( );

	if ( _dragging && !_click )
		_dragging = false;

	if ( _resizing && !_click )
		_resizing = false;

	if ( _dragging && _click )
	{
		_pos_radar.x = _mouse_pos.x - _drag_x;
		_pos_radar.y = _mouse_pos.y - _drag_y;
	}

	if ( mouse_in_params( _pos_radar.x, _pos_radar.y, _width, _height ) && !( mouse_in_params( _pos_radar.x + 20, _pos_radar.y + 20, _width - 10, _height - 10 ) ) )
	{
		_dragging = true;
		_drag_x = _mouse_pos.x - _pos_radar.x;
		_drag_y = _mouse_pos.y - _pos_radar.y;
	}

	g_pSurface->FilledRect( _pos_radar.x, _pos_radar.y, _width, _height, Color( 62, 62, 62, 145 ) );
	g_pSurface->OutlinedRect( _pos_radar.x, _pos_radar.y, _width, _height, Color( 32, 32, 32, 145 / 2 ) );
	g_pSurface->FilledRect( _pos_radar.x + 10, _pos_radar.y + 20, _width - 20, _height - 30, Color( 42, 42, 42, 255 ) );
	g_pSurface->OutlinedRect( _pos_radar.x + 9, _pos_radar.y + 19, _width - 18, _height - 28, Color( 32, 32, 32, 255 / 2 ) );

	g_pSurface->DrawT( _pos_radar.x + ( _width / 2 ), _pos_radar.y + 1, Color( 255, 255, 255 ), g::fonts::menu_font, true, "Radar" );

	g_pSurface->Line(_pos_radar.x + 10, _pos_radar.y + 20 + (_width / 2), _pos_radar.x + _width - 20, _pos_radar.y + 20 + (_width / 2), Color(255, 255, 255));
}

struct box_stuff {
	int left, top, right, bottom;

	box_stuff(int _l, int _t, int _r, int _b) {
		left = _l; top = _t; right = _r; bottom = _b;
	}
};

void c_menu::draw_esp_preview( int x_pos, int y_pos )
{
	static int _width = 270;
	static int _height = 360;

	bool _click = false;

	if ( GetAsyncKeyState( VK_LBUTTON ) )
		_click = true;

	Vector2D _mouse_pos = mouse_pos( );

	_pos_esp_preview.x = x_pos;
	_pos_esp_preview.y = y_pos;

	g_pSurface->FilledRect(_pos_esp_preview.x, _pos_esp_preview.y, _width, _height, Color( 62, 62, 62, 145 ) );
	g_pSurface->OutlinedRect(_pos_esp_preview.x, _pos_esp_preview.y, _width, _height, Color( 32, 32, 32, 145 / 2 ) );
	g_pSurface->FilledRect(_pos_esp_preview.x + 10, _pos_esp_preview.y + 20, _width - 20, _height - 30, Color( 42, 42, 42, 255 ) );
	g_pSurface->OutlinedRect(_pos_esp_preview.x + 9, _pos_esp_preview.y + 19, _width - 18, _height - 28, Color( 32, 32, 32, 255 / 2 ) );

	g_pSurface->DrawT(_pos_esp_preview.x + ( _width / 2 ), _pos_esp_preview.y + 1, Color( 255, 255, 255 ), g::fonts::menu_font, true, "ESP Preview" );

	box_stuff box(_pos_esp_preview.x + (_width / 2) - 75, _pos_esp_preview.y + (_height / 2) - 120, 150, 250);

	// let's get to drawing shit.
	if (g_Menu.Config.Box) {
		g_pSurface->OutlinedRect(box.left, box.top, box.right, box.bottom, g_Menu.Config.box_color);
		g_pSurface->OutlinedRect(box.left + 1, box.top + 1, box.right - 2, box.bottom - 2, Color(g_Menu.Config.box_color.red, g_Menu.Config.box_color.green, g_Menu.Config.box_color.blue, 40));
		g_pSurface->OutlinedRect(box.left - 1, box.top - 1, box.right + 2, box.bottom + 2, Color(g_Menu.Config.box_color.red, g_Menu.Config.box_color.green, g_Menu.Config.box_color.blue, 40));
	}

	if (g_Menu.Config.Name) {
		if (g_Menu.Config.Money)
			g_pSurface->DrawT(box.left + (box.right / 2), box.top - 26, g_Menu.Config.EspColor, g::fonts::esp_font, true, "Name");
		else
			g_pSurface->DrawT(box.left + (box.right / 2), box.top - 15, g_Menu.Config.EspColor, g::fonts::esp_font, true, "Name");
	}

	static std::string money = "$16420";
	
	if (g_Menu.Config.Money)
		g_pSurface->DrawT(box.left + (box.right / 2), box.top - 15, g_Menu.Config.EspColor, g::fonts::esp_font, true, money.c_str());

	if ( g_Menu.Config.Weapon ) {
		g_pSurface->DrawT( box.left + ( box.right / 2 ), box.top + box.bottom, g_Menu.Config.EspColor, g::fonts::esp_font, true, "Weapon" );
		g_pSurface->DrawT( box.left + ( box.right / 2 ), box.top + box.bottom + 11, g_Menu.Config.EspColor, g::fonts::weapon_icons_font, true, "J" );
	}
	if (g_Menu.Config.HealthBar)
	{
		static int pixelValue = 100 * box.bottom / 100;
		g_pSurface->FilledRect(box.left - 5, box.top + box.bottom - pixelValue, 2, pixelValue, Color(0, 255, 0, 220));
	}

	if (g_Menu.Config.AmmoBar)
	{
		static int pixelValue = 50;
		g_pSurface->FilledRect(box.left - 5 - 5, box.top + box.bottom - pixelValue, 2, pixelValue, Color(65, 105, 225, 220));
	}

}

void c_menu::draw( ) {
	if ( GetAsyncKeyState( VK_INSERT ) & 1 ) {
		menu_opened = !menu_opened;

		// do interface shit
		g_pInputSystem->enable_input( !menu_opened );
	}

	g_pEngine->GetScreenSize( screen_width, screen_width );

	static Vector2D _mouse_pos;

	static int _drag_x = 5;
	static int _drag_y = 5;
	static int _width = 860; //860
	static int _height = 615; //525

	static bool _dragging = false;
	bool _click = false;

	// TODO: button for this, ez af to do ok?
	if ( save_config ) {
		g_Config->Save( Config.select_this_nigga_config );
		save_config = false;
	}
	if ( load_config ) {
		g_Config->Load( Config.select_this_nigga_config );
		load_config = false;
	}
	if ( full_update ) {
		g_pClientState->ForceFullUpdate( );
		full_update = false;
	}
	if (force_apply) {
		Config.ForceApply = true;
		force_apply = false;
	}
	else
		Config.ForceApply = false;

	update_input( );

	if ( menu_opened ) {
		if ( GetAsyncKeyState( VK_LBUTTON ) )
			_click = true;

		Vector2D _mouse_pos = mouse_pos( );

		if ( _dragging && !_click )

			_dragging = false;

		if ( _dragging && _click ) {
			_pos.x = _mouse_pos.x - _drag_x;
			_pos.y = _mouse_pos.y - _drag_y;
		}

		if ( mouse_in_params( _pos.x, _pos.y, _width, _height ) && !( mouse_in_params( _pos.x + 20, _pos.y + 20, _width - 10, _height - 10 ) ) ) {
			_dragging = true;
			_drag_x = _mouse_pos.x - _pos.x;
			_drag_y = _mouse_pos.y - _pos.y;
		}

		std::string final_tooltip;
		final_tooltip = this->tooltip;

		int in_sizew = _width - 20;

		static int tab_selected = 0;
		static const int tab_amount = 6;
		static std::string tab_names[ tab_amount ] = { "Legitbot", "Ragebot", "Visuals", "Miscelleanous", "Skins", "Customization" };

		g_pSurface->FilledRect( _pos.x, _pos.y, _width, _height, Color( 62, 62, 62, 145 ) );
		g_pSurface->OutlinedRect( _pos.x, _pos.y, _width, _height, Color( 32, 32, 32, 145 / 2 ) );
		g_pSurface->FilledRect( _pos.x + 10, _pos.y + 20, _width - 20, _height - 30, Color( 42, 42, 42, 255 ) );
		g_pSurface->FilledRect( _pos.x + 10, _pos.y + 20, _width - 20, 25, Color( 32, 32, 32, 255 ) );
		g_pSurface->OutlinedRect( _pos.x + 9, _pos.y + 19, _width - 18, _height - 28, Color( 32, 32, 32, 255 / 2 ) );
		g_pSurface->DrawT( _pos.x + ( _width / 2 ), _pos.y + 1, Color( 255, 255, 255 ), g::fonts::menu_font, true, "Wanheda Self-Leak - Sorry" );
		for ( int i = 0; i < tab_amount; i++ )
		{
			RECT text_size = g_pSurface->GetTextSizeRect( g::fonts::menu_font, tab_names[ i ].c_str( ) );

			int tab_area[ 4 ] = { _pos.x + i * ( in_sizew / tab_amount ) + 10, _pos.y + 20,  ( in_sizew / tab_amount ), 30 };

			if ( GetAsyncKeyState( VK_LBUTTON ) && mouse_in_params( tab_area[ 0 ], tab_area[ 1 ], tab_area[ 2 ], tab_area[ 3 ] ) )
				tab_selected = i;

			if ( tab_selected == i ) {
				g_pSurface->DrawT( tab_area[ 0 ] + ( tab_area[ 2 ] / 2 ), tab_area[ 1 ] + ( tab_area[ 3 ] / 2 ) - ( text_size.bottom / 2 ) - 2, Color( 255, 255, 255, 255 ), g::fonts::menu_font, true, tab_names[ i ].c_str( ) );
				g_pSurface->vertical_gradient_rect( tab_area[ 0 ], tab_area[ 1 ] + 25, tab_area[ 2 ], 5, Color( 0, 0, 0, 255 ), Color( 42, 42, 42 ) );
				g_pSurface->FilledRect( tab_area[ 0 ], tab_area[ 1 ] + 25, tab_area[ 2 ], 5, Color( 52, 52, 52 ) );
			}
			else {
				g_pSurface->DrawT( tab_area[ 0 ] + ( tab_area[ 2 ] / 2 ), tab_area[ 1 ] + ( tab_area[ 3 ] / 2 ) - ( text_size.bottom / 2 ) - 2, Color( 215, 215, 215, 255 ), g::fonts::menu_font, true, tab_names[ i ].c_str( ) );
			}
		}

		bool shit;

		int c_r = 255;
		int c_g = 0;
		int c_b = 255;
		int c_a = 255;

		switch ( tab_selected ) {
		case legit:
		groupbox( 20, 60, ( _width / 2 ) - 40, _height - 80, "WH9DFQ8UFE98GHWYEG3" );
		checkbox( "Toggle Legitbot", &Config.legitbotEnable, "" );
		checkbox( "On Key", &Config.legitbotOnKey, "" );
		keybind( Config.legitbot__Key, "legitbot_key_hahah_lol_edgy" );
		checkbox( "Friendly Fire", &Config.FriendlyFireLegit, "make the legit aimbot also target teammates" );
		checkbox("Silent Aim", &Config.LegitSilentAim, "");
		slider( 100, "Legit FoV", &Config.legitbotFov, "%", 0.1f );
		slider( 100, "Legit Smoothness", &Config.legitbotSmooth, "%", 0.1f );
		checkbox( "Legit Recoil Control System", &Config.legitRecoilControlSystem, "" );
		if (Config.legitRecoilControlSystem) {

			slider(100, "RCS X Factor", &Config.LegitbotRCSX, "%", 1);
			slider(100, "RCS Y Factor", &Config.LegitbotRCSY, "%", 1);
		}
		checkbox( "Legit Cubic Interpolation", &Config.legitbotCubicInterpolation, "" );
		combobox( 6, "Aim Hitbox ", legitHitbox, &Config.legitHitbox );
		checkbox( "Aim at Nearest Hitbox", &Config.legitbotNearestHitbox, "" );
		checkbox( "Auto Pistol", &Config.AutoPistol, "" );

		groupbox( 20 + ( _width / 2 ), 60, ( _width / 2 ) - 40, _height - 80, "autist" );
		checkbox("Legit Backtrack", &Config.LegitBacktrack, "");

		break;
		case rage:
		groupbox( 20, 60, ( _width / 2 ) - 40, _height - 80, "WH9DFQ8UFE98GHWYEG4" );
		checkbox( "Toggle Ragebot (BETA)", &Config.Aimbot, "toggle the rage aimbot" );
		slider( 100, "Minimum Damage", &Config.Mindmg, "%", 1 );
		checkbox("Friendly Fire", &Config.FriendlyFire, "make the rage aimbot also target teammates");
		checkbox( "Auto Stop", &Config.AutoStop, "automatically cease all movement when the aimbot is about to shoot, improving accuracy" );
		checkbox( "Auto Scope", &Config.AutoScope, "automatically scope in if you're not scoped when the aimbot is about to shoot, improving accuracy" );
		checkbox( "Auto Revolver", &Config.AutoRevolver, "automatically cock the revolver" );
		slider( 256, "Hitchance Seeds", &Config.Hitchance, "", 1 );
		slider( 100, "Hitchance Value", &Config.HitchanceValue, "%", 1 );
		checkbox( "Resolver", &Config.Resolver, "predict the correct angle of enemies using desync" );
		//checkbox( "Override Resolver", &Config.ResolverOverride, "allow yourself to manually correct the enemy's angle" );
		//keybind( Config.override_resolver_key, "override_resolver_nigga" );
		checkbox( "Toggle Antiaims", &Config.Antiaim, "toggle antiaims on" );
		checkbox( "Desync", &Config.DesyncAngle, "fake your rotation to other clients" );
		combobox( 6, "Pitch Antiaim", pitch_aas, &Config.pitch_aa );
		combobox( 4, "Yaw Antiaim", yaw_aas, &Config.yaw_aa );
		checkbox("Jitter", &Config.RandJitterInRange, "rapidly switch position between a certain range");
		if (Config.RandJitterInRange)
			slider(360, "Range", &Config.JitterRange, "%", 1);

		groupbox( 20 + ( _width / 2 ), 60, ( _width / 2 ) - 40, _height - 80, "WH9DFQ8UFE98GHWYEG4" );

		checkbox( "Multipoint", &Config.MultiPoint, "make the aimbot scan more points of the hitscanned hitboxes" );
		if ( Config.MultiPoint )
		{
			slider( 100, "Head Scale", &Config.HeadScale, "%", 1 );
			slider( 100, "Body Scale", &Config.BodyScale, "%", 1 );
		}
		checkbox( "Delay Shot", &Config.DelayShot, "wait for an accurate moment to shoot when the enemy is fakelagging" );
		checkbox( "Override Hitbox", &Config.HeadOnly, "force the aimbot to only aim at the head" );
		if ( Config.HeadOnly ) {
			checkbox( "Override Hitbox Only On Key", &Config.HeadOnlyOnlyOnKey, "force the aimbot to aim at the head only while holding a selected key" );
			keybind( Config.HeadOnlyOnlyOnKey_key, "pulameaoverrideuratule" );
		}
		checkbox( "Ignore Limbs While Enemy Walking", &Config.IgnoreLimbs, "make the aimbot ignore the arms/legs of the enemy while he is walking");
		checkbox( "Exploit Lag Compensation", &Config.PosBacktrack, "allow yourself to shoot at the past position of your target (200ms) and still have your shot registered" );
		if (g_Menu.Config.PosBacktrack)
			checkbox( "Prioriotise Shot Ticks", &Config.ShotBacktrack, "shoots at and prioriotises backtrack ticks in which the enemy has shot" );
		checkbox( "Baim If Lethal", &Config.BaimLethal, "bodyaim the enemy whenever it's lethal" );
		checkbox( "Baim In Air", &Config.BaimInAir, "bodyaim whenever the enemy is in the air" );
		checkbox( "Baim On Key", &Config.BaimOnKey, "bodyaim whenever you are holding a key" );
		keybind(Config.BaimOnKeyKey, "suprememememme");
		slider(15, "Baim After X Shots Missed", &Config.BaimAfterShots, "", 1);

		checkbox( "Slow Walk", &Config.SlowWalkHAH, "walk slowly to maintain a high accuracy and make it harder for enemies to hit you" );
		keybind( Config.SlowWalkHAH_Key, "dudewtfweed" );
		slider(Config.MaxProcessTicks > 0 ? Config.MaxProcessTicks + 1 : 62, "Fakelag", &Config.Fakelag, "", 1 );
		if ( Config.Fakelag ) {
			checkbox( "Fakelag While Shooting", &Config.FakelagWhileShooting, "fakelag even while shooting with the downside of delaying your shots" );
		}
		slider(57, "sv_maxusrcmdprocessticks", &Config.MaxProcessTicks, "", 1);
		checkbox("Fake Duck", &Config.FakeDuckHAH, "aim higher than you're normally able to while being crouched on the server");
		keybind(Config.FakeDuckHAH_Key, "fuckinghersdfjhkasd");
		break;
		case visuals:
			
		groupbox( 20 + ( _width / 2 ), 60, ( _width / 2 ) - 40, _height - 80, "WH9DFQ8UFE98GHWYEG6" );

		checkbox( "No Zoom", &Config.NoZoom, "remove the zoom effect" );
		checkbox( "No Scope", &Config.NoScope, "remove the scope effect" );
		checkbox( "No Recoil", &Config.NoRecoil, "remove the recoil effect" );
		checkbox( "No Smoke", &Config.NoSmoke, "remove the smoke texture" );
		checkbox( "No Flashbang", &Config.NoFlash, "remove the flashbang effect" );
		checkbox( "No Post Processing", &Config.DisablePostProcessing, "remove the post processing effect" );
		checkbox( "Force Ragdoll Gravity", &Config.Ragdoll, "change ragdoll force amount to a huge one" );

		//checkbox( "crosshair", &Config.Crosshair, " " );
		checkbox( "Nightmode", &Config.Nightmode, "dim the world to a darker color and change the sky texture" );
		color_selector( "sky colour", Config.Skybox ); 
		slider( 100, "Darkness Value", &Config.Nightval, "%", 1 );
		checkbox( "Render Crosshair", &Config.Crosshair, "render a crosshair" );
		checkbox("Recoil Crosshair", &Config.RecoilCrosshair, "render a recoil crosshair");
		slider( 150, "Fov", &Config.Fov, "%", 1 );
		slider( 180, "Viewmodel Fov", &Config.ViewmodelFov, "%", 1 );

		//checkbox( "Antiaim Arrows", &Config.Arrows, "render arrows to show the location of your real and fake angle positions" );

		checkbox( "Thirdperson", &Config.Thirdperson_NIGGA, "switch camera perspective" );
		keybind( Config.Thirdperson_NIGGA_Key, "yesssss" );
		checkbox("Indicators", &Config.Indicators, "show useful information on the bottom left of your screen");

		checkbox( "Grenade Prediction", &Config.GrenadePrediction, "" );
		color_selector( "Grenade Prediction Colour", Config.GrenadePredictionColor );

		groupbox(20, 60, (_width / 2) - 40, _height - 80, "WH9DFQ8UFE98GHWYEG5"); {

			checkbox("Toggle Visuals", &Config.Esp, "toggle visual features");
			color_selector("esp colour", Config.EspColor);
			checkbox("Render Boxes", &Config.Box, "render boxes around enemies");
			color_selector("box colour", Config.box_color);
			checkbox("Render Health Bar", &Config.HealthBar, "render a bar containing the amount of the enemy's health next to his model");
			checkbox("Render Ammo Bar", &Config.AmmoBar, "render the a bar containing the amount of the enemy's ammo under his model");
			checkbox("Render Names", &Config.Name, "render the enemy's name over his model");
			checkbox("Render Money", &Config.Money, "render the enemy's money next to his model");
			checkbox("Render Weapons", &Config.Weapon, "render the enemy's weapon under his model");
			checkbox( "Render Arrows", &Config.RenderArrows, "" );
			color_selector( "Arrows Colour", Config.ArrowsColor );
			checkbox("Render Hitmarker", &Config.Hitmarker, "render a marker on the screen every time you hit an enemy");
			//	color_selector("crosshair", &Config.Hitcolor);
			checkbox("Hitmarker Sound", &Config.HitmarkerSound, "play a sound every time you hit an enemy ");
			checkbox("Enemy Glow", &Config.Glow, "render glow on the enemy's model");
			color_selector("glow colour", Config.GlowColor);
			combobox(4, "Enemy Glow Style", glow_styles, &Config.GlowStyle);
			checkbox("Local Glow", &Config.LocalGlow, "render glow on the local player's model");
			color_selector("local glow colour", Config.LocalGlowColor);
			combobox(4, "Local Glow Style", glow_styles, &Config.LocalGlowStyle);

			//	checkbox("render local bullet tracers", &Config.BulletTracers_Local, "draws a line which shows how the bullet travelled after a local shot");
			//	color_selector("local tracer colour", &Config.BulletTracers_Local_Color); 
			//	checkbox("render enemy bullet tracers", &Config.BulletTracers_Enemy, "draws a line which shows how the bullet travelled after an enemy's shot");
			//	color_selector("enemy tracer colour", &Config.BulletTracers_Enemy_Color);
			//	checkbox("render team bullet tracers", &Config.BulletTracers_Team, "draws a line which shows how the bullet travelled after a teammate's shot");
			//	color_selector("team tracer colour", &Config.BulletTracers_Team_Color);

			//checkbox( "Render Skeleton", &Config.Skeleton_Nigga, "render the enemy's skeleton" );
			//		color_selector( "skeleton colour", &Config.SkeletonColour ); 
			//checkbox( "Render Backtrack Skeleton", &Config.Skeleton_Nigga_Backtrack, "render the enemy's backtracked skeleton" );

			checkbox("Render Visible Chams", &Config.bChams, "");
			color_selector("vis chams colour", Config.VisChams);
			checkbox("Render Invisible Chams", &Config.bChamsZ, "");
			color_selector("invis chams colour", Config.InVisChams);

			//checkbox( "Render Chams On Local Player", &Config.LocalChams, " " ); //bug
			//checkbox( "Render Real Angle Chams On Local Player", &Config.bChamsDesync, " " );
			//	color_selector( "desync chams colour", &Config.DesyncChams ); 
			//slider( 5, "visuals tightness", &Config.BoxWidth, "%", 1 );

			//ColorPicker("chams color", Config.ChamsColor);

			//ColorPicker("box color", Config.BoxColor);
			//MultiComboBox("draw skeleton", { "normal", "backtrack" }, Config.Skeleton);
			//ColorPicker("skele color", Config.SkeletonColor);
		}

		draw_esp_preview( _pos.x + _width + 2, _pos.y );

		break;
		case misc:
		groupbox( 20, 60, ( _width / 2 ) - 40, _height - 80, "WH9DFQ8UFE98GHWYEG7" );
		checkbox( "Bunny Hop", &Config.Bhop, "continuously jump while holding space" );
		//checkbox( "wasd movement", &Config.WASDMovement, "strafes for you automatically" );
		checkbox( "Auto Strafe", &Config.AutoStrafe, "strafe automatically" );
		checkbox( "Fast Duck", &Config.InfinityDuck, "remove ducking/crouching delay" );
		checkbox( "Clantag Changer", &Config.Clantag, "change your clantag to wanheda dot redacted" );
		checkbox( "Logs", &Config.Logs, "show useful logs in the top left of your screen" );
		checkbox( "Remove Fakelag Limit", &Config.RemoveFakelagLimit, "remove fakelag limit" );
		checkbox( "Viewmodel Offset Adjustment", &Config.ViewmodelAdjust, "change your viewmodel offset position to a custom one" );
		if ( Config.ViewmodelAdjust ) {
			slider( 20, "Viewmodel X Position", &Config.ViewmodelX, "%", 1 );
			slider( 20, "Viewmodel Y Position", &Config.ViewmodelY, "%", 1 );
			slider( 20, "Viewmodel Z Position", &Config.ViewmodelZ, "%", 1 );
		}
		checkbox( "Watermark", &Config.Watermark, "render the cheat's watermark watermark" );
		combobox( 2, "Watermarks", watermarks, &Config.Watermarks );

		groupbox( 20 + ( _width / 2 ), 60, ( _width / 2 ) - 40, _height - 80, "WH9DFQ8UFE98GHWYEG8" );
		//checkbox( "Anti Untrusted", &Config.AntiUT, "disables features to make your account safe from untrusted bans" );
		combobox( 6, "Config List", config_nigga, &Config.select_this_nigga_config );
		checkbox( "Save Config", &save_config, "save your current configuration to a configuration file" );
		checkbox( "Load Config", &load_config, "load a configuration from a configuration file");

		text("\n");
		text("Menu Accent Colour");
		color_selector("menu_accent_colour", Config.AccentColour);

		break;
		case skins:
		groupbox( 20, 60, ( _width / 2 ) - 40, _height - 80, "WH9DFQ8UFE98GHWYEG9" );
		                                                                              
		static int SelectedWeapon;

		if (SelectedWeapon == 0) {
			combobox(34, "Weapon", weapon_options, &SelectedWeapon);
			combobox(15, "Knife Model", knife_options, &g_Menu.Config.KnifeModel);
			slider( 682, "Knife Skin Paint ID", &Config.KnifeSkin, "", 1 );
		}
		else {
			combobox(34, "Weapon", weapon_options, &SelectedWeapon);
			// ok retard
			// this took too much time
			switch ( SelectedWeapon ) {
			case 1: slider(682, "AK-47 Skin Paint ID", &Config.Ak47Skin, "", 1); break;
			case 2: slider(682, "AUG Skin Paint ID", &Config.AugSkin, "", 1); break;
			case 3: slider(682, "AWP Skin Paint ID", &Config.AwpSkin, "", 1); break;
			case 4: slider(682, "CZ75-Auto Skin Paint ID", &Config.Cz75Skin, "", 1); break;
			case 5: slider(682, "Desert Eagle Skin Paint ID", &Config.DeagleSkin, "", 1); break;
			case 6: slider(682, "Dual Berettas Skin Paint ID", &Config.DualsSkin, "", 1); break;
			case 7: slider(682, "FAMAS Skin Paint ID", &Config.FamasSkin, "", 1); break;
			case 8: slider(682, "Five-Seven Skin Paint ID", &Config.FiveSevenSkin, "", 1); break;
			case 9: slider(682, "G3SG1 Skin Paint ID", &Config.G3sg1Skin, "", 1); break;
			case 10: slider(682, "Galil AR Skin Paint ID", &Config.GalilArSkin, "", 1); break;
			case 11: slider(682, "Glock-18 Skin Paint ID", &Config.Glock18Skin, "", 1); break;
			case 12: slider(682, "M249 Skin Paint ID", &Config.M249Skin, "", 1); break;
			case 13: slider(682, "M4A1 Skin Paint ID", &Config.M4a1SSkin, "", 1); break;
			case 14: slider(682, "M4A4 Skin Paint ID", &Config.M4a4Skin, "", 1); break;
			case 15: slider(682, "MAC-10 Skin Paint ID", &Config.Mac10Skin, "", 1); break;
			case 16: slider(682, "MAG-7 Skin Paint ID", &Config.Mag7Skin, "", 1); break;
			case 17: slider(682, "MP5-SD Skin Paint ID", &Config.Mp5SdSkin, "", 1); break;
			case 18: slider(682, "MP7 Skin Paint ID", &Config.Mp7Skin, "", 1); break;
			case 19: slider(682, "MP9 Skin Paint ID", &Config.Mp9Skin, "", 1); break;
			case 20: slider(682, "Negev Skin Paint ID", &Config.NegevSkin, "", 1); break;
			case 21: slider(682, "Nova Skin Paint ID", &Config.NovaSkin, "", 1); break;
			case 22: slider(682, "P2000 Skin Paint ID", &Config.P2000Skin, "", 1); break;
			case 23: slider(682, "P250 Skin Paint ID", &Config.P250Skin, "", 1); break;
			case 24: slider(682, "P90 Skin Paint ID", &Config.P90Skin, "", 1); break;
			case 25: slider(682, "PP-Bizon Skin Paint ID", &Config.PPBizonSkin, "", 1); break;
			case 26: slider(682, "R8 Revolver Skin Paint ID", &Config.R8Skin, "", 1); break;
			case 27: slider(682, "SCAR-20 Skin Paint ID", &Config.Scar20Skin, "", 1); break;
			case 28: slider(682, "SG 553 Skin Paint ID", &Config.Sg553Skin, "", 1); break;
			case 29: slider(682, "SSG 08 Skin Paint ID", &Config.Ssg08Skin, "", 1); break;
			case 30: slider(682, "Sawed-Off Skin Paint ID", &Config.SawedOffSkin, "", 1); break;
			case 31: slider(682, "Tec-9 Skin Paint ID", &Config.Tec9Skin, "", 1); break;
			case 32: slider(682, "UMP-45 Skin Paint ID", &Config.Ump45Skin, "", 1); break;
			case 33: slider(682, "USP Skin Paint ID", &Config.UspSkin, "", 1); break;
			case 34: slider(682, "XM1014 Skin Paint ID", &Config.Xm1014Skin, "", 1); break;
			}

		}
		checkbox("Apply Changes", &force_apply, "reload gun models/skins");
		groupbox( 20 + ( _width / 2 ), 60, ( _width / 2 ) - 40, _height - 80, "WH9DFQ8UFE98GHWYEG10" );
		combobox( 5, "Gloves Model", gloves_model, &g_Menu.Config.GlovesModel );

		//groupbox( 20 + ( _width / 2 ), 60, ( _width / 2 ) - 40, _height - 80, "autist" );
		break;
		case customization:
		groupbox( 20, 60, ( _width / 2 ) - 40, _height - 80, "WH9DFQ8UFE98GHWYEG11" );

		//checkbox( "full update", &full_update, "force full update" );

		groupbox( 20 + ( _width / 2 ), 60, ( _width / 2 ) - 40, _height - 80, "WH9DFQ8UFE98GHWYEG12" );

		checkbox( "Rank Revealer", &Config.RevealRanks, " " );
		checkbox("Auto Derank", &Config.AutoDerank, "");
		checkbox("AFK Bot", &Config.AfkBot, "");

		}
		

		// tooltip thang
		//g_pSurface->DrawT( _pos.x + ( _width / 2 ), _pos.y + _height - 13, Color( 125, 125, 125, 255 ), g::fonts::menu_font, true, final_tooltip.c_str( ) );

		// wtf is this mickey mouse bullshit i see before my eyes
		// fuck the retard that added this "mouse cursor" looks like shit retard
		/*g_pSurface->Circle( _mouse_pos.x, _mouse_pos.y - 5, 10, 3, Color( 70, 119, 207, 145 ) );
		g_pSurface->Circle( _mouse_pos.x, _mouse_pos.y - 5, 9, 3, Color( 70, 119, 207, 145 ) );
		g_pSurface->Circle( _mouse_pos.x, _mouse_pos.y - 5, 8, 3, Color( 70, 119, 207, 145 ) );
		g_pSurface->Circle( _mouse_pos.x, _mouse_pos.y - 5, 7, 3, Color( 70, 119, 207, 145 ) );
		g_pSurface->Circle( _mouse_pos.x, _mouse_pos.y - 5, 6, 3, Color( 70, 119, 207, 145 ) );
		g_pSurface->Circle( _mouse_pos.x, _mouse_pos.y - 5, 5, 3, Color( 70, 119, 207, 145 ) );
		g_pSurface->Circle( _mouse_pos.x, _mouse_pos.y - 5, 4, 3, Color( 70, 119, 207, 145 ) );
		g_pSurface->Circle( _mouse_pos.x, _mouse_pos.y - 5, 3, 3, Color( 70, 119, 207, 145 ) );
		g_pSurface->Circle( _mouse_pos.x, _mouse_pos.y - 5, 2, 3, Color( 70, 119, 207, 145 ) );
		g_pSurface->Circle( _mouse_pos.x, _mouse_pos.y - 5, 1, 3, Color( 70, 119, 207, 145 ) );*/
	}

	/*while ( ShowCursor( false ) >= 0 );
	while ( ShowCursor( true ) <= 0 );


	if ( menu_opened )
		ShowCursor( false );
	else if ( !menu_opened )
		ShowCursor( true );*/
}
