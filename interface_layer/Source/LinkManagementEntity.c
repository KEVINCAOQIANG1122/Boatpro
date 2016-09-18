/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: LinkManagementEntity.c
  Author: lq       Version : V1.00        Date: 2014.07.11
  Description:     AIS��·����ʵ���
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "Include.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static BinMsgRxFifoStruct BinMsgFifoRx1;    
static BinMsgRxFifoStruct BinMsgFifoRx2;    //lq ��������·��ʹ��������Ϣ���ն��У������ͻ
static BinMsgTxFifoStruct BinMsgFifoTx;     //lq ����ֻ��һ·�������ͻ
 
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* ����  : LME_BinMsgRxFifoStructInit
* ����  : ��ʼ���ṹ��
* ����  : pBinMsgRxFifoStruct - BinMsgRxFifoStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void LME_BinMsgRxFifoStructInit(BinMsgRxFifoStruct *pBinMsgRxFifoStruct)
{
    pBinMsgRxFifoStruct->Input = 0;
    pBinMsgRxFifoStruct->Output = 0;    
}

/*******************************************************************************
* ����  : LME_BinMsgTxFifoStructInit
* ����  : ��ʼ���ṹ��
* ����  : pBinMsgTxFifoStruct - BinMsgTxFifoStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void LME_BinMsgTxFifoStructInit(BinMsgTxFifoStruct *pBinMsgTxFifoStruct)
{
    pBinMsgTxFifoStruct->Input = 0;
    pBinMsgTxFifoStruct->Output = 0;    
}

/*******************************************************************************
* ����  : LME_InstBinMsgFifoStructInit
* ����  : ��ʼ���ṹ��ʵ��
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void LME_InstBinMsgFifoStructInit(void)
{
    LME_BinMsgRxFifoStructInit(&BinMsgFifoRx1);
    LME_BinMsgRxFifoStructInit(&BinMsgFifoRx2);
    LME_BinMsgTxFifoStructInit(&BinMsgFifoTx);
}

/*******************************************************************************
* ����  : DLS_ModuleInit
* ����  : ģ���ʼ��
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void LME_ModuleInit(void)
{
    LME_InstBinMsgFifoStructInit();
}

/*******************************************************************************
* ����  : LME_BinMsgRxFifoIsEmpty
* ����  : �����Ƿ�Ϊ��
* ����  : pBinMsgRxFifoStruct - BinMsgRxFifoStruct�ṹ��ָ��		  
* ���  : ��
* ����  : TRUE - Ϊ�գ�FALSE - �ǿ�
*******************************************************************************/
static U8 LME_BinMsgRxFifoIsEmpty(BinMsgRxFifoStruct *pBinMsgRxFifoStruct)
{
    if (pBinMsgRxFifoStruct->Input == pBinMsgRxFifoStruct->Output)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* ����  : LME_BinMsgTxFifoIsEmpty
* ����  : �����Ƿ�Ϊ��
* ����  : pBinMsgTxFifoStruct - BinMsgTxFifoStruct�ṹ��ָ��		  
* ���  : ��
* ����  : TRUE - Ϊ�գ�FALSE - �ǿ�
*******************************************************************************/
static U8 LME_BinMsgTxFifoIsEmpty(BinMsgTxFifoStruct *pBinMsgTxFifoStruct)
{
    if (pBinMsgTxFifoStruct->Input == pBinMsgTxFifoStruct->Output)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* ����  : LME_BinMsgRxFifoIsFull
* ����  : �����Ƿ�����
* ����  : pBinMsgRxFifoStruct - BinMsgRxFifoStruct�ṹ��ָ��		  
* ���  : ��
* ����  : TRUE - ������FALSE - δ��
*******************************************************************************/
static U8 LME_BinMsgRxFifoIsFull(BinMsgRxFifoStruct *pBinMsgRxFifoStruct)
{
    if ((pBinMsgRxFifoStruct->Input + 1) % LME_BINMSG_FIFO_DEPTH
        == pBinMsgRxFifoStruct->Output)
    {
        return TRUE;
        
        debug_printf("$[%6d:%4d] %s : BinMsg rx fifo [%s]is full!\r\n",
            GetFrameNum(), GetSlotNum(), __func__,
            pBinMsgRxFifoStruct == &BinMsgFifoRx1 ? "A" : "B");
    }
    else
    {
        return FALSE;
    }     
}

/*******************************************************************************
* ����  : LME_BinMsgTxFifoIsFull
* ����  : �����Ƿ�����
* ����  : pBinMsgTxFifoStruct - BinMsgTxFifoStruct�ṹ��ָ��		  
* ���  : ��
* ����  : TRUE - ������FALSE - δ��
*******************************************************************************/
static U8 LME_BinMsgTxFifoIsFull(BinMsgTxFifoStruct *pBinMsgTxFifoStruct)
{
    if ((pBinMsgTxFifoStruct->Input + 1) % LME_BINMSG_FIFO_DEPTH
        == pBinMsgTxFifoStruct->Output)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }     
}

/*******************************************************************************
* ����  : LME_BinMsgRxFifoEnque
* ����  : �������
* ����  : pBinMsgRxFifoStruct - BinMsgRxFifoStruct�ṹ��ָ��
          pBinMsgRxCellStruct - ָ��������ݵ�Ԫ��ָ��         		  
* ���  : ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
static U8 LME_BinMsgRxFifoEnque(BinMsgRxFifoStruct *pBinMsgRxFifoStruct, const BinMsgRxCellStruct *pBinMsgRxCellStruct)
{
    BinMsgRxCellStruct *pCell;
    U16 i;
       
    if (LME_BinMsgRxFifoIsFull(pBinMsgRxFifoStruct) == FALSE)
    {      
        pCell = &(pBinMsgRxFifoStruct->FIFO[pBinMsgRxFifoStruct->Input]);
        
        pCell->DataLen = pBinMsgRxCellStruct->DataLen;
        for (i = 0; i < pBinMsgRxCellStruct->DataLen; i++)        
        {
            pCell->Data[i] = pBinMsgRxCellStruct->Data[i];         
        }
        pCell->RxChannel = pBinMsgRxCellStruct->RxChannel;
        pCell->ArrivalFrame = pBinMsgRxCellStruct->ArrivalFrame;
        pCell->ArrivalSlot = pBinMsgRxCellStruct->ArrivalSlot;
                
        pBinMsgRxFifoStruct->Input = (pBinMsgRxFifoStruct->Input + 1) % LME_BINMSG_FIFO_DEPTH;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }   
}

/*******************************************************************************
* ����  : LME_BinMsgTxFifoEnque
* ����  : �������
* ����  : pBinMsgTxFifoStruct - BinMsgTxFifoStruct�ṹ��ָ��
          pBinMsgTxCellStruct - ָ��������ݵ�Ԫ��ָ��        		  
* ���  : ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
static U8 LME_BinMsgTxFifoEnque(BinMsgTxFifoStruct *pBinMsgTxFifoStruct,
                                const BinMsgTxCellStruct *pBinMsgTxCellStruct)
{
    BinMsgTxCellStruct *pCell;
    U16 i;
       
    if (LME_BinMsgTxFifoIsFull(pBinMsgTxFifoStruct) == FALSE)
    {      
        pCell = &(pBinMsgTxFifoStruct->FIFO[pBinMsgTxFifoStruct->Input]);
        
        pCell->DataLen = pBinMsgTxCellStruct->DataLen;
        for (i = 0; i < pBinMsgTxCellStruct->DataLen; i++)        
        {
            pCell->Data[i] = pBinMsgTxCellStruct->Data[i];         
        }
        pCell->TxChannel = pBinMsgTxCellStruct->TxChannel;
        pCell->TxSlot = pBinMsgTxCellStruct->TxSlot;
                
        pBinMsgTxFifoStruct->Input = (pBinMsgTxFifoStruct->Input + 1) % LME_BINMSG_FIFO_DEPTH;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }   
}

/*******************************************************************************
* ����  : LME_Inst1BinMsgRxFifoEnque
* ����  : �������
* ����  : pBinMsgRxCellStruct - ָ��������ݵ�Ԫ��ָ��         		  
* ���  : ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
U8 LME_Inst1BinMsgRxFifoEnque(const BinMsgRxCellStruct *pBinMsgRxCellStruct)
{
    return LME_BinMsgRxFifoEnque(&BinMsgFifoRx1, pBinMsgRxCellStruct);
}

/*******************************************************************************
* ����  : LME_Inst2BinMsgRxFifoEnque
* ����  : �������
* ����  : pBinMsgRxCellStruct - ָ��������ݵ�Ԫ��ָ��         		  
* ���  : ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
U8 LME_Inst2BinMsgRxFifoEnque(const BinMsgRxCellStruct *pBinMsgRxCellStruct)
{
    return LME_BinMsgRxFifoEnque(&BinMsgFifoRx2, pBinMsgRxCellStruct);
}

/*******************************************************************************
* ����  : LME_InstBinMsgTxFifoEnque
* ����  : �������
* ����  : pBinMsgTxCellStruct - ָ��������ݵ�Ԫ��ָ��         		  
* ���  : ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
U8 LME_InstBinMsgTxFifoEnque(const BinMsgTxCellStruct *pBinMsgTxCellStruct)
{
    return LME_BinMsgTxFifoEnque(&BinMsgFifoTx, pBinMsgTxCellStruct);
}

/*******************************************************************************
* ����  : LME_BinMsgRxFifoDeque
* ����  : ���г���
* ����  : pBinMsgRxFifoStruct - BinMsgRxFifoStruct�ṹ��ָ��          
* ���  : pBinMsgRxCellStruct - ָ��������ݵ�Ԫ��ָ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
static U8 LME_BinMsgRxFifoDeque(BinMsgRxFifoStruct *pBinMsgRxFifoStruct,
                                BinMsgRxCellStruct *pBinMsgRxCellStruct)
{
    U16 i;
    BinMsgRxCellStruct *pCell;

    if (LME_BinMsgRxFifoIsEmpty(pBinMsgRxFifoStruct) == FALSE)
    {        
        pCell = &(pBinMsgRxFifoStruct->FIFO[pBinMsgRxFifoStruct->Output]);
        
        pBinMsgRxCellStruct->DataLen = pCell->DataLen;
        for (i = 0; i < pCell->DataLen; i++)        
        {
            pBinMsgRxCellStruct->Data[i] = pCell->Data[i];         
        }
        pBinMsgRxCellStruct->RxChannel = pCell->RxChannel;
        pBinMsgRxCellStruct->ArrivalFrame = pCell->ArrivalFrame;
        pBinMsgRxCellStruct->ArrivalSlot = pCell->ArrivalSlot;

        pBinMsgRxFifoStruct->Output = (pBinMsgRxFifoStruct->Output + 1) % LME_BINMSG_FIFO_DEPTH;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* ����  : LME_BinMsgTxFifoDeque
* ����  : ���г���
* ����  : pBinMsgTxFifoStruct - BinMsgTxFifoStruct�ṹ��ָ��          
* ���  : pBinMsgTxCellStruct - ָ��������ݵ�Ԫ��ָ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
static U8 LME_BinMsgTxFifoDeque(BinMsgTxFifoStruct *pBinMsgTxFifoStruct,
                                BinMsgTxCellStruct *pBinMsgTxCellStruct)
{
    U16 i;
    BinMsgTxCellStruct *pCell;

    if (LME_BinMsgTxFifoIsEmpty(pBinMsgTxFifoStruct) == FALSE)
    {        
        pCell = &(pBinMsgTxFifoStruct->FIFO[pBinMsgTxFifoStruct->Output]);
        
        pBinMsgTxCellStruct->DataLen = pCell->DataLen;
        for (i = 0; i < pCell->DataLen; i++)        
        {
            pBinMsgTxCellStruct->Data[i] = pCell->Data[i];         
        }
        pBinMsgTxCellStruct->TxChannel = pCell->TxChannel;
        pBinMsgTxCellStruct->TxSlot = pCell->TxSlot;

        pBinMsgTxFifoStruct->Output = (pBinMsgTxFifoStruct->Output + 1) % LME_BINMSG_FIFO_DEPTH;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* ����  : LME_Inst1BinMsgRxFifoDeque
* ����  : ���г���
* ����  : ��
* ���  : pBinMsgRxCellStruct - ָ��������ݵ�Ԫ��ָ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
U8 LME_Inst1BinMsgRxFifoDeque(BinMsgRxCellStruct *pBinMsgRxCellStruct)
{
    return LME_BinMsgRxFifoDeque(&BinMsgFifoRx1, pBinMsgRxCellStruct);
}

/*******************************************************************************
* ����  : LME_Inst2BinMsgRxFifoDeque
* ����  : ���г���
* ����  : ��
* ���  : pBinMsgRxCellStruct - ָ��������ݵ�Ԫ��ָ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
U8 LME_Inst2BinMsgRxFifoDeque(BinMsgRxCellStruct *pBinMsgRxCellStruct)
{
    return LME_BinMsgRxFifoDeque(&BinMsgFifoRx2, pBinMsgRxCellStruct);
}

/*******************************************************************************
* ����  : LME_InstBinMsgTxFifoDeque
* ����  : ���г���
* ����  : ��
* ���  : pBinMsgTxCellStruct - ָ��������ݵ�Ԫ��ָ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
U8 LME_InstBinMsgTxFifoDeque(BinMsgTxCellStruct *pBinMsgTxCellStruct)
{
    return LME_BinMsgTxFifoDeque(&BinMsgFifoTx, pBinMsgTxCellStruct);
}

/*******************************************************************************
* ����  : LME_BinMsgRxFifoPortIn
* ����  : ����֡�����е�����ת�������Ϣ����
* ����  : pBinMsgRxFifoStruct - BinMsgRxFifoStruct�ṹ��ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void LME_BinMsgRxFifoPortIn(BinMsgRxFifoStruct *pBinMsgRxFifoStruct)
{
    FrameRxCellStruct *pFrameRxCell;
    BinMsgRxCellStruct *pBinMsgRxCell;        
    U16 temp16;
    U16 i;
    U8 state;
#if 0
    U8 temp[2 * DLS_FRAME_MAX_LEN_BYTE + 30];
    U8 temp2[3];
#endif
    pFrameRxCell = (FrameRxCellStruct *)malloc(sizeof(FrameRxCellStruct));
    memset((FrameRxCellStruct *)pFrameRxCell, 0, sizeof(FrameRxCellStruct));

    if (pBinMsgRxFifoStruct == &BinMsgFifoRx1)
    {
        state = DLS_Inst1FrameRxFifoDeque(pFrameRxCell);
    }
    else if (pBinMsgRxFifoStruct == &BinMsgFifoRx2)
    {
        state = DLS_Inst2FrameRxFifoDeque(pFrameRxCell);    
    }        
    else
    {
        state = FALSE;
    }

    if (state == TRUE)
    {
        if (pFrameRxCell->DataLen >= 3)   //lq ֡������Ϊ3��1byte of data and 2bytes of CRC
        {
            temp16 = ((pFrameRxCell->Data[pFrameRxCell->DataLen - 2] << 8) & 0xFF00)
                      | (pFrameRxCell->Data[pFrameRxCell->DataLen - 1] & 0x00FF);    //lq CRC�ĸ�λ��ǰ 
            
            if (temp16 == AIS_Cal_CRC16(&pFrameRxCell->Data[0], pFrameRxCell->DataLen - 2))
            {
                pBinMsgRxCell = (BinMsgRxCellStruct *)malloc(sizeof(BinMsgRxCellStruct));
                memset((BinMsgRxCellStruct *)pBinMsgRxCell, 0, sizeof(BinMsgRxCellStruct));         
                       
                pBinMsgRxCell->DataLen = pFrameRxCell->DataLen - 2;                  //lq ����CRC
                for (i = 0; i < pBinMsgRxCell->DataLen; i++)
                {
                    pBinMsgRxCell->Data[i] = pFrameRxCell->Data[i];
                }
                pBinMsgRxCell->RxChannel = pFrameRxCell->RxChannel;
                pBinMsgRxCell->ArrivalFrame = pFrameRxCell->ArrivalFrame;
                pBinMsgRxCell->ArrivalSlot = pFrameRxCell->ArrivalSlot;

                LME_BinMsgRxFifoEnque(pBinMsgRxFifoStruct, pBinMsgRxCell);                   
                
                free((BinMsgRxCellStruct *)pBinMsgRxCell);

                /*lq ���ղ�����Ӧ���� */
                if (RxTestInstance.RxTestEn == TRUE)
                {
                    if (pFrameRxCell->RxChannel == ENUM_AIS_CHANNEL_A)
                    {
                        UartResponseMsg("$P712,A,OK!"); 
                    }
                    else if (pFrameRxCell->RxChannel == ENUM_AIS_CHANNEL_B)
                    {
                        UartResponseMsg("$P712,B,OK!");                     
                    }
                }
            }
            else
            {
#if 0
                usart_printf("$FrameProcess: CRC error!\r\n");
#endif
            }
