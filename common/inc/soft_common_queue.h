#ifndef __SOFT_COMMON_QUEUE_H__
#define __SOFT_COMMON_QUEUE_H__

/* IPC_NOWAIT value is 00004000 (defined in linux/ipc.h) */
#define SOFT_MSG_NOWAIT              00004000
#define SOFT_MSG_BLOCK               0

#define SOFT_QUEUE_RSP_INIT(pstDstMsg, pstSrcMsg) \
do \
{ \
    pstDstMsg->lDstID = pstSrcMsg->lSrcID;  \
    pstDstMsg->lSrcID = pstSrcMsg->lDstID;  \
    pstDstMsg->usCmdID = pstSrcMsg->usCmdID;    \
}while (0)

typedef struct _SOFT_QUEUE_MSG_s_{
    long int lDstID;        //目标模块
    long int lSrcID;        //源模块
    ULONG   ulRet;		    //执行结果，0成功，非零失败
    UINT16  usCmdID;
    UINT16  usLen;
    UINT8   aucData[0];		//采用0长数组，不占用额外的空间
}SOFT_QUEUE_MSG_s;

extern ULONG SoftQueue_modRegist(IN LONG lmoduleID);
extern ULONG SoftQueue_modUnregist(IN LONG lmoduleID);
extern ULONG SoftQueue_init(void);
extern ULONG SoftQueue_destory(void);
extern ULONG SoftQueue_snd(IN SOFT_QUEUE_MSG_s *msg_ptr, IN INT msgflg);
extern ULONG SoftQueue_rcv(INOUT SOFT_QUEUE_MSG_s *msg_ptr, IN INT msgflg);

#endif //__SOFT_COMMON_QUEUE_H__

