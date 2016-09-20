/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: SPI.c
  Author: lq       Version : V1.00        Date: 2014.05.09
  Description:     SPI
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "stm32f10x.h"
#include "def.h"
#include "SPI.h"
#include "Include.h"

/*lq SPIͨ�Žṹ�嶨�� */
#if 0
SPITxFIFOStruct SPI2TxFIFO;
SPIRxFIFOStruct SPI2RxFIFO;
#endif

/*******************************************************************************
* ����  : SPI2_Init
* ����  : ��ʼ��SPI2
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPI2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    /* Configure SPI2 pins: NSSΪ��ͨIO ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = SPI2_NSS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(SPI2_PORT, &GPIO_InitStructure); 
    SPI2_NSS_HIGH();             
    
    /* Configure SPI2 pins: SCK, MISO and MOSI ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = SPI2_SCK_PIN | SPI2_MISO_PIN | SPI2_MOSI_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    
    GPIO_Init(SPI2_PORT, &GPIO_InitStructure);  

    /* SPI2 Full-duplex Config -------------------------------------------------------------*/
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                   //lq CMX7042�ļĴ���Ϊ16λ���Ĵ�����ַΪ8Ϊ����1��CMX7042�Ĵ�����д������SPI����
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; //lq SPI ʱ��Ƶ��Ϊ36M/X(CMX7042��������ʱ��Ƶ��Ϊ5MHz)
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

    /*lq NSS�������ʹ�� */
    //lq SPI_SSOutputCmd(SPI2, ENABLE);

    /* Disable SPI2 TXE, RXNE interrupt */
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
    
    /* Disable SPI2 */
    SPI_Cmd(SPI2, ENABLE);     //lq SPI_Cmd(SPI2, DISABLE);
}

/*******************************************************************************
* ����  : SPI2_SendByte
* ����  : ʹ��SPI2����дһ���ֽڵ�����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 SPI2_SendByte(U8 byte)
{
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

	/* Send byte through the SPI2 peripheral */
	SPI_I2S_SendData(SPI2, byte);

	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI2);
}

/*******************************************************************************
* ����  : GPIO_SPI1_ExchangeByte
* ����  : ʹ��SPI1���ͺͽ���һ���ֽڵ����ݣ������ز�����SPI_CPOL = SPI_CPOL_Low��
          SPI_CPHA = SPI_CPHA_1Edge����MSB����
* ����  : sendData - �����ֽ�          
* ���  : ��
* ����  : receivedData - �����ֽ�
*******************************************************************************/
U8 GPIO_SPI1_ExchangeByte(U8 sendData)
{
    U16 i;
    U8  receivedData = 0;

    GPIO_SPI1_SCLK_LOW();    
    for (i = 0; i < 8; i++)
    {    
        /*lq send data */
        if ((sendData & 0x80) != 0)     //lq MSB first
        {
            GPIO_SPI1_SDI_HIGH();        
        }
        else
        {
            GPIO_SPI1_SDI_LOW();        
        }        
        GPIO_SPI1_SCLK_HIGH();                       
        sendData <<= 1;

        /*lq receive data */
        receivedData <<= 1;
        if (GPIO_SPI1_SDO_IN() != 0)
        {
            receivedData |= 1;
        }

        GPIO_SPI1_SCLK_LOW();
    }

    return receivedData;
}

/*******************************************************************************
* ����  : GPIO_SPI2_ExchangeByte
* ����  : ʹ��SPI1���ͺͽ���һ���ֽڵ����ݣ������ز�����SPI_CPOL = SPI_CPOL_Low��
          SPI_CPHA = SPI_CPHA_1Edge����MSB����
* ����  : sendData - �����ֽ�          
* ���  : ��
* ����  : receivedData - �����ֽ�
*******************************************************************************/
U8 GPIO_SPI2_ExchangeByte(U8 sendData)
{
    U16 i;
    U8  receivedData = 0;

    GPIO_SPI2_SCLK_LOW();    
    for (i = 0; i < 8; i++)
    {    
        /*lq send data */
        if ((sendData & 0x80) != 0)     //lq MSB first
        {
            GPIO_SPI2_SDI_HIGH();        
        }
        else
        {
            GPIO_SPI2_SDI_LOW();        
        }        
        GPIO_SPI2_SCLK_HIGH();                       
        sendData <<= 1;

        /*lq receive data */
        receivedData <<= 1;
        if (GPIO_SPI2_SDO_IN() != 0)
        {
            receivedData |= 1;
        }

        GPIO_SPI2_SCLK_LOW();
    }

    return receivedData;
}

