/*******************************************************************************
  Copyright (C), 2010, 712.Co.,Ltd.
  File name:	gps.h
  Author: ���      Version: V1.00       Date: 2012.03.12 
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
#ifndef __GPS_H__
#define __GPS_H__

#include "def.h"
#define GPS_PORT							GPIOA  					//GPS��Դ�ӿ�
#define GPS_RCC_APB2Periph_GPIO  	    	RCC_APB2Periph_GPIOC 	//��ʼGPSʱ��
#define GPS_PIN						    	GPIO_Pin_0 				//GPS��Դ����

#define GPSPOWER_LOW()						GPIO_ResetBits(GPS_PORT, GPS_PIN)
#define GPSPOWER_HIGH()						GPIO_SetBits(GPS_PORT, GPS_PIN)

#define GPS_POWER_OFF()						GPSPOWER_LOW()
#define GPS_POWER_ON()						GPSPOWER_HIGH()

#define PULSE_PORT							GPIOA  					//GPS��Դ�ӿ�
#define PULSE_RCC_APB2Periph_GPIO  	    	RCC_APB2Periph_GPIOA 	//��ʼGPSʱ��
#define PULSE_PIN						    GPIO_Pin_1 				//GPS��Դ����
#define PULSE_EN_SOURCE_PORT	        	GPIO_PortSourceGPIOA
#define PULSE_EN_SOURCE_PIN	           		GPIO_PinSource1
#define PULSE_EN_LINE        				EXTI_Line1

#define PPS_COUNT_THRESHOLD 1//����ǰ����������
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
extern struct UartTxFIFOStruct Uart2TxFIFO; //���ͻ���
extern struct UartRxFIFOStruct Uart2RxFIFO;
/* Private  function  ---------------------------------------------------------*/


/* Public  function  ---------------------------------------------------------*/

extern void InitUsart2(U32 baud);
extern void Usart2Interrupt(void);
extern void Exti1_Int(void);
extern void InitGPSPower(void);
extern void ResetPPSCount(void);

extern U32 GetTimCount(void);
extern U32 GetRtcCount(void);
extern U8 GetTimSaveState(void);
extern void ResetTimSaveState(void);
extern void SetGpsPulseEn(void);
extern void ResetGpsPulseEn(void);
#endif	//__USART_H__




