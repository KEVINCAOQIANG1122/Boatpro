/*******************************************************************************
  Copyright (C), 2016, 712.Co.,Ltd.
  FileName:    PTT_lnw.h 
  Author:    Nathanlnw    Version : V1.00        Date: 2012.07.10
  Description:      Realize  PTT  Related  fucntions
  Version:         V1.00
  Function List:   1.  PTT control  Function   
                        2.    Cut out power  check  
                        3. When  mainpower  cut  hanppened,  device's  working mode enter into  low power status
                            which   disable  AIS RX   but  enable   AIS  TX.
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>

*******************************************************************************/


#ifndef  _PTT
#define  _PTT


#if  CONFIG_PTT_EN

#include "Include.h"

/*
    Fuction description:

  
	9-21
	//	 PC8 PC9  (39 40) ����	 ��PC9	 ����  �Խ������ܼ�� �ߣ��޽��� �ͣ��н���
	  �ĳɿ�PTT �շ�--���
	PD2 PC12   (53 54) ���� ��PC12	 ���  PTT���� �ͣ���  �ߣ���	 // 9-20   �ĳɸ߿��͹�
	�ĳ� PTT ���� ��� ����

    
    
    PA14 ����  �Խ������ܼ��         �ߣ��޽���            �ͣ��н���     
                     ����Ƿ��н���
   PA15 ���  �������ϵ�   �ߣ��� �ͣ���
                 �����澯����� ������������ֱ�Ӹ������   ��90 ��/min��

  PC11 ����  �ϵ���  �ͣ��е� �ߣ��ϵ�
                 �ϵ磺 �͹��ģ�ֻ�����գ�
  PC12 ���  PTT���� �ͣ��� �ߣ���
                      ������������ �ж�û�н��ܣ�����ָ�������� ������Ϻ��PTT
  PB8  ���   �Խ����ϵ� �ߣ���   �ͣ���   
                 �Խ��������Ϳ��� �����رա�

    //  Stage 2
    //new   2016-9-19
	PC8 PC9  (39 40) ����   ��PC9   ����  �Խ������ܼ�� �ߣ��޽��� �ͣ��н���
	
	PA14 PA15  (49 50) ���� ��PA15   ���  �������ϵ�   �ߣ��� �ͣ���
	
	PC10 PC11 (51 52)  ���� ��PC11   ����  �ϵ���  �ͣ��е� �ߣ��ϵ�
	
	PD2 PC12   (53 54) ���� ��PC12   ���  PTT���� �ͣ���  �ߣ���
	
	PB8 PB9    (61 62)  ���� ��PB8    ���   �Խ����ϵ� �ߣ���   �ͣ���


                 
*/

//  Define   Pins
#define PTT_RX_PORT			        GPIOC             //  �Ƿ�����������
#define PTT_RX_PIN_IN			    GPIO_Pin_12


#define PTT_RX_PORT_2			    GPIOD        //  �Ƿ�����������2
#define PTT_RX_PIN_IN_2			    GPIO_Pin_2

//------------------
#define PTT_WARNLIGHT_PORT		    GPIOA              //  ���Ʊ�����
#define PTT_WARNLIGHT_PIN_OUT	    GPIO_Pin_15

#define PTT_WARNLIGHT_PORT_2		GPIOA              //  ���Ʊ����� 2
#define PTT_WARNLIGHT_PIN_OUT_2	    GPIO_Pin_14


//----------------
#define PTT_MAINPOWERCUT_PORT		GPIOC             //  ����ⲿ��Դ״̬
#define PTT_MAINPOWERCUT_IN			GPIO_Pin_11

#define PTT_MAINPOWERCUT_PORT_2		GPIOC             //  ����ⲿ��Դ״̬ 2
#define PTT_MAINPOWERCUT_IN_2		GPIO_Pin_10      



//---------------

#define PTT_SPK_CTL_PORT			GPIOC             // �Ƿ�������
#define PTT_SPK_CTL_PIN_OUT			GPIO_Pin_9

#define PTT_SPK_CTL_PORT_2			GPIOC             // �Ƿ�������2
#define PTT_SPK_CTL_PIN_OUT_2		GPIO_Pin_8



//--------------

#define PTT_MODULE_POWERCTL_PORT	GPIOB             //  ����PTT ���ֵ�Դ
#define PTT_MODULE_POWERCTL_OUT		GPIO_Pin_8

#define PTT_MODULE_POWERCTL_PORT_2	GPIOB             //  ����PTT ���ֵ�Դ2
#define PTT_MODULE_POWERCTL_OUT_2	GPIO_Pin_9




