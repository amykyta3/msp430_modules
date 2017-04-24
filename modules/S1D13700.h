/*
* Copyright (c) 2012, Alexander I. Mykyta
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met: 
* 
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer. 
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution. 
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
* \addtogroup MOD_S1D13700 S1D13700 Display Driver
* \brief Interface to the S1D13700 LCD Driver
* \author Alex Mykyta 
*
* \todo Needs to be documented
* 
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_S1D13700
* \author Alex Mykyta 
**/

#ifndef S1D13700_H
#define S1D13700_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "S1D13700_config.h"

#if LCD_PORTRAIT
    #define LCD_WIDTH    LCD_RESY
    #define LCD_HEIGHT    LCD_RESX
#else
    #define LCD_WIDTH    LCD_RESX
    #define LCD_HEIGHT    LCD_RESY
#endif

//==================================================================================================
//    Variables
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
// Low-level functions
//==================================================================================================
// Read & Write commands must follow an appropriate CMD
void lcd_Cmd(uint8_t cmd);
void lcd_Write(uint8_t b);
uint8_t lcd_Read(void);
#define LCD_CSclr()    LCDCTL_OUT &= ~LCD_CS
#define LCD_CSset()    LCDCTL_OUT |= LCD_CS

//==================================================================================================
// Base LCD Commands
//==================================================================================================

void lcdSystemSet(uint8_t use_CGRAM);
void lcdPowerSave(void);
void lcdDispOn(uint8_t DispAttr);
void lcdDispOff(void);
    // DispAttr def (sum one of each prefix)
    #define SB3_OFF         (0x00) //choose one SB3
    #define SB3_ON          (0x40)
    #define SB3_FLASH       (0x80)
    #define SB3_FLASH16     (0xC0)
    #define SB2_OFF         (0x00) //choose one SB2
    #define SB2_ON          (0x10)
    #define SB2_FLASH       (0x20)
    #define SB2_FLASH16     (0x30)
    #define SB1_OFF         (0x00) //choose one SB1
    #define SB1_ON          (0x04)
    #define SB1_FLASH       (0x08)
    #define SB1_FLASH16     (0x0C)
    #define CSR_OFF         (0x00) //choose one CUR
    #define CSR_ON          (0x01)
    #define CSR_FLASH       (0x02)
    #define CSR_FLASH16     (0x03)
void lcdScroll(uint16_t SB1addr,uint8_t SB1sz,uint16_t SB2addr,uint8_t SB2sz,uint16_t SB3addr); // ignoring SB4 since we will never use it
void lcdCSRForm(uint8_t CSRwidth,uint8_t CSRheight, uint8_t CSRmode);
    #define CSR_UNDERSCORE  (0x00)
    #define CSR_BLOCK       (0x80)
void lcdCSRDir(uint8_t dir);   // OPERATION        | LANDSCAPE | PORTRAIT
    #define CSR_P1          (0x00) // +1 to addr    |    Right    |    Up
    #define CSR_M1          (0x01) // -1 to addr    |    Left    |    Down
    #define CSR_PAP         (0x03) // +AP to addr    |    Down    |    Right
    #define CSR_MAP         (0x02) // -AP to addr    |    Up        |    Left
    // alternate definitions: (defined later)
    //            CSR_DIR_UP
    //            CSR_DIR_DOWN
    //            CSR_DIR_LEFT
    //            CSR_DIR_RIGHT
void lcdOvlay(uint8_t OvlayMode);
    // OvlayMode def (sum one of each prefix)
    #define OV_2LAYERS      (0x00)
    #define OV_3LAYERS      (0x10)
    #define SB3_TEXT        (0x00)
    #define SB3_GRAPHICS    (0x08)
    #define SB1_TEXT        (0x00)
    #define SB1_GRAPHICS    (0x04)
    #define MIX_OR          (0x00)
    #define MIX_XOR         (0x01)
    #define MIX_AND         (0x02)
void lcdCGRAMaddr(uint16_t CGRAMaddr);
void lcdHDotScr(uint8_t dots);
void lcdCSRW(uint16_t CSRaddr);
uint16_t lcdCSRR(void);


//==================================================================================================
// High-level LCD Commands
//==================================================================================================

void lcd_Init(CGRAM_t *CGRAM); // Inits screen. Clears RAM, Loads CGRAM
void lcd_SetDisplayMode(uint8_t DisplayMode);
    #define LCDMODE_2LAYER    0
    #define LCDMODE_3LAYER    1

uint16_t lcd_SetCursor(LCD_SB_t *SB, uint16_t x, uint16_t y); // supply pointer to screen block object
    // returns raw cursor address
void lcd_puts(LCD_SB_t *SB, uint16_t x, uint16_t y, const char *str);
    // prints string at coordinates (top-left corner of text start)
    // graphic or char SBs

// Graphic SB only drawing functions
    ///\todo check bounds (or something) on these functions. Crashes if large coords
void lcd_HLine(LCD_SB_t *SB, uint16_t x, uint16_t y, uint16_t x2, uint8_t value);
void lcd_VLine(LCD_SB_t *SB, uint16_t x, uint16_t y, uint16_t y2, uint8_t value);
void lcd_Rect(LCD_SB_t *SB, uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2, uint8_t value);
void lcd_PutImg(LCD_SB_t *SB, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *img, uint8_t invert);
/**
 * 
 *     Notes:
 *         - Two main modes of operation:
 *             - 2 layer mode:
 *                 Good for page view without a panning graphic area. Text can be redrawn quickly using CGRAM
 *                 - Layer 1: Text mode. Contains quickly updateable text chars
 *                 - Layer 2: Graphics. Used for graphical enhancements as well as highlighting text in layer 1.
 *             - 3 layer mode: All layers graphic mode
 *                 Used when a pannable graphic region is required
 *                 - Layer 1: Pannable graphic region. Sized larger than the actual viewable area
 *                 - Layer 2: Mask layer. Layer 1's visible region is set to black. Layers are mixed using AND
 *                 - Layer 3: All static content. Text must be drawn manually.
 * 
 * 
 **/

#ifdef __cplusplus
}
#endif

#endif

///\}
