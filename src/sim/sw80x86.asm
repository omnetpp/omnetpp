;===========================================================================
; SW80x86.ASM
;   contains:
;     void set_sp_reg(int *sp);   -- overwrite stack pointer
;     int *get_sp_reg();          -- return stack pointer
;     void use_stack(int *sp);    -- switch to another stack
;
;   i80x86 processor, Borland C++ 3.1
;
;   Author: Andras Varga
;
;===========================================================================

;----------------------------------------------------------------
;  Copyright (C) 1992,99 Andras Varga
;  Technical University of Budapest, Dept. of Telecommunications,
;  Stoczek u.2, H-1111 Budapest, Hungary.
;
;  This file is distributed WITHOUT ANY WARRANTY. See the file
;  `license' for details on this and other legal matters.
;----------------------------------------------------------------

        public  _set_sp_reg, _get_sp_reg, _use_stack
	extrn   __fmemcpy:far

SWITCH_TEXT     segment byte public 'CODE'
	assume  cs:SWITCH_TEXT,ds:DGROUP

_set_sp_reg     proc    far
	mov     bp,sp
	pop     ax
	pop     dx

	cli
	mov     sp,word ptr [bp+4]    ; load new ss:sp
	mov     ss,word ptr [bp+6]
	sti

	sub     sp,4    ; caller will pop the 4 bytes it passed
	push    dx
	push    ax
	ret
_set_sp_reg     endp

;-----------------------------------------

_get_sp_reg     proc    far
	mov     ax,sp
	mov     dx,ss
	ret
_get_sp_reg     endp

;-----------------------------------------

_use_stack      proc    far
	push    bp       ;save bp
	mov     bp,sp
; ** store args in global variables
; oss = FP_SEG(sp);
	mov     ax,word ptr [bp+8]
	mov     word ptr DGROUP:oss,ax
; osp = FP_OFF(sp);
	mov     ax,word ptr [bp+6]
	mov     word ptr DGROUP:osp,ax
	pop     bp       ;restore bp
; loc = _BP-_SP;
	mov     ax,bp
	sub     ax,sp
	mov     word ptr DGROUP:loc,ax
; ** copy args passed to caller to the new stack
; memcpy( MK_FP(oss:osp)-ARGS, MK_FP(_SS,_BP), ARGS );
	mov     ax,32
	push    ax
	push    ss
	push    bp
	mov     ax,word ptr DGROUP:oss
	push    ax
	mov     ax,word ptr DGROUP:osp
	add     ax,-32
	push    ax
	call    far ptr __fmemcpy
	add     sp,10
; ** calculate new SP and BP values
; nbp = osp-ARGS;
	mov     ax,word ptr DGROUP:osp
	add     ax,-32
	mov     word ptr DGROUP:nbp,ax
; nsp = _BP-loc;
	mov     ax,bp
	sub     ax,word ptr DGROUP:loc
	mov     word ptr DGROUP:nsp,ax
; ** switch to the new stack
; _SS:_SP = oss:nsp;
; _BP = nbp;
	pop     ax
	pop     dx
	cli
	mov     ss,word ptr DGROUP:oss  ; load new ss:sp
	mov     sp,word ptr DGROUP:nsp
	mov     bp,word ptr DGROUP:nbp
	sti
	push    dx
	push    ax
	ret
_use_stack      endp
SWITCH_TEXT     ends

;---------------------------------------

DGROUP  group   _DATA,_BSS
_DATA   segment word public 'DATA'
_DATA   ends
_BSS    segment word public 'BSS'
oss     label   word
	db      2 dup (?)
osp     label   word
        db      2 dup (?)
loc     label   word
        db      2 dup (?)
nbp     label   word
        db      2 dup (?)
nsp     label   word
        db      2 dup (?)
_BSS    ends

        end
