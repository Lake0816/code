#ifndef __SOFT_ACCESS_COMMON_H__
#define __SOFT_ACCESS_COMMON_H__

#define SOFT_ACCESS_BCD2DIGIT(a) ((((a) & 0xF0) >> 4) * 10 + ((a) & 0x0F))

#define SOFT_VEHICLE_GB_UART         "/dev/ttyS1"

typedef enum _SOFT_CMD_e_
{
    /* �ɼ���� */
    GB_UART_CLLC_STANDARD_VER = 0x00,                /* ִ�б�׼�汾 */
    GB_UART_CLLC_CURRENT_DRIVER_LINCENSE,            /* ��ǰ��ʻԱ��Ϣ*/
    GB_UART_CLLC_REAL_TIME,                          /* ʵʱʱ�� */
    GB_UART_CLLC_ACCUMULATED_MILEAGE,                /* �ۼ���ʻ��� */
    GB_UART_CLLC_IMPULSE_RATIO,                      /* ��ȡ����ϵ�� */
    GB_UART_CLLC_VEHICLE_INFO,                       /* ������Ϣ */
    GB_UART_CLLC_STATUS_SINGAL_CONFIG_INFO,          /* �ź�������Ϣ */
    GB_UART_CLLC_RECORDER_UNIQUE_NUMBER,             /* ��¼��Ψһ��� */
    GB_UART_CLLC_DRIVE_SPEED_ANNAL,                  /* ��ʻ�ٶȼ�¼ */
    GB_UART_CLLC_LOCAT_INFO_ANNAL,                   /* λ����Ϣ��¼ */
    GB_UART_CLLC_ACCIDENT_SUSPECT_ANNAL,             /* �¹��ɵ��¼ */
    GB_UART_CLLC_OVER_DRIVING_ANNAL,                 /* ��ʱ��ʻ��¼ */
    GB_UART_CLLC_DRIVER_IDENTITY_ANNAL,              /* ��ʻ����ݼ�¼ */
    GB_UART_CLLC_EXTERN_POWER_SUPPLY_ANNAL,          /* �ⲿ�����¼ */
    GB_UART_CLLC_PARAM_MODIFY_ANNAL,                 /* �����޸ļ�¼ */
    GB_UART_CLLC_SPEED_STATUS_LOG,                   /* �ٶ�״̬��¼ */
    GB_UART_CLLC_MAX = 0x1F,

    /* ������� */
    GB_UART_SET_VEHICLE_INFO = 0x82,                 /* ������Ϣ */
    GB_UART_SET_INITIAL_INSTALL_TIME,                /* ���ΰ�װ���� */
    GB_UART_SET_STATUS_SINGAL_CFG_INFO,              /* ״̬��������Ϣ */
    GB_UART_SET_REAL_TIME = 0xC2,                    /* ��¼��ʱ�� */
    GB_UART_SET_IMPULSE_RATIO,                       /* ��¼����ϵ�� */
    GB_UART_SET_INITIAL_MILEAGE,                     /* ���ΰ�װ����ʻ����� */
    GB_UART_SET_MAX = 0xCF,

    /* �춨��� */
    GB_UART_VERIFICATION_ENTER_OR_KEEP = 0xE0,       /* ����򱣳ּ춨״̬ */
    GB_UART_VERIFICATION_MILEAGE,                    /* ��������� */
    GB_UART_VERIFICATION_PULSE_COEFFICIENT,          /* ����ϵ�������� */
    GB_UART_VERIFICATION_REAL_TIME,                  /* ʵʱʱ�������� */
    GB_UART_VERIFICATION_EXIT,                       /* ��Χ����״̬ */

    GB_UART_MAX = 0xFF,
}SOFT_CMD_e;

extern ULONG SoftAccess_init(void);
extern void SoftAccess_destory(void);
extern BOOL SoftAccess_String2Bcd(IN CHAR *str, OUT UCHAR *BCD, OUT INT *BCD_length);
extern BOOL SoftAccess_Bcd2String(IN UCHAR *BCD, IN INT BCD_length, OUT CHAR *str);
extern ULONG SoftAccess_Bcd2ULONG(IN ULONG m);
extern ULONG SoftAccess_ULONG2Bcd(IN ULONG m);
extern USHORT SoftAccess_String2Short(UINT8 *pData);

#endif //__SOFT_ACCESS_COMMON_H__

