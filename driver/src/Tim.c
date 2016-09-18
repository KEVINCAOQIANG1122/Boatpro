/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: Tim.c
  Author: vozhj      Version : V1.00        Date: 2011.10.18
  Description:    Ӳ����ʱ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>

*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "def.h"
#include "Tim.h"
#include "Adc.h"
#include "systemtick.h"
#include "wdg.h"
#include "gmskmod.h"
#include "LinkPtlReport.h"
#include "initial.h"
#include "digunit.h"
#include "Dac.h"
#include "power.h"
#include "gpsctrl.h"
#include "LED.h"
#include "rtc.h"
#include "testing.h"
#include "SOTDMA.h"
#include "ledctrl.h"
#include "adjust.h"
#include "sleepctrl.h"
#include "config.h"
#include "SPI.h"
#include "gpiospi.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
U16 CCR1_Val;
U16 CCR2_Val;
U16 CCR3_Val;
S16 gsiPhaseCnt;                                                                //lq  gsiPhaseCnt��gsiSlotCnt��gslFrameCnt                                                                               
S16 gsiSlotCnt;                                                                 //lq  ���߽�������Ϣ���͵�ʱ����ƣ�
S32 gslFrameCnt;                                                                //lq  �����Ķ�ʱʹ�ñ���5ms��ʱ����

U32 TimSubCnt1;//��ʱ������������
U32 TimSubCnt2;//��ʱ������������
U32 TimSubCnt3;//��ʱ������������

S16 TimAdjust1;//��ʱ��ÿ2�����phase��
S16 TimAdjust10;//��ʱ��ÿ10�����phase��
S16 TimAdjust100;//��ʱ��ÿ50�����phase��

U32 GpsTimCount;//����TIM2ʱ������ļ�����                                      //lq ����Tim������������UTC��ƫ��ֵУ׼��
U32 TimCnt;                                                                     //lq ����Tim������δУ׼��                                                                
U32 TimCnt1s;                                                                   //lq ����Tim��UTC1s����ڵļ���ֵ
U32 TimCnt10s;                                                                  //lq ����Tim��UTC10s����ڵļ���ֵ
U32 TimCnt100s;                                                                 //lq ����Tim��UTC100s����ڵļ���ֵ

U8  gucFSKDataXmtEn = FALSE;
U8  gucFetchFirstFskDataEn = FALSE;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

