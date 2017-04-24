
#include <stdint.h>
#include <stddef.h>
#include "S1D13700.h"
#include <lcd_draw.h>


LCD_SB_t SB1;
LCD_SB_t SB2;
LCD_SB_t SB3;

//--------------------------------------------------------------------------------------------------
void lcdDispOn(uint8_t DispAttr){
	lcddraw_Refresh();
}

//--------------------------------------------------------------------------------------------------
void lcdDispOff(void){
	
}

//--------------------------------------------------------------------------------------------------
void lcd_Init(CGRAM_t *CGRAM){
	lcddraw_Init();
}

//--------------------------------------------------------------------------------------------------
void lcd_SetDisplayMode(uint8_t DisplayMode){
	
}

//--------------------------------------------------------------------------------------------------
void lcd_puts(LCD_SB_t *SB, uint16_t x, uint16_t y, const char *str){
	lcddraw_puts(x, y,str);
}

//--------------------------------------------------------------------------------------------------
void lcd_HLine(LCD_SB_t *SB, uint16_t x, uint16_t y, uint16_t x2, uint8_t value){
	uint16_t tmp;
	if(x > x2){
		tmp = x;
		x = x2;
		x2 = tmp;
	}
	x2 += 1;
	
	if(value == 1){
		lcddraw_Line(x,y,x2,y,C_BLACK);
	}else if(value == 0){
		lcddraw_Line(x,y,x2,y,C_WHITE);
	}else{
		lcddraw_Line(x,y,x2,y,C_RED);
	}
}

//--------------------------------------------------------------------------------------------------
void lcd_VLine(LCD_SB_t *SB, uint16_t x, uint16_t y, uint16_t y2, uint8_t value){
	uint16_t tmp;
	if(y > y2){
		tmp = y;
		y = y2;
		y2 = tmp;
	}
	y2 += 1;
	
	if(value == 1){
		lcddraw_Line(x,y,x,y2,C_BLACK);
	}else if(value == 0){
		lcddraw_Line(x,y,x,y2,C_WHITE);
	}else{
		lcddraw_Line(x,y,x,y2,C_RED);
	}
}

//--------------------------------------------------------------------------------------------------
void lcd_Rect(LCD_SB_t *SB, uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2, uint8_t value){
	uint16_t tmp;
	if(x1 > x2){
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if(y1 > y2){
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	
	if((x1 == x2) && (y1 == y2)){
		lcddraw_SetPixel(x1,y1,value);
		return;
	}else if(x1 == x2){
		lcd_VLine(SB,x1,y1,y2,value);
		return;
	}else if(y1 == y2){
		lcd_HLine(SB,x1,y1,x2,value);
		return;
	}
	
	y2+= 1;
	x2+= 1;
	
	if(value == 1){
		lcddraw_Rectangle(x1, y1, x2, y2,C_BLACK, C_BLACK);
	}else if(value == 0){
		lcddraw_Rectangle(x1, y1, x2, y2,C_WHITE, C_WHITE);
	}else{
		lcddraw_Rectangle(x1, y1, x2, y2,C_RED, C_RED);
	}
}

//--------------------------------------------------------------------------------------------------
void lcd_PutImg(LCD_SB_t *SB, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *img, uint8_t invert){
	uint16_t xi, yi;
	uint8_t mask;
	uint16_t bpc;
	size_t idx;
	
	if(h%8){
		bpc = h/8+1;
	}else{
		bpc = h/8;
	}
	
	// NOTE: bottom of img is yi=0
	
	for(xi=0;xi<w;xi++){
		for(yi=0;yi<h;yi++){
			idx = xi*bpc + yi/8;
			mask = 0x80 >> (yi%8);
			if(invert){
				lcddraw_SetPixel(x+xi,y+h-yi-1,!(img[idx] & mask));
			}else{
				lcddraw_SetPixel(x+xi,y+h-yi-1,img[idx] & mask);
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------

