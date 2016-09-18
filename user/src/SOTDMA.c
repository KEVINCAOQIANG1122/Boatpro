/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: SOTDMA.c
  Author: lq       Version : V1.00        Date: 2013.04.24
  Description:     ׼SOTDMA��Ϣ���Ϳ���ģ��������������壬
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include "def.h"
#include "SOTDMA.h"
#include "AIS_MsgModu.h"
#include "Tim.h"
#include "AISConfig.h"
#include "initial.h"
#include "gpsctrl.h"
#include "gmskmod.h"

SOTDMAParaStruct struSOTDMAPara;
ICommStateStruct struICommState;

/*******************************************************************************
* Function Name  : InitSOTDMAPara
* Description    : ��ʼ��SOTDMA����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitSOTDMAPara(void)
{
    struSOTDMAPara.BurstTxTimSlot = 0;
    struSOTDMAPara.BurstTxTimFrame = 0;
    struSOTDMAPara.MsgTxTimSlot = 0;
    struSOTDMAPara.MsgTxTimFrame = 0;
    
    struSOTDMAPara.BurstSendTimSlot = 0;
    struSOTDMAPara.BurstSendTimFrame = 0;
    struSOTDMAPara.MsgSendTimSlot = 0;
    struSOTDMAPara.MsgSendTimFrame = 0;
    
    struSOTDMAPara.Timeout = 0;
    struSOTDMAPara.MsgCount = 0;
    struSOTDMAPara.BurstFlg = TRUE;    
    struSOTDMAPara.TxChannel = 'A'; 
    struSOTDMAPara.BurstFinsFlg = FALSE; 
    struSOTDMAPara.BurstStarFlg = FALSE;
    
    struSOTDMAPara.SlotOffset = ITDMA_COM_STA_SLOT_INCRE_DEF_VAL;                          
}

/*******************************************************************************
* Function Name  : InitCommStateITDMA
* Description    : ��ʼ��ͨ��״̬
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitCommStateITDMA(void)
{
    struICommState.ucSyncState = 3;                                            // ͬ��״̬��                                    
    struICommState.usSlotIncrement = ITDMA_COM_STA_SLOT_INCRE_DEF_VAL;         // ʱ϶����
    struICommState.ucSlotNumbers = 2;                                          // ʱ϶����3��ʱ϶����1371_4��
    struICommState.ucKeepFlag = 0;                                             // ���ֱ�ǣ�������                                                        // ���յ�̨վ��
}

/*******************************************************************************
* Function Name  : GetBurstSendTim
* Description    : ��ȡ���ڵ�һ����Ϣ���Ͷ�ʱ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
U32 GetBurstSendTim(void)
{
    return struSOTDMAPara.BurstSendTimFrame * 2250 + struSOTDMAPara.BurstSendTimSlot;                     
}

/*******************************************************************************
* Function Name  : GetMsgSendTim
* Description    : ��ȡ������Ϣ���Ͷ�ʱ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
U32 GetMsgSendTim(void)
{
    return struSOTDMAPara.MsgSendTimFrame * 2250 + struSOTDMAPara.MsgSendTimSlot;                     
}
#if 0
/*******************************************************************************
* Function Name  : GetBurstSendTim
* Description    : ��ȡ�鶨ʱ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
U16 GetRandSlotOffset(void)
{
    /*lq... �������ֵ[2025,2475]*/
#if 1
    return RandData(&send_seed, 2025, 2475);
#else
    return 2250;
#endif
}
#endif

/*******************************************************************************
* Function Name  : GetRandSlotOffset
* Description    : ��ȡʱ϶����ƫ��
* Input          : PeriodSlots - �������ڣ���λ:slot��
* Output         : None
* Return         : �������ƫ���ʱ϶����
*******************************************************************************/
U16 GetRandSlotOffset(S16 PeriodSlots)
{
    S16 randSlotOffset;
#if 0
    /*lq ���ʱ϶ƫ��ֵ[-225,225]*/
    randSlotOffset = 225 - RandData(&send_seed, 0, 450);    

    return PeriodSlots + randSlotOffset;                    //lq ʱ϶����ֵΪ��������+���ʱ϶ƫ��
#else
    return PeriodSlots;                                     //lq ������
#endif
}

