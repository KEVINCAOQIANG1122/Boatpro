/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: TaskManager.h
  Author: lq       Version : V1.0.0        Date: 2015.07.31
  Description:     �����������
  Version:         V1.0.0
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
    <author>    <time>      <version >  <desc>
*******************************************************************************/
#ifndef _TASKMANAGER_H_
#define _TASKMANAGER_H_

/* Include -------------------------------------------------------------------*/
#include "Include.h"

/* Public define -------------------------------------------------------------*/
/*lq ʱ��궨�� */
#define TASK_TIME_PERIOD                200                 //lq unit:5ms
#define TASK_TIME_TIMEOUT               1                   //lq unit:1s
#define TASK_TIME_TIMEOUT_RECORD_CLOSE_TIME     3600        //lq unit:1s��i.e. 1h
#define TASK_TIME_TIMEOUT_UPDATE_REC_STATION    30          //lq unit:1s��i.e. 30s

/* Public typedef ------------------------------------------------------------*/
typedef struct 
{
	U32 TimeCnt;
	U16 TimePeriod;	
    U16 Timeout;
    U16 TimeoutCnt;
    
    U8  Enable;
}TaskTimStruct;

/* Public variables ----------------------------------------------------------*/
extern TaskTimStruct RecordCloseTimeTask;
extern TaskTimStruct UpdateRecStationTask;

/* Public functions ----------------------------------------------------------*/

#endif
/*==============================================================================
------------------------------------End of File---------------------------------
==============================================================================*/
