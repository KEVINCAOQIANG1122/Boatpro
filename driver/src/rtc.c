/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: rtc.c
  Author: vozhj      Version : V1.00        Date: 2011.10.18
  Description:    rtc��ʱ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>

*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "def.h"
#include "rtc.h"
#include "Tim.h"
#include "initial.h"
#include "power.h"
#include "LED.h"
#include "gpsctrl.h"
#include "adjust.h"
#include "wdg.h"
#include "debug.h"
#include "powerup.h"
#include "BKPRegister.h"
#include "sleepctrl.h"


extern U32 GpsTimCount;
extern U8 TestIO1Flag;
extern U8 TestIOFlag;
U32 RTCIntCount;//���ܵȳ���ʱ���������֤һ�����ڼӵ�phase����ȷ 
U32 GpsRtcCount;
U32 RtcSubCnt1;
U32 RtcSubCnt2;
U32 RtcSubCnt3;
U32 RtcCnt;
U32 RtcCnt1s;                                                                   //lq ����Rtc��UTC1s����ڵļ���ֵ
U32 RtcCnt10s;                                                                  //lq ����Rtc��UTC10s����ڵļ���ֵ
U32 RtcCnt100s;                                                                 //lq ����Rtc��UTC100s����ڵļ���ֵ

double RtcTimPhaseCnt1 = 0;
double RtcTimPhaseCnt10 = 0;
double RtcTimPhaseCnt100 = 0;

S16 RtcPhaseAdjust1;//rtcÿ1�������phase����
S16 RtcPhaseAdjust10;//rtcÿ10�������phase����
S16 RtcPhaseAdjust100;//rtcÿ100�������phase����
S16 RtcCntAdjust1;//rtcÿ1���������
S16 RtcCntAdjust10;//rtcÿ10���������
S16 RtcCntAdjust100;//rtcÿ100���������

U8 RtcHighCountFlag;//RTC��ƵFLAG
RtcPara struRtcPara;
RtcTimPhaseStruct struRtcTimPhase1;

