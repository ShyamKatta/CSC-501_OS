/* setnok.c - setnok */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  setnok  -  set next-of-kin (notified at death) for a given process
 *------------------------------------------------------------------------
 */
 unsigned long   ctr1000;
 extern int monitoring;
SYSCALL	setnok(int nok, int pid)
{
	if(monitoring == 1){
		start_timer( 14, currpid);
	}
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid)) {
		restore(ps);
		if(monitoring == 1){
			stop_timer( 14, currpid);
		}
		return(SYSERR);
	}
	pptr = &proctab[pid];
	pptr->pnxtkin = nok;
	restore(ps);
	if(monitoring == 1){
		stop_timer( 14, currpid);
	}
	return(OK);
}
