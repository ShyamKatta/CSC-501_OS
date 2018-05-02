/* sleep1000.c - sleep1000 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * sleep1000 --  delay the caller for a time specified in 1/100 of seconds
 *------------------------------------------------------------------------
 */
 unsigned long   ctr1000;
 extern int monitoring;
SYSCALL sleep1000(int n)
{
	if(monitoring == 1){
		start_timer( 21, currpid);
	}
	STATWORD ps;    

	if (n < 0  || clkruns==0){
		if(monitoring == 1){
			stop_timer( 21, currpid);
		}
		return(SYSERR);
	}
	disable(ps);
	if (n == 0) {		/* sleep1000(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);
	if(monitoring == 1){
		stop_timer( 21, currpid);
	}
	return(OK);
}
