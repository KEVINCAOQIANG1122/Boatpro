/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	interfunc.h
  Author: ���      Version: V1.00       Date: 2012.03.21 
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
#ifndef _TESTING_H
#define _TESTING_H

/* Private define ------------------------------------------------------------*/
#define TEST_200MS_PERIOD	40		//200ms��ʱ

#define TEST_KEYSTA_IDLE	0		//����ɨ��״̬��
#define TEST_KEYSTA_STEP1	1		//����ɨ��״̬��
#define TEST_KEYSTA_STEP2	2		//����ɨ��״̬��
#define TEST_KEYSTA_STEP3	3		//����ɨ��״̬��

#define TEST_RFXMT_IDLE		0		//����״̬��
#define TEST_RFXMT_STEP1	1		//����״̬��
#define TEST_RFXMT_STEP2	2		//����״̬��
#define TEST_RFXMT_STEP3	3		//����״̬��

#define TEST_XMTBUF_LEN		100

#define TEST_XMTTYPE_CARRIER	0
#define TEST_XMTTYPE_MOD		1
#define TEST_XMTTYPE_SIG2		2
#define TEST_XMTTYPE_T2			3
#define TEST_XMTTYPE_T3			4
#define TEST_XMTTYPE_STSIGNAL2	5   //SART ��׼�����ź�2��00001111
/* Private typedef -----------------------------------------------------------*/

/**
	@ingroup DEBUG
	@brief ����ģʽ���ƽṹ��
*/
typedef struct {
	U32 TimeCnt;
	U16 TimePeriod;			//200ms��ʱ

	U8  KeyScanSta;			//����ɨ��״̬��
	U8  LEDMsgFlag;			//��LED����ģ�鴫����Ϣ��־

	U8  ucTestNum;			//������

	U8  ucRFXmtSta;
	U8  ucXmtEn;			//���ݷ���ʹ��
	U8  ucXmtData;			//����������
	U8  ucXmtType;			
	U8  ucXmtBuf[TEST_XMTBUF_LEN];		//���������ݻ���
	U16 uiXmtBufLen;

	U8 FirstGenFlag;

    U8  FirstEnterTestFlag; //lq ��һ�ν������̬
} TestCtrlStruct;

//��ص��������ƽṹ��
#define BATTERY_TEST_CLOCK_INTERVAL 75

#define BATTERY_TEST_STATE_POWERUP 		0//����״̬
#define BATTERY_TEST_STATE_HIGHFREQ		1//��Ƶģʽ
#define BATTERY_TEST_STATE_LOWFREQ		2//��Ƶģʽ
#define BATTERY_TEST_STATE_WAIT			3//�ȴ�ģʽ

#define BATTERY_TEST_MIN_COUNT 			30//һ����  == 30*2 s

#define BATTERY_TEST_GPS_HOT_POWERUP	5//GPS������ʱ��10��

#define BATTERY_TEST_WAIT_STEP1			0
#define BATTERY_TEST_WAIT_STEP2			1
typedef struct{
	//��ʱ�����
	U32 ClockCount;
	U32 ClockSlotPast;//���ڸ���slot��ʱ
	
	U32 TimeCntGps;
	U32 TimeCntMessage;
	U32 TimeCntWait;
	U16 TimePeriod;	

	U8 CtrlState;//״̬��

	U8 GpsPowerOn;

	S32 FrameCntPast;

	//U8 WaitForSleep;
	//U8 WaitState;
	//U16 WaitCount;
}BatteryTestCtrlStruct;

///////////////////////////
//��Ϣ���ֿ���

#define MSG_TEST_STATE_POWERUP 	0
#define MSG_TEST_STATE_WAIT 	1
#define MSG_TEST_STATE_GPS		2
#define MSG_TEST_STATE_SEND		3

