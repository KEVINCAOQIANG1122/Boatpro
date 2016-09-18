/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: uartmsg.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    ���Ĵ���ģ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include <locale.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <stdarg.h>
#include "config.h"
#include "def.h"
#include "initial.h"
#include "uartmsg.h"
#include "common.h"
#include "testing.h"
#if !WIN32
#include "debug.h"
#include "parastorage.h"
#include "powerup.h"
#endif
#include "gpsctrl.h"
#include "digunit.h"
#include "mmi.h"
#include "powerdown.h"
#include "LinkPtlPort.h"
#include "sleepctrl.h"
#include "Tim.h"
#include "24c02.h"
#include "ledctrl.h"
#include "DAC.h"
#include "tempersensor.h"
#include "PhysicalLayer.h"
#include "RxTxCtrl.h"
#include "Include.h"
#include "filectrl.h"
#include "flashctrl.h"

/*******************************************************************************
* ����  : IniUartMsgPara
* ����  : ��ʼ������ͨ�ű�Ҫ�Ĳ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void IniUartMsgPara(void)
{	

}

/*******************************************************************************
* ����  : GetString
* ����  : ��ȡ�������ֶ�
* ����  : pInput : �����ַ���
		  SegmentIndex: ��ȡ�ֶ�n����һ������֮���Ϊ��һ�ֶ�
		  SumLength : �ַ����ܳ���
* ���  : Output : ����ַ���
* ����  : ��
*******************************************************************************/
U16 GetString(char *pInput, char *Output, U16 SegmentIndex, U16 SumLength, U16 Max_Bytes)
{
	U16 i;
	U16 uiIndex;
	U16 uiTempSegment;
	U8 ucSta;
	
	uiIndex = 0;
	uiTempSegment = 0;
	ucSta = 0;

	for (i = 0; i < SumLength; i++)
	{
		//״̬0����������ֶ�
		if (ucSta == 0)
		{
			if (pInput[i] == ',')
			{
				 uiTempSegment++;
	
				 //���ҵ�������ȡ�Ķ�
				 if (uiTempSegment == SegmentIndex)
				 {
				 	ucSta = 1;
				 }
				 else if (uiTempSegment > SegmentIndex)
				 {
					return (0);	
				 }
			}
			//��*λ���Բ��������������˳�
			else if (pInput[i] == '*')
			{
				return (0);	
			}
		}
		//״̬1��������ֶ�����ȡ����
		else if (ucSta == 1)
		{
			if (pInput[i] == ',' || pInput[i] == '*')
			{
				break;
			}
			else
			{
				Output[uiIndex++] = pInput[i];

				if (uiIndex >= Max_Bytes)
				{
					return (0);
				}
			}		
		}	
	}

	return (uiIndex);
}