U16 capture = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/ 
/*******************************************************************************
* ����  : SetTimAdjust1
* ����  : ���ö�ʱ��ÿ1�����phase��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetTimAdjust1(S16 data)
{
//lq	TimAdjust1 = TimAdjust1 + data;//��ԭ���Ļ����ϵ���                         //lq ����ֵ�ۼӣ�
	TimAdjust1 = data;                                                          //lq ��Ӧ�ۼӣ���Ϊÿ��ͳ�ƽ׶�����ͳ��ʱ������GpsTimCount���ǰ���ʱ��Tim���趨Ƶ�ʼ����ģ�
                                                                                //lq �������ڼ�����GpsTimCount��û��ʹ��֮ǰ����õ���У׼ֵ����У׼
}
/*******************************************************************************
* ����  : SetTimAdjust10
* ����  : ���ö�ʱ��ÿ10�����phase��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetTimAdjust10(S16 data)
{
//lq	TimAdjust10 = TimAdjust10 + data;//��ԭ���Ļ����ϵ���
    TimAdjust10 = data;
}
/*******************************************************************************
* ����  : SetTimAdjust100
* ����  : ���ö�ʱ��ÿ100�����phase��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetTimAdjust100(S16 data)
{
//lq	TimAdjust100 = TimAdjust100 + data;//��ԭ���Ļ����ϵ���
	TimAdjust100 = data;//��ԭ���Ļ����ϵ���
}

/*******************************************************************************
* ����  : CalcTimPhaseAdjust
* ����  : ����Tim��ʱ�����
* ����  : DiffNum 4096���������  PeriodCount ͳ������
* ���  : ��
* ����  : ��
*******************************************************************************/
void CalcTimPhaseAdjust(S32 DiffNum ,U16 PeriodCount)
{
    S16 TimAdjust1_temp;
    S16 TimAdjust10_temp;
    S16 TimAdjust100_temp;

	TimAdjust1_temp = DiffNum / PeriodCount;
	TimAdjust10_temp = (DiffNum * 10 - TimAdjust1_temp * PeriodCount * 10) / PeriodCount;
	TimAdjust100_temp = (DiffNum * 100 - TimAdjust1_temp * PeriodCount * 100 
                         - (TimAdjust10_temp * PeriodCount) * 10) / PeriodCount;

	usart_printf("TimAdjust1_temp  : %d\r\n", TimAdjust1_temp);				
	usart_printf("TimAdjust10_temp : %d\r\n", TimAdjust10_temp);				
	usart_printf("TimAdjust100_temp: %d\r\n", TimAdjust100_temp);	

    /*lq ��Tim��ʱ�����ֵת��Ϊ���ʱ�����ֵ*/
    TransPhaseAdjust(&TimAdjust1_temp, &TimAdjust10_temp, &TimAdjust100_temp);

	if (TimAdjust1_temp < ADJUST_PHASE_MAX_ADJUST 
        && TimAdjust1_temp > -ADJUST_PHASE_MAX_ADJUST)                          //lq RtcPhaseAdjust10_Temp,RtcPhaseAdjust100_Tempȡֵ��Χ[0,9]
	{
		TimAdjust1 = TimAdjust1_temp;
		TimAdjust10 = TimAdjust10_temp;
		TimAdjust100 = TimAdjust100_temp;
	}
}