#if 1
/*******************************************************************************
* ����  : CalcRtcPhaseAdjust
* ����  : ����RTC��ƵʱҪ���е�ʱ�����
* ����  : DiffNum 4096���������  PeriodCount ͳ������
* ���  : ��
* ����  : ��
*******************************************************************************/
void CalcRtcPhaseAdjust(S32 DiffNum ,U16 PeriodCount)
{
	S32 slTemp1;
    S16 RtcPhaseAdjust1_Temp;//rtcÿ1�������phase����
    S16 RtcPhaseAdjust10_Temp;//rtcÿ10�������phase����
    S16 RtcPhaseAdjust100_Temp;//rtcÿ100�������phase����

	if (IsRtcClockSourceLSE() == TRUE)
	{
		//LSE
		slTemp1 = DiffNum * 48000 / RTC_LSE_FREQ_VALUE;//����Ϊʱ϶��                          //lq ��RTC���жϴ���ת��Ϊ��λ��
	}
	else
	{
		//LSI
		slTemp1 = DiffNum * 48000 / RTC_LSI_FREQ_VALUE;//����Ϊʱ϶��	
	}

	RtcPhaseAdjust1_Temp = slTemp1 / PeriodCount;
	RtcPhaseAdjust10_Temp = (slTemp1 * 10 - RtcPhaseAdjust1_Temp * PeriodCount * 10) / PeriodCount;
	RtcPhaseAdjust100_Temp = (slTemp1 * 100 - RtcPhaseAdjust1_Temp * PeriodCount * 100 
                         - (RtcPhaseAdjust10_Temp * PeriodCount) * 10) / PeriodCount;
	
    usart_printf("RtcPhaseAdjust1_Temp  : %d\r\n", RtcPhaseAdjust1_Temp);				
	usart_printf("RtcPhaseAdjust10_Temp : %d\r\n", RtcPhaseAdjust10_Temp);				
	usart_printf("RtcPhaseAdjust100_Temp: %d\r\n", RtcPhaseAdjust100_Temp);

    /*lq ��Tim��ʱ�����ֵת��Ϊ���ʱ�����ֵ*/
    TransPhaseAdjust(&RtcPhaseAdjust1_Temp, &RtcPhaseAdjust10_Temp, &RtcPhaseAdjust100_Temp);

	if (RtcPhaseAdjust1_Temp < ADJUST_PHASE_MAX_ADJUST 
        && RtcPhaseAdjust1_Temp > -ADJUST_PHASE_MAX_ADJUST)                      //lq RtcPhaseAdjust10_Temp,RtcPhaseAdjust100_Tempȡֵ��Χ[0,9]
	{
		RtcPhaseAdjust1 = RtcPhaseAdjust1_Temp;
		RtcPhaseAdjust10 = RtcPhaseAdjust10_Temp;
		RtcPhaseAdjust100 = RtcPhaseAdjust100_Temp;
	}
}
#else
/*******************************************************************************
* ����  : CalcRtcPhaseAdjust
* ����  : ����RTC��ƵʱҪ���е�ʱ�����(RtcPhaseAdjust1ÿ����ԭֵ���ۼ�)
* ����  : DiffNum 4096���������  PeriodCount ͳ������
* ���  : ��
* ����  : ��
*******************************************************************************/
void CalcRtcPhaseAdjust(S32 DiffNum ,U16 PeriodCount)
{
	S32 slTemp1;
//    double slTemp1;
    S16 RtcPhaseAdjust1_Temp;//rtcÿ1�������phase����
    S16 RtcPhaseAdjust10_Temp;//rtcÿ10�������phase����
    S16 RtcPhaseAdjust100_Temp;//rtcÿ100�������phase����
    S16 RtcCntAdjust1_Temp;
    S16 RtcCntAdjust10_Temp;
    S16 RtcCntAdjust100_Temp;

    usart_printf("Rtc DiffNum  : %d\r\n", DiffNum);				

    /*lq Rtc����ƫ�����*/
    RtcCntAdjust1_Temp = DiffNum / PeriodCount;
    RtcCntAdjust10_Temp = (DiffNum * 10 - RtcCntAdjust1_Temp * PeriodCount * 10) / PeriodCount;
    RtcCntAdjust100_Temp = (DiffNum * 100 - RtcCntAdjust1_Temp * PeriodCount * 100 
                              - ((RtcCntAdjust10_Temp * PeriodCount) / 10) * 100) / PeriodCount;

	if (RtcCntAdjust1_Temp > ADJUST_PHASE_MAX_ADJUST 
        || RtcCntAdjust1_Temp < -ADJUST_PHASE_MAX_ADJUST)//������Χ�����ִ���
	{
		RtcCntAdjust1_Temp = 0;
	}

	if (RtcCntAdjust10_Temp > ADJUST_PHASE_MAX_ADJUST 
        || RtcCntAdjust10_Temp < -ADJUST_PHASE_MAX_ADJUST)//������Χ�����ִ���
	{
		RtcCntAdjust10_Temp = 0;
	}

	if (RtcCntAdjust100_Temp > ADJUST_PHASE_MAX_ADJUST 
        || RtcCntAdjust100_Temp < -ADJUST_PHASE_MAX_ADJUST)//������Χ�����ִ���
	{
		RtcCntAdjust100_Temp = 0;
	}

    RtcCntAdjust1 += RtcCntAdjust1_Temp;                                        //rtcÿ1���������
    RtcCntAdjust10 += RtcCntAdjust10_Temp;                                      //rtcÿ10���������
    RtcCntAdjust100 += RtcCntAdjust100_Temp;                                    //rtcÿ100���������

    /*lq Rtc��λƫ�����*/
	if (IsRtcClockSourceLSE() == TRUE)
	{
		//LSE
		slTemp1 = DiffNum * 48000 / RTC_LSE_FREQ_VALUE;//����Ϊʱ϶��                      //lq ��RTC���жϴ���ת��Ϊ��λ��
	}
	else
	{
		//LSI
		slTemp1 = DiffNum * 48000 / RTC_LSI_FREQ_VALUE;//����Ϊʱ϶��	
	}

#if 0
	RtcPhaseAdjust1_Temp = (S16)(slTemp1 / PeriodCount);
	RtcPhaseAdjust10_Temp = (S16)((slTemp1 * 10 - RtcPhaseAdjust1_Temp * PeriodCount * 10) / PeriodCount);
	RtcPhaseAdjust100_Temp = (S16)((slTemp1 * 100 - RtcPhaseAdjust1_Temp * PeriodCount * 100 
                              - ((RtcPhaseAdjust10_Temp * PeriodCount) / 10) * 100) / PeriodCount);
#else
	RtcPhaseAdjust1_Temp = slTemp1 / PeriodCount;
	RtcPhaseAdjust10_Temp = (slTemp1 * 10 - RtcPhaseAdjust1_Temp * PeriodCount * 10) / PeriodCount;
	RtcPhaseAdjust100_Temp = (slTemp1 * 100 - RtcPhaseAdjust1_Temp * PeriodCount * 100 
                              - ((RtcPhaseAdjust10_Temp * PeriodCount) / 10) * 100) / PeriodCount;
#endif

	if (RtcPhaseAdjust1_Temp > ADJUST_PHASE_MAX_ADJUST 
        || RtcPhaseAdjust1_Temp < -ADJUST_PHASE_MAX_ADJUST)//������Χ�����ִ���
	{
		RtcPhaseAdjust1_Temp = 0;
	}

	if (RtcPhaseAdjust10_Temp > ADJUST_PHASE_MAX_ADJUST 
        || RtcPhaseAdjust10_Temp < -ADJUST_PHASE_MAX_ADJUST)//������Χ�����ִ���
	{
		RtcPhaseAdjust10_Temp = 0;
	}

	if (RtcPhaseAdjust100_Temp > ADJUST_PHASE_MAX_ADJUST 
        || RtcPhaseAdjust100_Temp < -ADJUST_PHASE_MAX_ADJUST)//������Χ�����ִ���
	{
		RtcPhaseAdjust100_Temp = 0;
	}

	RtcPhaseAdjust1 += RtcPhaseAdjust1_Temp;
	RtcPhaseAdjust10 += RtcPhaseAdjust10_Temp;
	RtcPhaseAdjust100 += RtcPhaseAdjust100_Temp;
}
#endif

/*******************************************************************************
* ����  : InitRtcPara
* ����  : ��ʼ��RTC���Ʋ���
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitRtcPara(void)
{
	RTCIntCount = 0;
	GpsRtcCount = 0;
    RtcCnt = 0;

	RtcPhaseAdjust1 = 0;//������ʱ϶��
	RtcPhaseAdjust10 = 0;//������ʱ϶��
	RtcPhaseAdjust100 = 0;//������ʱ϶��
    
    RtcCntAdjust1 = 0;//rtcÿ1���������
    RtcCntAdjust10 = 0;//rtcÿ10���������
    RtcCntAdjust100 = 0;//rtcÿ100���������

	RtcHighCountFlag = TRUE;
}

/*******************************************************************************
* ����  : ResetRtcCount
* ����  : ��λRTC����
* ����  : RTC��ʱֵ
* ���  : ��
* ����  : ��
*******************************************************************************/
void ResetRtcCount(U16 usAlarmValue)
{
	RTC_WaitForLastTask();
	RTC_EnterConfigMode(); 
		
	RTC_WaitForLastTask();
	RTC_SetCounter(0);//���¿�ʼRTC����	
    RTC_WaitForSynchro();                                                   //lq �������״̬������״̬��RTCƵ�ʲ�һ�µ�����

	RTC_WaitForLastTask();
	RTC_SetAlarm(RTC_GetCounter() + usAlarmValue);

	RTC_WaitForLastTask();
	RTC_ExitConfigMode();

	RTC_WaitForLastTask();
}	

