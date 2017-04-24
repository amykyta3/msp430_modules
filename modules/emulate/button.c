#include <stdint.h>
#include "button.h"
#include "event_queue.h"


//==================================================================================================
// Internal Functions
//==================================================================================================
static void buttonDownEventProcess(void){
	struct{
		uint8_t port;
		uint8_t flags;
	}b;
	event_PopEventData(&b,sizeof(b));
	onButtonDown(b.port,b.flags);
}

static void buttonUpEventProcess(void){
	struct{
		uint8_t port;
		uint8_t flags;
	}b;
	event_PopEventData(&b,sizeof(b));
	onButtonUp(b.port,b.flags);
}

static void buttonHoldEventProcess(void){
	struct b{
		uint8_t port;
		uint8_t flags;
	}b;
	event_PopEventData(&b,sizeof(b));
	onButtonHold(b.port,b.flags);
}

//--------------------------------------------------------------------------------------------------

void button_emu_down(uint8_t bits){
	struct{
		uint8_t port;
		uint8_t flags;
	}b;
	
	b.port = 1;
	b.flags = bits;
	
	event_PushEvent(buttonDownEventProcess,&b,sizeof(b));
}

void button_emu_up(uint8_t bits){
	struct{
		uint8_t port;
		uint8_t flags;
	}b;
	
	b.port = 1;
	b.flags = bits;
	
	event_PushEvent(buttonUpEventProcess,&b,sizeof(b));
}

void button_emu_hold(uint8_t bits){
	struct{
		uint8_t port;
		uint8_t flags;
	}b;
	
	b.port = 1;
	b.flags = bits;
	
	event_PushEvent(buttonHoldEventProcess,&b,sizeof(b));
}

//==================================================================================================
// User Functions
//==================================================================================================
void button_init(void){
	
}

void button_uninit(void){
	
}

void button_SetupPort(uint8_t en,uint8_t inverted, uint8_t port){
	
}


