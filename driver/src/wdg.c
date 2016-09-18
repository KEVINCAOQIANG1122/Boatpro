/*******************************************************************************
  Copyright (C), 2010, 712.Co.,Ltd.
  File name:	power.h
  Author: ������      Version: V1.00       Date: 2012.08.16 
  Description:  
  				ʹ���ڹ�IWDG
				
  Others:         // �������ݵ�˵��
  Function List:  // ��Ҫ�����б�ÿ����¼Ӧ���������������ܼ�Ҫ˵��
  History:        // �޸���ʷ��¼�б�ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸�
                  // �߼��޸����ݼ���  
    1. Date:
       Author:
       Modification:
*******************************************************************************/

#include "stm32f10x.h"
#include "stm32f10x_iwdg.h"
#include "def.h"
#include "wdg.h"

U8 Is_Feed_Dog;
/********************************************* ***************
*  Function:       Initia_wdg								*
*  Description:    ��Դ���Ʋ�����ʼ��								*
*  Input:          ��										*
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void Initia_wdg(void)
{

//    GPIO_InitTypeDef GPIO_InitStructure;
//
//	GPIO_InitStructure.GPIO_Pin = WDG_PIN;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(WDG_PORT, &GPIO_InitStructure);
	
	*(U32 *)DBGMCU_CR |= (u32)1 << 8;//�����ڹ���DEBUG��ֹͣ 
	
	/* Enable write access to IWDG_PR and IWDG_RLR registers */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//ʹ�ܶ�IWDG������
	
	/* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz */
	IWDG_SetPrescaler(IWDG_Prescaler_32);                                       //lq? IWDGʹ��ר�õ�40kHzLSI?    

	/* Set counter reload value */
  	IWDG_SetReload(2500);//2500-2�벻ι���͸�λ ;���ֵ4095����Ӧ3.2768S

	/* Reload IWDG counter */
  	IWDG_ReloadCounter();                                                          
	
	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  	IWDG_Enable();
	
	Is_Feed_Dog = FALSE;
}


/************************************************************
*  Function:       Feed_Dog							    	*
*  Description:    ι��     								*
*  Input:          ��										*
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void Feed_Dog(void)
{
	if(Is_Feed_Dog == TRUE)
	{
		Is_Feed_Dog = FALSE;
		
		/* Reload IWDG counter */
  		IWDG_ReloadCounter();
	}
}
