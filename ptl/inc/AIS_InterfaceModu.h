/******************************************************************
Copyright(c) 2010, TJU-TCB
All right reserved
File name:		AIS_InterfaceModu.h
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
#ifndef AIS_INTERFACEMODU
#define AIS_INTERFACEMODU
#include "def.h"
#include "gentype.h"

#define MSG_6_MAX_LEN   115                                                     //lq ��λbyte ��920bits��
#define MSG_14_MAX_LEN  126                    //lq ��λByte
#define MSG_18_MAX_LEN  21
#define MSG_19_MAX_LEN  39
#define MSG_1_MAX_LEN  MSG_18_MAX_LEN	/** message 1�ĳ�����message 18��ͬ */
#define MSG_21_MAX_LEN  45

//ȫ�ֱ�������
extern U16   AISMin; 
extern U16   AISSlot;

extern U32   Send18Time;		     //���η���18����Ϣ��ʱ��
extern U32   Send19Time;		     //���η���19����Ϣ��ʱ��

extern U16   SendSlotSelectedFlag;//wt ѡȡ����Ϣ����ʱ϶��ǣ�1��ʾ�Ѿ�ѡȡ����
extern U8   G_Msg1[];
extern U8   G_Msg6IFM0[];        //lq ��Ϣ6 
extern U8   G_Msg18[];			 //��Ϣ18ģ�棬��ʼ�������ձ�ģ������18����Ϣ
extern U8   G_Msg19[];			 //��Ϣ19ģ�棬��ʼ�������ձ�ģ������19����Ϣ
extern U8   G_Msg14[];			 //��Ϣ14�洢λ��
extern U8   G_Msg21[];
extern U32   Send14Time;
extern U16   Msg14NeedToSend;		 //����Ϣ14��Ҫ����
extern U8 Msg6AppDataText[];     //lq 6-bit ASCII����Сд�ַ�

extern U32   SendInterval;	 //4503��Ϣ���ͼ��ָʾ		6����
extern U32   SendInterval_4501_dyn_low;	 //4501��̬��Ϣ���ٷ��ͼ��ָʾ	3����
extern U32   SendInterval_4501_dyn_high;	 //4501��̬��Ϣ���ٷ��ͼ��ָʾ	30�� 1125ʱ϶
extern U32   SendInterval_4501_sta;	 //4501��̬̬��Ϣ���ͼ��ָʾ	6����
#define SPEEDFLAG 20		//�ߵͺ��ٵ�����ֵ

//��������
U16 InitInterfacePara(void);
void CreatMsgMana(void);
void CreatMsgMana_4501(void);
void CreatMsgMana_4503(void);
U16 Change4501Channel(void);

extern U16 ChangeChannel(void);

/**
	@ingroup Э��ջ
	@brief ������Ϣ14���ӿ�FIFO
	@param ��
	@retval 0 ����
*/
extern U16 SendMsg14(void);

/**
	@ingroup Э��ջ
	@brief ������Ϣ18���ӿ�FIFO
	@param ��
	@retval 0 ����
*/
extern U16 SendMsg18(U8 burstFlg);

/**
	@ingroup Э��ջ
	@brief ������Ϣ19���ӿ�FIFO
	@param ��
	@retval 0 ����
*/
extern U16 SendMsg19(U8 burstFlg);

extern U16 SendMsg14_2(U8 inChannelNum);
extern U16 SendMsg18_2(U8 inChannelNum);
extern U16 SendMsg19_2(U8 inChannelNum, U8 inTimeoutFlg);
extern U16 SendMsg1_SO(U8 ucBurstFlg, U8 ucTestFlg);
extern U16 SendMsg14_SO(U8 ucBurstFlg, U8 ucTestFlg);
#endif

