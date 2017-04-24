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
* Alex M.       2014-06-10   Initial emulation
* 
*=================================================================================================*/

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <result.h>
#include "spi_flash.h"

#include <spi_flash_config.h>

typedef enum{
    S_UNINIT,
    S_IDLE,
    S_WR_SEQ,
    S_RD_SEQ
} flash_state_t;

static char *state_strings[] = {
    "S_UNINIT",
    "S_IDLE",
    "S_WR_SEQ",
    "S_RD_SEQ"
};

static flash_state_t flash_state = S_UNINIT;

static uint8_t flash_buf[DEVICE_SIZE*DEVICE_COUNT];

#define ERR_ADDR(addr)  printf("Error: Invalid address (%X) when calling %s()\n", addr, __FUNCTION__);
#define ERR_ADDR_RANGE(addr, size)  printf("Error: Invalid address range (%X-%X) when calling %s()\n", addr, addr+size, __FUNCTION__);

//--------------------------------------------------------------------------------------------------

static void check_state(flash_state_t desired_state, const char *func){
    if(desired_state != flash_state){
        printf("Error: Invalid call to %s(). SPI flash is in '%s' state. Expected '%s' state.\n",
                func, state_strings[flash_state], state_strings[desired_state]);
    }
    
    // set to desired state to suppress further errors
    flash_state = desired_state;
}
#define CHECK_STATE(x)  check_state(x, __FUNCTION__)
#define SET_STATE(x)    flash_state = x
//--------------------------------------------------------------------------------------------------
RES_t spi_flash_init(void){
    bool file_valid;
    FILE *fp;
    
    CHECK_STATE(S_UNINIT);
    
    fp = fopen(SPI_FLASH_FILENAME, "rb");
    if(fp){
        size_t size;
        // File exists. Check size
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        rewind(fp);
        if(size != sizeof(flash_buf)){
            file_valid = false;
            fclose(fp);
            fp = NULL;
        }else{
            file_valid = true;
        }
    }else{
        file_valid = false;
    }
    
    // fill memory buffer
    if(file_valid){
        fread(flash_buf, 1, sizeof(flash_buf), fp);
        fclose(fp);
    }else{
        memset(flash_buf, 0xFF, sizeof(flash_buf));
    }
    
    SET_STATE(S_IDLE);
    return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
void spi_flash_emu_dump_mem(void){
    FILE *fp;
    fp = fopen(SPI_FLASH_FILENAME, "wb");
    fwrite(flash_buf, 1, sizeof(flash_buf), fp);
    fclose(fp);
}

//--------------------------------------------------------------------------------------------------
void spi_flash_uninit(void){
    CHECK_STATE(S_IDLE);
    
    spi_flash_emu_dump_mem();
    SET_STATE(S_UNINIT);
}

//--------------------------------------------------------------------------------------------------
RES_t spi_flash_read(uint32_t address, void *dst, uint16_t nBytes){
    CHECK_STATE(S_IDLE);
    
    if((address + nBytes) > sizeof(flash_buf)){
        ERR_ADDR_RANGE(address, nBytes);
        return(RES_PARAMERR);
    }
    
    memcpy(dst, &flash_buf[address], nBytes);
    return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
RES_t spi_flash_write(uint32_t address, const void *src, uint16_t nBytes){
    const uint8_t *bsrc = src;
    CHECK_STATE(S_IDLE);
    if((address + nBytes) > sizeof(flash_buf)){
        ERR_ADDR_RANGE(address, nBytes);
        return(RES_PARAMERR);
    }
    
    while(nBytes){
        flash_buf[address] &= *bsrc;
        bsrc++;
        nBytes--;
        address++;
    }
    
    return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
RES_t spi_flash_4k_erase(uint32_t address){
    CHECK_STATE(S_IDLE);
    if(address > sizeof(flash_buf)){
        ERR_ADDR(address);
        return(RES_PARAMERR);
    }
    
    address &= ~(0x00000FFFL); // round address down to nearest 4k
    memset(&flash_buf[address], 0xFF, 0x1000);
    return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
RES_t spi_flash_32k_erase(uint32_t address){
    CHECK_STATE(S_IDLE);
    if(address > sizeof(flash_buf)){
        ERR_ADDR(address);
        return(RES_PARAMERR);
    }
    address &= ~(0x00007FFFL); // round address down to nearest 32k
    memset(&flash_buf[address], 0xFF, 0x8000L);
    return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
RES_t spi_flash_64k_erase(uint32_t address){
    CHECK_STATE(S_IDLE);
    if(address > sizeof(flash_buf)){
        ERR_ADDR(address);
        return(RES_PARAMERR);
    }
    
    address &= ~(0x0000FFFFL); // round address down to nearest 64k
    memset(&flash_buf[address], 0xFF, 0x10000L);
    return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
void spi_flash_bulk_erase(void){
    CHECK_STATE(S_IDLE);
    memset(flash_buf, 0xFF, sizeof(flash_buf));
}

//--------------------------------------------------------------------------------------------------
bool spi_flash_erase_busy(void){
    CHECK_STATE(S_IDLE);
    return(false);
}

//--------------------------------------------------------------------------------------------------
static uint32_t current_address = 0;
void spi_flash_seq_wr_start(uint32_t address){
    CHECK_STATE(S_IDLE);
    if(address > sizeof(flash_buf)){
        ERR_ADDR(address);
        return;
    }
    current_address = address;
    SET_STATE(S_WR_SEQ);
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
    CHECK_STATE(S_WR_SEQ);
    if(current_address > sizeof(flash_buf)){
        ERR_ADDR(current_address);
        return;
    }
    flash_buf[current_address++] &= b;
}

//--------------------------------------------------------------------------------------------------
void spi_flash_seq_wr_end(void){
    CHECK_STATE(S_WR_SEQ);
    SET_STATE(S_IDLE);
}

//--------------------------------------------------------------------------------------------------
void spi_flash_seq_rd_start(uint32_t address){
    CHECK_STATE(S_IDLE);
    if(address > sizeof(flash_buf)){
        ERR_ADDR(address);
        return;
    }
    current_address = address;
    SET_STATE(S_RD_SEQ);
}

//--------------------------------------------------------------------------------------------------
void spi_flash_seq_rd(void *dst, uint16_t nBytes){
    uint8_t *bdst = dst;
    while(nBytes){
        *bdst = spi_flash_seq_rd_byte();
        bdst++;
        nBytes--;
    }
}

//--------------------------------------------------------------------------------------------------
uint8_t spi_flash_seq_rd_byte(void){
    CHECK_STATE(S_RD_SEQ);
    if(current_address > sizeof(flash_buf)){
        ERR_ADDR(current_address);
        return(0);
    }
    return(flash_buf[current_address++]);
}

//--------------------------------------------------------------------------------------------------
void spi_flash_seq_rd_end(void){
    CHECK_STATE(S_RD_SEQ);
    SET_STATE(S_IDLE);
}
