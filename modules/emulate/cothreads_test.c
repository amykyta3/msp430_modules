
#include <stdio.h>
#include <unistd.h>

#include "cothread.h"

uint8_t alt_stack[64];
cothread_t ct_home;
cothread_t ct_alt1;
cothread_t ct_alt2;





int ct_f1(void){
	sleep(1);
	printf("a1\n");
	cothread_switch(&ct_home);
	
	sleep(1);
	printf("a2\n");
	cothread_switch(&ct_home);
	
	cothread_exit(0);
	
	sleep(1);
	printf("a3\n");
	cothread_switch(&ct_home);
	
	sleep(1);
	printf("a-exit\n");
	
	return(0);
}




int main(void){
	
	cothread_init(&ct_home);
	
	ct_alt1.alt_stack = alt_stack;
	ct_alt1.alt_stack_size = sizeof(alt_stack);
	ct_alt1.co_exit = &ct_home;
	cothread_create(&ct_alt1,ct_f1);
	
	printf("h1\n");
	cothread_switch(&ct_alt1);
	
	printf("h2\n");
	cothread_switch(&ct_alt1);
	
	printf("h3\n");
	cothread_switch(&ct_alt1);
	
	printf("h4\n");
	cothread_switch(&ct_alt1);
	
	printf("h5\n");
	cothread_switch(&ct_alt1);
	
	printf("h6\n");
	cothread_switch(&ct_alt1);
	
	printf("h7\n");
	cothread_switch(&ct_alt1);
	
}
