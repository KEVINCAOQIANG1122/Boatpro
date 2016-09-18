/*******************************************************************************
  Copyright (C), 2010, 712.Co.,Ltd.
  File name:	flashctrl.h
  Author: �ż���      Version: V1.00       Date: 2015.12.21 
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
#ifndef __FLASHCTRL_H__
#define __FLASHCTRL_H__

/* Includes ------------------------------------------------------------------*/
#include "Include.h"
#include "def.h"
#include "config.h"
#include "AisInfo.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
	REGION_U32=0,
	REGION_U16,
	REGION_U8,
	REGION_CHAR
}Para_Type;

/* Private define ------------------------------------------------------------*/

/* ==============STM32��FLASH��ض���================== */
#define STM32_FLASH_PAGES      						256
#define STM32_FLASH_U32_BASE						(STM32_FLASH_BASE+200*0x00000800)
#define STM32_FLASH_U16_BASE						(STM32_FLASH_BASE+201*0x00000800)
#define STM32_FLASH_U8_BASE							(STM32_FLASH_BASE+202*0x00000800)
#define STM32_FLASH_CHAR_BASE						(STM32_FLASH_BASE+203*0x00000800)
#define STM32_FLASH_PAGE_DATA_MAXLEN				100

/*
FLASH�е��������洢��ʽ��
++++++++++REGION_U32:++++++++++++++++++++
  4Byte   1Byte   
������ֵ+ У����

������ֵ�ֶδ�����ͣ�С��ģʽ����ֵ����
У�����ֶδ�����ͣ���ֵ����
�޲����ַ�ʵ�ʳ����ֶ�
++++++++++REGION_U16:++++++++++++++++++++
  2Byte   1Byte   
������ֵ+ У����

������ֵ�ֶδ�����ͣ�С��ģʽ����ֵ����
У�����ֶδ�����ͣ���ֵ����
�޲����ַ�ʵ�ʳ����ֶ�
++++++++++REGION_U8:++++++++++++++++++++
  1Byte   1Byte   
������ֵ+ У����

������ֵ�ֶδ�����ͣ�С��ģʽ����ֵ����
У�����ֶδ�����ͣ���ֵ����
�޲����ַ�ʵ�ʳ����ֶ�
++++++++++REGION_CHAR:++++++++++++++++++++
       1Byte       maxlenByte    1Byte   
�����ַ�ʵ�ʳ��� + ʵ�ʲ���ֵ +  У����

�����ַ�ʵ�ʳ����ֶδ�����ͣ���ֵ����
ʵ�ʲ���ֵ�ֶδ�����ͣ�ASCII
У�����ֶδ�����ͣ���ֵ����

��������ʵ�ʳ���С����󳤶�ʱ����ʵ�ʳ��ȡ��ֶΡ����������ֶμ���У���롱�ֶ�
��������ŵģ��м��޿����ֽ�
*/

/* ======================FLASH�����洢��ַ�궨��========================== */
/*-----------------REGION_U32--------------------*/                                      
#define STM32_FLASH_MMSI_ADDR				    (STM32_FLASH_U32_BASE + 0)         	//MMSI��
#define STM32_FLASH_MMSI_DEST_ADDR             	(STM32_FLASH_U32_BASE + 6)         	//Ŀ��MMSI
#define STM32_FLASH_MMSI_BACKUP_ADDR		    (STM32_FLASH_U32_BASE + 12)        	//MMSI���ݴ洢
#define STM32_FLASH_MMSI_DEST_BACKUP_ADDR      	(STM32_FLASH_U32_BASE + 18)        	//Ŀ��MMSI���ݴ洢
#define STM32_FLASH_SERIAL_ADDR			    	(STM32_FLASH_U32_BASE + 24)			//�������кŴ洢��ַ

/*-----------------REGION_U16--------------------*/
#define STM32_FLASH_SHIPA_ADDR				    (STM32_FLASH_U16_BASE + 0)			//������СA��λ�ô洢��ַ
#define STM32_FLASH_SHIPB_ADDR				    (STM32_FLASH_U16_BASE + 4)			//������СB��λ�ô洢��ַ

