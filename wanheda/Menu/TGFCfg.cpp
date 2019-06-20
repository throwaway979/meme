#include "Menu.h"
#include "TGFCfg.h"
#include "../Security/XorStrings.h"
// credits to team gamer food

void CConfig::Setup( )
{
	SetupValue(g_Menu.Config.FriendlyFire, false, XorStr("154"));
	SetupValue(g_Menu.Config.FriendlyFireLegit, false, XorStr("155"));

	SetupValue( g_Menu.Config.Aimbot, false, XorStr( "1" ) );
	SetupValue( g_Menu.Config.Hitchance, 0, XorStr( "2" ) );
	SetupValue( g_Menu.Config.HitchanceValue, 0, XorStr( "3" ) );
	SetupValue( g_Menu.Config.Mindmg, 0, XorStr( "4" ) );
	SetupValue( g_Menu.Config.MultiPoint, false, XorStr( "5" ) );
	SetupValue( g_Menu.Config.BodyScale, 0, XorStr( "6" ) );
	SetupValue( g_Menu.Config.HeadScale, 0, XorStr( "7" ) );
	SetupValue( g_Menu.Config.Resolver, false, XorStr( "8" ) );
	//SetupValue( g_Menu.Config.ResolverOverride, false, XorStr( "106" ) );
	SetupValue( g_Menu.Config.DelayShot, false, XorStr( "9" ) );
	SetupValue( g_Menu.Config.IgnoreLimbs, false, XorStr( "10" ) );
	SetupValue( g_Menu.Config.AutoStop, false, XorStr( "11" ) );
	SetupValue( g_Menu.Config.AutoScope, false, XorStr( "12" ) );
	SetupValue( g_Menu.Config.AutoRevolver, false, XorStr( "13" ) );
	//SetupValue( g_Menu.Config.FixShotPitch, false, "pitch_adjust" );
	SetupValue( g_Menu.Config.ShotBacktrack, false, XorStr( "14" ) );
	SetupValue( g_Menu.Config.PosBacktrack, false, XorStr( "15" ) );
	SetupValue( g_Menu.Config.BaimLethal, false, XorStr( "16" ) );

	//SetupValue(g_Menu.Config.BaimPitch, false, "bamepitch");
	SetupValue( g_Menu.Config.BaimInAir, false, XorStr( "17" ) );

	SetupValue( g_Menu.Config.Antiaim, false, XorStr( "18" ) );
	SetupValue( g_Menu.Config.DesyncAngle, false, XorStr( "19" ) );
	SetupValue( g_Menu.Config.RandJitterInRange, false, XorStr( "20" ) );
	SetupValue( g_Menu.Config.JitterRange, 0, XorStr( "21" ) );
	//SetupValue( g_Menu.Config.desync, 0, "desyncchronization_type" );
	SetupValue( g_Menu.Config.pitch_aa, 0, XorStr( "22" ) );
	SetupValue( g_Menu.Config.yaw_aa, 0, XorStr( "23" ) );
	SetupValue( g_Menu.Config.Fakelag, 0, XorStr( "24" ) );
	SetupValue(g_Menu.Config.MaxProcessTicks, 16, XorStr("128"));
	//SetupValue( g_Menu.Config.FakeLagOnPeek, false, XorStr( "25" ) );
	SetupValue( g_Menu.Config.FakelagWhileShooting, false, XorStr( "26" ) );

	SetupValue( g_Menu.Config.Esp, false, XorStr( "27" ) );
	SetupValue( g_Menu.Config.EspColor, 255.f, XorStr( "28" ) );
	SetupValue( g_Menu.Config.Name, false, XorStr( "29" ) );
	SetupValue( g_Menu.Config.Money, false, XorStr( "30" ) );
	SetupValue( g_Menu.Config.Weapon, false, XorStr( "31" ) );
	SetupValue( g_Menu.Config.Box, false, XorStr( "32" ) );
	SetupValue( g_Menu.Config.box_color, 255.f, XorStr( "33" ) );
	SetupValue( g_Menu.Config.VisChams, 255.f, XorStr( "34" ) );
	SetupValue( g_Menu.Config.InVisChams, 255.f, XorStr( "35" ) );
	SetupValue( g_Menu.Config.AccentColour, 255.f, XorStr( "1003" ) );
	SetupValue( g_Menu.Config.HealthBar, false, XorStr( "36" ) );
	SetupValue( g_Menu.Config.AmmoBar, false, XorStr( "37" ) );

	//SetupValue( g_Menu.Config.BulletTracers_Local, false, XorStr( "38" ) );
	//SetupValue( g_Menu.Config.BulletTracers_Enemy, false, XorStr( "39" ) );
	//SetupValue( g_Menu.Config.BulletTracers_Team, false, XorStr( "40" ) );
	//SetupValue( g_Menu.Config.BulletTracers_Local_Color, 255.f, XorStr( "41" ) );
	//SetupValue( g_Menu.Config.BulletTracers_Enemy_Color, 255.f, XorStr( "42" ) );
	//SetupValue( g_Menu.Config.BulletTracers_Team_Color, 255.f, XorStr( "43" ) );

	//SetupValue( g_Menu.Config.Skeleton, false, 2, "skeleton" );
	//SetupValue( g_Menu.Config.SkeletonColor, 255.f, "skeletonclr" );
	SetupValue( g_Menu.Config.Glow, false, XorStr( "44" ) );
	SetupValue( g_Menu.Config.GlowStyle, 0, XorStr( "45" ) );
	SetupValue( g_Menu.Config.GlowColor, 255.f, XorStr( "46" ) );
	SetupValue( g_Menu.Config.LocalGlow, false, XorStr( "47" ) );
	SetupValue( g_Menu.Config.LocalGlowStyle, 0, XorStr( "48" ) );
	SetupValue( g_Menu.Config.LocalGlowColor, 255.f, XorStr( "49" ) );
	//SetupValue( g_Menu.Config.SkeletonColour, 255.f, XorStr( "50" ) );
	//SetupValue( g_Menu.Config.Skeleton_Nigga_Backtrack, false, XorStr( "51" ) );
	//SetupValue( g_Menu.Config.Skeleton_Nigga, false, XorStr( "52" ) );

	//SetupValue( g_Menu.Config.Chams, false, "chams" );
	SetupValue( g_Menu.Config.bChams, false, XorStr( "53" ) );
	SetupValue( g_Menu.Config.bChamsZ, false, XorStr( "54" ) );
	//SetupValue( g_Menu.Config.bChamsDesync, false, XorStr( "55" ) );
	//SetupValue( g_Menu.Config.bChamsWhileScoped, false, XorStr( "56" ) );
	//SetupValue( g_Menu.Config.LocalChams, false, XorStr( "57" ) );
	SetupValue( g_Menu.Config.LocalGlow, false, XorStr( "58" ) );

	//SetupValue( g_Menu.Config.MaterialMode, 0, XorStr( "59" ) );
	//SetupValue( g_Menu.Config.MaterialMode_local, 0, XorStr( "60" ) );
	//SetupValue( g_Menu.Config.iMaterial, 0, XorStr( "61" ) );
	//SetupValue( g_Menu.Config.iMaterial_invis, 0, XorStr( "62" ) );
	//SetupValue( g_Menu.Config.iMaterialLocal, 0, XorStr( "63" ) );
	//SetupValue( g_Menu.Config.VisChams, 255.f, XorStr( "64" ) );
	//SetupValue( g_Menu.Config.InVisChams, 255.f, XorStr( "65" ) );
	//SetupValue( g_Menu.Config.DesyncChams, 255.f, XorStr( "66" ) );

	//SetupValue( g_Menu.Config.ArmsChams, false, "armschams" );

	//SetupValue( g_Menu.Config.BoxWidth, 4, XorStr( "67" ) );

	SetupValue( g_Menu.Config.Hitmarker, false, XorStr( "68" ) );
	SetupValue( g_Menu.Config.HitmarkerSound, false, XorStr( "69" ) );
	SetupValue( g_Menu.Config.NoZoom, false, XorStr( "70" ) );
	SetupValue( g_Menu.Config.NoScope, false, XorStr( "71" ) );
	SetupValue( g_Menu.Config.NoRecoil, false, XorStr( "72" ) );
	SetupValue( g_Menu.Config.NoSmoke, false, XorStr( "73" ) );
	SetupValue( g_Menu.Config.NoFlash, false, XorStr( "74" ) );
	//SetupValue( g_Menu.Config.NoSleeves, false, XorStr( "75" ) );
	SetupValue( g_Menu.Config.Fov, 0, XorStr( "76" ) );
	SetupValue( g_Menu.Config.ViewmodelFov, 0, XorStr( "77" ) );
	SetupValue( g_Menu.Config.ViewmodelAdjust, false, XorStr( "78" ) );
	SetupValue( g_Menu.Config.ViewmodelX, 0, XorStr( "79" ) );
	SetupValue( g_Menu.Config.ViewmodelY, 0, XorStr( "80" ) );
	SetupValue( g_Menu.Config.ViewmodelZ, 0, XorStr( "81" ) );
	SetupValue( g_Menu.Config.Arrows, false, XorStr( "82" ) );
	SetupValue( g_Menu.Config.Indicators, false, XorStr( "83" ) );
	SetupValue( g_Menu.Config.Crosshair, false, XorStr( "84" ) );
	SetupValue( g_Menu.Config.RecoilCrosshair, false, XorStr( "1000" ) );
	SetupValue( g_Menu.Config.Nightmode, false, XorStr( "85" ) );
	SetupValue( g_Menu.Config.Skybox, 255.f, XorStr( "86" ) );
	SetupValue( g_Menu.Config.Nightval, 100, XorStr( "87" ) );
	SetupValue( g_Menu.Config.Ragdoll, false, XorStr("118") );

	SetupValue( g_Menu.Config.Bhop, false, XorStr( "88" ) );
	//SetupValue( g_Menu.Config.WASDMovement, false, XorStr( "89" ) );
	SetupValue( g_Menu.Config.AutoStrafe, false, XorStr( "90" ) );
	SetupValue( g_Menu.Config.Clantag, false, XorStr( "91" ) );
	SetupValue( g_Menu.Config.InfinityDuck, false, XorStr( "92" ) );
	SetupValue( g_Menu.Config.Logs, false, XorStr( "93" ) );
	SetupValue( g_Menu.Config.Watermark, true, XorStr( "94" ) );
	SetupValue( g_Menu.Config.Watermarks, 1, XorStr( "95" ) );

	SetupValue( g_Menu.Config.HeadOnly, false, XorStr( "96" ) );
	SetupValue( g_Menu.Config.HeadOnlyOnlyOnKey, false, XorStr( "97" ) );
	SetupValue( g_Menu.Config.Thirdperson_NIGGA, false, XorStr( "98" ) );
	SetupValue( g_Menu.Config.Thirdperson_NIGGA_Key, false, XorStr( "99" ) );
	SetupValue( g_Menu.Config.FakeDuckHAH, false, XorStr( "100" ) );
	SetupValue( g_Menu.Config.FakeDuckHAH_Key, false, XorStr( "101" ) );
	SetupValue( g_Menu.Config.SlowWalkHAH, false, XorStr( "102" ) );
	SetupValue( g_Menu.Config.SlowWalkHAH_Key, false, XorStr( "103" ) );
	//SetupValue( g_Menu.Config.override_resolver_key, false, XorStr( "104" ) );
	SetupValue( g_Menu.Config.HeadOnlyOnlyOnKey_key, false, XorStr( "105" ) );

	SetupValue(g_Menu.Config.KnifeModel, 0, XorStr("107"));
	SetupValue(g_Menu.Config.KnifeSkin, 0, XorStr("108"));
	SetupValue(g_Menu.Config.Scar20Skin, 0, XorStr("109"));
	SetupValue(g_Menu.Config.G3sg1Skin, 0, XorStr("110"));
	SetupValue(g_Menu.Config.Ssg08Skin, 0, XorStr("111"));
	SetupValue(g_Menu.Config.Ak47Skin, 0, XorStr("112"));
	SetupValue(g_Menu.Config.M4a4Skin, 0, XorStr("113"));
	SetupValue(g_Menu.Config.M4a1SSkin, 0, XorStr("114"));
	SetupValue(g_Menu.Config.DeagleSkin, 0, XorStr("115"));
	SetupValue(g_Menu.Config.R8Skin, 0, XorStr("116"));
	SetupValue(g_Menu.Config.Cz75Skin, 0, XorStr("117"));
	SetupValue(g_Menu.Config.AugSkin, 0, XorStr("129"));
	SetupValue(g_Menu.Config.AwpSkin, 0, XorStr("130"));
	SetupValue(g_Menu.Config.DualsSkin, 0, XorStr("131"));
	SetupValue(g_Menu.Config.FamasSkin, 0, XorStr("132"));
	SetupValue(g_Menu.Config.FiveSevenSkin, 0, XorStr("133"));
	SetupValue(g_Menu.Config.GalilArSkin, 0, XorStr("134"));
	SetupValue(g_Menu.Config.Glock18Skin, 0, XorStr("135"));
	SetupValue(g_Menu.Config.M249Skin, 0, XorStr("136"));
	SetupValue(g_Menu.Config.Mac10Skin, 0, XorStr("137"));
	SetupValue(g_Menu.Config.Mag7Skin, 0, XorStr("138"));
	SetupValue(g_Menu.Config.Mp5SdSkin, 0, XorStr("139"));
	SetupValue(g_Menu.Config.Mp7Skin, 0, XorStr("140"));
	SetupValue(g_Menu.Config.Mp9Skin, 0, XorStr("141"));
	SetupValue(g_Menu.Config.NegevSkin, 0, XorStr("142"));
	SetupValue(g_Menu.Config.NovaSkin, 0, XorStr("143"));
	SetupValue(g_Menu.Config.P2000Skin, 0, XorStr("144"));
	SetupValue(g_Menu.Config.P250Skin, 0, XorStr("145"));
	SetupValue(g_Menu.Config.P90Skin, 0, XorStr("146"));
	SetupValue(g_Menu.Config.PPBizonSkin, 0, XorStr("147"));
	SetupValue(g_Menu.Config.Sg553Skin, 0, XorStr("148"));
	SetupValue(g_Menu.Config.SawedOffSkin, 0, XorStr("149"));
	SetupValue(g_Menu.Config.Tec9Skin, 0, XorStr("150"));
	SetupValue(g_Menu.Config.Ump45Skin, 0, XorStr("151"));
	SetupValue(g_Menu.Config.UspSkin, 0, XorStr("152"));
	SetupValue(g_Menu.Config.Xm1014Skin, 0, XorStr("153"));


	SetupValue(g_Menu.Config.legitbotEnable, false, XorStr("118"));
	SetupValue(g_Menu.Config.legitbotFov, 0, XorStr("119"));
	SetupValue(g_Menu.Config.legitbotSmooth, 0, XorStr("120"));
	SetupValue(g_Menu.Config.legitbotCubicInterpolation, false, XorStr("121"));
	SetupValue(g_Menu.Config.legitbotNearestHitbox, 0, XorStr("122"));
	SetupValue(g_Menu.Config.legitHitbox, 0, XorStr("123"));
	SetupValue(g_Menu.Config.legitRecoilControlSystem, false, XorStr("124"));
	SetupValue(g_Menu.Config.RevealRanks, false, XorStr("125"));
	SetupValue(g_Menu.Config.DisablePostProcessing, false, XorStr("126"));
	SetupValue(g_Menu.Config.AutoPistol, false, XorStr("127"));
	SetupValue(g_Menu.Config.BaimOnKey, false, XorStr("999"));
	SetupValue(g_Menu.Config.BaimOnKeyKey, 0, XorStr("1001"));
	SetupValue(g_Menu.Config.BaimAfterShots, 0, XorStr("1002"));
	SetupValue(g_Menu.Config.LegitBacktrack, false, XorStr("1003"));
	SetupValue(g_Menu.Config.LegitbotRCSX, 0, XorStr("1004"));
	SetupValue(g_Menu.Config.LegitbotRCSY, 0, XorStr("1005"));
	SetupValue(g_Menu.Config.legitbotOnKey, false, XorStr("1006"));
	SetupValue(g_Menu.Config.legitbot__Key, 0, XorStr("1007"));
	SetupValue(g_Menu.Config.LegitSilentAim, false, XorStr("1008"));
	SetupValue( g_Menu.Config.GrenadePrediction, false, XorStr( "1009" ) );
	SetupValue( g_Menu.Config.Arrows, false, XorStr( "1010" ) );
	SetupValue( g_Menu.Config.ArrowsColor, 255.f, XorStr( "1011" ) );
	SetupValue( g_Menu.Config.AutoDerank, false, XorStr( "1012" ) );
	SetupValue( g_Menu.Config.AfkBot, false, XorStr( "1012" ) );
	//	SetupValue(g_Menu.Config.LegitBacktrack, false, "legitbacktrack");
	//	SetupValue(g_Menu.Config.Ak47meme, false, "ak47meme");
}

