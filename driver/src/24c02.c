/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: I2c.c
  Author: vozhj      Version : V1.00        Date: 2011.10.18
  Description:    I2c����24c02
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>

*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f10x.h"
#include "def.h"
#include "24c02.h"
#include "common.h"
#include "debug.h"
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
U16 EEPROM_ADDRESS;
U16 SR1_Tmp = 0;
U32 I2C_Count;
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : GPIO5_Configuration
* Description    : Configure the used I/O ports pin
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO5_Configuration(void)                                                  //lq ����24C02��WP����
{
  GPIO_InitTypeDef  GPIO_InitStructure; 

  /* Configure I2C1 pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configure the used I/O ports pin
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO6_Configuration(void)                                                  //lq ����24C02��SCL����
{
  GPIO_InitTypeDef  GPIO_InitStructure; 

  /* Configure I2C1 pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configure the used I/O ports pin
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO7_Configuration(U8 PIN_Dir)                                            //lq ����24C02��SDA����
{
  GPIO_InitTypeDef  GPIO_InitStructure; 

  /* Configure I2C1 pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  if(PIN_Dir == PIN_OUT)
  {
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  }
  else
  {
 		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  }

  GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name  : I2C_EE_Init
* Description    : Initializes peripherals used by the I2C EEPROM driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_Init()
{
  /* GPIO configuration */
  GPIO5_Configuration();                                                        //lq ����24C02��WP����
  GPIO6_Configuration();                                                        //lq ����24C02��SCL����
  GPIO7_Configuration(PIN_OUT);                                                 //lq ����24C02��SDA����

  WP_H;                                                                         //lq+д��������    
}
/*******************************************************************************
* Function Name  : I2C function
* Description    : I2C relative driver functions.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_delay(void)
{	
   U8 i = 1; //�ȴ�1����������֮ǰ20
   while(i) 
   { 
     	i--; 
   } 
}

U8 I2C_Start(void)
{                                                                               //lq �ȸ����ݣ��ٸ�ʱ�ӣ������أ�
	SDA_H;                                                                      //lq ARM��SDA���������1��
	SCL_H;                                                                      //lq ARM��SCL���������1��
	I2C_delay();
	if(!SDA_read)
	{
		return FALSE;	//SDA��Ϊ�͵�ƽ������æ,�˳�
	}
	SDA_L;
	I2C_delay();
	if(SDA_read)
    {
		return FALSE;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
	}
	SDA_L;                                                                      //lq 24C02��START condition "��SCL=1ʱ��SDA��1��Ϊ0"
	I2C_delay();

	return TRUE;
}

void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;                                                                      //lq �ȸ����ݣ��ٸ�ʱ�ӣ������أ�
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;                                                                      //lq 24C02��STOP condition "��SCL=1ʱ��SDA��0��Ϊ1"    
	I2C_delay();
}

void I2C_Ack(void)
{	
	SCL_L;
	I2C_delay();                                                                //lq �ȸ����ݣ��ٸ�ʱ�ӣ������أ�
	SDA_L;                                                                      //lq 24C02��ACK condition "��SCL=1ʱ��SDAΪ0"
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

void I2C_NoAck(void)
{	
	SCL_L;
	I2C_delay();                                                                //lq �ȸ����ݣ��ٸ�ʱ�ӣ������أ�
	SDA_H;                                                                      //lq 24C02��ACK condition "��SCL=1ʱ��SDAΪ1"
	I2C_delay();
	SCL_H;                                                                      
	I2C_delay();
	SCL_L;
	I2C_delay();
}

U8 I2C_WaitAck(void) 	 //����Ϊ:=1��ACK,=0��ACK
{
	SCL_L;
	I2C_delay();
	SDA_H;			                                                            //lq �ȸ����ݣ��ٸ�ʱ�ӣ������أ�
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
      SCL_L;                                                                    //lq �����1����������1����˵����(1)��ǰΪд״̬����2����ǰΪ��״̬��������Ϊ��1����
      return (FALSE);                                                           //lq (1)�ͣ�2����˵��24C02��δ����'ACK'
	}
	SCL_L;                                                                      //lq �����1�������������ǡ�0����˵����ǰд��Ч������ǰΪ��״̬��������Ϊ��0����
	return TRUE;                                                                //lq ˵��24C02�ѷ���'ACK'
}

void I2C_SendByte(U8 SendByte) //���ݴӸ�λ����λ//
{
    U8 i=8;
    while(i--)
    {
        SCL_L;
        I2C_delay();
        if(SendByte&0x80)
        {
        	SDA_H;                                                              //lq �ȸ����ݣ��ٸ�ʱ�ӣ������أ�����ʾ���豸24C02��������
        }
        else
        {
        	SDA_L;
        }
        SendByte<<=1;
        I2C_delay();
		SCL_H;
        I2C_delay();
    }
    SCL_L;
}

U8 I2C_ReceiveByte(void)  //���ݴӸ�λ����λ//
{ 
    U8 i=8;
    U8 ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
        ReceiveByte<<=1;      
        SCL_L;
        I2C_delay();
	    SCL_H;                                                                  //lq �ṩʱ�ӣ������أ�����ʾ���豸24C02�������ݣ�Master��������
        I2C_delay();	
        if(SDA_read)
        {
        	ReceiveByte|=0x01;
        }
    }
    SCL_L;
    return ReceiveByte;
}

/*******************************************************************************
* Function Name  : I2C_EE_BufferWrite
* Description    : Writes buffer of data to the I2C EEPROM.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
U8 I2C_EE_BufferWrite(char* pBuffer, U8 WriteAddress, U16 NumByteToWrite)
{
	U16 i;
	U16 wait_count=0;//�ȴ�ʱ�������
    WP_L;                                                                       //lq+д�����ر�
    
    if(!I2C_Start())
	{
        WP_H;                                                                   //lq+д��������
		return FALSE;
	}
    I2C_SendByte(0xA0);//���ø���ʼ��ַ+������ַ                                //lq �����豸��ַ
    if(!I2C_WaitAck())
	{
		I2C_Stop(); 
        WP_H;                                                                   //lq+д��������
		return FALSE;
	} 
    I2C_SendByte((U8)(WriteAddress & 0x00FF));   //���õ���ʼ��ַ               //lq �����ֽڵ�ַ
    I2C_WaitAck();
	
	for(i=0; i<NumByteToWrite; i++)
	{	
    	I2C_SendByte(pBuffer[i]);                                               //lq 24C02�ڲ��е�ַ�Զ���1����
    	I2C_WaitAck();                                                          //lq ÿ��(START��STOP֮��)���д16B
	}   
    I2C_Stop(); 
	//ע�⣺��Ϊ����Ҫ�ȴ�EEPROMд�꣬���Բ��ò�ѯ����ʱ��ʽ(50ms)
	//Delay_ms(50);
    while (I2C_WaitAck()==FALSE)//��ACK                                         //lq?
	{
		wait_count++;
		if (wait_count>3200)//����Ӧ����
		{
            break;
        }
		
		if(!I2C_Start())
		{
            WP_H;                                                               //lq+д��������
			return FALSE;
		}
		I2C_SendByte(0xA0);//���ø���ʼ��ַ+������ַ 	
	}

    WP_H;                                                                       //lq+д��������
    return TRUE;
 
}

/*******************************************************************************
* ����  : EraseAllData
* ����  : ������������
* ����  : ��
* ����  : TRUE �ɹ� FALSE ʧ��
*******************************************************************************/
U8 EraseAllData(void)
{
	U16 i;
	char chTemp[10];
	U8 ucCheck;
	U8 ucResult;

	for (i=0 ; i < 10; i++)
	{
		chTemp[i] = 0;                                                          //lq ȫдΪ��0��
	}

	ucResult = TRUE;
	for (i=0; i < 32; i++)
	{
		ucCheck = I2C_EE_BufferWrite(chTemp, i * 8,8);                          //lq ÿ��д8B

		if (ucCheck == FALSE)
		{
			ucResult = FALSE;
		}
	}

	return (ucResult);
}
	
		

