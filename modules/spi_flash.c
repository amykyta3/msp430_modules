/*
* Copyright (c) 2014, Alexander I. Mykyta
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
* Alex M.       2014-05-20   Rewrote & merged original SST25VF and FlashSPAN modules
* 
*=================================================================================================*/

/**
* \addtogroup MOD_SPI_FLASH
* \{
**/

/**
* \file
* \brief Code for \ref MOD_SPI_FLASH
* \author Alex Mykyta
**/

#include <stdint.h>
#include <stdbool.h>
#include <result.h>
#include <msp430_xc.h>
#include "spi_flash.h"
#include "spi.h"

#include <spi_flash_config.h>
#include <delay.h>
//==================================================================================================
// Constant Definitions
//==================================================================================================
// Status Register Bits
#define SR_BUSY    0x01
#define SR_WEL     0x02
#define SR_BP0     0x04
#define SR_BP1     0x08
#define SR_BP2     0x10
#define SR_BP3     0x20

// Commands
#define CMD_RD          0x03
#define CMD_ERASE4k     0x20
#define CMD_ERASE32k    0x52
#define CMD_ERASE64k    0xD8
#define CMD_CHIPERASE   0x60
#define CMD_WRAAI       0xAD
#define CMD_RDSR        0x05
#define CMD_WRSR        0x01
#define CMD_WREN        0x06
#define CMD_WRDI        0x04
#define CMD_JEDECID     0x9F
#define CMD_EBSY        0x70
#define CMD_DBSY        0x80
#define CMD_EN4B        0xB7
#define CMD_EX4B        0xE9
#define CMD_DP          0xB9
#define CMD_RDP         0xAB

#if(DEVICE_SIZE > 0xFFFFFFL)
    // Requires 4-byte address mode
    #define ADDR_BYTES  4
#else
    #define ADDR_BYTES  3
#endif

#if(DEVICE_COUNT > 1)
    #define DEVICE_IDX(addr)    (addr / DEVICE_SIZE)
    #define PHYSICAL_ADDR(addr) (addr % DEVICE_SIZE)
#else
    #define PHYSICAL_ADDR(addr) (addr)
#endif

//==================================================================================================

#if(DEVICE_COUNT > 1)
    static uint8_t current_device;
    static const uint8_t CE_MAP[] = CE_DEV_BITMAP;
#endif

//--------------------------------------------------------------------------------------------------
static void send_address(uint32_t address){
    #if(ADDR_BYTES == 4)
        uint8_t addrbuf[4];
        addrbuf[3] = address & 0xFF;
        address >>= 8;
        addrbuf[2] = address & 0xFF;
        address >>= 8;
        addrbuf[1] = address & 0xFF;
        address >>= 8;
        addrbuf[0] = address & 0xFF;
    #else
        uint8_t addrbuf[3];
        addrbuf[2] = address & 0xFF;
        address >>= 8;
        addrbuf[1] = address & 0xFF;
        address >>= 8;
        addrbuf[0] = address & 0xFF;
    #endif
    spi_tx_frame(addrbuf,sizeof(addrbuf));
}

//--------------------------------------------------------------------------------------------------
static void set_CE(void){
    #if(DEVICE_COUNT == 1)
        CE_POUT &= ~CE_DEV_BITMASK;
    #else
        CE_POUT &= ~CE_MAP[current_device];
    #endif
}

//--------------------------------------------------------------------------------------------------
static void clear_CE(void){
    CE_POUT |= CE_DEV_BITMASK;
}

//--------------------------------------------------------------------------------------------------
static uint32_t cmd_JEDECID(void){
    uint8_t id_arr[3];
    uint32_t id;
    
    set_CE();
    spi_transfer_byte(CMD_JEDECID);
    spi_rx_frame(id_arr, 3);
    clear_CE();
    
    id = (((uint32_t)id_arr[0]) << 16) | (((uint32_t)id_arr[1]) << 8) | id_arr[2];
    
    return(id);
}

//--------------------------------------------------------------------------------------------------
static void simple_cmd(uint8_t opcode){
    set_CE();
    spi_transfer_byte(opcode);
    clear_CE();
}

