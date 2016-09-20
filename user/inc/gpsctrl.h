/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	gpsmsg.h
  Author: vozhj      Version: V1.00       Date: 2012.03.21 
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
#ifndef _GPSCTRL_H
#define _GPSCTRL_H
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/*lq ���Ժ궨��*/
#define GPS_CTRL_DEBUG_MSG

#define GPS_OUTRANGE		(0xFFFF)
#define GPS_RANGE			(10)
#define GPS_POSITION_BUFF_LENGTH (4)//λ��ƽ������������
#define GPS_SYN_COUNT_THRESHOLD (2)//(4)//ͬ�����ٱ���ʱ��4��

/*lq GPS�������ݺ궨��*/
#define GPS_DATA_PDOP_CEIL       (999)      //lq ʵ��ֵ��10��
#define GPS_DATA_PDOP_FLOOR      (5)        //lq ʵ��ֵ��10��
#define GPS_DATA_PDOP_DEFAULT    (999)      //lq ʵ��ֵ��10��


/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/

/**
	@ingroup GPS
	@brief GPS��Ϣ�ṹ��
*/
typedef struct {
	U8	GPSTimeBuf[6];
	U16 GPS_TimeY;
	U16 GPS_TimeMon;
	U16 GPS_TimeD;
	U16 GPS_TimeH;
	U16 GPS_TimeM;
	U16 GPS_TimeS;
	S16 GPS_LatiD;
	S32 GPS_Latitude;
	S32 GPS_Lati100;
	S32 GPS_Longitud;
	S32 GPS_Long100;
	U32 GPS_Sog;                                                                //lq ��¼ֵΪʵ��ֵ��10����������ת��Ϊ���ͣ�
	U32 GPS_Sog_Buf[18];
	U16 GPS_Sog_Index;
	U32 GPS_Cog;
	U16 GPS_Stat;
	U16 GPS_errlat;
	U16 GPS_errlon;
	U16 GPS_PosPrec;
    U16 GPS_QualityIndicator;
    U16 GPS_FixMode;
    U16 GPS_PDOP;
    U8  GPS_RAIMFlag;
	S32 GPS_Longitud_Buf[GPS_POSITION_BUFF_LENGTH];
	S32 GPS_Latitude_Buf[GPS_POSITION_BUFF_LENGTH];
	U16 GPS_Longitud_Buf_Index;
	U16 GPS_Latitude_Buf_Index;
    U8  GPS_LonLatFilterFlg;                                                    //lq GPS��γ���Ƿ����
    U8  GPS_LatLonCnt;                                                          //lq ��ʼ�ľ�γ�Ȳ�����

	/*lq ���ڸ��ݾ�γ�ȼ��㺽��*/
	S32 GPS_Latitude_Last;
	S32 GPS_Longitud_Last;
	U16 GPS_TimeD_Last;
	U16 GPS_TimeH_Last;
	U16 GPS_TimeM_Last;
    U16 GPS_TimeS_Last;	
	U32 GPS_Sog_Last; 
    U8  GPS_FirstLLFlg;                                                         //lq ������һ�λ�ȡ��γ��

    /*lq �������������Ӱ��*/
	U8  GPS_LeapSecondFlg;
    U16 GPS_TimeHLeapLast;
	U16 GPS_TimeMLeapLast;
	U16 GPS_TimeSLeapLast;
    U16 GPS_TimeSLeap;                                                          //lq �������֮ǰ��Ϊ��������У�������

    /*lq GPS PPS���ɱ�־ */
    U8  GPS_PPSGenFlag;
    U8  GPS_MsgGenFlag;
}GPSStruct;

#define GPS_CTRL_PROTECT_PERIOD (1)//GPS��һ�ζ�λʱ������Ϊ1����

#define GPS_CTRL_1MINPEROID		12000    //��λ5ms
#define GPS_CTRL_3MINCNT		3
#define	GPS_CTRL_2MINCNT		2
#define GPS_CTRL_1MINCNT		1
#define GPS_CTRL_5MINCNT		5//��һ�ζ�λ��5���ӱ���
#define GPS_CTRL_15MINCNT		15
#define GPS_CTRL_30MINCNT       30
#define GPS_CTRL_40SECPEROID	8000
#define GPS_CTRL_10SECPEROID	2000

