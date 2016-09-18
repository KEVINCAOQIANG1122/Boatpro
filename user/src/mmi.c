/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: mmi.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    MMIģ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include "def.h"
#include "initial.h"
#include "debug.h"
#include "systemtick.h"
#include "keyscan.h"
#include "mmi.h"
#include "sleepctrl.h"
#include "PTL_Main.h"
#include "Tim.h"
#include "gpsctrl.h"
#include "stdio.h"
#include "parastorage.h"
#include "sotdma.h"

MsgCtrlStruct MsgCtrlPara;
#if CONFIG_KEY
MMIStruct MMIPara;
/*******************************************************************************
* ����  : IniMMIPara
* ����  : ��ʼ��MMI��Ҫ�Ĳ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void IniMMIPara(void)
{
	MMIPara.MMICtrlSta = MMI_CTRLSTA_IDLE;
	MMIPara.MMISta = MMI_STA_NONE;
	MMIPara.Time100msCnt = 0;

	MMIPara.TimeCnt = 0;
	MMIPara.TimePeriod = 0;

	MMIPara.SOSConfirm=FALSE;//sos�澯��

//	MMIPara.MMIKeyValue = eqEquipPara.SosKeyTime * 10;
    MMIPara.MMIKeyValue = 10;
}

/*******************************************************************************
* ����  : RstMMIPara
* ����  : ��λMMI��Ҫ�Ĳ���,����SOS
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void RstMMIPara(void)//��ini�����ǲ�����sosconfirm
{
	MMIPara.MMICtrlSta = MMI_CTRLSTA_IDLE;
	MMIPara.MMISta = MMI_STA_NONE; //�������
	MMIPara.Time100msCnt = 0;

	MMIPara.TimeCnt = 0;
	MMIPara.TimePeriod = 0;
}

/*******************************************************************************
* ����  : MMIProcess
* ����  : MMI����������
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void MMIProcess(void)
{
	U8 ucKeySta;
	//U16 MMIKeyValue;

	//MMIKeyValue = eqEquipPara.SosKeyTime * 10;

	//IDLE״̬����ⰴ���Ƿ���Ч
	if (MMIPara.MMICtrlSta == MMI_CTRLSTA_IDLE)
	{
#if !WIN32
		ucKeySta = GetIndivIO(KEY_SOS_ADDR);
		
		if (ucKeySta == KEY_LOW)
		{
			//����������ⶨʱ��
			MMIPara.TimePeriod = MMI_100SECPEROID;
			MMIPara.TimeCnt = SetTimePeriodProcess(MMIPara.TimePeriod);

			MMIPara.MMICtrlSta = MMI_CTRLSTA_STEP1;			
		}
#endif
		
	}
	//�����������¼��
	else if (MMIPara.MMICtrlSta == MMI_CTRLSTA_STEP1)
	{
		//100ms��ʱ����
		if (MMIPara.TimeCnt == GetLocalClockTickProcess())
		{
#if !WIN32
			ucKeySta = GetIndivIO(KEY_SOS_ADDR);

			//��ʾ������������
			if (ucKeySta == KEY_LOW)
			{				
				MMIPara.TimeCnt = SetTimePeriodProcess(MMIPara.TimePeriod);
				
				MMIPara.Time100msCnt++;

				if (MMIPara.Time100msCnt >= MMIPara.MMIKeyValue)
				{
					MMIPara.Time100msCnt = 0;
					//�˴�������Э��ջ������Ϣ
					//?????????????????????????
					MMIPara.SOSConfirm=TRUE;//ȷ��sos

					UartResponseMsg("$P712,001,,,SOS Key Down!");
					MMIPara.MMICtrlSta = MMI_CTRLSTA_STEP2;
				}
			}
			else
			{
				RstMMIPara();
			}
#endif
		}
	}
	else
	{
#if !WIN32
		ucKeySta = GetIndivIO(KEY_SOS_ADDR);
		
		if (ucKeySta == KEY_HIGH)
		{
			//��ʾ����̧�𣬻ص���ʼ״̬
			RstMMIPara();
		}
#endif
	}
}

/*******************************************************************************
* ����  : GetMMISOSState
* ����  : ��ȡMMIģ��SOS�澯״̬
* ����  : ��		  
* ���  : ��
* ����  : TRUE��ʾ�澯����һ�κ�λ
*******************************************************************************/
U8 GetMMISOSState(void)
{
	if (MMIPara.SOSConfirm == TRUE)
	{
		MMIPara.SOSConfirm = FALSE;
		return (TRUE);		
	}
	else
	{
	 	return (FALSE);
	}
}

/*******************************************************************************
* ����  : GetMMISleepSta
* ����  : ��ȡMMIģ��ʡ��״̬
* ����  : ��		  
* ���  : ��
* ����  : TRUE��ʾ���Խ���ʡ�磬FALSE��ֹ����ʡ��
*******************************************************************************/
U8 GetMMISleepSta(void)
{
	if (MMIPara.MMICtrlSta == MMI_CTRLSTA_IDLE)
	{
		return (TRUE);
	}		
	else
	{			   
		return (FALSE);
	}
}
#endif

