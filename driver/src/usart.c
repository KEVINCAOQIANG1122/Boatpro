/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: Usart.c
  Author: vozhj       Version : V1.00        Date: 2011.09.23
  Description:    ���ڵײ�������Ӧ�ô������
  Version:         V1.00
  Function List:   // ��Ҫ�������书��    	 
			1����������InitUart0
    	   	   �������ܣ���ʼ������0
 
			2����������Uart0Interrupt
    	   	   �������ܣ�����0
    	   	 
    	   	3����������UartTxMainProcess
    	   	   �������ܣ����ڷ��ͳ���
    	   	   
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "stm32f10x.h"
#include "def.h"
#include "usart.h"
#include "common.h"
#include "gps.h"
#include "uartmsg.h"
#include "gpsctrl.h"
#include "UbloxProtocol.h"  

/* Private variables ---------------------------------------------------------*/
struct UartTxFIFOStruct Uart1TxFIFO;
struct UartRxFIFOStruct Uart1RxFIFO;

UsartConfigStruct Uart1Config;

/* Private  function  ---------------------------------------------------------*/
void InitUsart1(U32 baud);
void UartTxIntProcess(struct UartTxFIFOStruct *pUartTxFIFO);
void UartRxProcess(struct UartRxFIFOStruct *pUartRxFIFOStruct, U8 data);

/*******************************************************************************
* ����  : UsartConfigStructInit
* ����  : �ṹ���ʼ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void UsartConfigStructInit(UsartConfigStruct *pUsartConfigStruct, U32 Baud, U8 Data, U8 Parity, U8 Stop)
{
    pUsartConfigStruct->Baud = Baud;
    pUsartConfigStruct->Data = Data;
    pUsartConfigStruct->Parity = Parity;
    pUsartConfigStruct->Stop = Stop;
}

/*******************************************************************************
* ����  : USART1_UsartConfigStructInit
* ����  : USART1�ṹ���ʼ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void USART1_UsartConfigStructInit(U32 Baud, U8 Data, U8 Parity, U8 Stop)
{
    UsartConfigStructInit(&Uart1Config, Baud, Data, Parity, Stop);
}

/*******************************************************************************
* ����  : Usart1Interrupt
* ����  : ����1�жϷ������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void Usart1Interrupt(void)	 //�����жϺ���                                     //lq ���ͻ�������һ���Ĵ������գ��������Է��ͣ�д���ݣ� or   
{                                                                               //lq ���ջ�������һ���Ĵ������ǿգ��������Խ��գ������ݣ�
	U8 Data;
	
	//�������������������Ҫ�ȶ�SR���ٶ�DR�Ĵ�������������������жϵ�����
	if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET)
	{
	 	USART_ClearFlag(USART1,USART_FLAG_ORE);			// ��SR��ʵ����������־
		USART_ReceiveData(USART1);						// ��DR
	}                                                                    //lq ʱ�������ж�

	//�����ж�   �ж�����һ���ж�
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)                       //lq �ɽ��գ��ɶ����ж�
	{
		
		/* Read one byte from the receive data register */
		Data = USART_ReceiveData(USART1);
		
		UartRxProcess(&Uart1RxFIFO, Data);
//		USART_SendData(USART1, Data);
        
        /*lq MCU��������ʱ�����ڻ���MCU*/
        SetUsartFlg();	
	}
		
	//�����ж�
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)                        //lq �ɷ��ͣ���д���ж�
	{   
		UartTxIntProcess(&Uart1TxFIFO);
	}
}

/*******************************************************************************
* ����  : InitUart1
* ����  : ��ʼ������1
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitUsart1(U32 baud) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART1 configuration ------------------------------------------------------*/
	/* USART1 configured as follow:
		- BaudRate = 38400 baud  
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USART1 */
  	USART_Init(USART1, &USART_InitStructure);

	/* Enable USART1 Receive and Transmit interrupts */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //ʹ�ܴ����ж�               //lq ���ջ������ǿ��ж�ʹ�ܣ����Խ�������
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);                              //lq ���ͻ��������ж�ʧ�ܣ����ɷ�������
	

	/* Enable the USART1 */
  	USART_Cmd(USART1, ENABLE);
	//InitCommStruc(COMM1, 38400);           
}

