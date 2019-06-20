#include "ESP.h"
#include "..\Aimbot\Aimbot.h"
#include "..\Aimbot\LagComp.h"
#include "..\..\Utils\Utils.h"
#include "..\..\Utils/SPoint.h"
#include "..\..\SDK\IVEngineClient.h"
#include "..\..\SDK\PlayerInfo.h"
#include "..\..\SDK\ISurface.h"
#include "..\..\SDK\Hitboxes.h"
#include "..\..\Utils\Math.h"
#include "..\..\Menu\Menu.h"
#include "..\..\SDK\Vector.h"
#include "..\..\SDK\VMatrix.h"
#include "..\..\SDK\Hitboxes.h"

#include <array>

#include <map>

#include <d3d9.h>

ESP g_ESP;

// Maps weapon id to the corresponding character

void BombIndicator( C_BaseEntity* pEnt )
{
	int screen_width, screen_height;
	g_pEngine->GetScreenSize( screen_width, screen_height );

	if ( pEnt->GetClientClass( )->ClassID == ( int )126 )
	{
		float flBlow = pEnt->C4Timer( );
		float lifetime = flBlow - ( g_pGlobalVars->intervalPerTick * g::pLocalEntity->GetTickBase( ) );
		g_pSurface->FilledRect( screen_width / 2 - 100, 20, 200, 20, Color( 0, 0, 0, 75 ) );

		g_pSurface->OutlinedRect( screen_width / 2 - 100, 20, 200, 20, Color( 39, 39, 47, 255 ) );
		g_pSurface->FilledRect( screen_width / 2 - 80, 25, 160, 10, Color( 0, 0, 0, 140 ) );
		g_pSurface->vertical_gradient_rect( screen_width / 2 - 80, 25, 160 / lifetime, 10, Color( 0, 255, 0 ), Color( 0, 0, 0 ) );
		g_pSurface->OutlinedRect( screen_width / 2 - 80, 25, 160, 10, Color( 0, 0, 0 ) );
		g_pSurface->DrawT( screen_width / 2, 40, Color( 255, 255, 255 ), g::fonts::watermark_font, true, "BOMB" );
	}
}

void ESP::RenderBox( )
{
	g_pSurface->OutlinedRect( Box.left, Box.top, Box.right, Box.bottom, color );
	g_pSurface->OutlinedRect( Box.left + 1, Box.top + 1, Box.right - 2, Box.bottom - 2, Color( color.red, color.green, color.blue, 40 ) );
	g_pSurface->OutlinedRect( Box.left - 1, Box.top - 1, Box.right + 2, Box.bottom + 2, Color( color.red, color.green, color.blue, 40 ) );
}

void ESP::RenderName( C_BaseEntity* pEnt, int iterator )
{
	static auto sanitize = [ ]( char* name ) -> std::string {
		name[ 127 ] = '\0';

		std::string tmp( name );

		if ( tmp.length( ) > 11 ) {
			tmp.erase( 11, ( tmp.length( ) - 11 ) );
			tmp.append( "..." );
		}

		return tmp;
	};
	PlayerInfo_t pInfo;
	g_pEngine->GetPlayerInfo( iterator, &pInfo );

	std::string name = sanitize( pInfo.szName );

	if ( g_Menu.Config.Name ) {
		if ( g_Menu.Config.Money )
			g_pSurface->DrawT( Box.left + ( Box.right / 2 ), Box.top - 26, textcolor, g::fonts::esp_font, true, name.c_str( ) );
		else
			g_pSurface->DrawT( Box.left + ( Box.right / 2 ), Box.top - 15, textcolor, g::fonts::esp_font, true, name.c_str( ) );
	}
}

void ESP::RenderMoney( C_BaseEntity * pEnt )
{
	std::string money = "$" + std::to_string( pEnt->GetMoney( ) );

	if ( g_Menu.Config.Money )
		g_pSurface->DrawT( Box.left + ( Box.right / 2 ), Box.top - 15, textcolor, g::fonts::esp_font, true, money.c_str( ) );
}