/*******************************************************************************
* ����  : IsAscStringOk
* ����  : �ж�ASC�ַ���0��9֮��
* ����  : ��
* ���  : ��
* ����  : TRUE����ȷ��FALSE������
*******************************************************************************/
U8 IsAscStringOk(char *pInput)
{
	U8 i, size;
	
	size = (U8)strlen(pInput);

	for (i = 0; i < size; i++)
	{
		if ((pInput[i] < '0') || (pInput[i] > '9'))
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

/*******************************************************************************
* ����  : IsHexStringOk
* ����  : �ж�ASC�ַ���0��F֮��
* ����  : ��
* ���  : ��
* ����  : TRUE����ȷ��FALSE������
*******************************************************************************/
U8 IsHexStringOk(char *pInput)
{
	U8 i, size;
	
	size = (U8)strlen(pInput);

	for (i = 0; i < size; i++)
	{
		if (!(((pInput[i] >= '0') && (pInput[i] <= '9'))
            || ((pInput[i] >= 'A') && (pInput[i] <= 'F')) 
            || ((pInput[i] >= 'a') && (pInput[i] <= 'f'))))
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

/*******************************************************************************
* ����  : IsVersionString
* ����  : �ж�ASC�ַ�Ϊ���ֺ�С����
* ����  : ��
* ���  : ��
* ����  : TRUE����ȷ��FALSE������
*******************************************************************************/
U8 IsVersionString(char *pInput)
{
	U8 i, size;
	
	size = (U8)strlen(pInput);

	for (i = 0; i < size; i++)
	{
		if (!(((pInput[i] >= '0') && (pInput[i] <= '9'))
            || ((pInput[i] == '.'))))
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

/*******************************************************************************
* ����  : ConvertBcdToByte
* ����  : BCD��ת��Ϊ�ֽ�
* ����  : pInput-BCD�루��HEX�ַ����ַ���
* ���  : pOutput-�ϲ�����ֽ��ַ���
* ����  : TRUE����ȷ��FALSE������
*******************************************************************************/
U8 ConvertBcdToByte(char *pInput, U8 *pOutput)
{
	U8 i, size, TempChar;
	
    if (!IsHexStringOk(pInput))
    {
        /*lq ���з�HEX�ַ�*/
        return FALSE;
    }
    else
    {
	    size = (U8)strlen(pInput);
        for (i = 0; 2 * i < size; i++)
        {              
            /*lq �ֽڵĵ�4λ*/ 
            pOutput[i] &= 0;
            pOutput[i] = pInput[2 * i] & 0x0F;

            /*lq �ֽڵĸ�4λ*/            
            if (2 * i + 1 < size) 
            {
                TempChar = pInput[2 * i + 1] << 4;
                TempChar &= 0xF0;
                pOutput[i] |= TempChar;            
            }
        }                   
        pOutput[i] = '\0';

        return TRUE;    
    }
}

/*******************************************************************************
* ����  : ConvertByteToBcd
* ����  : �ֽ�ת��ΪBCD��
* ����  : pInput-��ת���ֽ��ַ���
* ���  : pOutput-ת�����BCD�루��HEX�ַ����ַ���
* ����  : TRUE����ȷ��FALSE������
*******************************************************************************/
U8 ConvertByteToBcd(char *pInput, U8 *pOutput)
{
	U8 i, size, TempByte;
	
    size = (U8)strlen(pInput);
    for (i = 0; i < size; i++)
    {              
        /*lq �ֽڵĵ�4λ*/
        pOutput[2 * i] = pInput[i] & 0x0F;
        
        /*lq �ֽڵĸ�4λ*/
        TempByte = pInput[i] >> 4;
        TempByte &= 0x0F;
        pOutput[2 * i + 1] = TempByte;
    }                   
    pOutput[2 * i] = '\0';

    return TRUE;    
}

/*******************************************************************************
* ����  : GetSegment
* ����  : ��ȡ�������ֶΣ����Ա������ֶ����ݽ��д���ת��Ϊ16��������
* ����  : pInput : �����ַ���
		  SegmentIndex: ��ȡ�ֶ�n����һ������֮���Ϊ��һ�ֶ�
		  SumLength : �ַ����ܳ���
		  Max_Bytes���������ֽڳ���
* ���  : Output�����صĶ���������
* ����  : ת���Ƿ���ȷ
*******************************************************************************/
U8 GetSegment(char *pInput, char *Output, U16 SegmentIndex, U16 SumLength, U16 Max_Bytes)
{
	U16 uiLen;
//	U8 ucCheck;

	uiLen = GetString(pInput, Output, SegmentIndex, SumLength, Max_Bytes);

	//�ɹ�����ַ���
	if (uiLen != 0)
	{
		Output[uiLen] = '\0';
		
		return (TRUE);
	}

	return (FALSE);
}

#if 0
/*******************************************************************************
* ����  : GetHexData
* ����  : ��ȡ�������ֶΣ����Ա������ֶ����ݽ��д���ת��Ϊ16��������
* ����  : pInput : �����ַ���
		  SegmentIndex: ��ȡ�ֶ�n����һ������֮���Ϊ��һ�ֶ�
		  SumLength : �ַ����ܳ���
* ���  : Output�����صĶ���������
* ����  : ת���Ƿ���ȷ
*******************************************************************************/
U8 GetHexData(char *pInput, U32 *Output, U16 SegmentIndex, U16 SumLength)
{
	char chRetrunS1[33];
	U8 ucCheck;
	
	//�ַ�У����ȷ
	ucCheck = GetSegment(pInput, chRetrunS1, SegmentIndex, SumLength, 32);

	if (ucCheck == TRUE)
	{
		ucCheck = IsAscStringOk(chRetrunS1);

		if (ucCheck == TRUE)
		{
			Output[0] = atol(chRetrunS1);
			return (TRUE);
		}	
	}

	return (FALSE);
}
#endif

/*******************************************************************************
* ����  : SartMmsiFormCheck
* ����  : �ж�MMSI�Ƿ����SART��׼
* ����  : ������ַ���
* ���  : ��
* ����  : TRUE����ȷ��FALSE������
*******************************************************************************/
U8 SartMmsiFormCheck(char *pInput)
{
	U8 size;
	
	size = (U8)strlen(pInput);

    if (size != 9)
    {
        return FALSE;
    }

    if (pInput[0] == '9' && pInput[1] == '7' && pInput[2] == '0')
    {
        return TRUE;
    }
    else
    {
	    return FALSE;
    }
}

/*******************************************************************************
* ����  : IsLatFormat
* ����  : �ж��Ƿ�����Ч��γ��DDMM.mmmm
          �������ֳ��ȹ̶�Ϊ4��С���������Precisionλ
* ����  : pData-������ַ���
          Precision-С���������λ����������
* ���  : pOut-��λΪPrecision��γ��
* ����  : TRUE����Ч��FALSE����Ч
*******************************************************************************/
U8 IsLatFormat(char *pData, U32 *pOut, U8 Precision)
{
    char *pEnd;
    U8  ucLen;
    U8  i;
	U32 ulTemp1;
	U32 ulTemp2;
	U32 ulTemp3;
	U32 ulTemp4;

    ucLen = strlen(pData);
    
    /*lq �ܳ�����Ч���ж�*/
    if (ucLen == 4 || (ucLen > 5 && ucLen <= 5 + Precision))
    {
        /*lq ���������ַ���Ч���ж�*/
        for (i = 0; i < 4; i++)    
        {
            if (!(pData[i] >= '0' && pData[i] <= '9'))
            {
                return FALSE;
            }
        }

        /*lq γ��-����Ч���ж� */
        ulTemp1 = (pData[0] - '0') * 10 + (pData[1] - '0');   //lq ��
        if (ulTemp1 > 90)
        {
    		return FALSE;                              
        }
    
        /*lq ����Ч���ж� */
        ulTemp2 = (pData[2] - '0') * 10 + (pData[3] - '0');   //lq ��
        if (ulTemp2 >= 60)
        {
    		return FALSE;                              
        }
        
        ulTemp3 = ulTemp1 * 60 + ulTemp2;
    
        /*lq ��������չ*/
        for (i = 0; i < Precision; i++)
        {
            ulTemp3 *= 10;                                    //lq ����
        }

        /*lq С�����ִ���*/
        ulTemp4 = 0;
        if (ucLen != 4)        
        {
            if (pData[4] != '.')                                
            {
        		return FALSE;                                   
            }
    
            /*lq С��������ֵ��Ч���ж�*/
            for (i = 0; pData[5 + i] != '\0'; i++)
            {
                if ((pData[5 + i] >= '0' && pData[5 + i] <= '9'))
                {
        		    ulTemp4 = ulTemp4 * 10 + (pData[5 + i] - '0');
                }
                else
                {
                    return FALSE;                                   
                }
            } 
            
            for (; i < Precision; i++)
            {
                ulTemp4 *= 10;
            }       
        }

        /*lq ʵ��γ��ֵ*/
        ulTemp3 = ulTemp3 + ulTemp4;
        
        /*lq γ��ת��Ϊ��Ӧ���ȵ����ֵ*/
        ulTemp4 = 90 * 60;
        for (i = 0; i < Precision; i++)
        {
    		ulTemp4 *= 10;                                   
        }

        /*lq γ����Ч���ж�*/
        if (ulTemp3 > ulTemp4)
        {
            return FALSE;        
        }
        else
        {
            *pOut = ulTemp3;
            return TRUE;        
        }
    }
    else
    {
        return FALSE;
    }
}


/*******************************************************************************
* ����  : IsLonFormat
* ����  : �ж��Ƿ�����Ч�ľ���DDDMM.mmmm
          �������ֳ��ȹ̶�Ϊ5��С���������Precisionλ
* ����  : pData-������ַ���
          Precision-С���������λ����������
* ���  : pOut-��λΪPrecision�ľ���
* ����  : TRUE����Ч��FALSE����Ч
*******************************************************************************/
U8 IsLonFormat(char *pData, U32 *pOut, U8 Precision)
{
    char *pEnd;
    U8  ucLen;
    U8  i;
	U32 ulTemp1;
	U32 ulTemp2;
	U32 ulTemp3;
	U32 ulTemp4;

    ucLen = strlen(pData);
    
    /*lq �ܳ�����Ч���ж�*/
    if (ucLen == 5 || (ucLen > 6 && ucLen <= 6 + Precision))
    {
        /*lq ���������ַ���Ч���ж�*/
        for (i = 0; i < 5; i++)    
        {
            if (!(pData[i] >= '0' && pData[i] <= '9'))
            {
                return FALSE;
            }
        }

        /*lq ����-����Ч���ж� */
        ulTemp1 = (pData[0] - '0') * 100 + (pData[1] - '0') * 10 + (pData[2] - '0');   //lq ��
        if (ulTemp1 > 180)
        {
    		return FALSE;                              
        }
    
        /*lq ����Ч���ж� */
        ulTemp2 = (pData[3] - '0') * 10 + (pData[4] - '0');   //lq ��
        if (ulTemp2 >= 60)
        {
    		return FALSE;                              
        }
        
        ulTemp3 = ulTemp1 * 60 + ulTemp2;
    
        /*lq ��������չ*/
        for (i = 0; i < Precision; i++)
        {
            ulTemp3 *= 10;                                    //lq ����
        }

        /*lq С�����ִ���*/
        ulTemp4 = 0;
        if (ucLen != 5)        
        {
            if (pData[5] != '.')                                
            {
        		return FALSE;                                   
            }
    
            /*lq С��������ֵ��Ч���ж�*/
            for (i = 0; pData[6 + i] != '\0'; i++)
            {
                if ((pData[6 + i] >= '0' && pData[6 + i] <= '9'))
                {
        		    ulTemp4 = ulTemp4 * 10 + (pData[6 + i] - '0');
                }
                else
                {
                    return FALSE;                                   
                }
            } 
            
            for (; i < Precision; i++)
            {
                ulTemp4 *= 10;
            }       
        }

        /*lq ʵ�ʾ���ֵ*/
        ulTemp3 = ulTemp3 + ulTemp4;
        
        /*lq ����ת��Ϊ��Ӧ���ȵ����ֵ*/
        ulTemp4 = 180 * 60;
        for (i = 0; i < Precision; i++)
        {
    		ulTemp4 *= 10;                                   
        }

        /*lq ������Ч���ж�*/
        if (ulTemp3 > ulTemp4)
        {
            return FALSE;        
        }
        else
        {
            *pOut = ulTemp3;
            return TRUE;        
        }
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : ToLatFormat
* ����  : ����λΪ1Precision��γ��ת��ΪDDMM.mmmm��ʽ
          �������ֳ��ȹ̶�Ϊ4��С������ΪPrecisionλ
* ����  : Data-��λΪ1Precision��γ��
          Precision-ת����С������λ����������
* ���  : pOut-DDMM.mmmm��ʽ��γ��
* ����  : TRUE����Ч��FALSE����Ч
*******************************************************************************/
U8 ToLatFormat(U32 Data, U8 *pOut, U8 Precision)
{
    double MaxValue;
    double  lfTemp;
    U32 ulTemp;
    char RetBuf[30];
    char TempBuf[30];
    U8  i;

    MaxValue = 90 * 60;     //lq ����Ϊ1��
    lfTemp = (double)Data;

    /*lq ת��Ϊ��λΪ1��*/
    for (i = 0; i < Precision; i++)
    {
        lfTemp /= 10;
    }

    if (lfTemp > MaxValue)
    {
        return FALSE;       //lq ����ָ���ľ���ת����Ϊ��Чֵ
    }
    else
    {
        ulTemp = (U16)(lfTemp / 60);
        lfTemp -= (ulTemp * 60);

        sprintf(pOut, "%0*d", 2, ulTemp);
        if (Precision == 0)
        {
            sprintf(TempBuf, "%0*d", 2, (U16)lfTemp);        
        }
        else
        {
            sprintf(TempBuf, "%0*.*Lf", 3 + Precision, Precision, lfTemp);        
        }
        strcat(pOut, TempBuf);
        return TRUE;
    }
}

/*******************************************************************************
* ����  : ToLonFormat
* ����  : ����λΪ1Precision�ľ���ת��ΪDDDMM.mmmm��ʽ
          �������ֳ��ȹ̶�Ϊ5��С������ΪPrecisionλ
* ����  : Data-��λΪ1Precision�ľ���
          Precision-ת����С������λ����������
* ���  : pOut-DDDMM.mmmm��ʽ�ľ���
* ����  : TRUE����Ч��FALSE����Ч
*******************************************************************************/
U8 ToLonFormat(U32 Data, U8 *pOut, U8 Precision)
{
    double MaxValue;
    double  lfTemp;
    U32 ulTemp;
    char RetBuf[30];
    char TempBuf[30];
    U8  i;

    MaxValue = 180 * 60;     //lq ����Ϊ1��
    lfTemp = (double)Data;

    /*lq ת��Ϊ��λΪ1��*/
    for (i = 0; i < Precision; i++)
    {
        lfTemp /= 10;
    }

    if (lfTemp > MaxValue)
    {
        return FALSE;       //lq ����ָ���ľ���ת����Ϊ��Чֵ
    }
    else
    {
        ulTemp = (U16)(lfTemp / 60);
        lfTemp -= (ulTemp * 60);

        sprintf(pOut, "%0*d", 3, ulTemp);
        if (Precision == 0)
        {
            sprintf(TempBuf, "%0*d", 2, (U16)lfTemp);        
        }
        else
        {
            sprintf(TempBuf, "%0*.*Lf", 3 + Precision, Precision, lfTemp);        
        }
        strcat(pOut, TempBuf);
        return TRUE;
    }
}

#if 0
void IsLatValidTest(char *Lat)
{
    U8 i;
    U32 ulLat;
    U8 precision;
    // char Lat[10];

    precision = 4;
    // sprintf(Lat, "%s", "8959");
    // sprintf(Lat, "%s", "8959.");
    // sprintf(Lat, "%s", "900");

    for (i = 0; i < precision + 1; i++)
    {
        if (IsLatFormat(Lat, &ulLat, i) == TRUE)
        {
            usart_printf("Lat valid for precision of %d: %ld\r\n", i, ulLat);            
        }
        else
        {
            usart_printf("Lat invalid for precision of %d\r\n", i);
        }
    }
}

void IsLonValidTest(char *Lon)
{
    U8 i;
    U32 ulLon;
    U8 precision;
    // char Lat[10];

    precision = 4;
    // sprintf(Lat, "%s", "8959");
    // sprintf(Lat, "%s", "8959.");
    // sprintf(Lat, "%s", "900");

    for (i = 0; i < precision + 1; i++)
    {
        if (IsLonFormat(Lon, &ulLon, i) == TRUE)
        {
            usart_printf("Lon valid for precision of %d: %ld\r\n", i, ulLon);            
        }
        else
        {
            usart_printf("Lon invalid for precision of %d\r\n", i);
        }
    }
}

void ToLatFormatTest(U32 ulLon)
{
    U8 i;
    //U32 ulLon;
    U8 precision;
    char Lat[10];

    precision = 4;
    // sprintf(Lat, "%s", "8959");
    // sprintf(Lat, "%s", "8959.");
    // sprintf(Lat, "%s", "900");
    for (i = 0; i < precision + 1; i++)
    {
        if (ToLatFormat(ulLon, Lat, i) == TRUE)
        {
            usart_printf("Lon valid for precision of %d: %s\r\n", i, Lat);            
        }
        else
        {
            usart_printf("Lon invalid for precision of %d\r\n", i);
        }
    }
}

void ToLonFormatTest(U32 ulLon)
{
    U8 i;
    //U32 ulLon;
    U8 precision;
    char Lat[10];

    precision = 4;
    // sprintf(Lat, "%s", "8959");
    // sprintf(Lat, "%s", "8959.");
    // sprintf(Lat, "%s", "900");
    for (i = 0; i < precision + 1; i++)
    {
        if (ToLonFormat(ulLon, Lat, i) == TRUE)
        {
            usart_printf("Lon valid for precision of %d: %s\r\n", i, Lat);            
        }
        else
        {
            usart_printf("Lon invalid for precision of %d\r\n", i);
        }
    }
}
#endif
/*******************************************************************************
* ����  : UartMsgProcess
* ����  : ���ڱ��Ľ�����
          ��ͨ��һ��������ö������ʱ����ĳ�ֶ�Ϊ������Ը��ֶΣ�����ĳ�ֶε�ȡ
          ֵ��Чʱ����Ϊ����ʧ�ܣ�����֮ǰ�����óɹ���,�������ֶλ���Ϊ�ջ���ȡֵ��
          Ч�����óɹ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 UartMsgProcess(U8 *pDat, U16 uiLen)
{
#if !WIN32
	char chTemp[83];
    char chTemp2[83];
	U8 	ucTempBuf[40];
	U8 	ucCheck;
	U8 	ucLen;
//	U8	ucRetLen;
    S32 slTemp;
	U32 ulTemp;
	U32 ulTemp1;
	U32 ulTemp2;
	U32 ulTemp3;
	U32 ulTemp4;
    char *pEnd;
    char *pChar;
	U8 ucTemp;
	char chRetBuf[100];
    U8  ByteTemp[40];
    U8  ucSartFlg;
	U8	uiTempValue;
	U8	uiTempValue2;
    float sfTemp;
    double lfTemp;
    static U16 VerSeqMsgId = 0;
    U8  buffer[10];
    S16 i;
    AisChannelEnum channel;
	U32	ulDeviceIDBuff[11];

    /*lq AIS���ֽӿڱ�׼������ */
    if (AISDI_SentenceParse(pDat, uiLen) == FALSE)
    {
        /*lq �ѽ��� */
        return (TRUE);
    }
    //===================================================================================
    /*lq P712��ͷ����������*/
	//  Process  type  1     :  note by Nathan
	else if (strncmp((char *)pDat, "P712", 4) == FALSE)
	{
		//��ȡ��1�ֶ����ݣ��жϵ�ǰ����
		ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);

		if (ucCheck == FALSE)
		{
			return (FALSE);
		}
		if (DevDynamicPara.ucWorkMode == EQ_WORKMOD_NORMAL)
		{
			//MMSI�Ÿ���
			if (strncmp(chTemp, "010", 3) == FALSE)
			{
				//��ȡEEPROM��MMSIֵ����Ϊ0��������£������������
				//Para_Read(PARA_MMSI_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_MMSI_TYPE, ucTempBuf);

				//MMSI����Ҫ�ȸ�λ�ٸ���
				if (strncmp((char *)ucTempBuf, "000000000", 9) == FALSE)
				{
					//д��MMSI�ţ���ȡMMSI��
					ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);

					if (ucCheck == FALSE)
					{
						UartResponseMsg("$DUAIR,0,010");
						return (FALSE);
					}

					ucLen = strlen(chTemp);
					ucCheck = IsAscStringOk(chTemp);

#if CONFIG_MMSI_SARTFORM_CHECK
                    /*lq SART mmsi ��ʽ����*/
                    ucSartFlg = SartMmsiFormCheck(chTemp);
#else
                    ucSartFlg = TRUE;
#endif
					if ((ucLen != 9) || (ucCheck == FALSE) || (ucSartFlg == FALSE))
					{
						UartResponseMsg("$DUAIR,0,010");
						return (FALSE);
					}

					//д��MMSI��
					//Para_Save(PARA_MMSI_TYPE, (U8 *)chTemp, ucLen);
					FlashWrite(STM32_FLASH_MMSI_TYPE, (U8 *)chTemp,ucLen);
					//Para_Save(PARA_MMSI_BACKUP_TYPE, (U8 *)chTemp, ucLen);//ͬʱд����
					FlashWrite(STM32_FLASH_MMSI_BACKUP_TYPE, (U8 *)chTemp,ucLen);

					//�˴����Ӹ���Э��ջ�ӿ�
					AisStaticPara.MMSI = atol(chTemp);
					AisStaticPara.Refresh = TRUE;

					UartResponseMsg("$DUAIR,1,010");
				}
				else
				{
					UartResponseMsg("$DUAIR,0,010");
					return (FALSE);
				}
			}
			//��λMMSI��
			else if (strncmp(chTemp, "013", 3) == FALSE)
			{
				strncpy(&chTemp[0],"000000000",10);
				
                /*lq ��λMMSI*/
				//Para_Save(PARA_MMSI_TYPE, (U8 *)chTemp, 9);
				FlashWrite(STM32_FLASH_MMSI_TYPE, (U8 *)chTemp,9);
				AisStaticPara.MMSI = atol(chTemp);
#if 0
                /*lq ��λĿ��MMSI*/
				Para_Save(PARA_MMSI_DEST_TYPE, (U8 *)chTemp, 9);
				siStaticPara.DestMMSI = atol(chTemp);
#endif
				AisStaticPara.Refresh = TRUE;
				UartResponseMsg("$DUAIR,1,013");	
			}
			//���к�д��
			else if (strncmp(chTemp, "SNW", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
			
				ucLen = strlen(chTemp);

				if ((ucCheck == FALSE) || (ucLen != 8))
				{
					UartResponseMsg("$P712,0,SNW");
					return (FALSE);
				}

				//д�����к�
				//Para_Save(PARA_SERIAL_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_SERIAL_TYPE, (U8 *)chTemp,ucLen);

				UartResponseMsg("$P712,1,SNW");
				return (TRUE);
			}

			//������Ϣ����
			else if (strncmp(chTemp, "SRM", 3) == FALSE)
			{
				//***************************VendID*******************************//
				//д��VendID
				ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
	
				if (ucCheck == FALSE)
				{
					UartResponseMsg("$DUAIR,0,SRM");
					return (FALSE);
				}
	
				ucLen = strlen(chTemp);
	
				if (ucLen != 7)
				{
					UartResponseMsg("$DUAIR,0,SRM");
					return (FALSE);
				}
	
				//д��VendID
				//Para_Save(PARA_VENDID_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_VENDID_TYPE, (U8 *)chTemp,ucLen);
	
				//�˴����Ӹ���Э��ջ�ӿ�
				//lq strncpy((char *)&siStaticPara.VendID[0], chTemp, ucLen);
				strcpy((char *)&AisStaticPara.VendID[0], chTemp);                    //lq ͬʱ���ơ�\0��
		
				AisStaticPara.Refresh = TRUE;
				UartResponseMsg("$DUAIR,1,SRM");		
			}
			//GPS����������״̬
			else if (strncmp(chTemp, "GPSDATA", 7) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);				
				
				if (ucCheck == FALSE)
				{
					UartResponseMsg("$DUAIR,0,GPS");
					return (FALSE);	
				}

				ucCheck = IsAscStringOk(chTemp);

				if (ucCheck == FALSE)
				{
					UartResponseMsg("$DUAIR,0,GPS");
					return (FALSE);
				}

				if (chTemp[0] == '0')
				{
					DevDynamicPara.ucGpsOutput = FALSE;
                    DEVINFO_InstResetGpsOutputCnt();                
					UartResponseMsg("$DUAIR,1,GPS1");	
				}
				else if (chTemp[0] == '1')
				{
					DevDynamicPara.ucGpsOutput = TRUE;	
                    DEVINFO_InstResetGpsOutputCnt();                
					UartResponseMsg("$DUAIR,1,GPS0");
				}
				else
				{
					UartResponseMsg("$DUAIR,0,GPS");	
				}	
			}

			/*lq GPS�������Ƶ������ */
			else if (strncmp(chTemp, "GPSOP", 5) == FALSE)
			{
			    /*lq ��ȡ��2�ֶε����� */
                ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
				if (ucCheck == TRUE)                                            //lq ���ֶοգ��򲻽��и��������
				{
                    /*lq ������Ч���ж� */
                    ucLen = strlen(chTemp);
            		if (ucLen > 3)
            		{
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
            
            		/*lq �ַ���Ч���ж� */
                    ulTemp = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq �ַ���Ч */
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
                    else
                    {
                        /*lq ȡֵ��Χ��Ч���ж� */
                        if (ulTemp < DEV_INFO_GPS_OUTPUT_INTERVAL_MIN
                            || ulTemp > DEV_INFO_GPS_OUTPUT_INTERVAL_MAX)
                        {
            		        /*lq ȡֵ��Ч */
            				UartResponseMsg("$DUAIR,0,GPSOP");
                			return (FALSE);
                        }
                    }

                    DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_RMC] = ulTemp;
				}
                
			    /*lq ��ȡ��3�ֶε����� */
                ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
				if (ucCheck == TRUE)                                            //lq ���ֶοգ��򲻽��и��������
				{
                    /*lq ������Ч���ж� */
                    ucLen = strlen(chTemp);
            		if (ucLen > 3)
            		{
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
            
            		/*lq �ַ���Ч���ж� */
                    ulTemp = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq �ַ���Ч */
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
                    else
                    {
                        /*lq ȡֵ��Χ��Ч���ж� */
                        if (ulTemp < DEV_INFO_GPS_OUTPUT_INTERVAL_MIN
                            || ulTemp > DEV_INFO_GPS_OUTPUT_INTERVAL_MAX)
                        {
            		        /*lq ȡֵ��Ч */
            				UartResponseMsg("$DUAIR,0,GPSOP");
                			return (FALSE);
                        }
                    }

                    DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_VTG] = ulTemp;
				}

			    /*lq ��ȡ��4�ֶε����� */
                ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
				if (ucCheck == TRUE)                                            //lq ���ֶοգ��򲻽��и��������
				{
                    /*lq ������Ч���ж� */
                    ucLen = strlen(chTemp);
            		if (ucLen > 3)
            		{
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
            
            		/*lq �ַ���Ч���ж� */
                    ulTemp = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq �ַ���Ч */
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
                    else
                    {
                        /*lq ȡֵ��Χ��Ч���ж� */
                        if (ulTemp < DEV_INFO_GPS_OUTPUT_INTERVAL_MIN
                            || ulTemp > DEV_INFO_GPS_OUTPUT_INTERVAL_MAX)
                        {
            		        /*lq ȡֵ��Ч */
            				UartResponseMsg("$DUAIR,0,GPSOP");
                			return (FALSE);
                        }
                    }

                    DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GGA] = ulTemp;
				}
                
			    /*lq ��ȡ��5�ֶε����� */
                ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);
				if (ucCheck == TRUE)                                            //lq ���ֶοգ��򲻽��и��������
				{
                    /*lq ������Ч���ж� */
                    ucLen = strlen(chTemp);
            		if (ucLen > 3)
            		{
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
            
            		/*lq �ַ���Ч���ж� */
                    ulTemp = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq �ַ���Ч */
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
                    else
                    {
                        /*lq ȡֵ��Χ��Ч���ж� */
                        if (ulTemp < DEV_INFO_GPS_OUTPUT_INTERVAL_MIN
                            || ulTemp > DEV_INFO_GPS_OUTPUT_INTERVAL_MAX)
                        {
            		        /*lq ȡֵ��Ч */
            				UartResponseMsg("$DUAIR,0,GPSOP");
                			return (FALSE);
                        }
                    }

                    DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSA] = ulTemp;
				}
                
			    /*lq ��ȡ��6�ֶε����� */
                ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 30);
				if (ucCheck == TRUE)                                            //lq ���ֶοգ��򲻽��и��������
				{
                    /*lq ������Ч���ж� */
                    ucLen = strlen(chTemp);
            		if (ucLen > 3)
            		{
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
            
            		/*lq �ַ���Ч���ж� */
                    ulTemp = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq �ַ���Ч */
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
                    else
                    {
                        /*lq ȡֵ��Χ��Ч���ж� */
                        if (ulTemp < DEV_INFO_GPS_OUTPUT_INTERVAL_MIN
                            || ulTemp > DEV_INFO_GPS_OUTPUT_INTERVAL_MAX)
                        {
            		        /*lq ȡֵ��Ч */
            				UartResponseMsg("$DUAIR,0,GPSOP");
                			return (FALSE);
                        }
                    }

                    DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSV] = ulTemp;
				}
                
			    /*lq ��ȡ��7�ֶε����� */
                ucCheck = GetSegment((char *)pDat, chTemp, 7, uiLen, 30);
				if (ucCheck == TRUE)                                            //lq ���ֶοգ��򲻽��и��������
				{
                    /*lq ������Ч���ж� */
                    ucLen = strlen(chTemp);
            		if (ucLen > 3)
            		{
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
            
            		/*lq �ַ���Ч���ж� */
                    ulTemp = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq �ַ���Ч */
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
                    else
                    {
                        /*lq ȡֵ��Χ��Ч���ж� */
                        if (ulTemp < DEV_INFO_GPS_OUTPUT_INTERVAL_MIN
                            || ulTemp > DEV_INFO_GPS_OUTPUT_INTERVAL_MAX)
                        {
            		        /*lq ȡֵ��Ч */
            				UartResponseMsg("$DUAIR,0,GPSOP");
                			return (FALSE);
                        }
                    }

                    DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GLL] = ulTemp;
				}
                
			    /*lq ��ȡ��8�ֶε����� */
                ucCheck = GetSegment((char *)pDat, chTemp, 8, uiLen, 30);
				if (ucCheck == TRUE)                                            //lq ���ֶοգ��򲻽��и��������
				{
                    /*lq ������Ч���ж� */
                    ucLen = strlen(chTemp);
            		if (ucLen > 3)
            		{
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
            
            		/*lq �ַ���Ч���ж� */
                    ulTemp = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq �ַ���Ч */
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
                    else
                    {
                        /*lq ȡֵ��Χ��Ч���ж� */
                        if (ulTemp < DEV_INFO_GPS_OUTPUT_INTERVAL_MIN
                            || ulTemp > DEV_INFO_GPS_OUTPUT_INTERVAL_MAX)
                        {
            		        /*lq ȡֵ��Ч */
            				UartResponseMsg("$DUAIR,0,GPSOP");
                			return (FALSE);
                        }
                    }

                    DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GBS] = ulTemp;
				}
                
                DEVINFO_InstResetGpsOutputCnt();                
        	    UartResponseMsg("$DUAIR,1,GPSOP");
            }			

			//GPSͬ��
			else if (strncmp(chTemp, "SYNCH", 5) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);				
				
				if (ucCheck == FALSE)
				{
					UartResponseMsg("$DUAIR,0,SYNCH");
					return (FALSE);	
				}

				ucCheck = IsAscStringOk(chTemp);

				if (ucCheck == FALSE)
				{
					UartResponseMsg("$DUAIR,0,SYNCH");
					return (FALSE);
				}

				if (chTemp[0] == '0')
				{
					GpsSynCell.GPSSyn_Enable = FALSE;
					UartResponseMsg("$DUAIR,1,SYNCH0");	
				}
				else if (chTemp[0] == '1')
				{
					GpsSynCell.GPSSyn_Enable = TRUE;	
					UartResponseMsg("$DUAIR,1,SYNCH1");
				}
				else
				{
					UartResponseMsg("$DUAIR,0,SYNCH");	
				}	
			}
#if 0
			//��λ������������
			else if (strncmp(chTemp, "RNPOC", 5) == FALSE)
			{
                sprintf(chTemp, "%2d", 0);
				Para_Save(PARA_NORMAL_POWER_ON_CNT_TYPE, (U8 *)chTemp, 2);

				UartResponseMsg("$DUAIR,1,RNPOC");  						
			}

			//��λ�û����Դ���
			else if (strncmp(chTemp, "RSTC", 4) == FALSE)
			{
                sprintf(chTemp, "%2d", 0);
				Para_Save(PARA_USER_TEST_CNT_TYPE, (U8 *)chTemp, 2);

				UartResponseMsg("$DUAIR,1,RSTC");  						
			}
#endif
			//��λ������ʱ��
			else if (strncmp(chTemp, "RWTS", 4) == FALSE)
			{
                sprintf(chTemp, "%5d", 0);
				Para_Save(PARA_WORKTIME_TYPE, (U8 *)chTemp, 5);

				UartResponseMsg("$DUAIR,1,RWTS");  						
			}

			//��λ�ϴο�������ʱ��
			else if (strncmp(chTemp, "RLWT", 4) == FALSE)
			{
                sprintf(chTemp, "%5d", 0);
				Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)chTemp, 5);

				UartResponseMsg("$DUAIR,1,RLWT");  						
			}

            //����Ӳ���汾��
			else if (strncmp(chTemp, "HWF", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
			
				ucLen = strlen(chTemp);

				if ((ucCheck == FALSE) || (ucLen > STM32_FLASH_HWVERSION_MAXLEN))
				{
					UartResponseMsg("$DUAIR,0,HWF");
					return (FALSE);
				}

                if (IsVersionString(chTemp) == FALSE)
                {
					UartResponseMsg("$DUAIR,0,HWF");
					return (FALSE);                       
                }

				//д��Ӳ���汾��
				//Para_Save(PARA_HWVERSION_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_HWVERSION_TYPE, (U8 *)chTemp,ucLen);
				UartResponseMsg("$DUAIR,1,HWF");
				
                return (TRUE);  						
			}

			/*lq AIS������� */
			else if (strncmp(chTemp, "AISCLASS", 8) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);//��ȡ�ڶ��ֶε�����

				if (ucCheck == FALSE)
				{
    				/*lq �ֶ�Ϊ�� */
                    UartResponseMsg("$DUAIR,0,AISCLASS");
					return (FALSE);
				}
                
                /*lq ������Ч���ж� */
                ucLen = strlen(chTemp);
        		if (ucLen != 1)
        		{
    				UartResponseMsg("$DUAIR,0,AISCLASS");
        			return (FALSE);
        		}
        
        		/*lq �ַ���Ч���ж� */
                ulTemp = strtoul(chTemp, &pEnd, 10);
        		if ((*pEnd) != 0)
        		{
        		    /*lq �ַ���Ч */
    				UartResponseMsg("$DUAIR,0,AISCLASS");
        			return (FALSE);
        		}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж� */
                    if (ulTemp < STM32_FLASH_AIS_CLASS_FLOOR
                        || ulTemp > STM32_FLASH_AIS_CLASS_CEIL)
                    {
        		        /*lq ȡֵ��Ч */
        				UartResponseMsg("$DUAIR,0,AISCLASS");
            			return (FALSE);
                    }
                }
                
                /*lq д��EEPROM */
                //Para_Save(PARA_AIS_CLASS_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_AIS_CLASS_TYPE, (U8 *)chTemp,ucLen);
        	    UartResponseMsg("$DUAIR,1,AISCLASS");
            }			

			/*lq Transceiver���� */
			else if (strncmp(chTemp, "TR", 2) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);//��ȡ�ڶ��ֶε�����

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}	
				/*lq Transceiver ��SI446X��״̬���� */
				else if (strncmp(chTemp, "006", 3) == FALSE)
				{
    				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);//��ȡ��3�ֶε�����    
    				if (ucCheck == FALSE)
    				{
        				UartResponseMsg("$DUAIR,0,TR,006");    					
                        return (FALSE);
    				}

                    /*lq ������Ч���ж�*/
                    ucLen = strlen(chTemp);
        			if (ucLen != 1)
        			{
        				UartResponseMsg("$DUAIR,0,TR,006");
        				return (FALSE);
        			}
        
        			/*lq �ַ���Ч���ж�*/
                    ucCheck = IsAscStringOk(chTemp);
        			if (ucCheck == FALSE)
        			{
        				UartResponseMsg("$DUAIR,0,TR,006");
        				return (FALSE);
        			}
                    else
                    {
                        /*lq ȡֵ��Χ��Ч���ж�*/
                        ulTemp = strtoul(chTemp, &pEnd, 10);
                        if (ulTemp == 1)
                        {
            				ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);//��ȡ��4�ֶε�����    
            				if (ucCheck == FALSE)
            				{
                				UartResponseMsg("$DUAIR,0,TR,006");    					
                                return (FALSE);
            				}
        
                            /*lq ������Ч���ж�*/
                            ucLen = strlen(chTemp);
                			if (ucLen != 1)
                			{
                				UartResponseMsg("$DUAIR,0,TR,006");
                				return (FALSE);
                			}
                
                			/*lq �ַ���Ч���ж�*/
                            ucCheck = IsAscStringOk(chTemp);
                			if (ucCheck == FALSE)
                			{
                				UartResponseMsg("$DUAIR,0,TR,006");
                				return (FALSE);
                			}
                            else
                            {
                                /*lq ȡֵ��Χ��Ч���ж�*/
                                ulTemp = strtoul(chTemp, &pEnd, 10);
                                if ((ulTemp >= 0) && (ulTemp <= 8))
                                {
                                    SI446X_CHANGE_STATE(ulTemp, SI446X_CHIP_A);
                                    UartResponseMsg("$DUAIR,1,TR,006");         
                                }
                                else
                                {
                 			        UartResponseMsg("$DUAIR,0,TR,006");                                        
                                }
                            }
                        }
                        else if (ulTemp == 2)
                        {
            				ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);//��ȡ��4�ֶε�����    
            				if (ucCheck == FALSE)
            				{
                				UartResponseMsg("$DUAIR,0,TR,006");    					
                                return (FALSE);
            				}
        
                            /*lq ������Ч���ж�*/
                            ucLen = strlen(chTemp);
                			if (ucLen != 1)
                			{
                				UartResponseMsg("$DUAIR,0,TR,006");
                				return (FALSE);
                			}
                
                			/*lq �ַ���Ч���ж�*/
                            ucCheck = IsAscStringOk(chTemp);
                			if (ucCheck == FALSE)
                			{
                				UartResponseMsg("$DUAIR,0,TR,006");
                				return (FALSE);
                			}
                            else
                            {
                                /*lq ȡֵ��Χ��Ч���ж�*/
                                ulTemp = strtoul(chTemp, &pEnd, 10);
                                if ((ulTemp >= 0) && (ulTemp <= 8))
                                {
                                    SI446X_CHANGE_STATE(ulTemp, SI446X_CHIP_B);
                                    UartResponseMsg("$DUAIR,1,TR,006");         
                                }
                                else
                                {
                 			        UartResponseMsg("$DUAIR,0,TR,006");                                        
                                }
                            }          
                        }
                        else
                        {
            				UartResponseMsg("$DUAIR,0,TR,006");
            				return (FALSE);                             
                        }
                    }                                                       
                }

				/*lq Transceiver ��SI446X������������ã���δд��EEPROM�� */
				else if (strncmp(chTemp, "007", 3) == FALSE)
				{
    				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);//��ȡ��3�ֶε�����    
    				if (ucCheck == FALSE)
    				{
        				UartResponseMsg("$DUAIR,0,TR,007");    					
                        return (FALSE);
    				}

                    /*lq ������Ч���ж� */
                    ucLen = strlen(chTemp);
            		if (ucLen != 1)
            		{
        				UartResponseMsg("$DUAIR,0,TR,007");
            			return (FALSE);
            		}
            
            		/*lq �ַ���Ч���ж� */
                    ulTemp = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq �ַ���Ч */
        				UartResponseMsg("$DUAIR,0,TR,007");
            			return (FALSE);
            		}

                    /*lq ȡֵ��Χ��Ч���ж� */
                    if ((ulTemp < 1) || (ulTemp > 2))
                    {
            		    /*lq ȡֵ��Ч */
        				UartResponseMsg("$DUAIR,0,TR,007");
            			return (FALSE);
                    }

    				ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);//��ȡ��4�ֶε�����    
    				if (ucCheck == FALSE)
    				{
        				UartResponseMsg("$DUAIR,0,TR,007");    					
                        return (FALSE);
    				}

                    /*lq ������Ч���ж�*/
                    ucLen = strlen(chTemp);
        			if ((ucLen < 1) || (ucLen > 3))
        			{
        				UartResponseMsg("$DUAIR,0,TR,007");
        				return (FALSE);
        			}

            		/*lq �ַ���Ч���ж� */
                    ulTemp1 = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq �ַ���Ч */
        				UartResponseMsg("$DUAIR,0,TR,007");
            			return (FALSE);
            		}

                    /*lq ȡֵ��Χ��Ч���ж� */
                    if ((ulTemp1 < 1) || (ulTemp1 > 127))
                    {
            		    /*lq ȡֵ��Ч */
        				UartResponseMsg("$DUAIR,0,TR,007");
            			return (FALSE);
                    }

                    if (ulTemp == 1)
                    {
                        SI446X_SetSi446xPaPwrLvl(ulTemp1, SI446X_CHIP_A);
                    }
                    else
                    {
                        SI446X_SetSi446xPaPwrLvl(ulTemp1, SI446X_CHIP_B);
                    }

                    UartResponseMsg("$DUAIR,1,TR,007");
                }

			}

			/*lq ����������� */
			else if (strncmp(chTemp, "DBO", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);//��ȡ�ڶ��ֶε�����

				if (ucCheck == FALSE)
				{
    				/*lq �ֶ�Ϊ�� */
                    UartResponseMsg("$DUAIR,0,DBO");
					return (FALSE);
				}
                
                /*lq ������Ч���ж� */
                ucLen = strlen(chTemp);
        		if (ucLen != 1)
        		{
    				UartResponseMsg("$DUAIR,0,DBO");
        			return (FALSE);
        		}
        
        		/*lq �ַ���Ч���ж� */
                ulTemp = strtoul(chTemp, &pEnd, 10);
        		if ((*pEnd) != 0)
        		{
        		    /*lq �ַ���Ч */
    				UartResponseMsg("$DUAIR,0,DBO");
        			return (FALSE);
        		}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж� */
                    if (ulTemp < 0
                        || ulTemp > 1)
                    {
        		        /*lq ȡֵ��Ч */
        				UartResponseMsg("$DUAIR,0,DBO");
            			return (FALSE);
                    }
                }
                
                /*lq ִ������ */
                if (ulTemp == 1)
                {
                    DebugOutputFlag = TRUE;
                }
                else
                {
                    DebugOutputFlag = FALSE;                    
                }
        	    UartResponseMsg("$DUAIR,1,DBO");
            }			

            //�ָ���������
			else if (strncmp(chTemp, "RST", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);	
				}

				if (strncmp(chTemp, "%%Reset", 7) == FALSE)//ָ����Ч
				{
					ucCheck = EraseAllData();

					if (ucCheck == TRUE)
					{
						UartResponseMsg("$DUAIR,RST,1");
						UartResponseMsg("$712,ResetFinish........");
					}
					else
					{
						UartResponseMsg("$DUAIR,RST,0");
					}
				}
			}
					

			/**********************************************************************/
			/**********************************************************************/
			/**********************************************************************/
			/**********************************************************************/
			/**********************************************************************/

			else if (strncmp(chTemp, "050", 3) == FALSE)//�����ò�������
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);//��ȡ�ڶ��ֶε�����

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}	
                //δд��
			
			}
			
			//�������̬
			else if (strncmp(chTemp, "007", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}

				// ��ʾ�������̬
				if (strncmp(chTemp, "%%Test", 6) == FALSE)
				{
					//�������̬
					DevDynamicPara.ucWorkMode = EQ_WORKMOD_TEST;
                    DevDynamicPara.TestSta = EQ_MOD_TEST_STA_MAIN;

                    /*lq ����ģʽLED��ʼ�� */    
            		LEDInit(LED_TYPE_NONE);
            		LEDInit(LED_TYPE_POWER_TEST);
    
					UartResponseMsg("$P712,Testing........");
				}
			}
