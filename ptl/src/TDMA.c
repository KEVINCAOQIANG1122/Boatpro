/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: TDMA.c
  Author: lq       Version : V1.0.0        Date: 2015.01.16
  Description:     TDMA ����ģ��
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
static TdmaSchedStruct TdmaSched;

/* Private function prototypes -----------------------------------------------*/
static U8 TDMA_SendMsg(AisChannelEnum channel, S16 txSlot,
             AISMsgCommStateEnum commStateType, AISMsgCommStateUnion commState);

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* ����  : SOTDMA_SotdmaParaStructInit
* ����  : SotdmaParaStruct�ṹ���ʼ��
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SOTDMA_SotdmaParaStructInit(SotdmaParaStruct *pSotdmaParaStruct)
{
    U16 i;

    pSotdmaParaStruct->NominalStartSlot = SLOT_INVALID_VALUE;
    pSotdmaParaStruct->ReportRate = SOTDMA_SC_RR_MIN;   //zjw ��������//lq �ó�ʼֵ����TdmaCtrlStruct.ReportIntervalSlot�ĳ�ʼֵ��ƥ��
    pSotdmaParaStruct->NominalIncrement = FRAME_SLOT_SIZE / (pSotdmaParaStruct->ReportRate);
    pSotdmaParaStruct->SelectIntervalOffset =
        SOTDMA_CalcSelectIntervalOffset(pSotdmaParaStruct->NominalIncrement);
    pSotdmaParaStruct->SelectIntervalLow = SLOT_INVALID_VALUE;
    pSotdmaParaStruct->SelectIntervalHigh = SLOT_INVALID_VALUE;
    for (i = 0; i < SOTDMA_NTS_BUFF_LEN; i++)
    {
        pSotdmaParaStruct->NominalSlots[i] = SLOT_INVALID_VALUE;
        pSotdmaParaStruct->NominalTxSlots[i] = SLOT_INVALID_VALUE;
        pSotdmaParaStruct->SlotTimeouts[i] = SOTDMA_SLOT_TMO_INVALID_VALUE;
    }
    pSotdmaParaStruct->NominalSlotIn = 0;
    pSotdmaParaStruct->NominalSlotOut = 0;
    pSotdmaParaStruct->NominalSlot = SLOT_INVALID_VALUE;
    pSotdmaParaStruct->NominalTxSlotTotal = 0;
    pSotdmaParaStruct->NominalTxSlotIn = 0;
    pSotdmaParaStruct->NominalTxSlotOut = 0;
    pSotdmaParaStruct->NominalTxSlot = SLOT_INVALID_VALUE;
    pSotdmaParaStruct->SlotTimeoutIn = 0;
    pSotdmaParaStruct->SlotTimeoutOut = 0;
    pSotdmaParaStruct->SlotTimeout = SOTDMA_SLOT_TMO_INVALID_VALUE;

    pSotdmaParaStruct->FrameReportCnt = 0;   
    pSotdmaParaStruct->SlotOffset = 0;

    pSotdmaParaStruct->LastTxFrame = FRAME_INVALID_VALUE;
    pSotdmaParaStruct->LastTxSlot = SLOT_INVALID_VALUE;
}

