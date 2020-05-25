#include "soft_public.h"

#include <termios.h>
#include <string.h>  
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "soft_common_uart.h"

/**********************************************************
 * Function Name:        SoftUart_cfg
 * Description:             串口配置
 * Parameters[in]:        IN INT fd
 *                               IN SOFT_UART_CFG_PARAM_s stCf
 * Parameters[out]:      void              
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.16
***********************************************************/
ULONG SoftUart_cfg(IN INT fd, IN SOFT_UART_CFG_PARAM_s stCfg)  
{  
     
	int   i;
	int   speed_arr[] = {B115200, B19200, B9600, B4800, B2400, B1200, B300};  
	int   name_arr[] = {115200,  19200,  9600,  4800,  2400,  1200,  300};          
	struct termios options;  
       
	if(tcgetattr(fd, &options) != 0)  
	{  
		perror("SetupSerial 1");      
		return SOFT_STAT_FAIL_FILE_TCGET;   
	}  
    
	for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)  
	{  
		if  (stCfg.baud_rate == name_arr[i])  
		{               
			cfsetispeed(&options, speed_arr[i]);   
			cfsetospeed(&options, speed_arr[i]);    
		}  
	}       
     
    options.c_cflag |= CLOCAL;
    options.c_cflag |= CREAD;  
    
    switch(stCfg.flow_ctrl)  
    {  
        
		case 0 :
            options.c_cflag &= ~CRTSCTS;  
            break;     
        
		case 1 :
            options.c_cflag |= CRTSCTS;  
            break;  
		case 2 :
            options.c_cflag |= IXON | IXOFF | IXANY;  
            break;
		default:     
            fprintf(stderr,"Unsupported flow ctrl\n");  
            return SOFT_STAT_FAIL_INVAILD_PARAM; 			
    }  

    options.c_cflag &= ~CSIZE;  
    switch (stCfg.data_bits)  
    {    
		case 5:  
            options.c_cflag |= CS5;  
            break;  
		case 6:  
            options.c_cflag |= CS6;  
            break;  
		case 7:      
            options.c_cflag |= CS7;  
            break;  
		case 8:      
            options.c_cflag |= CS8;  
            break;    
		default:     
            fprintf(stderr,"Unsupported data size\n");  
            return SOFT_STAT_FAIL_INVAILD_PARAM;   
    }  
	
    switch (stCfg.parity)  
    {    
        case 'n':  
        case 'N':
            options.c_cflag &= ~PARENB;   
            options.c_iflag &= ~INPCK;      
            break;   
        case 'o':    
        case 'O':
            options.c_cflag |= (PARODD | PARENB);   
            options.c_iflag |= INPCK;               
            break;   
        case 'e':   
        case 'E':    
            options.c_cflag |= PARENB;         
            options.c_cflag &= ~PARODD;         
            options.c_iflag |= INPCK;        
            break;  
        case 's':  
        case 'S':
            options.c_cflag &= ~PARENB;  
            options.c_cflag &= ~CSTOPB;  
            break;   
        default:    
            fprintf(stderr,"Unsupported parity\n");      
            return SOFT_STAT_FAIL_INVAILD_PARAM;   
    }   
 
    switch (stCfg.stop_bits)  
    {
        case 1:     
            options.c_cflag &= ~CSTOPB;
			break;   
        case 2:     
            options.c_cflag |= CSTOPB;
			break;  
        default:     
            fprintf(stderr,"Unsupported stop bits\n");   
            return SOFT_STAT_FAIL_INVAILD_PARAM;  
    }  
     
    options.c_oflag &= ~OPOST;  
    
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  
    //options.c_lflag &= ~(ISIG | ICANON);  

    // 屏蔽0x0d   0x11  0x13 解决字符收不到问题
    options.c_iflag &= ~(BRKINT | ICRNL| IGNCR | INPCK | ISTRIP | IXON);
    //options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON | OCRNL |

    /* 软件流控 */
    //IXON | IXOFF | IXANY);

    options.c_cc[VTIME] = 1;   
    options.c_cc[VMIN] = 1; 
     
    tcflush(fd,TCIFLUSH);  
     
    if (tcsetattr(fd,TCSANOW,&options) != 0)    
    {  
        perror("com set error!\n");    
        return SOFT_STAT_FAIL_FILE_TCSET;   
    }  
    return SOFT_STAT_SUCCESS;   
}  