#if CONFIG_SD_CARD_MODULE_EN
			//�����ȡSD���ļ�״̬
			else if (strncmp(chTemp, "008", 3) == FALSE)
			{ 
				ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}

				// ��ʾ�����ȡSD���ļ�״̬
				if (strncmp(chTemp, "%%ReadFile", 10) == FALSE)
				{
					//�����ȡSD���ļ�״̬
					DevDynamicPara.ucWorkMode = EQ_WORKMOD_READFILE;

                    /* ��ȡSD���ļ�ģʽLED��ʼ�� */ 
					InitiaLED();					

					/* ����AIS��GPS�Ƿ�ʹ�� */
					CtrlAISAndGPS(0);												 // AIS��GPS��Ч
										
					UartResponseMsg("$P712,ReadFile........");
					Delay_ms(25);														//25ms��ʱ��

					/* ������1�Ĳ����ʸ�Ϊ115200 */  
    				InitCommStruc(1, 115200); 					
				}			
			}
#endif
			
			
			else if (strncmp(chTemp, "ReadID", 6) == FALSE)
			{ 
				
				ReadDeviceID(ulDeviceIDBuff);
				Feed_Dog();
				sprintf(chTemp2,"$P712,DeviceID1");
				for(ucTemp = 0;ucTemp < 5;ucTemp++)
				{ 
					sprintf(chTemp, ",%d",ulDeviceIDBuff[ucTemp]);		
					strcat(chTemp2, chTemp);					
				}
				UartResponseMsg(chTemp2);
				sprintf(chTemp2,"$P712,DeviceID2");
				for(ucTemp = 5;ucTemp < 10;ucTemp++)
				{ 
					sprintf(chTemp, ",%d",ulDeviceIDBuff[ucTemp]);		
					strcat(chTemp2, chTemp);					
				}
				UartResponseMsg(chTemp2);
							
			}

			else if (strncmp(chTemp, "WriteCry", 8) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
		
				if (ucCheck == FALSE)
				{
					return (FALSE);
				}
				
				for(ucTemp = 0;ucTemp < 3;ucTemp++)
				{
					WriteCry(ucTemp,chTemp);				 	
				}

				
				ucTemp = 0;
				ReadDeviceID(ulDeviceIDBuff);
				ulTemp2 = strtoul(chTemp, &pEnd, 10);
				do
				{
					ulTemp1 = ReadCry(ucTemp);
					Feed_Dog();
					ucTemp++;
				}while(ucTemp < 3 && ulTemp2 != ulTemp1);

			
				if(ulTemp2 == ulTemp1)
				{
					UartResponseMsg("$P712,WriteCry,1");
				}
				else
				{
					UartResponseMsg("$P712,WriteCry,0");
				}
			}
							
		}
		else if (DevDynamicPara.ucWorkMode == EQ_WORKMOD_TEST)
		{
			ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);
			if (ucCheck == FALSE)
			{
				return (FALSE);
			}

			//д�����к�
			if (strncmp(chTemp, "SNW", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
			
				ucLen = strlen(chTemp);

				if ((ucCheck == FALSE) || (ucLen != 8))
				{
					UartResponseMsg("$P712,0,SNW");
					return (FALSE);
				}

				//д�����к�
				//Para_Save(PARA_SERIAL_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_SERIAL_TYPE, (U8 *)chTemp,ucLen);

				UartResponseMsg("$P712,1,SNW");
				return (TRUE);
			}

			if (strncmp(chTemp, "007", 3) == TRUE)
			{
				return (FALSE);
			}

			ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);

			if (ucCheck == TRUE)
			{
				// ��ʾ�������̬
				if (strncmp(chTemp, "%%Test", 6) == FALSE)
				{
					//��ʼ������̬����
					IniTesting();
					
					//�������̬
					DevDynamicPara.ucWorkMode = EQ_WORKMOD_TEST;
                    DevDynamicPara.TestSta = EQ_MOD_TEST_STA_MAIN;

                    /*lq ����ģʽLED��ʼ�� */    
            		LEDInit(LED_TYPE_NONE);
            		LEDInit(LED_TYPE_POWER_TEST);

					UartResponseMsg("$P712,Testing........");

					return (TRUE);
				}
			}		

			ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
			
			if (ucCheck == FALSE)
			{
				return (FALSE);
			}
			
			//��ʱ϶�����ز�
			if (strncmp(chTemp, "001", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}

				if (strncmp(chTemp, "2087", 4) == FALSE)
				{
					channel = ENUM_AIS_CHANNEL_A;
				}
				else if (strncmp(chTemp, "2088", 4) == FALSE)
				{
					channel = ENUM_AIS_CHANNEL_B;
				}
				else
				{
					channel = ENUM_AIS_CHANNEL_A;
				}

                TEST_TxTestInit(channel, TRCTRL_TEST_TX_SIGNAL_CW,
                    TransceiverTypeA);

				UartResponseMsg("$P712,007,001,1");
			}
			
            //lq �����׼�ź�4��α����źţ�or���ղ���
			else if (strncmp(chTemp, "002", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}

				ucCheck = GetSegment((char *)pDat, chTemp2, 4, uiLen, 30);   //lq ͨ�����ֶε������롰��ʱ϶���͵������ݡ���������

				if (ucCheck == FALSE)   //lq ��ʱ϶���͵�������
				{
    				if (strncmp(chTemp, "2087", 4) == FALSE)
    				{
					    channel = ENUM_AIS_CHANNEL_A;
    				}
    				else if (strncmp(chTemp, "2088", 4) == FALSE)
    				{
					    channel = ENUM_AIS_CHANNEL_B;
    				}
    				else
    				{
					    channel = ENUM_AIS_CHANNEL_A;
    				}
    
                    TEST_TxTestInit(channel, TRCTRL_TEST_TX_SIGNAL_NUM3,
                        TransceiverTypeA);

    				UartResponseMsg("$P712,007,002,1"); 
				}   
                else    //lq ���ղ���    
                {
	                DevDynamicPara.TestSta = EQ_MOD_TEST_STA_RX;

                    SI446X_ModuleInit();                   

                    InitRxTestStruct(&RxTestInstance);
                    RxTestInstance.RxTestEn = TRUE;
    			    LEDInit(LED_TYPE_TEST_RX);                     
                    RxTxCtrl(RXTX_CTRL_TYPE_RX, 0, 0);                                        
    				UartResponseMsg("$P712,Receiving........");
                }
			}
			
            //����Ƶ��׼ȷ�ȵ���ʹ��
			else if (strncmp(chTemp, "003", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}

				if (strncmp(chTemp, "2087", 4) == FALSE)
				{
					ulTemp = EQ_XMTFREQ_CHA;
				}
				else if (strncmp(chTemp, "2088", 4) == FALSE)
				{
					ulTemp = EQ_XMTFREQ_CHB;
				}
				else
				{
					ulTemp = EQ_XMTFREQ_CHA;
				}

				Testing_SetTestType(TEST_TESTSTA_FREQADJEN, ulTemp);

				UartResponseMsg("$P712,007,003,1");
			}
			
            //����Ƶ��׼ȷ�ȵ���
			else if (strncmp(chTemp, "004", 3) == FALSE)
			{
				ucCheck = Testing_SetTestType(TEST_TESTSTA_FREQADJ, ulTemp);

				if (ucCheck == TRUE)
				{
					ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

					if (ucCheck == FALSE)
					{
						UartResponseMsg("$P712,007,004,0");
						return (FALSE);
					}
					
					ucCheck = IsAscStringOk(chTemp);

					if (ucCheck == FALSE)
					{
						UartResponseMsg("$P712,007,004,0");
						return (FALSE);
					}

					ucLen = strlen(chTemp);

					DevStaticPara.uiFreqAdj = atoi(chTemp);		//�ò�����Ҫ���ڵ��ԣ�����

					if (DevStaticPara.uiFreqAdj < 16 || DevStaticPara.uiFreqAdj > 48)
					{
						UartResponseMsg("$P712,007,004,0");
						return (FALSE);
					}

					//����Ƶ��׼ȷ�ȵ���ֵ
					//Para_Save(PARA_FREQADJ_TYPE, (U8 *)chTemp, ucLen);
					FlashWrite(STM32_FLASH_FREQADJ_TYPE, (U8 *)chTemp,ucLen);

//lq    				DevStaticPara.uiFreqOffset = (U16)(((U32)DevStaticPara.uiFreqAdj * EQ_MOD_OFFSET ) >> 5);
                    DevStaticPara.slXmtFreqComp = (S32)((DevStaticPara.uiFreqAdj - 32) * 125);   //lq ������ ���ʵ������

					UartResponseMsg("$P712,007,004,1");
				}
				else
				{
					UartResponseMsg("$P712,007,004,0");
				}
				
			}
			
            //������ȵ���ʹ��
			else if (strncmp(chTemp, "005", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}

				if (strncmp(chTemp, "2087", 4) == FALSE)
				{
					ulTemp = EQ_XMTFREQ_CHA;
				}
				else if (strncmp(chTemp, "2088", 4) == FALSE)
				{
					ulTemp = EQ_XMTFREQ_CHB;
				}
				else
				{
					ulTemp = EQ_XMTFREQ_CHA;
				}

				Testing_SetTestType(TEST_TESTSTA_MODADJEN, ulTemp);

				UartResponseMsg("$P712,007,005,1");
			}
			
            //������ȵ���
			else if (strncmp(chTemp, "006", 3) == FALSE)
			{
				ucCheck = Testing_SetTestType(TEST_TESTSTA_MODADJ, ulTemp);

				if (ucCheck == TRUE)
				{

					ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
	
					if (ucCheck == FALSE)
					{
						UartResponseMsg("$P712,007,006,0");
						return (FALSE);
					}
	
					ucCheck = IsAscStringOk(chTemp);
	
					if (ucCheck == FALSE)
					{
						UartResponseMsg("$P712,007,006,0");
						return (FALSE);
					}

					ucLen = strlen(chTemp);	

					DevStaticPara.uiModAmpAdj = atoi(chTemp);		//�ò�����Ҫ���ڵ��ԣ�����
	
					if (DevStaticPara.uiModAmpAdj < 16 || DevStaticPara.uiModAmpAdj > 48)
					{
						UartResponseMsg("$P712,007,006,0");
						return (FALSE);
					}
	
					//������Ʒ��ȵ���ֵ
					//Para_Save(PARA_MODAMP_TYPE, (U8 *)chTemp, ucLen);
					FlashWrite(STM32_FLASH_MODAMP_TYPE, (U8 *)chTemp,ucLen);
	
					DevStaticPara.uiModAmp = (U16)(((U32)DevStaticPara.uiModAmpAdj * EQ_MOD_AMP + 16) >> 5);
	
					UartResponseMsg("$P712,007,006,1");
				}
				else
				{
					UartResponseMsg("$P712,007,006,0");
				}
			}
			
            //ֹͣ����
			else if (strncmp(chTemp, "007", 3) == FALSE)
			{
				TEST_TxTestStop(TransceiverTypeA);
				UartResponseMsg("$P712,007,007,1");
			}
			
            //lq �����׼�ź�3��00001111��
			else if (strncmp(chTemp, "008", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
				    UartResponseMsg("$P712,007,008,0");					
                    return (FALSE);
				}

				if (strncmp(chTemp, "2087", 4) == FALSE)
				{
					channel = ENUM_AIS_CHANNEL_A;
				}
				else if (strncmp(chTemp, "2088", 4) == FALSE)
				{
					channel = ENUM_AIS_CHANNEL_B;
				}
				else
				{
					channel = ENUM_AIS_CHANNEL_A;
				}

                TEST_TxTestInit(channel, TRCTRL_TEST_TX_SIGNAL_NUM2,
                    TransceiverTypeA);

				UartResponseMsg("$P712,007,008,1");
			}