/*******************************************************************************
* ����  : TransPhaseAdjust
* ����  : ��Tim��ʱ�����ֵת��Ϊ���ʱ�����ֵ
* ����  : pAdjust1:1sУ׼ֵ
          pAdjust10:10sУ׼ֵ
          pAdjust100:100sУ׼ֵ
* ���  : ת�����У׼ֵ
* ����  : ��
*******************************************************************************/
void TransPhaseAdjust(S16 * const pAdjust1, S16 * const pAdjust10, S16 * const pAdjust100)
{
    /*lq 100sУ׼ֵС��-4s*/
    if (*pAdjust100 < -4)
    {
        *pAdjust100 = 10 + *pAdjust100;
        (*pAdjust10)--;
    }
    else if (*pAdjust100 > 4)
    {
        *pAdjust100 = -10 + *pAdjust100;
        (*pAdjust10)++;
    } 

    /*lq 10sУ׼ֵС��-4s*/
    if (*pAdjust10 < -4)
    {
        *pAdjust10 = 10 + *pAdjust10;
        (*pAdjust1)--;
    }
    else if (*pAdjust10 > 4)
    {
        *pAdjust10 = -10 + *pAdjust10;
        (*pAdjust1)++;        
    }            
}
 /*******************************************************************************
* ����  : InitTim2Para
* ����  : ��ʼ����ʱ��2��ز���
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitTim2Para(void)
{
	CCR1_Val = 125; // 48 k
	CCR2_Val = 6000; // 1k
    CCR3_Val = 60; //100k
//	CCR4_Val = 4000;
	
	gsiPhaseCnt = 0;
	gsiSlotCnt = 0;
	gslFrameCnt = 0;
	TimSubCnt1 = 0;//TIM2��ʱ��������
	TimSubCnt2 = 0;
	TimSubCnt3 = 0;
	TimAdjust1 = 0;
	TimAdjust10 = 0;
	TimAdjust100 = 0;
	GpsTimCount = 0;

    TimCnt = 0;
    TimCnt1s = TIM_ADJUST_1S;
    TimCnt10s = TIM_ADJUST_10S;
    TimCnt100s = TIM_ADJUST_100S;
}

/*******************************************************************************
* ����  : InitTim2Para_2
* ����  : ��ʼ����ʱ��2��ز���
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitTim2Para_2(void)
{
	TimSubCnt1 = 0;//TIM2��ʱ��������
	TimSubCnt2 = 0;
	TimSubCnt3 = 0;
}

/*******************************************************************************
* ����  : InitTim2
* ����  : ��ʼ����ʱ��2
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitTim2(void)
{	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	InitTim2Para();

	/* ---------------------------------------------------------------
	TIM2 Configuration: Output Compare Timing Mode:
	TIM2CLK = 2*PCLK1 = 2*PLLCLK/2 = 72MHz, 
	Prescaler = (11)+1, TIM2 counter clock = 72/12 = 6 MHz                      //lq TIM2ʱ�����ü�Clock_Configuration()
	--------------------------------------------------------------- */	
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 65535;                                   //lq TIM_PeriodΪ�Ĵ���ARR��ֵ��
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	
	/* Prescaler configuration */
	TIM_PrescalerConfig(TIM2, 11, TIM_PSCReloadMode_Immediate);                 //lq 12��Ƶ��72MHz/12=6MHz


	/* ---------------------------------------------------------------
	CC1 update rate = TIM2 counter clock / CCR1_Val = 48KHz                 
	--------------------------------------------------------------- */	
	/* Output Compare Timing Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;                                   //lq CCR1_Val=125 6MHz/125=48k
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);	
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);

	/* ---------------------------------------------------------------
	CC1 update rate = TIM2 counter clock / CCR2_Val = 1KHz
	--------------------------------------------------------------- */
	/* Output Compare Timing Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR2_Val;	
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);	
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);

	/* ---------------------------------------------------------------
	CC1 update rate = TIM2 counter clock / CCR3_Val = 1KHz
	--------------------------------------------------------------- */	
	/* Output Compare Timing Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR3_Val;	
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);	
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);

#if 0	
	/* ---------------------------------------------------------------
	CC1 update rate = TIM2 counter clock / CCR4_Val = 
	--------------------------------------------------------------- */
	/* Output Compare Timing Mode configuration: Channel4 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;	
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);	
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Disable);
#endif	
	/* TIM IT enable */
	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);
	
	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);	
}

