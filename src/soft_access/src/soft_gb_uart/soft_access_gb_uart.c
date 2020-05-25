#include "soft_public.h"
#include "soft_common_queue.h"
#include "soft_common_uart.h"

#include "soft_access_common.h"

#include <string.h>
#include <pthread.h>
#include <time.h>
#include "soft_gb_uart/soft_access_gb_uart.h"
#include "soft_stor_common.h"

pthread_t gstRecvThread;
pthread_t gstParseThread;
int giUartFd = 0;

/**********************************************************
 * Function Name:        GB_uart_recv_thread
 * Description:             串口读取线程
 * Parameters[in]:        void
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
void *GB_uart_recv_thread(void * pParam)
{
    INT recvLen = 0;
    ULONG ulRet = 0;
    UCHAR aucBuff[sizeof(SOFT_QUEUE_MSG_s) + 1000] = {0};
    SOFT_QUEUE_MSG_s *pstMsg = (SOFT_QUEUE_MSG_s *)aucBuff;
    pstMsg->lDstID = SOFT_MODULE_PRI_UART_GB;
    pstMsg->usCmdID = 0xFFFF;
    
    while (1)
    {
        ulRet = SoftUart_recv(giUartFd, 1000, pstMsg->aucData, &recvLen);
        if ((ulRet != SOFT_STAT_SUCCESS) || (recvLen <= 0) || (recvLen > 1000))
        {
            continue;
        }

        pstMsg->usLen = recvLen;
        ulRet = SoftQueue_snd(pstMsg, SOFT_MSG_BLOCK);
        if (ulRet != SOFT_STAT_SUCCESS)
        {
            printf("[%s %d] SoftQueue_snd ulRet [%lu]\n", __FUNCTION__, __LINE__, ulRet);
        }

        memset(pstMsg->aucData, 0, 1000);
    }
}

/**********************************************************
 * Function Name:        GB_uart_getBCC
 * Description:             数据进行异或得到校验值
 * Parameters[in]:        IN UCHAR *pcData
                                 IN USHORT len
 * Parameters[out]:      void
 * ReturnValue:            UCHAR
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
UCHAR GB_uart_getBCC(IN UCHAR *pcData, IN USHORT len)
{
    UCHAR ucBCC = 0;
    int i = 0;
    
    for (i = 0; i < len; i++)
    {
        ucBCC = ucBCC ^ pcData[i];
    }

    return ucBCC;
}

/**********************************************************
 * Function Name:        GB_uart_sendRspErr
 * Description:             处理错误发送响应报文
 * Parameters[in]:        IN UCHAR ucCmd
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
void GB_uart_sendRspErr(IN UCHAR ucCmd)
{
    UCHAR aucErrPacket[8] = {0x55, 0x7A};
    
    if (ucCmd <= GB_UART_CLLC_SPEED_STATUS_LOG)
    {
        aucErrPacket[2] = 0xFA;
    }
    
    else if (((ucCmd >= GB_UART_SET_VEHICLE_INFO) && 
              (ucCmd <= GB_UART_SET_STATUS_SINGAL_CFG_INFO)) ||
             ((ucCmd >= GB_UART_SET_REAL_TIME) && 
              (ucCmd <= GB_UART_SET_INITIAL_MILEAGE)))
    {
        /* send err packet */
        aucErrPacket[2] = 0xFB;
    }

    aucErrPacket[4] = GB_uart_getBCC(aucErrPacket, 4);

    SoftUart_send(giUartFd, aucErrPacket, 5);

    return;
}

