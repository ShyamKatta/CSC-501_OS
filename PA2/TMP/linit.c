#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

int nextlck;
struct lentry lockarr[NLOCKS];

void linit(){
	int i;
	
	for (i=0 ; i<NLOCKS ; i++) {	/* initialize semaphores */
		lockarr[i].lstate = LFREE;
		lockarr[i].readercount=0;
		lockarr[i].lqtail = 1 + (lockarr[i].lqhead = newqueue());
	}
	for(i=0;i<NPROC; i++){
		proctab[i].waiting_for_lock=-1;
	}
}