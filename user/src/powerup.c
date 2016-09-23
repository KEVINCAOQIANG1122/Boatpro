/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: powerup.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    ����ģ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "def.h"
#include "initial.h"
#include "debug.h"
#include "keyscan.h"
#include "powerup.h"
#include "ledctrl.h"
#include "ioctrl.h"
#include "common.h"
#include "debug.h"
#include "parastorage.h"
#include "wdg.h"
#include "BKPRegister.h"
#include "gpsctrl.h"
#include "sotdma.h"
#include "rtc.h"
#include "tim.h"
#include "adjust.h"
#include "mmi.h"
#include "PTL_Main.h"
#include "gmskmod.h" 
#include "led.h"
#include "Include.h"
#include "flashctrl.h"

/* Private variables ---------------------------------------------------------*/
U8 ucStandbyWakeupFlg = FALSE;
U8 ucStandbyWakeupDelayFlg = FALSE;

/* Private function prototypes -----------------------------------------------*/  
void PowerUpLedCtrl(void);

/*******************************************************************************
* ����  : InitPowerUppara
* ����  : ��ʼ������ģ���Ҫ�Ĳ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitPowerUppara(void)
{	             

}

#if 0
/*******************************************************************************
* ����  : PowerUpProcess
* ����  : ����ģ�飬����������ʽ��ѯ����״̬�������Ƶ�Դ���أ�������Ӧ�Ĺ���״̬
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void PowerUpProcess(void)
{
	U8 ucPowerKey;
	U8 ucSOSKey;
	U8 i;
	char chTemp[100];
    U8 ucTempBuf[40];
	U16 PowerEnableCount;
	U16 SosEnableCount;
	U16 usTemp;

	U16 PowerKeyEnableValue;//power��������������ֵ
	U16 SosKeyEnableValue;//sos��������������ֵ

	PowerEnableCount = 0;
	SosEnableCount = 0;

	PowerKeyEnableValue = eqEquipPara.PowerOpenTime * 100;//ÿ10ms���һ��
	SosKeyEnableValue = eqEquipPara.SosOpenTime * 100;

	/* Check if the system has resumed from IWDG reset */
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)//����ǹ���λ����ֱ�ӿ���
	{
		/* Clear reset flags */
    	RCC_ClearFlag();
	
		IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);
		eqEquipPara.ucWorkMode = EQ_WORKMOD_NORMAL;

		for (i = 0; i < 3; i++)
		{
#if LED_EN_HIGH
			IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
			IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#else
			IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
			IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#endif
			Delay_ms(200);
			Is_Feed_Dog = TRUE;
			Feed_Dog();//ι��						
#if LED_EN_HIGH
			IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
			IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#else
			IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
			IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#endif
			Delay_ms(200);
			Is_Feed_Dog = TRUE;
			Feed_Dog();//ι��			
		}

		UartResponseMsg("$P712,001,,,Powerup By IWDG!");
		//�˴�����LED��������
		LEDInit(LED_TYPE_GPS_SEARCH);

        /*lq �����������������ڿ��Ź���λ������1*/
        Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
        usTemp = atol((char *)ucTempBuf);
        usTemp = ++usTemp > PARA_WDG_RESET_CNT_CEIL ? PARA_WDG_RESET_CNT_CEIL : usTemp;
        sprintf((char*)ucTempBuf, "%2d", usTemp);
		Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//��
	}
	else
	{
	
		while (1)
		{
			ucPowerKey = ReadSingleKeyValProcess(&PowerKey);
			ucSOSKey = ReadSingleKeyValProcess(&SOSKey);
	
			if (ucPowerKey == KEY_LOW)
			{
				if (PowerEnableCount < PowerKeyEnableValue)//����ʱ��δ���Ҳ��Ǹ�λ������
				{
					PowerEnableCount++;
					SosEnableCount = 0;
					
					Is_Feed_Dog = TRUE;
					Feed_Dog();//ι��
	
					Delay_ms(10);//10ms���һ��
				}
				else//�ﵽ��ֵ�����Ǹ�λ����
				{
					//���ƿ�����Դ��ͬʱ�õ�ǰΪ��������״̬
					IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);
					eqEquipPara.ucWorkMode = EQ_WORKMOD_NORMAL;
		
					for (i = 0; i < 3; i++)
					{
#if LED_EN_HIGH
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#else
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#endif
						Delay_ms(200);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//ι��						
#if LED_EN_HIGH	
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#else
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#endif
						Delay_ms(200);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//ι��							
					}
		
					UartResponseMsg("$P712,001,,,Powerup By PowerKey!");
					//�˴�����LED��������
					LEDInit(LED_TYPE_GPS_SEARCH);
                	
                    /*lq �����ϴ��������������������������Ź���λ����*/
                    Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
					Para_Save(PARA_LAST_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//��
                    
                    /*lq ���ο��������ڿ��Ź���λ������0*/
                    sprintf((char*)ucTempBuf, "%2d", 0);
					Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//��

                    /*lq ��������������1*/
                    Para_Read(PARA_NORMAL_POWER_ON_CNT_TYPE, ucTempBuf);
                    usTemp = atol((char *)ucTempBuf);
                    usTemp = ++usTemp > PARA_NORMAL_POWER_ON_CNT_CEIL ? PARA_NORMAL_POWER_ON_CNT_CEIL : usTemp;
                    sprintf((char*)ucTempBuf, "%2d", usTemp);
            		Para_Save(PARA_NORMAL_POWER_ON_CNT_TYPE, (U8 *)ucTempBuf, 2);//��
					break;
				}
			}
			else if (ucSOSKey == KEY_LOW)
			{
				if (SosEnableCount < SosKeyEnableValue)
				{
					SosEnableCount++;
					PowerEnableCount = 0;
	
					Is_Feed_Dog = TRUE;
					Feed_Dog();//ι��
	
					Delay_ms(10);//10ms���һ��
				}
				else
				{
					//���ƿ�����Դ��ͬʱ�õ�ǰΪ��������״̬
					IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);
					eqEquipPara.ucWorkMode = EQ_WORKMOD_TEST;
                    eqEquipPara.TestSta = EQ_MOD_TEST_STA_MSG;

					for (i = 0; i < 3; i++)
					{
#if LED_EN_HIGH
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_HIGH);
#else
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_LOW);
#endif
						Delay_ms(250);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//ι��						
	
#if LED_EN_HIGH						
                        IO_Ctrl(IO_RED2_TYPE, IO_VAL_LOW);
#else
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_HIGH);
#endif
						Delay_ms(750);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//ι��							
					}
				
					UartResponseMsg("$P712,001,,,Powerup By TestKey!");
					//�˴�����LED��������
					
					//LEDInit(LED_TYPE_TEST_STARTUP);
                    
                    /*lq �����ϴ��������������������������Ź���λ����*/
                    Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
					Para_Save(PARA_LAST_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//��
                    
                    /*lq ���ο��������ڿ��Ź���λ������0*/
                    sprintf((char*)ucTempBuf, "%2d", 0);
					Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//��	
                    
                    /*lq �û����Դ�����1*/
                    Para_Read(PARA_USER_TEST_CNT_TYPE, ucTempBuf);
                    usTemp = atol((char *)ucTempBuf);
                    usTemp = ++usTemp > PARA_USER_TEST_CNT_CEIL ? PARA_USER_TEST_CNT_CEIL : usTemp;
                    sprintf((char*)ucTempBuf, "%2d", usTemp);
            		Para_Save(PARA_USER_TEST_CNT_TYPE, (U8 *)ucTempBuf, 2);//��                    				
					break;
				}
			}
		}
	}
	
	UartResponseMsg("$P712,001,,,--------------------------------------");
	UartResponseMsg("$P712,001,,,-- AIS Transponder");
	sprintf(chTemp, "$P712,001,,,-- SW Ver %s", VERSION);
	UartResponseMsg(chTemp);
	sprintf(chTemp, "$P712,001,,,-- HW Ver %s", VERSION_HW);
	UartResponseMsg(chTemp);
	sprintf(chTemp, "$P712,001,,,-- Last compiled: %s %s", __DATE__, __TIME__);
	UartResponseMsg(chTemp);
	UartResponseMsg("$P712,001,,,--------------------------------------");
}
#endif

#if 0
/*******************************************************************************
* ����  : PowerUpProcess
* ����  : ����ģ�飬�°���λ�Ƿ�ʽ��������λ�����������ϵ縴λ�������п��Ź���λ��
          �����п��Ź���λ���������Ѹ�λby Wakeup pin ���������Ѹ�λby RTC Alarm��
          Reset Pin��λ�������ã�
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void PowerUpProcess(void)
{
	U8 ucPowerKey;
	U8 ucSOSKey;
	U8 i;
	char chTemp[100];
    U8 ucTempBuf[40];
	U16 PowerEnableCount;
	U16 SosEnableCount;
	U16 usTemp;
    U16 usTemp2;
	U16 PowerKeyEnableValue;//power��������������ֵ
	U16 SosKeyEnableValue;//sos��������������ֵ

	PowerEnableCount = 0;
	SosEnableCount = 0;
#if 0
    /*lq ��Դʱ�Ӵ�*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);                             //lq ��ȡ�������ݼĴ���ʱ��Ҫ
    
    if ((RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
        && (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET))
    {
        /* Clear reset flags */
    	RCC_ClearFlag();
                    
        PWR_WakeUpPinCmd(ENABLE);                                                   //lq ʹ��wakeup pin
        PWR_EnterSTANDBYMode();                                             //lq ��λ���Ź��Զ��ر�
                
    }