/*******************************************************************************
* ����  : RTCConfigurationPart1
* ����  : ����ʵʱʱ��RTC�ĵ�һ����
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void RTCConfigurationPart1(void)                                                //lq ���RTC�ж�����
{
	//����RTC�ж�
	EXTI_InitTypeDef EXTI_InitStructure;

	/* Configure EXTI Line17(RTC Alarm) to generate an interrupt on rising edge */
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;                                 //lq Line17ΪRTCAlarm�ж�
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

    /*lq ���ϵ翪��ʱִ��*/
    if (ucStandbyWakeupFlg == FALSE)
    {
	    /* Reset Backup Domain */
	    BKP_DeInit();

    	/* Enable the LSI OSC */
    	//RCC_LSICmd(ENABLE);
        RCC_LSEConfig(RCC_LSE_ON); //RCC����LSEʱ��                               //lq �ȴ��ⲿ���񣬴������Ѻ�LSE�ᱣ������֮ǰ��״̬�����Ḵλ
    }	
}

/*******************************************************************************
* ����  : RTCConfigurationPart2
* ����  : ����ʵʱʱ��RTC�ĵڶ�����
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void RTCConfigurationPart2(void)                                                //lq ����ѡ���RTCʱ��Դ������ʱ��Ƶ��
{
	//�˲��ֵĵȴ�������timeadjust�����ж�
//	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)//�ȴ�LSE��Ч
//	{}
//
//	UartResponseMsg("32.768MHz CLOCK READY!");
//
//	/* Select the RTC Clock Source */
//	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//����ʱ��ΪLSE
//	
	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);
	
	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();             //lq ��ʱԼ220us
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();            //lq ��ʱԼ600ns

	/* Enable the RTC Alarm interrupt */
	//RTC_ITConfig(RTC_IT_ALR, ENABLE);
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Set RTC prescaler: set RTC period to 1sec */
	
	if (IsRtcClockSourceLSE() == TRUE)
	{
		//LSE
		//lq RTC_SetPrescaler(31);//��Ƶ1024 ��Ƶ512                                 //lq 32��Ƶ����Ƶ����0-31��
	    RTC_SetPrescaler(RTC_LSE_PRE_VALUE - 1);                                                   //lq 512��Ƶ��Ƶ��Ϊ64
        RtcCnt1s = RTC_ADJUST_1S;
        RtcCnt10s = RTC_ADJUST_10S;
        RtcCnt100s = RTC_ADJUST_100S;
	}
	else
	{
		//LSI
		//lq RTC_SetPrescaler(39);//��Ƶ1000 ��Ƶ500                                 //lq 40��Ƶ
	    RTC_SetPrescaler(RTC_LSI_PRE_VALUE - 1);                                                   //lq 800��Ƶ��Ƶ��Ϊ50

	    RtcCnt1s = RTC_ADJUST_1S_LSI;
        RtcCnt10s = RTC_ADJUST_10S_LSI;
        RtcCnt100s = RTC_ADJUST_100S_LSI;
    }
		
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Alarm in 5 second */
	RTC_SetAlarm(RTC_GetCounter() + 30);                                        //lq?
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Enable the RTC Alarm interrupt */
	RTC_ITConfig(RTC_IT_ALR, ENABLE);
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}		

