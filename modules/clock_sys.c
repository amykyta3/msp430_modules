/*
* Copyright (c) 2013, Alexander I. Mykyta
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
* Alex M.       2011-09-19   born
* Alex M.       2013-11-06   Added support for 1xx devices
* 
*=================================================================================================*/

/**
* \addtogroup MOD_CLOCKSYS
* \{
**/

/**
* \file
* \brief Code for \ref MOD_CLOCKSYS "Clock System"
* \author Alex Mykyta 
* 
**/

///\}

#include <stdint.h>
#include <msp430_xc.h>
#include "clock_sys.h"

//##################################################################################################
//# Basic Clock Module                                                                             #
//# 1xx Devices                                                                                    #
//##################################################################################################
#ifdef __MSP430_HAS_BASIC_CLOCK__

void clock_init(void){
    
    #ifdef _USING_XT1
        // Setup XT1
        #if(XT1_FREQ == 32768)
            // LF Mode
            BCSCTL1 &= ~XTS;
        #else
            // HF Mode
            BCSCTL1 |= XTS;
        #endif
    #endif
    
    #ifdef _USING_XT2
        BCSCTL1 &= ~XT2OFF;
    #else
        BCSCTL1 |= XT2OFF;
    #endif
    
    #ifdef _USING_DCO
        DCOCTL = (DCO_DCO<<5) | DCO_MOD;
        BCSCTL1 &= ~0x07;
        BCSCTL1 |= DCO_RSEL;
    #endif
    
    
    #if (MCLK_SRC == 0) // DCO
        BCSCTL2 &= ~SELM_3;
    #elif (MCLK_SRC == 1) // XT1
        BCSCTL2 |= SELM_3;
    #elif (MCLK_SRC == 2) // XT2
        BCSCTL2 &= ~SELM_3;
        BCSCTL2 |= SELM_2;
    #else
        #error "Invalid MCLK_SRC"
    #endif
    
    #if (SMCLK_SRC == 0) // DCO
        BCSCTL2 &= ~SELS;
    #elif (SMCLK_SRC == 2) // XT2
        BCSCTL2 |= SELS;
    #else
        #error "Invalid SMCLK_SRC"
    #endif
    
    #if(MCLK_DIV > 3)
        #error "Invalid MCLK_DIV"
    #endif
    
    #if(SMCLK_DIV > 3)
        #error "Invalid SMCLK_DIV"
    #endif
    
    #if(ACLK_DIV > 3)
        #error "Invalid ACLK_DIV"
    #endif
    
    BCSCTL2 &= ~(DIVM_3 | DIVS_3);
    BCSCTL2 |= (MCLK_DIV<<4) | (SMCLK_DIV<<1);
    
    BCSCTL1 &= ~DIVA_3;
    BCSCTL1 |= (ACLK_DIV<<4);
    
}

#endif

//##################################################################################################
//# Basic Clock Module+                                                                            #
//# 2xx Devices                                                                                    #
//##################################################################################################
#ifdef __MSP430_HAS_BC2__
    
#endif

//##################################################################################################
//# FLL+ Clock Module                                                                              #
//# 4xx Devices                                                                                    #
//##################################################################################################
#ifdef __MSP430_HAS_FLLPLUS__

