===============================================================================================
ADDING A MODULE TO YOUR PROJECT: (For managed-make projects)

Adding MyMod example:

-COPY any necessary config headers to your project directory
	COPY	".../modules/MyMod_config.TEMPLATE.h" ---> "PROJECT_DIR/src/MyMod_config.h"
	EDIT	MyMod_config.h according to your project specifications

-LINK any other related C and H files to your project via a reference
	Copying is fine as well however it makes it hard to update a module globally.
	LINK	MyMod.h, MyMod.c, MyMod_internal.h

-Include the header file in your code:
	#include "MyMod.h"

NOTE: Linking to .h files doesn't always work properly. Depending on your IDE, you may have
	to add the .../modules directory to your include search path.
	I'm still trying to figure out a universal and clean way to do this.

===============================================================================================
WRITING A NEW C MODULE

Each module should follow this format for files:

MyMod.mk	Makefile for the module

MyMod.c		Contains code pertaining to the module. User references this file from project
		Must Contain:
		#include "MyMod.h"
		#include "MyMod_internal.h"

MyMod.h		Contains any user-side definitions and protptypes.
		If a #define or function is not useful to the user, DO NOT include it here!
		We want to prevent polluting namespaces
		This is the only file the user needs to explicitly include in their code
		Must Contain:
		#include "MyMod_config.h"
		
MyMod_internal.h
		Contains #defines and prototypes that are ONLY used in the scope of MyMod.c

MyMod_config.TEMPLATE.h
		Contains a template H file for user configuration of the module.
		Since this is an application-specific H file, The user must copy it into
		their project dir and get rid of the ".template" at the end of the filename


//==================================================================================================
// Generic Init and Uninit
//==================================================================================================
/**
* \brief Initializes the DEVICE
* \retval RES_OK
* \retval RES_FAIL Could not initialize DEVICE
**/
RES_t MODULE_init(void)

/**
* \brief Uninitializes the DEVICE
* \retval RES_OK
* \retval RES_FAIL Could not uninitialize DEVICE
**/
RES_t MODULE_uninit(void)

//==================================================================================================
// Generic Raw Binary I/O Functions
//==================================================================================================
/**
* \brief Read data from the DEVICE
* \param [out] buf Destination buffer of the data to be read. A \c NULL pointer discards the data.
* \param [in] size Number of bytes to be read.
* [\param Pointer to the DEVICE object]
* \return Number of bytes read successfully.
**/
size_t MODULE_read(void *buf, size_t size, [object])

/**
* \brief Write data to the DEVICE
* \param [in] buf Pointer to the data to be written.
* \param [in] size Number of bytes to be written.
* [\param Pointer to the DEVICE object]
* \return Number of bytes written successfully.
**/
size_t MODULE_write(void *buf, size_t size, [object])

/**
* \brief Get the number of bytes available to be read from the DEVICE
* [\param Pointer to the DEVICE object]
* \return Number of bytes
**/
size_t MODULE_rdcount([object])

/**
* \brief Get the number of bytes that can be written to the DEVICE
* [\param Pointer to the DEVICE object]
* \return Number of bytes
**/
size_t MODULE_wrcount([object])

/**
* \brief Discard any data that has already been received
* [\param Pointer to the DEVICE object]
**/
void MODULE_rdflush([object])

/**
* \brief <TBD. This description will vary>
* [\param Pointer to the DEVICE object]
**/
void MODULE_wrflush([object])

//==================================================================================================
// Generic String I/O Functions
//==================================================================================================
/**
* \brief Reads the next character from the DEVICE
* \details If a character is not immediately available, function will block until it receives one.
* [\param Pointer to the DEVICE object]
* \return The next available character
**/
char MODULE_getc([object])

/**
* \brief Reads in a string of characters until a new-line character ( \c \\n) is received
* 
* - Reads at most n-1 characters from the DEVICE
* - Resulting string is \e always null-terminated
* - If n is zero, a null character is written to str[0], and the function reads and discards
* 	characters until a new-line character is received.
* - If n-1 characters have been read, the function continues reading and discarding characters until
* 	a new-line character is received.
* - If an entire line is not immediately available, the function will block until it 
* 	receives a new-line character.
* 
* \param [out] str Pointer to the destination string buffer
* \param n The size of the string buffer \c str
* [\param Pointer to the DEVICE object]
* \return \c str on success. \c NULL otherwise
**/
char *MODULE_gets_s(char *str, size_t n)

/**
* \brief Writes a character to the DEVICE
* \param c character to be written
* [\param Pointer to the DEVICE object]
**/
void MODULE_putc(char c, [object])

/**
* \brief Writes a character string to the DEVICE
* \param s Pointer to the Null-terminated string to be sent
* [\param Pointer to the DEVICE object]
**/
void MODULE_puts(char *s, [object])



