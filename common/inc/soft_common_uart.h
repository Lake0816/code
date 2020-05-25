#ifndef __SOFT_COMMON_UART_H__
#define __SOFT_COMMON_UART_H__

typedef struct _SOFT_UART_CFG_PARAM_s_
{
    CHAR *pcName;
    int  baud_rate;
    int  flow_ctrl;
    int  data_bits;
    char parity;
    int  stop_bits;
}SOFT_UART_CFG_PARAM_s;

extern ULONG SoftUart_open(IN SOFT_UART_CFG_PARAM_s stCfg, OUT INT *pFd);
extern void SoftUart_close(IN INT fd);
extern ULONG SoftUart_recv(IN INT fd, IN INT data_len, OUT UCHAR *rcv_buf, OUT INT *recvLen);
extern ULONG SoftUart_send(IN INT fd, IN UCHAR *send_buf, IN INT data_len);

#endif //__SOFT_COMMON_UART_H__

