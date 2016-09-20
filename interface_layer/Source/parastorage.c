/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: main.c
  Author: ��ΰ       Version : V1.00        Date: 2011.09.23
  Description:    ��Ŀ�й���UCOS����ϵͳ
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			1����������Main
    	   �������ܣ�����ARM��������ļܹ�
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
												   	
#include "def.h"
#include "24c02.h"
#include "initial.h"
#include "common.h"
#include "parastorage.h"
#include "gmskmod.h"
#include "string.h"
#include "DAC.h"
#include "sotdma.h"
#include "config.h"
#include "Include.h"
#include "flashctrl.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
U8 Para_Addr_IndexBuf[PARA_INDEXBUF_LEN] = 
{
    /*-----------------REGION_U32--------------------*/
    PARA_WORKTIME_ADDR,
    PARA_WORKTIME_LAST_ADDR,
	PARA_SAILING_DISTINCE_ADDR,
    /*-----------------REGION_U16--------------------*/
    PARA_PERSONS_ADDR,
	PARA_WDG_RESET_CNT_ADDR,
    PARA_LAST_WDG_RESET_CNT_ADDR,
	PARA_NORMAL_POWER_ON_CNT_ADDR,
    /*-----------------REGION_U8---------------------*/
    PARA_ETA_MONTH_ADDR,
    PARA_NAVIGATION_STATUS_ADDR,
    PARA_REGION_APP_FLAG_ADDR,
    PARA_POWER_LEVEL_ADDR,
    PARA_ETA_DAY_ADDR,
    PARA_ETA_HOUR_ADDR,
    PARA_ETA_MINUTE_ADDR,
    PARA_OPEN_CLOSE_TIME_INDEX_ADDR,
    /*-----------------REGION_CHAR-------------------*/
    PARA_TIME_OPEN_0_ADDR,
    PARA_TIME_OPEN_1_ADDR,
    PARA_TIME_OPEN_2_ADDR,
    PARA_TIME_CLOSE_0_ADDR,
    PARA_TIME_CLOSE_1_ADDR,
    PARA_TIME_CLOSE_2_ADDR,
	PARA_GPS_FIRST_TIME_ADDR,
};

U8 Para_MaxLen_IndexBuf[PARA_INDEXBUF_LEN] = 
{
    /*-----------------REGION_U32--------------------*/
    PARA_WORKTIME_MAXLEN,
    PARA_WORKTIME_LAST_MAXLEN,
	PARA_SAILING_DISTINCE_MAXLEN,
    /*-----------------REGION_U16--------------------*/
    PARA_PERSONS_MAXLEN,
    PARA_WDG_RESET_CNT_MAXLEN,
	PARA_LAST_WDG_RESET_CNT_MAXLEN,
	PARA_NORMAL_POWER_ON_CNT_MAXLEN,
    /*-----------------REGION_U8---------------------*/
    PARA_ETA_MONTH_MAXLEN,
    PARA_NAVIGATION_STATUS_MAXLEN,
    PARA_REGION_APP_FLAG_MAXLEN,
    PARA_POWER_LEVEL_MAXLEN,
    PARA_ETA_DAY_MAXLEN,
    PARA_ETA_HOUR_MAXLEN,
    PARA_ETA_MINUTE_MAXLEN,
	PARA_OPEN_CLOSE_TIME_INDEX_MAXLEN,
    /*-----------------REGION_CHAR-------------------*/
    PARA_TIME_OPEN_MAXLEN,
    PARA_TIME_OPEN_MAXLEN,
    PARA_TIME_OPEN_MAXLEN,
    PARA_TIME_CLOSE_MAXLEN,
    PARA_TIME_CLOSE_MAXLEN,
    PARA_TIME_CLOSE_MAXLEN,
	PARA_GPS_FIRST_TIME_MAXLEN,
};

/*lq
* EEPROM����Ĭ��ֵ��Ϊ��Լ�ռ䣬���Խ�REGION_U32��REGION_U16��
REGION_U8��Ĭ��ֵ�ֱ���������ͬ���͵�����洢
*/
U32 Para_Default_IndexBuf[PARA_INDEXBUF_LEN] = 
{
    /*-----------------REGION_U32--------------------*/  
    PARA_WORKTIME_DEFAULT,
    PARA_WORKTIME_LAST_DEFAULT,
	PARA_SAILING_DISTINCE_DEFAULT,
    /*-----------------REGION_U16--------------------*/
    PARA_PERSONS_DEFAULT,
	PARA_WDG_RESET_CNT_DEFAULT,
	PARA_LAST_WDG_RESET_CNT_DEFAULT,
	PARA_NORMAL_POWER_ON_CNT_DEFAULT,
    /*-----------------REGION_U8---------------------*/
    PARA_ETA_MONTH_DEFAULT,
    PARA_NAVIGATION_STATUS_DEFAULT,
    PARA_REGION_APP_FLAG_DEFAULT,
    PARA_POWER_LEVEL_DEFAULT,
    PARA_ETA_DAY_DEFAULT,
    PARA_ETA_HOUR_DEFAULT,
    PARA_ETA_MINUTE_DEFAULT,
    PARA_OPEN_CLOSE_TIME_INDEX_DEFAULT,
    /*-----------------REGION_CHAR-------------------*/
    0,//lq ֻռλ����ʹ��
    0,
    0,
    0,
    0,
    0,
	0,				 
};

