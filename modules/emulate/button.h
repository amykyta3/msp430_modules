
#ifndef _BUTTON_H_
#define _BUTTON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void button_init(void);
void button_uninit(void);
void button_SetupPort(uint8_t en_mask,uint8_t inverted_mask, uint8_t port);

extern void onButtonDown(uint8_t port, uint8_t b);
extern void onButtonUp(uint8_t port, uint8_t b);
extern void onButtonHold(uint8_t port, uint8_t b);


// Emulation function calls:
void button_emu_down(uint8_t bits);
void button_emu_up(uint8_t bits);
void button_emu_hold(uint8_t bits);

#ifdef __cplusplus
}
#endif

#endif /*BUTTON_H_*/

