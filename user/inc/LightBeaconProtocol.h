/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: LightBeaconProtocol.h
  Author: lq       Version : V1.00        Date: 2014.01.21
  Description:     Protocol between AIS and light beacon  
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _LIGHT_BEACON_PROTOCOL_H
#define _LIGHT_BEACON_PROTOCOL_H

/* Include ------------------------------------------------------------*/
#include "usart.h"

/* Private define ------------------------------------------------------------*/
/*lq ���Ͷ������ */
#define LB_PRT_REPET_BUFF_LEN               10

/*lq ʱ�䶨ʱ�궨�� */
#define LB_PRT_TIME_RESPONSE_INTERVAL       2   //lq ��λ5ms
#define LB_PRT_TIME_REPETITION_INTERVAL     200 //lq ��λ5ms

/*lq RDM��ʽ��������*/
#define LB_PRT_RDM_DATE                     1
#define LB_PRT_RDM_UTC_TIME                 2
#define LB_PRT_RDM_ANCHOR                   3
#define LB_PRT_RDM_MMSI                     4
#define LB_PRT_RDM_LIGHT_BEACON_TYPE        5
#define LB_PRT_RDM_NAME                     6
#define LB_PRT_RDM_AIS_STATUE               7
#define LB_PRT_RDM_USART_SETTING            8

/*lq RDM��ʽ��������*/
#define LB_PRT_WRM_DATE                     1
#define LB_PRT_WRM_UTC_TIME                 2
#define LB_PRT_WRM_ANCHOR                   3
#define LB_PRT_WRM_MMSI                     4
#define LB_PRT_WRM_LIGHT_BEACON_TYPE        5
#define LB_PRT_WRM_NAME                     6
#define LB_PRT_WRM_ATON_STATUE              7
#define LB_PRT_WRM_USART_SETTING            8

/*lq CDM��ʽ��������*/
#define LB_PRT_CDM_START_INTERROGATION      1
#define LB_PRT_CDM_AIS_SLEEP                2
#define LB_PRT_CDM_STOP_INTERROGATION       3

/* Private typedef -----------------------------------------------------------*/

/*lq light beacon protocol ͨ�ſ��ƽṹ��*/
typedef struct{

	U32 TimeCnt;
	U16 TimePeriod;	

    U8  InterrogationFlag;
#if 0
    U8  SendFIFO[LB_PRT_SEND_FIFO_DEPTH][2];    //lq SendFIFO[][0]��ŷ��ͱ��ĵ����ͺţ�SendFIFO[][1]��ŷ��ͱ����ش���ʣ����� 
    U16 SendFIFOIn;
    U16 SendFIFOOut;       
#endif
	U16 RepetBuff[LB_PRT_REPET_BUFF_LEN];      //lq RepetBuff[0]����ش�ʣ�������RepetBuff[1]���Ҫ�ش������������RepetBuff[2-]���Ҫ�ش����������ͺ� 
    U8  LB_Stat;
    U16 LB_TimeY;
	U16 LB_TimeMon;
	U16 LB_TimeD;
	U16 LB_TimeH;
	U16 LB_TimeM;
	U16 LB_TimeS;
	S32 LB_Latitude;
	S32 LB_Longitud;

    //lq struct UartRxFIFOStruct Uart2RxFIFO2;

}LBPrtCommStruct;

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
extern LBPrtCommStruct LBPrtCommPara;
/* Public  function  ---------------------------------------------------------*/

#endif
