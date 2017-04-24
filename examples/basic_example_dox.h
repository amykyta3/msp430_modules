 
/**
    \page PG_EXAMPLE_BASIC Basic Example
    \brief Blink some LEDs using a timer and control them with pushbuttons.
    \tableofcontents
    
    \section SEC_EX_BAS1 1. Introduction
        \par
        This example demonstrates some basic modules and can be run without modifications on
        TI's [MSP430F5529 LaunchPad](http://www.ti.com/tool/msp-exp430f5529lp) or the
        [MSP430FG4618/F2013 Experimenter Board](http://www.ti.com/tool/msp-exp430fg4618).
        Choosing between the two can be done by uncommenting the appropriate processor variable in
        the Makefile.
        
        \par
        If you don't have either of the kits, all you need is an MSP430 connected to:
            - Two LEDs
            - Two pushbuttons
            - A 32.768 kHz crystal on XT1.
        
        \par
        This example demonstrates the use of the following modules:
            - \ref MOD_CLOCKSYS
            - \ref MOD_EVENT_QUEUE which also uses \ref MOD_FIFO internally.
            - \ref MOD_BUTTON
            - \ref MOD_TIMER
        
        \par
        In this example, a timer is used to blink two LEDs at different rates. Two pushbuttons
        pause and resume one of the LED's blinking.
        
    \section SEC_EX_BAS2 2. Description
        \par
        First, several header files are included:
        \dontinclude basic_example/main.c
        \skip msp430_xc.h
        \until timer.h
        The msp430_xc.h header is just like TI's msp430.h header except that it also provides
        several compatibility macros between the TI and GCC compilers.
        
        \par
        In this example, the preprocessor is used to control port configuration of the two
        development boards. If compiling for the x5529, pins will be set up for the Launchpad. If
        compiling for the x4618, settings for the Experimenter's board will be assumed.
        \skip #if
        \until #endif
        
        \subsection SEC_EX_BAS2_1 2.1 The main() Routine
            \par
            At the start of the main() routine, the board_init() function is called where the ports
            for the two LEDs are configured. Next, all of the modules being used are initialized:
            \skip clock_init
            \until button_SetupPort
            \until button_SetupPort
            clock_init() is almost always the first module to be initialized since others often
            depend on its configuration. Next, the remaining modules init routines are called. 
            The \ref MOD_BUTTON module is set up to respond to buttons on P2.1 and P1.1. Since these
            are active-low pull-down buttons, inversion of those bits is enabled.
            
            \par
            Next, two timers are initialized and started with different intervals. For the first 
            timer, a trigger period of 400 milliseconds is used. The timer is set to repeat 
            indefinitely. Whenever the timer expires, it will trigger the event function 
            OnTimerExpire1().
            \skip struct timerctl timer_settings
            \until timer_start(&Timer1,&timer_settings)
            The second timer is identical except it uses an interval of 500 ms
            
            \par
            Finally, since the pushbuttons and the three timers will be generating events, the event
            handler must be started.
            \skip __enable_interrupt
            \until event_StartHandler
            \b Note: The event_StartHandler() routine does not return. This contains the main loop for
            the program. Everything else is now handled using events and interrupts.
            
            \par
            That is all for the main() routine!
        
        \subsection SEC_EX_BAS2_2 2.2 Timer events
            \dontinclude basic_example/main.c
            \par
            Each of the timers have been set up to trigger their own separate events. All that each
            event function does is toggle the state of each of the LEDs.
                \skip void OnTimerExpire1
                \until void OnTimerExpire2
                \until }
        
        \subsection SEC_EX_BAS2_3 2.3 Pushbutton Events
            \par
            Lastly we want the two pushbuttons to do something. The following code makes one
            pushbutton start the first timer and the other button stops the timer.
            \skip onButtonDown
            \until onButtonHold
            \until }
            The other two events, onButtonUp() and onButtonHold() remain unused in this example.
            
***************************************************************************************************/