#endif	
    /*lq �豸ϵͳʱ�ӳ�ʼ��*/
    Target_SysClock_Init();

    /*lq ��Դʱ�Ӵ�*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);                             //lq ��ȡ�������ݼĴ���ʱ��Ҫ
    
    /*lq �ϵ縴λ*/
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)                                //lq �ϵ縴λ    
    {
        /*lq �����־λ*/
        RCC_ClearFlag();

    	/* Reset Backup Domain */
    	BKP_DeInit();  
              
        /*lq �������ݼĴ�����ʼ��*/
        InitBKP_DR();

        /*lq �������ݼĴ������ݳ�ʼ��*/
        InitBKP_DRData();
        
        /*lq �����ʼ��*/
    	InitPowerCtrl();
        IO_Ctrl(IO_POWER_TYPE, IO_VAL_LOW);                                     //lq ��Դ���عرգ������ɹ����ٿ���
        Target_Periph_Init();
        
        /*lq ������ʼ��*/
        Parameter_Init();//��ʼ����ز���                                           //lq ��ʼ�������ṹ��ĳ�Ա������ȫ�ֱ���

    	PowerKeyEnableValue = eqEquipPara.PowerOpenTime * 100;//ÿ10ms���һ��
    	SosKeyEnableValue = eqEquipPara.SosOpenTime * 100;

        UartResponseMsg("$Reset By PowerOn!");		

        /*lq �ϵ翪���������*/
		while (1)
		{
			ucPowerKey = ReadSingleKeyValProcess(&PowerKey);
			ucSOSKey = ReadSingleKeyValProcess(&SOSKey);
	
			if (ucPowerKey == KEY_LOW)
			{
				if (PowerEnableCount < PowerKeyEnableValue)//����ʱ��δ���Ҳ��Ǹ�λ������
				{
					PowerEnableCount++;
					SosEnableCount = 0;
					
					Is_Feed_Dog = TRUE;
					Feed_Dog();//ι��
	
					Delay_ms(10);//10ms���һ��
				}
				else//�ﵽ��ֵ�����Ǹ�λ����
				{
					/*lq ���ƿ�����Դ��ͬʱ�õ�ǰΪ��������״̬*/
					IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);
					eqEquipPara.ucWorkMode = EQ_WORKMOD_NORMAL;
		
                    /*lq ��������LEDָʾ*/
					for (i = 0; i < 3; i++)
					{
#if LED_EN_HIGH
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#else
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#endif
						Delay_ms(200);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//ι��						
#if LED_EN_HIGH	
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#else
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#endif			
            			Delay_ms(200);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//ι��							
					}
		
					UartResponseMsg("$P712,001,,,Powerup By PowerKey!");
                	
                    /*lq ��¼�ۼ��ܹ���ʱ��*/
                    Para_Read(PARA_WORKTIME_TYPE, ucTempBuf);               //lq �ϴο���֮ǰ������ʱ��
                    usTemp = atol((char *)ucTempBuf);
                    Para_Read(PARA_WORKTIME_LAST_TYPE, ucTempBuf);          //lq �ϴο���֮����ʱ��
                    usTemp2 = atol((char *)ucTempBuf);
                    usTemp += usTemp2;                                      //lq ���㱾�ο���֮ǰ������ʱ��
                    if (usTemp < PARA_WORKTIME_FLOOR)
                    {
                        usTemp = PARA_WORKTIME_FLOOR;
                    }
                
                    if (usTemp > PARA_WORKTIME_CEIL)
                    {
                        usTemp = PARA_WORKTIME_CEIL;
                    } 
                    sprintf((char*)ucTempBuf, "%5d", usTemp);
                	Para_Save(PARA_WORKTIME_TYPE, (U8 *)ucTempBuf, 5);      //lq �汾�ο���֮ǰ������ʱ��
                    eqEquipPara.WorkTimeSum = usTemp;
                    sprintf((char*)ucTempBuf, "%5d", 0);
					Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTempBuf, 5); //lq �ϴο���֮����ʱ����0����ֹ�´ο������ظ��ۼ� 

                    /*lq �����ϴ��������������������������Ź���λ����*/
                    Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
					Para_Save(PARA_LAST_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//��
                    
                    /*lq ���ο��������ڿ��Ź���λ������0*/
                    sprintf((char*)ucTempBuf, "%2d", 0);
					Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//��
#if 0
                    /*lq ��������������1*/
                    Para_Read(PARA_NORMAL_POWER_ON_CNT_TYPE, ucTempBuf);
                    usTemp = atol((char *)ucTempBuf);
                    usTemp = ++usTemp > PARA_NORMAL_POWER_ON_CNT_CEIL ? PARA_NORMAL_POWER_ON_CNT_CEIL : usTemp;
                    sprintf((char*)ucTempBuf, "%2d", usTemp);
            		Para_Save(PARA_NORMAL_POWER_ON_CNT_TYPE, (U8 *)ucTempBuf, 2);//��
#endif	
    				break;
				}
			}
			else if (ucSOSKey == KEY_LOW)
			{
				if (SosEnableCount < SosKeyEnableValue)
				{
					SosEnableCount++;
					PowerEnableCount = 0;
	
					Is_Feed_Dog = TRUE;
					Feed_Dog();//ι��
	
					Delay_ms(10);//10ms���һ��
				}
				else
				{
					/*lq ���ƿ�����Դ��ͬʱ�õ�ǰΪ��������״̬*/
					IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);
					eqEquipPara.ucWorkMode = EQ_WORKMOD_TEST;
                    eqEquipPara.TestSta = EQ_MOD_TEST_STA_MAIN;

                    /*lq �û����Կ���LEDָʾ*/
					for (i = 0; i < 3; i++)
					{
#if LED_EN_HIGH
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_HIGH);
#else
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_LOW);
#endif			
            			Delay_ms(250);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//ι��						
#if LED_EN_HIGH
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_LOW);
#else
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_HIGH);
#endif			
            			Delay_ms(750);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//ι��							
					}
				
					UartResponseMsg("$P712,001,,,Powerup By TestKey!");					

                    /*lq ��¼�ۼ��ܹ���ʱ��*/
                    Para_Read(PARA_WORKTIME_TYPE, ucTempBuf);               //lq �ϴο���֮ǰ������ʱ��
                    usTemp = atol((char *)ucTempBuf);
                    Para_Read(PARA_WORKTIME_LAST_TYPE, ucTempBuf);          //lq �ϴο���֮����ʱ��
                    usTemp2 = atol((char *)ucTempBuf);
                    usTemp += usTemp2;                                      //lq ���㱾�ο���֮ǰ������ʱ��
                    if (usTemp < PARA_WORKTIME_FLOOR)
                    {
                        usTemp = PARA_WORKTIME_FLOOR;
                    }
                
                    if (usTemp > PARA_WORKTIME_CEIL)
                    {
                        usTemp = PARA_WORKTIME_CEIL;
                    } 
                    sprintf((char*)ucTempBuf, "%5d", usTemp);
                	Para_Save(PARA_WORKTIME_TYPE, (U8 *)ucTempBuf, 5);      //lq �汾�ο���֮ǰ������ʱ��
                    eqEquipPara.WorkTimeSum = usTemp;                    
                    sprintf((char*)ucTempBuf, "%5d", 0);
					Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTempBuf, 5); //lq �ϴο���֮����ʱ����0����ֹ�´ο������ظ��ۼ� 
                                        
                    /*lq �����ϴ��������������������������Ź���λ����*/
                    Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
					Para_Save(PARA_LAST_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//��
                    
                    /*lq ���ο��������ڿ��Ź���λ������0*/
                    sprintf((char*)ucTempBuf, "%2d", 0);
					Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//��	
#if 0                    
                    /*lq �û����Դ�����1*/
                    Para_Read(PARA_USER_TEST_CNT_TYPE, ucTempBuf);
                    usTemp = atol((char *)ucTempBuf);
                    usTemp = ++usTemp > PARA_USER_TEST_CNT_CEIL ? PARA_USER_TEST_CNT_CEIL : usTemp;
                    sprintf((char*)ucTempBuf, "%2d", usTemp);
            		Para_Save(PARA_USER_TEST_CNT_TYPE, (U8 *)ucTempBuf, 2);//��                    				
#endif	
    				break;
				}
			}
		}
    }
    
    /*lq �����λ���رտ��Ź�*/
    else if (RCC_GetFlagStatus(RCC_FLAG_SFTRST) != RESET)
    {
		/* Clear reset flags */
    	RCC_ClearFlag();   
#if 1
        /*lq ��RTCʱ��ԴΪLSI���������¿���LSI����LSI�ڴ�������ʱ�Ḵλ*/
        /* Allow access to BKP Domain */
        PWR_BackupAccessCmd(ENABLE);

        /*lq ����LSIѡ��RTCʱ��Դ*/
        if ((RCC->BDCR | 0xFFFFFCFF) == 0xFFFFFEFF)
        {
    	 	/* Enable the LSI OSC */
    		RCC_LSICmd(ENABLE);                                             //lq �����ڲ�RC����,�ڴ�������ʱ��LSI�ᱻ��λ
    		
            /* Wait till LSI is ready */
    		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)//������ʱ��λ�˾�˵��LSI�񵴵�·�ڵ���Ҳ����
    		{}             
        }
