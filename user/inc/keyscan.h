/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	interfunc.h
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
#ifndef _KEYSCAN
#define _KEYSCAN
#define SYS_STATUE_NORMAL 0
#define SYS_STATUE_EMERGENT 1
void EMERGENCY_keyscan(void);
void Init_keyscan(void);
void POWER_keyscan(void);

extern U8 EMERGENCY_KeyStateFlg;

#endif
