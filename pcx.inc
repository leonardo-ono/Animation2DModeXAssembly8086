; sketch implementation of pcx file format decoder 
; written by Leonardo Ono (ono.leo@gmail.com)
; 04/10/2017

; si = pcx_data_0 or pcx_data_1
; page = [page]
draw_pcx_image:
		; mov si, pcx_data_0
		add si, 128

		mov cx, 0
		mov bx, 0

		jmp .next_byte

	.rep:
		dec dl
		cmp dl, 0
		jnz .normal

	.next_byte:
		
		mov al, [si]
		inc si
		cmp al, 0c0h
		mov dl, 1
		jb .normal

		and al, 3fh
		mov dl, al
		mov al, [si]		
		inc si		
		
	.normal:
		
		pusha
		;mov al, 15
		call pset_pcx
		popa

		inc cx
		cmp cx, 320
		jb .rep

		mov cx, 0
		inc bx

		cmp bx, 200
		jb .rep

	.handle_palette:
		mov al, [si]
		inc si
		cmp al, 0ch
		jnz .end ; palette not found

		mov bl, 0
		mov di, 0		
	.next_color:
		mov bh, [si + 0]
		mov cl, [si + 1]
		mov ch, [si + 2]
		shr bh, 2
		shr cl, 2
		shr ch, 2
		call change_palette
		add si, 3
		inc bl
		inc di
		cmp di, 256
		jb .next_color		

	.end:
		ret
		
; al = color index
; bx = row
; cx = col
pset_pcx:
	pusha
	
		; ax = row
		; bx = col
		; si = color index
		; di = page
		mov ah, 0
		mov si, ax
		mov di, [page]
		
		mov ax, bx
		mov bx, cx
		call pset
		
	popa
	ret
	
page dw 0

		
