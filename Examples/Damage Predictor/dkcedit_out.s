	.file	"test.c"
	.intel_syntax noprefix
	.text
	.globl	evil_trick
	.section	.text,"w"
	.align 8
evil_trick:
	.byte	68
	.byte	75
	.byte	67
	.byte	69
	.byte	72
	.byte	15
	.byte	31
	.byte	-63
	.globl	new_attack
	.align 8
new_attack:
	.ascii "Attack(####)\0"
	.globl	temp_rax
	.align 8
temp_rax:
	.ascii "1234567\0"
	.globl	temp_ret
	.align 8
temp_ret:
	.ascii "ABCDEFG\0"
	.text
	.globl	mod_main
	.def	mod_main;	.scl	2;	.type	32;	.endef
	.seh_proc	mod_main
mod_main:
	nop rcx
	push rax
	push rbx
	push rcx
	push rdx
	push rsi
	push rdi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push	rbp
	.seh_pushreg	rbp
	mov	rbp, rsp
	.seh_setframe	rbp, 0
	sub	rsp, 16
	.seh_stackalloc	16
	.seh_endprologue
	mov	QWORD PTR -8[rbp], 0
	mov	rax, QWORD PTR -8[rbp]
/APP
 # 7 "test.c" 1
	.intel_syntax noprefix
	nop %rbx
	mov rax, [%rip-0x2dc997]
	
 # 0 "" 2
/NO_APP
	mov	QWORD PTR -8[rbp], rax
	add	QWORD PTR -8[rbp], 7428
	mov	rax, QWORD PTR -8[rbp]
	mov	eax, DWORD PTR [rax]
	mov	DWORD PTR -12[rbp], eax
	mov	ecx, DWORD PTR -12[rbp]
	mov	edx, 274877907
	mov	eax, ecx
	imul	edx
	sar	edx, 6
	mov	eax, ecx
	sar	eax, 31
	sub	edx, eax
	mov	eax, edx
	add	eax, 48
	mov	BYTE PTR new_attack[rip+7], al
	mov	ecx, DWORD PTR -12[rbp]
	mov	edx, 274877907
	mov	eax, ecx
	imul	edx
	sar	edx, 6
	mov	eax, ecx
	sar	eax, 31
	sub	edx, eax
	mov	eax, edx
	imul	eax, eax, 1000
	sub	ecx, eax
	mov	eax, ecx
	mov	DWORD PTR -12[rbp], eax
	mov	ecx, DWORD PTR -12[rbp]
	mov	edx, 1374389535
	mov	eax, ecx
	imul	edx
	sar	edx, 5
	mov	eax, ecx
	sar	eax, 31
	sub	edx, eax
	mov	eax, edx
	add	eax, 48
	mov	BYTE PTR new_attack[rip+8], al
	mov	ecx, DWORD PTR -12[rbp]
	mov	edx, 1374389535
	mov	eax, ecx
	imul	edx
	sar	edx, 5
	mov	eax, ecx
	sar	eax, 31
	sub	edx, eax
	mov	eax, edx
	imul	eax, eax, 100
	sub	ecx, eax
	mov	eax, ecx
	mov	DWORD PTR -12[rbp], eax
	mov	ecx, DWORD PTR -12[rbp]
	mov	edx, 1717986919
	mov	eax, ecx
	imul	edx
	sar	edx, 2
	mov	eax, ecx
	sar	eax, 31
	sub	edx, eax
	mov	eax, edx
	add	eax, 48
	mov	BYTE PTR new_attack[rip+9], al
	mov	ecx, DWORD PTR -12[rbp]
	mov	edx, 1717986919
	mov	eax, ecx
	imul	edx
	sar	edx, 2
	mov	eax, ecx
	sar	eax, 31
	sub	edx, eax
	mov	eax, edx
	sal	eax, 2
	add	eax, edx
	add	eax, eax
	sub	ecx, eax
	mov	eax, ecx
	mov	DWORD PTR -12[rbp], eax
	mov	eax, DWORD PTR -12[rbp]
	add	eax, 48
	mov	BYTE PTR new_attack[rip+10], al
	nop
	add	rsp, 16
	pop	rbp
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rbx
	pop rax
	mov temp_rax[rip + 0], rax
	pop rax
	mov temp_ret[rip + 0], rax
	mov rax, temp_rax[rip + 0]
	nop rax
	call   0xffffffffffabcdef
	mov temp_rax[rip + 0], rax
	mov rax, temp_ret[rip + 0]
	push rax
	mov rax, temp_rax[rip + 0]
	ret
		.seh_endproc
	.ident	"GCC: (x86_64-posix-sjlj-rev0, Built by MinGW-W64 project) 8.1.0"