typedef struct{
	U8 CtrlState;
	U16 TimePeriod; 
	U32 SendIntervalTimeCnt;
	U32	GpsPoweronTimeCnt;
	U32 GpsProtectTimeCnt;
	U8 	WakeupFlag;
	U8 GpsPowerOn;
	U8 GpsProtect;
	S32 FrameCntPast;
}MsgTestCtrlStruct;

//////////////////////////////////
//���߿��Ʋ���
#define SLEEP_TEST_STATE_IDEL	0
#define SLEEP_TEST_STATE_STEP1	1

typedef struct{
	U8 CtrlState;
	U16 TimePeriod; 
	U32 TimeCnt;
	U32 WaitCount;
}SleepTestCtrlStruct;

typedef struct MsgTestStruct{
    U8 ucPowerDownEn;
}MsgTestStruct;

typedef struct RxTestStruct
{
    U8 RxTestEn;
}RxTestStruct;

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
#define TEST_TESTSTA_IDLE		0
#define TEST_TESTSTA_CARRIER	1
#define TEST_TESTSTA_MOD		2
#define TEST_TESTSTA_FREQADJEN	3
#define TEST_TESTSTA_MODADJEN	4
#define TEST_TESTSTA_FREQADJ	5
#define TEST_TESTSTA_MODADJ		6
#define TEST_TESTSTA_SIG2		7
#define TEST_TESTSTA_T2			8
#define TEST_TESTSTA_T3			9
#define TEST_TESTSTA_STSIGNAL_1 10
#define TEST_TESTSTA_STSIGNAL_2 11
#define TEST_TESTSTA_STSIGNAL_3 12
#define TEST_TESTSTA_GRIDADJEN  13
#define TEST_TESTSTA_GRIDADJ	14
#define TEST_TESTSTA_STSIGNAL2	15       //SART��׼�����ź�2��00001111

/* Public  variables ---------------------------------------------------------*/

extern TestCtrlStruct TestCtrlPara;
extern MsgTestStruct struMsgTestPara;
extern RxTestStruct RxTestInstance;

/* Public  function  ---------------------------------------------------------*/
extern void IniTestingPara(void);
extern void IniTesting(void);

/**
	@ingroup DEBUG
	@brief ����ģʽ������
	@param ��
	@retval ��
*/
extern void Testing(void);

/**
	@ingroup DEBUG
	@brief ����ģʽ����ɨ��
	@param ��
	@retval ��

	����ģʽ�°�SOS���л��źŷ���ģʽ
*/
extern void Testing_KeyScan(void);

/**
	@ingroup DEBUG
	@brief ����ģʽ������
	@param ucType ����ģʽ
	@param ulFreq �����ź�Ƶ��
	@retval TRUE ���ò���ģʽ�ɹ�
	@retval FALSE ���ò���ģʽʧ��
*/
extern U8 Testing_SetTestType(U8 ucType, U32 ulFreq);

/**
	@ingroup DEBUG
	@brief ����GMSK���ƺ�Ĳ����ź�
	@param ��
	@retval ��

	���ݲ�ͬ�Ĳ���ģʽ������ͬ�ĵ��ƺ�Ĳ����ź�
*/
extern void GMSKTest(void);

/**
	@ingroup DEBUG
	@brief ����ģʽ�¶�ʱ���������ź�
	@param ��
	@retval ��

	ÿ200ms����һ�β����ź�
*/
extern void Testing_TestMain(void);
extern void ResetTesting(void);

extern TestCtrlStruct TestCtrlPara;
extern void Testing_TestMain(void);

extern void BatteryTestCtrlLowFreq(void);
extern void InitBatteryTest(void);
//extern void BatteryTestCtrl(void);
extern void BatteryTestSendMsg(void);
extern void BattertTestClock(void);
extern U32 SetBatteryTestTimer(U32 period);
extern U32 GetBattertTestTick(void);

extern U8 GetMsgTestCtrlWakeupFlag(void);
extern void InitMsgTestCtrl(void);
extern void InitSleepTestCtrl(void);
extern void MsgTestCtrl(void);
extern void SleepTestCtrl(void);
extern void InitMsgTest(void);
#endif
