
#ifndef __S1D13700_H__
#define __S1D13700_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define LCD_WIDTH	240
#define LCD_HEIGHT	320

void lcdDispOn(uint8_t DispAttr);
void lcdDispOff(void);
	// DispAttr def (sum one of each prefix)
	#define SB3_OFF			(0x00) //choose one SB3
	#define SB3_ON			(0x40)
	#define SB3_FLASH		(0x80)
	#define SB3_FLASH16		(0xC0)
	#define SB2_OFF			(0x00) //choose one SB2
	#define SB2_ON			(0x10)
	#define SB2_FLASH		(0x20)
	#define SB2_FLASH16		(0x30)
	#define SB1_OFF			(0x00) //choose one SB1
	#define SB1_ON			(0x04)
	#define SB1_FLASH		(0x08)
	#define SB1_FLASH16		(0x0C)
	#define CSR_OFF			(0x00) //choose one CUR
	#define CSR_ON			(0x01)
	#define CSR_FLASH		(0x02)
	#define CSR_FLASH16		(0x03)

//==================================================================================================
//	Variables
//==================================================================================================
typedef struct {
	uint16_t origin;
	uint8_t size;
	uint8_t is_char_SB;
} LCD_SB_t;

typedef struct{
	uint8_t d[256*8];
}CGRAM_t;

extern LCD_SB_t SB1;
extern LCD_SB_t SB2;
extern LCD_SB_t SB3;

//==================================================================================================
// High-level LCD Commands
//==================================================================================================

void lcd_Init(CGRAM_t *CGRAM); // Inits screen. Clears RAM, Loads CGRAM
void lcd_SetDisplayMode(uint8_t DisplayMode);
	#define LCDMODE_2LAYER	0
	#define LCDMODE_3LAYER	1

void lcd_puts(LCD_SB_t *SB, uint16_t x, uint16_t y, const char *str);
	// prints string at coordinates (top-left corner of text start)
	// graphic or char SBs

// Graphic SB only drawing functions
void lcd_HLine(LCD_SB_t *SB, uint16_t x, uint16_t y, uint16_t x2, uint8_t value);
void lcd_VLine(LCD_SB_t *SB, uint16_t x, uint16_t y, uint16_t y2, uint8_t value);
void lcd_Rect(LCD_SB_t *SB, uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2, uint8_t value);
void lcd_PutImg(LCD_SB_t *SB, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *img, uint8_t invert);
	
#ifdef __cplusplus
}
#endif

#endif

