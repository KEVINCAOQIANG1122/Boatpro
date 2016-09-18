/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: Usart.c
  Author: ���       Version : V1.00        Date: 2012.03.12
  Description:    ���ڵײ�������Ӧ�ô������
  Version:         V1.00
  Function List:   // ��Ҫ�������书��    	 
			1����������InitUsart2
    	   	   �������ܣ���ʼ������2
 
			2����������Uart2Interrupt
    	   	   �������ܣ�����2
    	   	   
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "def.h"
#include "usart.h"
#include "gps.h"
#include "Tim.h"
#include "common.h"
#include "LED.h"
#include "gpsctrl.h"
#include "rtc.h"
#include "initial.h"
#include "config.h"
#include "Include.h"

/* Private variables ---------------------------------------------------------*/
struct UartTxFIFOStruct Uart2TxFIFO; //���ͻ���
struct UartRxFIFOStruct Uart2RxFIFO;

//lq U16 gps_pps_count = 0;//GPS����������������������λ������ϵ����ĸ���
U32 GpsTimCountSave;
U32	GpsRtcCountSave;
U32 TimCntSave = 0;
U32	RtcCntSave = 0;

U8	GpsTimSaveFlag = FALSE;
/* Private  function  ---------------------------------------------------------*/
void InitUsart2(U32 baud);

/*******************************************************************************
* ����  : GetTimCount
* ����  : ����TIM��ʱ���������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
U32 GetTimCount(void)
{
	return (GpsTimCountSave);
}

/*******************************************************************************
* ����  : GetRtcCount
* ����  : ����RTC��ʱ���������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
U32 GetRtcCount(void)
{
	return (GpsRtcCountSave);
}

/*******************************************************************************
* ����  : GetTimCnt
* ����  : ����Tim��ʱ���������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
U32 GetTimCnt(void)
{
	return (TimCntSave);
}

/*******************************************************************************
* ����  : GetRtcCnt
* ����  : ����RTC��ʱ���������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
U32 GetRtcCnt(void)
{
	return (RtcCntSave);
}

/*******************************************************************************
* ����  : GetTimSaveState
* ����  : �鿴�Ƿ�GPS�����嵽��������ʱ���������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetTimSaveState(void)
{
	if (GpsTimSaveFlag == TRUE)
	{
		GpsTimSaveFlag = FALSE;
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}

/*******************************************************************************
* ����  : ResetTimSaveState
* ����  : ��λGPS�����־λ
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void ResetTimSaveState(void)
{
	GpsTimSaveFlag = FALSE;
}

/*******************************************************************************
* ����  : Usart2Interrupt
* ����  : ����2�жϷ������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void Usart2Interrupt(void)
{
	U8 Data;
	
	//�������������������Ҫ�ȶ�SR���ٶ�DR�Ĵ�������������������жϵ�����
	if(USART_GetFlagStatus(USART2,USART_FLAG_ORE) == SET)
	{
	 	USART_ClearFlag(USART2,USART_FLAG_ORE);			// ��SR��ʵ����������־
		USART_ReceiveData(USART2);						// ��DR
	}

	//�����ж�
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		/* Read one byte from the receive data register */
		Data = USART_ReceiveData(USART2);
		/* ֧��3252��������͸�� */
		//HPI_WriteByte(Data);

        /*lq ������������ģʽ���Ҵ��ڵ͹���ģʽ�����GPS����*/
        if (!((DevDynamicPara.ucWorkMode == EQ_WORKMOD_NORMAL)
            && (DevDynamicPara.ucLowFreqSta == EQ_FREQSTA_LOW)))
        {
            /*lq NMEA protocol ��Ϣ���� */
		    UartRxProcess(&Uart2RxFIFO, Data);
#if CONFIG_GPS_UBX_EN            
            /*lq UBX protocol ��Ϣ���� */
            UBX_UartRxProcess(COMM2, Data); 
#endif                   
        }
		//USART_SendData(USART1, Data);
	}
		
	//�����ж�
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{   
		UartTxIntProcess(&Uart2TxFIFO);
	}
}

