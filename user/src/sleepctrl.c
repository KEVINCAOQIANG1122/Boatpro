/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: sleepctrl.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    ʡ�����ģ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include "config.h"
#include "def.h"
#include "sleepctrl.h"
#include "gpsctrl.h"
#include "systemtick.h"
#include "initial.h"
#include "powerdown.h"
#include "mmi.h"
#include "digunit.h"
#include "Tim.h"
#include "ledctrl.h"
#include "rtc.h"
#include "power.h"
#include "keyscan.h"
#include "LinkPtlPort.h"
#include "PTL_Main.h"
#include "debug.h"
#include "adjust.h"
#include "led.h"
#include "sotdma.h"
#include "MMI.h"
#include "bkpregister.h"
#include "gmskmod.h"
#include "parastorage.h"

SleepStruct		SleepCtrlPara;
U8 set_sleep_time_flag;//������������ʱ��
/*******************************************************************************
* ����  : IniSleepCtrlPara
* ����  : ��ʼ�����߹����Ҫ�Ĳ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void IniSleepCtrlPara(void)
{
	SleepCtrlPara.PowerSta = SLEEP_POWERSTA_POWERUP;

	SleepCtrlPara.EnterSleepSlot = 0;
	SleepCtrlPara.EnterSleepFrame = 0;		//����ʡ��֡

	SleepCtrlPara.WakeUpSlot = 0;		   	//�˳�ʡ��ʱ϶
	SleepCtrlPara.WakeUpFrame = 0;	   		//�˳�ʡ��֡

    SleepCtrlPara.TimeCnt = 0;
	SleepCtrlPara.TimePeriod = 0;

	SleepCtrlPara.TimeCnt1 = 0;
    SleepCtrlPara.TimePeriod1 = SLEEP_USART_FLAG_CLEAR_PERIOD;

	SleepCtrlPara.EnterSleepPrtTime = 0;	//����ʡ�籣��ʱ��
	
	SleepCtrlPara.SleepCtrlSta = SLEEP_STA_IDLE;
    
    SleepCtrlPara.ucMcuStopFlg = FALSE;
    SleepCtrlPara.ucMcuStandbyFlg = FALSE;
    
    SleepCtrlPara.ucGpsPulseFlg = FALSE; 
    SleepCtrlPara.ucMcuWakeupFlg = FALSE; 
    SleepCtrlPara.ucUsartFlg = FALSE; 
    SleepCtrlPara.ucStandbyAlarmSetFlg = FALSE;
    SleepCtrlPara.ucStandbyAlarmSetOk = FALSE; 	
    SleepCtrlPara.ucTimToRtcSwitch = FALSE;
    SleepCtrlPara.ucRtcToTimSwitch = FALSE;
}
#if 0
/*******************************************************************************
* ����  : SleepCtrlHigFreq
* ����  : ��������״̬��ʡ����ƣ�4501ģʽ�²�ʡ�磬4503ģʽ��ʡ�磬Ҫ����
*		  ����״̬��ʡ�����ڽ��п���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SleepCtrlHigFreq(void)
{
	U8 ucGpsSta;
	U8 ucDigUnitSta;
	U8 ucMMISta;
	U8 ucPowerDownSta;
	U8 ucPtlFifoSta;
	//U8 ucPtlSta; //�����Э��ջ״̬�жϺ��������ڷ���Э��ջ��״̬
	U8 ucMsgCtrlSta;
	U8 ucAdjustSta;
	U8 ucChk;
//	U8 ucEnterSleep;

	//ֻ����4503ģʽ�½���ʡ����ز���
	if (eqEquipPara.ucPowerSaveMode == EQ_MOD_POWERSAVE)	
	{
		ucGpsSta = GetGpsSleepSta();
		ucDigUnitSta = GetDigUnitSleepSta();
		ucMMISta = GetMMISleepSta();
		ucPowerDownSta = GetPowerDownSleepSta();//�����ʱû�йػ�����
		ucPtlFifoSta = GetPtlFifoSleepSta();//�����Ϣ����������
		
		ucMsgCtrlSta = GetMsgCtrlSleepState();		
		ucAdjustSta = GetAdjustSleepState();//ʱ�����ģ���Ƿ������
        //ucPtlSta=Is_4503_Idle();		

		ucChk = (ucGpsSta == TRUE)
				 & (ucDigUnitSta == TRUE)
				 & (ucMMISta == TRUE)
				 & (ucPowerDownSta == TRUE)
				 & (ucPtlFifoSta == TRUE)
				 & (ucMsgCtrlSta == TRUE)
				 & (ucAdjustSta == TRUE);
				 //& (ucPtlSta == TRUE)//Э��ջ 
		
//		ucChk = TRUE;

		if (SleepCtrlPara.SleepCtrlSta == SLEEP_STA_IDLE)
		{
			//����ʡ�綨ʱ��
			if (ucChk == TRUE)
			{
				UartResponseMsg("$P712,sleep state true");
				SleepCtrlPara.TimePeriod = SLEEP_PRT_PERIOD;
				SleepCtrlPara.TimeCnt = SetTimePeriodProcess(SleepCtrlPara.TimePeriod);
				SleepCtrlPara.EnterSleepPrtTime = 0;
				SleepCtrlPara.SleepCtrlSta = SLEEP_STA_STEP1;					
			}
		}
		else if (SleepCtrlPara.SleepCtrlSta == SLEEP_STA_STEP1)
		{
			if (ucChk == FALSE)
			{
				SleepCtrlPara.SleepCtrlSta = SLEEP_STA_IDLE; 	
			}
			else
			{
				//����ط�Ӧ�������ж� SleepCtrlPara.TimeCnt==GetLocalClockTickProcess()
				if (SleepCtrlPara.TimeCnt==GetLocalClockTickProcess())
				{
					SleepCtrlPara.TimeCnt = SetTimePeriodProcess(SleepCtrlPara.TimePeriod);//�����ʱ10ms������
					SleepCtrlPara.EnterSleepPrtTime++;
				}
				//����1s��������
				if (SleepCtrlPara.EnterSleepPrtTime >= SLEEP_1S_PRTTIME)
				{
					//���뵽ʡ��״̬
					eqEquipPara.ucLowFreqSta = EQ_FREQSTA_LOW;
					
					UartResponseMsg("SLEEP");
					//�ر�LED��
					LEDInit(LED_TYPE_NONE);
					//sleep_4503();
//lq                    TEST_OUT_LOW();   //lq ���ص�Դ��Ϊ������ģʽ
					SleepCtrlPara.SleepCtrlSta = SLEEP_STA_IDLE;
					ResetRtcCount();
							
				}		
			}	
		}				
	}
}
#endif

S32 gslTimToRtcCnt_2 = 0;                                                                
S32 gslRtcToTimCnt_2 = 0; 
S32 gslTimToRtcIdleCnt_2 = 0;                                                                
 
#if 0
/*******************************************************************************
* ����  : SleepCtrlHigFreq
* ����  : ��������״̬��ʡ����ƣ�4501ģʽ�²�ʡ�磬4503ģʽ��ʡ�磬Ҫ����
*		  ����״̬��ʡ�����ڽ��п���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SleepCtrlHigFreq(void)
{
	U8 ucGpsSta;
	U8 ucDigUnitSta;
	U8 ucMMISta;
	U8 ucPowerDownSta;
	U8 ucPtlFifoSta;
	U8 ucMsgCtrlSta;
	U8 ucAdjustSta;
	U8 ucChk;
    U8 ucFSKDataXmtSta;
    U8 ucGpsPowerSta;
    U8 ucBurstStartFlg;
    U8 ucBurstFinisFlg;
    U8 ucUbxGpsCommSta;
    U8 ucLB1CommSta;
    U8 ucUsartRecSta;

    /*lq ��ʱ����USART���ѱ�־ */
    if (GetUsartFlg() == TRUE)
    {
	    if (SleepCtrlPara.TimeCnt1 == GetLocalClockTickProcess())
        {
            ResetUsartFlg();
        }                   
    }

	/*lq ��������*/
    ucGpsSta = GetGpsSleepSta();
	ucDigUnitSta = GetDigUnitSleepSta();