/*******************************************************************************
* ����  : Usart1_UsartConfig
* ����  : ����Usart1
* ����  : Baud - ������
          Data - ����λλ��
          Parity - ��żУ�鷽ʽ
          Stop - ֹͣλλ��
* ���  : ��
* ����  : FALSE��ʧ�ܣ�TRUE���ɹ�
*******************************************************************************/
U8 Usart1_UsartConfig(U32 Baud, U8 Data, U8 Parity, U8 Stop)
{
	USART_InitTypeDef USART_InitStructure;
    U16 WordLength;
    U16 StopBits;
    U16 ParitySelect;

	/* USART1 configuration ------------------------------------------------------*/
	/* USART1 configured as follow:
		- BaudRate = 38400 baud  
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/

    if (Data == 8)
    {
        WordLength = USART_WordLength_8b;    
    }
    else if (Data == 9)
    {
        WordLength = USART_WordLength_9b;        
    }
    else
    {
        return FALSE;    
    }

    if (Parity == 0)
    {
        ParitySelect = USART_Parity_No;
    }
    else if (Parity == 1)
    {
        ParitySelect = USART_Parity_Even;    
    }
    else if (Parity == 2)
    {
        ParitySelect = USART_Parity_Odd;        
    }
    else
    {
        return FALSE;    
    }

    if (Stop == 1)
    {
        StopBits = USART_StopBits_1;    
    }
    else if (Stop == 2)
    {
        StopBits = USART_StopBits_2;        
    }
    else if (Stop == 3)
    {
        StopBits = USART_StopBits_0_5;        
    }
    else if (Stop == 4)
    {
        StopBits = USART_StopBits_1_5;        
    }
    else
    {
        return FALSE;    
    }

	USART_InitStructure.USART_BaudRate = Baud;
	USART_InitStructure.USART_WordLength = WordLength;
	USART_InitStructure.USART_StopBits = StopBits;
	USART_InitStructure.USART_Parity = ParitySelect;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USART1 */
  	USART_Init(USART1, &USART_InitStructure);

	/* Enable USART1 Receive and Transmit interrupts */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);	

	/* Enable the USART1 */
  	USART_Cmd(USART1, ENABLE);  

    return TRUE;
}

/*******************************************************************************
* ����  : UartTxFIFOStructInit
* ����  : �ṹ���ʼ��
* ����  : putCell-UartTxFIFOStruct����ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void UartTxFIFOStructInit(struct UartTxFIFOStruct *putCell)
{
	putCell->TxFIFOInput = 0;
	putCell->TxFIFOOutput = 0;
	putCell->TxState = FALSE;
}

/*******************************************************************************
* ����  : UartRxFIFOStructInit
* ����  : �ṹ���ʼ��
* ����  : pUartRxFIFO-UartRxFIFOStruct����ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void UartRxFIFOStructInit(struct UartRxFIFOStruct *pUartRxFIFO)
{
	pUartRxFIFO->RxFIFOInput = 0;
	pUartRxFIFO->RxFIFOOutput = 0;
	pUartRxFIFO->RxEnFlg = FALSE;
	pUartRxFIFO->RxBuffIndex = 0;
	pUartRxFIFO->Rx0x0dFlg = FALSE;
    pUartRxFIFO->Rx0xb5Flg = FALSE;	
	pUartRxFIFO->DoubleFlg = FALSE;
}

/*********************************************************************************************************
;** ��������: InitCommStruc
;** ��������: ��ʼ�����ڷ���ṹ��
;** ��    ��: Comm : Comm��ַ
;** �� �� ֵ: ��
;********************************************************************************************************/
U8 InitCommStruc(U8 Comm, U32 band) 
{
	struct UartTxFIFOStruct *putCell;
	struct UartRxFIFOStruct *pUartRxFIFO;
	
	if(Comm == COMM1)
	{
		UartTxFIFOStructInit(&Uart1TxFIFO);
        UartRxFIFOStructInit(&Uart1RxFIFO);
        InitUsart1(band);
        USART1_UsartConfigStructInit(band, 8, 0, 1);
	}
	else if(Comm == COMM2)
	{
		UartTxFIFOStructInit(&Uart2TxFIFO);
        UartRxFIFOStructInit(&Uart2RxFIFO);
		InitUsart2(band);
	}
	else
	{
		return(COMM_ERR);
	}

	return(COMM_NO_ERR);
}

