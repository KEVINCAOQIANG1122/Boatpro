/****************************************************************************
*  Copyright (C), 2010, 712.Co.,Ltd.		                                *
*  FileName: rtc.h	 	                                  	            	*
*  Description:  ʵʱʱ��													*
*****************************************************************************
*  History:               				                                    *
*      <author>  <time>    <version >   <desc>                              *
*      vozhj   	 10/12/02     1.0       build this moudle		            *
****************************************************************************/
#ifndef __RTC_H
#define __RTC_H

/*lq RTC��Ƶ���ú궨��*/
#define RTC_LSE_PRE_VALUE   (64)                        //lq ��ӦƵ��512
#define RTC_LSI_PRE_VALUE   (80)                        //lq ��ӦƵ��500
#define RTC_LSE_FREQ_VALUE  (32768 / RTC_LSE_PRE_VALUE)
#define RTC_LSI_FREQ_VALUE  (40000 / RTC_LSI_PRE_VALUE)

#define RTC_ALARM_INTERVAL_VALUE (1)                    //lq ��ӦLSE��Ƶ���һ������
#define RTC_LSE_ALARM_FREQ_VALUE (RTC_LSE_FREQ_VALUE / RTC_ALARM_INTERVAL_VALUE) //lq ��������ʱ������ʹ��
#define RTC_LSI_ALARM_FREQ_VALUE (RTC_LSI_FREQ_VALUE / RTC_ALARM_INTERVAL_VALUE) //lq ��������ʱ������ʹ��

#define RTC_ADJUST_1S       (RTC_LSE_FREQ_VALUE)        //rtcƵ����1��
#define RTC_ADJUST_10S      (RTC_LSE_FREQ_VALUE * 10)   //rtcƵ����10��
#define RTC_ADJUST_100S     (RTC_LSE_FREQ_VALUE * 100)  //rtcƵ����100��

#define RTC_ADJUST_1S_LSI   (RTC_LSI_FREQ_VALUE)        //rtcƵ����1��
#define RTC_ADJUST_10S_LSI  (RTC_LSI_FREQ_VALUE * 10)   //rtcƵ����10��
#define RTC_ADJUST_100S_LSI (RTC_LSI_FREQ_VALUE * 100)  //rtcƵ����100��

/*lq RTCʱ��Դ�궨��*/
#define RTC_CLK_SOURCE_LSI  0
#define RTC_CLK_SOURCE_LSE  1

/*lq MCU�Ӵ�����ʱ��������GPS��ʱorMCU��ʱ*/
#define RTC_ALARM_GPS  0
#define RTC_ALARM_MCU  1

/*lq RTC��Tim��Ƶ�ʱ任����*/
#define RTC_TIM_PHASE_PRECISE       (1000000.0)
#define RTC_TIM_PHASE_NUM_DEFAULT   (93750000)
typedef struct RtcPara
{
    U16 usRtcFreq;          //lq ���ߴ����ڼ�RTC Alarm Ƶ��
    U8  ucRtcLSEorLSI;      //lq ʹ���ڲ�or�ⲿ����
    U8  ucRtcAlarmMcuOrGps;	//lq Standby��������Arm��ʱorGPS��ʱ
    // U16 ssRtcAlarmValue;    //lq Standby����ʱ��
    U32 ssRtcAlarmValue;    //lq Standby����ʱ��

}RtcPara;

typedef struct RtcTimPhaseStruct
{
    U8  RtcTimPhaseFlg;
    U32 RtcTimPhasePeriodNum;
    U32 RtcTimPhaseCntLast;
    U32 RtcTimPhaseCntNow;
    double RtcTimPhaseNumTemp;
    U32 RtcTimPhaseNum;
    double lfRtcTimPhaseNum;
}RtcTimPhaseStruct;

typedef struct RtcSelfCheckStruct
{
    U8  StartFlg;    
    U8  CompleteFlg;
    U8  RtcLseOkFlg;
}RtcSelfCheckStruct;

extern RtcTimPhaseStruct struRtcTimPhase1;
extern U32 GpsRtcCount;
extern U32 RtcCnt;
extern U32 RtcCnt1s;                                                                   //lq ����Rtc��UTC1s����ڵļ���ֵ
extern U32 RtcCnt10s;                                                                  //lq ����Rtc��UTC10s����ڵļ���ֵ
extern U32 RtcCnt100s;                                                                 //lq ����Rtc��UTC100s����ڵļ���ֵ

extern S16 RtcPhaseAdjust1;//rtcÿ1�������phase����
extern S16 RtcPhaseAdjust10;//rtcÿ10�������phase����
extern S16 RtcPhaseAdjust100;//rtcÿ100�������phase����

extern S16 RtcCntAdjust1;//rtcÿ1���������
extern S16 RtcCntAdjust10;//rtcÿ10���������
extern S16 RtcCntAdjust100;//rtcÿ100���������
extern RtcPara struRtcPara;

extern U16 RtcTimPhaseCnt;

extern U8 gucTestRtc;
//extern void RTC_Configuration(void);
extern void RTCAlarmInt(void);

extern void InitRtcPara(void);
extern void CalcRtcPhaseAdjust(S32 DiffNum ,U16 PeriodCount);
extern void ResetRtcCount(U16 usAlarmValue);

extern void RTCConfigurationPart1(void);
extern void RTCConfigurationPart2(void);

//lnw add
extern void InitRtcParaStruct(RtcPara *pRtcPara);
extern 	void InitRtcTimPhaseStruct(void);
extern void RTC_RtcSelfCheckStructInit(RtcSelfCheckStruct *pRtcSelfCheckStruct);
extern void RTC_SelfCheck(RtcSelfCheckStruct *pRtcSelfCheckStruct);


#endif