#if CONFIG_KEY
	ucMMISta = GetMMISleepSta();
#endif
#if CONFIG_POWERDOWN
	ucPowerDownSta = GetPowerDownSleepSta();                                    //�����ʱû�йػ�����
#endif
	ucPtlFifoSta = GetPtlFifoSleepSta();                                        //�����Ϣ����������
	ucMsgCtrlSta = GetMsgCtrlSleepState();		
	ucAdjustSta = GetAdjustSleepState();                                        //ʱ�����ģ���Ƿ������
    ucFSKDataXmtSta = GetFSKDataXmtSleepState();                                //�������ݷ���ģ��				
    ucUbxGpsCommSta = UBX1_GetGpsCommSleepSta();                                //�Ƿ�������GPSͨ��
#if 0
    ucLB1CommSta = LB1_GetLBCommSleepSta();                                     //�Ƿ�������Light Beaconͨ��
#else
    ucLB1CommSta = TRUE;
#endif
    ucUsartRecSta = GetUsartFlg(); 
#if 0
    ucGpsPowerSta = GetGpsPowerSta();                                           //lq ��standbyģʽ�»����轫GPS�Ĺ���״̬��Ϊ����
#endif
	ucChk = (ucGpsSta == TRUE)
			 & (ucDigUnitSta == TRUE)
#if CONFIG_KEY
			 & (ucMMISta == TRUE)
#endif
#if CONFIG_POWERDOWN
			 & (ucPowerDownSta == TRUE)
