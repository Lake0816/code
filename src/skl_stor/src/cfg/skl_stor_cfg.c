#include "cfg/skl_stor_cfg.h"
#include "skl_stor_common.h"
#include "skl_access_common.h"

#include <unistd.h>
#include <time.h>
#include "skl_common_queue.h"

static SKL_STOR_CFG_s gstStorCfg;

/**********************************************************
 * Function Name:        SklStor_cfg_init
 * Description:             配置子模块初始化
 * Parameters[in]:        void
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
ULONG SklStor_cfg_init(void)
{
    ULONG ulRet = SKL_STAT_SUCCESS;

    if (access(SKL_DEV_CFG_FILE, F_OK) == 0)
    {
        ulRet = skl_xml_loadXml(SKL_DEV_CFG_FILE, &gstStorCfg.pstCfgRoot);
        if (ulRet != SKL_STAT_SUCCESS)
        {
            printf("[%s %d] loadXml fail 0x%lx\n", __FUNCTION__, __LINE__, ulRet);
            return ulRet;
        }
    }
    else
    {
        ulRet = skl_xml_loadString(SKL_DEV_CFG_DEFAUT, &gstStorCfg.pstCfgRoot);
        if (ulRet != SKL_STAT_SUCCESS)
        {
            printf("[%s %d] loadXml fail 0x%lx\n", __FUNCTION__, __LINE__, ulRet);
            return ulRet;
        }

        SklStor_cfg_save();
    }

    return ulRet;
}

/**********************************************************
 * Function Name:        sklStor_cfg_getText
 * Description:             获取节点信息
 * Parameters[in]:        IN CHAR *pcPath
                                 IN INT iLen
                                 INOUT CHAR *pcTxt
 * Parameters[out]:      INOUT CHAR *pcTxt
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
ULONG sklStor_cfg_getText(IN CHAR *pcPath, IN INT iLen, INOUT CHAR *pcTxt)
{
    ULONG ulRet = SKL_STAT_SUCCESS;
    SKL_XML_NODE_s *pstNode = NULL;
    
    ulRet = skl_xml_getNodeByPath(pcPath, gstStorCfg.pstCfgRoot, &pstNode);
    if (ulRet != SKL_STAT_SUCCESS)
    {
        printf("[%s %d] getNodeByPath fail 0x%lx\n", __FUNCTION__, __LINE__, ulRet);
        return ulRet;
    }

    skl_xml_getNodeText(pstNode, iLen, pcTxt);

    return ulRet;
}

/**********************************************************
 * Function Name:        sklStor_cfg_setText
 * Description:             配置节点信息
 * Parameters[in]:        IN CHAR *pcPath
                                 IN CHAR *pcTxt
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
ULONG sklStor_cfg_setText(IN CHAR *pcPath, IN CHAR *pcTxt)
{
    ULONG ulRet = SKL_STAT_SUCCESS;
    SKL_XML_NODE_s *pstNode = NULL;
    
    ulRet = skl_xml_getNodeByPath(pcPath, gstStorCfg.pstCfgRoot, &pstNode);
    if (ulRet != SKL_STAT_SUCCESS)
    {
        printf("[%s %d] getNodeByPath fail 0x%lx\n", __FUNCTION__, __LINE__, ulRet);
        return ulRet;
    }

    ulRet = skl_xml_setNodeText(pstNode, pcTxt);
    if (ulRet != SKL_STAT_SUCCESS)
    {
        printf("[%s %d] setNodeText fail 0x%lx\n", __FUNCTION__, __LINE__, ulRet);
        return ulRet;
    }

    return ulRet;
}

/**********************************************************
 * Function Name:        SklStor_cfg_save
 * Description:             配置文件保存
 * Parameters[in]:        IN CHAR *pcPath
                                 IN CHAR *pcTxt
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
ULONG SklStor_cfg_save(void)
{
    ULONG ulRet = SKL_STAT_SUCCESS;

    ulRet = skl_xml_saveXml(SKL_DEV_CFG_FILE, gstStorCfg.pstCfgRoot);
    if (ulRet != SKL_STAT_SUCCESS)
    {
        printf("[%s %d] saveXml fail 0x%lx\n", __FUNCTION__, __LINE__, ulRet);
        return ulRet;
    }

    return ulRet;
}

/**********************************************************
 * Function Name:        SklStor_cfg_destory
 * Description:             存储模块销毁
 * Parameters[in]:        IN CHAR *pcPath
                                 IN CHAR *pcTxt
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
void SklStor_cfg_destory(void)
{
    skl_xml_destoryXml(&gstStorCfg.pstCfgRoot);

    return;
}

/**********************************************************
 * Function Name:        SklStor_cfg_setVehicleInfo
 * Description:             设置车辆信息
 * Parameters[in]:        IN SKL_QUEUE_MSG_s *pstSrcMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
static void SklStor_cfg_setVehicleInfo(IN SKL_QUEUE_MSG_s *pstSrcMsg)
{
    ULONG ulRet = 0;
    UCHAR aucBuff[sizeof(SKL_QUEUE_MSG_s)] = {0};
    SKL_QUEUE_MSG_s *pstDstMsg = (SKL_QUEUE_MSG_s *)aucBuff;
    SKL_STOR_VEHICLE_INFO_s *pstVehicleInfo = (SKL_STOR_VEHICLE_INFO_s *)pstSrcMsg->aucData;

    SKL_QUEUE_RSP_INIT(pstDstMsg, pstSrcMsg);

    /* 缓存中保存的一份信息进行比较，不同进行设置 */
    /* 配置错误，回退之前的配置 */

    /* 修改配置文件 */
    ulRet = sklStor_cfg_setText(SKL_CFG_XML_INDENTIFY, (CHAR *)pstVehicleInfo->aucVehicleIndentify);
    if (ulRet != SKL_STAT_SUCCESS)
    {
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);
        return;
    }

    SKL_CODE_DEBUG("[%s %d] after set INDENTIFY\n", __FUNCTION__, __LINE__);

    ulRet = sklStor_cfg_setText(SKL_CFG_XML_PLATE_NUM, (CHAR *)pstVehicleInfo->aucVehicleLicensePlate);
    if (ulRet != SKL_STAT_SUCCESS)
    {
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);
        return;
    }

    ulRet = sklStor_cfg_setText(SKL_CFG_XML_PLATE_TYPE, (CHAR *)pstVehicleInfo->aucVehicleLicensePlateType);
    if (ulRet != SKL_STAT_SUCCESS)
    {
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);
        return;
    }
    
    /* 保存 */
    SklStor_cfg_save();

    pstDstMsg->ulRet = SKL_STAT_SUCCESS;
    pstDstMsg->usLen = 0;
    SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);

    /* 同步更新缓存结构体 */
}

