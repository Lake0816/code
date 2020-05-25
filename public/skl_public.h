#ifndef __SKL_PUBLIC_H__
#define __SKL_PUBLIC_H__

#include <stdio.h>
#include <errno.h>

#define IN 
#define OUT 
#define INOUT 

#define BOOL_TRUE                   1
#define BOOL_FALSE                  0

#define SKL_ADDFLAGS(x, y)          ((x) |= (y))
#define SKL_REMOVEFLAGS(x, y)       ((x) &= (~(y)))
#define SKL_CHECKFLAGS(x, y)        ((x) & (y))

#define SKL_CODE_DEBUG_CTL
#if defined(SKL_CODE_DEBUG_CTL)
#define SKL_CODE_DEBUG printf
#else
#define SKL_CODE_DEBUG(...)
#endif

typedef char                        CHAR;
typedef unsigned char               UCHAR;
typedef unsigned char               UINT8;
typedef unsigned char               BOOL;
typedef short                       SHORT;
typedef unsigned short              USHORT;
typedef unsigned short              UINT16;
typedef int                         INT;
typedef unsigned int                UINT;
typedef long                        LONG;
typedef unsigned long               ULONG;
typedef double                      DOUBLE;
typedef float                       FLOAT;

/* Module ID */
typedef enum _SKL_MODULE_ID_e_
{
    SKL_MODULE_PRI_UART_GB = 1,     /* RS232消息处理私有队列 */
    SKL_MODULE_PRI_UART_MCU,        /* MCU消息解析私有队列 */
    
    SKL_MODULE_ACCESS,          /* 接入模块 */
    SKL_MODULE_STOR,            /* 存储模块 */

    SKL_MODULE_MAX,
}SKL_MODULE_ID_e;

/* Error Code */
typedef enum _SKL_STAT_CODE_e_
{
    /* 0 ~ 9 */
    SKL_STAT_SUCCESS = 0,
    SKL_STAT_FAIL_INVAILD_PARAM,
    SKL_STAT_FAIL_FTOK,
    SKL_STAT_FAIL_GET_TIME,
    SKL_STAT_FAIL_GET_LOCALTIME,
    SKL_STAT_FAIL_SELECT,
    SKL_STAT_FAIL_SELECT_NO_DATA,

    /* 10 ~ 19 */
    SKL_STAT_FAIL_QUEUE_MSGGET = 10,
    SKL_STAT_FAIL_QUEUE_SEND,
    SKL_STAT_FAIL_QUEUE_FULL,
    SKL_STAT_FAIL_QUEUE_RECV,
    SKL_STAT_FAIL_QUEUE_BUSY,

    /* 20 ~ 29 */
    SKL_STAT_FAIL_FILE_OPEN = 20,
    SKL_STAT_FAIL_FILE_LOCK,
    SKL_STAT_FAIL_FILE_UNLOCK,
    SKL_STAT_FAIL_FILE_FCNTL,
    SKL_STAT_FAIL_FILE_TCGET,
    SKL_STAT_FAIL_FILE_TCSET,
    SKL_STAT_FAIL_FILE_NONEXIST,

    /* 30 ~ 39 */
    SKL_STAT_FAIL_GB_BEGIN_FLAG = 30,
    SKL_STAT_FAIL_GB_BCC_CHECK,
    SKL_STAT_FAIL_UART_WRITE,
    SKL_STAT_FAIL_UART_CFG,
    SKL_STAT_FAIL_UART_TYPE,

    /* 40 ~ 49 */
    SKL_STAT_FAIL_XML_LOAD = 40,
    SKL_STAT_FAIL_XML_SAVE,
    SKL_STAT_FAIL_XML_NONEXIT,
    SKL_STAT_FAIL_XML_SET,

    /* 50 ~ 59 */
    SKL_STAT_FAILE_THREAD_CREATE = 50,
    SKL_STAT_FAILE_THREAD_DETACH,
}SKL_STAT_CODE_e;

#endif //__SKL_PUBLIC_H__

