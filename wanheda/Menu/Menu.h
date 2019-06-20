#pragma once
#include <Windows.h>
#include <string>
#include <filesystem>

#include "..\SDK\ISurface.h"
#include "..\SDK\Vector.h"
#include "..\Utils\Color.h"

namespace fs = std::experimental::filesystem;

struct ColorV2 {
	float red, green, blue, alpha;
};

enum {
	check_box,
	slider,
	combo_box,
	multi_box
};

class c_menu {
public:
	bool menu_opened;
	Vector2D mouse_pos( );
	bool mouse_in_params( int x, int y, int x2, int y2 );
	void draw( );
	void draw_hitgroups( );
	void draw_radar( );
	void draw_esp_preview( int x_pos, int y_pos );

	struct {
		bool  legitbotEnable;
		bool  legitbotOnKey;
		int  legitbot__Key;
		int	  legitbotFov;
		bool  legitbotCubicInterpolation;
		bool  legitbotNearestHitbox;
		int	  legitbotSmooth;
		int	  LegitbotRCSX;
		int	  LegitbotRCSY;
		bool  legitRecoilControlSystem;
		int   legitHitbox = 0;


		bool FriendlyFire;
		bool FriendlyFireLegit;
		bool Aimbot;
		int Hitchance = 0;
		int desync = 0;
		int pitch_aa = 0;
		int yaw_aa = 0;
		int HitchanceValue;
		int Mindmg;
		bool Resolver;
		bool ResolverOverride;
		int BodyScale;
		int HeadScale;
		bool MultiPoint;
		bool DelayShot;
		bool IgnoreLimbs;
		bool AutoStop;
		bool AutoRevolver;
		bool AutoScope;
		bool FixShotPitch;
		bool PosBacktrack;
		bool ShotBacktrack;
		bool BaimLethal;
		bool BaimPitch;
		bool BaimInAir;
		bool BaimOnKey;
		int BaimOnKeyKey;
		int BaimAfterShots;
		bool HeadOnly;
		bool HeadOnlyOnlyOnKey;
		int HeadOnlyOnlyOnKey_key;

		bool Antiaim;
		bool DesyncAngle;
		bool RandJitterInRange;
		int	JitterRange;
		int	Fakelag;
		int  MaxProcessTicks = 16;
		bool FakeLagOnPeek;
		bool FakelagWhileShooting;

		bool Esp;
		Color EspColor = { 255.f, 255.f, 255.f, 255.f };
		int Font;
		ColorV2 FontColor = { 255.f,255.f,255.f,255.f };
		bool Name;
		bool Money;
		bool HealthVal;
		bool Weapon;
		bool Box;
		ColorV2 BoxColor = { 255.f,255.f,255.f,255.f };
		Color box_color = { 255.f, 255.f, 255.f, 255.f };
		bool HealthBar;
		bool AmmoBar;
		bool Skeleton[ 2 ] = { false,false };
		ColorV2 SkeletonColor = { 0.f,146.f,235.f,255.f }; //0, 146, 235
		Color SkeletonColour = { 255.f, 255.f, 255.f, 255.f };
		bool HitboxPoints;
		bool Glow;
		int GlowStyle;
		Color GlowColor = { 255.f, 255.f, 255.f, 255.f };
		bool Skeleton_Nigga;
		bool Skeleton_Nigga_Backtrack;
		bool Chams;
		bool ArmsChams;
		bool LocalChams;
		bool LocalGlow;
		int LocalGlowStyle;
		Color LocalGlowColor;
		int MaterialMode;
		int MaterialMode_local;
		bool bChamsWhileScoped;
		int iMaterial;
		int iMaterial_invis;
		int iMaterialLocal;

		bool BulletTracers_Local;
		bool BulletTracers_Enemy;
		bool BulletTracers_Team;
		Color BulletTracers_Local_Color;
		Color BulletTracers_Enemy_Color;
		Color BulletTracers_Team_Color;

		bool bChams;
		bool bChamsZ;
		bool bChamsDesync;
		Color VisChams = { 255.f, 255.f, 255.f, 255.f };
		Color InVisChams = { 255.f, 255.f, 255.f, 255.f };
		Color DesyncChams = { 255.f, 255.f, 255.f, 255.f };
		Color AccentColour = { 70.f, 119.f, 207.f, 255.f };

		int BoxWidth;

