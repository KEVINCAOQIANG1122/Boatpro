/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: LinkManagementEntity.h
  Author: lq       Version : V1.00        Date: 2014.07.11
  Description:     AIS��·����ʵ���
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _LINK_MANAGEMENT_ENTITY_H
#define _LINK_MANAGEMENT_ENTITY_H

/* Include ------------------------------------------------------------------*/
#include "Include.h"

/* Public define ------------------------------------------------------------*/
#define LME_BINMSG_FIFO_DEPTH              7
#define LME_BINMSG_MAX_LEN_BYTE            160  //lq 5��ʱ϶�ܹ�256*5=1280bit=160byte

/* Public typedef -----------------------------------------------------------*/
#if 0
typedef struct BinMsgFifoStruct
{
    U8  FIFO[LME_BINMSG_FIFO_DEPTH][LME_BINMSG_MAX_LEN_BYTE + 1];  //lq DataBuff[0][]���ʵ�����ݳ��ȣ�5��ʱ϶�ܹ�256*5=1280bit=160byte, 
    U16 Input;
    U16 Output;
}BinMsgFifoStruct;
#endif

typedef struct BinMsgRxCellStruct
{
	U8  Data[LME_BINMSG_MAX_LEN_BYTE + 1];
	U16 DataLen;                                        //lq ��λ��1byte
	//lq U16 DCOffset;
	S32 ArrivalFrame;
    S16 ArrivalSlot;
    AisChannelEnum  RxChannel;	
}BinMsgRxCellStruct;

typedef struct BinMsgTxCellStruct
{
	U8  Data[LME_BINMSG_MAX_LEN_BYTE + 1];
	U16 DataLen;                                        //lq ��λ��1byte
    S16 TxSlot;
    AisChannelEnum TxChannel;	
}BinMsgTxCellStruct;

typedef struct BinMsgRxFifoStruct
{
    BinMsgRxCellStruct FIFO[LME_BINMSG_FIFO_DEPTH];
    U16 Input;
    U16 Output;
}BinMsgRxFifoStruct;

typedef struct BinMsgTxFifoStruct
{
    BinMsgTxCellStruct FIFO[LME_BINMSG_FIFO_DEPTH];
    U16 Input;
    U16 Output;
}BinMsgTxFifoStruct;

/* Public variables ---------------------------------------------------------*/

/* Public function ----------------------------------------------------------*/

#endif
