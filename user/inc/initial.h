/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	interfunc.h
  Author: ���      Version: V1.00       Date: 2012.03.21 
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
#ifndef _INITIAL_H
#define _INITIAL_H
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
#define EQ_FREQSTA_LOW			0		//����״̬
#define EQ_FREQSTA_HIG			1		//��������״̬

//#define EQ_MOD_POWERSAVE		0		//ʡ��ģʽ
#define EQ_MOD_NORMAL			0		//ʡ��ģʽ
#define EQ_MOD_POWERSAVE		1		//����AISģʽ

#define EQ_WORKMOD_TEST			0		//����̬
#define EQ_WORKMOD_NORMAL		1		//��������״̬
#define EQ_WORKMOD_READFILE		2		//��ȡSD���ļ�״̬

#define EQ_XMTFREQ_CHA			161975000
#define EQ_XMTFREQ_CHB			162025000

#define EQ_MOD_OFFSET			1850		//Ĭ��״̬�µ�Ƶ��׼ȷ��ֵ
#define EQ_MOD_AMP				30			//Ĭ��״̬�µ��Ʒ��ȵ�ֵ

#define EQ_MOD_TEST_STA_MAIN        0
#define EQ_MOD_TEST_STA_BATTERY     1
#define EQ_MOD_TEST_STA_MSG         2
#define EQ_MOD_TEST_STA_RX          3
#define EQ_MOD_TEST_STA_SELF_CHECK  4

/*lq �ַ����� */
#define EQ_VEND_ID_LEN              7
#define EQ_ATON_NAME_LEN            34

/*lq */
#define VIRTUAL_ATON_FLAG_REAL      0
#define VIRTUAL_ATON_FLAG_VIRTUAL   1

#if 0
/**
	@ingroup ������ȡ
	@brief ���������ṹ��
*/
typedef struct {						
	U8  ucLowFreqSta;					///<���߱�־
	U8  ucWorkMode;						///<����̬����������״̬
	U8  ucGpsOutput;					///<GPS������״̬
	
	U16 uiFreqAdj;						///<��Ƶ����Ƶ��׼ȷ��
	U16 uiFreqOffset;					///<Ƶ��ƫ��
	U16 uiModAmpAdj;					///<���Ʒ��Ȳ�����16 ~ 48
	U16 uiModAmp;						///<���Ʒ���
	U32 uiXmtFreq;
    S32 slXmtFreqComp;                  //lq RDA1846����Ƶ�ʲ���ֵ	   slXmtFreqComp= siXmtFreqAdj*1850
	U16 GridVoltAdj;                    //lq դѹDA���Ƶ���
    S16 GridVoltOffset;

	U16 PowerOpenTime;                  //power��������ʱ��
	U16 SosOpenTime;                    //sos��������ʱ��
	U16 GpsHotTime;                     //GPS��������ʱʱ��
	U16 NormPoweronCntThres;            //��������������ֵ
	U16 BatteryLowLevel;                //��ص�ѹ������
	U16 BatteryUltraLowLevel;           //��ص�ѹ��������
	U16 CloseTime;                      //�ػ�������Ӧʱ��
	U16 UserTestCntThres;               //�û����Դ�����ֵ
	U16 GpsPoweronProtectTime;          //gps������λ����ʱ�䣬��λ����
	U16 GpsFixFallThres;                //GPS����������λ������ֵ 
    U16 WorkTimeSum;                    //������ʱ��
	U16 SendInterval;                   //��Ϣ���ͼ������λ����
	U16 NormalSendTime;                 //������Ϣ���ʹ���

	U8	BatteryTestSta;                 //����̬�����ģʽ��־λ
    U8  TestSta;
} EqipStruc;
#endif
#if 0
//��̬��Ϣ�ṹ��
/**
	@ingroup ������ȡ
	@brief ��̬�����ṹ��
*/
typedef struct {
	U32 MMSI;              // (xxxxxxxxx)
    U32 DestMMSI;          // (xxxxxxxxx)

    /*lq P712*/
	char VendID[EQ_VEND_ID_LEN + 1];          //lq ����\0��
	U32 SerialNumber;
	U8  Refresh;

    /*lq ACE(X Q)*/
    U8  AtoNStatus;       	// (hh)
    U16 OffPositionThresh;	// (xxxx)
    U8  AckProcedure;     	// (x)Type1����Ҫ
    U8  OffPositionBehav; 	// (x)
    U8  SynchLostBehav;		// (x)
    char NameofAtoN[EQ_ATON_NAME_LEN + 1];    //����\0��, (c--c)maximum 34 characters
    U16	ShipA;			    // (xxx)
    U16	ShipB;			    // (xxx)
    U8	ShipC;			    // (xx)
    U8	ShipD;			    // (xx)
    
    /*lq ACF(X Q)*/
    U8	TypeofEPFS;		    // (x)
    S32	NomLongitude;		// (yyyyyyyyy)
    S32	NomLatitude;		// (llllllll)
    U8	PositionAccuracy;	// (x)
    U16 TxChannel1;		    // (xxxx)
    U16 TxChannel2;		    // (xxxx)
    U8	PowerLevel;		    // (x)
    U8  TypeofAtoN;		    // (xx)
    U8	VirtualAtoNFlag;	// (x)

    /*lq AAR(X Q)*/
    U8	TxHourChannel1;		// (xx)
    U8	TxMinChannel1;		// (xx)
    U16	TxStartSlotChannel1;// (xxxx)
    S32	TxSlotIntervalChannel1;	// (xxxxxxx)
    S32	TxSlotIntervalChannel1Off;	// (xxxxxxx)
    U8	TxHourChannel2;		// (xx)
    U8	TxMinChannel2;		// (xx)
    U16	TxStartSlotChannel2;// (xx)
    S32	TxSlotIntervalChannel2;	// (xxxxxxx)0-24*60*2250

} StaticInforStruc;
#endif

#if 0
extern StaticInforStruc	siStaticPara;		//��̬����
extern EqipStruc eqEquipPara;               //��������
#endif

#define DRIVER_1846_POWERDOWN		0
#define DRIVER_1846_XMT				1
#define DRIVER_1846_NONE			2
#define DRIVER_1846_REC				3

typedef struct {
	U8  uc1846XmtSta;					//1846���ͽ���״̬����
}DriverStru;

extern DriverStru dsDriverPara;
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/
extern void Parameter_Init(void);
#endif
