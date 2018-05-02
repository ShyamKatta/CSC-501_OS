/* getprio.c - getprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * getprio -- return the scheduling priority of a given process
 *------------------------------------------------------------------------
 */
  unsigned long   ctr1000;
  extern int monitoring;
SYSCALL getprio(int pid)
{	
	if(monitoring == 1){
		start_timer( 3, currpid);
	}
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		if(monitoring == 1){
			stop_timer( 3, currpid);
		}
		return(SYSERR);
	}
	restore(ps);
	if(monitoring == 1){
		stop_timer( 3, currpid);
	}
	return(pptr->pprio);
}