#if CONFIG_PARASTORAGE_BTT
			//��ѯ��ص���ģ��ģʽ����ʱ��
			else if (strncmp(chTemp, "011", 3) == FALSE)
			{
				Para_Read(PARA_BATTERY_TEST_TIME_TYPE, ucTempBuf);

				sprintf(chRetBuf, "$P712,007,011,");
				strcat(chRetBuf,(char *)ucTempBuf);

				UartResponseMsg(chRetBuf);
			}
#endif			
            //lq ���ͱ�׼�ź�2(01010101)
			else if (strncmp(chTemp, "012", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
				    UartResponseMsg("$P712,007,012,0");					
                    return (FALSE);
				}

				if (strncmp(chTemp, "2087", 4) == FALSE)
				{
					channel = ENUM_AIS_CHANNEL_A;
				}
				else if (strncmp(chTemp, "2088", 4) == FALSE)
				{
					channel = ENUM_AIS_CHANNEL_B;
				}
				else
				{
					channel = ENUM_AIS_CHANNEL_A;
				}

                TEST_TxTestInit(channel, TRCTRL_TEST_TX_SIGNAL_NUM1,
                    TransceiverTypeA);

				UartResponseMsg("$P712,007,012,1");
			}
            //դѹ���Ƶ���ʹ��
			else if (strncmp(chTemp, "015", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}

				if (strncmp(chTemp, "2087", 4) == FALSE)
				{
					ulTemp = EQ_XMTFREQ_CHA;
				}
				else if (strncmp(chTemp, "2088", 4) == FALSE)
				{
					ulTemp = EQ_XMTFREQ_CHB;
				}
				else
				{
					ulTemp = EQ_XMTFREQ_CHA;
				}

				Testing_SetTestType(TEST_TESTSTA_GRIDADJEN, ulTemp);

				UartResponseMsg("$P712,007,015,1");
			} 

            //դѹ���Ƶ���
			else if (strncmp(chTemp, "016", 3) == FALSE)
			{
				ucCheck = Testing_SetTestType(TEST_TESTSTA_GRIDADJ, ulTemp);

				if (ucCheck == TRUE)
				{

					ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
	
					if (ucCheck == FALSE)
					{
						UartResponseMsg("$P712,007,016,0");
						return (FALSE);
					}
	
					ucCheck = IsAscStringOk(chTemp);
	
					if (ucCheck == FALSE)
					{
						UartResponseMsg("$P712,007,016,0");
						return (FALSE);
					}

					ucLen = strlen(chTemp);	

					DevStaticPara.GridVoltAdj = atoi(chTemp);		//�ò�����Ҫ���ڵ��ԣ�����
	
					if (DevStaticPara.GridVoltAdj < STM32_FLASH_RF_VOLT_ADJUST_FLOOR 
                        || DevStaticPara.GridVoltAdj > STM32_FLASH_RF_VOLT_ADJUST_CEIL)
					{
						UartResponseMsg("$P712,007,016,0");
						return (FALSE);
					}
	
					//������Ʒ��ȵ���ֵ
					//Para_Save(PARA_RF_VOLT_ADJUST_TYPE, (U8 *)chTemp, ucLen);
					FlashWrite(STM32_FLASH_RF_VOLT_ADJUST_TYPE, (U8 *)chTemp,ucLen);
	
                    DevStaticPara.GridVoltOffset = (DevStaticPara.GridVoltAdj - 10) * DA_VALUE_01V;
	
					UartResponseMsg("$P712,007,016,1");
				}
				else
				{
					UartResponseMsg("$P712,007,016,0");
				}
			} 
