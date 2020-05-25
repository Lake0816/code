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
    long int lDstID;        //Ŀ��ģ��
    long int lSrcID;        //Դģ��
    ULONG   ulRet;		    //ִ�н����0�ɹ�������ʧ��
    UINT16  usCmdID;
    UINT16  usLen;
    UINT8   aucData[0];		//����0�����飬��ռ�ö���Ŀռ�
}SKL_QUEUE_MSG_s;

extern ULONG SklQueue_modRegist(IN LONG lmoduleID);
extern ULONG SklQueue_modUnregist(IN LONG lmoduleID);
extern ULONG SklQueue_init(void);
extern ULONG SklQueue_destory(void);
extern ULONG SklQueue_snd(IN SKL_QUEUE_MSG_s *msg_ptr, IN INT msgflg);
extern ULONG SklQueue_rcv(INOUT SKL_QUEUE_MSG_s *msg_ptr, IN INT msgflg);

#endif //__SKL_COMMON_QUEUE_H__

