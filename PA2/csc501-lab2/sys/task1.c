#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

unsigned long ctr1000;	// used to print the time value at current executing statement

#define DEFAULT_LOCK_PRIO	20

void writerlock11 (char *msg, int lck){	
		// use this lock 
		//kprintf("Process %s, waiting for lock at time %d\n", proctab[currpid].pname,ctr1000);
		// request for lock 
		// request for reader lock on the lck variable
		lock (lck, READ, DEFAULT_LOCK_PRIO);
		//kprintf("Process %s, acquired  sem at time, %d, prio = \n", proctab[currpid].pname,ctr1000, proctab[currpid].pprio);
		sleep10(1);
        //sleep(1);
        int i=0;
		kprintf("Process %s, acquired lock at time %d\n", proctab[currpid].pname,ctr1000);
		
		kprintf("\n");
        for(i=0; i<10; i++)
        {
            kprintf("%c",msg);
        }
		kprintf("\n");
        releaseall (1, lck);
		kprintf("Process %s, released lock at time %d\n", proctab[currpid].pname,ctr1000);
}


void writerlock22(char* msg, int sem){

        //kprintf("Process %s, waiting for semaphore at time %d\n", proctab[currpid].pname,ctr1000);
        wait(sem);
		kprintf("Process %s, acquired semaphore at time %d\n", proctab[currpid].pname,ctr1000);
        sleep10(1);
        //sleep(1);
        //kprintf ("  %s: to release semaphore\n", msg);
        int i=0;
        kprintf("\n");
		for(i=0; i<10; i++)
        {
            kprintf("%c",msg);
        }
		kprintf("\n");
        signal(sem);
		kprintf("Process %s, releasing semaphore at time %d\n", proctab[currpid].pname,ctr1000);
}

void readerlock11 (char *msg, int lck){
		// dummy reader just to block the process entering after this
		int i=0;
		kprintf("\n");
        for(i=0; i<10; i++)
        {
            kprintf("%c",msg);
        }
		kprintf("\n");
		sleep(10);	// sleep so that this holds the resource for more time before releasing
}

void task1 (){

        int a, b, c;
        kprintf("\Priority inheritance with Lock implementation\n");

        int lck1  = lcreate ();
        if(lck1 == SYSERR || lck1 == DELETED){
            kprintf("error creating the lock!\n");
            return;
        }
		kprintf("locks implementation\n");
		long l1 = ctr1000;	// timer to use it in running time 
		// ctr1000 used to show corresponding results that the higher priority process under LOCKS has less turn around time when compared to XINU semaphore implementation
		// writer11 - locks 
		int lck2 = lcreate();	
        a = create(writerlock11, 2000, 20, "A", 2, 'A', lck2);
        b = create(readerlock11, 2000, 10, "B", 2, 'B', lck1);
        c = create(writerlock11, 2000, 30, "C", 2, 'C', lck1);

        resume(b);
		kprintf("scheduled b\t");
        resume(a);
		kprintf("scheduled a\t");
        resume(c);
		kprintf("scheduled c\t");
        sleep(5);
		// above order is chosen to show the variance in scheduling the process in priority inheretence 
		

        kprintf("\XINU semaphore implementation\n");
		kprintf("Semaphore implementation\n");
        int sem = screate(1);
        if(sem == SYSERR || sem == DELETED){
            kprintf("error creating the lock!\n");
            return;
        }    
			//kprintf("semaphore implementation\n");
			l1 = ctr1000;
			//int sem1 = screate(1);
        a = create(writerlock22, 2000, 20, "A", 2, 'A', sem);
        b = create(readerlock11, 2000, 10, "B", 2, 'B',sem);
        c = create(writerlock22, 2000, 30, "C", 2, 'C', sem);
        
		resume(b);
		kprintf("scheduled b\t");
        resume(a);
		kprintf("scheduled a\t");
        resume(c);
		kprintf("scheduled c\t");

        sleep(10);
		// above order is chosen to show default scheduling implementation of XINU os where higher priority process waits for resource blocked by lower priority process.
		// same order of execution is implemented in both locks and semaphores 

        kprintf("Task1 success!\n");
}