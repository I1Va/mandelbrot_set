	.file	"godbolt_samp.cpp"
	.text
	.p2align 4
	.globl	_Z31display_with_array_optimizationP14display_info_tb
	.def	_Z31display_with_array_optimizationP14display_info_tb;	.scl	2;	.type	32;	.endef
	.seh_proc	_Z31display_with_array_optimizationP14display_info_tb
_Z31display_with_array_optimizationP14display_info_tb:
.LFB6458:
	pushq	%r15
	.seh_pushreg	%r15
	pushq	%r14
	.seh_pushreg	%r14
	pushq	%r12
	.seh_pushreg	%r12
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	pushq	%rsi
	.seh_pushreg	%rsi
	pushq	%rbx
	.seh_pushreg	%rbx
	subq	$304, %rsp
	.seh_stackalloc	304
	vmovaps	%xmm6, 144(%rsp)
	.seh_savexmm	%xmm6, 144
	vmovaps	%xmm7, 160(%rsp)
	.seh_savexmm	%xmm7, 160
	vmovaps	%xmm8, 176(%rsp)
	.seh_savexmm	%xmm8, 176
	vmovaps	%xmm9, 192(%rsp)
	.seh_savexmm	%xmm9, 192
	vmovaps	%xmm10, 208(%rsp)
	.seh_savexmm	%xmm10, 208
	vmovaps	%xmm11, 224(%rsp)
	.seh_savexmm	%xmm11, 224
	vmovaps	%xmm12, 240(%rsp)
	.seh_savexmm	%xmm12, 240
	vmovaps	%xmm13, 256(%rsp)
	.seh_savexmm	%xmm13, 256
	vmovaps	%xmm14, 272(%rsp)
	.seh_savexmm	%xmm14, 272
	vmovaps	%xmm15, 288(%rsp)
	.seh_savexmm	%xmm15, 288
	.seh_endprologue
	movl	44(%rcx), %edi
	movq	%rcx, %rax
	testl	%edi, %edi
	jle	.L21
	vmovsd	24(%rcx), %xmm2
	vmovsd	32(%rcx), %xmm14
	xorl	%r9d, %r9d
	movl	40(%rcx), %esi
.L5:
	testl	%esi, %esi
	jle	.L3
	vxorps	%xmm11, %xmm11, %xmm11
	vmovsd	16(%rax), %xmm15
	vmovss	.LC0(%rip), %xmm12
	movl	$1, %eax
	vcvtsi2sdl	%r9d, %xmm11, %xmm0
	vmovd	%eax, %xmm8
	leaq	16(%rsp), %r11
	vmovdqa	.LC3(%rip), %ymm9
	leaq	48(%rsp), %r10
	leaq	80(%rsp), %rcx
	vbroadcastss	%xmm12, %ymm10
	vcvtsd2ss	%xmm14, %xmm14, %xmm4
	leaq	iters_arr(%rip), %r8
	vpxor	%xmm13, %xmm13, %xmm13
	vpbroadcastd	%xmm8, %ymm8
	vmovsd	%xmm15, %xmm15, %xmm5
	vmulsd	%xmm14, %xmm0, %xmm0
	vaddsd	%xmm2, %xmm0, %xmm0
	vcvtsd2ss	%xmm0, %xmm0, %xmm6
	vmovd	%xmm6, %edx
.L4:
	xorl	%ebx, %ebx
	vmovsd	%xmm14, %xmm14, %xmm6
	movl	%edx, %r15d
.L11:
	vcvtsi2sdl	%ebx, %xmm11, %xmm0
	vmovdqa	%xmm13, 16(%rsp)
	xorl	%eax, %eax
	vmovdqa	%xmm13, 32(%rsp)
	vmovdqa	%xmm13, 48(%rsp)
	vmovdqa	%xmm13, 64(%rsp)
	vmulsd	%xmm6, %xmm0, %xmm0
	vmovdqa	%xmm13, 80(%rsp)
	vmovdqa	%xmm13, 96(%rsp)
	vaddsd	%xmm5, %xmm0, %xmm0
	vcvtsd2ss	%xmm0, %xmm0, %xmm1
	.p2align 4,,10
	.p2align 3
.L7:
	vcvtsi2ssl	%eax, %xmm11, %xmm0
	movslq	%eax, %rbp
	movl	%r15d, (%r10,%rax,4)
	vmovss	%xmm12, (%rcx,%rax,4)
	movl	$0, (%r8,%rbp,4)
	vmulss	%xmm4, %xmm0, %xmm0
	vaddss	%xmm1, %xmm0, %xmm0
	vmovss	%xmm0, (%r11,%rax,4)
	addq	$1, %rax
	cmpq	$8, %rax
	jne	.L7
	vxorps	%xmm3, %xmm3, %xmm3
	movl	$500, %ebp
	vmovsd	%xmm2, (%rsp)
	vmovups	16(%rsp), %ymm15
	vmovups	48(%rsp), %ymm14
	vmovups	80(%rsp), %ymm7
	vmovaps	%ymm3, %ymm1
	vmovsd	%xmm6, 8(%rsp)
	.p2align 4,,10
	.p2align 3
