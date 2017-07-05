/**
* \file
* \brief MSP430 Cross-Compiler Header
* \author Alex Mykyta
* Include file for all MSP430 device variants. \n
* This include allows for code compatibility between the following compilers:
*    - MSPGCC
*    - TI Compiler
**/
        
#ifndef MSP430_XC_H
#define MSP430_XC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <msp430.h>
#include <isr_xc.h>
#include <intrinsics_xc.h>

#if defined(__GNUC__)
#define __memory_barrier()  __asm__ volatile ("" ::: "memory")
#else
#define __memory_barrier()
#endif

#ifdef __cplusplus
}
#endif

#endif