void ESP::RenderWeaponName( C_BaseEntity * pEnt )
{
	auto weapon = pEnt->GetActiveWeapon( );

	if ( !weapon )
		return;

	auto strWeaponName = weapon->GetName( );
	strWeaponName.erase( 0, 7 );

	std::transform( strWeaponName.begin( ), strWeaponName.end( ), strWeaponName.begin( ), toupper );

	if ( g_Menu.Config.Weapon ) {
		g_pSurface->DrawT( Box.left + ( Box.right / 2 ), Box.top + Box.bottom, textcolor, g::fonts::esp_font, true, strWeaponName.c_str( ) );

		g_pSurface->DrawT( Box.left + ( Box.right / 2 ), Box.top + Box.bottom + 11, textcolor, g::fonts::weapon_icons_font, true, weapon->GetNameWeaponIcon( ) );
	}
}

void ESP::RenderHealth( C_BaseEntity * pEnt )
{
	if ( g_Menu.Config.HealthBar )
	{
		//g_pSurface->FilledRect( Box.left - 6, Box.top - 1, 4, Box.bottom + 2, Color( 0, 0, 0, 210 ) );
		int pixelValue = pEnt->GetHealth( ) * Box.bottom / 100;
		std::clamp( pixelValue, 0, 100 );
		g_pSurface->FilledRect( Box.left - 5, Box.top + Box.bottom - pixelValue, 2, pixelValue, Color( 0, 255, 0, 220 ) );
	}
}

void ESP::RenderAmmo( C_BaseEntity * pEnt )
{
	auto weapon = pEnt->GetActiveWeapon( );

	if ( g_Menu.Config.AmmoBar )
	{
		int pixelValue = ( weapon->GetAmmo( ) * Box.bottom ) / weapon->GetCSWpnData( )->max_clip;
		std::clamp( pixelValue, 0, 1000 );
		g_pSurface->FilledRect( Box.left - 5 - 5, Box.top + Box.bottom - pixelValue, 2, pixelValue, Color( 65, 105, 225, 220 ) );
	}
}

void ESP::RenderHitboxPoints( C_BaseEntity * pEnt )
{
	for ( int hitbox = 0; hitbox < 28; hitbox++ )
	{
		Vector2D w2sHitbox;
		Utils::WorldToScreen( g::AimbotHitbox[ pEnt->EntIndex( ) ][ hitbox ], w2sHitbox );
		g_pSurface->FilledRect( w2sHitbox.x - 2, w2sHitbox.y - 2, 4, 4, color );
	}
}

