/*******************************************************************************
  Copyright (C), 2012, 712.Co.,Ltd.
  FileName: adjust.h
  Author: ������      Version : V1.00        Date: 2012.07.28
  Description:    ʱ���Զ�У��ģ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#ifndef _ADJUST_
#define _ADJUST_

#include "def.h"

#define ADJUST_MAX_DIFF_VALUE	(20000)//����������������ܵ����ƫ��
#define ADJUST_MAX_WAIT_PLUS_VALUE (96000)//���ȴ�2���GPS���壬������Ϊʧ��
#define ADJUST_CALC_PERIOD		(60)//ͳ�ƵĴ���
#define ADJUST_PHASE_MAX_ADJUST	(100)//������ֵ 

#define ADJUST_TIM_CNT_1S_CEIL      (48000 + 1000)
#define ADJUST_TIM_CNT_1S_FLOOR     (48000 - 1000)

#define ADJUST_RTC_LSE_CNT_1S_CEIL      (512 + 100)
#define ADJUST_RTC_LSE_CNT_1S_FLOOR     (512 - 100)

#define ADJUST_RTC_LSI_CNT_1S_CEIL      (500 + 100)
#define ADJUST_RTC_LSI_CNT_1S_FLOOR     (500 - 100)

#define ADJUST_CTRL_STA_IDEL		(0)		//����״̬
#define ADJUST_CTRL_STA_FRISTFLAG 	(1)		//�ȴ���һ������
#define ADJUST_CTRL_STA_COUNT 		(2)		//ͳ�ƽ׶�
#define ADJUST_CTRL_STA_CALC		(3)		//����׶�

#define ADJUST_RTC_SOURCE_LSI		(0)		//rtcʱ��ԴΪLSI
#define ADJUST_RTC_SOURCE_LSE		(1)		//rtcʱ��ԴΪLSE
#define ADJUST_WAIT_LSI_TIME		(400)	//�ȴ�������2��ѡ��RTCʱ��Դ
	 										//�˿����ȴ�ʱ��Ӧ����С�ھ���GPS������λʱ��
typedef struct{
	U8 AdjustCtrlState;//״̬��
	U32 TimPastCount;//Tim��ȥ�ļ���
	U32 RtcPastCount;//Rtc��ȥ�ļ���
	//U32 NowCount;//���ڵļ���
	S32 TimDiffCount;//Tim����ƫ��
	S32 RtcDiffCount;//Rtc����ƫ��
	S32 TimDiffSum;//Timƫ���
	S32 RtcDiffSum;//Rtcƫ���
	U16 PeriodCount;//���ڼ���
	U8	CalcNowFlag;//���̼���flag

    U32 TimCntLast;                                                                 //lq �ϴα���ı���Tim�ļ���ֵ��δУ׼��
    U32 RtcCntLast;                                                                 //lq �ϴα���ı���Rtc�ļ���ֵ��δУ׼��
    S32 TimCntSec1;                                                                 //lq UTC1s����ڱ���Tim�ļ���ֵ
    S32 RtcCntSec1;                                                                 //lq UTC1s����ڱ���Rtc�ļ���ֵ
    S32 TimCntSec1Sum;
    S32 RtcCntSec1Sum;

	U16 PeriodCountLast;//���ڼ���
	//rtcʱ��ѡ�����
	U32 TimeCnt;
	U16 TimePeriod;	
	U8 	RtcClockSource;//ʱ��Դ
	U8	RtcClockChoice;//ʱ��ѡ��flag  �Ƿ����ʱ��ѡ��

}TimeAdjustStruct;

extern TimeAdjustStruct TimAdjustPara;

extern void InitTimAdjust(void);
extern void ResetTimAdjust(void);
extern void ResetTimAdjustCtrlState(void);
extern void TimAdjustProcess(void);

extern void SetAdjustCalcNow(void);
extern U8 GetAdjustSleepState(void);

extern U8 IsRtcClockSourceLSE(void);

extern void RtcClockChoice(void);

#endif
