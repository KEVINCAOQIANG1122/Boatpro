/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: main.c
  Author: ��ΰ       Version : V1.00        Date: 2011.09.23
  Description:    ��Ŀ�й���UCOS����ϵͳ
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			1����������Main
    	   �������ܣ�����ARM��������ļܹ�
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include "config.h"

#if CONFIG_KEY
#include "stm32f10x.h"
#include "def.h"
#include "systemtick.h"
#include "keyscan.h"
#include "power.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private  function  --------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
SingleKeyStruct PowerKey;
SingleKeyStruct SOSKey;
/* Public  function  ---------------------------------------------------------*/


/*******************************************************************************
* ����  : SingleKeyStateCtrlProcess
* ����  : ��������״̬���ƴ���(Ӧ�ýӿں���)
* ����  : IO_Addr��������ַ
*		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void IniSingleKey(U8 IO_Addr, U8 InitVal)
{	
	SingleKeyStruct *pSingleKeyStruct;
	
	if(IO_Addr == KEY_POWER_ADDR)
	{
		pSingleKeyStruct = &PowerKey;
	}
	else if(IO_Addr == KEY_SOS_ADDR)
	{
		pSingleKeyStruct = &SOSKey;
	}
		
	pSingleKeyStruct->TimePeriod = 1;
	pSingleKeyStruct->XDouCnt = 0;
	pSingleKeyStruct->Val = InitVal;
	//pSingleKeyStruct->Val = ReadSingleKeyValProcess(pSingleKeyStruct);
	pSingleKeyStruct->ChangeFlg = FALSE;



	pSingleKeyStruct->TimeCnt = SetTimePeriodProcess(pSingleKeyStruct->TimePeriod);
}

/*******************************************************************************
* ����  : ReadSingleKeyValProcess
* ����  : ����������ֵ����(�ײ���������)
* ����  : pSingleKeyStruct��
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 ReadSingleKeyValProcess(SingleKeyStruct *pSingleKeyStruct)
{	
	U8 KeyVal = 0;
	
	if (pSingleKeyStruct == &PowerKey)
	{
		if (READ_POWER_ON_PIN() == FALSE)				//�˿�Ϊ�͵�ƽ
		{
			KeyVal = KEY_LOW; 
		}
		else										//�˿�Ϊ�ߵ�ƽ
		{
			KeyVal = KEY_HIGH; 
		}
	}
	else if (pSingleKeyStruct == &SOSKey)
	{		
		if (READ_POWER_EMERGENCY_PIN() == FALSE)				//�˿�Ϊ�͵�ƽ
		{
			KeyVal = KEY_LOW; 
		}
		else										//�˿�Ϊ�ߵ�ƽ
		{
			KeyVal = KEY_HIGH; 
		}
	}
	
	return KeyVal;
}
/*******************************************************************************
* ����  : SingleKeyCtrlMainProcess
* ����  : ��������������������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void SingleKeyCtrl(SingleKeyStruct *pSingleKeyStruct)
{	
	U8 KeyVal;
			
	if (GetLocalClockTickProcess() == pSingleKeyStruct->TimeCnt)
	{				
		pSingleKeyStruct->TimeCnt = SetTimePeriodProcess(pSingleKeyStruct->TimePeriod);								
	
		KeyVal = ReadSingleKeyValProcess(pSingleKeyStruct);
		
		if (KeyVal != pSingleKeyStruct->Val)
		{
			pSingleKeyStruct->XDouCnt++;
			if (pSingleKeyStruct->XDouCnt >= 2)
			{
				pSingleKeyStruct->XDouCnt = 0;
				
				pSingleKeyStruct->Val = KeyVal;
				pSingleKeyStruct->ChangeFlg = TRUE;
			}
		}
		else
		{
			pSingleKeyStruct->XDouCnt = 0;
		}	
	}	
}

/*******************************************************************************
* ����  : SingleKeyCtrlMainProcess
* ����  : ��������������������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void KeyScanProcess(void)
{
	SingleKeyCtrl(&PowerKey);
	SingleKeyCtrl(&SOSKey);	
}
/*******************************************************************************
* ����  : GetIndivIO
* ����  : ��ȡ����IOֵ
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetIndivIO(U8 IO_Addr)
{
	if(IO_Addr == KEY_POWER_ADDR)
	{
		return PowerKey.Val;
	}
	else if(IO_Addr == KEY_SOS_ADDR)
	{
		return SOSKey.Val;
	}
	else
	{
		return 0xFF;
	}
}
#endif
