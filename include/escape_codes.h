/**
* \file
* \brief Common ANSI/VT100 Terminal Escape Codes
* \author Alex Mykyta
* \date 4/7/2011
**/

#ifndef ESCAPE_CODES_H
#define ESCAPE_CODES_H

///\brief Clears the terminal screen
#define E_CLC   "\x1B[2J\x1B[H"

///\name Text Colors
#define E_BLACK       "\x1B[30m"
#define E_RED         "\x1B[31m"
#define E_GREEN       "\x1B[32m"
#define E_YELLOW      "\x1B[33m"
#define E_BLUE        "\x1B[34m"
#define E_MAGENTA     "\x1B[35m"
#define E_CYAN        "\x1B[36m"
#define E_WHITE       "\x1B[37m"

///\name Background Colors
#define E_BG_BLACK    "\x1B[40m"
#define E_BG_RED      "\x1B[41m"
#define E_BG_GREEN    "\x1B[42m"
#define E_BG_YELLOW   "\x1B[43m"
#define E_BG_BLUE     "\x1B[44m"
#define E_BG_MAGENTA  "\x1B[45m"
#define E_BG_CYAN     "\x1B[46m"
#define E_BG_WHITE    "\x1B[47m"

///\name Text Effects
#define E_F_RST         "\x1B[0m"
#define E_F_BRIGHT      "\x1B[1m"
#define E_F_DIM         "\x1B[2m"
#define E_F_UNDERLINE   "\x1B[4m"
#define E_F_BLINK       "\x1B[5m"
#define E_F_REVERSE     "\x1B[7m"
#define E_F_HIDDEN      "\x1B[8m"

#endif
