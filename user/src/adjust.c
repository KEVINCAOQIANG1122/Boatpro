/*******************************************************************************
  Copyright (C), 2012, 712.Co.,Ltd.
  FileName: adjust.c
  Author: ������      Version : V1.00        Date: 2012.07.28
  Description:    ʱ���Զ�У��ģ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include "stm32f10x.h"
#include "Include.h"
#include "def.h"
#include "adjust.h"
#include "gpsctrl.h"
#include "gps.h"
#include "Tim.h"
#include "initial.h"
#include "rtc.h"
#include "debug.h"
#include "systemtick.h"

TimeAdjustStruct TimAdjustPara;

/*******************************************************************************
* ����  : InitTimAdjust
* ����  : ��ʱ��������ʼ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitTimAdjust(void)
{
	TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_IDEL;
	TimAdjustPara.TimPastCount = 0;
	TimAdjustPara.RtcPastCount = 0;
	//TimAdjustPara.NowCount = 0;
	//TimAdjustPara.DiffCount = 0;
	TimAdjustPara.TimDiffCount = 0;
	TimAdjustPara.RtcDiffCount = 0;
	TimAdjustPara.TimDiffSum = 0;
	TimAdjustPara.RtcDiffSum = 0;
	TimAdjustPara.PeriodCount = 0;
	TimAdjustPara.CalcNowFlag = FALSE;

    TimAdjustPara.TimCntLast = 0;
    TimAdjustPara.RtcCntLast = 0;
    TimAdjustPara.TimCntSec1 = 0;
    TimAdjustPara.RtcCntSec1 = 0;
    TimAdjustPara.TimCntSec1Sum = 0;
    TimAdjustPara.RtcCntSec1Sum = 0;

    TimAdjustPara.PeriodCountLast = 10;                                         //lq ��ʼ��ֵ��Ϊ10s

	//TimAdjustPara.TimeCnt = 0;
	TimAdjustPara.TimePeriod = ADJUST_WAIT_LSI_TIME;
	TimAdjustPara.TimeCnt = SetTimePeriodProcess(TimAdjustPara.TimePeriod);//���õȴ�ʱ��
	TimAdjustPara.RtcClockSource = ADJUST_RTC_SOURCE_LSE;//Ĭ��ʹ��LSEʱ��Դ
	TimAdjustPara.RtcClockChoice = FALSE;//����δѡ��RTCʱ��
}

/*******************************************************************************
* ����  : ResetTimAdjust
* ����  : ��ʱ��������λ
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/	
void ResetTimAdjust(void)
{
	TimAdjustPara.TimPastCount = 0;
	TimAdjustPara.RtcPastCount = 0;
	//TimAdjustPara.NowCount = 0;
	//TimAdjustPara.DiffCount = 0;
	TimAdjustPara.TimDiffCount = 0;
	TimAdjustPara.RtcDiffCount = 0;
	TimAdjustPara.TimDiffSum = 0;
	TimAdjustPara.RtcDiffSum = 0;
	TimAdjustPara.PeriodCount = 0;
	TimAdjustPara.CalcNowFlag = FALSE;
    
    TimAdjustPara.TimCntLast = 0;
    TimAdjustPara.RtcCntLast = 0;
    TimAdjustPara.TimCntSec1 = 0;
    TimAdjustPara.RtcCntSec1 = 0;
    TimAdjustPara.TimCntSec1Sum = 0;
    TimAdjustPara.RtcCntSec1Sum = 0;
        	
}	

/*******************************************************************************
* ����  : ResetTimAdjustCtrlState
* ����  : ��λ��ʱ����ģ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void ResetTimAdjustCtrlState(void)
{
	TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_IDEL;
}	

/*******************************************************************************
* ����  : TimAdjustProcess
* ����  : ��ʱ����������������GPSͬ���ڼ����GPS������ϵ���
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void TimAdjustProcess(void)//��ģ��ֻ������ʡ��ģʽ                             //lq ����ģʽ�£�GPS������ 
{                                                                               //lq Tim����һֱ��GPSͬ���� 
	S16 Phase1Adjust;//ʱ϶ÿ1�����ֵ                                          //lq ʵ����Ϊʡ��ģʽ������״̬��ƫ��ĵ�����׼��
	S16 Phase10Adjust;//ʱ϶ÿ10�����ֵ
	S16 Phase100Adjust;//ʱ϶ÿ100�����ֵ

	U32 TempCount;
	U32 TempCount2;
	
	//����ǿ���
	if (TimAdjustPara.AdjustCtrlState == ADJUST_CTRL_STA_IDEL)
	{
		if (IsGpsPowerupProtect() == TRUE)//GPS�Ƿ��ڿ�����λ����״̬       //lq ֻ�д���GPS������λ����״̬ʱ���Ž��е���
		{
			ResetTimSaveState();//��λGPS�����־λ
			ResetTimAdjust();//��λ�Ĵ���

            TimCnt = 0;
            RtcCnt = 0;

			TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_FRISTFLAG;
			debug_printf("$ADJUST_CTRL_STA : ADJUST_CTRL_STA_FRISTFLAG!");
		}
	}
	//�ȵ�һ��GPS����
	else if (TimAdjustPara.AdjustCtrlState == ADJUST_CTRL_STA_FRISTFLAG)
	{
		if (GetTimSaveState() == TRUE)
		{
			TimAdjustPara.TimPastCount = GetTimCount();//�����һ��
			TimAdjustPara.RtcPastCount = GetRtcCount();//�����һ��

            TimAdjustPara.TimCntLast = GetTimCnt();
            TimAdjustPara.RtcCntLast = GetRtcCnt();

			TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_COUNT;
			debug_printf("$ADJUST_CTRL_STA : ADJUST_CTRL_STA_COUNT!");
		}
		else
		{
			if (TimAdjustPara.CalcNowFlag == TRUE)  //lq ���뿪����λ������һֱδ�ٴζ���λ
			{
				TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_CALC;//��ʼ�������ֵ
			    debug_printf("$ADJUST_CTRL_STA : ADJUST_CTRL_STA_CALC!");
			}
		}
	}
	//ͳ�ƽ׶�
	else if (TimAdjustPara.AdjustCtrlState == ADJUST_CTRL_STA_COUNT)
	{
		if (GetTimSaveState() == TRUE)
		{				
			//Timƫ��
			TempCount = GetTimCount();
			TimAdjustPara.TimDiffCount = TempCount - TimAdjustPara.TimPastCount - 48000;//lq 1s��Tim�ļ�����У׼�����׼ֵ֮��  
			TimAdjustPara.TimPastCount = TempCount;//�����ȥ�ļ���״̬			

			TempCount = GetTimCnt();           
            TimAdjustPara.TimCntSec1 = TempCount - TimAdjustPara.TimCntLast;
            TimAdjustPara.TimCntLast = TempCount;

			//Rtcƫ��
		   	TempCount = GetRtcCount();												   //rtc���ж�Ƶ��Ϊ8192 //lq?
			if (IsRtcClockSourceLSE() == TRUE)
			{
				//LSEƫ��
				TimAdjustPara.RtcDiffCount = TempCount - TimAdjustPara.RtcPastCount
                                             - RTC_LSE_FREQ_VALUE;//�������512�����	
			}                                                                             //lq 1s��RTC��ʵ���ж������׼ֵ�� 
			else                                                                          //lq ��ƵʱΪ512����ƵʱΪ1024������Ƶʱÿ2���жϼ���һ�Σ�֮��     
			{
				//LSIƫ��
				TimAdjustPara.RtcDiffCount = TempCount - TimAdjustPara.RtcPastCount
                                             - RTC_LSI_FREQ_VALUE;//�������500�����
			}	
			TimAdjustPara.RtcPastCount = TempCount;//�����ȥ�ļ���״

			TempCount = GetRtcCnt();           
            TimAdjustPara.RtcCntSec1 = TempCount - TimAdjustPara.RtcCntLast;
            TimAdjustPara.RtcCntLast = TempCount;

			if (TimAdjustPara.TimDiffCount > ADJUST_MAX_DIFF_VALUE)//����GPS�����Ƕϵ���
			{
				//�˴�����ֱ�ӽ������ͳ�ƽ��״̬
				debug_printf("TIM RTC GPS PLUS FAIL !!!");
				TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_CALC;//��ʼ�������ֵ
			    debug_printf("$ADJUST_CTRL_STA : ADJUST_CTRL_STA_CALC!");
			}
			else                                                            //lq �����ۼ�ƫ���
			{
				TimAdjustPara.TimDiffSum += TimAdjustPara.TimDiffCount;
				TimAdjustPara.RtcDiffSum += TimAdjustPara.RtcDiffCount;
				
                TimAdjustPara.TimCntSec1Sum += TimAdjustPara.TimCntSec1;
                TimAdjustPara.RtcCntSec1Sum += TimAdjustPara.RtcCntSec1;

                TimAdjustPara.PeriodCount++;//��1��ʼ��                     //lq �ۼƴ���������������
            }

//lqdb				debug_printf("TIM ADJUST COUNT : %d\r\n ",TimAdjustPara.PeriodCount);
		}
		//���û��GPS����
		else
		{											//����ǳ���16���Ӿ͵���һ��ʱ��
			if (TimAdjustPara.CalcNowFlag == TRUE || TimAdjustPara.PeriodCount > 1000)//GPSģ�鿪������ʱ���ȥ�ˣ����������ֵ
			{
				TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_CALC;//��ʼ�������ֵ   //lq ������λ����ʱ�䵽or����16min(1000s)����ʼ����
			}
			else//�鿴����Ǵ���2����Ȼû��GPS���壬�����¼���
			{
				TempCount2 = GpsTimCount - TimAdjustPara.TimPastCount;//�������һ�������嵽���ڵ�ʱ��
				
				if (TempCount2 > ADJUST_MAX_WAIT_PLUS_VALUE)//�ȴ���ʱ      //lq ����2sû���յ�GPS������
				{
					debug_printf("TIM RTC GPS PLUS FAIL !!!");
					TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_CALC;//��ʼ�������ֵ   //lq ����2sû���յ�GPS������,��ʼ����
			        debug_printf("$ADJUST_CTRL_STA : ADJUST_CTRL_STA_CALC!");				
                }				
			}
		}
	}	

	//����׶�
	else if (TimAdjustPara.AdjustCtrlState == ADJUST_CTRL_STA_CALC)
	{
        if (TimAdjustPara.PeriodCount < TimAdjustPara.PeriodCountLast)          //lq С���ϴεļ������ڵĻ����Ͳ�������
		{
			debug_printf("TIM RTC Adjust Fail,Much less Sample Point !!!");
			debug_printf("Sample Point Num : %d, Last Sample Point Num : %d\r\n",
                         TimAdjustPara.PeriodCount, TimAdjustPara.PeriodCountLast);				

			TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_IDEL;//״̬������
			debug_printf("$ADJUST_CTRL_STA : ADJUST_CTRL_STA_IDEL!");

		}
		else//���Լ���
		{
    		/*lq ��¼���μ�������ֵ*/
            TimAdjustPara.PeriodCountLast = TimAdjustPara.PeriodCount;

            /*lq ���㱾��Tim��UTC1s�ڵļ���ֵ*/
    		TempCount = (U32)((double)TimAdjustPara.TimCntSec1Sum / (double)TimAdjustPara.PeriodCount + 0.5);
            if (TempCount > ADJUST_TIM_CNT_1S_FLOOR && TempCount < ADJUST_TIM_CNT_1S_CEIL)
            {
                TimCnt1s = TempCount;
                TimCnt10s = (U32)(((double)TimAdjustPara.TimCntSec1Sum / (double)TimAdjustPara.PeriodCount) * 10 + 0.5);
                TimCnt100s = (U32)(((double)TimAdjustPara.TimCntSec1Sum / (double)TimAdjustPara.PeriodCount) * 100 + 0.5);   
            }
    
    		/*lq ���㱾��Rtc��UTC1s�ڵļ���ֵ*/
            TempCount = (U32)((double)TimAdjustPara.RtcCntSec1Sum / (double)TimAdjustPara.PeriodCount + 0.5);
            if (IsRtcClockSourceLSE() == TRUE)
            {
                if (TempCount > ADJUST_RTC_LSE_CNT_1S_FLOOR && TempCount < ADJUST_RTC_LSE_CNT_1S_CEIL)
                {
                    RtcCnt1s = TempCount;
                    RtcCnt10s = (U32)(((double)TimAdjustPara.RtcCntSec1Sum / (double)TimAdjustPara.PeriodCount) * 10 + 0.5);
                    RtcCnt100s = (U32)(((double)TimAdjustPara.RtcCntSec1Sum / (double)TimAdjustPara.PeriodCount) * 100 + 0.5);
                }
            }
            else
            {
                if (TempCount > ADJUST_RTC_LSI_CNT_1S_FLOOR && TempCount < ADJUST_RTC_LSI_CNT_1S_CEIL)
                {
                    RtcCnt1s = TempCount;
                    RtcCnt10s = (U32)(((double)TimAdjustPara.RtcCntSec1Sum / (double)TimAdjustPara.PeriodCount) * 10 + 0.5);
                    RtcCnt100s = (U32)(((double)TimAdjustPara.RtcCntSec1Sum / (double)TimAdjustPara.PeriodCount) * 100 + 0.5);
                }        
            }
    
    		debug_printf("Tim 1s Count  : %d\r\n", TimCnt1s);				
    		debug_printf("Tim 10s Count : %d\r\n", TimCnt10s);				
    		debug_printf("Tim 100s Count: %d\r\n", TimCnt100s);				
    
    		debug_printf("Rtc 1s Count  : %d\r\n", RtcCnt1s);				
    		debug_printf("Rtc 10s Count : %d\r\n", RtcCnt10s);				
    		debug_printf("Rtc 100s Count: %d\r\n", RtcCnt100s);				

			debug_printf("TIM RTC Adjust Calc Begin!!!");
			debug_printf("Sample Point Num : %d\r\n", TimAdjustPara.PeriodCount);
        
        #if 0
			                                                                //lq ����Tim2ʱ�ӵ�ƫ���
			//Phase2Adjust = (U16)TimError;//1��Ҫ������phase����           //lq Tim���жϴ�������λ����ȣ�48kHzTim���жϼ��������λʱ����    
			Phase1Adjust = TimAdjustPara.TimDiffSum / TimAdjustPara.PeriodCount;//�������������ȥ  //lq ƫ����������֣�ÿһ�����������λ����
			Phase10Adjust = (TimAdjustPara.TimDiffSum * 10 - Phase1Adjust * TimAdjustPara.PeriodCount * 10) / TimAdjustPara.PeriodCount;//ÿ10�벹��Ĳ���  //lq ƫ���ʮ��λС�����֣�ʮ��λС������ÿ10s����һ�Σ�
			Phase100Adjust = (TimAdjustPara.TimDiffSum * 100 - Phase1Adjust * TimAdjustPara.PeriodCount * 100 
                              - ((Phase10Adjust * TimAdjustPara.PeriodCount) / 10) *100) / TimAdjustPara.PeriodCount;//ÿ100�벹��Ĳ���    //lq ƫ��İٷ�λС�����֣��ٷ�λС��ÿ100s����һ�Σ� 				

			if (Phase1Adjust > ADJUST_PHASE_MAX_ADJUST || Phase1Adjust< -ADJUST_PHASE_MAX_ADJUST)//������Χ�����ִ���
			{
				Phase1Adjust = 0;
			}

			if (Phase10Adjust > ADJUST_PHASE_MAX_ADJUST || Phase10Adjust< -ADJUST_PHASE_MAX_ADJUST)//������Χ�����ִ���
			{
				Phase10Adjust = 0;
			}

			if (Phase100Adjust > ADJUST_PHASE_MAX_ADJUST || Phase100Adjust< -ADJUST_PHASE_MAX_ADJUST)//������Χ�����ִ���
			{
				Phase100Adjust = 0;
			}

			SetTimAdjust1(Phase1Adjust);                                    //lq ����ֵ�ۼ�    
			SetTimAdjust10(Phase10Adjust);
			SetTimAdjust100(Phase100Adjust);
        #else
			CalcTimPhaseAdjust(TimAdjustPara.TimDiffSum, TimAdjustPara.PeriodCount); //lq ����RTCʱ�ӵ�ƫ���
        #endif

			CalcRtcPhaseAdjust(TimAdjustPara.RtcDiffSum, TimAdjustPara.PeriodCount); //lq ����RTCʱ�ӵ�ƫ���

			debug_printf("TIM RTC Adjust Finish !!!");

    		debug_printf("TimAdjust1  : %d\r\n", TimAdjust1);				
    		debug_printf("TimAdjust10 : %d\r\n", TimAdjust10);				
    		debug_printf("TimAdjust100: %d\r\n", TimAdjust100);				
    
    		debug_printf("RtcPhaseAdjust1  : %d\r\n", RtcPhaseAdjust1);				
    		debug_printf("RtcPhaseAdjust10 : %d\r\n", RtcPhaseAdjust10);				
    		debug_printf("RtcPhaseAdjust100: %d\r\n", RtcPhaseAdjust100);
    		debug_printf("RtcTimPhaseCnt   : %d\r\n", struRtcTimPhase1.RtcTimPhaseNum);
            	
			TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_IDEL;//״̬������
			debug_printf("$ADJUST_CTRL_STA : ADJUST_CTRL_STA_IDEL!");
		}
	}
	else
	{
		TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_IDEL;//״̬������
		debug_printf("$ADJUST_CTRL_STA : ADJUST_CTRL_STA_IDEL!");        	
	}
	                                                                            
	RtcClockChoice();                                                           //lq? Ϊ���ڴ�ʱѡ��RTCʱ��Դ��
}