#endif
			 & (ucPtlFifoSta == TRUE)
			 & (ucMsgCtrlSta == TRUE)
			 & (ucAdjustSta == TRUE)
             & (ucFSKDataXmtSta == TRUE
             & (ucUbxGpsCommSta == TRUE)
             & (ucLB1CommSta == TRUE)
             & (ucUsartRecSta == FALSE));

	if (SleepCtrlPara.SleepCtrlSta == SLEEP_STA_IDLE)
	{
		//����ʡ�綨ʱ��
		if (ucChk == TRUE)
		{
			UartResponseMsg("$Sleep Conditions True!");
			SleepCtrlPara.TimePeriod = SLEEP_PRT_PERIOD;
			SleepCtrlPara.TimeCnt = SetTimePeriodProcess(SleepCtrlPara.TimePeriod);
			SleepCtrlPara.EnterSleepPrtTime = 0;
			SleepCtrlPara.SleepCtrlSta = SLEEP_STA_STEP1;					
		}
	}
	else if (SleepCtrlPara.SleepCtrlSta == SLEEP_STA_STEP1)
	{
		if (ucChk == FALSE)
		{
			SleepCtrlPara.SleepCtrlSta = SLEEP_STA_IDLE; 
			SleepCtrlPara.EnterSleepPrtTime = 0;            	
		}
		else
		{
			if (SleepCtrlPara.TimeCnt == GetLocalClockTickProcess())
			{
				SleepCtrlPara.TimeCnt = SetTimePeriodProcess(SleepCtrlPara.TimePeriod);//�����ʱ10ms������
				SleepCtrlPara.EnterSleepPrtTime++;
			}

			if (SleepCtrlPara.EnterSleepPrtTime >= SLEEP_1S_PRTTIME)			//����1s��������
			{
                #if 0
                    /*lq ����GPS�ɹ���λ������GPS����3�β���λ�Ĵ�����GPS5min����������EEPROM */
                	sprintf((char *)ucTemp, "%5d", GpsCtrlPara.GpsFixCount);
                   	Para_Save(PARA_GPS_FIX_COUNT_TYPE, ucTemp, 5);
                
                	sprintf((char *)ucTemp, "%3d", GpsCtrlPara.GpsWaitCount);
                   	Para_Save(PARA_GPS_WAIT_COUNT_TYPE, ucTemp, 3);
                
                	sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
                   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
                
                    /*lq ÿ8min����һ�ιػ�ʱ��*/
                    if (struSOCommState.ucSlotTimeout == 7)
                    {
                    	if (GetGpsNormalFixState() == TRUE)                                //lq ֻ�б��ζ�λ�ɹ��󣬲Ż��¼�ػ�ʱ��
                    	{
                    		/*lq ��¼GPS���ں�ʱ��*/
                            sprintf((char *)ucTemp, "%2d", GpsPara.GPS_TimeMon);
                    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeD);
                    		strcat((char *)ucTemp, (char *)ucTemp1);
                            sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeH);
                    		strcat((char *)ucTemp, (char *)ucTemp1);
                    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeM);
                    		strcat((char *)ucTemp, (char *)ucTemp1);
                        	Para_Save(PARA_TIME_CLOSE_TYPE, ucTemp, PARA_TIME_CLOSE_MAXLEN);                           //lq [2013/04/16]
                    		
#if CONFIG_PARASTORAGE_BTT                
                            /*lq ��¼����֡�����Ĺ���ʱ��*/
                			sprintf((char *)ucTemp,"%5d",GetFrameNum());
                			Para_Save(PARA_BATTERY_TEST_TIME_TYPE,ucTemp,5);
#endif                    	
                        } 
                    }
                #endif   
#if 0                          
                /*��λ������Tim��ʱתΪRTC��ʱ*/
                SleepCtrlPara.ucTimToRtcSwitch = TRUE; //lq SleepCtrlPara.ucTimToRtcSwitch����eqEquipPara.ucLowFreqSta֮ǰ���ã���ֹ��ʱ�ж��ڶ���֮�䴥��
                				
                /*���뵽ʡ��״̬*/
				eqEquipPara.ucLowFreqSta = EQ_FREQSTA_LOW;
#endif
                /*lq ��λ����Ϣ���Ϳ�ʼ/������־*/
                ResetBurstFinsFlg();
                ResetBurstStarFlg();
				
				LEDInit(LED_TYPE_NONE); //�ر�LED��

				SleepCtrlPara.SleepCtrlSta = SLEEP_STA_IDLE;
                SleepCtrlPara.ucMcuWakeupFlg = FALSE;

                /*lq ��GPS�ѹرգ������Standbyģʽ��GPS����״̬�£�����StandbyģʽGPS����磩*/
#if 0
                if (GetGpsPowerSta() == FALSE)  //lq SART
#else
                if (0)                      //lq AtoN����ģʽ�£�MCU������standby
