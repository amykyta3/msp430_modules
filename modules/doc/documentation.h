/**
    \mainpage Overview
    
    Welcome to my collection of MSP430 C modules!
    
    Over the years I have developed a variety of drivers and services for TI's MSP430 series  
    microcontrollers. Since these modules have been developed on an "as I need them" basis, the
    collection is far from complete. Instead of letting them sit around unused on my hard-disk, I 
    have decided to release them to be used freely by the public.
    
    Below is a list of some of the programming philosophies I try to stick to when developing these
    modules:
    
    - <b> Configured at compile-time </b> \n
        Firmware design for microcontrollers tends to be quite static. It is rare that a small 
        system's configuration needs to be changed at runtime. For this reason, module configuration
        is generally done at compile-time using configuration *.h files and heavier use of the C
        preprocessor. Doing so reduces code size and RAM usage. Memory is always allocated statically
        (on the stack vs. using malloc) for more robust and deterministic design.
        
    - <b> Universal </b> \n
        These modules have been written to be compatible with as many of the processors as possible.
        Ideally, a program using these modules can be written for a 1xx device and then be 
        recompiled the next day for a higher-performing 6xx device with minimal modification.
        I tend to primarily use the 5xx family of devices so support will inevitably be best for
        those.
        
    - <b> Modular </b> \n
        With a few exceptions, the modules are written so that they can be used as completely
        standalone pieces of firmware that do not rely on other libraries. This makes them easy to
        drop in to a project as needed.
    
    <hr>
    \section SEC_COMPILER_SUPPORT Compiler Support
        Module code and makefiles are primarily designed to use the
        [msp430-elf GCC compiler](http://www.ti.com/tool/msp430-gcc-opensource). However in most
        cases, the TI compiler will work too.
        
    <hr>
    \section SEC_GETTING_STARTED Getting Started
            To help get you started, a few example projects have been included in the download and
            can be found in the "examples" folder.
            
            These projects should be compiled using the included makefiles. This can be done though
            a command line or from within Code Composer Studio by creating a custom makefile project
            (\e not a managed-make)
            
            In order for the makefiles to work, the following environment variables must be set:
                - \c PATH: Add the msp430-elf compiler's \c bin directory.
                - \c MSP430_HEADER_PATH: Set to point to the MSP430 header folder
                  (contains device-specific .h and .ld files)
            
            \b Examples:
                - \ref PG_EXAMPLE_BASIC - Blink some LEDs using a timer and control them with pushbuttons.
                - \ref PG_EXAMPLE_UART - Communicate with the MSP430 using a serial port
                - \ref PG_EXAMPLE_USB - Communicate with the MSP430 using the 5xx/6xx-series USB interface
    
    
    <hr>
    \section SEC_MODULE_BASICS Module Basics
        All code in this library is broken up into separate modules. Each module is made up of a
        collection of related functions, variables, and definitions.
        
        \subsection SEC_MODULE_COMPONENTS Parts of a Module
            Modules in this library are typically made up of several files that follow a consistent 
            naming convention. Some modules may require additional source files while others my only
            use a subset of the ones listed. Given a generic module called MODULE, its files may be
            organized as follows:
            
            - \b MODULE.c
                - Contains the source code for the module
            
            - \b MODULE.h
                - Contains definitions to functions and variables which provide an interface to the
                module.
                - When using a module in your project, this is typically the only header file that
                must be included.
            
            - \b MODULE_internal.h
                - Contains any internal definitions that are probably not useful to the end-user.
            
            - \b MODULE_config.TEMPLATE.h
                - Some modules are configured at compile-time by the pre-processor. This file is a 
                template configuration file that allows the user to modify some aspects of the
                module.
                - If a module comes with one of these files, it \e must be copied to your project's
                local config directory.
                - After copying the file, rename it to \b MODULE_config.h (remove "TEMPLATE." from
                the name)
                - This config directory \e must be added to the compiler's list of \#include search
                paths
            
            - \b MODULE.mk
                - This makefile instructs the compiler how to build the module.
    
    <hr>
    \section SEC_LICENSE Terms of use
        Copyright &copy; 2010-2015 by Alexander Mykyta.
        
        Permission to use, copy, modify, and distribute this software and its documentation is
        hereby granted. No representations are made about the suitability of this software for any
        purpose. It is provided "as is" without express or implied warranty. See the
        \ref PG_BSD_LICENSE for more details. 
***************************************************************************************************/