/*******************************************************************************
* Function Name  : SetBurstTxTim
* Description    : �����鷢�䶨ʱ
* Input          : usValue - ʱ϶����
* Output         : None
* Return         : None
*******************************************************************************/
#if 0
void SetBurstTxTim(S16 usValue)
{
    if (usValue < 0)
    {
        usValue = 0;
    }

    struSOTDMAPara.BurstTxTimSlot += usValue;
    if (struSOTDMAPara.BurstTxTimSlot >= 2250)
    {
       struSOTDMAPara.BurstTxTimFrame += struSOTDMAPara.BurstTxTimSlot / 2250; 
       struSOTDMAPara.BurstTxTimSlot %= 2250;    
    } 
}
#else
void SetBurstTxTim(S32 usValue)
{
    S32 tempSlot;

    if (usValue < 0)
    {
        usValue = 0;
    }

    tempSlot = struSOTDMAPara.BurstTxTimSlot + usValue;
    if (tempSlot >= 2250)
    {
       struSOTDMAPara.BurstTxTimFrame += tempSlot / 2250; 
       struSOTDMAPara.BurstTxTimSlot = tempSlot % 2250;    
    }
    else
    {
       struSOTDMAPara.BurstTxTimSlot = tempSlot;
    } 
}
#endif
/*******************************************************************************
* Function Name  : SetMsgTxTim
* Description    : ������Ϣ���䶨ʱ
* Input          : usValue - ʱ϶����
* Output         : None
* Return         : None
*******************************************************************************/
void SetMsgTxTim(S16 usValue)
{
    if (usValue < 0)
    {
        usValue = 0;
    }

    if (GetBurstFlg() == TRUE)
    {
        struSOTDMAPara.MsgTxTimFrame = struSOTDMAPara.BurstTxTimFrame;
        struSOTDMAPara.MsgTxTimSlot = struSOTDMAPara.BurstTxTimSlot + usValue;
        if (struSOTDMAPara.MsgTxTimSlot >= 2250)
        {
            struSOTDMAPara.MsgTxTimFrame += struSOTDMAPara.MsgTxTimSlot / 2250;
            struSOTDMAPara.MsgTxTimSlot %= 2250;
        }    
    }
    else
    {
        struSOTDMAPara.MsgTxTimSlot += usValue;
        if (struSOTDMAPara.MsgTxTimSlot >= 2250)
        {
           struSOTDMAPara.MsgTxTimFrame += struSOTDMAPara.MsgTxTimSlot / 2250; 
           struSOTDMAPara.MsgTxTimSlot %= 2250;
        }     
    }
}

/*******************************************************************************
* Function Name  : SetBurstSendTim
* Description    : �����鷢�Ͷ�ʱ
* Input          : usValue - ���Ͷ�ʱ�ͷ��䶨ʱ��ʱ϶��ֵ
* Output         : None
* Return         : None
*******************************************************************************/
void SetBurstSendTim(S16 usValue)
{
    if (usValue < 0)
    {
        usValue = 0;
    }
    else if (usValue > 2250)
    {
        usValue = 2250;
    }

    struSOTDMAPara.BurstSendTimFrame = struSOTDMAPara.BurstTxTimFrame;
    struSOTDMAPara.BurstSendTimSlot = struSOTDMAPara.BurstTxTimSlot - usValue;
    if (struSOTDMAPara.BurstSendTimSlot < 0)
    {
        struSOTDMAPara.BurstSendTimSlot += 2250;
        struSOTDMAPara.BurstSendTimFrame--; 
        if (struSOTDMAPara.BurstSendTimFrame < 0)
        {
            struSOTDMAPara.BurstSendTimSlot = 0;
            struSOTDMAPara.BurstSendTimFrame = 0; 
        }
    } 
}

