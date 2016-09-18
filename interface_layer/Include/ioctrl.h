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
#ifndef _IOCTRL_H
#define _IOCTRL_H
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
#define IO_LED_GREEN_TYPE		0		//Green LED����
#define IO_LED_YELLOW_TYPE		1		//Yellow LED����
#define IO_LED_RED_TYPE		    2		//Red LED����
#define IO_LED_BLUE_TYPE		3		//Blue LED����
#define IO_POWER_TYPE		    4	 	//������Դ����
#define IO_GPSPOWER_TYPE	    5	   	//GPS��Դ����
#define IO_RDA1846_1_TYPE	    6		//RDA1846����
#define IO_RDA1846_2_TYPE	    7		//RDA1846����
#define IO_RX_EN_TYPE           8       //���տ���
#define IO_ANT_SW_RX_TYPE       9       //����ת������
#define IO_RDA1846_1_TX_TYPE    10      
#define IO_RDA1846_1_RX_TYPE    11
#define IO_RDA1846_2_TX_TYPE    12
#define IO_RDA1846_2_RX_TYPE    13
#define IO_RF_VCC_TYPE          14

#define IO_VAL_LOW			0 
#define IO_VAL_HIGH			1 

#define IO_VAL_OFF			0 
#define IO_VAL_ON			1 

/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/
extern void IO_Ctrl(U8 ucType, U8 ucVal);
extern void IO_Write(U8 ucType, U32 ulVal);
#endif