/* Private  function  --------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/

/*******************************************************************************
* ����  : Para_Save
* ����  : �����洢��eeprom
* ����  : ucPara_Type����������
*		  pPara����Ҫ�洢�Ĳ���
          ��ʽ����������Ϊ��ֵ���ͣ���Ϊ��ֵ��ʮ������ʽ���ַ���
                ��������Ϊ�ַ����ͣ���Ϊ�ַ���
*		  ucLen�������洢���ȣ��ò��������ַ��Ͳ�����Ч��������ֵ�Ͳ�����Ĭ��ֵ
* ���  : ��
* ����  : �ɹ�/ʧ��
*******************************************************************************/
U8 Para_Save(U8 ucPara_Type, U8 *pPara, U8 ucLen)
{
	U8 i;
	U8 ucAddr;
	U8 ucMaxLen;
	U8 ucCheck;
	U8 ucIndex;
	U8 ucTempBuf[40];
    char *pEnd;
    U32 ulTemp;

	ucAddr = Para_Addr_IndexBuf[ucPara_Type];
	ucMaxLen = Para_MaxLen_IndexBuf[ucPara_Type];                               //lq ��󳤶ȵĺ����Ϊ�����в���������ַ���    

	if (ucLen > ucMaxLen || ucLen < 1)
	{
		return (FALSE);
	}

    /*lq char�����ݴ洢��, EEPROM�ĸߵ�ַ��*/
    if (ucAddr >= PARA_REGION_CHAR_BASE_ADDR)
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
    else
    {
        ulTemp = strtoul((const char*)pPara, &pEnd, 10);
        if ((*pEnd) != '\0')
        {
		    return (FALSE);        
        }

        /*lq U8�����ݴ洢��*/
        else if (ucAddr >= PARA_REGION_U8_BASE_ADDR)
        {
    		if (ucLen > 3 || ulTemp > 0xFF)                                      //lq 0-255
            {
                return (FALSE);                             
            }

            ucIndex = 1;
            ucLen = 1;
        }
        
        /*lq U16�����ݴ洢��*/
        else if (ucAddr >= PARA_REGION_U16_BASE_ADDR)
        {
    		if (ucLen > 5 || ulTemp > 0xFFFF)                                    //lq 0-65535
            {
                return (FALSE);                             
            }
                		
            ucIndex = 2;
            ucLen = 2;
        }
        
        /*lq U32�����ݴ洢����EEPROM�ĵ͵�ַ��*/
        else if (ucAddr >= PARA_REGION_U32_BASE_ADDR)
        {
    		if (ucLen > 10 || ulTemp > 0xFFFFFFFF)                              //lq 0-4294967295
            {
                return (FALSE);                             
            }
                		
            ucIndex = 4;
            ucLen = 4;
        } 

        /*lq �������ݣ�С��ģʽ*/                                
        for (i = 0; i < ucIndex; i++)
        {
            ucTempBuf[i] = (U8)(ulTemp & 0xFF);
            ulTemp >>= 8;
        } 

    	//����У�鲢����
    	ucCheck = EEpromChkSum(&ucTempBuf[0], ucLen);
    	ucTempBuf[ucIndex++] = ucCheck;                
    }

	//д�뵽EEPROM��
	for (i = 0; i < ucIndex; i++)
	{
		I2C_EE_BufferWrite((char *)&ucTempBuf[i], (ucAddr + i), 1);
	}

	return (TRUE);	
}

