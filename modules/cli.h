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
* \addtogroup MOD_CLI Command Line Interface
* \brief Generic Command Line Interface
* \author Alex Mykyta 
*
* This module implements a generic command line interface that can be attached to any character IO
* stream. Custom command functions can be executed by the user through the CLI.
*
* <b> Compilers Supported: </b>
*    - Any C89 compatible or newer
*
* \{
**/

/**
* \file
* \brief Include file for \ref MOD_CLI "Command Line Interface"
* \author Alex Mykyta 
**/

#ifndef CLI_H
#define CLI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct{
    char* strCommand;
    int (* cmdptr)(uint16_t argc, char *argv[]);
} cmdentry_t;

/** 
* \brief Incoming character processing function.
* 
* Pass each incoming character into this function. Strings are parsed and the appropriate command
* function is called. Any following words are passed in as arguments.
**/
void cli_process_char(char inchar);

void cli_puts(char *str);
void cli_putc(char chr);
void cli_print_prompt(void);
void cli_echo_off(void);
void cli_echo_on(void);

#ifdef __cplusplus
}
#endif

#endif

///\}
