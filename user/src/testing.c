/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: testing.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    ����̬ģ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include "stm32f10x.h"
#include "def.h"
#include "initial.h"
#include "debug.h"
#include "common.h"
#include "parastorage.h"
#include "testing.h"
#include "systemtick.h"
#include "keyscan.h"
#include "ledctrl.h"
#include "gmskmod.h"
#include "digunit.h"
#include "ioctrl.h"
#include "LED.h"
#include "Tim.h"
#include "stdio.h"
#include "sleepctrl.h"
#include "adjust.h"
#include "gpsctrl.h"
#include "config.h"
#include "RxTx.h"
#include "Include.h"

extern void EnterStopMod(void);

#if 0
#define SINGLE_LENTH		(29)
const U8 SingleTwo[SINGLE_LENTH] =
{
	0x33,0x33,0x33,0x01,
	0x55,0x55,0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,0x55,0x55,
	0x81,0x1E,
	0x01,0x00,
};
#else
#define SINGLE_LENTH		(21)
#define SINGLE_T2_LENTH     (20)
const U8 SingleTwo[SINGLE_LENTH] =
{
	0x48,0x99,0x7,0xad,0x40,0x0,0x4c,0xb,0xa6,0x74,0x52,
	0xe4,0xdb,0x3d,0x86,0x32,0xac,0x0,0x1e,0x0,0x6,
};
#endif

U8 TestTempFlag = 0;
const U8 SingleT2[SINGLE_T2_LENTH] = 
{
	0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
};
const U8 SingleT3[SINGLE_LENTH] = 
{
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,
};

/*lq SART��׼�����ź�1��01010101*/
const U8 STSignal_1[SINGLE_LENTH] = 
{
	0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
	0x55,
};

/*lq SART��׼�����ź�2��00001111*/
const U8 STSignal_2[SINGLE_LENTH] = 
{
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,
};

/*lq SART��׼�����ź�3��α�������*/
const U8 STSignal_3[SINGLE_LENTH] =
{
	0x48,0x99,0x7,0xad,0x40,0x0,0x4c,0xb,0xa6,0x74,0x52,
	0xe4,0xdb,0x3d,0x86,0x32,0xac,0x0,0x1e,0x0,0x6,
};

TestCtrlStruct TestCtrlPara;
BatteryTestCtrlStruct BatteryTestCtrlPara;
MsgTestStruct struMsgTestPara;

RxTestStruct RxTestInstance;

/*******************************************************************************
* ����  : TEST_TxTestLedInit
* ����  : ������Գ�ʼ��
* ����  : type - �����ź�����
* ���  : ��
* ����  : ��
*******************************************************************************/
void TEST_TxTestLedInit(U8 type)
{
	if (type == TRCTRL_TEST_TX_SIGNAL_CW)
	{
		LEDInit(LED_TYPE_TEST_TEST1);
	}
	else if (type == TRCTRL_TEST_TX_SIGNAL_NUM1)
	{
		LEDInit(LED_TYPE_TEST_TEST2);
	}
	else if (type == TRCTRL_TEST_TX_SIGNAL_NUM2)
	{
		LEDInit(LED_TYPE_TEST_TEST3);
	}
	else if (type == TRCTRL_TEST_TX_SIGNAL_NUM3)
	{
		LEDInit(LED_TYPE_TEST_TEST4);
	}
}

/*******************************************************************************
* ����  : TEST_TxTestInit
* ����  : ������Գ�ʼ��
* ����  : channel - �����ŵ�
          type - �����ź�����
          transceiverType - �շ�������
* ���  : ��
* ����  : ��
*******************************************************************************/
void TEST_TxTestInit(AisChannelEnum channel, U8 type,
                     TransceiverTypeEnum transceiverType)
{
    RxTxCtrl(RXTX_CTRL_TYPE_TX, 0, 0);
    
    TRCTRL_InstTransceiverTestStructInit(transceiverType);
    
    TRCTRL_InstTxTestSetTxSignalType(transceiverType,
        type);
    
    TRCTRL_InstTxTestSetTxChannel(transceiverType,
        channel);
    
    TRCTRL_InstSetTxTestFlg(transceiverType); 
    
    TEST_TxTestLedInit(type);   
}

/*******************************************************************************
* ����  : TEST_TxTestStop
* ����  : ������Խ���
* ����  : transceiverType - �շ�������
* ���  : ��
* ����  : ��
*******************************************************************************/
void TEST_TxTestStop(TransceiverTypeEnum transceiverType)
{  
    SI446XChipEnum cs;

    /*lq ѡ��ʵ�� */
    if (transceiverType == TransceiverTypeA)
    {
        cs = SI446X_CHIP_A;
    }
    else if (transceiverType == TransceiverTypeB)
    {
        cs = SI446X_CHIP_B;
    }
    else
    {
        return ;
    }

    /*lq �˳��շ�״̬ */
    RF_PowerDown();	    
    SI446X_StateTo(SI446X_MODE_IDLE, cs);
    TRCTRL_InstTransceiverTestStructInit(transceiverType);
    
    /*lq LED��ʼ�� */
    LEDInit(LED_TYPE_RED_NONE);
}

