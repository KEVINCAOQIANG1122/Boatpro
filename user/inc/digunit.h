/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	interfunc.h
  Author: ���      Version: V1.00       Date: 2012.03.21 
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
#ifndef _DIGUNIT_H
#define _DIGUNIT_H
/* Private typedef -----------------------------------------------------------*/
#define DIGUNIT_MOD_IDLE	0
#define DIGUNIT_MOD_STEP1	1
#define DIGUNIT_MOD_STEP2	2
//#define DIGUNIT_MOD_STEP3	3
/* Private define ------------------------------------------------------------*/

/**
	@ingroup ���ݴ���
	@brief ���ݷ���������ƽṹ��
*/
typedef struct {
	U8  ModSta;		//����״̬��״̬
	U8  TranEnd;	//���ݷ�����ɱ�־
} DigUnitStru;
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
extern DigUnitStru	DigUnitPara;
/* Public  function  ---------------------------------------------------------*/
extern void InitaDigDeal(void);
/**
	@ingroup ���ݴ���
	@brief ���ݴ���������
	@param ��
	@retval ��
*/
extern void DigDealMain(void);
/**
	@ingroup ���ݴ���
	@brief ���ݵ��Ʒ���״̬������
	@param ��
	@retval ��
*/
extern void DigDealUnit(void);
/**
	@ingroup ���ݴ���
	@brief GMSK����
	@param ��
	@retval �� 

	GMSK���� �ŵ�ѡ���Լ�1846���ؿ���
*/
extern U8 GMSKTran(void);
extern U8 GetDigUnitSta(void);
extern U8 GMSKTran_ITDMA(void);

/**
	@ingroup ����
	@brief �鿴�źŴ���ģ���Ƿ�æ
	@param ��
	@retval TRUE �źŴ���ģ�����
	@retval FALSE �źŴ���ģ��æ
*/
extern U8 GetDigUnitSleepSta(void);
extern void RDA1846Ctrl(U8 Sta, U32 ulFreq, U8 cs);
extern void Ptl_Main(void);
#endif
