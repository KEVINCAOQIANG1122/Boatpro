/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: flashctrl.c
  Author: �ż���       Version : V1.0.0        Date: 2015.12.21
  Description:     ��STM32��Flash����ģ��
  Version:         V1.0.0
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "Include.h"
#include "stm32f10x.h"
#include "flashctrl.h"
#include "def.h"
#include "parastorage.h"
#include "usart.h"
#include "24c02.h"
#include "initial.h"
#include "common.h"
#include "gmskmod.h"
#include "string.h"
#include "DAC.h"
#include "sotdma.h"
#include "config.h"
#include "stm32f10x_flash.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
U32 STM32_FLASH_Addr_IndexBuf[STM32_FLASH_INDEXBUF_LEN] = 
{
    /*-----------------REGION_U32--------------------*/
    STM32_FLASH_MMSI_ADDR,
    STM32_FLASH_MMSI_DEST_ADDR,
    STM32_FLASH_MMSI_BACKUP_ADDR,
    STM32_FLASH_MMSI_DEST_BACKUP_ADDR,
    STM32_FLASH_SERIAL_ADDR,
    /*-----------------REGION_U16--------------------*/
    STM32_FLASH_SHIPA_ADDR,
    STM32_FLASH_SHIPB_ADDR,
    /*-----------------REGION_U8---------------------*/
    STM32_FLASH_MAX_DRAUGHT_ADDR,
    STM32_FLASH_SHIPC_ADDR,
    STM32_FLASH_SHIPD_ADDR,
    STM32_FLASH_EPFS_TYPE_ADDR,
    STM32_FLASH_SHIP_TYPE_ADDR,
    STM32_FLASH_MODAMP_ADDR,
    STM32_FLASH_FREQADJ_ADDR,
    STM32_FLASH_RF_VOLT_ADJUST_ADDR,
    STM32_FLASH_DTE_ADDR,
    STM32_FLASH_AIS_CLASS_ADDR,
    /*-----------------REGION_CHAR-------------------*/
    STM32_FLASH_DESTINATION_ADDR,
    STM32_FLASH_VENDID_ADDR,
    STM32_FLASH_SHIP_NAME_ADDR,
    STM32_FLASH_HWVERSION_ADDR,
};

U8 STM32_FLASH_MaxLen_IndexBuf[STM32_FLASH_INDEXBUF_LEN] = 
{
    /*-----------------REGION_U32--------------------*/
    STM32_FLASH_MMSI_MAXLEN,
    STM32_FLASH_MMSI_DEST_MAXLEN,
    STM32_FLASH_MMSI_BACKUP_MAXLEN,
    STM32_FLASH_MMSI_DEST_BACKUP_MAXLEN,
    STM32_FLASH_SERIAL_MAXLEN,
    /*-----------------REGION_U16--------------------*/
    STM32_FLASH_SHIPA_MAXLEN,
    STM32_FLASH_SHIPB_MAXLEN,
    /*-----------------REGION_U8---------------------*/
    STM32_FLASH_MAX_DRAUGHT_MAXLEN,
    STM32_FLASH_SHIPC_MAXLEN,
    STM32_FLASH_SHIPD_MAXLEN,
    STM32_FLASH_EPFS_TYPE_MAXLEN,
    STM32_FLASH_SHIP_TYPE_MAXLEN,
    STM32_FLASH_MODAMP_MAXLEN,
    STM32_FLASH_FREQADJ_MAXLEN,
    STM32_FLASH_RF_VOLT_ADJUST_MAXLEN,
    STM32_FLASH_DTE_MAXLEN,
    STM32_FLASH_AIS_CLASS_MAXLEN,
    /*-----------------REGION_CHAR-------------------*/    
    STM32_FLASH_DESTINATION_MAXLEN,
    STM32_FLASH_VENDID_MAXLEN,
    STM32_FLASH_SHIP_NAME_MAXLEN,
    STM32_FLASH_HWVERSION_MAXLEN,
};