/*******************************************************************************
* ����  : InitRxTestStruct
* ����  : ��ʼ�����ղ��Խṹ��
* ����  : pRxTestStruct - RxTestStruct�ṹ��ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitRxTestStruct(RxTestStruct *pRxTestStruct)
{
	pRxTestStruct->RxTestEn = FALSE;
}


/*******************************************************************************
* ����  : IniTestingPara
* ����  : ��ʼ����Ҫ�Ĳ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void IniTestingPara(void)
{
	TestCtrlPara.TimeCnt = 0;
	TestCtrlPara.TimePeriod = TEST_200MS_PERIOD;

	TestCtrlPara.TimeCnt = SetTimePeriodProcess(TestCtrlPara.TimePeriod);

	TestCtrlPara.ucTestNum = TEST_TESTSTA_IDLE;
	TestCtrlPara.KeyScanSta = TEST_KEYSTA_IDLE;
	TestCtrlPara.LEDMsgFlag = FALSE;
	TestCtrlPara.ucRFXmtSta = TEST_RFXMT_IDLE;
	TestCtrlPara.ucXmtEn = FALSE;
	TestCtrlPara.ucXmtData = 0;
	TestCtrlPara.FirstGenFlag = TRUE;//��ʼ�ź�û�в���

    TestCtrlPara.FirstEnterTestFlag = FALSE;
}

/*******************************************************************************
* ����  : IniTestingPara
* ����  : ��ʼ����Ҫ�Ĳ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void IniTesting(void)
{
	IniTestingPara();
	//InitFSKDataTranPara();
	DevStaticPara.uiXmtFreq = EQ_XMTFREQ_CHA;
#if 0
	RDA1846Ctrl(DRIVER_1846_POWERDOWN, DevStaticPara.uiXmtFreq, RDA1846_CS_2);
	//LEDInit(LED_TYPE_NONE);
#endif
}

/*******************************************************************************
* ����  : ResetTesting
* ����  : ��λ������ز���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void ResetTesting(void)
{
	TestCtrlPara.ucTestNum = TEST_TESTSTA_IDLE;
	TestCtrlPara.KeyScanSta = TEST_KEYSTA_IDLE;
	TestCtrlPara.LEDMsgFlag = FALSE;
	TestCtrlPara.ucXmtEn = FALSE;
	TestCtrlPara.ucXmtData = 0;
	DevStaticPara.uiXmtFreq = EQ_XMTFREQ_CHA;
}
#if CONFIG_KEY     
/*******************************************************************************
* ����  : TestingKeyScan
* ����  : ����̬�°���ɨ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void Testing_KeyScan(void)
{
	U8 ucKeySta;
#if !WIN32
	ucKeySta = GetIndivIO(KEY_SOS_ADDR);

	if (TestCtrlPara.KeyScanSta == TEST_KEYSTA_IDLE)
	{
		//������Ҫ��ⰴ���Ƿ�̧�𣬷�ֹ����ʼ�հ���SOS����
		if (ucKeySta == KEY_HIGH)
		{
			TestCtrlPara.KeyScanSta = TEST_KEYSTA_STEP1;
		}
	}
	else if (TestCtrlPara.KeyScanSta == TEST_KEYSTA_STEP1)
	{
		if (ucKeySta == KEY_LOW)
		{
#if 0
			//��ʾ�а������£��������1
			TestCtrlPara.ucTestNum++;

			TestCtrlPara.FirstGenFlag = TRUE;
			if (TestCtrlPara.ucTestNum > TEST_TESTSTA_MOD)
			{
				TestCtrlPara.ucTestNum = TEST_TESTSTA_CARRIER;
			}
#endif
			TestCtrlPara.FirstGenFlag = TRUE;//Ӧ���²����ź�			  		//lq ������ı䣬�����²���������ĵ����źţ������ò�������״η���ʱ�����źţ��ò�����ĺ������������ٽ��е���
																				
			if (TestCtrlPara.ucTestNum == TEST_TESTSTA_CARRIER)
			{
				TestCtrlPara.ucTestNum = TEST_TESTSTA_T2;
			}
			else if (TestCtrlPara.ucTestNum == TEST_TESTSTA_T2)
			{
				TestCtrlPara.ucTestNum = TEST_TESTSTA_SIG2;
			}
			else if (TestCtrlPara.ucTestNum == TEST_TESTSTA_SIG2)
			{
				TestCtrlPara.ucTestNum = TEST_TESTSTA_STSIGNAL2;
			}
			else 		
			{
				TestCtrlPara.ucTestNum = TEST_TESTSTA_CARRIER;
			}
			TestCtrlPara.LEDMsgFlag = TRUE;                                     //lq ����LEDָʾʹ��

			TestCtrlPara.KeyScanSta = TEST_KEYSTA_IDLE;
		}
	}
	else
	{
		TestCtrlPara.KeyScanSta = TEST_KEYSTA_IDLE;
	}
#endif
}
#endif
/*******************************************************************************
* ����  : Testing_LEDCtrl
* ����  : ����̬��LED����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void Testing_LEDCtrl(void)
{
    if (TestCtrlPara.FirstEnterTestFlag == TRUE)
    {
        TestCtrlPara.FirstEnterTestFlag = FALSE;

        /*lq ����ģʽLED��ʼ�� */    
		LEDInit(LED_TYPE_NONE);
		LEDInit(LED_TYPE_POWER_TEST);
    }

	if (TestCtrlPara.LEDMsgFlag == TRUE)
	{
		TestCtrlPara.LEDMsgFlag = FALSE;

		if (TestCtrlPara.ucTestNum == TEST_TESTSTA_CARRIER)
		{
			LEDInit(LED_TYPE_TEST_TEST1);
		}
		else if (TestCtrlPara.ucTestNum == TEST_TESTSTA_T2)
		{
			LEDInit(LED_TYPE_TEST_TEST2);
		}
		else if (TestCtrlPara.ucTestNum == TEST_TESTSTA_SIG2)
		{
			LEDInit(LED_TYPE_TEST_TEST3);
		}
		else if (TestCtrlPara.ucTestNum == TEST_TESTSTA_STSIGNAL2)
		{
			LEDInit(LED_TYPE_TEST_TEST4);
		}
	}
}

