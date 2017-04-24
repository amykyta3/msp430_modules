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

/*==================================================================================================
* File History:
* NAME          DATE         COMMENTS
* Alex M.       2011-08-01   born
* Alex M.       2012-02-22   Portions rewritten in ASM for reliability
* 
*=================================================================================================*/

/**
* \addtogroup MOD_S1D13700
* \todo Needs to be documented
* \{
**/

/**
* \file
* \brief Code for \ref MOD_S1D13700
* \author Alex Mykyta 
**/

#include <stdint.h>
#include <sleep.h>

#include "msp430_xc.h"
#include "S1D13700.h"
#include "S1D13700_internal.h"



LCD_SB_t SB1;
LCD_SB_t SB2;
LCD_SB_t SB3;

//==================================================================================================
// Low-level functions
//==================================================================================================

void lcd_Cmd(uint8_t cmd){
    LCDCTL_OUT |= LCD_A0;    // command mode
    LCDD_DIR = 0xFF;

    LCDCTL_OUT &= ~LCD_WR; // Write Command
    LCDD_OUT = cmd;
    LCDCTL_OUT |= LCD_WR;
    LCDCTL_OUT &= ~LCD_A0; // parameter mode
}

//--------------------------------------------------------------------------------------------------
void lcd_Write(uint8_t b){
    LCDD_DIR = 0xFF;
    LCDCTL_OUT &= ~LCD_WR;
    LCDD_OUT = b;
    LCDCTL_OUT |= LCD_WR;
}

//--------------------------------------------------------------------------------------------------
uint8_t lcd_Read(void){
    uint8_t b;
    LCDCTL_OUT |= LCD_A0;
    LCDD_DIR = 0x00;
    LCDCTL_OUT &= ~LCD_RD;
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    b = LCDD_IN;                    
    LCDCTL_OUT |= LCD_RD;
    LCDCTL_OUT &= ~LCD_A0;
    return(b);
}

//==================================================================================================
// Base LCD Commands
//==================================================================================================
void lcdSystemSet(uint8_t use_CGRAM){
    // do SYSTEM_SET
    LCD_CSclr();
    lcd_Cmd(LCDCMD_SYSTEM_SET);
    msleep(3); // wait 3ms
    if(use_CGRAM){
        lcd_Write(0x11); //orgin compensated, 1 panel, 8-tall CGCHAR, CGRAM
    }else{
        lcd_Write(0x10); //orgin compensated, 1 panel, 8-tall CGCHAR
    }
    lcd_Write(0x00 + (LCD_CHARX-1));    //16 line AC drive + [FX]
    lcd_Write(LCD_CHARY-1);    //[FY]
    lcd_Write(LCD_CPR); //[C/R]
    lcd_Write(LCD_TCPR); //[TC/R]
    lcd_Write(LCD_RESY-1); //[L/F]
    lcd_Write(LCD_CANVASX_0);
    lcd_Write(LCD_CANVASX_1);
    LCD_CSset();
}

//--------------------------------------------------------------------------------------------------
void lcdPowerSave(void){
    LCD_CSclr();
    lcd_Cmd(LCDCMD_POWER_SAVE);
    LCD_CSset();
}

//--------------------------------------------------------------------------------------------------
void lcdDispOn(uint8_t DispAttr){
    LCD_CSclr();
    lcd_Cmd(LCDCMD_DISPON);
    lcd_Write(DispAttr); // write reg 0A
    LCD_CSset();
}

//--------------------------------------------------------------------------------------------------
void lcdDispOff(void){
    LCD_CSclr();
    lcd_Cmd(LCDCMD_DISPOFF);
    lcd_Write(0); // write reg 0A
    LCD_CSset();
}

