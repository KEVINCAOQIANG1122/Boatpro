/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	ledctrl.h
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
#ifndef _LEDCTRL_H
#define _LEDCTRL_H
/* Private typedef -----------------------------------------------------------*/

/**
	@ingroup MMI
	@brief LED�ṹ�壬����LED�����������ȼ���
*/
typedef struct {
	U8  *pLEDGreen;			//LED�̵ƿ���ָ��
	U8  *pLEDRed;			//LED��ƿ���ָ��
	U8  *pLEDTimer;			//LED���ƶ�ʱ��
	U8  LEDReCycleFlag;		//ѭ����־
	U8  LEDPriority;		//���ȼ�	
	U8	LEDEn;				//ʹ�ܱ�־
	U8  ucType;				//����
	U8  ucBufLen;			//���峤��
	U8  ucLedFragmentTimer;	//�Ӽ����������ڼ���û�ε�ʱ��
	U8  ucLedFragment;		//�����������ڼ����ε���Ŀ
}LEDStruct;

/**
	@ingroup MMI
	@brief LED���ƽṹ�壬���ڿ�������LED
*/
typedef struct {
	U32 TimeCnt;
	U16 TimePeriod;			//����250ms��ʱ

	LEDStruct	LedGreenPara;	//��ɫLED1(Green)
	LEDStruct	LedYellowPara;	//��ɫLED2(Yellow)
	LEDStruct	LedRedPara;	//��ɫLED3(Red)
	LEDStruct	LedBluePara;	//��ɫLED4(Blue)
}LEDCtrlStruct;

/* Private define ------------------------------------------------------------*/
/*lq ���¶���LED�������ȼ���0Ϊ������ȼ���3Ϊ������ȼ������ȼ��Ƚ�ֻ����ͬһ��LED */
#define LED_CTRL_PRIORITY0		0
#define LED_CTRL_PRIORITY1		1
#define LED_CTRL_PRIORITY2		2
#define LED_CTRL_PRIORITY3		3
#define LED_CTRL_PRIORITY_H		(LED_CTRL_PRIORITY0)        //lq ������ȼ�
#define LED_CTRL_PRIORITY_L		(LED_CTRL_PRIORITY3)        //lq ������ȼ�

#define LED_CTRL_BUFF_DEPTH     12
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/*lq LED�������Ͷ��� */
#define LED_TYPE_NONE			        0
#define LED_TYPE_GPS_SEARCH		        1
#define LED_TYPE_GPS_FIX		        2
#define LED_TYPE_RF_XMT			        3
#define LED_TYPE_POWER_NORMAL	        4
#define LED_TYPE_POWER_TEST	            5
#define LED_TYPE_TEST_TEST1	            6
#define LED_TYPE_TEST_TEST2		        7
#define LED_TYPE_TEST_TEST3		        8
#define LED_TYPE_TEST_TEST4		        9
#define LED_TYPE_TEST_RX		        10
#define LED_TYPE_SELF_CHECK_IDLE        11
#define LED_TYPE_SELF_CHECK_RUNNING     12
#define LED_TYPE_SELF_CHECK_OK          13
#define LED_TYPE_SELF_CHECK_ERR         14
#define LED_TYPE_TIMEOUT                15
#define LED_TYPE_ERR                    16
#define LED_TYPE_GREEN_NONE			    17
#define LED_TYPE_YELLOW_NONE			18
#define LED_TYPE_RED_NONE			    19
#define LED_TYPE_BLUE_NONE			    20

/*lq LED����ѭ������ */
#define LED_RECYCLE_NONE		FALSE
#define LED_RECYCLE				TRUE

/* Public  variables ---------------------------------------------------------*/
extern LEDCtrlStruct LEDCtrlPara;
/* Public  function  ---------------------------------------------------------*/
/**
	@ingroup MMI
	@param ucType LED��˸����
	@retval ��
	@brief LED���ƽӿں�����ָ��LED��˸����
*/
extern void LEDInit(U8 ucType);
extern void InitaLEDCtrlPara(void);
/**
	@ingroup MMI
	@param ��
	@retval ��
	@brief LED����������
*/
extern void LEDCtrlProcess(void);
extern void LEDAllLight(void);
extern void LED1AllDown(void);
extern void LED2AllDown(void);
extern void LEDAllDown(void);
#endif