/*******************************************************************************
* ����  : Para_DefaultRead
* ����  : Ĭ�ϲ�����ȡ
* ����  : Para_Type����������
*		  pPara����Ҫ��ȡ�Ĳ���
* ���  : ��
* ����  : ��������
*******************************************************************************/
U8 Para_DefaultRead(U8 ucPara_Type, U8 *pPara)
{
	U8 ucMaxLen;
	U8 ucAddr;
    U32 ulDefVal;

	ucMaxLen = Para_MaxLen_IndexBuf[ucPara_Type];
    ucAddr = Para_Addr_IndexBuf[ucPara_Type];
    ulDefVal = Para_Default_IndexBuf[ucPara_Type];

    /*lq char�����ݴ洢��, EEPROM�ĸߵ�ַ��*/
    if (ucAddr >= PARA_REGION_CHAR_BASE_ADDR)
    {
//	    /*lq Destination */
//        if (ucPara_Type == PARA_DESTINATION_TYPE)
//        {
//            sprintf((char *)pPara, "%*s", ucMaxLen, PARA_DESTINATION_DEFAULT);
//        }

//	    /*lq Vend ID*/
//        else if (ucPara_Type == PARA_VENDID_TYPE)
//        {
//            sprintf((char *)pPara, "%*s", ucMaxLen, PARA_VENDID_DEFAULT);                    
//        }

	    /*lq ����ʱ��*/
        if ((ucPara_Type >= PARA_TIME_OPEN_0_TYPE)
                && (ucPara_Type < PARA_TIME_OPEN_0_TYPE + PARA_TIME_OPEN_CLOSE_TOTAL))
        {
            sprintf((char *)pPara, "%*s", ucMaxLen, PARA_TIME_OPEN_DEFAULT);                    
        }
	    
        /*lq �ػ�ʱ��*/
        else if ((ucPara_Type >= PARA_TIME_CLOSE_0_TYPE)
                && (ucPara_Type < PARA_TIME_CLOSE_0_TYPE + PARA_TIME_OPEN_CLOSE_TOTAL))
        {
            sprintf((char *)pPara, "%*s", ucMaxLen, PARA_TIME_CLOSE_DEFAULT);                    
        }                                    

	    /* GPS��һ�ζ�λʱ�� */
        if (ucPara_Type == PARA_GPS_FIRST_TIME_TYPE)
        {
            sprintf((char *)pPara, "%*s", ucMaxLen, PARA_GPS_FIRST_TIME_DEFAULT);
        }

//	    /*lq Ӳ���汾�� */
//        if (ucPara_Type == PARA_HWVERSION_TYPE)
//        {
//            sprintf((char *)pPara, "%*s", ucMaxLen, PARA_HWVERSION_DEFAULT);
//        }

    }
    else
    {
        sprintf((char *)pPara, "%0*ld", ucMaxLen, ulDefVal);            
    }

	
	return (ucMaxLen);	
}
/*******************************************************************************
* ����  : Para_Read
* ����  : ������eeprom��ȡ���ڴ���
* ����  : ucPara_Type����������
*		  pPara����Ҫ��ȡ�Ĳ�����
          ��ʽ:��������Ϊ��ֵ���ͣ��򷵻���ֵ��ʮ������ʽ���ַ�����
               ��������Ϊ�ַ����ͣ��򷵻��ַ���
* ���  : ��
* ����  : ��������
*******************************************************************************/
U8 Para_Read(U8 ucPara_Type, U8 *pPara)
{
	U8 i;
	U8 ucAddr;
	U8 ucLen;
	U8 ucMaxLen;
	U8 ucCheck;
	U8 ucIndex;
	U8 ucTempBuf[40];
    U32 ulTemp;
    //U8 ucFormLen;
	 
	ucAddr = Para_Addr_IndexBuf[ucPara_Type];
	ucMaxLen = Para_MaxLen_IndexBuf[ucPara_Type];                               //lq ��󳤶ȵĺ����Ϊ�����в���������ַ���

    /*lq char�����ݴ洢��, EEPROM�ĸߵ�ַ��*/
    if (ucAddr >= PARA_REGION_CHAR_BASE_ADDR)
    {
    	I2C_EE_BufferRead((char *)&ucLen, ucAddr++, 1);
    
    	//EEPROM��ȡ���ݳ��ȱ���
    	//���ݳ��ȶ�ȡ����
    	if (ucLen > ucMaxLen || ucLen < 1)
    	{
    		//���ظ����͵�Ĭ������
    		ucLen = Para_DefaultRead(ucPara_Type, pPara);	
    	}
    	//���ݳ��ȶ�ȡ��ȷ
    	else
    	{
    		ucIndex = 0;
    
    		ucTempBuf[ucIndex++] = ucLen;
    		
    		//��EERPOM�ж�ȡ������У������
    		for (i = 0; i < (ucLen + 1); i++)
    		{
    			I2C_EE_BufferRead((char *)&ucTempBuf[ucIndex++], ucAddr++, 1);  //lq ��������ʵ�ʳ���С����󳤶�ʱ����ʵ�ʳ��ȡ��ֶΡ����������ֶμ���У���롱�ֶ���������ŵģ��м��޿����ֽ�
    		}
    
    		//��EEPROM��ȡ�����ݽ���У��
    		ucCheck = EEpromChkSum(&ucTempBuf[0], ucLen + 1);
    
    		//У����ȷ
    		if (ucCheck == ucTempBuf[ucIndex - 1])
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
    			ucLen = Para_DefaultRead(ucPara_Type, pPara);
    		}
    	}    
    }
    else
    {
        /*lq U8�����ݴ洢��*/
        if (ucAddr >= PARA_REGION_U8_BASE_ADDR)
        {
    		ucIndex = 0;
            ucLen = 1;
            //lq ucFormLen = 3;                                                      //lq 0-255    
        }
        
        /*lq U16�����ݴ洢��*/
        else if (ucAddr >= PARA_REGION_U16_BASE_ADDR)
        {
    		ucIndex = 0;
            ucLen = 2;
            //lq ucFormLen = 5;                                                      //lq 0-65535
        }
        
        /*lq U32�����ݴ洢����EEPROM�ĵ͵�ַ��*/
        else if (ucAddr >= PARA_REGION_U32_BASE_ADDR)
        {
    		ucIndex = 0;
            ucLen = 4;
            //lq ucFormLen = 10;                                                     //lq 0-4294967295
        }
        
		/*lq ��EERPOM�ж�ȡ������У������*/
		I2C_EE_BufferRead((char *)&ucTempBuf[ucIndex], ucAddr, ucLen + 1);      //lq ����+У����	

		/*lq ��EEPROM��ȡ�����ݽ���У��*/
		ucCheck = EEpromChkSum(&ucTempBuf[0], ucLen);

		/*lq У����ȷ*/
		if (ucCheck == ucTempBuf[ucLen])
		{
            ulTemp = 0;
            for (i = 0; i < ucLen; i++)
            {
                ulTemp <<= 8;                                          //lq ������λ��Ӱ��
                ulTemp |= (ucTempBuf[ucLen - 1 - i] & 0xFF);           //lq С��ģʽ
            }
            
            //lq sprintf(pPara, "%*l", ucFormLen, ulTemp);             //lq ���ַ��Ͳ���ͳһ��ʽ������ʮ������ֵ���ַ��� 
            sprintf((char*)pPara, "%0*lu", ucMaxLen, ulTemp);                 //lq ���ַ��Ͳ���ͳһ��ʽ������ʮ������ֵ���ַ���
                                                                       //lq ʹ��ucMaxLen��Ϊ����֮ǰ��д��ʽ���ݣ����ϲ����͸��
		}
		/*lq У�����*/
		else
		{
			/*lq ���ظ����͵�Ĭ������*/
			ucLen = Para_DefaultRead(ucPara_Type, pPara);
		}                
    }

	return (ucLen);
}

