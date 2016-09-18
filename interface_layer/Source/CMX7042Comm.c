/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: CMX7042Comm.c
  Author: lq       Version : V1.00        Date: 2014.05.21
  Description:     CMX7042��Ϣ���ͺͽ���ͨ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "stm32f10x_lib.h"
#include "def.h"
#include "CMX7042.h"
#include "CMX7042Comm.h"
#include "sotdma.h"
#include "RxTxCtrl.h"

CMX7042CommStruct CMX7042CommInstance;
U8 CMX7042TestMsgFlg = FALSE;
U8 CMX7042SovTxFlg = FALSE;

/*******************************************************************************
* ����  : CMXCOMM_InitStruct
* ����  : ��ʼ���ṹ��CMX7042CommStruct����
* ����  : pCMX7042CommStruct-�ṹ��ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static CMXCOMM_InitStruct(CMX7042CommStruct* pCMX7042CommStruct)
{
    pCMX7042CommStruct->RxFIFO.FIFOInput = 0;
    pCMX7042CommStruct->RxFIFO.FIFOOutput = 0;

    pCMX7042CommStruct->TxFIFO.FIFOInput = 0;
    pCMX7042CommStruct->TxFIFO.FIFOOutput = 0;
    pCMX7042CommStruct->TxCtrlCell.TimePeriod = CMXCOMM_TIMEOUT_TX_DONE_WAITING;
    pCMX7042CommStruct->TxCtrlCell.TimeCnt = 0;
    pCMX7042CommStruct->TxCtrlCell.State = CMXCOMM_TX_CTRL_STATE_IDLE;
}

/*******************************************************************************
* ����  : CMXCOMM_InitInstance
* ����  : ��ʼ���ṹ��CMX7042CommStructʵ��
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMXCOMM_InitInstance(void)
{
    CMXCOMM_InitStruct(&CMX7042CommInstance);
}

/*******************************************************************************
* ����  : CMXCOMM_CtrlProcess
* ����  : CMX7042ͨ�ſ��ƽ���
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMXCOMM_CtrlProcess(void)
{
	U16	data[4];

	/*lq... CMX7042ʱ϶���£������޷���ͽ�������ʱ���У������л�����ģʽ�� */
	if (CMX7042_GetSlotCntCalibFlgRst() == TRUE)
	{
		CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_EnterConfigMode);
		data[0] = GetSlotNum();
		CMX7042_ExecCfgTask(CMX7042_CTASK_SetSlotCount, data, 1);
		CMX7042_ExecCfgTaskNoPara(CMX7042_CTASK_EXIT_CONFIG);				

	    /*lq ������������ */
	    CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Rx1_RXB);
	    CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Rx2_RXB);
	}

	/*lq ���� */
	CMXCOMM_ReceiveProcess(&CMX7042CommInstance); 

    /*lq ���� */
    CMXCOMM_TransmitProcess(&CMX7042CommInstance);

    if (CMX7042TestMsgFlg == TRUE)
    {
        CMX7042TestMsgFlg = FALSE;
        CMXCOMM_TxTest();
    }

    if (CMX7042SovTxFlg == TRUE)
    {
        CMX7042SovTxFlg = FALSE;
        CMXCOMM_SimpleOperVeriTx();
    }
}

