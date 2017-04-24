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

/**
* \addtogroup MOD_FLASHFS Flash File System
* \brief Light-weight file system optimized for SPI Flash
* \author Alex Mykyta
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_FLASHFS "Flash File System"
* \author Alex Mykyta
**/

#ifndef FLASH_FS_H
#define FLASH_FS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <result.h>
#include <stdbool.h>

typedef uint16_t block_idx_t;

typedef enum {
    FFS_CLOSED = 0, ///< File is not open (Used internally)
    FFS_RD,         ///< Open file for reading
    FFS_WR_APPEND,  ///< Open file for writing. Appends data. Creates new file if needed.
    FFS_WR_REPLACE  ///< Open file for writing. Replaces file. Creates new file if needed.
} ffs_filemode_t;

// File object
typedef struct{
    uint32_t vir_addr; // Current address in file
    uint32_t phy_addr; // Current hardware address. If in read mode, and phy_addr points to the
                       // start of a block, then the block is full and it is the last block.
    block_idx_t start_block;
    ffs_filemode_t filemode;
    bool eof;
} ffs_file_t;

typedef struct{
    uint32_t next_entry_addr; // points to next FTE to be read. 0 if reached the end
} ffs_dir_t;

#define MAX_FILENAME_LEN    32
typedef struct{
    char filename[MAX_FILENAME_LEN];
} ffs_file_info_t;

//==================================================================================================

///\name Filesystem operations
///\{

/**
* \brief Initializes the filesystem and the flash hardware
* \retval RES_FAIL      Initialization of the storage medium failed
* \retval RES_INVALID   Initialized flash, but no valid filesystem is present. Bulk erase the memory
*                       and try again (see spi_flash_bulk_erase())
* \retval RES_OK        Success!
**/
RES_t ffs_init(void);

/**
* \brief Uninitializes the filesystem and the flash hardware
**/
void ffs_uninit(void);
 
/**
* \brief Returns the number of unused blocks
* 
* \returns Number of free blocks
**/
uint16_t ffs_blocks_free(void);
///\}

///\name File Operations
///\{
    
/**
* \brief Opens a file for reading or writing.
* 
* \param f         Pointer to the file object
* \param filename  File name string
* \param filemode  Mode to open the file (See #ffs_filemode_t)
* 
* \retval RES_FULL      Storage medium is full (write)
* \retval RES_NOTFOUND  File not found (read)
* \retval RES_OK        Success!
**/
RES_t ffs_fopen(ffs_file_t *f, const char *filename, ffs_filemode_t filemode);

/**
* \brief Closes the file
* 
* \param f  Pointer to the file object
**/
void ffs_fclose(ffs_file_t *f);

/**
* \brief Deletes a file by name
* 
* \param filename  File name string
**/
void ffs_remove(const char *filename);

/**
 * \brief Initializes a directory object to start seeking through existing files
 * 
 * \param dir pointer to the directory object to be initialized
 **/
void ffs_opendir(ffs_dir_t *dir);

/**
 * \brief Read the next file in the directory
 * 
 * \param dir pointer to the directory object
 * \param info File info
 * \returns \c true if info object is valid. \c false if reached the end of the directory.
 **/
bool ffs_readdir(ffs_dir_t *dir, ffs_file_info_t *info);

/**
* \brief Returns the current byte offset from the beginning of the file
* 
* \param f  Pointer to the file object
* 
* \return Byte offset from the beginning of the file
**/
uint32_t ffs_ftell(ffs_file_t *f);

///\}

///\name Read mode functions
///\{

/**
* \brief Reads data from the file
* 
* \param f     Pointer to the file object
* \param data  Pointer to destination buffer
* \param size  Number of bytes to read from file
* 
* \return Number of bytes successfully read
**/
size_t ffs_fread(void *data, size_t size, ffs_file_t *f);

/**
* \brief Seeks to the specified byte offset from the beginning of the file.
* 
* For read files only!
* 
* \param f       Pointer to the file object
* \param offset  Number of bytes from the beginning of the file
* 
* \retval RES_PARAMERR  Invalid input parameter
* \retval RES_END       Requested offset is greater than the file length. Seek is set to EOF.
* \retval RES_OK        Success!
**/
RES_t ffs_fseek(ffs_file_t *f, uint32_t offset);

/**
* \brief Checks if the end of the given file has been reached
* 
* \note This function only reports the state as reported by the most recent I/O operation, it does
* not examine the data source. For example, if the most recent I/O was a single byte read, which
* returned the last byte of the file, feof returns false. The next read fails and changes the state
* to end-of-file. Only then feof returns true.
* 
* \param f  Pointer to the file object
* 
* \retval true   Reached the end of the file
* \retval false  Read pointer is not at the end of the file
**/
bool ffs_feof(ffs_file_t *f);
///\}

///\name Write mode functions
///\{
    
/**
* \brief Writes data to the file
* 
* \param f     Pointer to the file object
* \param data  Pointer to source data
* \param size  Number of bytes to write to the file
* 
* \return Number of bytes successfully written
**/
size_t ffs_fwrite(const void *data, size_t size, ffs_file_t *f);

///\}

#ifdef __cplusplus
}
#endif

#endif
///\}
