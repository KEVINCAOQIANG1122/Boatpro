/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: SPIRIT1.c
  Author: lq       Version : V1.00        Date: 2014.06.30
  Description:     SPIRIT1����
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "stm32f10x_lib.h"
#include "def.h"
#include "SPIRIT1.h"
#include "gpiospi.h"
#include "spi.h"
#include "PhysicalLayer.h"
#include "DataLinkService.h"
#include "LED.h"
#include "initial.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
//lq static void SPIRIT1_ReadRegisters(SPIRIT1TypeEnum cs);

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* ����  : SPIRIT1_InitPins
* ����  : ��ʼ��SPIRIT1����
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_InitPins(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

    /*lq ------------------SPIRIT1_A ��������---------------------------*/
    /* Configure SPIRIT1_A SDN ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = SPIRIT1_A_SDN_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(SPIRIT1_A_SDN_PORT, &GPIO_InitStructure);
    SPIRIT1_A_SDN_LOW();                                    //lq оƬ����        

    /* Configure SPIRIT1_A SPI ---------------------------------*/
#if 0
    GPIO_InitStructure.GPIO_Pin = SPIRIT1_A_SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(SPIRIT1_A_SPI_CS_PORT, &GPIO_InitStructure);
    SPIRIT1_A_SPI_CS_HIGH();                                //lq Ƭѡʧ��
    
    GPIO_InitStructure.GPIO_Pin = SPIRIT1_A_SPI_SCLK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(SPIRIT1_A_SPI_SCLK_PORT, &GPIO_InitStructure);
    SPIRIT1_A_SPI_SCLK_LOW();

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_A_SPI_MOSI_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(SPIRIT1_A_SPI_MOSI_PORT, &GPIO_InitStructure);
    SPIRIT1_A_SPI_MOSI_LOW();                               

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_A_SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
    GPIO_Init(SPIRIT1_A_SPI_MISO_PORT, &GPIO_InitStructure);
#else
    SPI2_Init();
#endif
    /* Configure SPIRIT1_A GPIO ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = SPIRIT1_A_GPIO0_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //lq Tx Data
    GPIO_Init(SPIRIT1_A_GPIO0_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_A_GPIO1_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //lq Rx Data 
    GPIO_Init(SPIRIT1_A_GPIO1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_A_GPIO2_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;           //lq Tx Clock    
    GPIO_Init(SPIRIT1_A_GPIO2_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_A_GPIO3_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;           //lq Rx Clock
    GPIO_Init(SPIRIT1_A_GPIO3_PORT, &GPIO_InitStructure);

	/* Connect EXTI Line to GPIO Pin */
	GPIO_EXTILineConfig(SPIRIT1_A_GPIO2_SOURCE_PORT, SPIRIT1_A_GPIO2_SOURCE_PIN);   //lq TX clock
	
	/* Configure EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = SPIRIT1_A_GPIO2_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //lq TxʱMCU��ʱ���½��ظ�������
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 

	/* Connect EXTI Line to GPIO Pin */
	GPIO_EXTILineConfig(SPIRIT1_A_GPIO3_SOURCE_PORT, SPIRIT1_A_GPIO3_SOURCE_PIN);   //lq RX clock

	/* Configure EXTI Line to generate an interrupt on rising edge */  
	EXTI_InitStructure.EXTI_Line = SPIRIT1_A_GPIO3_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
#if 0
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //lq RxʱMCU��ʱ�������ز�������
#else
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
#endif
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 

    /*lq ------------------SPIRIT1_B ��������---------------------------*/
    /* Configure SPIRIT1_B SDN ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = SPIRIT1_B_SDN_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(SPIRIT1_B_SDN_PORT, &GPIO_InitStructure);
    SPIRIT1_B_SDN_LOW();                                    //lq оƬ����        

    /* Configure SPIRIT1_B SPI ---------------------------------*/
#if 0
    GPIO_InitStructure.GPIO_Pin = SPIRIT1_B_SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(SPIRIT1_B_SPI_CS_PORT, &GPIO_InitStructure);
    SPIRIT1_B_SPI_CS_HIGH();                                //lq Ƭѡʧ��
    
    GPIO_InitStructure.GPIO_Pin = SPIRIT1_B_SPI_SCLK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(SPIRIT1_B_SPI_SCLK_PORT, &GPIO_InitStructure);
    SPIRIT1_B_SPI_SCLK_LOW();

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_B_SPI_MOSI_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(SPIRIT1_B_SPI_MOSI_PORT, &GPIO_InitStructure);
    SPIRIT1_B_SPI_MOSI_LOW();                               

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_B_SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
    GPIO_Init(SPIRIT1_B_SPI_MISO_PORT, &GPIO_InitStructure);
#else
    GPIOSPI_InstanceInit(GPIOSPI1, GPIOSPI_CPOL_Low, GPIOSPI_CPHA_1Edge, 4, 
                         GPIOSPI_FirstBit_MSB, GPIOSPI_DataSize_8b);
#endif
    /* Configure SPIRIT1_B GPIO ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = SPIRIT1_B_GPIO0_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;           //lq Tx Clock    
    GPIO_Init(SPIRIT1_B_GPIO0_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_B_GPIO1_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;           //lq Rx Clock
    GPIO_Init(SPIRIT1_B_GPIO1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_B_GPIO2_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //lq Tx Data
    GPIO_Init(SPIRIT1_B_GPIO2_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_B_GPIO3_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //lq Rx Data 
    GPIO_Init(SPIRIT1_B_GPIO3_PORT, &GPIO_InitStructure);

	/* Connect EXTI Line to GPIO Pin */
	GPIO_EXTILineConfig(SPIRIT1_B_GPIO0_SOURCE_PORT, SPIRIT1_B_GPIO0_SOURCE_PIN);   //lq TX clock
	
	/* Configure EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = SPIRIT1_B_GPIO0_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //lq TxʱMCU��ʱ���½��ظ�������
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 

	/* Connect EXTI Line to GPIO Pin */
	GPIO_EXTILineConfig(SPIRIT1_B_GPIO1_SOURCE_PORT, SPIRIT1_B_GPIO1_SOURCE_PIN);   //lq RX clock
	
	/* Configure EXTI Line to generate an interrupt on rising edge */  
	EXTI_InitStructure.EXTI_Line = SPIRIT1_B_GPIO1_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
#if 0
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //lq RxʱMCU��ʱ�������ز�������
#else
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
#endif	
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

/*******************************************************************************
* ����  : SPIRIT1_Init
* ����  : ��ʼ��SPIRIT1
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_Init(void)
{
    //lq CMX7042_InitInstance();
    SPIRIT1_InitPins();
}

/*******************************************************************************
* ����  : SPIRIT1_A_RxClockIRQS
* ����  : SPIRIT1_A����ʱ���ж�
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_A_RxClockIRQS(void)
{
    U8  currValue;
    U8  decodeValue;

	if(EXTI_GetITStatus(SPIRIT1_A_GPIO3_LINE) != RESET)
	{ 
		/* Clear the EXTI line pending bit */  
		EXTI_ClearITPendingBit(SPIRIT1_A_GPIO3_LINE);

        /*lq ��ȡRX data GPIO���� */
        currValue = SPIRIT1_A_GPIO1_IN_READ();
#if 1
        if (PHY_InstNRZIDecode(NRZIType1, currValue, &decodeValue) == TRUE)
        {
            DLS_InstBitStreamPutBit(PacketProbeType1, decodeValue);
        }
#else
        DLS_InstBitStreamPutBit(PacketProbeType1, currValue);
#endif
#if 1
        if (currValue == 0)
        {
            TEST_OUT_LOW_1();        
        }
        else
        {
            TEST_OUT_HIGH_1();
        }
#endif
	}
}

