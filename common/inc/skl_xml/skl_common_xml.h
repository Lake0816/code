#ifndef __SKY_COMMON_XML_H__
#define __SKY_COMMON_XML_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mxml.h"

typedef mxml_node_t SKL_XML_NODE_s;

extern ULONG skl_xml_loadXml(IN CHAR *pcFilePath, INOUT SKL_XML_NODE_s **ppstNodeRoot);
extern ULONG skl_xml_saveXml(IN CHAR *pcFilePath, IN SKL_XML_NODE_s *pstNodeRoot);
extern void skl_xml_destoryXml(INOUT SKL_XML_NODE_s **ppstNodeRoot);
extern ULONG skl_xml_getNodeByPath(IN char *pcPath, IN SKL_XML_NODE_s *pstRoot,
                                             OUT SKL_XML_NODE_s **ppstNode);
extern void skl_xml_getNodeText(IN SKL_XML_NODE_s * pstNode, IN UINT8 ucLen, 
                                         OUT CHAR *pcTxt);
extern ULONG skl_xml_setNodeText(IN SKL_XML_NODE_s * pstNode, IN CHAR *pcTxt);
extern ULONG skl_xml_loadString(IN CHAR *pcString, INOUT SKL_XML_NODE_s **ppstNodeRoot);

#endif