#define GPS_CTRL_SHUT_UP_KEEP_MIN   15
#define GPS_CTRL_FIRST_HOUR_KEEP_MIN    30
#define GPS_CTRL_UNFIX_PERIOD       60
#define GPS_CTRL_NORMAL_KEEP_MIN    5

#define GPS_CTRL_GPSPOWERUP		0
#define GPS_CTRL_GPSPOWERDOWN	1

/*lq �û�����ģʽ�£�GPS״̬��״̬*/
#define GPS_CTRL_STA_MSG_TEST_START             0
#define GPS_CTRL_STA_MSG_TEST_WAIT_FIX          1
#define GPS_CTRL_STA_MSG_TEST_POWER_DOWN        2
#define GPS_CTRL_STA_MSG_TEST_IDLE              3

#if 0
/*lq SART��GPS״̬��״̬*/
#define GPS_CTRL_STA_START                          0
#define GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN           1
#define GPS_CTRL_STA_UNFIX_FIRST_HOUR_LNG_OPEN      2
#define GPS_CTRL_STA_UNFIX_BEY_FIRST_HOUR_LNG_OPEN  3
#define GPS_CTRL_STA_FIX_LNG_OPEN                   4
#define GPS_CTRL_STA_NORMAL_WORKING                 5
#define GPS_CTRL_STA_POWER_DOWN                     6
#define GPS_CTRL_STA_IDLE                           7
#endif

/*lq AtoN��GPS״̬��״̬ */
#define ATON_GPS_CTRL_STA_START                     0
#define ATON_GPS_CTRL_STA_FIX_LNG_OPEN              1
#define ATON_GPS_CTRL_STA_NORMAL_WORKING            2
#define ATON_GPS_CTRL_STA_OFF_POSITION_JUDGE        3
#define ATON_GPS_CTRL_STA_OFF_POSITION              4
#define ATON_GPS_CTRL_STA_IDLE                      5

/*lq M4508D��GPS״̬��״̬*/
#define GPS_CTRL_STA_START                          0
#define GPS_CTRL_STA_UNFIXED                        1
#define GPS_CTRL_STA_FIXED                          2
#define GPS_CTRL_STA_UNFIXED_ERR                    3

/*lq GPSʱ������궨��*/
#define GPS_CTRL_TIME_UNFIX_1_HOUR                  5   //min
#define GPS_CTRL_TIME_UNFIX_FIRST_LONG_OPEN         5   //min
#define GPS_CTRL_TIME_UNFIX_1HOUR_LONG_OPEN         1   //min
#define GPS_CTRL_TIME_UNFIX_BEY_1HOUR_LONG_OPEN     2   //min
#define GPS_CTRL_TIME_FIX_LONG_OPEN                 5   //min
#define GPS_CTRL_TIME_PERIOD_1S		                200     //lq unit: 5ms
#define GPS_CTRL_TIME_TIMEOUT_UNFIX_ASSERT		    5       //lq unit: 1s
#define GPS_CTRL_TIME_TIMEOUT_UNFIX_ERR		        1800    //lq unit: 1s

#define GPS_CTRL_PERIOD_UNFIX_1HOUR_LONG_OPEN       1   //��Ϣ��������
#define GPS_CTRL_PERIOD_UNFIX_BEY_1HOUR_LONG_OPEN   2   //��Ϣ��������

/*lq GPS��ֵ�궨��*/
#define GPS_CTRL_CNT_THRES_5MIN_LON_OPEN            10  //��
#define GPS_CTRL_TIME_THRES_MSG_TEST                15  //min

/*lq GPSʡ����������궨��*/
#define GPS_SAVE_JUDGE_PERIOD           20      //unit 5ms
#define GPS_SAVE_COND_OK_TIME           100     //unit 100ms    //�����������ʱ��
#define GPS_SAVE_COND_WORKTIME          4320    //unit min     ������ʱ��
#define GPS_SAVE_COND_BATTERYVOLT       450     //unit 0.01V   ��ص�ѹ
#define GPS_SAVE_COND_TEMPER            0       //unit c       �¶�

/*lq GPS�Բ�궨�� */
#define GPS_SLFCHK_TIME_PERIOD          200     //lq unit:5ms
//#define GPS_SLFCHK_TIME_TIMEOUT         10      //lq unit:1s
#define GPS_SLFCHK_TIME_TIMEOUT         4       //lq unit:1s
#define GPS_SLFCHK_TIME_TIMEOUT_TIMEPULSE 120	// unit:1s
#define GPS_SLFCHK_TIME_TIMEOUT_POWERON 2       //lq unit:1s

#define GPS_SLFCHK_CTRL_STATE_IDLE      0
#define GPS_SLFCHK_CTRL_STATE_POWERON   1
#define GPS_SLFCHK_CTRL_STATE_TIMEPULSE	2
#define GPS_SLFCHK_CTRL_STATE_POWEROFF  3

/**
	@ingroup GPS
	@brief GPS״̬���ƽṹ��
*/
typedef struct {
	U32 TimeCnt;
	U16 TimePeriod;	
	U16 Timeout;	
	U16 TimeoutCnt;	
	U16 Time1mCnt;		                //1���Ӽ�ʱ��

    S16 GpsUnfix1hPeriodTimSlot;        //lq δ��λ1h��ʱ
    S32 GpsUnfix1hPeriodTimFrame;            
    
    S16 GpsOnTimSlot;                   //lq GPS������ʱ
    S32 GpsOnTimFrame;    
    S16 GpsOffTimSlot;                  //lq GPS�رն�ʱ
    S32 GpsOffTimFrame;    
    U8  ucGpsOnTimEn;                   //lq GPS������ʱ��Ч
    U8  ucGpsOffTimEn;                  //lq GPS�رն�ʱ��Ч

	U8  GpsPowerSta;	                //GPS����״̬
	U8  GpsCtrlSta;		                //GPS����״̬��
	U8	GPSSyn_Flag;
	
	U8  GpsPowerupFix;
	U8  GpsNormalFix;
	U8	SaveOpenTime;                   //���濪��ʱ���־λ�����ڽ��GPS������������������ظ����濪��ʱ������

	U16 GpsSynCount;                    //ͬ��ʱ�����  ���ٱ�֤4��

	U16 GpsFixCount;                    //gps������λ����
	U16 GpsFallCount;                   //GPS��λʧ�ܼ���
	U16	GpsWaitCount;		            //gps������������
	U8	GpsWaitFix;                     //GPS�����ȶ�λ��־
    U8  ucGpsUnfix1hLngOpenFlg;         //GPSδ��λ1h�ڳ�����־��1min����
    U8  ucGpsUnfixBey1hLngOpenFlg;      //GPSδ��λ����1h������־��2min����
    U8  ucGps5minLngOpenEn;             //GPS5min����ʹ�ܱ�־��
    S16 ssGps5minLngOpenCnt;            //GPS5min����������
    U8  GpsIntervalCnt;                 //GPS���������    

    U8  ucGpsSaveFlg;                   //GPSʡ���־��������λʱÿ��һ���������ڿ���һ�Σ�
    U8  ucGpsOnEn;                      //GPSʡ��ģʽ�£�GPS����
	U32 GpsSaveJudgeTimeCnt;
	U16 GpsSaveJudgeTimePeriod;	
	U16 GpsSaveJudgeCntWorkTime;	    //GPSʡ��Ĺ���ʱ���жϼ�ʱ
	U16 GpsSaveJudgeCntBatVolt;	
	U16 GpsSaveJudgeCntTemper;	
	U8  GpsSaveJudgeFlgWorkTime;	    //GPSʡ��Ĺ���ʱ���жϼ�ʱ
	U8  GpsSaveJudgeFlgBatVolt;	
	U8  GpsSaveJudgeCFlgTemper;
//lq	U8  GpsMsgEnable;//����Э��ջ������Ϣ
//lq    U8  ucGpsUnfixLngOpenFlg;//GPSδ��λ������־��
//lq    S16 ssGpsUnfixCnt;//GPSδ��λ����������
//lq    S16 ssGpsUnfixBey3Cnt;//GPSδ��λ��������3�εļ�����
//lq    S16 ssGpsNormalFixCnt;//GPS������λ����������

    U8  GpsOffPositionSampleEn;                 //lq GPSλ��ƫ�Ʋ���ʹ��
    U8  GpsPositionSampleNewFlg;                //lq GPS�²������־
}GPSCtrlStruct;

