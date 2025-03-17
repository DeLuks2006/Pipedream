#pragma once

#define NTDLL 0x633a87bf
#define KRNL32 0xce70de87
#define OPENPROC 0x5ea49a38
#define NT_VIRTUAL_ALLOC 0xca67b978
#define NT_VIRTUAL_PROTECT 0xbd799926

// all the below shamelessly stolen from titanldr :)

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