/*******************************************************************************
* Function Name  : SetMsgSendTim
* Description    : ����������Ϣ���Ͷ�ʱ
* Input          : usValue - ���Ͷ�ʱ�ͷ��䶨ʱ��ʱ϶��ֵ
* Output         : None
* Return         : None
*******************************************************************************/
void SetMsgSendTim(S16 usValue)
{
    if (usValue < 0)
    {
        usValue = 0;
    }
    else if (usValue > 2250)
    {
        usValue = 2250;
    }

    struSOTDMAPara.MsgSendTimFrame = struSOTDMAPara.MsgTxTimFrame;
    struSOTDMAPara.MsgSendTimSlot = struSOTDMAPara.MsgTxTimSlot - usValue;
    if (struSOTDMAPara.MsgSendTimSlot < 0)
    {
        struSOTDMAPara.MsgSendTimSlot += 2250;
        struSOTDMAPara.MsgSendTimFrame--; 
        if (struSOTDMAPara.MsgSendTimFrame < 0)
        {
            struSOTDMAPara.MsgSendTimSlot = 0;
            struSOTDMAPara.MsgSendTimFrame = 0; 
        }
    } 
}
#if 0
/*******************************************************************************
* Function Name  : SetGpsOnTim
* Description    : ����GPS������ʱ
* Input          : usValue - GPS������ʱ�ͷ��䶨ʱ��ʱ϶��ֵ
* Output         : None
* Return         : None
*******************************************************************************/
void SetGpsOnTim(S16 usValue)
{
    S32 slSlotNums;

    if (usValue < 0)
    {
        usValue = 0;
    }

    slSlotNums = struSOTDMAPara.BurstTxTimFrame * 2250 + struSOTDMAPara.BurstTxTimSlot;
    slSlotNums -= usValue;
    if (slSlotNums < 0)
    {
        slSlotNums = 0;
    }

    SetGpsOnTimFrame(slSlotNums / 2250);
    SetGpsOnTimSlot(slSlotNums % 2250);
}
#endif
#if 0
/*******************************************************************************
* Function Name  : SetCommState_Test
* Description    : ����SOTDMA��ͨ��״̬
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommState_Test(void)
{
    /*lq ����ͬ��״̬*/
    if (GetGpsNormalFixState() == TRUE)
    {
        struSOCommState.ucSyncState = 0;    
    } 
    else
    {
        struSOCommState.ucSyncState = 3;    
    }
    
    /*lq ����ʱ϶��ʱֵ*/
    struSOCommState.ucSlotTimeout = 0;

    /*lq ����ͨ��״̬����Ϣ*/
    struSOCommState.usSubMsg = 0;
}
#endif

#if 0
/*******************************************************************************
* Function Name  : SetCommState
* Description    : ����SOTDMA��ͨ��״̬
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommState(void)
{
    /*lq ����ͬ��״̬*/
    if (GetGpsNormalFixState() == TRUE)
    {
        struSOCommState.ucSyncState = 0;    
    } 
    else
    {
        struSOCommState.ucSyncState = 3;    
    }
    
    /*lq ����ʱ϶��ʱֵ������ͨ��״̬����Ϣ*/
    SetCommStaSubMsg(struSOCommState.ucSlotTimeout);
}
#endif

/*******************************************************************************
* Function Name  : SetCommStateITDMA_Test
* Description    : �û�����ģʽ������ITDMA��ͨ��״̬
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommStateITDMA_Test(U8 synFlg)
{
    /*lq ����ͬ��״̬*/
    if (synFlg == TRUE)
    {
        struICommState.ucSyncState = 0;    
    } 
    else
    {
        struICommState.ucSyncState = 3;    
    }

    /*lq ����ʱ϶������Ԥռʱ϶��*/
    struICommState.usSlotIncrement = 0;         //lq ��Ԥռʱ϶
    struICommState.ucSlotNumbers = 1;

    /*lq ���ñ��ֱ�־*/
    struICommState.ucKeepFlag = 0;              // ���ֱ�ǣ�������                                                        // ���յ�̨վ��
}

