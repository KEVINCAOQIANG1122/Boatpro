/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	Debug.h
  Author: ��־��      Version: V1.00       Date: 2010.09.10 
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
#ifndef __DEBUG_H__
#define __DEBUG_H__

#define MMI_DEBUG_MSG	0
#define PTL_DEBUG_MSG	1
#define ARM_DEBUG_MSG	2
#define DSP_DEBUG_MSG	3
#define PD_DEBUG_MSG	4
#define FLASH_DEBUG_MSG 5
#define NO_DEBUG_MSG    10

extern U8 DebugOutputFlag;

U8 UartTxString(char *pStr);
void usart_printf(char * format, ...);

U8 UartDebugMsg(U8 Type,  U8 Addr,  char * format,...);

U8 UartResponseMsg(char *pData);
U8 UartTxString(char *pStr);

#endif