/*******************************************************************************
* ����  : SPIRIT1_B_RxClockIRQS
* ����  : SPIRIT1_B����ʱ���ж�
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_B_RxClockIRQS(void)
{
	if(EXTI_GetITStatus(SPIRIT1_B_GPIO1_LINE) != RESET)
	{ 
		/* Clear the EXTI line pending bit */  
		EXTI_ClearITPendingBit(SPIRIT1_B_GPIO1_LINE);

        /*lq */
        //lq... ��ȡRX data GPIO����
	}
}

/*******************************************************************************
* ����  : SPIRIT1_A_TxClockIRQS
* ����  : SPIRIT1_A����ʱ���ж�
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_A_TxClockIRQS(void)
{
	if(EXTI_GetITStatus(SPIRIT1_A_GPIO2_LINE) != RESET)
	{ 
		/* Clear the EXTI line pending bit */  
		EXTI_ClearITPendingBit(SPIRIT1_A_GPIO2_LINE);
#if 1
        /*lq �ṩ�������� */
        if ((eqEquipPara.ucWorkMode == EQ_WORKMOD_TEST)
            && (eqEquipPara.TestSta == EQ_MOD_TEST_STA_MAIN))
        {
            /*lq ����̬���� */
            if (SPRTCTRL_InstGetTxTestFlg(SPIRIT1TypeA) == TRUE)
            {
                if (SPRTCTRL_InstTxTestIsTx(SPIRIT1TypeA) == TRUE)
                {
                    if (SPRTCTRL_InstTxTestGetSignalBit(SPIRIT1TypeA) == 0)
                    {
                        SPIRIT1_A_GPIO0_LOW(); 
                    }
                    else
                    {
                        SPIRIT1_A_GPIO0_HIGH();
                    }                
                }
            }        
        }
        else
        {
            /*lq ����ģʽ���� */
            if (SPRTCTRL_InstTxGetSignalBit() == 0)
            {
                SPIRIT1_A_GPIO0_LOW(); 
            }
            else
            {
                SPIRIT1_A_GPIO0_HIGH();
            }
        }
#endif
	}
}

/*******************************************************************************
* ����  : SPIRIT1_B_TxClockIRQS
* ����  : SPIRIT1_B����ʱ���ж�
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_B_TxClockIRQS(void)
{
	if(EXTI_GetITStatus(SPIRIT1_B_GPIO0_LINE) != RESET)
	{ 
		/* Clear the EXTI line pending bit */  
		EXTI_ClearITPendingBit(SPIRIT1_B_GPIO0_LINE);

        /*lq �ṩ�������� */
        if (SPRTCTRL_InstGetTxTestFlg(SPIRIT1TypeB) == TRUE)
        {
            if (SPRTCTRL_InstTxTestIsTx(SPIRIT1TypeB) == TRUE)
            {
                if (SPRTCTRL_InstTxTestGetSignalBit(SPIRIT1TypeB) == 0)
                {
                    SPIRIT1_B_GPIO2_LOW(); 
                }
                else
                {
                    SPIRIT1_B_GPIO2_HIGH();
                }                
            }
        }
	}
}

/*******************************************************************************
* ����  : SPIRIT1_WriteByte
* ����  : д�ֽ�����
* ����  : cs - SPIRIT1оƬ����
          addr - ��ַ
          data - ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_WriteByte(SPIRIT1TypeEnum cs, U8 addr, U8 data)
{
    if (cs == SPIRIT1TypeA)
    {
        /*lq ����SPI2 */
#if 0   
        SPI_Cmd(SPI2, ENABLE);
#endif    
        SPI2_NSS_LOW();             

        /*lq ����ͷ */
        SPI2_SendByte(SPIRIT1_HEADER_ADDRESS_WRITE);
            
        /*lq ���͵�ַ */
        SPI2_SendByte(addr);
    
        /*lq �������� */
        SPI2_SendByte(data); 
    
        /*lq �ر�SPI */
        SPI2_NSS_HIGH(); 
#if 0                
        SPI_Cmd(SPI2, DISABLE);
#endif 

        Delay_us(5);    //lq SPIRIT1 Ҫ������2us    
    }
    else
    {
        /*lq ����ͷ */
        GPIOSPI_InstSendData(GPIOSPI1, SPIRIT1_HEADER_ADDRESS_WRITE);

        /*lq ���͵�ַ */
        GPIOSPI_InstSendData(GPIOSPI1, addr);

        /*lq �������� */
        GPIOSPI_InstSendData(GPIOSPI1, data);        

        Delay_us(5);    //lq SPIRIT1 Ҫ������2us
    }   
}

/*******************************************************************************
* ����  : SPIRIT1_WriteWord
* ����  : д������
* ����  : cs - SPIRIT1оƬ����
          addr - ��ַ
          data - ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_WriteWord(SPIRIT1TypeEnum cs, U8 addr, U16 data)
{
    if (cs == SPIRIT1TypeA)
    {
        /*lq ����SPI2 */
#if 0   
        SPI_Cmd(SPI2, ENABLE);
#endif    
        SPI2_NSS_LOW();             

        /*lq ����ͷ */
        SPI2_SendByte(SPIRIT1_HEADER_ADDRESS_WRITE);
            
        /*lq ���͵�ַ */
        SPI2_SendByte(addr);
    
        /*lq ���͸�8λ */
        SPI2_SendByte(data >> 8); 
    
        /*lq ���͵�8λ */
        SPI2_SendByte(data & 0xFF);  
    
        /*lq �ر�SPI */
        SPI2_NSS_HIGH(); 
#if 0                
        SPI_Cmd(SPI2, DISABLE);
#endif     
        Delay_us(5);    //lq SPIRIT1 Ҫ������2us
    }
    else
    {
        /*lq ����ͷ */
        GPIOSPI_InstSendData(GPIOSPI1, SPIRIT1_HEADER_ADDRESS_WRITE);

        /*lq ���͵�ַ */
        GPIOSPI_InstSendData(GPIOSPI1, addr);

        /*lq ���͸�8λ */
        GPIOSPI_InstSendData(GPIOSPI1, data >> 8); 

        /*lq ���͵�8λ */
        GPIOSPI_InstSendData(GPIOSPI1, data & 0xFF);                

        Delay_us(5);    //lq SPIRIT1 Ҫ������2us
    }   
}

/*******************************************************************************
* ����  : SPIRIT1_ReadByte
* ����  : ���ֽ�����
* ����  : cs - SPIRIT1оƬ����
          addr - ��ַ
* ���  : ��
* ����  : RegVal - �����ֽ�
*******************************************************************************/
U8 SPIRIT1_ReadByte(SPIRIT1TypeEnum cs, U8 addr)
{
    U8 RegVal;

    if (cs == SPIRIT1TypeA)
    {
        /*lq ����SPI2 */
#if 0
        SPI_Cmd(SPI2, ENABLE);
#endif
        SPI2_NSS_LOW();             
    
        /*lq ����ͷ */
        SPI2_SendByte(SPIRIT1_HEADER_ADDRESS_READ);

        /*lq ���͵�ַ */
        SPI2_SendByte(addr);
    
        /*lq ��������*/
        RegVal = SPI2_SendByte(SPI_DUMMY_BYTE); 
    
        /*lq �ر�SPI2 */
        SPI2_NSS_HIGH();
#if 0                 
        SPI_Cmd(SPI2, DISABLE);
#endif            
        Delay_us(5);    //lq SPIRIT1 Ҫ������2us
    }   
    else
    {
        /*lq ����ͷ */
        GPIOSPI_InstSendData(GPIOSPI1, SPIRIT1_HEADER_ADDRESS_READ);

        /*lq ���͵�ַ */
        GPIOSPI_InstSendData(GPIOSPI1, addr); 
        
        /*lq �������� */
        RegVal = GPIOSPI_InstSendData(GPIOSPI1, SPI_DUMMY_BYTE);   

        Delay_us(5);    //lq SPIRIT1 Ҫ������2us
    } 
   
    return RegVal;        
}

