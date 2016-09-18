/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: filectrl.c
  Author: �ż���       Version : V1.0.0        Date: 2015.11.10
  Description:     SD���ļ�����ģ��
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
#include "filectrl.h"
#include "def.h"
#include "parastorage.h"
#include "usart.h"
#include "flashctrl.h"
#include "sdcard.h"
#include "ff.h"
#include "diskio.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define FILE_LENGTH_MAX						0x200000//1024*256//0x200000       //�������ļ�¼�ļ������Ϊ2MB

/* Private macro -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/
FATFS fs;            					// Work area (file system object) for logical drive
FIL fp,fprm, fdst;      				// file objects
BYTE buffer[512]; 						// file copy buffer
FRESULT res;         					// FatFs function common result code
UINT br, bw;         					// File R/W count
U8 FileName[13];			 			//FileName = "TCB"+��λ��λ�����ţ����㿪ʼ������
U8 RecordName[13]="FileName.txt";		//�����ļ��洢��¼
FileHeadStruct FileHead;				//ÿ�����ļ�¼���ļ�ͷ

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* ����  : CreateFile
* ����  : �����ļ�����
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void CreateFile()
{
	FRESULT res;
	char cData[50];
	U8 Temp[17];
	UINT count=0;
	UINT a;
	U8 HardwareVersion[6]; 
	
	InitFileHead();				//��ʼ���ļ���ͷ

	/**************���¼�ļ�д�뱨�ļ�¼�ļ���************************************/
	res=f_mount(&fs,"",0);
	if(res)
	{
		//debug_printf("Error 1 %d\r\n",(U8)res);
		return ;
	}
	res=f_open(&fp, RecordName, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
	if(res)
	{
		//debug_printf("Error 2 %d\r\n",(U8)res);
		return ;
	}
	//while (f_gets(cTemp, 13, &fsrc))		//�洢�ļ���ʱ������
	while (f_gets(cData, 15, &fp))		//�洢�ļ���ʱ����
	{
		count++;
	}
	if(count>=10000)
	{
		sprintf(FileName, "TCB");
		sprintf(Temp,"%d",count);
		strcat(FileName, Temp);
		strcat(FileName, ".txt");
	}
	else if((count>=1000)&&(count<10000))
	{ 
		sprintf(FileName, "TCB0");
		sprintf(Temp,"%d",count);
		strcat(FileName, Temp);
		strcat(FileName, ".txt");	 
	}
	else if((count>=100)&&(count<1000))
	{
		sprintf(FileName, "TCB00");
		sprintf(Temp,"%d",count);
		strcat(FileName, Temp);
		strcat(FileName, ".txt"); 	 
	}
	else if((count>=10)&&(count<100))
	{ 
		sprintf(FileName, "TCB000");
		sprintf(Temp,"%d",count);
		strcat(FileName, Temp);
		strcat(FileName, ".txt");	 
	}
	else if((count>=0)&&(count<10))
	{
		sprintf(FileName, "TCB0000");
		sprintf(Temp,"%d",count);
		strcat(FileName, Temp);
		strcat(FileName, ".txt");
	}	
	strcat(FileName,"\r\n");			  //�洢�ļ���ʱ����
	//res = f_write(&fsrc, FileName,12,&a);	 //�洢�ļ���ʱ������
	res = f_write(&fp, FileName,14,&a);	 //�洢�ļ���ʱ����
	if(res)
	{
		f_close(&fp);
		return ;
	}		
	f_close(&fp);

	/***************�������ļ�¼***************************************/
	res=f_open(&fp, FileName, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
	if(res)
 	{
		//debug_printf("Error 4 %d\r\n",(U8)res);
		return ;
	}
	/****************�����ļ���ͷ**************************************/
	f_printf(&fp, "MMSI : %D\r\n", FileHead.MMSI);
	f_printf(&fp, "ShipName : %s\r\n", FileHead.ShipName);
	f_printf(&fp, "SerialNumber : %s\r\n", FileHead.SerialNumber);
	f_printf(&fp, "SoftwareVersion : %s\r\n", FileHead.SoftwareVersion);
	//f_printf(&fp, "HardwareVersion : %s\r\n", FileHead.HardwareVersion);
	//Para_Read(PARA_HWVERSION_TYPE, HardwareVersion);
	FlashRead(STM32_FLASH_HWVERSION_TYPE, HardwareVersion);
	f_printf(&fp, "HardwareVersion : %s\r\n", HardwareVersion);
	f_printf(&fp, "CallSign : %s\r\n", FileHead.CallSign); 
	f_printf(&fp, "ShipType : %d\r\n", FileHead.ShipType);
	f_printf(&fp, "IMONumber : %d\r\n", FileHead.IMONumber);  
	f_printf(&fp, "ShipParameter ��A=%d,B=%d,C=%d,D=%d\r\n",
			FileHead.ShipA, FileHead.ShipB, FileHead.ShipC, FileHead.ShipD);
	f_close(&fp);
}

/*******************************************************************************
* ����  : WriteFile
* ����  : д��SD���ļ�����
* ����  : buffд�������
		  bufflenд�����ݵĳ���	
* ���  : ��
* ����  : ��
*******************************************************************************/
void WriteFile(U8 * buff,UINT bufflen)
{
	FRESULT res;
	UINT a;
	
	f_mount(&fs,"",0);
	res = f_open(&fp, FileName, FA_WRITE | FA_OPEN_ALWAYS);
	/**************************�Ե����ļ��Ĵ�С�������ƣ�����Ϊ2M***********************/
	if(f_size(&fp) > FILE_LENGTH_MAX)
	{
		f_close(&fp);
		CreateFile();
		res=f_open(&fp, FileName, FA_WRITE | FA_OPEN_ALWAYS);
	}
	res = f_lseek(&fp,f_size(&fp));					   		//��ָ��ָ���ļ���β��
	res = f_write(&fp, buff,bufflen,&a);				    //���ļ�β��д������
	f_close(&fp);	
}

/*******************************************************************************
* ����  : ReadFile
* ����  :��ȡSD���ļ�����
* ����  : FileNameBuffָ��Ԥ���ļ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void ReadFile(char * FileNameBuff)
{
	FRESULT res;
	char cData[200];
	
	f_mount(&fs,"",0);
	res=f_open(&fp, FileNameBuff, FA_READ | FA_OPEN_EXISTING);		 //��ֻ����ʽ�򿪣�����ļ�����������ô����ʾ��ʧ��
	
	//CtrlAISAndGPS(0);												  // AIS��GPS��Ч
		
	//AdjustUSART1Band(115200);
	strcat(FileNameBuff,"\r\n");
	UartTxDataBuf(1, (U8*)FileNameBuff, strlen(FileNameBuff));                                                // ����USB����
	while (f_gets(cData, 256, &fp))		
	{
		UartTxDataBuf(1, (U8*)cData, strlen(cData));
		Delay_ms(25);														//25ms��ʱ��
		Feed_Dog();
	}
	f_close(&fp);
	//AdjustUSART1Band(38400);                                                //lq ����USB����

	//CtrlAISAndGPS(1);														// AIS��GPS��Ч

	Feed_Dog();
}

/*******************************************************************************
* ����  : InitFileHead
* ����  : ��ʼ���ļ�ͷ����
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitFileHead()
{
	U8  SerialNumber[9];			//�豸���к�
	U8  HardwareVersion[6];			//Ӳ���汾��
	U8  MMSI[10];
	UINT i;

	/******************����AIS��̬����************************************/
	FileHead.MMSI = AisStaticPara.MMSI;
	sprintf(FileHead.ShipName, AisStaticPara.ShipName);	
	sprintf(FileHead.CallSign, AisStaticPara.CallSign);
	FileHead.ShipType = AisStaticPara.TypeOfShip;
	FileHead.IMONumber = AisStaticPara.IMONumber;
	FileHead.ShipA = AisStaticPara.ShipA; 
	FileHead.ShipB = AisStaticPara.ShipB;
	FileHead.ShipC = AisStaticPara.ShipC;
	FileHead.ShipD = AisStaticPara.ShipD;
	
	/******************����EEROM����************************************/
	//Para_Read(PARA_MMSI_TYPE,MMSI);
	FlashRead(STM32_FLASH_MMSI_TYPE, MMSI);
	FileHead.MMSI = atol(MMSI);
	//Para_Read(PARA_SERIAL_TYPE,SerialNumber);
	FlashRead(STM32_FLASH_SERIAL_TYPE, SerialNumber);
	sprintf(FileHead.SerialNumber, SerialNumber);
	//Para_Read(PARA_HWVERSION_TYPE, HardwareVersion);
	FlashRead(STM32_FLASH_HWVERSION_TYPE, HardwareVersion);
	sprintf(FileHead.HardwareVersion, HardwareVersion);

	/******************�����Զ������************************************/
	sprintf(FileHead.SoftwareVersion, "%s", VERSION); 
}