		int KnifeModel;
		int KnifeSkin;
		int Ssg08Skin;
		int Scar20Skin;
		int G3sg1Skin;
		int Ak47Skin;
		int DeagleSkin;
		int R8Skin;
		int M4a4Skin;
		int M4a1SSkin;
		int Cz75Skin;
		int AugSkin;
		int AwpSkin;
		int DualsSkin;
		int FamasSkin;
		int FiveSevenSkin;
		int GalilArSkin;
		int Glock18Skin;
		int M249Skin;
		int Mac10Skin;
		int Mag7Skin;
		int Mp5SdSkin;
		int Mp7Skin;
		int Mp9Skin;
		int NegevSkin;
		int NovaSkin;
		int P2000Skin;
		int P250Skin;
		int P90Skin;
		int PPBizonSkin;
		int Sg553Skin;
		int SawedOffSkin;
		int Tec9Skin;
		int Ump45Skin;
		int UspSkin;
		int Xm1014Skin;
		bool ForceApply;

		ColorV2 ChamsColor = { 255.f,255.f,255.f,255.f };
		bool NoZoom;
		bool NoScope;
		bool NoRecoil;
		bool NoSmoke;
		bool NoFlash;
		bool NoSleeves;
		int Fov;
		int ViewmodelFov;
		bool ViewmodelAdjust;
		int ViewmodelX;
		int ViewmodelY;
		int ViewmodelZ;
		bool Arrows;
		bool Indicators;
		bool Thirdperson_NIGGA;
		int Thirdperson_NIGGA_Key;
		bool Hitmarker;
		bool HitmarkerSound;
		Color Hitcolor = { 255.f, 255.f, 255.f };
		bool Crosshair;
		bool Nightmode;
		Color Skybox = { 255.f, 255.f, 255.f };
		int Nightval;

		bool Ragdoll;

		bool Bhop;
		bool WASDMovement;
		bool AutoStrafe;
		bool Clantag;
		bool InfinityDuck;
		bool FakeDuckHAH;
		int FakeDuckHAH_Key;
		bool SlowWalkHAH;
		int SlowWalkHAH_Key;
		int override_resolver_key;
		bool Logs;
		bool RemoveFakelagLimit;
		bool Watermark;
		int  Watermarks;
		bool LegitBacktrack;
		bool Ak47meme;
		int	Test;
		int select_this_nigga_config = 0;

		bool RevealRanks;
		bool DisablePostProcessing;
		bool RecoilCrosshair;
		bool AutoPistol;
		bool LegitSilentAim;

		bool RenderArrows;
		Color ArrowsColor = { 255.f, 255.f, 255.f, 255.f };

		bool GrenadePrediction;
		Color GrenadePredictionColor = { 255.f, 255.f, 255.f, 255.f };

		bool AutoDerank;

		bool AfkBot;

		int GlovesModel;
	}Config;

private:

	Vector2D _pos = Vector2D( 100.f, 150.f );
	Vector2D _pos_radar = Vector2D( 960.f, 150.f );
	Vector2D _pos_esp_preview = Vector2D( 960.f, 150.f );
	int screen_width;
	int screen_height;
	int y_offset;
	int x_offset;
	int menu_alpha_main;
	int menu_alpha_text;
	int groupbox_scroll_add;
	int groupbox_width;
	int groupbox_bottom;
	int groupbox_top;
	bool is_keybind_selected;
	std::string keybind_selected_name;
	std::string tooltip = "";

	void groupbox( int x, int y, int w, int h, std::string name );
	void checkbox( std::string name, bool* item, std::string tooltip );
	void text(std::string name);
	void button( std::string name, bool* item );
	void combobox( int size, std::string name, std::string* itemname, int* item );
	void update_input( );
	bool is_key_pressed( int vkey );
	bool is_key_held( int vkey );
	bool is_key_released( int vkey );
	void keybind( int& item, std::string name );
	void color_selector(std::string name, Color &colour);
	void slider( int max, std::string name, int* item, std::string suffix, int step );

	RECT get_text_size( DWORD font, const char* text, ... )
	{
		size_t origsize = strlen( text ) + 1;
		const size_t newsize = 100;
		size_t convertedChars = 0;
		wchar_t wcstring[ newsize ];
		mbstowcs_s( &convertedChars, wcstring, origsize, text, _TRUNCATE );

		RECT rect; int x, y;
		g_pSurface->GetTextSize( font, wcstring, x, y );
		rect.left = x; rect.bottom = y;
		rect.right = x;
		return rect;
	}

	void color_selector( std::string name, int* red, int* blue, int* green, int* alpha );
};

extern c_menu g_Menu;