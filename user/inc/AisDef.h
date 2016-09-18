/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: AisDef.h
  Author: lq       Version : V1.0.0        Date: 2015.04.29
  Description:     AISͨ�ö���
  Version:         V1.0.0
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _AISDEF_H_
#define _AISDEF_H_

/* Include -------------------------------------------------------------------*/
#include "def.h"

/* Public define -------------------------------------------------------------*/
/*lq ��̬������Ч���� */
#define STATIC_LIST_INDEX_NULL          -1

/*lq AIS �ŵ���ض��� */
#define AIS_CHANNEL_FREQ_A              161975000u  //lq ��λ��1Hz
#define AIS_CHANNEL_FREQ_B              162025000u
#define AIS_CHANNEL_NAME_A              "A"
#define AIS_CHANNEL_NAME_B              "B"
#define AIS_CHANNEL_CHAR_A              'A'
#define AIS_CHANNEL_CHAR_B              'B'
#define AIS_CHANNEL_AIS_TOTAL           2

/*lq �ŵ��궨�壨���º궨�����ڼ��ݣ�������ʹ�ã� */
#define CHANNEL_AIS_TOTAL               2   //lq AISר���ŵ�����Channel A and Channel B��
#define CHANNEL_NAME_A                  (AIS_CHANNEL_NAME_A)
#define CHANNEL_NAME_B                  (AIS_CHANNEL_NAME_B)
#define CHANNEL_FREQ_A                  (AIS_CHANNEL_FREQ_A)
#define CHANNEL_FREQ_B                  (AIS_CHANNEL_FREQ_B)

/*lq AISƵ�� */
#define AIS_FREQ_A                      (AIS_CHANNEL_FREQ_A)
#define AIS_FREQ_B                      (AIS_CHANNEL_FREQ_B)

/*lq AIS��� */
#define AIS_CLASS_A                     0
#define AIS_CLASS_B                     1
#define AIS_CLASS_DEFAULT               (AIS_CLASS_B)

/*lq AIS bitʱ������ */
#define AIS_BIT_PHASE_SIZE              5       //lq unit: 1phase
#define AIS_BIT_TIME_DATA_START         6       //lq unit: 1bit
#define AIS_BIT_TIME_DATA_END           8       //lq unit: 1bit

/*lq AISʱ϶������� */ 
#define AIS_SLOT_INTERVAL_3M                            ((FRAME_SLOT_SIZE) * 3)     //lq unit: 1slot��3min
#define AIS_SLOT_INTERVAL_6M                            ((FRAME_SLOT_SIZE) * 6)     //lq unit: 1slot��6min
#define AIS_SLOT_INTERVAL_MSG24A_TO_MSG19               375                         //lq unit: 1slot��msg24A��msg19֮��ķ��ͼ��
#define AIS_SLOT_INTERVAL_MSG24B_TO_MSG24A              375                         //lq unit: 1slot��msg24B��msg24A֮��ķ��ͼ��
#define AIS_SLOT_INTERVAL_SEND_TO_TX                    2                           //lq unit: 1slot����Ϣ��ʼ���ͣ��齨��Ϣ�����뷢�Ͷ��У�����ʼ�����ʱ����
#define AIS_SLOT_INTERVAL_FIXED_PERIOD_TX_PERIOD        (AIS_SLOT_INTERVAL_6M)      //lq unit: 1slot���̶����ڷ��������
#define AIS_SLOT_INTERVAL_SLOT_SELECT_OFFSET_MAX        ((S32)((AIS_SLOT_INTERVAL_FIXED_PERIOD_TX_PERIOD) * 0.01))     //lq unit: 1slot���̶����ڷ����£�����ʱ϶����ʱ϶��ƫ�������ֵ���˴�(S32)����ת����Ϊ���Random()����bug��

#define AIS_PHASE_INTERVAL_DATA_START                   ((AIS_BIT_PHASE_SIZE) * (AIS_BIT_TIME_DATA_START))

/*lq AISȷ�� */
#define AIS_ACK_ONCE_QUANTITY_MAX       4       //lq AISһ��ȷ���а�����Ŀ�ĵ����������ֵ

/* Public typedef ------------------------------------------------------------*/
/*lq �ŵ�ö�����Ͷ��� */
typedef enum
{
    ENUM_AIS_CHANNEL_A = 0,             //lq 161.975MHz
    ENUM_AIS_CHANNEL_B,                 //lq 162.025MHz
    ENUM_AIS_CHANNEL_BOTH,              //lq A and B    
    ENUM_AIS_CHANNEL_EITHER             //lq A or B    
}AisChannelEnum;

/* Public variables ----------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

#endif
/*==============================================================================
------------------------------------End of File---------------------------------
==============================================================================*/