//==================================================================================================

/**
    \page PG_DOWNLOAD Download

    Libraries can be found at GitHub:
    
    &nbsp;&nbsp;&nbsp;<a href="https://github.com/amykyta3/msp430_modules">GitHub: amykyta3/msp430_modules</a>

***************************************************************************************************/

//==================================================================================================

/**
    \page EVENT_LIST_PAGE Event Listing
    \brief A listing of callback functions that are required by some modules.

***************************************************************************************************/

/**
    \page test Test List
    \brief This page lists modules and functions that have yet to be tested.

***************************************************************************************************/

/**
    \page todo Todo List
    \brief My personal to-do list.

***************************************************************************************************/

/**
    \page bug Bug List
    \brief This page lists all of the known bugs.

***************************************************************************************************/

/**
    \page PG_MODULE_DOC Module Documentation
    
    \moduletable{Peripheral Drivers}
    \moduleentry{MOD_CLOCKSYS,Initializes the system's clock distribution.}
    \moduleentry{MOD_I2C, Application-level I2C master driver.}
    \moduleentry{MOD_SPI,Provides basic functions for the MSP430 SPI controller.}
    \moduleentry{MOD_UART,Provides basic text IO functions for the MSP430 UART controller.}
    \moduleentry{MOD_USB,API for MSP430's USB peripheral.}
    \endmoduletable
    
    \moduletable{Component Drivers}
    \moduleentry{MOD_BUTTON,Debounces pushbutton inputs and generates events based on their state.}
    \moduleentry{MOD_SPI_FLASH,Interface to SPI Flash devices.}
    \endmoduletable
    
    \moduletable{Services}
    \moduleentry{MOD_CLI,Generic Command Line Interface.}
    \moduleentry{MOD_COTHREADS,Cooperative Processor Threads.}
    \moduleentry{MOD_EVENT_QUEUE,A simple first-in first-out event handler.}
    \moduleentry{MOD_FLASHFS,Light-weight file system for Flash volumes.}
    \moduleentry{MOD_TIMER,Timer Driver.}
    \endmoduletable

    \moduletable{Utilities}
    \moduleentry{MOD_FIFO,A generic First-in First-out buffer.}
    \moduleentry{MOD_SLEEP,Sleep functions to kill time.}
    \moduleentry{MOD_STRING_EXT,Additional string functions.}
    \endmoduletable
    
    <hr>
    Related Pages
    -------------
    
    Below are links to some other relevant documentation pages:
    
    - \ref EVENT_LIST_PAGE  A listing of callback functions that are required by some modules.
    - \ref test This page lists modules and functions that have yet to be tested.
    - \ref todo My personal to-do list.
    - \ref bug  This page lists all of the known bugs.
    
    
***************************************************************************************************/

//==================================================================================================

/**
    \page PG_BSD_LICENSE License
    
    All of the module code in this library is protected by a FreeBSD License (below) unless stated
    otherwise at the top of the source file.
    
        Copyright (C) 2015, Alexander I. Mykyta
        All rights reserved.

        Redistribution and use in source and binary forms, with or without
        modification, are permitted provided that the following conditions are met: 

        1. Redistributions of source code must retain the above copyright notice, this
           list of conditions and the following disclaimer. 
        2. Redistributions in binary form must reproduce the above copyright notice,
           this list of conditions and the following disclaimer in the documentation
           and/or other materials provided with the distribution. 

        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
        ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
        WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
        DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
        ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
        (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
        LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
        ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
        (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
        SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
    
***************************************************************************************************/
