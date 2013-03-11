	.file	"qsufsort.c"
	.text
.Ltext0:
	.p2align 4,,15
	.type	transform, @function
transform:
.LFB5:
	.file 1 "qsufsort.c"
	.loc 1 203 0
	.cfi_startproc
.LVL0:
	pushq	%r15
.LCFI0:
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	.loc 1 207 0
	movl	%ecx, %r15d
	.loc 1 203 0
	pushq	%r14
.LCFI1:
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	.loc 1 207 0
	subl	%r8d, %r15d
.LVL1:
	.loc 1 203 0
	pushq	%r13
.LCFI2:
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
.LCFI3:
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
.LCFI4:
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
.LCFI5:
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	.loc 1 207 0
	je	.L2
	movl	%r15d, %eax
	xorl	%r10d, %r10d
.LVL2:
	.p2align 4,,10
	.p2align 3
.L3:
	.loc 1 208 0 discriminator 2
	addl	$1, %r10d
.LVL3:
	.loc 1 207 0 discriminator 2
	sarl	%eax
.LVL4:
	jne	.L3
	.loc 1 209 0
	movl	$2147483647, %r14d
	movl	%r10d, %ecx
.LVL5:
	.loc 1 210 0
	movl	$0, r(%rip)
	.loc 1 209 0
	sarl	%cl, %r14d
.LVL6:
	.loc 1 210 0
	testl	%edx, %edx
	jle	.L75
	testl	%r14d, %r14d
	js	.L6
.LVL7:
.L38:
	cmpl	%r9d, %r15d
	jg	.L6
	movq	%rdi, %r12
	movslq	%r15d, %rbp
	movl	$1, %r13d
	xorl	%r11d, %r11d
	movl	%r10d, %ecx
	jmp	.L9
.LVL8:
	.p2align 4,,10
	.p2align 3
.L7:
	.loc 1 210 0 is_stmt 0 discriminator 2
	cmpl	%ebp, %r14d
	jl	.L73
	leal	1(%r13), %r13d
	.loc 1 210 0 discriminator 1
	movl	%ebp, %ebx
	addq	$4, %r12
	sall	%cl, %ebx
	orl	%r15d, %ebx
.LVL9:
	cmpl	%r9d, %ebx
	jg	.L73
	.loc 1 210 0
	movslq	%ebx, %rbp
.LVL10:
.L9:
	.loc 1 211 0 is_stmt 1 discriminator 1
	movl	%r11d, %eax
	movl	(%r12), %r11d
	sall	%cl, %eax
	subl	%r8d, %r11d
	addl	$1, %r11d
	orl	%eax, %r11d
.LVL11:
	.loc 1 210 0 discriminator 1
	cmpl	%r13d, %edx
	movl	%r13d, %eax
	jg	.L7
.L73:
	movl	%ecx, %r10d
	movl	$1, %ebx
	movslq	%eax, %r14
	leal	-1(%rax), %ecx
	movl	%eax, r(%rip)
	salq	$2, %r14
	imull	%r10d, %ecx
	sall	%cl, %ebx
	subl	$1, %ebx
.LVL12:
.L5:
	.loc 1 215 0
	movslq	%edx, %rcx
	.loc 1 216 0
	cmpl	%ebp, %edx
	.loc 1 215 0
	leaq	(%rdi,%rcx,4), %r9
.LVL13:
	leal	-1(%r8), %ecx
	movl	%ecx, (%r9)
	.loc 1 216 0
	jge	.L76
.LVL14:
	.loc 1 239 0
	addq	%rdi, %r14
.LVL15:
	cmpq	%r14, %r9
	jb	.L33
	.loc 1 239 0 is_stmt 0 discriminator 2
	movq	%r14, %rdx
.LVL16:
	movq	%rdi, %rsi
.LVL17:
	movl	%r10d, %ecx
.LVL18:
	.p2align 4,,10
	.p2align 3
.L34:
	.loc 1 240 0 is_stmt 1 discriminator 2
	movl	%r11d, (%rsi)
	.loc 1 241 0 discriminator 2
	movl	%r11d, %eax
	.loc 1 239 0 discriminator 2
	addq	$4, %rsi
.LVL19:
	.loc 1 241 0 discriminator 2
	movl	(%rdx), %r11d
.LVL20:
	andl	%ebx, %eax
.LVL21:
	.loc 1 239 0 discriminator 2
	addq	$4, %rdx
.LVL22:
	.loc 1 241 0 discriminator 2
	sall	%cl, %eax
	subl	%r8d, %r11d
	addl	$1, %r11d
	orl	%eax, %r11d
.LVL23:
	.loc 1 239 0 discriminator 2
	cmpq	%rdx, %r9
	jae	.L34
	.loc 1 202 0
	movq	%r9, %rax
	movl	%ecx, %r10d
	subq	%r14, %rax
	shrq	$2, %rax
	leaq	4(%rdi,%rax,4), %rdi
.LVL24:
.L33:
	.loc 1 243 0 discriminator 1
	cmpq	%rdi, %r9
	movl	%r10d, %ecx
	jbe	.L37
	.p2align 4,,10
	.p2align 3
.L53:
	.loc 1 244 0
	movl	%r11d, (%rdi)
	.loc 1 245 0
	andl	%ebx, %r11d
.LVL25:
	.loc 1 244 0
	addq	$4, %rdi
.LVL26:
	.loc 1 245 0
	sall	%cl, %r11d
.LVL27:
	.loc 1 243 0
	cmpq	%rdi, %r9
	ja	.L53
.L37:
	.loc 1 247 0
	leal	1(%rbp), %eax
.LVL28:
.L31:
	.loc 1 249 0
	movl	$0, (%r9)
	.loc 1 251 0
	popq	%rbx
.LCFI6:
	.cfi_remember_state
	.cfi_def_cfa_offset 48
.LVL29:
	popq	%rbp
.LCFI7:
	.cfi_def_cfa_offset 40
	popq	%r12
.LCFI8:
	.cfi_def_cfa_offset 32
	popq	%r13
.LCFI9:
	.cfi_def_cfa_offset 24
	popq	%r14
.LCFI10:
	.cfi_def_cfa_offset 16
	popq	%r15
.LCFI11:
	.cfi_def_cfa_offset 8
	ret
.LVL30:
	.p2align 4,,10
	.p2align 3
.L76:
.LCFI12:
	.cfi_restore_state
	.loc 1 217 0
	leaq	(%rsi,%rbp,4), %r12
	cmpq	%r12, %rsi
	ja	.L22
.LVL31:
.L11:
	.loc 1 202 0
	movq	%rsi, %rcx
	movq	%r12, %r13
	movq	%rsi, %rdx
	andl	$15, %ecx
	subq	%rsi, %r13
	shrq	$2, %rcx
	shrq	$2, %r13
	negq	%rcx
	addq	$1, %r13
	andl	$3, %ecx
	cmpq	%rcx, %r13
	cmovbe	%r13, %rcx
	cmpq	$6, %r13
	cmovbe	%r13, %rcx
	testq	%rcx, %rcx
	je	.L15
.LVL32:
	xorl	%ebp, %ebp
.LVL33:
	.p2align 4,,10
	.p2align 3
.L16:
	addq	$1, %rbp
	.loc 1 218 0
	movl	$0, (%rdx)
	.loc 1 217 0
	addq	$4, %rdx
.LVL34:
	cmpq	%rcx, %rbp
	jb	.L16
	cmpq	%rcx, %r13
	je	.L22
.L15:
	subq	%rcx, %r13
	movq	%r13, -8(%rsp)
	shrq	$2, %r13
	leaq	0(,%r13,4), %r15
	testq	%r15, %r15
	je	.L52
	leaq	(%rsi,%rcx,4), %rbp
	.loc 1 202 0
	xorl	%ecx, %ecx
	.loc 1 218 0
	pxor	%xmm0, %xmm0
.LVL35:
	.p2align 4,,10
	.p2align 3
.L19:
	addq	$1, %rcx
	.loc 1 218 0 is_stmt 0 discriminator 2
	movdqa	%xmm0, 0(%rbp)
	addq	$16, %rbp
	cmpq	%r13, %rcx
	jb	.L19
	cmpq	%r15, -8(%rsp)
	leaq	(%rdx,%r15,4), %rdx
	je	.L22
	.p2align 4,,10
	.p2align 3
.L52:
	.loc 1 218 0
	movl	$0, (%rdx)
	.loc 1 217 0 is_stmt 1
	addq	$4, %rdx
.LVL36:
	cmpq	%r12, %rdx
	jbe	.L52
.LVL37:
.L22:
	.loc 1 219 0
	addq	%rdi, %r14
.LVL38:
	movl	%r11d, %edx
	movl	%r10d, %ecx
	cmpq	%r9, %r14
	movq	%r14, %rbp
	ja	.L13
.LVL39:
	.p2align 4,,10
	.p2align 3
.L51:
	.loc 1 220 0 discriminator 2
	movslq	%edx, %r13
	movl	$1, (%rsi,%r13,4)
	.loc 1 221 0 discriminator 2
	movl	%edx, %r13d
	movl	0(%rbp), %edx
.LVL40:
	andl	%ebx, %r13d
.LVL41:
	.loc 1 219 0 discriminator 2
	addq	$4, %rbp
.LVL42:
	.loc 1 221 0 discriminator 2
	sall	%cl, %r13d
	subl	%r8d, %edx
	addl	$1, %edx
	orl	%r13d, %edx
.LVL43:
	.loc 1 219 0 discriminator 2
	cmpq	%r9, %rbp
	jbe	.L51
	movl	%ecx, %r10d
.L13:
.LVL44:
	.loc 1 223 0 discriminator 1
	cmpl	$1, %eax
	movl	$1, %ebp
.LVL45:
	movl	%r10d, %ecx
	jle	.L27
.LVL46:
	.p2align 4,,10
	.p2align 3
.L50:
	.loc 1 224 0 discriminator 2
	movslq	%edx, %r13
	.loc 1 223 0 discriminator 2
	addl	$1, %ebp
	.loc 1 225 0 discriminator 2
	andl	%ebx, %edx
.LVL47:
	sall	%cl, %edx
.LVL48:
	.loc 1 223 0 discriminator 2
	cmpl	%eax, %ebp
	.loc 1 224 0 discriminator 2
	movl	$1, (%rsi,%r13,4)
	.loc 1 223 0 discriminator 2
	jne	.L50
	movl	%ecx, %r10d
.LVL49:
.L27:
	.loc 1 227 0
	cmpq	%r12, %rsi
	movq	%rsi, %rdx
.LVL50:
	movl	$1, %eax
	ja	.L26
.LVL51:
	.p2align 4,,10
	.p2align 3
.L49:
	.loc 1 228 0
	movl	(%rdx), %ecx
	testl	%ecx, %ecx
	je	.L28
	.loc 1 229 0
	movl	%eax, (%rdx)
	addl	$1, %eax
.LVL52:
.L28:
	.loc 1 227 0
	addq	$4, %rdx
.LVL53:
	cmpq	%r12, %rdx
	jbe	.L49
.LVL54:
.L26:
	.loc 1 230 0 discriminator 1
	cmpq	%r9, %r14
	ja	.L29
	.loc 1 230 0 is_stmt 0 discriminator 2
	movq	%r14, %rdx
	movq	%rdi, %r12
	movl	%r10d, %ecx
.LVL55:
	.p2align 4,,10
	.p2align 3
.L30:
	.loc 1 231 0 is_stmt 1 discriminator 2
	movslq	%r11d, %rbp
	movl	(%rsi,%rbp,4), %ebp
	movl	%ebp, (%r12)
	.loc 1 232 0 discriminator 2
	movl	%r11d, %ebp
	.loc 1 230 0 discriminator 2
	addq	$4, %r12
.LVL56:
	.loc 1 232 0 discriminator 2
	movl	(%rdx), %r11d
.LVL57:
	andl	%ebx, %ebp
.LVL58:
	.loc 1 230 0 discriminator 2
	addq	$4, %rdx
.LVL59:
	.loc 1 232 0 discriminator 2
	sall	%cl, %ebp
	subl	%r8d, %r11d
	addl	$1, %r11d
	orl	%ebp, %r11d
.LVL60:
	.loc 1 230 0 discriminator 2
	cmpq	%r9, %rdx
	jbe	.L30
	.loc 1 202 0
	movq	%r9, %rdx
.LVL61:
	movl	%ecx, %r10d
	subq	%r14, %rdx
	shrq	$2, %rdx
	leaq	4(%rdi,%rdx,4), %rdi
.LVL62:
.L29:
	.loc 1 234 0 discriminator 1
	cmpq	%r9, %rdi
	jae	.L31
	movl	%r10d, %ecx
	.p2align 4,,10
	.p2align 3
.L48:
	.loc 1 235 0
	movslq	%r11d, %rdx
	.loc 1 236 0
	andl	%ebx, %r11d
.LVL63:
	.loc 1 235 0
	movl	(%rsi,%rdx,4), %edx
.LVL64:
	.loc 1 236 0
	sall	%cl, %r11d
	.loc 1 235 0
	movl	%edx, (%rdi)
	addq	$4, %rdi
.LVL65:
	.loc 1 234 0
	cmpq	%r9, %rdi
	jb	.L48
	jmp	.L31
.LVL66:
.L6:
	.loc 1 214 0
	movl	%r10d, %ecx
	movl	$1, %ebx
	.loc 1 215 0
	movslq	%edx, %rdx
	.loc 1 214 0
	negl	%ecx
	.loc 1 217 0
	movq	%rsi, %r12
	.loc 1 215 0
	xorl	%r14d, %r14d
	leal	-1(%r8), %eax
	.loc 1 214 0
	sall	%cl, %ebx
	.loc 1 215 0
	xorl	%r11d, %r11d
	leaq	(%rdi,%rdx,4), %r9
.LVL67:
	.loc 1 214 0
	subl	$1, %ebx
.LVL68:
	.loc 1 215 0
	movl	%eax, (%r9)
.LVL69:
	xorl	%eax, %eax
	jmp	.L11
.LVL70:
.L75:
	movl	%r10d, %ecx
	movl	$1, %ebx
	.loc 1 210 0
	xorl	%r14d, %r14d