//Define   USE
#define PTT_RX_STATUS_GET                GPIO_ReadInputDataBit(PTT_RX_PORT, PTT_RX_PIN_IN)


#define PTT_WARNLIGHT_ON       			GPIO_SetBits(PTT_WARNLIGHT_PORT, PTT_WARNLIGHT_PIN_OUT);GPIO_SetBits(PTT_WARNLIGHT_PORT_2, PTT_WARNLIGHT_PIN_OUT_2)
#define PTT_WARNLIGHT_OFF      		    GPIO_ResetBits(PTT_WARNLIGHT_PORT, PTT_WARNLIGHT_PIN_OUT);GPIO_ResetBits(PTT_WARNLIGHT_PORT_2, PTT_WARNLIGHT_PIN_OUT_2)


#define PTT_MAINPOWERCUT_STATUS_GET        GPIO_ReadInputDataBit(PTT_MAINPOWERCUT_PORT, PTT_MAINPOWERCUT_IN)



#define PTT_SPK_CTL_SD       			GPIO_SetBits(PTT_SPK_CTL_PORT, PTT_SPK_CTL_PIN_OUT);GPIO_SetBits(PTT_SPK_CTL_PORT_2, PTT_SPK_CTL_PIN_OUT_2)
#define PTT_SPK_CTL_RX    				GPIO_ResetBits(PTT_SPK_CTL_PORT, PTT_SPK_CTL_PIN_OUT);GPIO_ResetBits(PTT_SPK_CTL_PORT_2, PTT_SPK_CTL_PIN_OUT_2)

#define PTT_MODULE_POWER_ON       		GPIO_SetBits(PTT_MODULE_POWERCTL_PORT, PTT_MODULE_POWERCTL_OUT);GPIO_SetBits(PTT_MODULE_POWERCTL_PORT_2, PTT_MODULE_POWERCTL_OUT_2)
#define PTT_MODULE_POWER_OFF      		GPIO_ResetBits(PTT_MODULE_POWERCTL_PORT, PTT_MODULE_POWERCTL_OUT);GPIO_ResetBits(PTT_MODULE_POWERCTL_PORT_2, PTT_MODULE_POWERCTL_OUT_2)

#define PTT_MODULE_POWER_STATUS_GET     GPIO_ReadInputDataBit(PTT_MODULE_POWERCTL_PORT, PTT_MODULE_POWERCTL_OUT)

//#define PTT_RX_GET                      GPIO_ReadInputDataBit(PTT_RX_PORT, PTT_RX_PIN_IN)

#define RX_EN_PIN_GET                   GPIO_ReadInputDataBit(RX_EN_PORT, RX_EN_PIN)


//   struct  variable

typedef  struct 
{
   //  WarnLight
  U8    WarnLight_Control_Enable;   // Enable Warnlight  Work
  U32   WarnLight_time_counter;  //   timer
  U32   WarnLight_Keep_Duraiton;  //  warn keep  duration

  //  Main Power
  U8   LowPowerMode_Staus;  //  Lowpower Mode    0: idle    1:  enalbe   ,   if  it's enabled  : only send  AIS msg  not   receive AIS msg
  U8   MainPower_Status;     //      0 :  main normal      1 :    main power  cut  donwn    2:  other  idle state  for  programming use 
  
  //  PTT  control
  U8    PTT_powerOn_RunOnce;  // ruan ONCE
  U8    PTT_TX_Enable;   // Enable TX
  U8    PTT_Rx_STATE;//    0:  IDLE    1:   running
  U32   PTT_TX_timecounter;  //   timer
  U32   PTT_TX_Keep_Duraiton;  // tx  duration
  U32   PTT_IDLE_timecouteer;//  not send  timer
  U32   PTT_IDLE_Duration;//  idle  keep 
  U8    PTT_Demo_Enable;   //   0:   demo  stop     1:  demo start 

  //  timer
  U16   OneSecond_counter;// in   1ms  timer


} PTT_CLASS;

extern PTT_CLASS  PTT_OBJ;

#define WARNLIGHT_KEEP      5    //  30s
#define PTT_WAIT_KEEP       60 //
#define PTT_TX_KEEP         17
#define PTT_IDLE_KEEP       17


#define  Enable_PTT                   1
#define  Disable_PTT                  0

// Main Power Status
#define MAIN__POWER_NORMAL     0
#define MAIN__POWER_CutDown    1



extern void PTT_Work_Process(void);
extern void PTT_Timer_Service(void);
extern void PTT_WARNLIGHT_ENABLE(void);
extern void PTT_OBJ_INIT(void);
extern void PTT_Init_IO(void);


 #endif  //end  of  #if  CONFIG_PTT_EN
#endif

