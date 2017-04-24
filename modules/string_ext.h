/*
* Copyright (c) 2012, Alexander I. Mykyta
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
* \addtogroup MOD_STRING_EXT Extended String Functions
* \brief Additional string functions.
* \author Alex Mykyta 
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_STRING_EXT "Extended String Functions"
* \author Alex Mykyta 
**/

#ifndef STRING_EXT_H
#define STRING_EXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/**
* \brief Converts an 8-bit integer into a hexadecimal string
* \param [out] buffer Pointer to a character string to write to. Outputs null-terminated string
* \param [in] buf_size Up to buf_size - 1 characters may be written, plus the null terminator
* \param [in] num Integer to be converted
* \return Number of characters written if successful. If the resulting string gets truncated due to 
*     \c buf_size limit, function returns the total number of characters (not including the
*     terminating null-byte) which would have been written, if the limit was not imposed.
**/
uint8_t snprint_x8(char *buffer, size_t buf_size, uint8_t num);

/**
* \brief Converts a 16-bit integer into a hexadecimal string
* \param [out] buffer Pointer to a character string to write to. Outputs null-terminated string
* \param [in] buf_size Up to buf_size - 1 characters may be written, plus the null terminator
* \param [in] num Integer to be converted
* \return Number of characters written if successful. If the resulting string gets truncated due to 
*     \c buf_size limit, function returns the total number of characters (not including the
*     terminating null-byte) which would have been written, if the limit was not imposed.
**/
uint8_t snprint_x16(char *buffer, size_t buf_size, uint16_t num);

/**
* \brief Converts a 32-bit integer into a hexadecimal string
* \param [out] buffer Pointer to a character string to write to. Outputs null-terminated string
* \param [in] buf_size Up to buf_size - 1 characters may be written, plus the null terminator
* \param [in] num Integer to be converted
* \return Number of characters written if successful. If the resulting string gets truncated due to 
*     \c buf_size limit, function returns the total number of characters (not including the
*     terminating null-byte) which would have been written, if the limit was not imposed.
**/
uint8_t snprint_x32(char *buffer, size_t buf_size, uint32_t num);

/**
* \brief Converts an unsigned 8-bit integer into a decimal string
* \param [out] buffer Pointer to a character string to write to. Outputs null-terminated string
* \param [in] buf_size Up to buf_size - 1 characters may be written, plus the null terminator
* \param [in] num Integer to be converted
* \return Number of characters written if successful. If the resulting string gets truncated due to 
*     \c buf_size limit, function returns the total number of characters (not including the
*     terminating null-byte) which would have been written, if the limit was not imposed.
**/
uint8_t snprint_d8(char *buffer, size_t buf_size, uint8_t num);

/**
* \brief Converts an unsigned 16-bit integer into a decimal string
* \param [out] buffer Pointer to a character string to write to. Outputs null-terminated string
* \param [in] buf_size Up to buf_size - 1 characters may be written, plus the null terminator
* \param [in] num Integer to be converted
* \return Number of characters written if successful. If the resulting string gets truncated due to 
*     \c buf_size limit, function returns the total number of characters (not including the
*     terminating null-byte) which would have been written, if the limit was not imposed.
**/
uint8_t snprint_d16(char *buffer, size_t buf_size, uint16_t num);

/**
* \brief Converts an unsigned 32-bit integer into a decimal string
* \param [out] buffer Pointer to a character string to write to. Outputs null-terminated string
* \param [in] buf_size Up to buf_size - 1 characters may be written, plus the null terminator
* \param [in] num Integer to be converted
* \return Number of characters written if successful. If the resulting string gets truncated due to 
*     \c buf_size limit, function returns the total number of characters (not including the
*     terminating null-byte) which would have been written, if the limit was not imposed.
**/
uint8_t snprint_d32(char *buffer, size_t buf_size, uint32_t num);

/**
* \brief Converts a signed 8-bit integer into a decimal string
* \param [out] buffer Pointer to a character string to write to. Outputs null-terminated string
* \param [in] buf_size Up to buf_size - 1 characters may be written, plus the null terminator
* \param [in] num Integer to be converted
* \return Number of characters written if successful. If the resulting string gets truncated due to 
*     \c buf_size limit, function returns the total number of characters (not including the
*     terminating null-byte) which would have been written, if the limit was not imposed.
**/
uint8_t snprint_sd8(char *buffer, size_t buf_size, int8_t num);

/**
* \brief Converts a signed 16-bit integer into a decimal string
* \param [out] buffer Pointer to a character string to write to. Outputs null-terminated string
* \param [in] buf_size Up to buf_size - 1 characters may be written, plus the null terminator
* \param [in] num Integer to be converted
* \return Number of characters written if successful. If the resulting string gets truncated due to 
*     \c buf_size limit, function returns the total number of characters (not including the
*     terminating null-byte) which would have been written, if the limit was not imposed.
**/
uint8_t snprint_sd16(char *buffer, size_t buf_size, int16_t num);

/**
* \brief Converts a signed 32-bit integer into a decimal string
* \param [out] buffer Pointer to a character string to write to. Outputs null-terminated string
* \param [in] buf_size Up to buf_size - 1 characters may be written, plus the null terminator
* \param [in] num Integer to be converted
* \return Number of characters written if successful. If the resulting string gets truncated due to 
*     \c buf_size limit, function returns the total number of characters (not including the
*     terminating null-byte) which would have been written, if the limit was not imposed.
**/
uint8_t snprint_sd32(char *buffer, size_t buf_size, int32_t num);

#ifdef __cplusplus
}
#endif

#endif
///\}