/**********************************************************
 * Function Name:        GB_uart_packet_bcc_check
 * Description:             BCC进行报文校验
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstMsg
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
ULONG GB_uart_packet_bcc_check(IN SOFT_QUEUE_MSG_s *pstMsg)
{
    UCHAR *pcData = pstMsg->aucData;
    UCHAR ucBCC = 0;
    
    if ((pstMsg->usLen < 7) || (pcData[0] != 0xAA) || (pcData[1] != 0x75))
    {
        printf("[%s %d] recv err packet [%s] len %d\n", __FUNCTION__, __LINE__, 
               pcData, pstMsg->usLen);
        return SOFT_STAT_FAIL_GB_BEGIN_FLAG;
    }

    ucBCC = GB_uart_getBCC(pcData, pstMsg->usLen - 1);
    if (pcData[pstMsg->usLen - 1] != ucBCC)
    {
        GB_uart_sendRspErr(pcData[GB_UART_INDEX_CMD]);
        return SOFT_STAT_FAIL_GB_BCC_CHECK;
    }

    return SOFT_STAT_SUCCESS;
}

/**********************************************************
 * Function Name:        GB_uart_TimeFormatBCD
 * Description:             当前时间转换成BCD时间
 * Parameters[in]:        void
 * Parameters[out]:      OUT UCHAR *buff
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
ULONG GB_uart_TimeFormatBCD(OUT UCHAR *buff)
{
    CHAR szTmp[16] = {0};
    time_t cur_time;
    struct tm local_tm;

    cur_time = time(NULL);
    if (cur_time < 0)
    {
        perror("time");
        return SOFT_STAT_FAIL_GET_TIME;    
    }
    
    if (localtime_r(&cur_time, &local_tm) == NULL)
    {
        perror("localtime" );
        return SOFT_STAT_FAIL_GET_LOCALTIME;
    }

#if 0
struct tm{
    int tm_sec; /* Seconds. [0-60] (1 leap second) */
    int tm_min; /* Minutes. [0-59] */
    int tm_hour; /* Hours. [0-23] */
    int tm_mday; /* Day. [1-31] */
    int tm_mon; /* Month. [0-11] */
    int tm_year; /* Year - 1900. */
    int tm_wday; /* Day of week. [0-6] */
    int tm_yday; /* Days in year.[0-365] */
    int tm_isdst; /* DST. [-1/0/1]*/
#ifdef __USE_BSD
    long int tm_gmtoff; /* Seconds east of UTC. */
    __const char *tm_zone; /* Timezone abbreviation. */
#else
    long int __tm_gmtoff; /* Seconds east of UTC. */
    __const char *__tm_zone; /* Timezone abbreviation. */
#endif
};
#endif

    snprintf(szTmp, sizeof(szTmp), "%04d%02d%02d%02d%02d%02d", 
             local_tm.tm_year + 1900, local_tm.tm_mon + 1, local_tm.tm_mday,
             local_tm.tm_hour, local_tm.tm_min, local_tm.tm_sec);

    SoftAccess_String2Bcd(szTmp + 2, buff, NULL);

    return SOFT_STAT_SUCCESS;
}

/**********************************************************
 * Function Name:        GB_uart_sendRspRealTime
 * Description:             发送实际时间响应报文
 * Parameters[in]:        void
 * Parameters[out]:      OUT UCHAR *buff
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
ULONG GB_uart_sendRspRealTime(void)
{
    UCHAR send_buf[GB_UART_LEN_COMMON + GB_UART_LEN_TIME + 1] = 
        {0x55, 0x7A, GB_UART_CLLC_REAL_TIME};
    ULONG ulRet = SOFT_STAT_SUCCESS;

    /* format date to BCD code */
    GB_uart_TimeFormatBCD(send_buf + GB_UART_INDEX_DATA);

    GB_UART_CLC_BCC(send_buf, GB_UART_LEN_TIME);
    
    ulRet = SoftUart_send(giUartFd, send_buf, GB_UART_LEN_COMMON + GB_UART_LEN_TIME + 1);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] Queue_snd : Ret %lu\n", __FUNCTION__, __LINE__, ulRet);
    }

    return ulRet;
}