/*******************************************************************************
* ����  : SPIRIT1_ReadWord
* ����  : ��������
* ����  : cs - SPIRIT1оƬ����
          addr - ��ַ
* ���  : ��
* ����  : RegVal - �����ֽ�
*******************************************************************************/
U16 SPIRIT1_ReadWord(SPIRIT1TypeEnum cs, U8 addr)
{
    U16 RegVal;

    if (cs == SPIRIT1TypeA)
    {
        /*lq ����SPI2 */
#if 0
        SPI_Cmd(SPI2, ENABLE);
#endif
        SPI2_NSS_LOW();             
    
        /*lq ����ͷ */
        SPI2_SendByte(SPIRIT1_HEADER_ADDRESS_READ);

        /*lq ���͵�ַ */
        SPI2_SendByte(addr);
    
        /*lq ��������*/
        RegVal = SPI2_SendByte(SPI_DUMMY_BYTE); 
        RegVal <<= 8;
        RegVal |= SPI2_SendByte(SPI_DUMMY_BYTE);
            
        /*lq �ر�SPI2 */
        SPI2_NSS_HIGH();
#if 0                 
        SPI_Cmd(SPI2, DISABLE);
#endif

        Delay_us(5);    //lq SPIRIT1 Ҫ������2us            
    }   
    else
    {
        /*lq ����ͷ */
        GPIOSPI_InstSendData(GPIOSPI1, SPIRIT1_HEADER_ADDRESS_READ);

        /*lq ���͵�ַ */
        GPIOSPI_InstSendData(GPIOSPI1, addr); 
        
        /*lq �������� */
        RegVal = GPIOSPI_InstSendData(GPIOSPI1, SPI_DUMMY_BYTE);
        RegVal <<= 8;
        RegVal |= GPIOSPI_InstSendData(GPIOSPI1, SPI_DUMMY_BYTE);           

        Delay_us(5);    //lq SPIRIT1 Ҫ������2us
    } 
   
    return RegVal;        
}

/*******************************************************************************
* ����  : SPIRIT1_WriteCommand
* ����  : ��SPIRIT1дһ�������ֽ�
* ����  : command - �����ֽ�		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_WriteCommand(SPIRIT1TypeEnum cs, U8 command)
{
    if (cs == SPIRIT1TypeA)
    {
        /*lq ����SPI2 */
#if 0
        SPI_Cmd(SPI2, ENABLE);
#endif    
        SPI2_NSS_LOW();             

        /*lq ����ͷ */
        SPI2_SendByte(SPIRIT1_HEADER_COMMAND_WRITE);
    
        /*lq �������� */
        SPI2_SendByte(command);
    
        /*lq �ر�SPI */
        SPI2_NSS_HIGH();
#if 0                     
        SPI_Cmd(SPI2, DISABLE);
#endif    
        Delay_us(5);    //lq SPIRIT1 Ҫ������2us
    }
    else
    {
        /*lq ����ͷ */
        GPIOSPI_InstSendData(GPIOSPI1, SPIRIT1_HEADER_COMMAND_WRITE);

        /*lq �������� */
        GPIOSPI_InstSendData(GPIOSPI1, command);    

        Delay_us(5);    //lq SPIRIT1 Ҫ������2us
    }
}

