
#include <stdio.h>
#include "cli_commands.h"

//==================================================================================================
// Device-specific output functions
//==================================================================================================

#include <usb_api.h>
#include <string.h>

void cli_puts(char *str){
    USB_cdcSend(str,strlen(str),0,0);
}

void cli_putc(char chr){
    USB_cdcSend(&chr,1,0,0);
}

void cli_print_prompt(void){
    cli_putc('>');
}

void cli_print_error(int error){
    
}

void cli_print_notfound(char *strcmd){
    cli_puts("Command '");
    cli_puts(strcmd);
    cli_puts("' not found\r\n");
}

//==================================================================================================
// Custom Commands
//==================================================================================================

int cmdHello(uint16_t argc, char *argv[]){
    cli_puts("Hello World\r\n");
    return(0);
}

//--------------------------------------------------------------------------------------------------
int cmdBye(uint16_t argc, char *argv[]){
    cli_puts("Goodbye!\r\n");
    return(0);
}

//--------------------------------------------------------------------------------------------------
int cmdListArgs(uint16_t argc, char *argv[]){
    cli_puts("Argument List:\r\n");
    int i;
    for(i=0;i<argc;i++){
        cli_putc('[');
        cli_puts(argv[i]);
        cli_puts("]\r\n");
    }
    return(0);
}

//--------------------------------------------------------------------------------------------------
int cmdHelp(uint16_t argc, char *argv[]){
    cli_puts("Available commands:\r\n");
    cli_puts("  hi       Print \"Hello World\" message\r\n");
    cli_puts("  bye      Print \"Goodbye!\" message\r\n");
    cli_puts("  listargs List arguments\r\n");
    cli_puts("  help     Prints this message\r\n");
    return(0);
}


