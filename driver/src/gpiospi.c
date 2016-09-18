/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: gpiospi.h
  Author: lq       Version : V1.00        Date: 2014.07.01
  Description:     ͨ��GPIOģ��spi
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "def.h"
#include "stm32f10x.h"
#include "gpiospi.h"
#include "Include.h"

/*lq ģ��ʵ������*/
static GpioSpiStruct GpioSpiInstance1;     //lq ʵ��1

/*******************************************************************************
* ����  : GPIOSPI_StructInit
* ����  : GpioSpiStruct�ṹ���ʼ��
* ����  : pGpioSpiStruct - GpioSpiStruct����ָ��
          cpol - ʱ�Ӽ���,
          cpha - ʱ����λ,
          baudRatePrescaler - �����ʷ�Ƶֵ,
          fistBit - ���͵�һλ,
          dataSize - �������ݴ�С)		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void GPIOSPI_StructInit(GpioSpiStruct *pGpioSpiStruct,
                               GpioSpiCPOLEnum cpol,
                               GpioSpiCPHAEnum cpha,
                               U16 baudRatePrescaler,
                               GpioSpiFirstBitEnum fistBit,
                               GpioSpiDataSizeEnum dataSize)
{
    pGpioSpiStruct->SPI_CPOL = cpol;
    pGpioSpiStruct->SPI_CPHA = cpha;
    pGpioSpiStruct->SPI_BaudRatePrescaler = baudRatePrescaler;
    pGpioSpiStruct->SPI_FirstBit = fistBit;
    pGpioSpiStruct->SPI_DataSize = dataSize;
    pGpioSpiStruct->SPI_ClockCounter = 0;  
}

/*******************************************************************************
* ����  : GPIOSPI_InstanceInit
* ����  : GpioSpiʵ����ʼ��
* ����  : GpioSpiTypeEnum - ʵ������
          cpol - ʱ�Ӽ���,
          cpha - ʱ����λ,
          baudRatePrescaler - �����ʷ�Ƶֵ,
          fistBit - ���͵�һλ,
          dataSize - �������ݴ�С)		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void GPIOSPI_InstanceInit(GpioSpiTypeEnum spiTye,
                          GpioSpiCPOLEnum cpol,
                          GpioSpiCPHAEnum cpha,
                          U16 baudRatePrescaler,
                          GpioSpiFirstBitEnum fistBit,
                          GpioSpiDataSizeEnum dataSize)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    if (spiTye == GPIOSPI1)
    {
        GpioSpiInstance1.InstanceNum = GPIOSPI1;
        GPIOSPI_StructInit(&GpioSpiInstance1, cpol, cpha, baudRatePrescaler, fistBit, dataSize);

        GPIO_InitStructure.GPIO_Pin = GPIOSPI1_CS_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
        GPIO_Init(GPIOSPI1_CS_PORT, &GPIO_InitStructure);
        GPIOSPI1_CS_HIGH();                                //lq Ƭѡʧ��
        
        GPIO_InitStructure.GPIO_Pin = GPIOSPI1_SCLK_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
        GPIO_Init(GPIOSPI1_SCLK_PORT, &GPIO_InitStructure);
        if (GpioSpiInstance1.SPI_CPOL == GPIOSPI_CPOL_Low)
        {
            GPIOSPI1_SCLK_LOW();
        }
        else
        {
            GPIOSPI1_SCLK_HIGH();
        }
    
        GPIO_InitStructure.GPIO_Pin = GPIOSPI1_MOSI_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
        GPIO_Init(GPIOSPI1_MOSI_PORT, &GPIO_InitStructure);
        GPIOSPI1_MOSI_LOW();                               
    
        GPIO_InitStructure.GPIO_Pin = GPIOSPI1_MISO_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
        GPIO_Init(GPIOSPI1_MISO_PORT, &GPIO_InitStructure);
    }
}

/*******************************************************************************
* ����  : GPIOSPI_WaitClockPeriod
* ����  : ʱ������
* ����  : pGpioSpiStruct - GpioSpiStruct����ָ��		  
* ���  : ��
* ����  : TRUE - ʱ�����ڶ�ʱ����FALSE - ʱ�����ڶ�ʱδ��
*******************************************************************************/
static U8 GPIOSPI_WaitClockPeriod(GpioSpiStruct *pGpioSpiStruct)
{
    if (pGpioSpiStruct->SPI_ClockCounter >= pGpioSpiStruct->SPI_BaudRatePrescaler)
    {
        return TRUE;
    }        
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : GPIOSPI_InstWaitClockPeriod
* ����  : ʱ������
* ����  : spiTye - SPI����		  
* ���  : ��
* ����  : TRUE - ʱ�����ڶ�ʱ����FALSE - ʱ�����ڶ�ʱδ��
*******************************************************************************/
U8 GPIOSPI_InstWaitClockPeriod(GpioSpiTypeEnum spiTye)
{
    if (spiTye == GPIOSPI1)
    {
        return GPIOSPI_WaitClockPeriod(&GpioSpiInstance1);
    }
}

/*******************************************************************************
* ����  : GPIOSPI_WaitClockPeriod
* ����  : ʱ�ӵ�һ��λ
* ����  : pGpioSpiStruct - GpioSpiStruct����ָ��		  
* ���  : ��
* ����  : TRUE - ʱ�����ڶ�ʱ����FALSE - ʱ�����ڶ�ʱδ��
*******************************************************************************/
static U8 GPIOSPI_WaitClockPhase(GpioSpiStruct *pGpioSpiStruct)
{
    if (pGpioSpiStruct->SPI_ClockCounter >= (pGpioSpiStruct->SPI_BaudRatePrescaler >> 1))
    {
        return TRUE;
    }        
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : GPIOSPI_ResetClockPeriod
* ����  : ��λʱ�Ӽ���
* ����  : pGpioSpiStruct - GpioSpiStruct����ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void GPIOSPI_ResetClockCounter(GpioSpiStruct *pGpioSpiStruct)
{
    pGpioSpiStruct->SPI_ClockCounter = 0;
}

/*******************************************************************************
* ����  : GPIOSPI_InstResetClockCounter
* ����  : ��λʱ�Ӽ���
* ����  : spiTye - SPI����			  
* ���  : ��
* ����  : ��
*******************************************************************************/
void GPIOSPI_InstResetClockCounter(GpioSpiTypeEnum spiTye)
{
    if (spiTye == GPIOSPI1)
    {
        GPIOSPI_ResetClockCounter(&GpioSpiInstance1);
    }
}

/*******************************************************************************
* ����  : GPIOSPI_ClockCount
* ����  : ʱ�Ӽ���
* ����  : pGpioSpiStruct - GpioSpiStruct����ָ��			  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void GPIOSPI_ClockCount(GpioSpiStruct *pGpioSpiStruct)
{
    if (pGpioSpiStruct->SPI_ClockCounter < pGpioSpiStruct->SPI_BaudRatePrescaler)    
    {
        pGpioSpiStruct->SPI_ClockCounter++;
    }
}

/*******************************************************************************
* ����  : GPIOSPI_InstClockCount
* ����  : ʱ�Ӽ���
* ����  : spiType - SPI����			  
* ���  : ��
* ����  : ��
*******************************************************************************/
void GPIOSPI_InstClockCount(GpioSpiTypeEnum spiType)
{
    if (spiType == GPIOSPI1)
    {
        GPIOSPI_ClockCount(&GpioSpiInstance1);
    }
}

#if 0
/*******************************************************************************
* ����  : GPIOSPI_SendData
* ����  : ��������
* ����  : pGpioSpiStruct - GpioSpiStruct����ָ��
          data - ��������			  
* ���  : ��
* ����  : returnData - ��������
*******************************************************************************/
static U16 GPIOSPI_SendData(GpioSpiStruct *pGpioSpiStruct, U16 data)
{
    U8  dataSize;
    U16 dataMask;
    U16 i;
    U16 returnData;

    returnData = 0;

    if (pGpioSpiStruct->SPI_DataSize == GPIOSPI_DataSize_8b)
    {
        dataSize = 8;
        if (pGpioSpiStruct->SPI_FirstBit == GPIOSPI_FirstBit_MSB)
        {
            dataMask = 0x0080; 
        }
        else
        {
            dataMask = 0x0001;             
        }
    }
    else
    {
        dataSize = 16;
        if (pGpioSpiStruct->SPI_FirstBit == GPIOSPI_FirstBit_MSB)
        {
            dataMask = 0x8000; 
        }
        else
        {
            dataMask = 0x0001;             
        }
    }

    if (pGpioSpiStruct->InstanceNum == GPIOSPI1)    
    {
        /*lq Ƭѡʹ�� */
        if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
        {
            GPIOSPI1_SCLK_LOW();
        }
        else
        {
            GPIOSPI1_SCLK_HIGH();        
        }
        GPIOSPI1_CS_LOW();
    
        /*lq �������� */
        for (i = 0; i < dataSize; i++)
        {
            if (pGpioSpiStruct->SPI_CPHA == GPIOSPI_CPHA_1Edge)
            {
                /*lq �ṩ�������� */
                if ((data & dataMask) == dataMask)
                {
                    GPIOSPI1_MOSI_HIGH();   //lq bit = 1;
                }
                else
                {
                    GPIOSPI1_MOSI_LOW();    //lq bit = 0;
                }
                
                /*lq �ṩʱ����1 */ 
                if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
                {
                    GPIOSPI1_SCLK_HIGH();                    
                }
                else
                {
                    GPIOSPI1_SCLK_LOW();
                }
                GPIOSPI_ResetClockCounter(pGpioSpiStruct);

                while (GPIOSPI_WaitClockPhase(pGpioSpiStruct) == FALSE);            
                
                /*lq ��ȡ�������ݣ����ṩʱ�ӱ���2֮ǰ�� */
                returnData <<= 1;
                returnData |= GPIOSPI1_MISO_IN_READ();

                /*lq �ṩʱ����2 */ 
                if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
                {
                    GPIOSPI1_SCLK_LOW();
                }
                else
                {
                    GPIOSPI1_SCLK_HIGH();        
                }
                
                while (GPIOSPI_WaitClockPeriod(pGpioSpiStruct) == FALSE);                            

                /*lq ����������λ */
                if (pGpioSpiStruct->SPI_FirstBit == GPIOSPI_FirstBit_MSB)
                {
                    data <<= 1; 
                }
                else
                {
                    data >>= 1;             
                }
            }
            else
            {            
                /*lq �ṩʱ����1 */ 
                if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
                {
                    GPIOSPI1_SCLK_HIGH();                    
                }
                else
                {
                    GPIOSPI1_SCLK_LOW();
                }
                GPIOSPI_ResetClockCounter(pGpioSpiStruct);

                while (GPIOSPI_WaitClockPhase(pGpioSpiStruct) == FALSE);            
                
                /*lq �ṩ�������� */
                if ((data & dataMask) == dataMask)
                {
                    GPIOSPI1_MOSI_HIGH();   //lq bit = 1;
                }
                else
                {
                    GPIOSPI1_MOSI_LOW();    //lq bit = 0;
                }

                /*lq �ṩʱ����2 */ 
                if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
                {
                    GPIOSPI1_SCLK_LOW();
                }
                else
                {
                    GPIOSPI1_SCLK_HIGH();        
                }
                                                               
                while (GPIOSPI_WaitClockPeriod(pGpioSpiStruct) == FALSE);                            

                /*lq ��ȡ�������ݣ����ṩʱ�ӱ���1֮ǰ�� */
                returnData <<= 1;
                returnData |= GPIOSPI1_MISO_IN_READ();

                /*lq ����������λ */
                if (pGpioSpiStruct->SPI_FirstBit == GPIOSPI_FirstBit_MSB)
                {
                    data <<= 1; 
                }
                else
                {
                    data >>= 1;             
                }            
            }         
        }

        /*lq Ƭѡʧ�� */
        GPIOSPI1_CS_HIGH();        
    }

    return returnData;
}
#else
/*******************************************************************************
* ����  : GPIOSPI_SendData
* ����  : �������ݣ��޸�����ʱ���غͶ�д����λ��˳��
* ����  : pGpioSpiStruct - GpioSpiStruct����ָ��
          data - ��������			  
* ���  : ��
* ����  : returnData - ��������
*******************************************************************************/
static U16 GPIOSPI_SendData(GpioSpiStruct *pGpioSpiStruct, U16 data)
{
    U8  dataSize;
    U16 dataMask;
    U16 i;
    U16 returnData;

    returnData = 0;

    if (pGpioSpiStruct->SPI_DataSize == GPIOSPI_DataSize_8b)
    {
        dataSize = 8;
        if (pGpioSpiStruct->SPI_FirstBit == GPIOSPI_FirstBit_MSB)
        {
            dataMask = 0x0080; 
        }
        else
        {
            dataMask = 0x0001;             
        }
    }
    else
    {
        dataSize = 16;
        if (pGpioSpiStruct->SPI_FirstBit == GPIOSPI_FirstBit_MSB)
        {
            dataMask = 0x8000; 
        }
        else
        {
            dataMask = 0x0001;             
        }
    }

    if (pGpioSpiStruct->InstanceNum == GPIOSPI1)    
    {
        /*lq Ƭѡʹ�� */
        if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
        {
            GPIOSPI1_SCLK_LOW();
        }
        else
        {
            GPIOSPI1_SCLK_HIGH();        
        }
        GPIOSPI1_CS_LOW();
    
        /*lq �������� */
        for (i = 0; i < dataSize; i++)
        {
            if (pGpioSpiStruct->SPI_CPHA == GPIOSPI_CPHA_1Edge)
            {
                /*lq �ṩ��������λ */
                if ((data & dataMask) == dataMask)
                {
                    GPIOSPI1_MOSI_HIGH();   //lq bit = 1;
                }
                else
                {
                    GPIOSPI1_MOSI_LOW();    //lq bit = 0;
                }

                GPIOSPI_ResetClockCounter(pGpioSpiStruct);

                while (GPIOSPI_WaitClockPhase(pGpioSpiStruct) == FALSE);
                                
                /*lq �ṩʱ����1��ָʾ���豸������ */ 
                if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
                {
                    GPIOSPI1_SCLK_HIGH();                    
                }
                else
                {
                    GPIOSPI1_SCLK_LOW();
                }
                            
                /*lq ��ȡ�������ݣ����豸������ */
                returnData <<= 1;
                returnData |= GPIOSPI1_MISO_IN_READ();

                while (GPIOSPI_WaitClockPeriod(pGpioSpiStruct) == FALSE);                            

                /*lq �ṩʱ����2��ָʾ���豸����������λ��*/ 
                if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
                {
                    GPIOSPI1_SCLK_LOW();
                }
                else
                {
                    GPIOSPI1_SCLK_HIGH();        
                }
                
                /*lq ����������λ */
                if (pGpioSpiStruct->SPI_FirstBit == GPIOSPI_FirstBit_MSB)
                {
                    data <<= 1; 
                }
                else
                {
                    data >>= 1;             
                }
            }
            else
            {            
                /*lq �ṩʱ����1��ָʾ���豸����������λ�� */ 
                if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
                {
                    GPIOSPI1_SCLK_HIGH();                    
                }
                else
                {
                    GPIOSPI1_SCLK_LOW();
                }
                
                /*lq �ṩ��������λ */
                if ((data & dataMask) == dataMask)
                {
                    GPIOSPI1_MOSI_HIGH();   //lq bit = 1;
                }
                else
                {
                    GPIOSPI1_MOSI_LOW();    //lq bit = 0;
                }

                GPIOSPI_ResetClockCounter(pGpioSpiStruct);

                while (GPIOSPI_WaitClockPhase(pGpioSpiStruct) == FALSE);            
                
                /*lq �ṩʱ����2��ָʾ���豸������ */ 
                if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
                {
                    GPIOSPI1_SCLK_LOW();
                }
                else
                {
                    GPIOSPI1_SCLK_HIGH();        
                }

                /*lq ��ȡ�������ݣ����豸������ */
                returnData <<= 1;
                returnData |= GPIOSPI1_MISO_IN_READ();
                                                                               
                while (GPIOSPI_WaitClockPeriod(pGpioSpiStruct) == FALSE);                            

                /*lq ����������λ */
                if (pGpioSpiStruct->SPI_FirstBit == GPIOSPI_FirstBit_MSB)
                {
                    data <<= 1; 
                }
                else
                {
                    data >>= 1;             
                }            
            }         
        }

        /*lq Ƭѡʧ�� */
        GPIOSPI1_CS_HIGH();        
    }

    return returnData;
}
#endif
/*******************************************************************************
* ����  : GPIOSPI_ResetClockPeriod
* ����  : ��λʱ�Ӽ���
* ����  : spiTye - SPI����
          data - ��������      			  
* ���  : ��
* ����  : ��������
*******************************************************************************/
U16 GPIOSPI_InstSendData(GpioSpiTypeEnum spiTye, U16 data)
{
    if (spiTye == GPIOSPI1)
    {
        return GPIOSPI_SendData(&GpioSpiInstance1, data);
    }
}