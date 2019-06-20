#include "EnginePrediction.h"
#include "..\..\SDK\CInput.h"
#include "..\..\SDK\CEntity.h"
#include "..\..\Utils\GlobalVars.h"
#include "..\..\SDK\CPrediction.h"
#include "..\..\SDK\CGlobalVarsBase.h"
#include "..\..\Menu\Menu.h"
#include "..\..\Menu\Menu.h"

float flOldCurtime;
float flOldFrametime;
int* m_nPredictionRandomSeed;
int* m_pSetPredictionPlayer;
static bool bInit = false;

void engine_prediction::RunEnginePred( )
{
	if ( !g_Menu.Config.Aimbot )
		return;

	if ( !g_pEngine->IsInGame( ) || !g_pEngine->IsConnected( ) )
		return;

	static int flTickBase;
	static CUserCmd * pLastCmd;

	if ( pLastCmd )
	{
		if ( pLastCmd->hasbeenpredicted )
			flTickBase = g::pLocalEntity->GetTickBase( );
		else
			++flTickBase;
	}

	if ( !m_nPredictionRandomSeed )
		m_nPredictionRandomSeed = *reinterpret_cast< int** >( Utils::FindSignature( "client_panorama.dll", "8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04" ) + 2 );

	// dam point la movedata

	CMoveData m_MoveData;

	// dam store la curtime si frametime ca sa le dam return la valoriile vechi o data ce ii dam end

	pLastCmd = g::pCmd;
	flOldCurtime = g_pGlobalVars->curtime;
	flOldFrametime = g_pGlobalVars->frametime;

	// schimbam curtime si frametime in tickurile in care facem pred

	g_pGlobalVars->curtime = flTickBase * g_pGlobalVars->intervalPerTick;
	g_pGlobalVars->frametime = g_pGlobalVars->intervalPerTick;

	// TPD este un concept din care iti reiese ideea ca se fac mai multe """"""erori"""""" ca sa networkezi corect hiturile

	g_pMovement->StartTrackPredictionErrors( g::pLocalEntity );

	// schimbam size

	memset( &m_MoveData, 0, sizeof( m_MoveData ) );

	// schimbam host

	g_pMoveHelper->SetHost( g::pLocalEntity );

	// setup move rebuild il folosim cu memory setul nostru

	g_pPrediction->SetupMove( g::pLocalEntity, g::pCmd, g_pMoveHelper, &m_MoveData );

	// process movement rebuilt il folosim cu CMoveData

	g_pMovement->ProcessMovement( g::pLocalEntity, &m_MoveData );

	// terminam predul

	g_pPrediction->FinishMove( g::pLocalEntity, g::pCmd, &m_MoveData );
}

void engine_prediction::EndEnginePred( )
{
	if ( !g_Menu.Config.Aimbot )
		return;

	if ( !g_pEngine->IsInGame( ) || !g_pEngine->IsConnected( ) )
		return;

	// lafel ca si sus doar ca resetam tot in schimb

	g_pMovement->FinishTrackPredictionErrors( g::pLocalEntity );
	g_pMoveHelper->SetHost( 0 );

	*m_nPredictionRandomSeed = -1;

	g_pGlobalVars->curtime = flOldCurtime;
	g_pGlobalVars->frametime = flOldFrametime;
}