/**********************************************************
 * Function Name:        SoftUart_open
 * Description:             打开串口
 * Parameters[in]:        IN SOFT_UART_CFG_PARAM_s stCfg
 * Parameters[out]:      OUT int *pFd              
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.16
***********************************************************/
ULONG SoftUart_open(IN SOFT_UART_CFG_PARAM_s stCfg, OUT int *pFd)  
{
    if (pFd == NULL || stCfg.pcName == NULL)
    {
        return SOFT_STAT_FAIL_INVAILD_PARAM;
    }

    *pFd = open(stCfg.pcName, O_RDWR | O_NOCTTY | O_NDELAY);  
    if ((*pFd) < 0)  
    {  
        perror("Can't Open Serial Port");  
        return SOFT_STAT_FAIL_FILE_OPEN;  
    }
	                          
    if(fcntl(*pFd, F_SETFL, 0) < 0)  
    {  
        printf("fcntl failed!\n");  
        return SOFT_STAT_FAIL_FILE_FCNTL;  
    }       
	   
    if(0 == isatty(STDIN_FILENO))  
    {  
        printf("standard input is not a terminal device\n");  
        return SOFT_STAT_FAIL_UART_TYPE;  
    }

    if (SoftUart_cfg(*pFd, stCfg) != SOFT_STAT_SUCCESS)
    {
        return SOFT_STAT_FAIL_UART_CFG;
    }
	
	return SOFT_STAT_SUCCESS;  
}   

/**********************************************************
 * Function Name:        SoftUart_open
 * Description:             打开串口
 * Parameters[in]:        IN SOFT_UART_CFG_PARAM_s stCfg
 * Parameters[out]:      OUT int *pFd              
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.16
***********************************************************/
void SoftUart_close(IN INT fd)  
{  
	close(fd);  
}

/**********************************************************
 * Function Name:        SoftUart_recv
 * Description:             串口读取
 * Parameters[in]:        IN SOFT_UART_CFG_PARAM_s stCfg
 * Parameters[out]:      OUT int *pFd              
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.16
***********************************************************/
ULONG SoftUart_recv(IN INT fd, IN INT data_len, OUT UCHAR *rcv_buf, OUT INT *recvLen)  
{  
    INT len,fs_sel;  
    fd_set fs_read;  
     
    struct timeval time;  
     
    FD_ZERO(&fs_read);  
    FD_SET(fd,&fs_read);  
     
    time.tv_sec = 0;  
    time.tv_usec = 1000;  
   
    fs_sel = select(fd + 1, &fs_read, NULL, NULL, &time);  
    if(fs_sel <= 0)
    {
        return SOFT_STAT_FAIL_SELECT; 
    }  
    else  
    {  
        if(FD_ISSET(fd,&fs_read))
        {
            len = read(fd, rcv_buf, data_len); 
            *recvLen = len;
            return SOFT_STAT_SUCCESS;
        }
        else
        {
            return SOFT_STAT_FAIL_SELECT_NO_DATA;
        }
    }
}  

/**********************************************************
 * Function Name:        SoftUart_send
 * Description:             串口发送
 * Parameters[in]:        IN INT fd
                                 IN UCHAR *send_buf
                                 IN INT data_len
 * Parameters[out]:      void
 * ReturnValue:            ULONG
 * Author:                   liuke
 * Date:                      2020.05.16
***********************************************************/
ULONG SoftUart_send(IN INT fd, IN UCHAR *send_buf, IN INT data_len)  
{  
    int len = 0;  
     
    len = write(fd,send_buf,data_len);  
    if (len != data_len )  
    {
        tcflush(fd, TCOFLUSH);  
        return SOFT_STAT_FAIL_UART_WRITE;  
    }

    return SOFT_STAT_SUCCESS;
}

