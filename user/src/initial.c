/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: initial.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    ��ʼ����������ȫ��ģ��Ĳ����ڴ˴����г�ʼ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include "def.h"
#include "initial.h"
#include "parastorage.h"
#include "gmskmod.h"
#include "LinkPtlPort.h"
#include "LinkPtlReport.h"
#include "parastorage.h"
#include "powerup.h"
#include "powerdown.h"
#include "uartmsg.h"
#include "gpsctrl.h"
#include "mmi.h"
#include "sleepctrl.h"
#include "testing.h"
#include "keyscan.h"
#include "digunit.h"
#include "ledctrl.h"
#include "batteryctrl.h"
#include "ledctrl.h"
#include "adjust.h"
#include "rtc.h"
#include "DAC.h"
#include "sotdma.h"
#include "offposition.h"
#include "config.h"
#include "Include.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private  function  --------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
#if 0
EqipStruc			eqEquipPara;		//��������
StaticInforStruc	siStaticPara;		//��̬����	
DriverStru			dsDriverPara;
#endif
/* Public  function  ---------------------------------------------------------*/
#if 0
/*******************************************************************************
* ����  : InitEquipCell
* ����  : ��ʼ�������Ĳ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitEquipCell(void)
{
	if (ucStandbyWakeupFlg == TRUE)
    {
        eqEquipPara.ucLowFreqSta = EQ_FREQSTA_LOW;	//��������      
    }
    else
    {
        eqEquipPara.ucLowFreqSta = EQ_FREQSTA_HIG;	//����������
    }
	eqEquipPara.UserTestCntThres = 98;
	eqEquipPara.ucWorkMode = EQ_WORKMOD_NORMAL;
	eqEquipPara.ucGpsOutput = FALSE;

	eqEquipPara.uiFreqAdj = PARA_FREQADJ_DEFAULT;		//�ò�����Ҫ���ڵ��ԣ�����
	eqEquipPara.uiFreqOffset = (U16)(((U32)eqEquipPara.uiFreqAdj * EQ_MOD_OFFSET ) >> 5);
	eqEquipPara.uiModAmpAdj = PARA_MODAMP_DEFAULT;
	eqEquipPara.uiModAmp = (U16)(((U32)eqEquipPara.uiModAmpAdj * EQ_MOD_AMP + 16) >> 5);

	eqEquipPara.uiXmtFreq = EQ_XMTFREQ_CHA;
    eqEquipPara.slXmtFreqComp = 0;                  //lq

    eqEquipPara.GridVoltAdj = PARA_RF_VOLT_ADJUST_DEFAULT;
    eqEquipPara.GridVoltOffset = (eqEquipPara.GridVoltAdj - 10) * DA_VALUE_01V;
#if 0
	eqEquipPara.SendInterval = 6;                   //���ͼ��Ĭ��6����
	eqEquipPara.SendErrorPeriod = eqEquipPara.SendInterval >> 1;    //���ͳ�ʱʱ�䷢��һ��

	if (eqEquipPara.SendErrorPeriod < 1)
	{
		eqEquipPara.SendErrorPeriod = 1;
	}	
#endif	
	eqEquipPara.PowerOpenTime = 3;                  //Ĭ�Ͽ���ʱ��3��
	eqEquipPara.SosOpenTime = 3;                    //Ĭ��SOS����ʱ��3��
	eqEquipPara.GpsHotTime = 4;                     //Ĭ����������ʱʱ��40��
	eqEquipPara.NormPoweronCntThres = 1;            //�澯SOS����ʱ��
	eqEquipPara.BatteryLowLevel = PARA_BATTERY_LOW_DEFAULT;             //Ĭ�ϵ�����6.8v
	eqEquipPara.BatteryUltraLowLevel = PARA_BATTERY_ULTRALOW_DEFAULT;   //��������6.2v
	eqEquipPara.CloseTime = 3;                      //Ĭ�Ϲػ�����ʱ��3��
	eqEquipPara.GpsPoweronProtectTime = 5;          //Ĭ�Ͽ�����һ�ζ�λʱ��5����
	eqEquipPara.GpsFixFallThres = 3;                //Ĭ��������ʧ��������������

    eqEquipPara.WorkTimeSum = PARA_WORKTIME_DEFAULT;
	eqEquipPara.SendInterval = ITDMA_MSG_SEND_INTERVAL_DEF_VAL;         //��Ϣ���ͼ������λ����
	eqEquipPara.NormalSendTime = ITDMA_MSG_SEND_TIME_DEF_VAL;           //������Ϣ���ʹ���

    eqEquipPara.TestSta = EQ_MOD_TEST_STA_MAIN;
}
#endif

#if 0
/*******************************************************************************
* ����  : InitStaticInforCell
* ����  : ��ʼ����̬��Ϣ����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitStaticInforCell(void)
{
    U16 i;

    siStaticPara.MMSI = PARA_MMSI_DEFAULT;
    siStaticPara.DestMMSI = PARA_MMSI_DEST_DEFAULT;
    sprintf(siStaticPara.VendID, "%s", PARA_VENDID_DEFAULT);    

    siStaticPara.SerialNumber = PARA_SERIAL_DEFAULT;
    siStaticPara.Refresh = FALSE;
    siStaticPara.AtoNStatus = PARA_ATON_STATUS_DEFAULT;
    siStaticPara.OffPositionThresh = PARA_OFF_POSITION_THRES_DEFAULT;
    siStaticPara.AckProcedure = 0;                                      //lq... Type1 AtoN����Ҫ
    siStaticPara.OffPositionBehav = PARA_OFF_POSITION_BEHAV_DEFAULT;
    siStaticPara.SynchLostBehav = PARA_SYNCH_LOST_BEHAV_DEFAULT;
    sprintf(siStaticPara.NameofAtoN, "%s", PARA_ATON_NAME_DEFAULT);    
    
    siStaticPara.ShipA = PARA_SHIPA_DEFAULT;
    siStaticPara.ShipB = PARA_SHIPB_DEFAULT;
    siStaticPara.ShipC = PARA_SHIPC_DEFAULT;
    siStaticPara.ShipD = PARA_SHIPD_DEFAULT;
    siStaticPara.TypeofEPFS = PARA_EPFS_TYPE_DEFAULT;
    siStaticPara.NomLongitude = PARA_NOM_LONGITUDE_DEFAULT;
    siStaticPara.NomLatitude = PARA_NOM_LATITUDE_DEFAULT;
    siStaticPara.PositionAccuracy = PARA_POSITION_ACCURACY_DEFAULT;
    siStaticPara.TxChannel1 = PARA_CHANNEL_1_DEFAULT;
    siStaticPara.TxChannel2 = PARA_CHANNEL_2_DEFAULT;
    siStaticPara.PowerLevel = PARA_POWER_LEVEL_DEFAULT;
    siStaticPara.TypeofAtoN = PARA_ATON_TYPE_DEFAULT;
    siStaticPara.VirtualAtoNFlag = PARA_VIRTUAL_ATON_FLAG_DEFAULT;
    siStaticPara.TxHourChannel1 = PARA_TX_HOUR_CHANNEL_1_DEFAULT;
    siStaticPara.TxMinChannel1 = PARA_TX_MIN_CHANNEL_1_DEFAULT;
    siStaticPara.TxStartSlotChannel1 = PARA_TX_START_SLOT_CHANNEL_1_DEFAULT;
    siStaticPara.TxSlotIntervalChannel1 = PARA_TX_INTERVAL_CHANNEL_1_DEFAULT;
    siStaticPara.TxSlotIntervalChannel1Off = PARA_TX_INTERVAL_CHANNEL_1_OFF_DEFAULT;
    
    /*lq �ŵ�2�ķ���ʱ϶��FATDMA�ṹ�����������ָ�����˴����ṩ��ʼֵ*/
    siStaticPara.TxHourChannel2 = PARA_TX_HOUR_CHANNEL_1_DEFAULT;
    siStaticPara.TxMinChannel2 = PARA_TX_MIN_CHANNEL_1_DEFAULT;
    siStaticPara.TxStartSlotChannel2 = PARA_TX_START_SLOT_CHANNEL_1_DEFAULT;
    siStaticPara.TxSlotIntervalChannel2 =PARA_TX_INTERVAL_CHANNEL_1_DEFAULT;
}
#endif