void ESP::RenderSkeleton( C_BaseEntity * pEnt ) // the best
{
	if ( g_LagComp.PlayerRecord[ pEnt->EntIndex( ) ].size( ) == 0 )
		return;
	int size = 0;

	if ( g_Menu.Config.Skeleton_Nigga )
		size++;
	if ( g_Menu.Config.Skeleton_Nigga_Backtrack )
		size++;

	for ( int mode = 0; size; mode++ )
	{
		Vector Hitbox[ 19 ];
		Vector2D Hitboxw2s[ 19 ];

		int Record = 0;

		if ( mode == 0 && g_Menu.Config.Skeleton_Nigga )
		{
			Record = g_LagComp.PlayerRecord[ pEnt->EntIndex( ) ].size( ) - 1;

			if ( g_LagComp.ShotTick[ pEnt->EntIndex( ) ] != -1 )
				Record = g_LagComp.ShotTick[ pEnt->EntIndex( ) ];
		}

		for ( int hitbox = 0; hitbox < 19; hitbox++ )
		{
			Hitbox[ hitbox ] = pEnt->GetHitboxPosition( hitbox, g_LagComp.PlayerRecord[ pEnt->EntIndex( ) ].at( Record ).Matrix );
			Utils::WorldToScreen( Hitbox[ hitbox ], Hitboxw2s[ hitbox ] );
		}

		//spine
		g_pSurface->Line( Hitboxw2s[ HITBOX_HEAD ].x, Hitboxw2s[ HITBOX_HEAD ].y, Hitboxw2s[ HITBOX_NECK ].x, Hitboxw2s[ HITBOX_NECK ].y, skelecolor );
		g_pSurface->Line( Hitboxw2s[ HITBOX_NECK ].x, Hitboxw2s[ HITBOX_NECK ].y, Hitboxw2s[ HITBOX_UPPER_CHEST ].x, Hitboxw2s[ HITBOX_UPPER_CHEST ].y, skelecolor );
		g_pSurface->Line( Hitboxw2s[ HITBOX_UPPER_CHEST ].x, Hitboxw2s[ HITBOX_UPPER_CHEST ].y, Hitboxw2s[ HITBOX_LOWER_CHEST ].x, Hitboxw2s[ HITBOX_LOWER_CHEST ].y, skelecolor );
		g_pSurface->Line( Hitboxw2s[ HITBOX_LOWER_CHEST ].x, Hitboxw2s[ HITBOX_LOWER_CHEST ].y, Hitboxw2s[ HITBOX_THORAX ].x, Hitboxw2s[ HITBOX_THORAX ].y, skelecolor );
		g_pSurface->Line( Hitboxw2s[ HITBOX_THORAX ].x, Hitboxw2s[ HITBOX_THORAX ].y, Hitboxw2s[ HITBOX_BELLY ].x, Hitboxw2s[ HITBOX_BELLY ].y, skelecolor );
		g_pSurface->Line( Hitboxw2s[ HITBOX_BELLY ].x, Hitboxw2s[ HITBOX_BELLY ].y, Hitboxw2s[ HITBOX_PELVIS ].x, Hitboxw2s[ HITBOX_PELVIS ].y, skelecolor );

		//right leg
		g_pSurface->Line( Hitboxw2s[ HITBOX_PELVIS ].x, Hitboxw2s[ HITBOX_PELVIS ].y, Hitboxw2s[ HITBOX_RIGHT_THIGH ].x, Hitboxw2s[ HITBOX_RIGHT_THIGH ].y, skelecolor );
		g_pSurface->Line( Hitboxw2s[ HITBOX_RIGHT_THIGH ].x, Hitboxw2s[ HITBOX_RIGHT_THIGH ].y, Hitboxw2s[ HITBOX_RIGHT_CALF ].x, Hitboxw2s[ HITBOX_RIGHT_CALF ].y, skelecolor );
		g_pSurface->Line( Hitboxw2s[ HITBOX_RIGHT_CALF ].x, Hitboxw2s[ HITBOX_RIGHT_CALF ].y, Hitboxw2s[ HITBOX_RIGHT_FOOT ].x, Hitboxw2s[ HITBOX_RIGHT_FOOT ].y, skelecolor );

		//right arm
		g_pSurface->Line( Hitboxw2s[ HITBOX_NECK ].x, Hitboxw2s[ HITBOX_NECK ].y, Hitboxw2s[ HITBOX_RIGHT_UPPER_ARM ].x, Hitboxw2s[ HITBOX_RIGHT_UPPER_ARM ].y, skelecolor );
		g_pSurface->Line( Hitboxw2s[ HITBOX_RIGHT_UPPER_ARM ].x, Hitboxw2s[ HITBOX_RIGHT_UPPER_ARM ].y, Hitboxw2s[ HITBOX_RIGHT_FOREARM ].x, Hitboxw2s[ HITBOX_RIGHT_FOREARM ].y, skelecolor );
		g_pSurface->Line( Hitboxw2s[ HITBOX_RIGHT_FOREARM ].x, Hitboxw2s[ HITBOX_RIGHT_FOREARM ].y, Hitboxw2s[ HITBOX_RIGHT_HAND ].x, Hitboxw2s[ HITBOX_RIGHT_HAND ].y, skelecolor );

		//left leg
		g_pSurface->Line( Hitboxw2s[ HITBOX_PELVIS ].x, Hitboxw2s[ HITBOX_PELVIS ].y, Hitboxw2s[ HITBOX_LEFT_THIGH ].x, Hitboxw2s[ HITBOX_LEFT_THIGH ].y, skelecolor );
		g_pSurface->Line( Hitboxw2s[ HITBOX_LEFT_THIGH ].x, Hitboxw2s[ HITBOX_LEFT_THIGH ].y, Hitboxw2s[ HITBOX_LEFT_CALF ].x, Hitboxw2s[ HITBOX_LEFT_CALF ].y, skelecolor );
		g_pSurface->Line( Hitboxw2s[ HITBOX_LEFT_CALF ].x, Hitboxw2s[ HITBOX_LEFT_CALF ].y, Hitboxw2s[ HITBOX_LEFT_FOOT ].x, Hitboxw2s[ HITBOX_LEFT_FOOT ].y, skelecolor );

		//left arm
		g_pSurface->Line( Hitboxw2s[ HITBOX_NECK ].x, Hitboxw2s[ HITBOX_NECK ].y, Hitboxw2s[ HITBOX_LEFT_UPPER_ARM ].x, Hitboxw2s[ HITBOX_LEFT_UPPER_ARM ].y, skelecolor );
		g_pSurface->Line( Hitboxw2s[ HITBOX_LEFT_UPPER_ARM ].x, Hitboxw2s[ HITBOX_LEFT_UPPER_ARM ].y, Hitboxw2s[ HITBOX_LEFT_FOREARM ].x, Hitboxw2s[ HITBOX_LEFT_FOREARM ].y, skelecolor );
		g_pSurface->Line( Hitboxw2s[ HITBOX_LEFT_FOREARM ].x, Hitboxw2s[ HITBOX_LEFT_FOREARM ].y, Hitboxw2s[ HITBOX_LEFT_HAND ].x, Hitboxw2s[ HITBOX_LEFT_HAND ].y, skelecolor );
	}
}

