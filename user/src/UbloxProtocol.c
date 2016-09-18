/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: UbloxProtocol.c
  Author: lq       Version : V1.00        Date: 2014.01.13
  Description:     ublox protocol
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "def.h"
#include "UbloxProtocol.h"
#include "usart.h"
#include "config.h"

/*lq ģ��ʵ������*/
static UBXPrtCommStruct UBXPrtCommPara;

/*lq ��Ϣ����*/
/*lq continuous mode */
const U8 UBX_PRT_MSG_CFG_RXM_CONTINUOUS[8] = {0xB5, 0x62, 0x06, 0x11, 0x02, 0x00, 0x08, 0x00};

/*lq Power Save Mode */
const U8 UBX_PRT_MSG_CFG_RXM_PSM[8] = {0xB5, 0x62, 0x06, 0x11, 0x02, 0x00, 0x08, 0x01};

/*lq PSM-cyclic operation-normal,update period=1s,search period=10s acquisition timeout=0s,on time=0s */
const U8 UBX_PRT_MSG_CFG_PM2_NORM[50] = {0xb5, 0x62, 0x06, 0x3b, 0x2c, 0x00, 0x01, 0x06,
                                         0x00, 0x00, 0x00, 0x90, 0x02, 0x00, 0xe8, 0x03,
                                         0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00,
                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x01,
                                         0x00, 0x00, 0x4f, 0xc1, 0x03, 0x00, 0x86, 0x02,
                                         0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x64, 0x40,
                                         0x01, 0x00};
/*lq PSM-cyclic operation-normal,update period=10s,search period=10s acquisition timeout=0s,on time=0s */
const U8 UBX_PRT_MSG_CFG_PM2_SAVE[50] = {0xb5, 0x62, 0x06, 0x3b, 0x2c, 0x00, 0x01, 0x06,
                                         0x00, 0x00, 0x00, 0x90, 0x02, 0x00, 0x10, 0x27,//lq 0x10, 0x27 (little endian)��ʾupdateperiod=10000ms
                                         0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00,
                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x01,
                                         0x00, 0x00, 0x4f, 0xc1, 0x03, 0x00, 0x86, 0x02,
                                         0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x64, 0x40,
                                         0x01, 0x00};
/*lq �ָ�Ĭ�����ò�����*/
const U8 UBX_PRT_MSG_CFG_CFG_DEFAULT[19] = {0xb5, 0x62, 0x06, 0x09, 0x0d, 0x00, 0xff, 0xff,
                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
                                            0x00, 0x00, 0x03};
/*lq �ָ�Ĭ������*/
const U8 UBX_PRT_MSG_CFG_CFG_CLEAR[19] = {0xb5, 0x62, 0x06, 0x09, 0x0d, 0x00, 0xff, 0xff,
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x00, 0x03};
/*lq ���浱ǰ����*/
const U8 UBX_PRT_MSG_CFG_CFG_SAVE[19] = {0xb5, 0x62, 0x06, 0x09, 0x0d, 0x00, 0x00, 0x00,
                                         0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
                                         0x00, 0x00, 0x03};
/*lq ������������*/
const U8 UBX_PRT_MSG_CFG_CFG_LOAD[19] = {0xb5, 0x62, 0x06, 0x09, 0x0d, 0x00, 0x00, 0x00,
                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
                                         0x00, 0x00, 0x03};

/*lq ������ʹ��*/
const U8 UBX_PRT_MSG_CFG_TP5_ACTIVE[38] = {0xb5, 0x62, 0x06, 0x31, 0x20, 0x00, 0x00, 0x00,
                                           0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x40, 0x42,
                                           0x0f, 0x00, 0x40, 0x42, 0x0f, 0x00, 0x00, 0x00,
                                           0x00, 0x00, 0xa0, 0x86, 0x01, 0x00, 0x00, 0x00,
                                           0x00, 0x00, 0xf7, 0x00, 0x00, 0x00};
/*lq ������ʧ��*/
const U8 UBX_PRT_MSG_CFG_TP5_INACTIVE[38] = {0xb5, 0x62, 0x06, 0x31, 0x20, 0x00, 0x00, 0x00,
                                             0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x40, 0x42,
                                             0x0f, 0x00, 0x40, 0x42, 0x0f, 0x00, 0x00, 0x00,
                                             0x00, 0x00, 0xa0, 0x86, 0x01, 0x00, 0x00, 0x00,
                                             0x00, 0x00, 0xf6, 0x00, 0x00, 0x00};
 
