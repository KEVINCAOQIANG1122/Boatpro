/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: tempersensor.c
  Author: lq       Version : V1.00        Date: 2013.01.15
  Description:     �¶ȿ�����ع��ܵ�ʵ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include "config.h"

#if CONFIG_TEMPERATURE_MONITOR
#include "def.h"
#include "tempersensor.h"
#include "Adc.h"

const float fsAvg_Slope = 4.3;
const float fsV25  = 1.43;

TemperSensorStruct struTemperSensorPara;

/*******************************************************************************
* Function Name  : InitTemperSensorPara
* Description    : ��ʼ������״̬���Ʋ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitTemperSensorPara(void)
{
 	U16 i;

    struTemperSensorPara.TimePeriod = TEMPER_20MS_PERIOD;
	struTemperSensorPara.TimeCnt = SetTimePeriodProcess(struTemperSensorPara.TimePeriod);	

	struTemperSensorPara.AvgBufIndex = 0;

	for (i = 0; i < (TEMPER_AVGBUF_LEN + 1); i++)
	{
		struTemperSensorPara.AvgBuf[i] = 0xFFF;
	}

	struTemperSensorPara.AvgSum = (U32)0xFFF * 16;
    
    struTemperSensorPara.fsTemper = 25.0;	

    ResetADC1_TemperConvFinishFlag();
    
    ResetTemperOnFlag();    	   
}

/*******************************************************************************
* Function Name  : GetTemper
* Description    : ��ȡ��ǰ�¶�ֵ
* Input          : None
* Output         : None
* Return         : ��ǰ�¶�ֵ
*******************************************************************************/
float GetTemper(void)
{
    return struTemperSensorPara.fsTemper;
}

/*******************************************************************************
* Function Name  : CalcuTemper
* Description    : ���㵱ǰ�¶�ֵ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CalcuTemper(void)
{
	float fsVsense;
    U16 uiTempAvg;

    if (struTemperSensorPara.TimeCnt == GetLocalClockTickProcess())
	{
		struTemperSensorPara.TimeCnt = SetTimePeriodProcess(struTemperSensorPara.TimePeriod);


        /*lq ADCchannel8��14��15��16ת�����*/
        if (GetADC1_TemperConvFinishFlag())
        {
             ResetADC1_TemperConvFinishFlag();
        }
        else
        {
            return ;
        }
		
		//ƽ���˲�
		struTemperSensorPara.AvgSum -= struTemperSensorPara.AvgBuf[struTemperSensorPara.AvgBufIndex];
		struTemperSensorPara.AvgBuf[struTemperSensorPara.AvgBufIndex] = arrADC1_Val[3];
		struTemperSensorPara.AvgSum += (U32)arrADC1_Val[3];

		struTemperSensorPara.AvgBufIndex++;
		struTemperSensorPara.AvgBufIndex &= TEMPER_AVGBUF_LEN;

		uiTempAvg = (U16)(struTemperSensorPara.AvgSum >> 4);                         //lq ƽ��ֵ        
        
        if (!GetTemperOnFlag())
        {
            if (struTemperSensorPara.AvgBufIndex >= TEMPER_AVGBUF_LEN)
            {
                SetTemperOnFlag();
            }
            
            struTemperSensorPara.fsTemper = 25.0;
        }
        else
        {
            /*lq ���㵱ǰ�¶�ֵ*/
            fsVsense = GetTemperVolt(uiTempAvg);
        
            struTemperSensorPara.fsTemper = (1000.0 * (fsV25 - fsVsense) / fsAvg_Slope) + 25;     		    
        }                	
    }    
}

/*******************************************************************************
* Function Name  : GetTemperVolt
* Description    : ����AD����ֵ���㴫���������ѹֵ
* Input          : usSampValue - AD����ֵ
* Output         : None
* Return         : �����������ѹֵ
*******************************************************************************/
float GetTemperVolt(const U16 usSampValue)
{
    //lq... ��ʱʹ�ù�ʽ���㣬�Ժ����ʵ��ֵ
    return (3.3 * usSampValue) / 4096.0;                                          

}

/*******************************************************************************
* Function Name  : SetADC1_TemperConvFinishFlag
* Description    : �����¶�ת����ɱ�־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetADC1_TemperConvFinishFlag(void)
{
    struTemperSensorPara.ucADC1_TemperConvFinishFlag = TRUE;    
}

/*******************************************************************************
* Function Name  : ResetADC1_TemperConvFinishFlag
* Description    : ��λ�¶�ת����ɱ�־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetADC1_TemperConvFinishFlag(void)
{
    struTemperSensorPara.ucADC1_TemperConvFinishFlag = FALSE;    
}

/*******************************************************************************
* Function Name  : GetADC1_TemperConvFinishFlag
* Description    : �����¶�ת����ɱ�־
* Input          : None
* Output         : None
* Return         : ��ǰ״̬
*******************************************************************************/
U8 GetADC1_TemperConvFinishFlag(void)
{
    return struTemperSensorPara.ucADC1_TemperConvFinishFlag; 
}

/*******************************************************************************
* Function Name  : GetTemperOnFlag
* Description    : ��ȡ�¶�ֵ�Ƿ���Ч��־
* Input          : None
* Output         : None
* Return         : ��ǰ״̬
*******************************************************************************/
U8 GetTemperOnFlag(void)
{
    return struTemperSensorPara.ucTemperOnFlag;
}

/*******************************************************************************
* Function Name  : SetTemperOnFlag
* Description    : �����¶�ֵ��Ч��־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetTemperOnFlag(void)
{
    struTemperSensorPara.ucTemperOnFlag = TRUE;
}

/*******************************************************************************
* Function Name  : ResetTemperOnFlag
* Description    : ��λ�¶�ֵ��Ч��־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetTemperOnFlag(void)
{
    struTemperSensorPara.ucTemperOnFlag = FALSE;
}
#endif