/*******************************************************************************
* ���� ��UartTxIntProcess
* ���� �����ڷ��͹��̣��ɴ��ڷ����жϵ���
* ���� ��pUartTxFIFO�����ڷ��ͽṹ��ָ��
			&Uart0TxFIFO��
			&Uart1TxFIFO��
			&Uart2TxFIFO��
* ��� ����
* ���� ����
*******************************************************************************/
void UartTxIntProcess(struct UartTxFIFOStruct *pUartTxFIFO)
{
	pUartTxFIFO->TxFIFOOutput++;
	pUartTxFIFO->TxFIFOOutput &= UART_TX_FIFO_DEPTH - 1;//ָ��ص�ͷ            //lq ����ָ��ѭ����ģUART_TX_FIFO_DEPTH - 1ȡ�ࣩ
		
	if(pUartTxFIFO->TxFIFOOutput != pUartTxFIFO->TxFIFOInput)             //lq �����������ݽṹ����Ϊ�գ�������Ҫ����
	{	
		pUartTxFIFO->TxState = TRUE;	//��������������	

		if (pUartTxFIFO == &Uart1TxFIFO)//����Ǵ���1
		{
			USART_SendData(USART1, pUartTxFIFO->TxFIFO[pUartTxFIFO->TxFIFOOutput]); //lq ��USART1��TDRд����			
		}
		if(pUartTxFIFO == &Uart2TxFIFO)
		{
			USART_SendData(USART2, pUartTxFIFO->TxFIFO[pUartTxFIFO->TxFIFOOutput]);
		}
													
	}
	else                                                                        //lq �����������ݽṹ��������
	{
		if (pUartTxFIFO == &Uart1TxFIFO)//����Ǵ���1
	  	{
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);                      //lq ���ͻ�������һ���Ĵ��������ж�ʧ�ܣ�
			pUartTxFIFO->TxState  = FALSE;	//����                              //lq ����Ƶ�����ж����ѣ���ʱ���ɺ���UartTxMainProcess()��
		}                                                                       //lq ״̬��־pUartTxFIFO->TxStateѭ�������������Ҫ����
		if (pUartTxFIFO == &Uart2TxFIFO)//����Ǵ���1
		{
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
			pUartTxFIFO->TxState  = FALSE;	//����
		}
	  	
	}
}

/*******************************************************************************
* ���� ��UartTxMainProcess
* ���� �����ڷ�������
* ���� ��pUartTxFIFO�����ڷ��ͽṹ��ָ��
			&Uart0TxFIFO��
			&Uart1TxFIFO��
			&Uart2TxFIFO��
* ��� ����
* ���� ����
*******************************************************************************/
void UartTxMainProcess(struct UartTxFIFOStruct *pUartTxFIFO)                    //��TXE�ж�ʧ�ܵ�����£��ж���������Ҫ��
{
	if(pUartTxFIFO->TxState == FALSE)	//����                                  //lq ���ڷ��͵�ǰ��
	{
		if(pUartTxFIFO->TxFIFOInput != pUartTxFIFO->TxFIFOOutput)               //lq �����ݿɷ�
		{
			pUartTxFIFO->TxState = TRUE;	//æ                                //lq ��ǰ����������
			
			if (pUartTxFIFO == &Uart1TxFIFO)
			{
				USART_SendData(USART1, pUartTxFIFO->TxFIFO[pUartTxFIFO->TxFIFOOutput]);	
				USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 
			}
			else if (pUartTxFIFO == &Uart2TxFIFO)
			{
				USART_SendData(USART2, pUartTxFIFO->TxFIFO[pUartTxFIFO->TxFIFOOutput]);
				USART_ITConfig(USART2, USART_IT_TXE, ENABLE); 
			}            							
		}
	}
}

