	.file	"ExampleMemoryOrder.cpp"
	.text
#APP
	.globl _ZSt21ios_base_library_initv
#NO_APP
	.section	.text._ZNSt6thread24_M_thread_deps_never_runEv,"axG",@progbits,_ZNSt6thread24_M_thread_deps_never_runEv,comdat
	.p2align 4
	.weak	_ZNSt6thread24_M_thread_deps_never_runEv
	.type	_ZNSt6thread24_M_thread_deps_never_runEv, @function
_ZNSt6thread24_M_thread_deps_never_runEv:
.LFB573:
	.cfi_startproc
	endbr64
	ret
	.cfi_endproc
.LFE573:
	.size	_ZNSt6thread24_M_thread_deps_never_runEv, .-_ZNSt6thread24_M_thread_deps_never_runEv
	.text
	.p2align 4
	.globl	_Z7thread1v
	.type	_Z7thread1v, @function
_Z7thread1v:
.LFB2734:
	.cfi_startproc
	endbr64
	movl	$1, x(%rip)
	movl	y(%rip), %eax
	movl	%eax, r1(%rip)
	ret
	.cfi_endproc
.LFE2734:
	.size	_Z7thread1v, .-_Z7thread1v
	.p2align 4
	.globl	_Z7thread2v
	.type	_Z7thread2v, @function
_Z7thread2v:
.LFB2735:
	.cfi_startproc
	endbr64
	movl	$1, y(%rip)
	movl	x(%rip), %eax
	movl	%eax, r2(%rip)
	ret
	.cfi_endproc
.LFE2735:
	.size	_Z7thread2v, .-_Z7thread2v
	.section	.text._ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEE6_M_runEv,"axG",@progbits,_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEE6_M_runEv,comdat
	.align 2
	.p2align 4
	.weak	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEE6_M_runEv
	.type	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEE6_M_runEv, @function
_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEE6_M_runEv:
.LFB3439:
	.cfi_startproc
	endbr64
	jmp	*8(%rdi)
	.cfi_endproc
.LFE3439:
	.size	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEE6_M_runEv, .-_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEE6_M_runEv
	.section	.text._ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED2Ev,"axG",@progbits,_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED5Ev,comdat
	.align 2
	.p2align 4
	.weak	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED2Ev
	.type	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED2Ev, @function
_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED2Ev:
.LFB3436:
	.cfi_startproc
	endbr64
	leaq	16+_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE(%rip), %rax
	movq	%rax, (%rdi)
	jmp	_ZNSt6thread6_StateD2Ev@PLT
	.cfi_endproc
.LFE3436:
	.size	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED2Ev, .-_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED2Ev
	.weak	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED1Ev
	.set	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED1Ev,_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED2Ev
	.section	.text._ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED0Ev,"axG",@progbits,_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED5Ev,comdat
	.align 2
	.p2align 4
	.weak	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED0Ev
	.type	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED0Ev, @function
_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED0Ev:
.LFB3438:
	.cfi_startproc
	endbr64
	leaq	16+_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE(%rip), %rax
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movq	%rdi, %rbx
	movq	%rax, (%rdi)
	call	_ZNSt6thread6_StateD2Ev@PLT
	movq	%rbx, %rdi
	movl	$16, %esi
	popq	%rbx
	.cfi_def_cfa_offset 8
	jmp	_ZdlPvm@PLT
	.cfi_endproc
.LFE3438:
	.size	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED0Ev, .-_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED0Ev
	.section	.text._ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED2Ev,"axG",@progbits,_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED5Ev,comdat
	.align 2
	.p2align 4
	.weak	_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED2Ev
	.type	_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED2Ev, @function
_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED2Ev:
.LFB3226:
	.cfi_startproc
	endbr64
	movq	(%rdi), %rdi
	testq	%rdi, %rdi
	je	.L9
	movq	(%rdi), %rax
	jmp	*8(%rax)
	.p2align 4,,10
	.p2align 3
.L9:
	ret
	.cfi_endproc
.LFE3226:
	.size	_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED2Ev, .-_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED2Ev
	.weak	_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED1Ev
	.set	_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED1Ev,_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED2Ev
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"Reordering observed!\n"
.LC1:
	.string	"Still trying...\n"
	.section	.text.unlikely,"ax",@progbits
.LCOLDB2:
	.section	.text.startup,"ax",@progbits
