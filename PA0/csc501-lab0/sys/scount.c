/* scount.c - scount */

#include <conf.h>
#include <kernel.h>
#include <sem.h>
#include <proc.h>
#include <lab0.h>

/*------------------------------------------------------------------------
 *  scount  --  return a semaphore count
 *------------------------------------------------------------------------
 */
  unsigned long   ctr1000;
  extern int monitoring;
SYSCALL scount(int sem)
{
	if(monitoring == 1){
		start_timer( 10, currpid);
	}
	
	extern	struct	sentry	semaph[];

	if (isbadsem(sem) || semaph[sem].sstate==SFREE){
		if(monitoring == 1){
			stop_timer( 10, currpid);
		}
		return(SYSERR);
	}
	if(monitoring == 1){
		stop_timer( 10, currpid);
	}
	return(semaph[sem].semcnt);
}