/*******************************************************************************
* ���� ��UartTxString
* ���� �����ڷ����ַ���
* ���� ��COMM�����ڵ�ַ��־
			COMM1��
		 pStr���ַ���ָ��
		 Length���ַ�������
* ��� ����
* ���� ����
*******************************************************************************/
U8 UartTxDataBuf(U8 COMM, U8 *pStr,U16 Length)
{				
	U16 i, lastcnt;
	U8 uchRet;
	struct UartTxFIFOStruct *pUartTxFIFO;
	
	if(COMM == COMM1)
	{
		pUartTxFIFO = &Uart1TxFIFO;
	}
	else if ((COMM == COMM2))
	{
		pUartTxFIFO = &Uart2TxFIFO;
	}
	
	lastcnt = (UART_TX_FIFO_DEPTH - 1) 
		- ((pUartTxFIFO->TxFIFOInput - pUartTxFIFO->TxFIFOOutput)               //lq ʣ��ռ䣨�ֽڣ��� 
		& (UART_TX_FIFO_DEPTH - 1));                                            //lq?
	if (lastcnt >= Length)                                                      //lq ʣ��ռ乻
	{
		for (i = 0; i < Length; i++)
		{			
			pUartTxFIFO->TxFIFO[pUartTxFIFO->TxFIFOInput++] = pStr[i];
			
			pUartTxFIFO->TxFIFOInput &= (UART_TX_FIFO_DEPTH - 1);		
		}	
		
		uchRet = TRUE;		
	}                                                                           
	else                                                                        //lq ʣ��ռ䲻��
	{
		pUartTxFIFO->TxFIFO[pUartTxFIFO->TxFIFOInput++] = 0xFF;                 //lq?
		pUartTxFIFO->TxFIFOInput &= (UART_TX_FIFO_DEPTH - 1);		
		uchRet = FALSE;
	}
	
	UartTxMainProcess(pUartTxFIFO);

	return uchRet;	
}