/*******************************************************************************
* ����  : Para_OpenTimeSave
* ����  : ����ʱ��洢��eeprom
* ����  : 
*		  pPara����Ҫ�洢�Ĳ���
          ��ʽ����������Ϊ��ֵ���ͣ���Ϊ��ֵ��ʮ������ʽ���ַ���
                ��������Ϊ�ַ����ͣ���Ϊ�ַ���
*		  index������ʱ��������ֵ
* ���  : ��
* ����  : �ɹ�/ʧ��
*******************************************************************************/
U8 Para_OpenTimeSave(U8 *pPara, U16 index)
{
    return Para_Save(PARA_TIME_OPEN_0_TYPE + index, pPara, PARA_TIME_OPEN_MAXLEN);
}

/*******************************************************************************
* ����  : Para_CloseTimeSave
* ����  : �ػ�ʱ��洢��eeprom
* ����  : 
*		  pPara����Ҫ�洢�Ĳ���
          ��ʽ����������Ϊ��ֵ���ͣ���Ϊ��ֵ��ʮ������ʽ���ַ���
                ��������Ϊ�ַ����ͣ���Ϊ�ַ���
*		  index���ػ�ʱ��������ֵ
* ���  : ��
* ����  : �ɹ�/ʧ��
*******************************************************************************/
U8 Para_CloseTimeSave(U8 *pPara, U16 index)
{
    return Para_Save(PARA_TIME_CLOSE_0_TYPE + index, pPara, PARA_TIME_CLOSE_MAXLEN);
}

/*******************************************************************************
* ����  : IniEEPROM_Para
* ����  : ������eeprom��ȡ���ڴ��У���ȡȫ������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void IniEEPROM_Para(void)
{
	U8 ucTempBuf[40];
	U8 ucLen;
//    U16 i;

    /*lq -----------------AIS��̬��Ϣ---------------------- */	
	/*lq ��ȡMMSI�� */
	//Para_Read(PARA_MMSI_TYPE, ucTempBuf);
	FlashRead(STM32_FLASH_MMSI_TYPE, ucTempBuf);
	AisStaticPara.MMSI = atol((char *)ucTempBuf);                                //�˴�������Э��ջ�ӿ�

	if (AisStaticPara.MMSI == 0)                                                 //���û�ж���
	{
		//Para_Read(PARA_MMSI_BACKUP_TYPE, ucTempBuf);                            //������
		FlashRead(STM32_FLASH_MMSI_BACKUP_TYPE, ucTempBuf);									 //������
		if (AisStaticPara.MMSI == 0)                                             //��δ����
		{
			AisStaticPara.MMSI = STM32_FLASH_MMSI_DEFAULT;                              //Ĭ��ֵΪ���
		}
		else
		{
			ucLen = strlen((char *)ucTempBuf);
			AisStaticPara.MMSI = atol((char *)ucTempBuf); 
			//Para_Save(PARA_MMSI_TYPE, ucTempBuf,ucLen);                         //��д
			FlashWrite(STM32_FLASH_MMSI_TYPE, ucTempBuf,ucLen);
		}	
	}		
#if 0
	/*lq ��ȡĿ��MMSI�� */
	Para_Read(PARA_MMSI_DEST_TYPE, ucTempBuf);

	AisStaticPara.DestMMSI = atol((char *)ucTempBuf);            	            //�˴�������Э��ջ�ӿ�

	if (AisStaticPara.DestMMSI == 0)                                             //���û�ж���
	{
		Para_Read(PARA_MMSI_DEST_BACKUP_TYPE, ucTempBuf);                       //������

		if (AisStaticPara.DestMMSI == 0)                                         //��δ����
		{
			AisStaticPara.DestMMSI = PARA_MMSI_DEST_DEFAULT;                     //Ĭ��ֵΪ���
		}
		else
		{
			ucLen = strlen((char *)ucTempBuf);
			AisStaticPara.DestMMSI = atol((char *)ucTempBuf); 
			//Para_Save(PARA_MMSI_DEST_TYPE, ucTempBuf,ucLen);                    //��д
			FlashWrite(STM32_FLASH_MMSI_DEST_TYPE, ucTempBuf,ucLen);
		}	
	}
