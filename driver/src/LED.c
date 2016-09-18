/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: LED.c
  Author: vozhj      Version : V1.00        Date: 2011.10.18
  Description:    LED����ģ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include "stm32f10x.h"
#include "LED.h"
#include "Include.h"

PinPulseStruct struPinPulsePara;

/************************************************************
*  Function:       InitPinPulse								*
*  Description:    �������������ʼ��						*
*  Input:          ��										*
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void InitPinPulse(void)
{
   struPinPulsePara.TimeCnt = 0;
   struPinPulsePara.TimePeriod = 1; 
   struPinPulsePara.PinPulseFlg = FALSE;
}

/************************************************************
*  Function:       PinPulseDown								*
*  Description:    ������������						        *
*  Input:          ��										*
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void PinPulseDown(void)
{
    if (struPinPulsePara.PinPulseFlg == TRUE)
    {
       if (struPinPulsePara.TimeCnt == GetLocalClockTickProcess())
       {
           struPinPulsePara.PinPulseFlg = FALSE;
           TEST_OUT_LOW_1();       
       }
    }
}

/************************************************************
*  Function:       InitiaGPIO								*
*  Description:    ������ʼ��								*
*  Input:          ��										*
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void InitiaLED(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = LED_GREEN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_GREEN_PORT, &GPIO_InitStructure);
		
	GPIO_InitStructure.GPIO_Pin = LED_YELLOW_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_YELLOW_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LED_RED_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_RED_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LED_BLUE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_BLUE_PORT, &GPIO_InitStructure);

	LED_GREEN_OFF();
	LED_YELLOW_OFF();
	LED_RED_OFF();
	LED_BLUE_OFF();
}

#if 1
/*����--------------*/

/************************************************************
*  Function:       InitTestIO								*
*  Description:    ��ʼ�����Թܽţ�IO4								*
*  Input:          ��										*
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void InitTestIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	 
	GPIO_InitStructure.GPIO_Pin = TEST_OUT_PIN_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TEST_OUT_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = TEST_OUT_PIN_2;
	GPIO_Init(TEST_OUT_PORT, &GPIO_InitStructure);

    TEST_OUT_LOW_1();
    TEST_OUT_LOW_2();    	
}

#endif