/*******************************************************************************
* ����  : SOTDMA_SotdmaParaStructReset
* ����  : ��λSotdmaParaStruct�ṹ����뱨������ֱ����صĲ���֮��Ĳ���
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SOTDMA_SotdmaParaStructReset(SotdmaParaStruct *pSotdmaParaStruct)
{
    U16 i;

    pSotdmaParaStruct->NominalStartSlot = SLOT_INVALID_VALUE;
    pSotdmaParaStruct->SelectIntervalLow = SLOT_INVALID_VALUE;
    pSotdmaParaStruct->SelectIntervalHigh = SLOT_INVALID_VALUE;
    for (i = 0; i < SOTDMA_NTS_BUFF_LEN; i++)
    {
        pSotdmaParaStruct->NominalSlots[i] = SLOT_INVALID_VALUE;
        pSotdmaParaStruct->NominalTxSlots[i] = SLOT_INVALID_VALUE;
        pSotdmaParaStruct->SlotTimeouts[i] = SOTDMA_SLOT_TMO_INVALID_VALUE;
    }
    pSotdmaParaStruct->NominalSlotIn = 0;
    pSotdmaParaStruct->NominalSlotOut = 0;
    pSotdmaParaStruct->NominalSlot = SLOT_INVALID_VALUE;
    pSotdmaParaStruct->NominalTxSlotTotal = 0;
    pSotdmaParaStruct->NominalTxSlotIn = 0;
    pSotdmaParaStruct->NominalTxSlotOut = 0;
    pSotdmaParaStruct->NominalTxSlot = SLOT_INVALID_VALUE;
    pSotdmaParaStruct->SlotTimeoutIn = 0;
    pSotdmaParaStruct->SlotTimeoutOut = 0;
    pSotdmaParaStruct->SlotTimeout = SOTDMA_SLOT_TMO_INVALID_VALUE;

    pSotdmaParaStruct->FrameReportCnt = 0;   
    pSotdmaParaStruct->SlotOffset = 0;

    pSotdmaParaStruct->LastTxFrame = FRAME_INVALID_VALUE;
    pSotdmaParaStruct->LastTxSlot = SLOT_INVALID_VALUE;
}

/*******************************************************************************
* ����  : SOTDMA_SotdmaParaStructInit
* ����  : SotdmaParaStruct�ṹ�帴��
* ����  : pSotdmaCtrlDst - ָ��Ŀ��SotdmaCtrlStruct�ṹ���ָ��
          pSotdmaCtrlSrc - ָ��ԴSotdmaCtrlStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SOTDMA_SotdmaParaStructCopy(SotdmaParaStruct *pSotdmaParaDst,
                                        SotdmaParaStruct *pSotdmaParaSrc)
{
    U16 i;

    pSotdmaParaDst->NominalStartSlot = pSotdmaParaSrc->NominalStartSlot;
    pSotdmaParaDst->ReportRate = pSotdmaParaSrc->ReportRate;
    pSotdmaParaDst->NominalIncrement = pSotdmaParaSrc->NominalIncrement;
    pSotdmaParaDst->SelectIntervalOffset = pSotdmaParaSrc->SelectIntervalOffset;
    pSotdmaParaDst->SelectIntervalLow = pSotdmaParaSrc->SelectIntervalLow;
    pSotdmaParaDst->SelectIntervalHigh = pSotdmaParaSrc->SelectIntervalHigh;
    for (i = 0; i < SOTDMA_NTS_BUFF_LEN; i++)
    {
        pSotdmaParaDst->NominalSlots[i] = pSotdmaParaSrc->NominalSlots[i];
        pSotdmaParaDst->NominalTxSlots[i] = pSotdmaParaSrc->NominalTxSlots[i];
        pSotdmaParaDst->SlotTimeouts[i] = pSotdmaParaSrc->SlotTimeouts[i];
    }
    pSotdmaParaDst->NominalSlotIn = pSotdmaParaSrc->NominalSlotIn;
    pSotdmaParaDst->NominalSlotOut = pSotdmaParaSrc->NominalSlotOut;
    pSotdmaParaDst->NominalSlot = pSotdmaParaSrc->NominalSlot;
    pSotdmaParaDst->NominalTxSlotTotal = pSotdmaParaSrc->NominalTxSlotTotal;
    pSotdmaParaDst->NominalTxSlotIn = pSotdmaParaSrc->NominalTxSlotIn;
    pSotdmaParaDst->NominalTxSlotOut = pSotdmaParaSrc->NominalTxSlotOut;
    pSotdmaParaDst->NominalTxSlot = pSotdmaParaSrc->NominalTxSlot;
    pSotdmaParaDst->SlotTimeoutIn = pSotdmaParaSrc->SlotTimeoutIn;
    pSotdmaParaDst->SlotTimeoutOut = pSotdmaParaSrc->SlotTimeoutOut;
    pSotdmaParaDst->SlotTimeout = pSotdmaParaSrc->SlotTimeout;

    pSotdmaParaDst->FrameReportCnt = pSotdmaParaSrc->FrameReportCnt;
    pSotdmaParaDst->SlotOffset = pSotdmaParaSrc->SlotOffset;

    pSotdmaParaDst->LastTxFrame = pSotdmaParaSrc->LastTxFrame;
    pSotdmaParaDst->LastTxSlot = pSotdmaParaSrc->LastTxSlot;
}

/*******************************************************************************
* ����  : SOTDMA_SotdmaCtrlStructInit
* ����  : SotdmaCtrlStruct�ṹ���ʼ��
* ����  : pSotdmaCtrlStruct - ָ��SotdmaCtrlStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SOTDMA_SotdmaCtrlStructInit(SotdmaCtrlStruct *pSotdmaCtrlStruct)
{
    pSotdmaCtrlStruct->TimeCnt = 0;
    pSotdmaCtrlStruct->TimePeriod = SOTDMA_CTRL_TIM_PERIOD;
    pSotdmaCtrlStruct->Timeout = SOTDMA_CTRL_TIMEOUT_INIT_PHASE;
    pSotdmaCtrlStruct->TimeoutCnt = 0;
    pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_IDLE;
    pSotdmaCtrlStruct->SotdmaEn = FALSE;
}

#if 0 // lnw mask  noused
/*******************************************************************************
* ����  : SOTDMA_SotdmaCtrlStructInit
* ����  : SotdmaCtrlStruct�ṹ�帴��
* ����  : pSotdmaCtrlDst - ָ��Ŀ��SotdmaCtrlStruct�ṹ���ָ��
          pSotdmaCtrlSrc - ָ��ԴSotdmaCtrlStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SOTDMA_SotdmaCtrlStructCopy(SotdmaCtrlStruct *pSotdmaCtrlDst,
                                        SotdmaCtrlStruct *pSotdmaCtrlSrc)
{
    pSotdmaCtrlDst->TimeCnt = pSotdmaCtrlSrc->TimeCnt;
    pSotdmaCtrlDst->TimePeriod = pSotdmaCtrlSrc->TimePeriod;
    pSotdmaCtrlDst->Timeout = pSotdmaCtrlSrc->Timeout;
    pSotdmaCtrlDst->TimeoutCnt = pSotdmaCtrlSrc->TimeoutCnt;
    pSotdmaCtrlDst->CtrlState = pSotdmaCtrlSrc->CtrlState;
    pSotdmaCtrlDst->SotdmaEn = pSotdmaCtrlSrc->SotdmaEn;
}
#endif
/*******************************************************************************
* ����  : SOTDMA_SotdmaSchedStructInit
* ����  : SotdmaSchedStruct�ṹ���ʼ��
* ����  : pSotdmaSchedStruct - ָ��SotdmaSchedStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SOTDMA_SotdmaSchedStructInit(SotdmaSchedStruct *pSotdmaSchedStruct)
{
    SOTDMA_SotdmaParaStructInit(&(pSotdmaSchedStruct->SotdmaPara));
    SOTDMA_SotdmaParaStructInit(&(pSotdmaSchedStruct->SotdmaParaNew));
    SOTDMA_SotdmaCtrlStructInit(&(pSotdmaSchedStruct->SotdmaCtrl));
}

/*******************************************************************************
* ����  : SOTDMA_SotdmaSchedDualStructInit
* ����  : SotdmaSchedDualStruct�ṹ���ʼ��
* ����  : pSotdmaSchedDualStruct - ָ��SotdmaSchedDualStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SOTDMA_SotdmaSchedDualStructInit(SotdmaSchedDualStruct *pSotdmaSchedDualStruct)
{
    SOTDMA_SotdmaSchedStructInit(&(pSotdmaSchedDualStruct->SotdmaSchedA));
    SOTDMA_SotdmaSchedStructInit(&(pSotdmaSchedDualStruct->SotdmaSchedB));
    SOTDMA_SotdmaCtrlStructInit(&(pSotdmaSchedDualStruct->SotdmaCtrl));
}

/*******************************************************************************
* ����  : SOTDMA_SotdmaSchedEnable
* ����  : SOTDMA����ʹ������
* ����  : flag - ʹ�ܱ�־
          pSotdmaCtrlStruct - ָ��SotdmaCtrlStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SOTDMA_SotdmaSchedEnable(U8 flag,
                                     SotdmaCtrlStruct *pSotdmaCtrlStruct)
{
    pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_IDLE;
    pSotdmaCtrlStruct->SotdmaEn = flag;
}

/*******************************************************************************
* ����  : SOTDMA_SetRr
* ����  : SOTDMA֡�ڱ����������
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SOTDMA_FrameReportCntIncrease(SotdmaParaStruct *pSotdmaParaStruct)
{
    pSotdmaParaStruct->FrameReportCnt = (pSotdmaParaStruct->FrameReportCnt + 1)
        % pSotdmaParaStruct->ReportRate;
}

/*******************************************************************************
* ����  : SOTDMA_SetRr
* ����  : SOTDMA���ñ�������ͬʱ�����뱨������صĲ���
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
          reportRate - ��������
* ���  : ��
* ����  : ��
*******************************************************************************/
static U8 SOTDMA_SetRr(SotdmaParaStruct *pSotdmaParaStruct, S16 reportRate)
{
    if ((reportRate >= SOTDMA_SC_RR_MIN) && (reportRate <= SOTDMA_SC_RR_MAX))
    {
        pSotdmaParaStruct->ReportRate = reportRate;

        pSotdmaParaStruct->NominalIncrement = FRAME_SLOT_SIZE
            / (pSotdmaParaStruct->ReportRate);

        pSotdmaParaStruct->SelectIntervalOffset =
            SOTDMA_CalcSelectIntervalOffset(pSotdmaParaStruct->NominalIncrement);
    
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SOTDMA_SetRrDual
* ����  : SOTDMAʵ�����ñ�������
* ����  : reportRate - ��������
* ���  : ��
* ����  : ��
*******************************************************************************/
void SOTDMA_SetRrDual(S16 reportRate)
{
    SOTDMA_SetRr(&(TdmaSched.SotdmaSched.SotdmaSchedA.SotdmaPara), reportRate);
    SOTDMA_SetRr(&(TdmaSched.SotdmaSched.SotdmaSchedB.SotdmaPara), reportRate);
}

/*******************************************************************************
* ����  : SOTDMA_IsAtSi
* ����  : SOTDMA�ж��Ƿ�λ��ѡ������
* ����  : slot - �ж�ʱ϶
          ns - ���ʱ϶
          offset - ѡ����ƫ����
* ���  : ��
* ����  : ��
*******************************************************************************/
static U8 SOTDMA_IsAtSi(S16 slot, S16 ns, S16 offset)
{     
    S16 tempS16;

    tempS16 = slot - ns;
    if (tempS16 < 0)
    {
        tempS16 = -tempS16; 
    }

    if (tempS16 <= offset)
    {
        return TRUE;
    }
    else
    {
        return FALSE;    
    }
}

/*******************************************************************************
* ����  : SOTDMA_IsAtSi2
* ����  : SOTDMA�жϴ��ж�ʱ϶����AIS_SLOT_INTERVAL_SEND_TO_TX���Ƿ�λ��ѡ����
          ��
* ����  : slot - �ж�ʱ϶
          ns - ���ʱ϶
          offset - ѡ����ƫ����
* ���  : ��
* ����  : ��
*******************************************************************************/
static U8 SOTDMA_IsAtSi2(S16 slot, S16 ns, S16 offset)
{     
    return SOTDMA_IsAtSi(CalcSlotNum(0, slot, AIS_SLOT_INTERVAL_SEND_TO_TX),
            ns, offset);
}

/*******************************************************************************
* ����  : SOTDMA_GetAvailableNts
* ����  : SOTDMA��ȡ���õķ���ʱ϶
* ����  : pSotdmaParaStruct - ָ�������SotdmaParaStruct�ṹ���ָ��
          ns - ���ʱ϶
          offset - ѡ����ƫ����
* ���  : ��
* ����  : ��
*******************************************************************************/
static U16 SOTDMA_GetAvailableNts(SotdmaParaStruct *pSotdmaParaStruct, S16 ns, S16 offset)
{     
    S16 i;

    for (i = 0; i < pSotdmaParaStruct->ReportRate; i++)
    {
        if (SOTDMA_IsAtSi(pSotdmaParaStruct->NominalTxSlots[i], ns, offset)
            == TRUE)
        {
            return pSotdmaParaStruct->NominalTxSlots[i];
        }
    }
    
    return SLOT_INVALID_VALUE;
}

#if   0  // lnw mask  no use
/*******************************************************************************
* ����  : SOTDMA_IsAtNts
* ����  : SOTDMA�ж��Ƿ�λ�ڱ�Ʒ���ʱ϶
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
          slot - �ж�ʱ϶
* ���  : ��
* ����  : ��
*******************************************************************************/
static U8 SOTDMA_IsAtNts(SotdmaParaStruct *pSotdmaParaStruct, S16 slot)
{     
    if (pSotdmaParaStruct->NominalTxSlot == slot)    
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif
/*******************************************************************************
* ����  : SOTDMA_IsNtsArrived
* ����  : SOTDMA�ж��Ƿ񵽴��Ʒ���ʱ϶
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
          slot - �ж�ʱ϶
* ���  : ��
* ����  : ��
*******************************************************************************/
static U8 SOTDMA_IsNtsArrived(SotdmaParaStruct *pSotdmaParaStruct, S16 slot)
{     
    if (((slot - pSotdmaParaStruct->NominalTxSlot + FRAME_SLOT_SIZE)
        % FRAME_SLOT_SIZE) < AIS_SLOT_INTERVAL_SEND_TO_TX)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SOTDMA_IsSendTimArrived
* ����  : SOTDMA�жϷ���ʱ϶��������Ϣ���е�ʱ϶���Ƿ񵽴�
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
          frame - �ж�֡������pSotdmaParaStruct->ReportRate == 1ʱ���ã����
          ReportRate = 1ʱ������һ����ʱ϶�뵱ǰ����ʱ϶��ͬ�����´��ڱ�Ʒ���ʱ
          ϶ʱ��Ϣ��η��͵����⡣��
          slot - �ж�ʱ϶
* ���  : ��
* ����  : ��
*******************************************************************************/
static U8 SOTDMA_IsSendTimArrived(SotdmaParaStruct *pSotdmaParaStruct,
                                  S32 frame, S16 slot)
{     
    if (SOTDMA_IsNtsArrived(pSotdmaParaStruct, CalcSlotNum(0, slot,
        AIS_SLOT_INTERVAL_SEND_TO_TX)) == TRUE)    
    {
        if (pSotdmaParaStruct->ReportRate == 1)
        {
            if ((pSotdmaParaStruct->LastTxFrame == FRAME_INVALID_VALUE)
                || (pSotdmaParaStruct->LastTxSlot == SLOT_INVALID_VALUE))
            {
                pSotdmaParaStruct->LastTxFrame = frame;
                pSotdmaParaStruct->LastTxSlot = slot;

                return TRUE;                
            }
            else
            {
                if (CalcSlotOffset(pSotdmaParaStruct->LastTxFrame,
                    pSotdmaParaStruct->LastTxSlot, frame, slot)
                    > (pSotdmaParaStruct->NominalIncrement / 2))
                {
                    pSotdmaParaStruct->LastTxFrame = frame;
                    pSotdmaParaStruct->LastTxSlot = slot;
    
                    return TRUE;                
                }
                else
                {
                    return FALSE;                
                }
            }

        }
        else
        {
            return TRUE;
        }
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SOTDMA_SetNss
* ����  : SOTDMA���ñ����ʼʱ϶��ͬʱָ����һ��NS
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
          nss - �����ʼʱ϶
* ���  : ��
* ����  : ��
*******************************************************************************/
static U8 SOTDMA_SetNss(SotdmaParaStruct *pSotdmaParaStruct, S16 nss)
{
    if (nss >= 0 && nss < FRAME_SLOT_SIZE)
    {
        pSotdmaParaStruct->NominalStartSlot = nss;                     
        
        return TRUE;
    }
    else
    {
        return FALSE;    
    }
}

/*******************************************************************************
* ����  : SOTDMA_SetNextNs
* ����  : ѡ����һ�����ʱ϶
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SOTDMA_SetFirstNs(SotdmaParaStruct *pSotdmaParaStruct)
{
    pSotdmaParaStruct->NominalSlots[pSotdmaParaStruct->NominalSlotIn]
        = pSotdmaParaStruct->NominalStartSlot;

    pSotdmaParaStruct->NominalSlotIn = (pSotdmaParaStruct->NominalSlotIn + 1)
        % pSotdmaParaStruct->ReportRate;
}

#if   0  // lnw mask  no use
/*******************************************************************************
* ����  : SOTDMA_SetNs
* ����  : ���ñ��ʱ϶
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SOTDMA_SetNs(SotdmaParaStruct *pSotdmaParaStruct, U16 index, U16 slot)
{
    pSotdmaParaStruct->NominalSlots[index] = slot;
}
#endif 
/*******************************************************************************
* ����  : SOTDMA_SetNextNs
* ����  : ѡ����һ�����ʱ϶
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
* ���  : ��
* ����  : ��һ�����ʱ϶
*******************************************************************************/
static U16 SOTDMA_SetNextNs(SotdmaParaStruct *pSotdmaParaStruct)
{
    U16 tempU16;
    
    tempU16 = (pSotdmaParaStruct->NominalSlot + pSotdmaParaStruct->NominalIncrement)
        % FRAME_SLOT_SIZE;

    pSotdmaParaStruct->NominalSlots[pSotdmaParaStruct->NominalSlotIn] = tempU16;
    pSotdmaParaStruct->NominalSlotIn = (pSotdmaParaStruct->NominalSlotIn + 1)
        % pSotdmaParaStruct->ReportRate;

    return tempU16;
}

/*******************************************************************************
* ����  : SOTDMA_SetNextNs
* ����  : ѡ����һ�����ʱ϶
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
* ���  : ��
* ����  : ��һ�����ʱ϶
*******************************************************************************/
static U16 SOTDMA_GetNextNs(SotdmaParaStruct *pSotdmaParaStruct)
{
    pSotdmaParaStruct->NominalSlot =
        pSotdmaParaStruct->NominalSlots[pSotdmaParaStruct->NominalSlotOut];
    
    pSotdmaParaStruct->NominalSlotOut =
        (pSotdmaParaStruct->NominalSlotOut + 1) % pSotdmaParaStruct->ReportRate;

    return pSotdmaParaStruct->NominalSlot;
}

/*******************************************************************************
* ����  : SOTDMA_SetNts
* ����  : ���ñ�Ʒ���ʱ϶
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SOTDMA_SetNts(SotdmaParaStruct *pSotdmaParaStruct, U16 index, U16 slot)
{
    pSotdmaParaStruct->NominalTxSlots[index] = slot;
}

/*******************************************************************************
* ����  : SOTDMA_SetNextNts
* ����  : SOTDMA������һ����Ʒ���ʱ϶
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
          slot - ��Ʒ���ʱ϶
* ���  : ��
* ����  : ��
*******************************************************************************/
static U8 SOTDMA_SetNextNts(SotdmaParaStruct *pSotdmaParaStruct, S16 slot)
{
    if (slot >= 0 && slot < FRAME_SLOT_SIZE)
    {
        pSotdmaParaStruct->NominalTxSlots[pSotdmaParaStruct->NominalTxSlotIn] = slot;
        pSotdmaParaStruct->NominalTxSlotIn = (pSotdmaParaStruct->NominalTxSlotIn + 1)
            %  pSotdmaParaStruct->ReportRate;
        
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SOTDMA_GetNextNts
* ����  : SOTDMA��ȡ��һ��Ʒ���ʱ϶
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
* ���  : ��
* ����  : ��һ��Ʒ���ʱ϶
*******************************************************************************/
static S16 SOTDMA_GetNextNts(SotdmaParaStruct *pSotdmaParaStruct)
{
    pSotdmaParaStruct->NominalTxSlot = 
        pSotdmaParaStruct->NominalTxSlots[pSotdmaParaStruct->NominalTxSlotOut];
    
    pSotdmaParaStruct->NominalTxSlotOut =
        (pSotdmaParaStruct->NominalTxSlotOut + 1) %  pSotdmaParaStruct->ReportRate;

    return pSotdmaParaStruct->NominalTxSlot;
}

/*******************************************************************************
* ����  : SOTDMA_IsRrChanged
* ����  : ���������Ƿ�ı�
* ����  : channel - �ŵ�
* ���  : ��
* ����  : TRUE - �Ѹı䣻FALSE - δ�ı�
*******************************************************************************/
U8 SOTDMA_IsRrChanged(AisChannelEnum channel)
{
    if (channel == ENUM_AIS_CHANNEL_A)
    {
        if (TdmaSched.SotdmaSched.SotdmaSchedA.SotdmaParaNew.ReportRate
            != TdmaSched.SotdmaSched.SotdmaSchedA.SotdmaPara.ReportRate)
        {
            return TRUE;        
        }
        else
        {
            return FALSE;
        }
    }
    else if (channel == ENUM_AIS_CHANNEL_B)
    {
        if (TdmaSched.SotdmaSched.SotdmaSchedB.SotdmaParaNew.ReportRate
            != TdmaSched.SotdmaSched.SotdmaSchedB.SotdmaPara.ReportRate)
        {
            return TRUE;        
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }                
}

#if 0
/*******************************************************************************
* ����  : SotdmaIsRrChanged
* ����  : ���������Ƿ�ı�
* ����  : ��
* ���  : ��
* ����  : TRUE - �Ѹı䣻FALSE - δ�ı�
*******************************************************************************/
U8 SotdmaIsRrChanged(void)
{
    if ((SOTDMA_IsRrChanged(&(SotdmaSched.SotdmaParaA)) == TRUE)
        || (SOTDMA_IsRrChanged(&(SotdmaSched.SotdmaParaB)) == TRUE))
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }
}
#endif

/*******************************************************************************
* ����  : SOTDMA_SetNssDual
* ����  : SOTDMA���������ŵ��ı����ʼʱ϶
* ����  : channel - nss��Ӧ���ŵ�
          nss - �����ʼʱ϶
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 SOTDMA_SetNssDual(AisChannelEnum channel, S16 nss)
{
    if (nss >= 0 && nss < FRAME_SLOT_SIZE)
    {
        if (channel == ENUM_AIS_CHANNEL_A)
        {
            SOTDMA_SetNss(&(TdmaSched.SotdmaSched.SotdmaSchedA.SotdmaPara), nss);
            SOTDMA_SetNss(&(TdmaSched.SotdmaSched.SotdmaSchedB.SotdmaPara),
                (nss + (TdmaSched.SotdmaSched.SotdmaSchedB.SotdmaPara.NominalIncrement >> 1))   //lq �˴���NominalIncrementΪ���ŵ����������Ϊ˫�ŵ����������2��
                % FRAME_SLOT_SIZE);        
        }
        else if (channel == ENUM_AIS_CHANNEL_B)
        {
            SOTDMA_SetNss(&(TdmaSched.SotdmaSched.SotdmaSchedB.SotdmaPara), nss);
            SOTDMA_SetNss(&(TdmaSched.SotdmaSched.SotdmaSchedA.SotdmaPara),
                (nss + (TdmaSched.SotdmaSched.SotdmaSchedA.SotdmaPara.NominalIncrement >> 1))
                % FRAME_SLOT_SIZE);
        }
        else
        {
            return FALSE;
        }                
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

#if   0  // lnw mask  no use
/*******************************************************************************
* ����  : SOTDMA_SetTmo
* ����  : SOTDMA����ʱ϶��ʱ
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
          index - ����ֵ
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SOTDMA_SetTmo(SotdmaParaStruct *pSotdmaParaStruct, U16 index)
{ 
    pSotdmaParaStruct->SlotTimeouts[index] =
        SOTDMA_TMO_MIN + (rand() % (SOTDMA_TMO_MAX - SOTDMA_TMO_MIN + 1));
}
#endif
/*******************************************************************************
* ����  : SOTDMA_GetTmo
* ����  : SOTDMA��ȡʱ϶��ʱ
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
          index - ����ֵ
* ���  : ��
* ����  : ʱ϶��ʱ
*******************************************************************************/
static S8 SOTDMA_GetTmo(SotdmaParaStruct *pSotdmaParaStruct, U16 index)
{ 
    return pSotdmaParaStruct->SlotTimeouts[index];
}

/*******************************************************************************
* ����  : SOTDMA_SetNextNts
* ����  : SOTDMA������һ����Ʒ���ʱ϶
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
          nts - ��Ʒ���ʱ϶
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SOTDMA_SetNextTmo(SotdmaParaStruct *pSotdmaParaStruct)
{ 
    pSotdmaParaStruct->SlotTimeouts[pSotdmaParaStruct->SlotTimeoutIn] =
        SOTDMA_TMO_MIN + (rand() % (SOTDMA_TMO_MAX - SOTDMA_TMO_MIN + 1));

    pSotdmaParaStruct->SlotTimeoutIn = (pSotdmaParaStruct->SlotTimeoutIn + 1)
        %  pSotdmaParaStruct->ReportRate;
}

/*******************************************************************************
* ����  : SOTDMA_GetNextTmo
* ����  : ѡ����һ��ʱ϶��ʱֵ��ͬʱ������ֵ��1����ʱ϶��ʱֵ��1��С��0��������
          ѡ���ʼֵ��
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
* ���  : ��
* ����  : ��һ��ʱ϶��ʱֵ
*******************************************************************************/
static S8 SOTDMA_GetNextTmo(SotdmaParaStruct *pSotdmaParaStruct)
{
    pSotdmaParaStruct->SlotTimeout =
        (pSotdmaParaStruct->SlotTimeouts[pSotdmaParaStruct->SlotTimeoutOut])--;

    /*lq ��ʱ϶��ʱֵ��1��С��0��������ѡ���ʼֵ */
    if (pSotdmaParaStruct->SlotTimeouts[pSotdmaParaStruct->SlotTimeoutOut] < 0)    
    {
        pSotdmaParaStruct->SlotTimeouts[pSotdmaParaStruct->SlotTimeoutOut] =
            SOTDMA_TMO_MIN + (rand() % (SOTDMA_TMO_MAX - SOTDMA_TMO_MIN + 1));        
    }

    pSotdmaParaStruct->SlotTimeoutOut =
        (pSotdmaParaStruct->SlotTimeoutOut + 1) % pSotdmaParaStruct->ReportRate;

    return pSotdmaParaStruct->SlotTimeout;
}

/*******************************************************************************
* ����  : SOTDMA_GetSubMsg
* ����  : ����ʱ϶��ʱֵ��ȡ����Ϣ��
* ����  : slotTimeout - ʱ϶��ʱֵ��
          pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ�룻
* ���  : ��
* ����  : ����Ϣ
*******************************************************************************/
static U16 SOTDMA_GetSubMsg(U8 slotTimeout, SotdmaParaStruct *pSotdmaParaStruct)
{
    U16 subMsg;
	U16 utcHour;
	U16 utcMinute;

    if (slotTimeout == 0)
    {
        /*lq Slot offset */
        subMsg = pSotdmaParaStruct->SlotOffset;   
    }
    else if (slotTimeout == 1)
    {
        /*lq UTC hour and minute */
        if (IsGpsFix() == TRUE)
        {
            /*lq �Ѷ�λ */
            utcHour = GpsPara.GPS_TimeH;
            utcMinute = GpsPara.GPS_TimeM;
        }
        else
        {
            /*lq δ��λ */
            utcHour = 24;
            utcMinute = 60;
        }

        subMsg = (utcHour << 19) || (utcMinute << 2);
    }
    else if ((slotTimeout == 2)
             || (slotTimeout == 4)
             || (slotTimeout == 6))
    {
        subMsg = pSotdmaParaStruct->NominalTxSlot;
    }
    else if ((slotTimeout == 3)
             || (slotTimeout == 5)
             || (slotTimeout == 7))
    {
        subMsg = AISINFO_InstRecStationGetQuantity();
    }

    return subMsg;
}

/*******************************************************************************
* ����  : SOTDMA_CalcSelectIntervalOffset
* ����  : ����SOTDMA�ı��ʱ϶��������ʱ϶ѡ������ƫ����
* ����  : nominalIncrement - SOTDMA���ʱ϶����
* ���  : ��
* ����  : ʱ϶ѡ������ƫ����
*******************************************************************************/
S16 SOTDMA_CalcSelectIntervalOffset(S16 nominalIncrement)
{
    return nominalIncrement * SOTDMA_SI_OFFSET_SCALE;
}

/*******************************************************************************
* ����  : SOTDMA_SetSlotState
* ����  : ����ͨ��״̬����ǰ֡�ż�ʱ϶������ʱ϶ӳ�������Ӧʱ϶��״̬
* ����  : channel - ʱ϶���ڵ��ŵ�
          commState - SOTDMAͨ��״̬
          currFrame - ��ǰ֡��
          currSlot - ��ǰʱ϶��
          slotState - ʱ϶״̬
* ���  : ��
* ����  : ��
*******************************************************************************/
void SOTDMA_SetSlotState(AisChannelEnum channel, SOTDMACommStateStruct commState,
                       S32 currFrame, S16 currSlot, U8 slotState)
{
//    S32 tempS32;
//    S16 tempS16;
    S16 i;

    if ((commState.SlotTimeout >= 1)
        && (commState.SlotTimeout <= 7))
    {
        for (i = 1; i <= commState.SlotTimeout; i++)
        {
            SetSlotState(channel, currSlot, slotState, currFrame + i);        
        }
    }
    else if (commState.SlotTimeout == 0)
    {
        if (commState.SubMessage.SlotOffset != 0)
        {
            //tempS32 =  //lnw mask
					    CalcFrameNum(currFrame, currSlot,
                commState.SubMessage.SlotOffset);
            
           // tempS16 =  //lnw mask
					     CalcSlotNum(currFrame, currSlot,
                commState.SubMessage.SlotOffset);
        }
    }
}

/*******************************************************************************
* ����  : TDMA_GetSyncState
* ����  : ��ȡͬ����ʽ
* ����  : ��
* ���  : ��
* ����  : ͬ��״̬
*******************************************************************************/
U8 TDMA_GetSyncState(void)
{
    return TDMA_SYNC_STATE_UTC_DIRECT;  //lq ��ǰͬ����ʽֻ֧��UTC direct��ʽ
}

#if 0
/*******************************************************************************
* ����  : SotdmaIsAtNts
* ����  : SOTDMA�Ƿ���NTS
* ����  : channel - slot��Ӧ���ŵ�
          slot - ��ǰʱ϶
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 SotdmaIsAtNts(AisChannelEnum channel, S16 slot)
{
    SotdmaParaStruct *pSotdmaPara;

    if (channel == ENUM_AIS_CHANNEL_A)
    {
        pSotdmaPara = &(SotdmaSched.SotdmaParaA);    
    }
    else if (channel == ENUM_AIS_CHANNEL_B)
    {
        pSotdmaPara = &(SotdmaSched.SotdmaParaB);    
    }
    else
    {
        return FALSE;
    }
    
    if (pSotdmaPara->NominalTxSlot == slot)    
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif

/*******************************************************************************
* ����  : SOTDMA_Schedule
* ����  : ���ŵ�SOTDMA����
* ����  : pSotdmaSchedStruct - ָ��SotdmaSchedStruct�ṹ���ָ��
          currFrame - ��ǰ֡��
          currSlot - ��ǰʱ϶��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SOTDMA_Schedule(SotdmaSchedStruct *pSotdmaSchedStruct, S32 currFrame, S16 currSlot)
{
    AisChannelEnum channel;
    SotdmaCtrlStruct *pSotdmaCtrlStruct;
    SotdmaParaStruct *pSotdmaPara;
    SotdmaParaStruct *pSotdmaParaNew;
    AISMsgCommStateUnion commState;
    U16 tempU16;
    U16 tempU16_1;
    S32 tempS32;

    if (pSotdmaSchedStruct == &(TdmaSched.SotdmaSched.SotdmaSchedA))
    {
        channel = ENUM_AIS_CHANNEL_A;
    }
    else if (pSotdmaSchedStruct == &(TdmaSched.SotdmaSched.SotdmaSchedB))
    {
        channel = ENUM_AIS_CHANNEL_B;
    }
    else
    {
        return ;
    }
    pSotdmaCtrlStruct = &(pSotdmaSchedStruct->SotdmaCtrl);
    pSotdmaPara = &(pSotdmaSchedStruct->SotdmaPara);
    pSotdmaParaNew = &(pSotdmaSchedStruct->SotdmaParaNew);

    switch (pSotdmaCtrlStruct->CtrlState)
    {
        case SOTDMA_STA_IDLE :

            if (pSotdmaCtrlStruct->SotdmaEn == TRUE)
            {
                pSotdmaCtrlStruct->SotdmaEn = FALSE;

                /*lq ת�뿪ʼ״̬ */
                pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_START;
            }

            break;

        case SOTDMA_STA_START :

            pSotdmaCtrlStruct->TimeCnt = SetTimePeriodProcess(pSotdmaCtrlStruct->TimePeriod);

            /*lq ת���ʼ���׶� */
            pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_INIT_PHASE;            
            
            break;

        case SOTDMA_STA_INIT_PHASE :
            
            /*lq SOTDMA��ʼ���׶�1min�ȴ� */
            if (pSotdmaCtrlStruct->TimeCnt == GetLocalClockTickProcess())
            {
                pSotdmaCtrlStruct->TimeCnt = SetTimePeriodProcess(pSotdmaCtrlStruct->TimePeriod);                
                
                pSotdmaCtrlStruct->TimeoutCnt++;
                if (pSotdmaCtrlStruct->TimeoutCnt >= pSotdmaCtrlStruct->Timeout)
                {
                    pSotdmaCtrlStruct->TimeoutCnt = 0;

                    /*lq ת���������׶� */
                    pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_NETWORK_ENTRY_PHASE;                 
                }                 
            }

            break;

        case SOTDMA_STA_NETWORK_ENTRY_PHASE :
            
            /*lq ѡ��NSS */
            if (pSotdmaPara->NominalIncrement != AIS_SLOT_INTERVAL_SEND_TO_TX)
            {
                tempU16 = Random(pSotdmaPara->NominalIncrement,
                    AIS_SLOT_INTERVAL_SEND_TO_TX);
            }
            else
            {
                tempU16 = pSotdmaPara->NominalIncrement;
            }            
            tempU16 = (currSlot + tempU16) % FRAME_SLOT_SIZE;
            SOTDMA_SetNss(pSotdmaPara, tempU16);

            /*lq ѡ���ŵ���NTS */
            if (pSotdmaPara->NominalStartSlot < currSlot)
            {
                /*lq NSSλ�ڵ�ǰ֡����һ֡ */
                tempS32 = currFrame + 1;
            }
            else
            {
                /*lq NSSλ�ڵ�ǰ֡ */
                tempS32 = currFrame;
            }
            tempU16 = SelectCandidateSlot(channel, pSotdmaPara->NominalStartSlot,
                pSotdmaPara->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_UNI);      //lq ʹ�õ���ѡ�񣬱���ѡ���ʱ϶С�ڵ�ǰʱ϶
            
            if ((IsSlotValid(tempU16) == TRUE))
            {
                /*lq ��������ʼ���� */
                SOTDMA_SetFirstNs(pSotdmaPara);
                SOTDMA_GetNextNs(pSotdmaPara);
                SOTDMA_SetNextNts(pSotdmaPara, tempU16);
                SOTDMA_GetNextNts(pSotdmaPara);
                SOTDMA_SetNextTmo(pSotdmaPara);
                SOTDMA_GetNextTmo(pSotdmaPara);
                pSotdmaPara->FrameReportCnt = 0;
               
                /*lq ת���һ֡�׶� */
                pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_FIRST_FRAME_PHASE;            
            }

            break;

        case SOTDMA_STA_FIRST_FRAME_PHASE :
            
            /*lq �ȴ���ǰ�ŵ���NTS */
            if (pSotdmaPara->FrameReportCnt < pSotdmaPara->ReportRate)
            {
                if (SOTDMA_IsSendTimArrived(pSotdmaPara, currFrame, currSlot) == TRUE)
                {
                    /*lq �ж��Ƿ���һ֡�е����һ�η��� */
                    if ((pSotdmaPara->FrameReportCnt + 1) >= pSotdmaPara->ReportRate)
                    {
                        pSotdmaPara->SlotOffset = 0;
                    }
                    else
                    {
                        /*lq ������һ��NS */
                        tempU16_1 = SOTDMA_SetNextNs(pSotdmaPara);
    
                        /*lq ѡ����һ��NTS */
                        if (tempU16_1 < currSlot)
                        {
                            tempS32 = currFrame + 1;
                        }
                        else
                        {
                            tempS32 = currFrame;
                        }
                        tempU16 = SelectCandidateSlot(channel, tempU16_1,
                            pSotdmaPara->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_BI);
    
                        if (IsSlotValid(tempU16) == FALSE)
                        {
                            tempU16 = tempU16_1;
                        }
    
                        /*lq ����slotOffset */
                        pSotdmaPara->SlotOffset = (tempU16 - pSotdmaPara->NominalTxSlot
                            + FRAME_SLOT_SIZE) % FRAME_SLOT_SIZE;                    
                        
                        /*lq ������һ��NTS */
                        SOTDMA_SetNextNts(pSotdmaPara, tempU16);

                        /*lq ������һ��slotTimeout*/
                        SOTDMA_SetNextTmo(pSotdmaPara);
                    }
    
                    /*lq ��ʼ��18����Ϣ */
                    /*lq ʹ��ITDMAͨ��״̬��SendMsg18()��������SOTDMA_GetNextNts()����֮ǰ*/
                    commState.ITDMACommState.SyncState = TDMA_GetSyncState();
                    commState.ITDMACommState.SlotIncrement =
                        ITDMA_CalcCommStateSlotIncrement(pSotdmaPara->SlotOffset);
                    commState.ITDMACommState.NumberOfSlots =
                        ITDMA_CalcCommStateNumberOfSlots(1,
                        pSotdmaPara->SlotOffset);
                    commState.ITDMACommState.KeepFlag =
                        ITDMA_CalcCommStateKeepFlag(TRUE);
                    TDMA_SendMsg(channel, pSotdmaPara->NominalTxSlot,
                        AIS_MSG_COMMSTATE_ITDMA, commState);    //lq ���ò������ṩchannel, txSlot, commStateType, commState
    
                    SOTDMA_GetNextNs(pSotdmaPara);
                    SOTDMA_GetNextNts(pSotdmaPara);
                    SOTDMA_GetNextTmo(pSotdmaPara);
                    SOTDMA_FrameReportCntIncrease(pSotdmaPara);
                }            
            }

            /*lq �жϵ�һ֡�׶��Ƿ���� */
            if (pSotdmaPara->FrameReportCnt >= pSotdmaPara->ReportRate)
            {
                pSotdmaPara->FrameReportCnt = 0;

                /*lq ת�����������׶� */
                pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CONTINUOUS_PHASE;
            }

            break;

        case SOTDMA_STA_CONTINUOUS_PHASE :

            /*lq �ȴ���ǰ�ŵ���NTS */
            if (SOTDMA_IsSendTimArrived(pSotdmaPara, currFrame, currSlot) == TRUE)            
            {
                /*lq �ж��Ƿ�ΪԤռ�����һ֡ */
                if (pSotdmaPara->SlotTimeout == 0)
                {
                    /*lq Ϊ��ǰ��NSѡ���µ�NTS */
                    if (pSotdmaPara->NominalSlot < currSlot)
                    {
                        tempS32 = currFrame + 1;
                    }
                    else
                    {
                        tempS32 = currFrame;
                    }
                    tempU16 = SelectCandidateSlot(channel, pSotdmaPara->NominalSlot,
                        pSotdmaPara->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_BI);

                    if (IsSlotValid(tempU16) == FALSE)
                    {
                        tempU16 = pSotdmaPara->NominalSlot;                    
                    }

                    /*lq ����slotOffset */
                    pSotdmaPara->SlotOffset = tempU16 - pSotdmaPara->NominalTxSlot
                        + FRAME_SLOT_SIZE;
                    
                    /*lq ����NTS */                                            
                    SOTDMA_SetNts(pSotdmaPara, pSotdmaPara->FrameReportCnt,
                        tempU16);

                    /*lq ����slotTimeout */                                            
                    pSotdmaPara->SlotTimeout = SOTDMA_GetTmo(pSotdmaPara,
                        pSotdmaPara->FrameReportCnt);
                }
                else
                {
                    pSotdmaPara->SlotOffset = 0;                
                }

                /*lq ��ʼ��18����Ϣ */
                /*lq ʹ��SOTDMAͨ��״̬��SendMsg18()��������SOTDMA_GetNextNts()����֮ǰ*/
                commState.SOTDMACommState.SyncState = TDMA_GetSyncState();
                commState.SOTDMACommState.SlotTimeout = pSotdmaPara->SlotTimeout;
                commState.SOTDMACommState.SubMessage.SubMsg =
                    SOTDMA_GetSubMsg(commState.SOTDMACommState.SlotTimeout,
                    pSotdmaPara);
                TDMA_SendMsg(channel, pSotdmaPara->NominalTxSlot,
                    AIS_MSG_COMMSTATE_SOTDMA, commState);       //lq ���ò������ṩchannel, txSlot, commStateType, commState

                SOTDMA_GetNextNs(pSotdmaPara);
                SOTDMA_GetNextNts(pSotdmaPara);
                SOTDMA_GetNextTmo(pSotdmaPara);

                /*lq һ֡�еı������������1 */
                SOTDMA_FrameReportCntIncrease(pSotdmaPara);
            }

            if (SOTDMA_IsRrChanged(channel) == TRUE)
            {
                SOTDMA_SotdmaParaStructReset(pSotdmaParaNew);

                /*lq ת��ı䱨����״̬1 */
                pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_1;                             
            }

            break;

        case SOTDMA_STA_CHANGE_REPORT_INTERVAL_1 :

            /*lq �ȴ���ǰ�ŵ���NTS */
            if (SOTDMA_IsSendTimArrived(pSotdmaPara, currFrame, currSlot) == TRUE)
            {
                /*lq �±��������£�һ֡�еĵ�һ�η��� */
                SOTDMA_SetNss(pSotdmaParaNew, pSotdmaPara->NominalSlot);
                SOTDMA_SetFirstNs(pSotdmaParaNew);
                SOTDMA_GetNextNs(pSotdmaParaNew);
                SOTDMA_SetNextNts(pSotdmaParaNew, pSotdmaPara->NominalTxSlot);
                SOTDMA_GetNextNts(pSotdmaParaNew);
                SOTDMA_SetNextTmo(pSotdmaParaNew);
                SOTDMA_GetNextTmo(pSotdmaParaNew);

                if (pSotdmaPara->SlotTimeout == 0)
                {
                    pSotdmaPara->SlotTimeout = 1;
                }

                /*lq �±��������£�һ֡�еĵڶ��η��� */
                SOTDMA_SetNextNs(pSotdmaParaNew);
                SOTDMA_GetNextNs(pSotdmaParaNew);

                tempU16 = SOTDMA_GetAvailableNts(pSotdmaPara,
                    pSotdmaParaNew->NominalSlot,
                    pSotdmaParaNew->SelectIntervalOffset);
                
                if (tempU16 != SLOT_INVALID_VALUE)
                {
                    /*lq ʹ�þɱ�����������Ԥռ��ʱ϶��Ϊ�±��������µķ���ʱ϶ */
                    SOTDMA_SetNextNts(pSotdmaParaNew, tempU16);
                    SOTDMA_SetNextTmo(pSotdmaParaNew);

                    if (pSotdmaParaNew->NominalSlot == pSotdmaParaNew->NominalStartSlot)
                    {
                        /*lq ת��ı䱨����״̬3 */
                        pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_3;                                           
                    }
                    else
                    {                 
                        /*lq ��ʼ��18����Ϣ */
                        /*lq ʹ��SOTDMAͨ��״̬�Ͳ���SotdmaPara��SendMsg18()��������SOTDMA_GetNextNts()����֮ǰ*/
                        commState.SOTDMACommState.SyncState = TDMA_GetSyncState();
                        commState.SOTDMACommState.SlotTimeout = pSotdmaPara->SlotTimeout;
                        commState.SOTDMACommState.SubMessage.SubMsg =
                            SOTDMA_GetSubMsg(commState.SOTDMACommState.SlotTimeout,
                            pSotdmaPara);
                        TDMA_SendMsg(channel, pSotdmaPara->NominalTxSlot,
                            AIS_MSG_COMMSTATE_SOTDMA, commState);       //lq ���ò������ṩchannel, txSlot, commStateType, commState

                        /*lq �±������ʺ;ͱ��������µķ���ʱ϶�غϣ���Ϊһ�η��� */
                        SOTDMA_GetNextNts(pSotdmaPara);
                        SOTDMA_GetNextTmo(pSotdmaPara);
                        SOTDMA_GetNextNts(pSotdmaParaNew);
                        SOTDMA_GetNextTmo(pSotdmaParaNew);
    
                        /*lq һ֡�еı������������1 */
                        SOTDMA_FrameReportCntIncrease(pSotdmaPara);
                        SOTDMA_FrameReportCntIncrease(pSotdmaParaNew);

                        /*lq ת��ı䱨����״̬2 */
                        pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_2;
                    }
                }
                else
                {
                    /*lq ѡ���µ�NTS */
                    if (pSotdmaParaNew->NominalSlot < currSlot)
                    {
                        tempS32 = currFrame + 1;
                    }
                    else
                    {
                        tempS32 = currFrame;
                    }
                    tempU16 = SelectCandidateSlot(channel, pSotdmaParaNew->NominalSlot,
                        pSotdmaParaNew->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_BI);
                    if (IsSlotValid(tempU16) == FALSE)
                    {
                        tempU16 = pSotdmaParaNew->NominalSlot;
                    }

                    /*lq ����slotOffset */
                    pSotdmaPara->SlotOffset = tempU16 - pSotdmaPara->NominalTxSlot
                        + FRAME_SLOT_SIZE;

                    SOTDMA_SetNextNts(pSotdmaParaNew, tempU16);
                    SOTDMA_SetNextTmo(pSotdmaParaNew);
                   
                    /*lq ��ʼ��18����Ϣ */
                    /*lq ʹ��ITDMAͨ��״̬��keepFlag = 1���Ͳ���SotdmaPara��SendMsg18()��������SOTDMA_GetNextNts()����֮ǰ*/
                    commState.ITDMACommState.SyncState = TDMA_GetSyncState();
                    commState.ITDMACommState.SlotIncrement =
                        ITDMA_CalcCommStateSlotIncrement(pSotdmaPara->SlotOffset);
                    commState.ITDMACommState.NumberOfSlots =
                        ITDMA_CalcCommStateNumberOfSlots(1,
                        pSotdmaPara->SlotOffset);
                    commState.ITDMACommState.KeepFlag =
                        ITDMA_CalcCommStateKeepFlag(TRUE);
                    TDMA_SendMsg(channel, pSotdmaPara->NominalTxSlot,
                        AIS_MSG_COMMSTATE_ITDMA, commState);    //lq ���ò������ṩchannel, txSlot, commStateType, commState

                    /*lq �±������ʺ;ͱ��������µķ���ʱ϶�غϣ���Ϊһ�η��� */
                    SOTDMA_GetNextNts(pSotdmaPara);
                    SOTDMA_GetNextTmo(pSotdmaPara);
                    SOTDMA_GetNextNts(pSotdmaParaNew);
                    SOTDMA_GetNextTmo(pSotdmaParaNew);

                    /*lq һ֡�еı������������1 */
                    SOTDMA_FrameReportCntIncrease(pSotdmaPara);
                    SOTDMA_FrameReportCntIncrease(pSotdmaParaNew);

                    /*lq ת��ı䱨����״̬2 */
                    pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_2;
                }
            }            
            
            break;

        case SOTDMA_STA_CHANGE_REPORT_INTERVAL_2 :

            /*lq �ȴ��±��������µ���һSI */
            if (SOTDMA_IsAtSi2(currSlot, pSotdmaParaNew->NominalSlot,
                pSotdmaParaNew->SelectIntervalOffset) == TRUE)
            {
                /*lq �ȴ��±��������µ���һNTS */
                if (SOTDMA_IsSendTimArrived(pSotdmaParaNew, currFrame, currSlot) == TRUE)
                {
                    /*lq �±��������µ���һ��NS */
                    SOTDMA_SetNextNs(pSotdmaParaNew);
                    SOTDMA_GetNextNs(pSotdmaParaNew);
    
                    tempU16 = SOTDMA_GetAvailableNts(pSotdmaPara,
                        pSotdmaParaNew->NominalSlot,
                        pSotdmaParaNew->SelectIntervalOffset);

                    if (tempU16 != SLOT_INVALID_VALUE)
                    {
                        /*lq ʹ�þɱ���������Ԥռ��ʱ϶��Ϊ�±��������µķ���ʱ϶ */
                        SOTDMA_SetNextNts(pSotdmaParaNew, tempU16);
                        SOTDMA_SetNextTmo(pSotdmaParaNew);
                            
                        if (pSotdmaParaNew->NominalSlot == pSotdmaParaNew->NominalStartSlot)
                        {
                            /*lq ת��ı䱨����״̬3 */
                            pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_3;                                           
                        }
                        else
                        {
                            /*lq ��ʼ��18����Ϣ */
                            /*lq ʹ��SOTDMAͨ��״̬�Ͳ���SotdmaParaNew��SendMsg18()��������SOTDMA_GetNextNts()����֮ǰ*/
                            commState.SOTDMACommState.SyncState = TDMA_GetSyncState();
                            commState.SOTDMACommState.SlotTimeout = pSotdmaParaNew->SlotTimeout;
                            commState.SOTDMACommState.SubMessage.SubMsg =
                                SOTDMA_GetSubMsg(commState.SOTDMACommState.SlotTimeout,
                                pSotdmaParaNew);
                            TDMA_SendMsg(channel, pSotdmaParaNew->NominalTxSlot,
                                AIS_MSG_COMMSTATE_SOTDMA, commState);       //lq ���ò������ṩchannel, txSlot, commStateType, commState

                            SOTDMA_GetNextNts(pSotdmaParaNew);
                            SOTDMA_GetNextTmo(pSotdmaParaNew);
        
                            /*lq һ֡�еı������������1 */
                            SOTDMA_FrameReportCntIncrease(pSotdmaParaNew);
                        }
                    }
                    else
                    {
                        /*lq ѡ���µ�NTS */
                        if (pSotdmaParaNew->NominalSlot < currSlot)
                        {
                            tempS32 = currFrame + 1;
                        }
                        else
                        {
                            tempS32 = currFrame;
                        }
                        tempU16 = SelectCandidateSlot(channel, pSotdmaParaNew->NominalSlot,
                            pSotdmaParaNew->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_BI);
    
                        /*lq ����slotOffset */
                        pSotdmaParaNew->SlotOffset = tempU16 - pSotdmaParaNew->NominalTxSlot
                            + FRAME_SLOT_SIZE;

                        SOTDMA_SetNextNts(pSotdmaParaNew, tempU16);
                        SOTDMA_SetNextTmo(pSotdmaParaNew);
                        
                        /*lq ��ʼ��18����Ϣ */
                        /*lq ʹ��ITDMAͨ��״̬��keepFlag = 1���Ͳ���SotdmaParaNew��SendMsg18()��������SOTDMA_GetNextNts()����֮ǰ*/
                        commState.ITDMACommState.SyncState = TDMA_GetSyncState();
                        commState.ITDMACommState.SlotIncrement =
                            ITDMA_CalcCommStateSlotIncrement(pSotdmaParaNew->SlotOffset);
                        commState.ITDMACommState.NumberOfSlots =
                            ITDMA_CalcCommStateNumberOfSlots(1,
                            pSotdmaPara->SlotOffset);
                        commState.ITDMACommState.KeepFlag =
                            ITDMA_CalcCommStateKeepFlag(TRUE);
                        TDMA_SendMsg(channel, pSotdmaParaNew->NominalTxSlot,
                            AIS_MSG_COMMSTATE_ITDMA, commState);    //lq ���ò������ṩchannel, txSlot, commStateType, commState

                        SOTDMA_GetNextNts(pSotdmaParaNew);
                        SOTDMA_GetNextTmo(pSotdmaParaNew);
    
                        /*lq һ֡�еı������������1 */
                        SOTDMA_FrameReportCntIncrease(pSotdmaParaNew);       
                    }                    
                }

                /*lq �ɱ��������µ�NTS */
                if (SOTDMA_IsSendTimArrived(pSotdmaPara, currFrame, currSlot) == TRUE)
                {
                    pSotdmaPara->SlotTimeout = 0;  //lq ����Ԥռ
                    pSotdmaPara->SlotOffset = 0;

                    /*lq ��ʼ��18����Ϣ */
                    //lq �˴���������Ϣ

                    SOTDMA_GetNextNs(pSotdmaPara);
                    SOTDMA_GetNextNts(pSotdmaPara);
                    SOTDMA_GetNextTmo(pSotdmaPara);

                    SOTDMA_FrameReportCntIncrease(pSotdmaPara);                  
                }
            }
            else
            {
                if (SOTDMA_IsSendTimArrived(pSotdmaPara, currFrame, currSlot) == TRUE)
                {
                    pSotdmaPara->SlotTimeout = 0;  //lq ����Ԥռ
                    pSotdmaPara->SlotOffset = 0;

                    /*lq ��ʼ��18����Ϣ */
                    /*lq ʹ��SOTDMAͨ��״̬�Ͳ���SotdmaPara��SendMsg18()��������SOTDMA_GetNextNts()����֮ǰ*/
                    commState.SOTDMACommState.SyncState = TDMA_GetSyncState();
                    commState.SOTDMACommState.SlotTimeout = pSotdmaPara->SlotTimeout;
                    commState.SOTDMACommState.SubMessage.SubMsg =
                        SOTDMA_GetSubMsg(commState.SOTDMACommState.SlotTimeout,
                        pSotdmaPara);
                    TDMA_SendMsg(channel, pSotdmaPara->NominalTxSlot,
                        AIS_MSG_COMMSTATE_SOTDMA, commState);       //lq ���ò������ṩchannel, txSlot, commStateType, commState

                    SOTDMA_GetNextNs(pSotdmaPara);
                    SOTDMA_GetNextNts(pSotdmaPara);
                    SOTDMA_GetNextTmo(pSotdmaPara);

                    SOTDMA_FrameReportCntIncrease(pSotdmaPara);
                }            
            }

            break;

        case SOTDMA_STA_CHANGE_REPORT_INTERVAL_3 :

            /*lq ʹ���±��������µĲ��� */
            SOTDMA_SotdmaParaStructCopy(pSotdmaPara, pSotdmaParaNew);
            SOTDMA_SotdmaParaStructReset(pSotdmaParaNew);

            /*lq ת�����������׶� */
            pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CONTINUOUS_PHASE;
            
            break;

        default :
            
            SOTDMA_SotdmaSchedStructInit(pSotdmaSchedStruct);
            SOTDMA_SotdmaSchedEnable(FALSE, pSotdmaCtrlStruct);
            break;
    }
}

/*******************************************************************************
* ����  : SOTDMA_ScheduleDual
* ����  : ˫�ŵ�SOTDMA����
* ����  : pSotdmaSchedDualStruct - ָ��SotdmaSchedDualStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SOTDMA_ScheduleDual(SotdmaSchedDualStruct *pSotdmaSchedDualStruct)
{
    SotdmaSchedStruct *pSotdmaSchedA;
    SotdmaSchedStruct *pSotdmaSchedB;
    SotdmaParaStruct *pSotdmaParaA;
    SotdmaParaStruct *pSotdmaParaB;
    SotdmaCtrlStruct *pSotdmaCtrlA;
    SotdmaCtrlStruct *pSotdmaCtrlB;
    SotdmaCtrlStruct *pSotdmaCtrlStruct;
    S32 currFrame;
    S16 currSlot;
    U16 tempU16;
    U16 tempU16_1;
    S32 tempS32;

    pSotdmaCtrlStruct = &(pSotdmaSchedDualStruct->SotdmaCtrl);
    pSotdmaSchedA = &(pSotdmaSchedDualStruct->SotdmaSchedA);
    pSotdmaSchedB = &(pSotdmaSchedDualStruct->SotdmaSchedB);
    pSotdmaParaA = &(pSotdmaSchedDualStruct->SotdmaSchedA.SotdmaPara);
    pSotdmaParaB = &(pSotdmaSchedDualStruct->SotdmaSchedB.SotdmaPara);
    pSotdmaCtrlA = &(pSotdmaSchedDualStruct->SotdmaSchedA.SotdmaCtrl);
    pSotdmaCtrlB = &(pSotdmaSchedDualStruct->SotdmaSchedB.SotdmaCtrl);

    currFrame = GetFrameNum();
    currSlot = GetSlotNum();

    switch (pSotdmaCtrlStruct->CtrlState)
    {
        case SOTDMA_STA_IDLE :

            if (pSotdmaCtrlStruct->SotdmaEn == TRUE)
            {
                pSotdmaCtrlStruct->SotdmaEn = FALSE;

                /*lq ת�뿪ʼ״̬ */
                pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_START;
            }

            break;

        case SOTDMA_STA_START :

            pSotdmaCtrlStruct->TimeCnt = SetTimePeriodProcess(pSotdmaCtrlStruct->TimePeriod);

            /*lq ת���ʼ���׶� */
            pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_INIT_PHASE;            
            
            break;

        case SOTDMA_STA_INIT_PHASE :
            
            /*lq SOTDMA��ʼ���׶�1min�ȴ� */
            if (pSotdmaCtrlStruct->TimeCnt == GetLocalClockTickProcess())
            {
                pSotdmaCtrlStruct->TimeCnt = SetTimePeriodProcess(pSotdmaCtrlStruct->TimePeriod);                
                
                pSotdmaCtrlStruct->TimeoutCnt++;
                if (pSotdmaCtrlStruct->TimeoutCnt >= pSotdmaCtrlStruct->Timeout)
                {
                    pSotdmaCtrlStruct->TimeoutCnt = 0;

                    /*lq ת���������׶� */
                    pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_NETWORK_ENTRY_PHASE;                 
                }                 
            }

            break;

        case SOTDMA_STA_NETWORK_ENTRY_PHASE :
            
            /*lq ѡ��NSS */
            SOTDMA_SetNssDual(ENUM_AIS_CHANNEL_A, CalcSlotNum(0, currSlot,
                AIS_SLOT_INTERVAL_SEND_TO_TX));

	        debug_printf("$[%6d:%4d] %s : NETWORK_ENTRY_PHASE, Channel A NSS is [%4d] , Channel B NSS is [%4d]!\r\n",
                currFrame, currSlot, __func__, pSotdmaParaA->NominalStartSlot,
                pSotdmaParaB->NominalStartSlot);                        
            
            /*lq ѡ���ŵ�A��NTS */
            if (pSotdmaParaA->NominalStartSlot < currSlot)
            {
                /*lq NSSλ�ڵ�ǰ֡����һ֡ */
                tempS32 = currFrame + 1;
            }
            else
            {
                /*lq NSSλ�ڵ�ǰ֡ */
                tempS32 = currFrame;
            }
            tempU16 = SelectCandidateSlot(ENUM_AIS_CHANNEL_A, pSotdmaParaA->NominalStartSlot,
                pSotdmaParaA->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_UNI);     //lq ʹ�õ���ѡ�񣬱���ѡ���ʱ϶С�ڵ�ǰʱ϶
            
            /*lq ѡ���ŵ�B��NTS */
            if (pSotdmaParaB->NominalStartSlot < currSlot)
            {
                /*lq NSSλ�ڵ�ǰ֡����һ֡ */
                tempS32 = currFrame + 1;
            }
            else
            {
                /*lq NSSλ�ڵ�ǰ֡ */
                tempS32 = currFrame;
            }
            tempU16_1 = SelectCandidateSlot(ENUM_AIS_CHANNEL_B, pSotdmaParaB->NominalStartSlot,
                pSotdmaParaB->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_UNI);     //lq ʹ�õ���ѡ�񣬱���ѡ���ʱ϶С�ڵ�ǰʱ϶

            /*lq 
             *  ���޿��ú�ѡ����ʱ϶����ʹ�ñ��ʱ϶��Ϊ����ʱ϶��
             *  ��֮ǰ�ķ�ʽΪ�������ŵ��ĺ�ѡ����ʱ϶����Ч����ת����һ�׶�
             *   ����һֱ�޿��õķ���ʱ϶��
             */
            if (IsSlotValid(tempU16) == FALSE)
            {
                tempU16 = pSotdmaParaA->NominalStartSlot;
            }

            if (IsSlotValid(tempU16_1) == FALSE)
            {
                tempU16_1 = pSotdmaParaB->NominalStartSlot;
            }

            SOTDMA_SetFirstNs(pSotdmaParaA);
            SOTDMA_GetNextNs(pSotdmaParaA);
            SOTDMA_SetNextNts(pSotdmaParaA, tempU16);
            SOTDMA_GetNextNts(pSotdmaParaA);
            SOTDMA_SetNextTmo(pSotdmaParaA);
            SOTDMA_GetNextTmo(pSotdmaParaA);
            pSotdmaParaA->FrameReportCnt = 0;

            SOTDMA_SetFirstNs(pSotdmaParaB);
            SOTDMA_GetNextNs(pSotdmaParaB);
            SOTDMA_SetNextNts(pSotdmaParaB, tempU16_1);
            SOTDMA_GetNextNts(pSotdmaParaB);
            SOTDMA_SetNextTmo(pSotdmaParaB);
            SOTDMA_GetNextTmo(pSotdmaParaB);
            pSotdmaParaB->FrameReportCnt = 0;
            
            /*lq ת���һ֡�׶� */
            pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CONTINUOUS_PHASE;
            pSotdmaCtrlA->CtrlState = SOTDMA_STA_FIRST_FRAME_PHASE;
            pSotdmaCtrlB->CtrlState = SOTDMA_STA_FIRST_FRAME_PHASE; 

            break;

        case SOTDMA_STA_CONTINUOUS_PHASE :

            /*lq �ŵ�A/B���� */
            SOTDMA_Schedule(pSotdmaSchedA, currFrame, currSlot);
            SOTDMA_Schedule(pSotdmaSchedB, currFrame, currSlot);

            break;

        default :
            
            SOTDMA_SotdmaSchedDualStructInit(pSotdmaSchedDualStruct);
            SOTDMA_SotdmaSchedEnable(FALSE, pSotdmaCtrlStruct);
            
            break;
    }

}

#if 0
/*******************************************************************************
* ����  : SOTDMA_SetNss
* ����  : SOTDMA���ñ����ʼʱ϶����
* ����  : pSotdmaParaStruct - ָ��SotdmaParaStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SOTDMA_ScheduleDual(SotdmaSchedStruct *pSotdmaSchedStruct)
{
    SotdmaCtrlStruct *pSotdmaCtrlStruct;
    SotdmaParaStruct *pSotdmaParaA;
    SotdmaParaStruct *pSotdmaParaB;
    SotdmaParaStruct *pSotdmaParaANew;
    SotdmaParaStruct *pSotdmaParaBNew;
    S32 currFrame;
    S16 currSlot;
    U16 tempU16;
    U16 tempU16_1;
    S32 tempS32;

    pSotdmaCtrlStruct = &(pSotdmaSchedStruct->SotdmaCtrl);
    pSotdmaParaA = &(pSotdmaSchedStruct->SotdmaParaA);
    pSotdmaParaB = &(pSotdmaSchedStruct->SotdmaParaB);
    pSotdmaParaANew = &(pSotdmaSchedStruct->SotdmaParaANew);
    pSotdmaParaBNew = &(pSotdmaSchedStruct->SotdmaParaBNew);

    currFrame = GetFrameNum();
    currSlot = GetSlotNum();

    switch (pSotdmaCtrlStruct->CtrlState)
    {
        case SOTDMA_STA_IDLE :

            if (pSotdmaCtrlStruct->SotdmaEn == TRUE)
            {
                pSotdmaCtrlStruct->SotdmaEn = FALSE;

                /*lq ת�뿪ʼ״̬ */
                pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_START;
            }

            break;

        case SOTDMA_STA_START :

            pSotdmaCtrlStruct->TimeCnt = SetTimePeriodProcess(pSotdmaCtrlStruct->TimePeriod);

            /*lq ת���ʼ���׶� */
            pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_INIT_PHASE;            
            
            break;

        case SOTDMA_STA_INIT_PHASE :
            
            /*lq SOTDMA��ʼ���׶�1min�ȴ� */
            if (pSotdmaCtrlStruct->TimeCnt == GetLocalClockTickProcess())
            {
                pSotdmaCtrlStruct->TimeCnt = SetTimePeriodProcess(pSotdmaCtrlStruct->TimePeriod);                
                
                pSotdmaCtrlStruct->TimeoutCnt++;
                if (pSotdmaCtrlStruct->TimeoutCnt >= pSotdmaCtrlStruct->Timeout)
                {
                    pSotdmaCtrlStruct->TimeoutCnt = 0;

                    /*lq ת���������׶� */
                    pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_NETWORK_ENTRY_PHASE;                 
                }                 
            }

            break;

        case SOTDMA_STA_NETWORK_ENTRY_PHASE :
            
            /*lq ѡ��NSS */
            tempU16 = (currSlot + (rand() % pSotdmaParaA->NominalIncrement)) % FRAME_SLOT_SIZE;
            SotdmaSetNssDual(ENUM_AIS_CHANNEL_A, tempU16);
            
            /*lq ѡ���ŵ�A��NTS */
            tempU16 = SelectCandidateSlot(ENUM_AIS_CHANNEL_A, pSotdmaParaA->NominalStartSlot,
                pSotdmaParaA->SelectIntervalOffset, currFrame, SLOT_CAND_SELECT_UNI);   //lq ʹ�õ���ѡ�񣬱���ѡ���ʱ϶С�ڵ�ǰʱ϶
            
            /*lq ѡ���ŵ�B��NTS */
            tempU16_1 = SelectCandidateSlot(ENUM_AIS_CHANNEL_B, pSotdmaParaB->NominalStartSlot,
                pSotdmaParaB->SelectIntervalOffset, currFrame, SLOT_CAND_SELECT_UNI);   //lq ʹ�õ���ѡ�񣬱���ѡ���ʱ϶С�ڵ�ǰʱ϶

            if ((IsSlotValid(tempU16) == TRUE) && (IsSlotValid(tempU16_1) == TRUE))
            {
                SOTDMA_SetFirstNs(pSotdmaParaA);
                SOTDMA_GetNextNs(pSotdmaParaA);
                SOTDMA_SetNextNts(pSotdmaParaA, tempU16);
                SOTDMA_GetNextNts(pSotdmaParaA);
                SOTDMA_SetNextTmo(pSotdmaParaA);
                SOTDMA_GetNextTmo(pSotdmaParaA);
                pSotdmaParaA->FrameReportCnt = 0;

                SOTDMA_SetFirstNs(pSotdmaParaB);
                SOTDMA_GetNextNs(pSotdmaParaB);
                SOTDMA_SetNextNts(pSotdmaParaB, tempU16_1);
                SOTDMA_GetNextNts(pSotdmaParaB);
                SOTDMA_SetNextTmo(pSotdmaParaB);
                SOTDMA_GetNextTmo(pSotdmaParaB);
                pSotdmaParaB->FrameReportCnt = 0;
                
                /*lq ת���һ֡�׶� */
                pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_FIRST_FRAME_PHASE;
            } 
            break;

        case SOTDMA_STA_FIRST_FRAME_PHASE :
            
            /*lq �ȴ��ŵ�A��NTS */
            if (pSotdmaParaA->FrameReportCnt < pSotdmaParaA->ReportRate)
            {
                if (SOTDMA_IsAtNts(pSotdmaParaA, currSlot) == TRUE)
                {
                    /*lq �ж��Ƿ���һ֡�е����һ�η��� */
                    if ((pSotdmaParaA->FrameReportCnt + 1) >= pSotdmaParaA->ReportRate)
                    {
                        pSotdmaParaA->SlotOffset = 0;
                    }
                    else
                    {
                        /*lq ������һ��NS */
                        tempU16_1 = SOTDMA_SetNextNs(pSotdmaParaA);
    
                        /*lq ѡ����һ��NTS */
                        if (tempU16_1 < currSlot)
                        {
                            tempS32 = currFrame + 1;
                        }
                        else
                        {
                            tempS32 = currFrame;
                        }
                        tempU16 = SelectCandidateSlot(ENUM_AIS_CHANNEL_A, tempU16_1,
                            pSotdmaParaA->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_BI);
    
                        if (IsSlotValid(tempU16) == FALSE)
                        {
                            tempU16 = tempU16_1;
                        }
    
                        /*lq ����slotOffset */
                        pSotdmaParaA->SlotOffset = (tempU16 - pSotdmaParaA->NominalTxSlot
                            + FRAME_SLOT_SIZE) % FRAME_SLOT_SIZE;                    
                        
                        /*lq ������һ��NTS */
                        SOTDMA_SetNextNts(pSotdmaParaA, tempU16);

                        /*lq ������һ��slotTimeout*/
                        SOTDMA_SetNextTmo(pSotdmaParaA);
                    }
    
                    /*lq ��ʼ��18����Ϣ */
                    /*lq ʹ���ŵ�A��ITDMA��ʽ���õ�������SOTDMA_GetNextNts()����֮ǰ*/
                    SendMsg18();    //lq... �ӿ�SendMsg18()������ʵ�֣����ò������ṩchannel(txSlot, slotOffset, slotTimeout), tdmaSchema
    
                    SOTDMA_GetNextNs(pSotdmaParaA);
                    SOTDMA_GetNextNts(pSotdmaParaA);
                    SOTDMA_GetNextTmo(pSotdmaParaA);
                    SOTDMA_FrameReportCntIncrease(pSotdmaParaA);
                }            
            }

            /*lq �ȴ��ŵ�B��NTS */
            if (pSotdmaParaB->FrameReportCnt < pSotdmaParaB->ReportRate)
            {
                if (SOTDMA_IsAtNts(pSotdmaParaB, currSlot) == TRUE)
                {
                    /*lq �ж��Ƿ���һ֡�е����һ�η��� */
                    if ((pSotdmaParaB->FrameReportCnt + 1) >= pSotdmaParaB->ReportRate)
                    {
                        pSotdmaParaB->SlotOffset = 0;
                    }
                    else
                    {
                        /*lq ������һ��NS */
                        tempU16_1 = SOTDMA_SetNextNs(pSotdmaParaB);
    
                        /*lq ѡ����һ��NTS */
                        if (tempU16_1 < currSlot)
                        {
                            tempS32 = currFrame + 1;
                        }
                        else
                        {
                            tempS32 = currFrame;
                        }
                        tempU16 = SelectCandidateSlot(ENUM_AIS_CHANNEL_B, tempU16_1,
                            pSotdmaParaB->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_BI);
    
                        if (IsSlotValid(tempU16) == FALSE)
                        {
                            tempU16 = tempU16_1;
                        }
    
                        /*lq ����slotOffset */
                        pSotdmaParaB->SlotOffset = (tempU16 - pSotdmaParaB->NominalTxSlot
                            + FRAME_SLOT_SIZE) % FRAME_SLOT_SIZE;                    
                        
                        /*lq ������һ��NTS */
                        SOTDMA_SetNextNts(pSotdmaParaB, tempU16);

                        /*lq ������һ��slotTimeout*/
                        SOTDMA_SetNextTmo(pSotdmaParaB);
                    }
    
                    /*lq ��ʼ��18����Ϣ */
                    /*lq ʹ���ŵ�A��ITDMA��ʽ���õ�������SOTDMA_GetNextNts()����֮ǰ*/
                    SendMsg18();    //lq... �ӿ�SendMsg18()������ʵ�֣����ò������ṩchannel(txSlot, slotOffset, slotTimeout), tdmaSchema
    
                    SOTDMA_GetNextNs(pSotdmaParaB);
                    SOTDMA_GetNextNts(pSotdmaParaB);
                    SOTDMA_GetNextTmo(pSotdmaParaB);
                    SOTDMA_FrameReportCntIncrease(pSotdmaParaB);
                }            
            }

            /*lq �жϵ�һ֡�׶��Ƿ���� */
            if ((pSotdmaParaA->FrameReportCnt >= pSotdmaParaA->ReportRate)
                && (pSotdmaParaB->FrameReportCnt >= pSotdmaParaB->ReportRate))
            {
                pSotdmaParaA->FrameReportCnt = 0;
                pSotdmaParaB->FrameReportCnt = 0;

                /*lq ת�����������׶� */
                pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CONTINUOUS_PHASE;
            }

            break;

        case SOTDMA_STA_CONTINUOUS_PHASE :

            /*lq �ȴ��ŵ�A��NTS */
            if (SOTDMA_IsAtNts(pSotdmaParaA, currSlot) == TRUE)            
            {
                /*lq �ж��Ƿ�ΪԤռ�����һ֡ */
                if (pSotdmaParaA->SlotTimeout == 0)
                {
                    /*lq Ϊ��ǰ��NSѡ���µ�NTS */    
                    tempU16 = SelectCandidateSlot(ENUM_AIS_CHANNEL_A, pSotdmaParaA->NominalSlot,
                        pSotdmaParaA->SelectIntervalOffset, currFrame, SLOT_CAND_SELECT_BI);

                    if (IsSlotValid(tempU16) == FALSE)
                    {
                        tempU16 = pSotdmaParaA->NominalSlot;                    
                    }

                    /*lq ����slotOffset */
                    pSotdmaParaA->SlotOffset = tempU16 - pSotdmaParaA->NominalTxSlot
                        + FRAME_SLOT_SIZE;
                    
                    /*lq ����NTS */                                            
                    SOTDMA_SetNts(pSotdmaParaA, pSotdmaParaA->FrameReportCnt,
                        tempU16);

                    /*lq ����slotTimeout */                                            
                    pSotdmaParaA->SlotTimeout = SOTDMA_GetTmo(pSotdmaParaA,
                        pSotdmaParaA->FrameReportCnt);
                }
                else
                {
                    pSotdmaParaA->SlotOffset = 0;                
                }

                /*lq ��ʼ��18����Ϣ */
                SendMsg18();    //lq... �ӿ�SendMsg18()������ʵ�֣�ʹ���ŵ�A��SOTDMA��ʽ

                SOTDMA_GetNextNs(pSotdmaParaA);
                SOTDMA_GetNextNts(pSotdmaParaA);
                SOTDMA_GetNextTmo(pSotdmaParaA);

                /*lq һ֡�еı������������1 */
                SOTDMA_FrameReportCntIncrease(pSotdmaParaA);
            }

            /*lq �ȴ��ŵ�B��NTS */
            if (SOTDMA_IsAtNts(pSotdmaParaB, currSlot) == TRUE)            
            {
                /*lq �ж��Ƿ�ΪԤռ�����һ֡ */
                if (pSotdmaParaB->SlotTimeout == 0)
                {
                    /*lq Ϊ��ǰ��NSѡ���µ�NTS */    
                    tempU16 = SelectCandidateSlot(ENUM_AIS_CHANNEL_B, pSotdmaParaB->NominalSlot,
                        pSotdmaParaB->SelectIntervalOffset, currFrame, SLOT_CAND_SELECT_BI);

                    if (IsSlotValid(tempU16) == FALSE)
                    {
                        tempU16 = pSotdmaParaB->NominalSlot;                    
                    }

                    /*lq ����slotOffset */
                    pSotdmaParaB->SlotOffset = tempU16 - pSotdmaParaB->NominalTxSlot
                        + FRAME_SLOT_SIZE;
                                        
                    /*lq ����NTS */                                            
                    SOTDMA_SetNts(pSotdmaParaB, pSotdmaParaB->FrameReportCnt,
                        tempU16);

                    /*lq ����slotTimeout */                                            
                    pSotdmaParaB->SlotTimeout = SOTDMA_GetTmo(pSotdmaParaB,
                        pSotdmaParaB->FrameReportCnt);

                }
                else
                {
                    pSotdmaParaB->SlotOffset = 0;                
                }

                /*lq ��ʼ��18����Ϣ */
                SendMsg18();    //lq... �ӿ�SendMsg18()������ʵ�֣�ʹ���ŵ�B��SOTDMA��ʽ

                SOTDMA_GetNextNs(pSotdmaParaB);
                SOTDMA_GetNextNts(pSotdmaParaB);
                SOTDMA_GetNextTmo(pSotdmaParaB);

                /*lq һ֡�еı������������1 */
                SOTDMA_FrameReportCntIncrease(pSotdmaParaB);
            }

            if (SOTDMA_IsRrChanged(ENUM_AIS_CHANNEL_A) == TRUE)
            {
                SOTDMA_SotdmaParaStructReset(pSotdmaParaANew);

                /*lq ת��ı䱨����״̬1 */
                pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_1;                             
            }

            break;

        case SOTDMA_STA_CHANGE_REPORT_INTERVAL_1 :

            /*lq �ȴ��ŵ�A��NTS */
            if (SOTDMA_IsAtNts(pSotdmaParaA, currSlot) == TRUE)            
            {
                /*lq �±���������һ֡�еĵ�һ�η��� */
                SOTDMA_SetNss(pSotdmaParaANew, pSotdmaParaA->NominalSlot);
                SOTDMA_SetFirstNs(pSotdmaParaANew);
                SOTDMA_GetNextNs(pSotdmaParaANew);
                SOTDMA_SetNextNts(pSotdmaParaANew, pSotdmaParaA->NominalTxSlot);
                SOTDMA_GetNextNts(pSotdmaParaANew);
                SOTDMA_SetNextTmo(pSotdmaParaANew);
                SOTDMA_GetNextTmo(pSotdmaParaANew);

                if (pSotdmaParaA->SlotTimeout == 0)
                {
                    pSotdmaParaA->SlotTimeout = 1;
                }

                /*lq �±���������һ֡�еĵڶ��η��� */
                SOTDMA_SetNextNs(pSotdmaParaANew);
                SOTDMA_GetNextNs(pSotdmaParaANew);

                tempU16 = SOTDMA_GetAvailableNts(pSotdmaParaA,
                    pSotdmaParaANew->NominalSlot,
                    pSotdmaParaANew->SelectIntervalOffset);
                
                if (tempU16 != SLOT_INVALID_VALUE)
                {
                    /*lq ʹ�þɱ�����������Ԥռ��ʱ϶��Ϊ�±��������µķ���ʱ϶ */
                    SOTDMA_SetNextNts(pSotdmaParaANew, tempU16);
                    SOTDMA_SetNextTmo(pSotdmaParaANew);

                    if (pSotdmaParaANew->NominalSlot == pSotdmaParaANew->NominalStartSlot)
                    {
                        /*lq ת��ı䱨����״̬3 */
                        pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_3;                                           
                    }
                    else
                    {                 
                        /*lq ��ʼ��18����Ϣ */
                        SendMsg18();    //lq... ʹ�ò���SotdmaParaA��ʹ��SOTDMA��ʽ

                        /*lq �±������ʺ;ͱ��������µķ���ʱ϶�غϣ���Ϊһ�η��� */
                        SOTDMA_GetNextNts(pSotdmaParaA);
                        SOTDMA_GetNextTmo(pSotdmaParaA);
                        SOTDMA_GetNextNts(pSotdmaParaANew);
                        SOTDMA_GetNextTmo(pSotdmaParaANew);
    
                        /*lq һ֡�еı������������1 */
                        SOTDMA_FrameReportCntIncrease(pSotdmaParaA);
                        SOTDMA_FrameReportCntIncrease(pSotdmaParaANew);

                        /*lq ת��ı䱨����״̬2 */
                        pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_2;
                    }
                }
                else
                {
                    /*lq ѡ���µ�NTS */
                    if (pSotdmaParaANew->NominalSlot < currSlot)
                    {
                        tempS32 = currFrame + 1;
                    }
                    else
                    {
                        tempS32 = currFrame;
                    }
                    tempU16 = SelectCandidateSlot(ENUM_AIS_CHANNEL_A, pSotdmaParaANew->NominalSlot,
                        pSotdmaParaANew->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_BI);
                    if (IsSlotValid(tempU16) == FALSE)
                    {
                        tempU16 = pSotdmaParaANew->NominalSlot;
                    }

                    /*lq ����slotOffset */
                    pSotdmaParaA->SlotOffset = tempU16 - pSotdmaParaA->NominalTxSlot
                        + FRAME_SLOT_SIZE;

                    SOTDMA_SetNextNts(pSotdmaParaANew, tempU16);
                    SOTDMA_SetNextTmo(pSotdmaParaANew);
                   
                    /*lq ��ʼ��18����Ϣ */
                    SendMsg18();    //lq... ʹ�ò���SotdmaParaA��ʹ��ITDMA��ʽ��keepFlag = 1��

                    /*lq �±������ʺ;ͱ��������µķ���ʱ϶�غϣ���Ϊһ�η��� */
                    SOTDMA_GetNextNts(pSotdmaParaA);
                    SOTDMA_GetNextTmo(pSotdmaParaA);
                    SOTDMA_GetNextNts(pSotdmaParaANew);
                    SOTDMA_GetNextTmo(pSotdmaParaANew);

                    /*lq һ֡�еı������������1 */
                    SOTDMA_FrameReportCntIncrease(pSotdmaParaA);
                    SOTDMA_FrameReportCntIncrease(pSotdmaParaANew);

                    /*lq ת��ı䱨����״̬2 */
                    pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_2;
                }
            }            
            
            break;

        case SOTDMA_STA_CHANGE_REPORT_INTERVAL_2 :

            /*lq �ȴ��±��������µ���һSI */
            if (SOTDMA_IsAtSi(currSlot, pSotdmaParaANew->NominalSlot,
                pSotdmaParaANew->SelectIntervalOffset) == TRUE)
            {
                /*lq �ȴ��±��������µ���һNTS */
                if (SOTDMA_IsAtNts(pSotdmaParaANew, currSlot) == TRUE)
                {
                    /*lq �±��������µ���һ��NS */
                    SOTDMA_SetNextNs(pSotdmaParaANew);
                    SOTDMA_GetNextNs(pSotdmaParaANew);
    
                    tempU16 = SOTDMA_GetAvailableNts(pSotdmaParaA,
                        pSotdmaParaANew->NominalSlot,
                        pSotdmaParaANew->SelectIntervalOffset);

                    if (tempU16 != SLOT_INVALID_VALUE)
                    {
                        /*lq ʹ�þɱ���������Ԥռ��ʱ϶��Ϊ�±��������µķ���ʱ϶ */
                        SOTDMA_SetNextNts(pSotdmaParaANew, tempU16);
                        SOTDMA_SetNextTmo(pSotdmaParaANew);
                            
                        if (pSotdmaParaANew->NominalSlot == pSotdmaParaANew->NominalStartSlot)
                        {
                            /*lq ת��ı䱨����״̬3 */
                            pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_3;                                           
                        }
                        else
                        {
                            /*lq ��ʼ��18����Ϣ */
                            SendMsg18();    //lq... ʹ�ò���SotdmaParaANew��ʹ��SOTDMA��ʽ

                            SOTDMA_GetNextNts(pSotdmaParaANew);
                            SOTDMA_GetNextTmo(pSotdmaParaANew);
        
                            /*lq һ֡�еı������������1 */
                            SOTDMA_FrameReportCntIncrease(pSotdmaParaANew);
                        }
                    }
                    else
                    {
                        /*lq ѡ���µ�NTS */
                        if (pSotdmaParaANew->NominalSlot < currSlot)
                        {
                            tempS32 = currFrame + 1;
                        }
                        else
                        {
                            tempS32 = currFrame;
                        }
                        tempU16 = SelectCandidateSlot(ENUM_AIS_CHANNEL_A, pSotdmaParaANew->NominalSlot,
                            pSotdmaParaANew->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_BI);
    
                        SOTDMA_SetNextNts(pSotdmaParaANew, tempU16);
                        SOTDMA_SetNextTmo(pSotdmaParaANew);
                        
                        /*lq ��ʼ��18����Ϣ */
                        SendMsg18();    //lq... ʹ�ò���SotdmaParaANew��ʹ��ITDMA��ʽ

                        SOTDMA_GetNextNts(pSotdmaParaANew);
                        SOTDMA_GetNextTmo(pSotdmaParaANew);
    
                        /*lq һ֡�еı������������1 */
                        SOTDMA_FrameReportCntIncrease(pSotdmaParaANew);       
                    }                    
                }

                /*lq �ɱ��������µ�NTS */
                if (SOTDMA_IsAtNts(pSotdmaParaA, currSlot) == TRUE)
                {
                    pSotdmaParaA->SlotTimeout = 0;  //lq ����Ԥռ
                    pSotdmaParaA->SlotOffset = 0;

                    /*lq ��ʼ��18����Ϣ */
                    //lq �˴���������Ϣ

                    SOTDMA_GetNextNs(pSotdmaParaA);
                    SOTDMA_GetNextNts(pSotdmaParaA);
                    SOTDMA_GetNextTmo(pSotdmaParaA);

                    SOTDMA_FrameReportCntIncrease(pSotdmaParaA);                  
                }
            }
            else
            {
                if (SOTDMA_IsAtNts(pSotdmaParaA, currSlot) == TRUE)
                {
                    pSotdmaParaA->SlotTimeout = 0;  //lq ����Ԥռ
                    pSotdmaParaA->SlotOffset = 0;

                    /*lq ��ʼ��18����Ϣ */
                    SendMsg18();    //lq... ʹ�ò���SotdmaParaA��ʹ��SOTDMA��ʽ

                    SOTDMA_GetNextNs(pSotdmaParaA);
                    SOTDMA_GetNextNts(pSotdmaParaA);
                    SOTDMA_GetNextTmo(pSotdmaParaA);

                    SOTDMA_FrameReportCntIncrease(pSotdmaParaA);
                }            
            }

            break;

        case SOTDMA_STA_CHANGE_REPORT_INTERVAL_3 :

            /*lq ʹ���±��������µĲ��� */
            SOTDMA_SotdmaParaStructCopy(pSotdmaParaA, pSotdmaParaANew);
            SOTDMA_SotdmaParaStructReset(pSotdmaParaANew);

            /*lq ת�����������׶� */
            pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CONTINUOUS_PHASE;
            
            break;

        default :
            
            SOTDMA_SotdmaCtrlStructInit(pSotdmaCtrlStruct);

            break;
    }
}