#if 0
            sprintf(temp, "$FrameProcess: ");            
            for (i = 0; i < pFrameRxCell->DataLen; i++)
            {
                sprintf(temp2, "%02X", pFrameRxCell->Data[i]);
                strcat(temp, temp2);
            }
            strcat(temp, "\r\n");
            usart_printf(temp);
#endif                                
        }               
    }
    
    free((FrameRxCellStruct *)pFrameRxCell);
}

/*******************************************************************************
* ����  : LME_InstBinMsgRxFifoPortIn
* ����  : ����֡�����е�����ת�������Ϣ����
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void LME_InstBinMsgRxFifoPortIn(void)
{
    LME_BinMsgRxFifoPortIn(&BinMsgFifoRx1);
    LME_BinMsgRxFifoPortIn(&BinMsgFifoRx2);
}

/*******************************************************************************
* ����  : LME_BinMsgRxFifoPortOut
* ����  : ����������Ϣ���ն����е���Ϣת����Ϣ�����б�
* ����  : pBinMsgRxFifoStruct - BinMsgRxFifoStruct�ṹ��ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void LME_BinMsgRxFifoPortOut(BinMsgRxFifoStruct *pBinMsgRxFifoStruct)
{
    BinMsgRxCellStruct *pBinMsgRxCell;        
    MsgCellStruct *pMsgCellStruct;
    U8  channel;

    pBinMsgRxCell = (BinMsgRxCellStruct *)malloc(sizeof(BinMsgRxCellStruct));
    memset((BinMsgRxCellStruct *)pBinMsgRxCell, 0, sizeof(BinMsgRxCellStruct));

    if (LME_BinMsgRxFifoDeque(pBinMsgRxFifoStruct, pBinMsgRxCell) == TRUE)
    {
        /*lq ���VDM��� */
        if (pBinMsgRxCell->RxChannel == ENUM_AIS_CHANNEL_A)
        {
            channel = AIS_CHANNEL_CHAR_A;
        }
        else
        {
            channel = AIS_CHANNEL_CHAR_B;
        }

        AISDI_BuildVDM(AIS_DI_TALKER_ID_DEFAULT, pBinMsgRxCell->Data,
            pBinMsgRxCell->DataLen, channel);                

        /*lq ����������Ϣ���ն����е���Ϣת����Ϣ�����б� */
        pMsgCellStruct = (MsgCellStruct *)malloc(sizeof(MsgCellStruct));
        memset((MsgCellStruct *)pMsgCellStruct, 0, sizeof(MsgCellStruct));

        pMsgCellStruct->Channel = pBinMsgRxCell->RxChannel;

        pMsgCellStruct->Frame = pBinMsgRxCell->ArrivalFrame;
        pMsgCellStruct->Slot = pBinMsgRxCell->ArrivalSlot;

        memcpy(&(pMsgCellStruct->Data[1]), &(pBinMsgRxCell->Data[0]),
            pBinMsgRxCell->DataLen);

        pMsgCellStruct->Data[0] = pBinMsgRxCell->DataLen;
        pMsgCellStruct->DataLength = pMsgCellStruct->Data[0] + 1;
        pMsgCellStruct->MsgPriority = ENUM_AIS_MSG_PRIORITY_DEFAULT;  //lq �յ�����Ϣ�����ȼ���ͬ��������˳����

        MSGMAN_RxMsgListInsert(pMsgCellStruct);
    
        free((MsgCellStruct *)pMsgCellStruct);
    }

    free((BinMsgRxCellStruct *)pBinMsgRxCell);
}