/*lq NEMA�������ʧ�ܣ�rate=0 */
const U8 UBX_PRT_MSG_CFG_MSG_R0_GXVTG[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x05,
                                             0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R0_GXGGA[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x00,
                                             0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R0_GXGSA[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x02,
                                             0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R0_GXGSV[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x03,
                                             0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R0_GXGLL[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x01,
                                             0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R0_GXRMC[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x04,
                                             0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R0_GXGBS[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x09,
                                             0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
/*lq NEMA�������ʹ�ܣ�rate=1 */
const U8 UBX_PRT_MSG_CFG_MSG_R1_GXVTG[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x05,
                                             0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R1_GXGGA[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x00,
                                             0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R1_GXGSA[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x02,
                                             0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R1_GXGSV[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x03,
                                             0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R1_GXGLL[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x01,
                                             0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R1_GXRMC[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x04,
                                             0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R1_GXGBS[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x09,
                                             0x01, 0x01, 0x01, 0x01, 0x01, 0x01};

/*******************************************************************************
* ����  : UBX_UBXPrtCommStructInit
* ����  : UBXPrtCommStruct�ṹ���ʼ��
* ����  : pUBXPrtCommStruct-UBXPrtCommStruct����ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void UBX_UBXPrtCommStructInit(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    U16 i, j;

    pUBXPrtCommStruct->TimePeriod = UBX_PRT_TIME_WAKEUP_WAIT;
    pUBXPrtCommStruct->TimeCnt = 0;

    for (i = 0; i < UBX_PRT_MSG_BUFFER_LEN; i++)
    {
        for (j = 0; j < UBX_PRT_MSG_BUFFER_DEPTH; j++)
        {
            pUBXPrtCommStruct->MsgTxBuffer[j][i] = 0;
        }
        pUBXPrtCommStruct->MsgTx[i] = 0;             
    }

    pUBXPrtCommStruct->CtrlState = UBX_PRT_COMM_CTRL_STAT_IDLE;
    pUBXPrtCommStruct->SendRequireFlag = FALSE;
    pUBXPrtCommStruct->SendFlag = FALSE;
    pUBXPrtCommStruct->MsgClass = UBX_PRT_MSG_CLASS_CFG;
    pUBXPrtCommStruct->MsgID = UBX_PRT_MSG_ID_CFG_CFG;
    pUBXPrtCommStruct->RespFlag = FALSE;
    pUBXPrtCommStruct->AckFlag = FALSE;
    pUBXPrtCommStruct->SendCnt = 0;

    UartRxFIFOStructInit(&(pUBXPrtCommStruct->Uart2RxFIFO2));
}

/*******************************************************************************
* ����  : UBX_UBXPrtCommInstanceInit
* ����  : UBXPrtCommStruct�ṹ��ʵ����ʼ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void UBX1_UBXPrtCommInstanceInit(void)
{
    UBX_UBXPrtCommStructInit(&UBXPrtCommPara);
}

/*******************************************************************************
* ����  : UBX_UBXPrtCommStructReset
* ����  : UBXPrtCommStruct�ṹ�帴λ
* ����  : pUBXPrtCommStruct-UBXPrtCommStruct����ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void UBX_UBXPrtCommStructReset(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    U16 i, j;

    pUBXPrtCommStruct->TimePeriod = UBX_PRT_TIME_WAKEUP_WAIT;
    pUBXPrtCommStruct->TimeCnt = 0;

    for (i = 0; i < UBX_PRT_MSG_BUFFER_LEN; i++)
    {
        for (j = 0; j < UBX_PRT_MSG_BUFFER_DEPTH; j++)
        {
            pUBXPrtCommStruct->MsgTxBuffer[j][i] = 0;
        }
        pUBXPrtCommStruct->MsgTx[i] = 0;             
    }

    pUBXPrtCommStruct->CtrlState = UBX_PRT_COMM_CTRL_STAT_IDLE;
    pUBXPrtCommStruct->SendRequireFlag = FALSE;
    pUBXPrtCommStruct->SendFlag = FALSE;
    pUBXPrtCommStruct->MsgClass = UBX_PRT_MSG_CLASS_CFG;
    pUBXPrtCommStruct->MsgID = UBX_PRT_MSG_ID_CFG_CFG;
    pUBXPrtCommStruct->RespFlag = FALSE;
    pUBXPrtCommStruct->AckFlag = FALSE;
    pUBXPrtCommStruct->SendCnt = 0;

    UartRxFIFOStructInit(&(pUBXPrtCommStruct->Uart2RxFIFO2));
}

/*******************************************************************************
* ����  : UBX1_UBXPrtCommInstanceReset
* ����  : UBXPrtCommStruct�ṹ��ʵ��1��λ
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void UBX1_UBXPrtCommInstanceReset(void)
{
    UBX_UBXPrtCommStructReset(&UBXPrtCommPara);
}

/*******************************************************************************
* ����  : UBX_UBXPrtMsgSendBuff
* ����  : ���淢�ͱ�����Ϣ
* ����  : pUBXPrtCommStruct-UBXPrtCommStruct����ָ��
          pMsg-Ҫ�������Ϣ		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static U8 UBX_UBXPrtMsgSendBuff(UBXPrtCommStruct *pUBXPrtCommStruct, const U8 *pMsg, U16 Len)
{
    U16 i;

    /*lq ����������� */
    if ((pUBXPrtCommStruct->TxFIFOIn + 1) % UBX_PRT_MSG_BUFFER_DEPTH
         == pUBXPrtCommStruct->TxFIFOOut)
    {
        return FALSE;  
    }
    else
    {
        pUBXPrtCommStruct->MsgTxBuffer[pUBXPrtCommStruct->TxFIFOIn][0] = (U8)Len;    
        for (i = 0; i < Len; i++)
        {
           pUBXPrtCommStruct->MsgTxBuffer[pUBXPrtCommStruct->TxFIFOIn][i + 1] = pMsg[i];
        }
        
        pUBXPrtCommStruct->TxFIFOIn++;
        pUBXPrtCommStruct->TxFIFOIn %= UBX_PRT_MSG_BUFFER_DEPTH;
        
        return TRUE;            
    }
}

/*******************************************************************************
* ����  : UBX_UBXPrtMsgSendFetch
* ����  : ��ȡһ�����ͱ�����Ϣ
* ����  : pUBXPrtCommStruct-UBXPrtCommStruct����ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static U8 UBX_UBXPrtMsgSendFetch(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    U16 i;
    U16 Len;

    /*lq ��Ϣ����Ϊ�� */
    if (pUBXPrtCommStruct->TxFIFOIn == pUBXPrtCommStruct->TxFIFOOut)
    {
        return FALSE;    
    } 
    else
    {
        Len = pUBXPrtCommStruct->MsgTxBuffer[pUBXPrtCommStruct->TxFIFOOut][0];
        for (i = 0; i <= Len; i++)
        {
            pUBXPrtCommStruct->MsgTx[i] = pUBXPrtCommStruct->MsgTxBuffer[pUBXPrtCommStruct->TxFIFOOut][i];
        }
        pUBXPrtCommStruct->MsgClass = pUBXPrtCommStruct->MsgTx[3];
        pUBXPrtCommStruct->MsgID = pUBXPrtCommStruct->MsgTx[4];

        pUBXPrtCommStruct->TxFIFOOut++;
        pUBXPrtCommStruct->TxFIFOOut %= UBX_PRT_MSG_BUFFER_DEPTH;

        return TRUE;    
    }
}

/*******************************************************************************
* ����  : UBX_UBXPrtMsgSendBuff
* ����  : ���淢�ͱ�����Ϣ
* ����  : pMsg-Ҫ�������Ϣ		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void UBX1_UBXPrtMsgSendBuff(U8 *pMsg, U16 Len)
{
    UBX_UBXPrtMsgSendBuff(&UBXPrtCommPara, pMsg, Len);
}

/*******************************************************************************
* ����  : UBX_SetSendFlag
* ����  : ���÷��ͱ�־
* ����  : pUBXPrtCommStruct-UBXPrtCommStruct����ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void UBX_SetSendFlag(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    pUBXPrtCommStruct->SendFlag = TRUE;
}

/*******************************************************************************
* ����  : UBX1_SetSendFlag
* ����  : ���÷��ͱ�־
* ����  : pUBXPrtCommStruct-UBXPrtCommStruct����ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void UBX1_SetSendFlag(void)
{
    UBX_SetSendFlag(&UBXPrtCommPara);
}

/*******************************************************************************
* ����  : UBX_ResetSendFlag
* ����  : ��λ���ͱ�־
* ����  : pUBXPrtCommStruct-UBXPrtCommStruct����ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void UBX_ResetSendFlag(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    pUBXPrtCommStruct->SendFlag = FALSE;
}

/*******************************************************************************
* ����  : UBX1_ResetSendFlag
* ����  : ��λ���ͱ�־
* ����  : pUBXPrtCommStruct-UBXPrtCommStruct����ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void UBX1_ResetSendFlag(void)
{
    UBX_ResetSendFlag(&UBXPrtCommPara);
}

/*******************************************************************************
* ���� ��UBX_UartTxUbxMsg
* ���� ��GPS���ڷ�����Ϣ
* ���� ��pStr����Ϣ����ָ��
		 Len����Ϣ�ֽڳ���
* ��� ����
* ���� ����
*******************************************************************************/
U8 UBX_UartTxUbxMsg(U8 *pStr, U16 Len)
{				
	UartTxDataBuf(COMM2, pStr, Len);

	return TRUE;		
}

/*******************************************************************************
* ���� ��UBX_UartMsgCheckSum
* ���� ��UBLOX protocol����У����
* ���� ��pStr���ַ���ָ��
		 Length���ַ�������
* ��� ����
* ���� ��Check_Sum - У����
*******************************************************************************/
U16 UBX_UartMsgCheckSum(char *pStr, U16 Length)
{
    U8 CK_A, CK_B;
    U16 i;
    U16 Check_Sum;

    CK_A = 0;
    CK_B = 0;
    Check_Sum = 0;

    for (i = 0; i < Length; i++)
    {
        CK_A = CK_A + pStr[i];
        CK_B = CK_B + CK_A;
    }

    Check_Sum = CK_A;
    Check_Sum = (Check_Sum << 8) | CK_B;

    return Check_Sum;
}

/*******************************************************************************
* ����  : UBX_UartResponseUbxMsg
* ����  : UBX Protocol��Ϣ���
* ����  : pData���ַ���ָ��
* ���  : ��
* ����  : FALSE��ʧ�ܣ�TRUE���ɹ�
*******************************************************************************/
U8 UBX_UartResponseUbxMsg(U8 *pData, U16 Len)
{
	U16 i, Index = 0;
	U16 Check = 0;
	char UartDebugBuff[100];
	
	if(Len > 90)
	{
		return FALSE;
	}
	
	Index = 0;
	for (i = 0; i < Len; i++)
	{
		UartDebugBuff[Index++] = pData[i];
	}
	
	if(Index > 2)
	{
		Check = UBX_UartMsgCheckSum((U8 *)&UartDebugBuff[2], Index - 2);
	}

	UartDebugBuff[Index++] = Check >> 8;    //lq Check_A
	UartDebugBuff[Index++] = Check & 0xFF;  //lq Check_B

	UBX_UartTxUbxMsg(UartDebugBuff, Index);
	
	return TRUE;
}

/*******************************************************************************
* ����  : UBX_UBXPrtCommCtrlProcess
* ����  : ʹ��UBX Protocol ��GPSͨ�ſ���
* ����  : pUBXPrtCommStruct-UBXPrtCommStruct����ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void UBX_UBXPrtCommCtrlProcess(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    U8 TempBuff[30];

    switch(pUBXPrtCommStruct->CtrlState)
    {
		case UBX_PRT_COMM_CTRL_STAT_IDLE :
			{
			/*lq ��UBLOX��ϢҪ����*/
				if (pUBXPrtCommStruct->SendRequireFlag == TRUE)
				{
					/*lq ��ȡһ����Ϣ */            
					if (UBX_UBXPrtMsgSendFetch(pUBXPrtCommStruct) == TRUE)
					{
						/*lq ת��GPS����״̬ */
						pUBXPrtCommStruct->CtrlState = UBX_PRT_COMM_CTRL_STAT_WAKEUP;
					}
					else
					{
						/*lq �����������Ϣ�ѷ������ */
						pUBXPrtCommStruct->SendRequireFlag = FALSE;            
					}
				}
				break;
			}
		case UBX_PRT_COMM_CTRL_STAT_WAKEUP:
			{
				/*lq ����GPS�������� */
				TempBuff[0] = UBX_PRT_DUMMY_SEQUENCE;
				UBX_UartTxUbxMsg(TempBuff, 1);
        
				/*lq ���ѵȴ���ʱ */        
				pUBXPrtCommStruct->TimePeriod = UBX_PRT_TIME_WAKEUP_WAIT;
				pUBXPrtCommStruct->TimeCnt = SetTimePeriodProcess(pUBXPrtCommStruct->TimePeriod);
    
				/*lq ת����Ϣ����״̬ */
				pUBXPrtCommStruct->CtrlState = UBX_PRT_COMM_CTRL_STAT_SEND;
				break;    
			}
		case UBX_PRT_COMM_CTRL_STAT_SEND:
		{
			/*lq GPS���ջ����Ѷ�ʱ��ʱ��*/
			if (pUBXPrtCommStruct->TimeCnt == GetLocalClockTickProcess())
			{
				/*lq ������Ϣ */
				UBX_UartResponseUbxMsg(&(pUBXPrtCommStruct->MsgTx[1]), pUBXPrtCommStruct->MsgTx[0]);
            
				/*lq ������Ϣ���ʹ�����1 */
				pUBXPrtCommStruct->SendCnt++;
            
				/*lq ��λ��Ϣ���ͱ�־ */
				pUBXPrtCommStruct->RespFlag = FALSE;
				pUBXPrtCommStruct->AckFlag = FALSE;
				pUBXPrtCommStruct->SendFlag = TRUE;

				/*lq ��Ӧ�ȴ���ʱ */
				pUBXPrtCommStruct->TimePeriod = UBX_PRT_TIME_RESPONSE_WAIT;
				pUBXPrtCommStruct->TimeCnt = SetTimePeriodProcess(pUBXPrtCommStruct->TimePeriod);

				/*lq ת����Ӧ�ȴ�״̬ */
				pUBXPrtCommStruct->CtrlState = UBX_PRT_COMM_CTRL_STAT_RESPONSE_WAIT;
			}   
			break;		
		}
		case UBX_PRT_COMM_CTRL_STAT_RESPONSE_WAIT:
		{
			/*lq GPS���ջ���Ϣ��Ӧ��ʱ����Ϣ��Ӧ�ɹ� */
			if (pUBXPrtCommStruct->TimeCnt == GetLocalClockTickProcess()
				|| pUBXPrtCommStruct->RespFlag == TRUE)
			{
				/*lq ��Ϣȷ�ϳɹ� */
				if (pUBXPrtCommStruct->AckFlag == TRUE)
				{
					/*lq ��λ��Ϣȷ�ϱ�־ */
					pUBXPrtCommStruct->RespFlag = FALSE;
					pUBXPrtCommStruct->AckFlag = FALSE;                
					pUBXPrtCommStruct->SendFlag = FALSE;
                
					/*lq ������Ϣ���ʹ�����0 */
					pUBXPrtCommStruct->SendCnt = 0;                                  
                
					/*lq ת�����״̬ */
					pUBXPrtCommStruct->CtrlState = UBX_PRT_COMM_CTRL_STAT_IDLE;    
				}
				else
				{
					/*lq ��λ��Ϣ���ͱ�־ */
					pUBXPrtCommStruct->RespFlag = FALSE;
					pUBXPrtCommStruct->AckFlag = FALSE;                
					pUBXPrtCommStruct->SendFlag = FALSE;            //lq ������Ӧ��ʱ�������ڴ˸�λ���ͱ�־

					debug_printf("UBX Msg %02X %02X repeat : %d\r\n", 
								pUBXPrtCommStruct->MsgClass, pUBXPrtCommStruct->MsgID, pUBXPrtCommStruct->SendCnt);

					/*lq δ�ﵽ���ʹ�����ֵ */
					if (pUBXPrtCommStruct->SendCnt < UBX_PRT_SINGLE_MSG_SEND_CNT_THRES)
					{
						/*lq ת�뻽��״̬ */
						pUBXPrtCommStruct->CtrlState = UBX_PRT_COMM_CTRL_STAT_WAKEUP;            
					}
					else
					{
						/*lq ������Ϣ���ʹ�����0 */
						pUBXPrtCommStruct->SendCnt = 0;                                  
	
						/*lq ת�����״̬ */
						pUBXPrtCommStruct->CtrlState = UBX_PRT_COMM_CTRL_STAT_IDLE; //lq �������Դ�����㲻�ٳ���    
					}            
				}
			} 
			break;			
		}
		default:
		{
			/*lq ת�����״̬ */        
			pUBXPrtCommStruct->CtrlState = UBX_PRT_COMM_CTRL_STAT_IDLE;    
		}
	}		
}

/*******************************************************************************
* ����  : UBX1_UBXPrtCommCtrlProcess
* ����  : ʹ��UBX Protocol ��GPSͨ�ſ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void UBX1_UBXPrtCommCtrlProcess(void)
{
    UBX_UBXPrtCommCtrlProcess(&UBXPrtCommPara);
}

/*******************************************************************************
* ���� ��UBX_UartRxProcess
* ���� �����ڽ��յ��ֽڣ�UBX protocol��
* ���� ��pUartRxFIFOStruct�����ڽ��սṹ��ָ��
		 data������
* ��� ����
* ���� ����
*******************************************************************************/
static void UBX_MsgRxProcess(struct UartRxFIFOStruct *pUartRxFIFOStruct, U8 data)
{	
    U16 Len;

	switch(data)
	{
        case '\xB5':                                                            //lq uBX protocol������ʼ�ַ�
			pUartRxFIFOStruct->Rx0xb5Flg = TRUE;
            break;

        case '\x62':                                                            //lq �����������ʼ�ַ�
            if (pUartRxFIFOStruct->Rx0xb5Flg == TRUE)
            {
                pUartRxFIFOStruct->Rx0xb5Flg = FALSE;                

                pUartRxFIFOStruct->RxBuffIndex = 1;	                            // ���ܽṹ���б�,0���������ܳ��ȣ���ͬ���ַ�\xB5\x62��
    			if (((pUartRxFIFOStruct->RxFIFOInput+1) & (UART_RX_FIFO_DEPTH-1)) // �жϽ��ܽṹ�������ǿ�
    					!= pUartRxFIFOStruct->RxFIFOOutput)                     //lq ���ջ������ṹ�廹�пռ�
    			{
    				pUartRxFIFOStruct->RxEnFlg = TRUE;
    			}
    			else                                                            //lq ���ջ������ṹ����
    			{
    				pUartRxFIFOStruct->RxEnFlg = FALSE;				
    			}			
            }
            else
            {
				pUartRxFIFOStruct->RxEnFlg = FALSE;            
            }
			break;
#if 0
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
#endif
		default:
			if(pUartRxFIFOStruct->Rx0xb5Flg == TRUE)                            //lq ���ĸ�ʽ��Ч��'\xB5'��δ��������'\x62'��    
			{
				pUartRxFIFOStruct->Rx0xb5Flg = FALSE;
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
                else if (pUartRxFIFOStruct->RxBuffIndex >= 5)
                {
                    /*lq ��ȡpayload���� */
                    Len = (pUartRxFIFOStruct->RxFIFO[pUartRxFIFOStruct->RxFIFOInput][4] << 8) 
                          | (pUartRxFIFOStruct->RxFIFO[pUartRxFIFOStruct->RxFIFOInput][3]); //lq Э����С��ģʽ�泤��
                    
                    /*lq ���ݽ������ */
                    if (pUartRxFIFOStruct->RxBuffIndex >= (Len + 7))    //lq (7=1�ܳ���+1Msgclass+1MsgID+2payloadLen+2checksum)                        
                    {
    					pUartRxFIFOStruct->RxEnFlg = FALSE;
    					
    					pUartRxFIFOStruct->RxFIFO[pUartRxFIFOStruct->RxFIFOInput++][0]  //lq RxFIFO[ ][0]��ű��ĵĳ��� 
                            = pUartRxFIFOStruct->RxBuffIndex - 1;	    //���浱ǰ�������ݳ��ȣ�������ͬ���ַ�\xB5\x62��
    					if (pUartRxFIFOStruct->RxFIFOInput >= UART_RX_FIFO_DEPTH)
    					{
    						pUartRxFIFOStruct->RxFIFOInput = 0;
    					}                    
                    }
                }
			}
			break;	
	}
}

/*******************************************************************************
* ���� ��UBX_UartRxProcess
* ���� �����ڽ��յ��ֽڣ�UBX protocol��
* ���� ��COMM���˿ں�
		 data������
* ��� ����
* ���� ����
*******************************************************************************/
void UBX_UartRxProcess(U8 COMM, U8 data)
{
    if (COMM == COMM2)
	{
        UBX_MsgRxProcess(&UBXPrtCommPara.Uart2RxFIFO2, data);
	}    
}

/*******************************************************************************
* ���� ��UBX_UbxMsgProcess
* ���� ������UbxMsg
* ���� ��COMM�����ڽ��սṹ��ָ��
* ��� ����
* ���� ����
*******************************************************************************/
static S16 UBX_MsgDealProcess(UBXPrtCommStruct *pUBXPrtCommStruct, U8 *pDat, U16 uiLen)
{
    /*lq ����Ӧ��ϢΪACK����Ϣ*/
    if (pDat[0] == UBX_PRT_MSG_CLASS_ACK)
    {
        /*lq ���͹�������Ϣ���� */
        if (pUBXPrtCommStruct->SendFlag == TRUE)
        {
            /*lq ��Ӧ��Ϣ�뷢����Ϣ��Ӧ */
            if (pDat[4] == pUBXPrtCommStruct->MsgClass
                && pDat[5] == pUBXPrtCommStruct->MsgID)
            {
                /*lq ���óɹ� */
                if (pDat[1] == UBX_PRT_MSG_ID_ACK_ACK)
                {
                    pUBXPrtCommStruct->AckFlag = TRUE;

                    debug_printf("UBX Msg %02X %02X : ACK\r\n", pDat[4], pDat[5]);
                }
                else
                {
                    pUBXPrtCommStruct->AckFlag = FALSE;

                    debug_printf("UBX Msg %02X %02X : NAK\r\n", pDat[4], pDat[5]);
                }
                
                pUBXPrtCommStruct->RespFlag = TRUE;
                pUBXPrtCommStruct->SendFlag = FALSE;                                  
            }    
        } 
    }              
}

/*******************************************************************************
* ���� ��UBX_GetUbxMess
* ���� ����ȡUBX Protocol��Ϣ
* ���� ��pUBXPrtCommStruct��UBXPrtCommStruct����ָ��
* ��� ����
* ���� ����
*******************************************************************************/
static U16 UBX_GetReceivedUbxMess(UBXPrtCommStruct *pUBXPrtCommStruct)
{
	U16 CheckSum;
	U16 uiLength;
	U16 Len;
	struct UartRxFIFOStruct *pUartRxFIFO;
	
	uiLength = 0;
    pUartRxFIFO = &(pUBXPrtCommStruct->Uart2RxFIFO2);
	
    if (pUartRxFIFO->RxFIFOOutput != pUartRxFIFO->RxFIFOInput)
	{
		Len = pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][0];
		if (Len > 2)    //lq (2=2checksum)
		{
			/*lq ����ʵ�ʽ������ݲ��ֵ�У���� */
            CheckSum = UBX_UartMsgCheckSum(&pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][1], Len - 2);
						
			if (((CheckSum >> 8) == pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][Len - 1])
				&& ((CheckSum & 0xFF) == pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][Len]))
			{
				uiLength = Len - 2; //lq (2=2checksum)
				
				/*lq UBX Protocol���Ľ������� */
                UBX_MsgDealProcess(pUBXPrtCommStruct, &pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][1], uiLength);	
			}
		}
        
        /*lq ���� */				
		pUartRxFIFO->RxFIFOOutput++;
		if (pUartRxFIFO->RxFIFOOutput >= UART_RX_FIFO_DEPTH)
		{
			pUartRxFIFO->RxFIFOOutput = 0;
		}
	}
	
	return uiLength;
}

/*******************************************************************************
* ���� ��UBX_GetCOMMMess
* ���� ����ȡ����UBX Protocol��Ϣ
* ���� ��pUBXPrtCommStruct��UBXPrtCommStruct����ָ��
* ��� ����
* ���� ����
*******************************************************************************/
U16 UBX_GetCOMMMess(U8 COMM)
{
	U16 uiLength;

    uiLength = 0;

    if (COMM == COMM2)
	{
        uiLength = UBX_GetReceivedUbxMess(&UBXPrtCommPara);
	}

    return uiLength;	

}

/*******************************************************************************
* ���� ��UBX_GpsStartupConfig
* ���� ��ʹ��UBX protocol ��Ϣ����GPS�����Զ�����������ģʽ
* ���� ��pUBXPrtCommStruct��UBXPrtCommStruct����ָ��
* ��� ����
* ���� ����
*******************************************************************************/
static void UBX_GpsStartupConfig(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    /*lq �ָ�Ĭ�����ò�����*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_CFG_DEFAULT, UBX_PRT_MSG_LEN_CFG_CFG);

    /*lq ����GPS����ģʽΪcontinuous mode����ֹ���ù�������Ϣ��ʧ*/
    //lq UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_RXM_CONTINUOUS, UBX_PRT_MSG_LEN_CFG_RXM);

    /*lq ����GPS���ջ�ֻ���GxRMC��GxGGA��GxGSA��GxGBS���� */
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R0_GXVTG, UBX_PRT_MSG_LEN_CFG_MSG);
    //lq UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R0_GXGGA, UBX_PRT_MSG_LEN_CFG_MSG);
    //lq UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R0_GXGSA, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R0_GXGSV, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R0_GXGLL, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R1_GXGBS, UBX_PRT_MSG_LEN_CFG_MSG);

    /*lq ����GPS cyclic operation�Ĳ���Ϊ������������*/
    //lq UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_PM2_NORM, UBX_PRT_MSG_LEN_CFG_PM2);

    /*lq ���浱ǰ���ã���ֹ����GPSΪPSM����ģʽ��GPSһ������inactive״̬�������������������������ʧ*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_CFG_SAVE, UBX_PRT_MSG_LEN_CFG_CFG);

    /*lq ����GPSΪPSM����ģʽ */
    //lq UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_RXM_PSM, UBX_PRT_MSG_LEN_CFG_RXM);

    /*lq ���浱ǰ����*/
    //lq UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_CFG_SAVE, UBX_PRT_MSG_LEN_CFG_CFG);

    /*lq ����UBX��Ϣ�ķ������� */
    pUBXPrtCommStruct->SendRequireFlag = TRUE;
}

/*******************************************************************************
* ���� ��UBX1_GpsStartupConfig
* ���� ��ʹ��UBX protocol ��Ϣ����GPS�����Զ�����������ģʽ
* ���� ��pUBXPrtCommStruct��UBXPrtCommStruct����ָ��
* ��� ����
* ���� ����
*******************************************************************************/
void UBX1_GpsStartupConfig(void)
{
    UBX_GpsStartupConfig(&UBXPrtCommPara);
}

/*******************************************************************************
* ���� ��UBX_GpsIdleConfig
* ���� ��ʹ��UBX protocol ��Ϣ����GPS�����Զ���ʡ�繤��ģʽ
* ���� ��pUBXPrtCommStruct��UBXPrtCommStruct����ָ��
* ��� ����
* ���� ����
*******************************************************************************/
static void UBX_GpsIdleConfig(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    /*lq �ڽ���GPSʡ������֮ǰ������������Ϊ��Ч*/
    ResetGpsPulseEn();

    /*lq ����GPS����ģʽΪcontinuous mode����ֹ���ù�������Ϣ��ʧ*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_RXM_CONTINUOUS, UBX_PRT_MSG_LEN_CFG_RXM);

    /*lq ����PPSʧ�ܣ��ȹ�PPS�����GxRMC */
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_TP5_INACTIVE, UBX_PRT_MSG_LEN_CFG_TP5);

    /*lq ����NMEA-GxRMC���ʧ��*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R0_GXRMC, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R0_GXGGA, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R0_GXGSA, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R0_GXGBS, UBX_PRT_MSG_LEN_CFG_MSG);

    /*lq ����GPS cyclic operation�Ĳ���Ϊʡ��ģʽ����*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_PM2_SAVE, UBX_PRT_MSG_LEN_CFG_PM2);

    /*lq ���浱ǰ���ã���ֹ����GPSΪPSM����ģʽ��GPSһ������inactive״̬�������������������������ʧ*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_CFG_SAVE, UBX_PRT_MSG_LEN_CFG_CFG);

    /*lq ����GPSΪPSM����ģʽ */
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_RXM_PSM, UBX_PRT_MSG_LEN_CFG_RXM);

    /*lq ���浱ǰ����*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_CFG_SAVE, UBX_PRT_MSG_LEN_CFG_CFG);

    /*lq ����UBX��Ϣ�ķ������� */
    pUBXPrtCommStruct->SendRequireFlag = TRUE;

}