/*******************************************************************************
* ����  : Testing_SetTestType
* ����  : �������õ�ǰ����ģʽ
* ����  : ucType������ģʽ		  
* ���  : ��
* ����  : �ɹ�/ʧ��
*******************************************************************************/
U8 Testing_SetTestType(U8 ucType, U32 ulFreq)
{
	if (ucType == TEST_TESTSTA_FREQADJ)
	{
		if (TestCtrlPara.ucTestNum != TEST_TESTSTA_FREQADJEN)
		{
			return (FALSE);
		}
	}
	else if (ucType == TEST_TESTSTA_MODADJ)
	{
		if (TestCtrlPara.ucTestNum != TEST_TESTSTA_MODADJEN)
		{
			return (FALSE);
		}
	}
    else if (ucType == TEST_TESTSTA_GRIDADJ)
    {
		if (TestCtrlPara.ucTestNum != TEST_TESTSTA_GRIDADJEN)
		{
			return (FALSE);
		}    
    }

	else
	{
		TestCtrlPara.FirstGenFlag = TRUE;//Ӧ���²����ź�
		TestCtrlPara.ucTestNum = ucType;
		DevStaticPara.uiXmtFreq = ulFreq;
	}

    TestCtrlPara.LEDMsgFlag = TRUE;

	return (TRUE);
}