/*-----------------REGION_U8---------------------*/
#define STM32_FLASH_MAX_DRAUGHT_ADDR           	(STM32_FLASH_U8_BASE + 0)          	//����ˮ���
#define STM32_FLASH_SHIPC_ADDR				    (STM32_FLASH_U8_BASE + 2)			//������СC��λ�ô洢��ַ
#define STM32_FLASH_SHIPD_ADDR				    (STM32_FLASH_U8_BASE + 4)			//������СD��λ�ô洢��ַ
#define STM32_FLASH_EPFS_TYPE_ADDR             	(STM32_FLASH_U8_BASE + 6)         	//���Ӷ�λϵͳ����
#define STM32_FLASH_SHIP_TYPE_ADDR             	(STM32_FLASH_U8_BASE + 8)         	//��������
#define STM32_FLASH_MODAMP_ADDR			    	(STM32_FLASH_U8_BASE + 10)			//���Ʒ��ȴ洢��ַ
#define STM32_FLASH_FREQADJ_ADDR			    (STM32_FLASH_U8_BASE + 12)			//����Ƶ��׼ȷ�ȴ洢��ַ
#define STM32_FLASH_RF_VOLT_ADJUST_ADDR        	(STM32_FLASH_U8_BASE + 14)         	//դѹDA���Ƶ���
#define STM32_FLASH_DTE_ADDR                   	(STM32_FLASH_U8_BASE + 16)         	//DTE
#define STM32_FLASH_AIS_CLASS_ADDR             	(STM32_FLASH_U8_BASE + 18)         	//AIS���

/*-----------------REGION_CHAR-------------------*/
#define STM32_FLASH_DESTINATION_ADDR           	(STM32_FLASH_CHAR_BASE + 0)        	//Ŀ�ĵ�	   20+1+1
#define STM32_FLASH_VENDID_ADDR			    	(STM32_FLASH_CHAR_BASE + 22)		//������Ϣ	   7+1+1
#define STM32_FLASH_SHIP_NAME_ADDR             	(STM32_FLASH_CHAR_BASE + 32)       	//��������	   20+1+1
#define STM32_FLASH_HWVERSION_ADDR             	(STM32_FLASH_CHAR_BASE + 54)       	//Ӳ���汾��   5+1+1

/* ====================FLASH�����洢�������Ⱥ궨��======================== */
/*
 *  REGION_CHAR�����洢����ʱ���õ�MAXLEN��
 *  REGION_U32��REGION_U16��REGION_U8�����洢����ʱ��ʹ�ã������洢�ĳ��ȹ̶�����
 *  �ڲ��������ñ����л��õ�
 */
/*-----------------REGION_U32--------------------*/
#define STM32_FLASH_MMSI_MAXLEN			    		9 			                            //MMSI�洢��󳤶�
#define STM32_FLASH_MMSI_DEST_MAXLEN           		9                                       //Ŀ��MMSI
#define STM32_FLASH_MMSI_BACKUP_MAXLEN		    	9                                       //mmsi���ݴ洢����
#define STM32_FLASH_MMSI_DEST_BACKUP_MAXLEN    		9                                       //Ŀ��mmsi���ݴ洢����
#define STM32_FLASH_SERIAL_MAXLEN			    	8			                            //�������кŴ洢��󳤶�
/*-----------------REGION_U16--------------------*/
#define STM32_FLASH_SHIPA_MAXLEN			    	3			                            //������СA��λ�ô洢��󳤶�
#define STM32_FLASH_SHIPB_MAXLEN			    	3			                            //������СB��λ�ô洢��󳤶�
/*-----------------REGION_U8---------------------*/
#define STM32_FLASH_MAX_DRAUGHT_MAXLEN         		3                                       //����ˮ���
#define STM32_FLASH_SHIPC_MAXLEN			    	2			                            //������СC��λ�ô洢��󳤶�
#define STM32_FLASH_SHIPD_MAXLEN			    	2			                            //������СD��λ�ô洢��󳤶�
#define STM32_FLASH_EPFS_TYPE_MAXLEN           		2                                       //���Ӷ�λϵͳ����
#define STM32_FLASH_SHIP_TYPE_MAXLEN           		3                                       //��������
#define STM32_FLASH_MODAMP_MAXLEN			    	2			                            //���Ʒ��ȴ洢��󳤶�
#define STM32_FLASH_FREQADJ_MAXLEN			    	2			                            //���书�ʴ洢��󳤶�
#define STM32_FLASH_RF_VOLT_ADJUST_MAXLEN      		2                                       //դѹDA���Ƶ���
#define STM32_FLASH_DTE_MAXLEN                 		1                                       //DTE
#define STM32_FLASH_AIS_CLASS_MAXLEN           		1                                       //AIS���
/*-----------------REGION_CHAR-------------------*/
#define STM32_FLASH_DESTINATION_MAXLEN         		20                                      //Ŀ�ĵ�
#define STM32_FLASH_VENDID_MAXLEN			    	7			                            //������Ϣ
#define STM32_FLASH_SHIP_NAME_MAXLEN 		    	20                                      //��������
#define STM32_FLASH_HWVERSION_MAXLEN           		5                                       //Ӳ���汾��

/* ===================FLASH�����洢���ͺ궨��============================= */
/*-----------------REGION_U32--------------------*/
#define STM32_FLASH_MMSI_TYPE			        	0 			                            //MMSI�洢��󳤶�
#define STM32_FLASH_MMSI_DEST_TYPE             		1                                       //Ŀ��MMSI
#define STM32_FLASH_MMSI_BACKUP_TYPE		    	2                                       //mmsi���ݴ洢����
#define STM32_FLASH_MMSI_DEST_BACKUP_TYPE      		3                                       //Ŀ��mmsi���ݴ洢����
#define STM32_FLASH_SERIAL_TYPE			    		4			                            //�������кŴ洢��󳤶�
/*-----------------REGION_U16--------------------*/
#define STM32_FLASH_SHIPA_TYPE			        	5			                            //������СA��λ�ô洢��󳤶�
#define STM32_FLASH_SHIPB_TYPE			        	6			                            //������СB��λ�ô洢��󳤶�
/*-----------------REGION_U8---------------------*/
#define STM32_FLASH_MAX_DRAUGHT_TYPE           		7                                      //����ˮ���
#define STM32_FLASH_SHIPC_TYPE			        	8			                            //������СC��λ�ô洢��󳤶�
#define STM32_FLASH_SHIPD_TYPE			        	9			                            //������СD��λ�ô洢��󳤶�
#define STM32_FLASH_EPFS_TYPE_TYPE             		10                                      //���Ӷ�λϵͳ����
#define STM32_FLASH_SHIP_TYPE_TYPE             		11                                      //��������
#define STM32_FLASH_MODAMP_TYPE			    		12			                            //���Ʒ��ȴ洢��󳤶�
#define STM32_FLASH_FREQADJ_TYPE			    	13			                            //���书�ʴ洢��󳤶�
#define STM32_FLASH_RF_VOLT_ADJUST_TYPE        		14                                      //դѹDA���Ƶ���
#define STM32_FLASH_DTE_TYPE                   		15                                      //DTE
#define STM32_FLASH_AIS_CLASS_TYPE             		16                                      //AIS���
/*-----------------REGION_CHAR-------------------*/
#define STM32_FLASH_DESTINATION_TYPE           		17                                      //Ŀ�ĵ�
#define STM32_FLASH_VENDID_TYPE			    		18			                            //������Ϣ
#define STM32_FLASH_SHIP_NAME_TYPE 		    		19                                      //��������
#define STM32_FLASH_HWVERSION_TYPE             		20                                      //Ӳ���汾��

/* FLASH�����ܸ���*/
#define STM32_FLASH_INDEXBUF_LEN			    	21