#endif

/*******************************************************************************
* ����  : ITDMA_ItdmaParaStructInit
* ����  : ItdmaParaStruct�ṹ���ʼ��
* ����  : pItdmaParaStruct - ָ��ItdmaParaStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void ITDMA_ItdmaParaStructInit(ItdmaParaStruct *pItdmaParaStruct)
{
    pItdmaParaStruct->NominalStartFrame = 0;
    pItdmaParaStruct->NominalStartSlot = 0;
    pItdmaParaStruct->NominalFrame = 0;
    pItdmaParaStruct->NominalSlot = 0;
    pItdmaParaStruct->NominalFrameNext = 0;
    pItdmaParaStruct->NominalSlotNext = 0;
    pItdmaParaStruct->NominalTxFrame = 0;
    pItdmaParaStruct->NominalTxSlot = 0;
    pItdmaParaStruct->NominalTxFrameNext = 0;
    pItdmaParaStruct->NominalTxSlotNext = 0;

    pItdmaParaStruct->NominalIncrement = ITDMA_REPORT_INTERVAL_SLOT_6M;
    pItdmaParaStruct->SelectIntervalOffset =
        ITDMA_CalcSelectIntervalOffset(pItdmaParaStruct->NominalIncrement);
    pItdmaParaStruct->SlotOffset = 0;
    pItdmaParaStruct->SlotQuantity = 0;
    pItdmaParaStruct->SlotKeep = FALSE;
}

/*******************************************************************************
* ����  : ITDMA_ItdmaCtrlStructInit
* ����  : ItdmaCtrlStruct�ṹ���ʼ��
* ����  : pItdmaCtrlStruct - ָ��ItdmaCtrlStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void ITDMA_ItdmaCtrlStructInit(ItdmaCtrlStruct *pItdmaCtrlStruct)
{
    pItdmaCtrlStruct->TimeCnt = 0;
    pItdmaCtrlStruct->TimePeriod = ITDMA_CTRL_TIM_PERIOD;
    pItdmaCtrlStruct->Timeout = 0;
    pItdmaCtrlStruct->TimeoutCnt = 0;
    pItdmaCtrlStruct->CtrlState = ITDMA_STA_IDLE;
    pItdmaCtrlStruct->ItdmaEn = FALSE;
}

/*******************************************************************************
* ����  : ITDMA_ItdmaSchedStructInit
* ����  : ItdmaSchedStruct�ṹ���ʼ��
* ����  : pItdmaSchedStruct - ָ��ItdmaSchedStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void ITDMA_ItdmaSchedStructInit(ItdmaSchedStruct *pItdmaSchedStruct)
{
    ITDMA_ItdmaParaStructInit(&(pItdmaSchedStruct->ItdmaPara));
    ITDMA_ItdmaParaStructInit(&(pItdmaSchedStruct->ItdmaParaNew));
    ITDMA_ItdmaCtrlStructInit(&(pItdmaSchedStruct->ItdmaCtrl));
}

/*******************************************************************************
* ����  : ITDMA_ItdmaSchedDualStructInit
* ����  : ItdmaSchedDualStruct�ṹ���ʼ��
* ����  : pItdmaSchedDualStruct - ָ��ItdmaSchedDualStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void ITDMA_ItdmaSchedDualStructInit(ItdmaSchedDualStruct *pItdmaSchedDualStruct)
{
    ITDMA_ItdmaSchedStructInit(&(pItdmaSchedDualStruct->ItdmaSchedA));
    ITDMA_ItdmaSchedStructInit(&(pItdmaSchedDualStruct->ItdmaSchedB));
    ITDMA_ItdmaCtrlStructInit(&(pItdmaSchedDualStruct->ItdmaCtrl));
}

/*******************************************************************************
* ����  : ITDMA_ItdmaSchedEnable
* ����  : ITDMA����ʹ������
* ����  : flag - ʹ�ܱ�־
          pItdmaCtrlStruct - ָ��ItdmaCtrlStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void ITDMA_ItdmaSchedEnable(U8 flag, ItdmaCtrlStruct *pItdmaCtrlStruct)
{
    pItdmaCtrlStruct->CtrlState = ITDMA_STA_IDLE;
    pItdmaCtrlStruct->ItdmaEn = flag;
}

#if   0  // lnw mask  no use
/*******************************************************************************
* ����  : ITDMA_IsAtNts
* ����  : ITDMA�ж��Ƿ�λ�ڱ�Ʒ���ʱ϶
* ����  : pItdmaParaStruct - ָ��ItdmaParaStruct�ṹ���ָ��
          frame - �ж�֡
          slot - �ж�ʱ϶
* ���  : ��
* ����  : ��
*******************************************************************************/
static U8 ITDMA_IsAtNts(ItdmaParaStruct *pItdmaParaStruct, S32 frame, S16 slot)
{     
    if ((pItdmaParaStruct->NominalTxFrame == frame)
        && (pItdmaParaStruct->NominalTxSlot == slot))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif
/*******************************************************************************
* ����  : ITDMA_IsNtsArrived
* ����  : ITDMA�ж��Ƿ񵽴��Ʒ���ʱ϶
* ����  : pItdmaParaStruct - ָ��ItdmaParaStruct�ṹ���ָ��
          frame - �ж�֡
          slot - �ж�ʱ϶
* ���  : ��
* ����  : ��
*******************************************************************************/
static U8 ITDMA_IsNtsArrived(ItdmaParaStruct *pItdmaParaStruct, S32 frame,
                             S16 slot)
{     
    if (CalcSlotOffset(pItdmaParaStruct->NominalTxFrame,
        pItdmaParaStruct->NominalTxSlot, frame, slot) >= 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : ITDMA_IsSendTimArrived
* ����  : ITDMA�жϷ���ʱ϶��������Ϣ���е�ʱ϶���Ƿ񵽴�
* ����  : pItdmaParaStruct - ָ��ItdmaParaStruct�ṹ���ָ��
          frame - �ж�֡
          slot - �ж�ʱ϶
* ���  : ��
* ����  : ��
*******************************************************************************/
static U8 ITDMA_IsSendTimArrived(ItdmaParaStruct *pItdmaParaStruct, S32 frame, S16 slot)
{     
    return ITDMA_IsNtsArrived(pItdmaParaStruct,
            CalcFrameNum(frame, slot, AIS_SLOT_INTERVAL_SEND_TO_TX),
            CalcSlotNum(frame, slot, AIS_SLOT_INTERVAL_SEND_TO_TX));
}

/*******************************************************************************
* ����  : ITDMA_SetNi
* ����  : ITDMA���ñ������
* ����  : pItdmaParaStruct - ָ��ItdmaParaStruct�ṹ���ָ��
          slotIncrement - �������
* ���  : ��
* ����  : ��
*******************************************************************************/
static void ITDMA_SetNi(ItdmaParaStruct *pItdmaParaStruct, S16 slotIncrement)
{     
    if (slotIncrement > ITDMA_REPORT_INTERVAL_SLOT_6M)
    {
        pItdmaParaStruct->NominalIncrement = ITDMA_REPORT_INTERVAL_SLOT_6M;
    }
    else
    {
        pItdmaParaStruct->NominalIncrement = slotIncrement;
    }
}

/*******************************************************************************
* ����  : ITDMA_SetNi
* ����  : ITDMA���ñ������
* ����  : pItdmaParaStruct - ָ��ItdmaParaStruct�ṹ���ָ��
          slotIncrement - �������
* ���  : ��
* ����  : ��
*******************************************************************************/
static void ITDMA_SetSelectIntervalOffset(ItdmaParaStruct *pItdmaParaStruct)
{
    pItdmaParaStruct->SelectIntervalOffset = 
        ITDMA_CalcSelectIntervalOffset(pItdmaParaStruct->NominalIncrement);
}

/*******************************************************************************
* ����  : ITDMA_SetSlotIncrement
* ����  : ITDMA����ʱ϶ƫ����
* ����  : pItdmaParaStruct - ָ��ItdmaParaStruct�ṹ���ָ��
          slotOffset - ʱ϶ƫ����
* ���  : ��
* ����  : ��
*******************************************************************************/
static void ITDMA_SetSlotOffset(ItdmaParaStruct *pItdmaParaStruct, S16 slotOffset)
{     
    if (slotOffset > ITDMA_SLOT_OFFSET_MAX)
    {
        pItdmaParaStruct->SlotOffset = ITDMA_SLOT_OFFSET_MAX;        
    }
    else
    {
        pItdmaParaStruct->SlotOffset = slotOffset;    
    }
}

/*******************************************************************************
* ����  : ITDMA_SetSlotQuantity
* ����  : ITDMA����Ԥռʱ϶��
* ����  : pItdmaParaStruct - ָ��ItdmaParaStruct�ṹ���ָ��
          slotQuantity - Ԥռʱ϶��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void ITDMA_SetSlotQuantity(ItdmaParaStruct *pItdmaParaStruct, S16 slotQuantity)
{     
    if (slotQuantity > ITDMA_SLOT_QUANTITY_MAX)
    {
        pItdmaParaStruct->SlotQuantity = ITDMA_SLOT_QUANTITY_MAX;    
    }
    else
    {
        pItdmaParaStruct->SlotQuantity = slotQuantity;
    }
}

/*******************************************************************************
* ����  : ITDMA_SetSlotKeep
* ����  : ITDMA����ʱ϶���ֱ�־
* ����  : pItdmaParaStruct - ָ��ItdmaParaStruct�ṹ���ָ��
          slotKeep - ���ֱ�־
* ���  : ��
* ����  : ��
*******************************************************************************/
static void ITDMA_SetSlotKeep(ItdmaParaStruct *pItdmaParaStruct, U8 slotKeep)
{     
    if (slotKeep == FALSE)
    {
        pItdmaParaStruct->SlotKeep = FALSE;    
    }
    else
    {
        pItdmaParaStruct->SlotKeep = TRUE;
    }
}

/*******************************************************************************
* ����  : ITDMA_CalcCommStateSlotIncrement
* ����  : ����ʱ϶ƫ��������ITDMAͨ��״̬�е�ʱ϶�����ֶ�ֵ
* ����  : slotOffset - ʱ϶ƫ����
* ���  : ��
* ����  : ITDMAͨ��״̬�е�ʱ϶�����ֶ�ֵ
*******************************************************************************/
S16 ITDMA_CalcCommStateSlotIncrement(S16 slotOffset)
{
    S16 slotIncrement;

    if (slotOffset > ITDMA_COMMSTATE_SLOT_INCREMENT_MAX)
    {
        slotIncrement = slotOffset - ITDMA_COMMSTATE_SLOT_INCREMENT_MAX - 1; 

        if (slotOffset > ITDMA_COMMSTATE_SLOT_INCREMENT_MAX)
        {
            slotOffset = ITDMA_COMMSTATE_SLOT_INCREMENT_MAX;
        }
    }
    else
    {
        slotIncrement = slotOffset;
    }

    return slotIncrement;
}

/*******************************************************************************
* ����  : ITDMA_CalcSlotOffset
* ����  : ����ITDMAͨ��״̬�е�ʱ϶������ʱ϶�����ֶ�ֵ����ʱ϶ƫ����
* ����  : slotIncrement - ITDMAͨ��״̬�е�ʱ϶�����ֶ�ֵ
          numberOfSlots - ITDMAͨ��״̬�е�ʱ϶�����ֶ�ֵ
* ���  : ��
* ����  : ʱ϶ƫ����
*******************************************************************************/
S16 ITDMA_CalcSlotOffset(U16 slotIncrement, U8 numberOfSlots)
{
    S16 slotOffset;
    S16 tempS16;

    /*lq ��Ч���ж� */
    if (slotIncrement > ITDMA_COMMSTATE_SLOT_INCREMENT_MAX)
    {
        tempS16 = ITDMA_COMMSTATE_SLOT_INCREMENT_MAX;
    }
    else
    {
        tempS16 = slotIncrement;    
    }

    if (numberOfSlots < ITDMA_COMMSTATE_NUM_OF_SLOTS_SPLIT)
    {
        slotOffset = tempS16; 
    }
    else
    {
        slotOffset = tempS16 + ITDMA_COMMSTATE_SLOT_INCREMENT_MAX + 1;
    }

    return slotOffset;
}

/*******************************************************************************
* ����  : ITDMA_CalcCommStateNumberOfSlots
* ����  : ����ʱ϶������ʱ϶ƫ��������ITDMAͨ��״̬�е�ʱ϶�����ֶ�ֵ
* ����  : numOfSlots - ʱ϶����
          slotOffset - ʱ϶ƫ����
* ���  : ��
* ����  : ITDMAͨ��״̬�е�ʱ϶�����ֶ�ֵ
*******************************************************************************/
U8 ITDMA_CalcCommStateNumberOfSlots(U8 numOfSlots, S16 slotOffset)
{
    U8 numberOfSlots;

    if (slotOffset > ITDMA_COMMSTATE_SLOT_INCREMENT_MAX)
    {
        if (numOfSlots > ITDMA_CONSECUTIVE_SLOTS_QUANTITY_2_MAX)
        {
            numberOfSlots = ITDMA_CONSECUTIVE_SLOTS_QUANTITY_2_MAX + 4;
        }
        else
        {
            numberOfSlots = numOfSlots + 4;               
        }
    }
    else
    {
        if (numOfSlots > ITDMA_CONSECUTIVE_SLOTS_QUANTITY_1_MAX)
        {
            numberOfSlots = ITDMA_CONSECUTIVE_SLOTS_QUANTITY_1_MAX - 1;   
        }
        else
        {
            numberOfSlots = numOfSlots - 1;               
        }
    }

    return numberOfSlots;
}

/*******************************************************************************
* ����  : ITDMA_CalcSlotQuantity
* ����  : ����ITDMAͨ��״̬�е�ʱ϶�����ֶ�ֵ��������Ԥռ��ʱ϶����
* ����  : numberOfSlots - ITDMAͨ��״̬�е�ʱ϶�����ֶ�ֵ
* ���  : ��
* ����  : ����Ԥռ��ʱ϶����
*******************************************************************************/
U8 ITDMA_CalcSlotQuantity(U8 numberOfSlots)
{
    S16 slotQuantity;
    S16 tempS16;

    /*lq ��Ч���ж� */
    if (numberOfSlots > ITDMA_COMMSTATE_NUM_OF_SLOTS_MAX)
    {
        tempS16 = ITDMA_COMMSTATE_NUM_OF_SLOTS_MAX;
    }
    else
    {
        tempS16 = numberOfSlots;    
    }

    if (numberOfSlots < ITDMA_COMMSTATE_NUM_OF_SLOTS_SPLIT)
    {
        slotQuantity = tempS16 + 1; 
    }
    else
    {
        slotQuantity = tempS16 - 4;
    }

    return slotQuantity;
}

/*******************************************************************************
* ����  : ITDMA_CalcCommStateKeepFlag
* ����  : ����ʱ϶���ֱ�־����ITDMAͨ��״̬�е�ʱ϶�����ֶ�ֵ
* ����  : slotKeep - ʱ϶���ֱ�־
* ���  : ��
* ����  : ITDMAͨ��״̬�е�ʱ϶�����ֶ�ֵ
*******************************************************************************/
U8 ITDMA_CalcCommStateKeepFlag(U8 slotKeep)
{
    if (slotKeep == FALSE)
    {
        return 0;    
    }
    else
    {
        return 1;
    }
}

/*******************************************************************************
* ����  : ITDMA_CalcSlotKeep
* ����  : ����ITDMAͨ��״̬�е�ʱ϶�����ֶ�ֵ����ʱ϶���ֱ�־
* ����  : keepFlag - ITDMAͨ��״̬�е�ʱ϶�����ֶ�ֵ
* ���  : ��
* ����  : ʱ϶���ֱ�־
*******************************************************************************/
U8 ITDMA_CalcSlotKeep(U8 keepFlag)
{
    if (keepFlag == 0)
    {
        return FALSE;    
    }
    else
    {
        return TRUE;
    }
}

/*******************************************************************************
* ����  : ITDMA_CalcSelectIntervalOffset
* ����  : ����ITDMA�ı����������ʱ϶ѡ������ƫ����
* ����  : nominalIncrement - ITDMA�������
* ���  : ��
* ����  : ʱ϶ѡ������ƫ����
*******************************************************************************/
S16 ITDMA_CalcSelectIntervalOffset(S16 nominalIncrement)
{
    return nominalIncrement * ITDMA_SI_OFFSET_SCALE;
}

/*******************************************************************************
* ����  : ITDMA_SetNss
* ����  : ITDMA���ñ����ʼʱ϶
* ����  : pItdmaParaStruct - ָ��ItdmaParaStruct�ṹ���ָ��
          frame - ֡��
          slot - ʱ϶��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void ITDMA_SetNss(ItdmaParaStruct *pItdmaParaStruct, S32 frame, S16 slot)
{      
    pItdmaParaStruct->NominalStartFrame = frame;
    pItdmaParaStruct->NominalStartSlot = slot;
}

#if   0  // lnw mask  no use
/*******************************************************************************
* ����  : ITDMA_SetNss
* ����  : ITDMA���ñ��ʱ϶
* ����  : pItdmaParaStruct - ָ��ItdmaParaStruct�ṹ���ָ��
          frame - ֡��
          slot - ʱ϶��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void ITDMA_SetNs(ItdmaParaStruct *pItdmaParaStruct, S32 frame, S16 slot)
{      
    pItdmaParaStruct->NominalFrame = frame;
    pItdmaParaStruct->NominalSlot = slot;
}
#endif
/*******************************************************************************
* ����  : ITDMA_GetFirstNs
* ����  : ITDMA��ȡ�׸����ʱ϶
* ����  : pItdmaParaStruct - ָ��ItdmaParaStruct�ṹ���ָ��
          frame - ֡��
          slot - ʱ϶��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void ITDMA_GetFirstNs(ItdmaParaStruct *pItdmaParaStruct)
{      
    pItdmaParaStruct->NominalFrame = pItdmaParaStruct->NominalStartFrame;
    pItdmaParaStruct->NominalSlot = pItdmaParaStruct->NominalStartSlot;
}

/*******************************************************************************
* ����  : ITDMA_SetNextNs
* ����  : ITDMA������һ���ʱ϶
* ����  : pItdmaParaStruct - ָ��ItdmaParaStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void ITDMA_SetNextNs(ItdmaParaStruct *pItdmaParaStruct)
{      
    S32 tempS32;

    tempS32 = pItdmaParaStruct->NominalSlot + pItdmaParaStruct->NominalIncrement;
    pItdmaParaStruct->NominalFrameNext = pItdmaParaStruct->NominalFrame
        + tempS32 / FRAME_SLOT_SIZE;
    pItdmaParaStruct->NominalSlotNext = tempS32 % FRAME_SLOT_SIZE;
}

/*******************************************************************************
* ����  : ITDMA_GetNextNs
* ����  : ITDMA��ȡ��һ���ʱ϶
* ����  : pItdmaParaStruct - ָ��ItdmaParaStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void ITDMA_GetNextNs(ItdmaParaStruct *pItdmaParaStruct)
{      
    pItdmaParaStruct->NominalFrame = pItdmaParaStruct->NominalFrameNext;
    pItdmaParaStruct->NominalSlot = pItdmaParaStruct->NominalSlotNext;
}

/*******************************************************************************
* ����  : ITDMA_SetNextNs
* ����  : ITDMA������һ���ʱ϶
* ����  : pItdmaParaStruct - ָ��ItdmaParaStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void ITDMA_SetNextNts(ItdmaParaStruct *pItdmaParaStruct, S32 frame, S16 slot)
{      
    pItdmaParaStruct->NominalTxFrameNext = frame;
    pItdmaParaStruct->NominalTxSlotNext = slot;
}

/*******************************************************************************
* ����  : ITDMA_GetNextNs
* ����  : ITDMA��ȡ��һ���ʱ϶
* ����  : pItdmaParaStruct - ָ��ItdmaParaStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void ITDMA_GetNextNts(ItdmaParaStruct *pItdmaParaStruct)
{      
    pItdmaParaStruct->NominalTxFrame = pItdmaParaStruct->NominalTxFrameNext;
    pItdmaParaStruct->NominalTxSlot = pItdmaParaStruct->NominalTxSlotNext;
}

/*******************************************************************************
* ����  : ITDMA_Schedule
* ����  : ���ŵ�ITDMA���ȡ��õ��ȷ�������������ITDMA���뷽ʽ������ITDMA���뷽ʽ
          �����á�
* ����  : pItdmaSchedStruct - ָ��ItdmaSchedStruct�ṹ���ָ��
          currFrame - ��ǰ֡��
          currSlot - ��ǰʱ϶��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void ITDMA_Schedule(ItdmaSchedStruct *pItdmaSchedStruct, S32 currFrame, S16 currSlot)
{
    AisChannelEnum channel;
    ItdmaCtrlStruct *pItdmaCtrlStruct;
    ItdmaParaStruct *pItdmaPara;
//    ItdmaParaStruct *pItdmaParaNew;
    AISMsgCommStateUnion commState;
    U16 tempU16;
//    U16 tempU16_1;
    S32 tempS32;

    if (pItdmaSchedStruct == &(TdmaSched.ItdmaSched.ItdmaSchedA))
    {
        channel = ENUM_AIS_CHANNEL_A;
    }
    else if (pItdmaSchedStruct == &(TdmaSched.ItdmaSched.ItdmaSchedB))
    {
        channel = ENUM_AIS_CHANNEL_B;
    }
    else
    {
        return ;
    }
    pItdmaCtrlStruct = &(pItdmaSchedStruct->ItdmaCtrl);
    pItdmaPara = &(pItdmaSchedStruct->ItdmaPara);
  //  pItdmaParaNew = &(pItdmaSchedStruct->ItdmaParaNew);

    switch (pItdmaCtrlStruct->CtrlState)
    {
        case ITDMA_STA_IDLE :

            if (pItdmaCtrlStruct->ItdmaEn == TRUE)
            {
                pItdmaCtrlStruct->ItdmaEn = FALSE;

                /*lq ת�뿪ʼ״̬ */
                pItdmaCtrlStruct->CtrlState = ITDMA_STA_CONTINUOUS_PHASE;
            
		        debug_printf("$[%6d:%4d] %s %s : State to ITDMA_STA_CONTINUOUS_PHASE!\r\n",
                    currFrame, currSlot, __func__,
                    channel == ENUM_AIS_CHANNEL_A ? AIS_CHANNEL_NAME_A : AIS_CHANNEL_NAME_B);                        
            }

            break;

        case ITDMA_STA_CONTINUOUS_PHASE :

            /*lq �ȴ���ǰ�ŵ���NTS */
            if (ITDMA_IsSendTimArrived(pItdmaPara, currFrame, currSlot) == TRUE)            
            {                
                ITDMA_SetNextNs(pItdmaPara);
                ITDMA_GetNextNs(pItdmaPara);

                /*lq Ϊ��ǰ��NSѡ���µ�NTS */    
                tempU16 = SelectCandidateSlot(channel, pItdmaPara->NominalSlot,
                    pItdmaPara->SelectIntervalOffset, pItdmaPara->NominalFrame,
                    SLOT_CAND_SELECT_BI);

                if (IsSlotValid(tempU16) == FALSE)
                {
                    ITDMA_SetNextNts(pItdmaPara, pItdmaPara->NominalFrame,
                        pItdmaPara->NominalSlot);
                }
                else
                {
                    tempS32 = SelectCandidateSlotFrame(tempU16,
                        pItdmaPara->NominalSlot, pItdmaPara->NominalFrame);

                    ITDMA_SetNextNts(pItdmaPara, tempS32, tempU16);
                }

                /*lq ����ʱ϶ƫ���� */
                tempS32 = CalcSlotOffset(pItdmaPara->NominalTxFrame,
                    pItdmaPara->NominalTxSlot, pItdmaPara->NominalTxFrameNext,
                    pItdmaPara->NominalTxSlotNext);

                ITDMA_SetSlotOffset(pItdmaPara, tempS32);
                ITDMA_SetSlotQuantity(pItdmaPara, 1);
                ITDMA_SetSlotKeep(pItdmaPara, TRUE);
                
                /*lq ��ʼ��18����Ϣ */
                /*lq ʹ��ITDMAͨ��״̬��SendMsg18()��������ITDMA_GetNextNts()����֮ǰ*/
                commState.ITDMACommState.SyncState = TDMA_GetSyncState();
                commState.ITDMACommState.SlotIncrement =
                    ITDMA_CalcCommStateSlotIncrement(pItdmaPara->SlotOffset);
                commState.ITDMACommState.NumberOfSlots =
                    ITDMA_CalcCommStateNumberOfSlots(pItdmaPara->SlotQuantity,
                        pItdmaPara->SlotOffset);
                commState.ITDMACommState.KeepFlag =
                    ITDMA_CalcCommStateKeepFlag(pItdmaPara->SlotKeep);

                TDMA_SendMsg(channel, pItdmaPara->NominalTxSlot,
                    AIS_MSG_COMMSTATE_ITDMA, commState);    //lq ���ò������ṩchannel, txSlot, commStateType, commState

                ITDMA_GetNextNts(pItdmaPara);
            }

            break;

        default :
            
            ITDMA_ItdmaSchedStructInit(pItdmaSchedStruct);
            ITDMA_ItdmaSchedEnable(FALSE, pItdmaCtrlStruct);
            break;
    }
}