#endif
    /*lq �������� */
	//Para_Read(PARA_SHIP_NAME_TYPE, ucTempBuf);
	FlashRead(STM32_FLASH_SHIP_NAME_TYPE, ucTempBuf);	
	strcpy((char *)&AisStaticPara.ShipName[0], AIS_INFO_SHIP_NAME_DEFAULT);     //lq ʹ��Ĭ��ֵ��@����д�����ַ�
	strncpy((char *)&AisStaticPara.ShipName[0], (const char*)ucTempBuf, strlen((const char*)ucTempBuf));  //lq ʹ��Ŀ��ֵ����ǰ��ġ�@��

	/*lq ��ȡ������СA��λ�� */
	//Para_Read(PARA_SHIPA_TYPE, ucTempBuf);
	FlashRead(STM32_FLASH_SHIPA_TYPE, ucTempBuf);
	AisStaticPara.ShipA = atol((char *)ucTempBuf);
	
	/*lq ��ȡ������СB��λ�� */
    //Para_Read(PARA_SHIPB_TYPE, ucTempBuf);
	FlashRead(STM32_FLASH_SHIPB_TYPE, ucTempBuf);
	AisStaticPara.ShipB = atol((char *)ucTempBuf);
	
	/*lq ��ȡ������СC��λ�� */
	//Para_Read(PARA_SHIPC_TYPE, ucTempBuf);
	FlashRead(STM32_FLASH_SHIPC_TYPE, ucTempBuf);
	AisStaticPara.ShipC = atol((char *)ucTempBuf);
	
	/*lq ��ȡ������СD��λ�� */
	//Para_Read(PARA_SHIPD_TYPE, ucTempBuf);
	FlashRead(STM32_FLASH_SHIPD_TYPE, ucTempBuf);
	AisStaticPara.ShipD = atol((char *)ucTempBuf);

	/*lq DTE */
	//Para_Read(PARA_DTE_TYPE, ucTempBuf);
	FlashRead(STM32_FLASH_DTE_TYPE, ucTempBuf);
	AisStaticPara.DTE = atol((char *)ucTempBuf);

 	/*lq �������� */
	//Para_Read(PARA_SHIP_TYPE_TYPE, ucTempBuf);
	FlashRead(STM32_FLASH_SHIP_TYPE_TYPE, ucTempBuf);
	AisStaticPara.TypeOfShip = atol((char *)ucTempBuf);

	/*lq ��ˮ��� */
    //Para_Read(PARA_MAX_DRAUGHT_TYPE, ucTempBuf);
	FlashRead(STM32_FLASH_MAX_DRAUGHT_TYPE, ucTempBuf);
	AisStaticPara.MaxDraught = atol((char *)ucTempBuf); 

	/*lq �������� */
    Para_Read(PARA_PERSONS_TYPE, ucTempBuf);
	AisStaticPara.Persons = atol((char *)ucTempBuf); 

    /*lq Ŀ�ĵ� */
	//Para_Read(PARA_DESTINATION_TYPE, ucTempBuf);
	FlashRead(STM32_FLASH_DESTINATION_TYPE, ucTempBuf);
	strcpy((char *)&AisStaticPara.Destination[0], AIS_INFO_DESTINATION_DEFAULT);    //lq ʹ��Ĭ��ֵ��@����д�����ַ�
	strncpy((char *)&AisStaticPara.Destination[0], (const char*)ucTempBuf, strlen((const char*)ucTempBuf));   //lq ʹ��Ŀ��ֵ����ǰ��ġ�@��

	/*lq ���Ƶ���ʱ�䣬��*/
    Para_Read(PARA_ETA_MONTH_TYPE, ucTempBuf);
	AisStaticPara.ETAMonth = atol((char *)ucTempBuf);                          

	/*lq ���Ƶ���ʱ�䣬�� */
	Para_Read(PARA_ETA_DAY_TYPE, ucTempBuf);
	AisStaticPara.ETADay = atol((char *)ucTempBuf);

	/*lq ���Ƶ���ʱ�䣬ʱ */
	Para_Read(PARA_ETA_HOUR_TYPE, ucTempBuf);
	AisStaticPara.ETAHour = atol((char *)ucTempBuf);

	/*lq ���Ƶ���ʱ�䣬�� */
	Para_Read(PARA_ETA_MINUTE_TYPE, ucTempBuf);
	AisStaticPara.ETAMinute = atol((char *)ucTempBuf);

	/*lq ����״̬ */
    Para_Read(PARA_NAVIGATION_STATUS_TYPE, ucTempBuf);
	AisStaticPara.NavigationStatus = atol((char *)ucTempBuf); 

	/*lq ����Ӧ�ñ�־ */
	Para_Read(PARA_REGION_APP_FLAG_TYPE, ucTempBuf);
	AisStaticPara.RegionAppFlag = atol((char *)ucTempBuf);

	/*lq ���Ӷ�λϵͳ���� */
	//Para_Read(PARA_EPFS_TYPE_TYPE, ucTempBuf);
	FlashRead(STM32_FLASH_EPFS_TYPE_TYPE, ucTempBuf);
	AisStaticPara.TypeOfEPFS = atol((char *)ucTempBuf);

    /*lq ��ȡ������Ϣ */
	//Para_Read(PARA_VENDID_TYPE, ucTempBuf);
	FlashRead(STM32_FLASH_VENDID_TYPE, ucTempBuf);
    sprintf((char*)AisStaticPara.VendID, "%s", ucTempBuf);    