.LHOTB2:
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB2736:
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	.cfi_lsda 0x1b,.LLSDA2736
	endbr64
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	leaq	16+_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE(%rip), %r14
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	leaq	_ZNSt6thread24_M_thread_deps_never_runEv(%rip), %r13
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	xorl	%ebx, %ebx
	subq	$40, %rsp
	.cfi_def_cfa_offset 96
	movq	%fs:40, %rax
	movq	%rax, 24(%rsp)
	xorl	%eax, %eax
	leaq	16(%rsp), %rbp
	movq	%rsp, %r12
	leaq	8(%rsp), %r15
	jmp	.L26
	.p2align 4,,10
	.p2align 3
.L25:
	cmpq	$0, 8(%rsp)
	jne	.L23
	cmpq	$0, (%rsp)
	jne	.L23
.L26:
	movl	$16, %edi
	movl	$0, r2(%rip)
	movl	$0, r1(%rip)
	movl	$0, y(%rip)
	movl	$0, x(%rip)
	movq	$0, (%rsp)
.LEHB0:
	call	_Znwm@PLT
.LEHE0:
	movq	%r13, %rdx
	movq	%rbp, %rsi
	movq	%r12, %rdi
	leaq	_Z7thread1v(%rip), %rcx
	movq	%r14, (%rax)
	movq	%rcx, 8(%rax)
	movq	%rax, 16(%rsp)
.LEHB1:
	call	_ZNSt6thread15_M_start_threadESt10unique_ptrINS_6_StateESt14default_deleteIS1_EEPFvvE@PLT
.LEHE1:
	movq	16(%rsp), %rdi
	testq	%rdi, %rdi
	je	.L12
	movq	(%rdi), %rax
	call	*8(%rax)
.L12:
	movq	$0, 8(%rsp)
	movl	$16, %edi
.LEHB2:
	call	_Znwm@PLT
.LEHE2:
	leaq	_Z7thread2v(%rip), %rcx
	movq	%r14, (%rax)
	movq	%r13, %rdx
	movq	%rbp, %rsi
	movq	%rcx, 8(%rax)
	movq	%r15, %rdi
	movq	%rax, 16(%rsp)
.LEHB3:
	call	_ZNSt6thread15_M_start_threadESt10unique_ptrINS_6_StateESt14default_deleteIS1_EEPFvvE@PLT
.LEHE3:
	movq	16(%rsp), %rdi
	testq	%rdi, %rdi
	je	.L16
	movq	(%rdi), %rax
	call	*8(%rax)
.L16:
	movq	%r12, %rdi
.LEHB4:
	call	_ZNSt6thread4joinEv@PLT
	movq	%r15, %rdi
	call	_ZNSt6thread4joinEv@PLT
	movl	r1(%rip), %eax
	orl	r2(%rip), %eax
	je	.L46
	addl	$1, %ebx
	imull	$197912093, %ebx, %eax
	rorl	$5, %eax
	cmpl	$42949, %eax
	ja	.L25
	movl	$16, %edx
	leaq	.LC1(%rip), %rsi
	leaq	_ZSt4cout(%rip), %rdi
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
	jmp	.L25
	.p2align 4,,10
	.p2align 3
.L46:
	movl	$21, %edx
	leaq	.LC0(%rip), %rsi
	leaq	_ZSt4cout(%rip), %rdi
	call	_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l@PLT
