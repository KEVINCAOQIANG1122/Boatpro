/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: Usart.c
  Author: ���       Version : V1.00        Date: 2012.03.12
  Description:    ���ڵײ�������Ӧ�ô������
  Version:         V1.00
  Function List:   // ��Ҫ�������书��    	 
			1����������Initia_POWER_Crl
    	   	   �������ܣ���ʼ������
 
			2����������
    	   	   �������ܣ�����2
    	   	   
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_dac.h"
#include "def.h"
#include "common.h"
#include "power.h"
#include "dac.h"
#include "initial.h"
#include "config.h"

PowerStru	PowerPara;
RFStruct RFPara;

U16 arrRFCtrlVoltVal[RF_DA_CTRL_VOLT_LEVEL_NUM] = {3250, 3380, 3500, 3630, 3750};
                                                 //2.6v  2.7v  2.8v  2.9v  3.0v                                                 
                                                 //{3510, 3620, 3740, 4095, 4095};
                                                 //2.8v  2.9v  3.0v  3.18v 3.18v
                                                 //{3620, 3740, 3860, 4095, 4095};
                                                 //2.9v  3.0v  3.1v  3.18v 3.18v
/************************************************************
*  Function:       InitPowerStru							*
*  Description:    ����״̬��ʼ��						    *
*  Input:          ��										*
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void InitPowerStru(void)
{
	PowerPara.SosKeyFlag = FALSE;
	PowerPara.PowerKeyFlag = FALSE;	
}

/*******************************************************************************
* ����  : RF_RFStructInit
* ����  : RFStruct�ṹ���ʼ��
* ����  : pRFStruct-RFStruct����ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void RF_RFStructInit(RFStruct *pRFStruct)
{
    pRFStruct->RFAntState = TRUE;   
}

/*******************************************************************************
* ����  : RF1_RFInstanceInit
* ����  : RFStruct�ṹ���ʼ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void RF1_RFInstanceInit(void)
{
    RF_RFStructInit(&RFPara);
}

/*******************************************************************************
* ���� ��RF_GetRFAntState
* ���� ����ѯRF����״̬
* ���� ��pRFStruct��RFStruct����ָ��
* ��� ����
* ���� ����
*******************************************************************************/
static U8 RF_GetRFAntState(RFStruct *pRFStruct)
{
    if (pRFStruct->RFAntState == TRUE)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ���� ��RF1_GetRFAntState
* ���� ����ѯRF����״̬
* ���� ����
* ��� ����
* ���� ����
*******************************************************************************/
U8 RF1_GetRFAntState(void)
{
    return RF_GetRFAntState(&RFPara);    
}

#if CONFIG_KEY
/************************************************************
*  Function:       Initia_POWER_Crl						    *
*  Description:    ��Դ���Ʋ�����ʼ����������ʼ��			*
*  Input:          ��										*
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void Initia_POWER_Crl(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
			
	GPIO_InitStructure.GPIO_Pin = POWER_ON_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_PORT, &GPIO_InitStructure);                                      //lq ����ARM����Դ�����ص�ƽ������

	GPIO_InitStructure.GPIO_Pin = POWER_EMERGENCY_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_PORT, &GPIO_InitStructure);                                      //lq ����ARM�����Լ����ص�ƽ������

	/*lq �����ж�����*/
	/* Connect EXTI Line to GPIO Pin*/ 
	GPIO_EXTILineConfig(POWER_SOURCE_PORT, POWER_ON_SOURCE_PIN);
	
	/* Configure EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = POWER_ON_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Connect EXTI Line to GPIO Pin */
	GPIO_EXTILineConfig(POWER_SOURCE_PORT, POWER_EMERGENCY_SOURCE_PIN);
	
	/* Configure EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = POWER_EMERGENCY_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}
#endif

#if CONFIG_PA_GRID_DA_CTRL
/************************************************************
*  Function:       Initia_RFPower							*
*  Description:    ��Ƶ���ſ��Ƴ�ʼ��(ͨ��DA����)			*
*  Input:          ��										*
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void Initia_RFPower(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef  DAC_InitStructure;

    /*lq RF��·LDO */
	GPIO_InitStructure.GPIO_Pin = RF_VCC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RF_VCC_PORT, &GPIO_InitStructure);
    RF_VCC_ON();                                  //lq ʹ��RF LDO
    	
    /*lq ��һ������դѹͨ��I/O����*/
	GPIO_InitStructure.GPIO_Pin = RF_POWER_1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RF_POWER_1_PORT, &GPIO_InitStructure);

    /*lq ����������ͨ��DA����*/
	/* Configure DAC channe1 outputs pins */
	GPIO_InitStructure.GPIO_Pin =  RF_POWER_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(RF_POWER_PORT, &GPIO_InitStructure); 

	/* DAC channel1 Configuration */
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	// DAC_Init(DAC_Channel_2, &DAC_InitStructure);

	/* Enable DAC Channel1 */
	DAC_Cmd(DAC_Channel_1, ENABLE);
	/* Enable DAC Channel2 */
	// DAC_Cmd(DAC_Channel_2, ENABLE);

	RF_PowerDown();
}
#else
/************************************************************
*  Function:       Initia_RFPower							*
*  Description:    ��Ƶ���ſ��Ƴ�ʼ��						*
*  Input:          ��										*
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void Initia_RFPower(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
	//������ԭ��ͼ����
	GPIO_InitStructure.GPIO_Pin = RF_POWER_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RF_POWER_PORT, &GPIO_InitStructure);
	
	RF_POWER_HIGH();                                                            //lq ��ʼ�����Źر�
//	RF_POWER_LOW();
}
#endif

/************************************************************
*  Function:       RF_PowerDown							*
*  Description:    ��Ƶ���Źر�(ͨ��DA����)			*
*  Input:          ��										*
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void RF_PowerDown(void)
{
    /*lq 2��3�����ſ���*/
    SetDAC1Val(0);

    /*lq 1�����ſ���*/
    RF_POWER_1_LOW(); 

