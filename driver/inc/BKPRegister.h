/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: ITDMA.h
  Author: lq       Version : V1.00        Date: 2013.01.15
  Description:     ARM ���� Standby ģʽʱ�������� Backup Register �еı�����ص�
                   �ꡢ�ṹ�嶨�壬��������������
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BKPREGISTER_H
#define _BKPREGISTER_H

/* Includes ------------------------------------------------------------------*/
#include "def.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct BKPRStruct
{
	S16 ssGPS_LongitudH;
	S16 ssGPS_LongitudL;
	S16 ssGPS_LatitudeH;    //lq ��ȷ��0.0001��
	S16 ssGPS_LatitudeL;    //lq ��ȷ��0.0001��
	U16 usGPS_Sog;
	U16 usGPS_Cog;
    U16 usGPS_TimeH;        //lq GPSСʱ
    U16 usGPS_TimeM;        //lq GPS����
    U16 usGpsFallCount;     //lq GPS��λʧ�ܼ���
    U8  ucGpsWaitFix;       //lq GPS������λ��־
    U8  ucGpsLngOpenFirstHour;//lq GPS��һСʱ�ڳ�����־
    U8  ucGpsLngOpenBeyFirstHour;//lq GPS����һСʱ������־
    U8  ucGps5minLngOpenEn; //lq GPS5min����ʹ�ܱ�־
    U8  ucGpsPowerupFix;	//lq GPS������λ��־
    U8  ucGpsCtrlSta;		//lq GPS״̬��״̬

    S16 GpsUnfix1hPeriodTimSlot;        //lq δ��λ1h��ʱ
    S16 GpsUnfix1hPeriodTimFrameH;
    S16 GpsUnfix1hPeriodTimFrameL;
	S16 ssGpsStartFrameH;
	S16 ssGpsStartFrameL;
	S16 ssGpsStartSlot;
    S16 ssGpsOffTimFrameH;    
    S16 ssGpsOffTimFrameL;    
    S16 ssGpsOffTimSlot;    
    S16 ucGpsOnTimEn;       //lq GPS������ʱ��Ч
    S16 ucGpsOffTimEn;      //lq GPS�رն�ʱ��Ч

    S16 ssBurstTxTimFrameH; //lq ����Ϣ���䶨ʱ֡��λ
    S16 ssBurstTxTimFrameL;
    S16 ssBurstTxTimSlot;   //lq ����Ϣ���䶨ʱʱ϶

    S8  ucSlotTimeout;      //lq ͨ��״̬��ʱ϶��ʱ
    U16 usSlotOffset;       //lq ͨ��״̬��ʱ϶ƫ��

    U8  ucRtcLSEorLSI;      //lq ʹ���ڲ�or�ⲿ����
    U8  ucRtcAlarmMcuOrGps;	//lq Standby��������Arm��ʱorGPS��ʱ
    S16 ssRtcAlarmValue;    //lq Standby����ʱ��

    S16 gsiPhaseCnt;        //lq ������λ����  
    S16 gsiSlotCnt;         //lq ����ʱ϶����  
    S16 gslFrameCntH;       //lq ����֡������λ  
    S16 gslFrameCntL;       //lq ����֡������λ 
#if 0
	U16 usGpsFallCount;     //GPS��λʧ�ܼ���
	U8	ucGpsWaitFix;       //GPS�����ȶ�λ��־
//    U8  ucShutdownFlg;    //lq �ػ�or���߱�־λ
    U8  ucStandbyMod;
#endif                                                              
    
    S16 TimAdjust1;         //��ʱ��ÿ2�����phase��
    S16 TimAdjust10;        //��ʱ��ÿ10�����phase��
    S16 TimAdjust100;       //��ʱ��ÿ50�����phase��
    S16 TimAdjustPeriodCntLast;//lq �ϴμ������ڣ��룩��
#if 0
    S16 RtcPhaseAdjust1;    //rtcÿ1�������phase����
    S16 RtcPhaseAdjust10;   //rtcÿ10�������phase����
    S16 RtcPhaseAdjust100;  //rtcÿ100�������phase����
#endif
}BKPRStruct;

/* Private macro ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function  ---------------------------------------------------------*/

/* Public  macro ------------------------------------------------------------*/
/*lq ���������ݼĴ������������궨��*/
#define BKPR_MAXNUM                     42                                      //lq BKP Registers �����Ŀ
#define BKPR_PARA_MAXNUM                42                                      //lq ����BKP Registers�еĲ�������Ŀ

