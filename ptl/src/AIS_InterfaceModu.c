/******************************************************************
Copyright(c) 2010, TJU-TCB
All right reserved
File name:		AIS_InterfaceModu.c
Description:	Protocol related Interface process
Others:			void
Author:			WT
Version:		1.0
Date:			2010.7.21
Function List:  
*****************************************************************************
*  History:               				                                    *
*      <author>   <time>    <version >     <desc>                           *
*         WT     10/07/21     1.0       build this moudle                   *
****************************************************************************/
#if 0
#include <stdio.h>
#include <stdlib.h>
#include "AIS_InterfaceModu.h"
//#include "AIS_TimeModu.h"
#include "Ptl_Interface.h"
#include "AISconfig.h"
#include "LinkPtlPort.h"
#include "AIS_MsgModu.h"
#include "AISMsgModule.h"

//ȫ�ֱ�������
U8 G_Msg1[MSG_1_MAX_LEN + 1];
U8 G_Msg6IFM0[MSG_6_MAX_LEN + 1];                                                   //lq ��Ϣ6 
U8 G_Msg14[MSG_14_MAX_LEN + 1];	         	//��Ϣ14�洢λ��
U8 G_Msg18[MSG_18_MAX_LEN + 1];	         	//��Ϣ18ģ�棬��ʼ�������ձ�ģ������18����Ϣ
U8 G_Msg19[MSG_19_MAX_LEN + 1];	         	//��Ϣ19ģ�棬��ʼ�������ձ�ģ������19����Ϣ
U8 G_Msg21[MSG_21_MAX_LEN + 1];	         	//��Ϣ21ģ�棬��ʼ�������ձ�ģ������21����Ϣ

U8 Msg6AppDataText[MSG6_IFM0_TEXT_LEN] = "ATON_MSG6!";     //lq 6-bit ASCII����Сд�ַ�

U16   AISMin = 0; 
U16   AISSlot = 0;

U32   Send18Time = 0;		     //���η���18����Ϣ��ʱ��
U32   Send19Time = 0;		     //���η���19����Ϣ��ʱ��
U32   Send14Time = 0;			 //���η���14����Ϣ��ʱ��

U16 	SendSlotSelectedFlag = 0;//wt ѡȡ����Ϣ����ʱ϶��ǣ�1��ʾ�Ѿ�ѡȡ����
U16 	Msg14NeedToSend;			 //����Ϣ14��Ҫ����
	
extern U8 Channel;

U16 Send14Flag = 0;//lg 1��ʾ��Ҫ������Ϣ14��0��ʾ����Ҫ������Ϣ14

U32   SendInterval = 0x6000;	 //4503��Ϣ���ͼ��ָʾ		6����
U32   SendInterval_4501_dyn_low = 0x3000;	 //4501��̬��Ϣ���ٷ��ͼ��ָʾ	3����
U32   SendInterval_4501_dyn_high = 0x465;	 //4501��̬��Ϣ���ٷ��ͼ��ָʾ	30�� 1125ʱ϶
U32   SendInterval_4501_sta = 0x6000;	 //4501��̬̬��Ϣ���ͼ��ָʾ	6����

/******************************************
������InitInterfacePara
����: ��ʼ����Ϣ����ṹ�����
����:
���:
����ֵ��
��ע��
*******************************************/
U16 InitInterfacePara(void)
{
    mmMessCell.MsgID = 18;
    mmMessCell.FouceSendFlg = 0;
	mmMessCell.ChannelNum = Channel_4503;
	mmMessCell.DataLength = 0;
	mmMessCell.GPSvalid = 0;
	mmMessCell.MessState = 0;
	mmMessCell.pData = NULL;
	mmMessCell.Timer = 0;
	mmMessCell.TranSlotNum = 0;
	mmMessCell.CurrentOperationFlg = 1;	//�ϵ��ʼ��ҵ��

	return 0;
}