/*******************************************************************************
* ����  : CMXCOMM_ReceiveProcess
* ����  : CMX7042��Ϣ����
* ����  : pCMX7042CommStruct - CMX7042CommStructָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMXCOMM_ReceiveProcess(CMX7042CommStruct* pCMX7042CommStruct)
{
    /*lq ���ջ�1 */
    if (CMX7042_IsRx1Ready() == TRUE)
    {
        /*lq �������� */ 
        if (CMXCOMM_GetRxData(pCMX7042CommStruct, CMX7042_RX_1) == TRUE)
        {
            //lq usart_printf("$CMX7042 RX1 has received!\r\n");                                
        }

        /*lq ������һ�ν��� */
        CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Rx1_RXB);
    }

    /*lq ���ջ�2 */    
    if (CMX7042_IsRx2Ready() == TRUE)
    {
        /*lq �������� */ 
        if (CMXCOMM_GetRxData(pCMX7042CommStruct, CMX7042_RX_2) == TRUE)
        {
            //lq usart_printf("$CMX7042 RX2 has received!\r\n");                                        
        }

        /*lq ������һ�ν��� */
        CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Rx2_RXB);
    }

    if (CMX7042_IsRxStateChange() == TRUE)
    {
        /*lq ���ջ�1���� */
        if (CMX7042_GetRxErrorRst(CMX7042_RX_1) == TRUE)
        {
            /*lq ������һ�ν��� */
            CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Rx1_RXB);
    
            if (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_TwoLong)
            {
                usart_printf("$CMX7042 RX1 ERR: TwoLong!\r\n");                                
            }
            else if (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_CRC)
            {
                usart_printf("$CMX7042 RX1 ERR: CRC!\r\n");                                        
            }
            else if (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_NewFrameTwoBuffFull)
            {
                usart_printf("$CMX7042 RX1 ERR: NewFrameTwoBuffFull!\r\n");                                        
            } 
            else if (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_EndFlgNotByteBound)
            {
                usart_printf("$CMX7042 RX1 ERR: EndFlgNotByteBound!\r\n");                                        
            }                        
        }
    
        /*lq ���ջ�2���� */
        if (CMX7042_GetRxErrorRst(CMX7042_RX_2) == TRUE)
        {
            /*lq ������һ�ν��� */
            CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Rx2_RXB); 
            
            if (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_TwoLong)
            {
                usart_printf("$CMX7042 RX2 ERR: TwoLong!\r\n");                                
            }
            else if (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_CRC)
            {
                usart_printf("$CMX7042 RX2 ERR: CRC!\r\n");                                        
            }
            else if (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_NewFrameTwoBuffFull)
            {
                usart_printf("$CMX7042 RX2 ERR: NewFrameTwoBuffFull!\r\n");                                        
            } 
            else if (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_EndFlgNotByteBound)
            {
                usart_printf("$CMX7042 RX2 ERR: EndFlgNotByteBound!\r\n");                                        
            }                
        }    
    }    
}