/*******************************************************************************
* ����  : ITDMA_ScheduleDual
* ����  : ˫�ŵ�ITDMA����
* ����  : pItdmaSchedDualStruct - ָ��ItdmaSchedDualStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void ITDMA_ScheduleDual(ItdmaSchedDualStruct *pItdmaSchedDualStruct)
{
    ItdmaSchedStruct *pItdmaSchedA;
    ItdmaSchedStruct *pItdmaSchedB;
//    ItdmaParaStruct *pItdmaParaA;
//    ItdmaParaStruct *pItdmaParaB;
    ItdmaCtrlStruct *pItdmaCtrlA;
    ItdmaCtrlStruct *pItdmaCtrlB;
    ItdmaCtrlStruct *pItdmaCtrlStruct;
    S32 currFrame;
    S16 currSlot;
//    U16 tempU16;
//    U16 tempU16_1;
//    S32 tempS32;

    pItdmaCtrlStruct = &(pItdmaSchedDualStruct->ItdmaCtrl);
    pItdmaSchedA = &(pItdmaSchedDualStruct->ItdmaSchedA);
    pItdmaSchedB = &(pItdmaSchedDualStruct->ItdmaSchedB);
   // pItdmaParaA = &(pItdmaSchedDualStruct->ItdmaSchedA.ItdmaPara);
  //  pItdmaParaB = &(pItdmaSchedDualStruct->ItdmaSchedB.ItdmaPara);
    pItdmaCtrlA = &(pItdmaSchedDualStruct->ItdmaSchedA.ItdmaCtrl);
    pItdmaCtrlB = &(pItdmaSchedDualStruct->ItdmaSchedB.ItdmaCtrl);

    currFrame = GetFrameNum();
    currSlot = GetSlotNum();

    switch (pItdmaCtrlStruct->CtrlState)
    {
        case ITDMA_STA_IDLE :

            if (pItdmaCtrlStruct->ItdmaEn == TRUE)
            {
                pItdmaCtrlStruct->ItdmaEn = FALSE;

                ITDMA_ItdmaSchedEnable(TRUE, pItdmaCtrlA);
                ITDMA_ItdmaSchedEnable(TRUE, pItdmaCtrlB);

                /*lq ת�뿪ʼ״̬ */
                pItdmaCtrlStruct->CtrlState = ITDMA_STA_CONTINUOUS_PHASE;
            
		        debug_printf("$[%6d:%4d] %s : State to ITDMA_STA_CONTINUOUS_PHASE!\r\n",
                    currFrame, currSlot, __func__);
            }

            break;

        case ITDMA_STA_CONTINUOUS_PHASE :

            /*lq �ŵ�A/B���� */
            ITDMA_Schedule(pItdmaSchedA, currFrame, currSlot);
            ITDMA_Schedule(pItdmaSchedB, currFrame, currSlot);

            break;

        default :
            
            ITDMA_ItdmaSchedDualStructInit(pItdmaSchedDualStruct);
            ITDMA_ItdmaSchedEnable(FALSE, pItdmaCtrlStruct);
            
            break;
    }

}