/*
FLASH����Ĭ��ֵ��Ϊ��Լ�ռ䣬���Խ�REGION_U32��REGION_U16��
REGION_U8��Ĭ��ֵ�ֱ���������ͬ���͵�����洢
*/
U32 STM32_FLASH_Default_IndexBuf[STM32_FLASH_INDEXBUF_LEN] = 
{
    /*-----------------REGION_U32--------------------*/    
    STM32_FLASH_MMSI_DEFAULT,
    STM32_FLASH_MMSI_DEST_DEFAULT,
    STM32_FLASH_MMSI_BACKUP_DEFAULT,
    STM32_FLASH_MMSI_DEST_BACKUP_DEFAULT,
    STM32_FLASH_SERIAL_DEFAULT,
    /*-----------------REGION_U16--------------------*/    
    STM32_FLASH_SHIPA_DEFAULT,
    STM32_FLASH_SHIPB_DEFAULT,
    /*-----------------REGION_U8---------------------*/
    STM32_FLASH_MAX_DRAUGHT_DEFAULT,
    STM32_FLASH_SHIPC_DEFAULT,
    STM32_FLASH_SHIPD_DEFAULT,
    STM32_FLASH_EPFS_TYPE_DEFAULT,
    STM32_FLASH_SHIP_TYPE_DEFAULT,
    STM32_FLASH_MODAMP_DEFAULT,
    STM32_FLASH_FREQADJ_DEFAULT,
    STM32_FLASH_RF_VOLT_ADJUST_DEFAULT,
    STM32_FLASH_DTE_DEFAULT,
    STM32_FLASH_AIS_CLASS_DEFAULT,
    /*-----------------REGION_CHAR-------------------*/
    0,// ֻռλ����ʹ��
    0,
    0,
    0,    
};

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* ����  : FlashWrite
* ����  : д��STM32Flash����
* ����  : ParaType	��������
		  pPara		����
		  ucLen     ��������
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 FlashWrite(U8 ucPara_Type, U8 *pPara, U8 ucLen)
{
	U8 i;
	U32 ulAddr;
	U8 ucMaxLen;
	U8 ucCheck;
	U8 ucIndex;
	U8 ucTempBuf[40];
	U16 usTempBuf[20];
    char *pEnd;
    U32 ulTemp;
	U8 usLen;
	U8 ucDebug;
	U16 usFlashData;
	U16 usFlashDataBuf[STM32_FLASH_PAGE_DATA_MAXLEN];
	U32 ulFlashPageAddr;
	U32 ulFlashPageDataAddr;
	U8  ucStar;
	U8  ucFlashDataLen;

	ulAddr = STM32_FLASH_Addr_IndexBuf[ucPara_Type];
	ucMaxLen = STM32_FLASH_MaxLen_IndexBuf[ucPara_Type];                              // ��󳤶ȵĺ����Ϊ�����в���������ַ��� 
	if (ucLen > ucMaxLen || ucLen < 1)
	{
		return (FALSE);
	}

	if(ucPara_Type >= 17 && ucPara_Type < 21)									  	  // CHAR����
	{
		ucIndex = 0;
    
    	//���泤��
    	ucTempBuf[ucIndex++] = ucLen; 
    
    	//��������
    	for (i = 0; i < ucLen; i++)
    	{
    		ucTempBuf[ucIndex++] = pPara[i];
		}
    	
		//����У�鲢����
    	ucCheck = EEpromChkSum(&ucTempBuf[0], ucLen + 1);
    	ucTempBuf[ucIndex++] = ucCheck; 
	}
	else																			  // ��CHAR����
	{
		ulTemp = strtoul(pPara, &pEnd, 10);
        if ((*pEnd) != '\0')
        {
		    return (FALSE);        
        }

		if(ucPara_Type >= 0 && ucPara_Type < 5)											  // U32���� 
		{
			if (ucLen > 10 || ulTemp > 0xFFFFFFFF)                              		  // 0-4294967295
            {
                return (FALSE);                             
            }
			                		
            ucIndex = 4;
            ucLen = 4;	
		}
		else if(ucPara_Type >= 5 && ucPara_Type < 7)									  // U16����
		{
			if (ucLen > 5 || ulTemp > 0xFFFF)                                    		  // 0-65535
            {
                return (FALSE);                             
            }
			                		
            ucIndex = 2;
            ucLen = 2;
		}
		else if(ucPara_Type >= 7 && ucPara_Type < 17)									  // U8����
		{
			if (ucLen > 3 || ulTemp > 0xFF)                                      		  // 0-255
            {
                return (FALSE);                             
            }

            ucIndex = 1;
            ucLen = 1;
		} 
		/* �������ݣ�С��ģʽ*/                                
	    for (i = 0; i < ucIndex; i++)
	    {
			ucTempBuf[i] = (U8)(ulTemp & 0xFF);	            
			ulTemp >>= 8;
	    }

		//����У�鲢����
    	ucCheck = EEpromChkSum(&ucTempBuf[0], ucLen);
    	ucTempBuf[ucIndex++] = ucCheck;
	}
	usLen = U8ToU16(ucTempBuf,usTempBuf,ucIndex);
	//RCC_HSICmd(ENABLE);
	/*************************����FLASH��д����������***********************************/
	FLASH_Unlock();
	/*************************���FLASH�������־λ*************************************/
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	/*************************����FLASHҳ��*********************************************/
	ulFlashPageAddr = ((ulAddr-STM32_FLASH_BASE)/STM32_FLASH_EVERY_PAGE_SIZE)*STM32_FLASH_EVERY_PAGE_SIZE;
	ulFlashPageDataAddr = ulFlashPageAddr;
	i=0;
	do
	{
		usFlashDataBuf[i] = FLASH_ReadHalfWord(ulFlashPageDataAddr);
		i++;
		ulFlashPageDataAddr +=2;
	}while(i < STM32_FLASH_PAGE_DATA_MAXLEN+1);
	ucFlashDataLen = STM32_FLASH_PAGE_DATA_MAXLEN;
	FLASH_ErasePage(ulFlashPageAddr);
	/*************************��Flash��д������*********************************************/
	ucStar = (ulAddr-ulFlashPageAddr)/2;
	for(i=0;i<usLen;i++)						 //����Ҫд������ݲ��뵽��д��������
	{
		usFlashDataBuf[i+ucStar] = usTempBuf[i]; 		
	}
	if((i+ucStar)>ucFlashDataLen)
	{
		ucFlashDataLen = (i+ucStar); 
	}
	for(i=0;i<ucFlashDataLen;i++)
	{
		ucDebug = FLASH_ProgramHalfWord(ulFlashPageAddr,usFlashDataBuf[i]);
		usFlashData = FLASH_ReadHalfWord(ulFlashPageAddr);
		if(usFlashData != usFlashDataBuf[i] || ucDebug != FLASH_COMPLETE)
		{
			break;
		}
		ulFlashPageAddr += 2;
		Feed_Dog();
	}
	FLASH_Lock();
	//RCC_HSICmd(DISABLE);
	return (TRUE);
}

