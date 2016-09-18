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
#include "config.h"

#if CONFIG_BAT_MONITOR
#include "def.h"
#include "stdio.h"
#include "string.h"
#include "initial.h"
#include "ledctrl.h"
#include "ioctrl.h"
#include "batteryctrl.h"
#include "systemtick.h"
#include "Adc.h"
#include "digunit.h"
#include "common.h"
#include "debug.h"
#include "gpsctrl.h"
#include "parastorage.h"
#include "batteryscan.h"
#include "wdg.h"

extern const U16 BatteryVolTable[20];
extern GPSStruct GpsPara;
BatteryCtrlStruct BatteryCtrlPara;

//AD�ɼ���ص�ѹ��Ӧ��ֵ
const U16 BatteryVolTable[20]=
{
	//5.5	5.6		5.7		5.8		5.9
	1520,	1550,	1578,	1608,	1635,

	//6.0	6.1		6.2		6.3		6.4
	1665,	1695,	1723,	1750,	1780,
	
	//6.5	6.6		6.7		6.8		6.9
	1808,	1838,	1866,	1896,	1926,
	
	//7.0	7.1		7.2		7.3		7.4
	1953,	1980,	2010,	2040,	2074
};

/*******************************************************************************
* ����  : InitBatteryCtrlPara
* ����  : ��ʼ����Դ�����Ҫ�Ĳ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitBatteryCtrlPara(void)
{	
	U8 i;
	U16 uiTemp;

	BatteryCtrlPara.TimePeriod = BATTERY_20MS_PERIOD;
	BatteryCtrlPara.TimeCnt = SetTimePeriodProcess(BatteryCtrlPara.TimePeriod);

#if 0
	BatteryCtrlPara.AvgBufIndex = 0;

	for (i = 0; i < (BATTERY_AVGBUF_LEN + 1); i++)
	{
		BatteryCtrlPara.AvgBuf[i] = 0xFFF;
	}

	BatteryCtrlPara.AvgSum = (U32)0xFFF * 16;
#endif

	BatteryCtrlPara.BatterySta = BATTERY_STA_IDLE;
	BatteryCtrlPara.BellowCnt = 0;
    BatteryCtrlPara.ucAboveCnt = 0;
	BatteryCtrlPara.SaveCloseTime = FALSE;
#if 0
	uiTemp = eqEquipPara.BatteryLowLevel - 55;

	if (uiTemp >= 20)//�����
	{
		uiTemp = 19;
	}

	BatteryCtrlPara.BatteryLowLevel = BatteryVolTable[uiTemp];//����ȡ��ѹ

	uiTemp = eqEquipPara.BatteryUltraLowLevel - 55;

	if (uiTemp >= 20)
	{
		uiTemp = 19;
	}

	BatteryCtrlPara.BatteryUltraLowLevel = BatteryVolTable[uiTemp];
#else
	uiTemp = eqEquipPara.BatteryLowLevel * 10;

	if (uiTemp > BATTERY_SCAN_VOLT_CEIL)//�����
	{
		uiTemp = BATTERY_SCAN_VOLT_CEIL;
	}
    else if (uiTemp < BATTERY_SCAN_VOLT_FLOOR)
    {
		uiTemp = BATTERY_SCAN_VOLT_FLOOR;
    }

	BatteryCtrlPara.BatteryLowLevel = uiTemp;//����ȡ��ѹ

	uiTemp = eqEquipPara.BatteryUltraLowLevel * 10;

	if (uiTemp > BATTERY_SCAN_VOLT_CEIL)//�����
	{
		uiTemp = BATTERY_SCAN_VOLT_CEIL;
	}
    else if (uiTemp < BATTERY_SCAN_VOLT_FLOOR)
    {
		uiTemp = BATTERY_SCAN_VOLT_FLOOR;
    }

	BatteryCtrlPara.BatteryUltraLowLevel = uiTemp;    
#endif
}

#if 0
/*******************************************************************************
* ����  : BatteryCtrlProcess
* ����  : ��ؿ���ģ�飬���ڼ���ص�ѹ�����ڵ͵�ѹ�澯�����͵�ѹ����ػ�ʱ��
* ����  : ��		  
* ���  : ģ��ִ���Ƿ�ɹ�
* ����  : ��
*******************************************************************************/
U8 BatteryCtrlProcess(void)
{
	U16 uiTempVal;
	U16 uiTempAvg;
	U8  uiRfSta;
	U8 ucTemp[10];
	U8 ucTemp1[10];

	if (BatteryCtrlPara.TimeCnt == GetLocalClockTickProcess())
	{
		BatteryCtrlPara.TimeCnt = SetTimePeriodProcess(BatteryCtrlPara.TimePeriod);

		//��ȡ��ǰ�����״̬
//lq		uiRfSta = GetDigUnitSta();
        uiRfSta = GetFSKDataXmtEn();

		//��Ϊ��״̬��������������
		if (uiRfSta == TRUE)
		{
			return (FALSE);
		}

#if 0

#if !WIN32
		//��ȡAD����ֵ
		uiTempVal = GetADC1_14_Val();
#else
		uiTempVal = 0;
#endif
		
		//ƽ���˲�
		BatteryCtrlPara.AvgSum -= BatteryCtrlPara.AvgBuf[BatteryCtrlPara.AvgBufIndex];
		BatteryCtrlPara.AvgBuf[BatteryCtrlPara.AvgBufIndex] = uiTempVal;
		BatteryCtrlPara.AvgSum += (U32)uiTempVal;
		BatteryCtrlPara.AvgBufIndex++;
		BatteryCtrlPara.AvgBufIndex &= BATTERY_AVGBUF_LEN;

		uiTempAvg = (U16)(BatteryCtrlPara.AvgSum >> 4);
#endif

        uiTempAvg = BS1_GetAvgValue();
		//usart_printf("$P712,Battery Val = %d\r\n", uiTempAvg);
		
		//��������״̬
		if (BatteryCtrlPara.BatterySta == BATTERY_STA_IDLE)
		{
			if (uiTempAvg < BatteryCtrlPara.BatteryLowLevel)
			{
				BatteryCtrlPara.BellowCnt++;

				if (BatteryCtrlPara.BellowCnt >= BATTERY_BELOW_CNT)
				{
					BatteryCtrlPara.BellowCnt = 0;
#if !WIN32
					LEDInit(LED_TYPE_LOWPOWER);
					UartResponseMsg("$P712,001,,,Low Power!");
#endif
					BatteryCtrlPara.BatterySta = BATTERY_STA_STEP1;
				}
			}
			else
			{
				BatteryCtrlPara.BellowCnt = 0;
			}
		}
		//�͵���״̬
		else if (BatteryCtrlPara.BatterySta == BATTERY_STA_STEP1)
		{
            if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_LOWPOWER)
            {
				LEDInit(LED_TYPE_LOWPOWER);
            }
            
            if (uiTempAvg < BatteryCtrlPara.BatteryUltraLowLevel)
			{
				BatteryCtrlPara.BellowCnt++;

				if (BatteryCtrlPara.BellowCnt >= BATTERY_BELOW_CNT)
				{
					BatteryCtrlPara.BellowCnt = 0;
					UartResponseMsg("$P712,001,,,Ultra Low Power!");
					BatteryCtrlPara.BatterySta = BATTERY_STA_STEP2;
				}
			}
			else
			{
				BatteryCtrlPara.BellowCnt = 0;
			}

			if (uiTempAvg >= BatteryCtrlPara.BatteryLowLevel)
			{
				BatteryCtrlPara.ucAboveCnt++;

				if (BatteryCtrlPara.ucAboveCnt >= BATTERY_BELOW_CNT)
				{
					BatteryCtrlPara.ucAboveCnt = 0;
#if !WIN32
					LEDInit(LED_TYPE_NONE);
					UartResponseMsg("$P712,001,,,Normal Power!");
#endif
					BatteryCtrlPara.BatterySta = BATTERY_STA_IDLE;
				}
			}
			else
			{
				BatteryCtrlPara.ucAboveCnt = 0;
			}
		}
		//���͵���״̬
		else if (BatteryCtrlPara.BatterySta == BATTERY_STA_STEP2)               //lq ���͵�ѹ״̬�޶���
		{
            if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_LOWPOWER)
            {
				LEDInit(LED_TYPE_LOWPOWER);
            }
			
            if (uiTempAvg >= BatteryCtrlPara.BatteryUltraLowLevel)
			{
				BatteryCtrlPara.ucAboveCnt++;

				if (BatteryCtrlPara.ucAboveCnt >= BATTERY_BELOW_CNT)
				{
					BatteryCtrlPara.ucAboveCnt = 0;
#if !WIN32
//lq					LEDInit(LED_TYPE_NONE);
					UartResponseMsg("$P712,001,,,LOW Power!");
#endif
					BatteryCtrlPara.BatterySta = BATTERY_STA_STEP1;
				}
			}
			else
			{
				BatteryCtrlPara.ucAboveCnt = 0;
			}

#if 0
			if (GetGpsPowerupFixState() == TRUE)                                //lq ֻ�п�����λ�ɹ��󣬲Ż��¼�ػ�ʱ��
			{
				if (BatteryCtrlPara.SaveCloseTime == FALSE)
				{
					//�ػ�ʱ��
                    sprintf((char *)ucTemp, "%2d", GpsPara.GPS_TimeMon);
            		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeD);
            		strcat((char *)ucTemp, (char *)ucTemp1);
                    sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeH);
            		strcat((char *)ucTemp, (char *)ucTemp1);
            		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeM);
            		strcat((char *)ucTemp, (char *)ucTemp1);
            		
            		Para_Save(PARA_TIME_CLOSE_TYPE, ucTemp, PARA_TIME_CLOSE_MAXLEN);
					UartResponseMsg("POWER DOWN");
					
					//�ػ�ԭ��
					sprintf((char *)ucTemp,"%d",2);
					Para_Save(PARA_CLOSE_RESION_TYPE,ucTemp,1);//�͵����ػ�
					BatteryCtrlPara.SaveCloseTime = TRUE;
				}
			}

			LEDAllLight();
			Delay_ms(2000);

			IO_Ctrl(IO_POWER_TYPE, IO_VAL_LOW);
	
			while (1)
			{
			}
