/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: stm32f10x_target.c
  Author: ��ΰ       Version : V1.00        Date: 2011.09.23
  Description:    STM32оƬ�������
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x_starup.s"
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "migrate.h"
#include "def.h"
#include "power.h"
#include "Adc.h"
#include "DAC.h"
#include "usart.h"
#include "Tim.h"
#include "LED.h"
#include "24c02.h"
#include "gps.h"
#include "wdg.h"
#include "gps.h"
#include "rtc.h"
#include "powerup.h"
#include "gpsctrl.h"
#include "parastorage.h"
#include "sotdma.h"
#include "RxTx.h"
#include "config.h"
#include "Include.h"
#include  "misc.h"
#include "sleepctrl.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;
void RCC_GetHSE_Value(vu32 User_HSE_Value);
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/

/*******************************************************************************
* ����  : RCC_Configuration
* ����  : stm32f10xʱ��ϵͳ����
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void RCC_Configuration(void)	
{
	/* RCC system reset(for debug purpose) */
	RCC_DeInit();
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);
	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	if(HSEStartUpStatus == SUCCESS)
	{
		/* Enable Prefetch Buffer */
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);		
		/* Flash 2 wait state */
		FLASH_SetLatency(FLASH_Latency_2);

		/* HCLK = SYSCLK */
		RCC_HCLKConfig(RCC_SYSCLK_Div1); 
		
		/* PCLK2 = HCLK */
		RCC_PCLK2Config(RCC_HCLK_Div1); 
		
		/* PCLK1 = HCLK/2 */
		RCC_PCLK1Config(RCC_HCLK_Div2);
		
		/* ADCCLK = PCLK2/4 */
		RCC_ADCCLKConfig(RCC_PCLK2_Div4);
		 
		/* PLLCLK = 12MHz * 6 = 72 MHz */
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);
		
		/* Enable PLL */ 
		RCC_PLLCmd(ENABLE);
		
		/* Wait till PLL is ready */
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
		
		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		
		/* Wait till PLL is used as system clock source */
		while(RCC_GetSYSCLKSource() != 0x08);
	}
	
	/* Enable GPIOx and AFIO clocks */                                          
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |        //lq ��Clock_Configuration()�е������ظ�
	                       RCC_APB2Periph_GPIOC , ENABLE);
	
}

/***************************************************************************************
** ��������: NVIC_Configuration
** ��������: stm32f10x���û��������ж�
** ��    ��: None
** �� �� ֵ: None       
** ����  ��: 
** ��  ����: 
**--------------------------------------------------------------------------------------
** �� �� ��: 
** �ա�  ��: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
static void NVIC_Configuration(void)
{ 
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Set the Vector Table base location at 0x08000000 */ 
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	
	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/* Enable the USART1 Interrupt */                                           //lq USB�����ж�
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable the USART2 Interrupt */                                           //lq GPS�����ж�
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the TIM2 gloabal Interrupt */                                     //lq TIM2��ʱ���ж�
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   //lq 3;   //lq 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //lq 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);
#if 0
	/* Enable the ADC1_2 gloabal Interrupt */                                   //lq ��ص������ADC�ж�
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);
#endif
	/* Enable the DMA1 gloabal Interrupt */                                     //lq [2013/10/30]��ص������DMA�ж�
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);

	/* Configure and enable EXTI1 interrupt --------------------------------*/  //lq GPS�������ж�
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

	/* Configure and enable RTC interrupt -----------------------------------*/ //lq RTC��ʱ���ж�
	NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);
#if CONFIG_KEY
	/* Configure and enable EXTI15_10 interrupt -----------------------------*/ //lq power���ж�
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

	/* Configure and enable EXTI15_10 interrupt -----------------------------*/ //lq sos���ж�
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif
#if 0
    /* Configure and enable SPI2 interrupt -------------------------------------*/
    NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);    	
#endif
	/* Enable the SPIRIT1 IRQ Interrupt */                                       //lq SPIRIT1�ж�
#if 0
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

	/* Configure and enable EXTI15_10 interrupt -----------------------------*/ //lq Si446x �շ�����ʱ���ж�
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

#if CONFIG_SD_CARD_MODULE_EN
	/* Enable the SDIO Interrupt */                                           //lq SDIO�ж�,��SD���ж�
	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

}

