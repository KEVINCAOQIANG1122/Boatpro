/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	powerdown.h
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
#ifndef _POWERDOWN_H
#define _POWERDOWN_H
/* Private typedef -----------------------------------------------------------*/

/**
	@ingroup MMI
	@brief �ػ����ƽṹ��
*/
typedef struct {
	U16 TimeCnt;
	U16 TimePeriod;	
    U16 TimeCnt1min;
    U16 TimePeriod1min;
    U16 TimeCntMin;

	U16 XDouCnt;
	U8 KeyVal;
	U8 KeyValPast;
	
	U8 Sta;

	U16 ShutDownTime;
} PowerDownStruct;
/* Private define ------------------------------------------------------------*/

#define POWERDOWN_STA_IDLE		0
#define POWERDOWN_STA_STEP1		1
#define POWERDOWN_STA_STEP2		2
//#define POWERDOWN_STA_STEP3		3

#define POWERDOWN_TIME_1min_PERIOD       12000
#define POWERDOWN_TIME_Min_PERIOD       1
#define POWERDOWN_POWERUP_PEROID	10		//��������ʱ�䣬���ڼ�ⰴ��̧��50ms��ʱ
#define POWERDOWN_NORMAL_PEROID		1		//��������״̬�¼��ؼ��������ڣ�5ms��ʱ
#define POWERDOWN_SHUTDOWN_TIME		600		//5ms��ʱ��3s����
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/
extern void IniPowerDownPara(void);
/**
	@ingroup MMI
	@brief �ػ�����
	@param ��
	@retval ��

	���POWER����3�����ػ�����
*/
extern void PowerDownProcess(void);

/**
	@ingroup ����
	@brief �鿴�ػ�ģ���Ƿ�æ
	@param ��
	@retval TRUE �ػ�ģ�����
	@retval FALSE �ػ�ģ��æ
*/
extern U8 GetPowerDownSleepSta(void);
extern void ExecShutdownAction(void);

#endif
