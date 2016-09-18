/*******************************************************************************
  Copyright (C), 2012, 712.Co.,Ltd.
  FileName: PTL_main.h
  Author: ����      Version : V1.00        Date: 2012.07.10
  Description:    ʵ��Э��ջ��Ϣ��֯�뷢��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#ifndef PTL_MAIN_H
#define PTL_MAIN_H

#include "gentype.h"

/**
	@brief 4503״̬��
*/
enum ctrlstate_4503
{
	INIT_4503,
	FIRST_GPS_4503,
	NORMAL_WORK_4503
};

/**
	@brief 4501״̬��
*/
enum ctrlstate_4501
{
	INIT_4501,
	NORMAL_WORK_BURST,
	NORMAL_WORK_MSG,
    NORMAL_WORK_IDLE,
    //lq+
    CHANGE_SEND_INTERVAL_4501
    //lq~
};

/*lq ��������*/
extern U8 state_4501;

/**
	@brief Э��ջ��ʼ��
	@retval 0 ���	
*/
U16 Ptl_Init(void);

/**
	@ingroup Э��ջ
	@brief 4503����������
	@param ��
	@retval 1 �ɹ�
	@retval 0 ʧ��
	
	4503ģʽ��Ϣ���Ϳ��ƺ���	
*/
U16 AISmain_4503(void);
/**
	@ingroup Э��ջ
	@brief 4501����������
	@param ��
	@retval 1 �ɹ�
	@retval 0 ʧ��
	
	4501ģʽ��Ϣ���Ϳ��ƺ���	
*/
U16 AISmain_4501(void);
/**
	@param time1 ����ʱ��1
	@param time2 ����ʱ��2
	@param timeout ʱ���
	@retval TRUE ��ʱ
	@retval FALSE δ��ʱ
*/
U16 IsTimeout(U16 time1, U16 time2, U16 timeout);

/**
	@ingroup Э��ջ
	@brief 4503����������
	@param ��
	@retval ��
	
	Э��ջ������	
*/
void Ptl_Main(void);

extern U16 Is_4503_Idle(void);
extern U16 sleep_4503(void);
extern U16 wakeup_4503(void);

extern U16 set_4501_interval(U16 speed);

extern U8 state_4503;

#endif
