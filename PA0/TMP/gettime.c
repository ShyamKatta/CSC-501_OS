/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include <proc.h>
#include <lab0.h>
extern int getutim(unsigned long *);
extern int monitoring;
/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */
  unsigned long   ctr1000;
SYSCALL	gettime(long *timvar)
{
	if(monitoring == 1){
		start_timer( 4, currpid);
	}
    /* long	now; */

	/* FIXME -- no getutim */
	if(monitoring == 1){
		stop_timer( 4, currpid);
	}
    return OK;
}