typedef struct {
	U32 TimeCnt;
	U16 TimePeriod;	
	U16 Time1mCnt;		                //1���Ӽ�ʱ��

	U8  GpsPowerupFix;
	U8  GpsCtrlSta;		                //GPS����״̬��
}GPSCtrlMsgTestStruct;

/**
	@ingroup GPS
	@brief GPSʱ��ͬ���ṹ��
*/
typedef struct {
	S16  GPSSyn_PulsePhase;	//���յ�������ʱ��ʱ϶��λ
	S16  GPSSyn_PhaseOffset;	//ʱ϶ͬ��ʱ��λƫ��
//lq-	U16 GPSSyn_PulseSlot;		//���յ�������ʱ��ʱ϶ֵ 
    S16  GPSSyn_PulseSlot;
	S16  GPSSyn_SlotOffset;	//ʱ϶ͬ��ʱ϶ƫ����
	U16 GPSSyn_SynLevel;		//ͬ�����ȼ�
	U16 GPSSyn_PhaseSynFlag;	//��λͬ����־
	U16 GPSSyn_SlotSynFlag;	//��λͬ����־
	U16 GPSSyn_ProtectCnt;	//��λͬ������ʱ��
	U16 GPSSyn_PulseEn;		//��������Ч��־
	U16 GPSSyn_Flag;			//GPSͬ����Ч��־
	U16 GPSSyn_SlotCnt;
	U16 GPSSyn_SlotEn;
	S32 GPSSyn_PulseMin;
	S16 GPSSyn_MinOffset;
	S32 GPSSyn_PulseHour;    
    S16 GPSSyn_HourOffset;	//lq Сʱƫ��

	U16 ChkTimeS;
	U16 ChkTimeM;
	U16 ChkTimeH;
    U32 ChkTimeFrame;       //lq һ���еĵڼ�֡
    U8  ucGPSSyn_SlotPhaseSynFlag;                                              //lq [20130129]
    //lq U8  GPSSyn_FirstSynFlag;
	U8  GPSSyn_Enable;      //lq GPSͬ��ʹ��
    
	U8  GPSSyn_HourFrameSynEnable;
    U8  ucGPSSyn_HourFrameSynFlag;                                              //lq Сʱ������ͬ��

}GPSSynStruct;

typedef struct GpsSelfCheckStruct
{
	U32 TimeCnt;
	U16 TimePeriod;	
    U16 TimeoutCnt;
    
    U8  CtrlState;
    U8  StartFlg;
    U8  CompleteFlg;
    U8  TimepulseFlg;
    U8  DataCommFlg;
    U8  OnDataCommFlg;
    U8  OffDataCommFlg;
    U8  AntennaFlg;
    U8  ModuleFlg;

    U8  VccCtrlOkFlg;
    U8  TimepulseOkFlg;
    U8  DataCommOkFlg;
    U8  AntennaOkFlg;
    U8  ModuleOkFlg;
}GpsSelfCheckStruct;

extern GPSSynStruct GpsSynCell;	   		//GPSͬ�������ṹ��
extern GPSStruct GpsPara;
extern GPSCtrlStruct GpsCtrlPara;
extern GPSCtrlMsgTestStruct GPSCtrlMsgTestPara;
extern GpsSelfCheckStruct GpsSelfCheckInst;

/* Public  function  ---------------------------------------------------------*/
extern void IniGpsPara(void);

/**
	@ingroup GPS
	@brief GPS���Ľ���������
	@param pDat ����ָ��
	@param uiLen ���ĳ���
	@retval TRUE �����ɹ�
	@retval FALSE ����ʧ��

	���GPS�ı��Ľ�������Ҫ����GPRMC����
*/
extern U8 GpsMsgProcess(U8 *pDat, U16 uiLen);

/**
	@ingroup GPS
	@brief GPS����������
	@param ��
	@retval ��

	���ڸ���GPS�Ƿ�λ����GPS�Ŀ��ص�
*/
extern void GpsCtrlProcess(void);
extern void GpsPowerUp(void);