/*******************************************************************************
* ����  : LME_InstBinMsgRxFifoPortOut
* ����  : ����������Ϣ��������е���Ϣת����Ϣ�����б�
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void LME_InstBinMsgRxFifoPortOut(void)
{
    LME_BinMsgRxFifoPortOut(&BinMsgFifoRx1);
    LME_BinMsgRxFifoPortOut(&BinMsgFifoRx2);
}

/*******************************************************************************
* ����  : LME_BinMsgTxFifoPortOut
* ����  : ������Ϣ�����е����ݾ�����֡�����ת�뷢��֡����
* ����  : pBinMsgTxFifoStruct - BinMsgTxFifoStruct�ṹ��ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void LME_BinMsgTxFifoPortOut(BinMsgTxFifoStruct *pBinMsgTxFifoStruct)
{
    FrameTxCellStruct *pFrameTxCell;
    BinMsgTxCellStruct *pBinMsgTxCell;        
    U8 *pBuff;
    U8 *pBuff2;
    U16 temp16;
    U16 i;
    U8  channel;

    pBinMsgTxCell = (BinMsgTxCellStruct *)malloc(sizeof(BinMsgTxCellStruct));
    memset((BinMsgTxCellStruct *)pBinMsgTxCell, 0, sizeof(BinMsgTxCellStruct));
        
    if (LME_BinMsgTxFifoDeque(pBinMsgTxFifoStruct, pBinMsgTxCell) == TRUE)
    {
        if (pBinMsgTxCell->DataLen > 0)   //lq ֡������Ϊ3��1byte of data and 2bytes of CRC
        {
            /*lq �м仺������ */
            pBuff = (U8 *)malloc((pBinMsgTxCell->DataLen + 2) * sizeof(U8));
            memset((U8 *)pBuff, 0, (pBinMsgTxCell->DataLen + 2) * sizeof(U8));
            pBuff[0] = pBinMsgTxCell->DataLen;
            pBuff[1] = 0;
            memcpy(&pBuff[2], &pBinMsgTxCell->Data[0], pBinMsgTxCell->DataLen);                
            
            pFrameTxCell = (FrameTxCellStruct *)malloc(sizeof(FrameTxCellStruct));
            memset((FrameTxCellStruct *)pFrameTxCell, 0, sizeof(FrameTxCellStruct));
            DLS_FrameBuild(&pBuff[0], &pFrameTxCell->Data[0]);      //lq ��֡
            free((U8 *)pBuff);
            pFrameTxCell->TxSlot = pBinMsgTxCell->TxSlot;
            pFrameTxCell->TxChannel = pBinMsgTxCell->TxChannel;

            if (DLS_InstFrameTxFifoEnque(pFrameTxCell) == TRUE)
            {
                /*lq ���VDO��� */
                if (pBinMsgTxCell->TxChannel == ENUM_AIS_CHANNEL_A)
                {
                    channel = AIS_CHANNEL_CHAR_A;
                }
                else
                {
                    channel = AIS_CHANNEL_CHAR_B;
                }

                AISDI_BuildVDO(AIS_DI_TALKER_ID_DEFAULT, pBinMsgTxCell->Data,
                    pBinMsgTxCell->DataLen, channel);                

            }
            free((FrameTxCellStruct *)pFrameTxCell);
        }               
    }
    
    free((BinMsgTxCellStruct *)pBinMsgTxCell);
}

