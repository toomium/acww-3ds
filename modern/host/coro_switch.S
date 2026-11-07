// Host build: host_coro_switch (coro.h) for each host CPU. The saved frame, from the stack pointer up:
//   i386:   edi esi ebx ebp, return address
//   x86-64 System V: r15 r14 r13 r12 rbx rbp, return address
//   x86-64 Windows:  xmm6-xmm15 (160 bytes), the TIB's StackLimit, StackBase and DeallocationStack, r15 r14 r13 r12
//                    rdi rsi rbx rbp, return address (256 bytes)
//   arm64:  x19-x28, x29 (fp), x30 (lr), d8-d15
//   arm (32-bit, AAPCS with VFP): d8-d15, r4-r11, r12 (only to keep the stack 8-byte aligned), lr
// host_coro_prepare (coro.c) builds the same frame on a new stack with host_coro_boot as the return address and
// fn / arg in callee-saved registers, which host_coro_boot passes on.

#if defined(__APPLE__)
#define SYM(x) _##x
#else
#define SYM(x) x
#endif

    .text

#if defined(__i386__)

    .globl SYM(host_coro_switch)
SYM(host_coro_switch):
    movl 4(%esp), %eax      // save
    movl 8(%esp), %edx      // to
    pushl %ebp
    pushl %ebx
    pushl %esi
    pushl %edi
    movl %esp, (%eax)
    movl %edx, %esp
    popl %edi
    popl %esi
    popl %ebx
    popl %ebp
    ret

// first entry of a prepared stack: ebx = fn, esi = arg; the stack is 16-byte aligned here
    .globl SYM(host_coro_boot)
SYM(host_coro_boot):
    subl $12, %esp
    pushl %esi
    call *%ebx
    ud2

#elif defined(__x86_64__) && !defined(_WIN32)

    .globl SYM(host_coro_switch)
SYM(host_coro_switch):
    pushq %rbp
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    movq %rsp, (%rdi)
    movq %rsi, %rsp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    popq %rbp
    ret

// r12 = fn, r13 = arg; the stack is 16-byte aligned here
    .globl SYM(host_coro_boot)
SYM(host_coro_boot):
    movq %r13, %rdi
    callq *%r12
    ud2

#elif defined(__aarch64__)

    .globl SYM(host_coro_switch)