/***************************************************************************************
** ��������: I2C_EE_BufferRead
** ��������: I2C�洢�����������ֽڶ�ȡ����
** ��    ��: pBuffer                / ����ָ��          
** ��    ��: WriteAddr                / д���ַ
** ��    ��: NumByteToRead        / д���ֽ���
** �� �� ֵ: None 
** ��    ע: ���е�����ֻ����һ������֮��       
***************************************************************************************/
U8 I2C_EE_BufferRead(char* pBuffer, U8 ReadAddress, U16 NumByteToRead)
{
	if(!I2C_Start())
	{
		return FALSE;
	}
	I2C_SendByte(0xA0);//���ø���ʼ��ַ+������ַ                                //lq �����豸��ַ 
	 
	if(!I2C_WaitAck())
	{
		I2C_Stop(); 
		return FALSE;
	}
    I2C_SendByte((U8)(ReadAddress & 0x00FF));   //���õ���ʼ��ַ                //lq �����ֽڵ�ַ
    I2C_WaitAck();
    I2C_Start();                                                                //lq?
    I2C_SendByte(0xA1);                                                         //lq?
    I2C_WaitAck();
    while(NumByteToRead)
    {
		*pBuffer = I2C_ReceiveByte();
		if(NumByteToRead == 1)
		{
			I2C_NoAck();
		}
		else 
		{
			I2C_Ack(); 
		}
      	pBuffer++;
      	NumByteToRead--;
    }
    I2C_Stop();
    return TRUE;
}
#if 0
//I2C����
/*******************************************************************************
* Function Name  : Buffercmp
* Description    : Compares two buffers.
* Input          : - pBuffer1, pBuffer2: buffers to be compared.
*                : - BufferLength: buffer's length
* Output         : None
* Return         : PASSED: pBuffer1 identical to pBuffer2
*                  FAILED: pBuffer1 differs from pBuffer2
*******************************************************************************/
U8 Buffercmp(char* pBuffer1, char* pBuffer2, U16 BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return 0;	//ʧ��
    }

    pBuffer1++;
    pBuffer2++;
  }

  return 1;
}


