#include "skl_public.h"
#include "skl_stor_common.h"
#include "skl_access_common.h"

#include "skl_common_queue.h"

#include "skl_stor.h"
#include "cfg/skl_stor_cfg.h"

#include <string.h>
#include <pthread.h>

pthread_t gstStorThread;

/**********************************************************
 * Function Name:        SklStor_main_process
 * Description:             存储模块消息处理
 * Parameters[in]:        IN SKL_QUEUE_MSG_s *pstMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
void SklStor_main_process(IN SKL_QUEUE_MSG_s *pstMsg)
{
    SKL_CODE_DEBUG("[%s %d] CmdID %x\n", __FUNCTION__, __LINE__, pstMsg->usCmdID);

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
            SklStor_cfg_main_process(pstMsg);
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
 * Function Name:        SklStor_recv_thread
 * Description:             存储模块消息读取
 * Parameters[in]:        IN void * pParam
 * Parameters[out]:      void
 * ReturnValue:            void *
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
void *SklStor_recv_thread(IN void * pParam)
{
    ULONG ulRet = 0;
    UCHAR aucBuff[sizeof(SKL_QUEUE_MSG_s) + 1000] = {0};
    SKL_QUEUE_MSG_s *pstMsg = (SKL_QUEUE_MSG_s *)aucBuff;
    pstMsg->lDstID = SKL_MODULE_STOR;

    while (1)
    {
        ulRet = SklQueue_rcv(pstMsg, SKL_MSG_NOWAIT);
        if (ulRet != SKL_STAT_SUCCESS)
        {
            continue;
        }

        SklStor_main_process(pstMsg);
    }

    return NULL;
}

/**********************************************************
 * Function Name:        SklStor_init
 * Description:             存储模块初始化
 * Parameters[in]:        void
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
ULONG SklStor_init(void)
{
    int iRet = SKL_STAT_SUCCESS;

    /* Initialize the configuration file */
    SklStor_cfg_init();

    /* create pthread */
    iRet = pthread_create(&gstStorThread, NULL, SklStor_recv_thread, NULL);
    if (iRet != 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        return SKL_STAT_FAILE_THREAD_CREATE;
    }

    iRet = pthread_detach(gstStorThread);
    if (iRet != 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        return SKL_STAT_FAILE_THREAD_DETACH;
    }
    
    /* Registration queue */
    SklQueue_modRegist(SKL_MODULE_STOR);

    return SKL_STAT_SUCCESS;
}

/**********************************************************
 * Function Name:        SklStor_destory
 * Description:             存储模块销毁
 * Parameters[in]:        void
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
void SklStor_destory(void)
{
    INT iRet = 0;
    ULONG ulRet = SKL_STAT_SUCCESS;
    
    SKL_CODE_DEBUG("[%s %d] Stor modUnregist\n", __FUNCTION__, __LINE__);
    /* Unregister queue */
    ulRet = SklQueue_modUnregist(SKL_MODULE_STOR);
    if (ulRet != SKL_STAT_SUCCESS)
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
    SklStor_cfg_destory();
    
    return;
}

