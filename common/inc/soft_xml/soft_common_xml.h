#ifndef __SKY_COMMON_XML_H__
#define __SKY_COMMON_XML_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mxml.h"

typedef mxml_node_t SOFT_XML_NODE_s;

extern ULONG soft_xml_loadXml(IN CHAR *pcFilePath, INOUT SOFT_XML_NODE_s **ppstNodeRoot);
extern ULONG soft_xml_saveXml(IN CHAR *pcFilePath, IN SOFT_XML_NODE_s *pstNodeRoot);
extern void soft_xml_destoryXml(INOUT SOFT_XML_NODE_s **ppstNodeRoot);
extern ULONG soft_xml_getNodeByPath(IN char *pcPath, IN SOFT_XML_NODE_s *pstRoot,
                                             OUT SOFT_XML_NODE_s **ppstNode);
extern void soft_xml_getNodeText(IN SOFT_XML_NODE_s * pstNode, IN UINT8 ucLen, 
                                         OUT CHAR *pcTxt);
extern ULONG soft_xml_setNodeText(IN SOFT_XML_NODE_s * pstNode, IN CHAR *pcTxt);
extern ULONG soft_xml_loadString(IN CHAR *pcString, INOUT SOFT_XML_NODE_s **ppstNodeRoot);

#endif
