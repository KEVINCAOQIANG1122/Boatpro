/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: powerdown.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    �ػ�ģ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include "config.h"

#if CONFIG_POWERDOWN
#include "def.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "initial.h"
#include "debug.h"
#include "keyscan.h"
#include "systemtick.h"
#include "powerdown.h"
#include "ledctrl.h"
#include "common.h"
#include "ioctrl.h"
#include "gpsctrl.h"
#include "parastorage.h"
#include "wdg.h"
#include "testing.h"
#include "tim.h"

PowerDownStruct PowerDownPara;
	
/*******************************************************************************
* ����  : IniPowerDownPara
* ����  : ��ʼ���ػ�ģ�����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void IniPowerDownPara(void)
{
//lq	PowerDownPara.TimeCnt = 0;	
	PowerDownPara.TimePeriod = POWERDOWN_POWERUP_PEROID;	//50ms���һ�°���״̬

    PowerDownPara.TimePeriod1min = POWERDOWN_TIME_1min_PERIOD;
    PowerDownPara.TimeCnt1min = SetTimePeriodProcess(PowerDownPara.TimePeriod1min);
    PowerDownPara.TimeCntMin = 0;

	PowerDownPara.XDouCnt = 0;
	PowerDownPara.Sta = POWERDOWN_STA_IDLE;

	PowerDownPara.KeyVal = KEY_LOW;
	PowerDownPara.KeyValPast = KEY_LOW;//������ȥֵ��ֻ�е������Ӹ߱�Ϊ�Ͳ���Ϊ��ʼpowerdown���̣���ֹ���������ػ�

	PowerDownPara.TimeCnt = SetTimePeriodProcess(PowerDownPara.TimePeriod);

	PowerDownPara.ShutDownTime = eqEquipPara.CloseTime * 200;
}

/*******************************************************************************
* ����  : PowerDownProcess
* ����  : �ػ�ģ�飬��ɰ���ɨ�裬LED���ƣ�����״̬���ƣ��ػ�����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void PowerDownProcess(void)
{
	U8 ucTemp[10];
	U8 ucTemp1[10];
//lq-	U8 ucTemp_key1;
	U16 PowerDownCount;
	//U16 ShutDownTime;
	
	PowerDownCount = 0;
#if 0		
	/*lq ÿ5min��¼һ��GPS���ں�ʱ�䣬��Ϊ�������ػ�ʱ��*/
    if (GetLocalClockTickProcess() == PowerDownPara.TimeCnt1min)
    {
        PowerDownPara.TimeCnt1min = SetTimePeriodProcess(PowerDownPara.TimePeriod1min);
        
        PowerDownPara.TimeCntMin++;
        if (PowerDownPara.TimeCntMin >= POWERDOWN_TIME_Min_PERIOD)
        {
            PowerDownPara.TimeCntMin = 0;
        	if (GetGpsPowerupFixState() == TRUE)                                //lq ֻ�п�����λ�ɹ��󣬲Ż��¼�ػ�ʱ��
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


                /*lq ��¼����֡�����Ĺ���ʱ��*/
    			sprintf((char *)ucTemp,"%5d",GetFrameNum());
    			Para_Save(PARA_BATTERY_TEST_TIME_TYPE,ucTemp,5);
        	}              
        }    
    }