/**********************************************************
 * Function Name:        GB_uart_sendRspRealTime
 * Description:             发送标准版本响应报文
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
void GB_uart_sendRspStandardVer(IN SOFT_QUEUE_MSG_s *pstMsg)
{
    UCHAR send_buf[GB_UART_LEN_COMMON + GB_UART_LEN_VER + 1] = 
        {0x55, 0x7A, GB_UART_CLLC_STANDARD_VER};
    ULONG ulRet = SOFT_STAT_SUCCESS;
    SOFT_STOR_RECORDER_STANDARD_VER_s *pstVer = 
        (SOFT_STOR_RECORDER_STANDARD_VER_s *)pstMsg->aucData;

    if (pstMsg->ulRet != SOFT_STAT_SUCCESS)
    {
        GB_uart_sendRspErr(pstMsg->usCmdID);
        return;
    }

    send_buf[GB_UART_INDEX_DATA] = pstVer->ucYear;
    send_buf[GB_UART_INDEX_DATA + 1] = pstVer->ucModifyNum;

    GB_UART_CLC_BCC(send_buf, GB_UART_LEN_VER);

    ulRet = SoftUart_send(giUartFd, send_buf, GB_UART_LEN_COMMON + GB_UART_LEN_VER + 1);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] Queue_snd : Ret %lu\n", __FUNCTION__, __LINE__, ulRet);
    }
}

/**********************************************************
 * Function Name:        GB_uart_sendRspDriverLincese
 * Description:             发送驾驶员证件号响应
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
void GB_uart_sendRspDriverLincese(IN SOFT_QUEUE_MSG_s *pstMsg)
{
    UCHAR send_buf[GB_UART_LEN_COMMON + GB_UART_LEN_DRIVER_LINCESE + 1] = 
        {0x55, 0x7A, GB_UART_CLLC_CURRENT_DRIVER_LINCENSE};
    ULONG ulRet = SOFT_STAT_SUCCESS;
    SOFT_STOR_DRIVER_LINCESE_s *pstDriverLincese = 
        (SOFT_STOR_DRIVER_LINCESE_s *)pstMsg->aucData;

    if (pstMsg->ulRet != SOFT_STAT_SUCCESS)
    {
        GB_uart_sendRspErr(pstMsg->usCmdID);
        return;
    }

    memcpy(send_buf + GB_UART_INDEX_DATA, pstDriverLincese->aucDriverLincese,
           sizeof(pstDriverLincese->aucDriverLincese));

    GB_UART_CLC_BCC(send_buf, GB_UART_LEN_DRIVER_LINCESE);

    ulRet = SoftUart_send(giUartFd, send_buf, GB_UART_LEN_COMMON + GB_UART_LEN_DRIVER_LINCESE + 1);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] Queue_snd : Ret %lu\n", __FUNCTION__, __LINE__, ulRet);
    }
}

/**********************************************************
 * Function Name:        GB_uart_sendRspVehicleInfo
 * Description:             发送车辆信息响应
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
void GB_uart_sendRspVehicleInfo(IN SOFT_QUEUE_MSG_s *pstMsg)
{
    UCHAR send_buf[GB_UART_LEN_COMMON + GB_UART_LEN_VEHICLE_INFO + 1] = 
        {0x55, 0x7A, GB_UART_CLLC_VEHICLE_INFO};
    ULONG ulRet = SOFT_STAT_SUCCESS;
    SOFT_STOR_VEHICLE_INFO_s *pstVehicleInfo = (SOFT_STOR_VEHICLE_INFO_s *)pstMsg->aucData;

    if (pstMsg->ulRet != SOFT_STAT_SUCCESS)
    {
        GB_uart_sendRspErr(pstMsg->usCmdID);
        return;
    }

    memcpy(send_buf + GB_UART_INDEX_DATA, pstVehicleInfo->aucVehicleIndentify, 17);
    memcpy(send_buf + GB_UART_INDEX_DATA + 17, pstVehicleInfo->aucVehicleLicensePlate, 11);
    memcpy(send_buf + GB_UART_INDEX_DATA + 29, pstVehicleInfo->aucVehicleLicensePlateType, 11);
    
    GB_UART_CLC_BCC(send_buf, GB_UART_LEN_VEHICLE_INFO);

    ulRet = SoftUart_send(giUartFd, send_buf, GB_UART_LEN_COMMON + GB_UART_LEN_VEHICLE_INFO + 1);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] Queue_snd : Ret %lu\n", __FUNCTION__, __LINE__, ulRet);
    }
}

/**********************************************************
 * Function Name:        GB_uart_sendRspRecordUnique
 * Description:             发送记录仪唯一标识响应
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
void GB_uart_sendRspRecordUnique(IN SOFT_QUEUE_MSG_s *pstMsg)
{
    UCHAR send_buf[GB_UART_LEN_COMMON + GB_UART_LEN_RECORD_UNIQUE + 1] = 
        {0x55, 0x7A, GB_UART_CLLC_RECORDER_UNIQUE_NUMBER};
    ULONG ulRet = SOFT_STAT_SUCCESS;
    SOFT_STOR_RECORDER_UNIQUE_INFO_s *pstRecordUnique = 
        (SOFT_STOR_RECORDER_UNIQUE_INFO_s *)pstMsg->aucData;

    if (pstMsg->ulRet != SOFT_STAT_SUCCESS)
    {
        GB_uart_sendRspErr(pstMsg->usCmdID);
        return;
    }

    memcpy(send_buf + GB_UART_INDEX_DATA, pstRecordUnique->auc3CIdentify, 7);
    memcpy(send_buf + GB_UART_INDEX_DATA + 7, pstRecordUnique->aucProductType, 16);
    memcpy(send_buf + GB_UART_INDEX_DATA + 23, pstRecordUnique->aucProductDate, 3);
    memcpy(send_buf + GB_UART_INDEX_DATA + 26, pstRecordUnique->aucProdSerialNumber, 4);
    
    GB_UART_CLC_BCC(send_buf, GB_UART_LEN_RECORD_UNIQUE);

    ulRet = SoftUart_send(giUartFd, send_buf, GB_UART_LEN_COMMON + GB_UART_LEN_RECORD_UNIQUE + 1);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] Queue_snd : Ret %lu\n", __FUNCTION__, __LINE__, ulRet);
    }
}

/**********************************************************
 * Function Name:        GB_uart_sendRspSet
 * Description:             发送设置响应报文
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
void GB_uart_sendRspSet(IN SOFT_QUEUE_MSG_s *pstMsg)
{
    UCHAR send_buf[GB_UART_LEN_COMMON + 1] = {0x55, 0x7A};
    ULONG ulRet = SOFT_STAT_SUCCESS;

    if (pstMsg->ulRet != SOFT_STAT_SUCCESS)
    {
        GB_uart_sendRspErr(pstMsg->usCmdID);
        return;
    }

    send_buf[2] = pstMsg->usCmdID;

    GB_UART_CLC_BCC(send_buf, 0);

    ulRet = SoftUart_send(giUartFd, send_buf, GB_UART_LEN_COMMON + 1);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] Queue_snd : Ret %lu\n", __FUNCTION__, __LINE__, ulRet);
    }
}

/**********************************************************
 * Function Name:        GB_uart_modulePacketProcess
 * Description:             处理其他模块发送给接入模块报文
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
void GB_uart_modulePacketProcess(IN SOFT_QUEUE_MSG_s *pstMsg)
{
    switch (pstMsg->usCmdID)
    {
        case GB_UART_CLLC_STANDARD_VER:                     /* 执行标准版本 */
            GB_uart_sendRspStandardVer(pstMsg);
            break;
            
        case GB_UART_CLLC_CURRENT_DRIVER_LINCENSE:          /* 当前驾驶员信息*/
            GB_uart_sendRspDriverLincese(pstMsg);
            break;
            
        case GB_UART_CLLC_ACCUMULATED_MILEAGE:              /* 累计行驶里程 */
            break;
            
        case GB_UART_CLLC_IMPULSE_RATIO:                    /* 获取脉冲系数 */
            break;
            
        case GB_UART_CLLC_VEHICLE_INFO:                     /* 车辆信息 */
            GB_uart_sendRspVehicleInfo(pstMsg);
            break;
            
        case GB_UART_CLLC_STATUS_SINGAL_CONFIG_INFO:        /* 信号配置信息 */
            break;
            
        case GB_UART_CLLC_RECORDER_UNIQUE_NUMBER:           /* 记录仪唯一编号 */
            GB_uart_sendRspRecordUnique(pstMsg);
            break;
            
        case GB_UART_CLLC_DRIVE_SPEED_ANNAL:                /* 行驶速度记录 */
            break;
            
        case GB_UART_CLLC_LOCAT_INFO_ANNAL:                 /* 位置信息记录 */
            break;
            
        case GB_UART_CLLC_ACCIDENT_SUSPECT_ANNAL:           /* 事故疑点记录 */
            break;
            
        case GB_UART_CLLC_OVER_DRIVING_ANNAL:               /* 超时驾驶记录 */
            break;
            
        case GB_UART_CLLC_DRIVER_IDENTITY_ANNAL:            /* 驾驶人身份记录 */
            break;
            
        case GB_UART_CLLC_EXTERN_POWER_SUPPLY_ANNAL:        /* 外部供电记录 */
            break;
            
        case GB_UART_CLLC_PARAM_MODIFY_ANNAL:               /* 参数修改记录 */
            break;
            
        case GB_UART_CLLC_SPEED_STATUS_LOG:                 /* 速度状态记录 */
            break;
            
        case GB_UART_SET_VEHICLE_INFO:                      /* 车辆信息 */
        case GB_UART_SET_INITIAL_INSTALL_TIME:              /* 初次安装日期 */
        case GB_UART_SET_REAL_TIME:                         /* 记录仪时间 */
        case GB_UART_SET_INITIAL_MILEAGE:                   /* 初次安装已行驶总里程 */
            GB_uart_sendRspSet(pstMsg);
            break;
        
        case GB_UART_SET_STATUS_SINGAL_CFG_INFO:            /* 状态量配置信息 */
            break;
            
        case GB_UART_SET_IMPULSE_RATIO:                     /* 记录脉冲系数 */
            break;
            
        case GB_UART_VERIFICATION_ENTER_OR_KEEP:            /* 进入或保持检定状态 */
            break;
            
        case GB_UART_VERIFICATION_MILEAGE:                  /* 里程误差测量 */
            break;
            
        case GB_UART_VERIFICATION_PULSE_COEFFICIENT:        /* 脉冲系数误差测量 */
            break;
            
        case GB_UART_VERIFICATION_REAL_TIME:                /* 实时时间误差测量 */
            break;
            
        case GB_UART_VERIFICATION_EXIT:                     /* 范围正常状态 */
            break;

        default:
            break;
    }
}