/*******************************************************************************
* ����  : InitMsgCtrlPara
* ����  : ��ʼ����Ϣ���Ʋ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitMsgCtrlPara(void)
{
	MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_POWERUP;
	MsgCtrlPara.GpsStartFrame = 0;
	MsgCtrlPara.GpsStartSlot = 0;
	MsgCtrlPara.GpsPowerup = FALSE;
	MsgCtrlPara.EnterHighFreq = FALSE;
	MsgCtrlPara.WaitCount = 0;
    MsgCtrlPara.slGpsOnSlot = 0;
}		

#if 0
/*******************************************************************************
* ����  : MsgSendErrorCheck
* ����  : ����Ƿ��ͳ�ʱ
* ����  : slotʱ϶ frame ֡		  
* ���  : ��
* ����  : TRUE ��ʱ FALSE ����
*******************************************************************************/
U8 MsgSendErrorCheck(S16 Slot,S16 Frame)
{
	if (Frame > eqEquipPara.SendErrorPeriod + MsgCtrlPara.GpsStartFrame) 
	{
		return (TRUE);
	}
	else if (Frame == eqEquipPara.SendErrorPeriod + MsgCtrlPara.GpsStartFrame)
	{
		if (Slot >= MsgCtrlPara.GpsStartSlot)
		{
			return (TRUE);
		}
	}
	else
	{
		return (FALSE);
	}
	
	return (FALSE);

}	
#endif

#if 0
/*******************************************************************************
* ����  : MsgCtrlProcess
* ����  : ��Ϣ������ģ��-SART
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void MsgCtrlProcess(void)                                                       //lq ��ģ������Ƿ���Է�����Ϣ�ˣ�������Ϣ�ķ���������ģ��ִ��
{
	S16 siTempSlot;
	S32 siTempFrame;
    S32 slSlotCurr;

	siTempSlot = GetSlotNum();//��ȡ���ڵ�ʱ��
	siTempFrame = GetFrameNum();
	slSlotCurr = GetSlotSum();

	if (MsgCtrlPara.MsgCtrlState == MSG_CTRL_STA_POWERUP)//�տ���
    {
        if (GetBurstStarFlg() == TRUE)
        {
            MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_SEND;                    
		    usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_SEND!\r\n", GetFrameNum(), GetSlotNum());	                            
        }                 
    }
    else if (MsgCtrlPara.MsgCtrlState == MSG_CTRL_STA_WAIT_SEND)
    {
        if (GetBurstStarFlg() == TRUE)
        {
            MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_SEND;                    
		    usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_SEND!\r\n", GetFrameNum(), GetSlotNum());	                            
        }            
    }
    else if (MsgCtrlPara.MsgCtrlState == MSG_CTRL_STA_SEND)
    {
        if (GetBurstFinsFlg() == TRUE)
        {
            /*lq �趨MCU���Ѷ�ʱ*/				
            SetMcuWakeupTim(SLOT_INTERVAL_MCU_WAKEUP_TO_TX);

            MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_IDLE;                    
		    usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_IDLE!\r\n", GetFrameNum(), GetSlotNum());            	                            
        }     
    }
    else if (MsgCtrlPara.MsgCtrlState == MSG_CTRL_STA_IDLE)
    {
        /*lq GPS������ʱ��Ч*/
        if (GetGpsOnTimEn() == TRUE)
        {
            /*lq GPS������ʱ��*/
            if (IsGpsOnTimeout() == TRUE)
            {
                MsgStartNew();

                /*lq ��λ����Ϣ���Ϳ�ʼ/������־*/  //lq [2013/08/06] GPS����5min�رպ�Ŀ�����ʱ���ڹرյ�ʱ�䣬����������δ��������
                ResetBurstFinsFlg();
                ResetBurstStarFlg();
            }
        }

        /*lq MCU���Ѷ�ʱ��*/                
        if (IsMcuWakeupTimeout() == TRUE)
        {
            /*lq ����MCU*/
            SleepCtrlPara.ucMcuWakeupFlg = TRUE;            
            
            /*lq ��λ����Ϣ���Ϳ�ʼ/������־*/
            ResetBurstFinsFlg();
            ResetBurstStarFlg();

            /*lq ���δ������Ѳ�����GPS��ֱ�ӽ���GPS�ر�״̬�����¸�����GPS������ʱ*/
            if (((GetGpsSaveFlg() == TRUE) || (GetGpsUnfix1hLngOpenFlg() == TRUE)
                || (GetGpsUnfixBey1hLngOpenFlg() == TRUE)) && (GetGpsOnEn() == FALSE))
            {
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;            
            }

            MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_WAIT_SEND;                    
		    usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_WAIT_SEND!\r\n", GetFrameNum(), GetSlotNum());	                            
        }
          
    }
    else
    {
        MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_POWERUP;
		usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_POWERUP!\r\n", GetFrameNum(), GetSlotNum());	                            
    }

}
#endif

