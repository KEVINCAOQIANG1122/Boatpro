/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: parastorage.h
  Author: lq       Version : V1.00        Date: 2013.12.05
  Description:     EEPROM�����궨��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
    <author>    <time>      <version >  <desc>
      lq        20131206                �����洢��ʽ��ʡȥ��1B�ļ��
      lq        20131230                �޸�EEPROM�в����Ĵ洢��ʽ
*******************************************************************************/

#ifndef _PARASTORAGE_H
#define _PARASTORAGE_H
/* Includes ------------------------------------------------------------------*/
#include "config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/*lq
EEPROM�е��������洢��ʽ��
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

/*lq ======================EEPROM�����洢��ַ�궨��========================== */
/*-----------------REGION_U32--------------------*/
#define PARA_REGION_U32_BASE_ADDR       2                                       //lq EEPROM����ʼ2���ֽڲ��ã����ȶ���
#define PARA_WORKTIME_ADDR              (PARA_REGION_U32_BASE_ADDR + 0)        //������ʱ��
#define PARA_WORKTIME_LAST_ADDR         (PARA_REGION_U32_BASE_ADDR + 5)        //�ϴι�����ʱ��
#define PARA_SAILING_DISTINCE_ADDR      (PARA_REGION_U32_BASE_ADDR + 10)       //�����ܾ��루��λ���ף�
/*-----------------REGION_U16--------------------*/
#define PARA_REGION_U16_BASE_ADDR       (PARA_REGION_U32_BASE_ADDR + 15)        // address = 17
#define PARA_PERSONS_ADDR				(PARA_REGION_U16_BASE_ADDR + 0)			//��������
#define PARA_WDG_RESET_CNT_ADDR	        (PARA_REGION_U8_BASE_ADDR + 4)         //�ӱ����������������ڣ����Ź���λ����
#define PARA_LAST_WDG_RESET_CNT_ADDR    (PARA_REGION_U8_BASE_ADDR + 7)         //���ϴ����������������������������Ź���λ����
#define PARA_NORMAL_POWER_ON_CNT_ADDR		(PARA_REGION_U8_BASE_ADDR + 10)		//������������
/*-----------------REGION_U8---------------------*/
#define PARA_REGION_U8_BASE_ADDR        (PARA_REGION_U16_BASE_ADDR + 13)         // address = 30
#define PARA_ETA_MONTH_ADDR             (PARA_REGION_U8_BASE_ADDR + 0)          //���Ƶ���ʱ�䣬��
#define PARA_NAVIGATION_STATUS_ADDR     (PARA_REGION_U8_BASE_ADDR + 2)          //����״̬
#define PARA_REGION_APP_FLAG_ADDR       (PARA_REGION_U8_BASE_ADDR + 4)         //����Ӧ�ñ�־
#define PARA_POWER_LEVEL_ADDR           (PARA_REGION_U8_BASE_ADDR + 6)         //����ˮƽ
#define PARA_ETA_DAY_ADDR               (PARA_REGION_U8_BASE_ADDR + 8)         //���Ƶ���ʱ�䣬��
#define PARA_ETA_HOUR_ADDR              (PARA_REGION_U8_BASE_ADDR + 10)         //���Ƶ���ʱ�䣬ʱ
#define PARA_ETA_MINUTE_ADDR            (PARA_REGION_U8_BASE_ADDR + 12)         //���Ƶ���ʱ�䣬��
#define PARA_OPEN_CLOSE_TIME_INDEX_ADDR (PARA_REGION_U8_BASE_ADDR + 14)         //���ػ�ʱ������		    
/*-----------------REGION_CHAR-------------------*/
#define PARA_REGION_CHAR_BASE_ADDR      (PARA_REGION_U8_BASE_ADDR + 16)         // address = 46
#define PARA_TIME_OPEN_0_ADDR 		    (PARA_REGION_CHAR_BASE_ADDR + 0)       //����ʱ��
#define PARA_TIME_OPEN_1_ADDR 		    (PARA_REGION_CHAR_BASE_ADDR + 14)      //����ʱ��
#define PARA_TIME_OPEN_2_ADDR 		    (PARA_REGION_CHAR_BASE_ADDR + 28)      //����ʱ��
#define PARA_TIME_CLOSE_0_ADDR 		    (PARA_REGION_CHAR_BASE_ADDR + 42)      //�ػ�ʱ��
#define PARA_TIME_CLOSE_1_ADDR 		    (PARA_REGION_CHAR_BASE_ADDR + 46)      //�ػ�ʱ��
#define PARA_TIME_CLOSE_2_ADDR 		    (PARA_REGION_CHAR_BASE_ADDR + 70)      //�ػ�ʱ��
#define PARA_GPS_FIRST_TIME_ADDR        (PARA_REGION_U32_BASE_ADDR +  84)      //GPS��һ�ζ�λʱ��
/*ʣ������ֽ���112B*/