/*******************************************************************************
* ����  : CMXCOMM_TransmitProcess
* ����  : CMX7042��Ϣ����
* ����  : pCMX7042CommStruct - CMX7042CommStructָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMXCOMM_TransmitProcess(CMX7042CommStruct* const pCMX7042CommStruct)
{
    CMXTraMsgCellStruct* pTraMsgCell;
    CMXTxCtrlCellStruct* pTxCtrlCell;
    S32 currSlotSum;
    S16 SlotDiff1;
    S16 SlotDiff2;
    U16 i;
    U8  txStatus;

    pTxCtrlCell = &(pCMX7042CommStruct->TxCtrlCell);
    currSlotSum = GetSlotNum();

    switch (pTxCtrlCell->State)
    {
        case CMXCOMM_TX_CTRL_STATE_IDLE:

            /*lq ������Ϣ���зǿ� */
            if (CMXCOMM_IsTxQueEmpty(pCMX7042CommStruct) == FALSE)
            {
                /*lq ���� */
                CMXCOMM_TxDeque(pCMX7042CommStruct, pTraMsgCell);
    
                /*lq ������Ƶ�Ԫ��ֵ */
                for (i = 0; i < pTraMsgCell->DataLen; i++)
                {
                    pTxCtrlCell->Data[i] = pTraMsgCell->Data[i]; 
                }
                pTxCtrlCell->DataLen = pTraMsgCell->DataLen;                     
                pTxCtrlCell->TxSlot = pTraMsgCell->TxSlot;                     
                pTxCtrlCell->TxChannel = pTraMsgCell->TxChannel;                     
                pTxCtrlCell->State = CMXCOMM_TX_CTRL_STATE_TX_WAITING; 
                usart_printf("$CMX7042 TX State: TX_WAITING!\r\n");                                                                   
            } 
                    
            break;    

        case CMXCOMM_TX_CTRL_STATE_TX_WAITING:

            SlotDiff1 = pTxCtrlCell->TxSlot - currSlotSum;
            SlotDiff2 = pTxCtrlCell->TxSlot + 2250 - currSlotSum;

            /*lq Ԥ��ʱ϶��Ч */
            if (!((SlotDiff1 > TIME_OFFSET_SEND_TO_TX)
                || (SlotDiff1 <= 0 && SlotDiff2 > TIME_OFFSET_SEND_TO_TX)))
            {
                /*lq ����ʱ϶�� */
                if ((currSlotSum + 1) % 2250 == pTxCtrlCell->TxSlot)    //lq CMX7042���յ����������ʱ϶����һʱ϶��ʼ����
                {
                    /*lq CMX7042 Tx buffer ���� */       
                    if (CMX7042_IsTxBufferFree() == TRUE)
                    {
                        pTxCtrlCell->State = CMXCOMM_TX_CTRL_STATE_TX_EXEC;
                        usart_printf("$CMX7042 TX State: TX_EXEC!\r\n");                                                                                                                                       
                    }                                      
                }                
            }
            else
            {
                pTxCtrlCell->State = CMXCOMM_TX_CTRL_STATE_IDLE;
                usart_printf("$CMX7042 TX State: IDLE!\r\n");             
            } 
                       
            break;

        case CMXCOMM_TX_CTRL_STATE_TX_EXEC:

            /*lq �շ�ת�� */
            if (pTxCtrlCell->TxChannel == AIS_CHANNEL_A)
            {
                RxTxCtrl(RXTX_CTRL_TYPE_TX, 0, AIS_FREQ_A);            
            }
            else
            {
                RxTxCtrl(RXTX_CTRL_TYPE_TX, 0, AIS_FREQ_B);                        
            }

            /*lq ������Ϣ���� */
            CMX7042_DTaskDataBulkWrite(pTxCtrlCell->Data, pTxCtrlCell->DataLen * 8);

    	    /*lq ������������ */
    	    CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Tx_TXB);

    		/*lq ���÷��䳬ʱ��ʱ */
            pTxCtrlCell->TimeCnt = SetTimePeriodProcess(pTxCtrlCell->TimePeriod);
            pTxCtrlCell->State = CMXCOMM_TX_CTRL_STATE_DONE_WAITING;
            usart_printf("$CMX7042 TX State: DONE_WAITING!\r\n");                                                                                                                                                                                               

            break;

        case CMXCOMM_TX_CTRL_STATE_DONE_WAITING:

            if (CMX7042_IsTxDone() == TRUE)
            {            
                /*lq ����״̬�ж� */
                txStatus = CMX7042_GetTxState();
                if (txStatus == CMX7042_TX_STATE_TxAborted_CSTDMA
                    | txStatus == CMX7042_TX_STATE_TxAborted_BuffNotReady
                    | txStatus == CMX7042_TX_STATE_TxAborted_MsgTooLong)
                {
            	    /*lq ������ֹ�������� */
            	    CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Tx_Abort);                
                }

                /*lq �շ�ת�� */
                RxTxCtrl(RXTX_CTRL_TYPE_RX, 0, AIS_FREQ_B);                                        

                pTxCtrlCell->State = CMXCOMM_TX_CTRL_STATE_IDLE;
                usart_printf("$CMX7042 TX State: IDLE!\r\n");                                                                                                                                                                                               
            }
            else
            {
                /*lq ���䳬ʱ */
                if (pTxCtrlCell->TimeCnt == GetLocalClockTickProcess())
            	{
            	    /*lq ������ֹ�������� */
            	    CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Tx_Abort); 

                    /*lq �շ�ת�� */
                    RxTxCtrl(RXTX_CTRL_TYPE_RX, 0, AIS_FREQ_B);                                        
    
                    pTxCtrlCell->State = CMXCOMM_TX_CTRL_STATE_IDLE;
                    usart_printf("$CMX7042 TX timeout!\r\n");                                            
                    usart_printf("$CMX7042 TX State: IDLE!\r\n");                                            
            	}
            }

            break;

        default :
            
            /*lq Ĭ�Ͻ��� */
            RxTxCtrl(RXTX_CTRL_TYPE_RX, 0, AIS_FREQ_B);
            pTxCtrlCell->State = CMXCOMM_TX_CTRL_STATE_IDLE;                                                
            usart_printf("$CMX7042 TX State: IDLE from default case!\r\n");                                                                                                                                                                                               
            
            break;
    }
}