#define BKPR_FLAG_BIT_GPS_LNG_OPEN_FIRST_HOUR_INDEX         0
#define BKPR_FLAG_BIT_GPS_LNG_OPEN_BEY_FIRST_HOUR_INDEX     1
#define BKPR_FLAG_BIT_GPS_5MIN_LONG_OPEN_EN_INDEX           2
#define BKPR_FLAG_BIT_GPS_POWER_UP_FIX_INDEX                3
#define BKPR_FLAG_BIT_GPS_ON_TIM_EN_INDEX                   4
#define BKPR_FLAG_BIT_GPS_OFF_TIM_EN_INDEX                  5
#define BKPR_FLAG_BIT_GPS_LEAP_SECOND_INDEX                 6
#define BKPR_FLAG_BIT_RTC_LSE_OR_LSI_INDEX                  7
#define BKPR_FLAG_BIT_RTC_TIM_PHASE_INDEX                   8
#define BKPR_FLAG_BIT_GPS_SYNCH_ENABLE_INDEX                9   //lq �����ã���ɾ��
#define BKPR_FLAG_BIT_GPS_SAVE_INDEX                        10
#define BKPR_FLAG_BIT_GPS_ON_INDEX                          11
#define BKPR_FLAG_BIT_EQ_WORKMOD_INDEX                      12  //lq ʹ�ò���ֵ��ֻ�������ֹ���ģʽ

#define BKPR_FLAG_INDEX                         0
#define BKPR_GPS_LONGITUDE_H_INDEX              1
#define BKPR_GPS_LONGITUDE_L_INDEX              2
#define BKPR_GPS_LATITUDE_H_INDEX               3
#define BKPR_GPS_LATITUDE_L_INDEX               4
#define BKPR_GPS_SOG_INDEX                      5
#define BKPR_GPS_COG_INDEX                      6
#define BKPR_RTC_ALARM_VALUE_L_INDEX            7
//#define BKPR_GPS_TIME_HOUR_INDEX                7
//#define BKPR_GPS_TIME_MIN_INDEX                 8
#define BKPR_RTC_TIM_PHASE_L_INDEX              8
#define BKPR_GPS_FALL_COUNT_INDEX               9
#define BKPR_GPS_CTRL_STA_INDEX                 10
#define BKPR_GPS_UNFIX_1H_PERIOD_TIM_FRAME_H_INDEX  11 
#define BKPR_GPS_UNFIX_1H_PERIOD_TIM_FRAME_L_INDEX  12
#define BKPR_GPS_UNFIX_1H_PERIOD_TIM_SLOT_INDEX     13
#define BKPR_GPS_START_FRAME_H_INDEX            14
#define BKPR_GPS_START_FRAME_L_INDEX            15
#define BKPR_GPS_START_SLOT_INDEX               16
#define BKPR_GPS_OFF_FRAME_H_INDEX              17 
#define BKPR_GPS_OFF_FRAME_L_INDEX              18
#define BKPR_GPS_OFF_SLOT_INDEX                 19
#define BKPR_BURST_TX_TIM_FRAME_H_INDEX         20
#define BKPR_BURST_TX_TIM_FRAME_L_INDEX         21
#define BKPR_BURST_TX_TIM_SLOT_INDEX            22
#define BKPR_SLOT_TIMEOUT_INDEX                 23
#define BKPR_SLOT_OFFSET_INDEX                  24
#define BKPR_RTC_TIM_PHASE_H_INDEX              25
#define BKPR_RTC_FREQUENCE_INDEX                26
#define BKPR_RTC_ALARM_MCU_OR_GPS_INDEX         27
#define BKPR_RTC_ALARM_VALUE_H_INDEX            28
#define BKPR_PHASE_CNT_INDEX                    29
#define BKPR_SLOT_CNT_INDEX                     30
#define BKPR_FRAME_CNT_H_INDEX                  31
#define BKPR_FRAME_CNT_L_INDEX                  32
#define BKPR_TIM_ADJUST_1_INDEX                 33
#define BKPR_TIM_ADJUST_10_INDEX                34
#define BKPR_TIM_ADJUST_100_INDEX               35
#define BKPR_TIM_ADJUST_PERIOD_CNT_LAST_INDEX   36
//#define BKPR_RTC_ADJUST_1_INDEX                 37
//#define BKPR_RTC_ADJUST_10_INDEX                38
//#define BKPR_RTC_ADJUST_100_INDEX               39
#define BKPR_GPS_INTERVAL_CNT_INDEX             37
#define BKPR_RAND_SEED_H_INDEX                  40
#define BKPR_RAND_SEED_L_INDEX                  41

#define STANDBY_MOD_SHUTDOWN            0
#define STANDBY_MOD_SLEEPING            1
/* Public  variables ---------------------------------------------------------*/
extern U16 arrBKPDRData[BKPR_MAXNUM];
extern const U8 arrBKP_DR_PARA_NAME[BKPR_PARA_MAXNUM][44];
/* Public  function  ---------------------------------------------------------*/
extern void InitBKP_DRData(void);
extern void SaveBKP_DRData(void);
extern void GetBKP_DRData(void);
extern void BKP_DRDataInit(void);
extern void InitBKP_DR(void);
extern U8 IsRtcAlarm(void);
#endif
