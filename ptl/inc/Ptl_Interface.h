/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: Ptl_Interface.h
  Author: lq       Version : V1.00        Date: 2012.04.24
  Description:     Э��ջ�ӿ�ģ��ꡢ�ṹ�嶨�壬��������������
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/

#ifndef PTL_INTERFACE_H
#define PTL_INTERFACE_H

#include "gentype.h"
#include "gpsctrl.h"
#include "initial.h"

//�ṹ�嶨��
/*****************************************************************************
*     		 GPS struct   		 *
******************************************************************************/
/*
typedef struct {
	UCHAR GPSTimeBuf[6];
	CHAR GPSDateBuf[6];				//20110926 lg
	U16  GPS_PosFix;					//GPS��λ״̬
	U16 GPS_TimeY;
	U16 GPS_TimeMon;
	U16 GPS_TimeD;
	U16 GPS_TimeH;
	U16 GPS_TimeM;
	U16 GPS_TimeS;
	S16 GPS_LatiD;
	S32 GPS_Latitude;
	S32 GPS_Lati100;
	S32 GPS_Longitud;
	S32 GPS_Long100;
	U32 GPS_Sog;
	U32 GPS_Sog_Buf[18];
	U16 GPS_Sog_Index;
	U32 GPS_Cog;
	U16 GPS_State;
	U16 GPS_errlat;
	U16 GPS_errlon;
	U16 GPS_PosPrec;
}GPSStruct;
*/

/*****************************************************************************
*     		 AISStc struct   		 *
******************************************************************************/

/**
	@ingroup Э��ջ 
	@brief Э��ջ�洢��̬��Ϣ�Ľṹ��
*/
typedef struct {
	ULINT  UserID;					    //MMSI
    USINT Dimension_Position[4];		//��װλ��
	char Name[EQ_ATON_NAME_LEN + 1];    //AtoN��,����\0��
	UCHAR VenderID[EQ_VEND_ID_LEN + 1];	//����ID,����\0��
	// char CallSign[7];			    //����
	char saferelatedtext[21];			//��ȫ��Ϣ                            //lq �޶�14�Ű�ȫ��Ϣ�İ�ȫ������ݵ���󳤶�Ϊ21Bytes����������Ϣ����������40bits��
	USINT SeqNum;                       //���к�

    U32 DestMMSI;           // (xxxxxxxxx)
    U8  AtoNStatus;       	// (hh)
    U8	TypeofEPFS;		    // (x)
    U8	PositionAccuracy;	// (x)
    U8	PowerLevel;		    // (x)
    U8  TypeofAtoN;		    // (xx)
    U8	VirtualAtoNFlag;	// (x)

} AISStcStr;
/*****************************************************************************
*     		MessMana struct   		 *
******************************************************************************/

/**
	@ingroup ���ݴ���
	@brief Э��ջ�����Ϣ�Ľṹ�壬������Ϣ�Ͷ�Ӧ�ŵ���
*/
typedef struct {
	U16 MsgID;					    //��Ϣ��	lg added 20110614
	U16 FouceSendFlg;				    //ǿ�Ʒ��ͱ�־  lg added 20110617
	U16 MessState;         			//��Ϣ����״̬                              //lq ��ʶ����Ϣ�Ƿ��ѷ���Э��ջ��TRUE-���Է��룬����δ����;FALSE-�����Է��룩
	S16 TranSlotNum;         			//����ʱ϶��                            //lq ��Ϣ����ʱ϶��
	U8  ChannelNum;		  			//�ŵ���־0/1
	U16 Timer;						//��ʱ��
	U8 *pData;             			//��������ָ��
	U16 DataLength;		 			//�������ݳ���                              //lq ��λΪByte
	U16 GPSvalid;					    //GPS��Ч״̬
	U16 CurrentOperationFlg;          //��ǰҵ���־
} MessManaStr;

/*****************************************************************************
*     		Interface struct   		 *
******************************************************************************/
typedef struct {
	USINT MsgID;					    //��Ϣ��
	USINT MessState;         			//��Ϣ����״̬
	USINT TranSlotNum;         			//����ʱ϶��
	USINT ChannelNum;		  			//�ŵ���־0/1
	USINT Timer;						//��ʱ��
	USINT pData[100];             			//��������ָ��
	USINT DataLength;		 			//�������ݳ���
	USINT GPSvalid;					    //GPS��Ч״̬
	USINT CurrentOperationFlg;          //��ǰҵ���־
} PTL_MAC_Interface;

typedef struct {
	USINT WakeUpFlag;		//���߻��ѱ�־(���Ѻ���1)
	USINT ReportFlag;		//��Ϣ���ͳɹ����ķ���������Ч�Ա�־
	USINT ReportType;		//��Ϣ���ͳɹ����ķ�����������
} MAC_PTL_Interface;

typedef struct {
	USINT Slot;
	USINT Min;
} PTL_TIMER;


extern PTL_TIMER Ptl_Timer;

extern GPSStruct	gpsStoreCell;	//GPS���ݽṹ��
extern AISStcStr	 aiAISCell;		//��̬�����ṹ��
extern UCHAR SOS_PressFlg;	//SOS����״̬ TRUE��ʾ����
extern UCHAR ModeFlag;				//0:4503	1:4501

extern PTL_MAC_Interface Ptl_Mac_Interface;		//Э��ջ�������ײ�ӿ�
extern MAC_PTL_Interface Mac_Ptl_Interface;		//�ײ㷢����Э��ջ�ӿ�
extern MessManaStr mmMessCell;		     //��Ϣ�������	

extern U8 Channel_4503;			//4503ģʽ�µ���ʼ�����ŵ�ֵ
extern U8 Channel_4501;			//4501ģʽ�µ���ʼ�����ŵ�ֵ
extern U8 Channel;
#define SENDMSG_SPACE 4		//��ǰ���͸��ײ��ʱ϶ֵ

USINT InitPtl_Interface(void);
USINT Timer_Run(void);

#endif