#endif
                {
                    /*lq �����������ʱ��*/
                    // struRtcPara.ssRtcAlarmValue = CalcuSleepSpan();
                    // usart_printf("ssAlarmValue : %ld\r\n",struRtcPara.ssRtcAlarmValue);				                    
                    // ResetRtcCount(struRtcPara.ssRtcAlarmValue);//lq ���߶�ʱ
#if 0                
                    gslTimToRtcCnt_2 = 0;
                    gslTimToRtcIdleCnt_2 = 0;
                    TEST_OUT_LOW_2();                               

#endif 
		            SleepCtrlPara.ucStandbyAlarmSetFlg = TRUE; 	
#if 1                          
                    /*��λ������Tim��ʱתΪRTC��ʱ*/
                    SleepCtrlPara.ucTimToRtcSwitch = TRUE; //lq SleepCtrlPara.ucTimToRtcSwitch����eqEquipPara.ucLowFreqSta֮ǰ���ã���ֹ��ʱ�ж��ڶ���֮�䴥��
                    				
                    /*���뵽ʡ��״̬*/
    				eqEquipPara.ucLowFreqSta = EQ_FREQSTA_LOW;
#endif
  
                    // usart_printf("[%6d:%4d]$MCU WorkMode : Standby!\r\n", GetFrameNum(), GetSlotNum());	                
                    
                    while(SleepCtrlPara.ucStandbyAlarmSetFlg == TRUE)
                    {
                       // Feed_Dog();
                    }

                    /*lq �������ģʽ*/
                    EnterStandbyMod();
                }
                else
                {
                    // usart_printf("[%6d:%4d]$MCU WorkMode : Stop!\r\n", GetFrameNum(), GetSlotNum());	
#if 0                
                    gslTimToRtcCnt_2 = 0;
                    gslTimToRtcIdleCnt_2 = 0;
                    TEST_OUT_LOW_2();                               

#endif 
#if 1                          
                    /*��λ������Tim��ʱתΪRTC��ʱ*/
                    SleepCtrlPara.ucTimToRtcSwitch = TRUE; //lq SleepCtrlPara.ucTimToRtcSwitch����eqEquipPara.ucLowFreqSta֮ǰ���ã���ֹ��ʱ�ж��ڶ���֮�䴥��
                    				
                    /*���뵽ʡ��״̬*/
    				eqEquipPara.ucLowFreqSta = EQ_FREQSTA_LOW;
#endif                                                    
 
                }	
    		}		
		}	
	}				
}
#endif

#if 0
/*******************************************************************************
* ����  : SleepCtrlLowFreq
* ����  : ʡ��״̬��ʡ�����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
extern void EnterStopMod(void);
extern void Tim2NVIC_Close(void);
extern void Tim2NVIC_Open(void);
void SleepCtrlLowFreq(void)
{	
	U8  ucSosKeyVal;
	U8  ucPowerKeyVal;
    U8  ucGpsPowerSta;
    U8  ucMsgWakeupFlg;
    U8  ucMcuWakeupFlg;
    U8  ucWakeupChkFlg;
    U8  ucUsartWakeupFlg;

    while(SleepCtrlPara.ucTimToRtcSwitch == TRUE);

	EnterStopMod();                                     //������ǽ���clock stop mode

	/*lq ��ѯ�͹���ģʽת�빤��ģʽ�������Ƿ����*/
#if CONFIG_KEY
    ucPowerKeyVal = ReadSingleKeyValProcess(&PowerKey); //lq ��Դ����
	ucSosKeyVal = ReadSingleKeyValProcess(&SOSKey);     //lq Sos����
#endif
#if 0
    ucMsgWakeupFlg = GetMsgWakeupState();               //lq GPS���Ѷ�ʱ��
#endif    
    ucMcuWakeupFlg = SleepCtrlPara.ucMcuWakeupFlg;      //lq MCU���Ѷ�ʱ��
    ucUsartWakeupFlg = GetUsartFlg();                   //lq USART���ѣ����Ѻ���Ҫ���500ms���Ҳ�����ȷ���յ�һ����Ϣ��

    ucWakeupChkFlg = (ucMcuWakeupFlg == TRUE)
#if CONFIG_KEY     
                     || (ucSosKeyVal == KEY_LOW)
                     || (ucPowerKeyVal == KEY_LOW)
#endif
#if 0                     
                     || (ucMsgWakeupFlg == TRUE) 
