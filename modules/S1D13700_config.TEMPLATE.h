#ifndef S1D13700_CONFIG_H
#define S1D13700_CONFIG_H

//==================================================================================================
// S1D13700 Config
//
// Configuration for: PROJECT_NAME
//==================================================================================================

//--------------------------------------------------------------------------------------------------
// Port Configuration (Edit uC Ports and pins)
//--------------------------------------------------------------------------------------------------
#define LCDCTL_IN       P1IN
#define LCDCTL_OUT      P1OUT
#define LCDCTL_DIR      P1DIR
#define LCDCTL_SEL      P1SEL
    #define LCD_RST     BIT0
    #define LCD_RD      BIT1
    #define LCD_WR      BIT2
    #define LCD_CS      BIT3
    #define LCD_A0        BIT4

#define LCDD_IN         P4IN
#define LCDD_OUT        P4OUT
#define LCDD_DIR        P4DIR
#define LCDD_SEL        P4SEL
//--------------------------------------------------------------------------------------------------
// LCD Configuration (Edit LCD configuration)
//--------------------------------------------------------------------------------------------------
#define LCD_RESX      320
#define LCD_RESY      240
#define LCD_CHARW     6 // Character block width
#define LCD_CLK_FREQ  40000000
#define LCD_CLK_DIV   16 //4, 8, 16
#define LCD_CANVASX   0x0030 // Virtual screen width (bytes) Try to pick a nice number
#define LCD_PORTRAIT  0 // enables software-end portrait mode. NOTE: in portrait mode, X and Y's of
                        // above parameters are swapped (RESX <-> RESY, CANVASX -> Y)
          // LCD_PORTRAIT = 0 -> Landscape Mode (Normal)
          // LCD_PORTRAIT = 1 -> Portrait Mode (Rotated)


#endif