/*******************************************************************************
* ����  : SPIRIT1_Configuration
* ����  : ����SPIRIT1
* ����  : cs - SPIRIT1Ƭѡ		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SPIRIT1_Configuration(SPIRIT1TypeEnum cs)
{
    U8  regValue;

    Feed_Dog();
    
    Delay_ms(50);   //lq �ϵ縴λ��ʱ

    /*lq ������λ*/
    SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_SRES);                 //lq ��λ
    Delay_ms(50);   //lq ��λ��ʱ

    SPIRIT1_StateToReady(cs);

    /*lq �������״̬ */        
    SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_STANDBY);              //lq ��������ʱ�Ӷ���Ƶʱ�����standby״̬

    /*lq -----------------General configuration registers----------------*/
    /*lq ANA_FUNC_CONF[1] */
    //lq Ĭ��ֵ

    /*lq ANA_FUNC_CONF[0] */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_ANA_FUNC_CONF0, 0xD0); //lq reference signal from XIN pin

    if (cs == SPIRIT1TypeA)
    {
        /*lq GPIO3_CONF */
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_GPIO3_CONF, 
                          (SPIRIT1_GPIO_FUC_OUT_RX_CLOCK | SPIRIT1_GPIO_MODE_DOUT_LOW));    //lq GPIO3: RX clock
    
        /*lq GPIO2_CONF */
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_GPIO2_CONF, 
                          (SPIRIT1_GPIO_FUC_OUT_TX_CLOCK | SPIRIT1_GPIO_MODE_DOUT_HIGH));   //lq GPIO2: TX clock
    
        /*lq GPIO1_CONF */
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_GPIO1_CONF, 
                          (SPIRIT1_GPIO_FUC_OUT_RX_DATA | SPIRIT1_GPIO_MODE_DOUT_LOW));     //lq GPIO1: RX data
    
        /*lq GPIO0_CONF */
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_GPIO0_CONF, 
                          (SPIRIT1_GPIO_FUC_IN_TX_DATA | SPIRIT1_GPIO_MODE_DIN));           //lq GPIO0: TX data
    }
    else if (cs == SPIRIT1TypeB)
    {
        /*lq GPIO3_CONF */
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_GPIO3_CONF, 
                          (SPIRIT1_GPIO_FUC_OUT_RX_DATA | SPIRIT1_GPIO_MODE_DOUT_LOW));     //lq GPIO3: RX data
    
        /*lq GPIO2_CONF */
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_GPIO2_CONF, 
                          (SPIRIT1_GPIO_FUC_IN_TX_DATA | SPIRIT1_GPIO_MODE_DIN));           //lq GPIO2: TX data
    
        /*lq GPIO1_CONF */
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_GPIO1_CONF, 
                          (SPIRIT1_GPIO_FUC_OUT_RX_CLOCK | SPIRIT1_GPIO_MODE_DOUT_LOW));    //lq GPIO1: RX clock
    
        /*lq GPIO0_CONF */
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_GPIO0_CONF, 
                          (SPIRIT1_GPIO_FUC_OUT_TX_CLOCK | SPIRIT1_GPIO_MODE_DOUT_HIGH));   //lq GPIO0: TX clock

    }

    /*lq MCU_CK_CONF */
    //lq Ĭ��ֵ

    /*lq XO_RCO_TEST */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_XO_RCO_TEST, 0x29);    //lq... PD_CLKDIV = 1(��ȫ�������Ƚ���standby״̬�����ֲ�8.5)
    
    /*lq SYNTH_CONFIG[0] */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_SYNTH_CONFIG0, 0xA0);  //lq split time: 3.47 ns

    /*lq SYNTH_CONFIG[1] */
    //lq Ĭ��ֵ

    /*lq IF_OFFSET_ANA */
    //lq Ĭ��ֵ

    /*lq -----------------Radio configuration registers (analog blocks)----------------*/
    /*lq SYNT3 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_SYNT3, 0x0C);     //lq SYNT = 26017792

    /*lq SYNT2 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_SYNT2, 0x68);     //lq SYNT = 26017792

    /*lq SYNT1 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_SYNT1, 0x00);     //lq SYNT = 26017792

    /*lq SYNT0 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_SYNT0, 0x05);     //lq SYNT = 26017792��BS = 5(B = 32)

    /*lq CHSPACE */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_CHSPACE, 0x3F);   //lq CHSPACE = 63

    /*lq IF_OFFSET_DIG */
    //lq Ĭ��ֵ

    /*lq FC_OFFSET1 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_FC_OFFSET1, 0x0F); //lq FC_OFFSET = -57

    /*lq FC_OFFSET0 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_FC_OFFSET0, 0xC7); //lq FC_OFFSET = -57

    /*lq PA_POWER8 */
    //lq Ĭ��ֵ    
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_PA_POWER(8), 0x03);//lq PA_LEVEL_7 = 

    /*lq PA_POWER7 */
    //lq Ĭ��ֵ    
    //lq SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_PA_POWER(7), 0x);//lq PA_LEVEL_6 = 

    /*lq PA_POWER6 */
    //lq Ĭ��ֵ    
    //lq SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_PA_POWER(6), 0x);//lq PA_LEVEL_5 = 

    /*lq PA_POWER5 */
    //lq Ĭ��ֵ    
    //lq SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_PA_POWER(5), 0x);//lq PA_LEVEL_4 = 

    /*lq PA_POWER4 */
    //lq Ĭ��ֵ    
    //lq SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_PA_POWER(4), 0x);//lq PA_LEVEL_3 = 

    /*lq PA_POWER3 */
    //lq Ĭ��ֵ    
    //lq SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_PA_POWER(3), 0x);//lq PA_LEVEL_2 = 

    /*lq PA_POWER2 */
    //lq Ĭ��ֵ    
    //lq SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_PA_POWER(2), 0x);//lq PA_LEVEL_1 = 

    /*lq PA_POWER1 */
    //lq Ĭ��ֵ    
    //lq SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_PA_POWER(1), 0x);//lq PA_LEVEL_0 = 

    /*lq PA_POWER0 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_PA_POWER(0), 0x3F);//lq enable PA power ramping, step width = 4 * (1/8 bit period), final level = 7

    /*lq -----------------Radio configuration registers (digital blocks)----------------*/
    /*lq MOD1 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_MOD1, 0x83);      //lq DATARATE_M = 131��datarate=9600��

    /*lq MOD0 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_MOD0, 0x58);      //lq BT = 0.5, MOD_TYPE = GFSK, DATARATE_E = 8��datarate=9600��

    /*lq FDEV0 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_FDEV0, 0x24);     //lq FDEV_M = 4, CLOCK_REC_ALGO_SEL = DLL, FDEV_E = 2

    /*lq CHFLT */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_CHFLT, 0x25);     //lq CHFLT_M = 2, CHFLT_E = 5

    /*lq AFC2 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_AFC2, 0x08);      //lq... AFC_ENABLE = 0

    /*lq AFC1 */
    //lq Ĭ��ֵ

    /*lq AFC0 */
    //lq Ĭ��ֵ

    /*lq RSSI_FLT */
    //lq Ĭ��ֵ

    /*lq RSSI_TH */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_RSSI_TH, 0x0E);     //lq RSSI_TH = -123

    /*lq CLOCKREC */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_CLOCKREC, 0xC8);    //lq... 

    /*lq AGCCTRL2 */
    //lq Ĭ��ֵ             

    /*lq AGCCTRL1 */
    //lq Ĭ��ֵ             

    /*lq AGCCTRL0 */
    //lq Ĭ��ֵ             

    /*lq ANT_SELECT_CONF */
    //lq Ĭ��ֵ 

    /*lq -----------------Packet/protocol configuration registers----------------*/
    /*lq PCKTCTRL4 */
    //lq Ĭ��ֵ

    /*lq PCKTCTRL3 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(3), 0x27);      //lq RX mode: direct through GPIO,

    /*lq PCKTCTRL2 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(2), 0x1A);      //lq SYNC_LENGTH = 1��

    /*lq PCKTCTRL1 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(1), 0x08);      //lq CRC: No CRC, TX source data: direct through GPIO,

    /*lq PCKTLEN1 */
    //lq Ĭ��ֵ

    /*lq PCKTLEN0 */
    //lq Ĭ��ֵ

    /*lq SYNC4 */
    //lq Ĭ��ֵ

    /*lq SYNC3 */
    //lq Ĭ��ֵ

    /*lq SYNC2 */
    //lq Ĭ��ֵ

    /*lq SYNC1 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_SYNC(1), 0x01);           //lq SYNC1: 0x01(0x7E��NRZI���룬Ҳ�п�����0xFE)

    /*lq QI */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_QI, 0x00);                //lq... SQI_EN: disable

    /*lq MBUS_PRMBL */
    //lq Ĭ��ֵ

    /*lq MBUS_PSTMBL */
    //lq Ĭ��ֵ

    /*lq MBUS_CTRL */
    //lq Ĭ��ֵ

    /*lq FIFO_CONFIG3 */
    //lq Ĭ��ֵ

    /*lq FIFO_CONFIG2 */
    //lq Ĭ��ֵ

    /*lq FIFO_CONFIG1 */
    //lq Ĭ��ֵ

    /*lq FIFO_CONFIG0 */
    //lq Ĭ��ֵ

    /*lq PCKT_FLT_GOALS[12-0] */
    //lq Ĭ��ֵ

    /*lq PCKT_FLT_OPTIONS */
    //lq Ĭ��ֵ

    /*lq PROTOCOL2 */
    //lq Ĭ��ֵ
#if 0
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PROTOCOL(2), 0x00);      //lq disable the automatic VCO calibration
#endif
    /*lq PROTOCOL1 */
    //lq Ĭ��ֵ

    /*lq PROTOCOL0 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PROTOCOL(0), 0x0A);      //lq PERS_RX: persistent reception enabled

    /*lq TIMERS[5-0] */
    //lq Ĭ��ֵ

    /*lq CSMA_CONFIG[3-0] */
    //lq Ĭ��ֵ

    /*lq TX_CTRL_FIELD[3-0] */
    //lq Ĭ��ֵ

    /*lq PM_CONFIG2 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PM_CONFIG(2), 0x00);      //lq enable internal SMPS, SET_SMPS_VTUNE = 0, SET_SMPS_PLLBW = 0      

    /*lq PM_CONFIG1 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PM_CONFIG(1), 0x93);      //lq EN_RM = 1, KRM = 5041(Fsw = 4MHz)

    /*lq PM_CONFIG0 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PM_CONFIG(0), 0xB1);      //lq KRM = 5041(Fsw = 4MHz)

    /*lq XO_RCO_CONFIG */
    //lq Ĭ��ֵ

    /*lq TEST_SELECT */
    //lq Ĭ��ֵ

    /*lq PM_TEST */
    //lq Ĭ��ֵ

    /*lq -----------------Frequently used registers----------------*/
    /*lq CHNUM */
    if (cs == SPIRIT1TypeA)
    {
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_FU_CHNUM, SPIRIT1_PARA_CH_NUM_A); //lq CHNUM ��Ӧ161.975MHz    
    }
    else
    {
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_FU_CHNUM, SPIRIT1_PARA_CH_NUM_B); //lq CHNUM ��Ӧ162.025MHz        
    }

    /*lq VCO_CONFIG */
    //lq Ĭ��ֵ

    /*lq RCO_VCO_CALIBR_IN[2] */
    //lq Ĭ��ֵ

    /*lq RCO_VCO_CALIBR_IN[1] */
    //lq Ĭ��ֵ