//--------------------------------------------------------------------------------------------------
void lcdScroll(uint16_t SB1addr,uint8_t SB1sz,uint16_t SB2addr,uint8_t SB2sz,uint16_t SB3addr){
    SB1.origin = SB1addr;
    SB2.origin = SB2addr;
    SB3.origin = SB3addr;
    SB1.size = SB1sz;
    SB2.size = SB2sz;

    LCD_CSclr();
    lcd_Cmd(LCDCMD_SCROLL);
    lcd_Write(SB1addr & 0xFF);
    lcd_Write((SB1addr>>8) & 0xFF);
    lcd_Write(SB1sz);
    lcd_Write(SB2addr & 0xFF);
    lcd_Write((SB2addr>>8) & 0xFF);
    lcd_Write(SB2sz);
    lcd_Write(SB3addr & 0xFF);
    lcd_Write((SB3addr>>8) & 0xFF);
    lcd_Write(0); // SB4 unused
    lcd_Write(0); // SB4 unused
    LCD_CSset();
}

//--------------------------------------------------------------------------------------------------
void lcdCSRForm(uint8_t CSRwidth,uint8_t CSRheight, uint8_t CSRmode){
    LCD_CSclr();
    lcd_Cmd(LCDCMD_CSRFORM);
    lcd_Write(CSRwidth);
    lcd_Write(CSRheight + CSRmode);
    LCD_CSset();
}

//--------------------------------------------------------------------------------------------------
void lcdCSRDir(uint8_t dir){
    LCD_CSclr();
    lcd_Cmd(LCDCMD_CSRDIR + dir);
    LCD_CSset();
}

//--------------------------------------------------------------------------------------------------
void lcdOvlay(uint8_t OvlayMode){
    LCD_CSclr();
    lcd_Cmd(LCDCMD_OVLAY);
    lcd_Write(OvlayMode);
    LCD_CSset();
}

//--------------------------------------------------------------------------------------------------
void lcdCGRAMaddr(uint16_t CGRAMaddr){
    LCD_CSclr();
    lcd_Cmd(LCDCMD_CGRAMADR);
    lcd_Write(CGRAMaddr & 0xFF);
    lcd_Write((CGRAMaddr>>8) & 0xFF);
    LCD_CSset();
}

//--------------------------------------------------------------------------------------------------
void lcdHDotScr(uint8_t dots){
    LCD_CSclr();
    lcd_Cmd(LCDCMD_HDOTSCR);
    lcd_Write(dots);
    LCD_CSset();
}

//--------------------------------------------------------------------------------------------------
void lcdCSRW(uint16_t CSRaddr){
    LCD_CSclr();
    lcd_Cmd(LCDCMD_CSRW);
    lcd_Write(CSRaddr & 0xFF);
    lcd_Write((CSRaddr>>8) & 0xFF);
    LCD_CSset();
}

//--------------------------------------------------------------------------------------------------
uint16_t lcdCSRR(void){
    uint16_t addr;
    LCD_CSclr();
    lcd_Cmd(LCDCMD_CSRR);
    addr = lcd_Read();
    addr += (lcd_Read()<<8);
    LCD_CSset();
    return(addr);
}

//==================================================================================================
// High-level LCD Commands
//==================================================================================================

void lcd_Init(CGRAM_t *CGRAM){
    uint16_t i;
    
    LCDCTL_OUT &= ~(LCD_RST);// perform hardware reset
    msleep(1); // wait 1ms
    LCDCTL_OUT |= LCD_RST; // un-reset
    
    if(CGRAM){
        lcdSystemSet(1);
    }else{
        lcdSystemSet(0);
    }
    
    lcdDispOff();
    
    if(CGRAM){
        //load CGRAM
        lcdCSRW(0x8000-sizeof(CGRAM_t)); // set cursor to start of CGRAM
        lcdCSRDir(CSR_P1); // +1 increment mode
        
        LCD_CSclr();
        lcd_Cmd(LCDCMD_MEMWRITE);
        for(i=0;i<sizeof(CGRAM_t);i++){
            lcd_Write(CGRAM->d[i]);
        }
        LCD_CSset();
        lcdCGRAMaddr(0x8000-sizeof(CGRAM_t));
    }
    
    
}

