[BITS 64]

GLOBAL GetIp

GetIp:
	call	get_ret_ptr

get_ret_ptr:
	; pop address and sub diff
	pop		rax
	sub		rax,	5
	ret

Leave:
	db 'E', 'N', 'D', '-', 'P', 'I', 'P', 'E', 'D', 'R', 'E', 'A', 'M'
