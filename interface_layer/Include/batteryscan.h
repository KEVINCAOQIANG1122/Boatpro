/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: batteryscan.h
  Author: lq       Version : V1.00        Date: 2013.09.02
  Description:     ����ɨ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _BATTERYSCAN_H
#define _BATTERYSCAN_H

/* Private define ------------------------------------------------------------*/
#define BATTERY_SCAN_AVGBUF_LEN		    15
#define BATTERY_SCAN_20MS_PERIOD	    4

#define BATTERY_SCAN_VOLT_CEIL          840         //lq ��ѹ����ֵ����λ0.01v��
#define BATTERY_SCAN_VOLT_FLOOR         400         //lq ��ѹ����ֵ����λ0.01v��
#define BATTERY_SCAN_VOLT_PREC          10          //lq ��ѹ����ֵ����λ0.01v��

#define BATTERY_SCAN_VOLT_INTERVAL_NUM  ((BATTERY_SCAN_VOLT_CEIL - BATTERY_SCAN_VOLT_FLOOR) \
                                          / BATTERY_SCAN_VOLT_PREC) //lq ��ѹ������

#define BATTERY_SCAN_VOLT_STEADY_THRES  10          //lq ��ѹ�����ȶ���������λ20ms��

/* Private typedef -----------------------------------------------------------*/

/**
	@ingroup ���
	@brief ��ص������ṹ�壬������ص�ѹƽ����
*/
typedef struct {
	U32 TimeCnt;
	U16 TimePeriod;	
	
	U16 AvgBuf[BATTERY_SCAN_AVGBUF_LEN + 1];	        //   ƽ���˲���
	U8  AvgBufIndex;
	U32 AvgSum;                                         //lq ����ֵ�ܺ�
	U16 AvgValue;                                       //lq ƽ������ֵ

    S16 CurVoltIndex;                                   //lq ��ǰ��ѹ��������
    U16 CurVoltValue;                                   //lq ��ǰ��ѹֵ����λ0.01v��
    S16 NewVoltIndex;                                   //lq ��ȷ����ѹ��������
    U16 NewVoltCnt;                                     //lq ��ȷ����ѹ�������
    
    //U8  VoltInterval[BATTERY_SCAN_VOLT_INTERVAL_NUM + 2];   //lq ��ѹ����
} BatteryScanStruct;

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
extern BatteryScanStruct BatteryScanPara;

/* Public  function  ---------------------------------------------------------*/
/**
	@ingroup ���
	@brief ��ص������
*/

#endif