#if 0
	/*lq ƫ��Ԥ��������ֵ*/
    Para_Read(PARA_OFF_POSITION_THRES_TYPE, ucTempBuf);
	AisStaticPara.OffPositionThresh = atol((char *)ucTempBuf);                          

	/*lq Ԥ�辭�� */
	Para_Read(PARA_NOM_LONGITUDE_TYPE, ucTempBuf);
	AisStaticPara.NomLongitude = atol((char *)ucTempBuf) - 108000000;	

	/*lq Ԥ��γ�� */
	Para_Read(PARA_NOM_LATITUDE_TYPE, ucTempBuf);
	AisStaticPara.NomLatitude = atol((char *)ucTempBuf) - 54000000;

	/*lq �ŵ�1��� */
	Para_Read(PARA_CHANNEL_1_TYPE, ucTempBuf);
	AisStaticPara.TxChannel1 = atol((char *)ucTempBuf);

	/*lq �ŵ�2��� */
	Para_Read(PARA_CHANNEL_2_TYPE, ucTempBuf);
	AisStaticPara.TxChannel2 = atol((char *)ucTempBuf);
 
 	/*lq ����ˮƽ */
	Para_Read(PARA_POWER_LEVEL_TYPE, ucTempBuf);
	AisStaticPara.PowerLevel = atol((char *)ucTempBuf);
   
 	/*lq �ŵ�1������ʼʱ϶ */
	Para_Read(PARA_TX_START_SLOT_CHANNEL_1_TYPE, ucTempBuf);
	AisStaticPara.TxStartSlotChannel1 = atol((char *)ucTempBuf);

 	/*lq �ŵ�1������ */
	Para_Read(PARA_TX_INTERVAL_CHANNEL_1_TYPE, ucTempBuf);
	AisStaticPara.TxSlotIntervalChannel1 = atol((char *)ucTempBuf);

 	/*lq �ŵ�1������(λ��ƫ���) */
	Para_Read(PARA_TX_INTERVAL_CHANNEL_1_OFF_TYPE, ucTempBuf);
	AisStaticPara.TxSlotIntervalChannel1Off = atol((char *)ucTempBuf);
#endif
    /*lq -----------------�豸��̬����---------------------- */	
	/*lq ��ȡ���Ʒ���*/
	//Para_Read(PARA_MODAMP_TYPE, ucTempBuf);
	FlashRead(STM32_FLASH_MODAMP_TYPE, ucTempBuf);
	DevStaticPara.uiModAmpAdj = atol((char *)ucTempBuf);
	if ((DevStaticPara.uiModAmpAdj < STM32_FLASH_MODAMP_FLOOR) 
        || (DevStaticPara.uiModAmpAdj > STM32_FLASH_MODAMP_CEIL))
	{
		DevStaticPara.uiModAmpAdj = STM32_FLASH_MODAMP_DEFAULT;
	}
	DevStaticPara.uiModAmp = (U16)(((U32)DevStaticPara.uiModAmpAdj * EQ_MOD_AMP + 16) >> 5);
	
	/*lq ��ȡƵ��׼ȷ�� */
	//Para_Read(PARA_FREQADJ_TYPE, ucTempBuf);
	FlashRead(STM32_FLASH_FREQADJ_TYPE, ucTempBuf);
	DevStaticPara.uiFreqAdj = atol((char *)ucTempBuf);
	if ((DevStaticPara.uiFreqAdj < STM32_FLASH_FREQADJ_FLOOR)
        || (DevStaticPara.uiFreqAdj > STM32_FLASH_FREQADJ_CEIL))
	{
		DevStaticPara.uiFreqAdj = STM32_FLASH_FREQADJ_DEFAULT;
	}
   	DevStaticPara.slXmtFreqComp = (S32)((DevStaticPara.uiFreqAdj - 32) * 125);   //lq 

	/*lq ��ȡդѹ���� */
	//Para_Read(PARA_RF_VOLT_ADJUST_TYPE, ucTempBuf);
	FlashRead(STM32_FLASH_RF_VOLT_ADJUST_TYPE, ucTempBuf);
	DevStaticPara.GridVoltAdj = atol((char *)ucTempBuf);
	if ((DevStaticPara.GridVoltAdj < STM32_FLASH_RF_VOLT_ADJUST_FLOOR) 
        || (DevStaticPara.GridVoltAdj > STM32_FLASH_RF_VOLT_ADJUST_CEIL))
	{
		DevStaticPara.GridVoltAdj = STM32_FLASH_RF_VOLT_ADJUST_DEFAULT;
	}
    DevStaticPara.GridVoltOffset = (DevStaticPara.GridVoltAdj - 10) * DA_VALUE_01V;

	/*lq AIS��� */
    //Para_Read(PARA_AIS_CLASS_TYPE, ucTempBuf);
	FlashRead(STM32_FLASH_AIS_CLASS_TYPE, ucTempBuf);
	DevStaticPara.AisClass = atol((char *)ucTempBuf);
    if ((DevStaticPara.AisClass < STM32_FLASH_AIS_CLASS_FLOOR)
        || (DevStaticPara.AisClass > STM32_FLASH_AIS_CLASS_CEIL))
    {
        DevStaticPara.AisClass = STM32_FLASH_AIS_CLASS_DEFAULT;
    }

	/*lq ���к� */
    //Para_Read(PARA_SERIAL_TYPE, ucTempBuf);
	FlashRead(STM32_FLASH_SERIAL_TYPE, ucTempBuf);
	DevStaticPara.SerialNumber = atol((char *)ucTempBuf);                         //�������кţ�U32����
	srand(DevStaticPara.SerialNumber);                                            //lq ÿ�����кŶ�Ӧһ��ʱ϶�����������

#if 0
	//��ȡpower����ʱ��
	Para_Read(PARA_POWER_OPEN_TYPE, ucTempBuf);
	DevStaticPara.PowerOpenTime = atol((char *)ucTempBuf);
	if (DevStaticPara.PowerOpenTime < PARA_POWER_OPEN_FLOOR || DevStaticPara.PowerOpenTime > PARA_POWER_OPEN_CEIL)
	{
		DevStaticPara.PowerOpenTime = 3;
	}

	//��ȡSOS����ʱ��
	Para_Read(PARA_SOS_OPEN_TYPE, ucTempBuf);
	DevStaticPara.SosOpenTime = atol((char *)ucTempBuf);
	if (DevStaticPara.SosOpenTime < PARA_SOS_OPEN_FLOOR || DevStaticPara.SosOpenTime > PARA_SOS_OPEN_CEIL)
	{
		DevStaticPara.SosOpenTime = 3;
	}

	//��ȡGPS��������ʱʱ��
    Para_Read(PARA_GPS_HOT_TYPE, ucTempBuf);	
    DevStaticPara.GpsHotTime = atol((char *)ucTempBuf);
	if (DevStaticPara.GpsHotTime < PARA_GPS_HOT_FLOOR || DevStaticPara.GpsHotTime > PARA_GPS_HOT_CEIL)
	{
		DevStaticPara.GpsHotTime = 4;
	}

	//��ȡ��ص�ѹ����
	Para_Read(PARA_BATTERY_LOW_TYPE, ucTempBuf);
	DevStaticPara.BatteryLowLevel= atol((char *)ucTempBuf);

	Para_Read(PARA_BATTERY_ULTRALOW_TYPE, ucTempBuf);
	DevStaticPara.BatteryUltraLowLevel= atol((char *)ucTempBuf);
	
	if (DevStaticPara.BatteryLowLevel < PARA_BATTERY_LOW_FLOOR || DevStaticPara.BatteryLowLevel > PARA_BATTERY_LOW_CEIL)
	{
		DevStaticPara.BatteryLowLevel = PARA_BATTERY_LOW_DEFAULT;
	}

	if (DevStaticPara.BatteryUltraLowLevel < PARA_BATTERY_ULTRALOW_FLOOR || DevStaticPara.BatteryUltraLowLevel > PARA_BATTERY_ULTRALOW_CEIL)
	{
		DevStaticPara.BatteryUltraLowLevel = PARA_BATTERY_ULTRALOW_DEFAULT;
	}

	if (DevStaticPara.BatteryLowLevel < DevStaticPara.BatteryUltraLowLevel)
	{
		DevStaticPara.BatteryLowLevel = PARA_BATTERY_LOW_DEFAULT;
		DevStaticPara.BatteryUltraLowLevel = PARA_BATTERY_ULTRALOW_DEFAULT;
	}

	//��ȡ�ػ�����ʱ��
	Para_Read(PARA_CLOSE_TIME_TYPE, ucTempBuf);
	DevStaticPara.CloseTime= atol((char *)ucTempBuf);
	if (DevStaticPara.CloseTime < PARA_CLOSE_TIME_FLOOR || DevStaticPara.CloseTime > PARA_CLOSE_TIME_CEIL)
	{
		DevStaticPara.CloseTime = 3;
	}

	//��ȡGPS������λ����ʱ��
	Para_Read(PARA_GPS_POWERON_PROTECT_TYPE, ucTempBuf);
	DevStaticPara.GpsPoweronProtectTime= atol((char *)ucTempBuf);
	if (DevStaticPara.GpsPoweronProtectTime < PARA_GPS_POWERON_PROTECT_FLOOR || DevStaticPara.GpsPoweronProtectTime > PARA_GPS_POWERON_PROTECT_CEIL)
	{
		DevStaticPara.GpsPoweronProtectTime = 5;
	}

	//��ȡ��������
	Para_Read(PARA_SEND_INTERVAL_TYPE, ucTempBuf);
	DevStaticPara.SendInterval = atol((char *)ucTempBuf);
	if (DevStaticPara.SendInterval < PARA_SEND_INTERVAL_FLOOR || DevStaticPara.SendInterval > PARA_SEND_INTERVAL_CEIL)
	{
		DevStaticPara.SendInterval = ITDMA_MSG_SEND_INTERVAL_DEF_VAL;
	}    	

	//��ȡ��������
	Para_Read(PARA_NORMAL_SENDTIME_TYPE, ucTempBuf);
	DevStaticPara.NormalSendTime = atol((char *)ucTempBuf);
	if (DevStaticPara.NormalSendTime < PARA_NORMAL_SENDTIME_FLOOR || DevStaticPara.NormalSendTime > PARA_NORMAL_SENDTIME_CEIL)
	{
		DevStaticPara.NormalSendTime = ITDMA_MSG_SEND_TIME_DEF_VAL;
	} 
#endif
	AisStaticPara.Refresh = TRUE;		 
}