#if 0
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_FU_RCO_VCO_CALIBR_IN(1), 0x46);      //lq Word value for the VCO to be used in TX mode
#endif
    /*lq RCO_VCO_CALIBR_IN[0] */
    //lq Ĭ��ֵ
#if 0
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_FU_RCO_VCO_CALIBR_IN(0), 0x46);      //lq Word value for the VCO to be used in RX mode
#endif
    /*lq AES_KEY_IN[15-0] */
    //lq Ĭ��ֵ

    /*lq AES_DATA_IN[15-0] */
    //lq Ĭ��ֵ

    /*lq IRQ_MASK[3-0] */
    //lq Ĭ��ֵ

    /*lq IRQ_MASK[3-0] */
    //lq Ĭ��ֵ

    /*lq DEM_CONFIG */
    //lq... Ĭ��ֵ

    /*lq PM_CONFIG */
    //lq ���ü�"Packet/protocol configuration registers"����

    /*lq -----------------General information----------------*/
#if 0
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GI_PARTNUM);
    if (cs == SPIRIT1TypeA)
    {
        usart_printf("$SPIRIT1_A: Device part number %02X!\r\n", regValue);        
    }
    else
    {
        usart_printf("$SPIRIT1_B: Device part number %02X!\r\n", regValue);            
    } 

    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GI_VERSION);
    if (cs == SPIRIT1TypeA)
    {
        usart_printf("$SPIRIT1_A: Device version number %02X!\r\n", regValue);        
    }
    else
    {
        usart_printf("$SPIRIT1_B: Device version number %02X!\r\n", regValue);            
    }
#endif
#if 0    
    /*lq ��ȡ���õļĴ��� */
    SPIRIT1_ReadRegisters(cs); 
#endif
    /*lq �������״̬ */        
    SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_READY);
}

/*******************************************************************************
* ����  : SPIRIT1_InstConfiguration
* ����  : ����SPIRIT1
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_InstConfiguration(void)
{
    SPIRIT1_Configuration(SPIRIT1TypeA);
    SPIRIT1_Configuration(SPIRIT1TypeB);
#if 1
    /*lq �������״̬ */
    SPIRIT1_WriteCommand(SPIRIT1TypeA, SPIRIT1_COMMAND_RX); //lq SPIRIT1TypeB������ɺ��ٽ���RX̬����Ϊ��GPIO_RX_CLOCKʱ���жϵ����ȼ�����ģ��SPIʱ�Ӷ�ʱ��Tim���жϵ����ȼ����ᵼ��ģ��SPIʱ��Ƶ�ʱ����������Ź���λ��
    SPIRIT1_WriteCommand(SPIRIT1TypeB, SPIRIT1_COMMAND_RX); 
#endif
}