#endif
        PWR_WakeUpPinCmd(ENABLE);                                                   //lq ʹ��wakeup pin
        PWR_EnterSTANDBYMode();                                                 //lq ��λ���Ź��Զ��ر�         
    }

    /*lq ���Ź���λ*/
    else if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)                      //lq ���Ź���λ
    {
		/* Clear reset flags */
    	RCC_ClearFlag();
	
        if (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET)                            //lq �����п��Ź���λ
        {
#if 1
            /*lq ��RTCʱ��ԴΪLSI���������¿���LSI����LSI�ڴ�������ʱ�Ḵλ*/
            /* Allow access to BKP Domain */
	        PWR_BackupAccessCmd(ENABLE);

            /*lq ����LSIѡ��RTCʱ��Դ*/
            if ((RCC->BDCR | 0xFFFFFCFF) == 0xFFFFFEFF)
            {
        	 	/* Enable the LSI OSC */
        		RCC_LSICmd(ENABLE);                                             //lq �����ڲ�RC����,�ڴ�������ʱ��LSI�ᱻ��λ
        		
                /* Wait till LSI is ready */
        		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)//������ʱ��λ�˾�˵��LSI�񵴵�·�ڵ���Ҳ����
        		{}             
            }
#endif
            PWR_WakeUpPinCmd(ENABLE);                                                   //lq ʹ��wakeup pin
            PWR_EnterSTANDBYMode();                                             //lq ��λ���Ź��Զ��ر�
        }
        else                                                                    //lq �����п��Ź���λ    
        {
        	/* Reset Backup Domain */
        	BKP_DeInit();   

            /*lq �������ݼĴ�����ʼ��*/
            InitBKP_DR();
    
            /*lq �������ݼĴ������ݳ�ʼ��*/
            InitBKP_DRData();
  
            /*lq �����ʼ��*/
        	InitPowerCtrl();
            IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);                                //lq ��Դ���ؿ���
            Target_Periph_Init();
         	
            /*lq ������ʼ��*/
            Parameter_Init();                                                   //lq ��ʼ�������ṹ��ĳ�Ա������ȫ�ֱ���

			/*lq ��������LEDָʾ*/
            for (i = 0; i < 3; i++)
			{
#if LED_EN_HIGH
				IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
				IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#else
				IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
				IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#endif
				Delay_ms(200);
				Is_Feed_Dog = TRUE;
				Feed_Dog();//ι��						
#if LED_EN_HIGH
				IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
				IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#else
				IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
				IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#endif			
            	Delay_ms(200);
				Is_Feed_Dog = TRUE;
				Feed_Dog();//ι��							
			}

            UartResponseMsg("$Reset By IWDG!");		
       		UartResponseMsg("$P712,001,,,Powerup By IWDG!");

            /*lq ��¼�ۼ��ܹ���ʱ��*/
            Para_Read(PARA_WORKTIME_TYPE, ucTempBuf);               //lq �ϴο���֮ǰ������ʱ��
            usTemp = atol((char *)ucTempBuf);
            Para_Read(PARA_WORKTIME_LAST_TYPE, ucTempBuf);          //lq �ϴο���֮����ʱ��
            usTemp2 = atol((char *)ucTempBuf);
            usTemp += usTemp2;                                      //lq ���㱾�ο���֮ǰ������ʱ��
            if (usTemp < PARA_WORKTIME_FLOOR)
            {
                usTemp = PARA_WORKTIME_FLOOR;
            }
        
            if (usTemp > PARA_WORKTIME_CEIL)
            {
                usTemp = PARA_WORKTIME_CEIL;
            } 
            sprintf((char*)ucTempBuf, "%5d", usTemp);
        	Para_Save(PARA_WORKTIME_TYPE, (U8 *)ucTempBuf, 5);      //lq �汾�ο���֮ǰ������ʱ��
            sprintf((char*)ucTempBuf, "%5d", 0);
			Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTempBuf, 5); //lq �ϴο���֮����ʱ����0����ֹ�´ο������ظ��ۼ� 

            /*lq �����������������ڿ��Ź���λ������1*/
            Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
            usTemp = atol((char *)ucTempBuf);
            usTemp = ++usTemp > PARA_WDG_RESET_CNT_CEIL ? PARA_WDG_RESET_CNT_CEIL : usTemp;
            sprintf((char*)ucTempBuf, "%2d", usTemp);
    		Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//��
        }                                                                       
    } 
    
    /*lq �������Ѹ�λ*/       
    else if (PWR_GetFlagStatus(PWR_FLAG_WU) != RESET)                           //lq ��������
    {
        /*lq ��� WUF��־λ*/
        PWR_ClearFlag(PWR_FLAG_WU);
        
        /* Clear the StandBy pending flag */
        PWR_ClearFlag(PWR_FLAG_SB);

        /*lq ��RTCʱ��ԴΪLSI���������¿���LSI����LSI�ڴ�������ʱ�Ḵλ*/
        /* Allow access to BKP Domain */
        PWR_BackupAccessCmd(ENABLE);

        /*lq ����LSIѡ��RTCʱ��Դ*/
        if ((RCC->BDCR | 0xFFFFFCFF) == 0xFFFFFEFF)
        {
    	 	/* Enable the LSI OSC */
    		RCC_LSICmd(ENABLE);                                             //lq �����ڲ�RC����,�ڴ�������ʱ��LSI�ᱻ��λ
    		
            /* Wait till LSI is ready */
    		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)//������ʱ��λ�˾�˵��LSI�񵴵�·�ڵ���Ҳ����
    		{}             
        }  
              
        /*lq ���Ǵ�������������������RTC*/
        ucStandbyWakeupFlg = TRUE;
        
        /*lq �������ѳ�ʼ����ʱ����*/
        ucStandbyWakeupDelayFlg = TRUE;

        /*lq ��ȡ�������ݼĴ�������*/
        GetBKP_DRData();
        
        //lq if (RTC_GetFlagStatus(RTC_FLAG_ALR) != RESET)
        if (IsRtcAlarm())
        {
//            RTC_ClearFlag(RTC_FLAG_RSF);

            /*lq ʧ��wakeup pin*/
            PWR_WakeUpPinCmd(DISABLE);                                                   //lq ʹ��wakeup pin

            /*lq ��ϵͳ����*/
            InitPowerCtrl();      
            IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);                                //lq ��Դ���ؿ���
            Delay_ms(10);//lqdb                                                 //lq ��ʱ������10ms����ϵͳ�ϵ���ɣ��ٶ�1846�������������

            /*lq �豸��ʼ��*/
            Target_Periph_Init();//��ʼ�� arm                                                  //lq ����ARM
        	
            /*lq ������ʼ��*/
            Parameter_Init();//��ʼ����ز���                                           //lq ��ʼ�������ṹ��ĳ�Ա������ȫ�ֱ���
   
            /*lq ���ߴ������ѳ�ʼ�� */
            InitBKP_DRPara();
            SleepWakeUpInit();

            // UartResponseMsg("$Reset By RTCAlarm!");		
            //lq ���߻���
            // UartResponseMsg("$P712,001,,,-- Sleep Out");
    		//lq LEDInit(LED_TYPE_GPS_SEARCH);                                       //lq LED GPS��λ����ָʾ

            return;
        }
        else                                                                    //lq wakeup��������
        {
#if 1 
        	/*lq �豸��ʼ��*/
            InitPowerCtrl();
            IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);                                //lq ��Դ���ؿ���
            Target_Periph_Init();//��ʼ�� arm                                                  //lq ����ARM
        	
            /*lq ������ʼ��*/
            Parameter_Init();//��ʼ����ز���                                           //lq ��ʼ�������ṹ��ĳ�Ա������ȫ�ֱ���
        
        	PowerKeyEnableValue = eqEquipPara.CloseTime * 100;//ÿ10ms���һ��
        	//lq SosKeyEnableValue = eqEquipPara.CloseTime * 100;

            UartResponseMsg("$Reset By Wakeup Pin!");		
			
            /*lq ����ָʾ*/
#if LED_EN_HIGH
            IO_Ctrl(IO_RED1_TYPE, IO_VAL_HIGH);
#else
			IO_Ctrl(IO_RED1_TYPE, IO_VAL_LOW);
#endif
			Delay_ms(100);
#if LED_EN_HIGH			
            IO_Ctrl(IO_RED1_TYPE, IO_VAL_LOW);
#else
            IO_Ctrl(IO_RED1_TYPE, IO_VAL_HIGH);
#endif

            while (1)
    		{ 
			    ucPowerKey = ReadSingleKeyValProcess(&PowerKey);                
                //lq ucSOSKey = ReadSingleKeyValProcess(&SOSKey);  //lq �ػ�������ΪTest��

                if (ucPowerKey == KEY_LOW)
    			{
                    UartResponseMsg("KEY_LOW!");
                    if (PowerEnableCount < PowerKeyEnableValue)//����ʱ��δ���Ҳ��Ǹ�λ������
    				{
                        PowerEnableCount++;
    					//lq SosEnableCount++;
    					
    					Is_Feed_Dog = TRUE;
    					Feed_Dog();//ι��
    	
    					Delay_ms(10);//10ms���һ��
    				}
    				else
    				{
				        UartResponseMsg("$P712,001,,,PowerDown Key Down During Standby!");
		
    					/*lq �����йػ�*/
                        ExecShutdownAction(); //lq ����ʱ��������
    				}
    			}
                else
                {
                    UartResponseMsg("KEY_HIGH!");
#if 1
                    /* Generate a system reset */
                    NVIC_GenerateSystemReset();                                                 //lq �ص�IWDG
#else
                    PWR_WakeUpPinCmd(ENABLE);                                                   //lq ʹ��wakeup pin
                    PWR_EnterSTANDBYMode();
#endif
                }
    		}    

