/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
 unsigned long   ctr1000;
 extern int monitoring;
SYSCALL getpid()
{	
	if(monitoring ==1){
		start_timer(2, currpid);
	}
	if(monitoring ==1){
		stop_timer(2, currpid);
	}
	return(currpid);
}
