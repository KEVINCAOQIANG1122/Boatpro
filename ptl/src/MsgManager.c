/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: MsgManager.c
  Author: lq       Version : V1.0.0        Date: 2015.06.23
  Description:     ��Ϣ����ģ��
  Version:         V1.0.0
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "Include.h"

/* Private typedef -----------------------------------------------------------*/
static MsgStaticListStruct TxMsgList;
static MsgStaticListStruct RxMsgList;
static PeriodSchedStruct PeriodSched;
static BurstSchedStruct BurstSched;

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* ����  : MSGMAN_MsgCellStructInit
* ����  : ��ʼ���ṹ��
* ����  : pMsgCellStruct - MsgCellStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void MSGMAN_MsgCellStructInit(MsgCellStruct *pMsgCellStruct)
{
    pMsgCellStruct->MsgType = AISMsgTypeError;
    pMsgCellStruct->MsgPriority = ENUM_AIS_MSG_PRIORITY_DEFAULT;
    pMsgCellStruct->Channel = ENUM_AIS_CHANNEL_A;
    pMsgCellStruct->Slot = SLOT_INVALID_VALUE;
    pMsgCellStruct->Frame = FRAME_INVALID_VALUE;
    pMsgCellStruct->MsgState = FALSE;
	pMsgCellStruct->DataLength = 0;
}

/*******************************************************************************
* ����  : MSGMAN_MsgStaticListElemStructInit
* ����  : ��ʼ���ṹ��
* ����  : pMsgStaticListElemStruct - MsgStaticListElemStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void MSGMAN_MsgStaticListElemStructInit(
    MsgStaticListElemStruct *pMsgStaticListElemStruct)
{
    MSGMAN_MsgCellStructInit(&pMsgStaticListElemStruct->MsgCell);
    pMsgStaticListElemStruct->NextIndex = STATIC_LIST_INDEX_NULL;
}

/*******************************************************************************
* ����  : MSGMAN_MsgTxTaskStructInit
* ����  : ��ʼ���ṹ��
* ����  : pMsgTxTaskStruct - MsgTxTaskStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void MSGMAN_MsgTxTaskStructInit(MsgTxTaskStruct *pMsgTxTaskStruct)
{
    pMsgTxTaskStruct->Channel = ENUM_AIS_CHANNEL_A;
    pMsgTxTaskStruct->NominalFrame = FRAME_INVALID_VALUE;
    pMsgTxTaskStruct->NominalSlot = SLOT_INVALID_VALUE;
    pMsgTxTaskStruct->TxFrame = FRAME_INVALID_VALUE;
    pMsgTxTaskStruct->TxSlot = SLOT_INVALID_VALUE;
    pMsgTxTaskStruct->TxSlotInterval = AIS_SLOT_INTERVAL_FIXED_PERIOD_TX_PERIOD;    //lq ˫�ŵ�6min
    pMsgTxTaskStruct->ValidFlag = FALSE;
}

/*******************************************************************************
* ����  : MSGMAN_PeriodSchedStructInit
* ����  : ��ʼ���ṹ��
* ����  : PeriodSchedStruct - pPeriodSchedStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void MSGMAN_PeriodSchedStructInit(PeriodSchedStruct *pPeriodSchedStruct)
{
    MSGMAN_MsgTxTaskStructInit(&(pPeriodSchedStruct->MsgTxTaskMsg19));
    MSGMAN_MsgTxTaskStructInit(&(pPeriodSchedStruct->MsgTxTaskMsg24A));
    MSGMAN_MsgTxTaskStructInit(&(pPeriodSchedStruct->MsgTxTaskMsg24B));
	MSGMAN_MsgTxTaskStructInit(&(pPeriodSchedStruct->MsgTxTaskMsg5));
    pPeriodSchedStruct->CtrlState = ENUM_PERIOD_SCHED_CTRL_STATE_IDLE;
}

/*******************************************************************************
* ����  : MSGMAN_AisAckCellStructInit
* ����  : ��ʼ���ṹ��
* ����  : pAisAckCellStruct - AisAckCellStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void MSGMAN_AisAckCellStructInit(AisAckCellStruct *pAisAckCellStruct)
{
    pAisAckCellStruct->DestinationID = 0;
    pAisAckCellStruct->SequenceNumber = 0;    
}

/*******************************************************************************
* ����  : MSGMAN_AisAckCellStructCopy
* ����  : �ṹ�帴��
* ����  : pDst - Ŀ��ָ��
          pSrc - Դָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void MSGMAN_AisAckCellStructCopy(AisAckCellStruct *pDst,
                                        const AisAckCellStruct *pSrc)
{
    pDst->DestinationID = pSrc->DestinationID;
    pDst->SequenceNumber = pSrc->SequenceNumber;    
}

/*******************************************************************************
* ����  : MSGMAN_AisAckFifoStructInit
* ����  : ��ʼ���ṹ��
* ����  : pAisAckFifoStruct - AisAckFifoStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void MSGMAN_AisAckFifoStructInit(AisAckFifoStruct *pAisAckFifoStruct)
{
    S16 i;

    for (i = 0; i < MSG_MAN_ACK_FIFO_LEN; i++)
    {
        MSGMAN_AisAckCellStructInit(&(pAisAckFifoStruct->FIFO[i]));
    }

    pAisAckFifoStruct->Input = 0;
    pAisAckFifoStruct->Output = 0;    
}

/*******************************************************************************
* ����  : MSGMAN_BurstSchedStructInit
* ����  : ��ʼ���ṹ��
* ����  : pBurstSchedStruct - BurstSchedStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void MSGMAN_BurstSchedStructInit(BurstSchedStruct *pBurstSchedStruct)
{
    MSGMAN_AisAckFifoStructInit(&(pBurstSchedStruct->Msg6AckA));
    MSGMAN_AisAckFifoStructInit(&(pBurstSchedStruct->Msg6AckB));
    MSGMAN_AisAckFifoStructInit(&(pBurstSchedStruct->Msg12AckA));
    MSGMAN_AisAckFifoStructInit(&(pBurstSchedStruct->Msg12AckB));
}

/*******************************************************************************
* ����  : MSGMAN_MsgTxTaskTxCfg
* ����  : ������Ϣ��������
* ����  : msgType - ��Ϣ����
          channel - �����ŵ�
          frame - ����֡��
          slot - ����ʱ϶��
* ���  : ��
* ����  : �����Ƿ�ɹ�
*******************************************************************************/
void MSGMAN_MsgTxTaskTxCfg(AISMsgTypeEnum msgType, AisChannelEnum channel,
                           S32 frame, S16 slot)
{
    MsgTxTaskStruct *pMsgTxTaskStruct;
    
    switch (msgType)
    {
        case AISMsgType19 :

            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg19);

            break;

        case AISMsgType24A :
            
            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg24A);

            break;

        case AISMsgType24B :
            
            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg24B);

            break;

		case AISMsgType5 :
            
            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg5);

            break;

        default :
            
            return;
    }

    pMsgTxTaskStruct->Channel = channel;
    pMsgTxTaskStruct->NominalFrame = frame;
    pMsgTxTaskStruct->NominalSlot = slot;
    pMsgTxTaskStruct->TxFrame = frame;
    pMsgTxTaskStruct->TxSlot = slot;
    pMsgTxTaskStruct->ValidFlag = TRUE;
}