//--------------------------------------------------------------------------------------------------
static void stall_busy(void){
    set_CE();
    while((MISO_IN_PORT & MISO_IN_PIN) == 0);
    clear_CE();
}

//--------------------------------------------------------------------------------------------------
// nbytes is any nonzero number, operation spans exactly one device
static void device_read(uint32_t phy_address, void *dst, uint16_t nBytes){
    set_CE();
    spi_transfer_byte(CMD_RD);
    send_address(phy_address);
    spi_rx_frame(dst, nBytes);
    clear_CE();
}

//--------------------------------------------------------------------------------------------------
// nbytes is any nonzero number, operation spans exactly one device
static void device_write(uint32_t phy_address, const void *src, uint16_t nBytes){
    
    simple_cmd(CMD_EBSY);
    simple_cmd(CMD_WREN);
    
    // Send first pair of bytes
    set_CE();
    spi_transfer_byte(CMD_WRAAI);
    if(phy_address & 0x00000001L){
        const uint8_t *bsrc = src;
        // odd start address
        send_address(phy_address & 0xFFFFFFFEL);
        spi_transfer_byte(0xFF);
        spi_transfer_byte(*bsrc);
        src++;
        nBytes--;
    }else{
        send_address(phy_address);
        if(nBytes == 1){
            const uint8_t *bsrc = src;
            spi_transfer_byte(*bsrc);
            spi_transfer_byte(0xFF);
            nBytes = 0;
        }else{
            spi_tx_frame(src,2);
            src += 2;
            nBytes -= 2;
        }
    }
    clear_CE();
    stall_busy();
    
    // while more pairs can be written...
    while(nBytes >= 2){
        set_CE();
        spi_transfer_byte(CMD_WRAAI);
        spi_tx_frame(src,2);
        clear_CE();
        stall_busy();
        src += 2;
        nBytes -= 2;
    }
    
    // if an odd byte remains
    if(nBytes){
        const uint8_t *bsrc = src;
        set_CE();
        spi_transfer_byte(CMD_WRAAI);
        spi_transfer_byte(*bsrc);
        spi_transfer_byte(0xFF);
        clear_CE();
        stall_busy();
    }
    
    // Close write operation
    simple_cmd(CMD_WRDI);
    stall_busy();
    simple_cmd(CMD_DBSY);
}