#if 0
/*******************************************************************************
* ����  : SPITxFIFOStructInit
* ����  : �ṹ���ʼ��
* ����  : pSPITxFIFOStruct-�ṹ��ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPITxFIFOStructInit(SPITxFIFOStruct* pSPITxFIFOStruct)
{
	pSPITxFIFOStruct->TxFIFOInput = 0;
	pSPITxFIFOStruct->TxFIFOOutput = pSPITxFIFOStruct->TxFIFOInput;
	pSPITxFIFOStruct->TxState = FALSE;
}

/*******************************************************************************
* ����  : SPIRxFIFOStructInit
* ����  : �ṹ���ʼ��
* ����  : pSPIRxFIFOStruct-�ṹ��ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPIRxFIFOStructInit(SPIRxFIFOStruct* pSPIRxFIFOStruct)
{
	pSPIRxFIFOStruct->RxFIFOInput = 0;                                        //lq �б�
	pSPIRxFIFOStruct->RxFIFOOutput = pSPIRxFIFOStruct->RxFIFOInput;                                       
	pSPIRxFIFOStruct->RxEnFlg = FALSE;
	pSPIRxFIFOStruct->RxBuffIndex = 0;                                        //lq �б�
	pSPIRxFIFOStruct->Rx0x0dFlg = FALSE;	
    pSPIRxFIFOStruct->Rx0xb5Flg = FALSE;
	pSPIRxFIFOStruct->DoubleFlg = FALSE;
}

/*******************************************************************************
* ����  : SPIRxFIFOStructInit
* ����  : �ṹ���ʼ��
* ����  : pSPIRxFIFOStruct-�ṹ��ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPICommStructInit(U8 SPIInterface)
{
	if(SPIInterface == SPI_INTERFACE_2)
    {
        SPITxFIFOStructInit(&SPI2TxFIFO);
        SPIRxFIFOStructInit(&SPI2RxFIFO);    
    }
}

/*******************************************************************************
* ����  : SPI2Interrupt
* ����  : SPI2�ж�
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SPI2Interrupt(void)
{

}

/*******************************************************************************
* ���� ��SPITxIntProcess
* ���� ��SPI���͹��̣��ɴ��ڷ����жϵ���
* ���� ��pSPITxFIFOStruct��SPI���ͽṹ��ָ��
* ��� ����
* ���� ����
*******************************************************************************/
void SPITxIntProcess(SPITxFIFOStruct *pSPITxFIFOStruct)
{
	pSPITxFIFOStruct->TxFIFOOutput++;
	pSPITxFIFOStruct->TxFIFOOutput &= SPI_TX_FIFO_DEPTH - 1;                    //lq ����ָ��ѭ����ģUART_TX_FIFO_DEPTH - 1ȡ�ࣩ
		
	if(pSPITxFIFOStruct->TxFIFOOutput != pSPITxFIFOStruct->TxFIFOInput)         //lq �����������ݽṹ����Ϊ�գ�������Ҫ����
	{	
		pSPITxFIFOStruct->TxState = TRUE;	                                    //lq ��������������	

		if (pSPITxFIFOStruct == &SPI2TxFIFO)                                    //lq �����SPI2
		{
			SPI_I2S_SendData(SPI2, pSPITxFIFOStruct->TxFIFO[pSPITxFIFOStruct->TxFIFOOutput]); //lq ��SPI2��TDRд����			
		}												
	}
	else                                                                        //lq �����������ݽṹ��������
	{
		if (pSPITxFIFOStruct == &SPI2TxFIFO)                                    //lq �����SPI2
	  	{
            SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);                    //lq ���ͻ�������һ���Ĵ��������ж�ʧ�ܣ�
            SPI_Cmd(SPI2, DISABLE);			                                    //lq �ر�SPI2
            
            pSPITxFIFOStruct->TxState  = FALSE;                                 //lq ����Ƶ�����ж����ѣ���ʱ���ɺ���UartTxMainProcess()��
		                                                                        //lq ״̬��־pUartTxFIFO->TxStateѭ�������������Ҫ����	  	
	    }
    }
}

