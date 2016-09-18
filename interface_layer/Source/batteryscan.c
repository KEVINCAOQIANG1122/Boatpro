/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: batteryscan.c
  Author: lq       Version : V1.00        Date: 2013.09.02
  Description:     ����ɨ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include "config.h"

#if CONFIG_BAT_MONITOR 
#include "def.h"
#include "stdio.h"
#include "string.h"
#include "Adc.h"
#include "batteryscan.h"
#include "debug.h"

#if 0
/*lq ��ѹAD����ֵ���飨ADδ����У׼ʱ�Ĳ���ֵ������ѹ��Ϊ10/28*/
const U16 BatteryVolSampleTable[BATTERY_SCAN_VOLT_INTERVAL_NUM + 1]=
{
	//4.0	4.1		4.2		4.3		4.4
	1700,	1744,	1787,	1834,	1877,

	//4.5	4.6		4.7		4.8		4.9
	1922,	1966,	2009,	2058,	2103,
	
	//5.0	5.1		5.2		5.3		5.4
	2151,	2195,	2238,	2282,	2328,
	
	//5.5	5.6		5.7		5.8		5.9
	2372,	2415,	2460,	2505,	2549,

    //6.0
    2594
};
#else
/*lq ��ѹAD����ֵ���飨ADδ����У׼ʱ�Ĳ���ֵ������ѹ��Ϊ10/28*/
const U16 BatteryVolSampleTable[BATTERY_SCAN_VOLT_INTERVAL_NUM + 1]=
{
	//4.0	4.1		4.2		4.3		4.4
	1705,	1748,	1797,	1842,	1885,

	//4.5	4.6		4.7		4.8		4.9
	1931,	1975,	2019,	2064,	2108,
	
	//5.0	5.1		5.2		5.3		5.4
	2156,	2200,	2245,	2289,	2333,
	
	//5.5	5.6		5.7		5.8		5.9
	2377,	2421,	2467,	2515,	2561,

	//6.0	6.1		6.2		6.3		6.4
    2605,   2648,   2693,   2737,   2781,

	//6.5	6.6		6.7		6.8		6.9
    2825,   2871,   2913,   2958,   3001,

	//7.0	7.1		7.2		7.3		7.4
    3047,   3093,   3137,   3181,   3229,

	//7.5	7.6		7.7		7.8		7.9
    3273,   3321,   3361,   3405,   3457,

	//8.0	8.1		8.2		8.3		8.4
    3496,   3541,   3593,   3637,   3677,
};
#endif
/*lq ����ɨ���������*/
BatteryScanStruct BatteryScanPara;

/*******************************************************************************
* Function Name  : BS_BatteryScanStructInit
* Description    : ��ʼ��BatteryScanStruct����
* Input          : pBatteryScanPara - ָ��ṹ�������ָ��
* Output         : None
* Return         : None
*******************************************************************************/
static void BS_BatteryScanStructInit(BatteryScanStruct *pBatteryScanPara)
{
    U16 i;

    pBatteryScanPara->TimePeriod = BATTERY_SCAN_20MS_PERIOD;
    pBatteryScanPara->TimeCnt = SetTimePeriodProcess(pBatteryScanPara->TimePeriod);

	for (i = 0; i < (BATTERY_SCAN_AVGBUF_LEN + 1); i++)
	{
		pBatteryScanPara->AvgBuf[i] = 0xFFF;
	}	

	pBatteryScanPara->AvgBufIndex = 0;
	pBatteryScanPara->AvgSum = (U32)0xFFF * 16;                             
	pBatteryScanPara->AvgValue = (U16)(pBatteryScanPara->AvgSum >> 4);   //lq ƽ������ֵ
    
    pBatteryScanPara->CurVoltIndex = BATTERY_SCAN_VOLT_INTERVAL_NUM;     //lq ��ʼֵΪ���ֵ
    pBatteryScanPara->CurVoltValue = BATTERY_SCAN_VOLT_CEIL;             //lq ��ʼֵΪ���ֵ 
    pBatteryScanPara->NewVoltIndex = BATTERY_SCAN_VOLT_INTERVAL_NUM;     //lq ��ʼֵΪ���ֵ
    pBatteryScanPara->NewVoltCnt = 0;                                    //lq ��ȷ����ѹ����
}