/*******************************************************************************
* ����  : EnableRtcInterrupt
* ����  : ʹ��RTC�жϡ���ֹRTC�ж�Ƶ����ɿ����ӳ�
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void EnableRtcInterrupt(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
	

/*******************************************************************************
* ����  : Clock_Configuration
* ����  : ����ʵ����Ҫ�����͹ر�����ʱ�ӣ��Խ��͹��ġ�
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void Clock_Configuration(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SRAM
							| RCC_AHBPeriph_FLITF 
							| RCC_AHBPeriph_CRC
							| RCC_AHBPeriph_SDIO, DISABLE);
	/* Enable DMA1 clock */
  	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2, DISABLE);
  	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* Enable the FSMC Clock */	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, DISABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ALL, DISABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO							
							| RCC_APB2Periph_GPIOA
							| RCC_APB2Periph_GPIOB
							| RCC_APB2Periph_GPIOC
							| RCC_APB2Periph_GPIOD
							| RCC_APB2Periph_GPIOE
							| RCC_APB2Periph_GPIOF
							| RCC_APB2Periph_GPIOG
							| RCC_APB2Periph_ADC1
							| RCC_APB2Periph_USART1
							, ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_ALL, DISABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2
							| RCC_APB1Periph_BKP
							| RCC_APB1Periph_DAC 
							| RCC_APB1Periph_USART2
                            | RCC_APB1Periph_SPI2
							| RCC_APB1Periph_I2C1
                            , ENABLE);


}

/*******************************************************************************
* Function Name  : SYSCLKConfig_STOP
* Description    : Configures system clock after wake-up from STOP: enable HSE, PLL
*                  and select PLL as system clock source.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SYSCLKConfig_STOP(void)
{
	ErrorStatus HSEStartUpStatus;
	
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);
	
	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if(HSEStartUpStatus == SUCCESS)
	{
		/* Enable PLL */ 
		RCC_PLLCmd(ENABLE);
		
		/* Wait till PLL is ready */
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}
		
		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		
		/* Wait till PLL is used as system clock source */
		while(RCC_GetSYSCLKSource() != 0x08)
		{
		}
	}

    /*lq ʱ��У׼��ʱ��0*/
//    InitTim2Para_2();
#if 0
    /*lq ���RTC�Ĵ���ͬ����־λ*/
    RTC_WaitForLastTask();
    RTC_ClearFlag(RTC_FLAG_RSF);
    RTC_WaitForLastTask();   //20131017
#endif

}

/*******************************************************************************
* ����  : EnterStopMod
* ����  : ����ʡ��״̬
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void EnterStopMod(void)
{
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	//PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);//�򿪵�ѹת����
	/* Configures system clock after wake-up from STOP: enable HSE, PLL and select 
	   PLL as system clock source (HSE and PLL are disabled in STOP mode) */
	SYSCLKConfig_STOP();
}

/*******************************************************************************
* ����  : EnterStandbyMod
* ����  : �������״̬
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void EnterStandbyMod(void)
{
	#if 0
	U8 ucTemp[10];
	U8 ucTemp1[10];
  #endif
	
    /*lq �����������������ݼĴ���*/        
    SaveBKP_DRPara();
    SaveBKP_DRData(); 

    // usart_printf("ssAlarmValue : %ld\r\n",struRtcPara.ssRtcAlarmValue);				                    
#if 0
    /*lq ����GPS�ɹ���λ������GPS����3�β���λ�Ĵ�����GPS5min����������EEPROM */
	sprintf((char *)ucTemp, "%5d", GpsCtrlPara.GpsFixCount);
   	Para_Save(PARA_GPS_FIX_COUNT_TYPE, ucTemp, 5);

	sprintf((char *)ucTemp, "%3d", GpsCtrlPara.GpsWaitCount);
   	Para_Save(PARA_GPS_WAIT_COUNT_TYPE, ucTemp, 3);

	sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);

    /*lq ��¼���ο�������ʱ��*/
    sprintf(ucTemp, "%5d", (U16)gslFrameCnt);
    Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTemp, 5); 

    /*lq ÿ�ν������ǰ����һ�ιػ�ʱ��*/
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
		
		Para_Save(PARA_TIME_CLOSE_TYPE, ucTemp, PARA_TIME_CLOSE_MAXLEN);

#if CONFIG_PARASTORAGE_BTT
        /*lq ��¼����֡�����Ĺ���ʱ��*/
		sprintf((char *)ucTemp,"%5d",GetFrameNum());
		Para_Save(PARA_BATTERY_TEST_TIME_TYPE,ucTemp,5);
