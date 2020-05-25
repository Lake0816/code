#ifndef __SOFT_QUEUE_H__
#define __SOFT_QUEUE_H__

#define SOFT_QUEUE_LOCK_FILE     "./Queue.lockFd"
#define SOFT_QUEUE_MSGMAX        1024

/* define 'S' ( ascii code is 83 ) */
#define SOFT_QUEUE_PROJ_ID       83

#define SOFT_QUEUE_LOCK          1
#define SOFT_QUEUE_UNLOCK        0

typedef struct _SOFT_QUEUE_s_
{
    INT iQueueId;
    INT iLockFd;
    ULONG ulRegistMap;
}SOFT_QUEUE_s;

#endif

