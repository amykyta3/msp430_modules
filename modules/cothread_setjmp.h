
#ifndef COTHREAD_SETJMP_H
#define COTHREAD_SETJMP_H

#include <stdint.h>

#if __MSP430X__ && __MSP430X_LARGE__
    typedef uint32_t reg_type_t;
#else
    typedef uint16_t reg_type_t;
#endif

typedef struct{
    reg_type_t reg_sp;  // 0
    reg_type_t reg_sr;  // 1
    reg_type_t reg_r4;  // 2
    reg_type_t reg_r5;  // 3
    reg_type_t reg_r6;  // 4
    reg_type_t reg_r7;  // 5
    reg_type_t reg_r8;  // 6
    reg_type_t reg_r9;  // 7
    reg_type_t reg_r10; // 8
    reg_type_t reg_r11; // 9
    reg_type_t reg_r13; // 10
    reg_type_t reg_r14; // 11
    reg_type_t reg_pc;  // 12
    reg_type_t reg_r15; // 13
} ct_jmp_buf[1];

int ct_setjmp (ct_jmp_buf env);
__attribute__((__noreturn__)) void ct_longjmp (ct_jmp_buf env, int val);


#endif
