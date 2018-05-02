/* wait.c - wait */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

unsigned long   ctr1000;	//timer variable
int insertLock(int, int, int);
int gettail(int );
void run_inherit(int);
/*------------------------------------------------------------------------
 * lock  --  make current process wait on a semaphore
 *------------------------------------------------------------------------
 */
SYSCALL lock(int ldes1, int type, int priority){
	STATWORD ps;
	
	disable(ps);
	/* if(proctab[currpid].lock_time_copy[ldes1]==0){
		//proctab[currpid].prev_access=1;
		proctab[currpid].lock_time_copy[ldes1] = lockarr[ldes1].lock_creation_time;
	} */
	//	logic to check deleted process
	/* if(lockarr[ldes1].requested_lock[currpid]==-1) //|| proctab[currpid].lock_time_copy[ldes1] != lockarr[ldes1].lock_creation_time 
	{
		restore(ps);
		return(SYSERR);
	} */
	
	if (isbadlck(ldes1) || (lockarr[ldes1].lstate==LFREE) || (lockarr[ldes1].requested_lock[currpid]==-1) ) {
		//kprintf("SYSERR");
		restore(ps);
		return(SYSERR);
	}
	// check when to call resched
	// check is all locks are saved correctly when acquired in proctab
	
	//struct pentry *pptr;
	//pptr = &proctab[currpid];
	
	//proctab[currpid].lock_request_time = ctr1000;
	proctab[currpid].lockhold[ldes1] = type;	// save this type lock is acquired
	proctab[currpid].lpriority = priority;
	proctab[currpid].lmode = type;
	//lockarr[ldes1].locktype=type;

	if (lockarr[ldes1].lstate == LUSED){	// lock is created but no one has picked it => grant access to current process.
			if(type==READ){
				lockarr[ldes1].lstate= READ;
				lockarr[ldes1].readercount++;	// update readers count, no need to check for mutual exclusion as we disables interrupts
			}
			else
				lockarr[ldes1].lstate= WRITE;
			
			lockarr[ldes1].process_holding_lock[currpid]=1;	// track of all process currently acquired this lock
			proctab[currpid].inh_lock_hold[ldes1]=1;
			//	track of all locks acquired by process.
			//proctab[currpid].lockhold[ldes1] = lockarr[ldes1].lstate;
			//lockarr[ldes1].lstate = lockarr[ldes1].lstate;
			//enqueue(currpid,lptr->lqtail);
		}
	else{	// if the lock is already reserved
			//kprintf("READ");
			int temp = lockarr[ldes1].lqhead;	// retrive head id from lock queue.
			
			int tailprocess = gettail(lockarr[ldes1].lqtail);	// function to return process at tail, returns -1 if empty
			if(lockarr[ldes1].lstate == READ && type==READ){	// when reader acquired lock and new request is reader
			
			
			if(tailprocess<0 || proctab[tailprocess].lpriority<priority){	// Queue is empty, proceed with this process
				lockarr[ldes1].readercount++;
				//proctab[currpid].lockhold[ldes1] = READ;
				lockarr[ldes1].process_holding_lock[currpid]=1;
				proctab[currpid].inh_lock_hold[ldes1]=1;
				restore(ps);
				return(OK);
			}
			else if(proctab[tailprocess].lpriority>priority){	// process at tail has greater priority than current lock requesting process
				//kprintf("pushing process %d\n", currpid );
				run_inherit(ldes1);
				insert(currpid, temp, priority);
				proctab[currpid].pstate = PRWAIT;
				proctab[currpid].lock_request_time = ctr1000;
				proctab[currpid].pwaitret = OK;
				resched();
				//proctab[currpid].lockhold[ldes1] = READ;
				/* while(proctab[temp.qprev].lpriority<priority){	// check if current process lock priority is larger than process in queue lock 
					temp = temp.qprev;
				} */
			}
			else if(proctab[tailprocess].lpriority==priority) { // proctab[tailprocess].lpriority==priority
				// add the process to lock queue according to priority
			//kprintf("process %s acquiring lock asfsdf\n", proctab[currpid].pname);	//if(ctr1000-proctab[tailprocess].lock_request_time<500){
					lockarr[ldes1].readercount++;
					lockarr[ldes1].process_holding_lock[currpid]=1;
					proctab[currpid].inh_lock_hold[ldes1]=1;
					update_process_priority();
					restore(ps);
					return(OK);
				//}
				/*else{
					run_inherit(ldes1);
					insert(currpid, temp, priority);
					proctab[currpid].lock_request_time = ctr1000;
					proctab[currpid].pstate = PRWAIT;
					proctab[currpid].pwaitret=OK;
					resched();
					
				}*/
			}
			
			//else //(proctab[tailprocess].lpriority<priority){
				// do nothing.
			//}
			}
		else{
			//kprintf("pushing process %d\n", currpid );
			
			insert(currpid, temp,  priority);
			//if(lockarr[ldes1].locktype == READ && type==WRITE) OR WRITE and READ OR WRITE and WRITE
			proctab[currpid].pstate = PRWAIT;
			proctab[currpid].lock_request_time = ctr1000;
			proctab[currpid].pwaitret = OK;
			run_inherit(ldes1);
			resched();
		}
	}
	lockarr[ldes1].process_holding_lock[currpid]=1;	//	saving that this process acquired lock -- used for priority inheretence
	proctab[currpid].inh_lock_hold[ldes1]=1;
	//update_lock_acquired_process_priorities(ldes1);
	update_process_priority();
	//update_acquired_locks_recursive(currpid,proctab[currpid].pprio);
	restore(ps);
	//return(proctab[currpid].pwaitret);
	return (OK);
}
/*
int insertLock(int proc, int head, int key)
{
	int	next;			// runs through list		
	int	prev;

	next = q[head].qnext;
	while (proctab[next].lpriority < key)	// tail has maxint as key
		next = q[next].qnext;
	q[proc].qnext = next;
	q[proc].qprev = prev = q[next].qprev;
	//q[proc].qkey  = key;
	q[prev].qnext = proc;
	q[next].qprev = proc;
	return(OK);
}
*/