/*******************************************************************************
* Function Name  : BS_GetVoltIndex
* Description    : ȷ�����������ڵ�����
* Input          : sampleVal - AD����ֵ
* Output         : None
* Return         : ���������ڵ���������ֵ
*******************************************************************************/
static S16 BS_GetVoltIndex(U16 sampleVal)
{
    S16 i;
    
    /*lq ȷ�����������ڵ�����*/
    for (i = BATTERY_SCAN_VOLT_INTERVAL_NUM; i >= 0; i--)
    {
        if (sampleVal >= BatteryVolSampleTable[i])
        {
            break;
        }
    }
    
    return i;      //i = -1 ��ʾ����ֵ���ڶ������͵�ѹֵ
}

/*******************************************************************************
* Function Name  : BS_GetVoltValue
* Description    : ���ݵ�ѹ��������ֵȷ����ѹֵ
* Input          : VoltIndex - ��ѹ��������
* Output         : None
* Return         : ��ѹ����������Ӧ�ĵ�ѹֵ����λ0.01v��
*******************************************************************************/
static U16 BS_GetVoltValue(S16 VoltIndex)
{
    return BATTERY_SCAN_VOLT_FLOOR + VoltIndex * BATTERY_SCAN_VOLT_PREC;
}

/*******************************************************************************
* Function Name  : BS_BatteryScanProcess
* Description    : ����ɨ�����
* Input          : pBatteryScanPara - ָ��ṹ�������ָ��
* Output         : None
* Return         : None
*******************************************************************************/
static void BS_BatteryScanProcess(BatteryScanStruct *pBatteryScanPara)
{
	U8  uiRfSta;
	U16 uiTempVal;
    S16 siTempIndex;

	if (pBatteryScanPara->TimeCnt == GetLocalClockTickProcess())
	{
		pBatteryScanPara->TimeCnt = SetTimePeriodProcess(pBatteryScanPara->TimePeriod);

		/*lq ��ȡ��ǰ�����״̬*/
        uiRfSta = GetFSKDataXmtEn();

		/*lq ��Ϊ��״̬��������������*/
		if (uiRfSta == TRUE)
		{
			return ;
		}

#if !WIN32
        /*lq ADCchannel8��14��15ת�����*/
        if (GetADC1_ConvFinishFlag())
        {
             ResetADC1_ConvFinishFlag();
        }
        else
        {
            return ;
        }        
		
        /*lq ��ȡAD����ֵ*/
		uiTempVal = GetADC1_14_Val();
#else
		uiTempVal = 0;
#endif
		
		/*lq ƽ���˲�*/
		pBatteryScanPara->AvgSum -= pBatteryScanPara->AvgBuf[pBatteryScanPara->AvgBufIndex];
		pBatteryScanPara->AvgBuf[pBatteryScanPara->AvgBufIndex] = uiTempVal;
		pBatteryScanPara->AvgSum += (U32)uiTempVal;
		pBatteryScanPara->AvgBufIndex++;
		pBatteryScanPara->AvgBufIndex &= BATTERY_SCAN_AVGBUF_LEN;
		pBatteryScanPara->AvgValue = (U16)(pBatteryScanPara->AvgSum >> 4);
    
        /*lq ȷ������ֵ��Ӧ�ĵ�ѹ����*/
        siTempIndex = BS_GetVoltIndex(pBatteryScanPara->AvgValue);
        if (siTempIndex == pBatteryScanPara->NewVoltIndex)
        {
            /*lq ����ֵ�ȶ�*/
            pBatteryScanPara->NewVoltCnt++;
            if (pBatteryScanPara->NewVoltCnt >= BATTERY_SCAN_VOLT_STEADY_THRES)
            {
                /*lq ����ֵ�����ȶ���ʱ��ﵽ�趨ֵ�����øò���ֵ��Ϊ��ǰ��ѹֵ*/
                pBatteryScanPara->NewVoltCnt = 0;
                pBatteryScanPara->CurVoltIndex = pBatteryScanPara->NewVoltIndex;
                pBatteryScanPara->CurVoltValue = BS_GetVoltValue(pBatteryScanPara->CurVoltIndex);
            }
        }
        else
        {
            /*lq ����ֵ��������¼�µĲ���ֵ*/
            pBatteryScanPara->NewVoltIndex = siTempIndex;
            pBatteryScanPara->NewVoltCnt = 0;
        }
    }
}

/*******************************************************************************
* Function Name  : BS_GetAvgValue
* Description    : ��ȡ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static U16 BS_GetAvgValue(BatteryScanStruct *pBatteryScanPara)
{
    return pBatteryScanPara->AvgValue;
}

/*******************************************************************************
* Function Name  : BS_InstanceInit
* Description    : ��ʼ��batteryscanģ�����ز���
* Input          : pBatteryScanPara - ָ��ṹ�������ָ��
* Output         : None
* Return         : None
*******************************************************************************/
static void BS_InstanceInit(BatteryScanStruct *pBatteryScanPara)
{
    BS_BatteryScanStructInit(pBatteryScanPara);
}