/*******************************************************************************
* ����  : MSGMAN_MsgTxTaskNextTxCfg
* ����  : �´η�������
* ����  : msgType - ��Ϣ����
* ���  : ��
* ����  : �����Ƿ�ɹ�
*******************************************************************************/
U8 MSGMAN_MsgTxTaskNextTxCfg(AISMsgTypeEnum msgType)
{
    MsgTxTaskStruct *pMsgTxTaskStruct;
    S32 randValue;

    switch (msgType)
    {
        case AISMsgType19 :

            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg19);

            break;

        case AISMsgType24A :
            
            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg24A);

            break;

        case AISMsgType24B :
            
            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg24B);

            break;

		case AISMsgType5 :
            
            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg5);

            break;

        default :
            
            return FALSE;
    }

    if (pMsgTxTaskStruct->ValidFlag == FALSE)
    {
        return FALSE;
    }

    if (pMsgTxTaskStruct->Channel == ENUM_AIS_CHANNEL_A)
    {
        pMsgTxTaskStruct->Channel = ENUM_AIS_CHANNEL_B;    
    }
    else
    {
        pMsgTxTaskStruct->Channel = ENUM_AIS_CHANNEL_A;    
    }

    /*lq ������һ���ʱ϶ */
    pMsgTxTaskStruct->NominalFrame =
        CalcFrameNum(pMsgTxTaskStruct->NominalFrame,
            pMsgTxTaskStruct->NominalSlot,
            pMsgTxTaskStruct->TxSlotInterval);
    
    pMsgTxTaskStruct->NominalSlot =
        CalcSlotNum(pMsgTxTaskStruct->NominalFrame,
            pMsgTxTaskStruct->NominalSlot,
            pMsgTxTaskStruct->TxSlotInterval);

    /*lq ������һ����ʱ϶ */
    randValue = Random(-AIS_SLOT_INTERVAL_SLOT_SELECT_OFFSET_MAX,
        AIS_SLOT_INTERVAL_SLOT_SELECT_OFFSET_MAX);

    pMsgTxTaskStruct->TxFrame =
        CalcFrameNum(pMsgTxTaskStruct->NominalFrame,
            pMsgTxTaskStruct->NominalSlot,
            randValue);
    
    pMsgTxTaskStruct->TxSlot =
        CalcSlotNum(pMsgTxTaskStruct->NominalFrame,
            pMsgTxTaskStruct->NominalSlot,
            randValue);

    return TRUE;
}

/*******************************************************************************
* ����  : MSGMAN_MsgTxTaskIsTxTimExpired
* ����  : ���䶨ʱ�Ƿ�ʱ
* ����  : msgType - ��Ϣ����
          frame - ��ǰ֡��
          slot - ��ǰʱ϶��
* ���  : ��
* ����  : �Ƿ�ʱ
*******************************************************************************/
U8 MSGMAN_MsgTxTaskIsTxTimExpired(AISMsgTypeEnum msgType, S32 frame, S16 slot)
{
    MsgTxTaskStruct *pMsgTxTaskStruct;

    switch (msgType)
    {
        case AISMsgType19 :

            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg19);

            break;

        case AISMsgType24A :
            
            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg24A);

            break;

        case AISMsgType24B :
            
            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg24B);

            break;

		case AISMsgType5 :
            
            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg5);

            break;

        default :
            
            return FALSE;
    }

    if (pMsgTxTaskStruct->ValidFlag == FALSE)
    {
        return FALSE;
    }

    if (CalcSlotOffset(pMsgTxTaskStruct->TxFrame, pMsgTxTaskStruct->TxSlot,
        frame, slot) >= 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : MSGMAN_MsgTxTaskIsSendTimExpired
* ����  : ���ͣ����뷢����У���ʱ�Ƿ�ʱ
* ����  : msgType - ��Ϣ����
          frame - ��ǰ֡��
          slot - ��ǰʱ϶��
* ���  : ��
* ����  : �Ƿ�ʱ
*******************************************************************************/
U8 MSGMAN_MsgTxTaskIsSendTimExpired(AISMsgTypeEnum msgType, S32 frame, S16 slot)
{
    return MSGMAN_MsgTxTaskIsTxTimExpired(msgType,
            CalcFrameNum(frame, slot, AIS_SLOT_INTERVAL_SEND_TO_TX),
            CalcSlotNum(frame, slot, AIS_SLOT_INTERVAL_SEND_TO_TX));
}

/*******************************************************************************
* ����  : MSGMAN_PeriodSchedule
* ����  : �̶���������Ϣ��msg19/msg24A/msg24B������ ����
* ����  : pPeriodSchedStruct - PeriodSchedStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void MSGMAN_PeriodSchedule(PeriodSchedStruct *pPeriodSchedStruct)
{
    MsgTxTaskStruct *pMsgTxTaskMsg5;
	MsgTxTaskStruct *pMsgTxTaskMsg19;
    MsgTxTaskStruct *pMsgTxTaskMsg24A;
    MsgTxTaskStruct *pMsgTxTaskMsg24B;
    S32 currFrame;
    S16 currSlot;

	if (DevStaticPara.AisClass == AIS_CLASS_B)    
    {
	    pMsgTxTaskMsg19 = &(pPeriodSchedStruct->MsgTxTaskMsg19);
	    pMsgTxTaskMsg24A = &(pPeriodSchedStruct->MsgTxTaskMsg24A);
	    pMsgTxTaskMsg24B = &(pPeriodSchedStruct->MsgTxTaskMsg24B);
	}
	else
	{
		pMsgTxTaskMsg5 = &(pPeriodSchedStruct->MsgTxTaskMsg5);
	}

    currFrame = GetFrameNum();
    currSlot = GetSlotNum();

    switch (pPeriodSchedStruct->CtrlState)
    {
        case ENUM_PERIOD_SCHED_CTRL_STATE_IDLE :
            
            if (GetGpsPowerupFixState() == TRUE)
            {  
				if (DevStaticPara.AisClass == AIS_CLASS_B)    
				{          
	                /*lq 19����Ϣ�״η��䶨ʱ */
	                MSGMAN_MsgTxTaskTxCfg(AISMsgType19, ENUM_AIS_CHANNEL_A,
	                    CalcFrameNum(currFrame, currSlot,
	                        AIS_SLOT_INTERVAL_SEND_TO_TX),
	                    CalcSlotNum(currFrame, currSlot,
	                        AIS_SLOT_INTERVAL_SEND_TO_TX));
	
	                /*lq 24A����Ϣ�״η��䶨ʱ */
	                MSGMAN_MsgTxTaskTxCfg(AISMsgType24A, ENUM_AIS_CHANNEL_A,
	                    CalcFrameNum(pMsgTxTaskMsg19->NominalFrame,
	                        pMsgTxTaskMsg19->NominalSlot,
	                        AIS_SLOT_INTERVAL_MSG24A_TO_MSG19
	                        + AIS_SLOT_INTERVAL_SEND_TO_TX),
	                    CalcSlotNum(pMsgTxTaskMsg19->NominalFrame,
	                        pMsgTxTaskMsg19->NominalSlot,
	                        AIS_SLOT_INTERVAL_MSG24A_TO_MSG19
	                        + AIS_SLOT_INTERVAL_SEND_TO_TX));
	                
	                /*lq 24B����Ϣ�״η��䶨ʱ */
	                MSGMAN_MsgTxTaskTxCfg(AISMsgType24B, ENUM_AIS_CHANNEL_A,
	                    CalcFrameNum(pMsgTxTaskMsg24A->NominalFrame,
	                        pMsgTxTaskMsg24A->NominalSlot,
	                        AIS_SLOT_INTERVAL_MSG24B_TO_MSG24A
	                        + AIS_SLOT_INTERVAL_SEND_TO_TX),
	                    CalcSlotNum(pMsgTxTaskMsg24A->NominalFrame,
	                        pMsgTxTaskMsg24A->NominalSlot,
	                        AIS_SLOT_INTERVAL_MSG24B_TO_MSG24A
	                        + AIS_SLOT_INTERVAL_SEND_TO_TX));
				}
				else
				{
					/*lq 5����Ϣ�״η��䶨ʱ */
	                MSGMAN_MsgTxTaskTxCfg(AISMsgType5, ENUM_AIS_CHANNEL_A,
	                    CalcFrameNum(currFrame, currSlot,
	                        AIS_SLOT_INTERVAL_SEND_TO_TX),
	                    CalcSlotNum(currFrame, currSlot,
	                        AIS_SLOT_INTERVAL_SEND_TO_TX));			                                    
	        	}
				pPeriodSchedStruct->CtrlState =
	                    ENUM_PERIOD_SCHED_CTRL_STATE_RUNNIG;
	                    
                debug_printf("$[%6d:%4d] %s : Ctrl state to RUNNING!\r\n",
                    GetFrameNum(), GetSlotNum(), __func__);
			}

            break;

        case ENUM_PERIOD_SCHED_CTRL_STATE_RUNNIG:
            
            if (DevStaticPara.AisClass == AIS_CLASS_B)    
			{ 
				/*lq 19����Ϣ�����ж� */
	            if (MSGMAN_MsgTxTaskIsSendTimExpired(AISMsgType19, currFrame, currSlot)
	                == TRUE)
	            {
	                SendMsg19(pMsgTxTaskMsg19->Channel, pMsgTxTaskMsg19->TxSlot);
	                MSGMAN_MsgTxTaskNextTxCfg(AISMsgType19);
	            }
	
	            /*lq 24A����Ϣ�����ж� */
	            if (MSGMAN_MsgTxTaskIsSendTimExpired(AISMsgType24A, currFrame, currSlot)
	                == TRUE)
	            {
	                SendMsg24A(pMsgTxTaskMsg24A->Channel, pMsgTxTaskMsg24A->TxSlot);
	                MSGMAN_MsgTxTaskNextTxCfg(AISMsgType24A);
	            }
	
	            /*lq 24B����Ϣ�����ж� */
	            if (MSGMAN_MsgTxTaskIsSendTimExpired(AISMsgType24B, currFrame, currSlot)
	                == TRUE)
	            {
	                SendMsg24B(pMsgTxTaskMsg24B->Channel, pMsgTxTaskMsg24B->TxSlot);
	                MSGMAN_MsgTxTaskNextTxCfg(AISMsgType24B);
	            }
			}
			else
			{	
				/*lq 5����Ϣ�����ж� */
				if (MSGMAN_MsgTxTaskIsSendTimExpired(AISMsgType5, currFrame, currSlot)
	                == TRUE)
	            {
	                SendMsg5(pMsgTxTaskMsg5->Channel, pMsgTxTaskMsg5->TxSlot);
	                MSGMAN_MsgTxTaskNextTxCfg(AISMsgType5);
				}
			}

            break;
        
        default :
            
            MSGMAN_PeriodSchedStructInit(pPeriodSchedStruct);
            
            break;
    }
        
}

