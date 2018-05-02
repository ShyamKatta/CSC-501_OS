/* signal.c - signal */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * signal  --  signal a semaphore, releasing one waiting process
 *------------------------------------------------------------------------
 */
  unsigned long   ctr1000;
  extern int monitoring;
SYSCALL signal(int sem)
{
	if(monitoring == 1){
		start_timer( 16, currpid);
	}
	STATWORD ps;    
	register struct	sentry	*sptr;

	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		if(monitoring == 1){
			stop_timer( 16, currpid);
		}
		return(SYSERR);
	}
	if ((sptr->semcnt++) < 0)
		ready(getfirst(sptr->sqhead), RESCHYES);
	restore(ps);
	if(monitoring == 1){
		stop_timer( 16, currpid);
	}
	return(OK);
}
