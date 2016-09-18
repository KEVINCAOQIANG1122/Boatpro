/***********************************************
*
*		���	��	WT
*		�ļ���	��	AISconfig.h
*		����	��
*			���ļ����������������ò���
*
************************************************/
#ifndef AIS_CONFIG
#define AIS_CONFIG

/***********************************************
*
*		������������
*
***********************************************	 
#define UCHAR	unsigned char
#define SCHAR	char
#define USINT   unsigned short int
#define SINT    short int
#define ULINT   unsigned long int
#define SLINT   long int
 */

/************************************************
*
*		��������
*
************************************************/
//	�ŵ�����
#define  CHANNEL_A 'A'
#define  CHANNEL_B 'B'

//ʱ϶
#define FRAMETOSLOT 2250//ʱ϶�ĸ���

//	��Ϣʵ�����ݴ�С���� 8-bit
#define MSG18DATASIZE 21
#define MSG19DATASIZE 39
#define MSG14DATASIZE 21

//	��Ϣʵ�������ִ�С���� 16bit
#define MSG18MEMORYSIZE (((MSG18DATASIZE + 1) >> 1) + 1)
#define MSG19MEMORYSIZE (((MSG19DATASIZE + 1) >> 1) + 1)

//	�о�����
#define THRESHOLDH 40
#define THRESHOLDL 20

//GPS��ʼ���Ķ�λʱ�ޡ�GPS����ʹ��ʱ�Ķ�λʱ��
//#define INITGPSFIXTIME  0x3000
//#define COMGPSFIXTIME   0x1000

//��ʱ���Ѽ����7��
#define WAKEUPINTERVAL  0x7000

//#define TIMEOUT_INIT_GPS_4503  (FRAMETOSLOT * 3)
#define TIMEOUT_INIT_GPS_4503  (FRAMETOSLOT * 2)//��һ��GPS�ȴ�ʱ�䣬��Ϊ2����
#define TIMESTAMP_MASK	0xFF
#define TIMEOUT_SLEEP_4503	(FRAMETOSLOT * 5)//˯�����
#define TIMEOUT_NORMAL_GPS_4503	(FRAMETOSLOT * 1)//����1����
#define TIMEOUT_SOS_4503	(FRAMETOSLOT * 1)//sosһ����

#define TIMEOUT_ROUTINE_4501	(FRAMETOSLOT * 6)
#define TIMEOUT_HIGHSPEED_4501	(FRAMETOSLOT / 2)
#define TIMEOUT_LOWSPEED_4501	(FRAMETOSLOT * 3)
#define TIMEOUT_SOS_4501	(FRAMETOSLOT * 1)//sosһ����

#define SPEEDFLAG 20		//�ߵͺ��ٵ�����ֵ


#endif

