#include "RebuildSetupbones.h"
#define POINTERCHK( pointer ) ( pointer  && pointer !=0  && HIWORD( pointer ) )

SetupBonesRebuild g_SetupBones;
using IKInitFn = void( __thiscall* ) ( DWORD, studiohdr_t*, Vector&, Vector&, float, int, int );
extern IKInitFn IKInit;
using UpdateTargetsFn = void( __thiscall* ) ( DWORD, Vector*, Quaternion*, matrix3x4_t*, byte* );
extern UpdateTargetsFn oUpdateTargets;
using SolveDependenciesFn = void( __thiscall* ) ( DWORD, Vector*, Quaternion*, matrix3x4_t*, byte* );
extern SolveDependenciesFn oSolveDependencies;
IKInitFn oInit;
UpdateTargetsFn oUpdateTargets;
SolveDependenciesFn oSolveDependencies;

void ClearTargets( DWORD m_pIk )
{
	//??? valve pls
	int max = *( int* )( ( DWORD )m_pIk + 4080 );

	int v59 = 0;

	if ( max > 0 )
	{
		DWORD v60 = ( DWORD )( ( DWORD )m_pIk + 208 );
		do
		{
			*( int* )( v60 ) = -9999;
			v60 += 340;
			++v59;
		} while ( v59 < max );
	}
}

void Init( DWORD m_pIk, studiohdr_t* hdrs, Vector& angles, Vector& pos, float flTime, int iFramecounter, int boneMask )
{
	oInit( m_pIk, hdrs, angles, pos, flTime, iFramecounter, boneMask );
}

void SolveDependencies( DWORD m_pIk, Vector* pos, Quaternion* q, matrix3x4_t* bonearray, byte* computed )
{
	oSolveDependencies( m_pIk, pos, q, bonearray, computed );
}

void UpdateTargets( DWORD m_pIk, Vector* pos, Quaternion* q, matrix3x4_t* bonearray, byte* computed )
{
	oUpdateTargets( m_pIk, pos, q, bonearray, computed );
}

bool SetupBonesRebuild::HandleBoneSetup( C_BaseEntity* target, matrix3x4_t* pBoneToWorldOut, int boneMask, float currentTime )
{
	// no
	return true;
}