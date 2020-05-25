#ifndef __SKL_STOR_CFG_H__
#define __SKL_STOR_CFG_H__

#include "skl_public.h"
#include "skl_xml/skl_common_xml.h"

typedef struct _SKL_STOR_CFG_s_
{
    SKL_XML_NODE_s *pstCfgRoot;
}SKL_STOR_CFG_s;

extern ULONG SklStor_cfg_init(void);
extern ULONG SklStor_cfg_save(void);
extern void SklStor_cfg_destory(void);
extern void SklStor_cfg_main_process(void *pArg);

#endif  //__SKL_STOR_CFG_H__

