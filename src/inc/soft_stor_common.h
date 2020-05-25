#ifndef __SOFT_STOR_COMMON_H__
#define __SOFT_STOR_COMMON_H__

#define SOFT_DEV_CFG_FILE      "../../soft_vehicle_cfg.xml"
#define SOFT_DEV_CFG_DEFAUT    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" \
                              "<Cfg>\n" \
                              "\t<VehicleStdVer>12</VehicleStdVer>\n" \
                              "\t<ModifyNum></ModifyNum>\n" \
                              "\t<VehicleInstallDate>2020-05-20 15:43:00</VehicleInstallDate>\n" \
                              "\t<VehicleInitMileage></VehicleInitMileage>\n" \
                              "\t<CurrentDriver></CurrentDriver>\n" \
                              "\t<VehicleInfo>\n" \
                              "\t\t<Indentify></Indentify>\n" \
                              "\t\t<PlateNum></PlateNum>\n" \
                              "\t\t<PlateType></PlateType>\n" \
                              "\t</VehicleInfo>\n" \
                              "\t<VehicleUniqueNum>\n" \
                              "\t\t<3CIdentify></3CIdentify>\n" \
                              "\t\t<IdentifyProdModel></IdentifyProdModel>\n" \
                              "\t\t<BornDate>200520</BornDate>\n" \
                              "\t\t<serialNum>FFFFFFFF</serialNum>\n" \
                              "\t</VehicleUniqueNum>\n" \
                              "</Cfg>"

#define SOFT_CFG_XML_BASE                    "Cfg"
#define SOFT_CFG_XML_VEHICLE_STD_VER         SOFT_CFG_XML_BASE"/VehicleStdVer"
#define SOFT_CFG_XML_MODIFY_NUM              SOFT_CFG_XML_BASE"/ModifyNum"
#define SOFT_CFG_XML_VEHICLE_INSTALL_DATE    SOFT_CFG_XML_BASE"/VehicleInstallDate"
#define SOFT_CFG_XML_VEHICLE_INIT_MILEAGE    SOFT_CFG_XML_BASE"/VehicleInitMileage"
#define SOFT_CFG_XML_CURRENT_DRIVER          SOFT_CFG_XML_BASE"/CurrentDriver"
#define SOFT_CFG_XML_VEHICLE_INFO            SOFT_CFG_XML_BASE"/VehicleInfo"
#define SOFT_CFG_XML_INDENTIFY               SOFT_CFG_XML_VEHICLE_INFO"/Indentify"
#define SOFT_CFG_XML_PLATE_NUM               SOFT_CFG_XML_VEHICLE_INFO"/PlateNum"
#define SOFT_CFG_XML_PLATE_TYPE              SOFT_CFG_XML_VEHICLE_INFO"/PlateType"
#define SOFT_CFG_XML_VEHICLE_UNIQUE_NUM      SOFT_CFG_XML_BASE"/VehicleUniqueNum"
#define SOFT_CFG_XML_3C_IDENTIFY             SOFT_CFG_XML_VEHICLE_UNIQUE_NUM"/3CIdentify"
#define SOFT_CFG_XML_IDENTIFY_PDT_MODLE      SOFT_CFG_XML_VEHICLE_UNIQUE_NUM"/IdentifyProdModel"
#define SOFT_CFG_XML_PDT_BORNDATE            SOFT_CFG_XML_VEHICLE_UNIQUE_NUM"/BornDate"
#define SOFT_CFG_XML_SERIAL_NUM              SOFT_CFG_XML_VEHICLE_UNIQUE_NUM"/serialNum"


/* 全局配置结构体 */
typedef struct _SOFT_STOR_RECORDER_STANDARD_VER_s_
{
    UINT8 ucYear;               /* 执行保准年号后2位BCD */
    UINT8 ucModifyNum;          /* 修改单号 */
    UINT8 reserve[2];
}SOFT_STOR_RECORDER_STANDARD_VER_s;

/* 全局配置结构体 */
typedef struct _SOFT_STOR_DRIVER_LINCESE_s_
{
    UINT8 aucDriverLincese[18]; /* 驾驶员信息 */
    UINT8 reserve[2];
}SOFT_STOR_DRIVER_LINCESE_s;

typedef struct _SOFT_STOR_VEHICLE_INFO_s_
{
    UINT8 aucVehicleIndentify[20];			//max 17bytes
    UINT8 aucVehicleLicensePlate[12];		//max 11 bytes, 3bytes reserve
    UINT8 aucVehicleLicensePlateType[12];	//max 11 bytes, 4bytes reserve
}SOFT_STOR_VEHICLE_INFO_s;

typedef struct _SOFT_STOR_RECORDER_UNIQUE_INFO_s_
{
    UINT8 auc3CIdentify[8];
    UINT8 aucProductType[16];
    UINT8 aucProductDate[4];
    UINT8 aucProdSerialNumber[4];
}SOFT_STOR_RECORDER_UNIQUE_INFO_s;

typedef struct _SOFT_STOR_STANDARD_SIGNAL_CFG_DATA_s_
{
    UINT8 D0[10];
    UINT8 D1[10];
    UINT8 D2[10];
    UINT8 D3[10];
    UINT8 D4[10];
    UINT8 D5[10];
    UINT8 D6[10];
    UINT8 D7[10];
}SOFT_STOR_STANDARD_SIGNAL_CFG_DATA_s;

/* 每秒信息 */
typedef struct _SOFT_STOR_SPEED_INFO_s_
{
        UINT8 ucAverageSpeed;
        UINT8 ucStatusSignal;
}SOFT_STOR_SPEED_INFO_s;

typedef struct _SOFT_STOR_SPEED_RECORD_s_
{
        ULONG ulTimeStamp;			
        SOFT_STOR_SPEED_INFO_s astSpeed[60];       	/* 每秒钟信息 */
}SOFT_STOR_SPEED_RECORD_s;

typedef struct _SOFT_STOR_GPS_DETAIL_s_
{
        UINT8 aucLongitude[4];				/* 经度 */
        UINT8 aucLatitude[4];				/* 纬度 */
        UINT8 aucAltitude[2];				/* 海拔 */
}SOFT_STOR_GPS_DETAIL_s;

typedef struct _SOFT_STOR_GPS_INFO_s_
{
    SOFT_STOR_GPS_DETAIL_s stGpsInfo;
    UINT8 ucAverageSpeed;
}SOFT_STOR_GPS_INFO_s;

typedef struct _SOFT_STOR_LOCATE_RECORD_s_
{
    ULONG ulTimeStamp;			
    SOFT_STOR_GPS_INFO_s astGps[60];      	 /* 每分钟信息 */
}SOFT_STOR_LOCATE_RECORD_s;

typedef struct _SOFT_STOR_ACCUMULATED_MILEAGE_s_
{
    UINT8 aucRealTime[8];
    UINT8 aucInstallTime[8];
    UINT8 aucInitialMileage[4];
    UINT8 aucAccumulatedMileage[4];
}SOFT_STOR_ACCUMULATED_MILEAGE_s;

extern ULONG SoftStor_init(void);
extern void SoftStor_destory(void);

#endif //__SOFT_STOR_COMMON_H__

