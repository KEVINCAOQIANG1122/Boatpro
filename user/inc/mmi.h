/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	mmi.h
  Author: vozhj      Version: V1.00       Date: 2012.03.21 
  Description:  
  				��Ҫ���ܣ� 
				1. ������غ궨��
				2. ��������
				3. ��������
				4. ����ṹ������
				
  Others:         // �������ݵ�˵��
  Function List:  // ��Ҫ�����б�ÿ����¼Ӧ���������������ܼ�Ҫ˵��
  History:        // �޸���ʷ��¼�б�ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸�
                  // �߼��޸����ݼ���  
    1. Date:
       Author:
       Modification:
*******************************************************************************/
#ifndef _MMI_H
#define _MMI_H
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#define MMI_CTRLSTA_IDLE	0	//״̬��IDLE״̬
#define MMI_CTRLSTA_STEP1	1	//״̬��IDLE״̬
#define MMI_CTRLSTA_STEP2	2	//״̬��IDLE״̬

#define MMI_STA_NONE		0	//MMIΪ����״̬
#define MMI_STA_BUSY		1	//MMIΪæ״̬

#define MMI_100SECPEROID	20	//100ms��ʱ��
#define MMI_3SECCNT			30	//3s������

#define MSG_CTRL_STA_POWERUP (0)//��ʼ����
#define MSG_CTRL_STA_SEND (1)//����
#define MSG_CTRL_STA_IDLE (2)//����
#define MSG_CTRL_STA_WAIT_SEND (3)//�ȴ���λ

#define MSG_CTRL_WAITNUM (20)//�ȴ�20��ѭ��

/**
	@ingroup MMI
	@brief SOS�澯���ṹ��
*/
typedef struct {
	U32 TimeCnt;
	U16 TimePeriod;	
	U16 Time100msCnt;	//100ms��ʱ��

	U8  MMICtrlSta;		//MMI״̬��
	U8  MMISta;			//MMI״̬

	U8 	SOSConfirm;//ȷ��SOS�澯

	U16 MMIKeyValue;//SOS�澯��Ӧʱ����ֵ
}MMIStruct;

typedef struct{
	U8 MsgCtrlState;
	S16 GpsStartFrame;
	S16 GpsStartSlot;
    S32 slGpsOnSlot;                                                            //lq GPS������ʱ    
	U8 GpsPowerup;
	U8 EnterHighFreq;
	U16 WaitCount;//�����ⲿѭ��̫�죬�ȴ���ģʽ�л�����ѭ����ִ�в���
}MsgCtrlStruct;

/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
extern MsgCtrlStruct MsgCtrlPara;

/* Public  function  ---------------------------------------------------------*/

extern void IniMMIPara(void);
/**
	@ingroup MMI
	@brief SOS�澯���
	@param ��
	@retval ��

	����SOS���봥��SOS�澯
	�ø澯��Э��ջÿ����ֻ����Ӧһ��
*/
extern void MMIProcess(void);

/**
	@ingroup ����
	@brief ��ѯ�Ƿ�SOS��������״̬
	@param ��
	@retval TRUE SOS��������
	@retval FALSE SOS����æ
*/
extern U8 GetMMISleepSta(void);

/**
	@ingroup MMI
	@brief ��ѯ�Ƿ�SOS�澯����
	@param ��
	@retval TRUE SOS�澯
	@retval FALSE SOS���澯

	�澯״̬�ڲ�ѯ��һ�η���TRUE���ٴβ�ѯ����ΪFALSE
*/
extern U8 GetMMISOSState(void);

extern void InitMsgCtrlPara(void);
extern U8 MsgSendErrorCheck(S16 Slot,S16 Frame);
extern void MsgCtrlProcess(void);
extern U8 GetMsgCtrlSleepState(void);
extern void MsgStartNew(void);
extern void ResetMsgGpsPowerup(void);
extern void SetMsgGpsPowerup(void);

extern U8 GetMsgGpsPowerup(void);
extern U8 GetMsgWakeupState(void);

#endif