/**********************************************************
 * Function Name:        GB_uart_sendReqToModule
 * Description:             向其他模块发送请求
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstMsg
                                 IN ULONG ulDstModule
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
ULONG GB_uart_sendReqToModule(IN UCHAR *reqMsg, IN ULONG ulDstModule)
{
    UCHAR aucBuff[sizeof(SOFT_QUEUE_MSG_s) + 1000] = {0};
    ULONG ulRet = SOFT_STAT_SUCCESS;
    SOFT_QUEUE_MSG_s *pstModuleMsg = (SOFT_QUEUE_MSG_s *)aucBuff;

    /* restruct Data */
    GB_UART_MK_MODULE_MSG(reqMsg, pstModuleMsg);

    /* 转发存储模块 */
    pstModuleMsg->lDstID = ulDstModule;
    pstModuleMsg->lSrcID = SOFT_MODULE_ACCESS;

    ulRet = SoftQueue_snd(pstModuleMsg, SOFT_MSG_BLOCK);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] Queue_snd : Ret %lu\n", __FUNCTION__, __LINE__, ulRet);
    }

    return ulRet;
}

/**********************************************************
 * Function Name:        GB_uart_getCmd
 * Description:             处理采集报文
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
void GB_uart_getCmd(IN SOFT_QUEUE_MSG_s *pstMsg)
{
    UCHAR *pcData = pstMsg->aucData;

    switch (pcData[GB_UART_INDEX_CMD])
    {
        case GB_UART_CLLC_REAL_TIME:                    /* 实时时间 */
            GB_uart_sendRspRealTime();
            break;
            
        case GB_UART_CLLC_STANDARD_VER:                 /* 执行标准版本 */
        case GB_UART_CLLC_CURRENT_DRIVER_LINCENSE:      /* 当前驾驶员信息*/
        case GB_UART_CLLC_VEHICLE_INFO:                 /* 车辆信息 */
        case GB_UART_CLLC_RECORDER_UNIQUE_NUMBER:       /* 记录仪唯一编号 */
            GB_uart_sendReqToModule(pcData, SOFT_MODULE_STOR);
            break;

        case GB_UART_CLLC_ACCUMULATED_MILEAGE:          /* 累计行驶里程 */
            break;
        case GB_UART_CLLC_IMPULSE_RATIO:                /* 获取脉冲系数 */
            break;
        case GB_UART_CLLC_STATUS_SINGAL_CONFIG_INFO:    /* 信号配置信息 */
            break;
        case GB_UART_CLLC_DRIVE_SPEED_ANNAL:            /* 行驶速度记录 */
            break;
        case GB_UART_CLLC_LOCAT_INFO_ANNAL:             /* 位置信息记录 */
            break;
        case GB_UART_CLLC_ACCIDENT_SUSPECT_ANNAL:       /* 事故疑点记录 */
            break;
        case GB_UART_CLLC_OVER_DRIVING_ANNAL:           /* 超时驾驶记录 */
            break;
        case GB_UART_CLLC_DRIVER_IDENTITY_ANNAL:        /* 驾驶人身份记录 */
            break;
        case GB_UART_CLLC_EXTERN_POWER_SUPPLY_ANNAL:    /* 外部供电记录 */
            break;
        case GB_UART_CLLC_PARAM_MODIFY_ANNAL:           /* 参数修改记录 */
            break;
        case GB_UART_CLLC_SPEED_STATUS_LOG:             /* 速度状态记录 */
            break;
        default:
            break;
    }
}

