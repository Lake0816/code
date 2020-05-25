#include "cfg/soft_stor_cfg.h"
#include "soft_stor_common.h"
#include "soft_access_common.h"

#include <unistd.h>
#include <time.h>
#include "soft_common_queue.h"

static SOFT_STOR_CFG_s gstStorCfg;

/**********************************************************
 * Function Name:        SoftStor_cfg_init
 * Description:             ������ģ���ʼ��
 * Parameters[in]:        void
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
ULONG SoftStor_cfg_init(void)
{
    ULONG ulRet = SOFT_STAT_SUCCESS;

    if (access(SOFT_DEV_CFG_FILE, F_OK) == 0)
    {
        ulRet = soft_xml_loadXml(SOFT_DEV_CFG_FILE, &gstStorCfg.pstCfgRoot);
        if (ulRet != SOFT_STAT_SUCCESS)
        {
            printf("[%s %d] loadXml fail 0x%lx\n", __FUNCTION__, __LINE__, ulRet);
            return ulRet;
        }
    }
    else
    {
        ulRet = soft_xml_loadString(SOFT_DEV_CFG_DEFAUT, &gstStorCfg.pstCfgRoot);
        if (ulRet != SOFT_STAT_SUCCESS)
        {
            printf("[%s %d] loadXml fail 0x%lx\n", __FUNCTION__, __LINE__, ulRet);
            return ulRet;
        }

        SoftStor_cfg_save();
    }

    return ulRet;
}

/**********************************************************
 * Function Name:        softStor_cfg_getText
 * Description:             ��ȡ�ڵ���Ϣ
 * Parameters[in]:        IN CHAR *pcPath
                                 IN INT iLen
                                 INOUT CHAR *pcTxt
 * Parameters[out]:      INOUT CHAR *pcTxt
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
ULONG softStor_cfg_getText(IN CHAR *pcPath, IN INT iLen, INOUT CHAR *pcTxt)
{
    ULONG ulRet = SOFT_STAT_SUCCESS;
    SOFT_XML_NODE_s *pstNode = NULL;
    
    ulRet = soft_xml_getNodeByPath(pcPath, gstStorCfg.pstCfgRoot, &pstNode);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] getNodeByPath fail 0x%lx\n", __FUNCTION__, __LINE__, ulRet);
        return ulRet;
    }

    soft_xml_getNodeText(pstNode, iLen, pcTxt);

    return ulRet;
}

/**********************************************************
 * Function Name:        softStor_cfg_setText
 * Description:             ���ýڵ���Ϣ
 * Parameters[in]:        IN CHAR *pcPath
                                 IN CHAR *pcTxt
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
ULONG softStor_cfg_setText(IN CHAR *pcPath, IN CHAR *pcTxt)
{
    ULONG ulRet = SOFT_STAT_SUCCESS;
    SOFT_XML_NODE_s *pstNode = NULL;
    
    ulRet = soft_xml_getNodeByPath(pcPath, gstStorCfg.pstCfgRoot, &pstNode);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] getNodeByPath fail 0x%lx\n", __FUNCTION__, __LINE__, ulRet);
        return ulRet;
    }

    ulRet = soft_xml_setNodeText(pstNode, pcTxt);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] setNodeText fail 0x%lx\n", __FUNCTION__, __LINE__, ulRet);
        return ulRet;
    }

    return ulRet;
}

/**********************************************************
 * Function Name:        SoftStor_cfg_save
 * Description:             �����ļ�����
 * Parameters[in]:        IN CHAR *pcPath
                                 IN CHAR *pcTxt
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
ULONG SoftStor_cfg_save(void)
{
    ULONG ulRet = SOFT_STAT_SUCCESS;

    ulRet = soft_xml_saveXml(SOFT_DEV_CFG_FILE, gstStorCfg.pstCfgRoot);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] saveXml fail 0x%lx\n", __FUNCTION__, __LINE__, ulRet);
        return ulRet;
    }

    return ulRet;
}

/**********************************************************
 * Function Name:        SoftStor_cfg_destory
 * Description:             �洢ģ������
 * Parameters[in]:        IN CHAR *pcPath
                                 IN CHAR *pcTxt
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
void SoftStor_cfg_destory(void)
{
    soft_xml_destoryXml(&gstStorCfg.pstCfgRoot);

    return;
}

/**********************************************************
 * Function Name:        SoftStor_cfg_setVehicleInfo
 * Description:             ���ó�����Ϣ
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstSrcMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
static void SoftStor_cfg_setVehicleInfo(IN SOFT_QUEUE_MSG_s *pstSrcMsg)
{
    ULONG ulRet = 0;
    UCHAR aucBuff[sizeof(SOFT_QUEUE_MSG_s)] = {0};
    SOFT_QUEUE_MSG_s *pstDstMsg = (SOFT_QUEUE_MSG_s *)aucBuff;
    SOFT_STOR_VEHICLE_INFO_s *pstVehicleInfo = (SOFT_STOR_VEHICLE_INFO_s *)pstSrcMsg->aucData;

    SOFT_QUEUE_RSP_INIT(pstDstMsg, pstSrcMsg);

    /* �����б����һ����Ϣ���бȽϣ���ͬ�������� */
    /* ���ô��󣬻���֮ǰ������ */

    /* �޸������ļ� */
    ulRet = softStor_cfg_setText(SOFT_CFG_XML_INDENTIFY, (CHAR *)pstVehicleInfo->aucVehicleIndentify);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);
        return;
    }

    SOFT_CODE_DEBUG("[%s %d] after set INDENTIFY\n", __FUNCTION__, __LINE__);

    ulRet = softStor_cfg_setText(SOFT_CFG_XML_PLATE_NUM, (CHAR *)pstVehicleInfo->aucVehicleLicensePlate);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);
        return;
    }

    ulRet = softStor_cfg_setText(SOFT_CFG_XML_PLATE_TYPE, (CHAR *)pstVehicleInfo->aucVehicleLicensePlateType);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);
        return;
    }
    
    /* ���� */
    SoftStor_cfg_save();

    pstDstMsg->ulRet = SOFT_STAT_SUCCESS;
    pstDstMsg->usLen = 0;
    SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);

    /* ͬ�����»���ṹ�� */
}