#if 0
/*******************************************************************************
* ���� ��UartRxProcess
* ���� �����ڽ��յ��ֽ�
* ���� ��pUartRxFIFOStruct�����ڽ��սṹ��ָ��
		 data������
* ��� ����
* ���� ����
*******************************************************************************/
void UartRxProcess(struct UartRxFIFOStruct *pUartRxFIFOStruct, U8 data)
{	
	switch(data)
	{
		case '$':                                                               //lq �����������ʼ�ַ�
			pUartRxFIFOStruct->RxBuffIndex = 1;	//���ܽṹ���б�
			if (((pUartRxFIFOStruct->RxFIFOInput+1) & (UART_RX_FIFO_DEPTH-1)) // �жϽ��ܽṹ�������ǿ�
					!= pUartRxFIFOStruct->RxFIFOOutput)                         //lq ���ջ������ṹ�廹�пռ�
			{
				pUartRxFIFOStruct->RxEnFlg = TRUE;
			}
			else                                                                //lq ���ջ������ṹ����
			{
				pUartRxFIFOStruct->RxEnFlg = FALSE;				
			}			
			pUartRxFIFOStruct->Rx0x0dFlg = FALSE;  //�յ�0x0d��־
			break;
		case '\r':		//0x0d
			pUartRxFIFOStruct->Rx0x0dFlg = TRUE;
			break;
		case '\n':		//0x0a					  //�������
			if(pUartRxFIFOStruct->Rx0x0dFlg == TRUE)
			{
				pUartRxFIFOStruct->Rx0x0dFlg = FALSE;

				if (pUartRxFIFOStruct->RxEnFlg == TRUE)
				{
					pUartRxFIFOStruct->RxEnFlg = FALSE;
					
					pUartRxFIFOStruct->RxFIFO[pUartRxFIFOStruct->RxFIFOInput++][0]  //lq RxFIFO[ ][0]��ű��ĵĳ��� 
								= pUartRxFIFOStruct->RxBuffIndex - 1;	//���浱ǰ�������ݳ���
					if (pUartRxFIFOStruct->RxFIFOInput >= UART_RX_FIFO_DEPTH)
					{
						pUartRxFIFOStruct->RxFIFOInput = 0;
					}
				}
			}
			break;
		default:
			if(pUartRxFIFOStruct->Rx0x0dFlg == TRUE)                            //lq ���ĸ�ʽ��Ч��'\r'��δ��������'\n'��    
			{
				pUartRxFIFOStruct->Rx0x0dFlg = FALSE;
				pUartRxFIFOStruct->RxEnFlg = FALSE;
			}
			
			if (pUartRxFIFOStruct->RxEnFlg == TRUE)
			{
				pUartRxFIFOStruct->RxFIFO[pUartRxFIFOStruct->RxFIFOInput][pUartRxFIFOStruct->RxBuffIndex++] = data;
   				if (pUartRxFIFOStruct->RxBuffIndex >= UART_RX_BUFF_SIZE)        //lq ���ڱ��Ĺ������ñ�����Ч���ᱻ����ı��ĸ��ǣ�
				{
					pUartRxFIFOStruct->RxBuffIndex = 0;
					pUartRxFIFOStruct->RxEnFlg = FALSE;
				}
			}
			break;	
	}
}
#else
/*******************************************************************************
* ���� ��UartRxProcess
* ���� �����ڽ��յ��ֽ�
* ���� ��pUartRxFIFOStruct�����ڽ��սṹ��ָ��
		 data������
* ��� ����
* ���� ����
*******************************************************************************/
void UartRxProcess(struct UartRxFIFOStruct *pUartRxFIFOStruct, U8 data)
{	
	switch(data)
	{
        case '!':                                                               //lq ֧�����!--ABM
        case '$':                                                               //lq �����������ʼ�ַ�
			pUartRxFIFOStruct->RxBuffIndex = 1;	//���ܽṹ���б�
			if (((pUartRxFIFOStruct->RxFIFOInput+1) & (UART_RX_FIFO_DEPTH-1)) // �жϽ��ܽṹ�������ǿ�
					!= pUartRxFIFOStruct->RxFIFOOutput)                         //lq ���ջ������ṹ�廹�пռ�
			{
				pUartRxFIFOStruct->RxEnFlg = TRUE;
			}
			else                                                                //lq ���ջ������ṹ����
			{
				pUartRxFIFOStruct->RxEnFlg = FALSE;				
			}			
			pUartRxFIFOStruct->Rx0x0dFlg = FALSE;  //�յ�0x0d��־
			break;
		case '\r':		//0x0d
			pUartRxFIFOStruct->Rx0x0dFlg = TRUE;
			break;
		case '\n':		//0x0a					  //�������
			if(pUartRxFIFOStruct->Rx0x0dFlg == TRUE)
			{
				pUartRxFIFOStruct->Rx0x0dFlg = FALSE;

				if (pUartRxFIFOStruct->RxEnFlg == TRUE)
				{
					pUartRxFIFOStruct->RxEnFlg = FALSE;
					
					pUartRxFIFOStruct->RxFIFO[pUartRxFIFOStruct->RxFIFOInput++][0]  //lq RxFIFO[ ][0]��ű��ĵĳ��� 
								= pUartRxFIFOStruct->RxBuffIndex - 1;	//���浱ǰ�������ݳ���
					if (pUartRxFIFOStruct->RxFIFOInput >= UART_RX_FIFO_DEPTH)
					{
						pUartRxFIFOStruct->RxFIFOInput = 0;
					}
				}
			}
			else																//lq [2013-01-27]'\n'֮ǰδ����'\r',���ĸ�ʽ��Ч
			{
				pUartRxFIFOStruct->RxEnFlg = FALSE;
			}
			break;
		default:
			if(pUartRxFIFOStruct->Rx0x0dFlg == TRUE)                            //lq ���ĸ�ʽ��Ч��'\r'��δ��������'\n'��    
			{
				pUartRxFIFOStruct->Rx0x0dFlg = FALSE;
				pUartRxFIFOStruct->RxEnFlg = FALSE;
			}
			
			if (pUartRxFIFOStruct->RxEnFlg == TRUE)
			{
				pUartRxFIFOStruct->RxFIFO[pUartRxFIFOStruct->RxFIFOInput][pUartRxFIFOStruct->RxBuffIndex++] = data;
   				if (pUartRxFIFOStruct->RxBuffIndex >= UART_RX_BUFF_SIZE)        //lq ���ڱ��Ĺ������ñ�����Ч���ᱻ����ı��ĸ��ǣ�
				{
					pUartRxFIFOStruct->RxBuffIndex = 0;
					pUartRxFIFOStruct->RxEnFlg = FALSE;
				}
			}
			break;	
	}
}
#endif

