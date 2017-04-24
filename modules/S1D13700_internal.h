#ifndef S1D1300_INTERNAL_H
#define S1D1300_INTERNAL_H

#if (LCD_PORTRAIT == 0)
    // normal mode
    #define LCD_CHARX       LCD_CHARW
    #define LCD_CHARY       8
    #define CGRAM_SET       CGRAM_SET_NORMAL
    #define CSR_DIR_UP      CSR_MAP
    #define CSR_DIR_DOWN    CSR_PAP
    #define CSR_DIR_LEFT    CSR_M1
    #define CSR_DIR_RIGHT   CSR_P1
    #define CSR_INC_UP      (-LCD_CANVASX)
    #define CSR_INC_DOWN    (LCD_CANVASX)
    #define CSR_INC_LEFT    (-1)
    #define CSR_INC_RIGHT   (1)
#else
    //portrait mode
    #define LCD_CHARX       8
    #define LCD_CHARY       LCD_CHARW
    #define CGRAM_SET       CGRAM_SET_ROTATED
    #define CSR_DIR_UP      CSR_P1
    #define CSR_DIR_DOWN    CSR_M1
    #define CSR_DIR_LEFT    CSR_MAP
    #define CSR_DIR_RIGHT   CSR_PAP
    #define CSR_INC_UP      (1)
    #define CSR_INC_DOWN    (-1)
    #define CSR_INC_LEFT    (-LCD_CANVASX)
    #define CSR_INC_RIGHT   (LCD_CANVASX)
#endif

#define LCD_CPR             ((LCD_RESX/LCD_CHARX)+2)
#define LCD_TCPR            (((LCD_CLK_FREQ/LCD_RESY)/(70*2*LCD_CLK_DIV))-1)
#if ((LCD_TCPR >= 256)||((LCD_TCPR - LCD_CPR)< 2))
    #error "Check LCD configuration"
#endif

#if (LCD_CLK_DIV == 4)
    #define LCD_CLK_DIV_BITS 0
#elif (LCD_CLK_DIV == 8)
    #define LCD_CLK_DIV_BITS 1
#elif (LCD_CLK_DIV == 16)
    #define LCD_CLK_DIV_BITS 2
#endif

#define LCD_CANVASX_0 (LCD_CANVASX & 0xFF)
#define LCD_CANVASX_1 ((LCD_CANVASX >> 8) & 0xFF)

//==================================================================================================
//    Command Set
//==================================================================================================
#define LCDCMD_SYSTEM_SET   (0x40)
#define LCDCMD_POWER_SAVE   (0x53)
#define LCDCMD_DISPON       (0x59)
#define LCDCMD_DISPOFF      (0x58)
#define LCDCMD_SCROLL       (0x44)
#define LCDCMD_CSRFORM      (0x5D)
#define LCDCMD_CSRDIR       (0x4C)
#define LCDCMD_OVLAY        (0x5B)
#define LCDCMD_CGRAMADR     (0x5C)
#define LCDCMD_HDOTSCR      (0x5A)
#define LCDCMD_CSRW         (0x46)
#define LCDCMD_CSRR         (0x47)
#define LCDCMD_GRAYSCALE    (0x60)
#define LCDCMD_MEMWRITE     (0x42)
#define LCDCMD_MEMREAD      (0x43)

#endif