/*lq ====================EEPROM�����洢�������Ⱥ궨��======================== */
/*
 *  REGION_CHAR�����洢����ʱ���õ�MAXLEN��
 *  REGION_U32��REGION_U16��REGION_U8�����洢����ʱ��ʹ�ã������洢�ĳ��ȹ̶�����
 *  �ڲ��������ñ����л��õ�
 */
/*-----------------REGION_U32--------------------*/
#define PARA_WORKTIME_MAXLEN            5                                       //�ϴο���֮ǰ������ʱ��
#define PARA_WORKTIME_LAST_MAXLEN       5                                       //�ϴο���֮����ʱ��
#define PARA_SAILING_DISTINCE_MAXLEN	5										//�����ܾ��루��λ���ף�
/*-----------------REGION_U16--------------------*/
#define PARA_PERSONS_MAXLEN			    4			                            //��������
#define PARA_WDG_RESET_CNT_MAXLEN	    3                                       //���Ź���λ���� 
#define PARA_LAST_WDG_RESET_CNT_MAXLEN  3                                       //���ϴ����������������������������Ź���λ����
#define PARA_NORMAL_POWER_ON_CNT_MAXLEN	3 										//������������
/*-----------------REGION_U8---------------------*/
#define PARA_ETA_MONTH_MAXLEN           2                                       //���Ƶ���ʱ�䣬��
#define PARA_NAVIGATION_STATUS_MAXLEN   2                                       //����״̬
#define PARA_REGION_APP_FLAG_MAXLEN     2                                       //����Ӧ�ñ�־
#define PARA_POWER_LEVEL_MAXLEN         1                                       //����ˮƽ
#define PARA_ETA_DAY_MAXLEN             2                                       //���Ƶ���ʱ�䣬��
#define PARA_ETA_HOUR_MAXLEN            2                                       //���Ƶ���ʱ�䣬ʱ
#define PARA_ETA_MINUTE_MAXLEN          2                                       //���Ƶ���ʱ�䣬��
#define PARA_OPEN_CLOSE_TIME_INDEX_MAXLEN   2                                   //���ػ�ʱ������
/*-----------------REGION_CHAR-------------------*/
#define PARA_TIME_OPEN_MAXLEN 		    12                                      //����ʱ��
#define PARA_TIME_CLOSE_MAXLEN 		    12                                      //�ػ�ʱ��
#define PARA_GPS_FIRST_TIME_MAXLEN      12      								//GPS��һ�ζ�λʱ��

/*lq ===================EEPROM�����洢���ͺ궨��============================= */
/*-----------------REGION_U32--------------------*/
#define PARA_WORKTIME_TYPE              0                                       //�ϴο���֮ǰ������ʱ��
#define PARA_WORKTIME_LAST_TYPE         1                                       //�ϴο���֮����ʱ��
#define PARA_SAILING_DISTINCE_TYPE   	2										//�����ܾ��루��λ���ף�
/*-----------------REGION_U16--------------------*/
#define PARA_PERSONS_TYPE			    3			                            //��������
#define PARA_WDG_RESET_CNT_TYPE	        4                                       //���Ź���λ���� 
#define PARA_LAST_WDG_RESET_CNT_TYPE    5                                       //���ϴ����������������������������Ź���λ����
#define PARA_NORMAL_POWER_ON_CNT_TYPE	6								   		//������������
/*-----------------REGION_U8---------------------*/
#define PARA_ETA_MONTH_TYPE             7                                      //���Ƶ���ʱ�䣬��
#define PARA_NAVIGATION_STATUS_TYPE     8                                      //����״̬
#define PARA_REGION_APP_FLAG_TYPE       9                                      //����Ӧ�ñ�־
#define PARA_POWER_LEVEL_TYPE           10                                     //����ˮƽ
#define PARA_ETA_DAY_TYPE               11                                     //���Ƶ���ʱ�䣬��
#define PARA_ETA_HOUR_TYPE              12                                     //���Ƶ���ʱ�䣬ʱ
#define PARA_ETA_MINUTE_TYPE            13                                     //���Ƶ���ʱ�䣬��
#define PARA_OPEN_CLOSE_TIME_INDEX_TYPE 14                                     //���ػ�ʱ������
/*-----------------REGION_CHAR-------------------*/
#define PARA_TIME_OPEN_0_TYPE 		    15                                      //����ʱ��
#define PARA_TIME_OPEN_1_TYPE 		    16                                      //����ʱ��
#define PARA_TIME_OPEN_2_TYPE 		    17                                      //����ʱ��
#define PARA_TIME_CLOSE_0_TYPE 		    18                                      //�ػ�ʱ��
#define PARA_TIME_CLOSE_1_TYPE 		    19                                      //�ػ�ʱ��
#define PARA_TIME_CLOSE_2_TYPE 		    20                                      //�ػ�ʱ��
#define PARA_GPS_FIRST_TIME_TYPE        21      								//GPS��һ�ζ�λʱ��

/*lq EEPROM�����ܸ���*/
#define PARA_INDEXBUF_LEN			    22
#define PARA_TIME_OPEN_CLOSE_TOTAL 		3                                       //����/�ػ�ʱ���¼����

/*lq ==============EEPROM�����ϣ�ceil��/�£�floor���޺궨��================== */
/*-----------------REGION_U32--------------------*/
#define PARA_WORKTIME_FLOOR             0//0
#define PARA_WORKTIME_CEIL              0xFFFFFFFF								// 4294967295
#define PARA_WORKTIME_LAST_FLOOR        0
#define PARA_WORKTIME_LAST_CEIL         0xFFFFFFFF								// 4294967295
#define PARA_SAILING_DISTINCE_FLOOR	    0										//�����ܾ��루��λ���ף�
#define PARA_SAILING_DISTINCE_CELL	    0xFFFFFFFF								//�����ܾ��루��λ���ף�
/*-----------------REGION_U16--------------------*/
#define PARA_PERSONS_FLOOR			    (AIS_INFO_PERSONS_MIN)	                //��������
#define PARA_PERSONS_CEIL			    (AIS_INFO_PERSONS_MAX)	                //��������
#define PARA_WDG_RESET_CNT_FLOOR	    0                                       //���Ź���λ���� 
#define PARA_WDG_RESET_CNT_CEIL         0xFFFF                                   //���Ź���λ����
 																				//0xFFFF
#define PARA_LAST_WDG_RESET_CNT_FLOOR   0                                       //���ϴ����������������������������Ź���λ����
#define PARA_LAST_WDG_RESET_CNT_CEIL    0xFFFF                                   //���ϴ����������������������������Ź���λ����
#define PARA_NORMAL_POWER_ON_CNT_FLOOR	0									 	//������������
#define PARA_NORMAL_POWER_ON_CNT_CEIL	0xFFFF								 	//������������
/*-----------------REGION_U8---------------------*/
#define PARA_ETA_MONTH_FLOOR            (AIS_INFO_ETA_MONTH_MIN)                //���Ƶ���ʱ�䣬��
#define PARA_ETA_MONTH_CEIL             (AIS_INFO_ETA_MONTH_MAX)                //���Ƶ���ʱ�䣬��
#define PARA_NAVIGATION_STATUS_FLOOR    (AIS_INFO_NAVIGATION_STATUS_MIN)        //����״̬
#define PARA_NAVIGATION_STATUS_CEIL     (AIS_INFO_NAVIGATION_STATUS_MAX)        //����״̬
#define PARA_REGION_APP_FLAG_FLOOR      (AIS_INFO_REGION_APP_FLAG_MIN)          //����Ӧ�ñ�־
#define PARA_REGION_APP_FLAG_CEIL       (AIS_INFO_REGION_APP_FLAG_MAX)          //����Ӧ�ñ�־
#define PARA_POWER_LEVEL_FLOOR          0                                       //����ˮƽ
#define PARA_POWER_LEVEL_CEIL           9                                       //����ˮƽ
#define PARA_ETA_DAY_FLOOR              (AIS_INFO_ETA_DAY_MIN)                  //���Ƶ���ʱ�䣬��
#define PARA_ETA_DAY_CEIL               (AIS_INFO_ETA_DAY_MAX)                  //���Ƶ���ʱ�䣬��
#define PARA_ETA_HOUR_FLOOR             (AIS_INFO_ETA_HOUR_MIN)                 //���Ƶ���ʱ�䣬ʱ
#define PARA_ETA_HOUR_CEIL              (AIS_INFO_ETA_HOUR_MAX)                 //���Ƶ���ʱ�䣬ʱ
#define PARA_ETA_MINUTE_FLOOR           (AIS_INFO_ETA_MINUTE_MIN)               //���Ƶ���ʱ�䣬��
#define PARA_ETA_MINUTE_CEIL            (AIS_INFO_ETA_MINUTE_MAX)               //���Ƶ���ʱ�䣬��  
#define PARA_OPEN_CLOSE_TIME_INDEX_FLOOR    0
#define PARA_OPEN_CLOSE_TIME_INDEX_CEIL     (PARA_TIME_OPEN_TOTAL)