/*******************************************************************************
* ����  : Exti1_Int
* ����  : �ⲿ�жϷ������ (GPS�ж�)
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GpsPulseEn = FALSE;      //lq ����GPS�ϵ�͵���ʱ���������
void Exti1_Int(void)
{
	if(EXTI_GetITStatus(PULSE_EN_LINE) != RESET)
	{
        GpsSelfCheckInst.TimepulseFlg = TRUE;   //lq ���жϴ�������Ϊ����������
        
		if (IsGpsFix() == TRUE && GpsPulseEn == TRUE)                           //lq ����������GPS���ģ�����Ҫ���Ե�һ��������
        {                                                                       //lq ��������£�ֻ�ж�λ�ɹ���GPSģ��Żᴥ��������
			if (IsGpsPowerupProtect() == TRUE)                                  //lq Tim��RTCʱ��ƫ��������GPS���������ڼ����
			{                                                                   //lq Tim��RTCʱ��ƫ���������Զ�Ӧ��ÿ��ʱ���ж��н���
				/*lq ����ͳ��GPS��׼1s�ڱ���Tim��RTC������ֵ��ʵ�ʼ���ֵ��ƫ��*/
                GpsTimCountSave = GpsTimCount;                                  //lq ����Timʱ��ƫ���У׼
				GpsRtcCountSave = GpsRtcCount;//����RTC����ֵ                   //lq ����RTCʱ��ƫ���У׼
				GpsTimSaveFlag = TRUE;

                /*lq ����ͳ��GPS��׼1s�ڱ���Tim��RTC��ʵ�ʼ���ֵ*/
                TimCntSave = TimCnt;
                RtcCntSave = RtcCnt;
			}
			
            /*lq GPSͬ��ƫ�����ʹ��*/
            GpsSynCell.GPSSyn_PulseEn = TRUE;			//����ʹ��
			GpsSynCell.GPSSyn_ProtectCnt = 0;			//�屣��ʱ��	

            /*lq ��¼�����崥��ʱ�̱���ʱ϶��λ����ֵ*/
			GpsSynCell.GPSSyn_PulsePhase = gsiPhaseCnt;	//���������崥��ʱ��λ  //lq ������λ����,������GPSͬ��
			GpsSynCell.GPSSyn_PulseSlot = gsiSlotCnt;
			GpsSynCell.GPSSyn_PulseMin = gslFrameCnt;

            /*lq �������Ӧ��GPS��׼��*/
#if CONFIG_GPS_SYNCH_LEAP
            GpsSynCell.ChkTimeS = GpsPara.GPS_TimeSLeap + 1;                    //lq ���������壬����GPS����
#else
            GpsSynCell.ChkTimeS = GpsPara.GPS_TimeS + 1;                        //lq ���������壬����GPS����
#endif

#if CONFIG_GPS_SYNCH_HOUR_MIN
            GpsSynCell.ChkTimeM = GpsPara.GPS_TimeM;
            GpsSynCell.ChkTimeH = GpsPara.GPS_TimeH;
#endif		
            /*lq GPS PPS���ɱ�־ */
            GpsPara.GPS_PPSGenFlag = TRUE;
        }

		/* Clear the Key Button EXTI line pending bit */  
		EXTI_ClearITPendingBit(PULSE_EN_LINE);	                                //lq �����ش����ж�						
	}
}

/************************************************************
*  Function:       InitGPSPower								*
*  Description:    ������ʼ��								*
*  Input:          ��										*
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void InitGPSPower(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPS_PORT, &GPIO_InitStructure);
	
	//�ø�
	//lq GPSPOWER_HIGH(); //lq ��ֹÿ�δ������Ѻ���GPS
}

/*******************************************************************************
* ����  : EXTI_Configuration
* ����  : �����ⲿ�жϺ�RTC Alarm�ж�
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void EXTI_Configuration(void)
{	
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = PULSE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PULSE_PORT, &GPIO_InitStructure);

	/* Connect EXTI Line to GPIO Pin */
	GPIO_EXTILineConfig(PULSE_EN_SOURCE_PORT, PULSE_EN_SOURCE_PIN);
	
	/* Configure EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = PULSE_EN_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//�½��ش���������Ҫд�����ز���Ч����������//lq �������ش�������û����
	
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

/*******************************************************************************
* ����  : InitUsart2
* ����  : ��ʼ������2
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitUsart2(U32 baud) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* Configure USART1 Rx (PA.3) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART1 Tx (PA.2) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART1 configuration ------------------------------------------------------*/
	/* USART1 configured as follow:
		- BaudRate = 38400 baud  
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USART2 */
  	USART_Init(USART2, &USART_InitStructure);

	/* Enable USART1 Receive and Transmit interrupts */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);	

	/* Enable the USART1 */
  	USART_Cmd(USART2, ENABLE);  

	InitGPSPower(); 

	EXTI_Configuration();                                                       //lq �����ⲿ�ж�-GPS������
}

/*******************************************************************************
* ����  : SetGpsPulseEn
* ����  : ʹ��GPS�����壬����GPS�ϵ�͵���ʱ��GPS������ĸ���
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetGpsPulseEn(void)
{
    GpsPulseEn = TRUE;
}

/*******************************************************************************
* ����  : ResetGpsPulseEn
* ����  : ʧ��GPS�����壬����GPS�ϵ�͵���ʱ��GPS������ĸ���
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void ResetGpsPulseEn(void)
{
    GpsPulseEn = FALSE;
}
#if 0
/*******************************************************************************
* ����  : ResetPPSCount
* ����  : ��λ���������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void ResetPPSCount(void)
{
	gps_pps_count = 0;
}	
#endif

#if 0
/*******************************************************************************
* ���� ��GPS_UartTxString
* ���� ��GPS���ڷ����ַ���
* ���� ��COMM�����ڵ�ַ��־
			COMM2��
		 pStr���ַ���ָ��
		 Length���ַ�������
* ��� ����
* ���� ����
*******************************************************************************/
U8 GPS_UartTxString(char *pStr)
{				
	UartTxDataBuf(COMM2, (U8*)pStr, strlen(pStr));

	return TRUE;		
}
#endif
