
#include <stdio.h>
#include "cli_commands.h"

//==================================================================================================
// Device-specific output functions
//==================================================================================================

#include <stdio.h>

void cli_puts(char *str){
    printf(str); // Example using stdio.h
}

void cli_putc(char chr){
    putchar(chr); // Example using stdio.h
}

void cli_print_prompt(void){
    cli_puts("\r\n>");
}

void cli_print_error(int error){
    printf("Returned with error code %d\r\n", error); // Example using stdio.h
}

void cli_print_notfound(char *strcmd){
    cli_puts("Command not found\r\n");
}

//==================================================================================================
// Custom Commands
//==================================================================================================

int cmdHello(uint16_t argc, char *argv[]){
    cli_puts("Hello World\r\n");
    return(0);
}

//--------------------------------------------------------------------------------------------------
int cmdArgList(uint16_t argc, char *argv[]){
    cli_puts("Argument List:\r\n");
    int i;
    for(i=0;i<argc;i++){
        cli_putc('[');
        cli_puts(argv[i]);
        cli_puts("]\r\n");
    }
    return(0);
}