//    usart_printf("RFCtrlVoltVal : %4d", 0);
}

/************************************************************
*  Function:       RF_PowerOn							*
*  Description:    ��Ƶ���ſ���(ͨ��DA����)			*
*  Input:          ��										*
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void RF_PowerOn(void)
{
    /*lq 1�����ſ���*/
    RF_POWER_1_HIGH(); 
 
    /*lq 2��3�����ſ���*/
#if CONFIG_BAT_MONITOR 
    if (BS1_GetCurVoltValue() >= RF_DA_CTRL_VOLT_LEVEL_3)
    {
        SetDAC1Val(arrRFCtrlVoltVal[0] + eqEquipPara.GridVoltOffset);

//        usart_printf("RFCtrlVoltVal : %4d", arrRFCtrlVoltVal[0]);
    }
    else if (BS1_GetCurVoltValue() >= RF_DA_CTRL_VOLT_LEVEL_2)
    {
        SetDAC1Val(arrRFCtrlVoltVal[1] + eqEquipPara.GridVoltOffset);

//        usart_printf("RFCtrlVoltVal : %4d", arrRFCtrlVoltVal[1]);
    }
    else if (BS1_GetCurVoltValue() >= RF_DA_CTRL_VOLT_LEVEL_1)
    {
        SetDAC1Val(arrRFCtrlVoltVal[2] + eqEquipPara.GridVoltOffset);

//        usart_printf("RFCtrlVoltVal : %4d", arrRFCtrlVoltVal[2]);
    }
    else if (BS1_GetCurVoltValue() >= RF_DA_CTRL_VOLT_LEVEL_0)
    {
        SetDAC1Val(arrRFCtrlVoltVal[3] + eqEquipPara.GridVoltOffset);

//        usart_printf("RFCtrlVoltVal : %4d", arrRFCtrlVoltVal[3]);
    }
    else
    {
        SetDAC1Val(arrRFCtrlVoltVal[3] + eqEquipPara.GridVoltOffset);

//        usart_printf("RFCtrlVoltVal : %4d", arrRFCtrlVoltVal[3]);
    }
#else
    SetDAC1Val(4095);
#endif
}

/*******************************************************************************
* ����  : Exti3_Int
* ����  : �ⲿ�жϷ������(Power����)
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void Exti15_10_Int_PowerKey(void)
{
	if(EXTI_GetITStatus(POWER_ON_LINE) != RESET)
	{		 
		/* Clear the Key Button EXTI line pending bit */  
		EXTI_ClearITPendingBit(POWER_ON_LINE);//�½��ش����ж�
		
		PowerPara.PowerKeyFlag = TRUE; 							
	}
}

/*******************************************************************************
* ����  : Exti4_Int
* ����  : �ⲿ�жϷ������(SOS����)
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void Exti15_10_Int_SosKey(void)
{
	if(EXTI_GetITStatus(POWER_EMERGENCY_LINE) != RESET)
	{ 
		/* Clear the Key Button EXTI line pending bit */  
		EXTI_ClearITPendingBit(POWER_EMERGENCY_LINE);//�½��ش����ж�

		PowerPara.SosKeyFlag = TRUE;
	}
}

/*******************************************************************************
* ����  : GetSosLowPowerFlag
* ����  : ���SOS������������״̬��һ����Ч
* ����  : ��
* ���  : ��
* ����  : SOS״̬
*******************************************************************************/
U8 GetSosLowPowerSta(void)
{
	if (PowerPara.SosKeyFlag == TRUE)
	{
		PowerPara.SosKeyFlag = FALSE;

		return (TRUE);
	}		

	return (FALSE);
}

/*******************************************************************************
* ����  : GetPowerLowPowerSta
* ����  : ���Power������������״̬��һ����Ч
* ����  : ��
* ���  : ��
* ����  : Power״̬
*******************************************************************************/
U8 GetPowerLowPowerSta(void)
{
	if (PowerPara.PowerKeyFlag == TRUE)
	{
		PowerPara.PowerKeyFlag = FALSE;

	    return (TRUE);
	}		

	return (FALSE);
}

/************************************************************
*  Function:       InitPowerCtrl							*
*  Description:    ��Դ���Ʋ�����ʼ��						*
*  Input:          ��										*
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void InitPowerCtrl(void)
{
  #if  0    // mask by Nathan  , because  it is no use actuallly
	GPIO_InitTypeDef GPIO_InitStructure;
    	
    /*lq ��Դ���ؿ�����������*/
	GPIO_InitStructure.GPIO_Pin = POWER_CRL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(POWER_PORT, &GPIO_InitStructure);                                      //lq ����ARM���Ƶ�ԴD��������D

	GPIO_InitStructure.GPIO_Pin = POWER_CLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(POWER_PORT, &GPIO_InitStructure);                                      //lq ����ARM���Ƶ�ԴD��������CLK
  #endif
  
#if 0
    /*lq ��ʼ����Դ����������Ч��ʹ��D������ʱ��Ӧ������������
    **  ����ÿ�δ�standby���Ѻ��������û�ʹϵͳ���磩
    */
    POWER_CRL_LOW();
    POWER_CLK_LOW();
    Delay_ms(10);
    POWER_CLK_HIGH();
#endif
}