/*******************************************************************************
* ����  : SPIRIT1_InstConfigure
* ����  : ����SPIRIT1
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_ReadRegisters(SPIRIT1TypeEnum cs)
{
    U8 regValue;
    U8 temp[2];

    if (cs == SPIRIT1TypeA)
    {
        temp[0] = 'A';
        temp[1] = '\0';
    }
    else
    {
        temp[0] = 'B';
        temp[1] = '\0';    
    }

    /*lq -----------------General configuration registers----------------*/
    /*lq ANA_FUNC_CONF[1] */
    //lq Ĭ��ֵ

    /*lq ANA_FUNC_CONF[0] */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_ANA_FUNC_CONF0); //lq reference signal from XIN pin
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_ANA_FUNC_CONF0,
                regValue, 0xD0);        

    /*lq GPIO3_CONF */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO3_CONF); //lq GPIO3: RX clock
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO3_CONF,
                regValue, (SPIRIT1_GPIO_FUC_OUT_RX_CLOCK | SPIRIT1_GPIO_MODE_DOUT_HIGH)); 

    /*lq GPIO2_CONF */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO2_CONF); //lq GPIO2: RX data
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO2_CONF,
                regValue, (SPIRIT1_GPIO_FUC_OUT_RX_DATA | SPIRIT1_GPIO_MODE_DOUT_LOW)); 

    /*lq GPIO1_CONF */
    //lq Ĭ��ֵ

    /*lq GPIO0_CONF */
    //lq Ĭ��ֵ

    if (cs == SPIRIT1TypeA)
    {
        /*lq GPIO3_CONF */
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO3_CONF);    //lq GPIO3: RX clock
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO3_CONF,
                    regValue, (SPIRIT1_GPIO_FUC_OUT_RX_CLOCK | SPIRIT1_GPIO_MODE_DOUT_LOW)); 
    
        /*lq GPIO2_CONF */
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO2_CONF);   //lq GPIO2: TX clock
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO2_CONF,
                    regValue, (SPIRIT1_GPIO_FUC_OUT_TX_CLOCK | SPIRIT1_GPIO_MODE_DOUT_HIGH));
                        
        /*lq GPIO1_CONF */
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO1_CONF);   //lq GPIO1: RX data
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO1_CONF,
                    regValue, (SPIRIT1_GPIO_FUC_OUT_RX_DATA | SPIRIT1_GPIO_MODE_DOUT_LOW));
    
        /*lq GPIO0_CONF */
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO0_CONF);   //lq GPIO0: TX data
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO0_CONF,
                    regValue, (SPIRIT1_GPIO_FUC_IN_TX_DATA | SPIRIT1_GPIO_MODE_DOUT_LOW));
    }
    else if (cs == SPIRIT1TypeB)
    {
        /*lq GPIO3_CONF */
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO3_CONF);     //lq GPIO2: RX data
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO3_CONF,
                    regValue, (SPIRIT1_GPIO_FUC_OUT_RX_DATA | SPIRIT1_GPIO_MODE_DOUT_LOW)); 
    
        /*lq GPIO2_CONF */
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO2_CONF);     //lq GPIO2: TX data
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO2_CONF,
                    regValue, (SPIRIT1_GPIO_FUC_IN_TX_DATA | SPIRIT1_GPIO_MODE_DOUT_LOW)); 
    
        /*lq GPIO1_CONF */
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO1_CONF);     //lq GPIO1: RX clock
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO1_CONF,
                    regValue, (SPIRIT1_GPIO_FUC_OUT_RX_CLOCK | SPIRIT1_GPIO_MODE_DOUT_LOW)); 
    
        /*lq GPIO0_CONF */
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO0_CONF);     //lq GPIO0: TX clock
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO0_CONF,
                    regValue, (SPIRIT1_GPIO_FUC_OUT_TX_CLOCK | SPIRIT1_GPIO_MODE_DOUT_HIGH)); 

    }

    /*lq MCU_CK_CONF */
    //lq Ĭ��ֵ

    /*lq XO_RCO_TEST */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_XO_RCO_TEST);    //lq... PD_CLKDIV = 1(��ȫ�������Ƚ���standby״̬�����ֲ�8.5)
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_XO_RCO_TEST,
                regValue, 0x29);        
    
    /*lq SYNTH_CONFIG[0] */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_SYNTH_CONFIG0);  //lq split time: 3.47 ns
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_SYNTH_CONFIG0,
                regValue, 0xA0);        

    /*lq SYNTH_CONFIG[1] */
    //lq Ĭ��ֵ 

    /*lq IF_OFFSET_ANA */
    //lq Ĭ��ֵ

    /*lq -----------------Radio configuration registers (analog blocks)----------------*/
    /*lq SYNT3 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCA_SYNT3);     //lq SYNT = 26017792
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCA_SYNT3,
                regValue, 0x0C);        

    /*lq SYNT2 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCA_SYNT2);     //lq SYNT = 26017792
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCA_SYNT2,
                regValue, 0x68);        

    /*lq SYNT1 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCA_SYNT1);     //lq SYNT = 26017792
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCA_SYNT1,
                regValue, 0x00);        

    /*lq SYNT0 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCA_SYNT0);     //lq SYNT = 26017792��BS = 5(B = 32)
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCA_SYNT0,
                regValue, 0x05);        

    /*lq CHSPACE */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCA_CHSPACE);   //lq CHSPACE = 63
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCA_CHSPACE,
                regValue, 0x3F);        

    /*lq IF_OFFSET_DIG */
    //lq Ĭ��ֵ

    /*lq FC_OFFSET1 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCA_FC_OFFSET1); //lq FC_OFFSET = -57
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCA_FC_OFFSET1,
                regValue, 0x0F);        

    /*lq FC_OFFSET0 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCA_FC_OFFSET0); //lq FC_OFFSET = -57
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCA_FC_OFFSET0,
                regValue, 0xC7);        

    /*lq PA_POWER0-8 */
    //lq Ĭ��ֵ

    /*lq -----------------Radio configuration registers (digital blocks)----------------*/
    /*lq MOD1 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCD_MOD1);      //lq DATARATE_M = 131��datarate=9600��
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCD_MOD1,
                regValue, 0x83);        

    /*lq MOD0 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCD_MOD0);      //lq BT = 0.5, MOD_TYPE = GFSK, DATARATE_E = 8��datarate=9600��
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCD_MOD0,
                regValue, 0x58);        

    /*lq FDEV0 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCD_FDEV0);     //lq FDEV_M = 4, FDEV_E = 2
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCD_FDEV0,
                regValue, 0x24);        

    /*lq CHFLT */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCD_CHFLT);     //lq CHFLT_M = 2, CHFLT_E = 5
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCD_CHFLT,
                regValue, 0x25);        

    /*lq AFC2 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCD_AFC2);      //lq... AFC_ENABLE = 0
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCD_AFC2,
                regValue, 0x08);        

    /*lq AFC1 */
    //lq Ĭ��ֵ

    /*lq AFC0 */
    //lq Ĭ��ֵ

    /*lq RSSI_FLT */
    //lq Ĭ��ֵ

    /*lq RSSI_TH */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCD_RSSI_TH);     //lq RSSI_TH = -123
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCD_RSSI_TH,
                regValue, 0x0E);

    /*lq CLOCKREC */
    //lq Ĭ��ֵ

    /*lq AGCCTRL2 */
    //lq Ĭ��ֵ             

    /*lq AGCCTRL1 */
    //lq Ĭ��ֵ             

    /*lq AGCCTRL0 */
    //lq Ĭ��ֵ                   

    /*lq ANT_SELECT_CONF */
    //lq Ĭ��ֵ 

    /*lq -----------------Packet/protocol configuration registers----------------*/
    /*lq PCKTCTRL4 */
    //lq Ĭ��ֵ

    /*lq PCKTCTRL3 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(3));      //lq RX mode: direct through GPIO,
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_PPC_PCKTCTRL(3),
                regValue, 0x27);        

    /*lq PCKTCTRL2 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(2));      //lq SYNC_LENGTH = 1��
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_PPC_PCKTCTRL(2),
                regValue, 0x1A);        

    /*lq PCKTCTRL1 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(1));      //lq CRC: No CRC, TX source data: direct through GPIO,
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_PPC_PCKTCTRL(1),
                regValue, 0x08);        

    /*lq PCKTLEN1 */
    //lq Ĭ��ֵ

    /*lq PCKTLEN0 */
    //lq Ĭ��ֵ

    /*lq SYNC4 */
    //lq Ĭ��ֵ

    /*lq SYNC3 */
    //lq Ĭ��ֵ

    /*lq SYNC2 */
    //lq Ĭ��ֵ

    /*lq SYNC1 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_SYNC(1));           //lq SYNC1: 0x33(ʹ����NRZI���룬��Ӧδʹ��NRZI��01010101)
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_PPC_SYNC(1),
                regValue, 0x01);        

    /*lq QI */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_QI);                //lq... SQI_EN: disable SQI
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_PPC_QI,
                regValue, 0x00);        


    /*lq MBUS_PRMBL */
    //lq Ĭ��ֵ

    /*lq MBUS_PSTMBL */
    //lq Ĭ��ֵ

    /*lq MBUS_CTRL */
    //lq Ĭ��ֵ

    /*lq FIFO_CONFIG3 */
    //lq Ĭ��ֵ

    /*lq FIFO_CONFIG2 */
    //lq Ĭ��ֵ

    /*lq FIFO_CONFIG1 */
    //lq Ĭ��ֵ

    /*lq FIFO_CONFIG0 */
    //lq Ĭ��ֵ

    /*lq PCKT_FLT_GOALS[12-0] */
    //lq Ĭ��ֵ

    /*lq PCKT_FLT_OPTIONS */
    //lq Ĭ��ֵ

    /*lq PROTOCOL2 */
    //lq Ĭ��ֵ

    /*lq PROTOCOL1 */
    //lq Ĭ��ֵ

    /*lq PROTOCOL0 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_PROTOCOL(0));      //lq PERS_RX: persistent reception enabled
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_PPC_PROTOCOL(0),
                regValue, 0x0A);        

    /*lq TIMERS[5-0] */
    //lq Ĭ��ֵ

    /*lq CSMA_CONFIG[3-0] */
    //lq Ĭ��ֵ

    /*lq TX_CTRL_FIELD[3-0] */
    //lq Ĭ��ֵ

    /*lq PM_CONFIG2 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_PM_CONFIG(2));      //lq disable internal SMPS      
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_PPC_PM_CONFIG(2),
                regValue, 0x2C);        

    /*lq PM_CONFIG1 */
    //lq Ĭ��ֵ

    /*lq PM_CONFIG0 */
    //lq Ĭ��ֵ

    /*lq XO_RCO_CONFIG */
    //lq Ĭ��ֵ

    /*lq TEST_SELECT */
    //lq Ĭ��ֵ

    /*lq PM_TEST */
    //lq Ĭ��ֵ

    /*lq -----------------Frequently used registers----------------*/
    /*lq CHNUM */
    if (cs == SPIRIT1TypeA)
    {
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_FU_CHNUM); //lq CHNUM ��Ӧ161.975MHz    
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_FU_CHNUM,
                     regValue, SPIRIT1_PARA_CH_NUM_A);        

    }
    else
    {
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_FU_CHNUM); //lq CHNUM ��Ӧ162.025MHz        
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_FU_CHNUM,
                     regValue, SPIRIT1_PARA_CH_NUM_B);  
    }

    /*lq VCO_CONFIG */
    //lq Ĭ��ֵ

    /*lq RCO_VCO_CALIBR_IN[2-0] */
    //lq Ĭ��ֵ

    /*lq AES_KEY_IN[15-0] */
    //lq Ĭ��ֵ

    /*lq AES_DATA_IN[15-0] */
    //lq Ĭ��ֵ

    /*lq IRQ_MASK[3-0] */
    //lq Ĭ��ֵ

    /*lq IRQ_MASK[3-0] */
    //lq Ĭ��ֵ

    /*lq DEM_CONFIG */
    //lq... Ĭ��ֵ

    /*lq PM_CONFIG */
    //lq ���ü�"Packet/protocol configuration registers"����                
}

