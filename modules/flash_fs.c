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

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "flash_fs.h"

#include <spi_flash.h>
#include <spi_flash_config.h>

//--------------------------------
// Block definitions

#define BLOCK_SIZE          (0x1000L)
#define BLOCK_ADDR_MASK     (0x0FFFL)
#define BLOCK_COUNT ((DEVICE_SIZE*DEVICE_COUNT)/BLOCK_SIZE)

#define BLOCK_ADDR(_block_idx)   ((uint32_t)(_block_idx) * BLOCK_SIZE)
#define BLOCK_IDX(_addr)         ((_addr) / BLOCK_SIZE)
#define BLOCK_OFFSET(_addr)      ((_addr) & BLOCK_ADDR_MASK)

// Each block starts with this header:
typedef struct{
    uint16_t type;
    block_idx_t next_block;
} bhdr_t;

#define TYPE_UNUSED     0xFFFF // Empty block
#define TYPE_TABLE      0x7AB1 // File table
#define TYPE_FILE_START 0xF1E0 // First block in File
#define TYPE_FILE       0xF1E1 // Remaining blocks in file

#define BLOCK_IDX_NONE  0xFFFF

#define BHDR_ADDR(_block_idx)   (BLOCK_ADDR(_block_idx))


//--------------------------------
// Hint Table 

#define MAX_HINT_TABLE_ENTRIES  (BLOCK_SIZE/sizeof(block_idx_t))

//--------------------------------
// File Table 

#define FTE_UNUSED  0xFFFF
#define FTE_INVALID 0x0000

// First FTE address must be aligned to entry size. Do some nonsense to round up if needed
#define ROUND_UP(x,n) ((((x) + (n) - 1) / (n)) * (n))
#define _FIRST_FTE_ADDR(_block_idx)     (BLOCK_ADDR(_block_idx) +  sizeof(bhdr_t))
#define FIRST_FTE_ADDR(_block_idx)      ROUND_UP(_FIRST_FTE_ADDR(_block_idx), sizeof(block_idx_t))

//--------------------------------
// File definitions



/* This first block in a file is organized:
 *  bhdr_t Block Header (.type = TYPE_FILE_START)
 *  ffs_file_info_t Info about the file (incl filename)
 *  ... Stream of data ...
 */
#define FILE_INFO_ADDR(_block_idx)   (BLOCK_ADDR(_block_idx) + sizeof(bhdr_t))

/* The remaining blocks in a file contain:
 *  bhdr_t Block Header (.type = TYPE_FILE)
 *  uint32_t file_pos virtual address at start of block
 *  ... Stream of data ...
 */
#define FILE_POS_ADDR(_block_idx) (BLOCK_ADDR(_block_idx) + sizeof(bhdr_t))

#define FILE_FIRST_DATA_START_ADDR(_block_idx) (BLOCK_ADDR(_block_idx) + sizeof(bhdr_t) + sizeof(ffs_file_info_t))
#define FILE_DATA_START_ADDR(_block_idx) (BLOCK_ADDR(_block_idx) + sizeof(bhdr_t) + sizeof(uint32_t))

//--------------------------------
/* Data stream format
 *  - Data bytes are organized a continuous stream.
 *  - The end of the stream is terminated once an unwritten location is encountered (0xFF)
 *  - Actual 0xFF byte is escaped using the following encoding:
 *      Phy data    Translated
 *      FF          Unwritten byte. Not valid, indicates EOF
 *      1B 0F       FF byte
 *      1B 0B       1B byte
 *      1B FF       Same as FF. Treated as indication of EOF
 *      1B [else]   Unnecessary escape, the byte after the 1B is used as is
 *  - If 1B is the last byte in a block, it is ignored
 */


//==================================================================================================
// Internal
//==================================================================================================

// Points to start location of a block search. (NOT guaranteed to be an empty block)
static block_idx_t block_search_idx;

// Tail block of the file table
static block_idx_t FT_Tail_Block; 

// Address of the next empty FTE. If the current block is full (FT needs to be extended), Address
// points to the base of the block
static uint32_t next_empty_FTE_addr;

// A file lookup saw too much garbage in the FT. A cleanup pass will be done the next time something
// is deleted.
static bool request_cleanup;

