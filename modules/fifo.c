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
* Alex M.       2011-04-07   born
* Alex M.       2013-05-01   Added atomic blocks to make all functions interrupt-safe
* Alex M.       2014-05-15   Added fifo_write_trample and fifo_read_max
* 
*=================================================================================================*/

/**
* \addtogroup MOD_FIFO
* \{
**/

/**
* \file
* \brief Code for \ref MOD_FIFO
* \author Alex Mykyta 
**/

#include <msp430_xc.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <result.h>
#include <atomic.h>

#include "fifo.h"

//--------------------------------------------------------------------------------------------------
void fifo_init(FIFO_t *fifo, void *bufptr, size_t bufsize){
    fifo->bufptr = bufptr;
    fifo->bufsize = bufsize;
    fifo->rdidx = 0;
    fifo->wridx = 0;
#if(FIFO_LOG_MAX_USAGE == 1)
    fifo->max = 0;
#endif
}

//--------------------------------------------------------------------------------------------------
RES_t fifo_write(FIFO_t *fifo, void *src, size_t size){
    size_t wrcount;
    wrcount = fifo_wrcount(fifo);
    if(size > wrcount){
        return(RES_FULL);
    }

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){

        if((wrcount = fifo->bufsize - fifo->wridx) <= size){
            // write operation will wrap around in fifo
            // write first half of fifo
            memcpy(fifo->bufptr+fifo->wridx,src,wrcount);
            
            //wrap around and continue
            fifo->wridx = 0;
            size -= wrcount;
            src = (uint8_t*)src + wrcount;
        }

        if(size > 0){
            memcpy(fifo->bufptr+fifo->wridx,src,size);
            fifo->wridx += size;
        }
        
        #if(FIFO_LOG_MAX_USAGE == 1)
            wrcount = fifo_rdcount(fifo);
            if(wrcount > fifo->max){
                fifo->max = wrcount;
            }
        #endif
    }
    
    return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
void fifo_write_trample(FIFO_t *fifo, void *src, size_t size){
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        if(size >= fifo->bufsize-1){
            // if writing more than can ever fit in the buffer,
            // only write the latter portion of src buf.
            
            fifo->wridx = fifo->bufsize-1;
            fifo->rdidx = 0;
            memcpy(fifo->bufptr, src + (size-(fifo->bufsize-1)), fifo->bufsize-1);
        }else{
            size_t wrcount;
            int overflow = 0;
            if(size > fifo_wrcount(fifo)){
                overflow = 1;
            }
            
            wrcount = fifo->bufsize - fifo->wridx;
            if(wrcount <= size){
                // write operation will wrap around in fifo
                // write first half of fifo
                memcpy(fifo->bufptr+fifo->wridx, src, wrcount);
                
                //wrap around and continue
                fifo->wridx = 0;
                size -= wrcount;
                src = (uint8_t*)src + wrcount;
            }

            if(size > 0){
                memcpy(fifo->bufptr+fifo->wridx, src, size);
                fifo->wridx += size;
            }
            
            // if overflow happens, move read pointer.
            if(overflow){
                if(fifo->wridx == fifo->bufsize-1){
                    fifo->rdidx = 0;
                }else{
                    fifo->rdidx = fifo->wridx + 1;
                }
            }
        }
    }
}

//--------------------------------------------------------------------------------------------------
RES_t fifo_read(FIFO_t *fifo, void *dst, size_t size){
    size_t rdcount;
    rdcount = fifo_rdcount(fifo);
    if(size > rdcount){
        return(RES_PARAMERR);
    }
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        rdcount = fifo->bufsize - fifo->rdidx;
        if(rdcount <= size){
            // read operation will wrap around in fifo
            // read first half of fifo
            if(dst != NULL){
                memcpy(dst, fifo->bufptr + fifo->rdidx, rdcount);
            }
            //wrap around and continue
            fifo->rdidx = 0;
            size -= rdcount;
            dst = (uint8_t*)dst + rdcount;
        }
        
        if(size > 0){
            if(dst != NULL){
                memcpy(dst, fifo->bufptr + fifo->rdidx, size);
            }
            fifo->rdidx += size;
        }
    }
    
    return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
size_t fifo_read_max(FIFO_t *fifo, void *dst, size_t max_size){
    size_t rdcount, nbytes;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        rdcount = fifo_rdcount(fifo);
        if(max_size > rdcount){
            max_size = rdcount;
        }
        nbytes = max_size;
        
        rdcount = fifo->bufsize - fifo->rdidx;
        if(rdcount <= nbytes){
            // read operation will wrap around in fifo
            // read first half of fifo
            if(dst != NULL){
                memcpy(dst, fifo->bufptr + fifo->rdidx, rdcount);
            }
            //wrap around and continue
            fifo->rdidx = 0;
            nbytes -= rdcount;
            dst = (uint8_t*)dst + rdcount;
        }
        
        if(nbytes > 0){
            if(dst != NULL){
                memcpy(dst, fifo->bufptr + fifo->rdidx, nbytes);
            }
            fifo->rdidx += nbytes;
        }
    }
    
    return(max_size);
}

//--------------------------------------------------------------------------------------------------
RES_t fifo_peek(FIFO_t *fifo, void *dst, size_t size){
    size_t rdcount;
    size_t rdidx;
    if(size > fifo_rdcount(fifo)){
        return(RES_PARAMERR);
    }
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        rdidx = fifo->rdidx;
        rdcount = fifo->bufsize - rdidx;
        if(rdcount <= size){
            // read operation will wrap around in fifo
            // read first half of fifo
            memcpy(dst,fifo->bufptr+rdidx,rdcount);
            //wrap around and continue
            rdidx = 0;
            size -= rdcount;
            dst = (uint8_t*)dst + rdcount;
        }
        
        if(size > 0){
            memcpy(dst,fifo->bufptr+rdidx,size);
            rdidx += size;
        }
    }
    
    return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
void fifo_clear(FIFO_t *fifo){
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        fifo->rdidx = 0;
        fifo->wridx = 0;
    }
}

//--------------------------------------------------------------------------------------------------
size_t fifo_rdcount(FIFO_t *fifo){
    size_t wridx,rdidx;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        wridx = fifo->wridx;
        rdidx = fifo->rdidx;
    }
    
    if(wridx >= rdidx){
        return(wridx-rdidx);
    }else{
        return((fifo->bufsize-rdidx)+wridx);
    }
}

//--------------------------------------------------------------------------------------------------
size_t fifo_wrcount(FIFO_t *fifo){
    size_t wridx,rdidx;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        wridx = fifo->wridx;
        rdidx = fifo->rdidx;
    }
    
    if(rdidx >= wridx+1){
        return(rdidx-wridx-1);
    }else{
        return((fifo->bufsize-wridx)+rdidx-1);
    }
}

///\}
