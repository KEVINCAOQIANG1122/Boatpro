/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: tempersensor.h
  Author: lq       Version : V1.00        Date: 2013.01.15
  Description:     �¶ȴ������ĺꡢ�ṹ�嶨�壬��������������
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _TEMPERSENSOR_H
#define _TEMPERSENSOR_H

/* Includes ------------------------------------------------------------------*/
#include "def.h"

/* Public  macro -------------------------------------------------------------*/
#define TEMPER_20MS_PERIOD		4
#define TEMPER_AVGBUF_LEN		15

/* Private typedef -----------------------------------------------------------*/
typedef struct {
	U32 TimeCnt;
	U16 TimePeriod;	
	
	U16 AvgBuf[TEMPER_AVGBUF_LEN + 1];	//ƽ���˲���
	U8  AvgBufIndex;
	U32 AvgSum;
    float fsTemper;
    U8 ucADC1_TemperConvFinishFlag;
    U8  ucTemperOnFlag;                                                         //lq �¶���ֵ��Ч
} TemperSensorStruct;
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function  ---------------------------------------------------------*/

/* Public  variables ---------------------------------------------------------*/
extern TemperSensorStruct struTemperSensorPara;

/* Public  function  ---------------------------------------------------------*/
extern void InitTemperSensorPara(void);
extern float GetTemper(void);
extern void CalcuTemper(void);
extern void SetADC1_TemperConvFinishFlag(void);
extern void ResetADC1_TemperConvFinishFlag(void);
extern U8 GetADC1_TemperConvFinishFlag(void);
extern float GetTemperVolt(const U16 usSampValue);
extern void SetTemperOnFlag(void);
extern void ResetTemperOnFlag(void);
extern U8 GetTemperOnFlag(void);

#endif