#if 0            
            //����SART��׼�ź�2:00001111
			else if (strncmp(chTemp, "017", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
				    UartResponseMsg("$P712,007,017,0");					
                    return (FALSE);
				}

				if (strncmp(chTemp, "2087", 4) == FALSE)
				{
					ulTemp = EQ_XMTFREQ_CHA;
				}
				else if (strncmp(chTemp, "2088", 4) == FALSE)
				{
					ulTemp = EQ_XMTFREQ_CHB;
				}
				else
				{
					ulTemp = EQ_XMTFREQ_CHA;
				}

				Testing_SetTestType(TEST_TESTSTA_STSIGNAL2, ulTemp);
				UartResponseMsg("$P712,007,017,1");
			}
#endif            
            //�����豸�Լ�ģʽ
			else if (strncmp(chTemp, "018", 3) == FALSE)
			{
    			ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
    			if (ucCheck == FALSE)
    			{
                    UartResponseMsg("$P712,007,018,0");
    				return (FALSE);
    			}
        
                /*lq ������Ч���ж�*/
                ucLen = strlen(chTemp);
        		if (ucLen != 8)
        		{
                    UartResponseMsg("$P712,007,018,0");
        			return (FALSE);
        		}
        
                ulTemp = strtoul(chTemp, &pEnd, 16);
        
        		/*lq �ַ���Ч���ж�*/
        		if ((*pEnd) != 0)
        		{
                    UartResponseMsg("$P712,007,018,0");
        			return (FALSE);
        		}

	            DevDynamicPara.TestSta = EQ_MOD_TEST_STA_SELF_CHECK;
			
				SLFCHK_InstSelfCheckStructInit();
                SLFCHK_InstCfgCheckSelect(ulTemp);
                SLFCHK_InstSelfCheckStart();
                LEDInit(LED_TYPE_SELF_CHECK_IDLE);                                
				
                UartResponseMsg("$P712,007,018,1");
			}                                              					
		}
#if CONFIG_SD_CARD_MODULE_EN		
		/********************��ȡSD���ļ�ģʽ*******************************************/
		else if (DevDynamicPara.ucWorkMode == EQ_WORKMOD_READFILE)
		{  
			ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);

			if (ucCheck == FALSE)
			{
				return (FALSE);
			}

			if (strncmp(chTemp, "001", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}

				if(strncmp(chTemp, "ALL", 3) == FALSE)
				{
					LED_RED_ON(); 
					ReadFileName(0);
					LED_RED_OFF();
				}
				else
				{
					LED_RED_ON(); 
					ReadFileName(atoi(chTemp));
					LED_RED_OFF();
				}
			}
			else if (strncmp(chTemp, "002", 3) == FALSE) 
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}
				LED_RED_ON();
				ReadFile(chTemp);
				LED_RED_OFF();
			}
			else if (strncmp(chTemp, "003", 3) == FALSE)
			{
				LED_RED_ON();
				ReadMulFile(1);
				LED_RED_OFF();
			}
			else if (strncmp(chTemp, "004", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}
				LED_RED_ON();
				ReadMulFile(atoi(chTemp));
				LED_RED_OFF();
			}
		}
