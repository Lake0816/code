#ifndef __SOFT_STOR_CFG_H__
#define __SOFT_STOR_CFG_H__

#include "soft_public.h"
#include "soft_xml/soft_common_xml.h"

typedef struct _SOFT_STOR_CFG_s_
{
    SOFT_XML_NODE_s *pstCfgRoot;
}SOFT_STOR_CFG_s;

extern ULONG SoftStor_cfg_init(void);
extern ULONG SoftStor_cfg_save(void);
extern void SoftStor_cfg_destory(void);
extern void SoftStor_cfg_main_process(void *pArg);

#endif  //__SOFT_STOR_CFG_H__

