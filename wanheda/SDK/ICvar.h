#pragma once

// ConVar flags
#define FCVAR_NONE				0

#define FCVAR_UNREGISTERED		(1<<0)
#define FCVAR_DEVELOPMENTONLY	(1<<1)
#define FCVAR_GAMEDLL			(1<<2)
#define FCVAR_CLIENTDLL			(1<<3)
#define FCVAR_HIDDEN			(1<<4)

#define FCVAR_PROTECTED			(1<<5)
#define FCVAR_SPONLY			(1<<6)
#define	FCVAR_ARCHIVE			(1<<7)
#define	FCVAR_NOTIFY			(1<<8)
#define	FCVAR_USERINFO			(1<<9)
#define FCVAR_CHEAT				(1<<14)

#define FCVAR_PRINTABLEONLY		(1<<10)
#define FCVAR_UNLOGGED			(1<<11)
#define FCVAR_NEVER_AS_STRING	(1<<12)
#define FCVAR_RELEASE			(1<<19)

#define FCVAR_REPLICATED		(1<<13)
#define FCVAR_DEMO				(1<<16)
#define FCVAR_DONTRECORD		(1<<17)

#define FCVAR_NOT_CONNECTED		(1<<22)
#define FCVAR_ARCHIVE_XBOX		(1<<24)
#define FCVAR_SERVER_CAN_EXECUTE	(1<<28)
#define FCVAR_SERVER_CANNOT_QUERY	(1<<29)
#define FCVAR_CLIENTCMD_CAN_EXECUTE	(1<<30)

class ConVar {
public:

	//void SetString(const char *pValue)
	//{
	//	typedef void(__thiscall* SetStringFn)(void*, const char *);
	//	call_vfunc<SetStringFn>(this, 17)(this, pValue);
	//}

	void SetValue( const char* str )
	{
		typedef void( __thiscall * Fn )( void*, const char* );
		return Utils::GetVFunc<Fn>( this, 13 )( this, str );
	}
	void SetValue( float value )
	{
		typedef void( __thiscall * Fn )( void*, float );
		return Utils::GetVFunc<Fn>( this, 15 )( this, value );
	}
	void SetValue( int value )
	{
		typedef void( __thiscall * Fn )( void*, int );
		return Utils::GetVFunc<Fn>( this, 16 )( this, value );
	}
	void SetValueChar( const char* name )
	{
		typedef void( __thiscall * Fn )( void*, const char* );
		return Utils::GetVFunc<Fn>( this, 14 )( this, name );
	}
	void InternalSetString( const char* str )
	{
		typedef void( __thiscall * Fn )( void*, const char* );
		return Utils::GetVFunc<Fn>( this, 17 )( this, str );
	}
	char* GetBaseName( )
	{
		typedef char* ( __thiscall * Fn )( void* );
		return Utils::GetVFunc<Fn>( this, 6 )( this );
	}
	float GetFloat( )
	{
		typedef float( __thiscall * Fn )( void* );
		return Utils::GetVFunc< Fn >( this, 12 )( this );
	}
	int GetInt( void ) const {
		return pParent->nValue;
	}
	const char* GetString( void ) const {
		return pParent->pszDefaultValue;
	}

	char pad_0x0000[ 0x4 ]; //0x0000
	ConVar* pNext; //0x0004
	__int32 bRegistered; //0x0008
	char* pszName; //0x000C
	char* pszHelpString; //0x0010
	__int32 nFlags; //0x0014
	char pad_0x0018[ 0x4 ]; //0x0018
	ConVar* pParent; //0x001C
	char* pszDefaultValue; //0x0020
	char* strString; //0x0024
	__int32 StringLength; //0x0028
	float fValue; //0x002C
	__int32 nValue; //0x0030
	__int32 bHasMin; //0x0034
	float fMinVal; //0x0038
	__int32 bHasMax; //0x003C
	float fMaxVal; //0x0040
	void* fnChangeCallback; //0x0044
};//Size=0x0048

class IAppSystem {
public:
	virtual ~IAppSystem( )
	{
	}

	virtual void func0( ) = 0;
	virtual void func1( ) = 0;
	virtual void func2( ) = 0;
	virtual void func3( ) = 0;
	virtual void func4( ) = 0;
	virtual void func5( ) = 0;
	virtual void func6( ) = 0;
	virtual void func7( ) = 0;
	virtual void func8( ) = 0;
	virtual void func9( ) = 0;
};

struct CVarDLLIdentifier_t;

class ICVar : public IAppSystem {
public:
	virtual void			func10( ) = 0;
	virtual void			RegisterConCommand( ConVar* pCommandBase ) = 0;
	virtual void			UnregisterConCommand( ConVar* pCommandBase ) = 0;
	virtual void			func13( ) = 0;
	virtual ConVar* FindVar( const char* var_name ) = 0;

	//	virtual void            ConsoleColorPrintf(const Color& clr, const char *pFormat, ...) const = 0;
	virtual void			func15( ) = 0;
	virtual void			func16( ) = 0;
	virtual void			func17( ) = 0;
	virtual void			func18( ) = 0;
	virtual void			func19( ) = 0;
	virtual void			func20( ) = 0;

	void const ConsoleColorPrintf( const Color& clr, const char* pFormat, ... )
	{
		typedef void( __cdecl * OriginalFn )( void*, const Color&, const char*, ... );

		if ( pFormat == nullptr )
			return;

		char buf[ 8192 ];

		va_list list;
		va_start( list, pFormat );
		_vsnprintf_s( buf, sizeof( buf ) - 1, pFormat, list );
		va_end( list );
		buf[ sizeof( buf ) - 1 ] = 0;

		Utils::GetVFunc<OriginalFn>( this, 25 )( this, clr, pFormat );
	}
};

extern ICVar* g_pCvar;