/**********************************************************
 * Function Name:        GB_uart_setVehicleInfo
 * Description:             设置车辆信息
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
void GB_uart_setVehicleInfo(IN SOFT_QUEUE_MSG_s *pstMsg)
{

    UCHAR *pcSrcMsg = pstMsg->aucData;
    UCHAR aucMsg[sizeof(SOFT_QUEUE_MSG_s) + sizeof(SOFT_STOR_VEHICLE_INFO_s)] = {0};
    USHORT usLen = 0; 
    ULONG ulRet = SOFT_STAT_SUCCESS;
    SOFT_QUEUE_MSG_s *pstDstMsg = (SOFT_QUEUE_MSG_s *)aucMsg;
    SOFT_STOR_VEHICLE_INFO_s *pstVehicleInfo = (SOFT_STOR_VEHICLE_INFO_s *)pstDstMsg->aucData;

    usLen = SoftAccess_String2Short(pcSrcMsg + GB_UART_INDEX_LEN);

    if (pcSrcMsg == NULL || usLen < 41)
    {
        printf("[%s %d] pcSrcMsg %p, Len %d\n", __FUNCTION__, __LINE__, pcSrcMsg, usLen);
        return;
    }
    
    pstDstMsg->lDstID = SOFT_MODULE_STOR;
    pstDstMsg->lSrcID = SOFT_MODULE_ACCESS;
    pstDstMsg->usCmdID = pcSrcMsg[GB_UART_INDEX_CMD];
    pstDstMsg->usLen = sizeof(*pstVehicleInfo);
    
    memcpy(pstVehicleInfo->aucVehicleIndentify, pcSrcMsg + GB_UART_INDEX_DATA, 17);
    memcpy(pstVehicleInfo->aucVehicleLicensePlate, pcSrcMsg + GB_UART_INDEX_DATA + 17, 12);
    memcpy(pstVehicleInfo->aucVehicleLicensePlateType, pcSrcMsg + GB_UART_INDEX_DATA + 29, 12);

    ulRet = SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] SoftQueue_snd ulRet [%lu]\n", __FUNCTION__, __LINE__, ulRet);
    }

    return;
}

/**********************************************************
 * Function Name:        GB_uart_setVehicleInfo
 * Description:             设置初始里程
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
void GB_uart_setInitialMileage(IN SOFT_QUEUE_MSG_s *pstMsg)
{
    UCHAR *pcSrcMsg = pstMsg->aucData;
    UCHAR aucMsg[sizeof(SOFT_QUEUE_MSG_s) + sizeof(SOFT_STOR_ACCUMULATED_MILEAGE_s)] = {0};
    USHORT usLen = SoftAccess_String2Short(pcSrcMsg + GB_UART_INDEX_LEN);
    ULONG ulRet = SOFT_STAT_SUCCESS;
    SOFT_QUEUE_MSG_s *pstDstMsg = (SOFT_QUEUE_MSG_s *)aucMsg;
    SOFT_STOR_ACCUMULATED_MILEAGE_s *pstAccInfo = 
        (SOFT_STOR_ACCUMULATED_MILEAGE_s *)pstDstMsg->aucData;

    if (pcSrcMsg == NULL || usLen < 20)
    {
        printf("[%s %d] pcSrcMsg %p, Len %d\n", __FUNCTION__, __LINE__, pcSrcMsg, usLen);
        return;
    }

    pstDstMsg->lDstID = SOFT_MODULE_STOR;
    pstDstMsg->lSrcID = SOFT_MODULE_ACCESS;
    pstDstMsg->usCmdID = pcSrcMsg[GB_UART_INDEX_CMD];
    pstDstMsg->usLen = sizeof(*pstAccInfo);
    
    memcpy(pstAccInfo->aucInitialMileage, pcSrcMsg + GB_UART_INDEX_DATA + 12, 4);

    ulRet = SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] SoftQueue_snd ulRet [%lu]\n", __FUNCTION__, __LINE__, ulRet);
    }

    return;
}

/**********************************************************
 * Function Name:        GB_uart_setCmd
 * Description:             处理设置报文
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
void GB_uart_setCmd(IN SOFT_QUEUE_MSG_s *pstMsg)
{
    UCHAR *pcData = pstMsg->aucData;

    switch (pcData[GB_UART_INDEX_CMD])
    {
        case GB_UART_SET_VEHICLE_INFO:              /* 车辆信息 */
            GB_uart_setVehicleInfo(pstMsg);
            break;

        case GB_UART_SET_INITIAL_INSTALL_TIME:      /* 初次安装日期 */
            /* 直接转发到配置模块，无需解析 */
            SOFT_CODE_DEBUG("[%s %d] setVehicleInfo after\n", __FUNCTION__, __LINE__);
            GB_uart_sendReqToModule(pcData, SOFT_MODULE_STOR);
            SOFT_CODE_DEBUG("[%s %d] setVehicleInfo after\n", __FUNCTION__, __LINE__);
            break;
        case GB_UART_SET_STATUS_SINGAL_CFG_INFO:    /* 状态量配置信息 */
            break;
            
        case GB_UART_SET_REAL_TIME:                 /* 记录仪时间 */
            /* 直接转发到配置模块，无需解析 */
            GB_uart_sendReqToModule(pcData, SOFT_MODULE_STOR);
            break;
            
        case GB_UART_SET_IMPULSE_RATIO:             /* 记录脉冲系数 */
            break;
            
        case GB_UART_SET_INITIAL_MILEAGE:           /* 初次安装已行驶总里程 */
            GB_uart_setInitialMileage(pstMsg);
            break;
           
        default:
            break;
    }
    
}

