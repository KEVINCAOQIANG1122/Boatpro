/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: TaskManager.c
  Author: lq       Version : V1.0.0        Date: 2015.07.31
  Description:     �����������
  Version:         V1.0.0
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
    <author>    <time>      <version >  <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "Include.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/
TaskTimStruct RecordCloseTimeTask;    // GPS ���ػ�
TaskTimStruct UpdateRecStationTask;

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* ����  : TASK_TaskTimStructInit
* ����  : TaskTimStruct��ʼ���ṹ��
* ����  : pTaskTimStruct - TaskTimStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void TASK_TaskTimStructInit(TaskTimStruct *pTaskTimStruct)
{
	pTaskTimStruct->TimeCnt = 0;
	pTaskTimStruct->TimePeriod = TASK_TIME_PERIOD;	
    pTaskTimStruct->Timeout = TASK_TIME_TIMEOUT;
    pTaskTimStruct->TimeoutCnt = 0;

    pTaskTimStruct->Enable = FALSE;
}

/*******************************************************************************
* ����  : TASK_TaskTimEnable
* ����  : ����ʱʹ��
* ����  : pTaskTimStruct - TaskTimStruct�ṹ��ָ��
          timeout - ��������	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void TASK_TaskTimEnable(TaskTimStruct *pTaskTimStruct, U16 timeout)
{
	pTaskTimStruct->TimeCnt = SetTimePeriodProcess(pTaskTimStruct->TimePeriod);
    pTaskTimStruct->Timeout = timeout;
    pTaskTimStruct->TimeoutCnt = 0;

    pTaskTimStruct->Enable = TRUE;
}

/*******************************************************************************
* ����  : TASK_TaskTimDisable
* ����  : ����ʱʧ��
* ����  : pTaskTimStruct - TaskTimStruct�ṹ��ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void TASK_TaskTimDisable(TaskTimStruct *pTaskTimStruct)
{
    pTaskTimStruct->Enable = FALSE;
}

/*******************************************************************************
* ����  : TASK_PeriodTaskIsTimeout
* ����  : ��������ʱ�ж�
* ����  : pTaskTimStruct - TaskTimStruct�ṹ��ָ��
* ���  : ��
* ����  : �Ƿ�����ʱ��
*******************************************************************************/
U8 TASK_PeriodTaskIsTimeout(TaskTimStruct *pTaskTimStruct)
{
    if (pTaskTimStruct->Enable == TRUE)
    {
        if (pTaskTimStruct->TimeCnt == GetLocalClockTickProcess())
    	{
    		pTaskTimStruct->TimeCnt = SetTimePeriodProcess(pTaskTimStruct->TimePeriod);
            pTaskTimStruct->TimeoutCnt++;                
            if (pTaskTimStruct->TimeoutCnt >= pTaskTimStruct->Timeout)
            {
                pTaskTimStruct->TimeoutCnt = 0;
    
            	return TRUE;
            }                        
        }    
    }

    return FALSE;
}

/*******************************************************************************
* ����  : TASK_SingleTaskIsTimeout
* ����  : ��������ʱ�ж�
* ����  : pTaskTimStruct - TaskTimStruct�ṹ��ָ��
* ���  : ��
* ����  : �Ƿ�����ʱ��
*******************************************************************************/
U8 TASK_SingleTaskIsTimeout(TaskTimStruct *pTaskTimStruct)
{
    if (pTaskTimStruct->Enable == TRUE)
    {
        if (pTaskTimStruct->TimeCnt == GetLocalClockTickProcess())
    	{
    		pTaskTimStruct->TimeCnt = SetTimePeriodProcess(pTaskTimStruct->TimePeriod);
            pTaskTimStruct->TimeoutCnt++;                
            if (pTaskTimStruct->TimeoutCnt >= pTaskTimStruct->Timeout)
            {
                pTaskTimStruct->TimeoutCnt = 0;
    
            	pTaskTimStruct->Enable = FALSE;
                
                return TRUE;
            }                        
        }    
    }

    return FALSE;
}

/*******************************************************************************
* ����  : TASK_TaskProcess
* ����  : ����������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void TASK_TaskProcess(void)
{
    DateTimeStruct dateTime;
	U8  ucTemp[10];
	U8  ucTemp1[15];

    /*lq ��¼ GPS �ػ�ʱ�� */
    if (TASK_PeriodTaskIsTimeout(&RecordCloseTimeTask) == TRUE)
    {
		/*lq ��¼�ػ�ʱ�� */
        dateTime.Year = GpsPara.GPS_TimeY;
        dateTime.Month = GpsPara.GPS_TimeMon;
        dateTime.Day = GpsPara.GPS_TimeD;
        dateTime.Hour = GpsPara.GPS_TimeH;
        dateTime.Minute = GpsPara.GPS_TimeM;
        dateTime.Second = GpsPara.GPS_TimeS;
        
        sprintf((char *)ucTemp, "%02d", (dateTime.Year) % 100);
    	sprintf((char *)ucTemp1, "%02d", dateTime.Month);
    	strcat((char *)ucTemp, (char *)ucTemp1);
        sprintf((char *)ucTemp1, "%02d", dateTime.Day);
    	strcat((char *)ucTemp, (char *)ucTemp1);
    	sprintf((char *)ucTemp1, "%02d", dateTime.Hour);
    	strcat((char *)ucTemp, (char *)ucTemp1);
        sprintf((char *)ucTemp1, "%02d", dateTime.Minute);
    	strcat((char *)ucTemp, (char *)ucTemp1);
    	sprintf((char *)ucTemp1, "%02d", dateTime.Second);
    	strcat((char *)ucTemp, (char *)ucTemp1);

        Para_CloseTimeSave(ucTemp, DevDynamicPara.OpenCloseTimeIndex);
        debug_printf("$[%6d:%4d] %s : Record close time!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);

    }

    /*lq ���½��մ�̨�� */
    if (TASK_PeriodTaskIsTimeout(&UpdateRecStationTask) == TRUE)
    {
        AISINFO_InstRecStationQuantityUpdate();
        debug_printf("$[%6d:%4d] %s : Update received stations!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    }
}

/*******************************************************************************
* ����  : TASK_ModuleInit
* ����  : ģ���ʼ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void TASK_ModuleInit(void) 
{
    TASK_TaskTimStructInit(&RecordCloseTimeTask);
    TASK_TaskTimStructInit(&UpdateRecStationTask);
    TASK_TaskTimEnable(&UpdateRecStationTask,
        TASK_TIME_TIMEOUT_UPDATE_REC_STATION);
}

/*==============================================================================
------------------------------------End of File---------------------------------
==============================================================================*/