/*******************************************************************************
* ���� ��UBX1_GpsIdleConfig
* ���� ��ʹ��UBX protocol ��Ϣ����GPS�����Զ���ʡ�繤��ģʽ
* ���� ��pUBXPrtCommStruct��UBXPrtCommStruct����ָ��
* ��� ����
* ���� ����
*******************************************************************************/
void UBX1_GpsIdleConfig(void)
{
    UBX_GpsIdleConfig(&UBXPrtCommPara);
}

/*******************************************************************************
* ���� ��UBX_GpsNormalConfig
* ���� ��ʹ��UBX protocol ��Ϣ����GPS�����Զ�����������ģʽ
* ���� ��pUBXPrtCommStruct��UBXPrtCommStruct����ָ��
* ��� ����
* ���� ����
*******************************************************************************/
static void UBX_GpsNormalConfig(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    /*lq ����GPS����ģʽΪcontinuous mode����ֹ���ù�������Ϣ��ʧ*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_RXM_CONTINUOUS, UBX_PRT_MSG_LEN_CFG_RXM);

    /*lq ����GPS cyclic operation�Ĳ���Ϊ������������*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_PM2_NORM, UBX_PRT_MSG_LEN_CFG_PM2);

    /*lq ����NMEA-GxRMC���ʹ�ܣ��ȿ�GxRMC����PPS*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R1_GXRMC, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R1_GXGGA, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R1_GXGSA, UBX_PRT_MSG_LEN_CFG_MSG);    
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R1_GXGBS, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R1_GXVTG, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R1_GXGSV, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R1_GXGLL, UBX_PRT_MSG_LEN_CFG_MSG);

    /*lq ����PPSʹ�� */
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_TP5_ACTIVE, UBX_PRT_MSG_LEN_CFG_TP5);
#if 0
    /*lq ���浱ǰ���ã���ֹ����GPSΪPSM����ģʽ��GPSһ������inactive״̬�������������������������ʧ*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_CFG_SAVE, UBX_PRT_MSG_LEN_CFG_CFG);

    /*lq ����GPSΪPSM����ģʽ */
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_RXM_PSM, UBX_PRT_MSG_LEN_CFG_RXM);
#endif
    /*lq ���浱ǰ����*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_CFG_SAVE, UBX_PRT_MSG_LEN_CFG_CFG);

    /*lq ����UBX��Ϣ�ķ������� */
    pUBXPrtCommStruct->SendRequireFlag = TRUE;

}