/*******************************************************************************
* Function Name  : SetCommStateITDMA
* Description    : ����ITDMA��ͨ��״̬
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommStateITDMA(U8 synFlg, U16 slotIncr)
{
    /*lq ����ͬ��״̬*/
    if (synFlg == TRUE)
    {
        struICommState.ucSyncState = 0;    
    } 
    else
    {
        struICommState.ucSyncState = 3;    
    }

    /*lq ����ʱ϶������Ԥռʱ϶��*/
    if (slotIncr > 8191)                                  //lq 1371-4��׼ ʱ϶��������8191
    {
        slotIncr -= 8192;
        struICommState.usSlotIncrement = slotIncr < 8192 ? slotIncr : 8191; //lq ���ֵ8191
        struICommState.ucSlotNumbers = 7;
    }
    else
    {
        struICommState.usSlotIncrement = slotIncr; //lq ���ֵ8191
        struICommState.ucSlotNumbers = 2;
    }

    /*lq ���ñ��ֱ�־*/
    struICommState.ucKeepFlag = 0;                                             // ���ֱ�ǣ�������                                                        // ���յ�̨վ��
}

/*******************************************************************************
* Function Name  : GetSetCommStateSlotIncr
* Description    : ��ȡʱ϶����ʵ��ֵ
* Input          : None
* Output         : None
* Return         : ʱ϶����ʵ��ֵ
*******************************************************************************/
U16 GetSetCommStateSlotIncr(void)
{
    if (struICommState.ucSlotNumbers < 5)   
    {
        /*lq Ԥռʱ϶������С��8192*/
        return struICommState.usSlotIncrement;
    } 
    else
    {
        /*lq Ԥռʱ϶���������ڵ���8192*/
        return struICommState.usSlotIncrement + 8192;        
    }
}
#if 0
/*******************************************************************************
* Function Name  : SetCommStaSubMsg
* Description    : ����ʱ϶��ʱֵ������SOTDMA��ͨ��״̬����Ϣ
* Input          : ucTimeout - ʱ϶��ʱֵ
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommStaSubMsg(S8 ucTimeout)
{
    if (ucTimeout < 0 || ucTimeout > 7)
    {
        ucTimeout = 0;
    }

    /*lq ����ʱ϶��ʱֵ������SOTDMA��ͨ��״̬����Ϣ*/
    if (ucTimeout == 0)
    {
        struSOCommState.usSubMsg = struSOCommState.usSlotOffset;
    }
    else if (ucTimeout == 1)
    {
        struSOCommState.usSubMsg = struSOCommState.usUTC_HourMin;
    }
    else if (ucTimeout == 2 || ucTimeout == 4 ||ucTimeout == 6)
    {
        struSOCommState.usSubMsg = struSOCommState.usSlotNum;
    }
    else if (ucTimeout == 3 || ucTimeout == 5 ||ucTimeout == 7)
    {
        struSOCommState.usSubMsg = struSOCommState.usRecStations;
    }
}
#endif
/*******************************************************************************
* Function Name  : GetBurstTxTimSlot
* Description    : ��ȡ���ڵ�һ����Ϣ�ķ���ʱ϶
* Input          : None
* Output         : None
* Return         : ���ڵ�һ����Ϣ�ķ���ʱ϶
*******************************************************************************/
U16 GetBurstTxTimSlot(void)
{
    return struSOTDMAPara.BurstTxTimSlot;
}

/*******************************************************************************
* Function Name  : GetMsgTxTimSlot
* Description    : ��ȡ����������Ϣ�ķ���ʱ϶
* Input          : None
* Output         : None
* Return         : ��������������Ϣ�ķ���ʱ϶
*******************************************************************************/
U16 GetMsgTxTimSlot(void)
{
    return struSOTDMAPara.MsgTxTimSlot;
}