void ESP::RenderArrows( C_BaseEntity* pEnt )
{
	if ( !g::pLocalEntity->IsAlive( ) && g_Menu.Config.Arrows )
		return;

	Vector screenPos;

	auto client_viewangles = Vector( );
	auto screen_width = 0, screen_height = 0;

	g_pEngine->GetViewAngles( client_viewangles );
	g_pEngine->GetScreenSize( screen_width, screen_height );
	
	auto radius = 450.f;
	Vector local_position = g::pLocalEntity->GetOrigin( ) + g::pLocalEntity->m_vecViewOffset( );

	const auto screen_center = Vector( screen_width / 2.f, screen_height / 2.f, 0 );
	const auto rot = DEG2RAD( client_viewangles.y - g_Math.CalcAngle( local_position, pEnt->GetHitboxPosition( pEnt, HITBOX_UPPER_CHEST ) ).y - 90 );

	std::vector<Vertex_t> Verticles;

	Verticles.push_back( Vertex_t( Vector2D( screen_center.x + cosf( rot ) * radius, screen_center.y + sinf( rot ) * radius ) ) );
	Verticles.push_back( Vertex_t( Vector2D( screen_center.x + cosf( rot + DEG2RAD( 2 ) ) * ( radius - 16 ), screen_center.y + sinf( rot + DEG2RAD( 2 ) ) * ( radius - 16 ) ) ) );
	Verticles.push_back( Vertex_t( Vector2D( screen_center.x + cosf( rot - DEG2RAD( 2 ) ) * ( radius - 16 ), screen_center.y + sinf( rot - DEG2RAD( 2 ) ) * ( radius - 16 ) ) ) );


	g_pSurface->TexturedPolygon( 3, Verticles, Color( g_Menu.Config.ArrowsColor.red, g_Menu.Config.ArrowsColor.green, g_Menu.Config.ArrowsColor.blue, 255 ) ); //255, 40, 230
}

