/*******************************************************************************
  Copyright (C),  2012,  712.Co., Ltd.
  File name:	Tim.h
  Author: vozhj      Version: V1.00       Date: 2012.03.21 
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

#ifndef __TIM_H
#define __TIM_H

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
#define TIM_ADJUST_1S (48000)//��ʱ��1��
#define TIM_ADJUST_10S (480000)//��ʱ��10��
#define TIM_ADJUST_100S (4800000)//��ʱ��100��
extern S16 gsiPhaseCnt;
extern S16 gsiSlotCnt;
extern S32 gslFrameCnt;
extern U32 GpsTimCount;
extern U32 TimCnt;                                                                     //lq ����Tim������δУ׼��                                                                
extern U32 TimCnt1s;                                                                   //lq ����Tim��UTC1s����ڵļ���ֵ
extern U32 TimCnt10s;                                                                  //lq ����Tim��UTC10s����ڵļ���ֵ
extern U32 TimCnt100s;                                                                 //lq ����Tim��UTC100s����ڵļ���ֵ
extern S16 TimAdjust1;//��ʱ��ÿ2�����phase��
extern S16 TimAdjust10;//��ʱ��ÿ10�����phase��
extern S16 TimAdjust100;//��ʱ��ÿ50�����phase��
extern U8 ucTestCnt;

extern U8  gucFSKDataXmtEn;
/* Public  function  ---------------------------------------------------------*/ 
extern void InitTim2(void);
extern void InitTim2Para_2(void);
extern void Tim2Interrupt(void);
extern S16 GetPhaseNum(void);
extern S16 GetSlotNum(void);
extern S32 GetFrameNum(void);
extern S32 GetSlotSum(void);
extern S32 SetSlotTimer(U16 inTimeInterval);
extern void SetTimAdjust1(S16 data);
extern void SetTimAdjust10(S16 data);
extern void SetTimAdjust100(S16 data);
extern void ResetTimeCount(void);
extern void SetFSKDataXmtEn(void);
extern void ResetFSKDataXmtEn(void);
extern U8 GetFSKDataXmtEn(void);
extern U8 GetFSKDataXmtSleepState(void);
extern void SetFetchFirstFskDataEn(void);
extern void ResetFetchFirstFskDataEn(void);
extern U8 GetFetchFirstFskDataEn(void);
extern void TransPhaseAdjust(S16 * const pAdjust1, S16 * const pAdjust10, S16 * const pAdjust100);
#endif
