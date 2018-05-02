
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

static unsigned long	*esp;
static unsigned long	*ebp;

extern  int     numproc;                /* currently active processes   */

/*------------------------------------------------------------------------
 *  printprocstks  -- For each existing process with larger priority than the parameter, print the stack base, stack size, stacklimit, and stack pointer. Also, for each process, include the process name, the process id and the process priority
 *------------------------------------------------------------------------
 */

void printprocstks(int priority)
{
	int i;
	for(i=1; i<NPROC ; i++){
	
	struct pentry	*proc = &proctab[i];
	unsigned long	*sp;
	
	if(proctab[i].pprio<=priority){ // if priority is less than or equal to parameter, skip this process.
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
        kprintf("\n \tpointer: 0x %08x",sp);

	}
}