void CConfig::SetupValue( int& value, int def, std::string name )
{
	value = def;
	ints.push_back( new ConfigValue< int >( name, &value ) );
}

void CConfig::SetupValue( float& value, float def, std::string name )
{
	value = def;
	floats.push_back( new ConfigValue< float >( name, &value ) );
}

void CConfig::SetupValue( bool& value, bool def, std::string name )
{
	value = def;
	bools.push_back( new ConfigValue< bool >( name, &value ) );
}

void CConfig::SetupValue( Color& value, float def, std::string name )
{
	value.red = def;
	value.green = def;
	value.blue = def;
	value.alpha = def;

	ints.push_back( new ConfigValue< int >( name + "red", &value.red ) );
	ints.push_back( new ConfigValue< int >( name + "green", &value.green ) );
	ints.push_back( new ConfigValue< int >( name + "blue", &value.blue ) );
	ints.push_back( new ConfigValue< int >( name + "alpha", &value.alpha ) );
}

void CConfig::SetupValue( bool* value, bool def, int size, std::string name ) // for multiboxes
{
	for ( int c = 0; c < size; c++ )
	{
		value[ c ] = def;

		name += std::to_string( c );

		bools.push_back( new ConfigValue< bool >( name, &value[ c ] ) );
	}
}

static char name[ ] = "redactedhack";

