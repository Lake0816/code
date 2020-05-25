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
����֡
| AA |				----------------  ����֡��ʶλ
| 75 |				----------------  ����֡��ʶλ 
| CMD |				----------------  ������
| DataLenH |			----------------  ���ݿ鳤�ȸ��ֽ�				
| DataLenL |			----------------  ���ݿ鳤�ȵ��ֽ�
| Reserve | 			----------------  �����ֶ�
| Datablock |  		----------------  ���ݿ�
| checkWord |  		----------------  У���֣�������У����֮��������ֽڣ�
Ӧ��֡
| 55 |				----------------  ����֡��ʶλ
| 7A |				----------------  ����֡��ʶλ 
| CMD |				----------------  ��������֡����������ͬ
| DataLenH |			----------------  ���ݿ鳤�ȣ�0~64k�����ֽ�
| DataLenL |			----------------  ���ݿ鳤�ȣ�0~64k�����ֽ�
| Reserve | 			----------------  �����ֶ�
| Datablock |            ----------------  ���ݿ�
| checkWord |           ----------------  У���֣�������У����֮��������ֽڣ�
*/

extern ULONG GB_uart_init(void);
extern void GB_uart_destory(void);
extern void GB_uart_modulePacketProcess(SOFT_QUEUE_MSG_s *pstMsg);

#endif //__SOFT_ACCESS_GB_UART_H__

