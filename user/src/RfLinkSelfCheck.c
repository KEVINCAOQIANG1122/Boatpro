/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: RfLinkSelfCheck.c
  Author: lq       Version : V1.0.0        Date: 2015.07.22
  Description:     ��Ƶ��·�Լ�ģ��
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

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* ����  : RFL_RfLinkSelfCheckStructInit
* ����  : ��ʼ���ṹ��
* ����  : pRfLinkSelfCheckStruct - RfLinkSelfCheckStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void RFL_RfLinkSelfCheckStructInit(RfLinkSelfCheckStruct *pRfLinkSelfCheckStruct)
{
	pRfLinkSelfCheckStruct->TimeCnt = 0;
	pRfLinkSelfCheckStruct->TimePeriod = RFL_SLFCHK_TIME_PERIOD;	
    pRfLinkSelfCheckStruct->Timeout = RFL_SLFCHK_TIME_TIMEOUT_TX;
    pRfLinkSelfCheckStruct->TimeoutCnt = 0;

    pRfLinkSelfCheckStruct->CtrlState = RFL_SLFCHK_CTRL_STATE_IDLE;
    pRfLinkSelfCheckStruct->StartFlg = TRUE;
    pRfLinkSelfCheckStruct->CompleteFlg = FALSE;

    pRfLinkSelfCheckStruct->RfLinkOkFlg = FALSE;           
}

/*******************************************************************************
* ����  : RFL_RfLinkSelfCheckReset
* ����  : �ṹ�帴λ
* ����  : pRfLinkSelfCheckStruct - RfLinkSelfCheckStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void RFL_RfLinkSelfCheckReset(RfLinkSelfCheckStruct *pRfLinkSelfCheckStruct)
{
    pRfLinkSelfCheckStruct->StartFlg = FALSE;
    pRfLinkSelfCheckStruct->CompleteFlg = FALSE;
    pRfLinkSelfCheckStruct->RfLinkOkFlg = FALSE;           
}

/*******************************************************************************
* ����  : RFL_SelfCheck
* ����  : �Լ����
* ����  : pRfLinkSelfCheckStruct - RfLinkSelfCheckStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void RFL_SelfCheck(RfLinkSelfCheckStruct *pRfLinkSelfCheckStruct)
{
    MsgCellStruct msgCell;

    switch (pRfLinkSelfCheckStruct->CtrlState)
    {
        case RFL_SLFCHK_CTRL_STATE_IDLE:
            
            if (pRfLinkSelfCheckStruct->StartFlg == TRUE)
            {
                RFL_RfLinkSelfCheckReset(pRfLinkSelfCheckStruct);

                /*lq SI446X���� */
                SI446X_ModuleInit();
        		
                pRfLinkSelfCheckStruct->Timeout = RFL_SLFCHK_TIME_TIMEOUT_TX;
                pRfLinkSelfCheckStruct->TimeoutCnt = 0;
                pRfLinkSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pRfLinkSelfCheckStruct->TimePeriod);

                pRfLinkSelfCheckStruct->CtrlState = RFL_SLFCHK_CTRL_STATE_TX;            
            }

            break; 
        case RFL_SLFCHK_CTRL_STATE_TX:

            /*lq ���ͳ�ʱ�ж� */
            if (pRfLinkSelfCheckStruct->TimeCnt == GetLocalClockTickProcess())
        	{
        		pRfLinkSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pRfLinkSelfCheckStruct->TimePeriod);
                pRfLinkSelfCheckStruct->TimeoutCnt++;                
                if (pRfLinkSelfCheckStruct->TimeoutCnt >= pRfLinkSelfCheckStruct->Timeout)
                {
                    pRfLinkSelfCheckStruct->TimeoutCnt = 0;

                	/*lq ʹ���ŵ�B����һ��6�Ų�����Ϣ */
//                    SendMsg6Test(ENUM_AIS_CHANNEL_B, SLOT_FORCE_TX_VALUE);
					SendMsg18Test(ENUM_AIS_CHANNEL_B, SLOT_FORCE_TX_VALUE);
            		
                    pRfLinkSelfCheckStruct->Timeout = RFL_SLFCHK_TIME_TIMEOUT_RX;
                    pRfLinkSelfCheckStruct->TimeoutCnt = 0;
                    pRfLinkSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pRfLinkSelfCheckStruct->TimePeriod);
                    pRfLinkSelfCheckStruct->CtrlState = RFL_SLFCHK_CTRL_STATE_RX;
                }                        
            }    

            break;
        case RFL_SLFCHK_CTRL_STATE_RX:

            /*lq ��Ϣ�����ж� */
            if (MSGMAN_RxMsgListGetElem(&msgCell) == TRUE)
            {
//                if (AISMSG_ParseMsg(&msgCell.Data[0]) == AISMsgType6)
//                {
//                    if (AISMsg_6.DestinationID == AisStaticPara.MMSI)
//                    {
//                        if ((AISMsg_6.AI_DAC == RFL_SLFCHK_MSG6_AI_DAC)
//                            && (AISMsg_6.AI_FI == RFL_SLFCHK_MSG6_AI_FI)
//                            && (AISMsg_6.ApplicationData[0] == RFL_SLFCHK_MSG6_AD)
//                            && (AISMsg_6.AppDataBitLen == RFL_SLFCHK_MSG6_AD_BIT_LEN))
//                        {
				if (AISMSG_ParseMsg(&msgCell.Data[0]) == AISMsgType18)
	                {
	                    if (AISMsg_18.UserID == AIS_INFO_MMSI_DEFAULT)
	                    {
	                        /*lq RL Link OK */
	                        pRfLinkSelfCheckStruct->RfLinkOkFlg = TRUE;
	                        pRfLinkSelfCheckStruct->CompleteFlg = TRUE;
	                        
							UartResponseMsg("B is OK!");
	                        pRfLinkSelfCheckStruct->CtrlState = RFL_SLFCHK_CTRL_STATE_IDLE;                                                   
//                        }
	                    }
	                }
            }
            
        	/*lq ���ճ�ʱ�ж� */
            if (pRfLinkSelfCheckStruct->TimeCnt == GetLocalClockTickProcess())
        	{
        		pRfLinkSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pRfLinkSelfCheckStruct->TimePeriod);
                pRfLinkSelfCheckStruct->TimeoutCnt++;                
                if (pRfLinkSelfCheckStruct->TimeoutCnt >= pRfLinkSelfCheckStruct->Timeout)
                {
                    pRfLinkSelfCheckStruct->TimeoutCnt = 0;

                    pRfLinkSelfCheckStruct->CompleteFlg = TRUE;
                    
					UartResponseMsg("B is not OK!");
                    pRfLinkSelfCheckStruct->CtrlState = RFL_SLFCHK_CTRL_STATE_IDLE;                                                   
                }                        
            }    
            
            break;
        default :
            
            RFL_RfLinkSelfCheckStructInit(pRfLinkSelfCheckStruct);            
            break;
    }   
}

/*==============================================================================
------------------------------------End of File---------------------------------
==============================================================================*/