/**********************************************************
 * Function Name:        SoftStor_cfg_setInitialInstallTime
 * Description:             ���ó��ΰ�װʱ��
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstSrcMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
static void SoftStor_cfg_setInitialInstallTime(IN SOFT_QUEUE_MSG_s *pstSrcMsg)
{
    ULONG ulRet = 0;
    UCHAR aucBuff[sizeof(SOFT_QUEUE_MSG_s)] = {0};
    CHAR szStringDate[16] = {0};
    SOFT_QUEUE_MSG_s *pstDstMsg = (SOFT_QUEUE_MSG_s *)aucBuff;

    SOFT_QUEUE_RSP_INIT(pstDstMsg, pstSrcMsg);

    SoftAccess_Bcd2String(pstSrcMsg->aucData, 6, szStringDate);
    ulRet = softStor_cfg_setText(SOFT_CFG_XML_VEHICLE_INSTALL_DATE, szStringDate);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);
        return;
    }
    
    /* ���� */
    SoftStor_cfg_save();

    pstDstMsg->ulRet = SOFT_STAT_SUCCESS;
    pstDstMsg->usLen = 0;
    SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);

    return;
}

/**********************************************************
 * Function Name:        SoftStor_cfg_setDeviceTime
 * Description:             ���ü�¼��ʱ��
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstSrcMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
static void SoftStor_cfg_setDeviceTime(SOFT_QUEUE_MSG_s *pstSrcMsg)
{
    UCHAR aucBuff[sizeof(SOFT_QUEUE_MSG_s)] = {0};
    struct tm _tm;  
    //struct timeval tv;  
    SOFT_QUEUE_MSG_s *pstDstMsg = (SOFT_QUEUE_MSG_s *)aucBuff;

    SOFT_QUEUE_RSP_INIT(pstDstMsg, pstSrcMsg); 

    _tm.tm_year = SOFT_ACCESS_BCD2DIGIT(pstSrcMsg->aucData[0]) + 2000; 
    _tm.tm_mon  = SOFT_ACCESS_BCD2DIGIT(pstSrcMsg->aucData[1]);
    _tm.tm_mday = SOFT_ACCESS_BCD2DIGIT(pstSrcMsg->aucData[2]);
    _tm.tm_hour  = SOFT_ACCESS_BCD2DIGIT(pstSrcMsg->aucData[3]);  
    _tm.tm_min  = SOFT_ACCESS_BCD2DIGIT(pstSrcMsg->aucData[4]);  
    _tm.tm_sec = SOFT_ACCESS_BCD2DIGIT(pstSrcMsg->aucData[5]); 

    SOFT_CODE_DEBUG("[%s %d] Date %04d-%02d-%02d %02d:%02d:%02d\n", __FUNCTION__, __LINE__,
                   _tm.tm_year, _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec);

    #if 0
    _tm.tm_year -= 1900;
    _tm.tm_mon -= 1;
    tv.tv_sec = mktime(&_tm);  
    tv.tv_usec = 0;  
    if(settimeofday (&tv, NULL) < 0)  
    {  
        printf("Set system datatime error!/n");  
        return -1;  
    }  
    #endif

    pstDstMsg->ulRet = SOFT_STAT_SUCCESS;
    pstDstMsg->usLen = 0;
    SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);

    return;
}

/**********************************************************
 * Function Name:        SoftStor_cfg_setDeviceTime
 * Description:             ���ó�ʼ���
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstSrcMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
static void SoftStor_cfg_setInitialMileage(SOFT_QUEUE_MSG_s *pstSrcMsg)
{
    ULONG ulRet = 0;
    UCHAR aucBuff[sizeof(SOFT_QUEUE_MSG_s)] = {0};
    CHAR szTmp[12] = {0};
    SOFT_QUEUE_MSG_s *pstDstMsg = (SOFT_QUEUE_MSG_s *)aucBuff;
    SOFT_STOR_ACCUMULATED_MILEAGE_s *pstAddedMileage = (SOFT_STOR_ACCUMULATED_MILEAGE_s *)pstSrcMsg->aucData;

    SOFT_QUEUE_RSP_INIT(pstDstMsg, pstSrcMsg);

    /* �����б���digital����, ʹ��SOFT_ACCESS_BCD2DIGITת�� */

    SoftAccess_Bcd2String(pstAddedMileage->aucInitialMileage, 4, szTmp);
    ulRet = softStor_cfg_setText(SOFT_CFG_XML_VEHICLE_INIT_MILEAGE, szTmp);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);
        return;
    }
    
    /* ���� */
    SoftStor_cfg_save();

    pstDstMsg->ulRet = SOFT_STAT_SUCCESS;
    pstDstMsg->usLen = 0;
    SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);

    return;
}