/*******************************************************************************
* ����  : Tim2Interrupt
* ����  : ��ʱ��2�жϷ������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void Tim2Interrupt(void)
{
    S32 slTemp1 = 0;

	/*lq 48KHZ��ʱ�� */
    if(TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)                              //lq 1/48k �뼴Ϊ1����λ 
	{                                                                           //lq ��Tim2��ʱ��һ����λ�����Ǳ�׼��һ����λ��
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
		
		if (DevDynamicPara.ucLowFreqSta == EQ_FREQSTA_HIG)                      //zjw ��������״̬   //lq Tim2ʱ�����ڹ���״̬����λ����
		{                                                                       //lq Tim2��EQ_FREQSTA_HIG��һֱ������
                                                                                //lq ��EQ_FREQSTA_LOW�¼�Ъ�ԵĿ����ر�
            /*lq ����HighFreq״̬���RTC��һ���жϴ���ǰ����ʹ��RTC��������ʹ��Tim��λ����*/
            if (SleepCtrlPara.ucRtcToTimSwitch != TRUE)			
            {
                //48K�жϿ���
    			TimSubCnt1++;                                                    //lq ����"1s"��ʱ
    			TimSubCnt2++;                                                   //lq ����"10s"��ʱ
    			TimSubCnt3++;                                                   //lq ����"100s"��ʱ
    			
    			if (IsGpsPowerupProtect() == TRUE)                              //lq ������λ����
    			{
    				GpsTimCount++;                                              //lq ��������״̬��Tim��λ����
                    TimCnt++;			
                }
    
    			gsiPhaseCnt++;	                                                //lq ��λ������δ����ǰ��
    						
#if 1           /*lq ʱ��У׼����Ҫ����GPSδ��λʱ*/
    			if (TimSubCnt1 >= TimCnt1s)                                     //zjw Tim��1��48k //lq ��������ֻҪTim�ڹ������õ����ͽ��У�
    			{                                                               //lq ÿTim2��1s����λ������������
    				TimSubCnt1 = 0;
                    if (GpsSynCell.ucGPSSyn_SlotPhaseSynFlag == FALSE)
                    {
                        gsiPhaseCnt -= TimAdjust1;                              //zjw Tim��ʱ��ÿ2�����phase��         
                    }
    			}
    
    			if (TimSubCnt2 >= TimCnt10s)                                    //zjw Tim��10��480k //lq ��������ֻҪTim�ڹ������õ����ͽ��У�
    			{                                                               //lq ÿTim2��10s����λ������������
    				TimSubCnt2 = 0;
                    if (GpsSynCell.ucGPSSyn_SlotPhaseSynFlag == FALSE)
                    {
                        gsiPhaseCnt -= TimAdjust10;                            //zjw Tim2��ʱ��ÿ10�����phase��          
                    }			
                }
    						
    			if (TimSubCnt3 >= TimCnt100s)                                   //zjw Tim��100��4800k //lq ��������ֻҪTim�ڹ������õ����ͽ��У�
    			{                                                               //lq ÿTim2��100s����λ������������
    				TimSubCnt3 = 0;
                    if (GpsSynCell.ucGPSSyn_SlotPhaseSynFlag == FALSE)			//zjw Tim2��ʱ��ÿ50�����phase��
                    {
                        gsiPhaseCnt -= TimAdjust100;                                      
                    }			
    			}
#endif    
    			/*lq ��λ��ʱ϶��֡�Ļ��㣬1280����λΪ1��ʱ϶��2250��ʱ϶Ϊ1֡��1���ӣ�*/
    			if (gsiPhaseCnt >= 1280)
    			{
    				gsiSlotCnt += gsiPhaseCnt / 1280;
                    gsiPhaseCnt %= 1280;
    				if (gsiSlotCnt >= 2250)                                     //2250��ʱ϶Ϊһ��֡��һ֡Ϊ60s��Ϊ1min���˴����Ӽ�������Զ
    				{
    					gslFrameCnt += gsiSlotCnt / 2250; 
                        gsiSlotCnt %= 2250;	
    				}
    			}
                else if (gsiPhaseCnt < 0)
                {
                    slTemp1 = gsiPhaseCnt / 1280;
                    slTemp1--;
                    gsiPhaseCnt += -slTemp1 * 1280;                             //lq ��λ
                    gsiSlotCnt += slTemp1;
                    if (gsiSlotCnt < 0)
                    {
                        slTemp1 = gsiSlotCnt / 2250;
                        slTemp1--;                                                              
                        gsiSlotCnt += -slTemp1 * 2250;                          //lq ��λ
                        gslFrameCnt += slTemp1;
                        if (gslFrameCnt < 0)
                        {
                            gslFrameCnt = 0;
                            gsiSlotCnt = 0;
                            gsiPhaseCnt = 0;
                        }
                    }
                }
    
                /*lq ʱ��У׼��ɺ��ٽ���GPSͬ��������GPS�Ѷ�λʱ */
#if 1            
                /*lq GPSʱ϶��λͬ������*/
                if (GpsSynCell.ucGPSSyn_SlotPhaseSynFlag == TRUE)
                {
        			GpsSynCell.ucGPSSyn_SlotPhaseSynFlag = FALSE;               //lq ÿ�����������һ�Σ���������ʱ��������
#if CONFIG_GPS_SYNCH_HOUR_MIN													//zjw û�н��У��˴��Ѿ�������
                    /*lq Сʱ����ͬ��*/                        				
                    if (GpsSynCell.GPSSyn_HourFrameSynEnable == TRUE)
                    {
                        if (GpsSynCell.ucGPSSyn_HourFrameSynFlag == TRUE)
                        {
                            GpsSynCell.ucGPSSyn_HourFrameSynFlag = FALSE;
                            GpsSynCell.GPSSyn_HourFrameSynEnable = FALSE;       //lq ÿ�ο�����Сʱ����ֻͬ��һ��
                            gslFrameCnt = GpsSynCell.ChkTimeFrame;   
                        }
                    }
#endif
                    /*lq ʱ϶ͬ������*/
                    gsiSlotCnt += GpsSynCell.GPSSyn_SlotOffset;
    				if (gsiSlotCnt >= 2250)
    				{
    					gslFrameCnt += gsiSlotCnt / 2250;
                        gsiSlotCnt %= 2250;
    				}
    				else if (gsiSlotCnt < 0)
    				{
                        slTemp1 = gsiSlotCnt / 2250;
                        slTemp1--;                                                              
                        gsiSlotCnt += -slTemp1 * 2250;                          //lq ��λ
                        gslFrameCnt += slTemp1;
                        if (gslFrameCnt < 0)
                        {
                            gslFrameCnt = 0;
                            gsiSlotCnt = 0;
                            gsiPhaseCnt = 0;
                        }
                    }	
                   
    				/*lq ��λͬ������*/
                    gsiPhaseCnt += GpsSynCell.GPSSyn_PhaseOffset;
        			if (gsiPhaseCnt >= 1280)
        			{
        				gsiSlotCnt += gsiPhaseCnt / 1280;
                        gsiPhaseCnt %= 1280;
        				if (gsiSlotCnt >= 2250)                                 //2250��ʱ϶Ϊһ��֡��һ֡Ϊ60s��Ϊ1min���˴����Ӽ�������Զ
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
                
                	TimSubCnt1 = 0;                                              //TIM2��ʱ��������
                	TimSubCnt2 = 0;
                	TimSubCnt3 = 0;
                }
#endif
            }        
		}
        else																	//zjw ����״̬
        {
            /*lq ����LowFreq״̬��δ����ʱ����RTC�жϴ���ǰ����ʹ��Tim��λ����*/
            if (SleepCtrlPara.ucTimToRtcSwitch == TRUE)
            {
#if 1                
                gslTimToRtcCnt_2++;
#endif    			
                //48K�жϿ���
    			TimSubCnt1++;                                                    //lq ����"1s"��ʱ
    			TimSubCnt2++;                                                   //lq ����"10s"��ʱ
    			TimSubCnt3++;                                                   //lq ����"100s"��ʱ
    			
    			gsiPhaseCnt++;	                                                //lq ��λ������δ����ǰ��
    						
#if 1           /*lq ʱ��У׼����Ҫ����GPSδ��λʱ*/
    			if (TimSubCnt1 >= TimCnt1s)                                     //zjw Tim��1��48k //lq ��������ֻҪTim�ڹ������õ����ͽ��У�
    			{                                                               //lq ÿTim2��1s����λ������������
    				TimSubCnt1 = 0;
                    if (GpsSynCell.ucGPSSyn_SlotPhaseSynFlag == FALSE)
                    {
                        gsiPhaseCnt -= TimAdjust1;                              //zjw Tim��ʱ��ÿ2�����phase��        
                    }					
    			}
    
    			if (TimSubCnt2 >= TimCnt10s)                                    //zjw Tim��10��480k //lq ��������ֻҪTim�ڹ������õ����ͽ��У�
    			{                                                               //lq ÿTim2��10s����λ������������
    				TimSubCnt2 = 0;
                    if (GpsSynCell.ucGPSSyn_SlotPhaseSynFlag == FALSE)
                    {
                        gsiPhaseCnt -= TimAdjust10;                             //zjw Tim��ʱ��ÿ102�����phase��         
                    }			
                }
    						
    			if (TimSubCnt3 >= TimCnt100s)                                   //zjw Tim��100��4800k //lq ��������ֻҪTim�ڹ������õ����ͽ��У�
    			{                                                               //lq ÿTim2��100s����λ������������
    				TimSubCnt3 = 0;
                    if (GpsSynCell.ucGPSSyn_SlotPhaseSynFlag == FALSE)
                    {
                        gsiPhaseCnt -= TimAdjust100;                            //zjw Tim��ʱ��ÿ50�����phase��          
                    }		
    			}
#endif
    
    			/*lq ��λ��ʱ϶��֡�Ļ��㣬1280����λΪ1��ʱ϶��2250��ʱ϶Ϊ1֡��1���ӣ�*/
    			if (gsiPhaseCnt >= 1280)
    			{
    				gsiSlotCnt += gsiPhaseCnt / 1280;
                    gsiPhaseCnt %= 1280;
    				if (gsiSlotCnt >= 2250)                                     //2250��ʱ϶Ϊһ��֡��һ֡Ϊ60s��Ϊ1min���˴����Ӽ�������Զ
    				{
    					gslFrameCnt += gsiSlotCnt / 2250; 
                        gsiSlotCnt %= 2250;	
    				}
    			}
                else if (gsiPhaseCnt < 0)
                {
                    slTemp1 = gsiPhaseCnt / 1280;
                    slTemp1--;
                    gsiPhaseCnt += -slTemp1 * 1280;                             //lq ��λ
                    gsiSlotCnt += slTemp1;
                    if (gsiSlotCnt < 0)
                    {
                        slTemp1 = gsiSlotCnt / 2250;
                        slTemp1--;                                                              
                        gsiSlotCnt += -slTemp1 * 2250;                          //lq ��λ
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
            else
            {
                gslTimToRtcIdleCnt_2++;
            }                    
        }
	
		capture = TIM_GetCapture1(TIM2);
		TIM_SetCompare1(TIM2, capture + CCR1_Val);		  		
	}

	/*lq 1KHZ��ʱ�� */
    else if(TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)	                        //lq 1ms��ʱ��ÿ1ms����һ��
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);

		if (DevDynamicPara.ucLowFreqSta == EQ_FREQSTA_HIG)						//zjw ��������״̬
		{
			Is_Feed_Dog = TRUE;                                                 //lq  EQ_FREQSTA_HIG״̬�£���1KHz��Timι����EQ_FREQSTA_LOW״̬�£���RTCι��

			ADC1Start();
	
			LocalClockTick.Time5msCnt++;
	
			if (LocalClockTick.Time5msCnt >= 5)
			{
				LocalClockTick.Time5msCnt = 0;
				LocalClockTick.TimeIntFlg = TRUE;                               //lq ÿ5msʹ�ܱ���ʱ�Ӽ���һ��
			}
		}


           //========================= 
           #if  CONFIG_PTT_EN	// lnw  create
              PTT_Timer_Service();
           #endif
           //=========================


		
	   	
		capture = TIM_GetCapture2(TIM2);
		TIM_SetCompare2(TIM2, capture + CCR2_Val);
	}
	
	if(TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);		

        /*lq GPIOSPI ʱ�ӷ�Ƶ���� */
        GPIOSPI_InstClockCount(GPIOSPI1);

		capture = TIM_GetCapture3(TIM2);
		TIM_SetCompare3(TIM2, capture + CCR3_Val);	
	}
#if 0
	else
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);

		capture = TIM_GetCapture4(TIM2);
		TIM_SetCompare4(TIM2, capture + CCR4_Val);
	}
