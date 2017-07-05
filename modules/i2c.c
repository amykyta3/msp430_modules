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
* Alex M.       2013-05-19   born
* Alex M.       2013-08-07   Fixed single-byte read bug
* 
*=================================================================================================*/

/**
* \addtogroup MOD_I2C
* \{
**/

/**
* \file
* \brief Code for \ref MOD_I2C
* \author Alex Mykyta 
* 
**/

#include <stdint.h>
#include <stdbool.h>

#include <msp430_xc.h>
#include <atomic.h>

#include "i2c.h"

#include "i2c_internal.h"

//--------------------------------------------------------------------------------------------------
typedef enum{
    SM_SEND_ADDR,
    SM_WRITE_DATA,
    SM_SEND_RESTART,
    SM_READ_DATA,
    SM_DONE
}i2c_state_t;

static struct{
    i2c_package_t    *pkg;
    uint16_t         idx;
    void (*callback)(i2c_status_t result);
    i2c_status_t     status;
    i2c_state_t      next_state;
} transfer;

//--------------------------------------------------------------------------------------------------
ISR(I2C_ISR){
    if(I2C_IV == USCI_I2C_UCNACKIFG){
        I2C_IFG = 0;
        I2C_IE = 0;
        transfer.status = I2C_FAILED;
        I2C_CTL1 |= UCTXSTP; // set stop condition
        if(transfer.callback){
            transfer.callback(I2C_FAILED);
        }
        
        return;
    }
    
    I2C_IFG = 0;
    
    switch(transfer.next_state){
        case SM_SEND_ADDR: // (TX Register address)
            // arrive here once STT has begun. Safe to write to TXBUF
            
            I2C_TXBUF = transfer.pkg->addr[transfer.idx];
            transfer.idx++;
            if(transfer.idx == transfer.pkg->addr_len){
                // that was the last address byte.
                
                // update next state
                if(transfer.pkg->data_len != 0){
                    transfer.idx = 0;
                    if(transfer.pkg->read){
                        transfer.next_state = SM_SEND_RESTART;
                    }else{
                        transfer.next_state = SM_WRITE_DATA;
                    }
                }else{
                    transfer.next_state = SM_DONE;
                }
            }
            break;
        case SM_WRITE_DATA:
            I2C_TXBUF = transfer.pkg->data[transfer.idx];
            transfer.idx++;
            if(transfer.idx == transfer.pkg->data_len){
                // that was the last data byte to send.
                
                // update next state
                transfer.next_state = SM_DONE;
            }
            
            break;
        case SM_SEND_RESTART:
            I2C_CTL1 &= ~UCTR;    // Set to receiver mode
            I2C_CTL1 |= UCTXSTT; // write (re)start condition
            
            if(transfer.pkg->data_len == 1){
                // wait for STT bit to drop
                while(I2C_CTL1 & UCTXSTT);
                I2C_CTL1 |= UCTXSTP; // schedule stop condition
            }
            // update next state
            transfer.next_state = SM_READ_DATA;
            break;
        case SM_READ_DATA:
            transfer.pkg->data[transfer.idx] = I2C_RXBUF;
            transfer.idx++;
            if(transfer.idx == transfer.pkg->data_len - 1){
                // next incoming byte is the last one.
                I2C_CTL1 |= UCTXSTP; // schedule stop condition
                break;
            }else if(transfer.idx < transfer.pkg->data_len){
                // still more to recv.
                break;
            }
            // otherwise, that was the last data byte to recv.
            // fall through
        case SM_DONE:
            I2C_IE = 0;
            if(transfer.pkg->read == false){
                // If finished a write, schedule a stop condition
                I2C_CTL1 |= UCTXSTP;
            }
            transfer.status = I2C_IDLE;
            if(transfer.callback){
                transfer.callback(I2C_IDLE);
            }
            break;
    }
}

//--------------------------------------------------------------------------------------------------
void i2c_init(void){
    
    I2C_CTL0 |= UCSWRST;    
    //Initialize all USCI registers with UCSWRST = 1 (including UCxCTL1).
    I2C_CTL0 = UCMST|UCMODE_3|UCSYNC;
    I2C_CTL1 = UCTR|UCSWRST|(I2C_CLK_SRC << 6);
    I2C_BR = I2C_CLK_DIV;
    
    I2C_CTL1 &= ~UCSWRST; // Clear reset
    
    transfer.status = I2C_IDLE;
}

//--------------------------------------------------------------------------------------------------
void i2c_uninit(void){
    I2C_CTL0 = UCSYNC; 
    I2C_CTL1 = UCSWRST;
    I2C_SA = 0;
    I2C_IE = 0;
}

//--------------------------------------------------------------------------------------------------
void i2c_transfer_start(const i2c_package_t *pkg, void (*callback)(i2c_status_t result)){
    
    if(i2c_transfer_status() == I2C_BUSY) return;
    
    transfer.pkg = (i2c_package_t*)pkg;
    transfer.idx = 0;
    transfer.callback = callback;
    transfer.status = I2C_BUSY;
    
    // update next state
    if(pkg->addr_len != 0){
        transfer.next_state = SM_SEND_ADDR;
    }else if(pkg->data_len != 0){
        if(pkg->read){
            transfer.next_state = SM_SEND_RESTART;
        }else{
            transfer.next_state = SM_WRITE_DATA;
        }
    }else{
        transfer.next_state = SM_DONE;
    }
    
    I2C_IFG = 0;
    I2C_IE = UCNACKIE | UCTXIE | UCRXIE;
    I2C_SA = pkg->slave_addr;
    I2C_CTL1 |= UCTR; // Set to transmitter mode
    I2C_CTL1 |= UCTXSTT; // start condition (sends slave address)
    
}

//--------------------------------------------------------------------------------------------------
i2c_status_t i2c_transfer_status(void){
    i2c_status_t status;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        status = transfer.status;
    }
    
    return(status);
}

///\}