.LVL71:
	negl	%ecx
	xorl	%eax, %eax
.LVL72:
	xorl	%ebp, %ebp
	sall	%cl, %ebx
	xorl	%r11d, %r11d
	subl	$1, %ebx
	jmp	.L5
.LVL73:
.L2:
	.loc 1 207 0
	xorl	%r10d, %r10d
	.loc 1 210 0
	testl	%edx, %edx
	.loc 1 209 0
	movl	$2147483647, %r14d
	.loc 1 210 0
	movl	$0, r(%rip)
.LVL74:
	jg	.L38
	xorl	%r14d, %r14d
	xorl	%ebx, %ebx
	xorl	%eax, %eax
	xorl	%ebp, %ebp
	xorl	%r11d, %r11d
	jmp	.L5
	.cfi_endproc
.LFE5:
	.size	transform, .-transform
	.p2align 4,,15
	.type	sort_split, @function
sort_split:
.LFB3:
	.loc 1 100 0
	.cfi_startproc
.LVL75:
	pushq	%r15
.LCFI13:
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	movl	%esi, %r15d
	pushq	%r14
.LCFI14:
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
.LCFI15:
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
.LCFI16:
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	movq	%rdi, %r12
	pushq	%rbp
.LCFI17:
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
.LCFI18:
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$8, %rsp
.LCFI19:
	.cfi_def_cfa_offset 64
	.loc 1 104 0
	cmpl	$6, %esi
	movq	V(%rip), %rbp
	movq	I(%rip), %rbx
	jle	.L157
.LVL76:
.L151:
.LBB10:
.LBB11:
	.loc 1 78 0
	movl	%r15d, %eax
	sarl	%eax
	.loc 1 79 0
	cmpl	$7, %r15d
	.loc 1 78 0
	cltq
	leaq	(%r12,%rax,4), %rdx
.LVL77:
	.loc 1 79 0
	je	.L158
.LVL78:
	.loc 1 84 0
	movl	h(%rip), %r10d
	.loc 1 81 0
	movslq	%r15d, %r13
	.loc 1 84 0
	movl	(%r12), %eax
	.loc 1 81 0
	leaq	-4(%r12,%r13,4), %r9
.LVL79:
	.loc 1 84 0
	addl	%r10d, %eax
	.loc 1 82 0
	cmpl	$40, %r15d
	.loc 1 84 0
	cltq
	.loc 1 82 0
	jg	.L91
	movl	(%rdx), %edx
.LVL80:
	movl	0(%rbp,%rax,4), %eax
	addl	%r10d, %edx
	movslq	%edx, %rdx
	movl	0(%rbp,%rdx,4), %esi
	movl	(%r9), %edx
	addl	%r10d, %edx
.LVL81:
.L92:
	.loc 1 88 0
	movslq	%edx, %rdx
	cmpl	%esi, %eax
	movl	0(%rbp,%rdx,4), %edx
	jge	.L98
.L168:
	cmpl	%esi, %edx
	jg	.L90
	cmpl	%eax, %edx
	movl	%eax, %esi
	cmovg	%edx, %esi
.LVL82:
.L90:
.LBE11:
.LBE10:
	.loc 1 99 0
	leaq	4(%r12), %rdi
.LBB17:
.LBB12:
	.loc 1 111 0
	movq	%r9, %rax
	.loc 1 99 0
	movq	%r12, %r11
	jmp	.L99
.LVL83:
	.p2align 4,,10
	.p2align 3
.L161:
.LBE12:
.LBE17:
	.loc 1 113 0 discriminator 2
	movl	-4(%rdi), %r14d
	leal	(%r14,%r10), %edx
	movslq	%edx, %rdx
.LVL84:
	cmpl	%esi, 0(%rbp,%rdx,4)
	jg	.L159
	.loc 1 114 0
	je	.L160
.LVL85:
.L122:
	addq	$4, %rdi
.L99:
	.loc 1 99 0 discriminator 1
	leaq	-4(%rdi), %r8
.LVL86:
	.loc 1 113 0 discriminator 1
	cmpq	%r8, %rax
	jae	.L161
.L101:
.LVL87:
	.loc 1 134 0
	movq	%r11, %rdx
	movq	%r8, %r14
	.loc 1 136 0
	movq	%r8, %rsi
	.loc 1 134 0
	subq	%r12, %rdx
	subq	%r11, %r14
	shrq	$2, %rdx
.LVL88:
	shrq	$2, %r14
.LVL89:
	cmpl	%edx, %r14d
	cmovle	%r14d, %edx
.LVL90:
	.loc 1 136 0
	movslq	%edx, %rcx
	salq	$2, %rcx
	subq	%rcx, %rsi
	testl	%edx, %edx
	movq	%rsi, %rcx
.LVL91:
	je	.L123
	.loc 1 99 0
	leaq	16(%rcx), %rdi
	movl	%edx, %esi
	movl	%edx, %r11d
.LVL92:
	leaq	16(%r12), %r15
	shrl	$2, %esi
	cmpq	%rdi, %r12
	leal	0(,%rsi,4), %r10d
	setae	%dil
	cmpq	%r15, %rcx
	setae	%r15b
	orl	%r15d, %edi
	cmpl	$6, %edx
	seta	%r15b
	testb	%r15b, %dil
	je	.L134
	testl	%r10d, %r10d
	je	.L134
.LBB18:
.LBB13:
	xorl	%edi, %edi
	xorl	%r15d, %r15d
.LVL93:
.L107:
.LBE13:
.LBE18:
	.loc 1 137 0 discriminator 2
	movdqu	(%r12,%rdi), %xmm0
.LVL94:
	addl	$1, %r15d
	movdqu	(%rcx,%rdi), %xmm1
	movdqu	%xmm1, (%r12,%rdi)
	movdqu	%xmm0, (%rcx,%rdi)
	addq	$16, %rdi
	cmpl	%esi, %r15d
	jb	.L107
	.loc 1 99 0
	movl	%r10d, %edi
	subl	%r10d, %edx
	salq	$2, %rdi
	leaq	(%r12,%rdi), %rsi
	addq	%rdi, %rcx
	cmpl	%r10d, %r11d
	je	.L123
	.p2align 4,,10
	.p2align 3
.L142:
	.loc 1 137 0
	movl	(%rsi), %edi
.LVL95:
	movl	(%rcx), %r10d
	movl	%r10d, (%rsi)
	.loc 1 136 0
	addq	$4, %rsi
.LVL96:
	.loc 1 137 0
	movl	%edi, (%rcx)
	.loc 1 136 0
	addq	$4, %rcx
.LVL97:
	subl	$1, %edx
.LVL98:
	jne	.L142
.LVL99:
.L123:
	.loc 1 133 0
	leaq	0(,%r13,4), %rdi
	.loc 1 138 0
	movq	%r9, %r15
	.loc 1 133 0
	leaq	(%r12,%rdi), %rdx
	.loc 1 138 0
	subq	%rax, %r15
	movq	%rdx, %rax
.LVL100:
	shrq	$2, %r15
.LVL101:
	subq	%r9, %rax
.LVL102:
	movl	%r15d, %ecx
	sarq	$2, %rax
	subl	$1, %eax
	cmpl	%r15d, %eax
	cmovle	%eax, %ecx
.LVL103:
	.loc 1 140 0
	movslq	%ecx, %rax
	salq	$2, %rax
	subq	%rax, %rdx
.LVL104:
	testl	%ecx, %ecx
	je	.L110
	.loc 1 99 0
	subq	%rax, %rdi
.LVL105:
	movl	%ecx, %r9d
.LVL106:
	movl	%ecx, %r11d
	leaq	16(%r8), %rsi
	addq	%r12, %rdi
	shrl	$2, %r9d
	leaq	16(%rdi), %rax
	leal	0(,%r9,4), %r10d
	cmpq	%rax, %r8
	setae	%al
	cmpq	%rsi, %rdi
	setae	%sil
	orl	%esi, %eax
	cmpl	$6, %ecx
	seta	%sil
	testb	%sil, %al
	je	.L141
	testl	%r10d, %r10d
	je	.L141
.LBB19:
.LBB14:
	xorl	%eax, %eax
	xorl	%esi, %esi
.LVL107:
	.p2align 4,,10
	.p2align 3
.L112:
.LBE14:
.LBE19:
	.loc 1 141 0 discriminator 2
	movdqu	(%r8,%rax), %xmm0
	addl	$1, %esi
	movdqu	(%rdi,%rax), %xmm1
	movdqu	%xmm1, (%r8,%rax)
	movdqu	%xmm0, (%rdi,%rax)
	addq	$16, %rax
	cmpl	%r9d, %esi
	jb	.L112
	.loc 1 99 0
	movl	%r10d, %eax
	subl	%r10d, %ecx
	salq	$2, %rax
	addq	%rax, %r8
.LVL108:
	addq	%rax, %rdx
	cmpl	%r11d, %r10d
	je	.L110
	.p2align 4,,10
	.p2align 3
.L141:
	.loc 1 141 0
	movl	(%r8), %eax
.LVL109:
	movl	(%rdx), %esi
	movl	%esi, (%r8)
	.loc 1 140 0
	addq	$4, %r8
.LVL110:
	.loc 1 141 0
	movl	%eax, (%rdx)
	.loc 1 140 0
	addq	$4, %rdx
.LVL111:
	subl	$1, %ecx
.LVL112:
	jne	.L141
.LVL113:
.L110:
	.loc 1 145 0
	testl	%r14d, %r14d
	jle	.L115
	.loc 1 146 0
	movl	%r14d, %esi
	movq	%r12, %rdi
	call	sort_split
.LVL114:
.L115:
	.loc 1 147 0
	movslq	%r15d, %rdi
	movslq	%r14d, %r14
	leaq	(%r12,%r14,4), %rax
.LVL115:
	subq	%rdi, %r13
.LVL116:
	leaq	-4(%r12,%r13,4), %rcx
.LVL117:
.LBB20:
.LBB21:
	.loc 1 35 0
	movslq	(%rax), %r8
	.loc 1 34 0
	movq	%rcx, %rdx
	subq	%rbx, %rdx
	sarq	$2, %rdx
	.loc 1 36 0
	cmpq	%rax, %rcx
	.loc 1 34 0
	movl	%edx, %esi
.LVL118:
	.loc 1 35 0
	movl	%edx, 0(%rbp,%r8,4)
	.loc 1 36 0
	je	.L162
.LVL119:
	.p2align 4,,10
	.p2align 3
.L140:
	.loc 1 40 0
	addq	$4, %rax
.LVL120:
	movslq	(%rax), %rdx
	.loc 1 41 0
	cmpq	%rax, %rcx
	.loc 1 40 0
	movl	%esi, 0(%rbp,%rdx,4)
	.loc 1 41 0
	ja	.L140
.LBE21:
.LBE20:
	.loc 1 148 0
	testl	%r15d, %r15d
	jle	.L77
.L169:
	.loc 1 149 0
	leaq	(%r12,%r13,4), %r12
.LVL121:
	.loc 1 104 0
	cmpl	$6, %r15d
	jg	.L151
.LVL122:
.L119:
.LBB23:
.LBB24:
	.loc 1 52 0
	leaq	-4(%r12,%rdi,4), %r10
.LVL123:
	.loc 1 53 0
	cmpq	%r12, %r10
	jbe	.L125
	movq	V(%rip), %rdi
	movl	h(%rip), %r11d
	movq	I(%rip), %rbx
.LVL124:
.L87:
	.loc 1 54 0
	movslq	(%r12), %rbp
	leaq	4(%r12), %rdx
.LVL125:
	cmpq	%rdx, %r10
	movq	%rdx, %r9
	leal	0(%rbp,%r11), %eax
	cltq
	movl	(%rdi,%rax,4), %r8d
.LVL126:
	jb	.L80
	movq	%rdx, %rax
	jmp	.L83
.LVL127:
	.p2align 4,,10
	.p2align 3
.L164:
	.loc 1 57 0
	movl	(%r12), %r8d
	movq	%rdx, %r9
.LVL128:
	movl	%r8d, (%rax)
	movl	%esi, %r8d
	movl	%ecx, (%r12)
.LVL129:
.L82:
	.loc 1 54 0
	addq	$4, %rax
.LVL130:
	cmpq	%rax, %r10
	jb	.L163
.LVL131:
.L83:
	.loc 1 55 0
	movl	(%rax), %ecx
	leal	(%rcx,%r11), %esi
	movslq	%esi, %rsi
	movl	(%rdi,%rsi,4), %esi
.LVL132:
	cmpl	%r8d, %esi
	jl	.L164
	.loc 1 59 0
	jne	.L82
.LVL133:
	.loc 1 60 0
	movl	(%r9), %esi
.LVL134:
	movl	%esi, (%rax)
	.loc 1 54 0
	addq	$4, %rax
.LVL135:
	.loc 1 60 0
	movl	%ecx, (%r9)
	.loc 1 61 0
	addq	$4, %r9
.LVL136:
	.loc 1 54 0
	cmpq	%rax, %r10
	jae	.L83
.LVL137:
.L163:
	movslq	(%r12), %rbp
.LVL138:
.L80:
	.loc 1 63 0
	leaq	-4(%r9), %rcx
.LVL139:
.LBB25:
.LBB26:
	.loc 1 34 0
	movq	%rcx, %rax
	subq	%rbx, %rax
	sarq	$2, %rax
	.loc 1 36 0
	cmpq	%rcx, %r12
	.loc 1 34 0
	movl	%eax, %esi
.LVL140:
	.loc 1 35 0
	movl	%eax, (%rdi,%rbp,4)
	.loc 1 36 0
	jne	.L152
	jmp	.L171
.LVL141:
	.p2align 4,,10
	.p2align 3
.L166:
	addq	$4, %rdx
.LVL142:
.L152:
	.loc 1 40 0
	movslq	(%rdx), %rax
	.loc 1 41 0
	cmpq	%rdx, %rcx
	.loc 1 40 0
	movl	%esi, (%rdi,%rax,4)
	.loc 1 41 0
	ja	.L166
.LVL143:
.LBE26:
.LBE25:
	.loc 1 53 0
	cmpq	%r10, %r9
	jae	.L79
.LVL144:
.L127:
	movq	%r9, %r12
	jmp	.L87
