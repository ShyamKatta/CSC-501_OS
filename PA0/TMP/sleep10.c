/* sleep10.c - sleep10 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * sleep10  --  delay the caller for a time specified in tenths of seconds
 *------------------------------------------------------------------------
 */
 
 unsigned long   ctr1000;
 extern int monitoring;
 
SYSCALL	sleep10(int n)
{
	if(monitoring ==1){
		start_timer(19, currpid);
	}
	STATWORD ps;    
	if (n < 0  || clkruns==0){
		if(monitoring == 1){
			stop_timer( 19, currpid);
		}
		return(SYSERR);
	}
	disable(ps);
	if (n == 0) {		/* sleep10(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n*100);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	//kprintf("5");
	resched();
	//kprintf("7");
        restore(ps);
	if(monitoring ==1){
		stop_timer(19, currpid);
	}
	return(OK);
}