/**********************************************************
 * Function Name:        GB_uart_verification
 * Description:             处理检定测试报文
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
void GB_uart_verification(SOFT_QUEUE_MSG_s *pstMsg)
{
    UCHAR aucBuff[sizeof(SOFT_QUEUE_MSG_s) + 1000] = {0};
    SOFT_QUEUE_MSG_s *pstModuleMsg = (SOFT_QUEUE_MSG_s *)aucBuff;

    /* restruct Data */
    GB_UART_MK_MODULE_MSG(pstMsg->aucData, pstModuleMsg);

    /* 转发检定测试模块(目前未确定如何与MCU 交互实现)  */
    pstModuleMsg->lDstID = SOFT_MODULE_STOR;
    pstModuleMsg->lSrcID = SOFT_MODULE_ACCESS;
}

/**********************************************************
 * Function Name:        GB_uart_main_process
 * Description:             串口报文处理
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
void GB_uart_main_process(IN SOFT_QUEUE_MSG_s *pstMsg)
{
    UCHAR *pcData = pstMsg->aucData;
    
    if (pcData[GB_UART_INDEX_CMD] <= GB_UART_CLLC_MAX)
    {
        /* 采集类信息 */
        GB_uart_getCmd(pstMsg);
    }
    else if (pcData[GB_UART_INDEX_CMD] <= GB_UART_SET_MAX)
    {
        /* 配置类信息 */
        GB_uart_setCmd(pstMsg);
    }
    else
    {
        /* 检定测试类 */
        GB_uart_verification(pstMsg);
    }
}