/*******************************************************************************
* ���� ��UBX1_GpsNormalConfig
* ���� ��ʹ��UBX protocol ��Ϣ����GPS�����Զ�����������ģʽ
* ���� ��pUBXPrtCommStruct��UBXPrtCommStruct����ָ��
* ��� ����
* ���� ����
*******************************************************************************/
void UBX1_GpsNormalConfig(void)
{
    UBX_GpsNormalConfig(&UBXPrtCommPara);
}

/*******************************************************************************
* ���� ��UBX_GetSendRequireFlag
* ���� ����ѯ���������־
* ���� ��pUBXPrtCommStruct��UBXPrtCommStruct����ָ��
* ��� ����
* ���� ����
*******************************************************************************/
static U8 UBX_GetSendRequireFlag(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    return pUBXPrtCommStruct->SendRequireFlag;    
}

/*******************************************************************************
* ���� ��UBX1_GetSendRequireFlag
* ���� ����ѯ���������־
* ���� ����
* ��� ����
* ���� ����
*******************************************************************************/
U8 UBX1_GetSendRequireFlag(void)
{
    return UBX_GetSendRequireFlag(&UBXPrtCommPara);    
}

/*******************************************************************************
* ���� ��UBX_GetGpsCommSleepSta
* ���� ����ѯGPSͨ��ģ����������
* ���� ��pUBXPrtCommStruct��UBXPrtCommStruct����ָ��
* ��� ����
* ���� ����
*******************************************************************************/
static U8 UBX_GetGpsCommSleepSta(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    if (pUBXPrtCommStruct->SendRequireFlag == FALSE)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ���� ��UBX1_GetGpsCommSleepSta
* ���� ����ѯGPSͨ��ģ����������
* ���� ����
* ��� ����
* ���� ����
*******************************************************************************/
U8 UBX1_GetGpsCommSleepSta(void)
{
    return UBX_GetGpsCommSleepSta(&UBXPrtCommPara);    
}

/*******************************************************************************
* ����  : UBX_ModuleInit
* ����  : ģ���ʼ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void UBX_ModuleInit(void) 
{
    UBX_UBXPrtCommStructInit(&UBXPrtCommPara);
    UBX_GpsNormalConfig(&UBXPrtCommPara);
}

