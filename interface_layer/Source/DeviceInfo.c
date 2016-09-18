/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: DeviceInfo.c
  Author: lq       Version : V1.0.0        Date: 2015.05.07
  Description:     �豸��Ϣģ��
  Version:         V1.0.0
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "Include.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/
DevStaticInfoStruct DevStaticPara;
DevDynamicInfoStruct DevDynamicPara;

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* ����  : DEVINFO_DevStaticInfoStructInit
* ����  : DevStaticInfoStruct�ṹ���ʼ��
* ����  : pDevInfoStruct - ָ��DevStaticInfoStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void DEVINFO_DevStaticInfoStructInit(DevStaticInfoStruct *pDevStaticInfoStruct)
{
	pDevStaticInfoStruct->uiFreqAdj = DEV_INFO_FREQADJ_DEFAULT;
	pDevStaticInfoStruct->uiFreqOffset =
        (U16)(((U32)pDevStaticInfoStruct->uiFreqAdj * EQ_MOD_OFFSET ) >> 5);
	pDevStaticInfoStruct->uiModAmpAdj = DEV_INFO_MODAMP_DEFAULT;
	pDevStaticInfoStruct->uiModAmp =
        (U16)(((U32)pDevStaticInfoStruct->uiModAmpAdj * EQ_MOD_AMP + 16) >> 5);
	pDevStaticInfoStruct->uiXmtFreq = EQ_XMTFREQ_CHA;
    pDevStaticInfoStruct->slXmtFreqComp = 0;
    pDevStaticInfoStruct->GridVoltAdj = DEV_INFO_RF_VOLT_ADJUST_DEFAULT;
    pDevStaticInfoStruct->GridVoltOffset =
        (pDevStaticInfoStruct->GridVoltAdj - 10) * DA_VALUE_01V;

	pDevStaticInfoStruct->AisClass = DEV_INFO_AIS_CLASS_DEFAULT;
    pDevStaticInfoStruct->SerialNumber = 0;
}

/*******************************************************************************
* ����  : DEVINFO_DevDynamicInfoStructInit
* ����  : DevDynamicInfoStruct�ṹ���ʼ��
* ����  : pDevDynamicInfoStruct - ָ��DevDynamicInfoStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void DEVINFO_DevDynamicInfoStructInit(DevDynamicInfoStruct *pDevDynamicInfoStruct)
{
	S16 i;

    if (ucStandbyWakeupFlg == TRUE)
    {
        pDevDynamicInfoStruct->ucLowFreqSta = EQ_FREQSTA_LOW;	//��������      
    }
    else
    {
        pDevDynamicInfoStruct->ucLowFreqSta = EQ_FREQSTA_HIG;	//����������
    }
	pDevDynamicInfoStruct->ucWorkMode = EQ_WORKMOD_NORMAL;
    pDevDynamicInfoStruct->TestSta = EQ_MOD_TEST_STA_MAIN;
	pDevDynamicInfoStruct->ucGpsOutput = TRUE;
    pDevDynamicInfoStruct->GpsOutputInterval[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_RMC] = 1;
    pDevDynamicInfoStruct->GpsOutputInterval[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_VTG] = 0;
    pDevDynamicInfoStruct->GpsOutputInterval[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GGA] = 0;
    pDevDynamicInfoStruct->GpsOutputInterval[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSA] = 0;
    pDevDynamicInfoStruct->GpsOutputInterval[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSV] = 0;
    pDevDynamicInfoStruct->GpsOutputInterval[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GLL] = 0;
    pDevDynamicInfoStruct->GpsOutputInterval[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GBS] = 1;
    for (i = 0; i < DEV_INFO_GPS_OUTPUT_INTERVAL_BUFF_LEN; i++)
    {
        pDevDynamicInfoStruct->GpsOutputCnt[i] = -1;
    }
    pDevDynamicInfoStruct->OpenCloseTimeIndex = 0;

}

/*******************************************************************************
* ����  : DEVINFO_ResetGpsOutputCnt
* ����  : ��λGPS�������
* ����  : pDevDynamicInfoStruct - ָ��DevDynamicInfoStruct�ṹ���ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void DEVINFO_ResetGpsOutputCnt(DevDynamicInfoStruct *pDevDynamicInfoStruct)
{
	S16 i;

    for (i = 0; i < DEV_INFO_GPS_OUTPUT_INTERVAL_BUFF_LEN; i++)
    {
        pDevDynamicInfoStruct->GpsOutputCnt[i] = -1;
    }
}

/*******************************************************************************
* ����  : DEVINFO_InstResetGpsOutputCnt
* ����  : ʵ����λGPS�������
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void DEVINFO_InstResetGpsOutputCnt(void)
{
    DEVINFO_ResetGpsOutputCnt(&DevDynamicPara);
}

/*******************************************************************************
* ����  : DEVINFO_ModuleInit
* ����  : ģ���ʼ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void DEVINFO_ModuleInit(void) 
{
    DEVINFO_DevStaticInfoStructInit(&DevStaticPara);
    DEVINFO_DevDynamicInfoStructInit(&DevDynamicPara);
}