/*******************************************************************************
* Function Name  : BS_GetCurVoltValue
* Description    : ���ص�ǰ��ѹֵ
* Input          : pBatteryScanPara - ָ��ṹ�������ָ��
* Output         : None
* Return         : None
*******************************************************************************/
static U16 BS_GetCurVoltValue(BatteryScanStruct *pBatteryScanPara)
{
    return pBatteryScanPara->CurVoltValue;
}

/*******************************************************************************
* Function Name  : BS_GetCurVoltValue
* Description    : ���ص�ǰ��ѹֵ
* Input          : None
* Output         : None
* Return         : ʵ��1�ĵ�ѹֵ
*******************************************************************************/
U16 BS1_GetCurVoltValue(void)
{
    return BatteryScanPara.CurVoltValue;
}

/*******************************************************************************
* Function Name  : BS1_InstanceInit
* Description    : ��ʼ��batteryscanģ�����ز���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BS1_InstanceInit(void)
{
    BS_BatteryScanStructInit(&BatteryScanPara);
}

/*******************************************************************************
* Function Name  : BS1_BatteryScanProcess
* Description    : ����ɨ�����ʵ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BS1_BatteryScanProcess(void)
{
    BS_BatteryScanProcess(&BatteryScanPara);
}

/*******************************************************************************
* Function Name  : BS1_GetAvgValue
* Description    : ��ȡʵ��1����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
U16 BS1_GetAvgValue(void)
{
   BS_GetAvgValue(&BatteryScanPara); 
}
#endif

#if 0
/*******************************************************************************
* Function Name  : BS_BatteryScanProcess
* Description    : ����ɨ�����
* Input          : pBatteryScanPara - ָ��ṹ�������ָ��
* Output         : None
* Return         : None
*******************************************************************************/
U16 tempValue = 0;
void BS_ModuleTest(void)
{
    BS1_BatteryScanProcess(); 
    
    if (tempValue != BatteryScanPara.CurVoltValue) 
    {
        tempValue = BatteryScanPara.CurVoltValue;
        usart_printf("$Current Volt : %04d \r\n", BatteryScanPara.CurVoltValue); 
    }  
}

/*lq AD����ͳ��*/
#define BATTERY_SCAN_VOLT_SAMPLE_NUM            1000    //lq �ܲ�������
#define BATTERY_SCAN_VOLT_SAMPLE_TIM_PERIOD     4       //lq 20ms
#define BATTERY_SCAN_VOLT_SAMPLE_TIM_PERIOD_CNT 1       //lq 20ms
#define BATTERY_SCAN_VOLT_SAMPLE_POINT_NUM      100     //lq ����������

typedef struct BatVoltSampleStruct
{
	U32 TimeCnt;
	U16 TimePeriod;
    U16 TimePeriodCnt;	    
    
    U16 SamplePoint[2][BATTERY_SCAN_VOLT_SAMPLE_POINT_NUM + 1];  //lq SamplePoint[����ֵ][��������]
    U16 MaxNumIndex;            //lq �����������ֵ��Ӧ������
    U16 CurSamplePointNum;      //lq ��ǰ�Ĳ�������
    U16 CurSampleCnt;           //lq ��ǰ�Ĳ������� 
}BatVoltSampleStruct;       

BatVoltSampleStruct  BatVoltSampleInsta;