/***************************************************************************************
** ��������: I2C_Test
** ��������: I2C���Ժ���
       
***************************************************************************************/
 void I2C_Test(void)
{
 
  /* Public  function  ---------------------------------------------------------*/

	char Tx1_Buffer[6] = "777777";
	char Tx2_Buffer[6] = "999999";
	char Rx1_Buffer[BufferSize1], Rx2_Buffer[BufferSize2];
	U8 status1=0;
	U8 status2=0;
	
	

	  I2C_EE_BufferWrite(Tx1_Buffer, EEPROM_WriteAddress0, BufferSize1); 
	
	
	  I2C_EE_BufferRead(Rx1_Buffer, EEPROM_ReadAddress0, BufferSize1); 
	  
	
	  /* Check if the data written to the memory is read correctly */
	  status1 = Buffercmp(Tx1_Buffer, Rx1_Buffer, BufferSize1);
	  
	  //�����
	  if(status1 == 1)
	  {
	   	//UartResponseMsg(Tx1_Buffer);
		UartTxString(Tx1_Buffer);
		UartTxString("\r\n");

		UartTxString(Rx1_Buffer);
		//UartResponseMsg(Rx1_Buffer);	
	  }
	  if(status1 == 0)
	  {
	
	  }
	  
	  Delay_ms(500);
	  I2C_EE_BufferWrite(Tx2_Buffer, EEPROM_WriteAddress1, BufferSize2); 
	
	 
	  I2C_EE_BufferRead(Rx2_Buffer, EEPROM_ReadAddress1, BufferSize2);
	
	 
	  status2 = Buffercmp(Tx2_Buffer, Rx2_Buffer, BufferSize2);
	  if(status2 == 1)
	  {
	
	  }
	  if(status2 == 0)
	  {
		
	  }
}

#endif

