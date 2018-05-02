/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
 extern int monitoring;
SYSCALL chprio(int pid, int newprio)
{
	if(monitoring == 1){
		start_timer( 1, currpid);
	}
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		if(monitoring == 1){
			stop_timer( 1, currpid);
		}
		return(SYSERR);
	}
	pptr->pprio = newprio;
	restore(ps);
	if(monitoring == 1){
		stop_timer( 1, currpid);
	}
	return(newprio);
}