#endif     
                     
                     || (ucUsartWakeupFlg == TRUE);

	if (ucWakeupChkFlg)
	{
#if 0	
    	if (SleepCtrlPara.ucGpsPulseFlg == TRUE)
        {
            SleepCtrlPara.ucGpsPulseFlg = FALSE; 
		    usart_printf("[%6d:%4d]$MCU Wakeup by GpsPulse!\r\n", GetFrameNum(), GetSlotNum());	                
        }
#endif
#if 0
        if (ucMcuWakeupFlg == TRUE)
        { 
		    usart_printf("[%6d:%4d]$MCU Wakeup by McuWakeupTim!\r\n", GetFrameNum(), GetSlotNum());	                
        }
#if CONFIG_KEY
        if (ucSosKeyVal == KEY_LOW)
        {
		    usart_printf("[%6d:%4d]$MCU Wakeup by SosKey!\r\n", GetFrameNum(), GetSlotNum());	                
        }

        if (ucPowerKeyVal == KEY_LOW)
        {
		    usart_printf("[%6d:%4d]$MCU Wakeup by PowerKey!\r\n", GetFrameNum(), GetSlotNum());	                
        }
#endif
        if (ucMsgWakeupFlg == TRUE)
        {
		    usart_printf("[%6d:%4d]$MCU Wakeup by MsgWakeupForGpsOn!\r\n", GetFrameNum(), GetSlotNum());	                
        }
#endif                
#if 1
        gslRtcToTimCnt_2 = 0;
#endif

        /*��λ������RTC��ʱתΪTim��ʱ*/
        SleepCtrlPara.ucRtcToTimSwitch = TRUE;         //lq SleepCtrlPara.ucRtcToTimSwitch����eqEquipPara.ucLowFreqSta֮ǰ���ã���ֹ��ʱ�ж��ڶ���֮�䴥��

        eqEquipPara.ucLowFreqSta = EQ_FREQSTA_HIG;

		// usart_printf("[%6d:%4d]$MCU WorkMode : Running!\r\n", GetFrameNum(), GetSlotNum());	                
	}		
}
#endif

/*******************************************************************************
* ����  : SaveBKP_DRPara
* ����  : ���汸�ݼĴ�������
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SaveBKP_DRPara(void)
{
#if 0
    /*lq ��־����*/
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_LNG_OPEN_FIRST_HOUR_INDEX,
                    GpsCtrlPara.ucGpsUnfix1hLngOpenFlg);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_LNG_OPEN_BEY_FIRST_HOUR_INDEX,
                    GpsCtrlPara.ucGpsUnfixBey1hLngOpenFlg);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_5MIN_LONG_OPEN_EN_INDEX,
                    GpsCtrlPara.ucGps5minLngOpenEn);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_POWER_UP_FIX_INDEX,
                    GpsCtrlPara.GpsPowerupFix);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_ON_TIM_EN_INDEX,
                    GpsCtrlPara.ucGpsOnTimEn);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_OFF_TIM_EN_INDEX,
                    GpsCtrlPara.ucGpsOffTimEn);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_SAVE_INDEX,
                    GpsCtrlPara.ucGpsSaveFlg);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_ON_INDEX,
                    GpsCtrlPara.ucGpsOnEn);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_LEAP_SECOND_INDEX,
                    GpsPara.GPS_LeapSecondFlg);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_RTC_LSE_OR_LSI_INDEX,
                    struRtcPara.ucRtcLSEorLSI);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_RTC_TIM_PHASE_INDEX,
                    struRtcTimPhase1.RtcTimPhaseFlg);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_SYNCH_ENABLE_INDEX,
                    GpsSynCell.GPSSyn_Enable);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_EQ_WORKMOD_INDEX,
                    eqEquipPara.ucWorkMode);                    

    arrBKPDRData[BKPR_GPS_LATITUDE_H_INDEX] = (GpsPara.GPS_Latitude >> 16) & 0xFFFF;
    arrBKPDRData[BKPR_GPS_LATITUDE_L_INDEX] = GpsPara.GPS_Latitude & 0xFFFF;
    arrBKPDRData[BKPR_GPS_LONGITUDE_H_INDEX] = (GpsPara.GPS_Longitud >> 16) & 0xFFFF;;                   
    arrBKPDRData[BKPR_GPS_LONGITUDE_L_INDEX] = GpsPara.GPS_Longitud & 0xFFFF;
    arrBKPDRData[BKPR_GPS_SOG_INDEX] = GpsPara.GPS_Sog;
    arrBKPDRData[BKPR_GPS_COG_INDEX] = GpsPara.GPS_Cog;
    //arrBKPDRData[BKPR_GPS_TIME_HOUR_INDEX] = GpsPara.GPS_TimeH;
    //arrBKPDRData[BKPR_GPS_TIME_MIN_INDEX] = GpsPara.GPS_TimeM;

    arrBKPDRData[BKPR_GPS_FALL_COUNT_INDEX] = GpsCtrlPara.GpsFallCount;
    arrBKPDRData[BKPR_GPS_CTRL_STA_INDEX] = GpsCtrlPara.GpsCtrlSta; 
    arrBKPDRData[BKPR_GPS_UNFIX_1H_PERIOD_TIM_FRAME_H_INDEX] = (GpsCtrlPara.GpsUnfix1hPeriodTimFrame >> 16) & 0xFFFF;
    arrBKPDRData[BKPR_GPS_UNFIX_1H_PERIOD_TIM_FRAME_L_INDEX] = GpsCtrlPara.GpsUnfix1hPeriodTimFrame & 0xFFFF;
    arrBKPDRData[BKPR_GPS_UNFIX_1H_PERIOD_TIM_SLOT_INDEX] = GpsCtrlPara.GpsUnfix1hPeriodTimSlot;
    arrBKPDRData[BKPR_GPS_START_FRAME_H_INDEX] = (GpsCtrlPara.GpsOnTimFrame >> 16) & 0xFFFF;
    arrBKPDRData[BKPR_GPS_START_FRAME_L_INDEX] = GpsCtrlPara.GpsOnTimFrame & 0xFFFF;
    arrBKPDRData[BKPR_GPS_START_SLOT_INDEX] = GpsCtrlPara.GpsOnTimSlot;
    arrBKPDRData[BKPR_GPS_OFF_FRAME_H_INDEX] = (GpsCtrlPara.GpsOffTimFrame >> 16) & 0xFFFF;
    arrBKPDRData[BKPR_GPS_OFF_FRAME_L_INDEX] = GpsCtrlPara.GpsOffTimFrame & 0xFFFF;
    arrBKPDRData[BKPR_GPS_OFF_SLOT_INDEX] = GpsCtrlPara.GpsOffTimSlot;                                  
