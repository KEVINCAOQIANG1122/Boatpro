/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: interfunc.c
  Author: ���      Version : V1.00        Date: 2012.03.21
  Description:    ϵͳʱ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
//#include "stm32f10x.h"
#include "def.h"
#include "systemtick.h"
//#include "Tim.h"
//#include "keyscan.h"

/* public  variables ---------------------------------------------------------*/
struct LocalClockTickStruct LocalClockTick;
U32 testTimval = 0; //��ʱ������

/*******************************************************************************
* ����  : init_systermtick
* ����  : ��ʼ����ʱ����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void init_systermtick(void)
{
	//��ʼ����ʱ����
	LocalClockStateCtrlProcess();			//�رջ�򿪱���ʱ��ģ��
}

/*******************************************************************************
* ����  : GetLocalClockTickProcess
* ����  : ��ȡ��ǰ����ʱ�ӽ��Ĵ�����(Ӧ�ýӿں���)
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 GetLocalClockTickProcess(void)
{		
	return (LocalClockTick.TimeCnt);
}

/*******************************************************************************
* ����  : SetTimePeriodProcess
* ����  : ���ö�ʱʱ�䴦��(Ӧ�ýӿں���)
* ����  : Period����ʱ����
* ���  : ��
* ����  : ��ʱʱ��
*******************************************************************************/
U32 SetTimePeriodProcess(U32 Period)
{
	U32 TimePeirod;
	U32 TimeCnt;

	if (Period >= LOCAL_CLOCK_MAX_PEIROD)
	{
		Period = LOCAL_CLOCK_MAX_PEIROD - 1;
	}
	else if (Period == 0)
	{
		Period = 1;
	}
	
	TimePeirod = GetLocalClockTickProcess() + Period;
	if (TimePeirod >= LOCAL_CLOCK_MAX_PEIROD)
	{
		TimeCnt = TimePeirod - LOCAL_CLOCK_MAX_PEIROD;
	}
	else
	{
		TimeCnt = TimePeirod;
	}	
	
	return TimeCnt;
}

/*******************************************************************************
* ����  : LocalClockStateCtrlProcess
* ����  : ����ʱ��״̬���ƴ���(Ӧ�ýӿں���)
* ����  : State��
			FALSE���ر�
			TRUE������
* ���  : ��
* ����  : ��
*******************************************************************************/
void LocalClockStateCtrlProcess(void)
{	
	//5ms���ȶ�ʱ��
	LocalClockTick.TimeIntFlg = FALSE;
	LocalClockTick.TimeCnt = 0;
	LocalClockTick.TimePeriod = LOCAL_CLOCK_MAX_PEIROD;
	LocalClockTick.Time5msCnt = 0;
}

/*******************************************************************************
* ����  : LocalClockCtrlMainProcess
* ����  : ����ʱ�ӿ�����������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void LocalClockCtrlMainProcess(void)
{					
	if (LocalClockTick.TimeIntFlg == TRUE)                                      //zjw ��Time.c���ж���˵��//lq ÿ5ms������ʱ�Ӽ���һ��
	{
		LocalClockTick.TimeIntFlg = FALSE;
		
		LocalClockTick.TimeCnt++;                                               //lq 5ms��ʱ
	    if (LocalClockTick.TimeCnt >= LOCAL_CLOCK_MAX_PEIROD)                   //lq LOCAL_CLOCK_MAX_PEIROD=4min
	    {                                                                       //lq ����ͨ����ʱ�����ķ�ʽ��������ʱ���Ķ�ʱ
	    	LocalClockTick.TimeCnt = 0;                                         //lq ����ʱ�������Ͻ�����һ�εĶ�ʱ����ʹ��ʱ������1��
	    }					    
	}
}