/*******************************************************************************
* ����  : SPIRIT1_GetBitNum
* ����  : ��������ֵ����λ��ţ���0��ʼ��
* ����  : bitMask - λ����(λ�����е�1��������)	  
* ���  : ��
* ����  : ��������Ӧ��λ��ţ���λ���������λ��1��λ��ţ�
*******************************************************************************/
U8 SPIRIT1_GetBitNum(const U8 bitMask)
{
    U8 i;
       
    for (i = 0; i < 8; i++)
    {
        if (((bitMask >> i) & 0x01) != 0)
        {
            break;
        }
    } 
    
    return i;   
}

/*******************************************************************************
* ����  : SPIRIT1_GetRegFieldValue
* ����  : ��ȡSPIRIT1�ĵ�ǰ״̬
* ����  : cs - SPIRIT1оƬ����
          regAddr - �Ĵ�����ַ
          bitMask - �ֶζ�Ӧ��λ����	  
* ���  : ��
* ����  : regValue - λ�����Ӧ��ֵ
*******************************************************************************/
U8 SPIRIT1_GetRegFieldValue(SPIRIT1TypeEnum cs, U8 regAddr, U8 bitMask)
{
    U8 regValue;

    regValue = SPIRIT1_ReadByte(cs, regAddr);
    regValue &= bitMask;
    regValue >>= SPIRIT1_GetBitNum(bitMask);
    
    return regValue;  
}

/*******************************************************************************
* ����  : SPIRIT1_GetMcState
* ����  : ��ȡSPIRIT1�ĵ�ǰ״̬
* ����  : cs - SPIRIT1оƬ����	  
* ���  : ��
* ����  : regValue - SPIRIT1�ĵ�ǰ״̬
*******************************************************************************/
U8 SPIRIT1_GetMcState(SPIRIT1TypeEnum cs)
{
    U8 value;

    value = SPIRIT1_GetRegFieldValue(cs, SPIRIT1_REGADDR_FU_MC_STATE(0),
                                     SPIRIT1_FU_MC_STATE0_BITMASK_STATE);
    
    return value;   
}

/*******************************************************************************
* ����  : SPIRIT1_IsXoOn
* ����  : ��ȡSPIRIT1�ĵ�ǰXO_ON״̬
* ����  : cs - SPIRIT1оƬ����	  
* ���  : ��
* ����  : value - SPIRIT1�ĵ�ǰ״̬
*******************************************************************************/
U8 SPIRIT1_IsXoOn(SPIRIT1TypeEnum cs)
{
    U8 value;
    
    value = SPIRIT1_GetRegFieldValue(cs, SPIRIT1_REGADDR_FU_MC_STATE(0),
                                     SPIRIT1_FU_MC_STATE0_BITMASK_XO_ON);

    return value;                                             
}

/*******************************************************************************
* ����  : SPIRIT1_GetAntSelect
* ����  : ��ȡSPIRIT1�ĵ�ǰ����ѡ��״̬
* ����  : cs - SPIRIT1оƬ����	  
* ���  : ��
* ����  : value - SPIRIT1�ĵ�ǰ״̬
*******************************************************************************/
U8 SPIRIT1_GetAntSelect(SPIRIT1TypeEnum cs)
{
    U8 value;
    
    value = SPIRIT1_GetRegFieldValue(cs, SPIRIT1_REGADDR_FU_MC_STATE(1),
                                     SPIRIT1_FU_MC_STATE1_BITMASK_ANT_SELECT);

    return value;                                             
}

/*******************************************************************************
* ����  : SPIRIT1_IsTxFifoFull
* ����  : ��ȡSPIRIT1�ĵ�ǰTX_FIFO״̬
* ����  : cs - SPIRIT1оƬ����	  
* ���  : ��
* ����  : value - SPIRIT1�ĵ�ǰ״̬
*******************************************************************************/
U8 SPIRIT1_IsTxFifoFull(SPIRIT1TypeEnum cs)
{
    U8 value;
    
    value = SPIRIT1_GetRegFieldValue(cs, SPIRIT1_REGADDR_FU_MC_STATE(1),
                                     SPIRIT1_FU_MC_STATE1_BITMASK_TX_FIFO_FULL);

    return value;                                             
}

/*******************************************************************************
* ����  : SPIRIT1_IsTxFifoFull
* ����  : ��ȡSPIRIT1�ĵ�ǰRX_FIFO״̬
* ����  : cs - SPIRIT1оƬ����	  
* ���  : ��
* ����  : value - SPIRIT1�ĵ�ǰ״̬
*******************************************************************************/
U8 SPIRIT1_IsRxFifoEmpty(SPIRIT1TypeEnum cs)
{
    U8 value;
    
    value = SPIRIT1_GetRegFieldValue(cs, SPIRIT1_REGADDR_FU_MC_STATE(1),
                                     SPIRIT1_FU_MC_STATE1_BITMASK_RX_FIFO_EMPTY);

    return value;                                             
}

/*******************************************************************************
* ����  : SPIRIT1_IsTxFifoFull
* ����  : ��ȡSPIRIT1�ĵ�ǰRco calibrator״̬
* ����  : cs - SPIRIT1оƬ����	  
* ���  : ��
* ����  : value - SPIRIT1�ĵ�ǰ״̬
*******************************************************************************/
U8 SPIRIT1_IsRcoCalibError(SPIRIT1TypeEnum cs)
{
    U8 value;
    
    value = SPIRIT1_GetRegFieldValue(cs, SPIRIT1_REGADDR_FU_MC_STATE(1),
                                     SPIRIT1_FU_MC_STATE1_BITMASK_ERROR_LOCK);

    return value;                                             
}

/*******************************************************************************
* ����  : SPIRIT1_GetVcoCalibrData
* ����  : ��ȡSPIRIT1�ĵ�ǰVCO calibratorֵ
* ����  : cs - SPIRIT1оƬ����	  
* ���  : ��
* ����  : value - SPIRIT1�ĵ�ǰVCO calibratorֵ
*******************************************************************************/
U8 SPIRIT1_GetVcoCalibrData(SPIRIT1TypeEnum cs)
{
    U8 value;
    
    value = SPIRIT1_GetRegFieldValue(cs, SPIRIT1_REGADDR_FU_RCO_VCO_CALIBR_OUT(0),
                                     SPIRIT1_FU_RCO_VCO_CALIBR_OUT0_BITMASK_VCO_CALIBR_DATA);

    return value; 
}

/*******************************************************************************
* ����  : SPIRIT1_GetVcoCalibrData
* ����  : ��ȡSPIRIT1�ĵ�ǰVCO calibrator TXֵ
* ����  : cs - SPIRIT1оƬ����	  
* ���  : ��
* ����  : value - SPIRIT1�ĵ�ǰVCO calibrator TXֵ
*******************************************************************************/
U8 SPIRIT1_GetVcoCalibrDataTx(SPIRIT1TypeEnum cs)
{
    U8 value;
    
    value = SPIRIT1_GetRegFieldValue(cs, SPIRIT1_REGADDR_FU_RCO_VCO_CALIBR_IN(1),
                                     SPIRIT1_FU_RCO_VCO_CALIBR_IN1_BITMASK_VCO_CALIBR_TX);

    return value; 
}

/*******************************************************************************
* ����  : SPIRIT1_GetVcoCalibrDataRx
* ����  : ��ȡSPIRIT1�ĵ�ǰVCO calibrator RXֵ
* ����  : cs - SPIRIT1оƬ����	  
* ���  : ��
* ����  : value - SPIRIT1�ĵ�ǰVCO calibrator RXֵ
*******************************************************************************/
U8 SPIRIT1_GetVcoCalibrDataRx(SPIRIT1TypeEnum cs)
{
    U8 value;
    
    value = SPIRIT1_GetRegFieldValue(cs, SPIRIT1_REGADDR_FU_RCO_VCO_CALIBR_IN(0),
                                     SPIRIT1_FU_RCO_VCO_CALIBR_IN0_BITMASK_VCO_CALIBR_RX);

    return value; 
}

