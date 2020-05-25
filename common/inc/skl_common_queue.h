#ifndef __SKL_COMMON_QUEUE_H__
#define __SKL_COMMON_QUEUE_H__

/* IPC_NOWAIT value is 00004000 (defined in linux/ipc.h) */
#define SKL_MSG_NOWAIT              00004000
#define SKL_MSG_BLOCK               0

#define SKL_QUEUE_RSP_INIT(pstDstMsg, pstSrcMsg) \
do \
{ \
    pstDstMsg->lDstID = pstSrcMsg->lSrcID;  \
    pstDstMsg->lSrcID = pstSrcMsg->lDstID;  \
    pstDstMsg->usCmdID = pstSrcMsg->usCmdID;    \
}while (0)

typedef struct _SKL_QUEUE_MSG_s_{
    long int lDstID;        //目标模块
    long int lSrcID;        //源模块
    ULONG   ulRet;		    //执行结果，0成功，非零失败
    UINT16  usCmdID;
    UINT16  usLen;
    UINT8   aucData[0];		//采用0长数组，不占用额外的空间
}SKL_QUEUE_MSG_s;

extern ULONG SklQueue_modRegist(IN LONG lmoduleID);
extern ULONG SklQueue_modUnregist(IN LONG lmoduleID);
extern ULONG SklQueue_init(void);
extern ULONG SklQueue_destory(void);
extern ULONG SklQueue_snd(IN SKL_QUEUE_MSG_s *msg_ptr, IN INT msgflg);
extern ULONG SklQueue_rcv(INOUT SKL_QUEUE_MSG_s *msg_ptr, IN INT msgflg);

#endif //__SKL_COMMON_QUEUE_H__