/*******************************************************************************
* ����  : FlashRead
* ����  : ��ȡSTM32Flash����
* ����  : ParaType	��������
		  pPara		����
		  ��ʽ:��������Ϊ��ֵ���ͣ��򷵻���ֵ��ʮ������ʽ���ַ�����
               ��������Ϊ�ַ����ͣ��򷵻��ַ���
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 FlashRead(U8 ucPara_Type, U8 *pPara)
{
	U8 ucInteger;									  // ��������
	U8 ucRemainder;									  // 0.5��������
	U8 i;
	U32 ulAddr;
	U8 ucMaxLen;
	U8 ucCheck;
	U8 ucTempBuf[40];
	U16 usTempBuf[20];
    char *pEnd;
    U32 ulTemp;
	U8 ucLen;
	U8 usLen;

	ulAddr = STM32_FLASH_Addr_IndexBuf[ucPara_Type];
	ucMaxLen = STM32_FLASH_MaxLen_IndexBuf[ucPara_Type];                              // ��󳤶ȵĺ����Ϊ�����в���������ַ��� 

	if(ucPara_Type >= 17 && ucPara_Type < 21)									  	  // CHAR����
	{
		ucLen = (U8)(FLASH_ReadHalfWord(ulAddr)& 0xFF);
		//���ݳ��ȶ�ȡ����
    	if (ucLen > ucMaxLen || ucLen < 1)
    	{
    		//���ظ����͵�Ĭ������
    		ucLen = STM32_FLASH_DefaultRead(ucPara_Type, pPara);	
    	}
		if((ucLen+2)%2==1)
		{
			usLen = ((ucLen+2)/2+1);
		}
		else
		{
			usLen = (ucLen+2)/2;
		}
		//FLASH_ReadOutProtection(DISABLE);
		for(i=0;i<usLen;i++)
		{
			usTempBuf[i] = FLASH_ReadHalfWord(ulAddr);
			ulAddr += 2;
			Feed_Dog();
		}
		//FLASH_ReadOutProtection(ENABLE);
		U16ToU8(usTempBuf,ucTempBuf,usLen);

		//��Flash��ȡ�����ݽ���У��
    	ucCheck = EEpromChkSum(&ucTempBuf[0], ucLen + 1);
		//У����ȷ
		if(ucCheck == ucTempBuf[ucLen+1])
		{
			//��������
    		for (i = 0; i < ucLen; i++)
    		{
    			pPara[i] = ucTempBuf[i + 1];
    		}
    
    		pPara[ucLen] = '\0';
		}
		//У�����
    	else
    	{
    		//���ظ����͵�Ĭ������
    		ucLen = STM32_FLASH_DefaultRead(ucPara_Type, pPara);
    	}
	}
	else																			  // ��CHAR����
	{
		if(ucPara_Type >= 0 && ucPara_Type < 5)											  // U32���� 
		{
			usLen = 3;
			ucLen = 4;	
		}
		else if(ucPara_Type >= 5 && ucPara_Type < 7)									  // U16����
		{
			usLen = 2;
			ucLen = 2;
		}
		else if(ucPara_Type >= 7 && ucPara_Type < 17)									  // U8����
		{
			usLen = 1;
			ucLen = 1;
		}
		//FLASH_ReadOutProtection(DISABLE);
		for(i=0;i<usLen;i++)
		{
			usTempBuf[i] = FLASH_ReadHalfWord(ulAddr);
 			ulAddr += 2;
			Feed_Dog();
		}
		//FLASH_ReadOutProtection(ENABLE);
		U16ToU8(usTempBuf,ucTempBuf,usLen);
		//��Flash��ȡ�����ݽ���У��
    	ucCheck = EEpromChkSum(&ucTempBuf[0], ucLen);

		/* У����ȷ*/
		if (ucCheck == ucTempBuf[ucLen])
		{
            ulTemp = 0;
            for (i = 0; i < ucLen; i++)
            {
                ulTemp <<= 8;                                          // ������λ��Ӱ��
                ulTemp |= (ucTempBuf[ucLen - 1 - i] & 0xFF);           // С��ģʽ
            }
             
            sprintf(pPara, "%0*lu", ucMaxLen, ulTemp);                 // ���ַ��Ͳ���ͳһ��ʽ������ʮ������ֵ���ַ���
                                                                       // ʹ��ucMaxLen��Ϊ����֮ǰ��д��ʽ���ݣ����ϲ����͸��
		}
		/*lq У�����*/
		else
		{
			/* ���ظ����͵�Ĭ������*/
			ucLen = STM32_FLASH_DefaultRead(ucPara_Type, pPara);
		}
	}	
}