/*******************************************************************************
* Function Name  : BS_BatVoltSampleStructInit
* Description    : �ṹ���ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void  BS_BatVoltSampleStructInit(BatVoltSampleStruct *pBatVoltSample)
{
    U16 i,j;

	pBatVoltSample->TimePeriod = BATTERY_SCAN_VOLT_SAMPLE_TIM_PERIOD;
	pBatVoltSample->TimeCnt = SetTimePeriodProcess(pBatVoltSample->TimePeriod);	
    pBatVoltSample->TimePeriodCnt = 0;

    for (i = 0; i < BATTERY_SCAN_VOLT_SAMPLE_POINT_NUM + 1; i++)
    {
        for (j = 0; j < 2; j++)
        {
            pBatVoltSample->SamplePoint[j][i] = 0;
        }
    }

    pBatVoltSample->MaxNumIndex = 0;
    pBatVoltSample->CurSamplePointNum = 0;
    pBatVoltSample->CurSampleCnt = 0;
}

/*******************************************************************************
* Function Name  : BS_InstanceInit
* Description    : �ṹ��ʵ����ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void  BS_TestInstanceInit(void)
{
   BS_BatVoltSampleStructInit(&BatVoltSampleInsta); 
}

/*******************************************************************************
* Function Name  : BS_BatVoltSampleProcess
* Description    : ��ѹ����
* Input          : pBatVoltSample - �ṹ��ָ��
* Output         : None
* Return         : None
*******************************************************************************/
static void BS_BatVoltSampleProcess(BatVoltSampleStruct *pBatVoltSample)
{
    U16 i;
    U16 uiTempVal;

	if (pBatVoltSample->CurSampleCnt < BATTERY_SCAN_VOLT_SAMPLE_NUM)
    {
        if (pBatVoltSample->TimeCnt == GetLocalClockTickProcess())
    	{
    		pBatVoltSample->TimeCnt = SetTimePeriodProcess(pBatVoltSample->TimePeriod);
    
            pBatVoltSample->TimePeriodCnt++;
            if (pBatVoltSample->TimePeriodCnt >= BATTERY_SCAN_VOLT_SAMPLE_TIM_PERIOD_CNT)
            {
                /*lq ���ڵ�*/
                pBatVoltSample->TimePeriodCnt = 0;
    
                /*lq ��ȡ����ֵ*/
                uiTempVal = GetADC1_14_Val();
                pBatVoltSample->CurSampleCnt++;
                usart_printf("Sample Point Value : %4d\r\n", uiTempVal);
                
                for (i = 1; i < pBatVoltSample->CurSamplePointNum + 1; i++)
                {
                    /*lq �жϵ�ǰ����ֵ�Ƿ��ڼ�¼�Ĳ�������*/
                    if (pBatVoltSample->SamplePoint[0][i] == uiTempVal)
                    {
                        /*lq ��ǰ����ֵ�ڼ�¼�Ĳ������У����������1*/
                        pBatVoltSample->SamplePoint[1][i]++;

                        if (pBatVoltSample->SamplePoint[1][i] > pBatVoltSample->SamplePoint[1][pBatVoltSample->MaxNumIndex])
                        {
                            pBatVoltSample->MaxNumIndex = i;
                        } 
                        break;
                    }
                }
    
                if (i >= pBatVoltSample->CurSamplePointNum + 1)
                {
                    /*lq �ò��������²����㣬����������������*/
                    if (pBatVoltSample->CurSamplePointNum < BATTERY_SCAN_VOLT_SAMPLE_POINT_NUM)
                    {
                        /*lq ����������1*/
                        pBatVoltSample->CurSamplePointNum++;
    
                        pBatVoltSample->SamplePoint[0][pBatVoltSample->CurSamplePointNum] = uiTempVal;
                        pBatVoltSample->SamplePoint[1][pBatVoltSample->CurSamplePointNum]++;
                    }
                    else
                    {
                        /*lq �����������е�0��Ԫ�ش洢δ��¼�µĲ�����Ĳ�����*/
                        pBatVoltSample->SamplePoint[1][0]++;
                    }
                }

                if (pBatVoltSample->CurSampleCnt >= BATTERY_SCAN_VOLT_SAMPLE_NUM)
                {
                    usart_printf("Sample Finished! \r\n \
                                  Total Sample Num        : %4d \r\n \
                                  Total Sample Point Num  : %4d \r\n \
                                  Most Sample Point Index : %4d \r\n \
                                  Most Sample Point Value : %4d \r\n \
                                  Most Sample Point Count : %4d \r\n \
                                  Beyond Sample Count     : %4d \r\n ",
                                  pBatVoltSample->CurSampleCnt,
                                  pBatVoltSample->CurSamplePointNum,
                                  pBatVoltSample->MaxNumIndex,
                                  pBatVoltSample->SamplePoint[0][pBatVoltSample->MaxNumIndex],
                                  pBatVoltSample->SamplePoint[1][pBatVoltSample->MaxNumIndex],
                                  pBatVoltSample->SamplePoint[1][0]);
                } 
            }
    
    
        
        }     
    }
}

/*******************************************************************************
* Function Name  : BS1_BatVoltSampleProcess
* Description    : ��ѹ����ʵ��1
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BS1_BatVoltSampleProcess(void)
{
    BS_BatVoltSampleProcess(&BatVoltSampleInsta);
}
#endif



