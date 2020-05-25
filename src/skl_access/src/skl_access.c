#include "skl_public.h"

#include <pthread.h>
#include <string.h>

#include "skl_common_queue.h"
#include "skl_access_common.h"
#include "skl_access.h"
#include "skl_gb_uart/skl_access_gb_uart.h"

pthread_t gstAccessThread;

/**********************************************************
 * Function Name:        SklAccess_String2Short
 * Description:             字符串转short
 * Parameters[in]:        void
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.22
***********************************************************/
USHORT SklAccess_String2Short(UINT8 *pData)
{   
    USHORT TempH = 0x0000;
    USHORT TempL = 0x0000;
    
    TempH = TempH | pData[0];
    TempL = TempL | pData[1];
    return ((TempH << 8) | (TempL));
}

/**********************************************************
 * Function Name:        SklAccess_Bcd2String
 * Description:             bcd字符串转普通字符串
 * Parameters[in]:        IN UCHAR *BCD
                                 IN INT BCD_length
 * Parameters[out]:      OUT CHAR *str                      
 * ReturnValue:            BOOL
 * Author:                   liuke
 * Date:                      2020.05.22
***********************************************************/
BOOL SklAccess_Bcd2String(IN UCHAR *BCD, IN INT BCD_length, OUT CHAR *str)
{
    int i = 0;
    int j = 0;

    if (BCD == 0 || BCD_length == 0)
    {
        return BOOL_FALSE;
    }

    for (i = 0, j = 0; i < BCD_length; i++, j+=2)
    {
        str[j]= (BCD[i] >> 4) > 9 ? (BCD[i] >> 4) - 10 + 'A' : (BCD[i] >>4) + '0';
        str[j + 1] =((BCD[i] & 0x0F) > 9) ? ((BCD[i] & 0x0F) - 10 + 'A') : ((BCD[i] & 0x0F)+'0');
    }
    
    str[j]='\0';
    
    return BOOL_TRUE;
}

/**********************************************************
 * Function Name:        SklAccess_String2Bcd
 * Description:             普通字符串转BCD字符串
 * Parameters[in]:        IN CHAR *str
 * Parameters[out]:      OUT UCHAR *BCD
                                 OUT INT *BCD_length
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.22
***********************************************************/
BOOL SklAccess_String2Bcd(IN CHAR *str, OUT UCHAR *BCD, OUT INT *BCD_length)
{
    INT tmp = strlen(str);
    INT i = 0;
    INT j = 0;

    if( str == NULL)
    {
        return BOOL_FALSE;
    }

    tmp -= tmp % 2;
    if(tmp==0)
    {
        return BOOL_FALSE;
    }

    for(i=0;i<tmp;i++)
    {
        if((str == 0) ||
            !((str[i]>='0' && str[i]<='9') || 
              (str[i]>='a' && str[i]<='f') || 
              (str[i]>='A' && str[i]<='F')))
        {
            return BOOL_FALSE;
        }
    }

    for(i = 0, j = 0; i < tmp / 2; i++, j += 2)
    {
        (str[j] > '9') ? 
        (
            (str[j] > 'F') ? (BCD[i] = str[j] - 'a' + 10) : (BCD[i] = str[j] - 'A' + 10)
        ): 
        (BCD[i] = str[j] - '0');

        (str[j + 1] > '9') ? 
        (
            (str[j + 1] > 'F') ? (BCD[i] = (BCD[i] << 4) + str[j + 1] - 'a' + 10) : (BCD[i] = (BCD[i] << 4) + str[j + 1] - 'A' + 10)
        ) : 
        (BCD[i] = (BCD[i] << 4) + str[j + 1] - '0');        
    }

    if(BCD_length)
    {
        *BCD_length=tmp/2;
    }

    return BOOL_TRUE;
}

/**********************************************************
 * Function Name:        SklAccess_Bcd2ULONG
 * Description:             BCD整型转ULONG
 * Parameters[in]:        IN ULONG m
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.22
***********************************************************/
ULONG SklAccess_Bcd2ULONG(IN ULONG m)
{   ULONG a = 0;
    ULONG b = 1;   
    ULONG n = 0;
    
    while(m)  
    {   
        n = 0xF & m;     
        m = m >> 4;   
        a = a + b * n;   
        b = b * 10;  
    }
    
    return a;
}