//--------------------------------------------------------------------------------------------------
void lcd_SetDisplayMode(uint8_t DisplayMode){
    uint16_t i;
    
    switch(DisplayMode){
        case LCDMODE_2LAYER:
            // Layer 1 - SB1: Text Mode
            // Layer 2 - SB2: Graphics
            SB1.is_char_SB = 1;
            SB2.is_char_SB = 0;
            lcdScroll(0x0000,LCD_RESY, //(LCD_RESY/LCD_CHARY)+1,
                      0x2000,LCD_RESY,
                      0x0000);
            
            // clear memory in SBs
            lcdCSRDir(CSR_P1); // +1 increment mode
            lcdCSRW(0x0000); // set cursor to start of SB1
            LCD_CSclr();
            lcd_Cmd(LCDCMD_MEMWRITE);
            for(i=0;i<((LCD_RESY/LCD_CHARY+1)*LCD_CANVASX);i++){
                lcd_Write(0);
            }
            LCD_CSset();
            
            lcdCSRW(0x2000); // set cursor to start of SB2
            LCD_CSclr();
            lcd_Cmd(LCDCMD_MEMWRITE);
            for(i=0;i<(LCD_RESY)*LCD_CANVASX;i++){
                lcd_Write(0);
            }
            LCD_CSset();
            
            lcdOvlay(OV_2LAYERS|SB1_TEXT|MIX_XOR);
            lcdDispOn(CSR_OFF|SB1_ON|SB2_ON|SB3_OFF);
            break;
        case LCDMODE_3LAYER:
            ///\todo Probably should switch to 8-wide chars for this mode. Otherwise, too much space
            ///        is required for each SB. Want to maximize pannable area for this mode.
            ///        Don't use this mode just yet.
            // Layer 1 - SB1: Graphics (pannable)
            // Layer 2 - SB2: Graphics (mask)
            // Layer 3 - SB3: Graphics (static)
            SB1.is_char_SB = 0;
            SB2.is_char_SB = 0;
            SB3.is_char_SB = 0;
            lcdScroll(0x0000,255,
                      0x8000-0x2580-0x2580,LCD_RESY,
                      0x8000-0x2580);
            lcdOvlay(OV_3LAYERS|SB1_GRAPHICS|SB3_GRAPHICS|MIX_AND);
            lcdDispOn(CSR_OFF|SB1_ON|SB2_ON|SB3_ON);
            break;
    }
}

//--------------------------------------------------------------------------------------------------
uint16_t lcd_SetCursor_native(LCD_SB_t *SB, uint16_t x, uint16_t y){
    // Sets the cursor position relative to the SB origin
    // Rounds down for SBs in char mode
    
    uint16_t addr;
    
    // normal mode
    x /= LCD_CHARX;
    if(SB->is_char_SB){
        y /= LCD_CHARY;
    }
    addr = SB->origin + x + (LCD_CANVASX * y);

    lcdCSRW(addr);
    
    return(addr);
}

uint16_t lcd_SetCursor(LCD_SB_t *SB, uint16_t x, uint16_t y){
    #if (LCD_PORTRAIT == 0)
        return(lcd_SetCursor_native(SB,x,y));
    #else
        return(lcd_SetCursor_native(SB,(LCD_RESX-1) - y,x));
    #endif
}

//--------------------------------------------------------------------------------------------------

void lcd_puts(LCD_SB_t *SB, uint16_t x, uint16_t y, const char *str){
    
    lcd_SetCursor(SB,x,y);
    
    if(SB->is_char_SB){
        lcdCSRDir(CSR_DIR_RIGHT);
        LCD_CSclr();
        lcd_Cmd(LCDCMD_MEMWRITE);
        while(*str){
            lcd_Write(*str);
            str++;
        }
        LCD_CSset();
    }else{
        ///\todo Write lcd_puts for graphical SB
        //#warning "todo: Write lcd_puts for graphical SB"
    }
}

//--------------------------------------------------------------------------------------------------