/*******************************************************************************
* ����  : AdjustUSART1Band
* ����  : �޸Ĵ���1�Ĳ�����
* ����  : Band������
* ���  : ��
* ����  : ��
*******************************************************************************/
void AdjustUSART1Band(U32 Band)
{
	Feed_Dog();
	InitUsart1(Band);
	Feed_Dog();
    USART1_UsartConfigStructInit(Band, 8, 0, 1);
	Feed_Dog();
}

/*******************************************************************************
* ����  : ReadFileName
* ����  :��ȡSD���ļ�������
* ����  : iNumber�ļ�������
* ���  : ��
* ����  : ��
*******************************************************************************/
void ReadFileName(UINT iNumber)
{
	FRESULT res;
	char cName[20];
	
	f_mount(&fs,"",0);
	res=f_open(&fp, RecordName, FA_READ | FA_OPEN_EXISTING);		 //��ֻ����ʽ�򿪣�����ļ�����������ô����ʾ��ʧ��
	
	//CtrlAISAndGPS(0);														 // AIS��GPS��Ч
	
	if(iNumber != 0)														   // ����ȫ���ļ���
	{
		f_lseek(&fp,(f_size(&fp)-14*iNumber));							// �ҵ���һ��ָ���ļ�����λ��
	}
	while (f_gets(cName, 15, &fp))		
	{
		UartTxDataBuf(1, (U8*)cName, strlen(cName));
		Delay_ms(25);														//25ms��ʱ��
		Feed_Dog();
	}
	f_close(&fp);

	//CtrlAISAndGPS(1);														// AIS��GPS��Ч

	Feed_Dog();	
}