void clock_init(void){
    
    #ifdef _USING_XT1
        // Setup XT1
        #if(XT1_FREQ == 32768)
            // LF Mode
            FLL_CTL0 &= ~(XTS_FLL | OSCCAP_3);
            #if(LFXT_LOAD_CAP < 4)
                FLL_CTL0 |= OSCCAP_0;
            #elif(LFXT_LOAD_CAP < 7)
                FLL_CTL0 |= OSCCAP_1;
            #elif(LFXT_LOAD_CAP < 9)
                FLL_CTL0 |= OSCCAP_2;
            #else
                FLL_CTL0 |= OSCCAP_3;
            #endif
            
            // Wait for xtal to stabilize
            while (IFG1 & OFIFG){
                
                IFG1 &= ~OFIFG;
                __delay_cycles(0x4800);
            }
            
            
        #else
            // HF Mode
            FLL_CTL0 |= XTS_FLL;
            FLL_CTL0 &= ~(XCAP11PF);
        #endif
        
        #ifdef LF1XT1S_2
            // route to XT1 if there is a choice to do so.
            FLL_CTL2 = LF1XT1S_0;
        #endif
        
    #endif
    
    #ifdef _USING_VLO
        // Setup VLO
        FLL_CTL0 &= ~(XTS_FLL | XCAP11PF);
        FLL_CTL2 = LF1XT1S_2;
        
    #endif
    
    #ifdef _USING_XT2
        
        #ifdef XT2S_0
            // XT2S must be configured
            #if((XT2_FREQ >= 400000) && (XT2_FREQ < 1000000))
                FLL_CTL2 = XT2S_0;
            #elif((XT2_FREQ >= 1000000) && (XT2_FREQ < 3000000))
                FLL_CTL2 = XT2S_1;
            #elif((XT2_FREQ >= 3000000) && (XT2_FREQ < 16000000))
                FLL_CTL2 = XT2S_2;
            #else
                #error XT2 is outside its frequency range
            #endif
        
        #endif
        
        FLL_CTL1 &= ~XT2OFF;                      // Clear bit = HFXT2 on

    #endif
    
    #ifdef _USING_DCO
        // Setup DCO
        
        //SCFQCTL = SCFQ_M | FLL_N;
        SCFQCTL = FLL_N;
        SCFI0 = FLL_FLLD | FLL_FN;
        SCFI1 = 0;
        
        FLL_CTL0 |= DCOPLUS;
        
        
        // Wait for DCO to settle
        __delay_cycles(0x1C00);
        IFG1 &= ~OFIFG;
        
    #endif
    
    #ifdef SELM0
        #if (MCLK_SRC == 0) // DCO
            FLL_CTL1 &= ~(SELM0 | SELM1);
        #elif (MCLK_SRC == 1) // XT1
            FLL_CTL1 |= SELM0 | SELM1;
        #elif (MCLK_SRC == 2) // XT2
            FLL_CTL1 |= SELM1;
            FLL_CTL1 &= ~SELM0;
        #elif (MCLK_SRC == 3) // VLO
            FLL_CTL1 |= SELM0 | SELM1;
        #else
            #error "Invalid MCLK_SRC"
        #endif
    #endif
    
    #ifdef XT2OFF
        #if (SMCLK_SRC == 0) // DCO
            FLL_CTL1 &= ~SELS;
        #elif (SMCLK_SRC == 2) // XT2
            FLL_CTL1 |= SELS;
        #else
            #error "Invalid SMCLK_SRC"
        #endif
    #endif
    
}

#endif

//##################################################################################################
//# Unified Clock System (UCS)                                                                     #
//# 5xx and 6xx Devices                                                                            #
//##################################################################################################
#ifdef __MSP430_HAS_UCS__

#define PMM_STATUS_OK     0
#define PMM_STATUS_ERROR  1

/*******************************************************************************
 * \brief   Increase Vcore by one level
 *
 * \param level     Level to which Vcore needs to be increased
 * \return status   Success/failure
 ******************************************************************************/