/*******************************************************************************
* ����  : MSGMAN_MsgStaticListStructInit
* ����  : ��ʼ���ṹ��
* ����  : pMsgStaticListStruct - MsgStaticListStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void MSGMAN_MsgStaticListStructInit(
    MsgStaticListStruct *pMsgStaticListStruct)
{
    S16 i;

    for (i = 0; i < MSG_MAN_MSG_STATIC_LIST_LEN; i++)
    {
        MSGMAN_MsgCellStructInit(&(pMsgStaticListStruct->StaticList[i].MsgCell));
        pMsgStaticListStruct->StaticList[i].NextIndex = i + 1;
    }
    pMsgStaticListStruct->StaticList[MSG_MAN_MSG_STATIC_LIST_LEN - 1].NextIndex
        = STATIC_LIST_INDEX_NULL;

    pMsgStaticListStruct->Head = STATIC_LIST_INDEX_NULL;
    pMsgStaticListStruct->FreeHead = 0;
}

/*******************************************************************************
* ����  : MSGMAN_MsgCellStructCopy
* ����  : �ṹ�帴��
* ����  : pMsgCellDst - Ŀ��
          pMsgCellSrc - Դ
* ���  : ��
* ����  : ��
*******************************************************************************/
static void MSGMAN_MsgCellStructCopy(MsgCellStruct *pMsgCellDst,
                                     const MsgCellStruct *pMsgCellSrc)
{
    S16 i;

    pMsgCellDst->MsgType = pMsgCellSrc->MsgType;
    pMsgCellDst->MsgPriority = pMsgCellSrc->MsgPriority;
    pMsgCellDst->Channel = pMsgCellSrc->Channel;
    pMsgCellDst->Slot = pMsgCellSrc->Slot;
    pMsgCellDst->Frame = pMsgCellSrc->Frame;
    pMsgCellDst->MsgState = pMsgCellSrc->MsgState;
	pMsgCellDst->DataLength = pMsgCellSrc->DataLength;   
    for (i = 0; i < pMsgCellSrc->DataLength; i++)
    {
        pMsgCellDst->Data[i] = pMsgCellSrc->Data[i];
    }
}

