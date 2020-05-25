#ifndef __SKL_COMMON_UART_H__
#define __SKL_COMMON_UART_H__

typedef struct _SKL_UART_CFG_PARAM_s_
{
    CHAR *pcName;
    int  baud_rate;
    int  flow_ctrl;
    int  data_bits;
    char parity;
    int  stop_bits;
}SKL_UART_CFG_PARAM_s;

extern ULONG SklUart_open(IN SKL_UART_CFG_PARAM_s stCfg, OUT INT *pFd);
extern void SklUart_close(IN INT fd);
extern ULONG SklUart_recv(IN INT fd, IN INT data_len, OUT UCHAR *rcv_buf, OUT INT *recvLen);
extern ULONG SklUart_send(IN INT fd, IN UCHAR *send_buf, IN INT data_len);

#endif //__SKL_COMMON_UART_H__

