; timer routines
; written by Leonardo Ono (ono.leo@gmail.com)
; 04/10/2017

; in:
;	si = number of 55.56 ms to wait
sleep:
		mov ah, 0
		int 1ah
		mov bx, dx
	.wait:
		mov ah, 0
		int 1ah
		sub dx, bx
		cmp dx, si
		jl .wait
		ret
