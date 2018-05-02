/* lcreate.c - lscreate, newlock */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

unsigned long   ctr1000;	//timer variable
LOCAL int newlck();

/*------------------------------------------------------------------------
 * lcreate  --  create and initialize a lock, returning its lock id
 *------------------------------------------------------------------------
 */
SYSCALL lcreate()
{
	STATWORD ps;    
	int	lck;

	disable(ps);
	if ((lck=newlck())==SYSERR ) {
		restore(ps);
		return(SYSERR);
	}
	// lock[lck].lckcnt = count;	// store the count in lock structure
	/* lqhead and lqtail were initialized at system startup in initialize.c, linit() */
	restore(ps);
	return(lck);
}

/*------------------------------------------------------------------------
 * newlck  --  allocate an unused lock and return its index
 *------------------------------------------------------------------------
 */
LOCAL int newlck()
{
	int	lck;
	int	i;

	for (i=0 ; i<NLOCKS ; i++) {
		lck=nextlck--;
		if (nextlck < 0)
			nextlck = NLOCKS-1;
		if (lockarr[lck].lstate==LFREE) {
			//lockarr[lck].lock_creation_time= ctr1000;
			lockarr[lck].readercount=0;
			lockarr[lck].lstate = LUSED;
			return(lck);
		}
	}
	return(SYSERR);
}