#if 0
    arrBKPDRData[BKPR_BURST_TX_TIM_FRAME_H_INDEX] = (struSOTDMAPara.BurstTxTimFrame >> 16) & 0xFFFF;
    arrBKPDRData[BKPR_BURST_TX_TIM_FRAME_L_INDEX] = struSOTDMAPara.BurstTxTimFrame & 0xFFFF;
    arrBKPDRData[BKPR_BURST_TX_TIM_SLOT_INDEX] = struSOTDMAPara.BurstTxTimSlot; 
#endif
#if 0
    arrBKPDRData[BKPR_SLOT_TIMEOUT_INDEX] = struSOCommState.ucSlotTimeout;
    arrBKPDRData[BKPR_SLOT_OFFSET_INDEX] = struSOCommState.usSlotOffset;
#endif
    arrBKPDRData[BKPR_RTC_TIM_PHASE_H_INDEX] = (struRtcTimPhase1.RtcTimPhaseNum >> 16) & 0xFFFF;
    arrBKPDRData[BKPR_RTC_TIM_PHASE_L_INDEX] = struRtcTimPhase1.RtcTimPhaseNum & 0xFFFF;

    arrBKPDRData[BKPR_RTC_FREQUENCE_INDEX] = struRtcPara.usRtcFreq;
    arrBKPDRData[BKPR_RTC_ALARM_MCU_OR_GPS_INDEX] = struRtcPara.ucRtcAlarmMcuOrGps;
    // arrBKPDRData[BKPR_RTC_ALARM_VALUE_INDEX] = struRtcPara.ssRtcAlarmValue;
    arrBKPDRData[BKPR_RTC_ALARM_VALUE_H_INDEX] = (struRtcPara.ssRtcAlarmValue >> 16) & 0xFFFF;
    arrBKPDRData[BKPR_RTC_ALARM_VALUE_L_INDEX] = struRtcPara.ssRtcAlarmValue & 0xFFFF;

    arrBKPDRData[BKPR_FRAME_CNT_H_INDEX] = (gslFrameCnt >> 16) & 0xFFFF;
    arrBKPDRData[BKPR_FRAME_CNT_L_INDEX] = gslFrameCnt & 0xFFFF;
	arrBKPDRData[BKPR_SLOT_CNT_INDEX] = gsiSlotCnt;
	arrBKPDRData[BKPR_PHASE_CNT_INDEX] = gsiPhaseCnt;

    arrBKPDRData[BKPR_TIM_ADJUST_1_INDEX] = TimAdjust1;
    arrBKPDRData[BKPR_TIM_ADJUST_10_INDEX] = TimAdjust10; 
    arrBKPDRData[BKPR_TIM_ADJUST_100_INDEX] = TimAdjust100;

    arrBKPDRData[BKPR_TIM_ADJUST_PERIOD_CNT_LAST_INDEX] = TimAdjustPara.PeriodCountLast; 

    // arrBKPDRData[BKPR_RTC_ADJUST_1_INDEX] = RtcPhaseAdjust1;
    // arrBKPDRData[BKPR_RTC_ADJUST_10_INDEX] = RtcPhaseAdjust10;
    // arrBKPDRData[BKPR_RTC_ADJUST_100_INDEX] = RtcPhaseAdjust100;
    arrBKPDRData[BKPR_GPS_INTERVAL_CNT_INDEX] = GpsCtrlPara.GpsIntervalCnt;

    arrBKPDRData[BKPR_RAND_SEED_H_INDEX] = (send_seed.seed >> 16) & 0xFFFF;
    arrBKPDRData[BKPR_RAND_SEED_L_INDEX] = send_seed.seed & 0xFFFF;
#endif
}

