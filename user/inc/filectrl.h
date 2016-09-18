/*******************************************************************************
  Copyright (C), 2010, 712.Co.,Ltd.
  File name:	filectrl.h
  Author: �ż���      Version: V1.00       Date: 2015.11.10 
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


#ifndef __FILECTRL_H__
#define __FILECTRL_H__

/* Includes ------------------------------------------------------------------*/
#include "Include.h"
#include "def.h"
#include "sdcard.h"
#include "ff.h"
#include "diskio.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
typedef struct 
{
	U32 MMSI;	 											//�豸MMSI
	U8  ShipName[AIS_INFO_SHIP_NAME_LEN + 1];				//��������
	U8  SerialNumber[9];									//�豸���к�
	U8  SoftwareVersion[9];									//����汾��
	U8  HardwareVersion[6];									//Ӳ���汾��
	U8  CallSign[AIS_INFO_CALL_SIGN_LEN + 1];				//����
	U8  ShipType;											//��������
	U16 ShipA;												//�ߴ�A
	U16 ShipB;												//�ߴ�B
	U8  ShipC;												//�ߴ�C
	U8  ShipD;												//�ߴ�D
	U32 IMONumber;											//IMO���
}FileHeadStruct;

/* Private macro -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void CreateFile(void);
void WriteFile(U8 * buff,UINT bufflen);
void ReadFile(char * FileNameBuff);
void InitFileHead(void);
void AdjustUSART1Band(U32 Band);
void ReadFileName(UINT iNumber);
void ReadMulFile(UINT iNumber);
void CtrlAISAndGPS(UINT iFlag);
FRESULT set_timestamp (
    char *obj,     /* Pointer to the file name */
    int year,
    int month,
    int mday,
    int hour,
    int min,
    int sec
);

#endif