#endif
}

/*******************************************************************************
* ����  : GetPhaseNum
* ����  : ��ȡ��λ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
S16 GetPhaseNum(void)
{
	return (gsiPhaseCnt);	
}

/*******************************************************************************
* ����  : GetSlotNum
* ����  : ��ȡʱ϶��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
S16 GetSlotNum(void)
{
	return (gsiSlotCnt);	
}

/*******************************************************************************
* ����  : GetFrameNum
* ����  : ��ȡ֡��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
S32 GetFrameNum(void)
{
	return (gslFrameCnt);	
}

/*******************************************************************************
* ����  : GetSlotSum
* ����  : ��ȡ��ǰ��ʱ϶��������
* ����  : ��
* ���  : ��
* ����  : ��ǰ��ʱ϶��������
*******************************************************************************/
S32 GetSlotSum(void)
{
    U32 ulFrameNumCurr;
    U16 usSlotNumCurr;
    U32 ulSlotSumCurr;                                                              

    ulFrameNumCurr = GetFrameNum();
    usSlotNumCurr = GetSlotNum();
    ulSlotSumCurr = ulFrameNumCurr * 2250 + usSlotNumCurr; 
    
    return ulSlotSumCurr;   
}

/*******************************************************************************
* ����  : SetSlotTimer
* ����  : ʹ��ʱ϶����ʱ
* ����  : ��
* ���  : ��
* ����  : ʱ϶��ʱֵ
*******************************************************************************/
S32 SetSlotTimer(U16 inTimeInterval)
{
    U32 ulSlotSumCurr;                                                              

    ulSlotSumCurr = GetSlotSum(); 
    
    return (ulSlotSumCurr + inTimeInterval);   
}

