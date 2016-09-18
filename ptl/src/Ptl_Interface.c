/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: Ptl_Interface.c
  Author: lq       Version : V1.00        Date: 2012.04.24
  Description:     Э��ջ�ӿ�ģ��������������壬��ʼ����ʼ��Э��ջ�ӿ�
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Ptl_Interface.h"
#include "AISconfig.h"
#include "initial.h"

/************************************************************************/
/* Э��ջ���ϲ�ӿ�														*/
/************************************************************************/
GPSStruct	gpsStoreCell;	//GPS���ݽṹ��		��GPS����Ϣ�ɵײ㶨ʱ����
AISStcStr	 aiAISCell;		//��̬�����ṹ��
UCHAR SOS_PressFlg = 0;	//SOS����״̬ TRUE��ʾ����
UCHAR ModeFlag = 0;				//0:4503	1:4501


/************************************************************************/
/* Э��ջ�͵ײ�ӿ�														*/
/************************************************************************/
PTL_MAC_Interface Ptl_Mac_Interface;		//Э��ջ�������ײ�ӿ�
MAC_PTL_Interface Mac_Ptl_Interface;		//�ײ㷢����Э��ջ�ӿ�
MessManaStr mmMessCell;		     //��Ϣ�������	
PTL_TIMER Ptl_Timer;		//Э��ջʹ�õĶ�ʱ�� ÿ1/2250���Ӹ���һ�Σ����ⲿʱ�ӳ������

/************************************************************************/
/* �����ŵ���Ϣ                                                                     */
/************************************************************************/
U8 Channel_4503;			//4503ģʽ�µ���ʼ�����ŵ�ֵ
U8 Channel_4501;			//4501ģʽ�µ���ʼ�����ŵ�ֵ
U8 Channel;

/*******************************************************************************
* ����  : AISStcStructInit
* ����  : ��ʼ��AISStcStr�ṹ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void AISStcStructInit(void)
{
	U8 len_1;
	U8 len_2;
	U8 len_3;

	aiAISCell.UserID = siStaticPara.MMSI;					    //MMSI
	aiAISCell.Dimension_Position[0] = siStaticPara.ShipA;		//��װλ��
	aiAISCell.Dimension_Position[1] = siStaticPara.ShipB;		//��װλ��
	aiAISCell.Dimension_Position[2] = siStaticPara.ShipC;		//��װλ��
	aiAISCell.Dimension_Position[3] = siStaticPara.ShipD;		//��װλ��
#if 0
	len_1 = strlen((char *)siStaticPara.NameofAtoN);
	len_2 = sizeof(aiAISCell.Name);
	len_3 = len_1 > len_2 ? len_2 : len_1;
	strncpy(aiAISCell.Name, (char *)siStaticPara.NameofAtoN, len_3);
#else
	strcpy(aiAISCell.Name, (char *)siStaticPara.NameofAtoN);
#endif

#if 0    
	len_1 = strlen((char *)siStaticPara.VendID);
	len_2 = sizeof(aiAISCell.VenderID);
	len_3 = len_1 > len_2 ? len_2 : len_1;
	strncpy((char*)aiAISCell.VenderID, (char *)siStaticPara.VendID, len_3);
#else
	strcpy((char*)aiAISCell.VenderID, (char *)siStaticPara.VendID);
#endif

	len_1 = strlen("MAYDAY MAYDAY");
	len_2 = sizeof(aiAISCell.saferelatedtext);
	len_3 = len_1 > len_2 ? len_2 : len_1;
	strncpy(aiAISCell.saferelatedtext, "MAYDAY MAYDAY", len_3);

	aiAISCell.SeqNum = siStaticPara.SerialNumber;				//��ͬ�����кŶ�Ӧ�ķ���ʱ϶��ͬ
    aiAISCell.DestMMSI = siStaticPara.DestMMSI;                 // (xxxxxxxxx)
    aiAISCell.AtoNStatus = siStaticPara.AtoNStatus;       	    // (hh)
    aiAISCell.TypeofEPFS = siStaticPara.TypeofEPFS;		        // (x)
    aiAISCell.PositionAccuracy = siStaticPara.PositionAccuracy;	// (x)
    aiAISCell.PowerLevel = siStaticPara.PowerLevel;		        // (x)
    aiAISCell.TypeofAtoN = siStaticPara.TypeofAtoN;		        // (xx)
    aiAISCell.VirtualAtoNFlag = siStaticPara.VirtualAtoNFlag;	// (x)
}

/*******************************************************************************
* ����  : InitPtl_Interface
* ����  : ��ʼ��Э��ջ�ӿ�
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 InitPtl_Interface(void)
{
	Channel_4501 = CHANNEL_A;
	Channel_4503 = CHANNEL_A;
	Channel = CHANNEL_A;
	//ModeFlag = 0;
    
    /*lq AIS��Ϣ�еľ�̬�ֶγ�ʼ��*/
    AISStcStructInit();
 
	mmMessCell.MsgID = 0;					    //��Ϣ��
	mmMessCell.MessState = 0;         			//��Ϣ����״̬
	mmMessCell.TranSlotNum = 0;         			//����ʱ϶��
	mmMessCell.ChannelNum = 0;		  			//�ŵ���־0/1
	mmMessCell.Timer = 0;						//��ʱ��
	mmMessCell.pData = NULL;             			//��������ָ��
	mmMessCell.DataLength = 0;		 			//�������ݳ���
	mmMessCell.GPSvalid = 1;					    //GPS��Ч״̬
	mmMessCell.CurrentOperationFlg = 1;          //��ǰҵ���־

	return 0;
}


