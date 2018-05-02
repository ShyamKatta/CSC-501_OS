/* sdelete.c - sdelete */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * ldelete  --  delete a lock by releasing its table entry
 *------------------------------------------------------------------------
 */
SYSCALL ldelete(int lck)
{
	STATWORD ps;    
	int	pid;
	struct	lentry	*lptr;

	disable(ps);
	if (isbadlck(lck) || lockarr[lck].lstate==LFREE) {
		restore(ps);
		return(SYSERR);
	}
	lptr = &lockarr[lck];
	lptr->lstate = LFREE;
	int i;
	for(i=0; i<NPROC; i++){
		if(lockarr[lck].requested_lock[i]==1)
			lockarr[lck].requested_lock[i]=-1;
	}
	if (nonempty(lptr->lqhead)) {
		while( (pid=getfirst(lptr->lqhead)) != EMPTY)
		  {
		    proctab[pid].pwaitret = DELETED;	// change logic here
		    ready(pid,RESCHNO);
		  }
		resched();
		restore(ps);
		return(DELETED);		// define deleted in kernel.h
	}
	restore(ps);
	return(OK);
}
