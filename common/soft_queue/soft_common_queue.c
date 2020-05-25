#include "soft_public.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "soft_common_queue.h"
#include "soft_queue.h"

static SOFT_QUEUE_s gstQueueCtrl = {
    .iQueueId = -1,
};

/**********************************************************
 * Function Name:        SoftQueue_init
 * Description:             消息初始化
 * Parameters[in]:        void
 * Parameters[out]:      void                        
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.15
***********************************************************/
ULONG SoftQueue_init(void)
{
    INT iFd = 0;
    key_t key = 0;
    
    /* create file lock */
    SOFT_CODE_DEBUG("create file %s\n", SOFT_QUEUE_LOCK_FILE);
    gstQueueCtrl.iLockFd = open(SOFT_QUEUE_LOCK_FILE, O_WRONLY | O_CREAT, 0777);
    if (gstQueueCtrl.iLockFd < 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        return SOFT_STAT_FAIL_FILE_OPEN;
    }

    key = ftok(SOFT_QUEUE_LOCK_FILE, SOFT_QUEUE_PROJ_ID);
    if (key < 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        return SOFT_STAT_FAIL_FTOK;
    }

    if (flock(gstQueueCtrl.iLockFd, LOCK_EX | LOCK_NB) < 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        close(gstQueueCtrl.iLockFd);
        gstQueueCtrl.iLockFd = -1;
        return SOFT_STAT_FAIL_FILE_LOCK;  
    }

    /*检查消息队列是否存在*/  
    iFd = msgget(key, IPC_EXCL);  
    if (iFd < 0)
    {
        /*创建消息队列*/  
        iFd = msgget(key, IPC_CREAT | 0666);
        if(iFd < 0)
        {  
            printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
            flock(gstQueueCtrl.iLockFd, LOCK_UN);
            close(gstQueueCtrl.iLockFd);
            gstQueueCtrl.iLockFd = -1;
            return SOFT_STAT_FAIL_QUEUE_MSGGET;  
        }
    } 

    gstQueueCtrl.iQueueId = iFd;

    if (flock(gstQueueCtrl.iLockFd, LOCK_UN) < 0)
    {
        /* remove Lock file */
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        close(gstQueueCtrl.iLockFd);
        gstQueueCtrl.iLockFd = -1;

        /* remove msg queue */
        msgctl(gstQueueCtrl.iQueueId, IPC_RMID, NULL);
        gstQueueCtrl.iQueueId = -1;
        return SOFT_STAT_FAIL_FILE_UNLOCK; 
    }

    return SOFT_STAT_SUCCESS;
}

/**********************************************************
 * Function Name:        SoftQueue_destory
 * Description:             消息注销
 * Parameters[in]:        void
 * Parameters[out]:      void                        
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.15
***********************************************************/
ULONG SoftQueue_destory(void)
{
    if (flock(gstQueueCtrl.iLockFd, LOCK_EX | LOCK_NB) < 0)
    {
        return SOFT_STAT_FAIL_FILE_LOCK;  
    }

    if (gstQueueCtrl.iQueueId < 0)
    {
        printf("[%s %d] Queue already destory\n", __FUNCTION__, __LINE__);
        return SOFT_STAT_FAIL_INVAILD_PARAM;
    }

    //IPC_RMID: Delete the message queue from the system
    msgctl(gstQueueCtrl.iQueueId, IPC_RMID, NULL);
    close(gstQueueCtrl.iQueueId);
    gstQueueCtrl.iQueueId = -1;

    //remove Lock
    flock(gstQueueCtrl.iLockFd, LOCK_UN);
    close(gstQueueCtrl.iLockFd);
    gstQueueCtrl.iLockFd = -1;
    remove(SOFT_QUEUE_LOCK_FILE);
    
    return SOFT_STAT_SUCCESS;
}

/**********************************************************
 * Function Name:        SoftQueue_modRegist
 * Description:             模块注册 
 * Parameters[in]:        LONG lmoduleID
 * Parameters[out]:      void                        
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.15
***********************************************************/
ULONG SoftQueue_modRegist(IN LONG lModuleID)
{
    if (flock(gstQueueCtrl.iLockFd, LOCK_EX | LOCK_NB) < 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        return SOFT_STAT_FAIL_FILE_LOCK;  
    }
    
    if ((lModuleID < 0) || (lModuleID > SOFT_MODULE_MAX) ||
        (gstQueueCtrl.iQueueId < 0))
    {
        printf("[%s %d] err iQueueId %d moduleID %ld\n", __FUNCTION__, __LINE__, 
                gstQueueCtrl.iQueueId, lModuleID);
        if (flock(gstQueueCtrl.iLockFd, LOCK_UN) < 0)
        {
            printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
            return SOFT_STAT_FAIL_FILE_UNLOCK;  
        }
        
        return SOFT_STAT_FAIL_INVAILD_PARAM;
    }

    SOFT_ADDFLAGS(gstQueueCtrl.ulRegistMap, (1UL << lModuleID));
    SOFT_CODE_DEBUG("[%s %d] RegistMap = %lx\n", __FUNCTION__, __LINE__, gstQueueCtrl.ulRegistMap);
    
    if (flock(gstQueueCtrl.iLockFd, LOCK_UN) < 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        return SOFT_STAT_FAIL_FILE_UNLOCK;  
    }

    return SOFT_STAT_SUCCESS;
}