//--------------------------------------------------------------------------------------------------
RES_t spi_flash_init(void){
    
    // make sure MISO pin is input
    MISO_DIR_PORT &= ~MISO_IN_PIN;
    
    // Init SPI
    spi_init(SPI_MODE0);
    
    #if(DEVICE_HAS_DEEP_POWER_DOWN)
        // Release from deep power down
        #if(DEVICE_COUNT > 1)
            for(current_device=0; current_device<DEVICE_COUNT; current_device++){
                simple_cmd(CMD_RDP);
            }
        #else
            simple_cmd(CMD_RDP);
        #endif
        
        // Requires at least 100 us delay before issuing any other command
        _delay_us(100);
    #endif
    
    #if(DEVICE_COUNT > 1)
        for(current_device=0; current_device<DEVICE_COUNT; current_device++){
            // disable busy signaling
            simple_cmd(CMD_DBSY);
            
            // check IDs
            if(cmd_JEDECID() != DEVICE_ID) return(RES_FAIL);
        
            #if(ADDR_BYTES == 4)
                // Set to 4-byte address mode
                simple_cmd(CMD_EN4B);
            #endif
            
            // Clear write protect bits
            set_CE();
            spi_transfer_byte(CMD_WRSR);
            spi_transfer_byte(0x00);
            clear_CE();
        }
    #else
        // disable busy signaling
        simple_cmd(CMD_DBSY);
        
        // check IDs
        if(cmd_JEDECID() != DEVICE_ID) return(RES_FAIL);
    
        #if(ADDR_BYTES == 4)
            // Set to 4-byte address mode
            simple_cmd(CMD_EN4B);
        #endif
        
        // Clear write protect bits
        simple_cmd(CMD_WREN);
        set_CE();
        spi_transfer_byte(CMD_WRSR);
        spi_transfer_byte(0x00);
        clear_CE();
        
    #endif
    
    return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
void spi_flash_uninit(void){
    
    #if(DEVICE_HAS_DEEP_POWER_DOWN)
        // Deep powerdown
        #if(DEVICE_COUNT > 1)
            for(current_device=0; current_device<DEVICE_COUNT; current_device++){
                simple_cmd(CMD_DP);
            }
        #else
            simple_cmd(CMD_DP);
        #endif
    #endif
}

//--------------------------------------------------------------------------------------------------
RES_t spi_flash_read(uint32_t address, void *dst, uint16_t nBytes){
    #if(DEVICE_COUNT > 1)
        if((address + nBytes) > (DEVICE_SIZE * DEVICE_COUNT)) return(RES_PARAMERR);
        
        current_device = DEVICE_IDX(address);
        address = PHYSICAL_ADDR(address);
        while(nBytes){
            uint32_t n_remaining;
            n_remaining = DEVICE_SIZE - address;
            
            if(nBytes < n_remaining){
                // operation fits within current device
                device_read(address, dst, nBytes);
                break;
            }else{
                // will overflow to the next device
                device_read(address, dst, n_remaining);
                
                nBytes -= n_remaining;
                dst += n_remaining;
                address = 0;
                current_device++;
            }
        }
    #else
        if((address + nBytes) > DEVICE_SIZE) return(RES_PARAMERR);
        device_read(address, dst, nBytes);
    #endif
    
    return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
RES_t spi_flash_write(uint32_t address, const void *src, uint16_t nBytes){
    #if(DEVICE_COUNT > 1)
        if((address + nBytes) > (DEVICE_SIZE * DEVICE_COUNT)) return(RES_PARAMERR);
        
        current_device = DEVICE_IDX(address);
        address = PHYSICAL_ADDR(address);
        while(nBytes){
            uint32_t n_remaining;
            n_remaining = DEVICE_SIZE - address;
            
            if(nBytes < n_remaining){
                // operation fits within current device
                device_write(address, src, nBytes);
                break;
            }else{
                // will overflow to the next device
                device_write(address, src, n_remaining);
                
                nBytes -= n_remaining;
                src += n_remaining;
                address = 0;
                current_device++;
            }
        }
    #else
        if((address + nBytes) > DEVICE_SIZE) return(RES_PARAMERR);
        device_write(address, src, nBytes);
    #endif
    
    return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
RES_t spi_flash_4k_erase(uint32_t address){
    #if(DEVICE_COUNT > 1)
        uint8_t dev = DEVICE_IDX(address);
        if(dev >= DEVICE_COUNT) return(RES_PARAMERR);
        current_device = dev;
    #else
        if(address >= DEVICE_SIZE) return(RES_PARAMERR);
    #endif
    
    simple_cmd(CMD_WREN);
    
    set_CE();
    spi_transfer_byte(CMD_ERASE4k);
    send_address(PHYSICAL_ADDR(address));
    clear_CE();
    
    return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
RES_t spi_flash_32k_erase(uint32_t address){
    #if(DEVICE_COUNT > 1)
        uint8_t dev = DEVICE_IDX(address);
        if(dev >= DEVICE_COUNT) return(RES_PARAMERR);
        current_device = dev;
    #else
        if(address >= DEVICE_SIZE) return(RES_PARAMERR);
    #endif
    
    simple_cmd(CMD_WREN);
    
    set_CE();
    spi_transfer_byte(CMD_ERASE32k);
    send_address(PHYSICAL_ADDR(address));
    clear_CE();
    
    return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
RES_t spi_flash_64k_erase(uint32_t address){
    #if(DEVICE_COUNT > 1)
        uint8_t dev = DEVICE_IDX(address);
        if(dev >= DEVICE_COUNT) return(RES_PARAMERR);
        current_device = dev;
    #else
        if(address >= DEVICE_SIZE) return(RES_PARAMERR);
    #endif
    
    simple_cmd(CMD_WREN);
    
    set_CE();
    spi_transfer_byte(CMD_ERASE64k);
    send_address(PHYSICAL_ADDR(address));
    clear_CE();
    
    return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
void spi_flash_bulk_erase(void){
    #if(DEVICE_COUNT > 1)
        for(current_device=0; current_device<DEVICE_COUNT; current_device++){
            simple_cmd(CMD_WREN);
            simple_cmd(CMD_CHIPERASE);
        }
    #else
        simple_cmd(CMD_WREN);
        simple_cmd(CMD_CHIPERASE);
    #endif
}

//--------------------------------------------------------------------------------------------------
bool spi_flash_erase_busy(void){
    uint8_t sr;
    
    #if(DEVICE_COUNT > 1)
        for(current_device=0; current_device<DEVICE_COUNT; current_device++){
            set_CE();
            spi_transfer_byte(CMD_RDSR);
            sr = spi_transfer_byte(0xFF);
            clear_CE();
            
            if(sr & SR_BUSY) return(true);
        }
    #else
        set_CE();
        spi_transfer_byte(CMD_RDSR);
        sr = spi_transfer_byte(0xFF);
        clear_CE();
        
        if(sr & SR_BUSY) return(true);
    #endif
    
    return(false);
}

//--------------------------------------------------------------------------------------------------
static uint8_t seq_wr_buf; // temporary storage for first byte in write burst
static uint8_t seq_wr_status; // bit0 indicates if buf is occupied. bit1 indicates if first write burst
void spi_flash_seq_wr_start(uint32_t address){
    #if(DEVICE_COUNT > 1)
        current_device = DEVICE_IDX(address);
        address = PHYSICAL_ADDR(address);
    #endif
    
    if(address & 0x00000001L){
        // odd address. Round down and fill the first byte
        address &= 0xFFFFFFFEL;
        seq_wr_buf = 0xFF;
        seq_wr_status = 0x03;
    }else{
        seq_wr_status = 0x02;
    }
    
    // prep write operation
    simple_cmd(CMD_EBSY);
    simple_cmd(CMD_WREN);
    
    // Initiate continuous write
    set_CE();
    spi_transfer_byte(CMD_WRAAI);
    send_address(address);
}

//--------------------------------------------------------------------------------------------------
void spi_flash_seq_wr(const void *src, uint16_t nBytes){
    const uint8_t *bsrc = src;
    while(nBytes){
        spi_flash_seq_wr_byte(*bsrc);
        bsrc++;
        nBytes--;
    }
}

//--------------------------------------------------------------------------------------------------
void spi_flash_seq_wr_byte(uint8_t b){
    if(seq_wr_status & 0x01){
        // buf is occupied. ready for a write burst
        if(seq_wr_status & 0x02){
            // first write burst
            // Transaction is already open so don't do anything
        }else{
            // Start a new burst transaction
            set_CE();
            spi_transfer_byte(CMD_WRAAI);
        }
        spi_transfer_byte(seq_wr_buf);
        spi_transfer_byte(b);
        clear_CE();
        stall_busy();
        seq_wr_status = 0;
    }else{
        // got first write in burst
        seq_wr_buf = b;
        seq_wr_status |= 0x01;
    }
}

//--------------------------------------------------------------------------------------------------
void spi_flash_seq_wr_end(void){
    if(seq_wr_status & 0x01){
        // Half a burst is queued. Finish it off
        spi_flash_seq_wr_byte(0xFF);
    }
    
    // Close write operation
    simple_cmd(CMD_WRDI);
    stall_busy();
    simple_cmd(CMD_DBSY);
}

//--------------------------------------------------------------------------------------------------
void spi_flash_seq_rd_start(uint32_t address){
    #if(DEVICE_COUNT > 1)
        current_device = DEVICE_IDX(address);
        address = PHYSICAL_ADDR(address);
    #endif
    
    set_CE();
    spi_transfer_byte(CMD_RD);
    send_address(address);
}

//--------------------------------------------------------------------------------------------------
void spi_flash_seq_rd(void *dst, uint16_t nBytes){
    spi_rx_frame((uint8_t*)dst, nBytes);
}

//--------------------------------------------------------------------------------------------------
uint8_t spi_flash_seq_rd_byte(void){
    return(spi_transfer_byte(0xFF));
}

//--------------------------------------------------------------------------------------------------
void spi_flash_seq_rd_end(void){
    clear_CE();
}

//--------------------------------------------------------------------------------------------------
uint32_t spi_flash_size(void){
    return(DEVICE_SIZE * DEVICE_COUNT);
}

///\}