.LVL145:
	.p2align 4,,10
	.p2align 3
.L159:
.LBE24:
.LBE23:
	.loc 1 120 0
	movl	(%rax), %ecx
	leal	(%r10,%rcx), %edx
.LVL146:
	movslq	%edx, %rdx
	movl	0(%rbp,%rdx,4), %edx
.LVL147:
	cmpl	%esi, %edx
	jge	.L143
	jmp	.L106
.LVL148:
	.p2align 4,,10
	.p2align 3
.L104:
	.loc 1 125 0
	subq	$4, %rax
.LVL149:
	.loc 1 120 0
	cmpq	%r8, %rax
	jb	.L101
	.loc 1 120 0 is_stmt 0 discriminator 2
	movl	(%rax), %ecx
	leal	(%r10,%rcx), %edx
	movslq	%edx, %rdx
	movl	0(%rbp,%rdx,4), %edx
.LVL150:
	cmpl	%esi, %edx
	jl	.L167
.L143:
	.loc 1 121 0 is_stmt 1
	cmpl	%esi, %edx
	jne	.L104
.LVL151:
	.loc 1 122 0
	movl	(%r9), %edx
.LVL152:
	movl	%edx, (%rax)
	movl	%ecx, (%r9)
	.loc 1 123 0
	subq	$4, %r9
.LVL153:
	jmp	.L104
.LVL154:
	.p2align 4,,10
	.p2align 3
.L167:
	movl	-4(%rdi), %r14d
.L106:
.LVL155:
	.loc 1 129 0
	movl	%ecx, -4(%rdi)
	movl	%r14d, (%rax)
.LVL156:
	.loc 1 131 0
	subq	$4, %rax
.LVL157:
	.loc 1 132 0
	jmp	.L122
.LVL158:
.L160:
	.loc 1 115 0
	movl	(%r11), %edx
.LVL159:
	movl	%r14d, (%r11)
	.loc 1 116 0
	addq	$4, %r11
.LVL160:
	.loc 1 115 0
	movl	%edx, -4(%rdi)
	jmp	.L122
.LVL161:
.L158:
	movl	(%rdx), %eax
	leaq	24(%r12), %r9
.LBB31:
.LBB15:
	.loc 1 79 0
	movl	$7, %r13d
	movl	h(%rip), %r10d
	addl	%r10d, %eax
	cltq
	movl	0(%rbp,%rax,4), %esi
	jmp	.L90
.LVL162:
.L91:
	.loc 1 83 0
	sarl	$3, %r15d
.LVL163:
	.loc 1 84 0
	movl	0(%rbp,%rax,4), %esi
	movslq	%r15d, %rcx
	movl	(%r12,%rcx,4), %eax
	leaq	0(,%rcx,4), %r11
	movl	(%r12,%rcx,8), %edi
	addl	%r10d, %eax
	cltq
	addl	%r10d, %edi
	movl	0(%rbp,%rax,4), %eax
	movslq	%edi, %rdi
	movl	0(%rbp,%rdi,4), %edi
	cmpl	%eax, %esi
	jge	.L93
	cmpl	%edi, %eax
	jl	.L94
	cmpl	%edi, %esi
	movl	%edi, %eax
	cmovge	%esi, %eax
.L94:
.LVL164:
	.loc 1 85 0
	movq	%r11, %rdi
	negq	%rdi
	movl	(%rdx,%rdi), %esi
	addl	%r10d, %esi
	movslq	%esi, %rsi
	movl	0(%rbp,%rsi,4), %r8d
	movl	(%rdx), %esi
	movl	(%rdx,%rcx,4), %edx
.LVL165:
	addl	%r10d, %esi
	movslq	%esi, %rsi
	addl	%r10d, %edx
	movl	0(%rbp,%rsi,4), %esi
	movslq	%edx, %rdx
	movl	0(%rbp,%rdx,4), %edx
	cmpl	%esi, %r8d
	jge	.L95
	cmpl	%edx, %esi
	jl	.L96
	cmpl	%edx, %r8d
	movl	%edx, %esi
	cmovge	%r8d, %esi
.L96:
.LVL166:
	.loc 1 86 0
	movq	%rdi, %rdx
	subq	%r11, %rdx
	movl	(%r9), %r11d
	movl	(%r9,%rdx), %ecx
	addl	%r10d, %r11d
	addl	%r10d, %ecx
	movslq	%r11d, %r14
	movslq	%ecx, %rdx
	movl	0(%rbp,%r14,4), %r14d
	movl	0(%rbp,%rdx,4), %r8d
	movl	(%r9,%rdi), %edx
	addl	%r10d, %edx
	movslq	%edx, %rdi
	movl	0(%rbp,%rdi,4), %edi
	cmpl	%edi, %r8d
	jge	.L97
	cmpl	%r14d, %edi
	jl	.L92
	cmpl	%r14d, %r8d
	movl	%ecx, %edx
	cmovl	%r11d, %edx
.LVL167:
	.loc 1 88 0
	cmpl	%esi, %eax
	movslq	%edx, %rdx
	movl	0(%rbp,%rdx,4), %edx
	jl	.L168
.LVL168:
.L98:
	cmpl	%esi, %edx
	jl	.L90
	cmpl	%eax, %edx
	movl	%eax, %esi
	cmovl	%edx, %esi
	jmp	.L90
.LVL169:
.L95:
	.loc 1 85 0
	cmpl	%edx, %esi
	jg	.L96
	cmpl	%edx, %r8d
	movl	%edx, %esi
	cmovle	%r8d, %esi
	jmp	.L96
.LVL170:
.L93:
	.loc 1 84 0
	cmpl	%edi, %eax
	jg	.L94
	cmpl	%edi, %esi
	movl	%edi, %eax
	cmovle	%esi, %eax
	jmp	.L94
.LVL171:
.L97:
	.loc 1 86 0
	cmpl	%r14d, %edi
	jg	.L92
	cmpl	%r14d, %r8d
	movl	%ecx, %edx
	cmovg	%r11d, %edx
	jmp	.L92
.LVL172:
.L162:
.LBE15:
.LBE31:
	.loc 1 148 0
	testl	%r15d, %r15d
.LBB32:
.LBB22:
	.loc 1 37 0
	movl	$-1, (%rcx)
.LBE22:
.LBE32:
	.loc 1 148 0
	jg	.L169
.LVL173:
.L77:
	.loc 1 150 0
	addq	$8, %rsp
.LCFI20:
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
.LCFI21:
	.cfi_def_cfa_offset 48
	popq	%rbp
.LCFI22:
	.cfi_def_cfa_offset 40
	popq	%r12
.LCFI23:
	.cfi_def_cfa_offset 32
	popq	%r13
.LCFI24:
	.cfi_def_cfa_offset 24
	popq	%r14
.LCFI25:
	.cfi_def_cfa_offset 16
	popq	%r15
.LCFI26:
	.cfi_def_cfa_offset 8
	ret
.LVL174:
.L171:
.LCFI27:
	.cfi_restore_state
.LBB33:
.LBB29:
	.loc 1 53 0
	cmpq	%r10, %r9
.LBB28:
.LBB27:
	.loc 1 37 0
	movl	$-1, (%r12)
.LVL175:
.LBE27:
.LBE28:
	.loc 1 53 0
	jb	.L127
.LVL176:
.L79:
	.loc 1 66 0
	cmpq	%r9, %r10
	jne	.L77
.LVL177:
.L170:
	.loc 1 67 0
	movq	%r10, %rax
	movslq	(%r10), %rcx
	subq	I(%rip), %rax
	movq	V(%rip), %rdx
	sarq	$2, %rax
	movl	%eax, (%rdx,%rcx,4)
	.loc 1 68 0
	movl	$-1, (%r10)
.LBE29:
.LBE33:
	.loc 1 150 0
	addq	$8, %rsp
.LCFI28:
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
.LCFI29:
	.cfi_def_cfa_offset 48
	popq	%rbp
.LCFI30:
	.cfi_def_cfa_offset 40
	popq	%r12
.LCFI31:
	.cfi_def_cfa_offset 32
	popq	%r13
.LCFI32:
	.cfi_def_cfa_offset 24
	popq	%r14
.LCFI33:
	.cfi_def_cfa_offset 16
	popq	%r15
.LCFI34:
	.cfi_def_cfa_offset 8
	ret
.LVL178:
.L125:
.LCFI35:
	.cfi_restore_state
.LBB34:
.LBB30:
	.loc 1 53 0
	movq	%r12, %r9
	.loc 1 66 0
	cmpq	%r9, %r10
	jne	.L77
.LVL179:
	jmp	.L170
.LVL180:
.L157:
	movslq	%esi, %rdi
.LVL181:
	.p2align 4,,2
	jmp	.L119
.LVL182:
.L134:
.LBE30:
.LBE34:
.LBB35:
.LBB16:
	.loc 1 99 0
	movq	%r12, %rsi
	.p2align 4,,5
	jmp	.L142
.LBE16:
.LBE35:
	.cfi_endproc
.LFE3:
	.size	sort_split, .-sort_split
	.p2align 4,,15
	.globl	suffixsort
	.type	suffixsort, @function
suffixsort:
.LFB6:
	.loc 1 259 0
	.cfi_startproc
.LVL183:
	pushq	%r14
.LCFI36:
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	.loc 1 266 0
	movl	%ecx, %eax
	.loc 1 259 0
	pushq	%r13
.LCFI37:
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	.loc 1 266 0
	subl	%r8d, %eax
	.loc 1 259 0
	pushq	%r12
.LCFI38:
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	.loc 1 266 0
	cmpl	%edx, %eax
	.loc 1 259 0
	movl	%edx, %r12d
	pushq	%rbp
.LCFI39:
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	movq	%rdi, %rbp
	pushq	%rbx
.LCFI40:
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	.loc 1 259 0
	movq	%rsi, %rbx
	.loc 1 263 0
	movq	%rdi, V(%rip)
	.loc 1 264 0
	movq	%rsi, I(%rip)
	.loc 1 266 0
	jg	.L173
	.loc 1 267 0
	movl	%edx, %r9d
	call	transform
.LVL184:
.LBB38:
.LBB39:
	.loc 1 165 0
	movslq	%eax, %r8
	leaq	(%rbx,%r8,4), %rsi
	cmpq	%rsi, %rbx
	jae	.L174
	.loc 1 258 0
	movq	%rbx, %rdi
	movq	%rbx, %rcx
.LBE39:
.LBE38:
	movq	%rbx, %rax
.LVL185:
.LBB47:
.LBB40:
	andl	$15, %ecx
	notq	%rdi
	addq	%rsi, %rdi
	shrq	$2, %rcx
	shrq	$2, %rdi
	negq	%rcx
	addq	$1, %rdi
	andl	$3, %ecx
	cmpq	%rcx, %rdi
	cmovbe	%rdi, %rcx
.LBE40:
.LBE47:
	cmpq	$6, %rdi
	cmovbe	%rdi, %rcx
	testq	%rcx, %rcx
	je	.L176
	xorl	%edx, %edx
.LVL186:
	.p2align 4,,10
	.p2align 3
.L177:
	addq	$1, %rdx
.LBB48:
.LBB41:
	.loc 1 166 0
	movl	$-1, (%rax)
	.loc 1 165 0
	addq	$4, %rax
.LVL187:
	cmpq	%rdx, %rcx
	ja	.L177
	cmpq	%rcx, %rdi
	je	.L174
.L176:
	movq	%rdi, %r10
	subq	%rcx, %r10
	movq	%r10, %rdi
	shrq	$2, %rdi
	leaq	0(,%rdi,4), %r9
	testq	%r9, %r9
	je	.L220
	leaq	(%rbx,%rcx,4), %rcx
.LBE41:
.LBE48:
	.loc 1 258 0
	xorl	%edx, %edx
.LBB49:
.LBB42:
	.loc 1 166 0
	pcmpeqd	%xmm0, %xmm0
.LVL188:
	.p2align 4,,10
	.p2align 3
.L180:
	addq	$1, %rdx
	movdqa	%xmm0, (%rcx)
	addq	$16, %rcx
	cmpq	%rdi, %rdx
	jb	.L180
	leaq	(%rax,%r9,4), %rax
	cmpq	%r9, %r10
	je	.L174
	.p2align 4,,10
	.p2align 3
.L220:
	movl	$-1, (%rax)
	.loc 1 165 0
	addq	$4, %rax
.LVL189:
	cmpq	%rax, %rsi
	ja	.L220
.LVL190:
.L174:
	.loc 1 167 0
	testl	%r12d, %r12d
	js	.L183
	xorl	%eax, %eax
.LVL191:
	.p2align 4,,10
	.p2align 3
.L184:
	.loc 1 168 0
	movslq	0(%rbp,%rax,4), %rdx
	leaq	(%rbx,%rdx,4), %rdx
	movl	(%rdx), %ecx
	movl	%ecx, 0(%rbp,%rax,4)
	.loc 1 169 0
	movl	%eax, (%rdx)
.LVL192:
	addq	$1, %rax
.LVL193:
	.loc 1 167 0
	cmpl	%eax, %r12d
	jge	.L184
.LVL194:
.L183:
	.loc 1 171 0
	leaq	-4(%rbx,%r8,4), %r9
.LVL195:
	cmpq	%r9, %rbx
	ja	.L185
	movl	%r12d, %edi
.LVL196:
	.p2align 4,,10
	.p2align 3
.L189:
	.loc 1 172 0
	movl	(%r9), %ecx
.LVL197:
	movslq	%ecx, %rax
	leaq	0(%rbp,%rax,4), %rdx
	movl	(%rdx), %eax
.LVL198:
	.loc 1 173 0
	movl	%edi, (%rdx)
	.loc 1 174 0
	testl	%eax, %eax
	js	.L186
	.loc 1 175 0
	leal	-1(%rdi), %esi
.LVL199:
	movslq	%edi, %rdx
	movl	%ecx, (%rbx,%rdx,4)
	.loc 1 258 0
	movslq	%esi, %rdx
	leaq	(%rbx,%rdx,4), %rdx
.LVL200:
	.p2align 4,,10
	.p2align 3
.L187:
	.loc 1 177 0
	movslq	%eax, %rcx
	.loc 1 179 0
	subl	$1, %esi
