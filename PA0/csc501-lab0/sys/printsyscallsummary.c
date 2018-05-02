
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  printsyscallsummary  -- print # times each syscall is called, group by process
 *------------------------------------------------------------------------
 */

unsigned long   ctr1000;	//timer variable

char sys_call_names[27][15] = {"freemem","chprio","getpid","getprio","gettime","kill","receive","recvclr","recvtim","resume","scount","sdelete","send","setdev","setnok","screate","signal","signaln","sleep","sleep10","sleep100","sleep1000","sreset","stacktrace","suspend","unsleep","wait"};
// sys_call_names array is initialized to 27 system call names
extern int monitoring;

void syscallsummary_start(){		// From here on monitoring starts
	
	int i=0,j=0;
	monitoring = 1;			// monitoring flag set to 1 indicating, we need to record systems calls and average executions for systems calls here after
	
 	for(i=0;i<NPROC;i++)
	{
		for(j=0;j<27;j++)
		{
			proctab[i].count_sys_calls[j] = 0;		// initialize count to 0
			proctab[i].avg_execution_times[j] = 0;		// initialize average execution times to zero
		}
	}
	
}
void syscallsummary_stop(){		//From here no need to monitor
	monitoring = 0;		// monitoring flag reset to 0, indicating no need to monitor system calls and average execution times here after.
}
void printsyscallsummary()
{

	int i,j;
	for(i=1; i<NPROC ; i++){
		if (strcmp(proctab[i].pname,"")!= 0)
			kprintf("\nProcess [pid:%d]\n", i);
		for(j=0; j<27 ; j++){
			if(proctab[i].count_sys_calls[j]>0){
				kprintf("\t Syscall: %s, ", sys_call_names[j]);
				kprintf("\t count: %d", proctab[i].count_sys_calls[j]);
				kprintf("\t average execution time: %d\n", proctab[i].avg_execution_times[j]);
			}
        }
	}
	
		
}

void start_timer(int i, int curr_proc){
		proctab[curr_proc].track = 1;	// 	introduced to ensure, syscalls stop_timer executes only if start_timer is executed.
		proctab[curr_proc].start_time[i]= ctr1000;		// track the start time
		proctab[curr_proc].count_sys_calls[i]++;		// increment number of times syscall is called by one
}

void stop_timer(int i, int curr_proc){
	
		if(proctab[curr_proc].track == 1 && proctab[curr_proc].count_sys_calls[i] != 0)
		{		
			long current_time = ctr1000-(proctab[curr_proc].start_time[i]);		// difference of start and stop time stored in current_time variable
			// Average time calculated and stored in corresponding system call array
			proctab[curr_proc].avg_execution_times[i] = 
			(((proctab[curr_proc].avg_execution_times[i])*(proctab[curr_proc].count_sys_calls[i]-1))+current_time)/(proctab[curr_proc].count_sys_calls[i]);
			//proctab[currpid].track = 0;		// need to revert the flag set in start_timer code.	// if internal sys calls are called this fails (eg sleep inturn calls sleep 10), hence commenting this. However monitoring flag doesnot allow to enter into this function if it is zero
		}
}