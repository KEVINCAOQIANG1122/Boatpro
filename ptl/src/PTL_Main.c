/*******************************************************************************
  Copyright (C), 2012, 712.Co.,Ltd.
  FileName: PTL_main.c
  Author: ����      Version : V1.00        Date: 2012.07.10
  Description:    ʵ��Э��ջ��Ϣ��֯�뷢��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>
      lq        2013.09.06               add AISmain_4503_ITDMA()
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "AIS_MsgModu.h"
#include "Ptl_Interface.h"
#include "PTL_Main.h"
#include "AISconfig.h"
#include "initial.h"
#include "sleepctrl.h"
#include "SOTDMA.h"
#include "gpsctrl.h"
#include "testing.h"

#if 1
#include "tim.h"
#endif 
U8 state_4503 = INIT_4503;

U8 state_4501 = INIT_4501;
U16 timeout_4501_interval;
U16 time_init_4503;
U16 timestamp_sos;
U16 flag_sos;
U16 idle_4503 = 0;

U16 SosSendProtectTime = 0; 

U8 ucChannelCurr = CHANNEL_A;
extern S32 GetFrameNum(void);
extern S16 GetSlotNum(void);
extern U8 GetMMISOSState(void);
extern U16 Update_GpsData_To_Msg(void);

/******************************************
������Ptl_Init
����: Э��ջ��ʼ��
����:
���:
����ֵ��
��ע��
*******************************************/
U16 Ptl_Init(void)
{
#if 0
	InitInterfacePara();

	SendSlotSelectedFlag = 0;
#endif
	InitPtl_Interface();
#if 0
	InitMsgPara();
#endif
	state_4503 = INIT_4503;
	state_4501 = INIT_4501;
	GPS_valid = 0;
	flag_sos = 0;

//	ModeFlag = eqEquipPara.ucPowerSaveMode == EQ_MOD_POWERSAVE ? 0 : 1;

//	SosSendProtectTime = eqEquipPara.SosProtectTime * 375;//ʱ϶10�� = 375

	return 0;
}

/*******************************************************************************
* ����  : get_timestamp
* ����  : ��ȡʱ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 get_timestamp(void)
{
	S32 min;
	S16 slot;
	U16 timestamp;
	
	min = GetFrameNum();
	slot = GetSlotNum();
	timestamp = (((U8)(min & TIMESTAMP_MASK)) * FRAMETOSLOT) + (U16)slot;
	
	return timestamp;
}

/*******************************************************************************
* ����  : IsTimeout
* ����  : ��֤�Ƿ�ʱ
* ����  : time1Ҫ�ȵ�ʱ��
		  time2��ǰʱ��	
		  timeout��ʱʱ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 IsTimeout(U16 time1, U16 time2, U16 timeout)
{
	if(time2 >= time1)
	{
		return (time2 - time1 >= timeout);
	}
	else
	{
	//lq	return ((time2 - time1 + (TIMESTAMP_MASK * FRAMETOSLOT + FRAMETOSLOT - 1)) >= timeout);
        return ((time2 - time1 + (TIMESTAMP_MASK * FRAMETOSLOT + FRAMETOSLOT)) >= timeout);
	}
}

/*******************************************************************************
* ����  : set_sos_timestamp
* ����  : ����SOS��ʱʱ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 set_sos_timestamp(void)
{
	timestamp_sos = get_timestamp();
	return 0;
}

/*******************************************************************************
* ����  : get_timestamp
* ����  : ��ȡʱ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
S32 get_timestamp_slot(void)
{
	S32 min;
	S16 slot;
	S32 timestamp;
	
	min = GetFrameNum();
	slot = GetSlotNum();
	timestamp = min * FRAMETOSLOT + (U16)slot;
	
	return timestamp;
}

/*******************************************************************************
* ����  : IsTimeout
* ����  : ��֤�Ƿ�ʱ
* ����  : time1Ҫ�ȵ�ʱ��
		  time2��ǰʱ��			  	  
* ���  : ��
* ����  : �Ƿ�ʱ
*******************************************************************************/
U8 IsTimeout_slot(S32 time1, S32 time2, S16 timeout)
{	
    return (time2 - time1 >= timeout);
}

