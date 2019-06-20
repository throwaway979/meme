#pragma once
#include "Definitions.h"
#include "IClientUnknown.h"
#include "IClientEntityList.h"
#include "CInput.h"
#include "..\Utils\Utils.h"
#include "..\Utils\NetvarManager.h"
#include "CUtlVector.h"
#include "..\SDK\IMemAlloc.h"
#include "..\SDK\CGlobalVarsBase.h"
#include "..\Utils\GlobalVars.h"
#include "..\Utils\Utils.h"
#include "..\Utils\Math.h"
#include "..\Console\Console.hpp"
#include "..\Menu\TGFCfg.h"
#include "..\Menu\Menu.h"
#include "IVModelInfo.h"
#include <array>

class Angle_t : public Vector { };
class Quaternion;
class RadianEuler {
public:
	RadianEuler( void ) { }
	RadianEuler( float X, float Y, float Z ) { x = X; y = Y; z = Z; }
	RadianEuler( Quaternion const& q );	// evil auto type promotion!!!
	RadianEuler( Angle_t const& angles );	// evil auto type promotion!!!

											// Initialization
	inline void Init( float ix = 0.0f, float iy = 0.0f, float iz = 0.0f ) { x = ix; y = iy; z = iz; }

	//	conversion to qangle
	Angle_t ToQAngle( void ) const;
	bool IsValid( ) const;
	void Invalidate( );

	inline float* Base( ) { return &x; }
	inline const float* Base( ) const { return &x; }

	// array access...
	float operator[]( int i ) const;
	float& operator[]( int i );

	float x, y, z;
};

class Quaternion				// same data-layout as engine's vec4_t,
{								//		which is a float[4]
public:
	inline Quaternion( void ) {}
	inline Quaternion( float ix, float iy, float iz, float iw ) : x( ix ), y( iy ), z( iz ), w( iw ) { }
	inline Quaternion( RadianEuler const& angle );	// evil auto type promotion!!!

	inline void Init( float ix = 0.0f, float iy = 0.0f, float iz = 0.0f, float iw = 0.0f ) { x = ix; y = iy; z = iz; w = iw; }

	bool IsValid( ) const;
	void Invalidate( );

	bool operator==( const Quaternion& src ) const;
	bool operator!=( const Quaternion& src ) const;

	float* Base( ) { return ( float* )this; }
	const float* Base( ) const { return ( float* )this; }

	// array access...
	float operator[]( int i ) const;
	float& operator[]( int i );

	float x, y, z, w;
};

class __declspec( align( 16 ) ) QuaternionAligned : public Quaternion {
public:
	inline QuaternionAligned( void ) {};
	inline QuaternionAligned( float X, float Y, float Z, float W )
	{
		Init( X, Y, Z, W );
	}

#ifdef VECTOR_NO_SLOW_OPERATIONS

private:
	// No copy constructors allowed if we're in optimal mode
	QuaternionAligned( const QuaternionAligned& vOther );
	QuaternionAligned( const Quaternion& vOther );

#else
public:
	explicit QuaternionAligned( const Quaternion& vOther )
	{
		Init( vOther.x, vOther.y, vOther.z, vOther.w );
	}

	QuaternionAligned& operator=( const Quaternion& vOther )
	{
		Init( vOther.x, vOther.y, vOther.z, vOther.w );
		return *this;
	}

#endif
};

#define TIME_TO_TICKS( dt )		( (int)( 0.5 + (float)(dt) / g_pGlobalVars->intervalPerTick ) )
#define TICKS_TO_TIME( t )		( g_pGlobalVars->intervalPerTick *( t ) )

// de ce n-am pune un sistem nou de netvaruri doar pt un cacat gen nu?
#define NETVAR(type, name, table, netvar)                           \
    type& name##() const {                                          \
        static int _##name = g_pNetvars->GetOffset(table, netvar);     \
        return *(type*)((uintptr_t)this + _##name);                 \
	}

#define PNETVAR(type, name, table, netvar)                           \
    type* name##() const {                                          \
        static int _##name = g_pNetvars->GetOffset(table, netvar);     \
        return (type*)((uintptr_t)this + _##name);                 \
	}