#endif
        }
    }
    
    /*lq JTAG�������Ÿ�λ*/
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)//lqdb nRST Pin ��λ      JTAG����ʱ��λ
    {
        /*lq �����־λ*/
        RCC_ClearFlag();

    	/* Reset Backup Domain */
    	BKP_DeInit();  
              
        /*lq �������ݼĴ�����ʼ��*/
        InitBKP_DR();

        /*lq �������ݼĴ������ݳ�ʼ��*/
        InitBKP_DRData();
        
        /*lq �����ʼ��*/
    	InitPowerCtrl();
        IO_Ctrl(IO_POWER_TYPE, IO_VAL_LOW);                                     //lq ��Դ���عرգ������ɹ����ٿ���
        Target_Periph_Init();
        
        /*lq ������ʼ��*/
        Parameter_Init();//��ʼ����ز���                                           //lq ��ʼ�������ṹ��ĳ�Ա������ȫ�ֱ���

    	PowerKeyEnableValue = eqEquipPara.PowerOpenTime * 100;//ÿ10ms���һ��
    	SosKeyEnableValue = eqEquipPara.SosOpenTime * 100;

        UartResponseMsg("$Reset By nRst Pin!");		

        /*lq �ϵ翪���������*/
		while (1)
		{
			ucPowerKey = ReadSingleKeyValProcess(&PowerKey);
			ucSOSKey = ReadSingleKeyValProcess(&SOSKey);
	
			if (ucPowerKey == KEY_LOW)
			{
				if (PowerEnableCount < PowerKeyEnableValue)//����ʱ��δ���Ҳ��Ǹ�λ������
				{
					PowerEnableCount++;
					SosEnableCount = 0;
					
					Is_Feed_Dog = TRUE;
					Feed_Dog();//ι��
	
					Delay_ms(10);//10ms���һ��
				}
				else//�ﵽ��ֵ�����Ǹ�λ����
				{
					//���ƿ�����Դ��ͬʱ�õ�ǰΪ��������״̬
					IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);
					eqEquipPara.ucWorkMode = EQ_WORKMOD_NORMAL;
		
					for (i = 0; i < 3; i++)
					{
#if LED_EN_HIGH
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#else
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#endif
						Delay_ms(200);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//ι��						
#if LED_EN_HIGH 	
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#else
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#endif			
            			Delay_ms(200);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//ι��							
					}
		
					UartResponseMsg("$P712,001,,,Powerup By PowerKey!");

                    /*lq ��¼�ۼ��ܹ���ʱ��*/
                    Para_Read(PARA_WORKTIME_TYPE, ucTempBuf);               //lq �ϴο���֮ǰ������ʱ��
                    usTemp = atol((char *)ucTempBuf);
                    Para_Read(PARA_WORKTIME_LAST_TYPE, ucTempBuf);          //lq �ϴο���֮����ʱ��
                    usTemp2 = atol((char *)ucTempBuf);
                    usTemp += usTemp2;                                      //lq ���㱾�ο���֮ǰ������ʱ��
                    if (usTemp < PARA_WORKTIME_FLOOR)
                    {
                        usTemp = PARA_WORKTIME_FLOOR;
                    }
                
                    if (usTemp > PARA_WORKTIME_CEIL)
                    {
                        usTemp = PARA_WORKTIME_CEIL;
                    } 
                    sprintf((char*)ucTempBuf, "%5d", usTemp);
                	Para_Save(PARA_WORKTIME_TYPE, (U8 *)ucTempBuf, 5);      //lq �汾�ο���֮ǰ������ʱ��
                    eqEquipPara.WorkTimeSum = usTemp;               
                    sprintf((char*)ucTempBuf, "%5d", 0);
					Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTempBuf, 5); //lq �ϴο���֮����ʱ����0����ֹ�´ο������ظ��ۼ� 
                                    	
                    /*lq �����ϴ��������������������������Ź���λ����*/
                    Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
					Para_Save(PARA_LAST_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//��
                    
                    /*lq ���ο��������ڿ��Ź���λ������0*/
                    sprintf((char*)ucTempBuf, "%2d", 0);
					Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//��
#if 0
                    /*lq ��������������1*/
                    Para_Read(PARA_NORMAL_POWER_ON_CNT_TYPE, ucTempBuf);
                    usTemp = atol((char *)ucTempBuf);
                    usTemp = ++usTemp > PARA_NORMAL_POWER_ON_CNT_CEIL ? PARA_NORMAL_POWER_ON_CNT_CEIL : usTemp;
                    sprintf((char*)ucTempBuf, "%2d", usTemp);
            		Para_Save(PARA_NORMAL_POWER_ON_CNT_TYPE, (U8 *)ucTempBuf, 2);//��
#endif	
    				break;
				}
			}
			else if (ucSOSKey == KEY_LOW)
			{
				if (SosEnableCount < SosKeyEnableValue)
				{
					SosEnableCount++;
					PowerEnableCount = 0;
	
					Is_Feed_Dog = TRUE;
					Feed_Dog();//ι��
	
					Delay_ms(10);//10ms���һ��
				}
				else
				{
					//���ƿ�����Դ��ͬʱ�õ�ǰΪ��������״̬
					IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);
					eqEquipPara.ucWorkMode = EQ_WORKMOD_TEST;
                    eqEquipPara.TestSta = EQ_MOD_TEST_STA_MAIN;

					for (i = 0; i < 3; i++)
					{
#if LED_EN_HIGH
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_HIGH);
#else
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_LOW);
#endif			
            			Delay_ms(250);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//ι��						
#if LED_EN_HIGH
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_LOW);
#else
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_HIGH);
#endif			
            			Delay_ms(750);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//ι��							
					}
				
					UartResponseMsg("$P712,001,,,Powerup By TestKey!");

                    /*lq ��¼�ۼ��ܹ���ʱ��*/
                    Para_Read(PARA_WORKTIME_TYPE, ucTempBuf);               //lq �ϴο���֮ǰ������ʱ��
                    usTemp = atol((char *)ucTempBuf);
                    Para_Read(PARA_WORKTIME_LAST_TYPE, ucTempBuf);          //lq �ϴο���֮����ʱ��
                    usTemp2 = atol((char *)ucTempBuf);
                    usTemp += usTemp2;                                      //lq ���㱾�ο���֮ǰ������ʱ��
                    if (usTemp < PARA_WORKTIME_FLOOR)
                    {
                        usTemp = PARA_WORKTIME_FLOOR;
                    }
                
                    if (usTemp > PARA_WORKTIME_CEIL)
                    {
                        usTemp = PARA_WORKTIME_CEIL;
                    } 
                    sprintf((char*)ucTempBuf, "%5d", usTemp);
                	Para_Save(PARA_WORKTIME_TYPE, (U8 *)ucTempBuf, 5);      //lq �汾�ο���֮ǰ������ʱ��
                    eqEquipPara.WorkTimeSum = usTemp;                    
                    sprintf((char*)ucTempBuf, "%5d", 0);
					Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTempBuf, 5); //lq �ϴο���֮����ʱ����0����ֹ�´ο������ظ��ۼ� 
                    					
                    /*lq �����ϴ��������������������������Ź���λ����*/
                    Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
					Para_Save(PARA_LAST_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//��
                    
                    /*lq ���ο��������ڿ��Ź���λ������0*/
                    sprintf((char*)ucTempBuf, "%2d", 0);
					Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//��	
#if 0                    
                    /*lq �û����Դ�����1*/
                    Para_Read(PARA_USER_TEST_CNT_TYPE, ucTempBuf);
                    usTemp = atol((char *)ucTempBuf);
                    usTemp = ++usTemp > PARA_USER_TEST_CNT_CEIL ? PARA_USER_TEST_CNT_CEIL : usTemp;
                    sprintf((char*)ucTempBuf, "%2d", usTemp);
            		Para_Save(PARA_USER_TEST_CNT_TYPE, (U8 *)ucTempBuf, 2);//��                    				
#endif	
    				break;
				}
			}
		}
    }
	
    //lq LEDInit(LED_TYPE_GPS_SEARCH);                               //lq LED GPS��λ����ָʾ

	UartResponseMsg("$P712,001,,,--------------------------------------");
	UartResponseMsg("$P712,001,,,-- AIS Tranceiver");
	sprintf(chTemp, "$P712,001,,,-- SW Ver %s", VERSION);
	UartResponseMsg(chTemp);
	sprintf(chTemp, "$P712,001,,,-- HW Ver %s", VERSION_HW);
	UartResponseMsg(chTemp);	
    sprintf(chTemp, "$P712,001,,,-- Last compiled: %s %s", __DATE__, __TIME__);
	UartResponseMsg(chTemp);
	UartResponseMsg("$P712,001,,,--------------------------------------");
}
#endif

