/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: TDMA.h
  Author: lq       Version : V1.0.0        Date: 2015.01.16
  Description:     TDMA ����ģ��
  Version:         V1.0.0
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _TDMA_H_
#define _TDMA_H_

/* Include -------------------------------------------------------------------*/

/* Public define -------------------------------------------------------------*/

/*lq SOTDMA������ֵ���� */
#define SOTDMA_NSS_MIN                  0
#define SOTDMA_NSS_MAX                  2249
#define SOTDMA_NS_MIN                   0
#define SOTDMA_NS_MAX                   2249
#define SOTDMA_RR_MIN                   2                                   //lq �����ŵ����ۺϱ�������
#define SOTDMA_RR_MAX                   30
#define SOTDMA_NI_MIN                   (FRAME_SLOT_SIZE / SOTDMA_RR_MAX)   //lq �����ŵ����ۺϱ�����
#define SOTDMA_NI_MAX                   (FRAME_SLOT_SIZE / SOTDMA_RR_MIN)
#define SOTDMA_NTS_MIN                  0
#define SOTDMA_NTS_MAX                  2249
#define SOTDMA_TMO_MIN                  3
#define SOTDMA_TMO_MAX                  7
#define SOTDMA_SC_RR_MIN                (SOTDMA_RR_MIN >> 1)                //lq �����ŵ��ı�������
#define SOTDMA_SC_RR_MAX                (SOTDMA_RR_MAX >> 1)                
#define SOTDMA_SC_NI_MIN                (FRAME_SLOT_SIZE / SOTDMA_SC_RR_MAX)//lq �����ŵ��ı�����
#define SOTDMA_SC_NI_MAX                (FRAME_SLOT_SIZE / SOTDMA_SC_RR_MIN)
#define SOTDMA_SI_OFFSET_SCALE          (0.1)

#define SOTDMA_SLOT_TMO_INVALID_VALUE   (-1)

/*lq һ֡�ڵ����ŵ��ı�Ʒ���ʱ϶���泤�� */
#define SOTDMA_NTS_BUFF_LEN             SOTDMA_SC_RR_MAX 

/*lq SOTDMA ����״̬ */
#define SOTDMA_STA_INIT_PHASE               0
#define SOTDMA_STA_NETWORK_ENTRY_PHASE      1
#define SOTDMA_STA_FIRST_FRAME_PHASE        2
#define SOTDMA_STA_CONTINUOUS_PHASE         3
#define SOTDMA_STA_CHANGE_REPORT_INTERVAL_1 4
#define SOTDMA_STA_CHANGE_REPORT_INTERVAL_2 5
#define SOTDMA_STA_CHANGE_REPORT_INTERVAL_3 6
#define SOTDMA_STA_IDLE                     7
#define SOTDMA_STA_START                    8

/*lq SOTDMA���ƶ�ʱ���� */
#define SOTDMA_CTRL_TIM_PERIOD              20  //lq unit: 5ms
#define SOTDMA_CTRL_TIMEOUT_INIT_PHASE      600 //lq unit: 100ms, i.e. 1min

/*lq ITDMA���ƶ�ʱ���� */
#define ITDMA_CTRL_TIM_PERIOD               20  //lq unit: 5ms

/*lq ITDMA����״̬ */
#define ITDMA_STA_IDLE                      0
#define ITDMA_STA_CONTINUOUS_PHASE          1

/*lq ITDMA���ͼ�� */
#define ITDMA_REPORT_INTERVAL_SLOT_MAX      16383   //lq unit: 1slot��i.e. 8192*2-1
#define ITDMA_REPORT_INTERVAL_SLOT_6M       13500   //lq unit: 1slot
#define ITDMA_SI_OFFSET_SCALE               (0.01)  //lq �Զ���ֵ

/*lq ITDMAͨ��״̬ */
#define ITDMA_COMMSTATE_SLOT_INCREMENT_MAX  8191    //lq unit: 1slot��slot increment�ֶ����ֵ
#define ITDMA_COMMSTATE_NUM_OF_SLOTS_MAX    7       //lq Number of slots�ֶ����ֵ
#define ITDMA_COMMSTATE_NUM_OF_SLOTS_SPLIT  5       //lq Number of slots�ֶ�ֵ����ָ��

#define ITDMA_CONSECUTIVE_SLOTS_QUANTITY_1_MAX      5       //lq unit: 1slot��ʱ϶ƫ����������slot incrementʱ������������ʱ϶�������
#define ITDMA_CONSECUTIVE_SLOTS_QUANTITY_2_MAX      3       //lq unit: 1slot��ʱ϶ƫ��������slot incrementʱ������������ʱ϶�������
#define ITDMA_SLOT_OFFSET_MAX               ((ITDMA_COMMSTATE_SLOT_INCREMENT_MAX) * 2 + 1)
#define ITDMA_SLOT_QUANTITY_MAX             5