/**********************************************************
 * Function Name:        SklStor_cfg_setInitialInstallTime
 * Description:             设置初次安装时间
 * Parameters[in]:        IN SKL_QUEUE_MSG_s *pstSrcMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
static void SklStor_cfg_setInitialInstallTime(IN SKL_QUEUE_MSG_s *pstSrcMsg)
{
    ULONG ulRet = 0;
    UCHAR aucBuff[sizeof(SKL_QUEUE_MSG_s)] = {0};
    CHAR szStringDate[16] = {0};
    SKL_QUEUE_MSG_s *pstDstMsg = (SKL_QUEUE_MSG_s *)aucBuff;

    SKL_QUEUE_RSP_INIT(pstDstMsg, pstSrcMsg);

    SklAccess_Bcd2String(pstSrcMsg->aucData, 6, szStringDate);
    ulRet = sklStor_cfg_setText(SKL_CFG_XML_VEHICLE_INSTALL_DATE, szStringDate);
    if (ulRet != SKL_STAT_SUCCESS)
    {
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);
        return;
    }
    
    /* 保存 */
    SklStor_cfg_save();

    pstDstMsg->ulRet = SKL_STAT_SUCCESS;
    pstDstMsg->usLen = 0;
    SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);

    return;
}

/**********************************************************
 * Function Name:        SklStor_cfg_setDeviceTime
 * Description:             设置记录仪时间
 * Parameters[in]:        IN SKL_QUEUE_MSG_s *pstSrcMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
static void SklStor_cfg_setDeviceTime(SKL_QUEUE_MSG_s *pstSrcMsg)
{
    UCHAR aucBuff[sizeof(SKL_QUEUE_MSG_s)] = {0};
    struct tm _tm;  
    //struct timeval tv;  
    SKL_QUEUE_MSG_s *pstDstMsg = (SKL_QUEUE_MSG_s *)aucBuff;

    SKL_QUEUE_RSP_INIT(pstDstMsg, pstSrcMsg); 

    _tm.tm_year = SKL_ACCESS_BCD2DIGIT(pstSrcMsg->aucData[0]) + 2000; 
    _tm.tm_mon  = SKL_ACCESS_BCD2DIGIT(pstSrcMsg->aucData[1]);
    _tm.tm_mday = SKL_ACCESS_BCD2DIGIT(pstSrcMsg->aucData[2]);
    _tm.tm_hour  = SKL_ACCESS_BCD2DIGIT(pstSrcMsg->aucData[3]);  
    _tm.tm_min  = SKL_ACCESS_BCD2DIGIT(pstSrcMsg->aucData[4]);  
    _tm.tm_sec = SKL_ACCESS_BCD2DIGIT(pstSrcMsg->aucData[5]); 

    SKL_CODE_DEBUG("[%s %d] Date %04d-%02d-%02d %02d:%02d:%02d\n", __FUNCTION__, __LINE__,
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

    pstDstMsg->ulRet = SKL_STAT_SUCCESS;
    pstDstMsg->usLen = 0;
    SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);

    return;
}

/**********************************************************
 * Function Name:        SklStor_cfg_setDeviceTime
 * Description:             设置初始里程
 * Parameters[in]:        IN SKL_QUEUE_MSG_s *pstSrcMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
static void SklStor_cfg_setInitialMileage(SKL_QUEUE_MSG_s *pstSrcMsg)
{
    ULONG ulRet = 0;
    UCHAR aucBuff[sizeof(SKL_QUEUE_MSG_s)] = {0};
    CHAR szTmp[12] = {0};
    SKL_QUEUE_MSG_s *pstDstMsg = (SKL_QUEUE_MSG_s *)aucBuff;
    SKL_STOR_ACCUMULATED_MILEAGE_s *pstAddedMileage = (SKL_STOR_ACCUMULATED_MILEAGE_s *)pstSrcMsg->aucData;

    SKL_QUEUE_RSP_INIT(pstDstMsg, pstSrcMsg);

    /* 缓存中保存digital数据, 使用SKL_ACCESS_BCD2DIGIT转换 */

    SklAccess_Bcd2String(pstAddedMileage->aucInitialMileage, 4, szTmp);
    ulRet = sklStor_cfg_setText(SKL_CFG_XML_VEHICLE_INIT_MILEAGE, szTmp);
    if (ulRet != SKL_STAT_SUCCESS)
    {
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);
        return;
    }
    
    /* 保存 */
    SklStor_cfg_save();

    pstDstMsg->ulRet = SKL_STAT_SUCCESS;
    pstDstMsg->usLen = 0;
    SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);

    return;
}

