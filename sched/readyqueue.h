#ifndef READYQUEUE_H
#define READYQUEUE_H

#include <sched/thread.h>

#define NROF_QUEUE_PRIORITES	8

thread_t readyQueuePop(void);

void readyQueuePush(thread_t thread);

void readyQueuePushFront(thread_t thread);

#endif