#if 0 //lq
/*******************************************************************************
* ����  : RTCAlarmInt
* ����  : RTC�жϷ������0.375K�жϣ��൱��48K��128��Ƶ
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
//U8 gucTestRtc = 0;
void RTCAlarmInt(void)                                                          //lq ���ж�ÿ����һ�Σ��൱��128����λ
{
	if(RTC_GetITStatus(RTC_IT_ALR) != RESET)
	{

		if (eqEquipPara.ucLowFreqSta == EQ_FREQSTA_LOW)                         //lq RTCʱ����������״̬����λ����
		{

			Is_Feed_Dog = TRUE;//ι����־λ
#if 1			
			
			RtcSubCnt1++;                                                       //lq RTC��"1s"����
			RtcSubCnt2++;                                                       //lq RTC��"10s"����
			RtcSubCnt3++;                                                       //lq RTC��"100s"����

			if (IsRtcClockSourceLSE() == TRUE)
			{
				//LSE�µ�ʱ϶����                                               //lq 48000/512=93.75 0.75*4=3 ��ÿ4�μ���3�Σ�
				if (RTCIntCount < 3)//����ǲ���0.75
				{
					gsiPhaseCnt += 94;
					RTCIntCount++;
				}
				else if (RTCIntCount == 3)
				{
					gsiPhaseCnt += 93;
					RTCIntCount = 0;
				}
				else
				{
					RTCIntCount = 0;
				}
			}
			else
			{
				//LSI�µ�ʱ϶����
				gsiPhaseCnt += 96;//500HZ�ж� ÿ�μ�96                          //lq 1s����48000����λ��48000/500=96
			}

#if 1                                                                           //lq ��ƵʱRTCΪ512Hz
			if (RtcSubCnt1 >= RTC_ADJUST_1S)//1��                                //lq ÿRTC��1s����λ������������
			{
				RtcSubCnt1 = 0;
				gsiPhaseCnt -= RtcPhaseAdjust1; //��Ƶģʽ�ж�Ƶ��һ�룬���Գ�2
				//GpsTimCount -= 8;
			}

			if (RtcSubCnt2 >= RTC_ADJUST_10S)//10��                             //lq ÿTim2��10s����λ������������
			{
				RtcSubCnt2 = 0;
				gsiPhaseCnt -= RtcPhaseAdjust10;
				//GpsTimCount -= 1;
			}

			if (RtcSubCnt3 >= RTC_ADJUST_100S)//100��                           //lq ÿTim2��100s����λ������������
			{
				RtcSubCnt3 = 0;
				gsiPhaseCnt -= RtcPhaseAdjust100;
				//GpsTimCount -= 1;
			}
#endif
				
			//1280����λΪһ��ʱ϶
			if (gsiPhaseCnt >= 1280)
			{
				gsiPhaseCnt -= 1280;
	
				gsiSlotCnt++;
				//2250��ʱ϶Ϊһ��֡��һ֡Ϊ60s��Ϊ1min���˴����Ӽ�������Զ
				if (gsiSlotCnt >= 2250)
				//if (gsiSlotCnt >= 225)
				{
					gsiSlotCnt = 0;
	
					gslFrameCnt++;
				}
			}
#endif
		}
		//��Ƶ����
		else if (eqEquipPara.ucLowFreqSta == EQ_FREQSTA_HIG)                    //lq ����RTC�ĵ���
		{
			if (IsGpsPowerupProtect() == TRUE)
			{
				if (RtcHighCountFlag == TRUE)                                   //lq ����״̬��RTCΪ1024��or 1000��Hz����Ƶ״̬�µ�2������
				{                                                               //lq? ����ÿ2���жϼ���1�� ��
					GpsRtcCount++;                                              //lq GPS��λ״̬�£�RTC����

					RtcHighCountFlag = FALSE;
				}
				else
				{
					RtcHighCountFlag = TRUE;
				}
			}
		}

		/* Clear EXTI line17 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line17);
		
		/* Check if the Wake-Up flag is set */
		if(PWR_GetFlagStatus(PWR_FLAG_WU) != RESET)
		{
			/* Clear Wake Up flag */
			PWR_ClearFlag(PWR_FLAG_WU);
		}		

		RTC_WaitForLastTask();
		RTC_EnterConfigMode(); 
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();   
		/* Clear RTC Alarm interrupt pending bit */
		RTC_ClearITPendingBit(RTC_IT_ALR);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
		/* Alarm in 5 second */
		RTC_SetAlarm(RTC_GetCounter()+ 1);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();		
		RTC_WaitForLastTask();
		RTC_ExitConfigMode();
		RTC_WaitForLastTask();	
	}
}
#endif

/*******************************************************************************
* ����  : RTCAlarmInt
* ����  : RTC�жϷ������0.375K�жϣ��൱��48K��128��Ƶ
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 gucTestRtc = TRUE;
void RTCAlarmInt(void)                                                          //lq ���ж�ÿ����һ�Σ��൱��128����λ
{
	U32 TempRtcInteral = 0;
    S32 PhaseDiff = 0;
    S32 Temp1sCnt = 0;
    S32 Temp10sCnt = 0;
    S32 Temp100sCnt = 0;
    U32 tempDivider = 0;
    S32 slTemp1 = 0;

    if(RTC_GetITStatus(RTC_IT_ALR) != RESET)
	{
#if 0   //lqdb
        if (gucTestRtc == TRUE)
        {
            TEST_OUT_HIGH_1();
            gucTestRtc = FALSE;
        }
        else
        {
            TEST_OUT_LOW_1();//lq ����
            gucTestRtc = TRUE;
        }
#endif
#if 0		
        RtcSubCnt1++;                                                           //lq RTC��"1s"����
		RtcSubCnt2++;                                                           //lq RTC��"10s"����
		RtcSubCnt3++;                                                           //lq RTC��"100s"����
#endif
		if (DevDynamicPara.ucLowFreqSta == EQ_FREQSTA_LOW)                      //lq ����״̬�£�RTCʱ��������λ������У׼
		{  
			Is_Feed_Dog = TRUE;//ι����־λ                                     //lq  EQ_FREQSTA_HIG״̬�£���1KHz��Timι����EQ_FREQSTA_LOW״̬�£���RTCι��
#if 0
            /*lq �������Ѻ�ĵ�һ�ζ�ʱ�ж�*/
            if (ucStandbyWakeupDelayFlg == TRUE)
            {
                ucStandbyWakeupDelayFlg = FALSE;

                /*lq ��RTC_CNTǰ���APB1��RTCʱ��ͬ��*/
            	RTC_WaitForSynchro();

                /*lq ����������ǰ�趨alarm��ʱ��ʱ�����������Ѻ�RTCʱ���жϿ�����ʱ����*/
                TempRtcInteral = RTC_GetCounter() - arrBKPDRData[BKPR_RTC_ALARM_VALUE_INDEX];//lq

                /*lq ������ʱ��Ӧ����λ��*/
                // PhaseDiff = (double)TempRtcInteral * 48000.0 / (double)arrBKPDRData[BKPR_RTC_FREQUENCE_INDEX] + 0.5;
                
                /*lq ������ʱ��Ӧ��Tim��������λ��*/
                PhaseDiff = TempRtcInteral * ((double)struRtcTimPhase1.RtcTimPhaseNum / RTC_TIM_PHASE_PRECISE) + 0.5;

                Temp1sCnt = PhaseDiff / 48000;
                Temp10sCnt = PhaseDiff / 480000;
                Temp100sCnt = PhaseDiff / 4800000;

                /*lq �Լ����λ������У׼*/
                PhaseDiff -= TimAdjust1 * Temp1sCnt;
                PhaseDiff -= TimAdjust10 * Temp10sCnt;
                PhaseDiff -= TimAdjust100 * Temp100sCnt;
                
                PhaseDiff += gsiPhaseCnt;

                gsiPhaseCnt = PhaseDiff % 1280;
                gsiSlotCnt += PhaseDiff / 1280;
                if (gsiSlotCnt >= 2250)
                {
                    gslFrameCnt +=  gsiSlotCnt / 2250;
                    gsiSlotCnt = gsiSlotCnt % 2250;
                }
            }