SYM(host_coro_switch):
    sub sp, sp, #176
    stp x19, x20, [sp, #0]
    stp x21, x22, [sp, #16]
    stp x23, x24, [sp, #32]
    stp x25, x26, [sp, #48]
    stp x27, x28, [sp, #64]
    stp x29, x30, [sp, #80]
    stp d8, d9, [sp, #96]
    stp d10, d11, [sp, #112]
    stp d12, d13, [sp, #128]
    stp d14, d15, [sp, #144]
    mov x9, sp
    str x9, [x0]
    mov sp, x1
    ldp x19, x20, [sp, #0]
    ldp x21, x22, [sp, #16]
    ldp x23, x24, [sp, #32]
    ldp x25, x26, [sp, #48]
    ldp x27, x28, [sp, #64]
    ldp x29, x30, [sp, #80]
    ldp d8, d9, [sp, #96]
    ldp d10, d11, [sp, #112]
    ldp d12, d13, [sp, #128]
    ldp d14, d15, [sp, #144]
    add sp, sp, #176
    ret

// x19 = fn, x20 = arg
    .globl SYM(host_coro_boot)
SYM(host_coro_boot):
    mov x0, x20
    blr x19
    brk #0

#elif defined(__arm__)

    .syntax unified
    .arm
    .fpu vfp

    .globl SYM(host_coro_switch)
    .type SYM(host_coro_switch), %function
SYM(host_coro_switch):
    push {r4-r12, lr}
    vpush {d8-d15}
    str sp, [r0]
    mov sp, r1
    vpop {d8-d15}
    pop {r4-r12, lr}
    bx lr

// r4 = fn, r5 = arg
    .globl SYM(host_coro_boot)
    .type SYM(host_coro_boot), %function
SYM(host_coro_boot):
    mov r0, r5
    blx r4
    bkpt #0

#elif !(defined(__x86_64__) && defined(_WIN32))
#error "coro_switch.S: no stack switch for this CPU yet"
#endif

// x86-64 Windows (Microsoft x64 calling convention): arguments in rcx, rdx; rbx, rbp, rdi, rsi, r12-r15 and
// xmm6-xmm15 are callee-saved; a callee may use the 32 bytes above its return address (shadow space). The thread's
// stack bounds in the TIB (StackBase gs:0x08, StackLimit gs:0x10, DeallocationStack gs:0x1478) go with the stack:
// the system checks the stack pointer against them (exception dispatch and unwinding, __chkstk's guard-page walk,
// stack overflow handling), so each stack carries its own. (No .pdata unwind information: a debugger's stack walk
// stops at host_coro_boot, whose frame's return address is 0.)
//
// HOST_CORO_WIN64_TEST (modern/host/tests/coro_win64_test.c, on x86-64 Linux): the same code assembled as
// host_coro_switch_w64 / host_coro_boot_w64 next to the System V functions, with the TIB read through the pointer
// host_coro_test_teb instead of gs (which Linux doesn't point at anything); the test calls them as ms_abi functions.
#if (defined(__x86_64__) && defined(_WIN32)) || (defined(__x86_64__) && defined(HOST_CORO_WIN64_TEST))

#if defined(_WIN32)
#define W64(x) x
#define LOAD_TEB(r) movq %gs:0x30, r   /* NT_TIB.Self: the TIB's linear address */
#else
#define W64(x) x##_w64
#define LOAD_TEB(r) movq host_coro_test_teb(%rip), r
#endif

    .globl W64(host_coro_switch)
W64(host_coro_switch):
    pushq %rbp
    pushq %rbx
    pushq %rsi
    pushq %rdi
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
    subq $184, %rsp         // (16-byte aligned from here: the entry's rsp is 8 mod 16)
    movaps %xmm6, 0(%rsp)
    movaps %xmm7, 16(%rsp)
    movaps %xmm8, 32(%rsp)
    movaps %xmm9, 48(%rsp)
    movaps %xmm10, 64(%rsp)
    movaps %xmm11, 80(%rsp)
    movaps %xmm12, 96(%rsp)
    movaps %xmm13, 112(%rsp)
    movaps %xmm14, 128(%rsp)
    movaps %xmm15, 144(%rsp)
    LOAD_TEB(%r10)
    movq 0x10(%r10), %rax   // StackLimit
    movq %rax, 160(%rsp)
    movq 0x08(%r10), %rax   // StackBase
    movq %rax, 168(%rsp)
    movq 0x1478(%r10), %rax // DeallocationStack
    movq %rax, 176(%rsp)
    movq %rsp, (%rcx)       // *save
    movq %rdx, %rsp         // to
    movq 160(%rsp), %rax
    movq %rax, 0x10(%r10)
    movq 168(%rsp), %rax
    movq %rax, 0x08(%r10)
    movq 176(%rsp), %rax
    movq %rax, 0x1478(%r10)
    movaps 0(%rsp), %xmm6
    movaps 16(%rsp), %xmm7
    movaps 32(%rsp), %xmm8
    movaps 48(%rsp), %xmm9
    movaps 64(%rsp), %xmm10
    movaps 80(%rsp), %xmm11
    movaps 96(%rsp), %xmm12
    movaps 112(%rsp), %xmm13
    movaps 128(%rsp), %xmm14
    movaps 144(%rsp), %xmm15
    addq $184, %rsp
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rdi
    popq %rsi
    popq %rbx
    popq %rbp
    ret

// r12 = fn, r13 = arg; rsp = the stack's 16-aligned top here: the shadow space for fn below it, then the call
// (which leaves rsp 8 mod 16 at fn's entry, as the convention wants)
    .globl W64(host_coro_boot)
W64(host_coro_boot):
    subq $32, %rsp
    movq %r13, %rcx
    callq *%r12
    ud2

#endif

#if defined(__linux__) && defined(__ELF__)
    .section .note.GNU-stack, "", @progbits
#endif
