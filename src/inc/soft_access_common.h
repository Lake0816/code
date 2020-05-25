#ifndef __SOFT_ACCESS_COMMON_H__
#define __SOFT_ACCESS_COMMON_H__

#define SOFT_ACCESS_BCD2DIGIT(a) ((((a) & 0xF0) >> 4) * 10 + ((a) & 0x0F))

#define SOFT_VEHICLE_GB_UART         "/dev/ttyS1"

typedef enum _SOFT_CMD_e_
{
    /* 采集命令集 */
    GB_UART_CLLC_STANDARD_VER = 0x00,                /* 执行标准版本 */
    GB_UART_CLLC_CURRENT_DRIVER_LINCENSE,            /* 当前驾驶员信息*/
    GB_UART_CLLC_REAL_TIME,                          /* 实时时间 */
    GB_UART_CLLC_ACCUMULATED_MILEAGE,                /* 累计行驶里程 */
    GB_UART_CLLC_IMPULSE_RATIO,                      /* 获取脉冲系数 */
    GB_UART_CLLC_VEHICLE_INFO,                       /* 车辆信息 */
    GB_UART_CLLC_STATUS_SINGAL_CONFIG_INFO,          /* 信号配置信息 */
    GB_UART_CLLC_RECORDER_UNIQUE_NUMBER,             /* 记录仪唯一编号 */
    GB_UART_CLLC_DRIVE_SPEED_ANNAL,                  /* 行驶速度记录 */
    GB_UART_CLLC_LOCAT_INFO_ANNAL,                   /* 位置信息记录 */
    GB_UART_CLLC_ACCIDENT_SUSPECT_ANNAL,             /* 事故疑点记录 */
    GB_UART_CLLC_OVER_DRIVING_ANNAL,                 /* 超时驾驶记录 */
    GB_UART_CLLC_DRIVER_IDENTITY_ANNAL,              /* 驾驶人身份记录 */
    GB_UART_CLLC_EXTERN_POWER_SUPPLY_ANNAL,          /* 外部供电记录 */
    GB_UART_CLLC_PARAM_MODIFY_ANNAL,                 /* 参数修改记录 */
    GB_UART_CLLC_SPEED_STATUS_LOG,                   /* 速度状态记录 */
    GB_UART_CLLC_MAX = 0x1F,

    /* 设置命令集 */
    GB_UART_SET_VEHICLE_INFO = 0x82,                 /* 车辆信息 */
    GB_UART_SET_INITIAL_INSTALL_TIME,                /* 初次安装日期 */
    GB_UART_SET_STATUS_SINGAL_CFG_INFO,              /* 状态量配置信息 */
    GB_UART_SET_REAL_TIME = 0xC2,                    /* 记录仪时间 */
    GB_UART_SET_IMPULSE_RATIO,                       /* 记录脉冲系数 */
    GB_UART_SET_INITIAL_MILEAGE,                     /* 初次安装已行驶总里程 */
    GB_UART_SET_MAX = 0xCF,

    /* 检定命令集 */
    GB_UART_VERIFICATION_ENTER_OR_KEEP = 0xE0,       /* 进入或保持检定状态 */
    GB_UART_VERIFICATION_MILEAGE,                    /* 里程误差测量 */
    GB_UART_VERIFICATION_PULSE_COEFFICIENT,          /* 脉冲系数误差测量 */
    GB_UART_VERIFICATION_REAL_TIME,                  /* 实时时间误差测量 */
    GB_UART_VERIFICATION_EXIT,                       /* 范围正常状态 */

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