/*******************************************************************************
* ����  : U8ToU16 
* ����  : �洢��������
* ����  : ucTemp U8����
		  usTemp U16����
		  ucLen U8�ĳ���
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 U8ToU16(U8 *ucTemp,U16 *usTemp,U8 ucLen)
{
	U8 ucInteger;									  // ��������
	U8 ucRemainder;									  // 0.5��������
	U8 i;

	for(i=0;i<ucLen;i++)
	{
		ucInteger = i/2;
		ucRemainder = i%2;
	  	if(ucRemainder == 0)
	  	{
			usTemp[ucInteger] = ucTemp[i];	
	  	}
	  	else
	  	{
			usTemp[ucInteger] += (ucTemp[i]<<8);
	  	}
	}
	return (ucInteger+1);	
}

/*******************************************************************************
* ����  : U16ToU8 
* ����  : �洢��������
* ����  : usTemp U16����
		  ucTemp U8����
		  usLen U8�ĳ���
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 U16ToU8(U16 *usTemp,U8 *ucTemp,U8 usLen)
{
	U8 ucInteger;									  // ��������
	U8 ucRemainder;									  // 0.5��������
	U8 i;

	for(i=0;i<usLen;i++)
	{
		ucTemp[(2*i)] = (U8)(usTemp[i]& 0xFF);
		ucTemp[(2*i+1)] = (U8)((usTemp[i]>>8)& 0xFF); 	
	}
	return (2*usLen);	
}

/*******************************************************************************
* ����  : FLASH_ReadHalfWord
* ����  : �洢CHAR��������
* ����  : address ������ַ
* ���  : ����ֵ
* ����  : ��
*******************************************************************************/
u16 FLASH_ReadHalfWord(u32 address)
{
	return *(u16*)address;
}

