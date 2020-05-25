#ifndef __SOFT_PUBLIC_H__
#define __SOFT_PUBLIC_H__

#include <stdio.h>
#include <errno.h>

#define IN 
#define OUT 
#define INOUT 

#define BOOL_TRUE                   1
#define BOOL_FALSE                  0

#define SOFT_ADDFLAGS(x, y)          ((x) |= (y))
#define SOFT_REMOVEFLAGS(x, y)       ((x) &= (~(y)))
#define SOFT_CHECKFLAGS(x, y)        ((x) & (y))

#define SOFT_CODE_DEBUG_CTL
#if defined(SOFT_CODE_DEBUG_CTL)
#define SOFT_CODE_DEBUG printf
#else
#define SOFT_CODE_DEBUG(...)
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
typedef enum _SOFT_MODULE_ID_e_
{
    SOFT_MODULE_PRI_UART_GB = 1,     /* RS232消息处理私有队列 */
    SOFT_MODULE_PRI_UART_MCU,        /* MCU消息解析私有队列 */
    
    SOFT_MODULE_ACCESS,          /* 接入模块 */
    SOFT_MODULE_STOR,            /* 存储模块 */

    SOFT_MODULE_MAX,
}SOFT_MODULE_ID_e;

/* Error Code */
typedef enum _SOFT_STAT_CODE_e_
{
    /* 0 ~ 9 */
    SOFT_STAT_SUCCESS = 0,
    SOFT_STAT_FAIL_INVAILD_PARAM,
    SOFT_STAT_FAIL_FTOK,
    SOFT_STAT_FAIL_GET_TIME,
    SOFT_STAT_FAIL_GET_LOCALTIME,
    SOFT_STAT_FAIL_SELECT,
    SOFT_STAT_FAIL_SELECT_NO_DATA,

    /* 10 ~ 19 */
    SOFT_STAT_FAIL_QUEUE_MSGGET = 10,
    SOFT_STAT_FAIL_QUEUE_SEND,
    SOFT_STAT_FAIL_QUEUE_FULL,
    SOFT_STAT_FAIL_QUEUE_RECV,
    SOFT_STAT_FAIL_QUEUE_BUSY,

    /* 20 ~ 29 */
    SOFT_STAT_FAIL_FILE_OPEN = 20,
    SOFT_STAT_FAIL_FILE_LOCK,
    SOFT_STAT_FAIL_FILE_UNLOCK,
    SOFT_STAT_FAIL_FILE_FCNTL,
    SOFT_STAT_FAIL_FILE_TCGET,
    SOFT_STAT_FAIL_FILE_TCSET,
    SOFT_STAT_FAIL_FILE_NONEXIST,

    /* 30 ~ 39 */
    SOFT_STAT_FAIL_GB_BEGIN_FLAG = 30,
    SOFT_STAT_FAIL_GB_BCC_CHECK,
    SOFT_STAT_FAIL_UART_WRITE,
    SOFT_STAT_FAIL_UART_CFG,
    SOFT_STAT_FAIL_UART_TYPE,

    /* 40 ~ 49 */
    SOFT_STAT_FAIL_XML_LOAD = 40,
    SOFT_STAT_FAIL_XML_SAVE,
    SOFT_STAT_FAIL_XML_NONEXIT,
    SOFT_STAT_FAIL_XML_SET,

    /* 50 ~ 59 */
    SOFT_STAT_FAILE_THREAD_CREATE = 50,
    SOFT_STAT_FAILE_THREAD_DETACH,
}SOFT_STAT_CODE_e;

#endif //__SOFT_PUBLIC_H__

