/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: Dac.c
  Author: vozhj      Version : V1.00        Date: 2011.10.18
  Description:    ��ģת��������
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>

*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_dac.h"
#include "def.h"
#include "Dac.h"
#include "Include.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/   

/*******************************************************************************
* ����  : Dac_Init
* ����  : DAC�Ĵ�����ʼ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void DAC_RegInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef  DAC_InitStructure;
	
	/* Configure DAC channe1 and DAC channel2 outputs pins */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

#if 1

	/* DAC channel1 Configuration */
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
//	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);

	/* Enable DAC Channel1 */
//	DAC_Cmd(DAC_Channel_1, ENABLE);
	/* Enable DAC Channel2 */
	DAC_Cmd(DAC_Channel_2, ENABLE);

#else

	//λ��������DAC
	DAC->CR = (u32)1<<17;		//OUTPUT BUFFER DISABLE,
	DAC->CR |= (U32)1<<16;		//enable dac channel2

#endif

//	SetDAC1Val(0);
	SetDAC2Val(2047);
}

#if 1
/*******************************************************************************
* ����  : SetDAC1Val
* ����  : ����DACͨ��1����Ӧ���书��
* ����  : siDACVal	DACͨ��1����ֵ
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetDAC1Val(S16 siDACVal)
{
	if (siDACVal > 4095)
	{
		siDACVal = 4095;
	}
	else if (siDACVal < 0)
	{
		siDACVal = 0;
	}
	DAC_SetChannel1Data(DAC_Align_12b_R, siDACVal);
}
#endif
/*******************************************************************************
* ����  : SetDAC2Val
* ����  : ����DACͨ��2����Ӧ�������
* ����  : siDACVal	DACͨ��2����ֵ
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetDAC2Val(S16 siDACVal)
{
	if (siDACVal > 4095)
	{
		siDACVal = 4095;
	}
	else if (siDACVal < 0)
	{
		siDACVal = 0;
	}
	DAC_SetChannel2Data(DAC_Align_12b_R, siDACVal);                             //lq 12λ�����Ҷ���
}