/*******************************************************************************
* ����  : RtcClockChoice
* ����  : ѡ��RTCʱ��Դ
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void RtcClockChoice(void)
{
	//RTCʱ��ѡ��ģ��
	if (TimAdjustPara.RtcClockChoice == FALSE)//δѡ��ʱ��Դ
	{
		if (TimAdjustPara.TimeCnt == GetLocalClockTickProcess())
		{
			TimAdjustPara.RtcClockChoice = TRUE;

			if (!(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET))//����ж�LSE����ɹ�
			{
				UartResponseMsg("$32.768KHz CLOCK READY!");	
				
				TimAdjustPara.RtcClockSource = ADJUST_RTC_SOURCE_LSE;//ѡ��ʱ��ԴΪLSE
				struRtcPara.ucRtcLSEorLSI = RTC_CLK_SOURCE_LSE;

				/* Select the RTC Clock Source */
				RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//����ʱ��ΪLSE	        //lq �ⲿ��������ɹ���LSE����ѡΪRTCʱ��Դ
			}
			else//��û��������LSI
			{
			 	/* Enable the LSI OSC */
				RCC_LSICmd(ENABLE);                                             //lq �����ڲ�RC����
				/* Wait till LSI is ready */
				while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)//������ʱ��λ�˾�˵��LSI�񵴵�·�ڵ���Ҳ����
				{}

				UartResponseMsg("$40KHZ CLOCK READY!");
				TimAdjustPara.RtcClockSource = ADJUST_RTC_SOURCE_LSI;//ѡ��ʱ��ԴΪLSI
				struRtcPara.ucRtcLSEorLSI = RTC_CLK_SOURCE_LSI;

				/* Select the RTC Clock Source */
				RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);                         //lq �ⲿ��������ʧ�ܣ�LSI����ѡΪRTCʱ��Դ
			}

			RTCConfigurationPart2();//���ʣ������
		}
	}
}

/*******************************************************************************
* ����  : IsRtcClockSourceLSE
* ����  : �ж�RTCʱ���Ƿ�ΪLSE
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 IsRtcClockSourceLSE(void)
{
	if (TimAdjustPara.RtcClockSource == ADJUST_RTC_SOURCE_LSE)	
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*******************************************************************************
* ����  : SetAdjustCalcNow
* ����  : ��ʱ�����ģ�����̼�������ֵ
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetAdjustCalcNow(void)
{
	TimAdjustPara.CalcNowFlag = TRUE;//���̼���
}	

/*******************************************************************************
* ����  : GetAdjustSleepState
* ����  : ��ȡ������״̬
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetAdjustSleepState(void)
{
	//��ȡ������״̬
	if (TimAdjustPara.AdjustCtrlState == ADJUST_CTRL_STA_IDEL)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}


	