//--------------------------------------------------------------------------------------------------
/**
 * \brief Finds the next free block
 * \retval 0 No free blocks were found
 * \retval others Block index
 **/
static block_idx_t find_unused_block(void){
    block_idx_t block;
    block_idx_t startingblock;
    uint16_t block_type;
    
    block = block_search_idx;
    startingblock = block;
    
    do{
        block++;

        if(block == BLOCK_COUNT){
            block = 1; // wrap to beginning. Block 0 is always used so skip it.
            if(startingblock == 0) return(0); // wrapped on initial block search
        }

        // "type" header item marks whether it is free or not
        spi_flash_read(BHDR_ADDR(block) + offsetof(bhdr_t,type), &block_type, sizeof(block_type));
        
        if(block == startingblock){
            // Has scanned all the blocks
            if(block_type == TYPE_UNUSED){
                return(block);
            }else{
                return(0);
            }
        }
        
    } while(block_type != TYPE_UNUSED);
    
    block_search_idx = block;
    return(block);
}

//--------------------------------------------------------------------------------------------------
/**
 * \brief Extends a chain of blocks
 * 
 * \param last_block Last block in the chain
 * \param block_type TYPE_TABLE or TYPE_FILE
 * \returns Index of the new block. Returns 0 if fails (out of memory)
 */
static block_idx_t extend_block_chain(block_idx_t last_block, uint16_t block_type){
    block_idx_t new_block;
    
    new_block = find_unused_block();
    if(new_block == 0) return(0); // Could not find an empty block.
    
    // Write to headers to link the blocks
    
    // Write the type of the new block
    spi_flash_write(BHDR_ADDR(new_block) + offsetof(bhdr_t,type), &block_type, sizeof(block_type));
    
    // Link chain to the new block
    spi_flash_write(BHDR_ADDR(last_block) + offsetof(bhdr_t,next_block), &new_block, sizeof(new_block));
    
    return(new_block);
}

//--------------------------------------------------------------------------------------------------
/**
 * \brief Deletes a chain of blocks
 * \note This does NOT delete any child blocks (if this is a file table)
 * 
 * \param first_block First block in the chain
 **/
static void delete_block_chain(block_idx_t first_block){
    block_idx_t block;
    
    block = first_block;
    // Traverse the block chain
    while(1){
        block_idx_t b_next;
        spi_flash_read(BHDR_ADDR(block) + offsetof(bhdr_t, next_block) , &b_next, sizeof(b_next));
        
        spi_flash_4k_erase(BLOCK_ADDR(block));
        while(spi_flash_erase_busy());
        
        // No more blocks in the chain
        if(b_next == BLOCK_IDX_NONE){
            break;
        }
        block = b_next;
    }
}

//--------------------------------------------------------------------------------------------------
static int compare_filename(block_idx_t file_block, const char *filename){
    uint8_t n = MAX_FILENAME_LEN;
    
    spi_flash_seq_rd_start(FILE_INFO_ADDR(file_block) + offsetof(ffs_file_info_t,filename));
    while(n){
        uint8_t b;
        b = spi_flash_seq_rd_byte();
        if(b != *filename){
            spi_flash_seq_rd_end();
            return 1;
        }
        
        if(b == 0){
            spi_flash_seq_rd_end();
            return 0;
        }
        filename++;
        n--;
    }
    
    spi_flash_seq_rd_end();
    return 1;
}

//--------------------------------------------------------------------------------------------------
/**
 * \brief Looks up a file in the file table
 * 
 * If during lookup, too many invalidated entries were encountered, request_cleanup is set to true
 * 
 * request_cleanup criteria:
 *  - Traversed into at least the second block
 *  - encountered at least 2x more invalid blocks than valid ones
 * 
 * \param filename       File name string
 * \param FTE_addr [out] Pointer to dst for the address of the file's FTE. NULL is OK
 * 
 * \returns Start block of file. 0 if not found
 **/