#if 0
/*******************************************************************************
* ����  : InitDsDriverPara
* ����  : ��ʼ���������ƵĲ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitDsDriverPara(void)
{
	dsDriverPara.uc1846XmtSta = DRIVER_1846_POWERDOWN;
}
#endif

/*******************************************************************************
* ����  : Parameter_Init
* ����  : ��ʼ����Ҫ�Ĳ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void Parameter_Init(void)
{	
    DEVINFO_ModuleInit();
    AISINFO_ModuleInit();	
    IniEEPROM_Para();                                                           //��EEPROM�ж�ȡ����
	IniGpsPara();                                                               //lq ������������GPS����
	InitGpsCell();
    InitaLEDCtrlPara();
	LEDInit(LED_TYPE_NONE);

#if CONFIG_POWERDOWN
	IniPowerDownPara();
#endif

	IniSleepCtrlPara();

#if CONFIG_BAT_MONITOR
    BS1_InstanceInit();                                                         //lq �������ģ��
	InitBatteryCtrlPara();
#endif

#if CONFIG_TEMPERATURE_MONITOR    
    InitTemperSensorPara();                                                     //lq �¶ȼ��	
#endif    

#if CONFIG_KEY
	IniMMIPara();	
	IniSingleKey(KEY_POWER_ADDR, KEY_HIGH);
	IniSingleKey(KEY_SOS_ADDR, KEY_HIGH);
#endif

	InitTimAdjust();                                                            //��ʱ��������ʼ��
	InitRtcPara();
    InitRtcParaStruct(&struRtcPara);
    InitPowerStru();                                                            //lq ����״̬��ʼ��

#if CONFIG_GPS_UBX_EN
    UBX_ModuleInit();
#endif

    InitRtcTimPhaseStruct();

    RF1_RFInstanceInit();
    PHY_ModuleInit();
    DLS_ModuleInit();
    LME_ModuleInit();    
    TRCTRL_ModuleInit();    	
    AISDI_ModuleInit();
    MSGMAN_ModuleInit();
    TDMA_ModuleInit();
    InitRxTestStruct(&RxTestInstance);
    TASK_ModuleInit();

#if 0
#if 0
	InitaDigDeal();
	InitFSKDataTranPara();
	InitaLinkPtlPortPara();                                                     //lq ��ʼ����Ϣ���У�δ����֮ǰ��
    InitFSKDataXmtFIFO(&gstruFSKDataXmtFIFO);                                   //lq ��ʼ����Ϣ������У�����֮��
	InitaReportPara();
	InitPowerUppara();
	IniUartMsgPara();		
	Ptl_Init();
	InitMsgCtrlPara();
    InitSOTDMAPara();
	IniTestingPara();
    IniGpsCtrlMsgTestStru(&GPSCtrlMsgTestPara);
#endif
    OP1_GPSOffPositionInstanceInit();
    LB1_LBPrtCommInstanceInit();
#endif

#if CONFIG_DEBUG
    /*lq �з�����*/
#if 0
    InitPinPulse();
    BS_TestInstanceInit();                                                      //lq ��ѹAD������ͳ��
    ADCTestInit();
#endif
#endif

#if CONFIG_SD_CARD_MODULE_EN
	/***************���������ļ�***********************************************/
	CreateFile();
#endif
}
#if 0
/*******************************************************************************
* ����  : IsFixedAtoN
* ����  : �Ƿ��ǹ̶�AtoN
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 IsFixedAtoN(void)
{
    if (siStaticPara.TypeofAtoN >= 5
        && siStaticPara.TypeofAtoN <= 19)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }
}
#endif
#if 0
/*******************************************************************************
* ����  : GetAtoNAlarmFlag
* ����  : ��ѯAtoN�����־
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetAtoNAlarmFlag(void)
{
    if ((siStaticPara.AtoNStatus & 0x1) == 1)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }
}
#endif