/*-----------------REGION_CHAR-------------------*/
//lq ����ֵ���Ͳ�������/����ֵ

/*lq =======================EEPROM����Ĭ��ֵ�궨��=============================== */
/*-----------------REGION_U32--------------------*/
#define PARA_WORKTIME_DEFAULT           (DEV_INFO_WORKTIME_DEFAULT)             //������ʱ��
#define PARA_WORKTIME_LAST_DEFAULT      0
#define PARA_SAILING_DISTINCE_DEFAULT   0										//�����ܾ��루��λ���ף�
/*-----------------REGION_U16--------------------*/
#define PARA_PERSONS_DEFAULT			(AIS_INFO_PERSONS_DEFAULT)	            //��������
#define PARA_WDG_RESET_CNT_DEFAULT	    0                                       //���Ź���λ���� 
#define PARA_LAST_WDG_RESET_CNT_DEFAULT 0                                       //���ϴ����������������������������Ź���λ����
#define PARA_NORMAL_POWER_ON_CNT_DEFAULT	0									//������������
/*-----------------REGION_U8--------------------*/
#define PARA_ETA_MONTH_DEFAULT          (AIS_INFO_ETA_MONTH_DEFAULT)            //���Ƶ���ʱ�䣬��
#define PARA_NAVIGATION_STATUS_DEFAULT  (AIS_INFO_NAVIGATION_STATUS_DEFAULT)    //����״̬
#define PARA_REGION_APP_FLAG_DEFAULT    (AIS_INFO_REGION_APP_FLAG_DEFAULT)      //����Ӧ�ñ�־
#define PARA_POWER_LEVEL_DEFAULT        0                                       //����ˮƽ,Ĭ��ֵ0-12.5we
#define PARA_ETA_DAY_DEFAULT            (AIS_INFO_ETA_DAY_DEFAULT)              //���Ƶ���ʱ�䣬��
#define PARA_ETA_HOUR_DEFAULT           (AIS_INFO_ETA_HOUR_DEFAULT)             //���Ƶ���ʱ�䣬ʱ
#define PARA_ETA_MINUTE_DEFAULT         (AIS_INFO_ETA_MINUTE_DEFAULT)           //���Ƶ���ʱ�䣬��
#define PARA_OPEN_CLOSE_TIME_INDEX_DEFAULT  0
/*-----------------REGION_CHAR-------------------*/
#define PARA_TIME_OPEN_DEFAULT 		    "000000246060"                          //����ʱ��,�����ض���ʽ
#define PARA_TIME_CLOSE_DEFAULT 	    "000000246060"	                        //�ػ�ʱ��,�����ض���ʽ
#define PARA_GPS_FIRST_TIME_DEFAULT     "000000246060"      					//GPS��һ�ζ�λʱ��,�����ض���ʽ
                    
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/

/**
	@ingroup ������ȡ
	@brief �����洢
	@param ucPara_Type ��Ĳ�������
	@param pPara ����ָ��
	@param ucLen ��������
	@retval TRUE �洢�ɹ�
	@retval FALSE �洢ʧ��

	�����洢��EEPROM
*/
U8 Para_Save(U8 ucPara_Type, U8 *pPara, U8 ucLen);

/**
	@ingroup ������ȡ
	@brief ������ȡ
	@param ucPara_Type ���Ĳ�������
	@param pPara ����ָ��
	@retval TRUE �洢�ɹ�
	@retval FALSE �洢ʧ��

	�����洢��EEPROM
*/
U8 Para_Read(U8 ucPara_Type, U8 *pPara);
void IniEEPROM_Para(void);



//lnw  add
extern U8 Para_OpenTimeSave(U8 *pPara, U16 index);
extern U8 Para_CloseTimeSave(U8 *pPara, U16 index);


#endif
