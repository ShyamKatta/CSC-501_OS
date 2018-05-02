/* lock.h - isbadlock */

#ifndef _LOCK_H_
#define _LOCK_H_

#ifndef	NLOCKS
#define	NLOCKS		50	/* number of locks, if not defined	*/
#endif

#define	LFREE	'\00'		/* this lock is free, not created		*/
#define	LUSED	'\02'		/* this lock is created/ used	but not acquired	*/
#define	READ	'\03'		/* the process is requesting read lock	*/
#define	WRITE	'\04'		/* the process is requesting write lock	*/
//#define NOLSTATE	'\00'	/* No Lock state - initially no lock*/

struct	lentry	{		/* semaphore table entry		*/
	char	lstate;		/* the lock state LFREE or LUSED or READ or WRITE		*/
	//char	locktype;	/* READ or WRITE type of lock - currently, initially NOLSTATE*/
	// int	lckcnt;		/* count for this Lock - lessthan 0 => process waiting		*/
	int readercount;	/*	keep track of # readers	*/
	int	lqhead;		/* q index of head of list		*/
	int	lqtail;		/* q index of tail of list		*/
	//long lock_creation_time;	/*	time when lock is created to handle delete anomalies	*/
	long requested_lock[NPROC];	/*	this is set to 1 in create.c when a process requests a lock and process marked 1 are marked to -1 if one process deletes lock, overridden when a new process is created and set to 0	*/
	// inheretence
	int max_priority;	//	max priority of all process in waiting list
	int process_holding_lock[NPROC];	//	list of process currently holding lock
};
extern	struct	lentry	lockarr[];
extern	int	nextlck;

#define	isbadlck(s)	(s<0 || s>=NLOCKS)
#endif

SYSCALL releaseall(int ,int, ...);
