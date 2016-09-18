/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	batteryctrl.h
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
#ifndef _BATTERYCTRL_H
#define _BATTERYCTRL_H

/* Private define ------------------------------------------------------------*/
#define BATTERY_AVGBUF_LEN		15
#define BATTERY_20MS_PERIOD		4

#define BATTERY_STA_IDLE		0
#define BATTERY_STA_STEP1		1
#define BATTERY_STA_STEP2		2
#define BATTERY_STA_STEP3		3
#define BATTERY_STA_STEP4		4

#define BATTERY_LOW_THRESHOLD		1896		//6.8V��ѹ����
#define BATTERY_ULTRALOW_THRESHOLD	1723		//6.2V��ѹ����

#define BATTERY_BELOW_CNT			500        // ����λ20ms��	

#define BATTERY_SELFCHECK_TIME_PERIOD               100     //lq unit:5ms
#define BATTERY_SELFCHECK_TIME_TIMEOUT              5       //lq unit:500ms
#define BATTERY_SELFCHECK_VOLT_CEIL                 740     //lq unit:0.01V ʹ�ô��ڹ����ʹ�õ�Դ���������0.2V
#define BATTERY_SELFCHECK_VOLT_FLOOR                700     //lq unit:0.01V ���Ϊ0.2V
#define BATTERY_SELFCHECK_VALID_CNT_THRESH          5       //lq unit:1

/* Private typedef -----------------------------------------------------------*/

/**
	@ingroup ���
	@brief ��ص������ṹ�壬������ص�ѹƽ����
*/
typedef struct {
	U32 TimeCnt;
	U16 TimePeriod;	
#if 0	
	U16 AvgBuf[BATTERY_AVGBUF_LEN + 1];	//ƽ���˲���
	U8  AvgBufIndex;
	U32 AvgSum;
#endif
	U8 BatterySta;			//״̬��״̬

    U16 ucAboveCnt;                                                              //lq �ɵͻ򳬵͵��������������޵Ĵ���
	U16 BellowCnt;			//���뵽�������µĴ���
	U8 SaveCloseTime;

	U16 BatteryLowLevel;//��ص͵�ѹ����
	U16 BatteryUltraLowLevel;//��س��͵�ѹ����
} BatteryCtrlStruct;

typedef struct BatterySelfCheckStruct
{
	U32 TimeCnt;
	U16 TimePeriod;	
    U16 TimeoutCnt;

    U8  ValidCnt;
    U8  StartFlg;
    U8  CompleteFlg;
    U8  SupplyVoltOkFlg;
}BatterySelfCheckStruct;

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
extern BatteryCtrlStruct BatteryCtrlPara;

/* Public  function  ---------------------------------------------------------*/
extern void InitBatteryCtrlPara(void);
/**
	@ingroup ���
	@brief ��ص������
*/
extern U8 BatteryCtrlProcess(void);
#endif