#endif 
		}
	}

	return (TRUE);
}
#else
/*******************************************************************************
* ����  : BatteryCtrlProcess
* ����  : ��ؿ���ģ�飬ʹ�õ�ص���ɨ��ģ��ĵ�ѹֵ�����ڵ͵�ѹ�澯�����͵�ѹ����ػ�ʱ��
* ����  : ��		  
* ���  : ģ��ִ���Ƿ�ɹ�
* ����  : ��
*******************************************************************************/
U8 BatteryCtrlProcess(void)
{
	U16 uiTempVal;
	U16 uiTempAvg;
	U8  uiRfSta;
	U8 ucTemp[10];
	U8 ucTemp1[10];
	U16 PowerDownCount = 0;

	if (BatteryCtrlPara.TimeCnt == GetLocalClockTickProcess())
	{
		BatteryCtrlPara.TimeCnt = SetTimePeriodProcess(BatteryCtrlPara.TimePeriod);

		//��ȡ��ǰ�����״̬
        uiRfSta = GetFSKDataXmtEn();

		//��Ϊ��״̬��������������
		if (uiRfSta == TRUE)
		{
			return (FALSE);
		}

#if 0
        /*lq ��ȡAD����ֵ*/
        uiTempAvg = BS1_GetAvgValue();
#else
        /*lq ��ȡ��ǰ��ѹֵ*/
        uiTempAvg = BS1_GetCurVoltValue();
#endif
        
		//��������״̬
		if (BatteryCtrlPara.BatterySta == BATTERY_STA_IDLE)
		{
			if (uiTempAvg < BatteryCtrlPara.BatteryLowLevel)
			{
				BatteryCtrlPara.BellowCnt++;

				if (BatteryCtrlPara.BellowCnt >= BATTERY_BELOW_CNT)
				{
					BatteryCtrlPara.BellowCnt = 0;
#if !WIN32
					LEDInit(LED_TYPE_LOWPOWER);
					UartResponseMsg("$P712,001,,,Low Power!");
#endif
					BatteryCtrlPara.BatterySta = BATTERY_STA_STEP1;
				}
			}
			else
			{
				BatteryCtrlPara.BellowCnt = 0;
			}
		}
		//�͵���״̬
		else if (BatteryCtrlPara.BatterySta == BATTERY_STA_STEP1)
		{
            if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_LOWPOWER)
            {
				LEDInit(LED_TYPE_LOWPOWER);
            }
            
            if (uiTempAvg < BatteryCtrlPara.BatteryUltraLowLevel)
			{
				BatteryCtrlPara.BellowCnt++;

				if (BatteryCtrlPara.BellowCnt >= BATTERY_BELOW_CNT)
				{
					BatteryCtrlPara.BellowCnt = 0;
					UartResponseMsg("$P712,001,,,Ultra Low Power!");
					BatteryCtrlPara.BatterySta = BATTERY_STA_STEP2;
				}
			}
			else
			{
				BatteryCtrlPara.BellowCnt = 0;
			}

			if (uiTempAvg >= BatteryCtrlPara.BatteryLowLevel)
			{
				BatteryCtrlPara.ucAboveCnt++;

				if (BatteryCtrlPara.ucAboveCnt >= BATTERY_BELOW_CNT)
				{
					BatteryCtrlPara.ucAboveCnt = 0;
#if !WIN32
					LEDInit(LED_TYPE_NONE);
					UartResponseMsg("$P712,001,,,Normal Power!");
#endif
					BatteryCtrlPara.BatterySta = BATTERY_STA_IDLE;
				}
			}
			else
			{
				BatteryCtrlPara.ucAboveCnt = 0;
			}
		}
		//���͵���״̬
		else if (BatteryCtrlPara.BatterySta == BATTERY_STA_STEP2)               //lq ���͵�ѹ״̬�޶���
		{
            if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_LOWPOWER)
            {
				LEDInit(LED_TYPE_LOWPOWER);
            }
			
            if (uiTempAvg >= BatteryCtrlPara.BatteryUltraLowLevel)
			{
				BatteryCtrlPara.ucAboveCnt++;

				if (BatteryCtrlPara.ucAboveCnt >= BATTERY_BELOW_CNT)
				{
					BatteryCtrlPara.ucAboveCnt = 0;
#if !WIN32
//lq					LEDInit(LED_TYPE_NONE);
					UartResponseMsg("$P712,001,,,LOW Power!");
#endif
					BatteryCtrlPara.BatterySta = BATTERY_STA_STEP1;
				}
			}
			else
			{
				BatteryCtrlPara.ucAboveCnt = 0;
			}

#if CONFIG_BATTERY_CTRL_AUTOPOWERDOWN
			if (GetGpsPowerupFixState() == TRUE)                                //lq ֻ�п�����λ�ɹ��󣬲Ż��¼�ػ�ʱ��
			{
				if (BatteryCtrlPara.SaveCloseTime == FALSE)
				{
					//�ػ�ʱ��
                    sprintf((char *)ucTemp, "%2d", GpsPara.GPS_TimeMon);
            		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeD);
            		strcat((char *)ucTemp, (char *)ucTemp1);
                    sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeH);
            		strcat((char *)ucTemp, (char *)ucTemp1);
            		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeM);
            		strcat((char *)ucTemp, (char *)ucTemp1);
            		
            		Para_Save(PARA_TIME_CLOSE_TYPE, ucTemp, PARA_TIME_CLOSE_MAXLEN);
					UartResponseMsg("POWER DOWN");
					
					BatteryCtrlPara.SaveCloseTime = TRUE;
				}
			}

			//�ػ�ԭ��
			sprintf((char *)ucTemp,"%d",2);
			Para_Save(PARA_CLOSE_RESION_TYPE,ucTemp,1);//�͵����ػ�

			LEDAllLight();
        
        	while (PowerDownCount < 20)//LED��2���ر�
        	{
        		Is_Feed_Dog = TRUE;
        		Feed_Dog();
        		Delay_ms(100);
        		PowerDownCount++;
        	}	
	
			IO_Ctrl(IO_POWER_TYPE, IO_VAL_OFF);

        	while (1)
        	{
        		Is_Feed_Dog = TRUE;
        		Feed_Dog();//ι��
        		Delay_ms(250);//һֱι��������֤�ػ��󳤰�POWER���ᵼ�¹ػ����ٴθ�λ����
        	} 

