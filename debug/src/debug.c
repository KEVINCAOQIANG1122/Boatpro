/*******************************************************************************
  Copyright (C),  2011,  712.Co., Ltd.
  FileName: debug.c
  Author: ��ΰ       Version : V1.00        Date: 2011.09.23
  Description:    ͨ���������debug��Ϣ	
  Version:         V1.00
  Function List:   // ��Ҫ�������书��    	 
			1����������UartDebugMsg
    	   	   �������ܣ����ڵ��Ա������	   	    	   	     	   	
    	   	   
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <locale.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <stdarg.h>

#include "def.h"
#include "debug.h"
#include "usart.h"
#include "common.h"
#include "usart.h"
#include "config.h"

U8 DebugOutputFlag = FALSE;             //lq Ĭ��ֵΪFALSE����Ĭ�ϲ����
//U8 DebugOutputFlag = TRUE;              //lq Ĭ��ֵΪTRUE����Ĭ�����

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
U8 UartTxString(char *pStr)
{				
	//U8 uchRet;

//	DCB_write(comm1_handle, 0, 0, 0, (U8*)pStr, strlen(pStr));
	UartTxDataBuf(COMM1, (U8*)pStr, strlen(pStr));
#if CONFIG_SD_CARD_MODULE_EN
	WriteFile((U8*)pStr,strlen(pStr));
#endif
	return TRUE;		
}


/*******************************************************************************
* ���� ��usart_printf
* ���� �����ڷ����ַ���
* ���� ��COMM�����ڵ�ַ��־
			COMM0��
			COMM1��
			COMM2��
		 format��print��ʽ
* ��� ����
* ���� ����
*******************************************************************************/
void usart_printf(char * format,...)
{
	va_list ap;
	static char string[256];

	va_start(ap, format);
	//sprintf(string, format, ap);
	vsprintf(string, format, ap);
	UartTxString(string);
	va_end(ap);
}

/*******************************************************************************
* ���� ��debug_printf
* ���� �����ڷ��͵����ַ���
* ���� ��format��print��ʽ
* ��� ����
* ���� ����
*******************************************************************************/
void debug_printf(char * format,...)
{
#if CONFIG_DEBUG_PRINT_EN
	va_list ap;
	static char string[256];

    /*lq �˴����Ը��ñ���������� */
    //lq...
    if (DebugOutputFlag == TRUE)
    {
    	va_start(ap, format);
    	//sprintf(string, format, ap);
    	vsprintf(string, format, ap);
    	UartTxString(string);
    	va_end(ap);
    }
#endif
}

/*******************************************************************************
* ����  : UartDebugMsg
* ����  : ���ڵ��Ա������
* ����  : Type������
			MMI_DEBUG_MSG
			PTL_DEBUG_MSG
			ARM_DEBUG_MSG
			DSP_DEBUG_MSG
			PD_DEBUG_MSG			
		  Addr��0x00~0x30
		  pData���ַ���ָ��
* ���  : ��
* ����  : FALSE��ʧ�ܣ�TRUE���ɹ�
*******************************************************************************/
U8 UartDebugMsg(U8 Type,  U8 Addr,  char * format,...)
{
	va_list ap;
	static char string[256];
	U16 Index;
	U8 Check;

//	OS_ENTER_CRITICAL();
//	if(wsWorkState.DebugInfoEnable == FALSE)
//	{
//		OS_EXIT_CRITICAL();
//		return FALSE;
//	}
//	OS_EXIT_CRITICAL();

	switch(Type)
	{
		case DSP_DEBUG_MSG:
			Addr += 0x00;
			break;
		case PTL_DEBUG_MSG:
			Addr += 0x30;
			break;
		case MMI_DEBUG_MSG:
			Addr += 0x60;
			break;
		case ARM_DEBUG_MSG:
			Addr += 0x90;
			break;
		case PD_DEBUG_MSG:
			Addr += 0xC0;
			break;
		default:
			break;
		
	}	
	if(Type < 10)
	{
		sprintf(string, "$T712,TXT,%.2X,,", Addr);
		Index = strlen(string);
		va_start(ap,format);
		//sprintf(string,format,ap);
		vsprintf(&string[Index],format,ap);
		
		Check = UartMsgCheckSum((U8 *)&string[1],  strlen(string) - 1);	//��$��*��֮���������У��
		sprintf(&string[strlen(string)], "*%.2X\r\n", Check);
	}
	else
	{
		va_start(ap,format);
		vsprintf(&string[0],format,ap);
		sprintf(&string[strlen(string)], "\r\n");
	}
	
	UartTxString(string);
	va_end(ap);
	
	return TRUE;

}

/*******************************************************************************
* ����  : UartResponseMsg
* ����  : ����Ӧ�������
* ����  : pData���ַ���ָ��
* ���  : ��
* ����  : FALSE��ʧ�ܣ�TRUE���ɹ�
*******************************************************************************/
U8 UartResponseMsg(char *pData)	  //����ַ���ר��
{
	U16 i, Index = 0;
	U16 Len;
	U8 Check = 0;
	char UartDebugBuff[100];
		
	Len = strlen(pData);

	if(Len > 90)
	{
		return FALSE;
	}
	
	for (i=0; i<Len; i++)
	{
		UartDebugBuff[Index++] = pData[i];
	}
	
	if(Index <= 1)
	{
		Check = UartMsgCheckSum((U8 *)&UartDebugBuff[1], Index);//��$��*��֮���������У��
	}
	else
	{
		Check = UartMsgCheckSum((U8 *)&UartDebugBuff[1], Index - 1);//��$��*��֮���������У��
	}

	UartDebugBuff[Index++] = '*';
	UartDebugBuff[Index++] = HexToAscII(Check >> 4);
	UartDebugBuff[Index++] = HexToAscII(Check & 0xf);
	UartDebugBuff[Index++] = '\r';
	UartDebugBuff[Index++] = '\n';
	UartDebugBuff[Index++] = '\0';

	UartTxString(UartDebugBuff);
	
	return TRUE;
}