/*******************************************************************************
* ����  : MsgCtrlProcess
* ����  : ��Ϣ������ģ��-AtoN
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void MsgCtrlProcess(void)                                                       //lq ��ģ������Ƿ���Է�����Ϣ�ˣ�������Ϣ�ķ���������ģ��ִ��
{
	S16 siTempSlot;
	S32 siTempFrame;
    S32 slSlotCurr;

	siTempSlot = GetSlotNum();//��ȡ���ڵ�ʱ��
	siTempFrame = GetFrameNum();
	slSlotCurr = GetSlotSum();

	if (MsgCtrlPara.MsgCtrlState == MSG_CTRL_STA_POWERUP)//�տ���
    {
        if (GetBurstStarFlg() == TRUE)
        {
            MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_SEND;                    
		    usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_SEND!\r\n", GetFrameNum(), GetSlotNum());	                            
        }                 
    }
    else if (MsgCtrlPara.MsgCtrlState == MSG_CTRL_STA_WAIT_SEND)
    {
        if (GetBurstStarFlg() == TRUE)
        {
            MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_SEND;                    
		    usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_SEND!\r\n", GetFrameNum(), GetSlotNum());	                            
        }            
    }
    else if (MsgCtrlPara.MsgCtrlState == MSG_CTRL_STA_SEND)
    {
        if (GetBurstFinsFlg() == TRUE)
        {
            /*lq �趨MCU���Ѷ�ʱ*/				
            SetMcuWakeupTim(SLOT_INTERVAL_MCU_WAKEUP_TO_TX);

            /*lq ��λGPS���ѱ�־*/
            ResetMsgGpsPowerup();       //lq ��ֹ��GPS����״̬�£�GPS���ѱ�־һֱ��Ч���ڳ�����ʱ��GPS���ߺ������������

            MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_IDLE;                    
		    usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_IDLE!\r\n", GetFrameNum(), GetSlotNum());            	                            
        }     
    }
    else if (MsgCtrlPara.MsgCtrlState == MSG_CTRL_STA_IDLE)
    {
        /*lq MCU���Ѷ�ʱ��*/
        if (IsMcuWakeupTimeout() == TRUE)
        {
            SleepCtrlPara.ucMcuWakeupFlg = TRUE;            

            MsgStartNew();

            /*lq ��λ����Ϣ���Ϳ�ʼ/������־*/
            ResetBurstFinsFlg();
            ResetBurstStarFlg();            

            MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_WAIT_SEND;                    
		    usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_WAIT_SEND!\r\n", GetFrameNum(), GetSlotNum());	                            
        }
    }
    else
    {
        MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_POWERUP;
		usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_POWERUP!\r\n", GetFrameNum(), GetSlotNum());	                            
    }

}

/*******************************************************************************
* ����  : GetMsgCtrlSleepState
* ����  : ��Ϣ����ģ���Ƿ�ɽ�������
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetMsgCtrlSleepState(void)
{
	if (MsgCtrlPara.MsgCtrlState == MSG_CTRL_STA_IDLE)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}

/*******************************************************************************
* ����  : MsgStartNew
* ����  : ��GPS������ϵͳ�����Ƶģʽ
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void MsgStartNew(void)
{
	MsgCtrlPara.GpsPowerup = TRUE;
	if (eqEquipPara.ucLowFreqSta == EQ_FREQSTA_LOW)//��������ڵ�Ƶģʽ
	{
		MsgCtrlPara.EnterHighFreq = TRUE;
	}

	//lq MsgCtrlPara.WaitCount = 0;

	ResetGpsNormalFixState();//��ֹ�ϴζԱ��ε�Ӱ�죬��Ϊģ�����ⲿ�������������ȼ��ߣ����ܵ�GPSģ�鸴λ
}

/*******************************************************************************
* ����  : ResetMsgGpsPowerup
* ����  : ��λ��GPS�ϵ��־λ,��GPS��ǰ���̿����ʱ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void ResetMsgGpsPowerup(void)
{
	MsgCtrlPara.GpsPowerup = FALSE;
}

/*******************************************************************************
* ����  : SetMsgGpsPowerup
* ����  : ��λ��GPS�ϵ��־λ
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetMsgGpsPowerup(void)
{
	MsgCtrlPara.GpsPowerup = TRUE;
}

/*******************************************************************************
* ����  : GetMsgGpsPowerup
* ����  : ��ȡGPS�ϵ��־λ
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetMsgGpsPowerup(void)
{
	if (MsgCtrlPara.GpsPowerup == TRUE)
	{
		MsgCtrlPara.GpsPowerup = FALSE;
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}

/*******************************************************************************
* ����  : GetMsgWakeupState
* ����  : ��ѯ�Ƿ�ɻ��� 
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetMsgWakeupState(void)
{
	if (MsgCtrlPara.EnterHighFreq == TRUE)
	{
		MsgCtrlPara.EnterHighFreq = FALSE;
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}
