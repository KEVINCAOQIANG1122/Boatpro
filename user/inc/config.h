/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: config.h
  Author: lq       Version : V1.00        Date: 2013.10.31
  Description:     ��������ͷ�ļ����������
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _CONFIG_H
#define _CONFIG_H

/* Includes ------------------------------------------------------------------*/

/* Public  macro -------------------------------------------------------------*/
/*lq ����ʹ�����к���ļ��м����ͷ�ļ�*/
/*lq 0-�����������������صĹ���ȫ���رգ�1-�������������صĹ��ܿ�������������ĵ������赥������*/
#define CONFIG_DEBUG                1

/*lq 0-���������е�״̬�����������ȫ���رգ�1-��������״̬���������صĹ��ܿ��������������������赥������*/
#define CONFIG_STAT_MSG_PRINT       1

/*lq 0-���ܹرգ�1-���ܿ������������ù�����رճ�ͻ��*/
#define CONFIG_PARASTORAGE_AISSD    1   //��̬���ݲ�������Ӧ���ġ�$AISSD,A--A,B--B,C,D,E,F,G,H*hh��

/*lq 1-ʹ�������������GPSʱ�����ͬ����0-ʹ��GPSԭʼʱ�����ͬ��*/
#define CONFIG_GPS_SYNCH_LEAP       0   //

/*lq 1-����դѹʹ��DA���ƣ�0-����դѹ��ʹ��DA����*/ 
#define CONFIG_PA_GRID_DA_CTRL      1

/*lq 1-��¼��ؼ��ģ�⹤��ʱ����0-����¼��ؼ��ģ�⹤��ʱ��*/
#define CONFIG_PARASTORAGE_BTT      0   //lq ������������EEPROM�з����ַ

/*lq 1-����GPSʡ����ƣ�0-����GPSʡ�����*/
#define CONFIG_GPS_SAVE             0   //

/*lq 1-18����Ϣ��SOGָʾ��ص����ٷֱȣ�0-18����Ϣ��SOGָʾ����*/
#define CONFIG_SOG_FOR_BAT_VOLUME   0

/*lq 1-18����Ϣ��COGָʾ�����¶ȣ�0-18����Ϣ��COGָʾ����*/
#define CONFIG_COG_FOR_TEMPERATURE  0

/*lq 1-SART MMSI��ʽУ��ʹ�ܣ�0-SART MMSI��ʽУ��ʧ��*/
#define CONFIG_MMSI_SARTFORM_CHECK  0     

/*lq 1-���͵����Զ��ػ���0-���͵������Զ��ػ�*/ 
#define CONFIG_BATTERY_CTRL_AUTOPOWERDOWN   0 

/*lq 0-�ٷֱȷ���1-ƽ��λ�����귨��2-ƽ�����뷨 */
#define CONFIG_OFF_POSITON_ALGORITHM        0  

/*lq 0-ʹ��RFM��һ������Խ�������1-ʹ��IFM��һ����޷������� */
#define CONFIG_MSG_6_DAC_IFM        0  

/*lq 0-������Сʱ����ͬ����1-����Сʱ����ͬ�� */
#define CONFIG_GPS_SYNCH_HOUR_MIN   0  

/*lq 0-ADC1���ܹرգ�1-ADC1���ܿ��� */
#define CONFIG_ADC1                 0

/*lq 0-�����е�ص�����⼰���ƣ�1-���е�ص�����⼰���� */
#define CONFIG_BAT_MONITOR          0

/*lq 0-�������¶ȼ�⣬1-�����¶ȼ�� */
#define CONFIG_TEMPERATURE_MONITOR  0

/*lq 0-�������ܹرգ�1-�������ܿ��� */
#define CONFIG_KEY                  0

/*lq 0-�ػ����ܹرգ�1-�ػ����ܿ��� */
#define CONFIG_POWERDOWN            0

/*lq 0-����������ܹرգ�1-����������ܿ��� */
#define CONFIG_DEBUG_PRINT_EN       1

/*lq 0-GPSģ���uBloxЭ��ͨ�Ź��ܹرգ�1-GPSģ���uBloxЭ��ͨ�Ź��ܿ��� */
#define CONFIG_GPS_UBX_EN           1

/*lq 0-AtoN��GPSλ��ƫ���жϹ��ܹرգ�1-AtoN��GPSλ��ƫ���жϹ��ܿ��� */
#define CONFIG_GPS_OFF_POSITION_EN  0

/*lq 0-��Դ�����ѹ�Լ칦�ܹرգ�1-��Դ�����ѹ�Լ칦�ܿ��� */
#define CONFIG_SLFCHK_POWER_SUPPLY_VOLT_EN      0

/* 0-SD��ģ�鹦�ܹرգ�1-SD��ģ�鹦�ܿ��� */
#define CONFIG_SD_CARD_MODULE_EN      0


/*  lnw    1.  2�ο��� ���������Խ�����ʾ���� */
#define CONFIG_PTT_EN          1

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function  ---------------------------------------------------------*/

/* Public  variables ---------------------------------------------------------*/

/* Public  function  ---------------------------------------------------------*/

#endif