/*lq TDMA����״̬ */
#define TDMA_STA_IDLE                       0
#define TDMA_STA_CONTINUOUS_PHASE           1

/*lq TDMAͬ��״̬ */
#define TDMA_SYNC_STATE_UTC_DIRECT          0
#define TDMA_SYNC_STATE_UTC_INDIRECT        1
#define TDMA_SYNC_STATE_UTC_BASE_STATION    2
#define TDMA_SYNC_STATE_UTC_STATION         3

/*lq TDMA��������صĺ��� */
#define TDMA_REPORT_INTERVAL_SOG_MAX_B_3M   20      //lq unit: 0.1knot��ClassB 3min�������������
#define TDMA_REPORT_INTERVAL_SOG_MAX_B_30S  140
#define TDMA_REPORT_INTERVAL_SOG_MAX_B_15S  230

/*lq TDMA������ */
#define TDMA_REPORT_INTERVAL_SLOT_3M        ((FRAME_SLOT_SIZE) * 3)     //lq unit: 1slot
#define TDMA_REPORT_INTERVAL_SLOT_30S       ((FRAME_SLOT_SIZE) / 2)
#define TDMA_REPORT_INTERVAL_SLOT_15S       ((FRAME_SLOT_SIZE) / 4)
#define TDMA_REPORT_INTERVAL_SLOT_5S        ((FRAME_SLOT_SIZE) / 12)

/*lq TDMA���ƶ�ʱ���� */
#define TDMA_CTRL_TIM_PERIOD                        20  //lq unit: 5ms
#define TDMA_CTRL_TIMEOUT_REPORT_INTERVAL_SAMPLE    10  //lq unit: 100ms, i.e. 1s

/*lq TDMA����ı������Чֵ */
#define TDMA_REPORT_INTERVAL_CHANGED_CNT_THRES      120 //lq unit: 1��ÿ��1���ӽ���һ���ж�

/* Public typedef ------------------------------------------------------------*/
typedef enum
{
    ENUM_TDMA_ACCESS_SCHEMA_ITDMA = 0,
    ENUM_TDMA_ACCESS_SCHEMA_SOTDMA,
    ENUM_TDMA_ACCESS_SCHEMA_RATDMA,
    ENUM_TDMA_ACCESS_SCHEMA_FATDMA,
    ENUM_TDMA_ACCESS_SCHEMA_CSTDMA
}TdmaAccessSchemaEnum;

typedef enum
{
    ENUM_TDMA_SCHED_SCHEMA_ITDMA = 0,
    ENUM_TDMA_SCHED_SCHEMA_SOTDMA,
}TdmaSchedSchemaEnum;

/*lq -------SOTDMA������Ͷ���-------- */
/*lq
 * ���ŵ�SOTDMA�����ṹ�嶨��
 */
typedef struct
{
    S16 NominalStartSlot;
    S16 NominalIncrement;                       //lq unit: 1slot
    S16 ReportRate;                             //lq unit: /min, �����ŵ��ı�����
    S16 SelectIntervalOffset;                   //lq unit: 1slot
    S16 SelectIntervalLow;                      //lq unit: 1slot
    S16 SelectIntervalHigh;                     //lq unit: 1slot
    S16 NominalSlots[SOTDMA_NTS_BUFF_LEN];
    S16 NominalSlotIn;
    S16 NominalSlotOut;
    S16 NominalSlot;                            //lq ��ǰֵ
    S16 NominalTxSlots[SOTDMA_NTS_BUFF_LEN];    //lq һ֡�е����б�Ʒ���ʱ϶
    U16 NominalTxSlotTotal;
    S16 NominalTxSlotIn;
    S16 NominalTxSlotOut;
    S16 NominalTxSlot;                          //lq ��ǰֵ
    S8  SlotTimeouts[SOTDMA_NTS_BUFF_LEN];      //lq unit: 1frame(i.e. 1min)
    S16 SlotTimeoutIn;
    S16 SlotTimeoutOut;
    S8  SlotTimeout;                            //lq ��ǰֵ

    S16 FrameReportCnt;                         //lq һ֡�б����������
    S16 SlotOffset;                             //lq unit: 1slot

    S32 LastTxFrame;                            //lq ��һ�η����֡�ţ�����pSotdmaParaStruct->ReportRate == 1ʱ���ã�
    S16 LastTxSlot;                             //lq ��һ�η����ʱ϶�ţ�����pSotdmaParaStruct->ReportRate == 1ʱ���ã�
}SotdmaParaStruct;

/*lq
 * ���ŵ�SOTDMA���ƽṹ�嶨��
 */
typedef struct
{
	U32 TimeCnt;
	U16 TimePeriod;    
    U16 Timeout;
    U16 TimeoutCnt;

    U8  CtrlState;
    U8  SotdmaEn;
}SotdmaCtrlStruct;

/*lq
 * ���ŵ�SOTDMA���Ƚṹ�嶨��
 */
