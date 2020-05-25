#include "soft_public.h"
#include "soft_stor_common.h"
#include "soft_access_common.h"

#include "soft_common_queue.h"

#include "soft_stor.h"
#include "cfg/soft_stor_cfg.h"

#include <string.h>
#include <pthread.h>

pthread_t gstStorThread;

/**********************************************************
 * Function Name:        SoftStor_main_process
 * Description:             存储模块消息处理
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
void SoftStor_main_process(IN SOFT_QUEUE_MSG_s *pstMsg)
{
    SOFT_CODE_DEBUG("[%s %d] CmdID %x\n", __FUNCTION__, __LINE__, pstMsg->usCmdID);

    switch (pstMsg->usCmdID)
    {
        case GB_UART_SET_VEHICLE_INFO:
        case GB_UART_SET_INITIAL_INSTALL_TIME:
        case GB_UART_SET_REAL_TIME:
        case GB_UART_SET_INITIAL_MILEAGE:
        case GB_UART_CLLC_STANDARD_VER:
        case GB_UART_CLLC_VEHICLE_INFO:
        case GB_UART_CLLC_RECORDER_UNIQUE_NUMBER:
        case GB_UART_CLLC_CURRENT_DRIVER_LINCENSE:
            SoftStor_cfg_main_process(pstMsg);
            break;
        case GB_UART_SET_STATUS_SINGAL_CFG_INFO:
            /* 状态配置信息 */
            
            break;
        case GB_UART_SET_IMPULSE_RATIO:
            /* 脉冲系数 */
            
            break;
        default:
            break;
    }
    
    return;
}

/**********************************************************
 * Function Name:        SoftStor_recv_thread
 * Description:             存储模块消息读取
 * Parameters[in]:        IN void * pParam
 * Parameters[out]:      void
 * ReturnValue:            void *
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
void *SoftStor_recv_thread(IN void * pParam)
{
    ULONG ulRet = 0;
    UCHAR aucBuff[sizeof(SOFT_QUEUE_MSG_s) + 1000] = {0};
    SOFT_QUEUE_MSG_s *pstMsg = (SOFT_QUEUE_MSG_s *)aucBuff;
    pstMsg->lDstID = SOFT_MODULE_STOR;

    while (1)
    {
        ulRet = SoftQueue_rcv(pstMsg, SOFT_MSG_NOWAIT);
        if (ulRet != SOFT_STAT_SUCCESS)
        {
            continue;
        }

        SoftStor_main_process(pstMsg);
    }

    return NULL;
}

/**********************************************************
 * Function Name:        SoftStor_init
 * Description:             存储模块初始化
 * Parameters[in]:        void
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
ULONG SoftStor_init(void)
{
    int iRet = SOFT_STAT_SUCCESS;

    /* Initialize the configuration file */
    SoftStor_cfg_init();

    /* create pthread */
    iRet = pthread_create(&gstStorThread, NULL, SoftStor_recv_thread, NULL);
    if (iRet != 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        return SOFT_STAT_FAILE_THREAD_CREATE;
    }

    iRet = pthread_detach(gstStorThread);
    if (iRet != 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        return SOFT_STAT_FAILE_THREAD_DETACH;
    }
    
    /* Registration queue */
    SoftQueue_modRegist(SOFT_MODULE_STOR);

    return SOFT_STAT_SUCCESS;
}

/**********************************************************
 * Function Name:        SoftStor_destory
 * Description:             存储模块销毁
 * Parameters[in]:        void
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
void SoftStor_destory(void)
{
    INT iRet = 0;
    ULONG ulRet = SOFT_STAT_SUCCESS;
    
    SOFT_CODE_DEBUG("[%s %d] Stor modUnregist\n", __FUNCTION__, __LINE__);
    /* Unregister queue */
    ulRet = SoftQueue_modUnregist(SOFT_MODULE_STOR);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d ] modUnregist fail err: %lu\n", __FUNCTION__, __LINE__, ulRet);
    }

    /* destory pthread */
    iRet = pthread_cancel(gstStorThread);
    if (iRet != 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
    }

    /* destory xml cfg */
    SoftStor_cfg_destory();
    
    return;
}