/*******************************************************************************
* ����  : set_sos_timestamp_slot
* ����  : ����SOS��ʱʱ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 set_sos_timestamp_slot(void)
{
	timestamp_sos = get_timestamp_slot();
	return 0;
}

/*******************************************************************************
* ����  : set_4501_interval
* ����  : ����4501ģʽ��Ϣ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 set_4501_interval(U16 speed)
{
	if(speed >= SPEEDFLAG)
	{
		timeout_4501_interval = TIMEOUT_HIGHSPEED_4501;
	}
	else
	{
		timeout_4501_interval = TIMEOUT_LOWSPEED_4501;
	}
	return 0;
}

U16 time_4501_last1;
U16 time_4501_last2;

#if 0
/*******************************************************************************
* ����  : AISmain_4503_SOTDMA
* ����  : ITDMA���뷽ʽ,WWY��Ϣ���ͷ�ʽ
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 AISmain_4503_ITDMA(void)
{
	U16 i;
	S32 time_4501_now;
    S32 slSlotCurr;
	U8 ucSendIntervalChangeFlg;
    U16 usTempSlot;
    //U16 usRandSlotOffset;
	
    /*�ж�MMSI�Ƿ���Ч*/   
	if(aiAISCell.UserID == 0)
	{   
		return 0;
	}

    slSlotCurr = get_timestamp_slot();                                          //lq ��ȡ��ǰʱ���

	switch(state_4501)
	{
		case INIT_4501:
            
            /*lq ��������1min�򿪻�1min�ڶ�λ��ͬ�������ʼ����*/
            if(slSlotCurr >= 2250 || GetGpsPowerupFixState() == TRUE)
			{
                //lq...
#if 0           //lq ���Ʒ���ʱ϶Ϊ������
                slSlotCurr = slSlotCurr + SLOT_INTERVAL_SEND_TO_TX + 1;
                slSlotCurr = slSlotCurr / 75 + 1;
                slSlotCurr *= 75;
                SetBurstTxTim(slSlotCurr);                    
#else
                SetBurstTxTim(slSlotCurr + SLOT_INTERVAL_SEND_TO_TX + 1);                    
#endif
                SetBurstSendTim(SLOT_INTERVAL_SEND_TO_TX);
			    state_4501 = NORMAL_WORK_BURST;
            }
			else
			{
                //lq...
			}

			break;
		case NORMAL_WORK_BURST:
			if(IsTimeout_slot(GetBurstSendTim(), slSlotCurr, 0))                //lq Burst���Ͷ�ʱ��
			{   					
                SetBurstStarFlg();
                //lq ResetBurstFinsFlg();//lq... ����standbyģʽ��ɾ�������
#if 0
                SetCommStaSlotNum(GetBurstTxTimSlot());				

                SetCommStaUTCHourMin();

                SetCommStaSlotOffset(GetRandSlotOffset());                      //lq ��һ���ĵ�һ���ʱ϶ƫ��ֵ

                struSOCommState.ucSlotTimeout = --struSOCommState.ucSlotTimeout < 0 
                                                ? 7 : struSOCommState.ucSlotTimeout;

                SetCommState();
#endif
                SetBurstFlg(TRUE);
 
                struSOTDMAPara.MsgCount = 1;
                SetTxChannel();

                /*lq ��ȡ�������ƫ�Ƶ�Ԥռʱ϶������*/
                struSOTDMAPara.SlotOffset = GetRandSlotOffset((S16)eqEquipPara.SendInterval * 2250); //lq ������ٴ��õ����豣��
                SetCommStateITDMA(GetGpsNormalFixState(), struSOTDMAPara.SlotOffset);

                /*lq �Ⱥ���18��19����Ϣ*/                 
                SendMsg18_SO(TRUE, FALSE);
			    usart_printf("[%6d:%4d]$Msg18 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);

                SendMsg19_SO(TRUE, FALSE);          
			    usart_printf("[%6d:%4d]$Msg19 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);
                                                                                       	 
                /*lq ������������Ϣ���估���Ͷ�ʱ*/
                SetMsgTxTim(SLOT_INTERVAL_MSG_IN_BURST);
                SetMsgSendTim(SLOT_INTERVAL_SEND_TO_TX);
                
                /*lq ��һ����Ϣ�ķ��估���Ͷ�ʱ*/                
                SetBurstTxTim(GetSetCommStateSlotIncr());                                
                SetBurstSendTim(SLOT_INTERVAL_SEND_TO_TX);

                //lq SetGpsOnTim(SLOT_INTERVAL_GPS_ON_TO_TX);
                
                state_4501 = NORMAL_WORK_MSG;
#if 0 //lqdb
                gsiPhaseCnt = 0;                                                                //lq  gsiPhaseCnt��gsiSlotCnt��gslFrameCnt                                                                               
                gsiSlotCnt = 0;                                                                 //lq  ���߽�������Ϣ���͵�ʱ����ƣ�
                gslFrameCnt = 0;                
#endif
			}
            
            break;
        case NORMAL_WORK_MSG:
			if(IsTimeout_slot(GetMsgSendTim(), slSlotCurr, 0))  //lq Burst��Msg���Ͷ�ʱ��
			{   					
                struSOTDMAPara.MsgCount++;
                SetTxChannel();

                SetBurstFlg(FALSE);

                SetCommStateITDMA(GetGpsNormalFixState(), struSOTDMAPara.SlotOffset);

                /*lq �Ⱥ���18��19����Ϣ*/
                SendMsg18_SO(FALSE, FALSE);
			    usart_printf("[%6d:%4d]$Msg18 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);
                
                SendMsg19_SO(FALSE, FALSE);    
			    usart_printf("[%6d:%4d]$Msg19 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);	 
                                                                          
                if(struSOTDMAPara.MsgCount < eqEquipPara.NormalSendTime)
                {
                    SetMsgTxTim(SLOT_INTERVAL_MSG_IN_BURST);
                    SetMsgSendTim(SLOT_INTERVAL_SEND_TO_TX);
                }
                else
                {
                    state_4501 = NORMAL_WORK_BURST;                
                }
			}

            break;
		default:
			break;
	}

	return 0;
}
#endif

#if 0
/*******************************************************************************
* ����  : AISmain_AtoN_FATDMA
* ����  : FATDMA���뷽ʽ��AtoN��Ϣ���ͷ�ʽ
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 AISmain_AtoN_FATDMA(void)
{
	U16 i;
	S32 time_4501_now;
    S32 slSlotCurr;
	U8 ucSendIntervalChangeFlg;
    U16 usTempSlot;
    //U16 usRandSlotOffset;
	U32 slFrameCurr;
    U32 slFrameDay;
    U32 slSlotDay;
    U32 slFirstTxTime;
    U32 slTemp;

    /*�ж�MMSI�Ƿ���Ч*/   
	if(aiAISCell.UserID == 0)
	{   
		return 0;
	}

    slSlotCurr = get_timestamp_slot();                                          //lq ��ȡ��ǰʱ���
    slFrameCurr = GetFrameNum();
	
    switch (state_4501)
	{
		case INIT_4501:
            
            /*lq ������λ��ŷ��͵�һ����Ϣ*/
            if (GetGpsPowerupFixState() == TRUE)
			{
                /*lq �����һ�η���ʱ��*/
                slFirstTxTime = (siStaticPara.TxHourChannel1 * 60 + siStaticPara.TxMinChannel1) * 2250 
                                   + siStaticPara.TxStartSlotChannel1;
                
                slFrameDay = slFrameCurr % 1440;                                //lq һ���е�֡��������0��Ϊ��׼��
                slSlotDay = slFrameDay * 2250 + GetSlotNum();

			    /*lq ��һ�η���ʱ�䵽 */
                if ((slSlotDay < slFirstTxTime)
                    && IsTimeout_slot(slFirstTxTime, slSlotDay + SLOT_INTERVAL_SEND_TO_TX, 0))
                {
                    slFirstTxTime += (slFrameCurr - slFrameDay) * 2250;         //lq ����ʱ�䶨ʱʹ�ñ���֡�������ʷ���ʱ��Ϊָ���ķ���ʱ϶�����0ʱ�����ʱ϶����������һ��0��ʱ����֡����ֵ
                    SetBurstTxTim(slFirstTxTime);                                                        
                    
                    SetBurstSendTim(SLOT_INTERVAL_SEND_TO_TX);
    			    
                    state_4501 = NORMAL_WORK_BURST;
                                
                }                
#if 0           //lq ���Ʒ���ʱ϶Ϊ������
                slSlotCurr = slSlotCurr + SLOT_INTERVAL_SEND_TO_TX + 1;
                slSlotCurr = slSlotCurr / 75 + 1;
                slSlotCurr *= 75;
                SetBurstTxTim(slSlotCurr);                    
#endif

            }
			else
			{
                //lq...
			}

			break;
		case NORMAL_WORK_BURST:
			if (IsTimeout_slot(GetBurstSendTim(), slSlotCurr, 0))                //lq Burst���Ͷ�ʱ��
			{   					
                SetBurstStarFlg();
                //lq ResetBurstFinsFlg();//lq... ����standbyģʽ��ɾ�������
#if 0
                SetCommStaSlotNum(GetBurstTxTimSlot());				

                SetCommStaUTCHourMin();

                SetCommStaSlotOffset(GetRandSlotOffset());                      //lq ��һ���ĵ�һ���ʱ϶ƫ��ֵ

                struSOCommState.ucSlotTimeout = --struSOCommState.ucSlotTimeout < 0 
                                                ? 7 : struSOCommState.ucSlotTimeout;

                SetCommState();
#endif
                SetBurstFlg(TRUE);
 
                struSOTDMAPara.MsgCount = 1;
                SetTxChannel();
#if 0
                /*lq ��ȡ�������ƫ�Ƶ�Ԥռʱ϶������*/
                struSOTDMAPara.SlotOffset = GetRandSlotOffset((S16)eqEquipPara.SendInterval * 2250); //lq ������ٴ��õ����豣��
                SetCommStateITDMA(GetGpsNormalFixState(), struSOTDMAPara.SlotOffset);
#endif
                /*lq ����21����Ϣ*/                 
                SendMsg21();
			    usart_printf("[%6d:%4d]$Msg21 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);

                SendMsg6();          
			    usart_printf("[%6d:%4d]$Msg06 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);

                /*lq 14��Ϣ */
                if (GetAtoNAlarmFlag() == TRUE)
                {
                    SendMsg14();          
    			    usart_printf("[%6d:%4d]$Msg14 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                                 struSOTDMAPara.MsgCount);                                    
                }

                if (struSOTDMAPara.MsgCount < eqEquipPara.NormalSendTime)
                {                                                                                       	 
                    /*lq ������������Ϣ���估���Ͷ�ʱ*/
                    SetMsgTxTim(SLOT_INTERVAL_MSG_IN_BURST);
                    SetMsgSendTim(SLOT_INTERVAL_SEND_TO_TX);

                    state_4501 = NORMAL_WORK_MSG;
                }

                /*lq ��һ����Ϣ�ķ��估���Ͷ�ʱ*/                
                if ((siStaticPara.OffPositionBehav == 1)
                    && (OP1_GetGPSOffPositionValid() == TRUE)
                    && (OP1_GetGPSOffPositionStat() == TRUE))
                {
                    /*lq λ��ƫ��ʱ��ʹ���µķ��ͼ��*/
                    SetBurstTxTim(siStaticPara.TxSlotIntervalChannel1Off);                                                
                }
                else
                {
                    SetBurstTxTim(siStaticPara.TxSlotIntervalChannel1);                                                
                }
                SetBurstSendTim(SLOT_INTERVAL_SEND_TO_TX);
			}
            
            break;

        case NORMAL_WORK_MSG:
			if(IsTimeout_slot(GetMsgSendTim(), slSlotCurr, 0))  //lq Burst��Msg���Ͷ�ʱ��
			{   					
                struSOTDMAPara.MsgCount++;
                SetTxChannel();

                SetBurstFlg(FALSE);

                SetCommStateITDMA(GetGpsNormalFixState(), struSOTDMAPara.SlotOffset);

                /*lq ����21����Ϣ*/
                SendMsg21();
			    usart_printf("[%6d:%4d]$Msg21 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);
                
                SendMsg6();    
			    usart_printf("[%6d:%4d]$Msg06 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);	 
                                                                          
                /*lq 14��Ϣ */
                if (GetAtoNAlarmFlag() == TRUE)
                {
                    SendMsg14();          
    			    usart_printf("[%6d:%4d]$Msg14 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                                 struSOTDMAPara.MsgCount);                                    
                }

                if(struSOTDMAPara.MsgCount < eqEquipPara.NormalSendTime)
                {
                    SetMsgTxTim(SLOT_INTERVAL_MSG_IN_BURST);
                    SetMsgSendTim(SLOT_INTERVAL_SEND_TO_TX);
                }
                else
                {
                    state_4501 = NORMAL_WORK_BURST;                
                }
			}

            break;

		default:
			break;
	}

	return 0;
}
#endif

#if 0
/*******************************************************************************
* ����  : AISMain_4501
* ����  : 4501��Ϣ����Э�飨��ʱ����WWY�ķ��ͷ�ʽ��18��19��Ϣ�������ͣ�ÿ�鷢����
          ����6/8/12����Ϣ��δ���ͣ�
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 AISMain_4501(void)
{
	U16 i;
	S32 time_4501_now;
    S32 slSlotCurr;
	U8 ucSendIntervalChangeFlg;
    U16 usTempSlot;
    //U16 usRandSlotOffset;
	
    /*�ж�MMSI�Ƿ���Ч*/   
	if(aiAISCell.UserID == 0)
	{   
		return 0;
	}

    slSlotCurr = get_timestamp_slot();                                          //lq ��ȡ��ǰʱ���

	switch(state_4501)
	{
		case INIT_4501:
            
            /*lq ��������1min�򿪻�1min�ڶ�λ��ͬ�������ʼ����*/
            if(slSlotCurr >= 2250 || GetGpsPowerupFixState() == TRUE)
			{
                //lq...
#if 0           //lq ���Ʒ���ʱ϶Ϊ������
                slSlotCurr = slSlotCurr + SLOT_INTERVAL_SEND_TO_TX + 1;
                slSlotCurr = slSlotCurr / 75 + 1;
                slSlotCurr *= 75;
                SetBurstTxTim(slSlotCurr);                    
#else
                SetBurstTxTim(slSlotCurr + SLOT_INTERVAL_SEND_TO_TX + 1);                    
#endif
                SetBurstSendTim(SLOT_INTERVAL_SEND_TO_TX);
			    state_4501 = NORMAL_WORK_BURST;
            }
			else
			{
                //lq...
			}

			break;
		case NORMAL_WORK_BURST:
			if(IsTimeout_slot(GetBurstSendTim(), slSlotCurr, 0))                //lq Burst���Ͷ�ʱ��
			{   					
                SetBurstStarFlg();

                SetBurstFlg(TRUE);
 
                struSOTDMAPara.MsgCount = 1;
                SetTxChannel();

                /*lq ��ȡ�������ƫ�Ƶ�Ԥռʱ϶������*/
                struSOTDMAPara.SlotOffset = GetRandSlotOffset((S16)eqEquipPara.SendInterval * 2250); //lq ������ٴ��õ����豣��
                SetCommStateITDMA(GetGpsNormalFixState(), struSOTDMAPara.SlotOffset);

                /*lq �Ⱥ���18��19����Ϣ*/                 
                SendMsg18(TRUE);
			    usart_printf("[%6d:%4d]$Msg18 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);

                SendMsg19(TRUE);          
			    usart_printf("[%6d:%4d]$Msg19 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);
                                                                                       	 
                /*lq ������������Ϣ���估���Ͷ�ʱ*/
                SetMsgTxTim(SLOT_INTERVAL_MSG_IN_BURST);
                SetMsgSendTim(SLOT_INTERVAL_SEND_TO_TX);
                
                /*lq ��һ����Ϣ�ķ��估���Ͷ�ʱ*/                
                SetBurstTxTim(GetSetCommStateSlotIncr());                                
                SetBurstSendTim(SLOT_INTERVAL_SEND_TO_TX);

                //lq SetGpsOnTim(SLOT_INTERVAL_GPS_ON_TO_TX);
                
                state_4501 = NORMAL_WORK_MSG;
			}
            
            break;
        case NORMAL_WORK_MSG:
			if(IsTimeout_slot(GetMsgSendTim(), slSlotCurr, 0))  //lq Burst��Msg���Ͷ�ʱ��
			{   					
                struSOTDMAPara.MsgCount++;
                SetTxChannel();

                SetBurstFlg(FALSE);

                SetCommStateITDMA(GetGpsNormalFixState(), struSOTDMAPara.SlotOffset);

                /*lq �Ⱥ���18��19����Ϣ*/
                SendMsg18(FALSE);
			    usart_printf("[%6d:%4d]$Msg18 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);
                
                SendMsg19(FALSE);    
			    usart_printf("[%6d:%4d]$Msg19 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);	 
                                                                          
                if(struSOTDMAPara.MsgCount < eqEquipPara.NormalSendTime)
                {
                    SetMsgTxTim(SLOT_INTERVAL_MSG_IN_BURST);
                    SetMsgSendTim(SLOT_INTERVAL_SEND_TO_TX);
                }
                else
                {
                    state_4501 = NORMAL_WORK_BURST;                
                }
			}

            break;
		default:
			break;
	}

	return 0;
}
#endif

#if 0
/*******************************************************************************
* ����  : AISmain_ITDMA_MsgTest
* ����  : ITDMA���뷽ʽ����Ϣ����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 AISmain_ITDMA_MsgTest(void)
{
	U16 i;
	S32 time_4501_now;
    S32 slSlotCurr;
	U8 ucSendIntervalChangeFlg;
    U16 usTempSlot;
	
    /*�ж�MMSI�Ƿ���Ч*/   
	if(aiAISCell.UserID == 0)
	{   
		return 0;
	}

    slSlotCurr = get_timestamp_slot();                                          //lq ��ȡ��ǰʱ���

	switch(state_4501)
	{
		case INIT_4501:
            
            /*lq ����������ʱ�򿪻���ʱ�ڶ�λ�ɹ���ʼ����*/           
            if(GetMsgTestSendFlg() == TRUE)
			{
                //lq...
                SetBurstTxTim(slSlotCurr + SLOT_INTERVAL_SEND_TO_TX + 1);                    
                SetBurstSendTim(SLOT_INTERVAL_SEND_TO_TX);
			    state_4501 = NORMAL_WORK_BURST;
            }
			else
			{
                //lq...
			}

			break;
		case NORMAL_WORK_BURST:
			if(IsTimeout_slot(GetBurstSendTim(), slSlotCurr, 0))                //lq Burst���Ͷ�ʱ��
			{   					
                SetBurstStarFlg();
                
                /*lq ��Burstȫ���������ٹر�GPS*/
                ResetBurstFinsFlg();

                /*lq ����ͨ��״̬*/
                SetCommStateITDMA_Test(GetGpsNormalFixState());
                
                SetBurstFlg(TRUE);
                
                struSOTDMAPara.MsgCount = 1;
                SetTxChannel();
                                                                          
                SendMsg18(TRUE);
			    usart_printf("[%6d:%4d]$Msg18 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);

                SendMsg19(TRUE);
			    usart_printf("[%6d:%4d]$Msg19 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);

                SetMsgTxTim(SLOT_INTERVAL_MSG_IN_BURST);
                SetMsgSendTim(SLOT_INTERVAL_SEND_TO_TX);
                                                                
                state_4501 = NORMAL_WORK_MSG;
			}
            
            break;
        case NORMAL_WORK_MSG:
			if(IsTimeout_slot(GetMsgSendTim(), slSlotCurr, 0))  //lq Burst��Msg���Ͷ�ʱ��
			{   					
                struSOTDMAPara.MsgCount++;
                SetTxChannel();

                SetBurstFlg(FALSE);

                if (GetSOParaMsgCnt() == eqEquipPara.NormalSendTime)
                {
                    /*lq ����ͨ��״̬*/
                    SetCommStateITDMA_Test(GetGpsNormalFixState());
                    	                     
                    SendMsg18(FALSE);    
			        usart_printf("[%6d:%4d]$Msg18 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                                 struSOTDMAPara.MsgCount);
                    SendMsg19(FALSE);    
			        usart_printf("[%6d:%4d]$Msg19 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                                 struSOTDMAPara.MsgCount);
                                                                 	                     
                    /*lq ���Թػ�*/
                    struMsgTestPara.ucPowerDownEn = TRUE;
                    
                    state_4501 = NORMAL_WORK_IDLE;
                }
                else
                {
                    /*lq ����ͨ��״̬*/
                    SetCommStateITDMA_Test(GetGpsNormalFixState());
                                          
                    SendMsg18(FALSE);    
			        usart_printf("[%6d:%4d]$Msg18 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                                 struSOTDMAPara.MsgCount);                

                    SendMsg19(FALSE);    
			        usart_printf("[%6d:%4d]$Msg19 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                                 struSOTDMAPara.MsgCount);
                }

                if(struSOTDMAPara.MsgCount < eqEquipPara.NormalSendTime)
                {
                    SetMsgTxTim(SLOT_INTERVAL_MSG_IN_BURST);
                    SetMsgSendTim(SLOT_INTERVAL_SEND_TO_TX);
                }
			}

            break;
        case NORMAL_WORK_IDLE:

            break;
        default:
			break;
	}

	return 0;
}
#endif

/******************************************
������Ptl_Main
����: Э��ջ���������������øú���ʵ����Ϣ
������ش���
����:
���:
����ֵ��
��ע��
*******************************************/
void Ptl_Main(void)
{
#if 0
	Update_GpsData_To_Msg();                                                    //lq ��ȡ�������GPS��Ϣ������Ҫ���͵�x����Ϣ
	
    AISMain_4501();
#endif

}

/******************************************
������Ptl_Main_Test
����: ��Ϣ����ģʽ��Э��ջ���������������øú���ʵ����Ϣ
������ش���
����:
���:
����ֵ��
��ע��
*******************************************/
void Ptl_Main_Test(void)
{
#if 0
	Update_GpsData_To_Msg();                                                    //lq ��ȡ�������GPS��Ϣ������Ҫ���͵�x����Ϣ
    AISmain_ITDMA_MsgTest();
#endif	
}