/**********************************************************
 * Function Name:        SklStor_cfg_getStandardVer
 * Description:             获取执行标准
 * Parameters[in]:        IN SKL_QUEUE_MSG_s *pstSrcMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
static void SklStor_cfg_getStandardVer(SKL_QUEUE_MSG_s *pstSrcMsg)
{
    ULONG ulRet = 0;
    CHAR szTmp[4] = {0};
    UCHAR aucBuff[sizeof(SKL_QUEUE_MSG_s) + sizeof(SKL_STOR_RECORDER_STANDARD_VER_s)] = {0};
    SKL_QUEUE_MSG_s *pstDstMsg = (SKL_QUEUE_MSG_s *)aucBuff;
    SKL_STOR_RECORDER_STANDARD_VER_s *pstStandVer = 
        (SKL_STOR_RECORDER_STANDARD_VER_s *)pstDstMsg->aucData;

    SKL_QUEUE_RSP_INIT(pstDstMsg, pstSrcMsg);

    ulRet = sklStor_cfg_getText(SKL_CFG_XML_VEHICLE_STD_VER, sizeof(szTmp), szTmp);
    if (ulRet != SKL_STAT_SUCCESS)
    {
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);
        return;
    }

    pstStandVer->ucYear =((szTmp[0] - '0') << 4) | (szTmp[1] - '0'); 
    SKL_CODE_DEBUG("[%s %d] Standard Year %d\n", __FUNCTION__, __LINE__, pstStandVer->ucYear);

    ulRet = sklStor_cfg_getText(SKL_CFG_XML_MODIFY_NUM, sizeof(szTmp), szTmp);
    if (ulRet == SKL_STAT_SUCCESS)
    {
        pstStandVer->ucModifyNum = strtoul(szTmp, 0, 10);
    }

    pstDstMsg->ulRet = SKL_STAT_SUCCESS;
    pstDstMsg->usLen = sizeof(SKL_STOR_RECORDER_STANDARD_VER_s);
    SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);
    
    return;
}

/**********************************************************
 * Function Name:        SklStor_cfg_getDriverLincese
 * Description:             获取当前驾驶员信息
 * Parameters[in]:        IN SKL_QUEUE_MSG_s *pstSrcMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
static void SklStor_cfg_getDriverLincese(IN SKL_QUEUE_MSG_s *pstSrcMsg)
{
    ULONG ulRet = 0;
    CHAR szTmp[20] = {0};
    UCHAR aucBuff[sizeof(SKL_QUEUE_MSG_s) + sizeof(SKL_STOR_DRIVER_LINCESE_s)] = {0};
    SKL_QUEUE_MSG_s *pstDstMsg = (SKL_QUEUE_MSG_s *)aucBuff;
    SKL_STOR_DRIVER_LINCESE_s *pstDriverLincese = 
        (SKL_STOR_DRIVER_LINCESE_s *)pstDstMsg->aucData;

    SKL_QUEUE_RSP_INIT(pstDstMsg, pstSrcMsg);

    ulRet = sklStor_cfg_getText(SKL_CFG_XML_CURRENT_DRIVER, sizeof(szTmp), szTmp);
    if (ulRet != SKL_STAT_SUCCESS)
    {
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);
        return;
    }

    memcpy(pstDriverLincese->aucDriverLincese, szTmp, 
           sizeof(pstDriverLincese->aucDriverLincese)); 

    pstDstMsg->ulRet = SKL_STAT_SUCCESS;
    pstDstMsg->usLen = sizeof(SKL_STOR_RECORDER_STANDARD_VER_s);
    SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);
    
    return;
}

/**********************************************************
 * Function Name:        SklStor_cfg_getVehicleInfo
 * Description:             获取车辆信息系
 * Parameters[in]:        IN SKL_QUEUE_MSG_s *pstSrcMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
static void SklStor_cfg_getVehicleInfo(IN SKL_QUEUE_MSG_s *pstSrcMsg)
{
    ULONG ulRet = 0;
    CHAR szTmp[20] = {0};
    UCHAR aucBuff[sizeof(SKL_QUEUE_MSG_s) + sizeof(SKL_STOR_VEHICLE_INFO_s)] = {0};
    SKL_QUEUE_MSG_s *pstDstMsg = (SKL_QUEUE_MSG_s *)aucBuff;
    SKL_STOR_VEHICLE_INFO_s *pstVehicleInfo = (SKL_STOR_VEHICLE_INFO_s *)pstDstMsg->aucData;

    SKL_QUEUE_RSP_INIT(pstDstMsg, pstSrcMsg);
    
    ulRet = sklStor_cfg_getText(SKL_CFG_XML_INDENTIFY, sizeof(szTmp), szTmp);
    if (ulRet != SKL_STAT_SUCCESS)
    {
        printf("[%s %d] %s not find\n", __FUNCTION__, __LINE__, SKL_CFG_XML_INDENTIFY);
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);
        return;
    }

    memcpy(pstVehicleInfo->aucVehicleIndentify, szTmp, 17);
    SKL_CODE_DEBUG("[%s %d] Indentify %s\n", __FUNCTION__, __LINE__, 
                    pstVehicleInfo->aucVehicleIndentify);

    ulRet = sklStor_cfg_getText(SKL_CFG_XML_PLATE_NUM, sizeof(szTmp), szTmp);
    if (ulRet != SKL_STAT_SUCCESS)
    {
        printf("[%s %d] %s not find\n", __FUNCTION__, __LINE__, SKL_CFG_XML_PLATE_NUM);
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);
        return;
    }

    memcpy(pstVehicleInfo->aucVehicleLicensePlate, szTmp, 11);
    SKL_CODE_DEBUG("[%s %d] Plate %s\n", __FUNCTION__, __LINE__, 
                    pstVehicleInfo->aucVehicleLicensePlate);

    ulRet = sklStor_cfg_getText(SKL_CFG_XML_PLATE_TYPE, sizeof(szTmp), szTmp);
    if (ulRet != SKL_STAT_SUCCESS)
    {
        printf("[%s %d] %s not find\n", __FUNCTION__, __LINE__, SKL_CFG_XML_PLATE_NUM);
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);
        return;
    }

    memcpy(pstVehicleInfo->aucVehicleLicensePlateType, szTmp, 11);
    SKL_CODE_DEBUG("[%s %d] PlateType %s\n", __FUNCTION__, __LINE__, 
                    pstVehicleInfo->aucVehicleLicensePlateType);
    
    pstDstMsg->usLen = sizeof(*pstVehicleInfo);
    SKL_CODE_DEBUG("[%s %d] Len %d\n", __FUNCTION__, __LINE__, pstDstMsg->usLen);

    SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);

    return;
}

/**********************************************************
 * Function Name:        SklStor_cfg_getRecoderUniqueNum
 * Description:             获取记录仪唯一性编号
 * Parameters[in]:        IN SKL_QUEUE_MSG_s *pstSrcMsg
 * Parameters[out]:      void
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
static void SklStor_cfg_getRecoderUniqueNum(IN SKL_QUEUE_MSG_s *pstSrcMsg)
{
    ULONG ulRet = 0;
    CHAR szTmp[20] = {0};
    UCHAR aucBuff[sizeof(SKL_QUEUE_MSG_s) + sizeof(SKL_STOR_RECORDER_UNIQUE_INFO_s)] = {0};
    SKL_QUEUE_MSG_s *pstDstMsg = (SKL_QUEUE_MSG_s *)aucBuff;
    SKL_STOR_RECORDER_UNIQUE_INFO_s *pstRecordUnique = 
        (SKL_STOR_RECORDER_UNIQUE_INFO_s *)pstDstMsg->aucData;

    SKL_QUEUE_RSP_INIT(pstDstMsg, pstSrcMsg);
    
    ulRet = sklStor_cfg_getText(SKL_CFG_XML_3C_IDENTIFY, sizeof(szTmp), szTmp);
    if (ulRet != SKL_STAT_SUCCESS)
    {
        printf("[%s %d] %s not find\n", __FUNCTION__, __LINE__, SKL_CFG_XML_INDENTIFY);
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);
        return;
    }

    memcpy(pstRecordUnique->auc3CIdentify, szTmp, 7);
    
    ulRet = sklStor_cfg_getText(SKL_CFG_XML_IDENTIFY_PDT_MODLE, sizeof(szTmp), szTmp);
    if (ulRet != SKL_STAT_SUCCESS)
    {
        printf("[%s %d] %s not find\n", __FUNCTION__, __LINE__, SKL_CFG_XML_PLATE_NUM);
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);
        return;
    }

    memcpy(pstRecordUnique->aucProductType, szTmp, 16);
    
    ulRet = sklStor_cfg_getText(SKL_CFG_XML_PDT_BORNDATE, sizeof(szTmp), szTmp);
    if (ulRet != SKL_STAT_SUCCESS)
    {
        printf("[%s %d] %s not find\n", __FUNCTION__, __LINE__, SKL_CFG_XML_PLATE_NUM);
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);
        return;
    }

    SklAccess_String2Bcd(szTmp, pstRecordUnique->aucProductDate, NULL);

    ulRet = sklStor_cfg_getText(SKL_CFG_XML_SERIAL_NUM, sizeof(szTmp), szTmp);
    if (ulRet != SKL_STAT_SUCCESS)
    {
        printf("[%s %d] %s not find\n", __FUNCTION__, __LINE__, SKL_CFG_XML_PLATE_NUM);
        pstDstMsg->ulRet = ulRet;
        pstDstMsg->usLen = 0;
        SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);
        return;
    }

    SklAccess_String2Bcd(szTmp, pstRecordUnique->aucProdSerialNumber, NULL);
    
    pstDstMsg->usLen = sizeof(*pstRecordUnique);

    SklQueue_snd(pstDstMsg, SKL_MSG_BLOCK);

    return;
}

/**********************************************************
 * Function Name:        SklStor_cfg_main_process
 * Description:            配置子模块主处理函数
 * Parameters[in]:        IN SKL_QUEUE_MSG_s *pstSrcMsg
 * Parameters[out]:      void *pArg -> SKL_QUEUE_MSG_s
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
void SklStor_cfg_main_process(void *pArg)
{
    SKL_QUEUE_MSG_s *pstMsg = (SKL_QUEUE_MSG_s *)pArg;
    
    switch (pstMsg->usCmdID)
    {
        case GB_UART_CLLC_STANDARD_VER:
            SklStor_cfg_getStandardVer(pstMsg);
            break;

        case GB_UART_CLLC_CURRENT_DRIVER_LINCENSE:
            SklStor_cfg_getDriverLincese(pstMsg);
            break;

        case GB_UART_CLLC_VEHICLE_INFO:
            SklStor_cfg_getVehicleInfo(pstMsg);
            break;
            
        case GB_UART_CLLC_RECORDER_UNIQUE_NUMBER:
            SklStor_cfg_getRecoderUniqueNum(pstMsg);
            break;
            
        case GB_UART_SET_VEHICLE_INFO:
            /* 车辆信息 */
            SKL_CODE_DEBUG("[%s %d] setVehicleInfo before\n", __FUNCTION__, __LINE__);
            SklStor_cfg_setVehicleInfo(pstMsg);
            SKL_CODE_DEBUG("[%s %d] setVehicleInfo after\n", __FUNCTION__, __LINE__);
            break;
            
        case GB_UART_SET_INITIAL_INSTALL_TIME:
            /* 初始安装时间 */
            SklStor_cfg_setInitialInstallTime(pstMsg);
            break;
            
        case GB_UART_SET_REAL_TIME:
            /* 设置设备时间 */
            SklStor_cfg_setDeviceTime(pstMsg);
            break;
            
        case GB_UART_SET_INITIAL_MILEAGE:
            /* 初始里程 */
            SklStor_cfg_setInitialMileage(pstMsg);
            break;
            
        default:
            break;
    }

    return;
}

