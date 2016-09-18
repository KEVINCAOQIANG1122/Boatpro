/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: Slot.c
  Author: lq       Version : V1.0.0        Date: 2015.01.15
  Description:     AIS ʱ϶����
  Version:         V1.0.0
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "Include.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static SlotMapStruct SlotMap;      //lq ʱ϶ӳ��

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* ����  : SlotMapStructInit
* ����  : �ṹ���ʼ��
* ����  : pSlotMapStruct - SlotMapStruct�ṹ��ָ��  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SlotMapStructInit(SlotMapStruct *pSlotMapStruct)
{
    U16 i;
    U16 j;

    for (i = 0; i < SLOT_MAP_FRAME_SIZE; i++)
    {
        for (j = 0; j < FRAME_SLOT_SIZE; j++)
        {
            pSlotMapStruct->MapTable[i][j] = (SLOT_STATE_FREE << 4) | SLOT_STATE_FREE;
        }
    }
    pSlotMapStruct->TableIndex = 0;
    pSlotMapStruct->IndexFrame = FRAME_INVALID_VALUE;

    for (i = 0; i < SLOT_CANDIDATE_BUFF_LEN; i++)
    {
        pSlotMapStruct->CandidateSlotBuff[i] = 0;
    }    
    pSlotMapStruct->CandidateSlotCnt = 0;
}

/*******************************************************************************
* ����  : SLOT_UpdateSlotMapIndexFrame
* ����  : ����ʱ϶ӳ���ǰ��������Ӧ��֡��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void SLOT_UpdateSlotMapIndexFrame(void) 
{
    S32 currFrame;
    S32 frameOffset;
    S16 tempS16;
    S16 i;
    S16 k;

    currFrame = GetFrameNum();

    if (SlotMap.IndexFrame == FRAME_INVALID_VALUE)
    {
        SlotMap.IndexFrame = currFrame;
    }
    else
    {
        frameOffset = currFrame - SlotMap.IndexFrame;
        if (frameOffset > 0)
        {
            if (frameOffset > SLOT_MAP_FRAME_SIZE)
            {
                frameOffset = SLOT_MAP_FRAME_SIZE;
            }

            /*lq ʱ϶���� */
            for (i = 0; i < frameOffset; i++)
            {
                tempS16 = (SlotMap.TableIndex + i) % SLOT_MAP_FRAME_SIZE;
                for (k = 0; k < FRAME_SLOT_SIZE; k++)
                {
                    SlotMap.MapTable[tempS16][k] =
                        (SLOT_STATE_FREE << 4) | SLOT_STATE_FREE;
                }
            }

            SlotMap.TableIndex = (SlotMap.TableIndex + frameOffset) % SLOT_MAP_FRAME_SIZE;
            SlotMap.IndexFrame = currFrame;
        }
    }
}

/*******************************************************************************
* ����  : SetSlotState
* ����  : ����ʱ϶״̬������չ�������ŵ���
* ����  : channel - �ŵ���
          slot - ʱ϶��
          slotState - ʱ϶״̬
          frame - ֡�ţ�����ֵ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 SetSlotState(AisChannelEnum channel, U16 slot, U8 slotSate, S32 frame) 
{
    U8 channelMask;
    U8 channelOffset;
    U8 tempU8;
    U16 tempU16;
    S32 tempS32;

    /*lq ����ʱ϶ӳ���ǰ֡ */
    SLOT_UpdateSlotMapIndexFrame();

    /*lq ����ָ��֡���뵱ǰ֡��֮�� */
    tempS32 = frame - SlotMap.IndexFrame;
    if ((tempS32 >= SLOT_MAP_FRAME_SIZE) || (tempS32 < 0))
    {
        return FALSE;
    }
    else
    {
        tempS32 = (SlotMap.TableIndex + tempS32) % SLOT_MAP_FRAME_SIZE; //lq ��frame֡��MapTable�е�����
    }

    /*lq �ŵ����ж� */
    if (channel == ENUM_AIS_CHANNEL_A)
    {
        channelMask = 0x0F;
        channelOffset = 0;
    }
    else if (channel == ENUM_AIS_CHANNEL_B)
    {
        channelMask = 0xF0;
        channelOffset = 4;
    }
    else
    {
        return FALSE;
    }

    /*lq ʱ϶����Ч�Դ��� */
    tempU16 = slot % FRAME_SLOT_SIZE;
    
    /*lq ʱ϶״̬תΪӳ���Ԫ�� */
    tempU8 = slotSate << channelOffset;
    tempU8 &= channelMask;

    /*lq �޸�ָ��ʱ϶��״̬ */
    SlotMap.MapTable[tempS32][tempU16] &= (~channelMask);
    SlotMap.MapTable[tempS32][tempU16] |= tempU8;

    return TRUE;        
}