.LVL201:
	.loc 1 177 0
	leaq	0(%rbp,%rcx,4), %rcx
	movl	(%rcx), %r8d
.LVL202:
	.loc 1 178 0
	movl	%edi, (%rcx)
	.loc 1 179 0
	movl	%eax, (%rdx)
	subq	$4, %rdx
	.loc 1 180 0
	testl	%r8d, %r8d
	.loc 1 177 0
	movl	%r8d, %eax
.LVL203:
	.loc 1 180 0
	jns	.L187
	.loc 1 179 0
	movl	%esi, %edi
.LVL204:
.L188:
	.loc 1 171 0
	subq	$4, %r9
.LVL205:
	cmpq	%r9, %rbx
	jbe	.L189
	movq	I(%rip), %rbx
.LVL206:
.L185:
.LBE42:
.LBE49:
	.loc 1 276 0
	movl	r(%rip), %eax
	.loc 1 278 0
	movl	%r12d, %r13d
	negl	%r13d
	.loc 1 276 0
	movl	%eax, h(%rip)
	.loc 1 278 0
	movslq	(%rbx), %rax
	cmpl	%eax, %r13d
	jg	.L199
	movslq	%r12d, %rdx
	leaq	(%rbx,%rdx,4), %rbp
.LVL207:
	.p2align 4,,10
	.p2align 3
.L206:
.LBB50:
.LBB43:
	.loc 1 271 0
	movq	%rbx, %rdi
	xorl	%edx, %edx
	jmp	.L204
.LVL208:
	.p2align 4,,10
	.p2align 3
.L200:
.LBE43:
.LBE50:
	.loc 1 286 0
	testl	%edx, %edx
	je	.L202
	.loc 1 287 0
	movslq	%edx, %rcx
	movl	%edx, (%rdi,%rcx,4)
.LVL209:
.L202:
	.loc 1 290 0
	movq	V(%rip), %rdx
	movslq	(%rdx,%rax,4), %rax
.LVL210:
	leaq	4(%rbx,%rax,4), %r14
.LVL211:
	.loc 1 291 0
	movq	%r14, %rsi
	subq	%rdi, %rsi
	sarq	$2, %rsi
	call	sort_split
.LVL212:
	.loc 1 292 0
	movq	%r14, %rdi
	.loc 1 291 0
	xorl	%edx, %edx
.LVL213:
	.loc 1 294 0
	cmpq	%rbp, %rdi
	ja	.L203
.LVL214:
.L238:
	movslq	(%rdi), %rax
.LVL215:
.L204:
	.loc 1 282 0
	testl	%eax, %eax
	jns	.L200
	.loc 1 283 0
	movslq	%eax, %rcx
	.loc 1 284 0
	addl	%eax, %edx
.LVL216:
	.loc 1 283 0
	salq	$2, %rcx
	subq	%rcx, %rdi
.LVL217:
	.loc 1 294 0
	cmpq	%rbp, %rdi
	jbe	.L238
.LVL218:
.L203:
	.loc 1 295 0
	testl	%edx, %edx
	je	.L205
	.loc 1 296 0
	movslq	%edx, %rax
	movl	%edx, (%rdi,%rax,4)
.L205:
	.loc 1 278 0
	movslq	(%rbx), %rax
	.loc 1 297 0
	sall	h(%rip)
	.loc 1 278 0
	cmpl	%r13d, %eax
	jge	.L206
.LVL219:
.L199:
	.loc 1 300 0 discriminator 1
	testl	%r12d, %r12d
	js	.L172
	movq	V(%rip), %rcx
	.loc 1 300 0 is_stmt 0
	xorl	%eax, %eax
.LVL220:
	.p2align 4,,10
	.p2align 3
.L208:
	.loc 1 301 0 is_stmt 1 discriminator 2
	movslq	(%rcx,%rax,4), %rdx
	movl	%eax, (%rbx,%rdx,4)
.LVL221:
	addq	$1, %rax
.LVL222:
	.loc 1 300 0 discriminator 2
	cmpl	%eax, %r12d
	jge	.L208
.LVL223:
.L172:
	.loc 1 302 0
	popq	%rbx
.LCFI41:
	.cfi_remember_state
	.cfi_def_cfa_offset 40
	popq	%rbp
.LCFI42:
	.cfi_def_cfa_offset 32
	popq	%r12
.LCFI43:
	.cfi_def_cfa_offset 24
.LVL224:
	popq	%r13
.LCFI44:
	.cfi_def_cfa_offset 16
.LVL225:
	popq	%r14
.LCFI45:
	.cfi_def_cfa_offset 8
	ret
.LVL226:
.L173:
.LCFI46:
	.cfi_restore_state
	.loc 1 270 0
	movl	$2147483647, %r9d
	call	transform
.LVL227:
	leal	1(%r12), %esi
	.loc 1 271 0
	testl	%r12d, %r12d
	js	.L190
	.loc 1 258 0
	movq	%rbx, %rcx
	movq	%rbx, %rdx
	andl	$15, %ecx
	shrq	$2, %rcx
	negq	%rcx
	andl	$3, %ecx
	cmpl	%ecx, %esi
	cmovb	%esi, %ecx
.LBB51:
.LBB44:
	cmpl	$5, %esi
	cmovbe	%esi, %ecx
	xorl	%eax, %eax
	testl	%ecx, %ecx
	je	.L192
.LVL228:
	.p2align 4,,10
	.p2align 3
.L193:
.LBE44:
.LBE51:
	.loc 1 272 0
	movl	%eax, (%rdx)
	.loc 1 271 0
	addl	$1, %eax
.LVL229:
	addq	$4, %rdx
	cmpl	%ecx, %eax
	jb	.L193
	cmpl	%ecx, %esi
	je	.L190
.LVL230:
.L192:
	movl	%esi, %r9d
	movl	%ecx, %edx
	subl	%ecx, %r9d
	movl	%r9d, %edi
	shrl	$2, %edi
	leal	0(,%rdi,4), %r8d
	testl	%r8d, %r8d
	je	.L195
	leal	2(%rax), %r10d
	movd	%eax, %xmm0
	leal	1(%rax), %ecx
	movd	%r10d, %xmm1
	leal	3(%rax), %r10d
	pinsrd	$1, %ecx, %xmm0
	pinsrd	$1, %r10d, %xmm1
	punpcklqdq	%xmm1, %xmm0
	leaq	(%rbx,%rdx,4), %rcx
	movdqa	.LC0(%rip), %xmm1
.LBB52:
.LBB45:
	xorl	%edx, %edx
	.p2align 4,,10
	.p2align 3
.L196:
	addl	$1, %edx
.LBE45:
.LBE52:
	.loc 1 272 0 discriminator 2
	movdqa	%xmm0, (%rcx)
	addq	$16, %rcx
	cmpl	%edi, %edx
	paddd	%xmm1, %xmm0
	jb	.L196
	addl	%r8d, %eax
	cmpl	%r8d, %r9d
	je	.L190
.L195:
	.loc 1 258 0
	movslq	%eax, %rdx
	leaq	(%rbx,%rdx,4), %rdx
	.p2align 4,,10
	.p2align 3
.L198:
	.loc 1 272 0
	movl	%eax, (%rdx)
	.loc 1 271 0
	addl	$1, %eax
.LVL231:
	addq	$4, %rdx
	cmpl	%eax, %r12d
	jge	.L198
.LVL232:
.L190:
	.loc 1 273 0
	movl	$0, h(%rip)
	.loc 1 274 0
	movq	%rbx, %rdi
	call	sort_split
.LVL233:
	jmp	.L185
.LVL234:
	.p2align 4,,10
	.p2align 3
.L186:
.LBB53:
.LBB46:
	.loc 1 182 0
	movslq	%edi, %rax
.LVL235:
	subl	$1, %edi
.LVL236:
	movl	$-1, (%rbx,%rax,4)
	jmp	.L188
.LBE46:
.LBE53:
	.cfi_endproc
.LFE6:
	.size	suffixsort, .-suffixsort
	.local	V
	.comm	V,8,16
	.local	I
	.comm	I,8,16
	.local	h
	.comm	h,4,16
	.local	r
	.comm	r,4,16
	.section	.rodata.cst16,"aM",@progbits,16
	.align 16
.LC0:
	.long	4
	.long	4
	.long	4
	.long	4
	.text
