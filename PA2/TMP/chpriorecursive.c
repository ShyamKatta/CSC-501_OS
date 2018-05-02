/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
void update_acquired_locks_recursive(int, int);

int chpriorecursive(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	//pptr->pprio = newprio;
	
	//	process is not waiting for any lock
	update_acquired_locks_recursive(pid, newprio);
	if(proctab[pid].waiting_for_lock!=-1){
		int ldes1 = proctab[pid].waiting_for_lock;
		update_wait_queue_priority(ldes1);
		update_lock_acquired_process_priorities(ldes1);
	}
	
	restore(ps);
	return(newprio);
}

//	update maximum priority among all lock queues acquired by current process
void update_acquired_locks_recursive(int pid, int newprio){
	int i;
	int max_prio_all_locks=0;	// maximum priority among all locks acquired

	//update_wait_queue_priority();
	for(i=0; i<NPROC; i++){
		if(proctab[pid].inh_lock_hold[i]==1){
			max_prio_all_locks = (max_prio_all_locks<lockarr[i].max_priority)? lockarr[i].max_priority : max_prio_all_locks;
		}
	}
	if(proctab[pid].pinh==0){	//	
		if(newprio>proctab[pid].pprio){
			proctab[pid].pinh=proctab[pid].pprio;
			proctab[pid].pprio=newprio;
		}
	}
	else{
		if(newprio>proctab[pid].pprio){
			proctab[pid].pprio=newprio;
		}
	}	
	//kprintf("max_prio_all_locks %d", max_prio_all_locks);
}