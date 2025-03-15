#pragma once
// all these shamelessly stolen from titanldr :)

// get relative offset of string or pointer
#define G_SYM( x )	( ULONG_PTR )( GetIp( ) - ( ( ULONG_PTR ) &GetIp - ( ULONG_PTR ) x ) )

// place function in specific region of mem
#define D_SEC( x )	__attribute__((section( ".text$" #x ) ))

// cast as pointer with specified typedef
#define D_API( x )	__typeof__( x ) * x

// cast unsigned pointer-wide int type 
#define U_PTR( x )	( ( ULONG_PTR ) x )

// cast unsigned pointer-wide type
#define C_PTR( x )	( ( PVOID ) x )

// Get end of code - Fuck 32bit btw
#define G_END( x )	U_PTR( GetIp( ) + 11 )