.Letext0:
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x67d
	.value	0x2
	.long	.Ldebug_abbrev0
	.byte	0x8
	.uleb128 0x1
	.long	.LASF3
	.byte	0x1
	.long	.LASF4
	.long	.LASF5
	.quad	.Ltext0
	.quad	.Letext0
	.long	.Ldebug_line0
	.uleb128 0x2
	.long	.LASF6
	.byte	0x1
	.byte	0xca
	.byte	0x1
	.long	0x123
	.quad	.LFB5
	.quad	.LFE5
	.long	.LLST0
	.byte	0x1
	.long	0x123
	.uleb128 0x3
	.string	"x"
	.byte	0x1
	.byte	0xca
	.long	0x12a
	.long	.LLST1
	.uleb128 0x3
	.string	"p"
	.byte	0x1
	.byte	0xca
	.long	0x12a
	.long	.LLST2
	.uleb128 0x3
	.string	"n"
	.byte	0x1
	.byte	0xca
	.long	0x123
	.long	.LLST3
	.uleb128 0x3
	.string	"k"
	.byte	0x1
	.byte	0xca
	.long	0x123
	.long	.LLST4
	.uleb128 0x4
	.string	"l"
	.byte	0x1
	.byte	0xca
	.long	0x123
	.byte	0x1
	.byte	0x58
	.uleb128 0x3
	.string	"q"
	.byte	0x1
	.byte	0xca
	.long	0x123
	.long	.LLST5
	.uleb128 0x5
	.string	"b"
	.byte	0x1
	.byte	0xcc
	.long	0x123
	.long	.LLST6
	.uleb128 0x5
	.string	"c"
	.byte	0x1
	.byte	0xcc
	.long	0x123
	.long	.LLST7
	.uleb128 0x5
	.string	"d"
	.byte	0x1
	.byte	0xcc
	.long	0x123
	.long	.LLST8
	.uleb128 0x5
	.string	"e"
	.byte	0x1
	.byte	0xcc
	.long	0x123
	.long	.LLST9
	.uleb128 0x5
	.string	"i"
	.byte	0x1
	.byte	0xcc
	.long	0x123
	.long	.LLST10
	.uleb128 0x5
	.string	"j"
	.byte	0x1
	.byte	0xcc
	.long	0x123
	.long	.LLST11
	.uleb128 0x5
	.string	"m"
	.byte	0x1
	.byte	0xcc
	.long	0x123
	.long	.LLST12
	.uleb128 0x5
	.string	"s"
	.byte	0x1
	.byte	0xcc
	.long	0x123
	.long	.LLST13
	.uleb128 0x5
	.string	"pi"
	.byte	0x1
	.byte	0xcd
	.long	0x12a
	.long	.LLST14
	.uleb128 0x5
	.string	"pj"
	.byte	0x1
	.byte	0xcd
	.long	0x12a
	.long	.LLST15
	.byte	0
	.uleb128 0x6
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x7
	.byte	0x8
	.long	0x123
	.uleb128 0x8
	.long	.LASF0
	.byte	0x1
	.byte	0x2f
	.byte	0x1
	.byte	0x1
	.long	0x195
	.uleb128 0x9
	.string	"p"
	.byte	0x1
	.byte	0x2f
	.long	0x12a
	.uleb128 0x9
	.string	"n"
	.byte	0x1
	.byte	0x2f
	.long	0x123
	.uleb128 0xa
	.string	"pa"
	.byte	0x1
	.byte	0x30
	.long	0x12a
	.uleb128 0xa
	.string	"pb"
	.byte	0x1
	.byte	0x30
	.long	0x12a
	.uleb128 0xa
	.string	"pi"
	.byte	0x1
	.byte	0x30
	.long	0x12a
	.uleb128 0xa
	.string	"pn"
	.byte	0x1
	.byte	0x30
	.long	0x12a
	.uleb128 0xa
	.string	"f"
	.byte	0x1
	.byte	0x31
	.long	0x123
	.uleb128 0xa
	.string	"v"
	.byte	0x1
	.byte	0x31
	.long	0x123
	.uleb128 0xa
	.string	"tmp"
	.byte	0x1
	.byte	0x31
	.long	0x123
	.byte	0
	.uleb128 0x8
	.long	.LASF1
	.byte	0x1
	.byte	0x1e
	.byte	0x1
	.byte	0x1
	.long	0x1c0
	.uleb128 0x9
	.string	"pl"
	.byte	0x1
	.byte	0x1e
	.long	0x12a
	.uleb128 0x9
	.string	"pm"
	.byte	0x1
	.byte	0x1e
	.long	0x12a
	.uleb128 0xa
	.string	"g"
	.byte	0x1
	.byte	0x20
	.long	0x123
	.byte	0
	.uleb128 0xb
	.long	.LASF7
	.byte	0x1
	.byte	0x4a
	.byte	0x1
	.long	0x123
	.byte	0x1
	.long	0x20b
	.uleb128 0x9
	.string	"p"
	.byte	0x1
	.byte	0x4a
	.long	0x12a
	.uleb128 0x9
	.string	"n"
	.byte	0x1
	.byte	0x4a
	.long	0x123
	.uleb128 0xa
	.string	"pl"
	.byte	0x1
	.byte	0x4b
	.long	0x12a
	.uleb128 0xa
	.string	"pm"
	.byte	0x1
	.byte	0x4b
	.long	0x12a
	.uleb128 0xa
	.string	"pn"
	.byte	0x1
	.byte	0x4b
	.long	0x12a
	.uleb128 0xa
	.string	"s"
	.byte	0x1
	.byte	0x4c
	.long	0x123
	.byte	0
	.uleb128 0xc
	.long	.LASF8
	.byte	0x1
	.byte	0x63
	.byte	0x1
	.quad	.LFB3
	.quad	.LFE3
	.long	.LLST16
	.byte	0x1
	.long	0x42f
	.uleb128 0x3
	.string	"p"
	.byte	0x1
	.byte	0x63
	.long	0x12a
	.long	.LLST17
	.uleb128 0x3
	.string	"n"
	.byte	0x1
	.byte	0x63
	.long	0x123
	.long	.LLST18
	.uleb128 0x5
	.string	"pa"
	.byte	0x1
	.byte	0x65
	.long	0x12a
	.long	.LLST19
	.uleb128 0x5
	.string	"pb"
	.byte	0x1
	.byte	0x65
	.long	0x12a
	.long	.LLST20
	.uleb128 0x5
	.string	"pc"
	.byte	0x1
	.byte	0x65
	.long	0x12a
	.long	.LLST21
	.uleb128 0x5
	.string	"pd"
	.byte	0x1
	.byte	0x65
	.long	0x12a
	.long	.LLST22
	.uleb128 0x5
	.string	"pl"
	.byte	0x1
	.byte	0x65
	.long	0x12a
	.long	.LLST23
	.uleb128 0x5
	.string	"pm"
	.byte	0x1
	.byte	0x65
	.long	0x12a
	.long	.LLST24
	.uleb128 0x5
	.string	"pn"
	.byte	0x1
	.byte	0x65
	.long	0x12a
	.long	.LLST25
	.uleb128 0x5
	.string	"f"
	.byte	0x1
	.byte	0x66
	.long	0x123
	.long	.LLST26
	.uleb128 0xa
	.string	"v"
	.byte	0x1
	.byte	0x66
	.long	0x123
	.uleb128 0x5
	.string	"s"
	.byte	0x1
	.byte	0x66
	.long	0x123
	.long	.LLST27
	.uleb128 0x5
	.string	"t"
	.byte	0x1
	.byte	0x66
	.long	0x123
	.long	.LLST28
	.uleb128 0x5
	.string	"tmp"
	.byte	0x1
	.byte	0x66
	.long	0x123
	.long	.LLST29
	.uleb128 0xd
	.long	0x1c0
	.quad	.LBB10
	.long	.Ldebug_ranges0+0
	.byte	0x1
	.byte	0x6d
	.long	0x33b
	.uleb128 0xe
	.long	0x1da
	.long	.LLST30
	.uleb128 0xe
	.long	0x1d1
	.long	.LLST31
	.uleb128 0xf
	.long	.Ldebug_ranges0+0x70
	.uleb128 0x10
	.long	0x1e3
	.long	.LLST32
	.uleb128 0x10
	.long	0x1ed
	.long	.LLST33
	.uleb128 0x10
	.long	0x1f7
	.long	.LLST34
	.uleb128 0x10
	.long	0x201
	.long	.LLST35
	.byte	0
	.byte	0
	.uleb128 0xd
	.long	0x195
	.quad	.LBB20
	.long	.Ldebug_ranges0+0xe0
	.byte	0x1
	.byte	0x93
	.long	0x374
	.uleb128 0xe
	.long	0x1ac
	.long	.LLST36
	.uleb128 0xe
	.long	0x1a2
	.long	.LLST37
	.uleb128 0xf
	.long	.Ldebug_ranges0+0x110
	.uleb128 0x10
	.long	0x1b6
	.long	.LLST38
	.byte	0
	.byte	0
	.uleb128 0xd
	.long	0x130
	.quad	.LBB23
	.long	.Ldebug_ranges0+0x140
	.byte	0x1
	.byte	0x69
	.long	0x414
	.uleb128 0x11
	.long	0x146
	.uleb128 0xe
	.long	0x13d
	.long	.LLST39
	.uleb128 0xf
	.long	.Ldebug_ranges0+0x180
	.uleb128 0x10
	.long	0x14f
	.long	.LLST40
	.uleb128 0x10
	.long	0x159
	.long	.LLST41
	.uleb128 0x10
	.long	0x163
	.long	.LLST42
	.uleb128 0x10
	.long	0x16d
	.long	.LLST43
	.uleb128 0x10
	.long	0x177
	.long	.LLST44
	.uleb128 0x10
	.long	0x180
	.long	.LLST45
	.uleb128 0x10
	.long	0x189
	.long	.LLST46
	.uleb128 0x12
	.long	0x195
	.quad	.LBB25
	.long	.Ldebug_ranges0+0x1c0
	.byte	0x1
	.byte	0x3f
	.uleb128 0xe
	.long	0x1ac
	.long	.LLST47
	.uleb128 0xe
	.long	0x1a2
	.long	.LLST48
	.uleb128 0xf
	.long	.Ldebug_ranges0+0x1f0
	.uleb128 0x10
	.long	0x1b6
	.long	.LLST49
	.byte	0
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x13
	.quad	.LVL114
	.long	0x20b
	.uleb128 0x14
	.byte	0x1
	.byte	0x55
	.byte	0x2
	.byte	0x7c
	.sleb128 0
	.uleb128 0x14
	.byte	0x1
	.byte	0x54
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0
	.byte	0
	.uleb128 0x8
	.long	.LASF2
	.byte	0x1
	.byte	0xa1
	.byte	0x1
	.byte	0x1
	.long	0x48f
	.uleb128 0x9
	.string	"x"
	.byte	0x1
	.byte	0xa1
	.long	0x12a
	.uleb128 0x9
	.string	"p"
	.byte	0x1
	.byte	0xa1
	.long	0x12a
	.uleb128 0x9
	.string	"n"
	.byte	0x1
	.byte	0xa1
	.long	0x123
	.uleb128 0x9
	.string	"k"
	.byte	0x1
	.byte	0xa1
	.long	0x123
	.uleb128 0xa
	.string	"pi"
	.byte	0x1
	.byte	0xa3
	.long	0x12a
	.uleb128 0xa
	.string	"i"
	.byte	0x1
	.byte	0xa3
	.long	0x123
	.uleb128 0xa
	.string	"c"
	.byte	0x1
	.byte	0xa3
	.long	0x123
	.uleb128 0xa
	.string	"d"
	.byte	0x1
	.byte	0xa3
	.long	0x123
	.uleb128 0xa
	.string	"g"
	.byte	0x1
	.byte	0xa3
	.long	0x123
	.byte	0
	.uleb128 0x15
	.byte	0x1
	.long	.LASF9
	.byte	0x1
	.value	0x102
	.byte	0x1
	.quad	.LFB6
	.quad	.LFE6
	.long	.LLST50
	.byte	0x1
	.long	0x634
	.uleb128 0x16
	.string	"x"
	.byte	0x1
	.value	0x102
	.long	0x12a
	.long	.LLST51
	.uleb128 0x16
	.string	"p"
	.byte	0x1
	.value	0x102
	.long	0x12a
	.long	.LLST52
	.uleb128 0x16
	.string	"n"
	.byte	0x1
	.value	0x102
	.long	0x123
	.long	.LLST53
	.uleb128 0x16
	.string	"k"
	.byte	0x1
	.value	0x102
	.long	0x123
	.long	.LLST54
	.uleb128 0x16
	.string	"l"
	.byte	0x1
	.value	0x102
	.long	0x123
	.long	.LLST55
	.uleb128 0x17
	.string	"pi"
	.byte	0x1
	.value	0x104
	.long	0x12a
	.long	.LLST56
	.uleb128 0x17
	.string	"pk"
	.byte	0x1
	.value	0x104
	.long	0x12a
	.long	.LLST57
	.uleb128 0x17
	.string	"i"
	.byte	0x1
	.value	0x105
	.long	0x123
	.long	.LLST58
	.uleb128 0x17
	.string	"j"
	.byte	0x1
	.value	0x105
	.long	0x123
	.long	.LLST59
	.uleb128 0x17
	.string	"s"
	.byte	0x1
	.value	0x105
	.long	0x123
	.long	.LLST60
	.uleb128 0x17
	.string	"sl"
	.byte	0x1
	.value	0x105
	.long	0x123
	.long	.LLST61
	.uleb128 0x18
	.long	0x42f
	.quad	.LBB38
	.long	.Ldebug_ranges0+0x220
	.byte	0x1
	.value	0x10c
	.long	0x5bf
	.uleb128 0xe
	.long	0x457
	.long	.LLST59
	.uleb128 0xe
	.long	0x44e
	.long	.LLST63
	.uleb128 0xe
	.long	0x445
	.long	.LLST64
	.uleb128 0xe
	.long	0x43c
	.long	.LLST65
	.uleb128 0xf
	.long	.Ldebug_ranges0+0x2b0
	.uleb128 0x10
	.long	0x460
	.long	.LLST66
	.uleb128 0x10
	.long	0x46a
	.long	.LLST67
	.uleb128 0x10
	.long	0x473
	.long	.LLST68
	.uleb128 0x10
	.long	0x47c
	.long	.LLST69
	.uleb128 0x10
	.long	0x485
	.long	.LLST70
	.byte	0
	.byte	0
	.uleb128 0x19
	.quad	.LVL184
	.long	0x2d
	.long	0x5f7
	.uleb128 0x14
	.byte	0x1
	.byte	0x55
	.byte	0x2
	.byte	0x76
	.sleb128 0
	.uleb128 0x14
	.byte	0x1
	.byte	0x54
	.byte	0x2
	.byte	0x73
	.sleb128 0
	.uleb128 0x14
	.byte	0x1
	.byte	0x51
	.byte	0x2
	.byte	0x7c
	.sleb128 0
	.uleb128 0x14
	.byte	0x1
	.byte	0x52
	.byte	0x3
	.byte	0xf3
	.uleb128 0x1
	.byte	0x52
	.uleb128 0x14
	.byte	0x1
	.byte	0x58
	.byte	0x3
	.byte	0xf3
	.uleb128 0x1
	.byte	0x58
	.uleb128 0x14
	.byte	0x1
	.byte	0x59
	.byte	0x2
	.byte	0x7c
	.sleb128 0
	.byte	0
	.uleb128 0x1a
	.quad	.LVL212
	.long	0x20b
	.uleb128 0x19
	.quad	.LVL227
	.long	0x2d
	.long	0x61f
	.uleb128 0x14
	.byte	0x1
	.byte	0x59
	.byte	0x5
	.byte	0xc
	.long	0x7fffffff
	.byte	0
	.uleb128 0x13
	.quad	.LVL233
	.long	0x20b
	.uleb128 0x14
	.byte	0x1
	.byte	0x55
	.byte	0x2
	.byte	0x73
	.sleb128 0
	.byte	0
	.byte	0
	.uleb128 0x1b
	.string	"I"
	.byte	0x1
	.byte	0x10
	.long	0x12a
	.byte	0x9
	.byte	0x3
	.quad	I
	.uleb128 0x1b
	.string	"V"
	.byte	0x1
	.byte	0x11
	.long	0x12a
	.byte	0x9
	.byte	0x3
	.quad	V
	.uleb128 0x1b
	.string	"r"
	.byte	0x1
	.byte	0x12
	.long	0x123
	.byte	0x9
	.byte	0x3
	.quad	r
	.uleb128 0x1b
	.string	"h"
	.byte	0x1
	.byte	0x13
	.long	0x123
	.byte	0x9
	.byte	0x3
	.quad	h
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x2117
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x2117
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x52
	.uleb128 0x1
	.uleb128 0x55
	.uleb128 0x6
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x55
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x34
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x52
	.uleb128 0x1
	.uleb128 0x55
	.uleb128 0x6
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0x4109
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x410a
	.byte	0
	.uleb128 0x2
	.uleb128 0xa
	.uleb128 0x2111
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x2117
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x17
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x18
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x52
	.uleb128 0x1
	.uleb128 0x55
	.uleb128 0x6
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x19
	.uleb128 0x4109
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1a
	.uleb128 0x4109
	.byte	0
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x31
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1b
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
.LLST0:
	.quad	.LFB5-.Ltext0
	.quad	.LCFI0-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI0-.Ltext0
	.quad	.LCFI1-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI1-.Ltext0
	.quad	.LCFI2-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 24
	.quad	.LCFI2-.Ltext0
	.quad	.LCFI3-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 32
	.quad	.LCFI3-.Ltext0
	.quad	.LCFI4-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 40
	.quad	.LCFI4-.Ltext0
	.quad	.LCFI5-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 48
	.quad	.LCFI5-.Ltext0
	.quad	.LCFI6-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 56
	.quad	.LCFI6-.Ltext0
	.quad	.LCFI7-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 48
	.quad	.LCFI7-.Ltext0
	.quad	.LCFI8-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 40
	.quad	.LCFI8-.Ltext0
	.quad	.LCFI9-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 32
	.quad	.LCFI9-.Ltext0
	.quad	.LCFI10-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 24
	.quad	.LCFI10-.Ltext0
	.quad	.LCFI11-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI11-.Ltext0
	.quad	.LCFI12-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI12-.Ltext0
	.quad	.LFE5-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 56
	.quad	0
	.quad	0
.LLST1:
	.quad	.LVL0-.Ltext0
	.quad	.LVL24-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL24-.Ltext0
	.quad	.LVL30-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	.LVL30-.Ltext0
	.quad	.LVL62-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL62-.Ltext0
	.quad	.LVL66-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	.LVL66-.Ltext0
	.quad	.LFE5-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	0
	.quad	0
.LLST2:
	.quad	.LVL0-.Ltext0
	.quad	.LVL17-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL17-.Ltext0
	.quad	.LVL30-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x54
	.byte	0x9f
	.quad	.LVL30-.Ltext0
	.quad	.LVL32-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL32-.Ltext0
	.quad	.LVL33-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL33-.Ltext0
	.quad	.LFE5-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	0
	.quad	0
.LLST3:
	.quad	.LVL0-.Ltext0
	.quad	.LVL16-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL16-.Ltext0
	.quad	.LVL30-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x51
	.byte	0x9f
	.quad	.LVL30-.Ltext0
	.quad	.LVL31-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL31-.Ltext0
	.quad	.LVL66-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x51
	.byte	0x9f
	.quad	.LVL66-.Ltext0
	.quad	.LFE5-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	0
	.quad	0
.LLST4:
	.quad	.LVL0-.Ltext0
	.quad	.LVL5-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	.LVL5-.Ltext0
	.quad	.LVL73-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x52
	.byte	0x9f
	.quad	.LVL73-.Ltext0
	.quad	.LFE5-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	0
	.quad	0