#endif
#if 0
                if (SleepCtrlPara.ucStandbyAlarmSetFlg == TRUE)
                {
                    SleepCtrlPara.ucStandbyAlarmSetFlg = FALSE;
                                  
                    /*lq �����������ʱ��*/
                    struRtcPara.ssRtcAlarmValue = CalcuSleepSpan();
                    SleepCtrlPara.ucStandbyAlarmSetOk = TRUE;
                    
                }   
#endif
            if (SleepCtrlPara.ucTimToRtcSwitch == TRUE)
            {            
                /*lq ֹͣʹ��Tim������λ����*/
                SleepCtrlPara.ucTimToRtcSwitch = FALSE;
 
                //lq TEST_OUT_HIGH_2();                               

                /*lq �趨Standby���Ѷ�ʱ*/
                if (SleepCtrlPara.ucStandbyAlarmSetFlg == TRUE)
                {
                    SleepCtrlPara.ucStandbyAlarmSetFlg = FALSE;
                                  
                    /*lq �����������ʱ��*/
                    struRtcPara.ssRtcAlarmValue = CalcuSleepSpan();
                    SleepCtrlPara.ucStandbyAlarmSetOk = TRUE;
                    
                }                
                //lq ��Timת��ΪRtc�ĵ�һ��RTC�ж�RTC��λ������Ӧ��94��93��                                   
            }
            else
            {
    #if 1
                /*lq �������Ѻ�ĵ�һ�ζ�ʱ�ж�*/
                if (ucStandbyWakeupDelayFlg == TRUE)
                {
                    ucStandbyWakeupDelayFlg = FALSE;
    
                    /*lq ��RTC_CNTǰ���APB1��RTCʱ��ͬ��*/
                	RTC_WaitForSynchro();
    
                    /*lq ����������ǰ�趨alarm��ʱ��ʱ�����������Ѻ�RTCʱ���жϿ�����ʱ����*/
#if 1                    
                    // TempRtcInteral = RTC_GetCounter() -1 - arrBKPDRData[BKPR_RTC_ALARM_VALUE_INDEX];//lq
                    TempRtcInteral = RTC_GetCounter() -1 - (((arrBKPDRData[BKPR_RTC_ALARM_VALUE_H_INDEX] << 16) & 0xFFFF0000) 
                                     | (arrBKPDRData[BKPR_RTC_ALARM_VALUE_L_INDEX] & 0xFFFF));
#else
                    tempCnt = RTC_GetCounter();
                    TempRtcInteral = tempCnt - arrBKPDRData[BKPR_RTC_ALARM_VALUE_INDEX];//lq
#endif    
                    /*lq ������ʱ��Ӧ����λ��*/
                    // PhaseDiff = (double)TempRtcInteral * 48000.0 / (double)arrBKPDRData[BKPR_RTC_FREQUENCE_INDEX] + 0.5;
                    
                    /*lq ������ʱ��Ӧ��Tim��������λ��*/
                    PhaseDiff = TempRtcInteral * ((double)struRtcTimPhase1.RtcTimPhaseNum / RTC_TIM_PHASE_PRECISE) + 0.5;
    
                    Temp1sCnt = PhaseDiff / 48000;
                    Temp10sCnt = PhaseDiff / 480000;
                    Temp100sCnt = PhaseDiff / 4800000;
    
                    /*lq �Լ����λ������У׼*/
                    PhaseDiff -= TimAdjust1 * Temp1sCnt;
                    PhaseDiff -= TimAdjust10 * Temp10sCnt;
                    PhaseDiff -= TimAdjust100 * Temp100sCnt;
                    
                    PhaseDiff += gsiPhaseCnt;
                    if (PhaseDiff < 0)
                    {
                        slTemp1 = PhaseDiff / 1280;
                        slTemp1--;                                                              
                        gsiPhaseCnt = (-slTemp1 * 1280 + PhaseDiff);                                //lq ��λ
                        gsiSlotCnt += slTemp1;
                        if (gsiSlotCnt < 0)
                        {
                            slTemp1 = gsiSlotCnt / 2250;
                            slTemp1--;                                                              
                            gsiSlotCnt = (-slTemp1 * 2250 + gsiSlotCnt);                            //lq ��λ
                            gslFrameCnt += slTemp1;
                            if (gslFrameCnt < 0)
                            {
                                gslFrameCnt = 0;
                                gsiSlotCnt = 0;
                                gsiPhaseCnt = 0;
                            }
                        }
                    }
                    else
                    {
                        gsiPhaseCnt = PhaseDiff % 1280;
                        gsiSlotCnt += PhaseDiff / 1280;
                        if (gsiSlotCnt >= 2250)
                        {
                            gslFrameCnt +=  gsiSlotCnt / 2250;
                            gsiSlotCnt = gsiSlotCnt % 2250;
                        }                    
                    }    
                }
                else
                {
            #if 1		
                    RtcSubCnt1++;                                                           //lq RTC��"1s"����
            		RtcSubCnt2++;                                                           //lq RTC��"10s"����
            		RtcSubCnt3++;                                                           //lq RTC��"100s"����
            #endif
        			if (IsRtcClockSourceLSE() == TRUE)
        			{
#if 1
        				/*lq LSE�µ�ʱ϶���㣬��ӦRTCƵ��512��alarmƵ��512*/                                          
        				if (RTCIntCount < 3)                                            //lq 48000/512=93.75 0.75*4=3 ��ÿ4�μ���3�Σ�
        				{
        					gsiPhaseCnt += 94;
        					RTCIntCount++;
        				}
        				else if (RTCIntCount == 3)
        				{
        					gsiPhaseCnt += 93;
        					RTCIntCount = 0;
        				}
        				else
        				{
        					RTCIntCount = 0;
        				}
#else
                        if (RTCIntCount < 15)
                        {
                            if (RTCIntCount % 2 == 0)
                            {
        					    gsiPhaseCnt += 23;
                            }
                            else
                            {
        					    gsiPhaseCnt += 24;                            
                            } 
                            RTCIntCount++;                       
                        }
                        else if (RTCIntCount == 15)
                        {
        					gsiPhaseCnt += 23;
        					RTCIntCount = 0;                        
                        }
                        else
                        {
        					RTCIntCount = 0;
                        }
#endif                                                               
        			}
        			else
        			{
        				/*lq LSI�µ�ʱ϶���㣬��ӦRTCƵ��500��alarmƵ��500*/                                          
        				gsiPhaseCnt += 96;                                              //lq 1s����48000����λ��48000/500=96
                	}
    #if 0
        			if (RtcSubCnt1 >= RtcCnt1s)                                         //lq ÿUTC��1s����λ������������
        			{
        				RtcSubCnt1 = 0;
        				gsiPhaseCnt -= RtcPhaseAdjust1; 
        			}
        
        			if (RtcSubCnt2 >= RtcCnt10s)                                        //lq ÿUTC��10s����λ������������
        			{
        				RtcSubCnt2 = 0;
        				gsiPhaseCnt -= RtcPhaseAdjust10;
        			}
        
        			if (RtcSubCnt3 >= RtcCnt100s)                                       //lq ÿUTC��100s����λ������������
        			{
        				RtcSubCnt3 = 0;
        				gsiPhaseCnt -= RtcPhaseAdjust100;
        			}
    #else
                    RtcTimPhaseCnt1 += struRtcTimPhase1.lfRtcTimPhaseNum;
                    if (RtcTimPhaseCnt1 >= TimCnt1s)
                    {
                        RtcTimPhaseCnt1 = 0;
                        gsiPhaseCnt -= TimAdjust1;                                      
                    }
                    
                    RtcTimPhaseCnt10 += struRtcTimPhase1.lfRtcTimPhaseNum;
                    if (RtcTimPhaseCnt10 >= TimCnt10s)
                    {
                        RtcTimPhaseCnt10 = 0;
                        gsiPhaseCnt -= TimAdjust10;                                      
                    }
                                    
                    RtcTimPhaseCnt100 += struRtcTimPhase1.lfRtcTimPhaseNum;
                    if (RtcTimPhaseCnt100 >= TimCnt100s)
                    {
                        RtcTimPhaseCnt100 = 0;
                        gsiPhaseCnt -= TimAdjust100;                                      
                    }
    
    #endif     
        			/*lq ��λ��ʱ϶��֡�Ļ��㣬1280����λΪ1��ʱ϶��2250��ʱ϶Ϊ1֡��1���ӣ�*/
        			if (gsiPhaseCnt >= 1280)
        			{
        				gsiSlotCnt += gsiPhaseCnt / 1280;
                        gsiPhaseCnt %= 1280;
        				if (gsiSlotCnt >= 2250)                                         //2250��ʱ϶Ϊһ��֡��һ֡Ϊ60s��Ϊ1min���˴����Ӽ�������Զ
        				{
        					gslFrameCnt += gsiSlotCnt / 2250; 
                            gsiSlotCnt %= 2250;	
        				}
        			}
                    else if (gsiPhaseCnt < 0)
                    {
                        slTemp1 = gsiPhaseCnt / 1280;
                        slTemp1--;
                        gsiPhaseCnt += -slTemp1 * 1280;                         //lq ��λ
                        gsiSlotCnt += slTemp1;
                        if (gsiSlotCnt < 0)
                        {
                            slTemp1 = gsiSlotCnt / 2250;
                            slTemp1--;                                                              
                            gsiSlotCnt += -slTemp1 * 2250;                      //lq ��λ
                            gslFrameCnt += slTemp1;
                            if (gslFrameCnt < 0)
                            {
                                gslFrameCnt = 0;
                                gsiSlotCnt = 0;
                                gsiPhaseCnt = 0;
                            }
                        }
                    }                
                }
    #endif

            }
       
		}
		else if (DevDynamicPara.ucLowFreqSta == EQ_FREQSTA_HIG)                    //lq ����״̬�£�ΪRTCʱ��У׼��׼��������״̬�µ���λ������У׼��Tim��ɣ�
		{
#if 1
            /*lq ����HighFreq״̬���RTC��һ���жϴ���ǰ����ʹ��RTC��������ʹ��Tim��λ����*/
            if (SleepCtrlPara.ucRtcToTimSwitch == TRUE)
            {    			
                SleepCtrlPara.ucRtcToTimSwitch = FALSE;     
#if 1
                gslRtcToTimCnt_2++;

#endif
        #if 1		
                RtcSubCnt1++;                                                           //lq RTC��"1s"����
        		RtcSubCnt2++;                                                           //lq RTC��"10s"����
        		RtcSubCnt3++;                                                           //lq RTC��"100s"����
        #endif
                if (IsRtcClockSourceLSE() == TRUE)
    			{
#if 1
    				/*lq LSE�µ�ʱ϶���㣬��ӦRTCƵ��512��alarmƵ��512*/                                          
    				if (RTCIntCount < 3)                                            //lq 48000/512=93.75 0.75*4=3 ��ÿ4�μ���3�Σ�
    				{
    					gsiPhaseCnt += 94;
    					RTCIntCount++;
    				}
    				else if (RTCIntCount == 3)
    				{
    					gsiPhaseCnt += 93;
    					RTCIntCount = 0;
    				}
    				else
    				{
    					RTCIntCount = 0;
    				}
#else
                    if (RTCIntCount < 15)
                    {
                        if (RTCIntCount % 2 == 0)
                        {
    					    gsiPhaseCnt += 23;
                        }
                        else
                        {
    					    gsiPhaseCnt += 24;                            
                        } 
                        RTCIntCount++;                       
                    }
                    else if (RTCIntCount == 15)
                    {
    					gsiPhaseCnt += 23;
    					RTCIntCount = 0;                        
                    }
                    else
                    {
    					RTCIntCount = 0;
                    }
#endif                                                                
    			}
    			else
    			{
    				/*lq LSI�µ�ʱ϶���㣬��ӦRTCƵ��500��alarmƵ��500*/                                          
    				gsiPhaseCnt += 96;                                              //lq 1s����48000����λ��48000/500=96
            	}
#if 1
#if 0
    			if (RtcSubCnt1 >= RtcCnt1s)                                         //lq ÿUTC��1s����λ������������
    			{
    				RtcSubCnt1 = 0;
    				gsiPhaseCnt -= RtcPhaseAdjust1; 
    			}
    
    			if (RtcSubCnt2 >= RtcCnt10s)                                        //lq ÿUTC��10s����λ������������
    			{
    				RtcSubCnt2 = 0;
    				gsiPhaseCnt -= RtcPhaseAdjust10;
    			}
    
    			if (RtcSubCnt3 >= RtcCnt100s)                                       //lq ÿUTC��100s����λ������������
    			{
    				RtcSubCnt3 = 0;
    				gsiPhaseCnt -= RtcPhaseAdjust100;
    			}
#else
                RtcTimPhaseCnt1 += struRtcTimPhase1.lfRtcTimPhaseNum;
                if (RtcTimPhaseCnt1 >= TimCnt1s)
                {
                    RtcTimPhaseCnt1 = 0;
                    gsiPhaseCnt -= TimAdjust1;                                      
                }
                
                RtcTimPhaseCnt10 += struRtcTimPhase1.lfRtcTimPhaseNum;
                if (RtcTimPhaseCnt10 >= TimCnt10s)
                {
                    RtcTimPhaseCnt10 = 0;
                    gsiPhaseCnt -= TimAdjust10;                                      
                }
                                
                RtcTimPhaseCnt100 += struRtcTimPhase1.lfRtcTimPhaseNum;
                if (RtcTimPhaseCnt100 >= TimCnt100s)
                {
                    RtcTimPhaseCnt100 = 0;
                    gsiPhaseCnt -= TimAdjust100;                                      
                }
#endif    
    			/*lq ��λ��ʱ϶��֡�Ļ��㣬1280����λΪ1��ʱ϶��2250��ʱ϶Ϊ1֡��1���ӣ�*/
    			if (gsiPhaseCnt >= 1280)
    			{
    				gsiSlotCnt += gsiPhaseCnt / 1280;
                    gsiPhaseCnt %= 1280;
    				if (gsiSlotCnt >= 2250)                                         //2250��ʱ϶Ϊһ��֡��һ֡Ϊ60s��Ϊ1min���˴����Ӽ�������Զ
    				{
    					gslFrameCnt += gsiSlotCnt / 2250; 
                        gsiSlotCnt %= 2250;	
    				}
    			}
                else if (gsiPhaseCnt < 0)
                {
                    slTemp1 = gsiPhaseCnt / 1280;
                    slTemp1--;
                    gsiPhaseCnt += -slTemp1 * 1280;                         //lq ��λ
                    gsiSlotCnt += slTemp1;
                    if (gsiSlotCnt < 0)
                    {
                        slTemp1 = gsiSlotCnt / 2250;
                        slTemp1--;                                                              
                        gsiSlotCnt += -slTemp1 * 2250;                      //lq ��λ
                        gslFrameCnt += slTemp1;
                        if (gslFrameCnt < 0)
                        {
                            gslFrameCnt = 0;
                            gsiSlotCnt = 0;
                            gsiPhaseCnt = 0;
                        }
                    }
                }
#endif       
            }
#endif
        	/*lq GPS5min��λ����ʱ������RTCʱ��ƫ���ͳ�Ƽ���*/
            if (IsGpsPowerupProtect() == TRUE)
			{
				RtcCnt++;                                                       //lq ����Rtc������δУ׼��
                GpsRtcCount++;                                                  //lq GPS��λ״̬�£�RTC������У׼��
#if 0
                /*lq ÿ��RTC���ڱ���Tim��ǰ����λ����*/
                RtcTimCountSave = GpsTimCount;
                RtcTimSaveFlag = TRUE;
#endif                
                if (struRtcTimPhase1.RtcTimPhaseFlg == FALSE)
                {
                    if (struRtcTimPhase1.RtcTimPhasePeriodNum == 0)
                    {
                        /*lq ��һ�μ���*/
                        struRtcTimPhase1.RtcTimPhaseCntLast = GpsTimCount;
                        struRtcTimPhase1.RtcTimPhasePeriodNum++;
                    }
                    else
                    {
                        struRtcTimPhase1.RtcTimPhaseCntNow = GpsTimCount;
                        struRtcTimPhase1.RtcTimPhaseNumTemp += (struRtcTimPhase1.RtcTimPhaseCntNow  - struRtcTimPhase1.RtcTimPhaseCntLast
                                                                 - struRtcTimPhase1.RtcTimPhaseNumTemp) / (double)struRtcTimPhase1.RtcTimPhasePeriodNum;
                        
                        struRtcTimPhase1.RtcTimPhaseCntLast = struRtcTimPhase1.RtcTimPhaseCntNow;

                        if (struRtcTimPhase1.RtcTimPhasePeriodNum >= GPS_CTRL_TIME_FIX_LONG_OPEN * 60 * 500)
                        {
                            struRtcTimPhase1.RtcTimPhaseNum = (U32)(struRtcTimPhase1.RtcTimPhaseNumTemp * RTC_TIM_PHASE_PRECISE);
                            struRtcTimPhase1.lfRtcTimPhaseNum = (double)struRtcTimPhase1.RtcTimPhaseNum / RTC_TIM_PHASE_PRECISE; 
                            
                            struRtcTimPhase1.RtcTimPhaseFlg = TRUE;
                        }
                        struRtcTimPhase1.RtcTimPhasePeriodNum++;
                    }                   
                }                   
            
            }
		}
		
        /* Clear EXTI line17 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line17);
		
		/* Check if the Wake-Up flag is set */
		if(PWR_GetFlagStatus(PWR_FLAG_WU) != RESET)
		{
			/* Clear Wake Up flag */
			PWR_ClearFlag(PWR_FLAG_WU);
		}		

		RTC_WaitForLastTask();
		RTC_EnterConfigMode(); 
		
        /* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();   
		
        /* Clear RTC Alarm interrupt pending bit */
		RTC_ClearITPendingBit(RTC_IT_ALR);
		
        /* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();                                                      //lq ��ʱԼ600ns
		
        if (SleepCtrlPara.ucStandbyAlarmSetOk == TRUE) 
        {
        	SleepCtrlPara.ucStandbyAlarmSetOk = FALSE;
            RTC_SetCounter(0);                                                      //���¿�ʼRTC����	
#if 0        	
            RTC_WaitForLastTask();
            RTC_WaitForSynchro();                                                   //lq �������״̬������״̬��RTCƵ�ʲ�һ�µ�����
            tempDivider = RTC_GetDivider();
#endif
        	RTC_WaitForLastTask();
        	RTC_SetAlarm(struRtcPara.ssRtcAlarmValue);           //lq ������ʱ 0����struRtcPara.ssRtcAlarmValue - 1����struRtcPara.ssRtcAlarmValue��512Ƶ������
        }
        else
        {
        	/* Alarm in 5 second */
            RTC_WaitForSynchro();                                                   //lq �������״̬������״̬��RTCƵ�ʲ�һ�µ�����
                                                                                    //lq ��ʱ30us           
            
            RTC_SetAlarm(RTC_GetCounter() + RTC_ALARM_INTERVAL_VALUE - 1);          //lq RTC_ALARM_INTERVAL_VALUE�����ڶ�ʱ
        }      
		
        /* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();		
		RTC_WaitForLastTask();
		RTC_ExitConfigMode();
		RTC_WaitForLastTask();	
    }
}



/*******************************************************************************
* ����  : InitRtcParaStruct
* ����  : ��ʼ��RtcPara�ṹ�����
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitRtcParaStruct(RtcPara *pRtcPara)
{
    pRtcPara->usRtcFreq = RTC_LSE_FREQ_VALUE;          
    pRtcPara->ucRtcLSEorLSI = RTC_CLK_SOURCE_LSE;
    pRtcPara->ucRtcAlarmMcuOrGps = RTC_ALARM_MCU;
    pRtcPara->ssRtcAlarmValue = 0;
}

/*******************************************************************************
* ����  : InitRtcTimPhaseStruct
* ����  : �ṹ���ʼ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitRtcTimPhaseStruct(void)
{
    struRtcTimPhase1.RtcTimPhaseFlg = FALSE;
    struRtcTimPhase1.RtcTimPhasePeriodNum = 0;
    struRtcTimPhase1.RtcTimPhaseCntLast = 0;
    struRtcTimPhase1.RtcTimPhaseCntNow = 0;
    struRtcTimPhase1.RtcTimPhaseNumTemp = 0;
    struRtcTimPhase1.RtcTimPhaseNum = RTC_TIM_PHASE_NUM_DEFAULT;
    struRtcTimPhase1.lfRtcTimPhaseNum = (double)struRtcTimPhase1.RtcTimPhaseNum / RTC_TIM_PHASE_PRECISE; 
}

/*******************************************************************************
* Function Name  : RTC_SelfCheck
* Description    : RTC�Լ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_SelfCheck(RtcSelfCheckStruct *pRtcSelfCheckStruct)
{
    if (pRtcSelfCheckStruct->StartFlg == TRUE)
    {
        if (TimAdjustPara.RtcClockChoice == TRUE)
        {
            if (TimAdjustPara.RtcClockSource == ADJUST_RTC_SOURCE_LSE)
            {
                pRtcSelfCheckStruct->RtcLseOkFlg = TRUE;        
            }
            else
            {
                pRtcSelfCheckStruct->RtcLseOkFlg = FALSE;                
            }
            
            pRtcSelfCheckStruct->CompleteFlg = TRUE;
            pRtcSelfCheckStruct->StartFlg = FALSE;
        }            
    }
}

/*******************************************************************************
* ����  : EEPROM_EepromSelfCheckStructInit
* ����  : ��ʼ���ṹ��
* ����  : pRtcSelfCheckStruct - RtcSelfCheckStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void RTC_RtcSelfCheckStructInit(RtcSelfCheckStruct *pRtcSelfCheckStruct)
{
    pRtcSelfCheckStruct->StartFlg = TRUE;
    pRtcSelfCheckStruct->CompleteFlg = FALSE;
    pRtcSelfCheckStruct->RtcLseOkFlg = FALSE;      
} 