extern S16 gusSXCount;	       		//2250ʱ϶����
/*******************************************************************************
* ����  : Testing_CarrierTest
* ����  : ����̬���ز�������
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
#define TEST_TESTBUF_LEN	60
void GMSKTest(void)                                                             //lq ����̬״̬����
{
#if 0
	if (TestCtrlPara.ucRFXmtSta == TEST_RFXMT_IDLE)
	{
		if (TestCtrlPara.ucXmtEn == TRUE)					 					//lq �����µķ���	
		{
			TestCtrlPara.ucXmtEn = FALSE;
			TestCtrlPara.ucRFXmtSta = TEST_RFXMT_STEP1;
		}
	}
	else if (TestCtrlPara.ucRFXmtSta == TEST_RFXMT_STEP1)
	{
        RxTxSwitch(RXTX_SWITCH_TYPE_RX_TO_TX);                           
#if 0
        RDA1846Ctrl(DRIVER_1846_XMT, DevStaticPara.uiXmtFreq + DevStaticPara.slXmtFreqComp, RDA1846_CS_2);//lq
#endif 
         
		if ((TestCtrlPara.ucXmtType == 0)
			|| (TestCtrlPara.ucXmtType == 1))
		{
			//GMSK����
			GMSKModTest(&TestCtrlPara.ucXmtBuf[0], 
						FSKDATA_NOWSLOT, 
						TestCtrlPara.uiXmtBufLen, 
						DevStaticPara.uiModAmp, 
						DevStaticPara.uiFreqOffset,
						TestCtrlPara.ucXmtType);//ע�����ֵ��Ʋ���֡ͷ֡β
		}
		else
		{
#if 0
			GMSKModTest2(&TestCtrlPara.ucXmtBuf[0], //����������ĵ��ƺ���
							FSKDATA_NOWSLOT, 
							TestCtrlPara.uiXmtBufLen, 
							DevStaticPara.uiModAmp, 
							DevStaticPara.uiFreqOffset);			
#else
			GMSKModTest3(&TestCtrlPara.ucXmtBuf[0], //lq ��ʹ��NRZI�������ݼ�У����
							FSKDATA_NOWSLOT, 
							TestCtrlPara.uiXmtBufLen, 
							DevStaticPara.uiModAmp, 
							DevStaticPara.uiFreqOffset);
#endif		
        }

		pFSKDataXmtStru2 = &FSKDataXmtStru;//lq+
		SetFSKDataXmtEn();

		TestCtrlPara.ucRFXmtSta = TEST_RFXMT_STEP2;		
//lq-	   	TestCtrlPara.ucRFXmtSta = TEST_RFXMT_IDLE;	
	}
	else if (TestCtrlPara.ucRFXmtSta == TEST_RFXMT_STEP2)
	{
		//�ȴ��������ݷ������
//lq-		if (FSKDataXmtStru.uiTran_Sta == FSKDATA_TRAN_IDLE)
	    if (!GetFSKDataXmtEn())	
    	{
			//DigUnitPara.TranEnd = FALSE;

			//�ر�1846
//lq-			RDA1846Ctrl(DRIVER_1846_POWERDOWN, DevStaticPara.uiXmtFreq);
			TestCtrlPara.ucRFXmtSta = TEST_RFXMT_IDLE;
		}
	}
	else
	{
		 TestCtrlPara.ucRFXmtSta = TEST_RFXMT_IDLE;
	}
#endif    	
}



/*******************************************************************************
* ����  : Testing_ModTest
* ����  : ����̬�µ��Ʋ�����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void Testing_ModTest(U8 ucType)                                                 //lq ����ʹ�ܣ���ָ������Ƶ�����
{
	U16 i;
	TestCtrlPara.ucXmtEn = TRUE;                                                //lq ����ʹ��

	if (TestCtrlPara.FirstGenFlag == TRUE)                                      //lq ����ĳ��������ʱִֻ��һ�Σ����ڸò�����ĵ�һ�η���ʱִ��
	{	

		if (ucType == TEST_XMTTYPE_CARRIER)
		{
			for (i = 0; i < TEST_TESTBUF_LEN; i++)
			{
				TestCtrlPara.ucXmtBuf[i] = 0;
			}
			TestCtrlPara.uiXmtBufLen = TEST_TESTBUF_LEN;
			TestCtrlPara.ucXmtType = 1;
		}
		else if (ucType == TEST_XMTTYPE_MOD)
		{
			for (i = 0; i < TEST_TESTBUF_LEN; i++)
			{
				TestCtrlPara.ucXmtBuf[i] = 0x55;
			}
			TestCtrlPara.uiXmtBufLen = TEST_TESTBUF_LEN;
			TestCtrlPara.ucXmtType = 0;
		}
		else if (ucType == TEST_XMTTYPE_SIG2)
		{
			for (i = 0; i < SINGLE_LENTH; i++)
			{
				TestCtrlPara.ucXmtBuf[i] = SingleTwo[i];
			}
			TestCtrlPara.uiXmtBufLen = SINGLE_LENTH;
			TestCtrlPara.ucXmtType = 2;
		}
		else if (ucType == TEST_XMTTYPE_T2)
		{
			for (i = 0; i < SINGLE_T2_LENTH; i++)
			{
				TestCtrlPara.ucXmtBuf[i] = SingleT2[i];
			}
			TestCtrlPara.uiXmtBufLen = SINGLE_T2_LENTH;
			TestCtrlPara.ucXmtType = 2;
		}
		else if (ucType == TEST_XMTTYPE_T3)
		{
			for (i = 0; i < SINGLE_LENTH; i++)
			{
				TestCtrlPara.ucXmtBuf[i] = SingleT3[i];
			}
			TestCtrlPara.uiXmtBufLen = SINGLE_LENTH;
			TestCtrlPara.ucXmtType = 2;
		}
		else if (ucType == TEST_XMTTYPE_STSIGNAL2)
		{
			for (i = 0; i < SINGLE_LENTH; i++)
			{
				TestCtrlPara.ucXmtBuf[i] = STSignal_2[i];
			}
			TestCtrlPara.uiXmtBufLen = SINGLE_LENTH;
			TestCtrlPara.ucXmtType = 2;
		}	
	}	
}

/*******************************************************************************
* ����  : Testing_TestMain
* ����  : ����̬�µ��Ʋ�����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void Testing_TestMain(void)
{
	//200ms���ڷ����ʱ
	if (TestCtrlPara.TimeCnt == GetLocalClockTickProcess())						//lq ÿ200ms����һ��
	{
		TestCtrlPara.TimeCnt = SetTimePeriodProcess(TestCtrlPara.TimePeriod);

		if ((TestCtrlPara.ucTestNum == TEST_TESTSTA_CARRIER)
			|| (TestCtrlPara.ucTestNum == TEST_TESTSTA_FREQADJEN))
		{
			Testing_ModTest(TEST_XMTTYPE_CARRIER);							   	//lq �����µ�һ�η��䣬��ָ���������ͺʹ����Ƶ�����
		}
		else if ((TestCtrlPara.ucTestNum == TEST_TESTSTA_MOD)
				|| (TestCtrlPara.ucTestNum == TEST_TESTSTA_MODADJEN))
		{
			Testing_ModTest(TEST_XMTTYPE_MOD);
		}
		else if (TestCtrlPara.ucTestNum == TEST_TESTSTA_SIG2)
		{
			Testing_ModTest(TEST_XMTTYPE_SIG2);	
		}
		else if (TestCtrlPara.ucTestNum == TEST_TESTSTA_T2)
		{
			Testing_ModTest(TEST_XMTTYPE_T2);	
		}
		else if (TestCtrlPara.ucTestNum == TEST_TESTSTA_T3)
		{
			Testing_ModTest(TEST_XMTTYPE_T3);	
		}
		else if (TestCtrlPara.ucTestNum == TEST_TESTSTA_STSIGNAL2)
		{
			Testing_ModTest(TEST_XMTTYPE_STSIGNAL2);	
		}
	}

	GMSKTest();
}
/*******************************************************************************
* ����  : Testing
* ����  : ����̬ģ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void Testing(void)
{
#if CONFIG_KEY     
	//�˴����밴��ɨ��
	Testing_KeyScan();
#endif	
	//����̬�������
	Testing_TestMain();

	//�˴�����LED����
	Testing_LEDCtrl();

	RtcClockChoice();//����RTCʱ��Դ�ĵ���
}

/*******************************************************************************
* ����  : BatteryTestCtrlLowFreq
* ����  : ��ص������ģʽ��Ƶ����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void BatteryTestCtrlLowFreq(void)
{
	U8  ucPowerKeyVal;

	EnterStopMod();

	if (GetMsgTestCtrlWakeupFlag() == TRUE)
	{
		DevDynamicPara.ucLowFreqSta = EQ_FREQSTA_HIG;
//lq        TEST_OUT_HIGH();   //lq ���ص�Դ��Ϊ������ģʽ
	}

#if CONFIG_KEY     
	ucPowerKeyVal = ReadSingleKeyValProcess(&PowerKey);//��ǰ����״̬
	if (ucPowerKeyVal == KEY_LOW)
	{
		DevStaticPara.ucLowFreqSta = EQ_FREQSTA_HIG;
//lq        TEST_OUT_HIGH();   //lq ���ص�Դ��Ϊ������ģʽ	
	}
#endif
}

/*******************************************************************************
* ����  : InitBatteryTest
* ����  : ��ʼ����ص���������,�˺����ڴ������õ�ʱ�����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitBatteryTest(void)
{
#if 0
	ResetTimeCount();
	
	BatteryTestCtrlPara.CtrlState = BATTERY_TEST_STATE_POWERUP;
	
	BatteryTestCtrlPara.TimeCntGps = 0;
	BatteryTestCtrlPara.TimeCntMessage = 0;
	BatteryTestCtrlPara.TimePeriod = 0;

	BatteryTestCtrlPara.GpsPowerOn = FALSE;

	DevStaticPara.ucLowFreqSta = EQ_FREQSTA_HIG;
	
	BatteryTestCtrlPara.ClockCount = 0;//ʱ������
	BatteryTestCtrlPara.ClockSlotPast = 0;
		
	BatteryTestCtrlPara.FrameCntPast = 0;

	InitMsgTestCtrl();
	
	InitSleepTestCtrl();

	//BatteryTestCtrlPara.WaitForSleep = FALSE;
#else
#if 0    
    GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_NORMAL_WORKING;    //lq GPS���뿪��״̬

    usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_NORMAL_WORKING!\r\n", GetFrameNum() ,GetSlotNum());	                
#else
    GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_NORMAL_WORKING;    //lq GPS���뿪��״̬

    usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : ATON_GPS_CTRL_STA_NORMAL_WORKING!\r\n", GetFrameNum() ,GetSlotNum());
#endif
#endif
}

///*******************************************************************************
//* ����  : BatteryTestCtrlLowFreq
//* ����  : ��ص������ģʽ���ƺ���
//* ����  : ��		  
//* ���  : ��
//* ����  : ��
//*******************************************************************************/
//void BatteryTestCtrl(void)
//{
//	S32 TempFrameNum;
//	U8 ucTemp[10];
//	
//	if (BatteryTestCtrlPara.CtrlState == BATTERY_TEST_STATE_POWERUP)
//	{
//		if (BatteryTestCtrlPara.GpsPowerOn == FALSE)
//		{
//			IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_HIGH);
//			UartResponseMsg("gps power on");
//
//			LEDInit(LED_TYPE_GPS_FIX);//ģ�ⶨλ
//			
//			BatteryTestSendMsg();//������Ϣ
//			
//			//���Ͷ�ʱ����
//			BatteryTestCtrlPara.TimePeriod = DevStaticPara.SendInterval * BATTERY_TEST_MIN_COUNT;//���ͼ��
//			BatteryTestCtrlPara.TimeCntMessage = SetBatteryTestTimer(BatteryTestCtrlPara.TimePeriod);//������Ϣ���ͼ��
//			
//			//gps������������
//			BatteryTestCtrlPara.TimePeriod = DevStaticPara.GpsPoweronProtectTime * BATTERY_TEST_MIN_COUNT;
//			BatteryTestCtrlPara.TimeCntGps = SetBatteryTestTimer(BatteryTestCtrlPara.TimePeriod); 
//				
//			BatteryTestCtrlPara.GpsPowerOn = TRUE;
//
//			UartResponseMsg("begin battery test");
//		}
//
//		//������Ϣ���Ͷ�ʱ���
//		if (BatteryTestCtrlPara.TimeCntMessage == GetBattertTestTick())
//		{
//			BatteryTestSendMsg();//������Ϣ  ��׼�ź�2
//
//			//�����´η���
//			BatteryTestCtrlPara.TimePeriod = DevStaticPara.SendInterval * BATTERY_TEST_MIN_COUNT;//���ͼ��
//			BatteryTestCtrlPara.TimeCntMessage = SetBatteryTestTimer(BatteryTestCtrlPara.TimePeriod);//������Ϣ���ͼ��
//		}
//		
//		//����gps������λ��������
//		if (BatteryTestCtrlPara.TimeCntGps == GetBattertTestTick())
//		{
//			IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_LOW);//�ر�GPS
//			UartResponseMsg("gps power off");
//			LEDInit(LED_TYPE_NONE);//�ر�LED
//
//			if (TestCtrlPara.ucRFXmtSta == TEST_RFXMT_IDLE)//����������
//			{
//				UartResponseMsg("battery test gps poweron protect finish");	
//						
//				LEDInit(LED_TYPE_NONE);//�ر�LED
//				DevStaticPara.ucLowFreqSta = EQ_FREQSTA_LOW;//�����Ƶģʽ
//				BatteryTestCtrlPara.CtrlState = BATTERY_TEST_STATE_LOWFREQ;//�����Ƶģʽ
//				UartResponseMsg("BatteryTestCtrlPara.CtrlState -> BATTERY_TEST_STATE_LOWFREQ");
//			}
//		}
//		
//		//��Ƶ����ʱ��
//		TempFrameNum = GetFrameNum();
//		if (BatteryTestCtrlPara.FrameCntPast != TempFrameNum)
//		{
//			BatteryTestCtrlPara.FrameCntPast = TempFrameNum;
//			//�洢�Ѿ������˶��
//			sprintf((char *)ucTemp,"%5d",TempFrameNum);
//			Para_Save(PARA_BATTERY_TEST_TIME_TYPE,ucTemp,5);
//		}		
//	}
//	else if (BatteryTestCtrlPara.CtrlState == BATTERY_TEST_STATE_LOWFREQ)
//	{
//		//���������Ҫ����Ϣʱ��
//		if (BatteryTestCtrlPara.TimeCntMessage == GetBattertTestTick())
//		{
//			IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_HIGH);//��GPS
//			UartResponseMsg("gps power on");
//
//			LEDInit(LED_TYPE_GPS_FIX);//ģ�ⶨλ
//
//			UartResponseMsg("battery test wakeup");
//
//			//GPS������ʱ10��
//			BatteryTestCtrlPara.TimePeriod = BATTERY_TEST_GPS_HOT_POWERUP;
//			BatteryTestCtrlPara.TimeCntGps = SetBatteryTestTimer(BatteryTestCtrlPara.TimePeriod);
//
//			//���Ͷ�ʱ����
//			BatteryTestCtrlPara.TimePeriod = DevStaticPara.SendInterval * BATTERY_TEST_MIN_COUNT;//���ͼ��
//			BatteryTestCtrlPara.TimeCntMessage = SetBatteryTestTimer(BatteryTestCtrlPara.TimePeriod);//������Ϣ���ͼ��
//		
//			DevStaticPara.ucLowFreqSta = EQ_FREQSTA_HIG;
//			BatteryTestCtrlPara.CtrlState = BATTERY_TEST_STATE_HIGHFREQ;//�����Ƶģʽ
//			UartResponseMsg("BatteryTestCtrlPara.CtrlState -> BATTERY_TEST_STATE_HIGHFREQ");
//		}
//	}
//
//	else if (BatteryTestCtrlPara.CtrlState == BATTERY_TEST_STATE_HIGHFREQ)
//	{
//		//��Ƶ����ʱ��
//		TempFrameNum = GetFrameNum();
//		if (BatteryTestCtrlPara.FrameCntPast != TempFrameNum)
//		{
//			BatteryTestCtrlPara.FrameCntPast = TempFrameNum;
//			//�洢�Ѿ������˶��
//			sprintf((char *)ucTemp,"%5d",TempFrameNum);
//			Para_Save(PARA_BATTERY_TEST_TIME_TYPE,ucTemp,5);
//		} 
//		
//		if (BatteryTestCtrlPara.TimeCntGps == GetBattertTestTick())//10���gps�������
//		{
//			IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_LOW);//��GPS
//			UartResponseMsg("gps power off");
//			
//			BatteryTestSendMsg();//������Ϣ
//
//			BatteryTestCtrlPara.TimePeriod = 2;//4����
//			BatteryTestCtrlPara.TimeCntWait = SetBatteryTestTimer(BatteryTestCtrlPara.TimePeriod);
//
//			BatteryTestCtrlPara.CtrlState = BATTERY_TEST_STATE_WAIT;//����ȴ�ģʽ
//			UartResponseMsg("BatteryTestCtrlPara.CtrlState -> BATTERY_TEST_STATE_WAIT");
//		}
//	}
//	else if (BatteryTestCtrlPara.CtrlState == BATTERY_TEST_STATE_WAIT)
//	{
//		if (BatteryTestCtrlPara.TimeCntWait == GetBattertTestTick())//ֱ�ӵȴ����ж�
//		{
//			DevStaticPara.ucLowFreqSta = EQ_FREQSTA_LOW;//�����Ƶģʽ
//			BatteryTestCtrlPara.CtrlState = BATTERY_TEST_STATE_LOWFREQ;//�����Ƶģʽ
//			UartResponseMsg("BatteryTestCtrlPara.CtrlState -> BATTERY_TEST_STATE_LOWFREQ");
//		}
//	}
//}		



