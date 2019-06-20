#pragma once
#include "..\..\Hooks.h"
#include "..\..\Console\Console.hpp"

namespace scan {
	bool valid_code_ptr( uintptr_t addr );
}

class Address {
protected:
	uintptr_t m_ptr;

public:
	// default c/dtor
	__forceinline Address( ) : m_ptr{} { };
	__forceinline ~Address( ) = default;

	__forceinline Address( uintptr_t a ) :
		m_ptr{ a } { }

	__forceinline Address( const void* a ) :
		m_ptr{ reinterpret_cast< uintptr_t >( a ) } { }

	__forceinline operator uintptr_t( ) const {
		return m_ptr;
	}

	__forceinline operator void* ( ) const {
		return reinterpret_cast< void* >( m_ptr );
	}

	__forceinline operator const void* ( ) const {
		return reinterpret_cast< const void* >( m_ptr );
	}

	// to is like as but dereferences.
	template< typename T = Address >
	__forceinline T to( ) const {
		return *reinterpret_cast< T* >( m_ptr );
	}

	template< typename T = Address >
	__forceinline T as( ) const {
		return reinterpret_cast< T >( m_ptr );
	}

	template< typename T = Address >
	__forceinline T at( ptrdiff_t offset ) const {
		return *reinterpret_cast< T* >( m_ptr + offset );
	}

	template< typename T = Address >
	__forceinline T add( ptrdiff_t offset ) const {
		return reinterpret_cast< T >( m_ptr + offset );
	}

	template< typename T = Address >
	__forceinline T sub( ptrdiff_t offset ) const {
		return reinterpret_cast< T >( m_ptr - offset );
	}

	template< typename T = Address >
	__forceinline T get( size_t dereferences = 1 ) {
		return reinterpret_cast< T >( get_( dereferences ) );
	}

	template< typename T = Address >
	__forceinline void set( T val ) {
		*reinterpret_cast< T* >( m_ptr ) = val;
	}

	template< typename T = Address >
	__forceinline T rel( size_t offset = 0 ) {
		uintptr_t out = m_ptr + offset;

		uint32_t rel = *reinterpret_cast< uint32_t* >( out );
		if ( !rel )
			return T{};

		out = ( out + 0x4 ) + rel;

		return reinterpret_cast< T >( out );
	}

	__forceinline static bool safe( Address to_check ) {
		static MEMORY_BASIC_INFORMATION32 mbi{};

		if ( !to_check
			|| to_check < 0x10000
			|| to_check > 0xFFE00000
			|| !VirtualQuery( to_check, reinterpret_cast< PMEMORY_BASIC_INFORMATION >( &mbi ), sizeof( mbi ) ) )
			return false;

		if ( !mbi.AllocationBase
			|| mbi.State != MEM_COMMIT
			|| mbi.Protect == PAGE_NOACCESS
			|| mbi.Protect & PAGE_GUARD )
			return false;

		return true;
	}

private:
	__forceinline uintptr_t get_( size_t dereferences ) {
		uintptr_t temp = m_ptr;

		while ( dereferences-- && safe( temp ) )
			temp = *reinterpret_cast< uintptr_t* >( temp );

		return temp;
	}
};

class c_vmt {
private:
	uintptr_t m_vmt_base; // VMT base ( on heap / in rdata ).
	uintptr_t* m_old_vmt; // actual VMT.
	uint16_t m_total_methods; // total number of methods in VMT.
	std::unique_ptr< uintptr_t[ ] > m_new_vmt; // our newly allocated VMT.
	uintptr_t* m_new_vmt_start; // the start ptr to our new VMT ( since RTTI was copied ).

	__forceinline uint16_t count_methods( ) const {
		uint16_t i = 0;

		while ( scan::valid_code_ptr( m_old_vmt[ i ] ) )
			++i;

		return i;
	}

public:
	c_vmt( ) : m_vmt_base{ 0 }, m_old_vmt{ nullptr }, m_total_methods{ 0 }, m_new_vmt{ nullptr }, m_new_vmt_start{ nullptr } { }

	explicit c_vmt( void* base ) : m_vmt_base{ 0 }, m_old_vmt{ nullptr }, m_total_methods{ 0 }, m_new_vmt{ nullptr }, m_new_vmt_start{ nullptr } {
		init( base );
	}

	explicit c_vmt( const uintptr_t base ) : m_vmt_base{ 0 }, m_old_vmt{ nullptr }, m_total_methods{ 0 }, m_new_vmt{ nullptr }, m_new_vmt_start{ nullptr } {
		init( base );
	}

	~c_vmt( ) {
	}

	bool init( void* base ) {
		return init( reinterpret_cast< uintptr_t >( base ) );
	}

	bool init( uintptr_t base ) {
		if ( !base )
			return false;

		m_vmt_base = base;

		m_old_vmt = *reinterpret_cast< uintptr_t * * >( base );
		if ( !m_old_vmt )
			return false;

		m_total_methods = count_methods( );
		if ( !m_total_methods )
			return false;

		// allocate new VMT, making room for RTTI ptr.
		m_new_vmt = std::make_unique< uintptr_t[ ] >( m_total_methods + 1 );
		if ( !m_new_vmt )
			return false;

		// get address of our newly allocated memory.
		const auto alloc_base = reinterpret_cast< uintptr_t >( m_new_vmt.get( ) );

		// copy VMT, starting from RTTI.
		std::memcpy(
			reinterpret_cast< void* >( alloc_base ),
			static_cast< const void* >( m_old_vmt - 1 ),
			( m_total_methods + 1 ) * sizeof( uintptr_t )
		);

		// get start for new VMT.
		// VMT start is actually 1 function ahead due to RTTI copy.
		m_new_vmt_start = reinterpret_cast< uintptr_t* >( alloc_base + sizeof( uintptr_t ) );

		// set base to our new VMT.
		*reinterpret_cast< uintptr_t* >( base ) = reinterpret_cast< uintptr_t >( m_new_vmt_start );

		return true;
	}

	bool hook_method( uint16_t index, void* func ) {
		if ( !m_old_vmt || !m_new_vmt_start || index > m_total_methods || !func )
			return false;

		m_new_vmt_start[ index ] = reinterpret_cast< uintptr_t >( func );
		//console::print_debug("hooking 0x% ==> 0x%.\n", m_old_vmt[index], func);
		return true;
	}

	bool unhook_method( uint16_t index ) {
		if ( !m_old_vmt || !m_new_vmt_start || index > m_total_methods )
			return false;

		m_old_vmt[ index ] = m_new_vmt_start[ index ];

		return true;
	}

	template< typename T = uintptr_t >
	__forceinline T get_old_method( uint16_t index ) {
		return reinterpret_cast< T >( m_old_vmt[ index ] );
	}

	bool unhook_all( ) {
		if ( !m_old_vmt || !m_vmt_base )
			return false;

		*reinterpret_cast< uintptr_t* >( m_vmt_base ) = reinterpret_cast< uintptr_t >( m_old_vmt );

		return true;
	}

	template< typename T = Address >
	__forceinline static T get_method( Address this_ptr, size_t index ) {
		return static_cast< T >( this_ptr.to< T* >( )[ index ] );
	}
};