/*******************************************************************************
* Function Name  : SetBurstFlg
* Description    : �������ڵ�һ����Ϣ�����־
* Input          : ucFlag - true:���ڵ�һ����Ϣ����,false:����������Ϣ����
* Output         : None
* Return         : None
*******************************************************************************/
void SetBurstFlg(U8 ucFlag)
{
    if (ucFlag == TRUE)
    {
        struSOTDMAPara.BurstFlg = TRUE;    
    }
    else
    {
        struSOTDMAPara.BurstFlg = FALSE;    
    }
}

/*******************************************************************************
* Function Name  : GetBurstFlg
* Description    : ��ȡ���ڵ�һ����Ϣ�����־
* Input          : None
* Output         : None
* Return         : ���ڵ�һ����Ϣ�����־
*******************************************************************************/
U8 GetBurstFlg(void)
{
    return struSOTDMAPara.BurstFlg;    
}

#if 0
/*******************************************************************************
* Function Name  : SetTxChannel
* Description    : ���÷����ŵ�,SART������ʽ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetTxChannel(void)
{
    if (struSOTDMAPara.MsgCount & 0x1 == 1)
    {
        struSOTDMAPara.TxChannel = 'A';                             
    }
    else
    {
        struSOTDMAPara.TxChannel = 'B';    
    }
}
#endif

/*******************************************************************************
* Function Name  : SetTxChannel
* Description    : ���÷����ŵ�,SART������ʽ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetTxChannel(void)
{
    if (siStaticPara.TxChannel1 == 2087)
    {
        struSOTDMAPara.TxChannel = 'A';                             
    }
    else if (siStaticPara.TxChannel1 == 2088)
    {
        struSOTDMAPara.TxChannel = 'B';    
    }
    else
    {
        struSOTDMAPara.TxChannel = 'A';    
    }
}

/*******************************************************************************
* Function Name  : GetTxChannel
* Description    : ��ȡ�����ŵ�
* Input          : None
* Output         : None
* Return         : �����ŵ�
*******************************************************************************/
U8 GetTxChannel(void)
{
    return struSOTDMAPara.TxChannel;    
}
#if 0
/*******************************************************************************
* Function Name  : SetCommStaSlotNum
* Description    : ����ͨ��״̬ʱ϶��
* Input          : ssSlotNum - ͨ��״̬ʱ϶��
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommStaSlotNum(S16 ssSlotNum)
{
    if (ssSlotNum < 0 || ssSlotNum >= 2250)
    {
       ssSlotNum = 0; 
    }

    struSOCommState.usSlotNum = ssSlotNum;
}

/*******************************************************************************
* Function Name  : SetCommStaUTCHourMin
* Description    : ����ͨ��״̬UTCСʱ�ͷ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommStaUTCHourMin(void)
{
    struSOCommState.usUTC_HourMin &= 0x0;

    if (IsGpsFix() == TRUE)
    {
       struSOCommState.usUTC_HourMin |= GpsPara.GPS_TimeH << 9;
       struSOCommState.usUTC_HourMin &= 0xFE00;
       struSOCommState.usUTC_HourMin |= GpsPara.GPS_TimeM << 2;
    }
}

/*******************************************************************************
* Function Name  : SetCommStaSlotOffset
* Description    : ����ͨ��״̬ʱ϶ƫ��ֵ
* Input          : ͨ��״̬ʱ϶ƫ��ֵ
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommStaSlotOffset(S16 ssSlotOffset)
{
    if (ssSlotOffset < COMM_STAT_SLOT_OFFSET_FLOOR)
    {
        ssSlotOffset = COMM_STAT_SLOT_OFFSET_FLOOR;
    }
    else if (ssSlotOffset > COMM_STAT_SLOT_OFFSET_CEIL)
    {
        ssSlotOffset = COMM_STAT_SLOT_OFFSET_CEIL;
    }

    struSOCommState.usSlotOffset = ssSlotOffset;
}

/*******************************************************************************
* Function Name  : GetCommStaSlotOffset
* Description    : ��ȡͨ��״̬ʱ϶ƫ��ֵ
* Input          : None
* Output         : None
* Return         : ͨ��״̬ʱ϶ƫ��ֵ
*******************************************************************************/
U16 GetCommStaSlotOffset(void)
{
    return struSOCommState.usSlotOffset;
}