void CConfig::Save( int config_number )
{
	static TCHAR path[ MAX_PATH ];
	std::string folder, file;

	if ( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, path ) ) )
	{
		folder = std::string( path ) + "\\redactedhack\\";
		file = std::string( path ) + "\\redactedhack\\config" + std::to_string( config_number ) + ".redactedhack";
	}

	CreateDirectory( folder.c_str( ), NULL );

	for ( auto value : ints )
		WritePrivateProfileString( name, value->name.c_str( ), std::to_string( *value->value ).c_str( ), file.c_str( ) );

	for ( auto value : floats )
		WritePrivateProfileString( name, value->name.c_str( ), std::to_string( *value->value ).c_str( ), file.c_str( ) );

	for ( auto value : bools )
		WritePrivateProfileString( name, value->name.c_str( ), *value->value ? "TRUE" : "FALSE", file.c_str( ) );
}

void CConfig::Load( int config_number )
{
	static TCHAR path[ MAX_PATH ];
	std::string folder, file;

	if ( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, path ) ) )
	{
		folder = std::string( path ) + "\\redactedhack\\";
		file = std::string( path ) + "\\redactedhack\\config" + std::to_string( config_number ) + ".redactedhack";
	}

	CreateDirectory( folder.c_str( ), NULL );

	char value_l[ 32 ] = { '\0' };

	for ( auto value : ints )
	{
		GetPrivateProfileString( name, value->name.c_str( ), "", value_l, 32, file.c_str( ) );
		*value->value = atoi( value_l );
	}

	for ( auto value : floats )
	{
		GetPrivateProfileString( name, value->name.c_str( ), "", value_l, 32, file.c_str( ) );
		*value->value = atof( value_l );
	}

	for ( auto value : bools )
	{
		GetPrivateProfileString( name, value->name.c_str( ), "", value_l, 32, file.c_str( ) );
		*value->value = !strcmp( value_l, "TRUE" );
	}
}

CConfig* g_Config = new CConfig( );
