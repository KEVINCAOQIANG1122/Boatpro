/*******************************************************************************
  Copyright (C), 2010, 712.Co.,Ltd.
  File name:	encrypt.h
  Author: �ż���      Version: V1.00       Date: 2016.03.10 
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
*******************************************************************************/

#ifndef __ENCRYPT_H
#define __ENCRYPT_H

/* Includes ------------------------------------------------------------------*/
#include "Include.h"
#include "def.h"
#include "config.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum CryType
{
	CRY_MAIN,
	CRY_BACKUP1,
	CRY_BACKUP2
}CryTypeEnum;

/* Private define ------------------------------------------------------------*/
/*-----------------���ĵ�ַ����--------------------*/
#define STM32_FLASH_CRY_BASE_ADDR  				(STM32_FLASH_U8_BASE + 20)					//������ʼ�洢��ַ
#define STM32_FLASH_CRY_0_7_ADDR				(STM32_FLASH_CRY_BASE_ADDR + 0)				//���ĵ�0_7λ
#define STM32_FLASH_CRY_0_7_BACKUP1_ADDR		(STM32_FLASH_CRY_BASE_ADDR + 2)				//���ı���1��0_7λ
#define STM32_FLASH_CRY_0_7_BACKUP2_ADDR		(STM32_FLASH_CRY_BASE_ADDR + 4)				//���ı���2��0_7λ
#define STM32_FLASH_CRY_8_15_ADDR				(STM32_FLASH_CRY_BASE_ADDR + 6)				//���ĵ�8_15λ
#define STM32_FLASH_CRY_8_15_BACKUP1_ADDR		(STM32_FLASH_CRY_BASE_ADDR + 8)				//���ı���1��8_15λ
#define STM32_FLASH_CRY_8_15_BACKUP2_ADDR		(STM32_FLASH_CRY_BASE_ADDR + 10)			//���ı���2��8_15λ
#define STM32_FLASH_CRY_16_23_ADDR				(STM32_FLASH_CRY_BASE_ADDR + 12)			//���ĵ�16_23λ
#define STM32_FLASH_CRY_16_23_BACKUP1_ADDR		(STM32_FLASH_CRY_BASE_ADDR + 14)			//���ı���1��16_23λ
#define STM32_FLASH_CRY_16_23_BACKUP2_ADDR		(STM32_FLASH_CRY_BASE_ADDR + 16)			//���ı���2��16_23λ
#define STM32_FLASH_CRY_23_31_ADDR				(STM32_FLASH_CRY_BASE_ADDR + 18)			//���ĵ�24_31λ
#define STM32_FLASH_CRY_23_31_BACKUP1_ADDR		(STM32_FLASH_CRY_BASE_ADDR + 20)			//���ı���1��24_31λ
#define STM32_FLASH_CRY_23_31_BACKUP2_ADDR		(STM32_FLASH_CRY_BASE_ADDR + 22)			//���ı���2��24_31λ
#define STM32_FLASH_CRY_CRC_0_7_ADDR			(STM32_FLASH_CRY_BASE_ADDR + 24)			//����CRCУ���0_7λ
#define STM32_FLASH_CRY_CRC_0_7_BACKUP1_ADDR	(STM32_FLASH_CRY_BASE_ADDR + 26)			//����CRCУ�鱸��1��0_7λ
#define STM32_FLASH_CRY_CRC_0_7_BACKUP2_ADDR	(STM32_FLASH_CRY_BASE_ADDR + 28)			//����CRCУ�鱸��2��0_7λ
#define STM32_FLASH_CRY_CRC_8_15_ADDR			(STM32_FLASH_CRY_BASE_ADDR + 30)			//����CRCУ���8_15λ
#define STM32_FLASH_CRY_CRC_8_15_BACKUP1_ADDR	(STM32_FLASH_CRY_BASE_ADDR + 32)			//����CRCУ�鱸��1��8_15λ
#define STM32_FLASH_CRY_CRC_8_15_BACKUP2_ADDR	(STM32_FLASH_CRY_BASE_ADDR + 34)			//����CRCУ�鱸��2��8_15λ
/* Private variables ---------------------------------------------------------*/

/* Private  function  ---------------------------------------------------------*/

/* Public define ------------------------------------------------------------*/

/* Public  variables ---------------------------------------------------------*/
U32 Decode(U32 ulSrc, U32 *ulKeyBuff, U8 KeySize);
void ReadDeviceID(U32 *ucIDBuff);
U32 ReadCry(U8 ucCryType);
void WriteCry(U8 ucCryType, U8 *ucCryBuff);    // lnw  modify     before    U8 return but  not return in  function
void FlashReadCry(U8 *ucCryBuff, U32 ulAddr);
void FlashWriteCry(U8 *ucCryBuff, U32 ulAddr);

#endif
