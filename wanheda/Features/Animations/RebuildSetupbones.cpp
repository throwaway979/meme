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
// wanted to remove as antipaste but tommy wanted to keep this in, give him some feedback, this is the only thing worth looking at in here anyway
	auto hdr = target->GetModelPtr( );
	if ( !hdr )
		return false;

	auto oldBones = target->GetBoneArrayForWrite( );

	matrix3x4_t baseMatrix;
	g_Math.AngleMatrix(target->GetAbsAngles(), target->GetAbsOrigin(), baseMatrix);

	target->get_effects() |= 0x008;

	auto m_pIk = *(DWORD*)((DWORD)target + 0x2670);

	if (m_pIk)
	{
		ClearTargets(m_pIk);

		typedef void(__thiscall *Initfn)(DWORD, studiohdr_t*, const Vector&, const Vector&, float, int, int);
		static auto Init = (Initfn)Utils::FindSignature("client_panorama.dll", "55 8B EC 83 EC 08 8B 45 08 56 57 8B F9 8D");//sub_10714F40(*(entity + 9836), v107, v62, v61, v104, v60, boneMask);
		Init(m_pIk, hdr, target->GetAbsAngles(), target->GetAbsOrigin(), currentTime, g_pGlobalVars->framecount, boneMask);
	}

	Vector *pos = (Vector*)(g_pMemAlloc->Alloc(sizeof(Vector[256])));
	QuaternionAligned *q = (QuaternionAligned*)(g_pMemAlloc->Alloc(sizeof(QuaternionAligned[256])));
	std::memset(pos, 0xFF, sizeof(pos));
	std::memset(q, 0xFF, sizeof(q));

	/**target->GetBoneArrayForWrite() = *pBoneToWorldOut;*/
	target->GetBoneAccessor()->SetBoneArrayForWrite(pBoneToWorldOut);

	target->StandardBlendingRules(hdr, pos, q, currentTime, boneMask);

	byte *boneComputed = (byte*)(g_pMemAlloc->Alloc(sizeof(byte[256])));
	std::memset(boneComputed, 0, sizeof(byte[256]));

	if (m_pIk)
	{
		target->UpdateIKLocks(currentTime);

		UpdateTargets(m_pIk, pos, q, target->GetBoneArrayForWrite(), &boneComputed[0]);

		target->CalculateIKLocks(currentTime);

		SolveDependencies(m_pIk, pos, q, target->GetBoneArrayForWrite(), &boneComputed[0]);
	}

	target->BuildTransformations(hdr, pos, q, baseMatrix, boneMask, &boneComputed[0]);

	target->get_effects() &= ~0x008;

	//*target->GetBoneArrayForWrite() = *oldBones;
	target->GetBoneAccessor()->SetBoneArrayForWrite(oldBones);
	return true;
}
