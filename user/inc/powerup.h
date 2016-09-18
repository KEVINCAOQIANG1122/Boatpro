/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	powerup.h
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
#ifndef _POWERUP_H
#define _POWERUP_H
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
#define VERSION		"V1.0.0"

/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/
#define KEY_ENABLE_VALUE (300)//3�� 10ms���һ��
extern void InitPowerUppara(void);

/**
   @ingroup MMI
   @brief ��������
   @param ��
   @retval ��

   ���ֿ���ģʽ������POWER�ͳ���SOS
*/
extern void PowerUpProcess(void);
extern void InitBKP_DRPara(void);
extern void SleepWakeUpInit(void);
extern U32 CalcuSleepSlots_Wakeup(const U32 ulAlarmValue);
extern U8 ucStandbyWakeupFlg;
extern U8 ucStandbyWakeupDelayFlg;
//extern void PowerUpLedCtrl(void);
#endif
