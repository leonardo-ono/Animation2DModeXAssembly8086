; sketch implemention of graphic vga mode x routines 
; - 320 x 200 px, 256 indexed colors with 4 pages 
; - allows page flipping
;
; written by Leonardo Ono (ono.leo@gmail.com)
; 04/10/2017

%define GC_INDEX		03ceh

%define SC_INDEX		03c4h
%define SC_DATA 		03c5h

%define CRTC_INDEX		03d4h
%define CRTC_DATA		03d5h

%define MAP_MASK		02h
%define MEMORY_MODE		04h
%define UNDERLINE_LOC 	14h
%define MODE_CONTROL	17h

; CRT controller registers
%define HIGH_ADDRESS 	0ch
%define LOW_ADDRESS		0dh

start_graphic_mode:
		mov ax, 0a000h
		mov es, ax
		mov ah, 0
		mov al, 13h
		int 10h
		call change_to_mode_x
		ret
	
return_text_mode:
		mov ah, 0
		mov al, 03h
		int 10h
		ret
		
; reference: http://www.brackeen.com/vga/unchain.html
change_to_mode_x:
		; turn 0ff chain-4 mode
		mov dx, SC_INDEX
		mov al, MEMORY_MODE
		out dx, al
		
		mov dx, SC_DATA
		mov al, 06h
		out dx, al
		
		; turn off long mode
		mov dx, CRTC_INDEX
		mov al, UNDERLINE_LOC
		out dx, al
		
		mov dx, CRTC_DATA
		mov al, 0
		out dx, al
		
		; turn on byte mode
		mov dx, CRTC_INDEX
		mov al, MODE_CONTROL
		out dx, al

		mov dx, CRTC_DATA
		mov al, 0e3h
		out dx, al
		
		ret

; change plane for reading
; in:
; 	plane = 0 1 2 3
;	   bl = 1 2 4 8
change_write_plane:
		mov dx, SC_INDEX
		mov al, MAP_MASK
		out dx, al

		mov dx, SC_DATA
		mov al, bl
		out dx, al
	
		ret
		
; -> change of plane is different between read and write (?)
;
; change plane for writing
; reference: http://twimgs.com/ddj/abrashblackbook/gpbb47.pdf (page 884)
; in:
; 	plane = 0 1 2 3
;	   bl = 0 1 2 3
change_read_plane:
		mov dx, GC_INDEX
		mov ah, bl
		mov al, MEMORY_MODE
		out dx, ax
		ret
	
; http://archive.gamedev.net/archive/reference/articles/article358.html
; bx = x
; ax = y
move_to:
		;o := y*size*2+x;
		mov cx, 80
		xor dx, dx
		mul cx
		shl ax, 2
		add ax, bx

		mov    bx, ax ; 10 * 80 * 2 + 0
		mov    ah, bh
		mov    al, HIGH_ADDRESS

		mov    dx, CRTC_INDEX
		out    dx, ax

		mov    ah, bl
		mov    al, LOW_ADDRESS
		mov    dx, CRTC_INDEX
		out    dx, ax		

		ret	
	
; in:
;	cx = page
change_page:
		mov ax, 50
		mul cx
		mov bx, 0
		call move_to
		ret
		
wait_retrace:
		mov dx, 3dah
	.l1:
		in al,dx
		test al, 08h
		jz .l1
		
	.l2:
		in al,dx
		test al, 08h
		jnz .l2
		
		ret


; bl = color index
; bh = red
; cl = green
; ch = blue
change_palette:
		mov dx, 3c8h
		mov al, bl ; color index
		out dx, al

		mov dx, 3c9h
		mov al, bh
		out dx, al ;red
		mov al, cl
		out dx, al ; green
		mov al, ch
		out dx, al ; blue

		ret

; in:
;   si = color index
;	ax = page number
clear_screen_page:
		mov cx, 16000
		xor dx, dx
		mul cx
		mov bx, ax
		mov di, 0

		mov ax, si
	.next_pixel:
		pusha
		mov bl, 0fh
		call change_write_plane ; do i have to call this for every pixel ?
		popa
		
		mov byte [es:bx + di], al
		mov bl, [es:bx + di]
		inc di
		
		
		loop .next_pixel
		
		ret


; ax = row
; bx = col
; si = color index
; di = page
pset:
		xor dx, dx
		push ax
		mov ax, 16000
		mul di
		mov di, ax
		pop ax
		
		mov cx, 320
		mul cx
		add ax, bx
		mov cx, ax
		and cl, 3
		mov ch, 1
		shl ch, cl
		mov bl, ch
		pusha
		call change_write_plane
		popa
		shr ax, 2
		mov bx, ax
		add bx, di
		mov ax, si
		mov byte [es:bx], al
		ret
	
; in:
;	ax = row
;	bx = col
;	di = page
; out:
;	si = color index
pget:
		xor dx, dx
		push ax
		mov ax, 16000
		mul di
		mov di, ax
		pop ax
		
		mov cx, 320
		mul cx
		add ax, bx
		mov bx, ax ; bl = 0 1 2 3 plane
		pusha
		call change_read_plane
		popa
		shr ax, 2
		mov bx, ax
		add bx, di
		mov ax, si
		mov al, byte [es:bx]
		mov ah, 0
		mov si, ax
		ret
	
bitblt:
		pusha
		mov word [.x], 0
		mov word [.y], 0
	.next_pixel:
		mov ax, [.sy]
		add ax, [.y]
		mov bx, [.sx]
		add bx, [.x]
		mov di, [.si]
		call pget

		mov ax, [.dy]
		add ax, [.y]
		mov bx, [.dx]
		add bx, [.x]
		mov di, [.di]
		call pset
		
		inc word [.x]
		mov ax, [.x]
		cmp ax, word [.w]
		jb .next_pixel
		
		mov word [.x], 0
		inc word [.y]
		mov ax, [.y]
		cmp ax, word [.h]
		jb .next_pixel
		
		popa	
		ret
		
		.sx dw 0
		.sy dw 0
		.si dw 0
		.dx dw 0
		.dy dw 0
		.di dw 0
		.w dw 0
		.h dw 0
		.x dw 0
		.y dw 0
