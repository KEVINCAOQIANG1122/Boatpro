/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: MsgCtrl.c
  Author: lq       Version : V1.00        Date: 2014.05.29
  Description:     ��Ϣ����
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "def.h"
#include "MsgCtrl.h"
#include "LinkManagementEntity.h"
#include "AisDef.h"
#include "AISDigitalInterface.h"

/*******************************************************************************
* ����  : MSG_MsgProcess
* ����  : ������յ���Ϣ
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void MSG_MsgProcess(void)
{
    BinMsgRxCellStruct binMsgRxCell;        
    U8 channel;

    /*lq ���ջ�1 */
    if (LME_Inst1BinMsgRxFifoDeque(&binMsgRxCell) == TRUE)
    {
        if (binMsgRxCell.RxChannel == ENUM_AIS_CHANNEL_A)
        {
            channel = AIS_CHANNEL_CHAR_A;
        }
        else
        {
            channel = AIS_CHANNEL_CHAR_B;        
        }
        AISDI_BuildVDM(AIS_DI_TALKER_ID_DEFAULT, binMsgRxCell.Data,
            binMsgRxCell.DataLen, channel);
    }

    /*lq ���ջ�2 */    
    if (LME_Inst2BinMsgRxFifoDeque(&binMsgRxCell) == TRUE)
    {
        if (binMsgRxCell.RxChannel == ENUM_AIS_CHANNEL_A)
        {
            channel = AIS_CHANNEL_CHAR_A;
        }
        else
        {
            channel = AIS_CHANNEL_CHAR_B;        
        }
        AISDI_BuildVDM(AIS_DI_TALKER_ID_DEFAULT, binMsgRxCell.Data,
            binMsgRxCell.DataLen, channel);
    }
}