/*******************************************************************************
* ����  : PowerUpProcess
* ����  : ����ģ�飬�ϵ�ֱ�ӿ�����ȡ��������⡣��λ�����������ϵ縴λ�������п��Ź���λ��
          �����п��Ź���λ���������Ѹ�λby Wakeup pin ���������Ѹ�λby RTC Alarm��
          Reset Pin��λ�������ã�
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void PowerUpProcess(void)
{
//	U8 ucPowerKey;
//	U8 ucSOSKey;
//	U8 i;
	char chTemp[100];
    U8 ucTempBuf[40];
//	U16 PowerEnableCount;
//	U16 SosEnableCount;
	U32 usTemp1;  // lnw  modify    old U16
    U32 usTemp2; // lnw modify  old U16
//	U16 PowerKeyEnableValue;//power��������������ֵ
//	U16 SosKeyEnableValue;//sos��������������ֵ

	//PowerEnableCount = 0;
//	SosEnableCount = 0;
	
    /*lq �豸ϵͳʱ�ӳ�ʼ��*/
    Target_SysClock_Init();

    /*lq ��Դʱ�Ӵ�*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);                             //lq ��ȡ�������ݼĴ���ʱ��Ҫ
    
    /*lq �ϵ縴λ*/
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)                                //lq �ϵ縴λ    
    {
        /*lq �����־λ*/
        RCC_ClearFlag();

    	/* Reset Backup Domain */
    	BKP_DeInit();  
              
        /*lq �������ݼĴ�����ʼ��*/
        InitBKP_DR();

        /*lq �������ݼĴ������ݳ�ʼ��*/
        InitBKP_DRData();
        
        /*lq �����ʼ��*/
    	InitPowerCtrl();        
    	
        //lq IO_Ctrl(IO_POWER_TYPE, IO_VAL_OFF);                                     //lq ��Դ���عرգ������ɹ����ٿ���
        Target_Periph_Init();
        
        /*lq ������ʼ��*/
        Parameter_Init();//��ʼ����ز���                                           //lq ��ʼ�������ṹ��ĳ�Ա������ȫ�ֱ���

        UartResponseMsg("$Reset By PowerOn!");		

		/*lq ���ƿ�����Դ��ͬʱ�õ�ǰΪ��������״̬*/
		//lq IO_Ctrl(IO_POWER_TYPE, IO_VAL_ON);
		DevDynamicPara.ucWorkMode = EQ_WORKMOD_NORMAL;

        /*lq ��������LEDָʾ*/
		PowerUpLedCtrl();

		UartResponseMsg("$P712,001,,,Powerup By PowerOn!");
    	
        /*lq ���ػ�ʱ���¼���� */
        Para_Read(PARA_OPEN_CLOSE_TIME_INDEX_TYPE, ucTempBuf);
        usTemp1 = atol((char *)ucTempBuf);
        DevDynamicPara.OpenCloseTimeIndex = usTemp1;
        usTemp1 = (usTemp1 + 1) % PARA_TIME_OPEN_CLOSE_TOTAL;     //lq �´ο���ʱҪ��ȡ������ֵ
        sprintf((char*)ucTempBuf, "%d", usTemp1);
    	Para_Save(PARA_OPEN_CLOSE_TIME_INDEX_TYPE, (U8 *)ucTempBuf, PARA_OPEN_CLOSE_TIME_INDEX_MAXLEN);

        /*lq ���ο��ػ�ʱ�临λ */
        Para_OpenTimeSave(PARA_TIME_OPEN_DEFAULT, DevDynamicPara.OpenCloseTimeIndex);
        Para_CloseTimeSave(PARA_TIME_CLOSE_DEFAULT, DevDynamicPara.OpenCloseTimeIndex);

        /*lq ��¼�ۼ��ܹ���ʱ��*/
        Para_Read(PARA_WORKTIME_TYPE, ucTempBuf);               //lq �ϴο���֮ǰ������ʱ��
        usTemp1 = atol((char *)ucTempBuf);
        Para_Read(PARA_WORKTIME_LAST_TYPE, ucTempBuf);          //lq �ϴο���֮����ʱ��
        usTemp2 = atol((char *)ucTempBuf);
        usTemp1 += usTemp2;                                      //lq ���㱾�ο���֮ǰ������ʱ��
        if (usTemp1 < PARA_WORKTIME_FLOOR)
        {
            usTemp1 = PARA_WORKTIME_FLOOR;
        }
    
        if (usTemp1 > PARA_WORKTIME_CEIL)
        {
            usTemp1 = PARA_WORKTIME_CEIL;
        } 
        sprintf((char*)ucTempBuf, "%5d", usTemp1);
    	Para_Save(PARA_WORKTIME_TYPE, (U8 *)ucTempBuf, 5);      //lq �汾�ο���֮ǰ������ʱ��

        sprintf((char*)ucTempBuf, "%5d", 0);
		Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTempBuf, 5); //lq �ϴο���֮����ʱ����0����ֹ�´ο������ظ��ۼ� 

        /*lq �����ϴ��������������������������Ź���λ����*/
        Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
		Para_Save(PARA_LAST_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//��
        
        /*lq ���ο��������ڿ��Ź���λ������0*/
        sprintf((char*)ucTempBuf, "%2d", 0);
		Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//��

        /*lq ��������������1*/
        Para_Read(PARA_NORMAL_POWER_ON_CNT_TYPE, ucTempBuf);
        usTemp1 = atol((char *)ucTempBuf);
        usTemp1 = ++usTemp1 > PARA_NORMAL_POWER_ON_CNT_CEIL ? PARA_NORMAL_POWER_ON_CNT_CEIL : usTemp1;
        sprintf((char*)ucTempBuf, "%2d", usTemp1);
		Para_Save(PARA_NORMAL_POWER_ON_CNT_TYPE, (U8 *)ucTempBuf, 2);//��

    }
    
    /*lq �����λ���رտ��Ź�*/
    else if (RCC_GetFlagStatus(RCC_FLAG_SFTRST) != RESET)
    {
		/* Clear reset flags */
    	RCC_ClearFlag();   
#if 1
        /*lq ��RTCʱ��ԴΪLSI���������¿���LSI����LSI�ڴ�������ʱ�Ḵλ*/
        /* Allow access to BKP Domain */
        PWR_BackupAccessCmd(ENABLE);

        /*lq ����LSIѡ��RTCʱ��Դ*/
        if ((RCC->BDCR | 0xFFFFFCFF) == 0xFFFFFEFF)
        {
    	 	/* Enable the LSI OSC */
    		RCC_LSICmd(ENABLE);                                             //lq �����ڲ�RC����,�ڴ�������ʱ��LSI�ᱻ��λ
    		
            /* Wait till LSI is ready */
    		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)//������ʱ��λ�˾�˵��LSI�񵴵�·�ڵ���Ҳ����
    		{}             
        }
