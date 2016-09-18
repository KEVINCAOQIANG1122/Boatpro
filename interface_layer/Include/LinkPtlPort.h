/****************************************************************************
*  Copyright (C), 2010, 712.Co.,Ltd.                                        *
*  FileName: LinkPtlReport.h								   						*
*  Description:  �ײ���Э��ջ�����ӿں�����ر������弰��������							*
*****************************************************************************
*  History:               				                                    *
*      <author>  <time>    <version >   <desc>                              *
*      vozhj   	 10/11/25     1.0       Build this file					    *
****************************************************************************/
#ifndef _LINKPTLPORT_H
#define _LINKPTLPORT_H

#define LINKPTL_DATABUF_LEN		65
#define LINKPTL_FIFOLEN			15

#include "Ptl_Interface.h"
/*****************************************************************************
***********************			�ṹ�嶨��   		***********************
****************************************************************************/
typedef struct {
	U16 MessState;         			//��Ϣ����״̬                              //lq ��ʶЭ��ջ�еĸ�����Ϣ�Ƿ���Է��ͣ�TRUE-���Է��ͣ�����δ���ͣ�
	S16 TranSlotNum;         		//����ʱ϶��
	U8  ChannelNum;		  			//�ŵ���־0/1
	U8  DataBuf[LINKPTL_DATABUF_LEN];             			//��������ָ��
	U16 DataLength;		 			//�������ݳ���
} LinkPtlStruc;

typedef struct {
	LinkPtlStruc LinkPtlPara[LINKPTL_FIFOLEN + 1];                           //lq ������ʱ�ӿ���һ��Ԫ�ز��ã��������ֶ��еĿպ���
	U8  FifoIn;
	U8  FifoOut;
} LinkPtlFifoStruc;

/*
typedef struct {
	U16 MessState;         			//��Ϣ����״̬
	U16 TranSlotNum;         			//����ʱ϶��
	U16 ChannelNum;		  			//�ŵ���־0/1
	U16 Timer;						//��ʱ��
	U16 *pData;             			//��������ָ��
	U16 DataLength;		 			//�������ݳ���
	U16 GPSvalid;					    //GPS��Ч״̬
	U16 CurrentOperationFlg;          //��ǰҵ���־
} MessManaStr;
*/
extern LinkPtlFifoStruc LinkPtlFifoPara;
//extern MessManaStr mmMessCell;
/*****************************************************************************
***********************			��������    		***********************
****************************************************************************/
extern void InitaLinkPtlPortPara (void);
//extern void LinkPtlPort (void);

/**
	@ingroup ���ݴ���
	@brief 	��Э��ջ��������Ϣ����ӿ�FIFO
	@param mmMessCell Э��ջ�����Ϣ�Ľṹ��
	@retval ��
*/
extern void LinkPtlPort (MessManaStr *mmMessCell);
extern U8 GetPtlSta (void);
extern U8 ChkPtlFifoSta(void);

/**
	@ingroup ����
	@brief �鿴Э��ջ�ӿ�FIFO�Ƿ�Ϊ��
	@param ��
	@retval TRUE ��Ϣ�����꣬����
	@retval FALSE FIFO������Ϣδ������
*/
extern U8 GetPtlFifoSleepSta(void);
extern U8 IsLinkPtlQueEmpty(void);

#endif