.LEHE4:
	cmpq	$0, 8(%rsp)
	jne	.L23
	cmpq	$0, (%rsp)
	jne	.L23
	movq	24(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L47
	addq	$40, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
.L23:
	.cfi_restore_state
	call	_ZSt9terminatev@PLT
.L47:
	call	__stack_chk_fail@PLT
.L33:
	endbr64
	movq	%rax, %rbx
	jmp	.L14
.L30:
	endbr64
	movq	%rax, %rdi
	jmp	.L19
.L32:
	endbr64
	movq	%rax, %rbx
	jmp	.L18
.L31:
	endbr64
	movq	%rax, %rdi
	jmp	.L27
	.globl	__gxx_personality_v0
	.section	.gcc_except_table,"a",@progbits
.LLSDA2736:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE2736-.LLSDACSB2736
.LLSDACSB2736:
	.uleb128 .LEHB0-.LFB2736
	.uleb128 .LEHE0-.LEHB0
	.uleb128 0
	.uleb128 0
	.uleb128 .LEHB1-.LFB2736
	.uleb128 .LEHE1-.LEHB1
	.uleb128 .L33-.LFB2736
	.uleb128 0
	.uleb128 .LEHB2-.LFB2736
	.uleb128 .LEHE2-.LEHB2
	.uleb128 .L30-.LFB2736
	.uleb128 0
	.uleb128 .LEHB3-.LFB2736
	.uleb128 .LEHE3-.LEHB3
	.uleb128 .L32-.LFB2736
	.uleb128 0
	.uleb128 .LEHB4-.LFB2736
	.uleb128 .LEHE4-.LEHB4
	.uleb128 .L31-.LFB2736
	.uleb128 0
.LLSDACSE2736:
	.section	.text.startup
	.cfi_endproc
	.section	.text.unlikely
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	.cfi_lsda 0x1b,.LLSDAC2736
	.type	main.cold, @function
main.cold:
.LFSB2736:
.L14:
	.cfi_def_cfa_offset 96
	.cfi_offset 3, -56
	.cfi_offset 6, -48
	.cfi_offset 12, -40
	.cfi_offset 13, -32
	.cfi_offset 14, -24
	.cfi_offset 15, -16
	movq	%rbp, %rdi
	call	_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED1Ev
	movq	24(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L48
	movq	%rbx, %rdi
.LEHB5:
	call	_Unwind_Resume@PLT
.L18:
	movq	%rbp, %rdi
	call	_ZNSt10unique_ptrINSt6thread6_StateESt14default_deleteIS1_EED1Ev
	movq	%rbx, %rdi
.L19:
	cmpq	$0, (%rsp)
	jne	.L23
	movq	24(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L49
	call	_Unwind_Resume@PLT
.LEHE5:
.L48:
	call	__stack_chk_fail@PLT
.L27:
	cmpq	$0, 8(%rsp)
	je	.L19
	jmp	.L23
.L49:
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE2736:
	.section	.gcc_except_table
.LLSDAC2736:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSEC2736-.LLSDACSBC2736
.LLSDACSBC2736:
	.uleb128 .LEHB5-.LCOLDB2
	.uleb128 .LEHE5-.LEHB5
	.uleb128 0
	.uleb128 0
.LLSDACSEC2736:
	.section	.text.unlikely
	.section	.text.startup
	.size	main, .-main
	.section	.text.unlikely
	.size	main.cold, .-main.cold
.LCOLDE2:
	.section	.text.startup
.LHOTE2:
	.weak	_ZTSNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE
	.section	.rodata._ZTSNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE,"aG",@progbits,_ZTSNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE,comdat
	.align 32
	.type	_ZTSNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE, @object
	.size	_ZTSNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE, 59
_ZTSNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE:
	.string	"NSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE"
	.weak	_ZTINSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE
	.section	.data.rel.ro._ZTINSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE,"awG",@progbits,_ZTINSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE,comdat
	.align 8
	.type	_ZTINSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE, @object
	.size	_ZTINSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE, 24
_ZTINSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE:
	.quad	_ZTVN10__cxxabiv120__si_class_type_infoE+16
	.quad	_ZTSNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE
	.quad	_ZTINSt6thread6_StateE
	.weak	_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE
	.section	.data.rel.ro.local._ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE,"awG",@progbits,_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE,comdat
	.align 8
	.type	_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE, @object
	.size	_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE, 40
_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE:
	.quad	0
	.quad	_ZTINSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE
	.quad	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED1Ev
	.quad	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED0Ev
	.quad	_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEE6_M_runEv
	.globl	r2
	.bss
	.align 4
	.type	r2, @object
	.size	r2, 4
r2:
	.zero	4
	.globl	r1
	.align 4
	.type	r1, @object
	.size	r1, 4
r1:
	.zero	4
	.globl	y
	.align 4
	.type	y, @object
	.size	y, 4
y:
	.zero	4
	.globl	x
	.align 4
	.type	x, @object
	.size	x, 4
x:
	.zero	4
	.hidden	DW.ref.__gxx_personality_v0
	.weak	DW.ref.__gxx_personality_v0
	.section	.data.rel.local.DW.ref.__gxx_personality_v0,"awG",@progbits,DW.ref.__gxx_personality_v0,comdat
	.align 8
	.type	DW.ref.__gxx_personality_v0, @object
	.size	DW.ref.__gxx_personality_v0, 8
DW.ref.__gxx_personality_v0:
	.quad	__gxx_personality_v0
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
