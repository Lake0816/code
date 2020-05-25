#ifndef __SOFT_STOR_H__
#define __SOFT_STOR_H__

extern void SoftStor_main_process(SOFT_QUEUE_MSG_s *pstMsg);
extern void *SoftStor_recv_thread(void * pParam);

#endif  //__SOFT_STOR_H__

