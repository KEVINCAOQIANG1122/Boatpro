/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: SOTDMA.h
  Author: lq       Version : V1.00        Date: 2012.04.24
  Description:     SOTDMA���뷽ʽ��Ϣ���Ϳ���ģ��ꡢ�ṹ�嶨�壬��������������
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _SOTDMA_H
#define _SOTDMA_H

/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef struct SOTDMAPara
{
    S16 BurstTxTimSlot;
    S32 BurstTxTimFrame;
    S16 MsgTxTimSlot;
    S32 MsgTxTimFrame;
    
    S16 BurstSendTimSlot;
    S32 BurstSendTimFrame;
    S16 MsgSendTimSlot;
    S32 MsgSendTimFrame;
    
    S8  Timeout;
    S8  MsgCount;
    U8  BurstFlg;
    U8  TxChannel;
    U8  BurstFinsFlg;
    U8  BurstStarFlg;
    U16 SlotOffset;
}SOTDMAParaStruct;

#if 0
typedef struct SOCommState
{
    U8  ucSyncState;                                                            // ͬ��״̬                                    
    S8  ucSlotTimeout;                                                          // ʱ϶��ʱ
    U16 usSubMsg;                                                               // ����Ϣ

    U16 usSlotNum;                                                              // ���ڴ˴η����ʱ϶��
    U16 usSlotOffset;                                                           // ʱ϶ƫ��
    U16 usUTC_HourMin;                                                          // UTCСʱ�ͷ���
    U16 usRecStations;                                                          // ���յ�̨վ��
}SOCommStateStruct;
#else
typedef struct ICommState
{
    U8  ucSyncState;                                                            // ͬ��״̬                                    
    U16 usSlotIncrement;                                                        // ʱ϶����
    U8  ucSlotNumbers;                                                          // ʱ϶��
    U8  ucKeepFlag;                                                             // ���ֱ��                                                         // ���յ�̨վ��
}ICommStateStruct;
#endif
/* Private macro ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function  ---------------------------------------------------------*/
/* Public  macro ------------------------------------------------------------*/
#define SLOT_INTERVAL_MSG_IN_BURST  75
#define SLOT_INTERVAL_SEND_TO_TX    10
#define SLOT_INTERVAL_GPS_ON_TO_TX  450     //12s
#define SLOT_INTERVAL_MCU_WAKEUP_TO_TX  375 //10s

#define TIME_OFFSET_SEND_TO_TX      10      //lq 10slots
#define TIME_OFFSET_GPSON_TO_TX     750     //lq 563slots = 20s��15s�޷����ͬ����

#define COMM_STAT_SLOT_OFFSET_CEIL  2475
#define COMM_STAT_SLOT_OFFSET_FLOOR 2025

/*lq Ĭ����Ϣ���ں�����*/
#define ITDMA_MSG_SEND_INTERVAL_DEF_VAL     3       //lq Ĭ�Ϸ��ͼ��3min
#define ITDMA_MSG_SEND_TIME_DEF_VAL         2       //lq Ĭ�Ϸ�������2

/*lq ITDMA Communication State*/
#define ITDMA_COM_STA_SLOT_INCRE_DEF_VAL    2250  //ITDMAͨ��״̬ ʱ϶����Ĭ��ֵ

/* Public  variables ---------------------------------------------------------*/
extern SOTDMAParaStruct struSOTDMAPara;
extern ICommStateStruct struICommState;

/* Public  function  ---------------------------------------------------------*/
extern void InitCommStateSOTDMA(void);
extern void InitSOTDMAPara(void);
extern U32 GetBurstSendTim(void);
extern U16 GetRandSlotOffset(S16 PeriodSlots);
extern void SetBurstTxTim(S32 usValue);
extern void SetMsgTxTim(S16 usValue);
extern void SetBurstSendTim(S16 usValue);
extern void SetMsgSendTim(S16 usValue);
extern void SetCommState(void);
extern U16 GetBurstTxTimSlot(void);
extern void SetCommStaSlotNum(S16 ssSlotNum);
extern void SetCommStaSubMsg(S8 ucTimeout);
extern void SetCommStaUTCHourMin(void);
extern void SetCommStaSlotOffset(S16 ssSlotOffset);
extern U16 GetCommStaSlotOffset(void);
extern U8 GetBurstFlg(void);


#endif