/*******************************************************************************
* Function Name  : I2C_EE_BufferWrite
* Description    : Writes buffer of data to the I2C EEPROM.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
                   - wpFlg : whether enable write protection or not.
* Output         : None
* Return         : None
*******************************************************************************/
U8 I2C_EE_BufferWriteSelfCheck(char* pBuffer, U8 WriteAddress, U16 NumByteToWrite, U8 wpFlg)
{
	U16 i;
	U16 wait_count=0;//�ȴ�ʱ�������

    if (wpFlg == TRUE)
    {
        WP_L;                                                                   //lq+д�����ر�    
    }   
    else
    {
        WP_H;                                                                   //lq+д��������    
    }
        
    if(!I2C_Start())
	{
        WP_H;                                                                   //lq+д��������
		return FALSE;
	}
    I2C_SendByte(0xA0);//���ø���ʼ��ַ+������ַ                                //lq �����豸��ַ
    if(!I2C_WaitAck())
	{
		I2C_Stop(); 
        WP_H;                                                                   //lq+д��������
		return FALSE;
	} 
    I2C_SendByte((U8)(WriteAddress & 0x00FF));   //���õ���ʼ��ַ               //lq �����ֽڵ�ַ
    I2C_WaitAck();
	
	for(i=0; i<NumByteToWrite; i++)
	{	
    	I2C_SendByte(pBuffer[i]);                                               //lq 24C02�ڲ��е�ַ�Զ���1����
    	I2C_WaitAck();                                                          //lq ÿ��(START��STOP֮��)���д16B
	}   
    I2C_Stop(); 
	//ע�⣺��Ϊ����Ҫ�ȴ�EEPROMд�꣬���Բ��ò�ѯ����ʱ��ʽ(50ms)
	//Delay_ms(50);
    while (I2C_WaitAck()==FALSE)//��ACK                                         //lq?
	{
		wait_count++;
		if (wait_count>3200)//����Ӧ����
		{
            break;
        }
		
		if(!I2C_Start())
		{
            WP_H;                                                               //lq+д��������
			return FALSE;
		}
		I2C_SendByte(0xA0);//���ø���ʼ��ַ+������ַ 	
	}

    WP_H;                                                                       //lq+д��������
    return TRUE;
 
}

/*******************************************************************************
* Function Name  : EEPROM_SelfCheck
* Description    : EEPROM�Լ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EEPROM_SelfCheck(EepromSelfCheckStruct *pEepromSelfCheckStruct)
{
  	U8 temp1;
    U8 temp2;
    U8 testValue1;
    U8 testValue2;
    U8 testAddr;

    if (pEepromSelfCheckStruct->StartFlg == TRUE)
    {
        testValue1 = 0xE2;
        testValue2 = 0x00;
        testAddr = 255;
    
        /*lq ��¼���Ե�ַ�ֽڵ�Ԫ��ԭʼ���� */
        I2C_EE_BufferRead((char *)&temp1, testAddr, 1);
    
        /*lq д�����رգ�������Ե�ַ��Ԫд��һ�ֽ����� */
        I2C_EE_BufferWriteSelfCheck((char *)&testValue1, testAddr, 1, TRUE);
    
        /*lq ��ȡд������� */
        I2C_EE_BufferRead((char *)&temp2, testAddr, 1);
    
        if (temp2 == testValue1)
        {
            pEepromSelfCheckStruct->DataCommOkFlg = TRUE;    
    
            /*lq д����������������Ե�ַ��Ԫд��һ�ֽ������� */
            I2C_EE_BufferWriteSelfCheck((char *)&testValue2, testAddr, 1, FALSE);
    
            /*lq ��ȡд������� */
            I2C_EE_BufferRead((char *)&temp2, testAddr, 1);
            
            /*lq �ж������Ƿ�ı� */
            if (temp2 == testValue1)
            {
                pEepromSelfCheckStruct->WPOkFlg = TRUE;                    
            }
            else
            {
                pEepromSelfCheckStruct->WPOkFlg = FALSE;        
            }                 
        }
        else
        {
            /*lq �޷���һ��ȷ�������׼ȷԭ�� */
            pEepromSelfCheckStruct->WPOkFlg = FALSE;
            pEepromSelfCheckStruct->DataCommOkFlg = FALSE;    
        }
    
        /*lq д�������ֵ */
        I2C_EE_BufferWrite((char *)&temp1, testAddr, 1);               
    
        pEepromSelfCheckStruct->CompleteFlg = TRUE;        
        pEepromSelfCheckStruct->StartFlg = FALSE;    
    }
}

/*******************************************************************************
* ����  : EEPROM_EepromSelfCheckStructInit
* ����  : ��ʼ���ṹ��
* ����  : pEepromSelfCheckStruct - EepromSelfCheckStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void EEPROM_EepromSelfCheckStructInit(EepromSelfCheckStruct *pEepromSelfCheckStruct)
{
    pEepromSelfCheckStruct->WPOkFlg = FALSE;
    pEepromSelfCheckStruct->DataCommOkFlg = FALSE;
    pEepromSelfCheckStruct->StartFlg = TRUE;       
    pEepromSelfCheckStruct->CompleteFlg = FALSE;        
}