#endif    
    //������ʼ״̬����Ҫ��⵽��Դ����̧���ܽ��뵽�����Ĺؼ�״̬
	if (PowerDownPara.Sta == POWERDOWN_STA_IDLE)
	{
		if (GetLocalClockTickProcess() == PowerDownPara.TimeCnt)
		{
			PowerDownPara.TimeCnt = SetTimePeriodProcess(PowerDownPara.TimePeriod);
			PowerDownPara.KeyValPast = PowerDownPara.KeyVal;
			PowerDownPara.KeyVal = GetIndivIO(KEY_POWER_ADDR);
			
			//����̧�𣬽�����һ������״̬
			//if (PowerDownPara.KeyVal == KEY_HIGH)
			if (PowerDownPara.KeyVal == KEY_LOW && PowerDownPara.KeyValPast == KEY_HIGH)
			{
				PowerDownPara.TimePeriod = POWERDOWN_NORMAL_PEROID;	//���ػ�������5ms�ٶȽ��м��
				PowerDownPara.TimeCnt = SetTimePeriodProcess(PowerDownPara.TimePeriod);
				PowerDownPara.Sta = POWERDOWN_STA_STEP1; 	//����ػ��������״̬
			}	
		}	
	}
	//��������״̬�����3s�ػ���������	
	else if (PowerDownPara.Sta == POWERDOWN_STA_STEP1)
	{
		if (GetLocalClockTickProcess() == PowerDownPara.TimeCnt)
		{
			PowerDownPara.TimeCnt = SetTimePeriodProcess(PowerDownPara.TimePeriod);
			PowerDownPara.KeyVal = GetIndivIO(KEY_POWER_ADDR);

			//�������£���ⰴ������ʱ��
			if (PowerDownPara.KeyVal == KEY_LOW)
			{
				PowerDownPara.XDouCnt++;

				//�����������³���3s
				if (PowerDownPara.XDouCnt >= PowerDownPara.ShutDownTime)
				{
					UartResponseMsg("$P712,001,,,PowerDown Key Down!");
					PowerDownPara.Sta = POWERDOWN_STA_STEP2; 	//����ػ��������״̬	
				}  				
			}
			else
			{
				PowerDownPara.XDouCnt = 0;
				PowerDownPara.Sta = POWERDOWN_STA_IDLE;//�̰���ظ�
			}
		}	
	}
	//�ػ�״̬��ִ�йػ�����
	else if (PowerDownPara.Sta == POWERDOWN_STA_STEP2)							//lq 2013-01-27
	{
        /*lq ��¼���ο�������ʱ��*/
        sprintf(ucTemp, "%5d", (U16)gslFrameCnt);
        Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTemp, 5);  
    
    	if (GetGpsPowerupFixState() == TRUE)                                    //lq ֻ�п�����λ�ɹ��󣬲Ż��¼�ػ�ʱ��
    	{
    		//�ػ�ʱ��
            sprintf((char *)ucTemp, "%2d", GpsPara.GPS_TimeMon);
    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeD);
    		strcat((char *)ucTemp, (char *)ucTemp1);
            sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeH);
    		strcat((char *)ucTemp, (char *)ucTemp1);
    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeM);
    		strcat((char *)ucTemp, (char *)ucTemp1);
        	Para_Save(PARA_TIME_CLOSE_TYPE, ucTemp, PARA_TIME_CLOSE_MAXLEN);                           //lq [2013/04/16]
    		UartResponseMsg("POWER DOWN");
    	}
    
    	/*lq �ػ�ԭ��*/
    	sprintf((char *)ucTemp, "%d", 1);
    	Para_Save(PARA_CLOSE_RESION_TYPE, ucTemp, 1);//�����ػ�
    
    	LEDAllLight();
    	
        IO_Ctrl(IO_POWER_TYPE, IO_VAL_OFF);	
    
    	while (PowerDownCount < 20)//LED��2���ر�
    	{
    		Is_Feed_Dog = TRUE;
    		Feed_Dog();
    		Delay_ms(100);
    		PowerDownCount++;
    	}	
    	
    //lq-		IO_Ctrl(IO_POWER_TYPE, IO_VAL_LOW);	
    	while (1)
    	{
    /*lq-
    		ucTemp_key1 = ReadSingleKeyValProcess(&PowerKey);
    		//ucTemp_key2 = ReadSingleKeyValProcess(&SOSKey);
    
    		if (ucTemp_key1 == KEY_LOW)//��power����һֱ����
    		{
    			Is_Feed_Dog = TRUE;
    			Feed_Dog();//ι��
    			Delay_ms(250);//һֱι��������֤�ػ��󳤰�POWER���ᵼ�¹ػ����ٴθ�λ����
    		}
    */
    //lq+		
    		Is_Feed_Dog = TRUE;
    		Feed_Dog();//ι��
    		Delay_ms(250);//һֱι��������֤�ػ��󳤰�POWER���ᵼ�¹ػ����ٴθ�λ����
    //lq~       
    	} 
	}
}