/*******************************************************************************
* ����  : ITDMA_SetSlotState
* ����  : ����ͨ��״̬����ǰ֡�ż�ʱ϶������ʱ϶ӳ�������Ӧʱ϶��״̬
* ����  : channel - ʱ϶���ڵ��ŵ�
          commState - SOTDMAͨ��״̬
          currFrame - ��ǰ֡��
          currSlot - ��ǰʱ϶��
          slotState - ʱ϶״̬
* ���  : ��
* ����  : ��
*******************************************************************************/
void ITDMA_SetSlotState(AisChannelEnum channel, ITDMACommStateStruct commState,
                        S32 currFrame, S16 currSlot, U8 slotState)
{
    S32 tempS32;
    S16 tempS16;
    S16 tempS16_2;
    S16 tempS16_3;
    S16 tempS16_4;
    S16 i;
    U8  keepFlg;
    
    keepFlg = ITDMA_CalcSlotKeep(commState.KeepFlag);
    tempS16 = ITDMA_CalcSlotQuantity(commState.NumberOfSlots);
    tempS16_4 = ITDMA_CalcSlotOffset(commState.SlotIncrement,
            commState.NumberOfSlots);

    if (tempS16_4 != 0)
    {
        for (i = 0; i < tempS16; i++)
        {
            tempS16_2 = tempS16_4 + i;
    
            tempS32 = CalcFrameNum(currFrame, currSlot, tempS16_2);            
            tempS16_3 = CalcSlotNum(currFrame, currSlot, tempS16_2);
    
            SetSlotState(channel, tempS16_3, slotState, tempS32);
            
            if (keepFlg == TRUE)
            {
                SetSlotState(channel, tempS16_3, slotState, tempS32 + 1);
            }        
        }
    }
}