/* ==============FLASH�����ϣ�ceil��/�£�floor���޺궨��================== */
/*-----------------REGION_U32--------------------*/
#define STM32_FLASH_MMSI_FLOOR			        (AIS_INFO_MMSI_MIN) 	                //MMSI�洢��󳤶�
#define STM32_FLASH_MMSI_CEIL			        (AIS_INFO_MMSI_MAX)                     //MMSI�洢��󳤶�
#define STM32_FLASH_MMSI_DEST_FLOOR            	(AIS_INFO_MMSI_MIN)                     //Ŀ��MMSI
#define STM32_FLASH_MMSI_DEST_CEIL             	(AIS_INFO_MMSI_MAX)                     //Ŀ��MMSI
#define STM32_FLASH_MMSI_BACKUP_FLOOR	    	(AIS_INFO_MMSI_MIN)                     //MMSI���ݴ洢����
#define STM32_FLASH_MMSI_BACKUP_CEIL		    (AIS_INFO_MMSI_MAX)                     //MMSI���ݴ洢����
#define STM32_FLASH_MMSI_DEST_BACKUP_FLOOR     	(AIS_INFO_MMSI_MIN)                     //Ŀ��MMSI���ݴ洢����
#define STM32_FLASH_MMSI_DEST_BACKUP_CEIL      	(AIS_INFO_MMSI_MAX)                     //Ŀ��MMSI���ݴ洢����
#define STM32_FLASH_SERIAL_FLOOR			    (DEV_INFO_SERIAL_MIN)			        //�������кŴ洢��󳤶�
#define STM32_FLASH_SERIAL_CEIL			    	(DEV_INFO_SERIAL_MIN)                   //�������кŴ洢��󳤶�
/*-----------------REGION_U16--------------------*/
#define STM32_FLASH_SHIPA_FLOOR			    	(AIS_INFO_SHIPA_MIN)                    //������СA��λ�ô洢��󳤶�
#define STM32_FLASH_SHIPA_CEIL			        (AIS_INFO_SHIPA_MAX)	                //������СA��λ�ô洢��󳤶�
#define STM32_FLASH_SHIPB_FLOOR			    	(AIS_INFO_SHIPB_MIN)	                //������СB��λ�ô洢��󳤶�
#define STM32_FLASH_SHIPB_CEIL			        (AIS_INFO_SHIPB_MAX)	                //������СB��λ�ô洢��󳤶�
/*-----------------REGION_U8---------------------*/
#define STM32_FLASH_MAX_DRAUGHT_FLOOR          	(AIS_INFO_MAX_DRAUGHT_MIN)              //����ˮ���
#define STM32_FLASH_MAX_DRAUGHT_CEIL           	(AIS_INFO_MAX_DRAUGHT_MAX)              //����ˮ���
#define STM32_FLASH_SHIPC_FLOOR			    	(AIS_INFO_SHIPC_MIN)	                //������СC��λ�ô洢��󳤶�
#define STM32_FLASH_SHIPC_CEIL			        (AIS_INFO_SHIPC_MAX)	                //������СC��λ�ô洢��󳤶�
#define STM32_FLASH_SHIPD_FLOOR			    	(AIS_INFO_SHIPD_MIN)	                //������СD��λ�ô洢��󳤶�
#define STM32_FLASH_SHIPD_CEIL			        (AIS_INFO_SHIPD_MAX)	                //������СD��λ�ô洢��󳤶�
#define STM32_FLASH_EPFS_TYPE_FLOOR            	(AIS_INFO_EPFS_TYPE_MIN)                //���Ӷ�λϵͳ����
#define STM32_FLASH_EPFS_TYPE_CEIL             	(AIS_INFO_EPFS_TYPE_MAX)                //���Ӷ�λϵͳ����
#define STM32_FLASH_SHIP_TYPE_FLOOR            	(AIS_INFO_SHIP_TYPE_MIN)                //��������
#define STM32_FLASH_SHIP_TYPE_CEIL             	(AIS_INFO_SHIP_TYPE_MAX)                //��������
#define STM32_FLASH_MODAMP_FLOOR			    16			                            //���Ʒ��ȴ洢��󳤶�
#define STM32_FLASH_MODAMP_CEIL			    	48			                            //���Ʒ��ȴ洢��󳤶�
#define STM32_FLASH_FREQADJ_FLOOR			    16			                            //���书�ʴ洢��󳤶�
#define STM32_FLASH_FREQADJ_CEIL			    48			                            //���书�ʴ洢��󳤶�
#define STM32_FLASH_RF_VOLT_ADJUST_FLOOR       	0           
#define STM32_FLASH_RF_VOLT_ADJUST_CEIL        	30  
#define STM32_FLASH_DTE_FLOOR                  	(AIS_INFO_DTE_MIN)                      //DTE
#define STM32_FLASH_DTE_CEIL                   	(AIS_INFO_DTE_MAX)                      //DTE
#define STM32_FLASH_AIS_CLASS_FLOOR            	(AIS_CLASS_A)                           //AIS���
#define STM32_FLASH_AIS_CLASS_CEIL             	(AIS_CLASS_B)                           //AIS���
/*-----------------REGION_CHAR-------------------*/
// ����ֵ���Ͳ�������/����ֵ

