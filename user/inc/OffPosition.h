/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: OffPosition.h
  Author: lq       Version : V1.00        Date: 2014.01.02
  Description:     λ��ƫ���ж�
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _OFFPOSITION_H
#define _OFFPOSITION_H

/* Private define ------------------------------------------------------------*/
/*lq λ��ƫ���ж���غ궨�� */
#define GPS_ON_POSITION_SAMPLE_NUM            5     //lq ������״̬�£���δƫ��״̬�£��ж�ƫ��״̬�Ĳ�������
#define GPS_OFF_POSITION_SAMPLE_NUM           100   //lq ��ƫ��״̬�£��ж�ƫ��״̬�Ĳ�������
#define GPS_OFF_POSITION_PERC_THRES           80    //lq �Ƿ�ƫ��İٷֱ���ֵ

/* Private typedef -----------------------------------------------------------*/

/*lq GPSλ��ƫ���жϽṹ��*/
typedef struct {
    
    /*lq ����GPSƫ���ж� */
    float PositionNormSample[GPS_ON_POSITION_SAMPLE_NUM + 1][2];    //lq ��������ʱ�Ĳ����㻺��
    float PositionOffSample[GPS_OFF_POSITION_SAMPLE_NUM + 1][2];    //lq ��γ�Ȳ�����Ļ��棬���ڼ��㾭γ��
    U8  PositionFlag[GPS_OFF_POSITION_SAMPLE_NUM + 1];              //lq �����������Ӧ��ƫ��״̬��1-δƫ�룬0-ƫ��
    U16 PositionOffIndexIn;
    U16 PositionOffIndexOut;
    U16 PositionFlagIndexIn;
    U16 PositionFlagIndexOut;
    U16 InsideSum;
    float AvarDist;
    float AvarLon;
    float AvarLat;

    //U8  OnPositionFlag;                                             //lq λ��δƫ���־
    U8  OffPositionFlag;                                            //lq λ��ƫ���־
    U8  OffPositionPossFlag;                                        //lq λ�ÿ���ƫ���־
    U8  OffPositionValid;                                           //lq λ��ƫ���־�Ƿ���Ч
    U8  OffPositionPossValid;                                       //lq λ�ÿ���ƫ���־�Ƿ���Ч
                
}GPSOffPositionStruct;

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
extern GPSOffPositionStruct GPSOffPositionPara;

/* Public  function  ---------------------------------------------------------*/
/**
	@ingroup ���
	@brief ��ص������
*/

#endif