/*******************************************************************************
* ����  : CMXCOMM_GetRxData
* ����  : ��ȡCMX7042������Ϣ
* ����  : pCMX7042CommStruct - CMX7042CommStructָ��
          rxChannel - ���ջ�	  
* ���  : ��
* ����  : TRUE - ��ȡ��Ϣ�ɹ��� FALSE - ��ȡ��Ϣʧ��
*******************************************************************************/
U8 CMXCOMM_GetRxData(CMX7042CommStruct* pCMX7042CommStruct, U8 rxChannel)
{
    CMXRecMsgCellStruct msgCell;
    U16 data[4];
    U16 data2[88];                  //lq CMX7042����data bufferΪ88words
    U16 i;
    U16 temp;

    /*lq ����ʼ�ĸ��� */
    CMX7042_DTaskDataRead(data, 64, TRUE, rxChannel);
    msgCell.BurstLen = data[0];
    msgCell.DCOffset = data[1];
    msgCell.ArrivalTime = data[2];
    msgCell.ArrivalSLot = data[3];
    msgCell.RxChannel = rxChannel;

    if (data[0] == 0)
    {
        /*lq �����ֽ���Ϊ0 */
        return FALSE;    
    }

    temp = data[0] * 8;             //lq �ܵı���������24bit SyncSeq + 8bit FrameHead + xbit data + 16bit FCS + 8bit FrameTail��

    /*lq ������ */    
    CMX7042_DTaskDataBulkRead(data2, temp, FALSE, rxChannel);

    temp = (data[0] - 1) / 2;           //lq ��ȥFrameTail
    for (i = 2; i < temp; i++)          //lq ǰ������Ϊͬ�����к�֡ͷ
    {
        msgCell.Data[(i - 2) * 2] = data2[i] & 0xFF;            //lq CMX7042word�Ĵ����д���ֽڵ�˳��ΪС��ģʽ����ǰ���ֽ����ݴ���ڵ��ֽڣ�
        msgCell.Data[(i - 2) * 2 + 1] = (data2[i] >> 8) & 0xFF;
    }
    if ((data[0] - 1) % 2 != 0)//lqif ((data[0] - 3) % 2 != 0)
    {
        msgCell.Data[(i - 2) * 2] = data2[i] & 0xFF;
    }

    msgCell.BurstLen = data[0] - 7;     //lq �������ݵĳ���

    temp = (msgCell.Data[data[0] - 5 - 2] << 8) | msgCell.Data[data[0] - 5 - 1];    //lq ���յ���У����
    
    if (temp == AIS_Cal_CRC16(&msgCell.Data[0], msgCell.BurstLen))
    {
        CMXCOMM_RxEnque(pCMX7042CommStruct, &msgCell);
        
        return TRUE;
    }
    else
    {
        return FALSE;
    }            
}