static block_idx_t lookup_file(const char *filename, uint32_t *FTE_addr){
    uint16_t valid_count = 0;
    uint16_t invalid_count = 0;
    
    uint32_t current_fte_addr = FIRST_FTE_ADDR(FT_Tail_Block);
    spi_flash_seq_rd_start(current_fte_addr);
    while(1){
        block_idx_t ft_entry;
        
        // read entry
        spi_flash_seq_rd(&ft_entry, sizeof(ft_entry));
        
        if(ft_entry == FTE_UNUSED){
            // Reached the end of the FT without finding anything
            spi_flash_seq_rd_end();
            break;
        }else if(ft_entry != FTE_INVALID){
            // Entry is valid.
            valid_count++;
            spi_flash_seq_rd_end();
            
            // Does the filename match?
            if(compare_filename(ft_entry, filename) == 0){
                if(FTE_addr){
                    *FTE_addr = current_fte_addr;
                }
                
                if(invalid_count > (valid_count/2)){
                    if(BLOCK_IDX(current_fte_addr) != FT_Tail_Block){
                        request_cleanup = true;
                    }
                }
                return(ft_entry);
            }
            
            if(BLOCK_OFFSET(current_fte_addr + sizeof(block_idx_t)) != 0){
                // wont be skipping to the next block. OK to restart sequential read
                spi_flash_seq_rd_start(current_fte_addr + sizeof(block_idx_t));
            }
        }else{ // ft_entry == FTE_INVALID
            invalid_count++;
            if(BLOCK_OFFSET(current_fte_addr + sizeof(block_idx_t)) == 0){
                // will be skipping to the next block. Stop the sequential read
                spi_flash_seq_rd_end();
            }
        }
        
        // next entry
        current_fte_addr += sizeof(block_idx_t);
        if(BLOCK_OFFSET(current_fte_addr) == 0){
            block_idx_t block;
            // reached the end of the block, attempt to jump to the next one
            current_fte_addr -= BLOCK_SIZE;
            spi_flash_read(current_fte_addr + offsetof(bhdr_t, next_block) , &block, sizeof(block));
            if(block == BLOCK_IDX_NONE){
                // No more blocks. reached the end of the FT
                break;
            }
            
            current_fte_addr = FIRST_FTE_ADDR(block);
            spi_flash_seq_rd_start(current_fte_addr);
        }
    }
    
    if(invalid_count > (valid_count/2)){
        if(BLOCK_IDX(current_fte_addr) != FT_Tail_Block){
            request_cleanup = true;
        }
    }
    
    return(0);
}

//--------------------------------------------------------------------------------------------------
/**
 * \brief Marks a FTE as invalid
 * \param entry_address Address of FTE
 */
static void invalidate_FTE(uint32_t FTE_addr){
    block_idx_t b = FTE_INVALID;
    spi_flash_write(FTE_addr, &b, sizeof(b));
}

//--------------------------------------------------------------------------------------------------
/**
 * \brief Writes a new FTE to the File Table. Extends the table if necessary
 * \param block Item's block index
 * 
 * \retval 0 success
 * \retval others Failed. Out of memory
 */
static int write_FTE(block_idx_t block){
    
    if((next_empty_FTE_addr & BLOCK_ADDR_MASK) == 0){
        // next_empty_FTE_addr indicates that the block is full and must be extended
        block_idx_t new_block;
        new_block = extend_block_chain(BLOCK_IDX(next_empty_FTE_addr), TYPE_TABLE);
        
        if(new_block){
            next_empty_FTE_addr = FIRST_FTE_ADDR(new_block);
        }else{
            // Could not extend the block chain. Out of memory
            return(-1);
        }
    }
    
    spi_flash_write(next_empty_FTE_addr, &block, sizeof(block));
    next_empty_FTE_addr += sizeof(block_idx_t);
    // check if block is full (if next_empty_FTE_addr wrapped to the next block)
    if((next_empty_FTE_addr & BLOCK_ADDR_MASK) == 0){
        // point it back to the base of the head FT block
        next_empty_FTE_addr -= BLOCK_SIZE;
    }
    return(0);
}

//--------------------------------------------------------------------------------------------------
/**
 * \brief Create a new file
 * \param info Pointer to file info object
 * 
 * \returns Start block of file. 0 if failed (out of memory)
 */
