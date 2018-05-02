/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lab1.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
int process_to_schedule();
int getProcess();
int sched=0;
epoch =0;
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */

 
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	
	/* no switch needed if current process priority higher than next*/

	/* if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<optr->pprio)) {
		return(OK);
	} */
	
	/* save old process to optr and push it to ready queue only if process is running*/
	optr= &proctab[currpid];
	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}
	
	/* remove highest priority process at end of ready list */
	
	// random scheduler code
	//struct qent temp;
	//temp=q[rdytail].qnext;
	
	switch(getschedclass()){
	/**************** random scheduler ***********/
	
	case RANDOMSCHED :
		if(q[rdytail].qprev>0)
		{
			int temp = q[rdytail].qprev;
			int sum =0;
			while( temp!=q[rdyhead].qnext){
				sum = sum + q[temp].qkey;
				temp = q[temp].qprev;
			}
			int prob = rand()%sum;
			temp = q[rdytail].qprev;	// reset temp to first element.

			while(prob >= q[temp].qkey){
				prob = prob - q[temp].qkey;
				temp = q[temp].qprev;
			}
			nptr = &proctab[ (currpid = dequeue(temp)) ];	// only if prob< q[temp].qkey
		}
		else{	// only for null process
		nptr = &proctab[ (currpid = getlast(rdytail)) ];
		}
		break;
	/**************** random scheduler ***********/
	
	
	/*************** Linux Scheduler ************/
	case LINUXSCHED :
		//if(currpid!=0)
		//kprintf("process %d, epoch value = %d \t",currpid,epoch);//proctab[currpid].goodness);
		
		if(epoch > 0 && process_to_schedule()>0){	// continue in old epoch. only current process goodness, quantum change
			//kprintf("\n epoch value is %d, curr process is %d, %d \n", epoch,currpid, proctab[currpid].quantum);
			
			proctab[currpid].quantum = preempt; //(ctr1000-proctab[currpid].last_start_time);
			if(proctab[currpid].quantum <= 0){
					proctab[currpid].goodness =0;
					proctab[currpid].quantum =0;
			}
			else
				proctab[currpid].goodness = (proctab[currpid].epoch_start_pprio + proctab[currpid].quantum);
			epoch = epoch - (proctab[currpid].quantum-preempt);

		}
		
		if(epoch == 0 || !process_to_schedule() ){
		//else{
			// start of epoch
			//kprintf("\nnew epoch \n");
			int temp = q[rdytail].qprev;
			//below logic to check ready list process
			/*while( temp!=q[rdyhead].qnext){
				kprintf("\nready list process %d, epoch flag =%d,quantum = %d\n",temp,(proctab[temp].epoch_flag),(proctab[temp].quantum));
				temp = q[temp].qprev;
			}*/
			int i;
			epoch =0;
			//kprintf("epoch start\t");
			for(i=0; (i<NPROC); i++){
				if((proctab[i].pstate==PRFREE))
					continue;
				//kprintf("\n curr process is %d, %s, %d ", i, proctab[i].pname, proctab[i].pprio);
				//kprintf("i value %d\n",i); 
				proctab[i].epoch_flag=1;
				proctab[i].epoch_start_pprio = proctab[i].pprio;
				proctab[i].quantum = proctab[i].epoch_start_pprio + ((proctab[i].quantum)/2);
				//proctab[i].last_start_time= 0;
				if(proctab[i].quantum ==0)
					proctab[i].goodness =0;
				else
					proctab[i].goodness = (proctab[i].epoch_start_pprio + proctab[i].quantum);
				epoch = epoch+proctab[i].quantum;
				//if(i!=0)
					//kprintf("\n epoch value is %d, curr process is %d, %s priority %d and quantum %d, goodness %d\n", epoch, i, proctab[i].pname, proctab[i].pprio, proctab[i].quantum, proctab[i].goodness);
			}
		}
		/*************** Linux Scheduler ************/
		
		/* if(getProcess()==0){
			int temp = q[rdytail].qprev;
			while( temp!=q[rdyhead].qnext){
				kprintf("\n here process id %d, goodness %d \t",temp,proctab[temp].goodness);
				//	high_goodness = proctab[temp].goodness;
				//	high_proc = temp;
				
				temp = q[temp].qprev;
			}
		} */
		//kprintf("quantum of process %d is %d, preempt is %d\n",currpid, proctab[currpid].quantum, preempt);
		nptr = &proctab[ (currpid = dequeue(getProcess())) ];
		break;
	//	nptr = &proctab[ (currpid = getlast(rdytail)) ];
	
	default :
		if ((optr->pstate == PRCURR) && (lastkey(rdytail)<optr->pprio))
			nptr = &proctab[ (currpid = dequeue(currpid)) ];
		else
			nptr = &proctab[ (currpid = getlast(rdytail)) ];
		//preempt = QUANTUM;
	}
	//QUANTUM = proctab[currpid].quantum;	// after cntxt switch happens
	nptr->pstate = PRCURR;		/* mark it currently running	*/
#ifdef	RTCLOCK
	if(sched ==1 || sched ==2){
	preempt = proctab[currpid].quantum;		/* reset preemption counter	*/
	}
	else{
		preempt = QUANTUM;
	}
#endif

	//proctab[currpid].last_start_time = ctr1000;

	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	
	/* The OLD process returns here when resumed. */
	return OK;
}

/* returns 1 if there is a process in ready queue that has epoch flag=1 and quantum>0 */
int process_to_schedule(){
	int temp = q[rdytail].qprev;
	while( temp!=q[rdyhead].qnext){
		if( (proctab[temp].epoch_flag==1) && (proctab[temp].quantum!=0)){
			return 1;
		}
		temp = q[temp].qprev;
	}
	return 0;	// only null process is available to schedule
}

/*	returns the process to schedule, according to goodness*/
int getProcess(){
	int high_goodness=0;
	int high_proc=0;
	int temp = q[rdytail].qprev;
	while( temp!=q[rdyhead].qnext){
		if(proctab[temp].goodness>high_goodness){
			high_goodness = proctab[temp].goodness;
			high_proc = temp;
		}
		temp = q[temp].qprev;
	}
	return high_proc;
}

void setschedclass(int sched_class){
	sched = sched_class;
}

int getschedclass(){
	return sched;
}