void lcd_HLine_native(LCD_SB_t *SB, uint16_t x, uint16_t y, uint16_t x2, uint8_t value){ 
    uint16_t CSRaddr;
    uint8_t mask, b;
    uint8_t maskpos;
    
    if(SB->is_char_SB) return; // graphic SBs only
    
    if(x > x2){
        uint16_t tmp;
        tmp = x2;
        x2 = x;
        x = tmp;
    }
    
    CSRaddr = lcd_SetCursor_native(SB,x,y);
    lcdCSRDir(CSR_P1); //(move cursor right)
    
    // read lead-in byte
    LCD_CSclr();
    lcd_Cmd(LCDCMD_MEMREAD);
    b = lcd_Read();
    LCD_CSset();
    
    // set bits in lead-in byte
    maskpos = x % LCD_CHARX;
    mask = 0x80;
    mask >>= maskpos;
    while((maskpos < LCD_CHARX) && (x <= x2)){
        if(value){
            b |= mask;
        }else{
            b &= ~mask;
        }
        maskpos++;
        x++;
        mask >>= 1;
    }
    
    // write back lead-in byte
    lcdCSRW(CSRaddr);
    LCD_CSclr();
    lcd_Cmd(LCDCMD_MEMWRITE);
    lcd_Write(b);
    
    CSRaddr++;
    
    if(x > x2){
        LCD_CSset();
        return; // if that was all, return
    }
    
    // write solid bytes
    while(x+LCD_CHARX <= x2){
        if(value){
            lcd_Write(0xFF);
        }else{
            lcd_Write(0x00);
        }
        x += LCD_CHARX;
        CSRaddr++;
    }
    LCD_CSset();
    
    if(x > x2){
        return; // if that was all, return
    }
    
    // read lead-out byte
    LCD_CSclr();
    lcd_Cmd(LCDCMD_MEMREAD);
    b = lcd_Read();
    LCD_CSset();
    
    maskpos = x2 % LCD_CHARX;
    mask = 0x80;
    mask >>= maskpos;
    if(value){
        b |= 0x80;
    }else{
        b &= ~0x80;
    }
    while(maskpos){
        if(value){
            b |= mask;
        }else{
            b &= ~mask;
        }
        maskpos--;
        mask <<= 1;
    }
    
    // write back lead-out byte
    lcdCSRW(CSRaddr);
    LCD_CSclr();
    lcd_Cmd(LCDCMD_MEMWRITE);
    lcd_Write(b);
    LCD_CSset();
    
}

//--------------------------------------------------------------------------------------------------
void lcd_VLine_native(LCD_SB_t *SB, uint16_t x, uint16_t y, uint16_t y2, uint8_t value){
    uint8_t buf[8];
    uint16_t count;
    uint16_t CSRaddr;
    uint8_t mask;
    uint16_t i;
    
    if(SB->is_char_SB) return; // graphic SBs only
    
    if(y > y2){
        uint16_t tmp;
        tmp = y2;
        y2 = y;
        y = tmp;
    }
    
    CSRaddr = lcd_SetCursor_native(SB,x,y);
    lcdCSRDir(CSR_PAP); //(move cursor down)
    
    mask = 0x80;
    mask >>= (x%LCD_CHARX);
    
    while(y <= y2){
        
        // select up to sizeof(buf) lines at a time
        count = y2-y+1;
        if(count > sizeof(buf)) count = sizeof(buf);
        
        // read them in
        LCD_CSclr();
        lcd_Cmd(LCDCMD_MEMREAD);
        for(i=0;i<count;i++){
            buf[i] = lcd_Read();
        }
        LCD_CSset();
        
        // seek back
        lcdCSRW(CSRaddr);
        
        // write back with line drawn
        LCD_CSclr();
        lcd_Cmd(LCDCMD_MEMWRITE);
        for(i=0;i<count;i++){
            if(value){
                lcd_Write(buf[i] | mask);
            }else{
                lcd_Write(buf[i] &= ~mask);
            }
        }
        LCD_CSset();
        
        if(y == y2) break;
        
        // increment for next block of lines
        CSRaddr += count * LCD_CANVASX;
        y += count;
    }
}

//--------------------------------------------------------------------------------------------------
void lcd_HLine(LCD_SB_t *SB, uint16_t x, uint16_t y, uint16_t x2, uint8_t value){
    #if (LCD_PORTRAIT == 0)
        lcd_HLine_native(SB,x,y,x2,value);
    #else
        lcd_VLine_native(SB,(LCD_RESX-1) - y ,x,x2,value);
    #endif
}