/*******************************************************************************
* ����  : STM32_FLASH_DefaultRead
* ����  : Ĭ�ϲ�����ȡ
* ����  : Para_Type����������
*		  pPara����Ҫ��ȡ�Ĳ���
* ���  : ��
* ����  : ��������
*******************************************************************************/
U8 STM32_FLASH_DefaultRead(U8 ucPara_Type, U8 *pPara)
{
	U8 ucMaxLen;
	U32 ulAddr;
    U32 ulDefVal;

	ucMaxLen = STM32_FLASH_MaxLen_IndexBuf[ucPara_Type];
    ulAddr = STM32_FLASH_Addr_IndexBuf[ucPara_Type];
    ulDefVal = STM32_FLASH_Default_IndexBuf[ucPara_Type];

    /* char�����ݴ洢�� */
    if (ulAddr >= STM32_FLASH_CHAR_BASE)
    {
	    /*lq Destination */
        if (ucPara_Type == STM32_FLASH_DESTINATION_TYPE)
        {
            sprintf((char *)pPara, "%*s", ucMaxLen, STM32_FLASH_DESTINATION_DEFAULT);
        }

	    /*lq Vend ID*/
        else if (ucPara_Type == STM32_FLASH_VENDID_TYPE)
        {
            sprintf((char *)pPara, "%*s", ucMaxLen, STM32_FLASH_VENDID_DEFAULT);                    
        }                                    

	    /*lq Ship name */
        if (ucPara_Type == STM32_FLASH_SHIP_NAME_TYPE)
        {
            sprintf((char *)pPara, "%*s", ucMaxLen, STM32_FLASH_SHIP_NAME_DEFAULT);
        }

	    /*lq Ӳ���汾�� */
        if (ucPara_Type == STM32_FLASH_HWVERSION_TYPE)
        {
            sprintf((char *)pPara, "%*s", ucMaxLen, STM32_FLASH_HWVERSION_DEFAULT);
        }

    }
    else
    {
        sprintf((char *)pPara, "%0*ld", ucMaxLen, ulDefVal);            
    }

	
	return (ucMaxLen);	
}