#if 0
/*lq 
* EEPROM������д����
*/
U8 Para_NAME_IndexBuf[PARA_INDEXBUF_LEN][34] = 
{
    /*-----------------REGION_U32--------------------*/
    "PARA_MMSI_TYPE                   ",
    "PARA_MMSI_DEST_TYPE              ",
    "PARA_MMSI_BACKUP_TYPE            ",
    "PARA_MMSI_DEST_BACKUP_TYPE       ",
    "PARA_NOM_LONGITUDE_TYPE          ",
    "PARA_NOM_LATITUDE_TYPE           ",
    "PARA_TX_INTERVAL_CHANNEL_1_TYPE  ",
    "PARA_SERIAL_TYPE                 ",
    "PARA_WORKTIME_TYPE               ",
    "PARA_WORKTIME_LAST_TYPE          ",
    /*-----------------REGION_U16--------------------*/
    "PARA_OFF_POSITION_THRES_TYPE     ",
    "PARA_SHIPA_TYPE                  ",
    "PARA_SHIPB_TYPE                  ",
    "PARA_CHANNEL_1_TYPE              ",
    "PARA_CHANNEL_2_TYPE              ",
    "PARA_TX_START_SLOT_CHANNEL_1_TYPE",
    "PARA_BATTERY_LOW_TYPE            ",
    "PARA_BATTERY_ULTRALOW_TYPE       ",
    "PARA_GPS_WAIT_COUNT_TYPE         ",
    "PARA_GPS_FIX_COUNT_TYPE          ",
    "PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE",
    /*-----------------REGION_U8---------------------*/
    "PARA_ATON_STATUS_TYPE            ",
    "PARA_OFF_POSITION_BEHAV_TYPE     ",
    "PARA_SYNCH_LOST_BEHAV_TYPE       ",
    "PARA_SHIPC_TYPE                  ",
    "PARA_SHIPD_TYPE                  ",
    "PARA_EPFS_TYPE_TYPE              ",
    "PARA_POSITION_ACCURACY_TYPE      ",
    "PARA_POWER_LEVEL_TYPE            ",
    "PARA_ATON_TYPE_TYPE              ",
    "PARA_VIRTUAL_ATON_FLAG_TYPE      ",
    "PARA_TX_HOUR_CHANNEL_1_TYPE      ",
    "PARA_TX_MIN_CHANNEL_1_TYPE       ",
    "PARA_MODAMP_TYPE                 ",
    "PARA_FREQADJ_TYPE                ",
    "PARA_RF_VOLT_ADJUST_TYPE         ",
    "PARA_WDG_RESET_CNT_TYPE          ",
    "PARA_LAST_WDG_RESET_CNT_TYPE     ",
    "PARA_CLOSE_RESION_TYPE           ",
    "PARA_POWER_OPEN_TYPE             ",
    "PARA_SOS_OPEN_TYPE               ",
    "PARA_GPS_HOT_TYPE                ",
    "PARA_CLOSE_TIME_TYPE             ",
    "PARA_GPS_POWERON_PROTECT_TYPE    ",
    "PARA_GPS_FIX_FALL_TYPE           ",
    "PARA_NORMAL_SENDTIME_TYPE        ",
    /*-----------------REGION_CHAR-------------------*/
    "PARA_ATON_NAME_TYPE              ",
    "PARA_VENDID_TYPE                 ",
    "PARA_TIME_OPEN_TYPE              ",
    "PARA_TIME_CLOSE_TYPE             ",
};
U32 Para_VALUE_IndexBuf[PARA_INDEXBUF_LEN] = 
{
    /*-----------------REGION_U32--------------------*/
    12345,
    1,
    0,
    5,
    4294967296,
    4294967295,
    999999999,
    75684,
    535,
    444,
    /*-----------------REGION_U16--------------------*/
    522,
    522,
    522,
    522,
    522,
    522,
    522,
    522,
    522,
    522,
    522,
    /*-----------------REGION_U8---------------------*/
    45,
    45,
    45,
    45,
    45,
    45,
    45,
    45,
    45,
    45,
    45,
    45,
    45,
    45,
    45,
    45,
    45,
    45,
    45,
    45,
    45,
    45,
    45,
    45,
    45,
    /*-----------------REGION_CHAR-------------------*/
    0,
    0,
    0,
    0,
};
/*******************************************************************************
* ����  : EEPROM_Para_Test
* ����  : EEPROM������д����
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void EEPROM_Para_Test(void)
{
    U8 i;
	U8 ucTempBuf[40];

    /*lq ��ֵ�����д*/
    for (i = 0; i < PARA_INDEXBUF_LEN - 4; i++)
    {
#if 0
        sprintf((char*)ucTempBuf, "%*ld", Para_MaxLen_IndexBuf[i], Para_VALUE_IndexBuf[i]);
        if (Para_Save(i, ucTempBuf, Para_MaxLen_IndexBuf[i]) == FALSE)
        {
            usart_printf("%34s : Save failed\r\n", Para_NAME_IndexBuf[i]);        
        }
        Delay_ms(100);
        Feed_Dog();
#endif
        Para_Read(i, ucTempBuf);
        usart_printf("%34s : %s\r\n", Para_NAME_IndexBuf[i], ucTempBuf);
        Delay_ms(100);
        Feed_Dog();
    }

    /*lq �ַ������д*/
    for (i = PARA_INDEXBUF_LEN - 4; i < PARA_INDEXBUF_LEN; i++)
    {
#if 0
        sprintf((char*)ucTempBuf, "%*s", Para_MaxLen_IndexBuf[i], "aaaaaa");
        if (Para_Save(i, ucTempBuf, Para_MaxLen_IndexBuf[i]) == FALSE)
        {
            usart_printf("%34s : Save failed\r\n", Para_NAME_IndexBuf[i]);        
        }
        Delay_ms(100);
        Feed_Dog();
#endif
        
        Para_Read(i, ucTempBuf);
        usart_printf("%34s : %s\r\n", Para_NAME_IndexBuf[i], ucTempBuf);
        Delay_ms(100);
		Feed_Dog();
    }
}
#endif
