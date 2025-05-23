;
; Copyright (C) 1993-1996 Id Software, Inc.
; Copyright (C) 1993-2008 Raven Software
; Copyright (C) 2016-2017 Alexey Khokholov (Nuke.YKT)
;
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 2
; of the License, or (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; DESCRIPTION: Assembly texture mapping routines for planar VGA mode
;

BITS 32
%include "macros.inc"

%ifdef MODE_VBE2

%ifndef SCREENWIDTH
%define SCREENWIDTH   320
%define SCREENHEIGHT  200
%endif

extern _backbuffer
extern _ptrprocessedpalette
extern _pcscreen

BEGIN_CODE_SECTION

CODE_SYM_DEF I_PatchFinishUpdate15bpp16bppLinear
	push 	ebx
	mov   	ebx,_backbuffer
	add		ebx,SCREENWIDTH*SCREENHEIGHT
	mov   	eax,patchEndBackbuffer15bpp16bpp+2
	mov   	[eax],ebx
	pop 	ebx
	ret

CODE_SYM_DEF I_FinishUpdate15bpp16bppLinear
	push	ebx
	push	ecx
	push	edx
	push	edi
	push	esi
	push	ebp

	mov		esi,[_ptrprocessedpalette]
	mov		edi,[_pcscreen]
	mov		ebp,_backbuffer
	xor		ecx,ecx
	xor		eax,eax
	xor		ebx,ebx

loop1516linear:
	mov		eax, [ebp]
	
	mov		bl,ah
	mov		cl,al
	
	mov		dx,[esi+ebx*2]
	shl		edx,16
	mov		dx,[esi+ecx*2]

	add		ebp,4

	mov		[edi],edx

	shr		eax,16

	mov		bl,ah
	mov		cl,al
	
	mov		dx,[esi+ebx*2]

	add		edi,8

	shl		edx,16

	mov		dx,[esi+ecx*2]

patchEndBackbuffer15bpp16bpp:
	cmp		ebp,0x12345678

	mov		[edi-4],edx

	jl		loop1516linear

	pop		ebp
	pop		esi
	pop		edi
	pop		edx
	pop		ecx
	pop		ebx
	ret

CODE_SYM_DEF I_PatchFinishUpdate24bppLinear
	push 	ebx
	mov   	ebx,_backbuffer
	add		ebx,SCREENWIDTH*SCREENHEIGHT
	mov   	eax,patchEndBackbuffer24bpp+1
	mov   	[eax],ebx
	pop 	ebx
	ret

CODE_SYM_DEF I_FinishUpdate24bppLinear
	push		ebx
	push		ecx
	push		edx
	push		esi
	push		edi
	push		ebp

	mov		edi,[_ptrprocessedpalette]
	mov		ebp,[_pcscreen]
	mov		eax,_backbuffer
	xor		ebx,ebx
	xor		ecx,ecx
	xor		edx,edx

loop24linear:

	mov		ebx, [eax]
	
	mov		cl, bl
	
	add		eax,4

	mov		edx, [edi+ecx*4]	 	; BLUE+GREEN+RED 1st pixel
	
	mov		cl, bh

	rol		edx,16

	mov		dh, [edi+ecx*4]		; BLUE 2nd pixel
	rol		edx,16

	shr		ebx,16

	mov		[ebp], edx			; Move 1st 32-bit data to VRAM

	mov		dx, [edi+ecx*4+1]	; GREEN+RED 2nd pixel
	
	mov		cl,bh
	and		ebx,0xFF

	shl		edx,16

	mov		dx, [edi+ebx*4]		; BLUE+GREEN 3rd pixel
	rol		edx,16

	add		ebp,12

	mov		[ebp-8], edx		; Move 2nd 32-bit data to VRAM

	mov		edx, [edi+ecx*4]	; BLUE+GREEN+RED 4th pixel

	shl		edx,8

	mov		dl, [edi+ebx*4+2]	; RED 3rd pixel
	
patchEndBackbuffer24bpp:
	cmp		eax,0x12345678

	mov		[ebp-4], edx		; Move 3rd 32-bit data to VRAM

	jl		loop24linear

	pop		ebp
	pop		edi
	pop		esi
	pop		edx
	pop		ecx
	pop		ebx
	ret

CODE_SYM_DEF I_PatchFinishUpdate32bppLinear
	push 	ebx
	mov   	ebx,_backbuffer
	add		ebx,SCREENWIDTH*SCREENHEIGHT
	mov   	eax,patchEndBackbuffer32bpp+1
	mov   	[eax],ebx
	pop 	ebx
	ret

CODE_SYM_DEF I_FinishUpdate32bppLinear
	push	ebx
	push	ecx
	push	edx
	push	edi
	push	esi
	push	ebp

	mov		esi,[_ptrprocessedpalette]
	mov		edi,[_pcscreen]
	mov		eax,_backbuffer
	xor		ecx,ecx
	xor		ebx,ebx
	xor		edx,edx

loop32linear:
	mov		ebx, [eax]
	
	mov		cl,bl
	mov		dl,bh
	
	add		eax,4

	mov		ebp,[esi+ecx*4]
	shr		ebx,16
	mov		[edi],ebp

	mov		ebp,[esi+edx*4]
	mov		cl,bh

	and		ebx,0xFF
	mov		[edi+4],ebp
	add		edi,16

	mov		ebp,[esi+ebx*4]
	mov		ebx,[esi+ecx*4]

patchEndBackbuffer32bpp
	cmp		eax,0x12345678

	mov		[edi-8],ebp
	mov		[edi-4],ebx
	
	jl		loop32linear

	pop		ebp
	pop		esi
	pop		edi
	pop		edx
	pop		ecx
	pop		ebx
	ret

%endif
