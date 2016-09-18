/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: DeviceInfo.h
  Author: lq       Version : V1.0.0        Date: 2015.05.07
  Description:     �豸��Ϣ����
  Version:         V1.0.0
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _DEVICEINFO_H_
#define _DEVICEINFO_H_

/* Include -------------------------------------------------------------------*/
#include "Include.h"

/* Public define -------------------------------------------------------------*/
/*lq ������ֵ */
#define DEV_INFO_SERIAL_MIN			    0			                            //�������кŴ洢��󳤶�
#define DEV_INFO_SERIAL_MAX			    99999999                                //�������кŴ洢��󳤶�
#define DEV_INFO_GPS_OUTPUT_INTERVAL_MIN        0                               //lq unit: 1s
#define DEV_INFO_GPS_OUTPUT_INTERVAL_MAX        255

/*lq Ĭ��ֵ */
#define DEV_INFO_FREQADJ_DEFAULT        32
#define DEV_INFO_MODAMP_DEFAULT			32
#define DEV_INFO_RF_VOLT_ADJUST_DEFAULT 10           
#define DEV_INFO_WORKTIME_DEFAULT       0
#define DEV_INFO_AIS_CLASS_DEFAULT      (AIS_CLASS_DEFAULT)                     //lq Ĭ��ΪClass B
#define DEV_INFO_GPS_OUTPUT_INTERVAL_DEFAULT    1                               //lq unit: 1s

/*lq GPS������涨�� */
#define DEV_INFO_GPS_OUTPUT_INTERVAL_BUFF_LEN   7
#define DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_RMC  0
#define DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_VTG  1
#define DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GGA  2
#define DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSA  3
#define DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSV  4
#define DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GLL  5
#define DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GBS  6

/* Public typedef ------------------------------------------------------------*/
/*lq �豸��̬��Ϣ�����ñ��棩 */
typedef struct
{
	U16 uiFreqAdj;						//lq ��Ƶ����Ƶ��׼ȷ��
	U16 uiFreqOffset;					//lq Ƶ��ƫ��
	U16 uiModAmpAdj;					//lq ���Ʒ��Ȳ�����16 ~ 48
	U16 uiModAmp;						//lq ���Ʒ���
	U32 uiXmtFreq;
    S32 slXmtFreqComp;                  //lq ����Ƶ�ʲ���ֵ	   slXmtFreqComp= siXmtFreqAdj*1850
	U16 GridVoltAdj;                    //lq դѹDA���Ƶ���
    S16 GridVoltOffset;
    U8  AisClass;

    /*lq P712*/
	U32 SerialNumber;
}DevStaticInfoStruct;

/*lq �豸��̬��Ϣ������ʱ��Ч�� */
typedef struct
{
	U8  ucLowFreqSta;					//lq ���߱�־
	U8  ucWorkMode;						//lq ����̬����������״̬
    U8  TestSta;
	U8  ucGpsOutput;					//lq GPS������״̬
    U8  GpsOutputInterval[DEV_INFO_GPS_OUTPUT_INTERVAL_BUFF_LEN];
    S16 GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_BUFF_LEN];
    U8  OpenCloseTimeIndex;				//lq ���ػ�ʱ������

}DevDynamicInfoStruct;

/* Public variables ----------------------------------------------------------*/
extern DevStaticInfoStruct DevStaticPara;
extern DevDynamicInfoStruct DevDynamicPara;

/* Public functions ----------------------------------------------------------*/

#endif
/*==============================================================================
------------------------------------End of File---------------------------------
==============================================================================*/