/*******************************************************************************
* Function Name  : GetCommStaSlotOffset
* Description    : ��ȡͨ��״̬ʱ϶��ʱֵ
* Input          : None
* Output         : None
* Return         : ͨ��״̬ʱ϶��ʱֵ
*******************************************************************************/
S8 GetCommStaSlotTimeout(void)
{
    return struSOCommState.ucSlotTimeout;
}
#endif
/*******************************************************************************
* Function Name  : GetMsgCnt
* Description    : ��ȡ������Ϣ��
* Input          : None
* Output         : None
* Return         : ������Ϣ��
*******************************************************************************/
S8 GetSOParaMsgCnt(void)
{
    return struSOTDMAPara.MsgCount;
}

/*******************************************************************************
* Function Name  : SetBurstFinsFlg
* Description    : ��λ�鷢�������־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetBurstFinsFlg(void)
{
    struSOTDMAPara.BurstFinsFlg = TRUE;                            
}

/*******************************************************************************
* Function Name  : ResetBurstFinsFlg
* Description    : ��λ�鷢�������־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetBurstFinsFlg(void)
{
    struSOTDMAPara.BurstFinsFlg = FALSE;                            
}

/*******************************************************************************
* Function Name  : GetBurstFinsFlg
* Description    : ��ȡ�鷢�������־
* Input          : None
* Output         : None
* Return         : �鷢�������־
*******************************************************************************/
U8 GetBurstFinsFlg(void)
{
    return struSOTDMAPara.BurstFinsFlg;
}

/*******************************************************************************
* Function Name  : GetBurstFinsFlgRst
* Description    : ��ȡ�鷢�������־,���Ѿ���λ��λ
* Input          : None
* Output         : None
* Return         : �鷢�������־
*******************************************************************************/
U8 GetBurstFinsFlgRst(void)
{
    if (struSOTDMAPara.BurstFinsFlg == TRUE)
    {
        ResetBurstFinsFlg();

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* Function Name  : SetBurstStarFlg
* Description    : ��λ�鷢�Ϳ�ʼ��־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetBurstStarFlg(void)
{
    struSOTDMAPara.BurstStarFlg = TRUE;                            
}

/*******************************************************************************
* Function Name  : ResetBurstStarFlg
* Description    : ��λ�鷢�Ϳ�ʼ��־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetBurstStarFlg(void)
{
    struSOTDMAPara.BurstStarFlg = FALSE;                            
}

/*******************************************************************************
* Function Name  : GetBurstStarFlg
* Description    : ��ȡ�鷢�Ϳ�ʼ��־
* Input          : None
* Output         : None
* Return         : �鷢�Ϳ�ʼ��־
*******************************************************************************/
U8 GetBurstStarFlg(void)
{
    return struSOTDMAPara.BurstStarFlg;                            
}

/*******************************************************************************
* Function Name  : GetBurstStarFlgRst
* Description    : ��ȡ�鷢�Ϳ�ʼ��־,����λ
* Input          : None
* Output         : None
* Return         : �鷢�Ϳ�ʼ��־
*******************************************************************************/
U8 GetBurstStarFlgRst(void)
{
    if (struSOTDMAPara.BurstStarFlg == TRUE)
    {
        ResetBurstStarFlg();                            

        return TRUE;
    }
    else
    {
        return FALSE;
    }                            
}