#define ANETVAR(type, funcname, num, table, netvar) std::array<type, num>& funcname() \
{ \
	static int _##name = g_pNetvars->GetOffset(table, netvar); \
	return *reinterpret_cast<std::array<type, num>*>( uintptr_t( this ) + _##name ); \
}

// class predefinition
class C_BaseCombatWeapon;

enum PoseParam_t {
	STRAFE_YAW,
	STAND,
	LEAN_YAW,
	SPEED,
	LADDER_YAW,
	LADDER_SPEED,
	JUMP_FALL,
	MOVE_YAW,
	MOVE_BLEND_CROUCH,
	MOVE_BLEND_WALK,
	MOVE_BLEND_RUN,
	BODY_YAW,
	BODY_PITCH,
	AIM_BLEND_STAND_IDLE,
	AIM_BLEND_STAND_WALK,
	AIM_BLEND_STAND_RUN,
	AIM_BLEND_COURCH_IDLE,
	AIM_BLEND_CROUCH_WALK,
	DEATH_YAW
};

class C_AnimState {
public:
	char pad[ 3 ];
	char bUnknown; //0x4
	char pad2[ 91 ];
	void* pBaseEntity; //0x60
	void* pActiveWeapon; //0x64
	void* pLastActiveWeapon; //0x68
	float m_flLastClientSideAnimationUpdateTime; //0x6C
	int m_iLastClientSideAnimationUpdateFramecount; //0x70
	float m_flEyePitch; //0x74
	float m_flEyeYaw; //0x78
	float m_flPitch; //0x7C
	float m_flGoalFeetYaw; //0x80
	float m_flCurrentFeetYaw; //0x84
	float m_flCurrentTorsoYaw; //0x88
	float m_flUnknownVelocityLean; //0x8C //changes when moving/jumping/hitting ground
	float m_flLeanAmount; //0x90
	char pad4[ 4 ]; //NaN
	float m_flFeetCycle; //0x98 0 to 1
	float m_flFeetYawRate; //0x9C 0 to 1
	float m_fUnknown2;
	float m_fDuckAmount; //0xA4
	float m_fLandingDuckAdditiveSomething; //0xA8
	float m_fUnknown3; //0xAC
	Vector m_vOrigin; //0xB0, 0xB4, 0xB8
	Vector m_vLastOrigin; //0xBC, 0xC0, 0xC4
	float m_vVelocityX; //0xC8
	float m_vVelocityY; //0xCC
	char pad5[ 4 ];
	float m_flUnknownFloat1; //0xD4 Affected by movement and direction
	char pad6[ 8 ];
	float m_flUnknownFloat2; //0xE0 //from -1 to 1 when moving and affected by direction
	float m_flUnknownFloat3; //0xE4 //from -1 to 1 when moving and affected by direction
	float m_unknown; //0xE8
	float speed_2d; //0xEC
	float flUpVelocity; //0xF0
	float m_flSpeedNormalized; //0xF4 //from 0 to 1
	float m_flFeetSpeedForwardsOrSideWays; //0xF8 //from 0 to 2. something  is 1 when walking, 2.something when running, 0.653 when crouch walking
	float m_flFeetSpeedUnknownForwardOrSideways; //0xFC //from 0 to 3. something
	float m_flTimeSinceStartedMoving; //0x100
	float m_flTimeSinceStoppedMoving; //0x104
	unsigned char m_bOnGround; //0x108
	unsigned char m_bInHitGroundAnimation; //0x109
	char pad7[ 10 ];
	float m_flLastOriginZ; //0x114
	float m_flHeadHeightOrOffsetFromHittingGroundAnimation; //0x118 from 0 to 1, is 1 when standing
	float m_flStopToFullRunningFraction; //0x11C from 0 to 1, doesnt change when walking or crouching, only running
	char pad8[ 4 ]; //NaN
	float m_flUnknownFraction; //0x124 affected while jumping and running, or when just jumping, 0 to 1
	char pad9[ 4 ]; //NaN
	float m_flUnknown3;
	char pad10[ 528 ];
};

class AnimationLayer {
public:
	char  pad_0000[ 20 ];

	// These should also be present in the padding, don't see the use for it though
	//float	m_flLayerAnimtime;
	//float	m_flLayerFadeOuttime;
	unsigned int m_nOrder; //0x0014
	unsigned int m_nSequence; //0x0018
	float m_flPrevCycle; //0x001C
	float m_flWeight; //0x0020
	float m_flWeightDeltaRate; //0x0024
	float m_flPlaybackRate; //0x0028
	float m_flCycle; //0x002C
	void* m_pOwner; //0x0030 // player's thisptr
	char  pad_0038[ 4 ]; //0x0034
}; //Size: 0x0038

class C_BaseAnimating;

class CBoneAccessor {
public:

	CBoneAccessor( );
	CBoneAccessor( matrix3x4_t* pBones ); // This can be used to allow access to all bones.

	// Initialize.
	void Init( const C_BaseAnimating* pAnimating, matrix3x4_t* pBones );

	int GetReadableBones( );
	void SetReadableBones( int flags );

	int GetWritableBones( );
	void SetWritableBones( int flags );

	// Get bones for read or write access.
	const matrix3x4_t& GetBone( int iBone ) const;
	const matrix3x4_t& operator[]( int iBone ) const;
	matrix3x4_t& GetBoneForWrite( int iBone );

	matrix3x4_t* GetBoneArrayForWrite( ) const;
	void SetBoneArrayForWrite( matrix3x4_t* p_bones );

	//private:

	// Only used in the client DLL for debug verification.
	const C_BaseAnimating* m_pAnimating;

	matrix3x4_t* m_pBones;

	int m_ReadableBones;		// Which bones can be read.
	int m_WritableBones;		// Which bones can be written.
};

inline CBoneAccessor::CBoneAccessor( )
{
	m_pAnimating = NULL;
	m_pBones = NULL;
	m_ReadableBones = m_WritableBones = 0;
}

inline CBoneAccessor::CBoneAccessor( matrix3x4_t * pBones )
{
	m_pAnimating = NULL;
	m_pBones = pBones;
}

inline void CBoneAccessor::Init( const C_BaseAnimating * pAnimating, matrix3x4_t * pBones )
{
	m_pAnimating = pAnimating;
	m_pBones = pBones;
}

inline int CBoneAccessor::GetReadableBones( )
{
	return m_ReadableBones;
}

inline void CBoneAccessor::SetReadableBones( int flags )
{
	m_ReadableBones = flags;
}

inline int CBoneAccessor::GetWritableBones( )
{
	return m_WritableBones;
}

inline void CBoneAccessor::SetWritableBones( int flags )
{
	m_WritableBones = flags;
}

inline const matrix3x4_t& CBoneAccessor::GetBone( int iBone ) const
{
#if defined( CLIENT_DLL ) && defined( _DEBUG )
	SanityCheckBone( iBone, true );
#endif
	return m_pBones[ iBone ];
}

inline const matrix3x4_t& CBoneAccessor::operator[]( int iBone ) const
{
	return m_pBones[ iBone ];
}

inline matrix3x4_t& CBoneAccessor::GetBoneForWrite( int iBone )
{
	return m_pBones[ iBone ];
}

inline matrix3x4_t* CBoneAccessor::GetBoneArrayForWrite( void ) const
{
	return m_pBones;
}

inline void CBoneAccessor::SetBoneArrayForWrite( matrix3x4_t * p_bones )
{
	m_pBones = p_bones;
}

class C_BaseEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable {
private:
	template<class T>
	T GetPointer( const int offset )
	{
		return reinterpret_cast< T* >( reinterpret_cast< std::uintptr_t >( this ) + offset );
	}

	// To get value from the pointer itself
	template<class T>
	T GetValue( const int offset )
	{
		return *reinterpret_cast< T* >( reinterpret_cast< std::uintptr_t >( this ) + offset );
	}

	template <typename T>
	T& SetValue( uintptr_t offset )
	{
		return *reinterpret_cast< T* >( reinterpret_cast< uintptr_t >( this ) + offset );
	}

public:

	void StandardBlendingRules(studiohdr_t *hdr, Vector pos[], QuaternionAligned q[], float currentTime, int boneMask)
	{
		typedef void(__thiscall* OriginalFn)(C_BaseEntity*, studiohdr_t*, Vector[], QuaternionAligned[], float, int);
		Utils::GetVFunc<OriginalFn>(this, 201)(this, hdr, pos, q, currentTime, boneMask);
	}

	void BuildTransformations(studiohdr_t *hdr, Vector *pos, Quaternion *q, const matrix3x4_t &cameraTransform, int boneMask, byte*boneComputed)//CStudioHdr *hdr, Vector *pos, Quaternion *q, const matrix3x4_t &cameraTransform, int boneMask, CBoneBitList &boneComputed
	{
		Utils::GetVFunc<void(__thiscall*)(void*, studiohdr_t *, Vector *, Quaternion *, const matrix3x4_t&, int, byte*)>(this, 187)(this, hdr, pos, q, cameraTransform, boneMask, boneComputed);//(*(*v80 + 748))(hdr, &v133, &v135, &v130, boneMask, &v128);
	}

	CBoneAccessor* GetBoneAccessor( )
	{
		return ( CBoneAccessor* )( ( uintptr_t )this + 0x26A8 );
	}

	matrix3x4_t* GetBoneArrayForWrite() {
		return (matrix3x4_t*)((uintptr_t)this + 0x26A8);
	}

	void UpdateIKLocks(float currentTime)
	{
		typedef void(__thiscall* oUpdateIKLocks)(PVOID, float currentTime);
		Utils::GetVFunc<oUpdateIKLocks>(this, 187)(this, currentTime);
	}

	void CalculateIKLocks( float currentTime )
	{
		typedef void( __thiscall* oCalculateIKLocks )( PVOID, float currentTime );
		Utils::GetVFunc<oCalculateIKLocks>( this, 188 )( this, currentTime );
	}

	studiohdr_t *GetModelPtr()
	{
		return *(studiohdr_t**)((uintptr_t)this + 0x294C);
	}

	//this probably contains a lot of mistakes
	bool RebuildBones( matrix3x4_t *boneout, int bonemask, float curtime );

	C_AnimState* AnimState( )
	{
		return *reinterpret_cast< C_AnimState** >( uintptr_t( this ) + 0x3900 );
	}

	uintptr_t& get_inverse_kinematics( void ) {
		return *( uintptr_t* )( ( uintptr_t )this + 0x68 );
	}

	uintptr_t& get_effects( void ) {
		return *( uintptr_t* )( ( uintptr_t )this + 0xEC );
	}
	CBaseHandle *GetWeapons()
	{
		return (CBaseHandle*)((DWORD)this + 0x2DE8);
	}
	int& getTakeDamage( ) {
		return *reinterpret_cast< int* >( uintptr_t( this ) + 0x280 );
	}

	AnimationLayer* AnimOverlays( )
	{
		return *reinterpret_cast< AnimationLayer** >( uintptr_t( this ) + 0x2980 );
	}

	CBaseHandle* m_hMyWeapons( )
	{
		return ( CBaseHandle* )( ( uintptr_t )this + 0x2DF8 );
	}
	
	CBaseHandle GetWearables( )
	{
		return *( CBaseHandle* )( ( uintptr_t )this + 0x2F04 );
	}

	HANDLE m_hWeaponWorldModel_h( )
	{
		return *( HANDLE* )( ( uintptr_t )this + 0x3234 );
	}

	bool IsVisible( Vector origin );

	CBaseHandle m_hWeaponWorldModel_c( )
	{
		return *( CBaseHandle* )( ( uintptr_t )this + 0x3234 );
	}

	short* ItemDefinitionIndex2()
	{
		return (short*)((DWORD)this + g_pNetvars->GetOffset("DT_BaseAttributableItem", "m_AttributeManager", "m_Item", "m_iItemDefinitionIndex"));
	}

	void SetModelIndex(const int index)
	{
		Utils::GetVFunc<void(__thiscall*)(C_BaseEntity*, int)>(this, 75)(this, index);
	}

	int* ModelIndex( )
	{
		return ( int* )( ( uintptr_t )this + 0x258 );
	}

	int* ViewModelIndex( )
	{
		return ( int* )( ( uintptr_t )this + 0x3220 );
	}

	short* SetSkin( )
	{
		return ( short* )( ( uintptr_t )this + 0x2FAA );
	}

	int* OwnerXuidLow()
	{
		return (int*)((uintptr_t)this + 0x31B0);
	}

	inline int* GetFallbackPaintKit() {
		return (int*)((DWORD)this + 0x31B8);
	}

	int* OwnerXuidHigh()
	{
		return (int*)((uintptr_t)this + 0x31B4);
	}

	float* FallbackWear()
	{
		return (float*)((uintptr_t)this + 0x31C0);
	}
	int* FallbackSeed()
	{
		return (int*)((uintptr_t)this + 0x31AC);
	}

	int& GetItemIDHigh()
	{
		return *(int *)((DWORD)this + g_pNetvars->GetOffset("DT_BaseAttributableItem", "m_AttributeManager", "m_Item", "m_iItemIDHigh"));
	}

	int& GetItemIDLow()
	{
		return *(int *)((DWORD)this + g_pNetvars->GetOffset("DT_BaseAttributableItem", "m_AttributeManager", "m_Item", "m_iItemIDLow"));
	}

	int* GetEntityQuality( )
	{
		return ( int* )( ( uintptr_t )this + 0x2FAC );
	}

	inline int* FallbackPaintKit( )
	{
		return ( int* )( ( DWORD )this + 0x31B8 );
	}

	int* ItemIDHigh( )
	{
		return ( int* )( ( uintptr_t )this + 0x2FC0 );
	}

	int NumOverlays( )
	{
		return 15;
	}

	void UpdateClientAnimation( )
	{
		Utils::GetVFunc<void( __thiscall* )( void* )>( this, 221 )( this );
	}

	void ClientAnimations( bool value )
	{
		static int m_bClientSideAnimation = g_pNetvars->GetOffset( "DT_BaseAnimating", "m_bClientSideAnimation" );
		*reinterpret_cast< bool* >( uintptr_t( this ) + m_bClientSideAnimation ) = value;
	}

	int GetSequence( )
	{
		static int m_nSequence = g_pNetvars->GetOffset( "DT_BaseAnimating", "m_nSequence" );
		return GetValue<int>( m_nSequence );
	}

	float crouch_ammount( )
	{
		static int m_fNigger = g_pNetvars->GetOffset( "DT_BasePlayer", "m_flDuckAmount" );
		return GetValue<float>( m_fNigger );
	}

	void SetSequence( int Sequence )
	{
		static int m_nSequence = g_pNetvars->GetOffset( "DT_BaseAnimating", "m_nSequence" );
		*reinterpret_cast< int* >( uintptr_t( this ) + m_nSequence ) = Sequence;
	}

	void SimulatedEveryTick( bool value )
	{
		static int m_bSimulatedEveryTick = g_pNetvars->GetOffset( "DT_BaseEntity", "m_bSimulatedEveryTick" );
		*reinterpret_cast< bool* >( uintptr_t( this ) + m_bSimulatedEveryTick ) = value;
	}

	void SetAbsAngles( Vector angles );
	void SetAbsOrigin( Vector origin );

	Vector& GetAbsAngles( )
	{
		typedef Vector& ( __thiscall * OriginalFn )( void* );
		return ( ( OriginalFn )Utils::GetVFunc<OriginalFn>( this, 11 ) )( this );
	}

	Vector GetAbsOrigin( )
	{
		return Utils::GetVFunc<Vector& ( __thiscall* )( void* )>( this, 10 )( this );
	}

	void SetAbsVelocity( Vector velocity );

	C_BaseCombatWeapon* GetActiveWeapon( )
	{
		static int m_hActiveWeapon = g_pNetvars->GetOffset( "DT_BaseCombatCharacter", "m_hActiveWeapon" );
		const auto weaponData = GetValue<CBaseHandle>( m_hActiveWeapon );
		return reinterpret_cast< C_BaseCombatWeapon* >( g_pEntityList->GetClientEntityFromHandle( weaponData ) );
	}

	int GetActiveWeaponIndex( )
	{
		static int m_hActiveWeapon = g_pNetvars->GetOffset( "DT_BaseCombatCharacter", "m_hActiveWeapon" );
		return *reinterpret_cast< int* >( uintptr_t( this ) + m_hActiveWeapon ) & 0xFFF;
	}

	int GetTeam( )
	{
		static int m_iTeamNum = g_pNetvars->GetOffset( "DT_BaseEntity", "m_iTeamNum" );
		return GetValue<int>( m_iTeamNum );
	}

	EntityFlags GetFlags( )
	{
		static int m_fFlags = g_pNetvars->GetOffset( "DT_BasePlayer", "m_fFlags" );
		return GetValue<EntityFlags>( m_fFlags );
	}

	void SetFlags( int offset )
	{
		static int m_fFlags = g_pNetvars->GetOffset( "DT_BasePlayer", "m_fFlags" );
		*reinterpret_cast< int* >( uintptr_t( this ) + m_fFlags ) = offset;
	}

	datamap_t *GetPredDescMap()
	{
		typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
		return Utils::GetVFunc<o_GetPredDescMap>(this, 17)(this);
	}

	uint32_t &m_iEFlags()
	{
		static unsigned int _m_iEFlags = Utils::FindInDataMap(GetPredDescMap(), "m_iEFlags");
		return *(uint32_t*)((uintptr_t)this + _m_iEFlags);
	}

	void InvalidateBoneCache( ) {
		static auto InvalidateBoneCache = Utils::FindSignature( "client_panorama.dll", "80 3D ?? ?? ?? ?? ?? 74 16 A1 ?? ?? ?? ?? 48 C7 81" );
		static auto ModelBoneCounter = **reinterpret_cast< uintptr_t * * >( InvalidateBoneCache + 10 );
		*reinterpret_cast< unsigned int* >( uintptr_t( this ) + 0x2924 ) = 0xFF7FFFFF; // m_flLastBoneSetupTime = -FLT_MAX;
		*reinterpret_cast< unsigned int* >( uintptr_t( this ) + 0x2690 ) = ( ModelBoneCounter - 1 ); // m_iMostRecentModelBoneCounter = g_iModelBoneCounter - 1;
	}

	MoveType_t GetMoveType( )
	{
		static int m_Movetype = g_pNetvars->GetOffset( "DT_BaseEntity", "m_nRenderMode" ) + 1;
		return GetValue<MoveType_t>( m_Movetype );
	}

	NETVAR( float, m_flFlashDuration, "DT_CSPlayer", "m_flFlashDuration" ); // imi urasc viata

	NETVAR( int, GetMoney, "DT_CSPlayer", "m_iAccount" );

	float GetSimulationTime( )
	{
		static int m_flSimulationTime = g_pNetvars->GetOffset( "DT_BaseEntity", "m_flSimulationTime" );
		return GetValue<float>( m_flSimulationTime );
	}
	matrix3x4_t GetBoneMatrix( int BoneID )
	{
		matrix3x4_t matrix;

		auto offset = *reinterpret_cast< uintptr_t* >( uintptr_t( this ) + 0x26A8 );
		if ( offset )
			matrix = *reinterpret_cast< matrix3x4_t* >( offset + 0x30 * BoneID );

		return matrix;
	}
	float GetOldSimulationTime( )
	{
		static int m_flOldSimulationTime = g_pNetvars->GetOffset( "DT_BaseEntity", "m_flSimulationTime" ) + 4;
		return GetValue<float>( m_flOldSimulationTime );
	}

	float GetLowerBodyYaw( )
	{
		static int m_flLowerBodyYawTarget = g_pNetvars->GetOffset( "DT_CSPlayer", "m_flLowerBodyYawTarget" );
		return GetValue<float>( m_flLowerBodyYawTarget );
	}

	//3 different funcs for posparams cause fuck you

	float GetPoseParams( )
	{
		static int offset = g_pNetvars->GetOffset( "CBaseAnimating", "m_flPoseParameter" );
		return GetValue<float>( offset );
	}

	float* GetPoseParamsModifiable( )
	{
		auto offset = 0x2774;
		return reinterpret_cast< float* >( uintptr_t( this ) + offset );
	}

	std::array<float, 24>& GetPoseParam( )
	{
		static int offset = g_pNetvars->GetOffset( "CBaseAnimating", "m_flPoseParameter" );
		return *( std::array<float, 24>* )( ( uintptr_t )this + offset );
	}

	void SetLowerBodyYaw( float value )
	{
		static int m_flLowerBodyYawTarget = g_pNetvars->GetOffset( "DT_CSPlayer", "m_flLowerBodyYawTarget" );
		*reinterpret_cast< float* >( uintptr_t( this ) + m_flLowerBodyYawTarget ) = value;
	}

	bool GetHeavyArmor( )
	{
		static int m_bHasHeavyArmor = g_pNetvars->GetOffset( "DT_CSPlayer", "m_bHasHeavyArmor" );
		return GetValue<bool>( m_bHasHeavyArmor );
	}

	int ArmorValue( )
	{
		static int m_ArmorValue = g_pNetvars->GetOffset( "DT_CSPlayer", "m_ArmorValue" );
		return GetValue<int>( m_ArmorValue );
	}

	bool HasHelmet( )
	{
		static int m_bHasHelmet = g_pNetvars->GetOffset( "DT_CSPlayer", "m_bHasHelmet" );
		return GetValue<bool>( m_bHasHelmet );
	}

	bool GetLifeState( )
	{
		static int m_lifeState = g_pNetvars->GetOffset( "DT_BasePlayer", "m_lifeState" );
		return GetValue<bool>( m_lifeState );
	}

	bool IsScoped( )
	{
		static int m_bIsScoped = g_pNetvars->GetOffset( "DT_CSPlayer", "m_bIsScoped" );
		return GetValue<bool>( m_bIsScoped );
	}

	int GetHealth( )
	{
		static int m_iHealth = g_pNetvars->GetOffset( "DT_BasePlayer", "m_iHealth" );
		return GetValue<int>( m_iHealth );
	}

	bool IsKnifeorNade( );
	bool IsNade( );
	bool IsPistol( );

	bool IsAlive( ) { return this->GetHealth( ) > 0 && this->GetLifeState( ) == 0; }

	uintptr_t ObserverTarget( )
	{
		static int dadd = g_pNetvars->GetOffset( "DT_BasePlayer", "m_hObserverTarget" );
		return GetValue<uintptr_t>( dadd );
	}

	bool IsEnemy( );

	bool IsImmune( )
	{
		static int m_bGunGameImmunity = g_pNetvars->GetOffset( "DT_CSPlayer", "m_bGunGameImmunity" );
		return GetValue<bool>( m_bGunGameImmunity );
	}

	NETVAR( float, LaggedMovementValue, "DT_CSPlayer", "m_flLaggedMovementValue" );
	NETVAR( float, NextAttack, "DT_CSPlayer", "m_flNextAttack" );
	float C4Timer( )
	{
		static int m_flC4Blow = g_pNetvars->GetOffset( "DT_PlantedC4", "m_flC4Blow" );
		return GetValue<float>( m_flC4Blow );
	}

	int GetTickBase( )
	{
		static int m_nTickBase = g_pNetvars->GetOffset( "DT_BasePlayer", "localdata", "m_nTickBase" );
		return GetValue<int>( m_nTickBase );
	}

	int GetShotsFired( )
	{
		static int m_iShotsFired = g_pNetvars->GetOffset( "DT_CSPlayer", "cslocaldata", "m_iShotsFired" );
		return GetValue<int>( m_iShotsFired );
	}

	void SetTickBase( int TickBase )
	{
		static int m_nTickBase = g_pNetvars->GetOffset( "DT_BasePlayer", "localdata", "m_nTickBase" );
		*reinterpret_cast< int* >( uintptr_t( this ) + m_nTickBase ) = TickBase;
	}

	Vector GetEyeAngles( )
	{
		static int m_angEyeAngles = g_pNetvars->GetOffset( "DT_CSPlayer", "m_angEyeAngles" );
		return GetValue<Vector>( m_angEyeAngles );
	}

	template<class T>
	T* GetFieldPointer( int offset ) {
		return ( T* )( ( DWORD )this + offset );
	}

	Vector* GetEyeAnglesPointer( )
	{
		static int m_angEyeAngles = g_pNetvars->GetOffset( "DT_CSPlayer", "m_angEyeAngles" );
		return GetFieldPointer<Vector>( m_angEyeAngles );
	}

	float GetNetworkLatency( )
	{
		INetChannelInfo* nci = g_pEngine->GetNetChannelInfo( );
		if ( nci )
		{
			return nci->GetAvgLatency( FLOW_INCOMING );
		}
		return 0.0f;
	}

	int GetNumberOfTicksChoked( )
	{
		float flSimulationTime = this->GetSimulationTime( );
		float flSimDiff = g_pGlobalVars->curtime - flSimulationTime;
		float latency = GetNetworkLatency( );
		return TIME_TO_TICKS( max( 0.0f, flSimDiff - latency ) );
	}

	void SetEyeAngles( Vector Angle )
	{
		static int m_angEyeAngles = g_pNetvars->GetOffset( "DT_CSPlayer", "m_angEyeAngles" );
		*reinterpret_cast< Vector* >( uintptr_t( this ) + m_angEyeAngles ) = Angle;
	}

	Vector GetOrigin( )
	{
		static int m_vecOrigin = g_pNetvars->GetOffset( "DT_BaseEntity", "m_vecOrigin" );
		return GetValue<Vector>( m_vecOrigin );
	}

	Vector m_vecViewOffset( )
	{
		return *reinterpret_cast< Vector* >( uintptr_t( this ) + uintptr_t( 0x108 ) );
	}

	Vector GetOldOrigin( )
	{
		static int m_vecOldOrigin = g_pNetvars->GetOffset( "DT_BasePlayer", "localdata", "m_flFriction" ) + 12;
		return GetValue<Vector>( m_vecOldOrigin );
	}

	Vector GetNetworkOrigin( )
	{
		static int m_vecNetworkOrigin = g_pNetvars->GetOffset( "DT_BasePlayer", "localdata", "m_flFriction" ) + 40;
		return GetValue<Vector>( m_vecNetworkOrigin );
	}

	void SetOrigin( Vector Origin )
	{
		static int m_vecOrigin = g_pNetvars->GetOffset( "DT_BaseEntity", "m_vecOrigin" );
		*reinterpret_cast< Vector* >( uintptr_t( this ) + m_vecOrigin ) = Origin;
	}

	Vector GetVelocity( )
	{
		static int m_vecVelocity = g_pNetvars->GetOffset( "DT_BasePlayer", "localdata", "m_vecVelocity[0]" );
		return GetValue<Vector>( m_vecVelocity );
	}

	void SetVelocity( Vector velocity )
	{
		static int m_vecVelocity = g_pNetvars->GetOffset( "DT_BasePlayer", "localdata", "m_vecVelocity[0]" );
		*reinterpret_cast< Vector* >( uintptr_t( this ) + m_vecVelocity ) = velocity;
	}

	Vector GetAimPunchAngle( )
	{
		return *reinterpret_cast< Vector* >( uintptr_t( this ) + uintptr_t( 0x302C ) );
	}

	Vector GetViewPunchAngle( )
	{
		return *reinterpret_cast< Vector* >( uintptr_t( this ) + uintptr_t( 0x3020 ) );
	}

	Vector GetPunchAngles( ) { return *reinterpret_cast< Vector* >( uintptr_t( this ) + 0x302C ); }

	Vector GetEyePosition( )
	{
		Vector ret;
		typedef void( __thiscall * OrigFn )( void*, Vector& );
		Utils::GetVFunc<OrigFn>( this, 281 )( this, ret );
		return ret;
	}

	ICollideable* GetCollideable( )
	{
		return ( ICollideable* )( ( DWORD )this + 0x318 );
	}

	void SetCurrentCommand( CUserCmd * cmd )
	{
		static int m_hConstraintEntity = g_pNetvars->GetOffset( "DT_BasePlayer", "localdata", "m_hConstraintEntity" );
		*reinterpret_cast< CUserCmd** >( uintptr_t( this ) + m_hConstraintEntity - 0xC ) = cmd;
	}

	float GetMaxDesyncDelta( ) {
		/*auto animstate = this->AnimState( );

		float duckammount = animstate->m_fDuckAmount;
		float speedfraction = max( 0, min( animstate->m_flFeetSpeedForwardsOrSideWays, 1 ) );

		float speedfactor = max( 0, min( 1, animstate->m_flFeetSpeedUnknownForwardOrSideways ) );

		float unk1 = ( ( animstate->m_flStopToFullRunningFraction * -0.30000001 ) - 0.19999999 ) * speedfraction;
		float unk2 = unk1 + 1.f;

		if ( duckammount > 0 )
			unk2 += ( ( duckammount * speedfactor ) * ( 0.5f - unk2 ) );

		return *( float * )( animstate + 0x334 ) * unk2;*/

		auto animstate = uintptr_t( this->AnimState( ) );

		float duckammount = *( float* )( animstate + 0xA4 );
		float speedfraction = max( 0, min( *reinterpret_cast< float* >( animstate + 0xF8 ), 1 ) );

		float speedfactor = max( 0, min( 1, *reinterpret_cast< float* > ( animstate + 0xFC ) ) );

		float unk1 = ( ( *reinterpret_cast< float* > ( animstate + 0x11C ) * -0.30000001 ) - 0.19999999 ) * speedfraction;
		float unk2 = unk1 + 1.f;
		float unk3;

		if ( duckammount > 0 )
			unk2 += ( ( duckammount * speedfactor ) * ( 0.5f - unk2 ) );

		unk3 = *( float* )( animstate + 0x334 ) * unk2;

		return unk3;
	}

	// get max desync delta
	float eso_delta( )
	{
		float v1; // xmm0_4
		float v2; // xmm1_4
		float v3; // xmm0_4
		int* v4; // eax
		float v5; // xmm4_4
		float v6; // xmm2_4
		float v7; // xmm0_4
		int* v8; // eax
		float v10; // [esp+0h] [ebp-Ch]
		float v11; // [esp+4h] [ebp-8h]
		int v12; // [esp+8h] [ebp-4h]

		float* animstate = ( float* )( ( uintptr_t )this->AnimState( ) );
		v1 = animstate[ 62 ];
		v2 = 1.0f;
		v12 = 0x3F800000;
		v10 = 0.0;
		v11 = v1;
		if ( v1 <= 1.0f )
		{
			v4 = ( int* )& v10;
			if ( v1 >= 0.0 )
				v4 = ( int* )& v11;
			v3 = *( float* )v4;
		}
		else
		{
			v3 = 1.0f;
		}
		v5 = animstate[ 41 ];
		v6 = ( float )( ( float )( ( float )( animstate[ 71 ] * -0.3f ) - 0.2f ) * v3 ) + 1.0f;
		if ( v5 > 0.0 )
		{
			v7 = animstate[ 63 ];
			v12 = 0x3F800000;
			v11 = 0.0;
			v10 = v7;
			if ( v7 <= 1.0f )
			{
				v8 = ( int* )& v11;
				if ( v7 >= 0.0 )
					v8 = ( int* )& v10;
				v2 = *( float* )v8;
			}
			v6 = v6 + ( float )( ( float )( v2 * v5 ) * ( float )( 0.5f - v6 ) );
		}
		return animstate[ 205 ] * v6;
	}

	Vector bone_pos(int i) {
		matrix3x4_t bone_matrix[128];
		this->FixSetupBones(bone_matrix);
		if(bone_matrix)
			return Vector(bone_matrix[i][0][3], bone_matrix[i][1][3], bone_matrix[i][2][3]);
		return Vector(0, 0, 0);
	}

	float FireRate( );

	void FixSetupBones( matrix3x4_t * Matrix );
	Vector GetHitboxPosition( int Hitbox, matrix3x4_t * Matrix, float* Radius );
	Vector GetHitbox( int Hitbox );
	Vector GetHitboxPosition( int Hitbox, matrix3x4_t * Matrix );
	Vector GetHitboxPosition( C_BaseEntity* entity, int hitbox_id );


};

class C_BaseCombatWeapon : public C_BaseEntity {
private:
	template<class T>
	T GetPointer( const int offset )
	{
		return reinterpret_cast< T* >( reinterpret_cast< std::uintptr_t >( this ) + offset );
	}

	// To get value from the pointer itself
	template<class T>
	T GetValue( const int offset )
	{
		return *reinterpret_cast< T* >( reinterpret_cast< std::uintptr_t >( this ) + offset );
	}

public:

	ItemDefinitionIndex GetItemDefinitionIndex( )
	{
		static int m_iItemDefinitionIndex = g_pNetvars->GetOffset( "DT_BaseAttributableItem", "m_AttributeManager", "m_Item", "m_iItemDefinitionIndex" );
		return GetValue<ItemDefinitionIndex>( m_iItemDefinitionIndex );
	}

	NETVAR( short, IsItemDefinitionIndex, "DT_WeaponBaseItem", "m_iItemDefinitionIndex" );

	float GetInaccuracy( )
	{
		typedef float( __thiscall * oInaccuracy )( PVOID );
		return Utils::GetVFunc< oInaccuracy >( this, 476 )( this );
	}

	float GetSpread( )
	{
		typedef float( __thiscall * oWeaponSpread )( PVOID );
		return Utils::GetVFunc< oWeaponSpread >( this, 446 )( this );
	}

	float GetAccuracyPenalty( )
	{
		static int m_fAccuracyPenalty = g_pNetvars->GetOffset( "DT_WeaponCSBase", "m_fAccuracyPenalty" );
		return GetValue<float>( m_fAccuracyPenalty );
	}

	int GetFireReadyTime( )
	{
		static int m_flPostponeFireReadyTime = g_pNetvars->GetOffset( "DT_WeaponCSBase", "m_flPostponeFireReadyTime" );
		return GetValue<int>( m_flPostponeFireReadyTime );
	}

	bool isSniper( ) {
		auto itemIndex = this->GetItemDefinitionIndex( );
		switch ( itemIndex )
		{
		case ItemDefinitionIndex::WEAPON_AWP:
		case ItemDefinitionIndex::WEAPON_SSG08:
		case ItemDefinitionIndex::WEAPON_SCAR20:
		case ItemDefinitionIndex::WEAPON_G3SG1:
		return true;
		default:
		return false;
		}
	}

	float GetLastShotTime( )
	{
		static int m_fLastShotTime = g_pNetvars->GetOffset( "DT_WeaponCSBase", "m_fLastShotTime" );
		return GetValue<float>( m_fLastShotTime );
	}

	void AccuracyPenalty( )
	{
		typedef void( __thiscall * OrigFn )( void* );
		return Utils::GetVFunc<OrigFn>( this, 477 )( this );
	}

	float GetNextPrimaryAttack( )
	{
		static int m_flNextPrimaryAttack = g_pNetvars->GetOffset( "DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_flNextPrimaryAttack" );
		return GetValue<float>( m_flNextPrimaryAttack );
	}

	int GetAmmo( )
	{
		static int m_iClip1 = g_pNetvars->GetOffset( "DT_BaseCombatWeapon", "m_iClip1" );
		return GetValue<int>( m_iClip1 );
	}

	WeaponInfo_t* GetCSWpnData( )
	{
		return Utils::CallVFunc<454, WeaponInfo_t*>( this );
	}

	std::string GetName( )
	{
		///TODO: Test if szWeaponName returns proper value for m4a4 / m4a1-s or it doesnt recognize them.
		return std::string( this->GetCSWpnData( )->weapon_name );
	}

	char* GetNameWeaponIcon( ) {
		auto id = GetItemDefinitionIndex( );
		if ( !this )
			return "";
		switch ( id )
		{
		case ItemDefinitionIndex::WEAPON_DEAGLE:
			return "F";
		case ItemDefinitionIndex::WEAPON_KNIFE:
		case ItemDefinitionIndex::WEAPON_KNIFE_T:
			return "J";

		case ItemDefinitionIndex::WEAPON_AUG:
			return "E";


		case ItemDefinitionIndex::WEAPON_G3SG1:
			return "I";

		case ItemDefinitionIndex::WEAPON_MAC10:
			return "L";

		case ItemDefinitionIndex::WEAPON_P90:
			return "M";

		case ItemDefinitionIndex::WEAPON_SSG08:
			return "N";

		case ItemDefinitionIndex::WEAPON_SCAR20:
			return "O";

		case ItemDefinitionIndex::WEAPON_UMP45:
			return "Q";

		case ItemDefinitionIndex::WEAPON_ELITE:
			return "S";

		case ItemDefinitionIndex::WEAPON_FAMAS:
			return "T";

		case ItemDefinitionIndex::WEAPON_FIVESEVEN:
			return "U";

		case ItemDefinitionIndex::WEAPON_GALILAR:
			return "V";

		case ItemDefinitionIndex::WEAPON_M4A1:
			return "W";

		case ItemDefinitionIndex::WEAPON_P250:
			return "Y";

		case ItemDefinitionIndex::WEAPON_M249:
			return "Z";

		case ItemDefinitionIndex::WEAPON_XM1014:
			return "]";

		case ItemDefinitionIndex::WEAPON_C4:
			return "d";

		case ItemDefinitionIndex::WEAPON_GLOCK:
			return "C";

		case ItemDefinitionIndex::WEAPON_HKP2000:
			return "Y";
		default:
			return "";
		}
	}
 };

class C_BaseViewModel
{
public:

	inline DWORD GetOwner() {

		return *(PDWORD)((DWORD)this + 0x29CC);
	}

	inline int GetModelIndex() {

		return *(int*)((DWORD)this + 0x258);
	}
};

class CBaseWeaponWorldModel : public C_BaseEntity
{
public:
	inline int* GetModelIndex() {

		return (int*)((DWORD)this + 0x258);
	}
};

class CGlowObjectManager {
public:

	int RegisterGlowObject( C_BaseEntity* pEntity, const Vector& vGlowColor, float flGlowAlpha, bool bRenderWhenOccluded, bool bRenderWhenUnoccluded, int nSplitScreenSlot ) {
		int nIndex;
		if ( m_nFirstFreeSlot == GlowObjectDefinition_t::END_OF_FREE_LIST ) {
			nIndex = m_GlowObjectDefinitions.AddToTail( );
		}
		else {
			nIndex = m_nFirstFreeSlot;
			m_nFirstFreeSlot = m_GlowObjectDefinitions[ nIndex ].m_nNextFreeSlot;
		}

		m_GlowObjectDefinitions[ nIndex ].m_hEntity = pEntity;
		m_GlowObjectDefinitions[ nIndex ].m_vGlowColor = vGlowColor;
		m_GlowObjectDefinitions[ nIndex ].m_flGlowAlpha = flGlowAlpha;
		m_GlowObjectDefinitions[ nIndex ].flUnk = 0.0f;
		m_GlowObjectDefinitions[ nIndex ].m_flBloomAmount = 1.0f;
		m_GlowObjectDefinitions[ nIndex ].localplayeriszeropoint3 = 0.0f;
		m_GlowObjectDefinitions[ nIndex ].m_bRenderWhenOccluded = bRenderWhenOccluded;
		m_GlowObjectDefinitions[ nIndex ].m_bRenderWhenUnoccluded = bRenderWhenUnoccluded;
		m_GlowObjectDefinitions[ nIndex ].m_bFullBloomRender = false;
		m_GlowObjectDefinitions[ nIndex ].m_nFullBloomStencilTestValue = 0;
		m_GlowObjectDefinitions[ nIndex ].m_nSplitScreenSlot = nSplitScreenSlot;
		m_GlowObjectDefinitions[ nIndex ].m_nNextFreeSlot = GlowObjectDefinition_t::ENTRY_IN_USE;

		return nIndex;
	}

	void UnregisterGlowObject( int nGlowObjectHandle ) {
		Assert( !m_GlowObjectDefinitions[ nGlowObjectHandle ].IsUnused( ) );

		m_GlowObjectDefinitions[ nGlowObjectHandle ].m_nNextFreeSlot = m_nFirstFreeSlot;
		m_GlowObjectDefinitions[ nGlowObjectHandle ].m_hEntity = NULL;
		m_nFirstFreeSlot = nGlowObjectHandle;
	}

	int HasGlowEffect( C_BaseEntity* pEntity ) const {
		for ( int i = 0; i < m_GlowObjectDefinitions.Count( ); ++i ) {
			if ( !m_GlowObjectDefinitions[ i ].IsUnused( ) && m_GlowObjectDefinitions[ i ].m_hEntity == pEntity ) {
				return i;
			}
		}

		return NULL;
	}

	class GlowObjectDefinition_t {
	public:
		void Set( float r, float g, float b, float a, float bloom, int style ) {
			m_vGlowColor = Vector( r, g, b );
			m_flGlowAlpha = a;
			m_bRenderWhenOccluded = true;
			m_bRenderWhenUnoccluded = false;
			m_flBloomAmount = bloom;
			m_nGlowStyle = style;
		}

		C_BaseEntity* GetEnt( ) {
			return m_hEntity;
		}

		bool IsUnused( ) const { return m_nNextFreeSlot != GlowObjectDefinition_t::ENTRY_IN_USE; }

	public:
		C_BaseEntity* m_hEntity;
		Vector            m_vGlowColor;
		float            m_flGlowAlpha;

		char            unknown[ 4 ]; //pad
		float            flUnk; //confirmed to be treated as a float while reversing glow functions
		float            m_flBloomAmount;
		float            localplayeriszeropoint3;

		bool            m_bRenderWhenOccluded;
		bool            m_bRenderWhenUnoccluded;
		bool            m_bFullBloomRender;
		char            unknown1[ 1 ]; //pad

		int                m_nFullBloomStencilTestValue; // 0x28
		int                m_nGlowStyle;
		int                m_nSplitScreenSlot; //Should be -1

											   // Linked list of free slots
		int                m_nNextFreeSlot;

		// Special values for GlowObjectDefinition_t::m_nNextFreeSlot
		static const int END_OF_FREE_LIST = -1;
		static const int ENTRY_IN_USE = -2;
	};

	CUtlVector< GlowObjectDefinition_t > m_GlowObjectDefinitions;
	int m_nFirstFreeSlot;
};

extern CGlowObjectManager* g_pGlow;

template<typename T>
class singleton {
protected:
	singleton( ) { }
	~singleton( ) { }

	singleton( const singleton& ) = delete;
	singleton& operator=( const singleton& ) = delete;

	singleton( singleton&& ) = delete;
	singleton& operator=( singleton&& ) = delete;
public:
	static T& get( ) {
		static T inst{};
		return inst;
	}
};