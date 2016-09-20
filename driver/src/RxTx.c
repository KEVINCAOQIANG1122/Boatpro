/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: RxTx.c
  Author: lq       Version : V1.00        Date: 2014.05.06
  Description:     �շ�����
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include "Include.h"

/*******************************************************************************
* ����  : RxTxSwitch
* ����  : �շ�ת��
* ����  : SwitchType-ת������		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void RxTxSwitch(U8 SwitchType)
{
    if (SwitchType == RXTX_SWITCH_TYPE_TX_TO_RX)
    {
        RX_EN_ON();
        ANT_SW_RX();    
    }
    else
    {
        RX_EN_OFF();
        ANT_SW_TX();            
    }        
}

/*******************************************************************************
* ����  : InitRxTx
* ����  : �շ���ʼ��
* ����  : ��        	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitRxTx(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = RX_EN_PIN;
	GPIO_Init(RX_EN_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ANT_SW_PIN;
	GPIO_Init(ANT_SW_PORT, &GPIO_InitStructure);
     
    RxTxSwitch(RXTX_SWITCH_TYPE_TX_TO_RX);        
}