#if 0
/*******************************************************************************
* ����  : GetSleepSpan
* ����  : ��������ʱ��������ʱ϶�
* ����  : ��		  
* ���  : ��
* ����  : RTC��ʱ����ֵ
*******************************************************************************/
U32 CalcuSleepSpan(void)
{
    S32 slCurrSlotSum;
    S32 slGpsOnslot;
    S32 slSlotDiff;
    S32 slAlarmValue;

	if (IsRtcClockSourceLSE() == TRUE)
	{
		//LSE
		struRtcPara.usRtcFreq = RTC_LSE_FREQ_VALUE;
//lq        struRtcPara.usRtcFreq = 64;
	}
	else
	{
		//LSI
		struRtcPara.usRtcFreq = RTC_LSI_FREQ_VALUE;
//lq        struRtcPara.usRtcFreq = 100;
	}
    
    slSlotDiff = CalcuSleepSlots_Sleep();
    if (slSlotDiff < 0)
    {   
       slSlotDiff = 0; 
    }
    usart_printf("slSlotDiff : %ld\r\n",slSlotDiff);
	usart_printf("slRTCFreq : %d\r\n",struRtcPara.usRtcFreq);

    slAlarmValue = slSlotDiff * struRtcPara.usRtcFreq / 37.5 + 0.5;

    return slAlarmValue;
}

#else
/*******************************************************************************
* ����  : GetSleepSpan
* ����  : ��������ʱ����������λ�
* ����  : ��		  
* ���  : ��
* ����  : RTC��ʱ����ֵ
*******************************************************************************/
U32 CalcuSleepSpan(void)
{
    S32 slCurrSlotSum;
    S32 slGpsOnslot;
    S32 slPhaseDiff;
    S32 slAlarmValue;


	if (IsRtcClockSourceLSE() == TRUE)
	{
		//LSE
		struRtcPara.usRtcFreq = RTC_LSE_FREQ_VALUE;
//lq        struRtcPara.usRtcFreq = 64;
	}
	else
	{
		//LSI
		struRtcPara.usRtcFreq = RTC_LSI_FREQ_VALUE;
//lq        struRtcPara.usRtcFreq = 100;
	}
   
    slPhaseDiff = CalcuSleepPhases_Sleep();
    if (slPhaseDiff < 0)
    {   
       slPhaseDiff = 0; 
    }
    // usart_printf("slPhaseDiff : %ld\r\n", slPhaseDiff);
	// usart_printf("slRTCFreq : %d\r\n", struRtcPara.usRtcFreq);
#if 0
    slAlarmValue = slPhaseDiff * struRtcPara.usRtcFreq / 48000 + 0.5;
#else
    slAlarmValue = (double)slPhaseDiff * ((double)struRtcPara.usRtcFreq / 48000.0) + 1;    //lq +1ȷ����ʱ���ѵ�ʱ�̴��ڶ�ʱ��ʱ�̷�ֹ���Ѻ�δ�������Ļ���ʱ���ٴν�������

    slAlarmValue = ((slAlarmValue >> 2) + 1) << 2;  //lq ��RTC��alarm��ʱֵת��Ϊ��С����ԭʼֵ��4�ı������Լ�С�������Ѻ�ת��Ϊ��λʱ�����
#endif
    return slAlarmValue;
}

#endif

/*******************************************************************************
* ����  : CalcuSleepSlots_Sleep
* ����  : MCU���Ѷ�ʱ�Ƿ�ʱ
* ����  : ��		  
* ���  : ��
* ����  : TRUE-��ʱ��FALSE-δ��ʱ
*******************************************************************************/
S32 CalcuSleepSlots_Sleep(void)
{
    S32 slGpsWakeupSlot;
    S32 slMcuWakeupSlot;
    S32 slSleepSlot;
    S32 slCurrSlot;

    /*lq ��ȡ��ǰʱ϶*/
    slCurrSlot = GetSlotSum();

    /*lq ��ȡMCU���Ѷ�ʱ*/
    slMcuWakeupSlot = GetMcuWakeupTim();

    /*lq ��ȡGPS������ʱ*/
    if (GetGpsOnTimEn() == TRUE)
    {
        slGpsWakeupSlot = GetGpsOnTim();    
    }
    else
    {
        slGpsWakeupSlot = 0;
    }

    /*lq ʹ��MCU���Ѷ�ʱ��MCU�Ӵ����л���*/
    if (slMcuWakeupSlot <= slGpsWakeupSlot)
    {
       slSleepSlot = slMcuWakeupSlot - slCurrSlot;

       struRtcPara.ucRtcAlarmMcuOrGps = RTC_ALARM_MCU;
    }
    else
    {
       slSleepSlot = slGpsWakeupSlot - slCurrSlot;

       struRtcPara.ucRtcAlarmMcuOrGps = RTC_ALARM_GPS;    
    }

    return slSleepSlot;
} 