void lcd_VLine(LCD_SB_t *SB, uint16_t x, uint16_t y, uint16_t y2, uint8_t value){
    #if (LCD_PORTRAIT == 0)
        lcd_VLine_native(SB,x,y,y2,value);
    #else
        lcd_HLine_native(SB,(LCD_RESX-1) - y,x,(LCD_RESX-1) - y2,value);
    #endif
}

//--------------------------------------------------------------------------------------------------

void lcd_Rect(LCD_SB_t *SB, uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2, uint8_t value){
    uint16_t tmp;
    if(SB->is_char_SB) return; // graphic SBs only
    
    #if (LCD_PORTRAIT == 0)
    
    #else
        // convert to native coordinates
        tmp = x1;
        x1 = (LCD_RESX-1) - y1;
        y1 = tmp;
        
        tmp = x2;
        x2 = (LCD_RESX-1) - y2;
        y2 = tmp;
    #endif
    
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
    
    uint16_t CSRaddr;
    uint8_t mask, b;
    uint8_t maskpos;
    uint16_t yi;
    
    CSRaddr = lcd_SetCursor_native(SB,x1,y1);
    
    lcdCSRDir(CSR_PAP); //(move cursor down)
    
    // Prepare mask
    maskpos = x1 % LCD_CHARX;
    b = 0x80;
    b >>= maskpos;
    mask = 0x00;
    while((maskpos < LCD_CHARX) && (x1 <= x2)){
        mask |= b;
        maskpos++;
        x1++;
        b >>= 1;
    }
    
    yi = y1;
    while(yi <= y2){
        // read lead-in byte
        LCD_CSclr();
        lcd_Cmd(LCDCMD_MEMREAD);
        b = lcd_Read();
        LCD_CSset();
        
        if(value){
            b |= mask;
        }else{
            b &= ~mask;
        }
        
        // write back lead-in byte
        lcdCSRW(CSRaddr);
        LCD_CSclr();
        lcd_Cmd(LCDCMD_MEMWRITE);
        lcd_Write(b);
        LCD_CSset();
        
        CSRaddr += LCD_CANVASX; // increment down
        yi++;
    }
    
    CSRaddr = lcd_SetCursor_native(SB,x1,y1);
    
    if(x1 > x2){
        return; // if that was all, return
    }
    
    // write solid bytes
    while(x1+LCD_CHARX <= x2){
        yi = y1;
        LCD_CSclr();
        lcd_Cmd(LCDCMD_MEMWRITE);
        if(value){
            while(yi <= y2){
                lcd_Write(0xFF);
                yi++;
            }
        }else{
            while(yi <= y2){
                lcd_Write(0x00);
                yi++;
            }
        }
        LCD_CSset();
        
        x1 += LCD_CHARX;
        CSRaddr++;
        lcdCSRW(CSRaddr); // rewind to top
    }
    
    if(x1 > x2){
        return; // if that was all, return
    }
    
    // prepare lead-out mask
    maskpos = x2 % LCD_CHARX;
    b = 0x80;
    b >>= maskpos;
    mask = 0x80;
    while(maskpos){
        mask |= b;
        maskpos--;
        b <<= 1;
    }
    
    yi = y1;
    while(yi <= y2){
        
        // read lead-out byte
        LCD_CSclr();
        lcd_Cmd(LCDCMD_MEMREAD);
        b = lcd_Read();
        LCD_CSset();
        
        
        if(value){
            b |= mask;
        }else{
            b &= ~mask;
        }
        
        // write back lead-out byte
        lcdCSRW(CSRaddr);
        LCD_CSclr();
        lcd_Cmd(LCDCMD_MEMWRITE);
        lcd_Write(b);
        LCD_CSset();
        
        CSRaddr += LCD_CANVASX; // increment down
        yi++;
    }
}

