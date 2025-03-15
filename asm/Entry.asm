[BITS 64]

EXTERN Pipedream
GLOBAL Start

Start:
	; Set up stack
	push	rsi
	mov		rsi,	rsp
	and		rsp,	0FFFFFFFFFFFFFFF0h

	; Execute Loader
	sub		rsp,	020h
	call	Pipedream

	; Clean up and exit
	mov		rsp,	rsi
	pop		rsi
	ret
