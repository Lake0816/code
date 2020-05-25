#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "soft_public.h"
#include "soft_xml/soft_common_xml.h"

/**********************************************************
 * Function Name:        soft_xml_loadXml
 * Description:             加载xml文件
 * Parameters[in]:        IN CHAR *pcFilePath
 * Parameters[out]:      INOUT SOFT_XML_NODE_s **ppstNodeRoot                        
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
ULONG soft_xml_loadXml(IN CHAR *pcFilePath, INOUT SOFT_XML_NODE_s **ppstNodeRoot)
{
    FILE *pstFile = NULL;

    pstFile = fopen(pcFilePath, "rb");
    if (pstFile == NULL)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        return SOFT_STAT_FAIL_FILE_OPEN;
    }

    *ppstNodeRoot = mxmlLoadFile(NULL, pstFile, MXML_OPAQUE_CALLBACK);
    fclose(pstFile);
    if (*ppstNodeRoot == NULL)
    {
        return SOFT_STAT_FAIL_XML_LOAD;
    }

    return SOFT_STAT_SUCCESS;
}


/**********************************************************
 * Function Name:        soft_xml_loadString
 * Description:             加载字符串
 * Parameters[in]:        IN CHAR *pcFilePath
 * Parameters[out]:      INOUT SOFT_XML_NODE_s **ppstNodeRoot                        
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
ULONG soft_xml_loadString(IN CHAR *pcString, INOUT SOFT_XML_NODE_s **ppstNodeRoot)
{
    *ppstNodeRoot = mxmlLoadString(NULL, pcString, MXML_OPAQUE_CALLBACK);
    if (*ppstNodeRoot == NULL)
    {
        return SOFT_STAT_FAIL_XML_LOAD;
    }

    return SOFT_STAT_SUCCESS;
}

/**********************************************************
 * Function Name:        soft_xml_saveXml
 * Description:             节点信息保存到文件中
 * Parameters[in]:        IN CHAR *pcFilePath
                                 IN SOFT_XML_NODE_s *pstNodeRoot
 * Parameters[out]:      void                        
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.15
***********************************************************/
ULONG soft_xml_saveXml(IN CHAR *pcFilePath, IN SOFT_XML_NODE_s *pstNodeRoot)
{
    int iRet = 0;
    FILE *pstFile = NULL;

    pstFile = fopen(pcFilePath, "w+");
    if (pstFile == NULL)
    {
        printf("[%s %d] err %d [%s]\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        return SOFT_STAT_FAIL_FILE_OPEN;
    }

    iRet = mxmlSaveFile(pstNodeRoot, pstFile, MXML_NO_CALLBACK);
    fclose(pstFile);
    if (iRet != 0)
    {
        return SOFT_STAT_FAIL_XML_SAVE;
    }

    return SOFT_STAT_SUCCESS;
}

/**********************************************************
 * Function Name:        soft_xml_getNodeByPath
 * Description:             根据路径获取节点
 * Parameters[in]:        IN char *pcPath
                                 IN SOFT_XML_NODE_s *pstRoot
 * Parameters[out]:      OUT SOFT_XML_NODE_s **ppstNode                      
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.15
***********************************************************/
ULONG soft_xml_getNodeByPath(IN char *pcPath, 
                                      IN SOFT_XML_NODE_s *pstRoot,
                                      OUT SOFT_XML_NODE_s **ppstNode)
{
    *ppstNode = mxmlFindPath(pstRoot, pcPath);
    if (*ppstNode == NULL)
    {
        return SOFT_STAT_FAIL_XML_NONEXIT;
    }

    return SOFT_STAT_SUCCESS;
}

/**********************************************************
 * Function Name:        SoftQueue_init
 * Description:             读取节点
 * Parameters[in]:        IN SOFT_XML_NODE_s * pstNode
                                 IN UINT8 ucLen
 * Parameters[out]:      OUT CHAR *pcTxt                        
 * ReturnValue:            void
 * Author:                   liuke
 * Date:                      2020.05.15
***********************************************************/
void soft_xml_getNodeText(IN SOFT_XML_NODE_s * pstNode, IN UINT8 ucLen, OUT CHAR *pcTxt)
{
    if (mxmlGetOpaque(pstNode) != NULL)
    {
        memcpy(pcTxt, mxmlGetOpaque(pstNode), ucLen);
    }
    
    return;
}

/**********************************************************
 * Function Name:        soft_xml_setNodeText
 * Description:             配置节点
 * Parameters[in]:        IN SOFT_XML_NODE_s * pstNode
                                 IN CHAR *pcTxt
 * Parameters[out]:      void                        
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
ULONG soft_xml_setNodeText(IN SOFT_XML_NODE_s * pstNode, IN CHAR *pcTxt)
{
    if (mxmlGetOpaque(pstNode) != NULL)
    {
        if (mxmlSetOpaque(pstNode, pcTxt) != 0)
        {
            return SOFT_STAT_FAIL_XML_SET;
        }
    }
    else
    {
        if (mxmlNewOpaque(pstNode, pcTxt) == NULL)
        {
            return SOFT_STAT_FAIL_XML_SET;
        }
    }

    return SOFT_STAT_SUCCESS;
}

/**********************************************************
 * Function Name:        soft_xml_destoryXml
 * Description:             销毁xml结构体
 * Parameters[in]:        INOUT SOFT_XML_NODE_s **ppstNodeRoot
 * Parameters[out]:      INOUT SOFT_XML_NODE_s **ppstNodeRoot                        
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.20
***********************************************************/
void soft_xml_destoryXml(INOUT SOFT_XML_NODE_s **ppstNodeRoot)
{
    mxmlDelete(*ppstNodeRoot);
    *ppstNodeRoot = NULL;

    return;
}