/*******************************************************************************
* ����  : GetSlotState
* ����  : ��ѯʱ϶״̬������չ�������ŵ���
* ����  : channel - �ŵ���
          slot - ʱ϶��
          frame - ֡�ţ�����ֵ��	  
* ���  : ��
* ����  : ʱ϶״̬
*******************************************************************************/
U8 GetSlotState(AisChannelEnum channel, U16 slot, S32 frame) 
{
    U8 channelMask;
    U8 channelOffset;
    U8 temp8;
    U8 slotSate;
    U16 tempU16;
    S32 tempS32;
    
    /*lq ����ʱ϶ӳ���ǰ֡ */
    SLOT_UpdateSlotMapIndexFrame();

    /*lq ����ָ��֡���뵱ǰ֡��֮�� */
    tempS32 = frame - SlotMap.IndexFrame;
    if ((tempS32 >= SLOT_MAP_FRAME_SIZE) || (tempS32 < 0))
    {
        return SLOT_STATE_INVALID;
    }
    else
    {
        tempS32 = (SlotMap.TableIndex + tempS32) % SLOT_MAP_FRAME_SIZE; //lq ��frame֡��MapTable�е�����
    }

    /*lq �ŵ����ж� */
    if (channel == ENUM_AIS_CHANNEL_A)
    {
        channelMask = 0x0F;
        channelOffset = 0;
    }
    else if (channel == ENUM_AIS_CHANNEL_B)
    {
        channelMask = 0xF0;
        channelOffset = 4;
    }
    else
    {
        return SLOT_STATE_INVALID;
    }

    /*lq ʱ϶����Ч�Դ��� */
    tempU16 = slot % FRAME_SLOT_SIZE;

    /*lq ��ȡָ��ʱ϶��״̬ */
    temp8 = SlotMap.MapTable[tempS32][tempU16] & channelMask;
    slotSate = temp8 >> channelOffset;
     
    return slotSate;        
}

/*******************************************************************************
* ����  : SelectCandidateSlot
* ����  : ѡ���ѡʱ϶
* ����  : channel - �ŵ���
          slot - ʱ϶��
          selectInterval - ��ѡʱ϶ѡ��������ƫ��ֵ
          frame - ֡�ţ�����ֵ��
          selectType - ����ѡ��or˫��ѡ��	  
* ���  : ��
* ����  : ѡ����ĳ����ѡʱ϶��
*******************************************************************************/
S16 SelectCandidateSlot(AisChannelEnum channel, S16 slot, U16 selectInterval,
                        S32 frame, CandSlotSelectEnum selectType) 
{
    S16 tempS16;
    S32 tempS32;
    U16 i;
    U16 candidateSlotIndex;

    if (SlotMap.IndexFrame == FRAME_INVALID_VALUE)
    {
        return SLOT_INVALID_VALUE;
    }

    /*lq ����ָ��֡���뵱ǰ֡��֮�� */
    tempS32 = frame - SlotMap.IndexFrame;
    if ((tempS32 >= SLOT_MAP_FRAME_SIZE) || (tempS32 < 0))
    {
        return SLOT_INVALID_VALUE;
    }

    /*lq �ŵ����ж� */
    if (!((channel == ENUM_AIS_CHANNEL_A) || (channel == ENUM_AIS_CHANNEL_B)))
    {
        return SLOT_INVALID_VALUE;
    }

    /*lq ʱ϶����Ч�Դ��� */
    slot = slot % FRAME_SLOT_SIZE;

    /*lq ѡ�������� */
    if (selectInterval < SLOT_CANDIDATE_SELECT_INTERVAL_OFFSET_MIN)
    {
        selectInterval = SLOT_CANDIDATE_SELECT_INTERVAL_OFFSET_MIN;
    }
    else if (selectInterval > SLOT_CANDIDATE_SELECT_INTERVAL_OFFSET_MAX)
    {
        selectInterval = SLOT_CANDIDATE_SELECT_INTERVAL_OFFSET_MAX;    
    }

    /*lq ȷ����ѡʱ϶���� */
    SlotMap.CandidateSlotCnt = 0;
    if (GetSlotState(channel, slot, frame) == SLOT_STATE_FREE)
    {
        SlotMap.CandidateSlotBuff[SlotMap.CandidateSlotCnt++] = slot;    
    }
    for (i = 1; i <= selectInterval; i++)
    {
        /*lq ˫��ѡ�� */
        if (selectType == SLOT_CAND_SELECT_BI)
        {
            if ((slot - i) < 0)
            {
                tempS32 = frame - 1;
            }
            else
            {
                tempS32 = frame;
            }
            tempS16 = (slot - i + FRAME_SLOT_SIZE) % FRAME_SLOT_SIZE;
            if (GetSlotState(channel, tempS16, tempS32) == SLOT_STATE_FREE)
            {
                SlotMap.CandidateSlotBuff[SlotMap.CandidateSlotCnt++] = tempS16;

                //lq... A/B�ŵ�����or���ʱ϶����
            }
        }

        if ((slot + i) >= FRAME_SLOT_SIZE)
        {
            tempS32 = frame + 1;
        }
        else
        {
            tempS32 = frame;
        }
        tempS16 = (slot + i) % FRAME_SLOT_SIZE;
        if (GetSlotState(channel, tempS16, tempS32) == SLOT_STATE_FREE)
        {
            SlotMap.CandidateSlotBuff[SlotMap.CandidateSlotCnt++] = tempS16;            
        }
    }

    if (SlotMap.CandidateSlotCnt == 0)
    {
        return SLOT_INVALID_VALUE;
    }

    /*lq �Ӻ�ѡʱ϶���������ѡ��һ��ʱ϶ */
    candidateSlotIndex = (rand() % SlotMap.CandidateSlotCnt);

    return SlotMap.CandidateSlotBuff[candidateSlotIndex];
}