/*******************************************************************************
* ����  : GetPowerDownSleepSta
* ����  : ��ȡ�ػ�ģ��״̬
* ����  : ��		  
* ���  : ��
* ����  : TRUE��ʾ���Խ���ʡ�磬FALSE��ֹ����ʡ��
*******************************************************************************/
U8 GetPowerDownSleepSta(void)
{
	if (PowerDownPara.Sta == POWERDOWN_STA_IDLE)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}		
}

/*******************************************************************************
* ����  : PowerDown_MsgTest
* ����  : ��Ϣ����ģʽ�£��Զ��ػ�ģ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void PowerDown_MsgTest(void)
{
	U8 ucTemp[10];
	U16 PowerDownCount;
	
	PowerDownCount = 0;
   
    /*lq ���һ����Ϣ�������*/  
    if (struMsgTestPara.ucPowerDownEn == TRUE && GetBurstFinsFlg() == TRUE)
    {
		UartResponseMsg("$Msg Testing Finish!");

        /*lq ��¼���ο�������ʱ��*/
        sprintf(ucTemp, "%5d", (U16)gslFrameCnt);
        Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTemp, 5);

		LEDAllLight();
		
        IO_Ctrl(IO_POWER_TYPE, IO_VAL_OFF);	
        
        /*lq LED��2���ر�*/
		while (PowerDownCount < 20)
		{
			Is_Feed_Dog = TRUE;
			Feed_Dog();
			Delay_ms(100);
			PowerDownCount++;
		}	
		
		/*lq һֱι��������֤�ػ��󳤰�POWER���ᵼ�¹ػ����ٴθ�λ����*/
        while (1)
		{
			Is_Feed_Dog = TRUE;
			Feed_Dog();//ι��
			Delay_ms(250);
		}    
    }
}

/*******************************************************************************
* ����  : ExecShutdownAction
* ����  : �ػ�����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void ExecShutdownAction(void)
{
	U8 ucTemp[10];
	U8 ucTemp1[10];
	U16 PowerDownCount = 0;

	/*lq �ػ�ԭ��*/
	sprintf((char *)ucTemp, "%d", 1);
	Para_Save(PARA_CLOSE_RESION_TYPE, ucTemp, 1);//�����ػ�

	LEDAllLight();
	
    IO_Ctrl(IO_POWER_TYPE, IO_VAL_OFF);	

	while (PowerDownCount < 20)//LED��2���ر�
	{
		Is_Feed_Dog = TRUE;
		Feed_Dog();
		Delay_ms(100);
		PowerDownCount++;
	}	
	
//lq-		IO_Ctrl(IO_POWER_TYPE, IO_VAL_LOW);	
	while (1)
	{
/*lq-
		ucTemp_key1 = ReadSingleKeyValProcess(&PowerKey);
		//ucTemp_key2 = ReadSingleKeyValProcess(&SOSKey);

		if (ucTemp_key1 == KEY_LOW)//��power����һֱ����
		{
			Is_Feed_Dog = TRUE;
			Feed_Dog();//ι��
			Delay_ms(250);//һֱι��������֤�ػ��󳤰�POWER���ᵼ�¹ػ����ٴθ�λ����
		}
*/
//lq+		
		Is_Feed_Dog = TRUE;
		Feed_Dog();//ι��
		Delay_ms(250);//һֱι��������֤�ػ��󳤰�POWER���ᵼ�¹ػ����ٴθ�λ����
//lq~       
	} 
}
#endif
