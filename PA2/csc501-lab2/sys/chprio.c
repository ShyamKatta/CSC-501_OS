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
void update_acquired_locks(int, int);

SYSCALL chprio(int pid, int newprio)
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
	update_acquired_locks(pid, newprio);
	
	if(proctab[pid].waiting_for_lock!=-1){
		int ldes1 = proctab[pid].waiting_for_lock;
		update_wait_queue_priority(ldes1);
		update_lock_acquired_process_priorities(ldes1); // declared in kill.c
	}
	
	restore(ps);
	return(newprio);
}

//	update maximum priority among all lock queues acquired by current process
void update_acquired_locks(int pid, int newprio){
	int i;
	int max_prio_all_locks=0;	// maximum priority among all locks acquired

	//update_wait_queue_priority();
	for(i=0; i<NPROC; i++){
		if(proctab[pid].inh_lock_hold[i]==1){
			max_prio_all_locks = (max_prio_all_locks<lockarr[i].max_priority)? lockarr[i].max_priority : max_prio_all_locks;
		}
		
	}
	//kprintf("max is %d",max_prio_all_locks);
	if(proctab[pid].pinh==0){	//	when we do not have inhereted priority
		if(newprio<max_prio_all_locks){
			proctab[pid].pprio=max_prio_all_locks;
			proctab[pid].pinh=newprio;
		}
		else{
			proctab[pid].pprio=newprio;
			proctab[pid].pinh=0;
		}
	}
	else{
		if(newprio<max_prio_all_locks){	// if inhereted priority is larger then update original priority retaining inhereted priority
			proctab[pid].pprio=max_prio_all_locks;
			proctab[pid].pinh=newprio;
		}
		else{	// if new priority is larger than inhereted then remove inhereted priority and save original priority
			proctab[pid].pprio=newprio;
			proctab[pid].pinh=0;
		}
	}	
}