//	update inhereted priorities for all the related process
void run_inherit(int ldes1){
	int i=0;
	//kprintf("entered inheret by process %d\n",currpid);
	proctab[currpid].waiting_for_lock = ldes1;
	// below statement to check for transitive
	if(lockarr[ldes1].max_priority<proctab[currpid].pprio){
		lockarr[ldes1].max_priority=proctab[currpid].pprio;
		for(i=0; i<NPROC; i++){
			//	implies process currently inhold of lock need to be updated
			if(lockarr[ldes1].process_holding_lock[i]==1){
				if(proctab[i].pprio<lockarr[ldes1].max_priority){
					// below statement => process running with its original priority and has not inherited priority till now
					if(proctab[i].pinh==0){
						proctab[i].pinh= proctab[i].pprio;	// save original priority to pinh and update inherited priority to pinh
					}
					proctab[i].pprio=lockarr[ldes1].max_priority;
					chpriorecursive(i,proctab[i].pprio);
					//run_inherit();
				}
				else{
					// ignore as the process running has greater priority than max priority of lock
				}
			}
		}
	}
	// kprintf("process completed id %d, prio is %d, pinh prio is %d\n",48,proctab[48].pprio, proctab[48].pinh);
	// kprintf("process completed id %d, prio is %d, pinh prio is %d\n",currpid,proctab[currpid].pprio, proctab[currpid].pinh);
}

//	returns the process id from tail - which is the highest priority process in queue
int gettail(int tail){
	if(q[tail].qprev>=NPROC)		// implies list is empty, return -1
		return -1;
	else
		return q[tail].qprev;
}