/*******************************************************************************
* ����  : CMXCOMM_IsRxQueFull
* ����  : ������Ϣ�����Ƿ�����
* ����  : pCMX7042CommStruct - CMX7042CommStructָ��
* ���  : ��
* ����  : True - ������FALSE - δ��
*******************************************************************************/
U8 CMXCOMM_IsRxQueFull(CMX7042CommStruct* pCMX7042CommStruct)
{
    CMXRecMsgFIFOStruct* pFIFO;

    pFIFO = &(pCMX7042CommStruct->RxFIFO);
    if (((pFIFO->FIFOInput + 1) % BUFF_FIFO_DEPTH) == pFIFO->FIFOOutput)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : CMXCOMM_IsTxQueFull
* ����  : ������Ϣ�����Ƿ�����
* ����  : pCMX7042CommStruct - CMX7042CommStructָ��
* ���  : ��
* ����  : True - ������FALSE - δ��
*******************************************************************************/
U8 CMXCOMM_IsTxQueFull(CMX7042CommStruct* const pCMX7042CommStruct)
{
    CMXTraMsgFIFOStruct* pFIFO;

    pFIFO = &(pCMX7042CommStruct->TxFIFO);
    if (((pFIFO->FIFOInput + 1) % BUFF_FIFO_DEPTH) == pFIFO->FIFOOutput)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : CMXCOMM_IsRxQueEmpty
* ����  : ������Ϣ�����Ƿ�Ϊ��
* ����  : pCMX7042CommStruct - CMX7042CommStructָ��
* ���  : ��
* ����  : True - Ϊ�գ�FALSE - �ǿ�
*******************************************************************************/
U8 CMXCOMM_IsRxQueEmpty(CMX7042CommStruct* pCMX7042CommStruct)
{
    CMXRecMsgFIFOStruct* pFIFO;

    pFIFO = &(pCMX7042CommStruct->RxFIFO);
    if (pFIFO->FIFOInput == pFIFO->FIFOOutput)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : CMXCOMM_IsTxQueEmpty
* ����  : ������Ϣ�����Ƿ�Ϊ��
* ����  : pCMX7042CommStruct - CMX7042CommStructָ��
* ���  : ��
* ����  : True - Ϊ�գ�FALSE - �ǿ�
*******************************************************************************/
U8 CMXCOMM_IsTxQueEmpty(CMX7042CommStruct* const pCMX7042CommStruct)
{
    CMXTraMsgFIFOStruct* pFIFO;

    pFIFO = &(pCMX7042CommStruct->TxFIFO);
    if (pFIFO->FIFOInput == pFIFO->FIFOOutput)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : CMXCOMM_RxEnque
* ����  : ������Ϣ���
* ����  : pCMX7042CommStruct - CMX7042CommStructָ��
          pCMXRecMsgCellStruct - ��Ϣ��Ԫָ��	  
* ���  : ��
* ����  : True - ��ӳɹ���FALSE - ���ʧ��
*******************************************************************************/
U8 CMXCOMM_RxEnque(CMX7042CommStruct* pCMX7042CommStruct, CMXRecMsgCellStruct* pCMXRecMsgCellStruct)
{
    CMXRecMsgFIFOStruct* pFIFO;
    U16 i;

    pFIFO = &(pCMX7042CommStruct->RxFIFO);
           
    if (CMXCOMM_IsRxQueFull(pCMX7042CommStruct) == FALSE)    //lq ���в���
    {
    	pFIFO->FIFO[pFIFO->FIFOInput].BurstLen = pCMXRecMsgCellStruct->BurstLen;
    	pFIFO->FIFO[pFIFO->FIFOInput].DCOffset = pCMXRecMsgCellStruct->DCOffset;
    	pFIFO->FIFO[pFIFO->FIFOInput].ArrivalTime = pCMXRecMsgCellStruct->ArrivalTime;
        pFIFO->FIFO[pFIFO->FIFOInput].ArrivalSLot = pCMXRecMsgCellStruct->ArrivalSLot;
        pFIFO->FIFO[pFIFO->FIFOInput].RxChannel = pCMXRecMsgCellStruct->RxChannel;                
        for (i = 0; i < pCMXRecMsgCellStruct->BurstLen; i++)
        {
            pFIFO->FIFO[pFIFO->FIFOInput].Data[i] = pCMXRecMsgCellStruct->Data[i];        
        }

        pFIFO->FIFOInput = (pFIFO->FIFOInput + 1) % BUFF_FIFO_DEPTH;

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : CMXCOMM_TxEnque
* ����  : ������Ϣ���
* ����  : pCMX7042CommStruct - CMX7042CommStructָ��
          pCMXTraMsgCellStruct - ������Ϣ��Ԫָ��	  
* ���  : ��
* ����  : True - ��ӳɹ���FALSE - ���ʧ��
*******************************************************************************/
U8 CMXCOMM_TxEnque(CMX7042CommStruct* pCMX7042CommStruct, CMXTraMsgCellStruct* pCMXTraMsgCellStruct)
{
    CMXTraMsgFIFOStruct* pFIFO;
    U16 i;

    pFIFO = &(pCMX7042CommStruct->TxFIFO);
           
    if (CMXCOMM_IsTxQueFull(pCMX7042CommStruct) == FALSE)    //lq ���в���
    {
    	pFIFO->FIFO[pFIFO->FIFOInput].DataLen = pCMXTraMsgCellStruct->DataLen;
    	pFIFO->FIFO[pFIFO->FIFOInput].TxSlot = pCMXTraMsgCellStruct->TxSlot;
    	pFIFO->FIFO[pFIFO->FIFOInput].TxChannel = pCMXTraMsgCellStruct->TxChannel;                       
        for (i = 0; i < pCMXTraMsgCellStruct->DataLen; i++)
        {
            pFIFO->FIFO[pFIFO->FIFOInput].Data[i] = pCMXTraMsgCellStruct->Data[i];        
        }

        pFIFO->FIFOInput = (pFIFO->FIFOInput + 1) % BUFF_FIFO_DEPTH;

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : CMXCOMM_RxDeque
* ����  : ������Ϣ���
* ����  : pCMX7042CommStruct - CMX7042CommStructָ��
          pCMXRecMsgCellStruct - ��Ϣ��Ԫָ��	  
* ���  : ��
* ����  : True - ���ӳɹ���FALSE - ����ʧ��
*******************************************************************************/
U8 CMXCOMM_RxDeque(CMX7042CommStruct* pCMX7042CommStruct, CMXRecMsgCellStruct* pCMXRecMsgCellStruct)
{
    CMXRecMsgFIFOStruct* pFIFO;
    U16 i;

    pFIFO = &(pCMX7042CommStruct->RxFIFO);
           
    if (CMXCOMM_IsRxQueEmpty(pCMX7042CommStruct) == FALSE)    //lq ���зǿ�
    {
    	pCMXRecMsgCellStruct->BurstLen = pFIFO->FIFO[pFIFO->FIFOOutput].BurstLen;
    	pCMXRecMsgCellStruct->DCOffset = pFIFO->FIFO[pFIFO->FIFOOutput].DCOffset;
    	pCMXRecMsgCellStruct->ArrivalTime = pFIFO->FIFO[pFIFO->FIFOOutput].ArrivalTime;
        pCMXRecMsgCellStruct->ArrivalSLot = pFIFO->FIFO[pFIFO->FIFOOutput].ArrivalSLot;
        pCMXRecMsgCellStruct->RxChannel = pFIFO->FIFO[pFIFO->FIFOOutput].RxChannel;
        for (i = 0; i < pFIFO->FIFO[pFIFO->FIFOOutput].BurstLen; i++)
        {
            pCMXRecMsgCellStruct->Data[i] = pFIFO->FIFO[pFIFO->FIFOOutput].Data[i]; 
        }

        pFIFO->FIFOOutput = (pFIFO->FIFOOutput + 1) % BUFF_FIFO_DEPTH;

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : CMXCOMM_TxDeque
* ����  : ������Ϣ����
* ����  : pCMX7042CommStruct - CMX7042CommStructָ��
          pCMXTraMsgCellStruct - ������Ϣ��Ԫָ��	  
* ���  : ��
* ����  : True - ���ӳɹ���FALSE - ����ʧ��
*******************************************************************************/
U8 CMXCOMM_TxDeque(CMX7042CommStruct* pCMX7042CommStruct, CMXTraMsgCellStruct* pCMXTraMsgCellStruct)
{
    CMXTraMsgFIFOStruct* pFIFO;
    U16 i;

    pFIFO = &(pCMX7042CommStruct->TxFIFO);
           
    if (CMXCOMM_IsTxQueEmpty(pCMX7042CommStruct) == FALSE)    //lq ���зǿ�
    {
    	pCMXTraMsgCellStruct->DataLen = pFIFO->FIFO[pFIFO->FIFOOutput].DataLen;
    	pCMXTraMsgCellStruct->TxSlot = pFIFO->FIFO[pFIFO->FIFOOutput].TxSlot;
    	pCMXTraMsgCellStruct->TxChannel = pFIFO->FIFO[pFIFO->FIFOOutput].TxChannel;
        for (i = 0; i < pFIFO->FIFO[pFIFO->FIFOOutput].DataLen; i++)
        {
            pCMXTraMsgCellStruct->Data[i] = pFIFO->FIFO[pFIFO->FIFOOutput].Data[i]; 
        }

        pFIFO->FIFOOutput = (pFIFO->FIFOOutput + 1) % BUFF_FIFO_DEPTH;

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : CMXCOMM_TxTest
* ����  : ������Ϣ����
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMXCOMM_TxTest(void)
{
    static U8 channel = AIS_CHANNEL_A;
    CMXTraMsgCellStruct traMsgCell;
    U16 i;
#if 1
    U8  data[53] = { 0x14,0x38
                    ,0x26,0x34
                    ,0xE0,0x02
                    ,0x33,0x9A
                    ,0x59,0xA2
                    ,0x51,0x53
                    ,0x30,0x00
                    ,0x14,0x41
                    ,0x19,0x4D
                    ,0x43,0xCE
                    ,0x14,0x00
                    ,0x00,0x00
                    ,0x00,0x00
                    ,0x00,0x00
                    ,0x00,0x4F
                    ,0x09,0xAB
                    ,0x4D,0x34
                    ,0x59,0xC6
                    ,0x00,0xE4
                    ,0x13,0x94
                    ,0x5C,0x54
                    ,0x90,0x00
                    ,0x00,0x00
                    ,0x00,0x00
                    ,0x00,0x00
                    ,0x00,0x00
                    ,0x00 };
#else
    U8  data[53] = { 0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55 };
#endif
    traMsgCell.DataLen = 53;
    for (i = 0; i < traMsgCell.DataLen; i++)
    {
        traMsgCell.Data[i] = data[i];
    }

    traMsgCell.TxSlot = (GetSlotNum() + TIME_OFFSET_SEND_TO_TX) % 2250;
    traMsgCell.TxChannel = channel;
#if 1
    channel = (channel == AIS_CHANNEL_A) ? AIS_CHANNEL_B : AIS_CHANNEL_A;
#else    
    channel = AIS_CHANNEL_A;
#endif
    CMXCOMM_TxEnque(&CMX7042CommInstance, &traMsgCell);
}

/*******************************************************************************
* ����  : CMXCOMM_SimpleOperVeriTx
* ����  : ����������
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMXCOMM_SimpleOperVeriTx(void)
{
    CMX7042_ExecModemTaskNoPara(0x002C);
    usart_printf("$CMX7042 : SimpleOperVeriTx!\r\n");                                                
}

/*******************************************************************************
* ����  : CMXCOMM_SetTxTestFlg
* ����  : ʹ�ܷ�����Ϣ����
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMXCOMM_SetTxTestFlg(void)
{
    CMX7042TestMsgFlg = TRUE;    
}

/*******************************************************************************
* ����  : CMXCOMM_SetSovTxFlg
* ����  : ʹ������������
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMXCOMM_SetSovTxFlg(void)
{
    CMX7042SovTxFlg = TRUE;    
}
