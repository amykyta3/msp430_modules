
#ifndef BITS_BYTESWAP_H
#define BITS_BYTESWAP_H

#include <stdint.h>
#include <msp430.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __bswap_16(x) __swap_bytes(x)

///\todo MSPGCC compiles this with a bunch of actual shifts. Rewrite as an asm macro
__inline__ __attribute__((__const__)) uint32_t __bswap_32 (uint32_t x){
    return((((uint32_t)__swap_bytes((x & 0xFFFF0000ul ) >> 16))       ) | 
           (((uint32_t)__swap_bytes((x & 0x0000FFFFul )      )) << 16));
}

///\todo MSPGCC compiles this with a bunch of actual shifts. Rewrite as an asm macro
__inline__ __attribute__((__const__)) uint64_t __bswap_64 (uint64_t x){
    return((((uint64_t)__swap_bytes((x & 0xFFFF000000000000ull ) >> 48))      ) | 
           (((uint64_t)__swap_bytes((x & 0x0000FFFF00000000ull ) >> 32)) << 16) |
           (((uint64_t)__swap_bytes((x & 0x00000000FFFF0000ull ) >> 16)) << 32) |
           (((uint64_t)__swap_bytes((x & 0x000000000000FFFFull )      )) << 48));
}

#ifdef __cplusplus
}
#endif

#endif