//--------------------------------------------------------------------------------------------------
/* IMAGE DATA:
 * Image data is always stored and interpreted in the native screen orientation.
 * Each byte of image data represents an 8-pixel wide region. 
 * These regions scan starting from the top-left corner (native) and continue left to right for each
 * row (reading order).
 * If an image is not a multiple of 8-pixels wide, the bytes representing the rightmost pixels are
 * padded with 0's
 * 
 * This format allows for CGRAM characters to be copied without modification
*/
void lcd_PutImg(LCD_SB_t *SB, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *img, uint8_t invert){
    uint16_t tmp;
    uint16_t shifter;
    uint8_t valid_left, valid_right;
    uint8_t mod;
    uint16_t wcount;
    uint8_t wbyte_count;
    uint16_t CSRaddr;
    uint16_t left_CSRaddr;
    #if (LCD_PORTRAIT == 0)
    
    #else
        // convert to native coordinates
        tmp = w;
        w = h;
        h = tmp;
        
        tmp = x;
        x = (LCD_RESX) - y - w;
        y = tmp;
    #endif
    
    CSRaddr = lcd_SetCursor_native(SB,x,y);
    left_CSRaddr = CSRaddr;
    lcdCSRDir(CSR_P1); //(move cursor right)
    
    mod = x % LCD_CHARX;
    
    while(h){
        wcount = w + mod;
        if(w%8){
            wbyte_count = (w/8)+1;
        }else{
            wbyte_count = (w/8);
        }
        
        // load first byte of image into shifter
        if(invert){
            tmp = (~(*img++) & 0xFF);
            if(wbyte_count == 1){
                // if it is the last byte
                if(w%8){
                    tmp >>= (8-(w%8));
                    tmp <<= (8-(w%8));
                }
            }
            shifter = tmp;
        }else{
            shifter = *img++;
        }
        valid_left = 8-mod;
        valid_right = 15;
        wbyte_count--;
        
        while(wcount){ // do until line is fully written
            
            // while new data in shifter is not aligned
            while(valid_left != 0){
                // if low byte of shifter is empty and image data is still to be written
                if((valid_right < 8) && wbyte_count){
                    // read next byte from image into the LSB of the shifter
                    if(invert){
                        tmp = (~(*img++) & 0xFF);
                        if(wbyte_count == 1){
                            // if it is the last byte
                            if(w%8){
                                tmp >>= (8-(w%8));
                                tmp <<= (8-(w%8));
                            }
                        }
                        shifter |= tmp;
                    }else{
                        shifter |= *img++;
                    }
                    wbyte_count--;
                    valid_right = 15;

                }
                
                // shift the shifter left once
                shifter <<= 1;
                valid_left--;
                valid_right--;
            }
            
            // if current screen byte is to be partially covered
            if(((wcount == (w+mod)) && (mod)) || (wcount < LCD_CHARX)){
                // load from screen into LSB of tmp
                LCD_CSclr();
                lcd_Cmd(LCDCMD_MEMREAD);
                tmp = lcd_Read();
                LCD_CSset();
                lcdCSRW(CSRaddr); // rewind screen pointer
                
                // clear appropriate bits
                uint8_t mask;
                uint8_t mstart, mlen;
                mstart = 0;
                if(wcount == (w+mod)){
                    mstart = mod;
                }
                mlen = 8;
                if(wcount < LCD_CHARX){
                    mlen = wcount;
                }
                mask = 0x80;
                mask >>= mstart;
                while(mlen){
                    tmp &= ~mask;
                    mask >>= 1;
                    mlen--;
                }
                
                // OR into the MSB of shifter
                shifter |= (tmp << 8);
            }
            
            // write MSB of shifter to screen
            LCD_CSclr();
            lcd_Cmd(LCDCMD_MEMWRITE);
            lcd_Write(shifter >> 8); 
            LCD_CSset();
            CSRaddr++;
            
            // update alignment variables & pixels written
            if(wcount > LCD_CHARX){
                wcount -= LCD_CHARX;
            }else{
                break;
            }
            valid_left = LCD_CHARX;
        }
        
        h--;
        if(h){
            left_CSRaddr += LCD_CANVASX;
            CSRaddr = left_CSRaddr;
            lcdCSRW(CSRaddr);
        }
    }
    
}

///\}
