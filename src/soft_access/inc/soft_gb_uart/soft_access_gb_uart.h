#ifndef __SOFT_ACCESS_GB_UART_H__
#define __SOFT_ACCESS_GB_UART_H__

#define GB_UART_INDEX_CMD            2
#define GB_UART_INDEX_LEN            3
#define GB_UART_INDEX_RESERVE        5
#define GB_UART_INDEX_DATA           6

/* Contains: head[2] cmd [1] len[2] reserve[1] */
#define GB_UART_LEN_COMMON           6
#define GB_UART_LEN_TIME             6
#define GB_UART_LEN_VER              2
#define GB_UART_LEN_DRIVER_LINCESE   18
#define GB_UART_LEN_VEHICLE_INFO     41
#define GB_UART_LEN_RECORD_UNIQUE    35

#define GB_UART_CLC_BCC(sendbuff, dataLen) \
do \
{ \
    sendbuff[GB_UART_INDEX_LEN] = dataLen / 256; \
    sendbuff[GB_UART_INDEX_LEN + 1] = dataLen % 256; \
    sendbuff[GB_UART_LEN_COMMON + dataLen] =  \
        GB_uart_getBCC(sendbuff, GB_UART_LEN_COMMON + dataLen); \
} while (0);

#define GB_UART_MK_MODULE_MSG(pcData, pstModMsg) \
do \
{ \
    pstModMsg->usCmdID = pcData[GB_UART_INDEX_CMD]; \
    pstModMsg->usLen = SoftAccess_String2Short(pcData + GB_UART_INDEX_LEN); \
    memcpy(pstModMsg->aucData, pcData + GB_UART_INDEX_DATA, pstModMsg->usLen); \
}while (0)

/*
命令帧
| AA |				----------------  数据帧标识位
| 75 |				----------------  数据帧标识位 
| CMD |				----------------  命令字
| DataLenH |			----------------  数据块长度高字节				
| DataLenL |			----------------  数据块长度低字节
| Reserve | 			----------------  保留字段
| Datablock |  		----------------  数据块
| checkWord |  		----------------  校验字（异或除了校验字之外的所有字节）
应答帧
| 55 |				----------------  数据帧标识位
| 7A |				----------------  数据帧标识位 
| CMD |				----------------  与命令字帧的命令字相同
| DataLenH |			----------------  数据块长度（0~64k）高字节
| DataLenL |			----------------  数据块长度（0~64k）低字节
| Reserve | 			----------------  保留字段
| Datablock |            ----------------  数据块
| checkWord |           ----------------  校验字（异或除了校验字之外的所有字节）
*/

extern ULONG GB_uart_init(void);
extern void GB_uart_destory(void);
extern void GB_uart_modulePacketProcess(SOFT_QUEUE_MSG_s *pstMsg);

#endif //__SOFT_ACCESS_GB_UART_H__

