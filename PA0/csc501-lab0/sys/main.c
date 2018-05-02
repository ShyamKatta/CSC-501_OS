/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
//	static unsigned long    *esp;


int prX;
void halt();

prch(c)
char c;
{
	int i;
	sleep(5);	
}
int main()
{
	kprintf("\n\nHello World, Xinu lives\n\n");
	//zfunction(0xaabbccdd, 0xffc03ff);	
	kprintf("%8X \n",zfunction(0xaabbccdd));
    kprintf("\nvoid printsegaddress()\n");

	printsegaddress();
	//temptest(-1);
        kprintf("\nvoid printtos()\n");
	
	printtos();
        kprintf("\nvoid printprocstks(int priority)\n");

	printprocstks(1);
	kprintf("\nvoid printsyscallsummary()\n");
	syscallsummary_start();

	//sleep(10);
	//sleep10(10);
	//sleep100(10);
	resume(prX = create(prch,2000,20,"proc X",1,'A'));
	sleep(10);
	syscallsummary_stop();
	printsyscallsummary();
	return 0;
}
