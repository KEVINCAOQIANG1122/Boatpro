/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: RxTxCtrl.c
  Author: lq       Version : V1.00        Date: 2014.05.06
  Description:     �շ�����
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "stm32f10x.h"
#include "def.h"
#include "RxTxCtrl.h"
#include "RxTx.h"

/*******************************************************************************
* ����  : RxTxCtrl
* ����  : �շ�����
* ����  : CtrlType-��������	
          Freq1-RDA1846_1Ƶ������
          Freq2-RDA1846_2Ƶ������          	  
* ���  : ��
* ����  : ��
*  Modify by  lnw    , add    lowpower  Process
*******************************************************************************/
void RxTxCtrl(U8 CtrlType, U32 Freq1, U32 Freq2)
{
    if (CtrlType == RXTX_CTRL_TYPE_TX)
    {
        RxTxSwitch(RXTX_SWITCH_TYPE_RX_TO_TX);                           
    }
    else if (CtrlType == RXTX_CTRL_TYPE_RX)
    {
      #if  CONFIG_PTT_EN
         if(PTT_OBJ.LowPowerMode_Staus==DISABLE)
	  #endif           
           RxTxSwitch(RXTX_SWITCH_TYPE_TX_TO_RX);
    }     
}