/*******************************************************************************
* ����  : TDMA_TdmaCtrlStructInit
* ����  : TdmaCtrlStruct�ṹ���ʼ��
* ����  : pTdmaCtrlStruct - ָ��TdmaCtrlStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void TDMA_TdmaCtrlStructInit(TdmaCtrlStruct *pTdmaCtrlStruct)
{
    pTdmaCtrlStruct->TimePeriod = TDMA_CTRL_TIM_PERIOD;
    pTdmaCtrlStruct->TimeCnt = SetTimePeriodProcess(pTdmaCtrlStruct->TimePeriod);
    pTdmaCtrlStruct->Timeout = TDMA_CTRL_TIMEOUT_REPORT_INTERVAL_SAMPLE;
    pTdmaCtrlStruct->TimeoutCnt = 0;

    pTdmaCtrlStruct->CtrlState = TDMA_STA_IDLE;
    pTdmaCtrlStruct->TdmaEn = FALSE;
    pTdmaCtrlStruct->SchedSchema = ENUM_TDMA_SCHED_SCHEMA_SOTDMA;
    pTdmaCtrlStruct->ReportIntervalSlot = TDMA_REPORT_INTERVAL_SLOT_30S;    //lq �ó�ʼֵ����SotdmaParaStruct.ReportRate�ĳ�ʼֵ��ƥ��
    pTdmaCtrlStruct->ReportIntervalSlotSample = TDMA_REPORT_INTERVAL_SLOT_30S;
    pTdmaCtrlStruct->ReportIntervalChangedCnt = 0;

}

/*******************************************************************************
* ����  : TDMA_TdmaSchedStructInit
* ����  : TdmaSchedStruct�ṹ���ʼ��
* ����  : pTdmaSchedStruct - ָ��TdmaSchedStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void TDMA_TdmaSchedStructInit(TdmaSchedStruct *pTdmaSchedStruct)
{
    SOTDMA_SotdmaSchedDualStructInit(&(pTdmaSchedStruct->SotdmaSched));
    ITDMA_ItdmaSchedDualStructInit(&(pTdmaSchedStruct->ItdmaSched));
    TDMA_TdmaCtrlStructInit(&(pTdmaSchedStruct->TdmaCtrl));
}

/*******************************************************************************
* ����  : TDMA_TdmaSchedEnable
* ����  : TDMA����ʹ������
* ����  : flag - ʹ�ܱ�־
          pTdmaCtrlStruct - ָ��TdmaCtrlStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void TDMA_TdmaSchedEnable(U8 flag, TdmaCtrlStruct *pTdmaCtrlStruct)
{
    pTdmaCtrlStruct->CtrlState = TDMA_STA_IDLE;
    pTdmaCtrlStruct->TdmaEn = flag;
}

/*******************************************************************************
* ����  : TDMA_InstReportIntervalSample
* ����  : TDMAʵ������������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void TDMA_InstReportIntervalSample(S16 reportInterval)
{
    TdmaSched.TdmaCtrl.ReportIntervalSlotSample = reportInterval;
}

/*******************************************************************************
* ����  : TDMA_Schedule
* ����  : TDMA���ȣ���ǰ�����ǵ����ֱ������Ѹı�ʱ����������һ����Ϣ�������ǵ�
          ��ʹ�þɱ�����Ԥռ����һ�η���
* ����  : pTdmaSchedStruct - ָ��TdmaSchedStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void TDMA_Schedule(TdmaSchedStruct *pTdmaSchedStruct)
{
    SotdmaSchedDualStruct *pSotdmaSched;
    ItdmaSchedDualStruct *pItdmaSched;
    TdmaCtrlStruct *pTdmaCtrl;
    S32 currFrame;
    S16 currSlot;

    pSotdmaSched = &(pTdmaSchedStruct->SotdmaSched);
    pItdmaSched = &(pTdmaSchedStruct->ItdmaSched);
    pTdmaCtrl = &(pTdmaSchedStruct->TdmaCtrl);

    /*lq ��ȡ����֡�ź�ʱ϶�� */
    currFrame = GetFrameNum();
    currSlot = GetSlotNum();

    switch (pTdmaCtrl->CtrlState)
    {
        case TDMA_STA_IDLE :
            
            if (GetGpsPowerupFixState() == TRUE)
            {
                if (pTdmaCtrl->TdmaEn == TRUE)
                {
                    pTdmaCtrl->TdmaEn = FALSE;
                    
                    /*lq ����ʹ��SOTDMA���ȷ��� */
                    SOTDMA_SotdmaSchedDualStructInit(pSotdmaSched);
                    SOTDMA_SotdmaSchedEnable(TRUE, &(pSotdmaSched->SotdmaCtrl));
                    SOTDMA_SotdmaSchedEnable(FALSE, &(pSotdmaSched->SotdmaSchedA.SotdmaCtrl));
                    SOTDMA_SotdmaSchedEnable(FALSE, &(pSotdmaSched->SotdmaSchedB.SotdmaCtrl));
    
                    ITDMA_ItdmaSchedDualStructInit(pItdmaSched);
                    ITDMA_ItdmaSchedEnable(FALSE, &(pItdmaSched->ItdmaCtrl));
                    ITDMA_ItdmaSchedEnable(FALSE, &(pItdmaSched->ItdmaSchedA.ItdmaCtrl));
                    ITDMA_ItdmaSchedEnable(FALSE, &(pItdmaSched->ItdmaSchedB.ItdmaCtrl));
    
                    pTdmaCtrl->CtrlState = TDMA_STA_CONTINUOUS_PHASE;
                    
                    debug_printf("$[%6d:%4d] %s : State to CONTINUOUS_PHASE!\r\n",
                        currFrame, currSlot, __func__);                                       
                }
            }

            break;

        case TDMA_STA_CONTINUOUS_PHASE :
            
            /*lq ������������ʱ */
            if (pTdmaCtrl->TimeCnt == GetLocalClockTickProcess())
            {
                pTdmaCtrl->TimeCnt = SetTimePeriodProcess(pTdmaCtrl->TimePeriod);
                
                pTdmaCtrl->TimeoutCnt++;
                if (pTdmaCtrl->TimeoutCnt >= pTdmaCtrl->Timeout)
                {
                    /*lq ������������ʱ�� */
                    pTdmaCtrl->TimeoutCnt = 0;
                
                    /*lq ���б����������ж� */
                    if (pTdmaCtrl->ReportIntervalSlotSample
                        != pTdmaCtrl->ReportIntervalSlot)
                    {
                        pTdmaCtrl->ReportIntervalChangedCnt++;
                        if (pTdmaCtrl->ReportIntervalChangedCnt
                            >= TDMA_REPORT_INTERVAL_CHANGED_CNT_THRES)
                        {
                            /*lq �ı����ʱ��ﵽҪ��ȷ�ϸı䱨���� */
                            pTdmaCtrl->ReportIntervalChangedCnt = 0;
        
                            if (pTdmaCtrl->ReportIntervalSlotSample
                                > TDMA_REPORT_INTERVAL_SLOT_30S)
                            {
                                /*lq
                                 *  ����˫���ŵ�����������1��0.5��min ��
                                 *  ʹ��ITDMA���ȷ�ʽ
                                 */    
                                ITDMA_ItdmaSchedDualStructInit(pItdmaSched);
                                
                                /*lq A�ŵ��������� */
                                ITDMA_SetNi(&(pItdmaSched->ItdmaSchedA.ItdmaPara),
                                    pTdmaCtrl->ReportIntervalSlotSample * 2);
                                
                                ITDMA_SetSelectIntervalOffset(
                                    &(pItdmaSched->ItdmaSchedA.ItdmaPara));
                                
                                ITDMA_SetNss(&(pItdmaSched->ItdmaSchedA.ItdmaPara),
                                    CalcFrameNum(currFrame, currSlot,
                                        AIS_SLOT_INTERVAL_SEND_TO_TX),
                                    CalcSlotNum(currFrame, currSlot,
                                        AIS_SLOT_INTERVAL_SEND_TO_TX));
                                
                                ITDMA_GetFirstNs(&(pItdmaSched->ItdmaSchedA.ItdmaPara));
                                
                                ITDMA_SetNextNts(&(pItdmaSched->ItdmaSchedA.ItdmaPara),
                                    pItdmaSched->ItdmaSchedA.ItdmaPara.NominalFrame,
                                    pItdmaSched->ItdmaSchedA.ItdmaPara.NominalSlot);
                                
                                ITDMA_GetNextNts(&(pItdmaSched->ItdmaSchedA.ItdmaPara));
        
                                /*lq B�ŵ��������� */
                                ITDMA_SetNi(&(pItdmaSched->ItdmaSchedB.ItdmaPara),
                                    pTdmaCtrl->ReportIntervalSlotSample * 2);
                                
                                ITDMA_SetSelectIntervalOffset(
                                    &(pItdmaSched->ItdmaSchedB.ItdmaPara));
                                
                                ITDMA_SetNss(&(pItdmaSched->ItdmaSchedB.ItdmaPara),
                                    CalcFrameNum(currFrame, currSlot,
                                        pTdmaCtrl->ReportIntervalSlotSample
                                        + AIS_SLOT_INTERVAL_SEND_TO_TX),
                                    CalcSlotNum(currFrame, currSlot,
                                        pTdmaCtrl->ReportIntervalSlotSample
                                        + AIS_SLOT_INTERVAL_SEND_TO_TX));
                                
                                ITDMA_GetFirstNs(&(pItdmaSched->ItdmaSchedB.ItdmaPara));
                                
                                ITDMA_SetNextNts(&(pItdmaSched->ItdmaSchedB.ItdmaPara),
                                    pItdmaSched->ItdmaSchedB.ItdmaPara.NominalFrame,
                                    pItdmaSched->ItdmaSchedB.ItdmaPara.NominalSlot);
                                
                                ITDMA_GetNextNts(&(pItdmaSched->ItdmaSchedB.ItdmaPara));
        
                                /*lq ʹ��ITDMA���� */
                                pItdmaSched->ItdmaCtrl.CtrlState = ITDMA_STA_IDLE;
                                ITDMA_ItdmaSchedEnable(TRUE, &(pItdmaSched->ItdmaCtrl));
        
                                /*lq �ر�SOTDMA���� */
                                SOTDMA_SotdmaSchedEnable(FALSE, &(pSotdmaSched->SotdmaCtrl));
                            
                                /*lq ��ΪITDMA���ȷ�ʽ */
                                pTdmaCtrl->SchedSchema = ENUM_TDMA_SCHED_SCHEMA_ITDMA;
                            
                                debug_printf("$[%6d:%4d] %s : TDMA has changed to ITDMA, Report interval is [%d] slots now!\r\n",
                                    currFrame, currSlot, __func__, pTdmaCtrl->ReportIntervalSlotSample);                                                                   
                            }
                            else
                            {
                                /*lq
                                 *  ����˫���ŵ�������δ����1��0.5��min��
                                 *  ʹ��SOTDMA���ȷ�ʽ
                                 */    
                                if (pTdmaCtrl->SchedSchema == ENUM_TDMA_SCHED_SCHEMA_SOTDMA)
                                {
                                    /*lq ֮ǰʹ�õĽ��뷽ʽΪSOTDMA */
                                    SOTDMA_SetRr(&(pSotdmaSched->SotdmaSchedA.SotdmaParaNew),
                                        FRAME_SLOT_SIZE / (pTdmaCtrl->ReportIntervalSlotSample * 2));
                                    SOTDMA_SetRr(&(pSotdmaSched->SotdmaSchedB.SotdmaParaNew),
                                        FRAME_SLOT_SIZE / (pTdmaCtrl->ReportIntervalSlotSample * 2));

                                    debug_printf("$[%6d:%4d] %s : TDMA remains SOTDMA, Report interval is [%d] slots now!\r\n",
                                        currFrame, currSlot, __func__, pTdmaCtrl->ReportIntervalSlotSample);                                                                   
                                }
                                else
                                {
                                    /*lq ֮ǰʹ�õĽ��뷽ʽΪITDMA */
                                    SOTDMA_SotdmaSchedDualStructInit(pSotdmaSched);
                                    
                                    SOTDMA_SetRrDual(FRAME_SLOT_SIZE
                                        / (pTdmaCtrl->ReportIntervalSlotSample * 2));
                                    
                                    SOTDMA_SetNssDual(ENUM_AIS_CHANNEL_A,
                                        CalcSlotNum(currFrame, currSlot,
                                            AIS_SLOT_INTERVAL_SEND_TO_TX));
        
                                    /*lq �¾ɲ����ĺ��ٱ���һ�� */
                                    SOTDMA_SotdmaParaStructCopy(
                                        &(pSotdmaSched->SotdmaSchedA.SotdmaParaNew),
                                        &(pSotdmaSched->SotdmaSchedA.SotdmaPara));
        
                                    SOTDMA_SotdmaParaStructCopy(
                                        &(pSotdmaSched->SotdmaSchedB.SotdmaParaNew),
                                        &(pSotdmaSched->SotdmaSchedB.SotdmaPara));
        
                                    /*lq ����SOTDMA���� */
                                    pSotdmaSched->SotdmaCtrl.CtrlState = SOTDMA_STA_NETWORK_ENTRY_PHASE;    //lq ֱ�ӽ����������׶Σ��ƹ���ʼ���׶Σ���ʼ���׶ν����豸������ִ��һ�Σ�
            
                                    /*lq �ر�ITDMA���� */
                                    ITDMA_ItdmaSchedEnable(FALSE, &(pItdmaSched->ItdmaCtrl));
        
                                    /*lq ��ΪSOTDMA���ȷ�ʽ */
                                    pTdmaCtrl->SchedSchema = ENUM_TDMA_SCHED_SCHEMA_SOTDMA;

                                    debug_printf("$[%6d:%4d] %s : TDMA has changed to SOTDMA, Report interval is [%d] slots now!\r\n",
                                        currFrame, currSlot, __func__, pTdmaCtrl->ReportIntervalSlotSample);                                                                   
                                }
                            }
        
                            /*lq ���±����� */
                            pTdmaCtrl->ReportIntervalSlot = 
                                pTdmaCtrl->ReportIntervalSlotSample;
                        }
                    }
                    else
                    {
                        pTdmaCtrl->ReportIntervalChangedCnt = 0;
                    }
                }
            }
        
            /*lq ˫�ŵ�SOTDMA���� */
            if (pTdmaCtrl->SchedSchema == ENUM_TDMA_SCHED_SCHEMA_SOTDMA)
            {
                SOTDMA_ScheduleDual(pSotdmaSched);
            }
            else
            {
                ITDMA_ScheduleDual(pItdmaSched);
            }

            break;
        
        default :

            TDMA_TdmaSchedStructInit(pTdmaSchedStruct);
            TDMA_TdmaSchedEnable(TRUE, pTdmaCtrl);
            
            break;
    }

}