typedef struct
{
    SotdmaParaStruct SotdmaPara;
    SotdmaParaStruct SotdmaParaNew;     //lq �ı䱨����ʱʹ��
    SotdmaCtrlStruct SotdmaCtrl;
}SotdmaSchedStruct;

/*lq
 * ˫�ŵ�SOTDMA���Ƚṹ�嶨��
 */
typedef struct
{
    SotdmaSchedStruct SotdmaSchedA;     //lq SotdmaPara of channel A
    SotdmaSchedStruct SotdmaSchedB;     //lq SotdmaPara of channel B
    SotdmaCtrlStruct SotdmaCtrl;
}SotdmaSchedDualStruct;

/*lq -------ITDMA������Ͷ���-------- */
/*lq
 * ���ŵ�ITDMA�����ṹ�嶨��
 */
typedef struct
{
    S32 NominalStartFrame;
    S16 NominalStartSlot;
    S32 NominalFrame;
    S16 NominalSlot;
    S32 NominalFrameNext;
    S16 NominalSlotNext;
    S32 NominalTxFrame;
    S16 NominalTxSlot;
    S32 NominalTxFrameNext;
    S16 NominalTxSlotNext;

    S16 NominalIncrement;               //lq unit: 1slot�������ŵ��ı�����
    S16 SelectIntervalOffset;           //lq unit: 1slot
    S16 SlotOffset;                     //lq maximum 8192*2-1��ʵ�ʷ����ʱ϶��������ͨ��״̬�е�ֵ��ͬ
    U8  SlotQuantity;                   //lq maximum 5, consecutive slots����ͨ��״̬�е�ֵ��ͬ
    U8  SlotKeep;                       //lq ��ͨ��״̬�е�ֵ��ͬ
}ItdmaParaStruct;

/*lq
 * ���ŵ�ITDMA���ƽṹ�嶨��
 */
typedef struct
{
	U32 TimeCnt;
	U16 TimePeriod;    
    U16 Timeout;
    U16 TimeoutCnt;

    U8  CtrlState;
    U8  ItdmaEn;
}ItdmaCtrlStruct;

/*lq
 * ���ŵ�ITDMA���Ƚṹ�嶨��
 */
typedef struct
{
    ItdmaParaStruct ItdmaPara;
    ItdmaParaStruct ItdmaParaNew;       //lq �ı䱨����ʱʹ��
    ItdmaCtrlStruct ItdmaCtrl;
}ItdmaSchedStruct;

/*lq
 * ˫�ŵ�ITDMA���Ƚṹ�嶨��
 */
typedef struct
{
    ItdmaSchedStruct ItdmaSchedA;     //lq ItdmaPara of channel A
    ItdmaSchedStruct ItdmaSchedB;     //lq ItdmaPara of channel B
    ItdmaCtrlStruct ItdmaCtrl;
}ItdmaSchedDualStruct;

/*lq -------TDMA������Ͷ���-------- */
/*lq
 * TDMA���ƽṹ�嶨��
 */
typedef struct
{
	U32 TimeCnt;
	U16 TimePeriod;    
    U16 Timeout;
    U16 TimeoutCnt;

    U8  CtrlState;
    U8  TdmaEn;

    TdmaSchedSchemaEnum SchedSchema; 
    S16 ReportIntervalSlot;             //lq unit: 1slot����ǰTDMAʹ�õı�������˫�ŵ���
    S16 ReportIntervalSlotSample;       //lq unit: 1slot������GPS����ʵʱ�õ��ı�����
    S16 ReportIntervalChangedCnt;       //lq unit: 1
}TdmaCtrlStruct;

/*lq
 * TDMA���Ƚṹ�嶨��
 */
typedef struct
{
    SotdmaSchedDualStruct SotdmaSched;
    ItdmaSchedDualStruct ItdmaSched;

    TdmaCtrlStruct TdmaCtrl;
}TdmaSchedStruct;

/* Public variables ----------------------------------------------------------*/
extern S16 ITDMA_CalcCommStateSlotIncrement(S16 slotOffset);
extern S16 ITDMA_CalcSlotOffset(U16 slotIncrement, U8 numberOfSlots);
extern U8 ITDMA_CalcCommStateNumberOfSlots(U8 numOfSlots, S16 slotOffset);
extern U8 ITDMA_CalcSlotQuantity(U8 numberOfSlots);
extern U8 ITDMA_CalcCommStateKeepFlag(U8 slotKeep);
extern U8 ITDMA_CalcSlotKeep(U8 keepFlag);
extern S16 ITDMA_CalcSelectIntervalOffset(S16 nominalIncrement);
extern S16 SOTDMA_CalcSelectIntervalOffset(S16 nominalIncrement);
extern U8 SOTDMA_SetNssDual(AisChannelEnum channel, S16 nss);

/* Public functions ----------------------------------------------------------*/

#endif