.L8:
	vmulps	%ymm1, %ymm1, %ymm2
	vmulps	%ymm3, %ymm3, %ymm6
	vmulps	%ymm3, %ymm1, %ymm1
	vaddps	%ymm6, %ymm2, %ymm0
	vsubps	%ymm6, %ymm2, %ymm2
	vaddps	%ymm1, %ymm1, %ymm1
	vcmpleps	%ymm10, %ymm0, %ymm0
	vaddps	%ymm15, %ymm2, %ymm2
	vaddps	%ymm14, %ymm1, %ymm1
	vpand	%ymm0, %ymm8, %ymm3
	vpand	%ymm0, %ymm9, %ymm0
	vminps	%ymm7, %ymm2, %ymm2
	vmovdqu	%ymm3, 112(%rsp)
	vextracti128	$0x1, %ymm0, %xmm3
	vminps	%ymm7, %ymm1, %ymm1
	vpor	%xmm0, %xmm3, %xmm0
	vpsrldq	$8, %xmm0, %xmm3
	vpor	%xmm3, %xmm0, %xmm3
	vpsrldq	$4, %xmm3, %xmm0
	vpor	%xmm0, %xmm3, %xmm0
	vmovd	%xmm0, %eax
	testl	%eax, %eax
	je	.L10
	xorl	%eax, %eax
	leaq	112(%rsp), %r14
	.p2align 4,,10
	.p2align 3
.L9:
	movslq	%eax, %r12
	movl	(%r8,%r12,4), %edx
	addl	(%r14,%rax,4), %edx
	addq	$1, %rax
	movl	%edx, (%r8,%r12,4)
	cmpq	$8, %rax
	jne	.L9
	subl	$1, %ebp
	je	.L10
	vmovaps	%ymm1, %ymm3
	vmovaps	%ymm2, %ymm1
	jmp	.L8
.L10:
	addl	$8, %ebx
	vmovsd	(%rsp), %xmm2
	vmovsd	8(%rsp), %xmm6
	cmpl	%esi, %ebx
	jl	.L11
	addl	$1, %r9d
	vmovsd	%xmm6, %xmm6, %xmm14
	cmpl	%r9d, %edi
	je	.L20
	vcvtsi2sdl	%r9d, %xmm11, %xmm0
	vmulsd	%xmm6, %xmm0, %xmm0
	vaddsd	%xmm2, %xmm0, %xmm0
	vcvtsd2ss	%xmm0, %xmm0, %xmm6
	vmovd	%xmm6, %edx
	jmp	.L4
.L3:
	addl	$1, %r9d
	cmpl	%edi, %r9d
	jne	.L5
.L21:
	vmovaps	144(%rsp), %xmm6
	vmovaps	160(%rsp), %xmm7
	vmovaps	176(%rsp), %xmm8
	vmovaps	192(%rsp), %xmm9
	vmovaps	208(%rsp), %xmm10
	vmovaps	224(%rsp), %xmm11
	vmovaps	240(%rsp), %xmm12
	vmovaps	256(%rsp), %xmm13
	vmovaps	272(%rsp), %xmm14
	vmovaps	288(%rsp), %xmm15
	addq	$304, %rsp
	popq	%rbx
	popq	%rsi
	popq	%rdi
	popq	%rbp
	popq	%r12
	popq	%r14
	popq	%r15
	ret
.L20:
	vzeroupper
	jmp	.L21
	.seh_endproc
	.section	.text.startup,"x"
	.p2align 4
	.def	_GLOBAL__sub_I_stable_radius_vec4;	.scl	3;	.type	32;	.endef
	.seh_proc	_GLOBAL__sub_I_stable_radius_vec4
_GLOBAL__sub_I_stable_radius_vec4:
.LFB6460:
	.seh_endprologue
	vbroadcastss	.LC0(%rip), %xmm0
	vmovaps	%xmm0, stable_radius_vec4(%rip)
	ret
	.seh_endproc
	.section	.ctors,"w"
	.align 8
	.quad	_GLOBAL__sub_I_stable_radius_vec4
	.globl	iters_arr
	.bss
	.align 32
iters_arr:
	.space 32
	.globl	stable_radius_vec4
	.align 16
stable_radius_vec4:
	.space 16
	.section .rdata,"dr"
	.align 4
.LC0:
	.long	1098907648
	.align 32
.LC3:
	.long	1
	.long	2
	.long	4
	.long	8
	.long	16
	.long	32
	.long	64
	.long	128
	.ident	"GCC: (MinGW-W64 x86_64-ucrt-posix-seh, built by Brecht Sanders) 13.1.0"