/*******************************************************************************
* ����  : MSGMAN_MsgStaticListIsEmpty
* ����  : ��̬�����Ƿ�Ϊ��
* ����  : pMsgStaticListStruct - MsgStaticListStruct�ṹ��ָ��	  
* ���  : ��
* ����  : �Ƿ�Ϊ��
*******************************************************************************/
static U8 MSGMAN_MsgStaticListIsEmpty(MsgStaticListStruct *pMsgStaticListStruct)
{
    if (pMsgStaticListStruct->Head == STATIC_LIST_INDEX_NULL)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : MSGMAN_MsgStaticListIsFull
* ����  : ��̬�����Ƿ�Ϊ��
* ����  : pMsgStaticListStruct - MsgStaticListStruct�ṹ��ָ��	  
* ���  : ��
* ����  : �Ƿ�����
*******************************************************************************/
static U8 MSGMAN_MsgStaticListIsFull(MsgStaticListStruct *pMsgStaticListStruct)
{
    if (pMsgStaticListStruct->FreeHead == STATIC_LIST_INDEX_NULL)
    {
        return TRUE;

        debug_printf("$[%6d:%4d] %s : Msg static list is full!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : MSGMAN_MsgStaticListInsert
* ����  : ��̬�������Ԫ�أ�����Ϣ���ȼ�����
* ����  : pMsgStaticListStruct - ��̬����ָ��	  
          pMsgCellStruct - ָ���Ų���Ԫ��ָ��
* ���  : ��
* ����  : �Ƿ����ɹ�
*******************************************************************************/
static U8 MSGMAN_MsgStaticListInsert(MsgStaticListStruct *pMsgStaticListStruct,
                                     const MsgCellStruct *pMsgCellStruct)
{
    S16 prior;
    S16 curr;
    S16 temp;

    if (MSGMAN_MsgStaticListIsFull(pMsgStaticListStruct) == FALSE)
    {
        prior = STATIC_LIST_INDEX_NULL;
        curr = pMsgStaticListStruct->Head;
        
        /*lq �������ȼ�ѡ������ */
        while (curr != STATIC_LIST_INDEX_NULL)
        {
            if (pMsgCellStruct->MsgPriority
                >= pMsgStaticListStruct->StaticList[curr].MsgCell.MsgPriority)
            {
                prior = curr;
                curr = pMsgStaticListStruct->StaticList[curr].NextIndex;
            }
            else
            {
                /*lq �����Ϊcurr֮ǰ */
                break;
            }            
        }

        /*lq �������Ԫ�ظ�ֵ */
        MSGMAN_MsgCellStructCopy(&(pMsgStaticListStruct->StaticList[
            pMsgStaticListStruct->FreeHead].MsgCell),
            pMsgCellStruct);
        
        /*lq ������������ */
        if (curr == STATIC_LIST_INDEX_NULL)
        {
            /*lq ��β������ */
            if (prior == STATIC_LIST_INDEX_NULL)
            {
                /*lq ����Ϊ�� */
                pMsgStaticListStruct->Head = pMsgStaticListStruct->FreeHead;
                
                pMsgStaticListStruct->FreeHead
                    = pMsgStaticListStruct->StaticList[
                    pMsgStaticListStruct->FreeHead].NextIndex;
                
                pMsgStaticListStruct->StaticList[pMsgStaticListStruct->Head].NextIndex
                    = STATIC_LIST_INDEX_NULL;
            }
            else
            {
                /*lq ����ǿ� */
                pMsgStaticListStruct->StaticList[prior].NextIndex
                    = pMsgStaticListStruct->FreeHead;
                
                pMsgStaticListStruct->FreeHead
                    = pMsgStaticListStruct->StaticList[
                    pMsgStaticListStruct->FreeHead].NextIndex;
                
                pMsgStaticListStruct->StaticList[
                    pMsgStaticListStruct->StaticList[prior].NextIndex].NextIndex
                    = STATIC_LIST_INDEX_NULL;
            }
        }
        else
        {
            /*lq �������в��� */ 
            if (prior == STATIC_LIST_INDEX_NULL)
            {
                /*lq ������ͷ֮ǰ���� */
                temp = pMsgStaticListStruct->FreeHead;
                pMsgStaticListStruct->FreeHead = pMsgStaticListStruct->StaticList[temp].NextIndex;
                pMsgStaticListStruct->StaticList[temp].NextIndex = pMsgStaticListStruct->Head;
                pMsgStaticListStruct->Head = temp;
            }
            else
            {
                temp = pMsgStaticListStruct->FreeHead;
                pMsgStaticListStruct->FreeHead = pMsgStaticListStruct->StaticList[temp].NextIndex;
                pMsgStaticListStruct->StaticList[temp].NextIndex = curr;
                pMsgStaticListStruct->StaticList[prior].NextIndex = temp;               
            }
        }

        return TRUE;
    } 
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : MSGMAN_MsgStaticListInsert
* ����  : ��ȡ��ɾ����ͷԪ��
* ����  : pMsgStaticListStruct - ��̬����ָ��
* ���  : pMsgCellStruct - ָ���ŷ���Ԫ�ص�ָ��
* ����  : �Ƿ��ȡ�ɹ�
*******************************************************************************/
static U8 MSGMAN_MsgStaticListGetHead(MsgStaticListStruct *pMsgStaticListStruct,
                                      MsgCellStruct *pMsgCellStruct)
{
    S16 temp;

    if (MSGMAN_MsgStaticListIsEmpty(pMsgStaticListStruct) == FALSE)
    {
        /*lq ��ȡԪ�ظ�ֵ */
        MSGMAN_MsgCellStructCopy(pMsgCellStruct,
            &(pMsgStaticListStruct->StaticList[
            pMsgStaticListStruct->Head].MsgCell));

        /*lq ���¾�̬�������� */
        temp = pMsgStaticListStruct->Head;
        pMsgStaticListStruct->Head = pMsgStaticListStruct->StaticList[temp].NextIndex;
        pMsgStaticListStruct->StaticList[temp].NextIndex = pMsgStaticListStruct->FreeHead;
        pMsgStaticListStruct->FreeHead = temp;
        
        return TRUE;
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* ����  : MSGMAN_TxMsgListIsEmpty
* ����  : ��Ϣ���������Ƿ�Ϊ��
* ����  : ��
* ���  : ��
* ����  : �Ƿ�Ϊ��
*******************************************************************************/
U8 MSGMAN_TxMsgListIsEmpty(void)
{
    return MSGMAN_MsgStaticListIsEmpty(&TxMsgList);
}

/*******************************************************************************
* ����  : MSGMAN_RxMsgListIsEmpty
* ����  : ��Ϣ���������Ƿ�Ϊ��
* ����  : ��
* ���  : ��
* ����  : �Ƿ�Ϊ��
*******************************************************************************/
U8 MSGMAN_RxMsgListIsEmpty(void)
{
    return MSGMAN_MsgStaticListIsEmpty(&RxMsgList);
}

/*******************************************************************************
* ����  : MSGMAN_TxMsgListIsFull
* ����  : ��Ϣ���������Ƿ�����
* ����  : ��
* ���  : ��
* ����  : �Ƿ�����
*******************************************************************************/
U8 MSGMAN_TxMsgListIsFull(void)
{
    return MSGMAN_MsgStaticListIsFull(&TxMsgList);
}

/*******************************************************************************
* ����  : MSGMAN_RxMsgListIsFull
* ����  : ��Ϣ���������Ƿ�����
* ����  : ��
* ���  : ��
* ����  : �Ƿ�����
*******************************************************************************/
U8 MSGMAN_RxMsgListIsFull(void)
{
    return MSGMAN_MsgStaticListIsFull(&RxMsgList);
}

/*******************************************************************************
* ����  : MSGMAN_TxMsgListInsert
* ����  : ��Ϣ�����������Ԫ�أ�����Ϣ���ȼ�����
* ����  : pMsgCellStruct - ָ���Ų���Ԫ��ָ��
* ���  : ��
* ����  : �Ƿ����ɹ�
*******************************************************************************/
U8 MSGMAN_TxMsgListInsert(const MsgCellStruct *pMsgCellStruct)
{
    return MSGMAN_MsgStaticListInsert(&TxMsgList, pMsgCellStruct);
}

/*******************************************************************************
* ����  : MSGMAN_RxMsgListInsert
* ����  : ��Ϣ�����������Ԫ��
* ����  : pMsgCellStruct - ָ���Ų���Ԫ��ָ��
* ���  : ��
* ����  : �Ƿ����ɹ�
*******************************************************************************/
U8 MSGMAN_RxMsgListInsert(const MsgCellStruct *pMsgCellStruct)
{
    return MSGMAN_MsgStaticListInsert(&RxMsgList, pMsgCellStruct);
}

/*******************************************************************************
* ����  : MSGMAN_TxMsgListGetElem
* ����  : ��Ϣ���������ȡԪ��
* ����  : ��
* ���  : pMsgCellStruct - ָ���ŷ���Ԫ�ص�ָ��
* ����  : �Ƿ����ɹ�
*******************************************************************************/
U8 MSGMAN_TxMsgListGetElem(MsgCellStruct *pMsgCellStruct)
{
    return MSGMAN_MsgStaticListGetHead(&TxMsgList, pMsgCellStruct);
}

/*******************************************************************************
* ����  : MSGMAN_RxMsgListGetElem
* ����  : ��Ϣ���������ȡԪ��
* ����  : ��
* ���  : pMsgCellStruct - ָ���ŷ���Ԫ�ص�ָ��
* ����  : �Ƿ����ɹ�
*******************************************************************************/
U8 MSGMAN_RxMsgListGetElem(MsgCellStruct *pMsgCellStruct)
{
    return MSGMAN_MsgStaticListGetHead(&RxMsgList, pMsgCellStruct);
}

/*******************************************************************************
* ����  : MSGMAN_AisAckFifoIsEmpty
* ����  : �����Ƿ�Ϊ��
* ����  : pAisAckFifoStruct - AisAckFifoStruct�ṹ��ָ��		  
* ���  : ��
* ����  : TRUE - Ϊ�գ�FALSE - �ǿ�
*******************************************************************************/
static U8 MSGMAN_AisAckFifoIsEmpty(AisAckFifoStruct *pAisAckFifoStruct)
{
    if (pAisAckFifoStruct->Input == pAisAckFifoStruct->Output)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* ����  : MSGMAN_AisAckFifoIsFull
* ����  : �����Ƿ�����
* ����  : pAisAckFifoStruct - AisAckFifoStruct�ṹ��ָ��		  
* ���  : ��
* ����  : TRUE - ������FALSE - δ��
*******************************************************************************/
static U8 MSGMAN_AisAckFifoIsFull(AisAckFifoStruct *pAisAckFifoStruct)
{
    if ((pAisAckFifoStruct->Input + 1) % MSG_MAN_ACK_FIFO_LEN
        == pAisAckFifoStruct->Output)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }     
}

/*******************************************************************************
* ����  : MSGMAN_AisAckFifoEnque
* ����  : �������
* ����  : pAisAckFifoStruct - AisAckFifoStruct�ṹ��ָ��
          pAisAckCellStruct - ָ��������ݵ�Ԫ��ָ��         		  
* ���  : ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
static U8 MSGMAN_AisAckFifoEnque(AisAckFifoStruct *pAisAckFifoStruct,
                                 const AisAckCellStruct *pAisAckCellStruct)
{
    AisAckCellStruct *pCell;
//    U16 i;
       
    if (MSGMAN_AisAckFifoIsFull(pAisAckFifoStruct) == FALSE)
    {      
        pCell = &(pAisAckFifoStruct->FIFO[pAisAckFifoStruct->Input]);
        
        pCell->DestinationID = pAisAckCellStruct->DestinationID;
        pCell->SequenceNumber = pAisAckCellStruct->SequenceNumber;
                
        pAisAckFifoStruct->Input = (pAisAckFifoStruct->Input + 1)
            % MSG_MAN_ACK_FIFO_LEN;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }   
}

/*******************************************************************************
* ����  : MSGMAN_AisAckFifoDeque
* ����  : ���г���
* ����  : pAisAckFifoStruct - AisAckFifoStruct�ṹ��ָ��          
* ���  : pAisAckCellStruct - ָ��������ݵ�Ԫ��ָ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
static U8 MSGMAN_AisAckFifoDeque(AisAckFifoStruct *pAisAckFifoStruct,
                                 AisAckCellStruct *pAisAckCellStruct)
{
//    U16 i;
    AisAckCellStruct *pCell;

    if (MSGMAN_AisAckFifoIsEmpty(pAisAckFifoStruct) == FALSE)
    {        
        pCell = &(pAisAckFifoStruct->FIFO[pAisAckFifoStruct->Output]);
        
        pAisAckCellStruct->DestinationID = pCell->DestinationID;
        pAisAckCellStruct->SequenceNumber = pCell->SequenceNumber;

        pAisAckFifoStruct->Output = (pAisAckFifoStruct->Output + 1)
            % MSG_MAN_ACK_FIFO_LEN;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* ����  : MSGMAN_BurstSchedule
* ����  : ͻ������Ϣ��msg6/msg7/msg8/msg12/msg13/msg14�����͹���
* ����  : pBurstSchedStruct - BurstSchedStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void MSGMAN_BurstSchedule(BurstSchedStruct *pBurstSchedStruct)
{
    AisAckFifoStruct *pMsg6AckA;
    AisAckFifoStruct *pMsg6AckB;
    AisAckFifoStruct *pMsg12AckA;
    AisAckFifoStruct *pMsg12AckB;
    AisAckCellStruct ackCell;
    U32 DestIDs[AIS_ACK_ONCE_QUANTITY_MAX];
    U8  SeqNumber[AIS_ACK_ONCE_QUANTITY_MAX];
    U8  ackType;
    S16 i;

    pMsg6AckA = &(pBurstSchedStruct->Msg6AckA);
    pMsg6AckB = &(pBurstSchedStruct->Msg6AckB);
    pMsg12AckA = &(pBurstSchedStruct->Msg12AckA);
    pMsg12AckB = &(pBurstSchedStruct->Msg12AckB);

    /*lq ���ŵ�A�ϵ�6����Ϣ����ȷ�� */
    if (MSGMAN_AisAckFifoIsEmpty(pMsg6AckA) != TRUE)
    {        
        /*lq ��ȡȷ����Ϣ */
        for (i = 0; i < AIS_ACK_ONCE_QUANTITY_MAX; i++)
        {
            if (MSGMAN_AisAckFifoDeque(pMsg6AckA, &ackCell) == TRUE)
            {
                DestIDs[i] = ackCell.DestinationID;
                SeqNumber[i] = ackCell.SequenceNumber;
            } 
            else
            {
                break;
            }
        }

        /*lq �ж�ȷ�����ͣ������� */
        if (i == 1)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_1;
        }
        else if (i == 2)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_2;        
        }
        else if (i == 3)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_3;        
        }
        else
        {
            ackType = AIS_MSG7_13_ACK_TYPE_4;        
        }

        /*lq ����ȷ����Ϣ */
        SendMsg7(ENUM_AIS_CHANNEL_A, SLOT_FORCE_TX_VALUE, ackType,
            DestIDs, SeqNumber);
    }

    /*lq ���ŵ�B�ϵ�6����Ϣ����ȷ�� */
    if (MSGMAN_AisAckFifoIsEmpty(pMsg6AckB) != TRUE)
    {        
        /*lq ��ȡȷ����Ϣ */
        for (i = 0; i < AIS_ACK_ONCE_QUANTITY_MAX; i++)
        {
            if (MSGMAN_AisAckFifoDeque(pMsg6AckB, &ackCell) == TRUE)
            {
                DestIDs[i] = ackCell.DestinationID;
                SeqNumber[i] = ackCell.SequenceNumber;
            } 
            else
            {
                break;
            }
        }

        /*lq �ж�ȷ�����ͣ������� */
        if (i == 1)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_1;
        }
        else if (i == 2)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_2;        
        }
        else if (i == 3)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_3;        
        }
        else
        {
            ackType = AIS_MSG7_13_ACK_TYPE_4;        
        }

        /*lq ����ȷ����Ϣ */
        SendMsg7(ENUM_AIS_CHANNEL_B, SLOT_FORCE_TX_VALUE, ackType,
            DestIDs, SeqNumber);
    }

    /*lq ���ŵ�A�ϵ�12����Ϣ����ȷ�� */
    if (MSGMAN_AisAckFifoIsEmpty(pMsg12AckA) != TRUE)
    {        
        /*lq ��ȡȷ����Ϣ */
        for (i = 0; i < AIS_ACK_ONCE_QUANTITY_MAX; i++)
        {
            if (MSGMAN_AisAckFifoDeque(pMsg12AckA, &ackCell) == TRUE)
            {
                DestIDs[i] = ackCell.DestinationID;
                SeqNumber[i] = ackCell.SequenceNumber;
            } 
            else
            {
                break;
            }
        }

        /*lq �ж�ȷ�����ͣ������� */
        if (i == 1)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_1;
        }
        else if (i == 2)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_2;        
        }
        else if (i == 3)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_3;        
        }
        else
        {
            ackType = AIS_MSG7_13_ACK_TYPE_4;        
        }

        /*lq ����ȷ����Ϣ */
        SendMsg13(ENUM_AIS_CHANNEL_A, SLOT_FORCE_TX_VALUE, ackType,
            DestIDs, SeqNumber);
    }

    /*lq ���ŵ�B�ϵ�12����Ϣ����ȷ�� */
    if (MSGMAN_AisAckFifoIsEmpty(pMsg12AckB) != TRUE)
    {        
        /*lq ��ȡȷ����Ϣ */
        for (i = 0; i < AIS_ACK_ONCE_QUANTITY_MAX; i++)
        {
            if (MSGMAN_AisAckFifoDeque(pMsg12AckB, &ackCell) == TRUE)
            {
                DestIDs[i] = ackCell.DestinationID;
                SeqNumber[i] = ackCell.SequenceNumber;
            } 
            else
            {
                break;
            }
        }

        /*lq �ж�ȷ�����ͣ������� */
        if (i == 1)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_1;
        }
        else if (i == 2)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_2;        
        }
        else if (i == 3)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_3;        
        }
        else
        {
            ackType = AIS_MSG7_13_ACK_TYPE_4;        
        }

        /*lq ����ȷ����Ϣ */
        SendMsg13(ENUM_AIS_CHANNEL_B, SLOT_FORCE_TX_VALUE, ackType,
            DestIDs, SeqNumber);
    }

    /*lq ����ͨ��ABM������õ���Ϣ��Msg6��Msg12�� */
    if (AISDI_ABMIsValid(&ABMPara) == TRUE)
    {
        ABMPara.ValidFlg = FALSE;

        if (AISMSG_ConvertToMsgType(ABMPara.MsgID, 0) == AISMsgType6)
        {
            SendMsg6(ABMPara.Channel, SLOT_FORCE_TX_VALUE);
        }
        else if (AISMSG_ConvertToMsgType(ABMPara.MsgID, 0) == AISMsgType12)
        {
            SendMsg12(ABMPara.Channel, SLOT_FORCE_TX_VALUE);
        }
    }

    /*lq ����ͨ��BBM������õ���Ϣ��Msg8��Msg14�� */
    if (AISDI_BBMIsValid(&BBMPara) == TRUE)
    {
        BBMPara.ValidFlg = FALSE;

        if (AISMSG_ConvertToMsgType(BBMPara.MsgID, 0) == AISMsgType8)
        {
            SendMsg8(BBMPara.Channel, SLOT_FORCE_TX_VALUE);
        }
        else if (AISMSG_ConvertToMsgType(BBMPara.MsgID, 0) == AISMsgType14)
        {
            SendMsg14(BBMPara.Channel, SLOT_FORCE_TX_VALUE);
        }
    }
}

