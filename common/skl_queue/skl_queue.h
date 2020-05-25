#ifndef __SKL_QUEUE_H__
#define __SKL_QUEUE_H__

#define SKL_QUEUE_LOCK_FILE     "./Queue.lockFd"
#define SKL_QUEUE_MSGMAX        1024

/* define 'S' ( ascii code is 83 ) */
#define SKL_QUEUE_PROJ_ID       83

#define SKL_QUEUE_LOCK          1
#define SKL_QUEUE_UNLOCK        0

typedef struct _SKL_QUEUE_s_
{
    INT iQueueId;
    INT iLockFd;
    ULONG ulRegistMap;
}SKL_QUEUE_s;

#endif

