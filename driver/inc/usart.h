/*******************************************************************************
  Copyright (C), 2010, 712.Co.,Ltd.
  File name:	usart.h
  Author: ��ΰ      Version: V1.00       Date: 2011.09.23 
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
#ifndef __USART_H__
#define __USART_H__

#include "def.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//�������
#define COMM0		(0)
#define COMM1		(1)
#define COMM2		(2)
//����״̬
#define COMM_NO_ERR		(0)
#define COMM_RX_EMPTY	(1)
#define COMM_TX_EMPTY	(2)
#define COMM_RX_FULL	(3)
#define COMM_TX_FULL	(4)
#define COMM_RX_TIMEOUT	(5)
#define COMM_TX_TIMEOUT	(6)
#define COMM_ERR		(7)
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define UART_TX_FIFO_DEPTH		(1024)			//COMM�����С

#define UART_RX_FIFO_DEPTH		(8)
#define UART_RX_BUFF_SIZE		(128)
/* Private define ------------------------------------------------------------*/
struct UartTxFIFOStruct
{
	U8 TxFIFO[UART_TX_FIFO_DEPTH];	 //���ͻ���
	U16 TxFIFOInput;
	U16 TxFIFOOutput;
	
	U8  TxState;	//FALSE:�����У�TRUE��æ
};

struct UartRxFIFOStruct
{
	U8  RxFIFO[UART_RX_FIFO_DEPTH][UART_RX_BUFF_SIZE];	//���ܻ���              //lq RxFIFO[��������][�����ڲ��ַ���]
	U16 RxFIFOInput;//����ʱʹ��                                                //lq �б�
	U16 RxFIFOOutput;//ָ���б�
	U8  RxEnFlg;		//TRUE���пռ䣻FALSE����
	U16 RxBuffIndex; //ָ���б�                                                 //lq �б�
	U8  Rx0x0dFlg;	
    U8  Rx0xb5Flg;	
	U8  DoubleFlg;
};

typedef struct UsartConfig
{
    U32 Baud;
    U8  Data;
    U8  Parity;
    U8  Stop;    
}UsartConfigStruct;

/* Public  function  ---------------------------------------------------------*/
extern UsartConfigStruct Uart1Config;

U8 InitCommStruc(U8 Comm, U32 band); 
U8 UartTxDataBuf(U8 COMM, U8 *pStr,U16 Length);
S16 TXbufIsEmpty(U8 COMM);
void Usart1Interrupt(void);
extern void InitUsart1(U32 baud);
extern void UartRxFIFOStructInit(struct UartRxFIFOStruct *pUartRxFIFO);
extern void UartTxMainProcess(struct UartTxFIFOStruct *pUartTxFIFO);
extern void UartRxProcess(struct UartRxFIFOStruct *pUartRxFIFOStruct, U8 data);
extern void UartTxIntProcess(struct UartTxFIFOStruct *pUartTxFIFO);
extern S16 GetCOMMMess(U8 COMM);

// lnw  add
extern void USART1_UsartConfigStructInit(U32 Baud, U8 Data, U8 Parity, U8 Stop);
extern void UartTxFIFOStructInit(struct UartTxFIFOStruct *putCell);


#endif	//__USART_H__