/*******************************************************************************
* ����  : SendMsg1
* ����  : ����1����Ϣ
* ����  : channel - �����ŵ�
          txSlot - ����ʱ϶
          commState - ͨ��״̬
* ���  : ��
* ����  : �Ƿ��ͳɹ�
*******************************************************************************/
U8 SendMsg1(AisChannelEnum channel, S16 txSlot, AISMsgCommStateUnion commState)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq ��ʼ����Ϣ */
    InitMsg1(commState);

    /*lq ��ʼ����Ϣ��Ԫ */
    msgCell.MsgType = AISMsgType1;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_1;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq ����ʱ����
    //lq msgCell.MsgState = ;   //lq ����ʱ����
	for (i = 0; i < AISMsgBinBuff_1[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_1[i];
    }
	msgCell.DataLength = AISMsgBinBuff_1[0] + 1;

    /*lq ������Ϣ�����б� */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SendMsg3
* ����  : ����3����Ϣ
* ����  : channel - �����ŵ�
          txSlot - ����ʱ϶
          commState - ͨ��״̬
* ���  : ��
* ����  : �Ƿ��ͳɹ�
*******************************************************************************/
U8 SendMsg3(AisChannelEnum channel, S16 txSlot, AISMsgCommStateUnion commState)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq ��ʼ����Ϣ */
    InitMsg3(commState);

    /*lq ��ʼ����Ϣ��Ԫ */
    msgCell.MsgType = AISMsgType3;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_1;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq ����ʱ����
    //lq msgCell.MsgState = ;   //lq ����ʱ����
	for (i = 0; i < AISMsgBinBuff_3[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_3[i];
    }
	msgCell.DataLength = AISMsgBinBuff_3[0] + 1;

    /*lq ������Ϣ�����б� */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SendMsg5
* ����  : ����5����Ϣ
* ����  : channel - �����ŵ�
          txSlot - ����ʱ϶
* ���  : ��
* ����  : �Ƿ��ͳɹ�
*******************************************************************************/
U8 SendMsg5(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq ��ʼ����Ϣ */
    InitMsg5();

    /*lq ��ʼ����Ϣ��Ԫ */
    msgCell.MsgType = AISMsgType5;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_4;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq ����ʱ����
    //lq msgCell.MsgState = ;   //lq ����ʱ����
	for (i = 0; i < AISMsgBinBuff_5[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_5[i];
    }
	msgCell.DataLength = AISMsgBinBuff_5[0] + 1;

    /*lq ������Ϣ�����б� */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SendMsg6
* ����  : ����6����Ϣ
* ����  : channel - �����ŵ�
          txSlot - ����ʱ϶
* ���  : ��
* ����  : �Ƿ��ͳɹ�
*******************************************************************************/
U8 SendMsg6(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq ��ʼ����Ϣ */
    InitMsg6();

    /*lq ��ʼ����Ϣ��Ԫ */
    msgCell.MsgType = AISMsgType6;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_4;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq ����ʱ����
    //lq msgCell.MsgState = ;   //lq ����ʱ����
	for (i = 0; i < AISMsgBinBuff_6[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_6[i];
    }
	msgCell.DataLength = AISMsgBinBuff_6[0] + 1;

    /*lq ������Ϣ�����б� */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SendMsg6Test
* ����  : ����6�Ų�����Ϣ
* ����  : channel - �����ŵ�
          txSlot - ����ʱ϶
* ���  : ��
* ����  : �Ƿ��ͳɹ�
*******************************************************************************/
U8 SendMsg6Test(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq ��ʼ����Ϣ */
    InitMsg6Test();

    /*lq ��ʼ����Ϣ��Ԫ */
    msgCell.MsgType = AISMsgType6;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_4;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq ����ʱ����
    //lq msgCell.MsgState = ;   //lq ����ʱ����
	for (i = 0; i < AISMsgBinBuff_6[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_6[i];
    }
	msgCell.DataLength = AISMsgBinBuff_6[0] + 1;

    /*lq ������Ϣ�����б� */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SendMsg7
* ����  : ����7����Ϣ
* ����  : channel - �����ŵ�
          txSlot - ����ʱ϶
          ackType - ȷ������
          pDestinationID - ȷ����Ϣ��Ŀ��ID����ָ��
          pSequenceNumber - ȷ����Ϣ���������ָ��
* ���  : ��
* ����  : �Ƿ��ͳɹ�
*******************************************************************************/
U8 SendMsg7(AisChannelEnum channel, S16 txSlot, U8 ackType, U32 *pDestinationID,
            U8 *pSequenceNumber)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq ��ʼ����Ϣ */
    InitMsg7(ackType, pDestinationID, pSequenceNumber);

    /*lq ��ʼ����Ϣ��Ԫ */
    msgCell.MsgType = AISMsgType7;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_1;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq ����ʱ����
    //lq msgCell.MsgState = ;   //lq ����ʱ����
	for (i = 0; i < AISMsgBinBuff_7[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_7[i];
    }
	msgCell.DataLength = AISMsgBinBuff_7[0] + 1;

    /*lq ������Ϣ�����б� */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SendMsg8
* ����  : ����8����Ϣ
* ����  : channel - �����ŵ�
          txSlot - ����ʱ϶
* ���  : ��
* ����  : �Ƿ��ͳɹ�
*******************************************************************************/
U8 SendMsg8(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq ��ʼ����Ϣ */
    InitMsg8();

    /*lq ��ʼ����Ϣ��Ԫ */
    msgCell.MsgType = AISMsgType8;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_4;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq ����ʱ����
    //lq msgCell.MsgState = ;   //lq ����ʱ����
	for (i = 0; i < AISMsgBinBuff_8[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_8[i];
    }
	msgCell.DataLength = AISMsgBinBuff_8[0] + 1;

    /*lq ������Ϣ�����б� */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SendMsg12
* ����  : ����12����Ϣ
* ����  : channel - �����ŵ�
          txSlot - ����ʱ϶
* ���  : ��
* ����  : �Ƿ��ͳɹ�
*******************************************************************************/
U8 SendMsg12(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq ��ʼ����Ϣ */
    InitMsg12();

    /*lq ��ʼ����Ϣ��Ԫ */
    msgCell.MsgType = AISMsgType12;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_2;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq ����ʱ����
    //lq msgCell.MsgState = ;   //lq ����ʱ����
	for (i = 0; i < AISMsgBinBuff_12[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_12[i];
    }
	msgCell.DataLength = AISMsgBinBuff_12[0] + 1;

    /*lq ������Ϣ�����б� */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SendMsg13
* ����  : ����13����Ϣ
* ����  : channel - �����ŵ�
          txSlot - ����ʱ϶
          ackType - ȷ������
          pDestinationID - ȷ����Ϣ��Ŀ��ID����ָ��
          pSequenceNumber - ȷ����Ϣ���������ָ��
* ���  : ��
* ����  : �Ƿ��ͳɹ�
*******************************************************************************/
U8 SendMsg13(AisChannelEnum channel, S16 txSlot, U8 ackType, U32 *pDestinationID,
            U8 *pSequenceNumber)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq ��ʼ����Ϣ */
    InitMsg13(ackType, pDestinationID, pSequenceNumber);

    /*lq ��ʼ����Ϣ��Ԫ */
    msgCell.MsgType = AISMsgType13;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_1;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq ����ʱ����
    //lq msgCell.MsgState = ;   //lq ����ʱ����
	for (i = 0; i < AISMsgBinBuff_13[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_13[i];
    }
	msgCell.DataLength = AISMsgBinBuff_13[0] + 1;

    /*lq ������Ϣ�����б� */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SendMsg14
* ����  : ����14����Ϣ
* ����  : channel - �����ŵ�
          txSlot - ����ʱ϶
* ���  : ��
* ����  : �Ƿ��ͳɹ�
*******************************************************************************/
U8 SendMsg14(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq ��ʼ����Ϣ */
    InitMsg14();

    /*lq ��ʼ����Ϣ��Ԫ */
    msgCell.MsgType = AISMsgType14;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_2;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq ����ʱ����
    //lq msgCell.MsgState = ;   //lq ����ʱ����
	for (i = 0; i < AISMsgBinBuff_14[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_14[i];
    }
	msgCell.DataLength = AISMsgBinBuff_14[0] + 1;

    /*lq ������Ϣ�����б� */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SendMsg18
* ����  : ����18����Ϣ
* ����  : channel - �����ŵ�
          txSlot - ����ʱ϶
          commStateType - ͨ��״̬����
          commState - ͨ��״̬ 
* ���  : ��
* ����  : �Ƿ��ͳɹ�
*******************************************************************************/
U8 SendMsg18(AisChannelEnum channel, S16 txSlot,
             AISMsgCommStateEnum commStateType, AISMsgCommStateUnion commState)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq ��ʼ����Ϣ */
    InitMsg18(commStateType, commState);

    /*lq ��ʼ����Ϣ��Ԫ */
    msgCell.MsgType = AISMsgType18;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_1;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq ����ʱ����
    //lq msgCell.MsgState = ;   //lq ����ʱ����
	for (i = 0; i < AISMsgBinBuff_18[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_18[i];
    }
	msgCell.DataLength = AISMsgBinBuff_18[0] + 1;

    /*lq ������Ϣ�����б� */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded at channel %s , tx slot [%4d]!\r\n",
            GetFrameNum(), GetSlotNum(), __func__,
            channel == ENUM_AIS_CHANNEL_A ? AIS_CHANNEL_NAME_A : AIS_CHANNEL_NAME_B,
            txSlot);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed at channel %s , tx slot [%4d]!\r\n",
            GetFrameNum(), GetSlotNum(), __func__,
            channel == ENUM_AIS_CHANNEL_A ? AIS_CHANNEL_NAME_A : AIS_CHANNEL_NAME_B,
            txSlot);
    
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SendMsg18
* ����  : ����18����Ϣ
* ����  : channel - �����ŵ�
          txSlot - ����ʱ϶
          commStateType - ͨ��״̬����
          commState - ͨ��״̬ 
* ���  : ��
* ����  : �Ƿ��ͳɹ�
*******************************************************************************/
U8 SendMsg18Test(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq ��ʼ����Ϣ */
    InitMsg18Test();

    /*lq ��ʼ����Ϣ��Ԫ */
    msgCell.MsgType = AISMsgType18;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_1;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq ����ʱ����
    //lq msgCell.MsgState = ;   //lq ����ʱ����
	for (i = 0; i < AISMsgBinBuff_18[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_18[i];
    }
	msgCell.DataLength = AISMsgBinBuff_18[0] + 1;

    /*lq ������Ϣ�����б� */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded at channel %s , tx slot [%4d]!\r\n",
            GetFrameNum(), GetSlotNum(), __func__,
            channel == ENUM_AIS_CHANNEL_A ? AIS_CHANNEL_NAME_A : AIS_CHANNEL_NAME_B,
            txSlot);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed at channel %s , tx slot [%4d]!\r\n",
            GetFrameNum(), GetSlotNum(), __func__,
            channel == ENUM_AIS_CHANNEL_A ? AIS_CHANNEL_NAME_A : AIS_CHANNEL_NAME_B,
            txSlot);
    
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SendMsg19
* ����  : ����19����Ϣ
* ����  : channel - �����ŵ�
          txSlot - ����ʱ϶
* ���  : ��
* ����  : �Ƿ��ͳɹ�
*******************************************************************************/
U8 SendMsg19(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq ��ʼ����Ϣ */
    InitMsg19();

    /*lq ��ʼ����Ϣ��Ԫ */
    msgCell.MsgType = AISMsgType19;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_1;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq ����ʱ����
    //lq msgCell.MsgState = ;   //lq ����ʱ����
	for (i = 0; i < AISMsgBinBuff_19[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_19[i];
    }
	msgCell.DataLength = AISMsgBinBuff_19[0] + 1;

    /*lq ������Ϣ�����б� */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SendMsg24A
* ����  : ����24A����Ϣ
* ����  : channel - �����ŵ�
          txSlot - ����ʱ϶
* ���  : ��
* ����  : �Ƿ��ͳɹ�
*******************************************************************************/
U8 SendMsg24A(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq ��ʼ����Ϣ */
    InitMsg24A();

    /*lq ��ʼ����Ϣ��Ԫ */
    msgCell.MsgType = AISMsgType24A;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_4;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq ����ʱ����
    //lq msgCell.MsgState = ;   //lq ����ʱ����
	for (i = 0; i < AISMsgBinBuff_24A[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_24A[i];
    }
	msgCell.DataLength = AISMsgBinBuff_24A[0] + 1;

    /*lq ������Ϣ�����б� */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SendMsg24B
* ����  : ����24B����Ϣ
* ����  : channel - �����ŵ�
          txSlot - ����ʱ϶
* ���  : ��
* ����  : �Ƿ��ͳɹ�
*******************************************************************************/
U8 SendMsg24B(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq ��ʼ����Ϣ */
    InitMsg24B();

    /*lq ��ʼ����Ϣ��Ԫ */
    msgCell.MsgType = AISMsgType24B;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_4;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq ����ʱ����
    //lq msgCell.MsgState = ;   //lq ����ʱ����
	for (i = 0; i < AISMsgBinBuff_24B[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_24B[i];
    }
	msgCell.DataLength = AISMsgBinBuff_24B[0] + 1;

    /*lq ������Ϣ�����б� */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* ����  : MSGMAN_MsgTxProcess
* ����  : ��Ϣ���͹���ģ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void MSGMAN_MsgTxProcess(void) 
{
    /*lq �ɱ���������Ϣ��msg18������ */
    TDMA_InstSchedule();
    
    /*lq �̶���������Ϣ��msg19/msg24A/msg24B������ */
    MSGMAN_PeriodSchedule(&PeriodSched);

    /*lq ͻ������Ϣ��msg6/msg7/msg8/msg12/msg13/msg14������ */
    MSGMAN_BurstSchedule(&BurstSched);
}

/*******************************************************************************
* ����  : MSGMAN_MsgRxProcess
* ����  : ��Ϣ���չ���ģ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void MSGMAN_MsgRxProcess(void) 
{
    MsgCellStruct msgCell;
    AisAckCellStruct aisAckCell;
    AISMsgCommStateEnum commStateType;

    if (MSGMAN_RxMsgListGetElem(&msgCell) == TRUE)
    {
        switch (AISMSG_ParseMsg(&msgCell.Data[0]))
        {
            case AISMsgType1 :
                
                /*lq ʱ϶Ԥռ���� */
                TDMA_SetSlotState(msgCell.Channel, AIS_MSG_COMMSTATE_SOTDMA,
                    AISMsg_1.CommunicationState, msgCell.Frame, msgCell.Slot,
                    SLOT_STATE_EXTER_ALLOC);

                break;

            case AISMsgType2 :
                
                /*lq ʱ϶Ԥռ���� */
                TDMA_SetSlotState(msgCell.Channel, AIS_MSG_COMMSTATE_SOTDMA,
                    AISMsg_2.CommunicationState, msgCell.Frame, msgCell.Slot,
                    SLOT_STATE_EXTER_ALLOC);

                break;

            case AISMsgType3 :
                
                /*lq ʱ϶Ԥռ���� */
                TDMA_SetSlotState(msgCell.Channel, AIS_MSG_COMMSTATE_ITDMA,
                    AISMsg_3.CommunicationState, msgCell.Frame, msgCell.Slot,
                    SLOT_STATE_EXTER_ALLOC);

                break;

            case AISMsgType4 :
                
                /*lq ʱ϶Ԥռ���� */
                TDMA_SetSlotState(msgCell.Channel, AIS_MSG_COMMSTATE_SOTDMA,
                    AISMsg_4.CommunicationState, msgCell.Frame, msgCell.Slot,
                    SLOT_STATE_EXTER_ALLOC);

                break;

            case AISMsgType5 :
                
                /*lq ���մ�̨���� */
                AISINFO_InstRecStationQuantityIncrease();

                break;

            case AISMsgType6 :
                
                /*lq ��Ϣȷ�� */
                if (AISMsg_6.DestinationID == AisStaticPara.MMSI)
                {
                    aisAckCell.DestinationID = AISMsg_6.SourceID;                
                    aisAckCell.SequenceNumber = AISMsg_6.SequenceNumber;
                    
                    if (msgCell.Channel == ENUM_AIS_CHANNEL_A)
                    {
                        MSGMAN_AisAckFifoEnque(&BurstSched.Msg6AckA,
                            &aisAckCell);                
                    }
                    else
                    {
                        MSGMAN_AisAckFifoEnque(&BurstSched.Msg6AckB,
                            &aisAckCell);                
                    }
                }
                
                break;

            case AISMsgType9 :
                
                /*lq ʱ϶Ԥռ���� */
                if (AISMsg_9.CommStateSelectFlag == 0)
                {
                    commStateType = AIS_MSG_COMMSTATE_SOTDMA;
                }
                else
                {
                    commStateType = AIS_MSG_COMMSTATE_ITDMA;
                }

                TDMA_SetSlotState(msgCell.Channel, commStateType,
                    AISMsg_9.CommunicationState, msgCell.Frame, msgCell.Slot,
                    SLOT_STATE_EXTER_ALLOC);

                break;

            case AISMsgType11 :
                
                /*lq ʱ϶Ԥռ���� */
                TDMA_SetSlotState(msgCell.Channel, AIS_MSG_COMMSTATE_SOTDMA,
                    AISMsg_11.CommunicationState, msgCell.Frame, msgCell.Slot,
                    SLOT_STATE_EXTER_ALLOC);

                break;

            case AISMsgType12 :
                
                /*lq ��Ϣȷ�� */
                if (AISMsg_12.DestinationID == AisStaticPara.MMSI)
                {
                    aisAckCell.DestinationID = AISMsg_12.SourceID;                
                    aisAckCell.SequenceNumber = AISMsg_12.SequenceNumber;
                    
                    if (msgCell.Channel == ENUM_AIS_CHANNEL_A)
                    {
                        MSGMAN_AisAckFifoEnque(&BurstSched.Msg12AckA,
                            &aisAckCell);                
                    }
                    else
                    {
                        MSGMAN_AisAckFifoEnque(&BurstSched.Msg12AckB,
                            &aisAckCell);                
                    }
                }

                break;

            case AISMsgType18 :
                
                /*lq ʱ϶Ԥռ���� */
                if (AISMsg_18.CommStateSelectFlag == 0)
                {
                    commStateType = AIS_MSG_COMMSTATE_SOTDMA;
                }
                else
                {
                    commStateType = AIS_MSG_COMMSTATE_ITDMA;
                }

                TDMA_SetSlotState(msgCell.Channel, commStateType,
                    AISMsg_18.CommunicationState, msgCell.Frame, msgCell.Slot,
                    SLOT_STATE_EXTER_ALLOC);

                break;

            case AISMsgType24A :
                
                /*lq ���մ�̨���� */
                AISINFO_InstRecStationQuantityIncrease();

                break;

            case AISMsgType26 :
                
                /*lq ʱ϶Ԥռ���� */
                if (AISMsg_26.CommStateSelectFlag == 0)
                {
                    commStateType = AIS_MSG_COMMSTATE_SOTDMA;
                }
                else
                {
                    commStateType = AIS_MSG_COMMSTATE_ITDMA;
                }

                TDMA_SetSlotState(msgCell.Channel, commStateType,
                    AISMsg_26.CommunicationState, msgCell.Frame, msgCell.Slot,
                    SLOT_STATE_EXTER_ALLOC);

                break;

            default :
                break;            
        }
    }
}

/*******************************************************************************
* ����  : MSGMAN_ModuleInit
* ����  : ģ���ʼ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void MSGMAN_ModuleInit(void) 
{
    MSGMAN_MsgStaticListStructInit(&TxMsgList);    
    MSGMAN_MsgStaticListStructInit(&RxMsgList);    
    MSGMAN_PeriodSchedStructInit(&PeriodSched);
    MSGMAN_BurstSchedStructInit(&BurstSched);
}

/*==============================================================================
------------------------------------End of File---------------------------------
==============================================================================*/