#endif
        PWR_WakeUpPinCmd(ENABLE);                                               //lq ʹ��wakeup pin
        PWR_EnterSTANDBYMode();                                                 //lq ��λ���Ź��Զ��ر�         
    }

    /*lq ���Ź���λ*/
    else if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)                      //lq ���Ź���λ
    {
		/* Clear reset flags */
    	RCC_ClearFlag();
	
        if (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET)                            //lq �����п��Ź���λ
        {
#if 1
            /*lq ��RTCʱ��ԴΪLSI���������¿���LSI����LSI�ڴ�������ʱ�Ḵλ*/
            /* Allow access to BKP Domain */
	        PWR_BackupAccessCmd(ENABLE);

            /*lq ����LSIѡ��RTCʱ��Դ*/
            if ((RCC->BDCR | 0xFFFFFCFF) == 0xFFFFFEFF)
            {
        	 	/* Enable the LSI OSC */
        		RCC_LSICmd(ENABLE);                                             //lq �����ڲ�RC����,�ڴ�������ʱ��LSI�ᱻ��λ
        		
                /* Wait till LSI is ready */
        		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)//������ʱ��λ�˾�˵��LSI�񵴵�·�ڵ���Ҳ����
        		{}             
            }
#endif
            PWR_WakeUpPinCmd(ENABLE);                                                   //lq ʹ��wakeup pin
            PWR_EnterSTANDBYMode();                                             //lq ��λ���Ź��Զ��ر�
        }
        else                                                                    //lq �����п��Ź���λ    
        {
        	/* Reset Backup Domain */
        	BKP_DeInit();   

            /*lq �������ݼĴ�����ʼ��*/
            InitBKP_DR();
    
            /*lq �������ݼĴ������ݳ�ʼ��*/
            InitBKP_DRData();
  
            /*lq �����ʼ��*/
        	InitPowerCtrl();
            //lq IO_Ctrl(IO_POWER_TYPE, IO_VAL_ON);                                //lq ��Դ���ؿ���
            Target_Periph_Init();
         	
            /*lq ������ʼ��*/
            Parameter_Init();                                                   //lq ��ʼ�������ṹ��ĳ�Ա������ȫ�ֱ���

			/*lq ��������LEDָʾ*/
    		PowerUpLedCtrl();

            UartResponseMsg("$Reset By IWDG!");		
       		UartResponseMsg("$P712,001,,,Powerup By IWDG!");

            /*lq ���ػ�ʱ���¼���� */
            Para_Read(PARA_OPEN_CLOSE_TIME_INDEX_TYPE, ucTempBuf);
            usTemp1 = atol((char *)ucTempBuf);
            DevDynamicPara.OpenCloseTimeIndex = usTemp1;
            usTemp1 = (usTemp1 + 1) % PARA_TIME_OPEN_CLOSE_TOTAL;     //lq �´ο���ʱҪ��ȡ������ֵ
            sprintf((char*)ucTempBuf, "%d", usTemp1);
        	Para_Save(PARA_OPEN_CLOSE_TIME_INDEX_TYPE, (U8 *)ucTempBuf, PARA_OPEN_CLOSE_TIME_INDEX_MAXLEN);

            /*lq ���ο��ػ�ʱ�临λ */
            Para_OpenTimeSave(PARA_TIME_OPEN_DEFAULT, DevDynamicPara.OpenCloseTimeIndex);
            Para_CloseTimeSave(PARA_TIME_CLOSE_DEFAULT, DevDynamicPara.OpenCloseTimeIndex);

            /*lq ��¼�ۼ��ܹ���ʱ��*/
            Para_Read(PARA_WORKTIME_TYPE, ucTempBuf);               //lq �ϴο���֮ǰ������ʱ��
            usTemp1 = atol((char *)ucTempBuf);
            Para_Read(PARA_WORKTIME_LAST_TYPE, ucTempBuf);          //lq �ϴο���֮����ʱ��
            usTemp2 = atol((char *)ucTempBuf);
            usTemp1 += usTemp2;                                      //lq ���㱾�ο���֮ǰ������ʱ��
            if (usTemp1 < PARA_WORKTIME_FLOOR)
            {
                usTemp1 = PARA_WORKTIME_FLOOR;
            }
        
            if (usTemp1 > PARA_WORKTIME_CEIL)
            {
                usTemp1 = PARA_WORKTIME_CEIL;
            } 
            sprintf((char*)ucTempBuf, "%5d", usTemp1);
        	Para_Save(PARA_WORKTIME_TYPE, (U8 *)ucTempBuf, 5);      //lq �汾�ο���֮ǰ������ʱ��
            sprintf((char*)ucTempBuf, "%5d", 0);
			Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTempBuf, 5); //lq �ϴο���֮����ʱ����0����ֹ�´ο������ظ��ۼ� 

            /*lq �����������������ڿ��Ź���λ������1*/
            Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
            usTemp1 = atol((char *)ucTempBuf);
            usTemp1 = ++usTemp1 > PARA_WDG_RESET_CNT_CEIL ? PARA_WDG_RESET_CNT_CEIL : usTemp1;
            sprintf((char*)ucTempBuf, "%2d", usTemp1);
    		Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//��
        }                                                                       
    } 
    
    /*lq �������Ѹ�λ*/       
    else if (PWR_GetFlagStatus(PWR_FLAG_WU) != RESET)                           //lq ��������
    {
        /*lq ��� WUF��־λ*/
        PWR_ClearFlag(PWR_FLAG_WU);
        
        /* Clear the StandBy pending flag */
        PWR_ClearFlag(PWR_FLAG_SB);

        /*lq ��RTCʱ��ԴΪLSI���������¿���LSI����LSI�ڴ�������ʱ�Ḵλ*/
        /* Allow access to BKP Domain */
        PWR_BackupAccessCmd(ENABLE);

        /*lq ����LSIѡ��RTCʱ��Դ*/
        if ((RCC->BDCR | 0xFFFFFCFF) == 0xFFFFFEFF)
        {
    	 	/* Enable the LSI OSC */
    		RCC_LSICmd(ENABLE);                                             //lq �����ڲ�RC����,�ڴ�������ʱ��LSI�ᱻ��λ
    		
            /* Wait till LSI is ready */
    		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)//������ʱ��λ�˾�˵��LSI�񵴵�·�ڵ���Ҳ����
    		{}             
        }  
              
        /*lq ���Ǵ�������������������RTC*/
        ucStandbyWakeupFlg = TRUE;
        
        /*lq �������ѳ�ʼ����ʱ����*/
        ucStandbyWakeupDelayFlg = TRUE;

        /*lq ��ȡ�������ݼĴ�������*/
        GetBKP_DRData();
        
        //lq if (RTC_GetFlagStatus(RTC_FLAG_ALR) != RESET)
        if (IsRtcAlarm())
        {
//            RTC_ClearFlag(RTC_FLAG_RSF);

            /*lq ʧ��wakeup pin*/
            PWR_WakeUpPinCmd(DISABLE);                                                   //lq ʹ��wakeup pin

            /*lq ��ϵͳ����*/
            InitPowerCtrl();      
            //lq IO_Ctrl(IO_POWER_TYPE, IO_VAL_ON);                                //lq ��Դ���ؿ���
            Delay_ms(10);//lqdb                                                 //lq ��ʱ������10ms����ϵͳ�ϵ���ɣ��ٶ�1846�������������

            /*lq �豸��ʼ��*/
            Target_Periph_Init();//��ʼ�� arm                                                  //lq ����ARM
        	
            /*lq ������ʼ��*/
            Parameter_Init();//��ʼ����ز���                                           //lq ��ʼ�������ṹ��ĳ�Ա������ȫ�ֱ���
   
            /*lq ���ߴ������ѳ�ʼ�� */
            InitBKP_DRPara();
#if 0
            SleepWakeUpInit();
#endif
            return;
        }
        else                                                                    //lq wakeup��������
        {
#if CONFIG_KEY
#if 1 
        	/*lq �豸��ʼ��*/
            InitPowerCtrl();
            //lq IO_Ctrl(IO_POWER_TYPE, IO_VAL_ON);                                //lq ��Դ���ؿ���
            Target_Periph_Init();//��ʼ�� arm                                                  //lq ����ARM
        	
            /*lq ������ʼ��*/
            Parameter_Init();//��ʼ����ز���                                           //lq ��ʼ�������ṹ��ĳ�Ա������ȫ�ֱ���
        
        	PowerKeyEnableValue = eqEquipPara.CloseTime * 100;//ÿ10ms���һ��
        	//lq SosKeyEnableValue = eqEquipPara.CloseTime * 100;

            UartResponseMsg("$Reset By Wakeup Pin!");		
			
            /*lq ����ָʾ*/
            IO_Ctrl(IO_LED_GREEN_TYPE, IO_VAL_ON);
			Delay_ms(100);
            IO_Ctrl(IO_LED_GREEN_TYPE, IO_VAL_OFF);

            while (1)
    		{ 
			    ucPowerKey = ReadSingleKeyValProcess(&PowerKey);                
                //lq ucSOSKey = ReadSingleKeyValProcess(&SOSKey);  //lq �ػ�������ΪTest��

                if (ucPowerKey == KEY_LOW)
    			{
                    UartResponseMsg("KEY_LOW!");
                    if (PowerEnableCount < PowerKeyEnableValue)//����ʱ��δ���Ҳ��Ǹ�λ������
    				{
                        PowerEnableCount++;
    					//lq SosEnableCount++;
    					
    					Is_Feed_Dog = TRUE;
    					Feed_Dog();//ι��
    	
    					Delay_ms(10);//10ms���һ��
    				}
    				else
    				{
				        UartResponseMsg("$P712,001,,,PowerDown Key Down During Standby!");
		
    					/*lq �����йػ�*/
                        ExecShutdownAction(); //lq ����ʱ��������
    				}
    			}
                else
                {
                    UartResponseMsg("KEY_HIGH!");
#if 1
                    /* Generate a system reset */
                    NVIC_GenerateSystemReset();                                                 //lq �ص�IWDG
#else
                    PWR_WakeUpPinCmd(ENABLE);                                                   //lq ʹ��wakeup pin
                    PWR_EnterSTANDBYMode();
#endif
                }
    		}    

#endif
#endif
        }
    }
    
    /*lq JTAG�������Ÿ�λ*/
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)//lqdb nRST Pin ��λ      JTAG����ʱ��λ
    {
        /*lq �����־λ*/
        RCC_ClearFlag();

    	/* Reset Backup Domain */
    	BKP_DeInit();  
              
        /*lq �������ݼĴ�����ʼ��*/
        InitBKP_DR();

        /*lq �������ݼĴ������ݳ�ʼ��*/
        InitBKP_DRData();
        
        /*lq �����ʼ��*/
    	InitPowerCtrl();
        //lq IO_Ctrl(IO_POWER_TYPE, IO_VAL_OFF);                                     //lq ��Դ���عرգ������ɹ����ٿ���
        Target_Periph_Init();
        
        /*lq ������ʼ��*/
        Parameter_Init();//��ʼ����ز���                                           //lq ��ʼ�������ṹ��ĳ�Ա������ȫ�ֱ���

        UartResponseMsg("$Reset By nRst Pin!");		

		//���ƿ�����Դ��ͬʱ�õ�ǰΪ��������״̬
		//lq IO_Ctrl(IO_POWER_TYPE, IO_VAL_ON);
		DevDynamicPara.ucWorkMode = EQ_WORKMOD_NORMAL;

		/*lq ��������LEDָʾ*/
		PowerUpLedCtrl();

		UartResponseMsg("$P712,001,,,Powerup By PowerOn!");

        /*lq ���ػ�ʱ���¼���� */
        Para_Read(PARA_OPEN_CLOSE_TIME_INDEX_TYPE, ucTempBuf);
        usTemp1 = atol((char *)ucTempBuf);
        DevDynamicPara.OpenCloseTimeIndex = usTemp1;
        usTemp1 = (usTemp1 + 1) % PARA_TIME_OPEN_CLOSE_TOTAL;     //lq �´ο���ʱҪ��ȡ������ֵ
        sprintf((char*)ucTempBuf, "%d", usTemp1);
    	Para_Save(PARA_OPEN_CLOSE_TIME_INDEX_TYPE, (U8 *)ucTempBuf, PARA_OPEN_CLOSE_TIME_INDEX_MAXLEN);

        /*lq ���ο��ػ�ʱ�临λ */
        Para_OpenTimeSave(PARA_TIME_OPEN_DEFAULT, DevDynamicPara.OpenCloseTimeIndex);
        Para_CloseTimeSave(PARA_TIME_CLOSE_DEFAULT, DevDynamicPara.OpenCloseTimeIndex);

        /*lq ��¼�ۼ��ܹ���ʱ��*/
        Para_Read(PARA_WORKTIME_TYPE, ucTempBuf);               //lq �ϴο���֮ǰ������ʱ��
        usTemp1 = atol((char *)ucTempBuf);
        Para_Read(PARA_WORKTIME_LAST_TYPE, ucTempBuf);          //lq �ϴο���֮����ʱ��
        usTemp2 = atol((char *)ucTempBuf);
        usTemp1 += usTemp2;                                      //lq ���㱾�ο���֮ǰ������ʱ��
        if (usTemp1 < PARA_WORKTIME_FLOOR)
        {
            usTemp1 = PARA_WORKTIME_FLOOR;
        }
    
        if (usTemp1 > PARA_WORKTIME_CEIL)
        {
            usTemp1 = PARA_WORKTIME_CEIL;
        } 
        sprintf((char*)ucTempBuf, "%5d", usTemp1);
    	Para_Save(PARA_WORKTIME_TYPE, (U8 *)ucTempBuf, 5);      //lq �汾�ο���֮ǰ������ʱ��

        sprintf((char*)ucTempBuf, "%5d", 0);
		Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTempBuf, 5); //lq �ϴο���֮����ʱ����0����ֹ�´ο������ظ��ۼ� 
                        	
        /*lq �����ϴ��������������������������Ź���λ����*/
        Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
		Para_Save(PARA_LAST_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//��
        
        /*lq ���ο��������ڿ��Ź���λ������0*/
        sprintf((char*)ucTempBuf, "%2d", 0);
		Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//��
#if 0
        /*lq ��������������1*/
        Para_Read(PARA_NORMAL_POWER_ON_CNT_TYPE, ucTempBuf);
        usTemp1 = atol((char *)ucTempBuf);
        usTemp1 = ++usTemp > PARA_NORMAL_POWER_ON_CNT_CEIL ? PARA_NORMAL_POWER_ON_CNT_CEIL : usTemp1;
        sprintf((char*)ucTempBuf, "%2d", usTemp1);
		Para_Save(PARA_NORMAL_POWER_ON_CNT_TYPE, (U8 *)ucTempBuf, 2);//��
#endif	
    }
	
    LEDInit(LED_TYPE_POWER_NORMAL);                               //lq LED ��Դָʾ

	UartResponseMsg("$P712,001,,,--------------------------------------");
	UartResponseMsg("$P712,001,,,-- AIS Tranceiver2");
	sprintf(chTemp, "$P712,001,,,-- SW Ver %s", VERSION);
	UartResponseMsg(chTemp);
	//Para_Read(PARA_HWVERSION_TYPE, ucTempBuf);	
	FlashRead(STM32_FLASH_HWVERSION_TYPE, ucTempBuf);
    sprintf(chTemp, "$P712,001,,,-- HW Ver %s", ucTempBuf);
	UartResponseMsg(chTemp);	
    sprintf(chTemp, "$P712,001,,,-- Last compiled: %s %s", __DATE__, __TIME__);
	UartResponseMsg(chTemp);
	UartResponseMsg("$P712,001,,,--------------------------------------");
	//File_Create(fp);
}