/**
	@ingroup ����
	@brief �鿴GPSģ���Ƿ�λ/ͬ�����
	@param ��
	@retval TRUE GPS��λ/ͬ����ɣ���GPS�ϵ�
	@retval FALSE GPSæ
*/
extern U8 GetGpsSleepSta(void);
extern U8 GetGpsSynSta(void);

/**
	@ingroup GPS
	@brief Э��ջ��GPSģ��ӿں���
	@param pGpsCell Э��ջ���ڴ�GPS���ݵĽṹ��
	@retval TRUE GPS��λ
	@retval FALSE GPSδ��λ
*/
extern U8 GetGpsFixSta(GPSStruct *pGpsCell);

/**
	@ingroup GPS
	@brief ʱ��ͬ������
	@param ��
	@retval ��

	����GPS���Ľ���ʱ���GPS������ʱ�����µı���ʱ��������ʱ�����Ӷ����ʱ��ͬ��
*/
extern void SlotSynClac(void);

//lq extern U8 GetGpsMsgRequest(void);
extern void SetGpsMsgRequest(void);
extern U8 GetGpsPowerupFixState(void);
extern U8 GetGpsNormalFixState(void);
extern U8 GetGpsNormalFixStateRst(void);
extern void SetGpsNormalFixState(void);
extern void ResetGpsNormalFixState(void);
extern U8 GetGpsIdelState(void);
extern U8 IsGpsFix(void);
extern void InitGpsSynStru(GPSSynStruct *pGpsSynStru);
extern void SetGpsPowerupFixState(void);
extern void SetGpsUnfixPeriodTim(S32 value);
extern void ResetGpsUnfixPeriodTim(void);
extern S32 GetGpsUnfixPeriodTim(void);
extern U8 GetGpsWakeupSta(void);
extern void GpsPowerDown(void);
extern void ResetGpsSynSta(void);
extern U8 IsGpsPowerupProtect(void);
extern U8 GetGpsPowerSta(void);
extern void SetGpsOffTimSlot(S16 value);
extern void SetGpsOffTimFrame(S32 value);
extern S32 GetGpsOffTim(void);
extern void ResetGps5minLngOpenEn(void);
extern U8 GetGps5minLngOpenEn(void);
extern void SetGpsUnfix1hPeriodTimFrame(S32 value);
extern void SetGpsUnfix1hPeriodTimSlot(S16 value);
extern S32 GetGpsUnfix1hPeriodTim(void);
extern void SetGpsUnfixLonOpenFlg(void);
extern void ResetGpsUnfixLonOpenFlg(void);
extern U8 GetGpsUnfixLonOpenFlg(void);
extern void SetGpsUnfix1hLngOpenFlg(void);
extern void ResetGpsUnfix1hLngOpenFlg(void);
extern U8 GetGpsUnfix1hLngOpenFlg(void);
extern void SetGpsUnfixBey1hLngOpenFlg(void);
extern void ResetGpsUnfixBey1hLngOpenFlg(void);
extern U8 GetGpsUnfixBey1hLngOpenFlg(void);
extern U8 IsGpsOffTimeout(void);
extern void SetGpsOnTimEn(void);
extern void ResetGpsOnTimEn(void);
extern U8 GetGpsOnTimEn(void);
extern U8 IsGpsOnTimeout(void);
extern U8 IsGpsUnfix1hPeriodTimeout(void);
extern void SetGpsOnTim(S16 usValue);
extern U8 GetMsgTestSendFlg(void);
extern void GpsCtrlProcess_MsgTest(void);
extern U8 GetGpsOnEn(void);
extern U8 GetGpsSaveFlg(void);
extern void GpsSaveCondJudge(void);
extern void SetGpsOnEn(void);
extern void ResetGpsOnEn(void);
extern U8 GetGpsOffPositionSampleEn(void);
extern U8 GetGpsPositionSampleNewFlgRst(void);
extern void SetGpsPositionSampleNewFlg(void);
extern void GpsWakeUp(void);
extern void ResetGpsOffPositionSampleEn(void);


extern void IniGpsMsgStru(GPSStruct *pGpsCell);//lnw add

#endif