/*******************************************************************************
* ����  : LME_InstBinMsgTxFifoPortOut
* ����  : ������Ϣ�����е����ݾ�����֡�����ת�뷢��֡����
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void LME_InstBinMsgTxFifoPortOut(void)
{
    LME_BinMsgTxFifoPortOut(&BinMsgFifoTx);
}

/*******************************************************************************
* ����  : LME_BinMsgTxFifoPortIn
* ����  : ����Ϣ�����б��е���Ϣת���������Ϣ�������
* ����  : pBinMsgTxFifoStruct - BinMsgTxFifoStruct�ṹ��ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void LME_BinMsgTxFifoPortIn(BinMsgTxFifoStruct *pBinMsgTxFifoStruct)
{
    BinMsgTxCellStruct *pBinMsgTxCell;        
    MsgCellStruct *pMsgCellStruct;

    pMsgCellStruct = (MsgCellStruct *)malloc(sizeof(MsgCellStruct));
    memset((MsgCellStruct *)pMsgCellStruct, 0, sizeof(MsgCellStruct));

    if (MSGMAN_TxMsgListGetElem(pMsgCellStruct) == TRUE)
    {
        pBinMsgTxCell = (BinMsgTxCellStruct *)malloc(sizeof(BinMsgTxCellStruct));
        memset((BinMsgTxCellStruct *)pBinMsgTxCell, 0, sizeof(BinMsgTxCellStruct));

        pBinMsgTxCell->TxChannel = pMsgCellStruct->Channel;
        pBinMsgTxCell->TxSlot = pMsgCellStruct->Slot;

        memcpy(&pBinMsgTxCell->Data[0], &(pMsgCellStruct->Data[1]),
            pMsgCellStruct->Data[0]);

        pBinMsgTxCell->DataLen = pMsgCellStruct->Data[0];
        
        LME_BinMsgTxFifoEnque(pBinMsgTxFifoStruct, pBinMsgTxCell);

        free((BinMsgTxCellStruct *)pBinMsgTxCell);
    }

    free((MsgCellStruct *)pMsgCellStruct);
}

/*******************************************************************************
* ����  : LME_InstBinMsgTxFifoPortIn
* ����  : ������Ϣ�����е����ݾ�����֡�����ת�뷢��֡����
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void LME_InstBinMsgTxFifoPortIn(void)
{
    LME_BinMsgTxFifoPortIn(&BinMsgFifoTx);
}

/*******************************************************************************
* ����  : LME_BinMsgPort
* ����  : ʵ����Ϣ�����ڽ���/����֡���С�����/������Ϣ���кͽ���/������Ϣ�б�֮
          ���ת��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void LME_BinMsgPort(void)
{
    LME_InstBinMsgTxFifoPortIn();
    LME_InstBinMsgTxFifoPortOut();
    LME_InstBinMsgRxFifoPortIn();
    LME_InstBinMsgRxFifoPortOut();
}
