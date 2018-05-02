/* signaln.c - signaln */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  signaln -- signal a semaphore n times
 *------------------------------------------------------------------------
 */
 int gettail(int);
 void update_process_priority();
 
SYSCALL releaseall(int numlocks, int ldes1, ...)
{	
	STATWORD ps; 
	
	disable(ps);
	unsigned long *templock;	// temporary pointer traversing all inputs.
	templock = (unsigned long *)(&ldes1);
	int i;
	//lockarr[*(temp++)];
	int currlock;
	int err_flag=0;
	int dequeue_process;
	for ( i=0; i < numlocks ; i++){
		
		currlock = *(templock++);
		// process release, remaining locks max update to process
		if(lockarr[currlock].lstate==LFREE ){
			//kprintf("1 LFREE 1.0 lock %d, lstate = %d\n",*(templock), lockarr[currlock].lstate);
			err_flag=1;
		}
		
		
		//proctab[currpid].lock_time_copy[currlock]=0;
		//kprintf("current state is %d",lockarr[currlock].lstate);
		proctab[currpid].inh_lock_hold[currlock]=0;	// update for inhereted code
		lockarr[currlock].requested_lock[currpid]=-1;
		lockarr[currlock].process_holding_lock[currpid]=0;
		if(lockarr[currlock].lstate == LFREE || proctab[currpid].lockhold[currlock] == LFREE){
			// sys error handle 
			//kprintf("SYSERR LFREE 1.1");
			err_flag=1;
			//kprintf("lock free\n");
		}
		else if(lockarr[currlock].lstate== LUSED ){
			// error handle; 
			//kprintf("lock uninitialied\n");
			//kprintf("SYSERR LUSED");
			err_flag=1;
		}
		else if(lockarr[currlock].lstate==READ){
			
			proctab[currpid].lmode = LFREE;
			proctab[currpid].lockhold[currlock] = LFREE;
			lockarr[currlock].readercount--;
			//if(gettail(lockarr[currlock].lqtail)<0){
				// nothing to schedule
			//kprintf("reader count --> %d\n",lockarr[currlock].readercount);
			if(lockarr[currlock].readercount==0){
				lockarr[currlock].lstate = LUSED; // handles case  when no process is waiting in queue
				//kprintf("\t tail next id --> %d\n",gettail(lockarr[currlock].lqtail));
				if(gettail(lockarr[currlock].lqtail)>0){
					lockarr[currlock].lstate = WRITE;
					proctab[gettail(lockarr[currlock].lqtail)].lockhold[currlock]= WRITE;
					proctab[gettail(lockarr[currlock].lqtail)].lmode = WRITE;
					dequeue_process = getlast(lockarr[currlock].lqtail);
					update_process_priority();	// update current process priority after a process is released from wait queue
					ready(dequeue_process, RESCHNO);
				}
			}
			// else do not schedule as this is not last reader;
			
		}
		else{	// lstate is WRITE
			int tailprocess = gettail(lockarr[currlock].lqtail);
			proctab[currpid].lmode = LFREE;
			proctab[currpid].lockhold[currlock] = LFREE;
			
			if(tailprocess<0){	// nothing in lock queue, release lock
				// proctab[currpid].lmode = LFREE;
				// proctab[currpid].lockhold[currlock] = LFREE;
				lockarr[currlock].lstate = LUSED;
			}
			else{	// there are some elements in queue.
				// retrive first element at tail.
				int first_process_type = proctab[tailprocess].lockhold[currlock];
				//int high_priority = proctab[tailprocess].lpriority;
				
				if(first_process_type==READ){
					lockarr[currlock].lstate = READ;
					//proctab[prev].lockhold[currlock]= READ; no need already done in lock.c
					//proctab[prev].lmode = READ;		// no need, already saved in lock.c
					//ready(getlast(lockarr[currlock].qltail), RESCHNO);
					int prev = q[lockarr[currlock].lqtail].qprev;
					int write_flag=0;
					int write_pid;
					int write_priority;
					int prev_flag=0;
					int temp_prev;
					while(prev!=lockarr[currlock].lqhead){
						if( write_flag==0){
							if(proctab[prev].lockhold[currlock]==WRITE){
								write_flag=1;
								write_priority = proctab[prev].lpriority;
								write_pid = prev;
							}
							else{
								// lock is saved to proctab while acquiring => lock.c
								//scheduling read process
								prev_flag=1;
								temp_prev = q[prev].qprev;
								lockarr[currlock].readercount++;
								dequeue_process = getlast(lockarr[currlock].lqtail);
								update_process_priority();
								ready(dequeue_process, RESCHNO);
						
							}
						}
						
						if(write_flag==1 && proctab[prev].lmode==READ){	// if write skip iteration
							if(proctab[prev].lpriority<write_priority)
								break;
							else if(proctab[prev].lpriority== write_priority){
								if((proctab[prev].lock_request_time - proctab[write_pid].lock_request_time)<=500){
									prev_flag=1;
									temp_prev= q[prev].qprev;
									
									dequeue_process = dequeue(prev);
									update_process_priority();
									ready(dequeue_process, RESCHNO);
								}
							}
						}
						
						//ready(getlast(lockarr[currlock].qltail), RESCHNO);
						if(prev_flag==1){
							prev_flag=0;
							prev = temp_prev;
						}
						else
							prev = q[prev].qprev;
					}
					
				}
				else{	// first process is WRITE
					int prev = tailprocess;
					int write_pid = tailprocess;
					int write_priority=proctab[tailprocess].lpriority;
					prev = q[prev].qprev;
					int write_flag=1; // item to push is first write by default
					int flag_type = 0; // item type to push READ/WRITE
					int prev_flag=0;
					int temp_prev;
					while(prev!=lockarr[currlock].lqhead){
						
						if(proctab[prev].lpriority<proctab[write_pid].lpriority){
							break;
						}
							
						else if(proctab[prev].lpriority == write_priority && proctab[prev].lmode==READ){
							if((proctab[prev].lock_request_time - proctab[write_pid].lock_request_time)<=500){
								write_flag=0;
								lockarr[currlock].lstate = READ;
								(lockarr[currlock].readercount)++;
								prev_flag=1;
								temp_prev= q[prev].qprev;
								dequeue_process = dequeue(prev);
								update_process_priority();
								ready(dequeue_process, RESCHNO);
							}
						}
						
						if(prev_flag==1){
							prev_flag=0;
							prev = temp_prev;
						}
						else
							prev = q[prev].qprev;
					}
					if(write_flag==1){
						lockarr[currlock].lstate= WRITE;
						dequeue_process = getlast(lockarr[currlock].lqtail);
						update_process_priority();
						ready(dequeue_process, RESCHNO);
					}
				}
				
			}
		}
	}
		resched();
		restore(ps);
		if(err_flag==1){
			err_flag=0;
			return SYSERR;
		}
		else
			return(OK);
}

void update_process_priority(){
	int i;
	int global_max_priority=0;
	//find maximum of lock priorities currently held by process
	for(i=0; i<NPROC; i++){
		if(proctab[currpid].inh_lock_hold[i]==1){
			if(lockarr[i].max_priority>global_max_priority){
				global_max_priority = lockarr[i].max_priority;
			}
		}
	}
	// update this priority to process inhereted priority
	if(proctab[currpid].pinh>0){
		if(proctab[currpid].pinh>global_max_priority){
			proctab[currpid].pprio = proctab[currpid].pinh;
			proctab[currpid].pinh=0;
		}
		else{
			proctab[currpid].pprio = global_max_priority;
			//chpriorecursive(currpid,global_max_priority);	// just update pinh prio
		}
	}
	else{
		if(global_max_priority>proctab[currpid].pprio){
			proctab[currpid].pinh = proctab[currpid].pprio;	// copy original to inherited
			proctab[currpid].pprio = global_max_priority;
			//chpriorecursive(currpid,global_max_priority);
		}
	}
}