/**********************************************************
 * Function Name:        SoftStor_cfg_getStandardVer
 * Description:             ��ȡִ�б�׼
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstSrcMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
static void SoftStor_cfg_getStandardVer(SOFT_QUEUE_MSG_s *pstSrcMsg)
{
    ULONG ulRet = 0;
    CHAR szTmp[4] = {0};
    UCHAR aucBuff[sizeof(SOFT_QUEUE_MSG_s) + sizeof(SOFT_STOR_RECORDER_STANDARD_VER_s)] = {0};
    SOFT_QUEUE_MSG_s *pstDstMsg = (SOFT_QUEUE_MSG_s *)aucBuff;
    SOFT_STOR_RECORDER_STANDARD_VER_s *pstStandVer = 
        (SOFT_STOR_RECORDER_STANDARD_VER_s *)pstDstMsg->aucData;

    SOFT_QUEUE_RSP_INIT(pstDstMsg, pstSrcMsg);

    ulRet = softStor_cfg_getText(SOFT_CFG_XML_VEHICLE_STD_VER, sizeof(szTmp), szTmp);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);
        return;
    }

    pstStandVer->ucYear =((szTmp[0] - '0') << 4) | (szTmp[1] - '0'); 
    SOFT_CODE_DEBUG("[%s %d] Standard Year %d\n", __FUNCTION__, __LINE__, pstStandVer->ucYear);

    ulRet = softStor_cfg_getText(SOFT_CFG_XML_MODIFY_NUM, sizeof(szTmp), szTmp);
    if (ulRet == SOFT_STAT_SUCCESS)
    {
        pstStandVer->ucModifyNum = strtoul(szTmp, 0, 10);
    }

    pstDstMsg->ulRet = SOFT_STAT_SUCCESS;
    pstDstMsg->usLen = sizeof(SOFT_STOR_RECORDER_STANDARD_VER_s);
    SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);
    
    return;
}

/**********************************************************
 * Function Name:        SoftStor_cfg_getDriverLincese
 * Description:             ��ȡ��ǰ��ʻԱ��Ϣ
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstSrcMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
static void SoftStor_cfg_getDriverLincese(IN SOFT_QUEUE_MSG_s *pstSrcMsg)
{
    ULONG ulRet = 0;
    CHAR szTmp[20] = {0};
    UCHAR aucBuff[sizeof(SOFT_QUEUE_MSG_s) + sizeof(SOFT_STOR_DRIVER_LINCESE_s)] = {0};
    SOFT_QUEUE_MSG_s *pstDstMsg = (SOFT_QUEUE_MSG_s *)aucBuff;
    SOFT_STOR_DRIVER_LINCESE_s *pstDriverLincese = 
        (SOFT_STOR_DRIVER_LINCESE_s *)pstDstMsg->aucData;

    SOFT_QUEUE_RSP_INIT(pstDstMsg, pstSrcMsg);

    ulRet = softStor_cfg_getText(SOFT_CFG_XML_CURRENT_DRIVER, sizeof(szTmp), szTmp);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);
        return;
    }

    memcpy(pstDriverLincese->aucDriverLincese, szTmp, 
           sizeof(pstDriverLincese->aucDriverLincese)); 

    pstDstMsg->ulRet = SOFT_STAT_SUCCESS;
    pstDstMsg->usLen = sizeof(SOFT_STOR_RECORDER_STANDARD_VER_s);
    SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);
    
    return;
}

/**********************************************************
 * Function Name:        SoftStor_cfg_getVehicleInfo
 * Description:             ��ȡ������Ϣϵ
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstSrcMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
static void SoftStor_cfg_getVehicleInfo(IN SOFT_QUEUE_MSG_s *pstSrcMsg)
{
    ULONG ulRet = 0;
    CHAR szTmp[20] = {0};
    UCHAR aucBuff[sizeof(SOFT_QUEUE_MSG_s) + sizeof(SOFT_STOR_VEHICLE_INFO_s)] = {0};
    SOFT_QUEUE_MSG_s *pstDstMsg = (SOFT_QUEUE_MSG_s *)aucBuff;
    SOFT_STOR_VEHICLE_INFO_s *pstVehicleInfo = (SOFT_STOR_VEHICLE_INFO_s *)pstDstMsg->aucData;

    SOFT_QUEUE_RSP_INIT(pstDstMsg, pstSrcMsg);
    
    ulRet = softStor_cfg_getText(SOFT_CFG_XML_INDENTIFY, sizeof(szTmp), szTmp);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] %s not find\n", __FUNCTION__, __LINE__, SOFT_CFG_XML_INDENTIFY);
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);
        return;
    }

    memcpy(pstVehicleInfo->aucVehicleIndentify, szTmp, 17);
    SOFT_CODE_DEBUG("[%s %d] Indentify %s\n", __FUNCTION__, __LINE__, 
                    pstVehicleInfo->aucVehicleIndentify);

    ulRet = softStor_cfg_getText(SOFT_CFG_XML_PLATE_NUM, sizeof(szTmp), szTmp);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] %s not find\n", __FUNCTION__, __LINE__, SOFT_CFG_XML_PLATE_NUM);
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);
        return;
    }

    memcpy(pstVehicleInfo->aucVehicleLicensePlate, szTmp, 11);
    SOFT_CODE_DEBUG("[%s %d] Plate %s\n", __FUNCTION__, __LINE__, 
                    pstVehicleInfo->aucVehicleLicensePlate);

    ulRet = softStor_cfg_getText(SOFT_CFG_XML_PLATE_TYPE, sizeof(szTmp), szTmp);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] %s not find\n", __FUNCTION__, __LINE__, SOFT_CFG_XML_PLATE_NUM);
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);
        return;
    }

    memcpy(pstVehicleInfo->aucVehicleLicensePlateType, szTmp, 11);
    SOFT_CODE_DEBUG("[%s %d] PlateType %s\n", __FUNCTION__, __LINE__, 
                    pstVehicleInfo->aucVehicleLicensePlateType);
    
    pstDstMsg->usLen = sizeof(*pstVehicleInfo);
    SOFT_CODE_DEBUG("[%s %d] Len %d\n", __FUNCTION__, __LINE__, pstDstMsg->usLen);

    SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);

    return;
}

/**********************************************************
 * Function Name:        SoftStor_cfg_getRecoderUniqueNum
 * Description:             ��ȡ��¼��Ψһ�Ա��
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstSrcMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
static void SoftStor_cfg_getRecoderUniqueNum(IN SOFT_QUEUE_MSG_s *pstSrcMsg)
{
    ULONG ulRet = 0;
    CHAR szTmp[20] = {0};
    UCHAR aucBuff[sizeof(SOFT_QUEUE_MSG_s) + sizeof(SOFT_STOR_RECORDER_UNIQUE_INFO_s)] = {0};
    SOFT_QUEUE_MSG_s *pstDstMsg = (SOFT_QUEUE_MSG_s *)aucBuff;
    SOFT_STOR_RECORDER_UNIQUE_INFO_s *pstRecordUnique = 
        (SOFT_STOR_RECORDER_UNIQUE_INFO_s *)pstDstMsg->aucData;

    SOFT_QUEUE_RSP_INIT(pstDstMsg, pstSrcMsg);
    
    ulRet = softStor_cfg_getText(SOFT_CFG_XML_3C_IDENTIFY, sizeof(szTmp), szTmp);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] %s not find\n", __FUNCTION__, __LINE__, SOFT_CFG_XML_INDENTIFY);
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);
        return;
    }

    memcpy(pstRecordUnique->auc3CIdentify, szTmp, 7);
    
    ulRet = softStor_cfg_getText(SOFT_CFG_XML_IDENTIFY_PDT_MODLE, sizeof(szTmp), szTmp);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] %s not find\n", __FUNCTION__, __LINE__, SOFT_CFG_XML_PLATE_NUM);
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);
        return;
    }

    memcpy(pstRecordUnique->aucProductType, szTmp, 16);
    
    ulRet = softStor_cfg_getText(SOFT_CFG_XML_PDT_BORNDATE, sizeof(szTmp), szTmp);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] %s not find\n", __FUNCTION__, __LINE__, SOFT_CFG_XML_PLATE_NUM);
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);
        return;
    }

    SoftAccess_String2Bcd(szTmp, pstRecordUnique->aucProductDate, NULL);

    ulRet = softStor_cfg_getText(SOFT_CFG_XML_SERIAL_NUM, sizeof(szTmp), szTmp);
    if (ulRet != SOFT_STAT_SUCCESS)
    {
        printf("[%s %d] %s not find\n", __FUNCTION__, __LINE__, SOFT_CFG_XML_PLATE_NUM);
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);
        return;
    }

    SoftAccess_String2Bcd(szTmp, pstRecordUnique->aucProdSerialNumber, NULL);
    
    pstDstMsg->usLen = sizeof(*pstRecordUnique);

    SoftQueue_snd(pstDstMsg, SOFT_MSG_BLOCK);

    return;
}

/**********************************************************
 * Function Name:        SoftStor_cfg_main_process
 * Description:            ������ģ����������
 * Parameters[in]:        IN SOFT_QUEUE_MSG_s *pstSrcMsg
 * Parameters[out]:      void *pArg -> SOFT_QUEUE_MSG_s
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
void SoftStor_cfg_main_process(void *pArg)
{
    SOFT_QUEUE_MSG_s *pstMsg = (SOFT_QUEUE_MSG_s *)pArg;
    
    switch (pstMsg->usCmdID)
    {
        case GB_UART_CLLC_STANDARD_VER:
            SoftStor_cfg_getStandardVer(pstMsg);
            break;

        case GB_UART_CLLC_CURRENT_DRIVER_LINCENSE:
            SoftStor_cfg_getDriverLincese(pstMsg);
            break;

        case GB_UART_CLLC_VEHICLE_INFO:
            SoftStor_cfg_getVehicleInfo(pstMsg);
            break;
            
        case GB_UART_CLLC_RECORDER_UNIQUE_NUMBER:
            SoftStor_cfg_getRecoderUniqueNum(pstMsg);
            break;
            
        case GB_UART_SET_VEHICLE_INFO:
            /* ������Ϣ */
            SOFT_CODE_DEBUG("[%s %d] setVehicleInfo before\n", __FUNCTION__, __LINE__);
            SoftStor_cfg_setVehicleInfo(pstMsg);
            SOFT_CODE_DEBUG("[%s %d] setVehicleInfo after\n", __FUNCTION__, __LINE__);
            break;
            
        case GB_UART_SET_INITIAL_INSTALL_TIME:
            /* ��ʼ��װʱ�� */
            SoftStor_cfg_setInitialInstallTime(pstMsg);
            break;
            
        case GB_UART_SET_REAL_TIME:
            /* �����豸ʱ�� */
            SoftStor_cfg_setDeviceTime(pstMsg);
            break;
            
        case GB_UART_SET_INITIAL_MILEAGE:
            /* ��ʼ��� */
            SoftStor_cfg_setInitialMileage(pstMsg);
            break;
            
        default:
            break;
    }

    return;
}

