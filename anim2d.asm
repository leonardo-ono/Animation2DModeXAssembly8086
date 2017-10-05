; 2d animation test (vga mode x + pcx image file format)
; written by Leonardo Ono (ono.leo@gmail.com)
; 04/10/2017
		bits 16
		org 100h
	
start:
		call start_graphic_mode

		mov cx, 2
		call change_page

		; si = pcx_data_0 or pcx_data_1
		; page = [page]
		mov si, pcx_data_0
		mov word [page], 0
		call draw_pcx_image

		mov si, pcx_data_1
		mov word [page], 1
		call draw_pcx_image
	
	.next_frame:
		mov ax, [pcx_data_0 + 128 + 1]
		mov si, ax
		mov bx, [frame_index]
		mov ax, [frames + bx + 10]
		call clear_screen_page
		
		mov bx, [frame_index]

		mov ax, [frames + bx + 0]
		mov word [bitblt.sx], ax
		mov ax, [frames + bx + 2]
		mov word [bitblt.sy], ax
		mov ax, [frames + bx + 4]
		mov word [bitblt.si], ax
		mov ax, [frames + bx + 6]
		mov word [bitblt.dx], ax
		mov ax, [frames + bx + 8]
		mov word [bitblt.dy], ax
		mov ax, [frames + bx + 10]
		mov word [bitblt.di], ax
		mov ax, [frames + bx + 12]
		mov word [bitblt.w], ax
		mov ax, [frames + bx + 14]
		mov word [bitblt.h], ax
		call bitblt

	.wait_18_ms:
		mov si, 1
		call sleep
		
	.render:
		call wait_retrace

		mov bx, [frame_index]
		mov cx, word [frames + bx + 16]
		call change_page

		
	.wait_for_key:
		mov ah, 1
		int 16h
		jz .no_key
		
		mov ah, 0
		int 16h
		
		cmp al, 27
		jz .exit_process
		
	.no_key:
		mov bx, [frame_index]
		add bx, 18
		
		cmp bx, 144
		jnz .l2
		mov bx, 0
	.l2:
		mov [frame_index], bx
		
		jmp .next_frame
		
	.exit_process:
		call return_text_mode
		
		mov ah, 4ch
		int 21h
		
	%include "timer.inc"
	%include "graphic.inc"
	%include "pcx.inc"
		
	frame_index db 0
	
	frames:
		; si = source page index
		; di = destination page index 
		;  sx   sy  si  dx   dy  di  w    h    test

		dw 000, 000, 0, 080, 050, 3, 160, 100, 3
		dw 160, 000, 0, 080, 050, 2, 160, 100, 2
		dw 000, 000, 1, 080, 050, 3, 160, 100, 3
		dw 160, 000, 1, 080, 050, 2, 160, 100, 2
		dw 000, 100, 0, 080, 050, 3, 160, 100, 3
		dw 160, 100, 0, 080, 050, 2, 160, 100, 2
		dw 000, 100, 1, 080, 050, 3, 160, 100, 3
		dw 160, 100, 1, 080, 050, 2, 160, 100, 2
	
	
pcx_data_0:
		incbin "cat0.pcx"

pcx_data_1:
		incbin "cat1.pcx"

	
		
