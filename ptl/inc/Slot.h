/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: Slot.h
  Author: lq       Version : V1.0.0        Date: 2015.01.15
  Description:     AIS ʱ϶����
  Version:         V1.0.0
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _SLOT_H_
#define _SLOT_H_

/* Include -------------------------------------------------------------------*/
#include "Include.h"

/* Public define -------------------------------------------------------------*/
/*lq ʱ϶��λ���� */
#define FRAME_SLOT_SIZE                 2250    //lq slot number of one frame
#define SLOT_PHASE_SIZE                 1280    //lq phase number of one slot

/*lq ʱ϶��Ч���ж� */
#define SLOT_VALIDITY_CHECK(slot)       (((slot >= 0) && (slot < FRAME_SLOT_SIZE)) ? TRUE : FALSE)

/*lq ר��ʱ϶ֵ���� */
#define SLOT_INVALID_VALUE              FRAME_SLOT_SIZE     //lq ��Чʱ϶ֵ
#define SLOT_FORCE_TX_VALUE             -1      //lq ǿ�Ʒ��ͱ�־ʱ϶ֵ��ʹ�õ�ǰ��ʱ϶�������ͣ�

/*lq ��Ч֡ */
#define FRAME_INVALID_VALUE             -1

/*lq ��λ */
#define PHASE_VALUE_TX_START            0       //lq ������ʼ��λ

/*lq ʱ϶ӳ��֡�� */
#define SLOT_MAP_FRAME_SIZE             7       //lq ʱ϶ӳ����֡��

/*lq ʱ϶״̬ */
#define SLOT_STATE_INVALID              0   //lq ��Ч״̬
#define SLOT_STATE_FREE                 1
#define SLOT_STATE_INTER_ALLOC          2
#define SLOT_STATE_EXTER_ALLOC          3
#define SLOT_STATE_AVAILABLE            4
#define SLOT_STATE_UNAVAILABLE          5

/*lq ��ѡʱ϶ѡ���� */
#define SLOT_CANDIDATE_SELECT_INTERVAL_OFFSET_MIN   7   //lq unit: 1slot, i.e. 75 / 10
#define SLOT_CANDIDATE_SELECT_INTERVAL_OFFSET_MAX   112 //lq unit: 1slot, i.e. 1125 / 10
#define SLOT_CANDIDATE_BUFF_LEN                     ((SLOT_CANDIDATE_SELECT_INTERVAL_OFFSET_MAX << 1) + 1)    

/* Public typedef ------------------------------------------------------------*/
/*lq ��ѡʱ϶ѡ��ʽ���Ͷ��� */
typedef enum
{
    SLOT_CAND_SELECT_UNI = 0,   //lq ����ѡ��
    SLOT_CAND_SELECT_BI,        //lq ˫��ѡ��
}CandSlotSelectEnum;

/*lq ʱ϶״̬ӳ���Ԫ�ض��� */
typedef U8 SlotMapArray [FRAME_SLOT_SIZE];  //lq һ֡��ʱ϶ӳ���

/*lq ��ѡʱ϶�������� */
typedef U16 CandidateSlotArray[SLOT_CANDIDATE_BUFF_LEN];

/*lq ʱ϶״̬ӳ��� */
typedef struct
{
    SlotMapArray MapTable[SLOT_MAP_FRAME_SIZE];
    U8 TableIndex;                              //lq ��ǰ֡����
    S32 IndexFrame;                             //lq TableIndex��Ӧ��֡�ţ�����ֵ��

    CandidateSlotArray CandidateSlotBuff;
    U16 CandidateSlotCnt;
}SlotMapStruct;

/*lq ʱ϶��ʱ�ṹ�� */
typedef struct
{
    S32 Frame;
    S16 slot;
    S16 phase;
}SlotTimeStruct;
/* Public variables ----------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
extern U8 SetSlotState(AisChannelEnum channel, U16 slotNum, U8 slotSate, S32 frame);
extern U8 GetSlotState(AisChannelEnum channel, U16 slotNum, S32 frame);
extern S16 SelectCandidateSlot(AisChannelEnum channel, S16 slot, U16 selectInterval, S32 frame, CandSlotSelectEnum selectType);
extern void SLOT_ModuleInit(void);

//  lnw  add  below
extern S16 CalcSlotNum(S32 frame, S16 slot, S32 slotOffset);//lnw add 
extern S32 CalcSlotOffset(S32 frame1, S16 slot1, S32 frame2, S16 slot2); 
extern U8 SetSlotState(AisChannelEnum channel, U16 slot, U8 slotSate, S32 frame);
extern U8 GetSlotState(AisChannelEnum channel, U16 slot, S32 frame);
extern S16 SelectCandidateSlot(AisChannelEnum channel, S16 slot, U16 selectInterval,S32 frame, CandSlotSelectEnum selectType);
extern S32 SelectCandidateSlotFrame(S16 candSlot, S16 nominalSlot, S32 nominalFrame); 
extern S32 CalcFrameNum(S32 frame, S16 slot, S32 slotOffset); 
extern S32 CalcSlotSum(S32 frame, S16 slot, S32 slotOffset); 
extern U8  IsSlotValid(S16 slot) ;


#endif