/*******************************************************************************
* ����  : CalcuSleepPhases_Sleep
* ����  : �������ߵ�ʱ϶��
* ����  : ��		  
* ���  : ��
* ����  : TRUE-��ʱ��FALSE-δ��ʱ
*******************************************************************************/
S32 CalcuSleepPhases_Sleep(void)
{
    S32 slGpsWakeupSlot;
    S32 slMcuWakeupSlot;
    S32 slSleepSlot;
    S32 slCurrSlot;

    /*lq ��ȡ��ǰʱ϶*/
    slCurrSlot = GetSlotSum();

    /*lq ��ȡMCU���Ѷ�ʱ*/
    slMcuWakeupSlot = GetMcuWakeupTim();

    /*lq ��ȡGPS������ʱ*/
    if (GetGpsOnTimEn() == TRUE)
    {
        slGpsWakeupSlot = GetGpsOnTim();    
    }
    else
    {
        //lq slGpsWakeupSlot = 0;
        slGpsWakeupSlot = 0x7FFFFFFF;
    }

    /*lq ʹ��MCU���Ѷ�ʱ��MCU�Ӵ����л���*/
    if (slMcuWakeupSlot <= slGpsWakeupSlot)
    {
       slSleepSlot = slMcuWakeupSlot - slCurrSlot;

       struRtcPara.ucRtcAlarmMcuOrGps = RTC_ALARM_MCU;
    }
    else
    {
       slSleepSlot = slGpsWakeupSlot - slCurrSlot;

       struRtcPara.ucRtcAlarmMcuOrGps = RTC_ALARM_GPS;    
    }

    return slSleepSlot * 1280 - GetPhaseNum();              //lq ���ش�����λ��������ʱ϶���������
} 

/*******************************************************************************
* ����  : IsMcuWakeupTimeout
* ����  : MCU���Ѷ�ʱ�Ƿ�ʱ
* ����  : ��		  
* ���  : ��
* ����  : TRUE-��ʱ��FALSE-δ��ʱ
*******************************************************************************/
U8 IsMcuWakeupTimeout(void)
{
    if (GetSlotSum() >= GetMcuWakeupTim())
    {
        return TRUE;
    }    
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : GetMcuWakeupTim
* ����  : ��ȡMCU��ʱ
* ����  : ��		  
* ���  : ��
* ����  : MCU��ʱ
*******************************************************************************/
S32 GetMcuWakeupTim(void)
{
    return SleepCtrlPara.WakeUpSlot + 2250 * SleepCtrlPara.WakeUpFrame;
}

/*******************************************************************************
* ����  : SetMcuWakeupSlot
* ����  : ����MCU����ʱ϶
* ����  : value - MCU����ʱ϶		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetMcuWakeupSlot(S16 value)
{
   if (value < 0)
   {
        value = GetSlotNum();
   }
   
   SleepCtrlPara.WakeUpSlot = value;
}

/*******************************************************************************
* ����  : SetMcuWakeupFrame
* ����  : ����MCU����֡
* ����  : value - MCU����֡		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetMcuWakeupFrame(S32 value)
{
   if (value < 0)
   {
        value = GetFrameNum() + 1;
   }
   
   SleepCtrlPara.WakeUpFrame = value;
}
#if 0
/*******************************************************************************
* Function Name  : SetMcuWakeupTim
* Description    : ����MCU���Ѷ�ʱ
* Input          : usValue - MCU���Ѷ�ʱ������Ϣ�����ʱ϶ƫ��
* Output         : None
* Return         : None
*******************************************************************************/
void SetMcuWakeupTim(S16 usValue)
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

    SetMcuWakeupFrame(slSlotNums / 2250);
    SetMcuWakeupSlot(slSlotNums % 2250);
}
#endif
/*******************************************************************************
* Function Name  : SetUsartFlg
* Description    : ����Usart���ѱ�־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetUsartFlg(void)
{
    SleepCtrlPara.ucUsartFlg = TRUE;

    SleepCtrlPara.TimeCnt1 = SetTimePeriodProcess(SleepCtrlPara.TimePeriod1);
}

/*******************************************************************************
* Function Name  : ResetUsartFlg
* Description    : ��λUsart���ѱ�־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetUsartFlg(void)
{
    SleepCtrlPara.ucUsartFlg = FALSE;
}

/*******************************************************************************
* Function Name  : GetUsartFlg
* Description    : ��ѯUsart���ѱ�־
* Input          : None
* Output         : None
* Return         : Usart���ѱ�־
*******************************************************************************/
U8 GetUsartFlg(void)
{
    return SleepCtrlPara.ucUsartFlg;
}

/*******************************************************************************
* Function Name  : GetUsartFlgRst
* Description    : ��ѯUsart���ѱ�־
* Input          : None
* Output         : None
* Return         : Usart���ѱ�־
*******************************************************************************/
U8 GetUsartFlgRst(void)
{
    if (SleepCtrlPara.ucUsartFlg == TRUE)
    {
        SleepCtrlPara.ucUsartFlg = FALSE;    
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
