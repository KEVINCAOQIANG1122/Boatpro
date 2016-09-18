/***********************************************
*
*		���	��	WT
*		�ļ���	��	AIS_MsgModu.h
*		����	��
*			���ļ�������Ϣ�������
*
************************************************/
#ifndef AIS_MSGMODU
#define AIS_MSGMODU

#include "Include.h"

/*lq ��ص�����͵�ѹ����ߵ�ѹ*/
#define BATTERY_VOLUME_MOST_VOLT    800
#define BATTERY_VOLUME_LEAST_VOLT   680

/*lq 6����Ϣ������󳤶�*/
#define MSG6_IFM0_TEXT_LEN  102                                                 //lq ���ܹ�����class-B Ҫ��Msg6��binary data����36bytes(6bits?)��IFM0��ȥָ��λ������31bytes

#define RAIM_ERR_THRES              10  //lq ��λm
typedef struct bits_map_struct
{
	U8 index;                   //lq array���ֽ�����
	U8 offset;                  //lq index��ָ�ֽ��е�λƫ�ƣ�offset==0��ʾָ��MSB
	U8* array;
	U8 array_size;
	
}BITMAP;

extern U8 bitmap_set_value_for_U8(BITMAP* map, U8 data, U8 data_bit_len);
extern U8 bitmap_set_value(BITMAP* map, U32 data, U8 data_bit_len);
extern void InitMsg1(AISMsgCommStateUnion commState);
extern void InitMsg14(void);
extern void InitMsg18(AISMsgCommStateEnum commStateType, AISMsgCommStateUnion commState);
extern void InitMsg19(void);
extern U16 InitMsgPara(void);
extern void InitGpsCell(void);
extern U16 GetTemperForCOG(float Temper);
extern float GetBatteryVolume(void);
extern U16 GetSOG(void);
static double rad(double d);
#if 0
double GetDistance(double lat1, double lng1, double lat2, double lng2);
#else
S32 GetDistance(double lat1, double lng1, double lat2, double lng2);
#endif
/**
	@ingroup Э��ջ
	@brief Э��ջ�͵ײ�ӿ�,����GPS������Э��ջ����������Ϣ
	@param ��
	@retval 0 ����

	����GPS������Э��ջ��������Ϣ18��19���ж��Ƿ�λ�������Ƿ�����Ϣ
*/
extern U16 Update_GpsData_To_Msg(void);

extern U8 GPS_valid;
//extern U16 set_4501_interval(U16 speed);
extern void ResetMsgPara(void);

#endif
