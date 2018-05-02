/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
 void update_wait_queue_priority(int);
 void update_lock_acquired_process_priorities(int);
 
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;
	int ldes1,i,max_priority=0;	// delcared for priority inheretence

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	
		
		pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	
		// set all locks acquired by this process to 0 in lockarr
		// should remain only in one queue, so update the queue details and update process which acquired corresponding lock.
		//kprintf("process to kill is %d, prio is %d, pinh prio is %d\n",pid,proctab[pid].pprio, proctab[pid].pinh);
		ldes1 = proctab[pid].waiting_for_lock;
		//kprintf("ldes1 = %d",ldes1);
		dequeue(pid);
		//proctab[pid][ldes1].inh_lock_hold[pid]=0; update all locks hold by this process to 0
		lockarr[ldes1].process_holding_lock[pid]=0;
		proctab[pid].waiting_for_lock = -1;
		// itereate in waiting queue and update max priority of updated queue;
		update_wait_queue_priority(ldes1);
		
		//kprintf("max prio is %d\n",max_priority);
		//update priorities of lock acquired process with new max_priority
		update_lock_acquired_process_priorities(ldes1);
		
		semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	return(OK);
}

void update_wait_queue_priority(int ldes1){
	int max_priority=0, i;
	for(i=0; i<NPROC; i++){
		if(proctab[i].waiting_for_lock==ldes1){
			if(proctab[i].pprio>max_priority){
				max_priority= proctab[i].pprio;
			}
		}
	}
	lockarr[ldes1].max_priority=max_priority;
	//return max_priority;
	//kprintf("max prio of queue %d", max_priority);

}
void update_lock_acquired_process_priorities(int ldes1 ){
	int i;
	int max_priority= lockarr[ldes1].max_priority;
	for(i=0;i<NPROC; i++){
		// pinh==0 ==> pprio has high priority than all process in lock queue, so deleting a p process in queue will not affect
		if(lockarr[ldes1].process_holding_lock[i]==1){
			if(proctab[i].pinh>0){
				if(proctab[i].pinh>max_priority){
					proctab[i].pprio = proctab[i].pinh;
					proctab[i].pinh=0;
				}
				else{
					proctab[i].pprio = max_priority;
					chpriorecursive(i,max_priority);	// just update pinh prio
				}
			}
			else{
				if(max_priority>proctab[i].pprio){
					proctab[i].pinh = proctab[i].pprio;	// copy original to inherited
					proctab[i].pprio = max_priority;
					chpriorecursive(i,max_priority);
				}
			}
		}
	}
}