/*******************************************************************************
* ����  : BatteryTestSendMsg
* ����  : ���ͱ�׼�ź�2
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void BatteryTestSendMsg(void)
{
	LEDInit(LED_TYPE_RF_XMT);
	Testing_ModTest(TEST_XMTTYPE_SIG2);	
	UartResponseMsg("send battery test sig2");
}

/*******************************************************************************
* ����  : BattertTestClock
* ����  : ȫ�ֶ�ʱ�������ܸ�/��ƵӰ��	2�뾫��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void BattertTestClock(void)
{
	S16 TempSlotCnt;
	//S32 TempFrameCnt;

	//TempFrameCnt = GetFrameNum();
	TempSlotCnt = GetSlotNum();

	if (TempSlotCnt > BatteryTestCtrlPara.ClockSlotPast)
	{
		if ((TempSlotCnt - BatteryTestCtrlPara.ClockSlotPast) > BATTERY_TEST_CLOCK_INTERVAL)
		{
			BatteryTestCtrlPara.ClockCount++;
			usart_printf("clock tick %d\r\n",2 * BatteryTestCtrlPara.ClockCount);
			BatteryTestCtrlPara.ClockSlotPast = TempSlotCnt;
		}
	}
	else if (BatteryTestCtrlPara.ClockSlotPast > TempSlotCnt)
	{
		if ((TempSlotCnt + 2250 - BatteryTestCtrlPara.ClockSlotPast) > BATTERY_TEST_CLOCK_INTERVAL)
		{
			BatteryTestCtrlPara.ClockCount++;
			usart_printf("clock tick %d\r\n",2 * BatteryTestCtrlPara.ClockCount);
			BatteryTestCtrlPara.ClockSlotPast = TempSlotCnt;
		}
	}
}		

/*******************************************************************************
* ����  : SetBatteryTestTimer
* ����  : ���ö�ʱ��Ҫ�ȴ���ʱ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U32 SetBatteryTestTimer(U32 period)
{
	return (BatteryTestCtrlPara.ClockCount + period);
}

/*******************************************************************************
* ����  : GetBattertTestTick
* ����  : ��ѯʱ��ֵ
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U32 GetBattertTestTick(void)
{
	return (BatteryTestCtrlPara.ClockCount);
}

//////////////////////////////////////////////////
//��Ϣ���Ϳ���
MsgTestCtrlStruct MsgTestCtrlPara; 

/*******************************************************************************
* ����  : InitMsgTestCtrl
* ����  : ��ʼ����Ϣ���ƽṹ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitMsgTestCtrl(void)
{
	MsgTestCtrlPara.CtrlState = MSG_TEST_STATE_POWERUP;
	MsgTestCtrlPara.TimePeriod = 0;
	MsgTestCtrlPara.SendIntervalTimeCnt = 0;
	MsgTestCtrlPara.GpsPoweronTimeCnt = 0;
	MsgTestCtrlPara.GpsProtectTimeCnt = 0;
	MsgTestCtrlPara.WakeupFlag = FALSE;	
	MsgTestCtrlPara.GpsPowerOn = FALSE;
	MsgTestCtrlPara.GpsProtect = FALSE;
	MsgTestCtrlPara.FrameCntPast = 0;
}

#if 0
/*******************************************************************************
* ����  : MsgTestCtrl
* ����  : �������ģʽ�¿�����Ϣ��ģ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void MsgTestCtrl(void)
{
	S32 TempFrameNum;
	U8 ucTemp[10];
	
	if (MsgTestCtrlPara.CtrlState == MSG_TEST_STATE_POWERUP)
	{
		BatteryTestSendMsg();//������Ϣ
		
		IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_HIGH);//��GPS
		LEDInit(LED_TYPE_GPS_FIX);

		MsgTestCtrlPara.GpsPowerOn = TRUE;

		UartResponseMsg("first gps power on");

		//���Ͷ�ʱ����
		MsgTestCtrlPara.TimePeriod = DevStaticPara.SendInterval * BATTERY_TEST_MIN_COUNT;//���ͼ��
		MsgTestCtrlPara.SendIntervalTimeCnt = SetBatteryTestTimer(MsgTestCtrlPara.TimePeriod);//������Ϣ���ͼ��
		
		//gps������������
		MsgTestCtrlPara.TimePeriod = DevStaticPara.GpsPoweronProtectTime * BATTERY_TEST_MIN_COUNT;
		MsgTestCtrlPara.GpsProtectTimeCnt = SetBatteryTestTimer(MsgTestCtrlPara.TimePeriod); 
			
		MsgTestCtrlPara.GpsPowerOn = TRUE;
		MsgTestCtrlPara.GpsProtect = TRUE;//gps��������	

		MsgTestCtrlPara.CtrlState = MSG_TEST_STATE_WAIT;

		UartResponseMsg("MsgTestCtrlPara.CtrlState -> MSG_TEST_STATE_WAIT");
	}
	else if (MsgTestCtrlPara.CtrlState == MSG_TEST_STATE_WAIT)
	{
		if (MsgTestCtrlPara.SendIntervalTimeCnt == GetBattertTestTick())
		{
			if (MsgTestCtrlPara.GpsPowerOn == FALSE)
			{
				IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_HIGH);//��GPS
				LEDInit(LED_TYPE_GPS_FIX);
				MsgTestCtrlPara.GpsPowerOn = TRUE;

				UartResponseMsg("normal gps power on");
			}
			
			//���Ͷ�ʱ����
			MsgTestCtrlPara.TimePeriod = DevStaticPara.SendInterval * BATTERY_TEST_MIN_COUNT;//���ͼ��
			MsgTestCtrlPara.SendIntervalTimeCnt = SetBatteryTestTimer(MsgTestCtrlPara.TimePeriod);//������Ϣ���ͼ��	

			//GPS������ʱ10��
			MsgTestCtrlPara.TimePeriod = BATTERY_TEST_GPS_HOT_POWERUP;
			MsgTestCtrlPara.GpsPoweronTimeCnt = SetBatteryTestTimer(MsgTestCtrlPara.TimePeriod);

			if (DevStaticPara.ucLowFreqSta == EQ_FREQSTA_LOW)
			{
				MsgTestCtrlPara.WakeupFlag = TRUE;
			}
			
			MsgTestCtrlPara.CtrlState = MSG_TEST_STATE_GPS;
			UartResponseMsg("MsgTestCtrlPara.CtrlState -> MSG_TEST_STATE_GPS");
		}
	}
	else if (MsgTestCtrlPara.CtrlState == MSG_TEST_STATE_GPS)
	{
		//��Ƶ����ʱ��
		TempFrameNum = GetFrameNum();
		if (MsgTestCtrlPara.FrameCntPast != TempFrameNum)
		{
			MsgTestCtrlPara.FrameCntPast = TempFrameNum;
			//�洢�Ѿ������˶��
#if CONFIG_PARASTORAGE_BTT
			sprintf((char *)ucTemp,"%5d",TempFrameNum);
			Para_Save(PARA_BATTERY_TEST_TIME_TYPE,ucTemp,5);
#endif
		} 
		
		if (MsgTestCtrlPara.GpsPoweronTimeCnt == GetBattertTestTick())//10�뵽��
		{
			if (MsgTestCtrlPara.GpsProtect == FALSE)//�������GPS��������
			{	
				IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_LOW);//��GPS
				MsgTestCtrlPara.GpsPowerOn = FALSE;	

				UartResponseMsg("normal gps power off");
			}

			BatteryTestSendMsg();//������Ϣ
			
			MsgTestCtrlPara.CtrlState = MSG_TEST_STATE_WAIT;
		}
	}

	if (MsgTestCtrlPara.GpsProtect == TRUE)
	{
		if (MsgTestCtrlPara.GpsProtectTimeCnt == GetBattertTestTick())//����ʱ�䵽
		{
			MsgTestCtrlPara.GpsProtect = FALSE;

			IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_LOW);//��GPS
			MsgTestCtrlPara.GpsPowerOn = FALSE;
			
			//LEDInit(LED_TYPE_NONE);//�ر�LED
			UartResponseMsg("protect gps power off");
		}
	}
}	
#endif

/*******************************************************************************
* ����  : GetGpsTestCtrlWakeupFlag
* ����  : �鿴�Ƿ�GPSTESTҪ����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetMsgTestCtrlWakeupFlag(void)
{
	if (MsgTestCtrlPara.WakeupFlag == TRUE)
	{
		MsgTestCtrlPara.WakeupFlag = FALSE;
		return TRUE;	
	}
	else
	{
		return FALSE;
	}	
}


////////////////////////////////////////////
//���߿��Ʋ���
SleepTestCtrlStruct SleepTestCtrlPara;

void InitSleepTestCtrl(void)
{
	SleepTestCtrlPara.CtrlState = SLEEP_TEST_STATE_IDEL;
	SleepTestCtrlPara.TimePeriod = 0;
	SleepTestCtrlPara.TimeCnt = 0;
	SleepTestCtrlPara.WaitCount = 0;
}

/*******************************************************************************
* ����  : SleepTestCtrl
* ����  : �������ģʽ�����߿���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SleepTestCtrl(void)
{
	U8 ucPowerKeyVal;
		
	if (SleepTestCtrlPara.CtrlState == SLEEP_TEST_STATE_IDEL)
	{
#if CONFIG_KEY     
		ucPowerKeyVal = ReadSingleKeyValProcess(&PowerKey);//��ǰ����״̬
#endif		
		if (MsgTestCtrlPara.GpsPowerOn == FALSE 
			&& TestCtrlPara.ucRFXmtSta == TEST_RFXMT_IDLE
#if CONFIG_KEY     
			&& ucPowerKeyVal == KEY_HIGH
#endif             
			)//�������
		{
			UartResponseMsg("sleep condition confirm");
			SleepTestCtrlPara.TimePeriod = SLEEP_PRT_PERIOD;
			SleepTestCtrlPara.TimeCnt = SetTimePeriodProcess(SleepTestCtrlPara.TimePeriod);
			SleepTestCtrlPara.WaitCount = 0;
			SleepTestCtrlPara.CtrlState = SLEEP_TEST_STATE_STEP1;
		}
	}
	else if (SleepTestCtrlPara.CtrlState == SLEEP_TEST_STATE_STEP1)
	{
		if (MsgTestCtrlPara.GpsPowerOn != FALSE || TestCtrlPara.ucRFXmtSta != TEST_RFXMT_IDLE)
		{
			SleepTestCtrlPara.CtrlState = SLEEP_TEST_STATE_IDEL;
			UartResponseMsg("back to awake");
		}
		else
		{
			if (SleepTestCtrlPara.TimeCnt == GetLocalClockTickProcess())
			{
				SleepTestCtrlPara.TimeCnt = SetTimePeriodProcess(SleepTestCtrlPara.TimePeriod);//�����ʱ10ms������
				SleepTestCtrlPara.WaitCount++;	
			}

			if (SleepTestCtrlPara.WaitCount >= SLEEP_1S_PRTTIME)
			{
				UartResponseMsg("sleep");
				LEDInit(LED_TYPE_NONE);//�ر�LED
//lq                TEST_OUT_LOW();   //lq ���ص�Դ��Ϊ������ģʽ
				DevDynamicPara.ucLowFreqSta = EQ_FREQSTA_LOW;
				SleepTestCtrlPara.CtrlState = SLEEP_TEST_STATE_IDEL;
			}
		}
	}
	
	RtcClockChoice();//����RTCʱ��Դ��ѡ��					
}

/*******************************************************************************
* ����  : InitMsgTestPara
* ����  : ��ʼ����Ϣ����ģ�����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitMsgTestPara(void)
{
    struMsgTestPara.ucPowerDownEn = FALSE;
}

/*******************************************************************************
* ����  : InitMsgTest
* ����  : ��ʼ����Ϣ����ģ�����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitMsgTest(void)
{
#if 0
	ResetTimeCount();

    InitMsgTestPara();

    IniGpsPara();
	InitGpsCell();

	InitaDigDeal();
	InitFSKDataTranPara();
	InitaLinkPtlPortPara();                                                     //lq ��ʼ����Ϣ���У�δ����֮ǰ��
    InitFSKDataXmtFIFO(&gstruFSKDataXmtFIFO); 

	LEDInit(LED_TYPE_NONE);

	Ptl_Init();

    InitSOTDMAPara();

    ResetFSKDataXmtEn();
    ResetFetchFirstFskDataEn();

	DevStaticPara.ucLowFreqSta = EQ_FREQSTA_HIG;
#endif
}

/*******************************************************************************
* ����  : DigDealMain_MsgTest
* ����  : ��Ϣ��������£���Ϣ���齨�����ͺ͵���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void DigDealMain_MsgTest(void)
{
#if 0
	/*lq ������Ϣ���齨�ͷ���*/
    Ptl_Main_Test();                                                                 //lq �ӽ������GPS��Ϣ����ȡ��Ϣ����x����Ϣ��������������Ϣ����Э��ջ

    /*lq ���Ʒ��Ͷ����е���Ϣ*/
	DigDealUnit();
#endif
}