/* =======================EEPROM����Ĭ��ֵ�궨��=============================== */
/*-----------------REGION_U32--------------------*/
#define STM32_FLASH_MMSI_DEFAULT		        (AIS_INFO_MMSI_DEFAULT)                 //MMSI�洢��󳤶�
#define STM32_FLASH_MMSI_DEST_DEFAULT          	(AIS_INFO_MMSI_DEFAULT)                 //Ŀ��MMSI
#define STM32_FLASH_MMSI_BACKUP_DEFAULT	    	(AIS_INFO_MMSI_DEFAULT)                 //mmsi���ݴ洢����
#define STM32_FLASH_MMSI_DEST_BACKUP_DEFAULT   	(AIS_INFO_MMSI_DEFAULT)                 //Ŀ��mmsi���ݴ洢����
#define STM32_FLASH_SERIAL_DEFAULT			    0			                            //�������кŴ洢��󳤶�
///*-----------------REGION_U16--------------------*/
#define STM32_FLASH_SHIPA_DEFAULT			    (AIS_INFO_SHIPA_DEFAULT)                //������СA��λ�ô洢��󳤶�
#define STM32_FLASH_SHIPB_DEFAULT			    (AIS_INFO_SHIPB_DEFAULT)	            //������СB��λ�ô洢��󳤶�
///*-----------------REGION_U8--------------------*/
#define STM32_FLASH_MAX_DRAUGHT_DEFAULT        	(AIS_INFO_MAX_DRAUGHT_DEFAULT)          //����ˮ���
#define STM32_FLASH_SHIPC_DEFAULT			    (AIS_INFO_SHIPC_DEFAULT)	            //������СC��λ�ô洢��󳤶�
#define STM32_FLASH_SHIPD_DEFAULT			    (AIS_INFO_SHIPD_DEFAULT)	            //������СD��λ�ô洢��󳤶�
#define STM32_FLASH_EPFS_TYPE_DEFAULT          	(AIS_INFO_EPFS_TYPE_DEFAULT)            //���Ӷ�λϵͳ���ͣ�Ĭ��ֵ1-GPS
#define STM32_FLASH_SHIP_TYPE_DEFAULT          	(AIS_INFO_SHIP_TYPE_DEFAULT)            //�������ͣ�Ĭ��ֵ0-not available
#define STM32_FLASH_MODAMP_DEFAULT			    (DEV_INFO_MODAMP_DEFAULT)	            //���Ʒ��ȴ洢��󳤶�
#define STM32_FLASH_FREQADJ_DEFAULT		    	(DEV_INFO_FREQADJ_DEFAULT)	            //���书�ʴ洢��󳤶�
#define STM32_FLASH_RF_VOLT_ADJUST_DEFAULT     	(DEV_INFO_RF_VOLT_ADJUST_DEFAULT)       
#define STM32_FLASH_DTE_DEFAULT                	(AIS_INFO_DTE_DEFAULT)                  //lq ��λ1��
#define STM32_FLASH_AIS_CLASS_DEFAULT          	(AIS_CLASS_DEFAULT)                     
/*-----------------REGION_CHAR-------------------*/
#define STM32_FLASH_DESTINATION_DEFAULT        	(AIS_INFO_DESTINATION_DEFAULT)          //Ŀ�ĵ�
#define STM32_FLASH_VENDID_DEFAULT			    (AIS_INFO_VEND_ID_DEFAULT)
#define STM32_FLASH_SHIP_NAME_DEFAULT          	(AIS_INFO_SHIP_NAME_DEFAULT)
#define STM32_FLASH_HWVERSION_DEFAULT          	"0.0.0"                                 //Ӳ���汾��

/* Private macro -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
U8 FlashWrite(U8 ucPara_Type, U8 *pPara, U8 ucLen);
U8 FlashRead(U8 ucPara_Type, U8 *pPara);
U8 U8ToU16(U8 *ucTemp,U16 *usTemp,U8 ucLen);
U8 U16ToU8(U16 *usTemp,U8 *ucTemp,U8 usLen);
void ReadCHAR(U8 *u8Temp,U16 *u16Temp,U8 u8Len);
u16 FLASH_ReadHalfWord(u32 address);
U8 STM32_FLASH_DefaultRead(U8 ucPara_Type, U8 *pPara);
#endif