/*******************************************************************************
* ����  : ResetTimeCount
* ����  : ��λʱ��Ĵ���
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void ResetTimeCount(void)
{
	gslFrameCnt = 0;
	gsiSlotCnt = 0;
	gsiPhaseCnt = 0;
}	

/*******************************************************************************
* ����  : SetFSKDataXmtEn
* ����  : ʹ�ܵ������ݷ���ʹ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetFSKDataXmtEn(void)
{
    gucFSKDataXmtEn = TRUE;
}

/*******************************************************************************
* ����  : ResetFSKDataXmtFlg
* ����  : ��λ�������ݷ���ʹ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void ResetFSKDataXmtEn(void)
{
    gucFSKDataXmtEn = FALSE;
}	

/*******************************************************************************
* ����  : GetFSKDataXmtEn
* ����  : ��ȡ�������ݷ���ʹ��
* ����  : ��
* ���  : ��
* ����  : �������ݷ���ʹ��״̬
*******************************************************************************/
U8 GetFSKDataXmtEn(void)
{
    return gucFSKDataXmtEn;
}

/*******************************************************************************
* ����  : GetFSKDataXmtSleepState
* ����  : ��ȡ�������ݷ���ģ���Ƿ�����MCU��������
* ����  : ��
* ���  : ��
* ����  : �Ƿ�����
*******************************************************************************/
U8 GetFSKDataXmtSleepState(void)
{
    if (!GetFSKDataXmtEn())
    {
        return TRUE;    
    }
    else
    {
        return FALSE; 
    }
}

/*******************************************************************************
* ����  : SetFetchFirstFskDataEn
* ����  : ʹ�ܴӵ��ƶ�����ȡ��һ����������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetFetchFirstFskDataEn(void)
{
    gucFetchFirstFskDataEn = TRUE;
}

/*******************************************************************************
* ����  : ResetFetchFirstFskDataEn
* ����  : ʧ�ܴӵ��ƶ�����ȡ��һ����������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void ResetFetchFirstFskDataEn(void)
{
    gucFetchFirstFskDataEn = FALSE;
}

/*******************************************************************************
* ����  : GetFetchFirstFskDataEn
* ����  : ��ѯ״̬
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetFetchFirstFskDataEn(void)
{
    return gucFetchFirstFskDataEn;
}