const bool ESP::calculate_box_position( C_BaseEntity * entity ) {
	Vector top, down, air;
	Vector2D s[ 2 ];

	Vector adjust = Vector( 0, 0, -15 * entity->crouch_ammount( ) );

	down = entity->GetAbsOrigin( );

	if ( !( entity->GetFlags( ) & FL_ONGROUND ) && ( entity->GetMoveType( ) != MoveType_t::MOVETYPE_LADDER ) )
		air = Vector( 0, 0, 10 );
	else
		air = Vector( 0, 0, 0 );

	down = entity->GetAbsOrigin( ) + air;

	if ( ( entity->GetFlags( ) & FL_DUCKING ) )
	{
		top = down + Vector( 0, 0, 58 ) + adjust;
	}
	else {
		top = down + Vector( 0, 0, 72 ) + adjust;
	}

	if ( Utils::WorldToScreen( top, s[ 1 ] ) && Utils::WorldToScreen( down, s[ 0 ] ) )
	{
		Vector delta = Vector( s[ 1 ].x - s[ 0 ].x, s[ 1 ].y - s[ 0 ].y, 0 );

		Box.bottom = static_cast< int >( fabsf( delta.y ) + 6 );
		Box.right = static_cast< int >( Box.bottom / 2 + 5 );

		Box.left = static_cast< int >( s[ 1 ].x - ( Box.right / 2 ) + 2 );
		Box.top = static_cast< int >( s[ 1 ].y - 1 );

		return true;
	}

	return false;
}

void ESP::Render( )
{
	if ( !g::pLocalEntity || !g_pEngine->IsInGame( ) || !g_Menu.Config.Esp )
		return;

	color = Color( g_Menu.Config.box_color.red, g_Menu.Config.box_color.green, g_Menu.Config.box_color.blue, 210 );
	textcolor = Color( g_Menu.Config.EspColor.red, g_Menu.Config.EspColor.green, g_Menu.Config.EspColor.blue, 210 );
	skelecolor = Color( g_Menu.Config.SkeletonColour.red, g_Menu.Config.SkeletonColour.green, g_Menu.Config.SkeletonColour.blue, 210 );

	for ( int i = 1; i < g_pEngine->GetMaxClients( ); ++i )
	{
		C_BaseEntity* pPlayerEntity = g_pEntityList->GetClientEntity( i );

		if ( !pPlayerEntity
			|| !pPlayerEntity->IsAlive( )
			|| pPlayerEntity->IsDormant( )
			|| pPlayerEntity == g::pLocalEntity
			|| pPlayerEntity->GetTeam( ) == g::pLocalEntity->GetTeam( ) )
			continue;

		offsetY = 0;

		if ( !calculate_box_position( pPlayerEntity ) )
			continue;

		if ( Box.bottom == 0 )
			continue;

		if ( g_Menu.Config.Skeleton_Nigga || g_Menu.Config.Skeleton_Nigga_Backtrack )
			RenderSkeleton( pPlayerEntity );

		if ( g_Menu.Config.Box )
			RenderBox( );

		if ( g_Menu.Config.HitboxPoints )
			RenderHitboxPoints( pPlayerEntity );

		if ( g_Menu.Config.Name )
			RenderName( pPlayerEntity, i );

		if ( g_Menu.Config.Money )
			RenderMoney( pPlayerEntity );

		if ( g_Menu.Config.Weapon )
			RenderWeaponName( pPlayerEntity );

		if ( g_Menu.Config.HealthBar || g_Menu.Config.HealthVal > 0 )
			RenderHealth( pPlayerEntity );

		if ( g_Menu.Config.AmmoBar )
			RenderAmmo( pPlayerEntity );

		if ( g_Menu.Config.RenderArrows )
			RenderArrows( pPlayerEntity );
	}
}