/**********************************************************
 * Function Name:        GB_uart_parse_thread
 * Description:             串口报文解析线程
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
void *GB_uart_parse_thread(void * pParam)
{
    ULONG ulRet = 0;
    UCHAR aucBuff[sizeof(SOFT_QUEUE_MSG_s) + 1000] = {0};
    SOFT_QUEUE_MSG_s *pstMsg = (SOFT_QUEUE_MSG_s *)aucBuff;
    pstMsg->lDstID = SOFT_MODULE_PRI_UART_GB;

    while (1)
    {
        ulRet = SoftQueue_rcv(pstMsg, SOFT_MSG_NOWAIT);
        if (ulRet != SOFT_STAT_SUCCESS)
        {
            continue;
        }

        if (pstMsg->usCmdID == 0xFFFF)
        {
            /* uart recv packet */
            if (GB_uart_packet_bcc_check(pstMsg) != SOFT_STAT_SUCCESS)
            {
                continue;
            }

            GB_uart_main_process(pstMsg);
        }
    }
}

/**********************************************************
 * Function Name:        GB_uart_init
 * Description:             国标串口初始化
 * Parameters[in]:        void
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
ULONG GB_uart_init(void)
{
    ULONG ulRet = SOFT_STAT_SUCCESS;

    SOFT_UART_CFG_PARAM_s stUartCom;
    
    /* init uart port */
    memset(&stUartCom, 0, sizeof(stUartCom));
    stUartCom.pcName = SOFT_VEHICLE_GB_UART;
    stUartCom.baud_rate = 115200;
    stUartCom.flow_ctrl = 0;;
    stUartCom.data_bits = 8;                                                     
	stUartCom.parity    = 'N';                                                      
	stUartCom.stop_bits = 1; 
    ulRet = SoftUart_open(stUartCom, &giUartFd);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d ] Uart_open %s fail err: %lu\n", __FUNCTION__, __LINE__, 
               SOFT_VEHICLE_GB_UART, ulRet);
        return ulRet;
    }
    
    /* register queue */
    ulRet = SoftQueue_modRegist(SOFT_MODULE_PRI_UART_GB);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d ] modRegist fail err: %lu\n", __FUNCTION__, __LINE__, ulRet);
        return ulRet;
    }

    /* create uart recv thread */
    pthread_create(&gstRecvThread, NULL, GB_uart_recv_thread, NULL);
    if (pthread_detach(gstRecvThread) != 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
    }

    pthread_create(&gstParseThread, NULL, GB_uart_parse_thread, NULL);
    if (pthread_detach(gstParseThread) != 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
    } 

    return SOFT_STAT_SUCCESS;
}

/**********************************************************
 * Function Name:        GB_uart_destory
 * Description:             国标串口销毁
 * Parameters[in]:        void
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.21
***********************************************************/
void GB_uart_destory(void)
{
    INT iRet = 0;
    ULONG ulRet = 0;

    SoftUart_close(giUartFd);

    /* destory pthread */
    iRet = pthread_cancel(gstRecvThread);
    if (iRet != 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
    }
    
    iRet = pthread_cancel(gstParseThread);    
    if (iRet != 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
    }

    printf("[%s %d] GB_uart modUnregist\n", __FUNCTION__, __LINE__);
    /* unregister queue */
    ulRet = SoftQueue_modUnregist(SOFT_MODULE_PRI_UART_GB);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d ] modUnregist fail err: %lu\n", __FUNCTION__, __LINE__, ulRet);
    }

    return;
}


