#ifndef CLI_COMMANDS_H
#define CLI_COMMANDS_H

#include <cli.h>
#include <stdint.h>

// If set to 1, command parsing recognizes multi-word arguments in quotes
#define PARSE_QUOTED_ARGS   1

// If set to 1, performs command lookup using a binary search instead of linear.
#define USE_BINARY_SEARCH   0

// maximum length of a command input line
#define CLI_STRBUF_SIZE    32

// Maximum number of arguments in a command (including command).
#define CLI_MAX_ARGC    5

// Table of commands: {"command_word" , function_name }
// Command words MUST be in alphabetical (ascii) order!! (A-Z then a-z)
#define CMDTABLE    {"bye"      , cmdBye      },\
                    {"help"     , cmdHelp     },\
                    {"hi"       , cmdHello    },\
                    {"listargs" , cmdListArgs }

// Custom command function prototypes:
int cmdBye(uint16_t argc, char *argv[]);
int cmdHelp(uint16_t argc, char *argv[]);
int cmdHello(uint16_t argc, char *argv[]);
int cmdListArgs(uint16_t argc, char *argv[]);

#endif