/*******************************************************************************
* ����  : TDMA_Schedule
* ����  : TDMA���ȣ���ǰ�����ǵ����ֱ������Ѹı�ʱ����������һ����Ϣ�������ǵ�
          ��ʹ�þɱ�����Ԥռ����һ�η���
* ����  : pTdmaSchedStruct - ָ��TdmaSchedStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void TDMA_InstSchedule(void)
{
    TDMA_Schedule(&TdmaSched);
}

/*******************************************************************************
* ����  : TDMA_SetSlotState
* ����  : ����ͨ��״̬����ǰ֡�ż�ʱ϶������ʱ϶ӳ�������Ӧʱ϶��״̬
* ����  : channel - ʱ϶���ڵ��ŵ�
          commStateType - ͨ��״̬����
          commState - SOTDMAͨ��״̬
          currFrame - ��ǰ֡��
          currSlot - ��ǰʱ϶��
          slotState - ʱ϶״̬
* ���  : ��
* ����  : ��
*******************************************************************************/
void TDMA_SetSlotState(AisChannelEnum channel, AISMsgCommStateEnum commStateType,AISMsgCommStateUnion commState, S32 currFrame,S16 currSlot, U8 slotState)
{
    /*lq ����ͬ����֮��ſ�����ʱ϶״̬ */
    if (GetGpsPowerupFixState() == TRUE)
    {
        if (commStateType == AIS_MSG_COMMSTATE_ITDMA)
        {
            ITDMA_SetSlotState(channel, commState.ITDMACommState, currFrame,
                currSlot, slotState);
        }
        else
        {
            SOTDMA_SetSlotState(channel, commState.SOTDMACommState, currFrame,
                currSlot, slotState);    
        }
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
static U8 TDMA_SendMsg(AisChannelEnum channel, S16 txSlot,
             AISMsgCommStateEnum commStateType, AISMsgCommStateUnion commState)
{
   U8   RTN_Value=0; // lnw add
   	
    if (DevStaticPara.AisClass == AIS_CLASS_B)    
    {
        /*lq Class B */
        RTN_Value=SendMsg18(channel, txSlot, commStateType, commState);   //lnw modify     
    }
    else
    {
        /*lq Class A */
        if (commStateType == AIS_MSG_COMMSTATE_SOTDMA)
        {
            /*lq ʹ��SOTDMAͨ��״̬������Msg1 */
            RTN_Value=SendMsg1(channel, txSlot, commState); // lnw modify      
        }
        else
        {
            /*lq ʹ��ITDMAͨ��״̬������Msg3 */
            RTN_Value=SendMsg3(channel, txSlot, commState);  //lnw modify      
        }
    }

	return  RTN_Value;  // lnw  add
}

/*******************************************************************************
* ����  : TDMA_ModuleInit
* ����  : ģ���ʼ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void TDMA_ModuleInit(void) 
{
    TDMA_TdmaSchedStructInit(&TdmaSched);
    TDMA_TdmaSchedEnable(TRUE, &(TdmaSched.TdmaCtrl));
}