static uint16_t SetVCoreUp(uint8_t level){
    uint16_t PMMRIE_backup, SVSMHCTL_backup, SVSMLCTL_backup;

    // The code flow for increasing the Vcore has been altered to work around
    // the erratum FLASH37. 
    // Please refer to the Errata sheet to know if a specific device is affected
    // DO NOT ALTER THIS FUNCTION

    // Open PMM registers for write access   
    PMMCTL0_H = 0xA5;

    // Disable dedicated Interrupts
    // Backup all registers
    PMMRIE_backup = PMMRIE;
    PMMRIE &= ~(SVMHVLRPE | SVSHPE | SVMLVLRPE | SVSLPE | SVMHVLRIE |
                SVMHIE | SVSMHDLYIE | SVMLVLRIE | SVMLIE | SVSMLDLYIE );
    SVSMHCTL_backup = SVSMHCTL;
    SVSMLCTL_backup = SVSMLCTL;

    // Clear flags
    PMMIFG = 0;

    // Set SVM highside to new level and check if a VCore increase is possible
    SVSMHCTL = SVMHE | SVSHE | (SVSMHRRL0 * level);    

    // Wait until SVM highside is settled
    while ((PMMIFG & SVSMHDLYIFG) == 0); 

    // Clear flag
    PMMIFG &= ~SVSMHDLYIFG;

    // Check if a VCore increase is possible
    if ((PMMIFG & SVMHIFG) == SVMHIFG) {      // -> Vcc is too low for a Vcore increase
        // recover the previous settings
        PMMIFG &= ~SVSMHDLYIFG;
        SVSMHCTL = SVSMHCTL_backup;

        // Wait until SVM highside is settled
        while ((PMMIFG & SVSMHDLYIFG) == 0);

        // Clear all Flags
        PMMIFG &= ~(SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG | SVMLVLRIFG | SVMLIFG | SVSMLDLYIFG);

        PMMRIE = PMMRIE_backup;                 // Restore PMM interrupt enable register
        PMMCTL0_H = 0x00;                       // Lock PMM registers for write access
        return PMM_STATUS_ERROR;                // return: voltage not set
    }

    // Set also SVS highside to new level	    
    // Vcc is high enough for a Vcore increase
    SVSMHCTL |= (SVSHRVL0 * level);

    // Wait until SVM highside is settled
    while ((PMMIFG & SVSMHDLYIFG) == 0);    

    // Clear flag
    PMMIFG &= ~SVSMHDLYIFG;

    // Set VCore to new level
    PMMCTL0_L = PMMCOREV0 * level;

    // Set SVM, SVS low side to new level
    SVSMLCTL = SVMLE | (SVSMLRRL0 * level) | SVSLE | (SVSLRVL0 * level);

    // Wait until SVM, SVS low side is settled
    while ((PMMIFG & SVSMLDLYIFG) == 0);

    // Clear flag
    PMMIFG &= ~SVSMLDLYIFG;
    // SVS, SVM core and high side are now set to protect for the new core level

    // Restore Low side settings
    // Clear all other bits _except_ level settings
    SVSMLCTL &= (SVSLRVL0+SVSLRVL1+SVSMLRRL0+SVSMLRRL1+SVSMLRRL2);

    // Clear level settings in the backup register,keep all other bits
    SVSMLCTL_backup &= ~(SVSLRVL0+SVSLRVL1+SVSMLRRL0+SVSMLRRL1+SVSMLRRL2);

    // Restore low-side SVS monitor settings
    SVSMLCTL |= SVSMLCTL_backup;

    // Restore High side settings
    // Clear all other bits except level settings
    SVSMHCTL &= (SVSHRVL0+SVSHRVL1+SVSMHRRL0+SVSMHRRL1+SVSMHRRL2);

    // Clear level settings in the backup register,keep all other bits
    SVSMHCTL_backup &= ~(SVSHRVL0+SVSHRVL1+SVSMHRRL0+SVSMHRRL1+SVSMHRRL2);

    // Restore backup 
    SVSMHCTL |= SVSMHCTL_backup;

    // Wait until high side, low side settled
    while (((PMMIFG & SVSMLDLYIFG) == 0) && ((PMMIFG & SVSMHDLYIFG) == 0));

    // Clear all Flags
    PMMIFG &= ~(SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG | SVMLVLRIFG | SVMLIFG | SVSMLDLYIFG);

    PMMRIE = PMMRIE_backup;                   // Restore PMM interrupt enable register
    PMMCTL0_H = 0x00;                         // Lock PMM registers for write access

    return PMM_STATUS_OK;  
}

/*******************************************************************************
 * \brief  Decrease Vcore by one level
 *
 * \param  level    Level to which Vcore needs to be decreased
 * \return status   Success/failure
 ******************************************************************************/