static block_idx_t create_file(ffs_file_info_t *info){
    block_idx_t block;
    
    // Find a starting block for the file
    block = find_unused_block();
    if(block == 0) return(0);
    
    // Write the file header & metadata to the block
    uint16_t type = TYPE_FILE_START;
    spi_flash_write(BHDR_ADDR(block) + offsetof(bhdr_t, type), &type, sizeof(type));
    spi_flash_write(FILE_INFO_ADDR(block), info, sizeof(ffs_file_info_t));
    
    // Write entry to FT
    if(write_FTE(block) != 0){
        return(0);
    }
    
    return(block);
}

//--------------------------------------------------------------------------------------------------
/**
 * \brief Find the last entry in the hint table
 * 
 * \param [out] hint entry address. Null is OK
 * \returns Tail block index of File Table. Returns 0 if hint table is empty.
 **/
static block_idx_t lookup_hint_table(uint32_t *hint_addr){
    uint16_t imin, imax;
    block_idx_t hint_entry;
    uint32_t addr;
    imin = 0;
    imax = (BLOCK_SIZE)/sizeof(block_idx_t);
    
    // binary search for the edge of the hint table entries
    while(imax != (imin+1)){
        uint16_t imid;
        
        imid = (imin + imax)/2;
        
        addr = imid;
        addr *= sizeof(hint_entry);
        spi_flash_read(addr, &hint_entry, sizeof(hint_entry));
        
        if(hint_entry == BLOCK_IDX_NONE){
            imax = imid;
        }else{
            imin = imid;
        }
    }
    // Now imin points to the last valid entry.
    
    addr = imin;
    addr *= sizeof(hint_entry);
    spi_flash_read(addr, &hint_entry, sizeof(hint_entry));
    
    if(hint_addr){
        *hint_addr = addr;
    }
    
    if(hint_entry == BLOCK_IDX_NONE){
        return(0);
    }else{
        return(hint_entry);
    }
}

//--------------------------------------------------------------------------------------------------

/**
 * \brief Do a single cleanup pass through the FT
 * 
 * \note Assumes that the FT Tail block is full and another block follows it!
 **/
static void ft_cleanup_pass(void){
    
    // relocate all entries in the first FT block to the end of the FT.
    uint32_t addr;
    addr = FIRST_FTE_ADDR(FT_Tail_Block);
    spi_flash_seq_rd_start(addr);
    while(1){
        block_idx_t fte;
        
        spi_flash_seq_rd(&fte, sizeof(block_idx_t));
        
        if(fte != FTE_INVALID){
            // Valid entry. Relocate it
            spi_flash_seq_rd_end();
            if(write_FTE(fte) != 0){
                // couldn't write the FTE. out of memory
                // Give up cleanup
                request_cleanup = false;
                return;
            }
            
            // mark the FTE as invalid
            fte = FTE_INVALID;
            spi_flash_write(addr, &fte, sizeof(block_idx_t));
            
            // Goto next
            addr += sizeof(block_idx_t);
            if(BLOCK_OFFSET(addr) == 0){
                // Reached end of block. Done
                break;
            }
            
            spi_flash_seq_rd_start(addr);
        }else{
            // Invalid entry. Goto next
            addr += sizeof(block_idx_t);
            if(BLOCK_OFFSET(addr) == 0){
                // Reached end of block. Done
                spi_flash_seq_rd_end();
                break;
            }
        }
    }
    
    addr -= BLOCK_SIZE;
    
    // Read location of next block from header
    block_idx_t new_tail_block;
    spi_flash_read(addr + offsetof(bhdr_t, next_block) , &new_tail_block, sizeof(block_idx_t));
    
    // write new tail block into hint table
    lookup_hint_table(&addr);
    addr += sizeof(block_idx_t);
    
    // check if hint table is full
    if(addr == BLOCK_SIZE){
        // seeked to the end of the hint table. No more room.
        // Erase hint table and start again
        spi_flash_4k_erase(0);
        while(spi_flash_erase_busy());
        
        addr = 0;
    }
    
    // write entry
    spi_flash_write(addr, &new_tail_block, sizeof(block_idx_t));
    
    // delete old tail block
    spi_flash_4k_erase(BLOCK_ADDR(FT_Tail_Block));
    while(spi_flash_erase_busy());
    
    FT_Tail_Block = new_tail_block;
    request_cleanup = false;
}

