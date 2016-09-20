/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: MsgManager.h
  Author: lq       Version : V1.0.0        Date: 2015.06.23
  Description:     ��Ϣ����ģ��
  Version:         V1.0.0
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _MSGMANAGER_H_
#define _MSGMANAGER_H_

/* Include -------------------------------------------------------------------*/
#include "Include.h"

/* Public define -------------------------------------------------------------*/
#define MSG_MAN_MSG_STATIC_LIST_LEN     7
#define MSG_MAN_ACK_FIFO_LEN            7

/* Public typedef ------------------------------------------------------------*/
/*lq 
 *  ������Ϣ���Ϳ���ö��
 */
typedef enum
{
    ENUM_PERIOD_SCHED_CTRL_STATE_IDLE = 0,
    ENUM_PERIOD_SCHED_CTRL_STATE_RUNNIG
}PeriodSchedCtrlStateEnum;

/*lq ��Ϣ���ȼ����� */
typedef enum
{
    ENUM_AIS_MSG_PRIORITY_H = 0,
    ENUM_AIS_MSG_PRIORITY_1 = 1,        //lq AIS��Ϣ�е�������ȼ�
    ENUM_AIS_MSG_PRIORITY_2,
    ENUM_AIS_MSG_PRIORITY_3,
    ENUM_AIS_MSG_PRIORITY_4,
    ENUM_AIS_MSG_PRIORITY_L,
    ENUM_AIS_MSG_PRIORITY_DEFAULT = ENUM_AIS_MSG_PRIORITY_L
}AisMsgPriorityEnum;

/*lq AIS��Ϣ��Ԫ */
typedef struct MsgCellStruct
{
    AISMsgTypeEnum MsgType;
    AisMsgPriorityEnum MsgPriority;
    AisChannelEnum Channel;
    S16 Slot;
    S32 Frame;
    U8 MsgState;
	AISMsgBinBuffArray Data;            //lq Data[0] �����Ч���ݵ��ֽڳ��ȣ�������Data[0]����AIS��Ϣ��Ϊ���ֽڳ� 			    
	U16 DataLength;		 			    //lq unit: 1Byte�����ݳ��ȣ�����Data[0]��DataLength = Data[0] + 1����AIS��Ϣ��Ϊ���ֽڳ�
}MsgCellStruct;

/*lq AIS��Ϣ��̬����Ԫ�� */
typedef struct MsgStaticListElemStruct
{
    MsgCellStruct MsgCell;
    S16 NextIndex;
}MsgStaticListElemStruct;

/*lq 
 *  AIS��Ϣ��̬����
 *  �����е�Ԫ�ذ���Ϣ���ȼ���������      
 */
typedef struct MsgStaticListStruct
{
    MsgStaticListElemStruct StaticList[MSG_MAN_MSG_STATIC_LIST_LEN];
    S16 Head;                           //lq �ѷ��侲̬����ͷ����
    S16 FreeHead;                       //lq ���о�̬����ͷ����
}MsgStaticListStruct;

/*lq 
 *  AIS�̶���������Ϣ��������ṹ��
 */
typedef struct
{
    AisChannelEnum Channel;
    S32 NominalFrame;
    S16 NominalSlot;                    //lq ���ʱ϶
    S32 TxFrame;                        
    S16 TxSlot;                         //lq ����ʱ϶Ϊ���ʱ϶����һ�����ֵ
    S32 TxSlotInterval;                 //lq unit: 1slot
    U8  ValidFlag;                      //lq ��ʱ��Ч��־
}MsgTxTaskStruct;

/*lq 
 *  AIS�̶���������Ϣ���Ƚṹ��
 */
typedef struct
{
    MsgTxTaskStruct MsgTxTaskMsg19;
    MsgTxTaskStruct MsgTxTaskMsg24A;
    MsgTxTaskStruct MsgTxTaskMsg24B;
	MsgTxTaskStruct MsgTxTaskMsg5;

    PeriodSchedCtrlStateEnum CtrlState;
}PeriodSchedStruct;

/*lq 
 *  AISȷ�ϵ�Ԫ�ṹ��
 */
typedef struct
{
    U32 DestinationID;
    U8  SequenceNumber;    
}AisAckCellStruct;

/*lq 
 *  AISȷ�ϵ�Ԫ�ṹ��
 */
typedef struct
{
    AisAckCellStruct FIFO[MSG_MAN_ACK_FIFO_LEN];    
    U16 Input;
    U16 Output;
}AisAckFifoStruct;

/*lq 
 *  AISͻ������Ϣ���Ƚṹ��
 */
typedef struct
{
    AisAckFifoStruct Msg6AckA;          //lq ���ŵ�A��6����Ϣ��ȷ�϶���
    AisAckFifoStruct Msg6AckB;
    AisAckFifoStruct Msg12AckA;
    AisAckFifoStruct Msg12AckB;

    /*lq �ɽ�ͨ��ABM��BBM������õ���Ϣ����ýṹ���� */
    //lq... ͨ��ABM��BBM������õ���Ϣ��ʱ��������

}BurstSchedStruct;

/* Public variables ----------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
extern U8 MSGMAN_TxMsgListIsEmpty(void);
extern U8 MSGMAN_TxMsgListIsFull(void);
extern U8 MSGMAN_TxMsgListInsert(const MsgCellStruct *pMsgCellStruct);
extern U8 MSGMAN_TxMsgListGetElem(MsgCellStruct *pMsgCellStruct);
extern void MSGMAN_ModuleInit(void);
extern U8 SendMsg1(AisChannelEnum channel, S16 txSlot, AISMsgCommStateUnion commState);
extern U8 SendMsg5(AisChannelEnum channel, S16 txSlot);
extern U8 SendMsg6(AisChannelEnum channel, S16 txSlot);
extern U8 SendMsg7(AisChannelEnum channel, S16 txSlot, U8 ackType, U32 *pDestinationID,
            U8 *pSequenceNumber);
extern U8 SendMsg8(AisChannelEnum channel, S16 txSlot);
extern U8 SendMsg12(AisChannelEnum channel, S16 txSlot);
extern U8 SendMsg13(AisChannelEnum channel, S16 txSlot, U8 ackType, U32 *pDestinationID,
            U8 *pSequenceNumber);
extern U8 SendMsg14(AisChannelEnum channel, S16 txSlot);
extern U8 SendMsg18(AisChannelEnum channel, S16 txSlot,
             AISMsgCommStateEnum commStateType, AISMsgCommStateUnion commState);
extern U8 SendMsg19(AisChannelEnum channel, S16 txSlot);
extern U8 SendMsg24A(AisChannelEnum channel, S16 txSlot);
extern U8 SendMsg24B(AisChannelEnum channel, S16 txSlot);



extern U8 MSGMAN_RxMsgListInsert(const MsgCellStruct *pMsgCellStruct);// lnw add
extern U8 SendMsg3(AisChannelEnum channel, S16 txSlot, AISMsgCommStateUnion commState);//lnw add
#endif
/*==============================================================================
------------------------------------End of File---------------------------------
==============================================================================*/