/*******************************************************************************
* ����  : InitBKP_DRPara
* ����  : ʹ�ñ������ݼĴ��������ݳ�ʼ����ز���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitBKP_DRPara(void)
{
#if 0
    /*lq ��־����*/
    GpsCtrlPara.ucGpsUnfix1hLngOpenFlg = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                         BKPR_FLAG_BIT_GPS_LNG_OPEN_FIRST_HOUR_INDEX);
    GpsCtrlPara.ucGpsUnfixBey1hLngOpenFlg = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                            BKPR_FLAG_BIT_GPS_LNG_OPEN_BEY_FIRST_HOUR_INDEX);
    GpsCtrlPara.ucGps5minLngOpenEn = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                     BKPR_FLAG_BIT_GPS_5MIN_LONG_OPEN_EN_INDEX);
    GpsCtrlPara.GpsPowerupFix = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                BKPR_FLAG_BIT_GPS_POWER_UP_FIX_INDEX);
    GpsCtrlPara.ucGpsOnTimEn = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                               BKPR_FLAG_BIT_GPS_ON_TIM_EN_INDEX);
    GpsCtrlPara.ucGpsOffTimEn = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                BKPR_FLAG_BIT_GPS_OFF_TIM_EN_INDEX);
    GpsCtrlPara.ucGpsSaveFlg = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                BKPR_FLAG_BIT_GPS_SAVE_INDEX);
    GpsCtrlPara.ucGpsOnEn = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                BKPR_FLAG_BIT_GPS_ON_INDEX);
    GpsPara.GPS_LeapSecondFlg = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                BKPR_FLAG_BIT_GPS_LEAP_SECOND_INDEX);
    struRtcPara.ucRtcLSEorLSI = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                BKPR_FLAG_BIT_RTC_LSE_OR_LSI_INDEX);
    struRtcTimPhase1.RtcTimPhaseFlg = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                BKPR_FLAG_BIT_RTC_TIM_PHASE_INDEX);
    GpsSynCell.GPSSyn_Enable = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                BKPR_FLAG_BIT_GPS_SYNCH_ENABLE_INDEX);
    eqEquipPara.ucWorkMode = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                BKPR_FLAG_BIT_EQ_WORKMOD_INDEX);                                                                
                                  
    GpsPara.GPS_Latitude = arrBKPDRData[BKPR_GPS_LATITUDE_H_INDEX];
    GpsPara.GPS_Latitude = ((GpsPara.GPS_Latitude << 16) & 0xFFFF0000) 
                            | (arrBKPDRData[BKPR_GPS_LATITUDE_L_INDEX] & 0xFFFF); 
    GpsPara.GPS_Longitud = arrBKPDRData[BKPR_GPS_LONGITUDE_H_INDEX];                   
    GpsPara.GPS_Longitud = ((GpsPara.GPS_Longitud << 16) & 0xFFFF0000) 
                            | (arrBKPDRData[BKPR_GPS_LONGITUDE_L_INDEX] & 0xFFFF); 
    GpsPara.GPS_Sog = arrBKPDRData[BKPR_GPS_SOG_INDEX];
    GpsPara.GPS_Cog = arrBKPDRData[BKPR_GPS_COG_INDEX];
    // GpsPara.GPS_TimeH = arrBKPDRData[BKPR_GPS_TIME_HOUR_INDEX];
    // GpsPara.GPS_TimeM = arrBKPDRData[BKPR_GPS_TIME_MIN_INDEX];
   
    GpsCtrlPara.GpsFallCount = arrBKPDRData[BKPR_GPS_FALL_COUNT_INDEX];
    GpsCtrlPara.GpsCtrlSta = arrBKPDRData[BKPR_GPS_CTRL_STA_INDEX]; 
    GpsCtrlPara.GpsUnfix1hPeriodTimFrame = arrBKPDRData[BKPR_GPS_UNFIX_1H_PERIOD_TIM_FRAME_H_INDEX];
    GpsCtrlPara.GpsUnfix1hPeriodTimFrame = ((GpsCtrlPara.GpsUnfix1hPeriodTimFrame << 16) & 0xFFFF0000) 
                                            | (arrBKPDRData[BKPR_GPS_UNFIX_1H_PERIOD_TIM_FRAME_L_INDEX] & 0xFFFF); 
    GpsCtrlPara.GpsUnfix1hPeriodTimSlot = arrBKPDRData[BKPR_GPS_UNFIX_1H_PERIOD_TIM_SLOT_INDEX];
    GpsCtrlPara.GpsOnTimFrame = arrBKPDRData[BKPR_GPS_START_FRAME_H_INDEX];
    GpsCtrlPara.GpsOnTimFrame = ((GpsCtrlPara.GpsOnTimFrame << 16) & 0xFFFF0000) 
                                 | (arrBKPDRData[BKPR_GPS_START_FRAME_L_INDEX] & 0xFFFF); 
    GpsCtrlPara.GpsOnTimSlot = arrBKPDRData[BKPR_GPS_START_SLOT_INDEX];
    GpsCtrlPara.GpsOffTimFrame = arrBKPDRData[BKPR_GPS_OFF_FRAME_H_INDEX]; 
    GpsCtrlPara.GpsOffTimFrame = ((GpsCtrlPara.GpsOffTimFrame << 16) & 0xFFFF0000) 
                                  | (arrBKPDRData[BKPR_GPS_OFF_FRAME_L_INDEX] & 0xFFFF); 
    GpsCtrlPara.GpsOffTimSlot = arrBKPDRData[BKPR_GPS_OFF_SLOT_INDEX];                                  

    struSOTDMAPara.BurstTxTimFrame = arrBKPDRData[BKPR_BURST_TX_TIM_FRAME_H_INDEX];
    struSOTDMAPara.BurstTxTimFrame = ((struSOTDMAPara.BurstTxTimFrame << 16) & 0xFFFF0000) 
                                      | (arrBKPDRData[BKPR_BURST_TX_TIM_FRAME_L_INDEX] & 0xFFFF); 
    struSOTDMAPara.BurstTxTimSlot = arrBKPDRData[BKPR_BURST_TX_TIM_SLOT_INDEX]; 
#if 0
    struSOCommState.ucSlotTimeout = arrBKPDRData[BKPR_SLOT_TIMEOUT_INDEX];
    struSOCommState.usSlotOffset = arrBKPDRData[BKPR_SLOT_OFFSET_INDEX];
