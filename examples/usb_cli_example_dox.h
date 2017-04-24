 
/**
    \page PG_EXAMPLE_USB USB CDC Example
    \brief Communicate to the MSP430 using a command-line interface using the 5xx/6xx-series USB
    \tableofcontents
    
    \section SEC_EX_USB1 1. Introduction
        \par
        This example demonstrates a USB CDC interface (Virtual COM port in Windows-speak) using the
        MSP430 5xx/6xx-series USB peripheral. All text IO is handled using the \ref MOD_CLI module
        to implement several simple commands.
        
        \par
        Any MSP430 development board that uses a 5xx/6xx series processor with USB will work. This
        includes TI's [MSP430F5529 LaunchPad](http://www.ti.com/tool/msp-exp430f5529lp).
        
        \par
        This example assumes there is a 4 MHz crystal oscillator attached to the appropriate XT2 pins.
        If using something other than the MSP430F5529, only the port initialization for the XT2 pins
        will need to be changed.
        
        \par
        This example demonstrates the use of the following modules:
            - \ref MOD_CLOCKSYS
            - \ref MOD_EVENT_QUEUE which also uses \ref MOD_FIFO internally.
            - \ref MOD_USB which also uses \ref MOD_SLEEP internally.
            - \ref MOD_CLI
        
    <hr>
    \section SEC_EX_USB2 2. Description
        
        \subsection SEC_EX_USB2_1 2.1 The main.c File
            \dontinclude usb_cli_example/main.c
            
            \subsubsection SEC_EX_USB2_1_1 2.1.1 The main() Routine
                \par
                As always, our program's entry point is main() where several things are initialized.
                Since the USB's PLL requires a stable clock source, the XT2 pins are set to their
                special function. All of the module init functions are called in order of their
                dependence
                \skip WDTCTL
                \until USB_init()
                
                \par
                Some further initialization of the USB API is required. Since we want the program to
                react to any characters received, the corresponding event is enabled. We also want
                to connect and disconnect the USB every time the cable is inserted and removed. This
                is handled using events.
                \skipline USB_setEnabledEvents
                
                \par
                At this point the USB interface has been initialized. In the special case that the 
                USB cable is already inserted into the host, we want to enable the connection so the
                host can enumerate it:
                \skip USB_VBUS_PRESENT
                \until }
                \until }
                
                \par
                Finally, the event-handler is started:
                \skip __enable_interrupt
                \until event_StartHandler
                
            \subsubsection SEC_EX_USB2_1_2 2.1.2 Event Handlers
                \par
                The previous section mentioned several events that are generated. Just below the 
                main() routine, they are handled.
                
                \par
                First, the USB API has been configured to generate events when the USB +5v is
                detected or lost. In these cases, we want to automatically connect and disconnect
                the USB device:
                \skip onUSB_Event
                \until default
                \until }
                \until }
                
                \par
                The other event that was enabled was the CDC Data Received event. Here we handle the
                incoming data. In this example, the data is read from the USB API and then fed into
                the \ref MOD_CLI module's character processor. This automates the process of parsing
                incoming text commands and then reacting to them.
                \skip onUSB_InterfaceEvent
                \until cli_process_char
                \until }
                \until }
                \until }
            
        \subsection SEC_EX_USB2_2 2.2 CLI Commands
            \par
            The whole purpose of the CLI module is to take a stream of incoming characters, parse
            them, and call the appropriate command function. This example uses a USB CDC interface
            but it can just as easily be adapted to use UART or something else.
            
            \par
            Each command mnemonic is easily linked to the corresponding command function in the 
            config/cli_commands.h file. This is done using a command table. The header file must 
            also include the function prototypes for the command functions.
            \dontinclude usb_cli_example/config/cli_commands.h
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
            \dontinclude usb_cli_example/config/cli_commands.c
            
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
            \dontinclude usb_cli_example/config/cli_commands.c
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
            
            
        \subsection SEC_EX_USB2_3 2.3 USB Configuration
            \par
            The files found in the config/USB_config folder are generated using TI's 
            [MSP430 USB Descriptor Tool](http://www.ti.com/tool/msp430usbdevpack)
    
***************************************************************************************************/