.LLST5:
	.quad	.LVL0-.Ltext0
	.quad	.LVL13-.Ltext0
	.value	0x1
	.byte	0x59
	.quad	.LVL13-.Ltext0
	.quad	.LVL66-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x59
	.byte	0x9f
	.quad	.LVL66-.Ltext0
	.quad	.LVL67-.Ltext0
	.value	0x1
	.byte	0x59
	.quad	.LVL67-.Ltext0
	.quad	.LVL70-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x59
	.byte	0x9f
	.quad	.LVL70-.Ltext0
	.quad	.LFE5-.Ltext0
	.value	0x1
	.byte	0x59
	.quad	0
	.quad	0
.LLST6:
	.quad	.LVL6-.Ltext0
	.quad	.LVL8-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL8-.Ltext0
	.quad	.LVL10-.Ltext0
	.value	0x1
	.byte	0x5b
	.quad	.LVL11-.Ltext0
	.quad	.LVL12-.Ltext0
	.value	0x1
	.byte	0x5b
	.quad	.LVL66-.Ltext0
	.quad	.LVL73-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL74-.Ltext0
	.quad	.LFE5-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	0
	.quad	0
.LLST7:
	.quad	.LVL7-.Ltext0
	.quad	.LVL8-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	.LVL9-.Ltext0
	.quad	.LVL10-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL15-.Ltext0
	.quad	.LVL20-.Ltext0
	.value	0x1
	.byte	0x5b
	.quad	.LVL20-.Ltext0
	.quad	.LVL21-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL21-.Ltext0
	.quad	.LVL23-.Ltext0
	.value	0x2
	.byte	0x74
	.sleb128 -4
	.quad	.LVL23-.Ltext0
	.quad	.LVL25-.Ltext0
	.value	0x1
	.byte	0x5b
	.quad	.LVL25-.Ltext0
	.quad	.LVL26-.Ltext0
	.value	0x2
	.byte	0x75
	.sleb128 0
	.quad	.LVL26-.Ltext0
	.quad	.LVL27-.Ltext0
	.value	0x2
	.byte	0x75
	.sleb128 -4
	.quad	.LVL27-.Ltext0
	.quad	.LVL30-.Ltext0
	.value	0x1
	.byte	0x5b
	.quad	.LVL38-.Ltext0
	.quad	.LVL39-.Ltext0
	.value	0x1
	.byte	0x5b
	.quad	.LVL39-.Ltext0
	.quad	.LVL40-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL40-.Ltext0
	.quad	.LVL41-.Ltext0
	.value	0x1
	.byte	0x5d
	.quad	.LVL43-.Ltext0
	.quad	.LVL47-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL47-.Ltext0
	.quad	.LVL48-.Ltext0
	.value	0x1
	.byte	0x5d
	.quad	.LVL48-.Ltext0
	.quad	.LVL50-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL54-.Ltext0
	.quad	.LVL57-.Ltext0
	.value	0x1
	.byte	0x5b
	.quad	.LVL57-.Ltext0
	.quad	.LVL58-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL60-.Ltext0
	.quad	.LVL63-.Ltext0
	.value	0x1
	.byte	0x5b
	.quad	.LVL63-.Ltext0
	.quad	.LVL64-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL65-.Ltext0
	.quad	.LVL66-.Ltext0
	.value	0x1
	.byte	0x5b
	.quad	0
	.quad	0
.LLST8:
	.quad	.LVL6-.Ltext0
	.quad	.LVL8-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL8-.Ltext0
	.quad	.LVL10-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL11-.Ltext0
	.quad	.LVL12-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL66-.Ltext0
	.quad	.LVL73-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL74-.Ltext0
	.quad	.LFE5-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	0
	.quad	0
.LLST9:
	.quad	.LVL6-.Ltext0
	.quad	.LVL7-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL70-.Ltext0
	.quad	.LVL71-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL71-.Ltext0
	.quad	.LVL73-.Ltext0
	.value	0xc
	.byte	0xc
	.long	0x7fffffff
	.byte	0x7a
	.sleb128 0
	.byte	0x8
	.byte	0xff
	.byte	0x1a
	.byte	0x26
	.byte	0x9f
	.quad	.LVL73-.Ltext0
	.quad	.LFE5-.Ltext0
	.value	0x6
	.byte	0xc
	.long	0x7fffffff
	.byte	0x9f
	.quad	0
	.quad	0
.LLST10:
	.quad	.LVL0-.Ltext0
	.quad	.LVL1-.Ltext0
	.value	0x6
	.byte	0x72
	.sleb128 0
	.byte	0x78
	.sleb128 0
	.byte	0x1c
	.byte	0x9f
	.quad	.LVL1-.Ltext0
	.quad	.LVL2-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	.LVL2-.Ltext0
	.quad	.LVL3-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL3-.Ltext0
	.quad	.LVL4-.Ltext0
	.value	0x5
	.byte	0x70
	.sleb128 0
	.byte	0x31
	.byte	0x26
	.byte	0x9f
	.quad	.LVL4-.Ltext0
	.quad	.LVL7-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL44-.Ltext0
	.quad	.LVL46-.Ltext0
	.value	0x2
	.byte	0x31
	.byte	0x9f
	.quad	.LVL48-.Ltext0
	.quad	.LVL49-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL70-.Ltext0
	.quad	.LVL72-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL73-.Ltext0
	.quad	.LFE5-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	0
	.quad	0
.LLST11:
	.quad	.LVL28-.Ltext0
	.quad	.LVL30-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL49-.Ltext0
	.quad	.LVL51-.Ltext0
	.value	0x2
	.byte	0x31
	.byte	0x9f
	.quad	.LVL52-.Ltext0
	.quad	.LVL54-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	0
	.quad	0
.LLST12:
	.quad	.LVL12-.Ltext0
	.quad	.LVL29-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL30-.Ltext0
	.quad	.LVL66-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL68-.Ltext0
	.quad	.LVL70-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LLST13:
	.quad	.LVL0-.Ltext0
	.quad	.LVL2-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL3-.Ltext0
	.quad	.LVL7-.Ltext0
	.value	0x1
	.byte	0x5a
	.quad	.LVL70-.Ltext0
	.quad	.LVL73-.Ltext0
	.value	0x1
	.byte	0x5a
	.quad	.LVL73-.Ltext0
	.quad	.LFE5-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	0
	.quad	0
.LLST14:
	.quad	.LVL14-.Ltext0
	.quad	.LVL18-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL18-.Ltext0
	.quad	.LVL19-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL19-.Ltext0
	.quad	.LVL23-.Ltext0
	.value	0x3
	.byte	0x74
	.sleb128 -4
	.byte	0x9f
	.quad	.LVL23-.Ltext0
	.quad	.LVL24-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL24-.Ltext0
	.quad	.LVL30-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL30-.Ltext0
	.quad	.LVL32-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL32-.Ltext0
	.quad	.LVL35-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL36-.Ltext0
	.quad	.LVL37-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL38-.Ltext0
	.quad	.LVL39-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL39-.Ltext0
	.quad	.LVL42-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL42-.Ltext0
	.quad	.LVL43-.Ltext0
	.value	0x3
	.byte	0x76
	.sleb128 -4
	.byte	0x9f
	.quad	.LVL43-.Ltext0
	.quad	.LVL45-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL49-.Ltext0
	.quad	.LVL51-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL51-.Ltext0
	.quad	.LVL54-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL54-.Ltext0
	.quad	.LVL55-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL55-.Ltext0
	.quad	.LVL56-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL56-.Ltext0
	.quad	.LVL60-.Ltext0
	.value	0x3
	.byte	0x7c
	.sleb128 -4
	.byte	0x9f
	.quad	.LVL60-.Ltext0
	.quad	.LVL62-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL62-.Ltext0
	.quad	.LVL66-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL69-.Ltext0
	.quad	.LVL70-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	0
	.quad	0
.LLST15:
	.quad	.LVL15-.Ltext0
	.quad	.LVL18-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL18-.Ltext0
	.quad	.LVL22-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL22-.Ltext0
	.quad	.LVL23-.Ltext0
	.value	0x3
	.byte	0x71
	.sleb128 -4
	.byte	0x9f
	.quad	.LVL23-.Ltext0
	.quad	.LVL24-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL54-.Ltext0
	.quad	.LVL55-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL55-.Ltext0
	.quad	.LVL59-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL59-.Ltext0
	.quad	.LVL60-.Ltext0
	.value	0x3
	.byte	0x71
	.sleb128 -4
	.byte	0x9f
	.quad	.LVL60-.Ltext0
	.quad	.LVL61-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	0
	.quad	0
.LLST16:
	.quad	.LFB3-.Ltext0
	.quad	.LCFI13-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI13-.Ltext0
	.quad	.LCFI14-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI14-.Ltext0
	.quad	.LCFI15-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 24
	.quad	.LCFI15-.Ltext0
	.quad	.LCFI16-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 32
	.quad	.LCFI16-.Ltext0
	.quad	.LCFI17-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 40
	.quad	.LCFI17-.Ltext0
	.quad	.LCFI18-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 48
	.quad	.LCFI18-.Ltext0
	.quad	.LCFI19-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 56
	.quad	.LCFI19-.Ltext0
	.quad	.LCFI20-.Ltext0
	.value	0x3
	.byte	0x77
	.sleb128 64
	.quad	.LCFI20-.Ltext0
	.quad	.LCFI21-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 56
	.quad	.LCFI21-.Ltext0
	.quad	.LCFI22-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 48
	.quad	.LCFI22-.Ltext0
	.quad	.LCFI23-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 40
	.quad	.LCFI23-.Ltext0
	.quad	.LCFI24-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 32
	.quad	.LCFI24-.Ltext0
	.quad	.LCFI25-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 24
	.quad	.LCFI25-.Ltext0
	.quad	.LCFI26-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI26-.Ltext0
	.quad	.LCFI27-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI27-.Ltext0
	.quad	.LCFI28-.Ltext0
	.value	0x3
	.byte	0x77
	.sleb128 64
	.quad	.LCFI28-.Ltext0
	.quad	.LCFI29-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 56
	.quad	.LCFI29-.Ltext0
	.quad	.LCFI30-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 48
	.quad	.LCFI30-.Ltext0
	.quad	.LCFI31-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 40
	.quad	.LCFI31-.Ltext0
	.quad	.LCFI32-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 32
	.quad	.LCFI32-.Ltext0
	.quad	.LCFI33-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 24
	.quad	.LCFI33-.Ltext0
	.quad	.LCFI34-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI34-.Ltext0
	.quad	.LCFI35-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI35-.Ltext0
	.quad	.LFE3-.Ltext0
	.value	0x3
	.byte	0x77
	.sleb128 64
	.quad	0
	.quad	0
.LLST17:
	.quad	.LVL75-.Ltext0
	.quad	.LVL76-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL76-.Ltext0
	.quad	.LVL180-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	.LVL180-.Ltext0
	.quad	.LVL181-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL181-.Ltext0
	.quad	.LVL182-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL182-.Ltext0
	.quad	.LFE3-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	0
	.quad	0
.LLST18:
	.quad	.LVL75-.Ltext0
	.quad	.LVL76-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL76-.Ltext0
	.quad	.LVL180-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x54
	.byte	0x9f
	.quad	.LVL180-.Ltext0
	.quad	.LVL182-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL182-.Ltext0
	.quad	.LFE3-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x54
	.byte	0x9f
	.quad	0
	.quad	0
.LLST19:
	.quad	.LVL82-.Ltext0
	.quad	.LVL83-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL83-.Ltext0
	.quad	.LVL92-.Ltext0
	.value	0x1
	.byte	0x5b
	.quad	.LVL145-.Ltext0
	.quad	.LVL161-.Ltext0
	.value	0x1
	.byte	0x5b
	.quad	0
	.quad	0
.LLST20:
	.quad	.LVL82-.Ltext0
	.quad	.LVL83-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL83-.Ltext0
	.quad	.LVL85-.Ltext0
	.value	0x1
	.byte	0x58
	.quad	.LVL86-.Ltext0
	.quad	.LVL108-.Ltext0
	.value	0x1
	.byte	0x58
	.quad	.LVL145-.Ltext0
	.quad	.LVL156-.Ltext0
	.value	0x1
	.byte	0x58
	.quad	.LVL156-.Ltext0
	.quad	.LVL158-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL158-.Ltext0
	.quad	.LVL160-.Ltext0
	.value	0x1
	.byte	0x58
	.quad	.LVL160-.Ltext0
	.quad	.LVL161-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL182-.Ltext0
	.quad	.LFE3-.Ltext0
	.value	0x1
	.byte	0x58
	.quad	0
	.quad	0
.LLST21:
	.quad	.LVL82-.Ltext0
	.quad	.LVL83-.Ltext0
	.value	0x1
	.byte	0x59
	.quad	.LVL83-.Ltext0
	.quad	.LVL100-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL145-.Ltext0
	.quad	.LVL161-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL182-.Ltext0
	.quad	.LFE3-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	0
	.quad	0
.LLST22:
	.quad	.LVL82-.Ltext0
	.quad	.LVL106-.Ltext0
	.value	0x1
	.byte	0x59
	.quad	.LVL145-.Ltext0
	.quad	.LVL161-.Ltext0
	.value	0x1
	.byte	0x59
	.quad	.LVL182-.Ltext0
	.quad	.LFE3-.Ltext0
	.value	0x1
	.byte	0x59
	.quad	0
	.quad	0
.LLST23:
	.quad	.LVL90-.Ltext0
	.quad	.LVL93-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL96-.Ltext0
	.quad	.LVL99-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL103-.Ltext0
	.quad	.LVL107-.Ltext0
	.value	0x1
	.byte	0x58
	.quad	.LVL110-.Ltext0
	.quad	.LVL113-.Ltext0
	.value	0x1
	.byte	0x58
	.quad	.LVL182-.Ltext0
	.quad	.LFE3-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	0
	.quad	0
.LLST24:
	.quad	.LVL91-.Ltext0
	.quad	.LVL93-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	.LVL97-.Ltext0
	.quad	.LVL99-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	.LVL104-.Ltext0
	.quad	.LVL107-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL111-.Ltext0
	.quad	.LVL113-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL182-.Ltext0
	.quad	.LFE3-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	0
	.quad	0
