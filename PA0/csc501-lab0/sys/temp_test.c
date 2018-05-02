
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

static unsigned long	*esp;
static unsigned long	*ebp;
//extern  struct  pentry proctab[];
extern  int     numproc;                /* currently active processes   */


void temptest(int priority)
{
	int i;
//	kprintf("%d",NPROC);
	for(i=0; i<NPROC ; i++){
	struct pentry	*proc = &proctab[i];
	unsigned long	*sp;//, *fp;
	kprintf("Process id - %d, name- %d, priority - %d",i,proctab[i].pname,proctab[i].pprio);	
	if(proctab[i].pprio<priority){//*((unsigned long *)proc->pprio)<priority){
		continue;
	}

//	if (i != 0 && isbadpid(i))
//		return SYSERR;
	if (i == currpid) {
		asm("movl %esp,esp");
		asm("movl %ebp,ebp");
		sp = esp;
	//	fp = ebp;
	} else {
		sp = (unsigned long *)proc->pesp;
	//	fp = sp + 2; 		/* where ctxsw leaves it */
	}
	
	kprintf("\nProcess [%s]",proctab[i].pname);
        kprintf("\n \tpid: %d",i);
        kprintf("\n \tpriority: %d",proctab[i].pprio);
        kprintf("\n \tbase: 0x%08x",proctab[i].pbase);
	kprintf("\n \tlimit: 0x%08x",proctab[i].plimit);
        kprintf("\n \tlen: %d",proctab[i].pstklen);
        kprintf("\n \tpointer: 0x %08x",sp);//proctab[i].pstklen);

        
//	kprintf("\nProcess [%s]",(unsigned long *)proc->pname[PNMLEN]);

	}
}
