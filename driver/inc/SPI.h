/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: SPI.h
  Author: lq       Version : V1.00        Date: 2014.05.09
  Description:     SPI����
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _SPI_H
#define _SPI_H

/* Include ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/*lq SPI���Ŷ��� */
#define SPI2_PORT       PUBLIC_PORT
#define SPI2_NSS_PIN    PUBLIC_PIN
#define SPI2_SCK_PIN    PUBLIC_PIN
#define SPI2_MISO_PIN   PUBLIC_PIN
#define SPI2_MOSI_PIN   PUBLIC_PIN

#define SPI2_NSS_HIGH() GPIO_SetBits(SPI2_PORT, SPI2_NSS_PIN)
#define SPI2_NSS_LOW()  GPIO_ResetBits(SPI2_PORT, SPI2_NSS_PIN)
#if 0
/*lq SPI�ӿ�ID */
#define SPI_INTERFACE_2         2

/*lq �ṹ����غ궨��*/
#define SPI_TX_FIFO_DEPTH       (1024)
#define SPI_RX_FIFO_DEPTH		(8)
#define SPI_RX_BUFF_SIZE		(128)

/*lq SPI����ģʽ*/
#define SPI_MODE_2LINES_FULLDUPLEX      0
#define SPI_MODE_2LINES_RXONLY          1
#endif

#define SPI_DUMMY_BYTE  0x00

/*lq GPIOģ��SPI �ӿڶ��� */
#define GPIO_SPI1_SCLK_HIGH()           SI446X_A_SCLK_HIGH()
#define GPIO_SPI1_SCLK_LOW()            SI446X_A_SCLK_LOW()
#define GPIO_SPI1_SDI_HIGH()            SI446X_A_SDI_HIGH()
#define GPIO_SPI1_SDI_LOW()             SI446X_A_SDI_LOW()
#define GPIO_SPI1_SDO_IN()              SI446X_A_SDO_IN()

#define GPIO_SPI2_SCLK_HIGH()           SI446X_B_SCLK_HIGH()
#define GPIO_SPI2_SCLK_LOW()            SI446X_B_SCLK_LOW()
#define GPIO_SPI2_SDI_HIGH()            SI446X_B_SDI_HIGH()
#define GPIO_SPI2_SDI_LOW()             SI446X_B_SDI_LOW()
#define GPIO_SPI2_SDO_IN()              SI446X_B_SDO_IN()

/* Private typedef -----------------------------------------------------------*/
#if 0
typedef struct SPITxFIFOStruct
{
	U8 TxFIFO[SPI_TX_FIFO_DEPTH];
	U16 TxFIFOInput;
	U16 TxFIFOOutput;
	
	U8  TxState;	                                        //lq FALSE:�����У�TRUE��æ
}SPITxFIFOStruct;

typedef struct SPIRxFIFOStruct
{
	U8  RxFIFO[SPI_RX_FIFO_DEPTH][SPI_RX_BUFF_SIZE];        //lq RxFIFO[��������][�����ڲ��ַ���]
	U16 RxFIFOInput;                                        //lq �б�
	U16 RxFIFOOutput;                                       
	U8  RxEnFlg;		                                    
	U16 RxBuffIndex;                                        //lq �б�
	U8  Rx0x0dFlg;	
    U8  Rx0xb5Flg;	
	U8  DoubleFlg;
}SPIRxFIFOStruct;
#endif

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/

/* Public  function  ---------------------------------------------------------*/
extern void SPI2_Init(void);
extern U8 GPIO_SPI2_ExchangeByte(U8 sendData);
extern U8 GPIO_SPI1_ExchangeByte(U8 sendData);

#endif
