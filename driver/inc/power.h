/*******************************************************************************
  Copyright (C), 2010, 712.Co.,Ltd.
  File name:	power.h
  Author: ���      Version: V1.00       Date: 2012.03.12 
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
#ifndef __POWER_H__
#define __POWER_H__
#include "Include.h"

/* Private define ------------------------------------------------------------*/
//���մ�λ�ǵ�·����
#define POWER_PORT						    PUBLIC_PORT	 
#define KEY_PORT						    GPIOA
#define POWER_PORT_RCC_APB2Periph_GPIO  	RCC_APB2Periph_GPIOB
//lq #define POWER_CRL_PIN						GPIO_Pin_13		   //s0             //lq ������D��������CLK
//lq #define POWER_ON_PIN						GPIO_Pin_14		   //s1             //lq ������D��������D
//lq #define POWER_EMERGENCY_PIN					GPIO_Pin_15		  //s2
#define POWER_CLK_PIN						PUBLIC_PIN		   //s0             //lq ������D��������CLK
#define POWER_CRL_PIN						PUBLIC_PIN		   //s1             //lq ������D��������D
#define POWER_ON_PIN						PUBLIC_PIN		   //s1
#define POWER_EMERGENCY_PIN					PUBLIC_PIN		  //s2

#if 0
#define RF_POWER_PORT						GPIOA
#define RF_POWER_PORT_RCC_APB2Periph_GPIO  	RCC_APB2Periph_GPIOB
#define RF_POWER_PIN						GPIO_Pin_4		   //s0
#else
#define RF_POWER_PORT						PUBLIC_PORT
#define RF_POWER_PORT_RCC_APB2Periph_GPIO  	PUBLIC_RCC_APB2Periph_GPIO
#define RF_POWER_PIN						PUBLIC_PIN		   //s0
#endif

/*lq RF����LDOʹ�� */
#define RF_VCC_PORT						    GPIOC
#define RF_VCC_PIN						    GPIO_Pin_12
#define RF_VCC_LOW()       				    GPIO_ResetBits(RF_VCC_PORT, RF_VCC_PIN) 
#define RF_VCC_HIGH()      				    GPIO_SetBits(RF_VCC_PORT, RF_VCC_PIN)

#define RF_VCC_OFF()       				    RF_VCC_LOW()
#define RF_VCC_ON()      				    RF_VCC_HIGH()

/*lq ��1�����ſ���*/
#define RF_POWER_1_PORT                     GPIOA
#define RF_POWER_1_PIN                      GPIO_Pin_8
//�ж϶���
#define POWER_SOURCE_PORT	           		GPIO_PortSourceGPIOA

#define POWER_ON_SOURCE_PIN	           		GPIO_PinSource12
#define POWER_ON_LINE        				EXTI_Line12

#define POWER_EMERGENCY_SOURCE_PIN	   		GPIO_PinSource11
#define POWER_EMERGENCY_LINE   				EXTI_Line11

//IO���ú궨��
#define POWER_CRL_LOW()       				GPIO_ResetBits(POWER_PORT, POWER_CRL_PIN) //S0�õ� 
#define POWER_CRL_HIGH()      				GPIO_SetBits(POWER_PORT, POWER_CRL_PIN)

#define POWER_CRL_OFF()       				POWER_CRL_LOW()
#define POWER_CRL_ON()      				POWER_CRL_HIGH()

#define POWER_CLK_LOW()                     GPIO_ResetBits(POWER_PORT, POWER_CLK_PIN)
#define POWER_CLK_HIGH()      				GPIO_SetBits(POWER_PORT, POWER_CLK_PIN)

#define RF_POWER_1_LOW()       				GPIO_ResetBits(RF_POWER_1_PORT, RF_POWER_1_PIN) //S0�õ� 
#define RF_POWER_1_HIGH()      				GPIO_SetBits(RF_POWER_1_PORT, RF_POWER_1_PIN)

#define RF_POWER_LOW()       				GPIO_ResetBits(RF_POWER_PORT, RF_POWER_PIN) //S0�õ� 
#define RF_POWER_HIGH()      				GPIO_SetBits(RF_POWER_PORT, RF_POWER_PIN)


#define READ_POWER_ON_PIN()                 GPIO_ReadInputDataBit(KEY_PORT, POWER_ON_PIN)//��S1
#define READ_POWER_EMERGENCY_PIN()          GPIO_ReadInputDataBit(KEY_PORT, POWER_EMERGENCY_PIN)//��S2

/*lq դѹ���Ƶ�ص�ѹ����*/
#define RF_DA_CTRL_VOLT_LEVEL_NUM   5
#define RF_DA_CTRL_VOLT_LEVEL_0  480 //����λ0.01v��
#define RF_DA_CTRL_VOLT_LEVEL_1  500 //����λ0.01v��
#define RF_DA_CTRL_VOLT_LEVEL_2  550 //����λ0.01v��
#define RF_DA_CTRL_VOLT_LEVEL_3  570 //����λ0.01v��

/* Private typedef -----------------------------------------------------------*/
typedef struct {
	U8  SosKeyFlag;		//SOS����״̬
	U8  PowerKeyFlag;	//��Դ����״̬
} PowerStru;

typedef struct {
    U8  RFAntState;     //lq ��Ƶ����״̬
} RFStruct;

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/ 
extern void Initia_POWER_Crl(void);
extern void Initia_RFPower(void);
extern void Exti15_10_Int_PowerKey(void);
extern void Exti15_10_Int_SosKey(void);
extern U8 GetSosLowPowerSta(void);
extern U8 GetPowerLowPowerSta(void);
extern void RF_PowerDown(void);
extern void RF_PowerOn(void);

extern void InitPowerCtrl(void);//add by lnw

#endif
