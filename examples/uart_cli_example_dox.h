 
/**
    \page PG_EXAMPLE_UART UART CLI Example
    \brief Communicate to the MSP430 using a command-line interface using the UART
    \tableofcontents
    
    \section SEC_EX_UART1 1. Introduction
        \par
        This example demonstrates a basic UART command-line interface using the MSP430 UART
        peripheral. All text IO is handled using the \ref MOD_CLI module to implement several simple
        commands.
        
        \par
        The example can be run without modifications on TI's
        [MSP430F5529 LaunchPad](http://www.ti.com/tool/msp-exp430f5529lp) or the
        [MSP430FG4618/F2013 Experimenter Board](http://www.ti.com/tool/msp-exp430fg4618).
        Choosing between the two can be done by uncommenting the appropriate processor variable in
        the Makefile.
        
        \par
        This example demonstrates the use of the following modules:
            - \ref MOD_CLOCKSYS
            - \ref MOD_UART which also uses \ref MOD_FIFO internally.
            - \ref MOD_SLEEP
            - \ref MOD_CLI
        
    <hr>
    \section SEC_EX_UART2 2. Description
        
        \subsection SEC_EX_UART2_1 2.1 The main() Routine
            \dontinclude uart_cli_example/main.c
            
            \par
            At the program's main() entry point, several things are initialized.
            Since the UART requires a stable baud rate reference, the XT2 pins are set to their
            special function. All of the module init functions are called in order of their
            dependence
            \skip UART_PORT_SEL
            \until uart_init()
            
            \par
            At this point, everything is ready to go. Interrupts are enabled, and a main program
            loop polls the UART for characters received. Any chars received using uart_getc()
            are passed into the \ref MOD_CLI module's character processor function. The
            cli_process_char() function collects received characters, parses the command
            arguments, and calls the appropriate command function.
            \skip while
            \until }
            
        \subsection SEC_EX_UART2_2 2.2 DMA Interrupt
            \dontinclude uart_cli_example/main.c
            
            \par
            By default, the \ref MOD_UART module is configured to receive characters using DMA.
            Since MSP430's use one common DMA interrupt vector for all DMA channels, the DMA
            interrupt must be implemented by the user. The \ref MOD_UART module supplies two simple
            functions to do so.
            \skip #if(UIO_RX_MODE == 2)
            \until #endif
            
        \subsection SEC_EX_UART2_3 2.3 CLI Commands
            \par
            The whole purpose of the CLI module is to take a stream of incoming characters, parse
            them, and call the appropriate command function. This example uses a UART interface
            but it can just as easily be adapted to use USB or something else.
            
            \par
            Each command mnemonic is easily linked to the corresponding command function in the 
            config/cli_commands.h file. This is done using a command table. The header file must 
            also include the function prototypes for the command functions.
            \dontinclude uart_cli_example/config/cli_commands.h
            \skip CMDTABLE
            \until int cmdListArgs
            
            \warning Because of how the command-lookup works, the table's command mnemonics 
            \e must be in ASCII alphabetical order! That means a command word starting with a 
            lower-case 'a' comes \e after a command that starts with an upper-case 'Z'. See the 
            [ASCII wiki page](http://en.wikipedia.org/wiki/Ascii#ASCII_printable_characters) for
            more details.
            
            \par
            So, in this table we linked up three commands:
                - The text "listargs" will execute the cmdListArgs() function
                - The text "bye" will execute the cmdBye() function
                - The text "hi" will execute the cmdHello() function
                - The text "help" will execute the cmdHelp() function
            The implementation of these commands can be found in config/cli_commands.c
            \dontinclude uart_cli_example/config/cli_commands.c
            
            \par
            For the "listargs" function, the program will simply echo back a list of arguments
            passed to the command. Note that each command function receives an argc and argv
            argument. These can be used exactly the same way you would when writing a C main()
            routine on a PC.
            \skip cmdListArgs
            \until return
            
            \par
            You may notice that all text output is done using the cli_putc() and cli_puts()
            functions. These functions must be defined for the USB interface and can be found in the
            same cli_commands.c file.
            \dontinclude uart_cli_example/config/cli_commands.c
            \skip cli_puts
            \until cli_putc
            \until }
            
            \par
            Lastly, a couple more abstraction functions must be defined:
            \skip cli_print_prompt
            \until cli_print_notfound
            \until }
            
            \par
            - cli_print_prompt() defines what (if anything) gets echoed back to the host as a
              prompt for a new command.
            - cli_print_error() allows you to output something if a command returns a non-zero
              response. This example does not print anything in this case.
            - cli_print_notfound() lets you customize the message if a command was not found in
              the command table.
            
    
***************************************************************************************************/