/*******************************************************************************
* ����  : SPIRIT1_CfgCwTx
* ����  : �����ز���������
* ����  : cs - SPIRIT1оƬ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_CfgCwTx(SPIRIT1TypeEnum cs)
{
    U8 regValue;

    /*lq ����Ready̬ */
    if (SPIRIT1_GetMcState(cs) == SPIRIT1_MC_STATE_RX
        || SPIRIT1_GetMcState(cs) == SPIRIT1_MC_STATE_TX)
    {
        /*lq �˳��շ�״̬ */
        SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_SABORT);
    }
    else
    {
        SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_READY);
    }

    /*lq MOD0 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCD_MOD0);
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_MOD0,
                      regValue | SPIRIT1_RCD_MOD0_BITMASK_CW);              //lq CW: enable��BT = 0.5, MOD_TYPE = GFSK, DATARATE_E = 8��datarate=9600��
        
    /*lq PCKTCTRL1 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(1));
    regValue &= ~SPIRIT1_PPC_PCKTCTRL1_BITMASK_TXSOURCE;                    //lq TXSOURCE����λ��0 
    regValue |= SPIRIT1_TX_SOURCE_DATA_PN9 << SPIRIT1_GetBitNum(SPIRIT1_PPC_PCKTCTRL1_BITMASK_TXSOURCE);     
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(1), regValue);       //lq CRC: No CRC, TX source data: PN9,

    /*lq ����TX̬ */
    SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_TX);        
}

/*******************************************************************************
* ����  : SPIRIT1_CfgPN9Tx
* ����  : ����α�����������
* ����  : cs - SPIRIT1оƬ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_CfgPN9Tx(SPIRIT1TypeEnum cs)
{
    U8 regValue;

    /*lq ����Ready̬ */
    if (SPIRIT1_GetMcState(cs) == SPIRIT1_MC_STATE_RX
        || SPIRIT1_GetMcState(cs) == SPIRIT1_MC_STATE_TX)
    {
        /*lq �˳��շ�״̬ */
        SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_SABORT);
    }
    else
    {
        SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_READY);
    }

    /*lq MOD0 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCD_MOD0);
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_MOD0,
                      regValue & (~SPIRIT1_RCD_MOD0_BITMASK_CW));           //lq CW: disable��BT = 0.5, MOD_TYPE = GFSK, DATARATE_E = 8��datarate=9600��

    /*lq PCKTCTRL1 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(1));
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(1),
                      regValue | SPIRIT1_PPC_PCKTCTRL1_BITMASK_TXSOURCE);   //lq CRC: No CRC, TX source data: PN9,

    /*lq ����TX̬ */
    SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_TX);        
}

/*******************************************************************************
* ����  : SPIRIT1_CfgGpioTx
* ����  : GPIOֱ�ӷ�������
* ����  : cs - SPIRIT1оƬ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_CfgGpioTx(SPIRIT1TypeEnum cs)
{
    U8 regValue;

    /*lq ����Ready̬ */
    if (SPIRIT1_GetMcState(cs) == SPIRIT1_MC_STATE_RX
        || SPIRIT1_GetMcState(cs) == SPIRIT1_MC_STATE_TX)
    {
        /*lq �˳��շ�״̬ */
        SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_SABORT);
    }
    else
    {
        SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_READY);
    }

    /*lq MOD0 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCD_MOD0);
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_MOD0,
                      regValue & (~SPIRIT1_RCD_MOD0_BITMASK_CW));           //lq CW: disable��BT = 0.5, MOD_TYPE = GFSK, DATARATE_E = 8��datarate=9600��

    /*lq PCKTCTRL1 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(1));
    regValue &= ~SPIRIT1_PPC_PCKTCTRL1_BITMASK_TXSOURCE;                    //lq TXSOURCE����λ��0 
    regValue |= SPIRIT1_TX_SOURCE_DATA_GPIO << SPIRIT1_GetBitNum(SPIRIT1_PPC_PCKTCTRL1_BITMASK_TXSOURCE);     
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(1), regValue);       //lq CRC: No CRC, TX source data: GPIO,

    /*lq ����TX̬ */
    SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_TX);        
}

/*******************************************************************************
* ����  : SPIRIT1_ToReady
* ����  : ����ready̬
* ����  : cs - SPIRIT1оƬ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_StateToReady(SPIRIT1TypeEnum cs)
{
    if (SPIRIT1_GetMcState(cs) != SPIRIT1_MC_STATE_READY)
    {
        /*lq ����Ready̬ */
        if (SPIRIT1_GetMcState(cs) == SPIRIT1_MC_STATE_RX
            || SPIRIT1_GetMcState(cs) == SPIRIT1_MC_STATE_TX)
        {
            /*lq �˳��շ�״̬ */
            SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_SABORT);
        }
        else
        {
            SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_READY);
        }     
    }       
} 

/*******************************************************************************
* ����  : SPIRIT1_StateToTx
* ����  : ����Tx̬
* ����  : cs - SPIRIT1оƬ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_StateToTx(SPIRIT1TypeEnum cs)
{
    /*lq ����Ready̬ */
    SPIRIT1_StateToReady(cs);        

    /*lq ����Tx̬ */
    SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_TX);
}

/*******************************************************************************
* ����  : SPIRIT1_StateToRx
* ����  : ����Rx̬
* ����  : cs - SPIRIT1оƬ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_StateToRx(SPIRIT1TypeEnum cs)
{
    /*lq ����Ready̬ */
    SPIRIT1_StateToReady(cs);        

    /*lq ����Tx̬ */
    SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_RX);
}

/*******************************************************************************
* ����  : SPIRIT1_CfgFreq
* ����  : ����Ƶ��
* ����  : cs - SPIRIT1оƬ����
          channel - �ŵ�	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_CfgFreq(SPIRIT1TypeEnum cs, U8 channel)
{
    if (channel == AIS_CHANNEL_A)
    {
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_FU_CHNUM, SPIRIT1_PARA_CH_NUM_A); //lq CHNUM ��Ӧ161.975MHz                
    }
    else if (channel == AIS_CHANNEL_B)
    {
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_FU_CHNUM, SPIRIT1_PARA_CH_NUM_B); //lq CHNUM ��Ӧ162.025MHz                    
    }
} 

/*******************************************************************************
* ����  : SPIRIT1_TxCfg
* ����  : ��������
* ����  : cs - SPIRIT1оƬ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_TxCfg(SPIRIT1TypeEnum cs, U8 channel)
{
    SPIRIT1_StateToReady(cs);
    SPIRIT1_CfgFreq(cs, channel);
    SPIRIT1_StateToTx(cs);
}   

/*******************************************************************************
* ����  : SPIRIT1_RxCfg
* ����  : ��������
* ����  : cs - SPIRIT1оƬ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRIT1_RxCfg(SPIRIT1TypeEnum cs, U8 channel)
{
    SPIRIT1_StateToReady(cs);
    SPIRIT1_CfgFreq(cs, channel);
    SPIRIT1_StateToRx(cs);
} 

/*******************************************************************************
* ����  : SPIRIT1_IsTxDone
* ����  : �жϷ����Ƿ����
* ����  : cs - SPIRIT1оƬ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 SPIRIT1_IsTxDone(SPIRIT1TypeEnum cs)
{
    return TRUE;    //lq... ������ɱ�ʾ��ʱδ��
}