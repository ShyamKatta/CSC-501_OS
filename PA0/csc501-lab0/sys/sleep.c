/* sleep.c - sleep */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * sleep  --  delay the calling process n seconds
 *------------------------------------------------------------------------
 */

//extern int count_sys_calls[27];

unsigned long   ctr1000;
extern int monitoring;
SYSCALL	sleep(int n)
{
	
	if(monitoring == 1){
		start_timer( 18, currpid);
	}

	STATWORD ps;    
	if (n<0 || clkruns==0){
		if(monitoring == 1){
			stop_timer( 18, currpid);
		}
		return(SYSERR);
	}
	if (n == 0) {
	        disable(ps);
		resched();
		restore(ps);
		if(monitoring == 1){
		stop_timer( 18, currpid);
		}
		return(OK);
	}
	while (n >= 1000) {
		sleep10(10000);
		n -= 1000;
	}
	if (n > 0)
		sleep10(10*n);
	if(monitoring ==1){
		stop_timer(18, currpid);
	}
	return(OK);
}