#endif	
	}

    //===================================================================================
    /*lq DUAIQ��ͷ��������ѯ*/
	//  Process  type  2      :  note by Nathan
    else if (strncmp((char *)pDat, "DUAIQ", 5) == FALSE)
	{
		/*lq ��ȡ��1�ֶ����ݣ��жϵ�ǰ���� */
		ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);

		if (ucCheck == FALSE)
		{
			return (FALSE);
		}

		if (DevDynamicPara.ucWorkMode == EQ_WORKMOD_NORMAL)
		{
            if (FALSE)
            {}
            /*lq SSD ��ѯ*/
			else if (strncmp(chTemp, "SSD", 3) == FALSE)
			{
                /*lq ����ַ�ֶ� */
				sprintf(chRetBuf, "$AISSD");

				/*lq... Call sign �����ֶ���Ϊ�գ�EEPROM��δ�洢�� */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
#if 0
				Para_Read(PARA_CALL_SIGN_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);
#endif

				/*lq Ship Name */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				//Para_Read(PARA_SHIP_NAME_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_SHIP_NAME_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq ShipA */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				//Para_Read(PARA_SHIPA_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_SHIPA_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq ShipB */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				//Para_Read(PARA_SHIPB_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_SHIPB_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq ShipC */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				//Para_Read(PARA_SHIPC_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_SHIPC_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq ShipD */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				//Para_Read(PARA_SHIPD_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_SHIPD_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq DTE */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				//Para_Read(PARA_DTE_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_DTE_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq... Source identifier ��EEPROM��δ�洢��*/
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
#if 0
				Para_Read(PARA_TALK_ID_TYPE, ucTempBuf);
#else
				sprintf((char *)ucTempBuf, AIS_DI_TALKER_ID_DEFAULT);
#endif	
    			strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);		
			}		

            /*lq VSD ��ѯ*/
			else if (strncmp(chTemp, "VSD", 3) == FALSE)
			{
                /*lq ����ַ�ֶ� */
				sprintf(chRetBuf, "$AIVSD");

                /*lq Type of ship and cargo category */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				//Para_Read(PARA_SHIP_TYPE_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_SHIP_TYPE_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq Maximum present static draught */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				//Para_Read(PARA_MAX_DRAUGHT_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_MAX_DRAUGHT_TYPE, ucTempBuf);
				sprintf(chTemp, "%.1f", strtod(ucTempBuf, &pEnd) / 10.0);
                strcat(chRetBuf, (char *)chTemp);

				/*lq Persons on-board */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				Para_Read(PARA_PERSONS_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq Destination */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				//Para_Read(PARA_DESTINATION_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_DESTINATION_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq Estimated UTC of arrival at destination */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				Para_Read(PARA_ETA_HOUR_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);
				Para_Read(PARA_ETA_MINUTE_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);
				sprintf(chTemp, "00");  //lq �룬EEPROM�в��洢��AIS��Ϣ�в�ʹ��
				strcat(chRetBuf, chTemp);

				/*lq Estimated day of arrival at destination */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				Para_Read(PARA_ETA_DAY_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq Estimated month of arrival at destination */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				Para_Read(PARA_ETA_MONTH_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq Navigational status */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				Para_Read(PARA_NAVIGATION_STATUS_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq Regional application flags */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				Para_Read(PARA_REGION_APP_FLAG_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);		
			}
			
			//��ѯ MMSI ��712��ʽ
			else if (strncmp(chTemp, "010", 3) == FALSE)
			{
				//��EEPROM�ж�ȡMMSI��
				//Para_Read(PARA_MMSI_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_MMSI_TYPE, ucTempBuf);
				ulTemp = atol((char *)ucTempBuf);

				if (ulTemp == 0)
				{
					//Para_Read(PARA_MMSI_BACKUP_TYPE, ucTempBuf);//������
					FlashRead(STM32_FLASH_MMSI_DEST_TYPE, ucTempBuf);
					ulTemp = atol((char *)ucTempBuf);
					
					if (ulTemp != 0)
					{
						ucLen = strlen((char *)ucTempBuf);
						//Para_Save(PARA_MMSI_TYPE, ucTempBuf,ucLen);//��д
						FlashWrite(STM32_FLASH_MMSI_TYPE, ucTempBuf,ucLen);
					}

				}//���ܶ��ĶԲ��ԣ���������
				
				//�鱨�ķ���MMSI��
				sprintf(chRetBuf, "$P712,010,,,");
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);		
			}

            //��ѯ���к�
			else if (strncmp(chTemp, "SNO", 3) == FALSE)
			{
				//��EEPROM�ж�ȡSERIAL
				//Para_Read(PARA_SERIAL_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_SERIAL_TYPE, ucTempBuf);
				
				//�鱨�ķ���SERIAL
				sprintf(chRetBuf, "$P712,SNO,");
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);		
			}
			
            //��ѯ�豸����������汾�Ų�ѯ
			else if (strncmp(chTemp, "SWF", 3) == FALSE)
			{
				sprintf(chTemp, "$P712,SWF,%s", VERSION);
				UartResponseMsg(chTemp);		
			}
			
            //�鴬������
			else if (strncmp(chTemp, "SRM", 3) == FALSE)
			{
				//��EEPROM�ж�ȡPOWERMOD
				//Para_Read(PARA_VENDID_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_VENDID_TYPE, ucTempBuf);
				
				//�鱨�ķ���POWERMOD
				sprintf(chRetBuf, "$P712,SRM,,,");
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);	
			}	
			
			//��ѯ��������
			else if (strncmp(chTemp, "CKS", 3) == FALSE)
			{
				ucTemp=GetGpsSleepSta();
				usart_printf("ucGpsSta %d\r\n",ucTemp);
#if 0
				ucTemp=GetDigUnitSleepSta();
				usart_printf("ucDigUnitSta %d\r\n",ucTemp);
#endif
#if CONFIG_KEY
				ucTemp=GetMMISleepSta();
				usart_printf("ucMMISta %d\r\n",ucTemp);
#endif
#if CONFIG_POWERDOWN
				ucTemp=GetPowerDownSleepSta();
#endif
				usart_printf("ucPowerDownSta %d\r\n",ucTemp);
#if 0
				ucTemp=GetPtlFifoSleepSta();
				usart_printf("ucPtlFifoSta %d\r\n",ucTemp);		
#endif
			}

			//��ѯ��������
			else if (strncmp(chTemp, "CKW", 3) == FALSE)
			{
				 usart_printf("wakeup frame %d\r\n",SleepCtrlPara.WakeUpFrame);
				 usart_printf("now frame %d\r\n",gslFrameCnt);
				 usart_printf("wakeup slot %d\r\n",SleepCtrlPara.WakeUpSlot);
				 usart_printf("now slot %d\r\n",gsiSlotCnt);
			}

			//��ѯ����ʱ��
			else if (strncmp(chTemp, "TOP", 3) == FALSE)//���ر��ĸ�ʽ $P712,TOP,i,YYYY-mm-DD HH:MM:SS
			{
				for (i = 0; i < PARA_TIME_OPEN_CLOSE_TOTAL; i++)
                {
    				/*lq ���㿪��ʱ��������� */
                    ulTemp = PARA_TIME_OPEN_0_TYPE + ((DevDynamicPara.OpenCloseTimeIndex + 1 + i)
                        % PARA_TIME_OPEN_CLOSE_TOTAL);

                    /*lq ��EEPROM�ж�ȡ����ʱ�� */
                    Para_Read(ulTemp, ucTempBuf);
                    
    				/*lq �鱨�ķ��ؿ���ʱ�� */
    				sprintf(chRetBuf, "$P712,TOP,%d,20", PARA_TIME_OPEN_CLOSE_TOTAL - 1 - i);
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[0]), 2);  //lq year
    				strcat((char *)chRetBuf, "-");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[2]), 2);  //lq month
    				strcat((char *)chRetBuf, "-");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[4]), 2);  //lq day
    				strcat((char *)chRetBuf, " ");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[6]), 2);  //lq hour
    				strcat((char *)chRetBuf, ":");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[8]), 2);  //lq minute
    				strcat((char *)chRetBuf, ":");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[10]), 2); //lq second
    
    				UartResponseMsg(chRetBuf);	
                }				
			} 

			//��ѯ��һ��GPS��λʱ��
			else if (strncmp(chTemp, "GPSFT", 5) == FALSE)//���ر��ĸ�ʽ $P712,GPSFT,YYYY-mm-DD HH:MM:SS
			{ 
                /* ��EEPROM�ж�ȡ��һ��GPS��λʱ�� */
                Para_Read(PARA_GPS_FIRST_TIME_TYPE, ucTempBuf);
                   
   				/* �鱨�ķ��ؿ���ʱ�� */
   				sprintf(chRetBuf, "$P712,GPSFT,20");
   				strncat((char *)chRetBuf, (char *)(&ucTempBuf[0]), 2);  //lq year
   				strcat((char *)chRetBuf, "-");
   				strncat((char *)chRetBuf, (char *)(&ucTempBuf[2]), 2);  //lq month
   				strcat((char *)chRetBuf, "-");
   				strncat((char *)chRetBuf, (char *)(&ucTempBuf[4]), 2);  //lq day
   				strcat((char *)chRetBuf, " ");
   				strncat((char *)chRetBuf, (char *)(&ucTempBuf[6]), 2);  //lq hour
   				strcat((char *)chRetBuf, ":");
   				strncat((char *)chRetBuf, (char *)(&ucTempBuf[8]), 2);  //lq minute
   				strcat((char *)chRetBuf, ":");
   				strncat((char *)chRetBuf, (char *)(&ucTempBuf[10]), 2); //lq second
   
   				UartResponseMsg(chRetBuf);					
			}

			//��ѯ�ػ�ʱ��
			else if (strncmp(chTemp, "TST", 3) == FALSE)//���ر��ĸ�ʽ $P712,TST,i,YYYY-mm-DD HH:MM:SS
			{
				for (i = 0; i < PARA_TIME_OPEN_CLOSE_TOTAL; i++)
                {
    				/*lq ����ػ�ʱ��������� */
                    ulTemp = PARA_TIME_CLOSE_0_TYPE + ((DevDynamicPara.OpenCloseTimeIndex + 1 + i)
                        % PARA_TIME_OPEN_CLOSE_TOTAL);

                    /*lq ��EEPROM�ж�ȡ�ػ�ʱ�� */
                    Para_Read(ulTemp, ucTempBuf);
                    
    				/*lq �鱨�ķ��عػ�ʱ�� */
    				sprintf(chRetBuf, "$P712,TST,%d,20", PARA_TIME_OPEN_CLOSE_TOTAL - 1 - i);
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[0]), 2);  //lq year
    				strcat((char *)chRetBuf, "-");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[2]), 2);  //lq month
    				strcat((char *)chRetBuf, "-");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[4]), 2);  //lq day
    				strcat((char *)chRetBuf, " ");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[6]), 2);  //lq hour
    				strcat((char *)chRetBuf, ":");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[8]), 2);  //lq minute
    				strcat((char *)chRetBuf, ":");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[10]), 2); //lq second
    
    				UartResponseMsg(chRetBuf);	
                }				
			}
            //���ο��Ź���λ������ѯ
			else if (strncmp(chTemp, "CWRC", 4) == FALSE)
			{
				Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);

				sprintf(chRetBuf, "$P712,CWRC,");
				strcat(chRetBuf,(char *)ucTempBuf);

				UartResponseMsg(chRetBuf);
			}
            
			//�ϴο��Ź���λ������ѯ
			else if (strncmp(chTemp, "LWRC", 4) == FALSE)
			{
				Para_Read(PARA_LAST_WDG_RESET_CNT_TYPE, ucTempBuf);

				sprintf(chRetBuf, "$P712,LWRC,");
				strcat(chRetBuf,(char *)ucTempBuf);

				UartResponseMsg(chRetBuf);
			} 
           
			//��������������ѯ
			else if (strncmp(chTemp, "NPOC", 4) == FALSE)
			{
				Para_Read(PARA_NORMAL_POWER_ON_CNT_TYPE, ucTempBuf);

				sprintf(chRetBuf, "$P712,NPOC,");
				strcat(chRetBuf,(char *)ucTempBuf);

				UartResponseMsg(chRetBuf);
			}  
#if 0 
			//�û����Դ�����ѯ
			else if (strncmp(chTemp, "STC", 3) == FALSE)
			{
				Para_Read(PARA_USER_TEST_CNT_TYPE, ucTempBuf);

				sprintf(chRetBuf, "$P712,STC,");
				strcat(chRetBuf,(char *)ucTempBuf);

				UartResponseMsg(chRetBuf);
			} 

            //GPS5min����������ѯ
			else if (strncmp(chTemp, "GLOC", 4) == FALSE)
			{
				Para_Read(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTempBuf);

				sprintf(chRetBuf, "$P712,GLOC,");
				strcat(chRetBuf,(char *)ucTempBuf);

				UartResponseMsg(chRetBuf);
			}
#endif
			//������ʱ����ѯ
			else if (strncmp(chTemp, "WTS", 3) == FALSE)
			{
				Para_Read(PARA_WORKTIME_TYPE, ucTempBuf);

				sprintf(chRetBuf, "$P712,WTS,");
				strcat(chRetBuf,(char *)ucTempBuf);

				UartResponseMsg(chRetBuf);
			}           

			//�ϴο�������ʱ����ѯ
			else if (strncmp(chTemp, "LWT", 3) == FALSE)
			{
				Para_Read(PARA_WORKTIME_LAST_TYPE, ucTempBuf);

				sprintf(chRetBuf, "$P712,LWT,");
				strcat(chRetBuf,(char *)ucTempBuf);

				UartResponseMsg(chRetBuf);
			}

			//�����ܾ����ѯ
			else if (strncmp(chTemp, "SAILDISINCE", 11) == FALSE)
			{
				Para_Read(PARA_SAILING_DISTINCE_TYPE, ucTempBuf);

				sprintf(chRetBuf, "$P712,SAILDISINCE,");
				strcat(chRetBuf,(char *)ucTempBuf);

				UartResponseMsg(chRetBuf);
			}