/*******************************************************************************
* ���� ��SPITxMainProcess
* ���� ��SPI������������TXE�ж�ʧ�ܵ�����£��ж���������Ҫ��
* ���� ��pSPITxFIFOStruct��SPI���ͽṹ��ָ��
* ��� ����
* ���� ����
*******************************************************************************/
void SPITxMainProcess(SPITxFIFOStruct *pSPITxFIFOStruct)                       
{
	if(pSPITxFIFOStruct->TxState == FALSE)	//����                                  //lq SPI���͵�ǰ��
	{
		if(pSPITxFIFOStruct->TxFIFOInput != pSPITxFIFOStruct->TxFIFOOutput)         //lq �����ݿɷ�
		{
			pSPITxFIFOStruct->TxState = TRUE;	//æ                                //lq ��ǰ����������
			
            if (pSPITxFIFOStruct == &SPI2TxFIFO)
			{
				/*lq ����SPI2 */
                SPI_Cmd(SPI2, ENABLE);
                SPI_I2S_SendData(SPI2, pSPITxFIFOStruct->TxFIFO[pSPITxFIFOStruct->TxFIFOOutput]);
				SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE); 
			}            							
		}
	}
}

/*******************************************************************************
* ���� ��SPITxDataBuf
* ���� ��SPI���ͻ�������
* ���� ��SPIInterface��SPI�ӿڱ�־
		 pData�����ݻ���ָ��
		 Length�����ݳ���
* ��� ����
* ���� ����
*******************************************************************************/
U8 SPITxDataBuf(U8 SPIInterface, U8 *pData, U16 Length)
{				
	U16 i, lastcnt;
	U8 uchRet;
	SPITxFIFOStruct *pSPITxFIFO;
	
	if(SPIInterface == SPI_INTERFACE_2)
	{
		pSPITxFIFO = &SPI2TxFIFO;
	}
    else
    {
        return FALSE;
    }
	
	lastcnt = (SPI_TX_FIFO_DEPTH - 1) 
		- ((pSPITxFIFO->TxFIFOInput - pSPITxFIFO->TxFIFOOutput)                 //lq ʣ��ռ䣨�ֽڣ��� 
		& (SPI_TX_FIFO_DEPTH - 1));                                             //lq?
	if (lastcnt >= Length)                                                      //lq ʣ��ռ乻
	{
		for (i = 0; i < Length; i++)
		{			
			pSPITxFIFO->TxFIFO[pSPITxFIFO->TxFIFOInput++] = pData[i];
			
			pSPITxFIFO->TxFIFOInput &= (SPI_TX_FIFO_DEPTH - 1);		
		}	
		
		uchRet = TRUE;		
	}                                                                           
	else                                                                        //lq ʣ��ռ䲻��
	{
		pSPITxFIFO->TxFIFO[pSPITxFIFO->TxFIFOInput++] = 0xFF;                   //lq?
		pSPITxFIFO->TxFIFOInput &= (SPI_TX_FIFO_DEPTH - 1);		
		
        uchRet = FALSE;
	}
	
	UartTxMainProcess(pSPITxFIFO);                                             //lq ���������һ�����ݵķ��ͣ���Ϊÿ�η������������ݺ�TXE�жϱ�ʧ�ܣ����ôη��ͺ������ݵķ���ͨ��TXE�ж����

	return uchRet;	
}

/*******************************************************************************
* ���� ��SPIRxMainProcess
* ���� ��SPI������������TXE�ж�ʧ�ܵ�����£��ж���������Ҫ��
* ���� ��pSPITxFIFOStruct��SPI���ͽṹ��ָ��
* ��� ����
* ���� ����
*******************************************************************************/
U8 SPIRxMainProcess(U8 SPIInterface)                       
{
	SPIRxFIFOStruct *pSPIRxFIFO;

	if(SPIInterface == SPI_INTERFACE_2)
	{
		pSPIRxFIFO = &SPI2RxFIFO;
	}
    else
    {
        return FALSE;
    }

	if(((pSPIRxFIFO->RxFIFOInput + 1) % SPI_RX_FIFO_DEPTH)
       != pSPIRxFIFO->RxFIFOOutput)                                     //lq �н��տռ�
	{
		pSPIRxFIFO->RxEnFlg = TRUE;	                                    //lq ����ʹ��
		
        if (pSPIRxFIFO == &SPI2RxFIFO)
		{
			/*lq ����SPI2 */
            SPI_Cmd(SPI2, ENABLE);
			SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE); 
		}            							
	}
}
#endif