/*******************************************************************************
* ����  : SelectCandidateSlotFrame
* ����  : ѡ���ѡʱ϶���ڵ�֡
* ����  : candSlot - ��ѡʱ϶
          nominalSlot - ���ʱ϶
          nominalFrame - ���֡
* ���  : ��
* ����  : ��ѡʱ϶���ڵ�֡
*******************************************************************************/
S32 SelectCandidateSlotFrame(S16 candSlot, S16 nominalSlot, S32 nominalFrame) 
{
    S32 tempS32;

    if (candSlot > nominalSlot)
    {
        if (candSlot - nominalSlot > SLOT_CANDIDATE_SELECT_INTERVAL_OFFSET_MAX)
        {
            tempS32 = nominalFrame - 1;
            if (tempS32 < 0)
            {
                tempS32 = 0;
            }
        }
        else
        {
            tempS32 = nominalFrame;
        }
    }
    else
    {
        if (nominalSlot - candSlot > SLOT_CANDIDATE_SELECT_INTERVAL_OFFSET_MAX)
        {
            tempS32 = nominalFrame + 1;
        }
        else
        {
            tempS32 = nominalFrame;
        }    
    }

    return tempS32;
}

/*******************************************************************************
* ����  : CalcSlotOffset
* ����  : ����ʱ϶ƫ����
* ����  : frame1 - Դ֡��
          slot1 - Դʱ϶��
          frame2 - Ŀ��֡��
          slot2 - Ŀ��ʱ϶��
* ���  : ��
* ����  : ʱ϶ƫ����
*******************************************************************************/
S32 CalcSlotOffset(S32 frame1, S16 slot1, S32 frame2, S16 slot2) 
{
    S32 tempS32;

    tempS32 = (frame2 - frame1) * FRAME_SLOT_SIZE + (slot2 - slot1);    

    return tempS32;
}

/*******************************************************************************
* ����  : CalcFrameNum
* ����  : ����֡��
* ����  : frame - ֡��
          slot - ʱ϶��
          slotOffset - ʱ϶ƫ����
* ���  : ��
* ����  : ����ƫ�������֡��
*******************************************************************************/
S32 CalcFrameNum(S32 frame, S16 slot, S32 slotOffset) 
{
    S32 tempS32;

    tempS32 = (frame * FRAME_SLOT_SIZE + slot + slotOffset) / FRAME_SLOT_SIZE;

    return tempS32;
    
}

/*******************************************************************************
* ����  : CalcSlotNum
* ����  : ����ʱ϶��
* ����  : frame - ֡��
          slot - ʱ϶��
          slotOffset - ʱ϶ƫ����
* ���  : ��
* ����  : ����ƫ�������ʱ϶��
*******************************************************************************/
S16 CalcSlotNum(S32 frame, S16 slot, S32 slotOffset) 
{
    S16 tempS16;

    tempS16 = (frame * FRAME_SLOT_SIZE + slot + slotOffset) % FRAME_SLOT_SIZE;

    return tempS16;
    
}

/*******************************************************************************
* ����  : CalcSlotOffset
* ����  : ����ʱ϶ƫ����
* ����  : frame - ֡��
          slot - ʱ϶��
          slotOffset - ʱ϶ƫ����
* ���  : ��
* ����  : ����ƫ�������ʱ϶����
*******************************************************************************/
S32 CalcSlotSum(S32 frame, S16 slot, S32 slotOffset) 
{
    S32 tempS32;

    tempS32 = frame * FRAME_SLOT_SIZE + slot + slotOffset;

    return tempS32;    
}

/*******************************************************************************
* ����  : IsSlotValid
* ����  : ʱ϶��Ч���ж�
* ����  : slot - ʱ϶
* ���  : ��
* ����  : TRUE- ��Ч�� FALSE - ��Ч
*******************************************************************************/
U8 IsSlotValid(S16 slot) 
{
    if (slot >= 0 && slot < FRAME_SLOT_SIZE)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* ����  : SLOT_ModuleInit
* ����  : ģ���ʼ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void SLOT_ModuleInit(void) 
{
    SlotMapStructInit(&SlotMap);
}