#if CONFIG_TEMPERATURE_MONITOR
			//��ǰ�¶Ȳ�ѯ
			else if (strncmp(chTemp, "TEMP", 4) == FALSE)
			{
#if 0
                sprintf(chRetBuf, "$P712,TEMP,%.4f", GetTemper());  //lq? Ϊ��GetTemper()��������ֵ����
#else
                sprintf(chRetBuf, "$P712,TEMP,%.4f", struTemperSensorPara.fsTemper);
#endif
				UartResponseMsg(chRetBuf);
			}
#endif
            //��ѯ�豸������Ӳ���汾�Ų�ѯ
			else if (strncmp(chTemp, "HWF", 3) == FALSE)
			{
				//Para_Read(PARA_HWVERSION_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_HWVERSION_TYPE, ucTempBuf);
				sprintf(chRetBuf, "$P712,HWF,");
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);		
			}                                                                                      			

            //��ѯ�豸������AIS����ѯ
			else if (strncmp(chTemp, "AISCLASS", 8) == FALSE)
			{
				//Para_Read(PARA_AIS_CLASS_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_AIS_CLASS_TYPE, ucTempBuf);
				sprintf(chRetBuf, "$P712,AISCLASS,");
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);		
			}                                                                                      			

            //��ѯTransceiver״̬
            else if (strncmp(chTemp, "TR", 2) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);//��ȡ�ڶ��ֶε�����
				if (ucCheck == FALSE)
				{
					return (FALSE);
				}	

				/*lq RSSI */
				if (strncmp(chTemp, "RSSI", 4) == FALSE)
				{
    				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);//��ȡ��3�ֶε�����    
    				if (ucCheck == FALSE)
    				{
                        return (FALSE);
    				}

                    /*lq ������Ч���ж�*/
                    ucLen = strlen(chTemp);
        			if (ucLen != 1)
        			{
        				return (FALSE);
        			}
        
        			/*lq �ַ���Ч���ж�*/
                    ucCheck = IsAscStringOk(chTemp);
        			if (ucCheck == FALSE)
        			{
        				return (FALSE);
        			}
                    else
                    {
                        /*lq ȡֵ��Χ��Ч���ж�*/
                        ulTemp = strtoul(chTemp, &pEnd, 10);
                        if (ulTemp == 1)
                        {
                            sprintf(chRetBuf, "$P712,TR,RSSI,1");
            
            				/*lq RSSI */
                            sprintf(ucTempBuf, "%d", SI446X_GetRssi(SI446X_CHIP_A));
            				strcat(chRetBuf, ",");               
                            strcat(chRetBuf, (char *)ucTempBuf);
                                                                                                                                                        
            				UartResponseMsg(chRetBuf);                        
                        }
                        else if (ulTemp == 2)
                        {
                            sprintf(chRetBuf, "$P712,TR,RSSI,2");
            
            				/*lq RSSI */
                            sprintf(ucTempBuf, "%d", SI446X_GetRssi(SI446X_CHIP_B));
            				strcat(chRetBuf, ",");               
                            strcat(chRetBuf, (char *)ucTempBuf);
                                                                                                                                                        
            				UartResponseMsg(chRetBuf);                        
                        }
                        else
                        {
                            return (FALSE);                        
                        }
                    }
                }
				
                /*lq machine state */
				if (strncmp(chTemp, "006", 3) == FALSE)
				{
    				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);//��ȡ��3�ֶε�����    
    				if (ucCheck == FALSE)
    				{
                        return (FALSE);
    				}

                    /*lq ������Ч���ж�*/
                    ucLen = strlen(chTemp);
        			if (ucLen != 1)
        			{
        				return (FALSE);
        			}
        
        			/*lq �ַ���Ч���ж�*/
                    ucCheck = IsAscStringOk(chTemp);
        			if (ucCheck == FALSE)
        			{
        				return (FALSE);
        			}
                    else
                    {
                        /*lq ȡֵ��Χ��Ч���ж�*/
                        ulTemp = strtoul(chTemp, &pEnd, 10);
                        if (ulTemp == 1)
                        {
                            sprintf(chRetBuf, "$P712,TR,006,1");
            
            				/*lq ״̬��ѯ */
                            SI446X_REQUEST_DEVICE_STATE(buffer, SI446X_CHIP_A);
                            sprintf(ucTempBuf, "%d", buffer[1]);
            				strcat(chRetBuf, ",");               
                            strcat(chRetBuf, (char *)ucTempBuf);
                                                                                                                                                        
            				UartResponseMsg(chRetBuf);                        
                        }
                        else if (ulTemp == 2)
                        {
                            sprintf(chRetBuf, "$P712,TR,006,2");
            
            				/*lq ״̬��ѯ */
                            SI446X_REQUEST_DEVICE_STATE(buffer, SI446X_CHIP_B);
                            sprintf(ucTempBuf, "%d", buffer[1]);
            				strcat(chRetBuf, ",");               
                            strcat(chRetBuf, (char *)ucTempBuf);
                                                                                                                                                        
            				UartResponseMsg(chRetBuf);                                                
                        }
                        else
                        {
                            return (FALSE);                        
                        }
                    }
                }

                /*lq SI4463��ǰ�������ֵ */
				if (strncmp(chTemp, "007", 3) == FALSE)
				{
    				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);//��ȡ��3�ֶε�����    
    				if (ucCheck == FALSE)
    				{
                        return (FALSE);
    				}

                    /*lq ������Ч���ж�*/
                    ucLen = strlen(chTemp);
        			if (ucLen != 1)
        			{
            		    /*lq ������Ч */
        				return (FALSE);
        			}

            		/*lq �ַ���Ч���ж� */
                    ulTemp = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq �ַ���Ч */
            			return (FALSE);
            		}

                    /*lq ȡֵ��Χ��Ч���ж� */
                    if ((ulTemp < 1) || (ulTemp > 2))
                    {
            		    /*lq ȡֵ��Ч */
            			return (FALSE);
                    }
        
        			if (ulTemp == 1)
                    {
                        ulTemp1 = SI446X_GetSi446xPaPwrLvl(SI446X_CHIP_A);
                    }
                    else
                    {
                        ulTemp1 = SI446X_GetSi446xPaPwrLvl(SI446X_CHIP_B);                    
                    }

                    sprintf(chRetBuf, "$P712,TR,007");  
                    sprintf(ucTempBuf, "%d", ulTemp);
    				strcat(chRetBuf, ",");               
                    strcat(chRetBuf, (char *)ucTempBuf);
                    sprintf(ucTempBuf, "%d", ulTemp1);
    				strcat(chRetBuf, ",");               
                    strcat(chRetBuf, (char *)ucTempBuf);
                                                                                                                                                
    				UartResponseMsg(chRetBuf);
                }

                /*lq ��ѯ */
                //lq...

                else
                {
                    return (FALSE);                                        
                }                                                                                                                                           			
            }
			
            /*lq ��ѯ�������״̬ */
			else if (strncmp(chTemp, "DBO", 3) == FALSE)
			{
                if (DebugOutputFlag == TRUE)
                {
                    sprintf(chRetBuf, "$P712,DBO,1");
                }
                else
                {
                    sprintf(chRetBuf, "$P712,DBO,0");                
                }
				
                UartResponseMsg(chRetBuf);		
			}

            /*lq ��ѯGPS�������Ƶ�� */
			else if (strncmp(chTemp, "GPSOP", 5) == FALSE)
			{
                /*lq ���ͷ */
				sprintf(chRetBuf, "$P712,GPSOP");

				/*lq RMC */
                sprintf(chTemp, ",%03d", DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_RMC]);
				strcat(chRetBuf, chTemp);

				/*lq VTG */
                sprintf(chTemp, ",%03d", DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_VTG]);
				strcat(chRetBuf, chTemp);

				/*lq GGA */
                sprintf(chTemp, ",%03d", DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GGA]);
				strcat(chRetBuf, chTemp);

				/*lq GSA */
                sprintf(chTemp, ",%03d", DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSA]);
				strcat(chRetBuf, chTemp);

				/*lq GSV */
                sprintf(chTemp, ",%03d", DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSV]);
				strcat(chRetBuf, chTemp);

				/*lq GLL */
                sprintf(chTemp, ",%03d", DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GLL]);
				strcat(chRetBuf, chTemp);

				/*lq GBS */
                sprintf(chTemp, ",%03d", DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GBS]);
				strcat(chRetBuf, chTemp);

                UartResponseMsg(chRetBuf);		
			}

            /***************************************************************************/
			/***************************************************************************/
			/***************************************************************************/
			/***************************************************************************/
			
			//������ʱ�������ѯ
			else if (strncmp(chTemp, "050", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}
                //δд��
			
            }
		}
		else if (DevDynamicPara.ucWorkMode == EQ_WORKMOD_TEST)
		{
			//��ѯƵ��׼ȷ��
			if (strncmp(chTemp, "FRE", 3) == FALSE)
			{
				//��EEPROM�ж�ȡPOWERMOD
				//Para_Read(PARA_FREQADJ_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_FREQADJ_TYPE, ucTempBuf);
				
				//�鱨�ķ���POWERMOD
				sprintf(chRetBuf, "$P712,FREQ,");
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);		
			}
			
            //��ѯ���Ʒ���
			else if (strncmp(chTemp, "AMP", 3) == FALSE)
			{
				//��EEPROM�ж�ȡPOWERMOD
				//Para_Read(PARA_MODAMP_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_MODAMP_TYPE, ucTempBuf);
				
				//�鱨�ķ���POWERMOD
				sprintf(chRetBuf, "$P712,AMP,");
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);	
			}

            //��ѯդѹ����
			else if (strncmp(chTemp, "GRID", 4) == FALSE)
			{
				//��EEPROM�ж�ȡPOWERMOD
				//Para_Read(PARA_RF_VOLT_ADJUST_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_RF_VOLT_ADJUST_TYPE, ucTempBuf);
				
				//�鱨�ķ���POWERMOD
				sprintf(chRetBuf, "$P712,GRID,");
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);	
			}			
            //��ѯ���к�
			else if (strncmp(chTemp, "SNO", 3) == FALSE)
			{
				//��EEPROM�ж�ȡSERIAL
				//Para_Read(PARA_SERIAL_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_SERIAL_TYPE, ucTempBuf);
				
				//�鱨�ķ���SERIAL
				sprintf(chRetBuf, "$P712,SNO,");
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);	
			}
		}
	}
    //=====================================================================================
    /*lq AISSD��ͷ����̬����*/
	//  Process  type  3     :  note by Nathan
	else if (strncmp((char *)pDat, "AISSD", 5) == FALSE)
	{
		if (DevDynamicPara.ucWorkMode == EQ_WORKMOD_NORMAL)
		{
			/*lq **********************Ship name**************************** */
			ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
			
            /*lq �ֶηǿ�*/			
            if (ucCheck != FALSE)
			{
    			ucLen = strlen(chTemp);
    			if (ucLen > AIS_INFO_SHIP_NAME_LEN)
    			{
    				UartResponseMsg("$DUAIR,0,SSD");
    				return (FALSE);
    			}
    
    			/*lq д��EEPROM */
    			//Para_Save(PARA_SHIP_NAME_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_SHIP_NAME_TYPE, (U8 *)chTemp,ucLen);
    
    			/*lq ���±��� */
    			strcpy((char *)&AisStaticPara.ShipName[0], AIS_INFO_SHIP_NAME_DEFAULT);     //lq ʹ��Ĭ��ֵ��@����д�����ַ�
    			strncpy((char *)&AisStaticPara.ShipName[0], chTemp, ucLen);                 //lq ʹ��Ŀ��ֵ����ǰ��ġ�@��

                AisStaticPara.Refresh = TRUE;
			}

			/*lq ************************ShipA****************************** */
			ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

            /*lq �ֶηǿ�*/			
			if (ucCheck != FALSE)
			{
                /*lq ������Ч���ж�*/
                ucLen = strlen(chTemp);
        		if (ucLen != 3)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
        
        		/*lq �ַ���Ч���ж�*/
                ulTemp = strtoul(chTemp, &pEnd, 10);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < STM32_FLASH_SHIPA_FLOOR
                        || ulTemp > STM32_FLASH_SHIPA_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,SSD");
            			return (FALSE);
                    }
                }
    
    			/*lq д��EEPROM */
    			//Para_Save(PARA_SHIPA_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_SHIPA_TYPE, (U8 *)chTemp,ucLen);
    
    			/*lq ���±��� */
    			AisStaticPara.ShipA = ulTemp;
			
                AisStaticPara.Refresh = TRUE;
            }

			/*lq ************************ShipB****************************** */
			ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);

            /*lq �ֶηǿ�*/			
			if (ucCheck != FALSE)
			{
                /*lq ������Ч���ж�*/
                ucLen = strlen(chTemp);
        		if (ucLen != 3)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
        
        		/*lq �ַ���Ч���ж�*/
                ulTemp = strtoul(chTemp, &pEnd, 10);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < STM32_FLASH_SHIPB_FLOOR
                        || ulTemp > STM32_FLASH_SHIPB_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,SSD");
            			return (FALSE);
                    }
                }
                    
    			/*lq д��EEPROM */
    			//Para_Save(PARA_SHIPB_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_SHIPB_TYPE, (U8 *)chTemp,ucLen);
    
    			/*lq ���±��� */
    			AisStaticPara.ShipB = ulTemp;
			
                AisStaticPara.Refresh = TRUE;
			}

			/*lq ************************ShipC****************************** */
			ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);

            /*lq �ֶηǿ�*/			
			if (ucCheck != FALSE)
			{
                /*lq ������Ч���ж�*/
                ucLen = strlen(chTemp);
        		if (ucLen != 2)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
        
        		/*lq �ַ���Ч���ж�*/
                ulTemp = strtoul(chTemp, &pEnd, 10);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < STM32_FLASH_SHIPC_FLOOR
                        || ulTemp > STM32_FLASH_SHIPC_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,SSD");
            			return (FALSE);
                    }
                }
                    
    			/*lq д��EEPROM */
    			//Para_Save(PARA_SHIPC_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_SHIPC_TYPE, (U8 *)chTemp,ucLen);
    
    			/*lq ���±��� */
    			AisStaticPara.ShipC = ulTemp;
			
                AisStaticPara.Refresh = TRUE;
			}

			/*lq ************************ShipD****************************** */
			ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 30);

            /*lq �ֶηǿ�*/			
			if (ucCheck != FALSE)
			{
                /*lq ������Ч���ж�*/
                ucLen = strlen(chTemp);
        		if (ucLen != 2)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
        
        		/*lq �ַ���Ч���ж�*/
                ulTemp = strtoul(chTemp, &pEnd, 10);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < STM32_FLASH_SHIPD_FLOOR
                        || ulTemp > STM32_FLASH_SHIPD_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,SSD");
            			return (FALSE);
                    }
                }
                    
    			/*lq д��EEPROM */
    			//Para_Save(PARA_SHIPD_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_SHIPD_TYPE, (U8 *)chTemp,ucLen);
    
    			/*lq ���±��� */
    			AisStaticPara.ShipD = ulTemp;
			
                AisStaticPara.Refresh = TRUE;
			}

			/*lq *************************DTE******************************* */
			ucCheck = GetSegment((char *)pDat, chTemp, 7, uiLen, 30);

            /*lq �ֶηǿ�*/			
			if (ucCheck != FALSE)
			{
                /*lq ������Ч���ж�*/
                ucLen = strlen(chTemp);
        		if (ucLen != 1)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
        
        		/*lq �ַ���Ч���ж�*/
                ulTemp = strtoul(chTemp, &pEnd, 10);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < STM32_FLASH_DTE_FLOOR
                        || ulTemp > STM32_FLASH_DTE_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,SSD");
            			return (FALSE);
                    }
                }
                    
    			/*lq д��EEPROM */
    			//Para_Save(PARA_DTE_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_DTE_TYPE, (U8 *)chTemp,ucLen);
    
    			/*lq ���±��� */
    			AisStaticPara.DTE = ulTemp;
			
                AisStaticPara.Refresh = TRUE;
			}

			UartResponseMsg("$DUAIR,1,SSD");
		}
	}
    //==========================================================================================
    /*lq AIVSD��ͷ������������*/
	//  Process  type  4     :  note by Nathan
	else if (strncmp((char *)pDat, "AIVSD", 5) == FALSE)
	{
		if (DevDynamicPara.ucWorkMode == EQ_WORKMOD_NORMAL)
		{           
            /*lq ********Type of ship and cargo category, 0 to 255********** */
			ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);
            
            /*lq �ֶηǿ�*/			
            if (ucCheck != FALSE)
			{
        		/*lq �ַ���Ч���ж�*/
                ulTemp = (U32)strtod(chTemp, &pEnd);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < STM32_FLASH_SHIP_TYPE_FLOOR
                        || ulTemp > STM32_FLASH_SHIP_TYPE_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,VSD");
            			return (FALSE);
                    }
                }
    
    			/*lq д��EEPROM */
    			sprintf(ucTempBuf, "%d", ulTemp);
                //Para_Save(PARA_SHIP_TYPE_TYPE, ucTempBuf, PARA_SHIP_TYPE_MAXLEN);
				FlashWrite(STM32_FLASH_SHIP_TYPE_TYPE, ucTempBuf,STM32_FLASH_SHIP_TYPE_MAXLEN);
    
    			/*lq ���±��� */
    			AisStaticPara.TypeOfShip = ulTemp;

                AisStaticPara.Refresh = TRUE;
            }

            /*lq ********Maximum present static draught, 0 to 25,5 m******** */
			ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
            
            /*lq �ֶηǿ�*/			
            if (ucCheck != FALSE)
			{
        		/*lq �ַ���Ч���ж�*/
                ulTemp = (U32)(strtod(chTemp, &pEnd) * 10);   //lq ������ʹ��ֵ�ĵ�λΪ0.1m
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < STM32_FLASH_MAX_DRAUGHT_FLOOR
                        || ulTemp > STM32_FLASH_MAX_DRAUGHT_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,VSD");
            			return (FALSE);
                    }
                }
    
    			/*lq д��EEPROM */
                sprintf(ucTempBuf, "%d", ulTemp);
    			//Para_Save(PARA_MAX_DRAUGHT_TYPE, ucTempBuf, PARA_MAX_DRAUGHT_MAXLEN);
				FlashWrite(STM32_FLASH_MAX_DRAUGHT_TYPE, ucTempBuf,STM32_FLASH_MAX_DRAUGHT_MAXLEN);
    
    			/*lq ���±��� */
    			AisStaticPara.MaxDraught = ulTemp;

                AisStaticPara.Refresh = TRUE;
            }

            /*lq *****************Persons on-board, 0 to 8191*************** */
			ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
            
            /*lq �ֶηǿ�*/			
            if (ucCheck != FALSE)
			{
        		/*lq �ַ���Ч���ж�*/
                ulTemp = (U32)strtod(chTemp, &pEnd);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < PARA_PERSONS_FLOOR
                        || ulTemp > PARA_PERSONS_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,VSD");
            			return (FALSE);
                    }
                }
    
    			/*lq д��EEPROM */
                sprintf(ucTempBuf, "%d", ulTemp);
    			Para_Save(PARA_PERSONS_TYPE, ucTempBuf, PARA_PERSONS_MAXLEN);
    
    			/*lq ���±��� */
    			AisStaticPara.Persons = ulTemp;

                AisStaticPara.Refresh = TRUE;
            }

			/*lq **************Destination, 1-20 characters***************** */
			ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
			
            /*lq �ֶηǿ�*/			
            if (ucCheck != FALSE)
			{
    			ucLen = strlen(chTemp);
    			if (ucLen > AIS_INFO_DESTINATION_LEN)
    			{
    				UartResponseMsg("$DUAIR,0,VSD");
    				return (FALSE);
    			}
    
    			/*lq д��EEPROM */
    			//Para_Save(STM32_FLASH_DESTINATION_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_DESTINATION_TYPE, (U8 *)chTemp,ucLen);
    
    			/*lq ���±��� */
    			strcpy((char *)&AisStaticPara.Destination[0], AIS_INFO_DESTINATION_DEFAULT);    //lq ʹ��Ĭ��ֵ��@����д�����ַ�
    			strncpy((char *)&AisStaticPara.Destination[0], chTemp, ucLen);                  //lq ʹ��Ŀ��ֵ����ǰ��ġ�@��

                AisStaticPara.Refresh = TRUE;
			}

			/*lq ********Estimated UTC of arrival at destination************ */
			ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);

            /*lq �ֶηǿ�*/			
			if (ucCheck != FALSE)
			{
                /*lq ������Ч���ж�*/
                ucLen = strlen(chTemp);
        		if ((ucLen < 6) || (ucLen > 9))
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
        
        		/*lq �ַ���Ч���ж�*/
                /*lq Hour */
                ulTemp = (chTemp[0] - '0') * 10 + (chTemp[1] - '0');
                if ((ulTemp < PARA_ETA_HOUR_FLOOR)
                    || (ulTemp > PARA_ETA_HOUR_CEIL))
                {
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
                }
                else
                {
                    /*lq д��EEPROM */
        			sprintf(chTemp2, "%d", ulTemp);
        			Para_Save(PARA_ETA_HOUR_TYPE, (U8 *)chTemp2, 2);
        
        			/*lq ���±��� */
        			AisStaticPara.ETAHour = ulTemp;
    			
                    AisStaticPara.Refresh = TRUE;                
                }

                /*lq Minute */
                ulTemp = (chTemp[2] - '0') * 10 + (chTemp[3] - '0');
                if ((ulTemp < PARA_ETA_MINUTE_FLOOR)
                    || (ulTemp > PARA_ETA_MINUTE_CEIL))
                {
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
                }
                else
                {
                    /*lq д��EEPROM */
        			sprintf(chTemp2, "%d", ulTemp);
        			Para_Save(PARA_ETA_MINUTE_TYPE, (U8 *)chTemp2, 2);
        
        			/*lq ���±��� */
        			AisStaticPara.ETAMinute = ulTemp;
    			
                    AisStaticPara.Refresh = TRUE;                
                }   
            }

			/*lq **Estimated day of arrival at destination, 00 to 31 (UTC)** */
			ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 30);

            /*lq �ֶηǿ�*/			
			if (ucCheck != FALSE)
			{
                /*lq ������Ч���ж�*/
                ucLen = strlen(chTemp);
        		if (ucLen != 2)
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
        
        		/*lq �ַ���Ч���ж�*/
                ulTemp = strtoul(chTemp, &pEnd, 10);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < PARA_ETA_DAY_FLOOR
                        || ulTemp > PARA_ETA_DAY_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,VSD");
            			return (FALSE);
                    }
                }
                    
    			/*lq д��EEPROM */
    			Para_Save(PARA_ETA_DAY_TYPE, (U8 *)chTemp, ucLen);
    
    			/*lq ���±��� */
    			AisStaticPara.ETADay = ulTemp;
			
                AisStaticPara.Refresh = TRUE;
			}

			/*lq *Estimated month of arrival at destination, 00 to 12 (UTC)* */
			ucCheck = GetSegment((char *)pDat, chTemp, 7, uiLen, 30);

            /*lq �ֶηǿ�*/			
			if (ucCheck != FALSE)
			{
                /*lq ������Ч���ж�*/
                ucLen = strlen(chTemp);
        		if (ucLen != 2)
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
        
        		/*lq �ַ���Ч���ж�*/
                ulTemp = strtoul(chTemp, &pEnd, 10);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < PARA_ETA_MONTH_FLOOR
                        || ulTemp > PARA_ETA_MONTH_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,VSD");
            			return (FALSE);
                    }
                }
                    
    			/*lq д��EEPROM */
    			Para_Save(PARA_ETA_MONTH_TYPE, (U8 *)chTemp, ucLen);
    
    			/*lq ���±��� */
    			AisStaticPara.ETAMonth = ulTemp;
			
                AisStaticPara.Refresh = TRUE;
			}

			/*lq **********Navigational status, 0 to 15********************* */
			ucCheck = GetSegment((char *)pDat, chTemp, 8, uiLen, 30);

            /*lq �ֶηǿ�*/			
            if (ucCheck != FALSE)
			{
        		/*lq �ַ���Ч���ж�*/
                ulTemp = (U32)strtod(chTemp, &pEnd);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < PARA_NAVIGATION_STATUS_FLOOR
                        || ulTemp > PARA_NAVIGATION_STATUS_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,VSD");
            			return (FALSE);
                    }
                }
    
    			/*lq д��EEPROM */
                sprintf(ucTempBuf, "%d", ulTemp);
    			Para_Save(PARA_NAVIGATION_STATUS_TYPE, ucTempBuf, PARA_NAVIGATION_STATUS_MAXLEN);
    
    			/*lq ���±��� */
    			AisStaticPara.NavigationStatus = ulTemp;

                AisStaticPara.Refresh = TRUE;
            }

			/*lq **********Regional application flags, 0 to 15************** */
			ucCheck = GetSegment((char *)pDat, chTemp, 9, uiLen, 30);

            /*lq �ֶηǿ�*/			
            if (ucCheck != FALSE)
			{
        		/*lq �ַ���Ч���ж�*/
                ulTemp = (U32)strtod(chTemp, &pEnd);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < PARA_REGION_APP_FLAG_FLOOR
                        || ulTemp > PARA_REGION_APP_FLAG_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,VSD");
            			return (FALSE);
                    }
                }
    
    			/*lq д��EEPROM */
                sprintf(ucTempBuf, "%d", ulTemp);
    			Para_Save(PARA_REGION_APP_FLAG_TYPE, ucTempBuf, PARA_REGION_APP_FLAG_MAXLEN);
    
    			/*lq ���±��� */
    			AisStaticPara.RegionAppFlag = ulTemp;

                AisStaticPara.Refresh = TRUE;
            }

			UartResponseMsg("$DUAIR,1,VSD");           		
		}
	}
#endif

    	
    /*lq light beacon protocol message */
    else
    {
#if 0
        /*lq light beacon protocol message*/
        LB1_MsgDealProcess(pDat, uiLen);    
#endif
    }
    return (TRUE);
}