/**********************************************************
 * Function Name:        SklAccess_Bcd2ULONG
 * Description:             ULONG转BCD整型
 * Parameters[in]:        IN ULONG m
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.22
***********************************************************/
ULONG SklAccess_ULONG2Bcd(IN ULONG m)
{    
    ULONG r = 0;
    ULONG n = 1;   
    INT a = 0;   

    while(m)   
    {      
        a = m % 10;    
        m = m / 10;      
        r = r + n * a;      
        a = a << 4;  
    }

    return r;
}

/**********************************************************
 * Function Name:        SklAccess_main_process
 * Description:             接入模块消息处理
 * Parameters[in]:        IN SKL_QUEUE_MSG_s *pstMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.22
***********************************************************/
void SklAccess_main_process(IN SKL_QUEUE_MSG_s *pstMsg)
{
    if (pstMsg->usCmdID >= 0 && pstMsg->usCmdID <= 0xFF)
    {
        GB_uart_modulePacketProcess(pstMsg);
    }
    
    return;
}

/**********************************************************
 * Function Name:        SklAccess_recv_thread
 * Description:             接入模块消息接收
 * Parameters[in]:        IN void * pParam
 * Parameters[out]:      void
 * ReturnValue:            void *
 * Author:                   liuke
 * Date:                      2020.05.22
***********************************************************/
void *SklAccess_recv_thread(void * pParam)
{
    ULONG ulRet = 0;
    UCHAR aucBuff[sizeof(SKL_QUEUE_MSG_s) + 1000] = {0};
    SKL_QUEUE_MSG_s *pstMsg = (SKL_QUEUE_MSG_s *)aucBuff;
    pstMsg->lDstID = SKL_MODULE_ACCESS;

    while (1)
    {
        ulRet = SklQueue_rcv(pstMsg, SKL_MSG_NOWAIT);
        if (ulRet != SKL_STAT_SUCCESS)
        {
            continue;
        }

        SklAccess_main_process(pstMsg);
    }

    return NULL;
}

/**********************************************************
 * Function Name:        SklAccess_init
 * Description:             接入模块初始化
 * Parameters[in]:        void
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.22
***********************************************************/
ULONG SklAccess_init(void)
{
    int iRet = 0;
    ULONG ulRet = SKL_STAT_SUCCESS;

    ulRet = GB_uart_init();
    if (ulRet != SKL_STAT_SUCCESS)
    {
        printf("[%s %d] GB_uart_init err : %lu\n", __FUNCTION__, __LINE__, ulRet);
        return ulRet;
    }

    /* create pthread */
    iRet = pthread_create(&gstAccessThread, NULL, SklAccess_recv_thread, NULL);
    if (iRet != 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        return SKL_STAT_FAILE_THREAD_CREATE;
    }

    iRet = pthread_detach(gstAccessThread);
    if (iRet != 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        return SKL_STAT_FAILE_THREAD_DETACH;
    }
    
    /* Registration queue */
    SklQueue_modRegist(SKL_MODULE_ACCESS);

    return SKL_STAT_SUCCESS;
}

/**********************************************************
 * Function Name:        SklAccess_destory
 * Description:             接入模块销毁
 * Parameters[in]:        void
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.22
***********************************************************/
void SklAccess_destory(void)
{
    INT iRet = 0;
    ULONG ulRet = SKL_STAT_SUCCESS;

    SKL_CODE_DEBUG("[%s %d] Access modUnregist\n", __FUNCTION__, __LINE__);
    /* Unregister queue */
    ulRet = SklQueue_modUnregist(SKL_MODULE_ACCESS);
    if (ulRet != SKL_STAT_SUCCESS)
    {
        printf("[%s %d ] modUnregist fail err: %lu\n", __FUNCTION__, __LINE__, ulRet);
    }

    /* destory pthread */
    iRet = pthread_cancel(gstAccessThread);
    if (iRet != 0)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
    }

    GB_uart_destory();
    
    return;
}