#endif
    struRtcTimPhase1.RtcTimPhaseNum = arrBKPDRData[BKPR_RTC_TIM_PHASE_H_INDEX];
    struRtcTimPhase1.RtcTimPhaseNum = ((struRtcTimPhase1.RtcTimPhaseNum << 16) & 0xFFFF0000) 
                            | (arrBKPDRData[BKPR_RTC_TIM_PHASE_L_INDEX] & 0xFFFF); 
    struRtcTimPhase1.lfRtcTimPhaseNum = (double)struRtcTimPhase1.RtcTimPhaseNum / RTC_TIM_PHASE_PRECISE; 

    struRtcPara.usRtcFreq = arrBKPDRData[BKPR_RTC_FREQUENCE_INDEX];
    struRtcPara.ucRtcAlarmMcuOrGps = arrBKPDRData[BKPR_RTC_ALARM_MCU_OR_GPS_INDEX];
    // struRtcPara.ssRtcAlarmValue = arrBKPDRData[BKPR_RTC_ALARM_VALUE_INDEX];
    struRtcPara.ssRtcAlarmValue = arrBKPDRData[BKPR_RTC_ALARM_VALUE_H_INDEX];
    struRtcPara.ssRtcAlarmValue = ((struRtcPara.ssRtcAlarmValue << 16) & 0xFFFF0000) 
                            | (arrBKPDRData[BKPR_RTC_ALARM_VALUE_L_INDEX] & 0xFFFF); 

    gslFrameCnt = arrBKPDRData[BKPR_FRAME_CNT_H_INDEX];
    gslFrameCnt = ((gslFrameCnt << 16) & 0xFFFF0000) | (arrBKPDRData[BKPR_FRAME_CNT_L_INDEX] & 0xFFFF); 
	gsiSlotCnt = arrBKPDRData[BKPR_SLOT_CNT_INDEX];
	gsiPhaseCnt = arrBKPDRData[BKPR_PHASE_CNT_INDEX];

    TimAdjust1 = arrBKPDRData[BKPR_TIM_ADJUST_1_INDEX];
    TimAdjust10 = arrBKPDRData[BKPR_TIM_ADJUST_10_INDEX]; 
    TimAdjust100 = arrBKPDRData[BKPR_TIM_ADJUST_100_INDEX];

    TimAdjustPara.PeriodCountLast = arrBKPDRData[BKPR_TIM_ADJUST_PERIOD_CNT_LAST_INDEX];  

    // RtcPhaseAdjust1 = arrBKPDRData[BKPR_RTC_ADJUST_1_INDEX];
    // RtcPhaseAdjust10 = arrBKPDRData[BKPR_RTC_ADJUST_10_INDEX];
    // RtcPhaseAdjust100 = arrBKPDRData[BKPR_RTC_ADJUST_100_INDEX];
    GpsCtrlPara.GpsIntervalCnt = arrBKPDRData[BKPR_GPS_INTERVAL_CNT_INDEX];
  
    send_seed.seed = arrBKPDRData[BKPR_RAND_SEED_H_INDEX];
    send_seed.seed = ((send_seed.seed << 16) & 0xFFFF0000) | (arrBKPDRData[BKPR_RAND_SEED_L_INDEX] & 0xFFFF);  
#endif
}

/*******************************************************************************
* ����  : CalcuSleepSlots_Wakeup
* ����  : �������ߵ�ʱ϶��
* ����  : RTC�趨ֵ		  
* ���  : ��
* ����  : RTC�趨ֵ��Ӧ��ʱ϶��
*******************************************************************************/
U32 CalcuSleepSlots_Wakeup(const U32 ulAlarmValue)
{
    U32 ulSlotDiff;
 
    ulSlotDiff = ulAlarmValue * 37.5 / arrBKPDRData[BKPR_RTC_FREQUENCE_INDEX] + 0.5;

    return ulSlotDiff;
}

/*******************************************************************************
* ����  : CalcuSleepPhases_Wakeup
* ����  : �������ߵ���λ��
* ����  : RTC�趨ֵ		  
* ���  : ��
* ����  : RTC�趨ֵ��Ӧ����λ��
*******************************************************************************/
U32 CalcuSleepPhases_Wakeup(const U32 ulAlarmValue)
{
    U32 ulPhaseDiff;
 
    ulPhaseDiff = ulAlarmValue * 48000 / arrBKPDRData[BKPR_RTC_FREQUENCE_INDEX];

    return ulPhaseDiff;
}
#if 0
/*******************************************************************************
* ����  : SleepWakeUpInit
* ����  : ���߻��ѳ�ʼ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SleepWakeUpInit(void)
{
    int i;
    S32 slGPS_Longitud_Buf_Elem;
    S32 slGPS_Latitude_Buf_Elem;
    U32 slGPS_SOG_Buf_Elem;
	U8 	ucTempBuf[40];
    S32 PhaseDiff;
    S32 Temp1sCnt = 0;
    S32 Temp10sCnt = 0;
    S32 Temp100sCnt = 0;
    S32 slTemp1;

    /*lq �������Ѻ��ʼ��*/
#if 0
    gsiSlotCnt += CalcuSleepSlots_Wakeup(struRtcPara.ssRtcAlarmValue);
    if (gsiSlotCnt >= 2250)
    {
        gslFrameCnt +=  gsiSlotCnt / 2250;
        gsiSlotCnt = gsiSlotCnt % 2250;
    }
#endif

#if 1
    /*lq ����ʱ����Ӧ����λ��*/
    // PhaseDiff = CalcuSleepPhases_Wakeup(struRtcPara.ssRtcAlarmValue);
    
    /*lq ��RTC����λ��ת��ΪTim����*/
    PhaseDiff = (struRtcPara.ssRtcAlarmValue + 1) * ((double)struRtcTimPhase1.RtcTimPhaseNum / RTC_TIM_PHASE_PRECISE) + 0.5;

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

#endif
#if 1    
    /*lq ÿ�ν���standby��RTC�ж�ʹ��λ�������Ч���������Ѻ�������ʹ��*/    
	EXTI_ClearITPendingBit(EXTI_Line17);	
    
    /* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();
	RTC_WaitForLastTask();   

	/* Clear RTC Alarm interrupt pending bit */
	RTC_ClearITPendingBit(RTC_IT_ALR);	
	
    /* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Enable the RTC Alarm interrupt */
	RTC_ITConfig(RTC_IT_ALR, ENABLE);

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

    /*lq ����RTC Alarmֵ*/
    //ResetRtcCount(30);
	RTC_EnterConfigMode();
     
	RTC_WaitForLastTask();
	//RTC_WaitForSynchro();
#if 1
    RTC_SetAlarm(RTC_GetCounter() + 1);
#else
    TempAlarm = RTC_GetCounter() + 3;
    RTC_SetAlarm(TempAlarm);
#endif
	RTC_WaitForLastTask();
	RTC_ExitConfigMode();

	RTC_WaitForLastTask();
#endif
    slGPS_Longitud_Buf_Elem = GpsPara.GPS_Longitud / GPS_POSITION_BUFF_LENGTH;
    slGPS_Latitude_Buf_Elem = GpsPara.GPS_Latitude / GPS_POSITION_BUFF_LENGTH;
    for (i = 0; i < GPS_POSITION_BUFF_LENGTH; i++)
    {
        GpsPara.GPS_Longitud_Buf[i] = slGPS_Longitud_Buf_Elem;
        GpsPara.GPS_Latitude_Buf[i] = slGPS_Latitude_Buf_Elem;    
    }

//lq    slGPS_SOG_Buf_Elem = GpsPara.GPS_Sog / 18;
    slGPS_SOG_Buf_Elem = GpsPara.GPS_Sog;
    for (i = 0; i < 18; i++)
    {
        GpsPara.GPS_Sog_Buf[i] = slGPS_SOG_Buf_Elem;
    }

    /*lq ��Ϣ�ֶγ�ʼ������Ҫ��GpsPara.GPS_Sog GpsPara.GPS_Cog��ʹ��Bkp_DR�е�ֵ���¸�ֵ֮��*/
    ResetMsgPara();
      
    /*lq ����״̬*/
    /*lq ������ģʽΪ����ģʽ����һ���ǵ�ز���ģʽ����������ģʽ����������ģʽ*/
    if (eqEquipPara.ucWorkMode == EQ_WORKMOD_TEST)
    {
        eqEquipPara.TestSta = EQ_MOD_TEST_STA_BATTERY;    
    }
    else
    {
	    eqEquipPara.ucWorkMode = EQ_WORKMOD_NORMAL;    
    }
    eqEquipPara.ucLowFreqSta = EQ_FREQSTA_LOW;

    MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_IDLE;
    state_4501 = NORMAL_WORK_BURST;                
	if (struRtcPara.ucRtcLSEorLSI == RTC_CLK_SOURCE_LSI)
    {
        TimAdjustPara.RtcClockSource = ADJUST_RTC_SOURCE_LSI;
	    RtcCnt1s = RTC_ADJUST_1S_LSI;
        RtcCnt10s = RTC_ADJUST_10S_LSI;
        RtcCnt100s = RTC_ADJUST_100S_LSI;              
    }
    else
    {
        TimAdjustPara.RtcClockSource = ADJUST_RTC_SOURCE_LSE;
        RtcCnt1s = RTC_ADJUST_1S;
        RtcCnt10s = RTC_ADJUST_10S;
        RtcCnt100s = RTC_ADJUST_100S;
    } 
	TimAdjustPara.RtcClockChoice = TRUE;

//    RTCConfigurationPart2();

    /*lq �趨MCU���Ѷ�ʱ*/				
    SetMcuWakeupTim(SLOT_INTERVAL_MCU_WAKEUP_TO_TX);
 
    /*lq �趨����Ϣ���Ͷ�ʱ*/
    SetBurstSendTim(SLOT_INTERVAL_SEND_TO_TX);

    /*lq ��EEPROM�ж�ȡGPS�ɹ���λ������GPS����3�β���λ�Ĵ�����GPS5min����������ֵ����Ӧ�ı���*/ 
	Para_Read(PARA_GPS_FIX_COUNT_TYPE, ucTempBuf);
    GpsCtrlPara.GpsFixCount = atol((char *)ucTempBuf);

	Para_Read(PARA_GPS_WAIT_COUNT_TYPE, ucTempBuf);
    GpsCtrlPara.GpsWaitCount = atol((char *)ucTempBuf);

    Para_Read(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTempBuf);
    GpsCtrlPara.ssGps5minLngOpenCnt = atol((char *)ucTempBuf);

    // UartResponseMsg("$MSG_CTRL_STATE : MSG_CTRL_STA_IDEL after sleeping!");
    
    TEST_OUT_HIGH_2();                               
            
}
#endif
/*******************************************************************************
* ����  : PowerUpLedCtrl
* ����  : ��������LEDָʾ
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void PowerUpLedCtrl(void)
{
	int i;

	for (i = 0; i < 2; i++)
	{
        LEDAllLight();
		Delay_ms(500);
		Is_Feed_Dog = TRUE;
		Feed_Dog();
           
        LEDAllDown();
		Delay_ms(500);
		Is_Feed_Dog = TRUE;
		Feed_Dog();
	}
}
