/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	systemtick.h
  Author: ���      Version: V1.00       Date: 2012.03.22 
  Description:  
  				��Ҫ���ܣ� 
				1. ������غ궨��
				2. ��������
				3. ��������
				4. ����ṹ������
				
  Others:         // �������ݵ�˵��
  Function List:  // ��Ҫ�����б�ÿ����¼Ӧ���������������ܼ�Ҫ˵��
  History:        // �޸���ʷ��¼�б�ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸�
                  // �߼��޸����ݼ���  
    1. Date:
       Author:
       Modification:
*******************************************************************************/
#ifndef _SYSTICK
#define _SYSTICK
#define LOCAL_CLOCK_MAX_PEIROD	(48000)
struct LocalClockTickStruct
{
	U8  State;
	U8  TimeIntFlg; 
	U16 TimeCnt;
	U16 TimePeriod;
	U8  Time5msCnt;	
};
extern struct LocalClockTickStruct LocalClockTick;
void LocalClockCtrlMainProcess(void);
U16 GetLocalClockTickProcess(void);
void LocalClockStateCtrlProcess(void);
U32 SetTimePeriodProcess(U32 Period);
void SystemTickKeyScan(void);
extern U32 testTimval;
#endif 
