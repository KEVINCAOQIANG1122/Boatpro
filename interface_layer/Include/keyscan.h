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
#ifndef _KEYSCAN_H
#define _KEYSCAN_H
/* Private typedef -----------------------------------------------------------*/

/**
	@ingroup MMI 
	@brief �����ṹ�壬�������ڰ�����ʱ�ͷ�������
*/
typedef struct {
	U16 TimeCnt;
	U16 TimePeriod;	
	
	U8 XDouCnt;
	U8 Val;
	U8 ChangeFlg;					
} SingleKeyStruct;

extern SingleKeyStruct PowerKey;
extern SingleKeyStruct SOSKey;
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
#define KEY_POWER_ADDR		0
#define KEY_SOS_ADDR		1

#define KEY_LOW				0
#define KEY_HIGH			1
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/
extern void IniSingleKey(U8 IO_Addr, U8 InitVal);
extern U8 ReadSingleKeyValProcess(SingleKeyStruct *pSingleKeyStruct);

/**
	@ingroup MMI
	@brief ��ȡ����״̬�ӿ�
	@param IO_Addr ����ΪSOS����POWER
	@retval KEY_HIGH �����ӿڸߵ�ƽ
	@retval KEY_LOW �����ӿڵ͵�ƽ
*/
extern U8 GetIndivIO(U8 IO_Addr);

/**
	@ingroup MMI
	@brief ����ɨ��������
	@param ��
	@retval ��
*/
extern void KeyScanProcess(void);

/**
	@ingroup MMI
	@brief ����ɨ��
	@param pSingleKeyStruct ������Ӧ�Ľṹ��
	@retval ��
*/
extern void SingleKeyCtrl(SingleKeyStruct *pSingleKeyStruct);
#endif
