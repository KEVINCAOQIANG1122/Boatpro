/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	uartmsg.h
  Author: vozhj     Version: V1.00       Date: 2012.03.21 
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
#ifndef _UARTMSG_H
#define _UARTMSG_H
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LON_LAT_PRECISION   4   //lq ��γ�Ⱦ��� 4 - 0.0001��

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/
extern void IniUartMsgPara(void);

/**
	@ingroup DEBUG 
	@brief ���ڵ��Ա��Ľ���
	@param pData ����ָ��
	@param uiLen ���ĳ���
	@retval TRUE �����ɹ�
	@retval FALSE ����ʧ��
*/
extern U8 UartMsgProcess(U8 *pData, U16 uiLen);
extern U8 GetSegment(char *pInput, char *Output, U16 SegmentIndex, U16 SumLength, U16 Max_Bytes);
#endif
