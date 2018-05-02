/* wait.c - wait */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 * wait  --  make current process wait on a semaphore
 *------------------------------------------------------------------------
 */
  unsigned long   ctr1000;
  extern int monitoring;
SYSCALL	wait(int sem)
{
	if(monitoring == 1){
		start_timer(26, currpid);
	}
	STATWORD ps;    
	struct	sentry	*sptr;
	struct	pentry	*pptr;

	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		if(monitoring == 1){
			stop_timer(26, currpid);
		}
		return(SYSERR);
	}
	
	if (--(sptr->semcnt) < 0) {
		(pptr = &proctab[currpid])->pstate = PRWAIT;
		pptr->psem = sem;
		enqueue(currpid,sptr->sqtail);
		pptr->pwaitret = OK;
		resched();
		restore(ps);
		if(monitoring == 1){
			stop_timer(26, currpid);
		}
		return pptr->pwaitret;
	}
	restore(ps);
	if(monitoring == 1){
		stop_timer(26, currpid);
	}
	return(OK);
}