#endif
	} 
#endif

#if 1
    /* Generate a system reset */
    NVIC_GenerateSystemReset();                                                 //lq �ص�IWDG
#else  

    PWR_WakeUpPinCmd(ENABLE);                                                   //lq ʹ��wakeup pin
   
	PWR_EnterSTANDBYMode();
#endif
}


/*******************************************************************************
* ����  : Target_Periph_Init
* ����  : ��������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void MCU_DebugConfig(void)
{
    DBGMCU_Config(DBGMCU_STANDBY | DBGMCU_STOP, ENABLE);    
}

/*******************************************************************************
* ����  : Target_Clock_Init
* ����  : ϵͳʱ������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void Target_SysClock_Init (void)
{
	//�����ⲿ����Ϊ12.288MHZ
	//RCC_GetHSE_Value(12000000);                                                 //lq?
	
	/* System Clocks Configuration */                                           //lq ����HSE PLL SYSCLK HCLK PCLK1 PCLK2 ADCCLK; 
	RCC_Configuration();	                                                    //lq ͬʱEnable GPIOx and AFIO clocks

	/*lq DebugMCU Configuration*/
	MCU_DebugConfig();
}

/*******************************************************************************
* ����  : Target_Periph_Init
* ����  : ��������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void Target_Periph_Init (void)
{

	//����ʵ����Ҫ�����͹ر�����ʱ�ӣ��Խ��͹��ġ�
	Clock_Configuration();                                                      //lq �����͹ر�����ʱ��

	//����ʵʱʱ��RTC
	//RTC_Configuration();
    
	RTCConfigurationPart1();                                                    //lq ����RTCAlarm�жϣ�Enable PWR and BKP clocks������LSEʱ��    

	NVIC_DeInit();




  
#if 1
	Initia_wdg();//���Ź�
#endif	
	InitiaLED();//��ʼ��led                                                     //lq ����LED�˿�����
	
	InitCommStruc(COMM1, 38400);                                                //lq ����USB����
	InitCommStruc(COMM2, 9600);                                                 //lq ����GPS����
#if CONFIG_KEY	
    Initia_POWER_Crl();//��ʼ����Դ                                             //lq ���õ�Դ���ض˿�����
#endif
	InitTim2();//��ʼ����ʱ��                                                   //lq ���ö�ʱ��Tim2

#if CONFIG_ADC1
    ADC1_PerInit();    //lq AIS Tranceiver����
#endif
	I2C_EE_Init();//��ʼ��i2c                                                   //lq ����EEPROM(CAT24C02)
	
#if 0
	DAC_RegInit();//��ʼ��DAC                                                   //lq ����DACͨ��2
#endif
#if 0
	Init_RDA1846();//��ʼ��1846                                                 //lq ����RDA1846
#endif
	Initia_RFPower();                                                           //lq ���ù��ŵ�Դ�������ţ���ʼ�����Źرգ�

    InitRxTx();
#if 0
    SPIRIT1_Init();
#endif





	//========================================
#if  CONFIG_PTT_EN		 //  lnw   create
	  PTT_Init_IO();
	  PTT_OBJ_INIT();
#endif
	//========================================







    SI446X_ModuleInit();
	
    /* NVIC configuration */
	NVIC_Configuration();                                                       //lq ���ø����жϣ��ж�Դ���ж����ȼ����ж�ʹ�ܣ�
#if 0
    /*lq ����SPIRIT1 */
    SPIRIT1_InstConfiguration();
#endif
}
/***************************************************************************************
** ��������: Target_Init
** ��������: �弶��ʼ������
** ��    ��: None
** �� �� ֵ: None       
** ����  ��: 
** ��  ����: 
**--------------------------------------------------------------------------------------
** �� �� ��: 
** �ա�  ��: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
void Target_Init (void)
{
   /*lq ϵͳʱ������*/
    Target_SysClock_Init();
    
    /*lq ��������*/
    InitPowerCtrl();
    Target_Periph_Init();
}




/*******************************************************************************
* ����  : BSP_CPU_ClkFreq
* ����  : ��ȡRCCƵ��
* ����  : ��
* ���  : ��
* ����  : RCCƵ��
*******************************************************************************/
u32 BSP_CPU_ClkFreq(void)
{
    static RCC_ClocksTypeDef rcc_clocks;

    RCC_GetClocksFreq(&rcc_clocks);

    return ((u32)rcc_clocks.HCLK_Frequency);
}

#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