/*******************************************************************************
* ����  : Change4501Channel
* ����  : 4501ģʽ�л��ŵ�
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 Change4501Channel(void)
{
	if (Channel_4501 == CHANNEL_A)
	{
		Channel_4501 = CHANNEL_B;
	} 
	else if (Channel_4501 == CHANNEL_B)
	{
		Channel_4501 = CHANNEL_A;
	}
	return 0;
}

/*******************************************************************************
* ����  : ChangeChannel
* ����  : �л��ŵ�
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 ChangeChannel(void)
{
	if (Channel == CHANNEL_A)
	{
		Channel = CHANNEL_B;
	} 
	else if (Channel == CHANNEL_B)
	{
		Channel = CHANNEL_A;
	}
	return 0;
}


/*******************************************************************************
* ����  : SendMsg19_SO
* ����  : ������Ϣ19 
* ����  : ��	  		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 SendMsg6(void)
{
    InitMsg6();

	mmMessCell.pData = G_Msg6IFM0 + 1;

	mmMessCell.MsgID = 6;
	mmMessCell.FouceSendFlg = 0;

    if (GetBurstFlg() == TRUE)
    {
        mmMessCell.TranSlotNum = (GetBurstTxTimSlot() + 2) % 2250;  //lq 6����Ϣ��21����Ϣ�ĺ�����ʱ϶����  
    }
    else
    {
        mmMessCell.TranSlotNum = (GetMsgTxTimSlot() + 2) % 2250;    
    }
    	
	mmMessCell.ChannelNum = GetTxChannel();
	mmMessCell.DataLength = G_Msg6IFM0[0];
	mmMessCell.MessState = 1; 
	
	LinkPtlPort(&mmMessCell);

	return 0;
}

#if 0
/*******************************************************************************
* ����  : SendMsg14_SO
* ����  : ������Ϣ14 
* ����  : ucBurstFlg - �Ƿ������ڵ�һ����Ϣ��
          ucTestFlg - �Ƿ��ǲ���̬����	  		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 SendMsg14_SO(U8 ucBurstFlg, U8 ucTestFlg)
{
	InitMsg14(ucTestFlg);

	mmMessCell.pData = G_Msg14 + 1;

	mmMessCell.MsgID = 14;
	mmMessCell.FouceSendFlg = 0;

    if (GetBurstFlg() == TRUE)
    {
        mmMessCell.TranSlotNum = GetBurstTxTimSlot();    
    }
    else
    {
        mmMessCell.TranSlotNum = GetMsgTxTimSlot();    
    }
	
	mmMessCell.ChannelNum = GetTxChannel();
	mmMessCell.DataLength = G_Msg14[0];
	mmMessCell.MessState = 1; 
	
	LinkPtlPort(&mmMessCell);

	return 0;
}
#endif

/*******************************************************************************
* ����  : SendMsg14
* ����  : ������Ϣ14 
* ����  : ��  		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 SendMsg14(void)
{
	InitMsg14();

	mmMessCell.pData = G_Msg14 + 1;

	mmMessCell.MsgID = 14;
	mmMessCell.FouceSendFlg = 0;

    if (GetBurstFlg() == TRUE)
    {
        mmMessCell.TranSlotNum = (GetBurstTxTimSlot() + 4) % 2250;  //lq 14����Ϣ��21����Ϣ�ĺ�4��ʱ϶����    
    }
    else
    {
        mmMessCell.TranSlotNum = (GetMsgTxTimSlot() + 4) % 2250;    
    }
	
	mmMessCell.ChannelNum = GetTxChannel();
	mmMessCell.DataLength = G_Msg14[0];
	mmMessCell.MessState = 1; 
	
	LinkPtlPort(&mmMessCell);

	return 0;
}

/*******************************************************************************
* ����  : SendMsg18
* ����  : ������Ϣ18 
* ����  : burstFlg - �Ƿ������ڵ�һ����Ϣ��  		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 SendMsg18(U8 burstFlg)
{
    InitMsg18();

	mmMessCell.pData = AISMsgBinBuff_18 + 1;

	mmMessCell.MsgID = 18;
	mmMessCell.FouceSendFlg = 0;

    if (GetBurstFlg() == TRUE)
    {
        mmMessCell.TranSlotNum = GetBurstTxTimSlot();  //lq 18����Ϣ  
    }
    else
    {
        mmMessCell.TranSlotNum = GetMsgTxTimSlot();    
    }
    	
	mmMessCell.ChannelNum = GetTxChannel();
	mmMessCell.DataLength = AISMsgBinBuff_18[0];
	mmMessCell.MessState = 1; 
	
	LinkPtlPort(&mmMessCell);

	return 0;
}

/*******************************************************************************
* ����  : SendMsg19
* ����  : ������Ϣ19 
* ����  : burstFlg - �Ƿ������ڵ�һ����Ϣ��
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 SendMsg19(U8 burstFlg)
{
    InitMsg19();

	mmMessCell.pData = AISMsgBinBuff_19 + 1;

	mmMessCell.MsgID = 19;
	mmMessCell.FouceSendFlg = 0;

    if (GetBurstFlg() == TRUE)
    {
        mmMessCell.TranSlotNum = (GetBurstTxTimSlot() + 1) % 2250;  //lq 19����Ϣ��18����Ϣ����һʱ϶����  
    }
    else
    {
        mmMessCell.TranSlotNum = (GetMsgTxTimSlot() + 1) % 2250;    
    }
    	
	mmMessCell.ChannelNum = GetTxChannel();
	mmMessCell.DataLength = AISMsgBinBuff_19[0];
	mmMessCell.MessState = 1; 
	
	LinkPtlPort(&mmMessCell);

	return 0;
}

/*******************************************************************************
* ����  : SendMsg21
* ����  : ������Ϣ21
* ����  : �� 		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 SendMsg21(void)
{
    InitMsg21();

	mmMessCell.pData = G_Msg21 + 1;

	mmMessCell.MsgID = 21;
	mmMessCell.FouceSendFlg = 0;

    if (GetBurstFlg() == TRUE)
    {
        mmMessCell.TranSlotNum = GetBurstTxTimSlot();  //lq 18����Ϣ  
    }
    else
    {
        mmMessCell.TranSlotNum = GetMsgTxTimSlot();    
    }
    	
	mmMessCell.ChannelNum = GetTxChannel();
	mmMessCell.DataLength = G_Msg21[0];
	mmMessCell.MessState = 1; 
	
	LinkPtlPort(&mmMessCell);

	return 0;
}
#endif