#endif 
		}
	}

	return (TRUE);
}
#endif

/*******************************************************************************
* ����  : SLFCHK_SelfCheckStructInit
* ����  : ��ʼ���ṹ��
* ����  : pSelfCheckStruct - SelfCheckStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void BAT_BatterySelfCheckStructInit(BatterySelfCheckStruct *pBatterySelfCheckStruct)
{
	pBatterySelfCheckStruct->TimePeriod = BATTERY_SELFCHECK_TIME_PERIOD;	
    pBatterySelfCheckStruct->TimeCnt = SetTimePeriodProcess(pBatterySelfCheckStruct->TimePeriod);
	pBatterySelfCheckStruct->TimeoutCnt = 0;

    pBatterySelfCheckStruct->ValidCnt = 0;    
    pBatterySelfCheckStruct->StartFlg = TRUE;
    pBatterySelfCheckStruct->CompleteFlg = FALSE;
    pBatterySelfCheckStruct->SupplyVoltOkFlg = FALSE;        
}

/*******************************************************************************
* Function Name  : BAT_SelfCheck
* Description    : ��ع����ѹ�Լ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BAT_SelfCheck(BatterySelfCheckStruct *pBatterySelfCheckStruct)
{
	U16 uiTempAvg;

    if (pBatterySelfCheckStruct->StartFlg == TRUE)
    {
    	if (pBatterySelfCheckStruct->TimeCnt == GetLocalClockTickProcess())
    	{
    		pBatterySelfCheckStruct->TimeCnt = SetTimePeriodProcess(pBatterySelfCheckStruct->TimePeriod);
            pBatterySelfCheckStruct->TimeoutCnt++;

#if 0    
    		//��ȡ��ǰ�����״̬
            uiRfSta = GetFSKDataXmtEn();
    
    		//��Ϊ��״̬��������������
    		if (uiRfSta == TRUE)
    		{
    			return (FALSE);
    		}
#endif    
#if 0
            /*lq ��ȡAD����ֵ*/
            uiTempAvg = BS1_GetAvgValue();
#else
            /*lq ��ȡ��ǰ��ѹֵ*/
            uiTempAvg = BS1_GetCurVoltValue();
#endif
            if ((uiTempAvg <= BATTERY_SELFCHECK_VOLT_CEIL)
                && (uiTempAvg >= BATTERY_SELFCHECK_VOLT_FLOOR))
            {
                pBatterySelfCheckStruct->ValidCnt++;
            }
            
            if (pBatterySelfCheckStruct->TimeoutCnt >= BATTERY_SELFCHECK_TIME_TIMEOUT)
            {
                pBatterySelfCheckStruct->TimeoutCnt = 0;

                if (pBatterySelfCheckStruct->ValidCnt >= BATTERY_SELFCHECK_VALID_CNT_THRESH)
                {
                    pBatterySelfCheckStruct->SupplyVoltOkFlg = TRUE;
                }
                else
                {
                    pBatterySelfCheckStruct->SupplyVoltOkFlg = FALSE;                
                }

                pBatterySelfCheckStruct->CompleteFlg = TRUE;
                pBatterySelfCheckStruct->StartFlg = FALSE;
            }                        
        }    
    }
}
#endif