.LLST25:
	.quad	.LVL87-.Ltext0
	.quad	.LVL116-.Ltext0
	.value	0x8
	.byte	0x7d
	.sleb128 0
	.byte	0x32
	.byte	0x24
	.byte	0x7c
	.sleb128 0
	.byte	0x22
	.byte	0x9f
	.quad	.LVL182-.Ltext0
	.quad	.LFE3-.Ltext0
	.value	0x8
	.byte	0x7d
	.sleb128 0
	.byte	0x32
	.byte	0x24
	.byte	0x7c
	.sleb128 0
	.byte	0x22
	.byte	0x9f
	.quad	0
	.quad	0
.LLST26:
	.quad	.LVL84-.Ltext0
	.quad	.LVL85-.Ltext0
	.value	0x7
	.byte	0x71
	.sleb128 0
	.byte	0x32
	.byte	0x24
	.byte	0x76
	.sleb128 0
	.byte	0x22
	.quad	.LVL145-.Ltext0
	.quad	.LVL146-.Ltext0
	.value	0x7
	.byte	0x71
	.sleb128 0
	.byte	0x32
	.byte	0x24
	.byte	0x76
	.sleb128 0
	.byte	0x22
	.quad	.LVL146-.Ltext0
	.quad	.LVL147-.Ltext0
	.value	0x10
	.byte	0x7e
	.sleb128 0
	.byte	0x7a
	.sleb128 0
	.byte	0x22
	.byte	0x8
	.byte	0x20
	.byte	0x24
	.byte	0x8
	.byte	0x20
	.byte	0x26
	.byte	0x32
	.byte	0x24
	.byte	0x76
	.sleb128 0
	.byte	0x22
	.quad	.LVL147-.Ltext0
	.quad	.LVL148-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL150-.Ltext0
	.quad	.LVL152-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL154-.Ltext0
	.quad	.LVL158-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL158-.Ltext0
	.quad	.LVL159-.Ltext0
	.value	0x7
	.byte	0x71
	.sleb128 0
	.byte	0x32
	.byte	0x24
	.byte	0x76
	.sleb128 0
	.byte	0x22
	.quad	.LVL159-.Ltext0
	.quad	.LVL161-.Ltext0
	.value	0x10
	.byte	0x7e
	.sleb128 0
	.byte	0x7a
	.sleb128 0
	.byte	0x22
	.byte	0x8
	.byte	0x20
	.byte	0x24
	.byte	0x8
	.byte	0x20
	.byte	0x26
	.byte	0x32
	.byte	0x24
	.byte	0x76
	.sleb128 0
	.byte	0x22
	.quad	0
	.quad	0
.LLST27:
	.quad	.LVL88-.Ltext0
	.quad	.LVL93-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL95-.Ltext0
	.quad	.LVL98-.Ltext0
	.value	0x3
	.byte	0x71
	.sleb128 -1
	.byte	0x9f
	.quad	.LVL98-.Ltext0
	.quad	.LVL99-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL101-.Ltext0
	.quad	.LVL103-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	.LVL103-.Ltext0
	.quad	.LVL107-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	.LVL109-.Ltext0
	.quad	.LVL112-.Ltext0
	.value	0x3
	.byte	0x72
	.sleb128 -1
	.byte	0x9f
	.quad	.LVL112-.Ltext0
	.quad	.LVL113-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	.LVL113-.Ltext0
	.quad	.LVL122-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL172-.Ltext0
	.quad	.LVL173-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL182-.Ltext0
	.quad	.LFE3-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	0
	.quad	0
.LLST28:
	.quad	.LVL89-.Ltext0
	.quad	.LVL101-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL101-.Ltext0
	.quad	.LVL102-.Ltext0
	.value	0xa
	.byte	0x70
	.sleb128 0
	.byte	0x79
	.sleb128 0
	.byte	0x1c
	.byte	0x34
	.byte	0x1b
	.byte	0x31
	.byte	0x1c
	.byte	0x9f
	.quad	.LVL102-.Ltext0
	.quad	.LVL104-.Ltext0
	.value	0xa
	.byte	0x71
	.sleb128 0
	.byte	0x79
	.sleb128 0
	.byte	0x1c
	.byte	0x34
	.byte	0x1b
	.byte	0x31
	.byte	0x1c
	.byte	0x9f
	.quad	.LVL104-.Ltext0
	.quad	.LVL105-.Ltext0
	.value	0xd
	.byte	0x7c
	.sleb128 0
	.byte	0x79
	.sleb128 0
	.byte	0x1c
	.byte	0x75
	.sleb128 0
	.byte	0x22
	.byte	0x34
	.byte	0x1b
	.byte	0x31
	.byte	0x1c
	.byte	0x9f
	.quad	.LVL105-.Ltext0
	.quad	.LVL106-.Ltext0
	.value	0xf
	.byte	0x7d
	.sleb128 0
	.byte	0x32
	.byte	0x24
	.byte	0x7c
	.sleb128 0
	.byte	0x22
	.byte	0x79
	.sleb128 0
	.byte	0x1c
	.byte	0x34
	.byte	0x1b
	.byte	0x31
	.byte	0x1c
	.byte	0x9f
	.quad	.LVL113-.Ltext0
	.quad	.LVL122-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	.LVL172-.Ltext0
	.quad	.LVL173-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	.LVL182-.Ltext0
	.quad	.LFE3-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	0
	.quad	0
.LLST29:
	.quad	.LVL95-.Ltext0
	.quad	.LVL99-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL109-.Ltext0
	.quad	.LVL113-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL151-.Ltext0
	.quad	.LVL154-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	.LVL155-.Ltext0
	.quad	.LVL158-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL159-.Ltext0
	.quad	.LVL161-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	0
	.quad	0
.LLST30:
	.quad	.LVL76-.Ltext0
	.quad	.LVL81-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	.LVL81-.Ltext0
	.quad	.LVL82-.Ltext0
	.value	0x1
	.byte	0x5d
	.quad	.LVL161-.Ltext0
	.quad	.LVL163-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	.LVL163-.Ltext0
	.quad	.LVL172-.Ltext0
	.value	0x1
	.byte	0x5d
	.quad	0
	.quad	0
.LLST31:
	.quad	.LVL76-.Ltext0
	.quad	.LVL121-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL145-.Ltext0
	.quad	.LVL173-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL182-.Ltext0
	.quad	.LFE3-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	0
	.quad	0
.LLST32:
	.quad	.LVL78-.Ltext0
	.quad	.LVL81-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL162-.Ltext0
	.quad	.LVL164-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL170-.Ltext0
	.quad	.LVL171-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	0
	.quad	0
.LLST33:
	.quad	.LVL77-.Ltext0
	.quad	.LVL80-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL80-.Ltext0
	.quad	.LVL81-.Ltext0
	.value	0x10
	.byte	0x7f
	.sleb128 0
	.byte	0x31
	.byte	0x26
	.byte	0x8
	.byte	0x20
	.byte	0x24
	.byte	0x8
	.byte	0x20
	.byte	0x26
	.byte	0x32
	.byte	0x24
	.byte	0x7c
	.sleb128 0
	.byte	0x22
	.byte	0x9f
	.quad	.LVL161-.Ltext0
	.quad	.LVL165-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL165-.Ltext0
	.quad	.LVL166-.Ltext0
	.value	0x10
	.byte	0x7d
	.sleb128 0
	.byte	0x31
	.byte	0x26
	.byte	0x8
	.byte	0x20
	.byte	0x24
	.byte	0x8
	.byte	0x20
	.byte	0x26
	.byte	0x32
	.byte	0x24
	.byte	0x7c
	.sleb128 0
	.byte	0x22
	.byte	0x9f
	.quad	.LVL169-.Ltext0
	.quad	.LVL170-.Ltext0
	.value	0x10
	.byte	0x7d
	.sleb128 0
	.byte	0x31
	.byte	0x26
	.byte	0x8
	.byte	0x20
	.byte	0x24
	.byte	0x8
	.byte	0x20
	.byte	0x26
	.byte	0x32
	.byte	0x24
	.byte	0x7c
	.sleb128 0
	.byte	0x22
	.byte	0x9f
	.quad	.LVL170-.Ltext0
	.quad	.LVL171-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	0
	.quad	0
.LLST34:
	.quad	.LVL79-.Ltext0
	.quad	.LVL81-.Ltext0
	.value	0x1
	.byte	0x59
	.quad	.LVL162-.Ltext0
	.quad	.LVL167-.Ltext0
	.value	0x1
	.byte	0x59
	.quad	.LVL169-.Ltext0
	.quad	.LVL172-.Ltext0
	.value	0x1
	.byte	0x59
	.quad	0
	.quad	0
.LLST35:
	.quad	.LVL162-.Ltext0
	.quad	.LVL163-.Ltext0
	.value	0x5
	.byte	0x7f
	.sleb128 0
	.byte	0x33
	.byte	0x26
	.byte	0x9f
	.quad	.LVL163-.Ltext0
	.quad	.LVL168-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	.LVL169-.Ltext0
	.quad	.LVL172-.Ltext0
	.value	0x1
	.byte	0x5f
	.quad	0
	.quad	0
.LLST36:
	.quad	.LVL117-.Ltext0
	.quad	.LVL122-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	.LVL172-.Ltext0
	.quad	.LVL173-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	0
	.quad	0
.LLST37:
	.quad	.LVL115-.Ltext0
	.quad	.LVL122-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL172-.Ltext0
	.quad	.LVL173-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	0
	.quad	0
.LLST38:
	.quad	.LVL118-.Ltext0
	.quad	.LVL119-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL119-.Ltext0
	.quad	.LVL122-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL172-.Ltext0
	.quad	.LVL173-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	0
	.quad	0
.LLST39:
	.quad	.LVL122-.Ltext0
	.quad	.LVL124-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL178-.Ltext0
	.quad	.LVL179-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL179-.Ltext0
	.quad	.LVL180-.Ltext0
	.value	0x1
	.byte	0x59
	.quad	0
	.quad	0
.LLST40:
	.quad	.LVL122-.Ltext0
	.quad	.LVL143-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL143-.Ltext0
	.quad	.LVL145-.Ltext0
	.value	0x1
	.byte	0x59
	.quad	.LVL174-.Ltext0
	.quad	.LVL175-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL175-.Ltext0
	.quad	.LVL178-.Ltext0
	.value	0x1
	.byte	0x59
	.quad	.LVL178-.Ltext0
	.quad	.LVL179-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL179-.Ltext0
	.quad	.LVL180-.Ltext0
	.value	0x1
	.byte	0x59
	.quad	0
	.quad	0
.LLST41:
	.quad	.LVL125-.Ltext0
	.quad	.LVL127-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL127-.Ltext0
	.quad	.LVL128-.Ltext0
	.value	0x1
	.byte	0x59
	.quad	.LVL129-.Ltext0
	.quad	.LVL145-.Ltext0
	.value	0x1
	.byte	0x59
	.quad	.LVL174-.Ltext0
	.quad	.LVL177-.Ltext0
	.value	0x1
	.byte	0x59
	.quad	0
	.quad	0
.LLST42:
	.quad	.LVL125-.Ltext0
	.quad	.LVL127-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL127-.Ltext0
	.quad	.LVL135-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL135-.Ltext0
	.quad	.LVL136-.Ltext0
	.value	0x3
	.byte	0x70
	.sleb128 -4
	.byte	0x9f
	.quad	.LVL136-.Ltext0
	.quad	.LVL138-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	0
	.quad	0
.LLST43:
	.quad	.LVL123-.Ltext0
	.quad	.LVL145-.Ltext0
	.value	0x1
	.byte	0x5a
	.quad	.LVL174-.Ltext0
	.quad	.LVL180-.Ltext0
	.value	0x1
	.byte	0x5a
	.quad	0
	.quad	0
.LLST44:
	.quad	.LVL126-.Ltext0
	.quad	.LVL127-.Ltext0
	.value	0x1
	.byte	0x58
	.quad	.LVL127-.Ltext0
	.quad	.LVL129-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL129-.Ltext0
	.quad	.LVL145-.Ltext0
	.value	0x1
	.byte	0x58
	.quad	.LVL174-.Ltext0
	.quad	.LVL177-.Ltext0
	.value	0x1
	.byte	0x58
	.quad	0
	.quad	0
.LLST45:
	.quad	.LVL127-.Ltext0
	.quad	.LVL131-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL132-.Ltext0
	.quad	.LVL134-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL134-.Ltext0
	.quad	.LVL138-.Ltext0
	.value	0x10
	.byte	0x72
	.sleb128 0
	.byte	0x7b
	.sleb128 0
	.byte	0x22
	.byte	0x8
	.byte	0x20
	.byte	0x24
	.byte	0x8
	.byte	0x20
	.byte	0x26
	.byte	0x32
	.byte	0x24
	.byte	0x75
	.sleb128 0
	.byte	0x22
	.quad	0
	.quad	0
.LLST46:
	.quad	.LVL127-.Ltext0
	.quad	.LVL129-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	.LVL133-.Ltext0
	.quad	.LVL137-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	0
	.quad	0
.LLST47:
	.quad	.LVL139-.Ltext0
	.quad	.LVL145-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	.LVL174-.Ltext0
	.quad	.LVL177-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	0
	.quad	0
.LLST48:
	.quad	.LVL139-.Ltext0
	.quad	.LVL141-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL141-.Ltext0
	.quad	.LVL144-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL174-.Ltext0
	.quad	.LVL176-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	0
	.quad	0
.LLST49:
	.quad	.LVL140-.Ltext0
	.quad	.LVL141-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL141-.Ltext0
	.quad	.LVL145-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL174-.Ltext0
	.quad	.LVL176-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL176-.Ltext0
	.quad	.LVL177-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	0
	.quad	0
.LLST50:
	.quad	.LFB6-.Ltext0
	.quad	.LCFI36-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI36-.Ltext0
	.quad	.LCFI37-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI37-.Ltext0
	.quad	.LCFI38-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 24
	.quad	.LCFI38-.Ltext0
	.quad	.LCFI39-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 32
	.quad	.LCFI39-.Ltext0
	.quad	.LCFI40-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 40
	.quad	.LCFI40-.Ltext0
	.quad	.LCFI41-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 48
	.quad	.LCFI41-.Ltext0
	.quad	.LCFI42-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 40
	.quad	.LCFI42-.Ltext0
	.quad	.LCFI43-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 32
	.quad	.LCFI43-.Ltext0
	.quad	.LCFI44-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 24
	.quad	.LCFI44-.Ltext0
	.quad	.LCFI45-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 16
	.quad	.LCFI45-.Ltext0
	.quad	.LCFI46-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 8
	.quad	.LCFI46-.Ltext0
	.quad	.LFE6-.Ltext0
	.value	0x2
	.byte	0x77
	.sleb128 48
	.quad	0
	.quad	0
