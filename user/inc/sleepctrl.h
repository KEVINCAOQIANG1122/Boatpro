/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	sleepctrl.h
  Author: vozhj      Version: V1.00       Date: 2012.03.21 
  Description:  
  				��Ҫ���ܣ� 
				1. ������غ궨��
				2. ��������
				3. ��������
				4. ����ṹ������
				
  Others:         // �������ݵ�˵��
  Function List:  // ��Ҫ�����б�ÿ����¼Ӧ���������������ܼ�Ҫ˵��
  History:        // �޸���ʷ��¼�б�ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸�
                  // �߼��޸����ݼ���  
    1. Date:
       Author:
       Modification:
*******************************************************************************/
#ifndef _SLEEPCTRL_H
#define _SLEEPCTRL_H
/* Private define ------------------------------------------------------------*/
#define SLEEP_POWERSTA_POWERUP		0
#define SLEEP_POWERSTA_NORMAL		1

#define SLEEP_PRT_PERIOD			2		//10ms����
#define SLEEP_1S_PRTTIME			100		//1s����ʡ�籣��ʱ��

#define SLEEP_USART_FLAG_CLEAR_PERIOD   200     //lq ��λ 5ms

#define SLEEP_STA_IDLE				0				
#define SLEEP_STA_STEP1				1				
#define SLEEP_STA_STEP2				2				
#define SLEEP_STA_STEP3				3

#define SLEEP_SLEEP_TIMER			5		//ʡ��ʱ�䣬5min
#define SLEEP_WAKEUP_TIMER			1		//��������ʱ�䣬1min				

/* Private typedef -----------------------------------------------------------*/
typedef struct {
	U8  PowerSta;
	
	S16 EnterSleepSlot;		//����ʡ��ʱ϶
	S32 EnterSleepFrame;	//����ʡ��֡

	S16 WakeUpSlot;		   //�˳�ʡ��ʱ϶
	S32 WakeUpFrame;	   //�˳�ʡ��֡
	
	U32 TimeCnt;
	U16 TimePeriod;

    U32 TimeCnt1;
	U16 TimePeriod1;

	U16 EnterSleepPrtTime;	//����ʡ�籣��ʱ��

	U8  SleepCtrlSta;		//ʡ�����״̬��

    U8  ucMcuStopFlg;
    U8  ucMcuStandbyFlg;
    U8  ucGpsPulseFlg;      //lq GPS�����廽��
    U8  ucMcuWakeupFlg;     //lq MCU���ѱ�־
    U8  ucUsartFlg;        //lq USART1�жϻ��ѱ�־
    U8  ucStandbyAlarmSetFlg;//lq�������Ѷ�ʱ����
    U8  ucStandbyAlarmSetOk;//lq �������Ѷ�ʱ�������
    U8  ucTimToRtcSwitch;   //lq ��λ������ʽ�л�
    U8  ucRtcToTimSwitch;
}SleepStruct;

extern U8 set_sleep_time_flag;//������������ʱ��
extern SleepStruct		SleepCtrlPara;
extern S32 gslTimToRtcCnt_2;                                                                
extern S32 gslRtcToTimCnt_2;
extern S32 gslTimToRtcIdleCnt_2; 
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/
extern void IniSleepCtrlPara(void);
/**
	@ingroup ����
	@brief ����ģʽ�������߿��ƺ���

	������ģ�鴦�ڿ���״̬����������ϵͳ�������ߣ����趨����ʱ���
*/
extern void SleepCtrlHigFreq(void);
/**
	@ingroup ����
	@brief ����ģʽ���ѿ��ƺ���

	������ʱ�䵽���߰���ʱ��ϵͳ���ѽ�������ģʽ
*/
extern void SleepCtrlLowFreq(void);
extern U8 IsMcuWakeupTimeout(void);
extern S32 GetMcuWakeupTim(void);
extern void SetMcuWakeupSlot(S16 value);
extern void SetMcuWakeupFrame(S32 value);
extern void SetMcuWakeupTim(S16 usValue);
extern void SaveBKP_DRPara(void);
extern U32 CalcuSleepSpan(void);
extern S32 CalcuSleepSlots_Sleep(void);
extern S32 CalcuSleepPhases_Sleep(void);
extern void ResetUsartFlg(void);
extern U8 GetUsartFlgRst(void);
extern U8 GetUsartFlg(void);
#endif