/**********************************************************
 * Function Name:        SoftQueue_modUnregist
 * Description:             模块注销
 * Parameters[in]:        LONG lmoduleID
 * Parameters[out]:      void                        
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.15
***********************************************************/
ULONG SoftQueue_modUnregist(IN LONG lModuleID)
{
    if (flock(gstQueueCtrl.iLockFd, LOCK_EX | LOCK_NB) < 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        return SOFT_STAT_FAIL_FILE_LOCK;  
    }

    if ((lModuleID < 0) || (lModuleID > SOFT_MODULE_MAX) ||
        (gstQueueCtrl.iQueueId < 0))
    {
        printf("[%s %d] err iQueueId %d moduleID %ld\n", __FUNCTION__, __LINE__, 
                gstQueueCtrl.iQueueId, lModuleID);

        if (flock(gstQueueCtrl.iLockFd, LOCK_UN) < 0)
        {
            printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
            return SOFT_STAT_FAIL_FILE_UNLOCK;  
        }
        return SOFT_STAT_FAIL_INVAILD_PARAM;
    }

    SOFT_REMOVEFLAGS(gstQueueCtrl.ulRegistMap, (1UL << lModuleID));
    SOFT_CODE_DEBUG("[%s %d] RegistMap = %lx\n", __FUNCTION__, __LINE__, gstQueueCtrl.ulRegistMap);

    if (flock(gstQueueCtrl.iLockFd, LOCK_UN) < 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        return SOFT_STAT_FAIL_FILE_UNLOCK;  
    }
    
    return SOFT_STAT_SUCCESS;
}

/**********************************************************
 * Function Name:        SoftQueue_snd
 * Description:             消息发送
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *msg_ptr
 *                              IN INT msgflg
 * Parameters[out]:      void                        
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.15
***********************************************************/
ULONG SoftQueue_snd(IN SOFT_QUEUE_MSG_s *msg_ptr, IN INT msgflg)
{
    INT iLen = 0;

    if (SOFT_CHECKFLAGS(gstQueueCtrl.ulRegistMap, (1UL << msg_ptr->lDstID)) == 0)
    {
        return SOFT_STAT_FAIL_INVAILD_PARAM;
    }

    if ((msg_ptr->lDstID < 0) || (msg_ptr->lDstID > SOFT_MODULE_MAX) ||
        (gstQueueCtrl.iQueueId < 0))
    {
        printf("[%s %d] srcId %ld, DstId %ld, QueueId %d\n", __FUNCTION__, __LINE__, 
                msg_ptr->lSrcID, msg_ptr->lDstID, gstQueueCtrl.iQueueId);
        return SOFT_STAT_FAIL_INVAILD_PARAM;
    }

    if (flock(gstQueueCtrl.iLockFd, LOCK_EX | LOCK_NB) < 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        return SOFT_STAT_FAIL_FILE_LOCK;  
    }

    iLen = msgsnd(gstQueueCtrl.iQueueId, msg_ptr, sizeof(*msg_ptr) + msg_ptr->usLen, msgflg);

    if (flock(gstQueueCtrl.iLockFd, LOCK_UN) < 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        return SOFT_STAT_FAIL_FILE_UNLOCK;  
    }

    if (iLen < 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        if ((msgflg == IPC_NOWAIT) && (errno == EAGAIN))
        {
            /* queue is full */
            return SOFT_STAT_FAIL_QUEUE_FULL;
        }
        else
        {
            /* send fail */
            return SOFT_STAT_FAIL_QUEUE_SEND;
        }
    }

    return SOFT_STAT_SUCCESS;
}

/**********************************************************
 * Function Name:        SoftQueue_rcv
 * Description:             消息接收
 * Parameters[in]:        INOUT SOFT_QUEUE_MSG_s *msg_ptr
 *                              IN INT msgflg
 * Parameters[out]:      INOUT SOFT_QUEUE_MSG_s *msg_ptr                        
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.15
***********************************************************/
ULONG SoftQueue_rcv(INOUT SOFT_QUEUE_MSG_s *msg_ptr, IN INT msgflg)
{
    INT iLen = 0;

    if (SOFT_CHECKFLAGS(gstQueueCtrl.ulRegistMap, (1UL << msg_ptr->lDstID)) == 0)
    {
        return SOFT_STAT_FAIL_INVAILD_PARAM;
    }

    if ((msg_ptr->lDstID < 0) || (msg_ptr->lDstID > SOFT_MODULE_MAX) ||
        (gstQueueCtrl.iQueueId < 0))
    {
        printf("[%s %d] srcId %ld, DstId %ld, QueueId %d\n", __FUNCTION__, __LINE__, 
                msg_ptr->lSrcID, msg_ptr->lDstID, gstQueueCtrl.iQueueId);
        return SOFT_STAT_FAIL_INVAILD_PARAM;
    }

    if (flock(gstQueueCtrl.iLockFd, LOCK_EX | LOCK_NB) < 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        return SOFT_STAT_FAIL_FILE_LOCK;  
    }

    /* MSGMAX = 8192, defined in linux/msg */
    iLen = msgrcv(gstQueueCtrl.iQueueId, msg_ptr, SOFT_QUEUE_MSGMAX, msg_ptr->lDstID, msgflg);
    if (flock(gstQueueCtrl.iLockFd, LOCK_UN) < 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        return SOFT_STAT_FAIL_FILE_UNLOCK;  
    }

    if (iLen < 0)
    {
        return SOFT_STAT_FAIL_QUEUE_RECV;
    }

    return SOFT_STAT_SUCCESS;
}

