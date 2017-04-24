#ifndef DELAY_H
#define DELAY_H

#include <msp430_xc.h>

#ifndef F_CPU
    #warning F_CPU not defined for <delay.h>. Using default
    #define F_CPU   1000000UL // 1 MHz
#endif

#if !(F_CPU)
    #error "F_CPU must be a constant value"
#endif

#define _delay_ms(ms)   __delay_cycles((F_CPU*ms)/1000UL)
#define _delay_us(us)   __delay_cycles((F_CPU*us)/1000000UL)

#endif