/*******************************************************************************
* ����  : ReadMulFile
* ����  :��ȡSD���ļ�������
* ����  :iNumber�ļ����� 
* ���  : ��
* ����  : ��
*******************************************************************************/
void ReadMulFile(UINT iNumber)
{
	FRESULT res;
	char cFileName[20]={""};
	char cName[20];
	char cData[200];
	
	f_mount(&fs,"",0);
	res=f_open(&fprm, RecordName, FA_READ | FA_OPEN_EXISTING);		 //��ֻ����ʽ�򿪣�����ļ�����������ô����ʾ��ʧ��
	
	//CtrlAISAndGPS(0);												 // AIS��GPS��Ч

	if(14*iNumber < f_size(&fprm))											// ���ļ���������iNumberʱִ�У����򲻽����ļ���λ�õ�ָ��
	{ 
		f_lseek(&fprm,(f_size(&fprm)-14*iNumber));							// �ҵ���һ��ָ���ļ�����λ��
	}
	while (f_gets(cName, 15, &fprm))									// ��ȡ�ļ�
	{
		strncpy(cFileName,cName,12);								// ��ȡȥ�������ַ����ļ���
		UartTxDataBuf(1, (U8*)cName, strlen(cName));				// ������ļ����ļ���

		/***********************��ȡĿ���ļ�*********************************/
		f_open(&fp, cFileName, FA_READ | FA_OPEN_EXISTING);		//��ֻ����ʽ�򿪣�����ļ�����������ô����ʾ��ʧ��
		while (f_gets(cData, 256, &fp))		
		{
			UartTxDataBuf(1, (U8*)cData, strlen(cData));
			Delay_ms(25);														//25ms��ʱ��
			Feed_Dog();
		}
		f_close(&fp);
		Delay_ms(25);												//25ms��ʱ��
		Feed_Dog();
	}
	f_close(&fprm);

    //CtrlAISAndGPS(1);														// AIS��GPS��Ч
	
	Feed_Dog();		
}

/*******************************************************************************
* ����  : CtrlAISAndGPS
* ����  :����AIS�Լ�GPS���ܺ���
* ����  : iFlag���Ʊ�־
* ���  : ��
* ����  : ��
*******************************************************************************/
void CtrlAISAndGPS(UINT iFlag)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	if(iFlag == 0)							  // AIS��GPS��Ч
	{ 	
		/*******************��ֹ����AIS��Ϣ�Լ�GPS��Ϣ������Խ��**************************************/
		EXTI_InitStructure.EXTI_Line = SI446X_A_DCLK_LINE | SI446X_B_DCLK_LINE 
									   | PULSE_EN_LINE; 
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
	    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	    EXTI_Init(&EXTI_InitStructure);
	}
	else if(iFlag == 1)						 // AIS��GPS��Ч
	{
		/*******************��ʼ�����ա������Լ�GPS�Ĵ洢�ṹ�壬�޳���Ч����**************************************/
		LME_ModuleInit(); 															//����ͽ���֡���г�ʼ��
		DLS_InstPacketProbeStructInit();
		UartTxFIFOStructInit(&Uart2TxFIFO);											//��ʼ��GPS�ṹ��
	    UartRxFIFOStructInit(&Uart2RxFIFO);											//��ʼ��GPS�ṹ��
		UBX_ModuleInit();															//��ʼ��GPSģ�����
	
		/*******************�ظ�����AIS��Ϣ�Լ�GPS��Ϣ**************************************/
		EXTI_InitStructure.EXTI_Line = SI446X_A_DCLK_LINE | SI446X_B_DCLK_LINE 
									   | PULSE_EN_LINE; 
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
	    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	    EXTI_Init(&EXTI_InitStructure);
	}
	Feed_Dog();	
}