/*******************************************************************************
* ���� ��TXbufIsEmpty
* ���� �����仺���Ƿ�Ϊ��
* ���� ��COMM�����ڽ��սṹ��ָ��
* ��� ����
* ���� ����
*******************************************************************************/
S16 TXbufIsEmpty(U8 COMM)
{
	struct UartTxFIFOStruct *pUartTxFIFO;
	
	if(COMM == COMM1)
	{
		pUartTxFIFO = &Uart1TxFIFO;
	}
	else
	{
		return FALSE;
	}
	
	if(pUartTxFIFO->TxFIFOInput == pUartTxFIFO->TxFIFOOutput)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*******************************************************************************
* ���� ��GetCOMMMess
* ���� ����ȡ������Ϣ
* ���� ��COMM�����ڽ��սṹ��ָ��
* ��� ����
* ���� ����
*******************************************************************************/
S16 GetCOMMMess(U8 COMM)
{
	U8 uchCrc;
	U16 uiLength;
	U8 AscHig, AscLow;
	U16 Len;
	struct UartRxFIFOStruct *pUartRxFIFO;
	
	uiLength = 0;

	if(COMM == COMM1)
	{
		pUartRxFIFO = &Uart1RxFIFO;
	}
	else if (COMM == COMM2)
	{
		pUartRxFIFO = &Uart2RxFIFO;
	}
	else
	{
		return uiLength;	
	}
	
	if(pUartRxFIFO->RxFIFOOutput != pUartRxFIFO->RxFIFOInput)
	{
		//CRCУ����ȷ
		Len = pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][0];

		if (Len > 3)
		{
			uchCrc = UartMsgCheckSum(&pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][1], Len - 3);
			AscHig = HexToAscII(uchCrc >> 4);
			AscLow = HexToAscII(uchCrc & 0xf);
						
			if ((AscHig == pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][Len - 1])
				&& (AscLow == pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][Len]))
			{
				uiLength = Len - 3;
				
				//�˴����봮�ڱ��Ľ�������
				if (pUartRxFIFO == &Uart1RxFIFO)
				{
					UartMsgProcess(&pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][1], uiLength);	
				}
				else if (pUartRxFIFO == &Uart2RxFIFO)//����2��GPS
				{
					GpsMsgProcess(&pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][1], uiLength);
				}					
			}
		}
				
		pUartRxFIFO->RxFIFOOutput++;
		if (pUartRxFIFO->RxFIFOOutput >= UART_RX_FIFO_DEPTH)
		{
			pUartRxFIFO->RxFIFOOutput = 0;
		}
	}
	
	return uiLength;
}



