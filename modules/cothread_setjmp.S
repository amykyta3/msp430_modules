
    .section .text.ct_sjlj
    
#if __MSP430X__ && __MSP430X_LARGE__
#define SAVEREG_SIZE 4
#define X_MOV   mova
#define X_BR    bra
#define X_POP   popx.a
#define X_PUSH  pushx.a
#define X_RET   reta
#else
#define SAVEREG_SIZE 2
#define X_MOV   mov
#define X_BR    br
#define X_POP   pop
#define X_PUSH  push
#define X_RET   ret
#endif

/* 
 * int ct_setjmp (jmp_buf env)
 * &env -> R12
 * retval <- R12
 */
    .global ct_setjmp
    .type   ct_setjmp, @function
    .func   ct_setjmp
ct_setjmp:
    X_MOV   R15, 13 * SAVEREG_SIZE(R12) ; Save R15 first
    X_POP   R15                         ; Get return PC
    X_MOV   R1,  0 * SAVEREG_SIZE(R12)  ; SP
    X_MOV   R2,  1 * SAVEREG_SIZE(R12)  ; SR
    X_MOV   R4,  2 * SAVEREG_SIZE(R12)  
    X_MOV   R5,  3 * SAVEREG_SIZE(R12)  
    X_MOV   R6,  4 * SAVEREG_SIZE(R12)  
    X_MOV   R7,  5 * SAVEREG_SIZE(R12)  
    X_MOV   R8,  6 * SAVEREG_SIZE(R12)
    X_MOV   R9,  7 * SAVEREG_SIZE(R12)
    X_MOV   R10, 8 * SAVEREG_SIZE(R12)
    X_MOV   R11, 9 * SAVEREG_SIZE(R12)
    X_MOV   R13, 10 * SAVEREG_SIZE(R12)
    X_MOV   R14, 11 * SAVEREG_SIZE(R12)
    X_MOV   R15, 12 * SAVEREG_SIZE(R12)  ; Save return PC
    clr     R12 ; retval 0
    X_BR    R15 ; return
.endfunc
    
/* 
 * void ct_longjmp (jmp_buf env, int val)
 * env -> R12
 * val -> R13
 */
    .global ct_longjmp
    .type   ct_longjmp, @function
    .func   ct_longjmp
ct_longjmp:
    
    X_MOV R12, R15      ; env pointer
    mov R13, R12        ; return value
    
    X_MOV    @R15+, R1  ; 0 SP
    X_MOV    @R15+, R2  ; 1 SR
    X_MOV    @R15+, R4  ; 2
    X_MOV    @R15+, R5  ; 3
    X_MOV    @R15+, R6  ; 4
    X_MOV    @R15+, R7  ; 5
    X_MOV    @R15+, R8  ; 6
    X_MOV    @R15+, R9  ; 7
    X_MOV    @R15+, R10 ; 8
    X_MOV    @R15+, R11 ; 9
    X_MOV    @R15+, R13 ; 10
    X_MOV    @R15+, R14 ; 11
    X_PUSH   @R15+      ; 12 Push return destination onto stack
    X_MOV    @R15,  R15 ; 13
    X_RET               ; Return 
.endfunc
