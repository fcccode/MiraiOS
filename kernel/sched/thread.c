#include <sched/thread.h>

#include <stdint.h>
#include <mm/paging.h>
#include <print.h>
#include <sched/readyqueue.h>
#include <stdbool.h>
#include <errno.h>
#include <mm/pagemap.h>
#include <arch/cpu.h>
#include <mm/memset.h>

extern void migrateMainStack(thread_t mainThread);

extern void kthreadInit(thread_t thread, void *(*start)(void *), void *arg);

void deallocThread(thread_t thread) {
	uintptr_t stackBottom = (uintptr_t)(thread) - (THREAD_STACK_SIZE - sizeof(struct ThreadInfo));
	deallocPages((void*)stackBottom, THREAD_STACK_SIZE);
}

struct ThreadInfo *allocKStack(void) {
	uintptr_t stackBottom = (uintptr_t)(allocKPages(THREAD_STACK_SIZE, PAGE_FLAG_WRITE | PAGE_FLAG_CLEAN));
	if (!stackBottom) {
		return NULL;
	}
	struct ThreadInfo *thrd = (thread_t)(stackBottom + THREAD_STACK_SIZE - sizeof(struct ThreadInfo));
	memset(thrd, 0, sizeof(struct ThreadInfo));
	return thrd;
}

int kthreadCreate(thread_t *thread, void *(*start)(void *), void *arg, int flags) {
	//alloc thread stack
	struct ThreadInfo *thrd = allocKStack();
	if (!thrd) return -ENOMEM;

	if (thread) {
		*thread = thrd;
	}

	if (flags & THREAD_FLAG_RT) {
		thrd->priority = 0;
		thrd->fixedPriority = true;
	} else {
		thrd->priority = 1;
		thrd->fixedPriority = flags & THREAD_FLAG_FIXED_PRIORITY;
	}

	//thrd->returnValue = NULL;
	thrd->state = THREADSTATE_SCHEDWAIT;
	//thrd->lock = 0;
	thrd->detached = flags & THREAD_FLAG_DETACHED;
	thrd->cpuAffinity = pcpuRead32(cpuInfosIndex);

	thrd->jiffiesRemaining = TIMESLICE_BASE << thrd->priority;

	//thrd->joinFirst = NULL;
	//thrd->joinLast = NULL;
	//thrd->nrofJoinThreads = 0;

	kthreadInit(thrd, start, arg); //initializes stackpointer

	//now push it to the scheduling queue
	readyQueuePush(thrd); //initializes nextThread and prevThread
	
	return 0;
}

int kthreadCreateFromMain(thread_t *thread) {
	//alloc thread stack
	struct ThreadInfo *thrd = allocKStack();
	if (!thrd) return -ENOMEM;
	
	*thread = thrd;
	migrateMainStack(thrd);

	thrd->state = THREADSTATE_RUNNING;
	thrd->priority = 1;
	//thrd->lock = 0;

	thrd->detached = true;
	//thrd->cpuAffinity = 0;
	thrd->fixedPriority = true;
	thrd->jiffiesRemaining = TIMESLICE_BASE << thrd->priority;

	//thrd->joinFirst = NULL;
	//thrd->joinLast = NULL;
	//thrd->nrofJoinThreads = 0;

	setCPUThreadLoad(1 << (NROF_QUEUE_PRIORITIES - 1));

	setCurrentThread(thrd);
	pcpuWrite32(threadLoad, 32);
	
	return 0;
}

thread_t kthreadSwitch(thread_t oldThread, bool higherThreadReleased, bool frontEn) {
	bool front = false;
	oldThread->jiffiesRemaining -= 1;
	if (oldThread->jiffiesRemaining > 0) {
		if (!higherThreadReleased) {
			return oldThread;
		}
		front = frontEn;
	}

	oldThread->state = THREADSTATE_SCHEDWAIT;
	thread_t newThread = readyQueueExchange(oldThread, front);
	if (newThread != oldThread) {
		acquireSpinlock(&newThread->lock);
	}
	newThread->state = THREADSTATE_RUNNING;
	setCurrentThread(newThread);
	return newThread;
}

void kthreadJoin(thread_t thread, void **returnValue) {
	thread_t curThread = getCurrentThread();
	acquireSpinlock(&curThread->lock);
	acquireSpinlock(&thread->lock);
	if (thread->state != THREADSTATE_FINISHED) {
		curThread->nextThread = NULL;
		if (thread->joinLast) {
			curThread->prevThread = thread->joinLast;
			thread->joinLast->nextThread = curThread;
			thread->joinLast = curThread;
		} else {
			curThread->prevThread = NULL;
			thread->joinFirst = curThread;
			thread->joinLast = curThread;
		}
		releaseSpinlock(&thread->lock);
		kthreadStop(); //will release curthread spinlock
		acquireSpinlock(&curThread->lock);
		acquireSpinlock(&thread->lock);
	}
	if (returnValue) {
		*returnValue = thread->returnValue;
	}
	releaseSpinlock(&thread->lock);
	releaseSpinlock(&curThread->lock);
	deallocThread(thread);
}

void kthreadFreeJoined(thread_t thread) {
	//spinlock is already acquired
	thread_t curFreeThread = thread->joinFirst;
	while (curFreeThread) {
		acquireSpinlock(&curFreeThread->lock);

		thread_t nextFreeThread = curFreeThread->nextThread;
		curFreeThread->state = THREADSTATE_SCHEDWAIT;
		readyQueuePush(curFreeThread);

		releaseSpinlock(&curFreeThread->lock);
		curFreeThread = nextFreeThread;
	}
	thread->joinFirst = NULL;
	thread->joinLast = NULL;
}

void kthreadDetach(void) {
	thread_t curThread = getCurrentThread();
	curThread->detached = true;
}