
#ifndef CLOCK_SYS_H_
#define CLOCK_SYS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <result.h>

//==================================================================================================
// Universal SYS defines:
//==================================================================================================

#define ACLK_FREQ			32768
#define SMCLK_FREQ			25000000
#define MCLK_FREQ			25000000

//==================================================================================================
// Universal Functions
//==================================================================================================

void clock_init(void);

RES_t clock_SetDivMCLK(uint8_t div);

#ifdef __cplusplus
}
#endif

#endif /*CLOCK_SYS_H_*/

///\}
