/* setdev.c - setdev */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  setdev  -  set the two device entries in the process table entry
 *------------------------------------------------------------------------
 */
 unsigned long   ctr1000;
 extern int monitoring;
SYSCALL	setdev(int pid, int dev1, int dev2)
{
	if(monitoring == 1){
		start_timer( 13, currpid);
	}
	short	*nxtdev;

	if (isbadpid(pid)){
		if(monitoring == 1){
			stop_timer( 13, currpid);
		}	
		return(SYSERR);
	}	
	nxtdev = (short *) proctab[pid].pdevs;
	*nxtdev++ = dev1;
	*nxtdev = dev2;
	if(monitoring == 1){
		stop_timer( 13, currpid);
	}
	return(OK);
}
