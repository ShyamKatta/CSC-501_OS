//#include <conf.h>
//#include <kernel.h>

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

static unsigned long    *esp;
static unsigned long    *ebp;

/*------------------------------------------------------------------------
 *  printtos  -- print top of stack system and 4 elements below top of stack
 *------------------------------------------------------------------------
 */

void printtos()
{

	asm("movl %esp,esp");
	asm("movl %ebp,ebp");
    kprintf("Before [0x%08x]: 0x%08x\n",ebp+2, *(ebp+2));
    kprintf("After [0x%08x]: 0x%08x\n",ebp, *ebp);
	int i;
	
	for(i=0;i<4;i++){
		esp = esp+1;
		if(esp>=ebp){	// condition to check, print elements only above stack base pointer
			break;
		}
		kprintf("\t element[0x%08x]: 0x%08x\n", esp, *esp);
	}
	
		
}