//==================================================================================================
// User functions
//==================================================================================================
/**
* \brief Initializes the filesystem and the flash hardware
* \retval RES_FAIL      Initialization of the storage medium failed
* \retval RES_INVALID   Initialized flash, but no valid filesystem is present. Bulk erase the memory
*                       and format the flash before trying again (see spi_flash_bulk_erase() and
*                       ffs_create_filesystem())
* \retval RES_OK        Success!
**/
RES_t ffs_init(void){
    
    if(spi_flash_init() != RES_OK){
        return(RES_FAIL);
    }
    
    FT_Tail_Block = lookup_hint_table(NULL);
    
    if(FT_Tail_Block == 0){
        // Hint table is empty
        // Assuming this means that the flash is completely erased
        
        // start a new FT at block 1
        FT_Tail_Block = 1;
        
        // create a new FT_Tail_Block
        uint16_t block_type = TYPE_TABLE;
        spi_flash_write(BHDR_ADDR(FT_Tail_Block) + offsetof(bhdr_t,type), &block_type, sizeof(block_type));
        
        // Write block id to hint table
        spi_flash_write(0, &FT_Tail_Block, sizeof(FT_Tail_Block));
    }
    
    // Seek through the FT and find next_empty_FTE_addr
    uint32_t current_fte_addr = FIRST_FTE_ADDR(FT_Tail_Block);
    spi_flash_seq_rd_start(current_fte_addr);
    while(1){
        block_idx_t ft_entry;
        
        // read entry
        spi_flash_seq_rd(&ft_entry, sizeof(ft_entry));
        
        if(ft_entry == FTE_UNUSED){
            // Reached the end of the FT
            spi_flash_seq_rd_end();
            break;
        }
        
        // next entry
        current_fte_addr += sizeof(block_idx_t);
        if(BLOCK_OFFSET(current_fte_addr) == 0){
            block_idx_t block;
            // reached the end of the block, attempt to jump to the next one
            current_fte_addr -= BLOCK_SIZE;
            spi_flash_seq_rd_end();
            spi_flash_read(current_fte_addr + offsetof(bhdr_t, next_block) , &block, sizeof(block));
            if(block == BLOCK_IDX_NONE){
                // No more blocks. reached the end of the FT
                // current_fte_addr points to the base of the last block in FT
                break;
            }
            
            current_fte_addr = FIRST_FTE_ADDR(block);
            spi_flash_seq_rd_start(current_fte_addr);
        }
    }
    next_empty_FTE_addr = current_fte_addr;
    
    // safely attempt to resume block search using whatever the variable already holds
    if(block_search_idx >= BLOCK_COUNT) block_search_idx = 1;
    block_search_idx = find_unused_block() - 1;
    
    request_cleanup = false;
    
    return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
/**
* \brief Uninitializes the filesystem and the flash hardware
**/
void ffs_uninit(void){
    spi_flash_uninit();
}

//--------------------------------------------------------------------------------------------------
uint16_t ffs_blocks_free(void){
    // returns the number of free blocks
    uint16_t freecount;
    uint16_t firstblock;
    freecount = 1;
    
    firstblock = find_unused_block();
    if(firstblock == 0){
        return(0);
    }
    
    while(find_unused_block() != firstblock){
        freecount++;
    }
    block_search_idx = firstblock - 1;
    return(freecount);
}

//--------------------------------------------------------------------------------------------------
RES_t ffs_fopen(ffs_file_t *f, const char *filename, ffs_filemode_t filemode){
    uint32_t FTE_addr;
    
    if(strlen(filename) >= MAX_FILENAME_LEN) return(RES_NOTFOUND);
    
    f->start_block = lookup_file(filename, &FTE_addr);
    if(filemode == FFS_RD){
        if(f->start_block != 0){
            // Exists. Open the file
            f->filemode = filemode;
            f->vir_addr = 0;
            f->phy_addr = FILE_FIRST_DATA_START_ADDR(f->start_block);
            f->eof = false; // assume there is data to be read
        }else{
            f->filemode = FFS_CLOSED;
            return(RES_NOTFOUND);
        }
    }else if(filemode == FFS_WR_APPEND){
        if(f->start_block != 0){
            // Exists. Open file and seek to the end
            f->filemode = FFS_RD;
            f->vir_addr = 0;
            f->phy_addr = FILE_FIRST_DATA_START_ADDR(f->start_block);
            f->eof = false;
            
            // try to seek reeeeaaaly far
            ffs_fseek(f, 0xFFFFFFFFUL);
            
            // If seek stopped right at the end of a block, create a new block.
            // this situation is marked by the phy_addr pointing to the head of a block.
            if(BLOCK_OFFSET(f->phy_addr) == 0){
                block_idx_t new_block;
                new_block = extend_block_chain(BLOCK_IDX(f->phy_addr), TYPE_FILE);
                if(new_block == 0){
                    f->filemode = FFS_CLOSED;
                    return(RES_FULL);
                }
                
                // write file pos
                spi_flash_write(FILE_POS_ADDR(new_block), &f->vir_addr, sizeof(f->vir_addr));
                
                // update file object
                f->phy_addr = FILE_DATA_START_ADDR(new_block);
            }
            
            f->filemode = FFS_WR_APPEND;
            f->eof = true; // when writing, always eof
        }else{
            // Does not exist. Create the file
            ffs_file_info_t info;
            
            // initialize metadata
            strncpy(info.filename, filename, MAX_FILENAME_LEN);
            
            f->start_block = create_file(&info);
            if(f->start_block == 0){
                f->filemode = FFS_CLOSED;
                return(RES_FULL);
            }
            
            f->filemode = filemode;
            f->vir_addr = 0;
            f->phy_addr = FILE_FIRST_DATA_START_ADDR(f->start_block);
            f->eof = true; // when writing, always eof
        }
    }else if(filemode == FFS_WR_REPLACE){
        if(f->start_block != 0){
            // Exists. Delete the old file first
            invalidate_FTE(FTE_addr);
            delete_block_chain(f->start_block);
            if(request_cleanup){
                ft_cleanup_pass();
            }
        }
        
        // Create the file
        ffs_file_info_t info;
        
        // initialize metadata
        strncpy(info.filename, filename, MAX_FILENAME_LEN);
        
        f->start_block = create_file(&info);
        if(f->start_block == 0){
            f->filemode = FFS_CLOSED;
            return(RES_FULL);
        }
        
        f->filemode = filemode;
        f->vir_addr = 0;
        f->phy_addr = FILE_FIRST_DATA_START_ADDR(f->start_block);
        f->eof = true; // when writing, always eof
    }else{
        // Unknown filemode...
        f->filemode = FFS_CLOSED;
        return(RES_NOTFOUND);
    }
    
    return(RES_OK);
}

//--------------------------------------------------------------------------------------------------
void ffs_fclose(ffs_file_t *f){
    f->filemode = FFS_CLOSED;
}

//--------------------------------------------------------------------------------------------------
void ffs_remove(const char *filename){
    uint32_t FTE_addr;
    block_idx_t file_block;
    
    file_block = lookup_file(filename, &FTE_addr);
    
    if(file_block != 0){
        // Found file
        invalidate_FTE(FTE_addr);
        delete_block_chain(file_block);
        if(request_cleanup){
            ft_cleanup_pass();
        }
    }
}

//--------------------------------------------------------------------------------------------------
void ffs_opendir(ffs_dir_t *dir){
    dir->next_entry_addr = FIRST_FTE_ADDR(FT_Tail_Block);
}

//--------------------------------------------------------------------------------------------------
bool ffs_readdir(ffs_dir_t *dir, ffs_file_info_t *info){
    block_idx_t fte;
    
    if(dir->next_entry_addr == 0) return(false);
    
    spi_flash_seq_rd_start(dir->next_entry_addr);
    while(1){
        // read entry
        spi_flash_seq_rd(&fte, sizeof(fte));
        
        if(fte == FTE_UNUSED){
            // reached end
            dir->next_entry_addr = 0;
            spi_flash_seq_rd_end();
            return(false);
        }
        
        dir->next_entry_addr += sizeof(fte);
        if(BLOCK_OFFSET(dir->next_entry_addr) == 0){
            block_idx_t block;
            // reached the end of the block, attempt to jump to the next one
            dir->next_entry_addr -= BLOCK_SIZE;
            spi_flash_seq_rd_end();
            spi_flash_read(dir->next_entry_addr + offsetof(bhdr_t, next_block) , &block, sizeof(block));
            dir->next_entry_addr = FIRST_FTE_ADDR(block);
            
            if(block == BLOCK_IDX_NONE){
                // No more blocks. reached the end of the FT
                dir->next_entry_addr = 0;
                
                if(fte == FTE_INVALID){
                    // previous lookup was invalid
                    return(false);
                }
            }
            
            if(fte != FTE_INVALID){
                // previous lookup found an entry. OK to break
                break;
            }
            
            spi_flash_seq_rd_start(dir->next_entry_addr);
        }
        
        if(fte != FTE_INVALID){
            // previous lookup found an entry. OK to break
            spi_flash_seq_rd_end();
            break;
        }
    }
    
    // found valid fte. read info
    spi_flash_read(FILE_INFO_ADDR(fte), info, sizeof(ffs_file_info_t));
    
    return(true);
}

//--------------------------------------------------------------------------------------------------
uint32_t ffs_ftell(ffs_file_t *f){
    return(f->vir_addr);
}

//--------------------------------------------------------------------------------------------------
size_t ffs_fread(void *data, size_t size, ffs_file_t *f){
    
    if(f->filemode != FFS_RD) return(0);
    if(f->eof) return(0);
    if(size == 0) return(0);
    
    uint8_t *data_b = data;
    size_t n_read = 0;
    int esc = 0;
    spi_flash_seq_rd_start(f->phy_addr);
    while(size){
        uint8_t b;
        
        b = spi_flash_seq_rd_byte();
        
        if(esc){
            // escaped byte
            
            if(b == 0xFF){
                // reached EOF
                f->eof = true;
                break;
            }else{
                // read byte
                if(b == 0x0F){
                    // escaped 0xFF
                    b = 0xFF;
                }else if(b == 0x0B){
                    // escaped '\e'
                    b = '\e';
                }
                esc = 0;
                *data_b++ = b;
                n_read++;
                f->vir_addr++;
                size--;
            }
        }else{
            // regular byte
            if(b == '\e'){
                esc = 1;
            }else if(b == 0xFF){
                // reached EOF
                f->eof = true;
                break;
            }else{
                *data_b++ = b;
                n_read++;
                f->vir_addr++;
                size--;
            }
        }
        
        f->phy_addr++;
        if(BLOCK_OFFSET(f->phy_addr) == 0){
            block_idx_t block;
            // reached the end of the block, attempt to jump to the next one (and reset esc)
            f->phy_addr -= BLOCK_SIZE;
            spi_flash_seq_rd_end();
            spi_flash_read(f->phy_addr + offsetof(bhdr_t, next_block) , &block, sizeof(block));
            if(block == BLOCK_IDX_NONE){
                // No more blocks. reached the end of the file
                f->eof = true;
                return(n_read);
            }
            
            f->phy_addr = FILE_DATA_START_ADDR(block);
            
            // check if done before reopening a sequential read.
            if(size == 0){
                return(n_read);
            }
            esc = 0;
            spi_flash_seq_rd_start(f->phy_addr);
        }
    }
    spi_flash_seq_rd_end();
    return(n_read);
}

//--------------------------------------------------------------------------------------------------
RES_t ffs_fseek(ffs_file_t *f, uint32_t offset){
    
    if(f->filemode != FFS_RD) return(RES_PARAMERR);
    
    // If rewinding, prep addresses for new seek start position
    if(offset < f->vir_addr){
        // needs to rewind
        if(BLOCK_IDX(f->phy_addr) == f->start_block){
            // still in the first block in a file. Start from the beginning
            f->vir_addr = 0;
            f->phy_addr = FILE_FIRST_DATA_START_ADDR(f->start_block);
            f->eof = false;
        }else{
            uint32_t block_vir_addr;
            // read current block's start vir_addr
            spi_flash_read(FILE_POS_ADDR(BLOCK_IDX(f->phy_addr)) , &block_vir_addr, sizeof(block_vir_addr));
            
            f->eof = false;
            
            if(offset < block_vir_addr){
                // before this block. Must start from the beginning
                f->vir_addr = 0;
                f->phy_addr = FILE_FIRST_DATA_START_ADDR(f->start_block);
            }else{
                // Somewhere in this block. Start at the beginning of the block
                f->vir_addr = block_vir_addr;
                f->phy_addr = FILE_DATA_START_ADDR(BLOCK_IDX(f->phy_addr));
            }
        }
    }
    // Now always seeking forward
    
    // check for some corner cases first
    if(offset == f->vir_addr){
        // No seek needed!
        return(RES_OK);
    }else{
        // seeking forward
        // check if already at the end of the file
        if(f->eof){
            return(RES_END);
        }
    }
    
    // Start seeking forward from current position
    // phy_addr is pointing to a valid location
    spi_flash_seq_rd_start(f->phy_addr);
    int esc = 0;
    while(f->vir_addr < offset){
        uint8_t b;
        
        b = spi_flash_seq_rd_byte();
        
        if(esc){
            // escaped byte
            if(b == 0xFF){
                // reached EOF
                f->eof = true;
                break;
            }else{
                esc = 0;
                f->vir_addr++;
            }
        }else{
            // regular byte
            if(b == '\e'){
                esc = 1;
            }else if(b == 0xFF){
                // reached EOF
                f->eof = true;
                break;
            }else{
                f->vir_addr++;
            }
        }
        
        f->phy_addr++;
        if(BLOCK_OFFSET(f->phy_addr) == 0){
            block_idx_t block;
            // reached the end of the block, attempt to jump to the next one (and reset esc)
            f->phy_addr -= BLOCK_SIZE;
            spi_flash_seq_rd_end();
            spi_flash_read(f->phy_addr + offsetof(bhdr_t, next_block) , &block, sizeof(block));
            if(block == BLOCK_IDX_NONE){
                // No more blocks. reached the end of the file
                f->eof = true;
                if(f->vir_addr == offset){
                    return(RES_OK);
                }else{
                    f->eof = true;
                    return(RES_END);
                }
            }
            
            f->phy_addr = FILE_DATA_START_ADDR(block);
            
            // check if done before reopening a sequential read.
            if(f->vir_addr == offset){
                return(RES_OK);
            }
            esc = 0;
            spi_flash_seq_rd_start(f->phy_addr);
        }
    }
    spi_flash_seq_rd_end();
    
    if(f->vir_addr == offset){
        return(RES_OK);
    }else{
        // didn't reach the desired address
        return(RES_END);
    }
}

//--------------------------------------------------------------------------------------------------
bool ffs_feof(ffs_file_t *f){
    return(f->eof);
}

//--------------------------------------------------------------------------------------------------
size_t ffs_fwrite(const void *data, size_t size, ffs_file_t *f){
    
    if((f->filemode != FFS_WR_APPEND) && (f->filemode != FFS_WR_REPLACE)) return(0);
    if(size == 0) return(0);
    if(BLOCK_OFFSET(f->phy_addr) == 0) return(0); // previously out of memory
    
    const uint8_t *data_b = data;
    size_t n_write = 0;
    uint8_t b = *data_b++;
    spi_flash_seq_wr_start(f->phy_addr);
    while(size){
        
        if((b == 0xFF) || (b == '\e')){
            // char needs to be escaped first
            spi_flash_seq_wr_byte('\e');
            
            if(BLOCK_OFFSET(f->phy_addr) == BLOCK_ADDR_MASK){
                // Special case where \e is the last byte in the block and no room for the
                // escaped byte.
                // Don't translate the byte. This will force it to try again at the next block.
            }else{
                b &= 0x0F; // translate byte
            }
        }else{
            // no need to escape
            spi_flash_seq_wr_byte(b);
            
            b = *data_b++;
            n_write++;
            f->vir_addr++;
            size--;
        }
        
        f->phy_addr++;
        if(BLOCK_OFFSET(f->phy_addr) == 0){
            // reached the end of block, extend to the next
            block_idx_t block;
            spi_flash_seq_wr_end();
            f->phy_addr -= BLOCK_SIZE;
            block = extend_block_chain(BLOCK_IDX(f->phy_addr), TYPE_FILE);
            if(block == 0){
                // out of memory!
                return(n_write);
            }
            
            // Write file pos
            spi_flash_seq_wr_start(FILE_POS_ADDR(block));
            spi_flash_seq_wr(&f->vir_addr, sizeof(f->vir_addr));
            f->phy_addr = FILE_DATA_START_ADDR(block);
        }
    }
    spi_flash_seq_wr_end();
    return(n_write);
}