static uint16_t SetVCoreDown(uint8_t level){
    uint16_t PMMRIE_backup, SVSMHCTL_backup, SVSMLCTL_backup;

    // The code flow for decreasing the Vcore has been altered to work around
    // the erratum FLASH37. 
    // Please refer to the Errata sheet to know if a specific device is affected
    // DO NOT ALTER THIS FUNCTION

    // Open PMM registers for write access
    PMMCTL0_H = 0xA5;

    // Disable dedicated Interrupts 
    // Backup all registers
    PMMRIE_backup = PMMRIE;
    PMMRIE &= ~(SVMHVLRPE | SVSHPE | SVMLVLRPE | SVSLPE | SVMHVLRIE |
        SVMHIE | SVSMHDLYIE | SVMLVLRIE | SVMLIE | SVSMLDLYIE );
    SVSMHCTL_backup = SVSMHCTL;
    SVSMLCTL_backup = SVSMLCTL;

    // Clear flags
    PMMIFG &= ~(SVMHIFG | SVSMHDLYIFG | SVMLIFG | SVSMLDLYIFG);

    // Set SVM, SVS high & low side to new settings in normal mode
    SVSMHCTL = SVMHE | (SVSMHRRL0 * level) | SVSHE | (SVSHRVL0 * level);
    SVSMLCTL = SVMLE | (SVSMLRRL0 * level) | SVSLE | (SVSLRVL0 * level);

    // Wait until SVM high side and SVM low side is settled
    while ((PMMIFG & SVSMHDLYIFG) == 0 || (PMMIFG & SVSMLDLYIFG) == 0);

    // Clear flags
    PMMIFG &= ~(SVSMHDLYIFG + SVSMLDLYIFG);
    // SVS, SVM core and high side are now set to protect for the new core level

    // Set VCore to new level
    PMMCTL0_L = PMMCOREV0 * level;

    // Restore Low side settings
    // Clear all other bits _except_ level settings
    SVSMLCTL &= (SVSLRVL0+SVSLRVL1+SVSMLRRL0+SVSMLRRL1+SVSMLRRL2);

    // Clear level settings in the backup register,keep all other bits
    SVSMLCTL_backup &= ~(SVSLRVL0+SVSLRVL1+SVSMLRRL0+SVSMLRRL1+SVSMLRRL2);

    // Restore low-side SVS monitor settings
    SVSMLCTL |= SVSMLCTL_backup;

    // Restore High side settings
    // Clear all other bits except level settings
    SVSMHCTL &= (SVSHRVL0+SVSHRVL1+SVSMHRRL0+SVSMHRRL1+SVSMHRRL2);

    // Clear level settings in the backup register, keep all other bits
    SVSMHCTL_backup &= ~(SVSHRVL0+SVSHRVL1+SVSMHRRL0+SVSMHRRL1+SVSMHRRL2);

    // Restore backup 
    SVSMHCTL |= SVSMHCTL_backup;

    // Wait until high side, low side settled
    while (((PMMIFG & SVSMLDLYIFG) == 0) && ((PMMIFG & SVSMHDLYIFG) == 0));	

    // Clear all Flags
    PMMIFG &= ~(SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG | SVMLVLRIFG | SVMLIFG | SVSMLDLYIFG);

    PMMRIE = PMMRIE_backup;                   // Restore PMM interrupt enable register
    PMMCTL0_H = 0x00;                         // Lock PMM registers for write access
    return PMM_STATUS_OK;		                // Return: OK
}

static uint16_t SetVCore(uint8_t level){
    uint16_t actlevel;
    uint16_t status = 0;

    level &= PMMCOREV_3;                       // Set Mask for Max. level
    actlevel = (PMMCTL0 & PMMCOREV_3);         // Get actual VCore
                                               // step by step increase or decrease
    while (((level != actlevel) && (status == 0)) || (level < actlevel)) {
        if (level > actlevel) {
            status = SetVCoreUp(++actlevel);
        }else{
            status = SetVCoreDown(--actlevel);
        }
    }
    return status;
}

