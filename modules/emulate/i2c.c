/*
* Copyright (c) 2013, Alexander I. Mykyta
* All rights reserved.
* Supported and funded in part by RIT / NTID Center on Access Technology
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
#include "i2c.h"

//--------------------------------------------------------------------------------------------------
void i2c_init(void){
    
}

//--------------------------------------------------------------------------------------------------
void i2c_uninit(void){
    
}

//--------------------------------------------------------------------------------------------------
void i2c_transfer_start(const i2c_package_t *pkg, void (*callback)(i2c_status_t result)){
    
}

//--------------------------------------------------------------------------------------------------
i2c_status_t i2c_transfer_status(void){
    return(I2C_IDLE);
}

///\}