.LLST51:
	.quad	.LVL183-.Ltext0
	.quad	.LVL184-1-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL184-1-.Ltext0
	.quad	.LVL207-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL207-.Ltext0
	.quad	.LVL226-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x55
	.byte	0x9f
	.quad	.LVL226-.Ltext0
	.quad	.LVL227-1-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL227-1-.Ltext0
	.quad	.LFE6-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	0
	.quad	0
.LLST52:
	.quad	.LVL183-.Ltext0
	.quad	.LVL184-1-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL184-1-.Ltext0
	.quad	.LVL206-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL206-.Ltext0
	.quad	.LVL226-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x54
	.byte	0x9f
	.quad	.LVL226-.Ltext0
	.quad	.LFE6-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LLST53:
	.quad	.LVL183-.Ltext0
	.quad	.LVL184-1-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL184-1-.Ltext0
	.quad	.LVL224-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL224-.Ltext0
	.quad	.LVL225-.Ltext0
	.value	0x4
	.byte	0x7d
	.sleb128 0
	.byte	0x1f
	.byte	0x9f
	.quad	.LVL225-.Ltext0
	.quad	.LVL226-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x51
	.byte	0x9f
	.quad	.LVL226-.Ltext0
	.quad	.LVL227-1-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL227-1-.Ltext0
	.quad	.LFE6-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	0
	.quad	0
.LLST54:
	.quad	.LVL183-.Ltext0
	.quad	.LVL184-1-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	.LVL184-1-.Ltext0
	.quad	.LVL226-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x52
	.byte	0x9f
	.quad	.LVL226-.Ltext0
	.quad	.LVL227-1-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	.LVL227-1-.Ltext0
	.quad	.LFE6-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x52
	.byte	0x9f
	.quad	0
	.quad	0
.LLST55:
	.quad	.LVL183-.Ltext0
	.quad	.LVL184-1-.Ltext0
	.value	0x1
	.byte	0x58
	.quad	.LVL184-1-.Ltext0
	.quad	.LVL226-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x58
	.byte	0x9f
	.quad	.LVL226-.Ltext0
	.quad	.LVL227-1-.Ltext0
	.value	0x1
	.byte	0x58
	.quad	.LVL227-1-.Ltext0
	.quad	.LFE6-.Ltext0
	.value	0x4
	.byte	0xf3
	.uleb128 0x1
	.byte	0x58
	.byte	0x9f
	.quad	0
	.quad	0
.LLST56:
	.quad	.LVL208-.Ltext0
	.quad	.LVL212-1-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL212-.Ltext0
	.quad	.LVL214-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	.LVL214-.Ltext0
	.quad	.LVL219-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	0
	.quad	0
.LLST57:
	.quad	.LVL211-.Ltext0
	.quad	.LVL214-.Ltext0
	.value	0x1
	.byte	0x5e
	.quad	0
	.quad	0
.LLST58:
	.quad	.LVL219-.Ltext0
	.quad	.LVL220-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL221-.Ltext0
	.quad	.LVL222-.Ltext0
	.value	0x3
	.byte	0x70
	.sleb128 1
	.byte	0x9f
	.quad	.LVL222-.Ltext0
	.quad	.LVL223-.Ltext0
	.value	0x3
	.byte	0x70
	.sleb128 0
	.byte	0x9f
	.quad	.LVL227-.Ltext0
	.quad	.LVL228-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL229-.Ltext0
	.quad	.LVL230-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL231-.Ltext0
	.quad	.LVL232-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	0
	.quad	0
.LLST59:
	.quad	.LVL184-.Ltext0
	.quad	.LVL185-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL185-.Ltext0
	.quad	.LVL196-.Ltext0
	.value	0x1
	.byte	0x58
	.quad	0
	.quad	0
.LLST60:
	.quad	.LVL208-.Ltext0
	.quad	.LVL210-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL215-.Ltext0
	.quad	.LVL218-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	0
	.quad	0
.LLST61:
	.quad	.LVL208-.Ltext0
	.quad	.LVL209-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL209-.Ltext0
	.quad	.LVL213-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL213-.Ltext0
	.quad	.LVL216-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	.LVL217-.Ltext0
	.quad	.LVL219-.Ltext0
	.value	0x1
	.byte	0x51
	.quad	0
	.quad	0
.LLST63:
	.quad	.LVL184-.Ltext0
	.quad	.LVL206-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL234-.Ltext0
	.quad	.LFE6-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	0
	.quad	0
.LLST64:
	.quad	.LVL184-.Ltext0
	.quad	.LVL206-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL234-.Ltext0
	.quad	.LFE6-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	0
	.quad	0
.LLST65:
	.quad	.LVL184-.Ltext0
	.quad	.LVL206-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	.LVL234-.Ltext0
	.quad	.LFE6-.Ltext0
	.value	0x1
	.byte	0x56
	.quad	0
	.quad	0
.LLST66:
	.quad	.LVL184-.Ltext0
	.quad	.LVL186-.Ltext0
	.value	0x1
	.byte	0x53
	.quad	.LVL186-.Ltext0
	.quad	.LVL188-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL189-.Ltext0
	.quad	.LVL190-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL195-.Ltext0
	.quad	.LVL206-.Ltext0
	.value	0x1
	.byte	0x59
	.quad	.LVL234-.Ltext0
	.quad	.LFE6-.Ltext0
	.value	0x1
	.byte	0x59
	.quad	0
	.quad	0
.LLST67:
	.quad	.LVL190-.Ltext0
	.quad	.LVL191-.Ltext0
	.value	0x2
	.byte	0x30
	.byte	0x9f
	.quad	.LVL192-.Ltext0
	.quad	.LVL193-.Ltext0
	.value	0x3
	.byte	0x70
	.sleb128 1
	.byte	0x9f
	.quad	.LVL193-.Ltext0
	.quad	.LVL194-.Ltext0
	.value	0x3
	.byte	0x70
	.sleb128 0
	.byte	0x9f
	.quad	.LVL195-.Ltext0
	.quad	.LVL196-.Ltext0
	.value	0x1
	.byte	0x5c
	.quad	.LVL196-.Ltext0
	.quad	.LVL199-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL199-.Ltext0
	.quad	.LVL201-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL201-.Ltext0
	.quad	.LVL202-.Ltext0
	.value	0x3
	.byte	0x74
	.sleb128 1
	.byte	0x9f
	.quad	.LVL202-.Ltext0
	.quad	.LVL204-.Ltext0
	.value	0x1
	.byte	0x54
	.quad	.LVL204-.Ltext0
	.quad	.LVL206-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL234-.Ltext0
	.quad	.LFE6-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	0
	.quad	0
.LLST68:
	.quad	.LVL191-.Ltext0
	.quad	.LVL193-.Ltext0
	.value	0x7
	.byte	0x70
	.sleb128 0
	.byte	0x32
	.byte	0x24
	.byte	0x76
	.sleb128 0
	.byte	0x22
	.quad	.LVL193-.Ltext0
	.quad	.LVL194-.Ltext0
	.value	0x7
	.byte	0x70
	.sleb128 -1
	.byte	0x32
	.byte	0x24
	.byte	0x76
	.sleb128 0
	.byte	0x22
	.quad	.LVL197-.Ltext0
	.quad	.LVL200-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	.LVL200-.Ltext0
	.quad	.LVL203-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL203-.Ltext0
	.quad	.LVL204-.Ltext0
	.value	0x2
	.byte	0x71
	.sleb128 4
	.quad	.LVL234-.Ltext0
	.quad	.LFE6-.Ltext0
	.value	0x1
	.byte	0x52
	.quad	0
	.quad	0
.LLST69:
	.quad	.LVL198-.Ltext0
	.quad	.LVL202-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL202-.Ltext0
	.quad	.LVL204-.Ltext0
	.value	0x1
	.byte	0x58
	.quad	.LVL234-.Ltext0
	.quad	.LVL235-.Ltext0
	.value	0x1
	.byte	0x50
	.quad	.LVL235-.Ltext0
	.quad	.LFE6-.Ltext0
	.value	0x2
	.byte	0x71
	.sleb128 0
	.quad	0
	.quad	0
.LLST70:
	.quad	.LVL198-.Ltext0
	.quad	.LVL204-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL204-.Ltext0
	.quad	.LVL205-.Ltext0
	.value	0xf
	.byte	0x79
	.sleb128 0
	.byte	0x94
	.byte	0x4
	.byte	0x8
	.byte	0x20
	.byte	0x24
	.byte	0x8
	.byte	0x20
	.byte	0x26
	.byte	0x32
	.byte	0x24
	.byte	0x76
	.sleb128 0
	.byte	0x22
	.quad	.LVL205-.Ltext0
	.quad	.LVL206-.Ltext0
	.value	0xf
	.byte	0x79
	.sleb128 4
	.byte	0x94
	.byte	0x4
	.byte	0x8
	.byte	0x20
	.byte	0x24
	.byte	0x8
	.byte	0x20
	.byte	0x26
	.byte	0x32
	.byte	0x24
	.byte	0x76
	.sleb128 0
	.byte	0x22
	.quad	.LVL234-.Ltext0
	.quad	.LVL236-.Ltext0
	.value	0x1
	.byte	0x55
	.quad	.LVL236-.Ltext0
	.quad	.LFE6-.Ltext0
	.value	0x2
	.byte	0x71
	.sleb128 0
	.quad	0
	.quad	0
	.section	.debug_aranges,"",@progbits
	.long	0x2c
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x8
	.byte	0
	.value	0
	.value	0
	.quad	.Ltext0
	.quad	.Letext0-.Ltext0
	.quad	0
	.quad	0
	.section	.debug_ranges,"",@progbits
.Ldebug_ranges0:
	.quad	.LBB10-.Ltext0
	.quad	.LBE10-.Ltext0
	.quad	.LBB17-.Ltext0
	.quad	.LBE17-.Ltext0
	.quad	.LBB18-.Ltext0
	.quad	.LBE18-.Ltext0
	.quad	.LBB19-.Ltext0
	.quad	.LBE19-.Ltext0
	.quad	.LBB31-.Ltext0
	.quad	.LBE31-.Ltext0
	.quad	.LBB35-.Ltext0
	.quad	.LBE35-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB11-.Ltext0
	.quad	.LBE11-.Ltext0
	.quad	.LBB12-.Ltext0
	.quad	.LBE12-.Ltext0
	.quad	.LBB13-.Ltext0
	.quad	.LBE13-.Ltext0
	.quad	.LBB14-.Ltext0
	.quad	.LBE14-.Ltext0
	.quad	.LBB15-.Ltext0
	.quad	.LBE15-.Ltext0
	.quad	.LBB16-.Ltext0
	.quad	.LBE16-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB20-.Ltext0
	.quad	.LBE20-.Ltext0
	.quad	.LBB32-.Ltext0
	.quad	.LBE32-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB21-.Ltext0
	.quad	.LBE21-.Ltext0
	.quad	.LBB22-.Ltext0
	.quad	.LBE22-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB23-.Ltext0
	.quad	.LBE23-.Ltext0
	.quad	.LBB33-.Ltext0
	.quad	.LBE33-.Ltext0
	.quad	.LBB34-.Ltext0
	.quad	.LBE34-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB24-.Ltext0
	.quad	.LBE24-.Ltext0
	.quad	.LBB29-.Ltext0
	.quad	.LBE29-.Ltext0
	.quad	.LBB30-.Ltext0
	.quad	.LBE30-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB25-.Ltext0
	.quad	.LBE25-.Ltext0
	.quad	.LBB28-.Ltext0
	.quad	.LBE28-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB26-.Ltext0
	.quad	.LBE26-.Ltext0
	.quad	.LBB27-.Ltext0
	.quad	.LBE27-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB38-.Ltext0
	.quad	.LBE38-.Ltext0
	.quad	.LBB47-.Ltext0
	.quad	.LBE47-.Ltext0
	.quad	.LBB48-.Ltext0
	.quad	.LBE48-.Ltext0
	.quad	.LBB49-.Ltext0
	.quad	.LBE49-.Ltext0
	.quad	.LBB50-.Ltext0
	.quad	.LBE50-.Ltext0
	.quad	.LBB51-.Ltext0
	.quad	.LBE51-.Ltext0
	.quad	.LBB52-.Ltext0
	.quad	.LBE52-.Ltext0
	.quad	.LBB53-.Ltext0
	.quad	.LBE53-.Ltext0
	.quad	0
	.quad	0
	.quad	.LBB39-.Ltext0
	.quad	.LBE39-.Ltext0
	.quad	.LBB40-.Ltext0
	.quad	.LBE40-.Ltext0
	.quad	.LBB41-.Ltext0
	.quad	.LBE41-.Ltext0
	.quad	.LBB42-.Ltext0
	.quad	.LBE42-.Ltext0
	.quad	.LBB43-.Ltext0
	.quad	.LBE43-.Ltext0
	.quad	.LBB44-.Ltext0
	.quad	.LBE44-.Ltext0
	.quad	.LBB45-.Ltext0
	.quad	.LBE45-.Ltext0
	.quad	.LBB46-.Ltext0
	.quad	.LBE46-.Ltext0
	.quad	0
	.quad	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF8:
	.string	"sort_split"
.LASF5:
	.string	"/media/garviz/Toci"
.LASF3:
	.string	"GNU C 4.7.2"
.LASF7:
	.string	"choose_pivot"
.LASF0:
	.string	"select_sort_split"
.LASF2:
	.string	"bucketsort"
.LASF1:
	.string	"update_group"
.LASF4:
	.string	"qsufsort.c"
.LASF6:
	.string	"transform"
.LASF9:
	.string	"suffixsort"
	.ident	"GCC: (GNU) 4.7.2"
	.section	.note.GNU-stack,"",@progbits