RES_t clock_SetDivMCLK(uint8_t div){
    register uint16_t tmp;
    
    // check if within voltage range
    #if (MCLK_DIV_MINIMUM_RESTRICT == 0)
        if(div > 5){
            return(RES_PARAMERR);
        }
    #else
        if((div < MCLK_DIV_MINIMUM_RESTRICT)||(div > 5)){
            return(RES_PARAMERR);
        }
    #endif
    
    
    tmp = UCSCTL5;
    tmp &= ~DIVM_7;
    tmp |= div;
    UCSCTL5 = tmp;

    return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
void clock_init(void){
    // Initializes Clock System for devices with UCS
    
    // Set VCore to minimum allowed
    #if(_MAX_SYS_FREQ < 8000000L)
        SetVCore(0);
    #elif(_MAX_SYS_FREQ < 12000000L)
        SetVCore(1);
    #elif(_MAX_SYS_FREQ < 20000000L)
        SetVCore(2);
    #else
        SetVCore(3);
    #endif

    // Start XT1
    #ifdef _USING_XT1
        #if (XT1_FREQ < 40000L)
            // LF Mode
            UCSCTL6_L = XT1DRIVE1_L + XT1DRIVE0_L + XT1_XCAP;
        #else
            // HF Mode
            UCSCTL6_L = XT1DRIVE1_L + XT1DRIVE0_L + XTS;
        #endif
    #endif
    
    // Start XT2
    #ifdef _USING_XT2
        UCSCTL6_H = XT2DRIVE1_H + XT2DRIVE0_H;
    #endif
    
    // Let crystals settle
    #ifdef _USING_XT1
        #if (XT1_FREQ < 40000L)
            // LFXT
            while(UCSCTL7 & XT1LFOFFG){ // loop until XT1LF fault clears
                UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT1HFOFFG+XT2OFFG);// Clear OSC fault Flags fault flags
                SFRIFG1 &= ~OFIFG;        // Clear OFIFG fault flag
            }
            UCSCTL6_L &= ~(XT1DRIVE1_L + XT1DRIVE0_L);
        #else
            //XT1 HF
            while(UCSCTL7 & XT1HFOFFG){ // loop until XT1HF fault clears
                UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT1HFOFFG+XT2OFFG); // Clear OSC fault Flags
                SFRIFG1 &= ~OFIFG;        // Clear OFIFG fault flag
            }
            
            #if (XT1_FREQ < 8000000L)
                UCSCTL6 &= ~((~XT1DRIVE_0) & (XT1DRIVE_3));
            #elif (XT1_FREQ < 16000000L)
                UCSCTL6 &= ~((~XT1DRIVE_1) & (XT1DRIVE_3));
            #elif (XT1_FREQ < 24000000L)
                UCSCTL6 &= ~((~XT1DRIVE_2) & (XT1DRIVE_3));
            #endif
        #endif
    #endif
    
    #ifdef _USING_XT2
        while(UCSCTL7 & XT2OFFG){ // loop until XT2 fault clears
            UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT1HFOFFG+XT2OFFG); // Clear OSC fault Flags
            SFRIFG1 &= ~OFIFG;                                // Clear OFIFG fault flag
        }
        #if (XT2_FREQ < 8000000L)
            UCSCTL6 &= ~((~XT2DRIVE_0) & (XT2DRIVE_3));
        #elif (XT2_FREQ < 16000000L)
            UCSCTL6 &= ~((~XT2DRIVE_1) & (XT2DRIVE_3));
        #elif (XT2_FREQ < 24000000L)
            UCSCTL6 &= ~((~XT2DRIVE_2) & (XT2DRIVE_3));
        #endif
    #endif
    
    
    // Setup DCO
    #ifdef _USING_DCO
        UCSCTL3 = FLL_SELREF + FLL_FLLREFDIV;
        
        UCSCTL0 = 0x0000;    // Set DCO to lowest Tap
        UCSCTL2 = (FLL_FLLD << 12) + FLL_FLLN;
        
        #if (DCO_FREQ <= 630000L)
            UCSCTL1 = DCORSEL_0;
        #elif (DCO_FREQ < 1250000L)
            UCSCTL1 = DCORSEL_1;
        #elif (DCO_FREQ < 2500000L)
            UCSCTL1 = DCORSEL_2;
        #elif (DCO_FREQ < 5000000L)
            UCSCTL1 = DCORSEL_3;
        #elif (DCO_FREQ < 10000000L)
            UCSCTL1 = DCORSEL_4;
        #elif (DCO_FREQ < 20000000L)
            UCSCTL1 = DCORSEL_5;
        #elif (DCO_FREQ < 40000000L)
            UCSCTL1 = DCORSEL_6;
        #else
            UCSCTL1 = DCORSEL_7;
        #endif
        
        
        while(UCSCTL7 & DCOFFG){ // loop until DCO fault clears
            UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT1HFOFFG+XT2OFFG); // Clear OSC fault Flags
            SFRIFG1 &= ~OFIFG;                                // Clear OFIFG fault flag
        }
    #endif
    
    // Setup subsystem clock division
    UCSCTL5 = (ACLK_DIV << 8) + (SMCLK_DIV << 4) + DIVM__16;
    
    // Clock Routing
    UCSCTL4 = _SELA + _SELS + _SELM;
    
    // Set MCLK startup value
    UCSCTL5 = (ACLK_DIV << 8) + (SMCLK_DIV << 4) + MCLK_DIV;
}
#endif

//##################################################################################################

