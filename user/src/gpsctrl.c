/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: gpsmsg.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    GPS���Ĵ���ģ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>

*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "def.h"
#include "initial.h"
#if !WIN32
#include "debug.h"
#include "common.h"
#include "ioctrl.h"
#endif
#include "gpsctrl.h"
#include "uartmsg.h"
#include "systemtick.h"
#include "gps.h"
#include "ledctrl.h"
#include "mmi.h"
#include "parastorage.h"
#include "adjust.h"
#include "AIS_MsgModu.h"
#include "LED.h"
#include "Tim.h"
#include "SOTDMA.h"
#include "powerup.h"
#include "tempersensor.h"
#include "config.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
GPSStruct 		GpsPara;
GPSCtrlStruct 	GpsCtrlPara;
GPSSynStruct 	GpsSynCell;	   		//GPSͬ�������ṹ��
GPSCtrlMsgTestStruct GPSCtrlMsgTestPara;
GpsSelfCheckStruct GpsSelfCheckInst; 
U16 usGPSSynContinueTime = 61;		   //����GPS��λ��ʱ����
double LastGPS_Latitude;				   //��һ��GPS��γ��
double LastGPS_Longitud;				   //��һ��GPS������
double NowGPS_Latitude;				   //����GPS��γ��
double NowGPS_Longitud;				   //����GPS������

/* Private function prototypes -----------------------------------------------*/
//U8 gps_msg_enable;
void SlotSynClac(void);

/*******************************************************************************
* ����  : IniGpsCtrlMsgTestStru
* ����  : ��ʼ����Ϣ����ģʽ��GPS���ƽṹ�����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void IniGpsCtrlMsgTestStru(GPSCtrlMsgTestStruct *pGPSCtrlMsgTestCell)
{
    pGPSCtrlMsgTestCell->TimeCnt = 0;
    pGPSCtrlMsgTestCell->TimePeriod = GPS_CTRL_1MINPEROID;
    pGPSCtrlMsgTestCell->Time1mCnt = 0;
    pGPSCtrlMsgTestCell->GpsPowerupFix = FALSE;
    pGPSCtrlMsgTestCell->GpsCtrlSta = GPS_CTRL_STA_MSG_TEST_START;
}

/*******************************************************************************
* ����  : IniGpsMsgStru
* ����  : ��ʼ��GPS����ͨ�ű�Ҫ�Ĳ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void IniGpsMsgStru(GPSStruct *pGpsCell)
{
	U8 i;

	for(i = 0; i < 6; i++)
	{
		pGpsCell->GPSTimeBuf[i] = '0';
	}

	//lq pGpsCell->GPS_Cog = 0;
    pGpsCell->GPS_Cog = 0xE10;                                                  //lq 360��        
	pGpsCell->GPS_Lati100 = 0;
	//lq pGpsCell->GPS_Latitude = 0;
	pGpsCell->GPS_Latitude = 0x3412140;                                         //lq 91��
	pGpsCell->GPS_Long100 = 0;
	//lq pGpsCell->GPS_Longitud = 0;
	pGpsCell->GPS_Longitud = 0x6791AC0;                                         //lq 181��
	pGpsCell->GPS_LatiD = 0;
	//lq pGpsCell->GPS_Sog = 0;
    pGpsCell->GPS_Sog = 0x3FF;                                                  //lq 102.3knot            
	pGpsCell->GPS_Stat = 0;
	pGpsCell->GPS_TimeM = 0;
	pGpsCell->GPS_TimeS = 0;
	pGpsCell->GPS_errlat = GPS_OUTRANGE;
	pGpsCell->GPS_errlon = GPS_OUTRANGE;
	pGpsCell->GPS_PosPrec = GPS_OUTRANGE;
	pGpsCell->GPS_QualityIndicator = 0;
    pGpsCell->GPS_FixMode = 1;
    pGpsCell->GPS_PDOP = GPS_DATA_PDOP_DEFAULT;
    pGpsCell->GPS_RAIMFlag = FALSE;
    pGpsCell->GPS_Sog_Index = 0;

	for(i=0; i<18; i++)
	{
		pGpsCell->GPS_Sog_Buf[i] = 0;
	}
	for (i=0; i<4; i++)
	{
		pGpsCell->GPS_Longitud_Buf[i] = pGpsCell->GPS_Longitud >> 2;
		pGpsCell->GPS_Latitude_Buf[i] = pGpsCell->GPS_Latitude >> 2;
	}
	pGpsCell->GPS_Longitud_Buf_Index = 0;
	pGpsCell->GPS_Latitude_Buf_Index = 0;

    pGpsCell->GPS_LonLatFilterFlg = FALSE;                                      
	pGpsCell->GPS_LatLonCnt = 0;

	pGpsCell->GPS_Latitude_Last = 0;
	pGpsCell->GPS_Longitud_Last = 0;
	pGpsCell->GPS_TimeD_Last = 0;
	pGpsCell->GPS_TimeH_Last = 0;
	pGpsCell->GPS_TimeM_Last = 60;
    pGpsCell->GPS_TimeS_Last = 0;
    pGpsCell->GPS_Sog_Last = 0;   
    pGpsCell->GPS_FirstLLFlg = FALSE; 
    
	pGpsCell->GPS_LeapSecondFlg = FALSE;
    pGpsCell->GPS_TimeHLeapLast = 60;
	pGpsCell->GPS_TimeMLeapLast = 60;
	pGpsCell->GPS_TimeSLeapLast = 60;    		    	
    pGpsCell->GPS_TimeSLeap = 60;

    pGpsCell->GPS_PPSGenFlag = FALSE;
    pGpsCell->GPS_MsgGenFlag = FALSE;
}

/*******************************************************************************
* ����  : ResetGpsMsgStru
* ����  : ��ʼ��GPS����ͨ�ű�Ҫ�Ĳ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void ResetGpsMsgStru(GPSStruct *pGpsCell)
{
	U8 i;
	
	pGpsCell->GPS_Stat = 0;
	pGpsCell->GPS_errlat = GPS_OUTRANGE;
	pGpsCell->GPS_errlon = GPS_OUTRANGE;
	pGpsCell->GPS_PosPrec = GPS_OUTRANGE;
	pGpsCell->GPS_QualityIndicator = 0;
    pGpsCell->GPS_FixMode = 1; 
    pGpsCell->GPS_PDOP = GPS_DATA_PDOP_DEFAULT;       
    pGpsCell->GPS_RAIMFlag = FALSE;
    
	pGpsCell->GPS_Sog_Index = 0;

	for(i=0; i<18; i++)
	{
		pGpsCell->GPS_Sog_Buf[i] = 0;
	}

    pGpsCell->GPS_LonLatFilterFlg = FALSE;                                         
	pGpsCell->GPS_LatLonCnt = 0;

    pGpsCell->GPS_PPSGenFlag = FALSE;
    pGpsCell->GPS_MsgGenFlag = FALSE;
}

/*******************************************************************************
* ����  : InitGpsSynStru
* ����  : ��ʼ��GPSͬ������
* ����  : GPSSynStruct����ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitGpsSynStru(GPSSynStruct *pGpsSynStru)
{
	pGpsSynStru->GPSSyn_PulsePhase = 0;	//���յ�������ʱ��ʱ϶��λ
	pGpsSynStru->GPSSyn_PhaseOffset = 0;	//ʱ϶ͬ��ʱ��λƫ��
    pGpsSynStru->GPSSyn_PulseSlot = 0;
	pGpsSynStru->GPSSyn_SlotOffset = 0;	//ʱ϶ͬ��ʱ϶ƫ����
	pGpsSynStru->GPSSyn_SynLevel = 0;		//ͬ�����ȼ�
	pGpsSynStru->GPSSyn_PhaseSynFlag = FALSE;	//��λͬ����־
	pGpsSynStru->GPSSyn_SlotSynFlag = FALSE;	//��λͬ����־
	pGpsSynStru->GPSSyn_ProtectCnt = 0;	//��λͬ������ʱ��
	pGpsSynStru->GPSSyn_PulseEn = FALSE;		//��������Ч��־
	pGpsSynStru->GPSSyn_Flag = FALSE;			//GPSͬ����Ч��־
	pGpsSynStru->GPSSyn_SlotCnt = 0;
	pGpsSynStru->GPSSyn_SlotEn = FALSE;
	pGpsSynStru->GPSSyn_PulseMin = 0;
	pGpsSynStru->GPSSyn_MinOffset = 0;

	pGpsSynStru->ChkTimeS = 0;
	pGpsSynStru->ChkTimeM = 0;
	pGpsSynStru->ChkTimeH = 0;
    pGpsSynStru->ChkTimeFrame = 0;
    pGpsSynStru->ucGPSSyn_SlotPhaseSynFlag = FALSE;
    //lq pGpsSynStru->GPSSyn_FirstSynFlag = FALSE; 
    pGpsSynStru->GPSSyn_Enable = TRUE;
#if CONFIG_GPS_SYNCH_HOUR_MIN   
    pGpsSynStru->GPSSyn_HourFrameSynEnable = TRUE;
    pGpsSynStru->ucGPSSyn_HourFrameSynFlag = FALSE;                                              //lq Сʱ������ͬ��
	pGpsSynStru->GPSSyn_PulseHour = 0;    
    pGpsSynStru->GPSSyn_HourOffset = 0;	//lq Сʱƫ��
#endif
}	

/*******************************************************************************
* ����  : IniGpsCtrlStru
* ����  : ��ʼ�����Ʋ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void IniGpsCtrlStru(GPSCtrlStruct *pGpsCtrlCell)
{
	pGpsCtrlCell->TimePeriod = GPS_CTRL_TIME_PERIOD_1S;
	pGpsCtrlCell->TimeCnt = SetTimePeriodProcess(pGpsCtrlCell->TimePeriod);
	pGpsCtrlCell->Timeout = 0;	
	pGpsCtrlCell->TimeoutCnt = 0;	
	pGpsCtrlCell->Time1mCnt = 0;

    pGpsCtrlCell->GpsUnfix1hPeriodTimSlot = 0;
    pGpsCtrlCell->GpsUnfix1hPeriodTimFrame = 0;

    pGpsCtrlCell->GpsOnTimSlot = 0;
    pGpsCtrlCell->GpsOnTimFrame = 0; 
    pGpsCtrlCell->GpsOffTimSlot = 0;
    pGpsCtrlCell->GpsOffTimFrame = 0;    
    pGpsCtrlCell->ucGpsOnTimEn = FALSE;
    pGpsCtrlCell->ucGpsOffTimEn = FALSE;

	pGpsCtrlCell->GpsPowerSta = GPS_CTRL_GPSPOWERDOWN;
	pGpsCtrlCell->GpsCtrlSta = GPS_CTRL_STA_START;
	pGpsCtrlCell->GPSSyn_Flag = FALSE;

	pGpsCtrlCell->GpsPowerupFix = FALSE;
	pGpsCtrlCell->GpsNormalFix = FALSE;
	pGpsCtrlCell->SaveOpenTime = FALSE;

	pGpsCtrlCell->GpsSynCount = 0;

	pGpsCtrlCell->GpsFixCount = 0;
	pGpsCtrlCell->GpsFallCount = 0;
	pGpsCtrlCell->GpsWaitCount = 0;
	pGpsCtrlCell->GpsWaitFix = FALSE;

    pGpsCtrlCell->ucGpsUnfix1hLngOpenFlg = FALSE;
    pGpsCtrlCell->ucGpsUnfixBey1hLngOpenFlg = FALSE;
    pGpsCtrlCell->ucGps5minLngOpenEn = TRUE;
    pGpsCtrlCell->ssGps5minLngOpenCnt = 0;
    pGpsCtrlCell->GpsIntervalCnt = 0;    

    pGpsCtrlCell->ucGpsSaveFlg = FALSE;
    pGpsCtrlCell->ucGpsOnEn = TRUE;
    pGpsCtrlCell->GpsSaveJudgeTimePeriod = GPS_SAVE_JUDGE_PERIOD;
    pGpsCtrlCell->GpsSaveJudgeTimeCnt = SetTimePeriodProcess(pGpsCtrlCell->GpsSaveJudgeTimePeriod);
	pGpsCtrlCell->GpsSaveJudgeCntWorkTime = 0;
	pGpsCtrlCell->GpsSaveJudgeCntBatVolt = 0;	
	pGpsCtrlCell->GpsSaveJudgeCntTemper = 0;
	pGpsCtrlCell->GpsSaveJudgeFlgWorkTime = FALSE;
	pGpsCtrlCell->GpsSaveJudgeFlgBatVolt = FALSE;	
	pGpsCtrlCell->GpsSaveJudgeCFlgTemper = FALSE;

    pGpsCtrlCell->GpsOffPositionSampleEn = TRUE;
    pGpsCtrlCell->GpsPositionSampleNewFlg = FALSE;
}

/*******************************************************************************
* ����  : IniGpsCtrlStru
* ����  : ��ʼ�����Ʋ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void ResetGpsCtrlStru(GPSCtrlStruct *pGpsCtrlCell)
{
	pGpsCtrlCell->TimeCnt = 0;
	pGpsCtrlCell->TimePeriod = GPS_CTRL_TIME_PERIOD_1S;
	pGpsCtrlCell->Timeout = 0;	
	pGpsCtrlCell->TimeoutCnt = 0;	
	pGpsCtrlCell->Time1mCnt = 0;

	pGpsCtrlCell->GpsPowerSta = GPS_CTRL_GPSPOWERUP;

	pGpsCtrlCell->GPSSyn_Flag = FALSE;
	pGpsCtrlCell->GpsNormalFix = FALSE;
    pGpsCtrlCell->ucGpsOnTimEn = FALSE;

	pGpsCtrlCell->GpsSynCount = 0;

    //lq SetGpsOffPositionSampleEn();
    pGpsCtrlCell->GpsOffPositionSampleEn = TRUE;
}

/*******************************************************************************
* ����  : IniGpsPara
* ����  : ��ʼ��GPSͨ�ű�Ҫ�Ĳ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void IniGpsPara(void)
{
	IniGpsMsgStru(&GpsPara);
	IniGpsCtrlStru(&GpsCtrlPara);
    InitGpsSynStru(&GpsSynCell);

#if !WIN32
    /*lq ���ϵ翪��ʱִ��*/
    if (ucStandbyWakeupFlg == FALSE)
    {    
	    IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_ON);                                     //lq ������������GPS����
        GpsCtrlPara.GpsPowerSta = GPS_CTRL_GPSPOWERUP;
    }
#endif
}

//lqdb U8 ucGpsSecFlg = FALSE;
/*******************************************************************************
* ����  : DealGpsMsg
* ����  : GPS���Ľ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 DealGpsMsg(GPSStruct *pGpsCell, U8 *pDat, U16 uiLen)
{
	U8 ucCheck;
	char chTemp[30];
	U16 uiTempH, uiTempL;
	U32 ulTempH;
	U32 ulMax, ulMin;
	U16 i;
	float flTmp;
	S32 sTemp;
	S32 sTempLon, sTempLat, sTempLon_EW, sTempLat_NS;
    U8  ucLLValid;
    S8  scNS, scEW;
    U16 usLatBuffIndex, usLonBuffIndex; 
    double latitude_D, longitud_D, latitudeLast_D, longitudLast_D, distance;
    U32  ulTemp;
	U8 	ucLen;
    char *pEnd;

    ucLLValid = TRUE;

	//����RMC���
	if (strncmp((char *)pDat, "GPRMC", 5) == FALSE)
	{
        GpsSelfCheckInst.DataCommFlg = TRUE;

        /*lq GPS��Ϣ����*/
        GpsPara.GPS_MsgGenFlag = TRUE;
		
        ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);

		if (ucCheck == FALSE)
		{
			return (FALSE);
		}

		pGpsCell->GPS_Stat = chTemp[0];

		//GPS������Ч
		if (chTemp[0] == 'A')                                     //lq ֻ��GPS��λ�󣬲Ÿ���GPS��GPRMC���ĸ���GPS�ṹ��
		{
			//��ȡʱ��
			ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);

			if (ucCheck == FALSE)
			{
				//return (FALSE);
			}
			else
			{
				//��ȡСʱ
				pGpsCell->GPS_TimeH = (chTemp[0] - '0') * 10 + (chTemp[1] - '0');
				
				//��ȡ����
				pGpsCell->GPS_TimeM = (chTemp[2] - '0') * 10 + (chTemp[3] - '0');
				
				//��ȡ��
				pGpsCell->GPS_TimeS = (chTemp[4] - '0') * 10 + (chTemp[5] - '0');

				/*lq ����ʱ�������ж��Ƿ��������*/
                if (pGpsCell->GPS_LeapSecondFlg == FALSE)
                {
    				/*lq ������ֱ�־*/
                    if (pGpsCell->GPS_TimeHLeapLast == pGpsCell->GPS_TimeH
                        && pGpsCell->GPS_TimeMLeapLast == pGpsCell->GPS_TimeM
                        && pGpsCell->GPS_TimeSLeapLast == pGpsCell->GPS_TimeS)
                    {
                        pGpsCell->GPS_LeapSecondFlg = TRUE;
                        
                        /*lq ������ֺ�ʹ��GPS�ṩ��ʱ�����ͬ��*/
                        pGpsCell->GPS_TimeSLeap = pGpsCell->GPS_TimeS;
                    }
                    else
                    {
                        pGpsCell->GPS_TimeHLeapLast = pGpsCell->GPS_TimeH;
        				pGpsCell->GPS_TimeMLeapLast = pGpsCell->GPS_TimeM;
        				pGpsCell->GPS_TimeSLeapLast = pGpsCell->GPS_TimeS;

                        /*lq ���������֮ǰ����Ϊ��1s����ʹ���������ֵ����ͬ��*/
                        pGpsCell->GPS_TimeSLeap = pGpsCell->GPS_TimeS == 0 ? 59 : pGpsCell->GPS_TimeS - 1;
                    }
                }
                else
                {
                    /*lq ������ֺ�ʹ��GPS�ṩ��ʱ�����ͬ��*/
                    pGpsCell->GPS_TimeSLeap = pGpsCell->GPS_TimeS;
                }

#if 0 //lqdb
                if (ucGpsSecFlg == TRUE)
                {
                    TEST_OUT_LOW_2();
                    ucGpsSecFlg = FALSE;
                }
                else
                {
                    TEST_OUT_HIGH_2();
                    ucGpsSecFlg = TRUE;
                }
#endif
//lq                SlotSynClac();
			}

			//��ȡγ��
			ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

			if (ucCheck == FALSE)
			{
				//return (FALSE);
                ucLLValid = FALSE;
			}
			else
			{
				uiTempH = (chTemp[0] - '0') * 10 + (chTemp[1] - '0');
				uiTempL = (chTemp[2] - '0') * 10 + (chTemp[3] - '0');			
	
				pGpsCell->GPS_LatiD = uiTempH;
	
				ulTempH = (U32)uiTempH * 60 + uiTempL;
	
				uiTempL = (chTemp[5] - '0') * 1000;
				uiTempL += (chTemp[6] - '0') * 100;
				uiTempL += (chTemp[7] - '0') * 10;
				uiTempL += (chTemp[8] - '0');
	
	            sTempLat = ulTempH * 10000 + uiTempL;
             }

			ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);

			if (ucCheck == FALSE)
			{
				//return (FALSE);
                ucLLValid = FALSE;
			}
			else
			{
				if(chTemp[0] == 'S')
				{
					scNS = -1;
                    sTempLat_NS = -sTempLat;
				}
                else
                {
					scNS = 1;
                    sTempLat_NS = sTempLat;
                }
			}

			ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);

			if (ucCheck == FALSE)
			{
				//return (FALSE);
                ucLLValid = FALSE;
			}
			else
			{
				uiTempH = ((U16)(chTemp[0] - '0')) * 100 + 
								(chTemp[1] - '0') * 10 +
								(chTemp[2] - '0');
	
				uiTempL = (chTemp[3] - '0') * 10 + (chTemp[4] - '0');
				ulTempH = (U32)uiTempH * 60 + uiTempL;

				uiTempL = (chTemp[6] - '0') * 1000;
				uiTempL += (chTemp[7] - '0') * 100;
				uiTempL += (chTemp[8] - '0') * 10;
				uiTempL += (chTemp[9] - '0');
	
				sTempLon = ulTempH * 10000 + uiTempL;
            }

			ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 30);

			if (ucCheck == FALSE)
			{
				//return (FALSE);
                ucLLValid = FALSE;
			}
			else
			{
				if(chTemp[0] == 'W')
				{
					scEW = -1;
                    sTempLon_EW = -sTempLon;
				}
                else
                {
					scEW = 1;
                    sTempLon_EW = sTempLon;
                }
			}

            /*lq ��γ�����ݴ���*/
            if (ucLLValid == TRUE)
            {
    			if (pGpsCell->GPS_LonLatFilterFlg == TRUE)
                {
            		latitude_D = (double)sTempLat_NS / 600000.0;
            		longitud_D = (double)sTempLon_EW / 600000.0;
            	 	latitudeLast_D = (double)pGpsCell->GPS_Latitude / 600000.0;
            		longitudLast_D = (double)pGpsCell->GPS_Longitud / 600000.0;
            		
            		distance = GetDistance(latitude_D, longitud_D, 
                                           latitudeLast_D, longitudLast_D); 
                                                           
                    /*lq λ��ͻ��������ͻ�����С����ֵ20�ף�����Ϊ������Ч������ͻ�����������ֵ��Ϊ������Ч*/
                    if (distance > 20 && pGpsCell->GPS_LatLonCnt < GPS_POSITION_BUFF_LENGTH)                                              //lq ������Χ�����ݰ���Ч����
                    {
                        pGpsCell->GPS_LatLonCnt++;
    
                        /*lq ʹ���ϴεĶ�λ����*/
                        usLatBuffIndex = pGpsCell->GPS_Latitude_Buf_Index - 1 < 0
                                         ? GPS_POSITION_BUFF_LENGTH - 1 : pGpsCell->GPS_Latitude_Buf_Index - 1;
                        sTempLat = pGpsCell->GPS_Latitude_Buf[usLatBuffIndex];
    
                        usLonBuffIndex = pGpsCell->GPS_Longitud_Buf_Index - 1 < 0
                                         ? GPS_POSITION_BUFF_LENGTH - 1 : pGpsCell->GPS_Longitud_Buf_Index - 1;
                        sTempLon = pGpsCell->GPS_Longitud_Buf[usLonBuffIndex];
                    }
                    else
                    {
                        pGpsCell->GPS_LatLonCnt = 0;
    
                        /*lq ʹ�ñ��εĶ�λ����*/
                        sTempLat = sTempLat >> 2;
                        sTempLon = sTempLon >> 2;                
                    }
                }
                else
                {
                    pGpsCell->GPS_LatLonCnt++;
                    if (pGpsCell->GPS_LatLonCnt >= GPS_POSITION_BUFF_LENGTH)
                    {
                        pGpsCell->GPS_LatLonCnt = 0;
                        pGpsCell->GPS_LonLatFilterFlg = TRUE;
                    }

                    /*lq ʹ�ñ��εĶ�λ����*/
                    sTempLat = sTempLat >> 2;
                    sTempLon = sTempLon >> 2;   
                }
    			
                /*lq γ�ȴ���*/	
                //��ȥҪ����������
    			pGpsCell->GPS_Latitude = pGpsCell->GPS_Latitude - 
                                         pGpsCell->GPS_Latitude_Buf[pGpsCell->GPS_Latitude_Buf_Index];
    			
                //����Ҫ���������
    			pGpsCell->GPS_Latitude_Buf[pGpsCell->GPS_Latitude_Buf_Index++] = sTempLat;
    
    			pGpsCell->GPS_Latitude += sTempLat;
    
    			if (pGpsCell->GPS_Latitude_Buf_Index > (GPS_POSITION_BUFF_LENGTH - 1))
    			{
    				pGpsCell->GPS_Latitude_Buf_Index = 0;
    			}
    //lq			uiTempL = chTemp[5] - '0';
    //lq			pGpsCell->GPS_Lati100 = ulTempH * 10 + uiTempL;
                pGpsCell->GPS_Latitude *= scNS;
                 
    			/*lq ���ȴ���*/
                //��ȥҪ����������
    			pGpsCell->GPS_Longitud = pGpsCell->GPS_Longitud - 
                                         pGpsCell->GPS_Longitud_Buf[pGpsCell->GPS_Longitud_Buf_Index];
    			
                //����Ҫ���������
    			pGpsCell->GPS_Longitud_Buf[pGpsCell->GPS_Longitud_Buf_Index++] = sTempLon;
    
    			pGpsCell->GPS_Longitud += sTempLon;
    
    			if (pGpsCell->GPS_Longitud_Buf_Index > (GPS_POSITION_BUFF_LENGTH - 1))
    			{
    				pGpsCell->GPS_Longitud_Buf_Index = 0;
    			}
    //lq			uiTempL = chTemp[6] - '0';
    //lq			pGpsCell->GPS_Long100 = ulTempH * 10 + uiTempL;
    			pGpsCell->GPS_Longitud *= scEW;

                /*lq λ��ƫ�������Ч */
                SetGpsPositionSampleNewFlg();
            }

			//�Եغ���ת��
			ucCheck = GetSegment((char *)pDat, chTemp, 7, uiLen, 30);

			if (ucCheck == FALSE)
			{
				//return (FALSE);
			}
			else
			{
				flTmp = (float)atof(chTemp);
	
				pGpsCell->GPS_Sog = (U32)(flTmp * 10);                          //lq ʵ�ʺ��ٵ�10���������ͱ�Ϊ���ͣ�
	
				//GPS sog��ƽ���㷨�������ٶ�ͻ��
				pGpsCell->GPS_Sog_Buf[pGpsCell->GPS_Sog_Index++] = pGpsCell->GPS_Sog;
	
				if(pGpsCell->GPS_Sog_Index >= 18)
				{
					pGpsCell->GPS_Sog_Index = 0;
				}
	
				ulMax = 0;
				ulMin = 0x0FFFFFFF;
				ulTempH = 0;
				for(i = 0; i < 18; i++)
				{
					ulTempH += pGpsCell->GPS_Sog_Buf[i];                        //lq ���
	
					if(pGpsCell->GPS_Sog_Buf[i] > ulMax)
					{				
						ulMax = pGpsCell->GPS_Sog_Buf[i];                       //lq ��¼���ֵ
					}
	
					if(pGpsCell->GPS_Sog_Buf[i] < ulMin)
					{
						ulMin = pGpsCell->GPS_Sog_Buf[i];                       //lq ��¼��Сֵ
					}
				}
				ulTempH -= ulMax;
				ulTempH -= ulMin;
	
				//����<0.3�ڣ���Ϊ0                                             //lq ����С��0.4��knot
				if(ulTempH <= 63)
				{
					pGpsCell->GPS_Sog = 0;	
				}
				else
				{
					pGpsCell->GPS_Sog = ulTempH >> 4;                           //lq ��ƽ��ֵ��/16��
				}
			}

			//�Եغ���ת��
			ucCheck = GetSegment((char *)pDat, chTemp, 8, uiLen, 30);

			if (ucCheck == FALSE)
			{
				//return (FALSE);
			}
			else
			{
				flTmp = (float)atof(chTemp);
				pGpsCell->GPS_Cog = (U32)((flTmp + 0.05) * 10);
			}

			//��������ȡ
			ucCheck = GetSegment((char *)pDat, chTemp, 9, uiLen, 30);

			if (ucCheck == FALSE)
			{
				//return (FALSE);
			}
			else
			{
				pGpsCell->GPS_TimeD = (chTemp[0] - '0') * 10 + (chTemp[1] - '0');
				pGpsCell->GPS_TimeMon = (chTemp[2] - '0') * 10 + (chTemp[3] - '0');
				pGpsCell->GPS_TimeY = (chTemp[4] - '0') * 10 + (chTemp[5] - '0');
			}
		}
        else
        {
            if (pGpsCell->GPS_LeapSecondFlg == FALSE)
            {
                pGpsCell->GPS_TimeHLeapLast = 60;
            	pGpsCell->GPS_TimeMLeapLast = 60;
            	pGpsCell->GPS_TimeSLeapLast = 60;               
            }
                    
        }
	}

    /*lq GGA������*/
    else if (strncmp((char *)pDat, "GPGGA", 5) == FALSE)
    {
        /*lq GPS��Ϣ����*/
        GpsPara.GPS_MsgGenFlag = TRUE;

		/*lq GPS quality indicator�����ֶβ���Ϊ�գ���׼�涨��*/
        ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 30);
		if (ucCheck != FALSE)
		{
            pGpsCell->GPS_QualityIndicator = chTemp[0] - '0';    
		}           
    }

    /*lq GSA������*/
    else if (strncmp((char *)pDat, "GPGSA", 5) == FALSE)
    {
        /*lq GPS��Ϣ����*/
        GpsPara.GPS_MsgGenFlag = TRUE;

		/*lq Mode��Mode=1 (fix not available, ������<3)��Mode=2 (2D, ������=3)��Mode=3 (3D, ������>3) */
        ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
		if (ucCheck != FALSE)
		{
            pGpsCell->GPS_FixMode = chTemp[0] - '0';    
		}
        else
        {
            pGpsCell->GPS_FixMode = 1;            
        }

		/*lq PDOP */
        ucCheck = GetSegment((char *)pDat, chTemp, 15, uiLen, 30);
		if (ucCheck != FALSE)
		{
            pGpsCell->GPS_PDOP = (U16)(atof(chTemp) * 10);
		}
        else
        {
            pGpsCell->GPS_PDOP = GPS_DATA_PDOP_DEFAULT;
        }                           
    }

    /*lq GBS������*/
    else if (strncmp((char *)pDat, "GPGBS", 5) == FALSE)
    {
        /*lq GPS��Ϣ����*/
        GpsPara.GPS_MsgGenFlag = TRUE;

	    /*lq Expected error in latitude*/
        ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
		if (ucCheck != FALSE)
		{
            pGpsCell->GPS_errlat = (U16)(atof(chTemp) + 0.5);
            pGpsCell->GPS_RAIMFlag = TRUE;    
		}
        else
        {
            pGpsCell->GPS_RAIMFlag = FALSE;            
        }

    	/*lq Expected error in longitude*/
        ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
		if (ucCheck != FALSE)
		{
            pGpsCell->GPS_errlon = (U16)(atof(chTemp) + 0.5);
            //lq pGpsCell->GPS_RAIMFlag = TRUE; //lq ֻ��2��3�ֶξ���Ϊ��ʱ����ΪTRUE                    
		}
        else
        {
            pGpsCell->GPS_RAIMFlag = FALSE;                    
        }        
    }

    /*lq TXT������*/
	else if (strncmp((char *)pDat, "GPTXT", 5) == FALSE)
    {
        GpsSelfCheckInst.DataCommFlg = TRUE;

		ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
		if (ucCheck == FALSE)
		{
			return (FALSE);
		} 

        /*lq ������Ч���ж�*/
        ucLen = strlen(chTemp);
		if (ucLen != 2)
		{
			return (FALSE);
		}

        ulTemp = strtoul(chTemp, &pEnd, 10);

		/*lq �ַ���Ч���ж�*/
		if ((*pEnd) != 0)
		{
			return (FALSE);
		}
                
        if ((ulTemp == 0) || (ulTemp == 1))     //lq 00 - error, 01 - warning         
        {
            GpsSelfCheckInst.ModuleFlg = FALSE; //lq ����һ�μ���Ϊ������
        }
        else if (ulTemp == 2)
        {   
            ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
            if (ucCheck == FALSE)
            {
                return (FALSE);
            } 
                        
			if (strncmp(chTemp, "ANTSTATUS=OK", 12) == FALSE)
            {
                GpsSelfCheckInst.AntennaFlg = TRUE;               
            }
        } 
    }

	return (TRUE);
}

/*******************************************************************************
* ����  : GpsOutputProcess
* ����  : GPS�����������
* ����  : pDat - GPS��仺��
          uiLen - GPS����ֽڳ���
* ���  : ��
* ����  : ��
*******************************************************************************/
void GpsOutputProcess(U8 *pDat, U16 uiLen)
{
	char chTemp[100];
	U16 i;
	U16 uiIndex;
	U8 ucCheck;
    U8 outputEn = FALSE;

	if (DevDynamicPara.ucGpsOutput == TRUE)
	{
		if (strncmp((char *)pDat, "GPRMC", 5) == FALSE)
        {
            if (DevDynamicPara.GpsOutputInterval[
                DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_RMC] != 0)
            {
                DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_RMC]++;            
                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_RMC]
                    >= DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_RMC])
                {
                    DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_RMC] = 0;
                }

                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_RMC] == 0)       
                {
                    outputEn = TRUE;   
                }
            }
        }
        else if (strncmp((char *)pDat, "GPVTG", 5) == FALSE)
        {
            if (DevDynamicPara.GpsOutputInterval[
                DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_VTG] != 0)
            {            
                DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_VTG]++;            
                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_VTG]
                    >= DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_VTG])
                {
                    DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_VTG] = 0;
                }

                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_VTG] == 0)       
                {
                    outputEn = TRUE;   
                }
            }
        }
        else if (strncmp((char *)pDat, "GPGGA", 5) == FALSE)
        {
            if (DevDynamicPara.GpsOutputInterval[
                DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GGA] != 0)
            {            
                DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GGA]++;            
                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GGA]
                    >= DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GGA])
                {
                    DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GGA] = 0;
                }

                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GGA] == 0)       
                {
                    outputEn = TRUE;   
                }
            }            
        }
        else if (strncmp((char *)pDat, "GPGSA", 5) == FALSE)
        {
            if (DevDynamicPara.GpsOutputInterval[
                DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSA] != 0)
            {            
                DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSA]++;            
                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSA]
                    >= DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSA])
                {
                    DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSA] = 0;
                }

                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSA] == 0)       
                {
                    outputEn = TRUE;   
                }
            }
        }
        else if (strncmp((char *)pDat, "GPGSV", 5) == FALSE)
        {
            if (DevDynamicPara.GpsOutputInterval[
                DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSV] != 0)
            {            
                /*lq ��ȡ��ǰ���ı�� */
                ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);   
        		if ((ucCheck == TRUE) && (chTemp[0] == '1'))    //lq ����䣬ֻ�Ե�һ��������
                {
                    DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSV]++;            
                    if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSV]
                        >= DevDynamicPara.GpsOutputInterval[
                        DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSV])
                    {
                        DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSV] = 0;
                    }            
                }

                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSV] == 0)       
                {
                    outputEn = TRUE;   
                }
            }
        }
        else if (strncmp((char *)pDat, "GPGLL", 5) == FALSE)
        {
            if (DevDynamicPara.GpsOutputInterval[
                DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GLL] != 0)
            {            
                DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GLL]++;            
                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GLL]
                    >= DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GLL])
                {
                    DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GLL] = 0;
                }

                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GLL] == 0)       
                {
                    outputEn = TRUE;   
                }
            }
        }
        else if (strncmp((char *)pDat, "GPGBS", 5) == FALSE)
        {
            if (DevDynamicPara.GpsOutputInterval[
                DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GBS] != 0)
            {            
                DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GBS]++;            
                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GBS]
                    >= DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GBS])
                {
                    DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GBS] = 0;
                }

                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GBS] == 0)       
                {
                    outputEn = TRUE;   
                }
            }
        }
                
		if (outputEn == TRUE)
        {
            uiIndex = 0;
    		chTemp[uiIndex++] = '$';
    		for (i = 0; i < uiLen; i++)
    		{
    			chTemp[uiIndex++] = pDat[i];	
    		}
    		chTemp[uiIndex++] = '\0';
    
    		UartResponseMsg(chTemp);	
        }
	}
}

/*******************************************************************************
* ����  : GpsMsgProcess
* ����  : GPS���Ľ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GpsMsgProcess(U8 *pDat, U16 uiLen)
{
	U8  ucCheck;

	/*lq ����GPS��� */
    ucCheck = DealGpsMsg(&GpsPara, pDat, uiLen);

    /*lq GPS��䴮��������� */
    GpsOutputProcess(pDat, uiLen);		

	return (ucCheck);
}

/*******************************************************************************
* ����  : GetGpsFixSta
* ����  : ����GPS��λ״̬
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetGpsFixSta(GPSStruct *pGpsCell)
{
	U16 x;

    /*lq GBS�������ݸ��� */
    pGpsCell->GPS_RAIMFlag = GpsPara.GPS_RAIMFlag;
	pGpsCell->GPS_errlat = GpsPara.GPS_errlat;
	pGpsCell->GPS_errlon = GpsPara.GPS_errlon; 

    /*lq GGA�������ݸ��� */        
    pGpsCell->GPS_QualityIndicator = GpsPara.GPS_QualityIndicator;
       	
    /*lq RMC�������ݸ��� */        
    if (IsGpsFix() == TRUE)
	{		
  		for (x = 0; x < 6; x++)
  		{
  			pGpsCell->GPSTimeBuf[x] = GpsPara.GPSTimeBuf[x];
  		}
	  	
  		for (x = 0; x < 18; x++)
  		{
  			pGpsCell->GPS_Sog_Buf[x] = GpsPara.GPS_Sog_Buf[x];
  		}
	  	
  		pGpsCell->GPS_TimeY = GpsPara.GPS_TimeY;
  		pGpsCell->GPS_TimeMon = GpsPara.GPS_TimeMon;
  		pGpsCell->GPS_TimeD = GpsPara.GPS_TimeD;
  		pGpsCell->GPS_TimeH = GpsPara.GPS_TimeH;
  		pGpsCell->GPS_TimeM = GpsPara.GPS_TimeM;
  		pGpsCell->GPS_TimeS = GpsPara.GPS_TimeS;
  		pGpsCell->GPS_LatiD = GpsPara.GPS_LatiD;
  		pGpsCell->GPS_Latitude = GpsPara.GPS_Latitude;
  		pGpsCell->GPS_Lati100 = GpsPara.GPS_Lati100;
  		pGpsCell->GPS_Longitud = GpsPara.GPS_Longitud;
  		pGpsCell->GPS_Long100 = GpsPara.GPS_Long100;
  		pGpsCell->GPS_Sog = GpsPara.GPS_Sog;
  		pGpsCell->GPS_Sog_Index = GpsPara.GPS_Sog_Index;
  		pGpsCell->GPS_Cog = GpsPara.GPS_Cog;
  		pGpsCell->GPS_PosPrec = GpsPara.GPS_PosPrec;

		return (TRUE);
	}
	else
	{
		return (FALSE);
	}	
}

#if 0
/*******************************************************************************
* ����  : GpsCtrlProcess
* ����  : GPS״̬������,������SARTʡ��ģʽ��GPS����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void GpsCtrlProcess(void)
{
	U8  ucTemp[10];
	U8  ucTemp1[10];
    S16 ssCurrSlot;
    S32 slCurrFrame;	

    ssCurrSlot = GetSlotNum();
    slCurrFrame = GetFrameNum();

	/*lq GPSʱ϶��λͬ��*/
    SlotSynClac();

#if CONFIG_GPS_SAVE
    
    /*lq GPSʡ�������ж�*/
    GpsSaveCondJudge();
#endif

    /*lq ��λ״̬��LEDָʾ������GPS������Ž���LEDָʾ*/
    if (GetGpsPowerSta() == TRUE)
    {
        if (IsGpsFix())
        {
            if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_GPS_FIX)
            {
    			LEDInit(LED_TYPE_GPS_FIX);
            }
        }
        else
        {
            if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_GPS_SEARCH)
            {
    			LEDInit(LED_TYPE_GPS_SEARCH);
            }                                                                      
        }
    }
    
    /*lq GPS�رպ�LED��GPSָʾֹͣ*/
    else
    {
        if ((LEDCtrlPara.LedGreenPara.ucType == LED_TYPE_GPS_SEARCH)
            || LEDCtrlPara.LedGreenPara.ucType == LED_TYPE_GPS_FIX) 
        {
            LEDInit(LED_TYPE_NONE);    
        }   
    }


    /*lq GPS״̬����*/
	if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_START)                	        //lq �豸����״̬
	{
        /*lq GPS��λ��ͬ��*/
        if (GpsCtrlPara.GPSSyn_Flag == TRUE)
        {
            GpsCtrlPara.GPSSyn_Flag = FALSE;

            /*lq GPS��λ�����رն�ʱ*/
            SetGpsOffTimSlot(ssCurrSlot);
            SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_FIX_LONG_OPEN);       //lq GPS��λ����5min

            /*lq GPS5min����������1*/
            GpsCtrlPara.ssGps5minLngOpenCnt++;
            if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
            {
                ResetGps5minLngOpenEn();

#ifdef GPS_CTRL_DEBUG_MSG
			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
            }
#if 0   //lq �ݲ�д��EEPROM����ֹÿ�ο������ϴμ�¼��ֵ����������
            /*lq ����GPS5min��������*/
			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
#endif
            GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIX_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
			usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif
        }
        else
        {
            /*lq GPSδ��λ�״γ����رն�ʱ*/
            SetGpsOffTimSlot(ssCurrSlot);
            SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_FIRST_LONG_OPEN);   //lq GPSδ��λ�״γ���5min

            /*lq GPSδ��λ1h��ʱ*/
            SetGpsUnfix1hPeriodTimSlot(ssCurrSlot);
            SetGpsUnfix1hPeriodTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_1_HOUR);

            /*lq GPS5min����������1*/
            GpsCtrlPara.ssGps5minLngOpenCnt++;
            if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
            {
                ResetGps5minLngOpenEn();

#ifdef GPS_CTRL_DEBUG_MSG
			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
            }
#if 0   //lq �ݲ�д��EEPROM����ֹÿ�ο������ϴμ�¼��ֵ����������
            /*lq ����GPS5min��������*/
			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
#endif
            GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
			usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif
        }
                                                                            	
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN)
    {
        /*lq GPS��λ��ͬ��*/
        if (GpsCtrlPara.GPSSyn_Flag == TRUE)
        {
            GpsCtrlPara.GPSSyn_Flag = FALSE;

            /*��λGPSδ��λ������־*/
            ResetGpsUnfixLonOpenFlg();
            ResetGpsUnfix1hLngOpenFlg();
            ResetGpsUnfixBey1hLngOpenFlg();
            
            /*lq GPS5min����ʹ��*/
            if (GetGps5minLngOpenEn() == TRUE)        
            {
                /*lq GPS��λ�����رն�ʱ*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_FIX_LONG_OPEN);   //lq GPS��λ����5min

                /*lq GPS5min����������1*/
                GpsCtrlPara.ssGps5minLngOpenCnt++;
                if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
                {
                    ResetGps5minLngOpenEn();
    
#ifdef GPS_CTRL_DEBUG_MSG
    			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
                }

                /*lq ����GPS5min��������*/
    			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
    		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
                
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIX_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                         
            }
            else
            {
                /*lq ת��GPS�ر�״̬*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif  
            }
        } 
        else
        {
            /*lq GPSδ��λ�״γ�����ʱ��*/
            if (IsGpsOffTimeout() == TRUE)
            {
                /*lq ��λGPSδ��ʱ1h������־*/
                SetGpsUnfixLonOpenFlg();
                SetGpsUnfix1hLngOpenFlg();

                /*lq ת��GPS�ر�״̬*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif 
            }
        }  
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_UNFIX_FIRST_HOUR_LNG_OPEN)
    {
        /*lq GPS��λ��ͬ��*/
        if (GpsCtrlPara.GPSSyn_Flag == TRUE)
        {
            GpsCtrlPara.GPSSyn_Flag = FALSE;
            
            /*lq ��λGPSδ��λ1h������־*/
            ResetGpsUnfixLonOpenFlg();
            ResetGpsUnfix1hLngOpenFlg();

            /*lq GPS5min����ʹ��*/
            if (GetGps5minLngOpenEn() == TRUE)        
            {
                /*lq GPS��λ�����رն�ʱ*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_FIX_LONG_OPEN);   //lq GPS��λ����5min

                /*lq GPS5min����������1*/
                GpsCtrlPara.ssGps5minLngOpenCnt++;
                if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
                {
                    ResetGps5minLngOpenEn();
    
#ifdef GPS_CTRL_DEBUG_MSG
    			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
                }

                /*lq ����GPS5min��������*/
    			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
    		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);

                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIX_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                    
            }
            else
            {
                /*lq ת��GPS�ر�״̬*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                  
            }
        }
        else
        {
            /*lq GPSδ��λ1h������ʱ��*/
            if (IsGpsOffTimeout() == TRUE)
            {
                /*lq 1h��ʱ��*/
                if (IsGpsUnfix1hPeriodTimeout() == TRUE)
                {
                    /*lq ��λGPSδ��1h������־*/
                    ResetGpsUnfix1hLngOpenFlg(); 

                    /*lq ��λGPSδ��λ����1h������־*/
                    SetGpsUnfixBey1hLngOpenFlg();
                }

                /*lq ת��GPS�ر�״̬*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                  
            }                                        
        }            
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_UNFIX_BEY_FIRST_HOUR_LNG_OPEN)
    {
        /*lq GPS��λ��ͬ��*/
        if (GpsCtrlPara.GPSSyn_Flag == TRUE)
        {
            GpsCtrlPara.GPSSyn_Flag = FALSE;

            /*lq ��λGPSδ��λ����1h������־*/
            ResetGpsUnfixLonOpenFlg();
            ResetGpsUnfixBey1hLngOpenFlg();

            /*lq GPS5min����ʹ��*/
            if (GetGps5minLngOpenEn() == TRUE)        
            {
                /*lq GPS��λ�����رն�ʱ*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_FIX_LONG_OPEN);   //lq GPS��λ����5min

                /*lq GPS5min����������1*/
                GpsCtrlPara.ssGps5minLngOpenCnt++;
                if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
                {
                    ResetGps5minLngOpenEn();
    
#ifdef GPS_CTRL_DEBUG_MSG
    			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
                }

                /*lq ����GPS5min��������*/
    			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
    		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);

                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIX_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                    
            }
            else
            {
                /*lq ת��GPS�ر�״̬*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                  
            }            
        }
        else
        {
            /*lq GPSδ��λ����1h������ʱ��*/
            if (IsGpsOffTimeout() == TRUE)
            {
                /*lq ת��GPS�ر�״̬*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                      
            }
        }                        
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_FIX_LNG_OPEN)
    {
        if (GetGpsPowerupFixState() == FALSE)
        {
            /*lq ��λGPS������λ��־*/
            SetGpsPowerupFixState();

			/*lq ��¼����ʱ��*/
            sprintf((char *)ucTemp, "%2d", GpsPara.GPS_TimeMon);
    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeD);
    		strcat((char *)ucTemp, (char *)ucTemp1);
            sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeH);
    		strcat((char *)ucTemp, (char *)ucTemp1);
    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeM);
    		strcat((char *)ucTemp, (char *)ucTemp1);
    		Para_Save(PARA_TIME_OPEN_TYPE, ucTemp, PARA_TIME_OPEN_MAXLEN);
        	Para_Save(PARA_TIME_CLOSE_TYPE, ucTemp, PARA_TIME_CLOSE_MAXLEN);                           //lq [2013/04/16]

			/*lq ����ػ�ԭ��*/
			sprintf((char *)ucTemp, "%d", 0);     		                        //lq ����ػ�ֻ����ΪĬ�Ϲػ���ʽ��ֻ�а����ػ��򳬵͵����ػ�ʱ���Ż��ٸ��Ĺػ�ԭ��
			Para_Save(PARA_CLOSE_RESION_TYPE, ucTemp, 1);
			
			/*lq ���GPS3��δ��λ����*/
			sprintf((char *)ucTemp, "%3d", 0);
		   	Para_Save(PARA_GPS_WAIT_COUNT_TYPE, ucTemp, 3);						

			/*lq ���GPS������λ����*/
			sprintf((char *)ucTemp, "%5d", 0);
		   	Para_Save(PARA_GPS_FIX_COUNT_TYPE, ucTemp, 5);
            
            /*lq ���GPS5min��������*/
			sprintf((char *)ucTemp, "%3d", 0);
		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
        } 
        
        /*lq GPS��λ������ʱ��*/ 
        if (IsGpsOffTimeout() == TRUE)
        {
            /*lq ����ʱ϶����λƫ�����ֵ����GPS������λ������ɺ���У�*/
			SetAdjustCalcNow();

            /*lq ת��GPS�ر�״̬*/            
            GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		    usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                
        }
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_NORMAL_WORKING)
    {
        /*lq ����Ϣδ��ʼ����*/
        if (GetBurstStarFlg() == FALSE)
        {
            /*lq GPS��λ��ͬ��*/
            if (GpsCtrlPara.GPSSyn_Flag == TRUE)
            {
                GpsCtrlPara.GPSSyn_Flag = FALSE;
                
                /*lq ��λ������λ��־*/
                SetGpsNormalFixState();

                /*lq ��λʧ�ܴ������㣬������λ������1*/
                GpsCtrlPara.GpsFallCount = 0;
                GpsCtrlPara.GpsFixCount++;
    			sprintf((char *)ucTemp, "%5d", GpsCtrlPara.GpsFixCount);
    		   	Para_Save(PARA_GPS_FIX_COUNT_TYPE, ucTemp, 5);//���Ϊ65536��

                /*lq ת��GPS�ر�״̬*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
			    usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                
            }
                    
        }
        else
        {
            /*lq ����Ϣ��ʼ���ͣ�GPS��λʧ��*/
            GpsCtrlPara.GpsFallCount++;
            usart_printf("$Gps Fall Count : %d\r\n", GpsCtrlPara.GpsFallCount);	                
            
            if (GpsCtrlPara.GpsFallCount >= eqEquipPara.GpsFixFallThres)
            {
            	/*lq GPS��λʧ�ܴ�������*/
                GpsCtrlPara.GpsFallCount = 0;
                
                /*lq GPSδ��λ����������1*/
                GpsCtrlPara.GpsWaitCount++;

                /*lq ����GPS3��δ��λ����*/
    			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.GpsWaitCount);
    		   	Para_Save(PARA_GPS_WAIT_COUNT_TYPE, ucTemp, 3);

                /*lq GPS5min����ʹ��*/
                if (GetGps5minLngOpenEn() == TRUE)        
                {
                    /*lq GPSδ��λ�״γ����رն�ʱ*/
                    SetGpsOffTimSlot(ssCurrSlot);
                    SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_FIRST_LONG_OPEN);   //lq GPSδ��λ�״γ���5min
            
                    /*lq GPSδ��λ1h��ʱ*/
                    SetGpsUnfix1hPeriodTimSlot(ssCurrSlot);
                    SetGpsUnfix1hPeriodTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_1_HOUR);

                    /*lq GPS5min����������1*/
                    GpsCtrlPara.ssGps5minLngOpenCnt++;
                    if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
                    {
                        ResetGps5minLngOpenEn();
        
#ifdef GPS_CTRL_DEBUG_MSG
        			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
                    }

                    /*lq ����GPS5min��������*/
        			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
        		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);

                    GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
			        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                       
                }
                else
                {
                    /*lq ��λδ��λ1h������־*/
                    SetGpsUnfixLonOpenFlg();
                    SetGpsUnfix1hLngOpenFlg();
                    
                    /*lq GPSδ��λ1h��ʱ*/    
                    SetGpsUnfix1hPeriodTimSlot(ssCurrSlot);
                    SetGpsUnfix1hPeriodTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_1_HOUR);

                    /*lq ת��GPS�ر�״̬*/
                    GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;           	 

#ifdef GPS_CTRL_DEBUG_MSG
			        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                       
                }
            } 
            else
            {
                /*lq ת��GPS�ر�״̬*/
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;           	 

#ifdef GPS_CTRL_DEBUG_MSG
			    usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                       
            }        
        }            
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_POWER_DOWN)
    {
        if (GetGpsPowerSta() == TRUE)
        {
            GpsPowerDown();

#ifdef GPS_CTRL_DEBUG_MSG
            UartResponseMsg("$GPS_POWER_OFF!");
#endif
        }
        
        /*lq ����Ϣ��ʼ����*/
        if (GetBurstStarFlg() == TRUE)
        {
            /*lq... GPS������ʱ*/
            SetGpsOnTim(SLOT_INTERVAL_GPS_ON_TO_TX);

            /*lq ת��GPS����״̬*/            
            GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_IDLE;

#ifdef GPS_CTRL_DEBUG_MSG
			usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_IDLE!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif      
        }     
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_IDLE)
    {
        /*lq GPS����״̬��Ч*/
        if (GetMsgGpsPowerup() == TRUE)
        {
            /*lq ����GPS��Դ������GPS������ز���*/                   
            GpsPowerUp();
#ifdef GPS_CTRL_DEBUG_MSG
            UartResponseMsg("$GPS_POWER_ON!");
#endif
            /*lq ����GPS��λ���ѡ����Ӧ��״̬*/
            if (GetGpsUnfix1hLngOpenFlg() == TRUE)                          //lq ת��GPSδ��λ1h����״̬
            {
                /*lq GPSδ��λ1h������ʱ*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_1HOUR_LONG_OPEN);   //lq GPSδ��λ1h�ڳ���1min                    

                /*lq ת��GPSδ��λ1h����״̬*/                
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIX_FIRST_HOUR_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_UNFIX_FIRST_HOUR_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif   
            }
            else if (GetGpsUnfixBey1hLngOpenFlg() == TRUE)                  //lq ת��GPSδ��λ����1h����״̬        
            {
                /*lq GPSδ��λ����1h������ʱ*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_BEY_1HOUR_LONG_OPEN);   //lq GPSδ��λ����1h����2min                    

                /*lq ת��GPSδ��λ����1h����״̬*/                
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIX_BEY_FIRST_HOUR_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_UNFIX_BEY_FIRST_HOUR_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif   
            }
            else                                                            //lq ת��GPS��������״̬ 
            {
                /*lq ת��GPS��������״̬*/                
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_NORMAL_WORKING;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_NORMAL_WORKING!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif   
            }
        }
    }
    else
    {
        /*lq ת��GPS��ʼ״̬*/                
        GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_START;

#ifdef GPS_CTRL_DEBUG_MSG
	    usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_START!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif   
    }    
}
#endif

#if 0
/*******************************************************************************
* ����  : GpsCtrlProcess
* ����  : GPS״̬�����ƣ�������AtoN����ģʽ��GPS���ơ�GPS���豸������һֱ����ʡ
          �繤��ģʽ������رա�
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void GpsCtrlProcess(void)
{
	U8  ucTemp[10];
	U8  ucTemp1[10];
    S16 ssCurrSlot;
    S32 slCurrFrame;	

    /*lq ��ȡ��ǰ֡��ʱ϶ */
    ssCurrSlot = GetSlotNum();
    slCurrFrame = GetFrameNum();

    /*lq GPS��λ״̬��LEDָʾ������GPS������Ž���LEDָʾ��GPS�رպ�LED��GPSָʾֹͣ*/
    if (GpsCtrlPara.GpsCtrlSta != ATON_GPS_CTRL_STA_IDLE)
    {
        if (IsGpsFix())
        {
            if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_GPS_FIX)
            {
    			LEDInit(LED_TYPE_GPS_FIX);
            }
        }
        else
        {
            if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_GPS_SEARCH)
            {
    			LEDInit(LED_TYPE_GPS_SEARCH);
            }                                                                      
        }
    }
    else
    {
        if ((LEDCtrlPara.LedGreenPara.ucType == LED_TYPE_GPS_SEARCH)
            || LEDCtrlPara.LedGreenPara.ucType == LED_TYPE_GPS_FIX) 
        {
            LEDInit(LED_TYPE_NONE);    
        }   
    }

	/*lq GPSʱ϶��λͬ��*/
    SlotSynClac();  //lq... AtoN���Ϊ֡��ʱ϶����λͬ��

    /*lq GPSƫ�����*/
    if (GetGpsOffPositionSampleEn() == TRUE)
    {
        if (GetGpsPositionSampleNewFlgRst() == TRUE)
        {
            OP1_GPSPositionAnal(GpsPara.GPS_Longitud, GpsPara.GPS_Latitude);
        }
    }
    
    /*lq AtoN GPS״̬����*/
    if (GpsCtrlPara.GpsCtrlSta == ATON_GPS_CTRL_STA_START)
    {
        /*lq GPS���ջ��������������*/
        if (UBX1_GetSendRequireFlag() == FALSE)
        {
            /*lq ��GPS����������ɺ󣬽�GPS��������Ϊ��Ч*/
            SetGpsPulseEn();

            /*lq GPS��λ��ͬ��*/
            if (GpsCtrlPara.GPSSyn_Flag == TRUE)
            {
                GpsCtrlPara.GPSSyn_Flag = FALSE;

                /*lq ��λ������λ��־*/
                SetGpsNormalFixState();
                        
                /*lq GPS��λ�����رն�ʱ*/
                GpsCtrlPara.TimePeriod = GPS_CTRL_1MINPEROID;
            	GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
            	GpsCtrlPara.Time1mCnt = 0;  

                GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_FIX_LNG_OPEN;
    
#if CONFIG_DEBUG
    			usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif
            }
            else
            {
                GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_NORMAL_WORKING;
    
#if CONFIG_DEBUG
    			usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_NORMAL_WORKING!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif
            }            
        }                    
    }
    else if (GpsCtrlPara.GpsCtrlSta == ATON_GPS_CTRL_STA_FIX_LNG_OPEN)
    {
        if (GetGpsPowerupFixState() == FALSE)
        {
            /*lq ��λGPS������λ��־*/
            SetGpsPowerupFixState();

			/*lq ��¼����ʱ��*/
            sprintf((char *)ucTemp, "%2d", GpsPara.GPS_TimeMon);
    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeD);
    		strcat((char *)ucTemp, (char *)ucTemp1);
            sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeH);
    		strcat((char *)ucTemp, (char *)ucTemp1);
    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeM);
    		strcat((char *)ucTemp, (char *)ucTemp1);
    		Para_Save(PARA_TIME_OPEN_TYPE, ucTemp, PARA_TIME_OPEN_MAXLEN);
        	Para_Save(PARA_TIME_CLOSE_TYPE, ucTemp, PARA_TIME_CLOSE_MAXLEN);                           //lq [2013/04/16]

			/*lq ����ػ�ԭ��*/
			sprintf((char *)ucTemp, "%d", 0);     		                        //lq ����ػ�ֻ����ΪĬ�Ϲػ���ʽ��ֻ�а����ػ��򳬵͵����ػ�ʱ���Ż��ٸ��Ĺػ�ԭ��
			Para_Save(PARA_CLOSE_RESION_TYPE, ucTemp, 1);
			
			/*lq ���GPS3��δ��λ����*/
			sprintf((char *)ucTemp, "%3d", 0);
		   	Para_Save(PARA_GPS_WAIT_COUNT_TYPE, ucTemp, 3);						

			/*lq ���GPS������λ����*/
			sprintf((char *)ucTemp, "%5d", 0);
		   	Para_Save(PARA_GPS_FIX_COUNT_TYPE, ucTemp, 5);
            
            /*lq ���GPS5min��������*/
			sprintf((char *)ucTemp, "%3d", 0);
		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
        } 

        /*lq ��������δ��λ��ͬ�������¶�λ��ͬ��*/                    
        if (GetGpsUnfixLonOpenFlg() == TRUE)
        {
            ResetGpsUnfixLonOpenFlg();
        }

    	if (GpsCtrlPara.TimeCnt == GetLocalClockTickProcess())
    	{
    		GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
            
            GpsCtrlPara.Time1mCnt++;
            if (GpsCtrlPara.Time1mCnt >= GPS_CTRL_TIME_FIX_LONG_OPEN)
            {
                /*lq ����ʱ϶����λƫ�����ֵ����GPS������λ������ɺ���У�*/
    			SetAdjustCalcNow();
    
                /*lq ʹ��UBX protocol ��Ϣ����GPS�����Զ���ʡ�繤��ģʽ*/
                UBX1_GpsIdleConfig();                
                
                /*lq ת��GPS����״̬*/            
                GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_IDLE;
    
#if CONFIG_DEBUG
    		    usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_IDLE!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                
            }
        
        }
    }
    else if (GpsCtrlPara.GpsCtrlSta == ATON_GPS_CTRL_STA_NORMAL_WORKING)
    {
        /*lq GPS���ջ������Զ�����������ģʽ���������*/
        if (UBX1_GetSendRequireFlag() == FALSE)
        {
            /*lq ��GPS����������ɺ󣬽�GPS��������Ϊ��Ч*/
            SetGpsPulseEn();

            /*lq ������λ��ͬ��*/
            if (GetGpsNormalFixState() == FALSE)                
            {
                /*lq GPS��λ��ͬ��*/
                if (GpsCtrlPara.GPSSyn_Flag == TRUE)
                {
                    GpsCtrlPara.GPSSyn_Flag = FALSE;
    
                    /*lq ��λ������λ��־*/
                    SetGpsNormalFixState();
        
                    /*lq ��λʧ�ܴ������㣬������λ������1*/
                    GpsCtrlPara.GpsFallCount = 0;
                    GpsCtrlPara.GpsFixCount++;
        			sprintf((char *)ucTemp, "%5d", GpsCtrlPara.GpsFixCount);
        		   	Para_Save(PARA_GPS_FIX_COUNT_TYPE, ucTemp, 5);//���Ϊ65536�� 
 
                    /*lq ��������δ��λ��ͬ�������¶�λ��ͬ�� OR �����״ζ�λ*/
                    if (GetGpsUnfixLonOpenFlg() == TRUE
                        || GetGpsPowerupFixState() == FALSE)
                    {
                        /*lq GPS��λ�����رն�ʱ*/
                        GpsCtrlPara.TimePeriod = GPS_CTRL_1MINPEROID;
                    	GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
                    	GpsCtrlPara.Time1mCnt = 0;  
                        
                        /*lq ת��GPS��λ����״̬*/            
                        GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_FIX_LNG_OPEN;
        
#if CONFIG_DEBUG
        		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif 
                    }
                }                                   
            }

            /*lq ����Ϣ�ѿ�ʼ����*/
            if (GetBurstStarFlg() == TRUE)
            {
                /*lq ������λ��ͬ��*/
                if (GetGpsNormalFixState() == TRUE)                
                {
                    /*lq ����ƫ���������� */
                    if (OP1_GetGPSOffPositionPossStat() == TRUE)
                    {
                        /*lq ��λƫ���жϽṹ�����*/
                        OP1_GPSOffPositionInstanceReset();

                        /*lq ת��GPSƫ���ж�״̬*/            
                        GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_OFF_POSITION_JUDGE;                        

#if CONFIG_DEBUG
		                usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_OFF_POSITION_JUDGE!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif
                    }
                    else
                    {
                        /*lq ʹ��UBX protocol ��Ϣ����GPS�����Զ���ʡ�繤��ģʽ*/
                        UBX1_GpsIdleConfig();
                        
                        /*lq ת��GPS����״̬*/            
                        GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_IDLE;                        

#if CONFIG_DEBUG
		                usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_IDLE!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                        
                    }
                }
                else
                {
                    /*lq ����Ϣ��ʼ���ͣ�GPS��λʧ��*/
                    GpsCtrlPara.GpsFallCount++;
    
#if CONFIG_DEBUG
                    usart_printf("$Gps Fall Count : %d\r\n", GpsCtrlPara.GpsFallCount);	                
#endif                
                    if (GpsCtrlPara.GpsFallCount >= eqEquipPara.GpsFixFallThres)
                    {
                    	/*lq GPS��λʧ�ܴ�������*/
                        GpsCtrlPara.GpsFallCount = 0;
                        
                        /*lq GPSδ��λ����������1*/
                        GpsCtrlPara.GpsWaitCount++;
        
                        /*lq ����GPS3��δ��λ����*/
            			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.GpsWaitCount);
            		   	Para_Save(PARA_GPS_WAIT_COUNT_TYPE, ucTemp, 3);
    
                        /*lq ��λδ��λ1h������־*/
                        SetGpsUnfixLonOpenFlg();                                                         
                    } 
    
                    /*lq ʹ��UBX protocol ��Ϣ����GPS�����Զ���ʡ�繤��ģʽ*/
                    UBX1_GpsIdleConfig();
                    
                    /*lq ת��GPS����״̬*/            
                    GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_IDLE;                        
    
#if CONFIG_DEBUG
                    usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_IDLE!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                 
                }
            }                          
        }        
    }
    else if (GpsCtrlPara.GpsCtrlSta == ATON_GPS_CTRL_STA_OFF_POSITION_JUDGE)
    {
        /*lq ������λ��ͬ��*/
        if (GetGpsNormalFixState() == FALSE)                
        {
            /*lq GPS��λ��ͬ��*/
            if (GpsCtrlPara.GPSSyn_Flag == TRUE)
            {
                GpsCtrlPara.GPSSyn_Flag = FALSE;

                /*lq ��λ������λ��־*/
                SetGpsNormalFixState();
            }                                   
        }

        /*lq �ȴ�ƫ��������Ч*/
        if (OP1_GetGPSOffPositionValid() == TRUE)
        {
            /*lq ƫ���������� */    
            if (OP1_GetGPSOffPositionStat() == TRUE)
            {
                /*lq ת��GPSƫ�볤��״̬ */            
                GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_OFF_POSITION;                        

#if CONFIG_DEBUG
                usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_OFF_POSITION!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                        
            }
            else
            {
                /*lq ʹ��UBX protocol ��Ϣ����GPS�����Զ���ʡ�繤��ģʽ*/
                UBX1_GpsIdleConfig();
                
                /*lq ת��GPS����״̬*/            
                GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_IDLE;                        

#if CONFIG_DEBUG
                usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_IDLE!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif            
            }                    
        }
    }
    else if (GpsCtrlPara.GpsCtrlSta == ATON_GPS_CTRL_STA_OFF_POSITION)
    {
        /*lq ������λ��ͬ��*/
        if (GetGpsNormalFixState() == FALSE)                
        {
            /*lq GPS��λ��ͬ��*/
            if (GpsCtrlPara.GPSSyn_Flag == TRUE)
            {
                GpsCtrlPara.GPSSyn_Flag = FALSE;

                /*lq ��λ������λ��־*/
                SetGpsNormalFixState();
            }                                   
        }

        /*lq �ȴ�ƫ��������Ч*/
        if (OP1_GetGPSOffPositionValid() == TRUE)
        {
            /*lq ƫ������������ */    
            if (OP1_GetGPSOffPositionStat() == FALSE)
            {
                /*lq ʹ��UBX protocol ��Ϣ����GPS�����Զ���ʡ�繤��ģʽ*/
                UBX1_GpsIdleConfig();
                
                /*lq ת��GPS����״̬*/            
                GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_IDLE;                        

#if CONFIG_DEBUG
                usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_IDLE!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif            
            }                    
        }    
    }
    else if (GpsCtrlPara.GpsCtrlSta == ATON_GPS_CTRL_STA_IDLE)
    {
        /*lq GPSƫ�����ʧ�� */
        if (GetGpsOffPositionSampleEn() == TRUE)
        {
            ResetGpsOffPositionSampleEn();
        }

        /*lq GPS���ջ������Զ���ʡ�繤��ģʽ���������*/
        if (UBX1_GetSendRequireFlag() == FALSE)
        {
            /*lq GPS����״̬��Ч��GPS��δ����*/
            if (GetMsgGpsPowerup() == TRUE)
            {
                /*lq ����GPS������ز���*/                   
                GpsWakeUp();
#if CONFIG_DEBUG
                UartResponseMsg("$GPS_WAKE_ON!");
#endif
                /*lq ʹ��UBX protocol ��Ϣ����GPS�����Զ�����������ģʽ*/ 
                UBX1_GpsNormalConfig();
                
                /*lq ת��GPS��������״̬*/                
                GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_NORMAL_WORKING;
    
#if CONFIG_DEBUG
                usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_NORMAL_WORKING!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif   
            }                    
        }    
    }
    else
    {
        /*lq GPS��������*/        
        UBX1_GpsStartupConfig();
        
        /*lq ת��GPS��������״̬*/                
        GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_START;

#if CONFIG_DEBUG
        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_START!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif     
    }
}

#endif

/*******************************************************************************
* ����  : GpsCtrlProcess
* ����  : GPS״̬���ƣ�������M4508D��GPS������һֱ������������ģʽ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void GpsCtrlProcess(void)
{
    DateTimeStruct dateTime;
	U8  ucTemp[10];
	U8  ucTemp1[15];
    S16 currSlot;
    S32 currFrame;
	U8  ucTempBuf[40];
	S32	distance;

		

    /*lq ��ȡ��ǰ֡��ʱ϶ */
    currSlot = GetSlotNum();
    currFrame = GetFrameNum();

    /*lq GPS��λ״̬LEDָʾ */
    if (IsGpsFix())
    {
		LEDInit(LED_TYPE_GPS_FIX);
		Para_Read(PARA_GPS_FIRST_TIME_TYPE,ucTempBuf);
		if(strncmp(ucTempBuf, PARA_GPS_FIRST_TIME_DEFAULT, 12) == FALSE)
		{ 
			/* ��¼GPS��һ�ζ�λ��ʱ�� */
			dateTime.Year = GpsPara.GPS_TimeY;
			dateTime.Month = GpsPara.GPS_TimeMon;
			dateTime.Day = GpsPara.GPS_TimeD;
			dateTime.Hour = GpsPara.GPS_TimeH;
			dateTime.Minute = GpsPara.GPS_TimeM;
			dateTime.Second = GpsPara.GPS_TimeS;
			
			sprintf((char *)ucTempBuf, "%02d", (dateTime.Year) % 100);
			sprintf((char *)ucTemp1, "%02d", dateTime.Month);
			strcat((char *)ucTempBuf, (char *)ucTemp1);
			sprintf((char *)ucTemp1, "%02d", dateTime.Day);
			strcat((char *)ucTempBuf, (char *)ucTemp1);
			sprintf((char *)ucTemp1, "%02d", dateTime.Hour);
			strcat((char *)ucTempBuf, (char *)ucTemp1);
			sprintf((char *)ucTemp1, "%02d", dateTime.Minute);
			strcat((char *)ucTempBuf, (char *)ucTemp1);
			sprintf((char *)ucTemp1, "%02d", dateTime.Second);
			strcat((char *)ucTempBuf, (char *)ucTemp1);
			
			Para_Save(PARA_GPS_FIRST_TIME_TYPE,ucTempBuf,PARA_GPS_FIRST_TIME_MAXLEN);		
		}
		if(usGPSSynContinueTime == 61)// || (((GpsPara.GPS_TimeM + 60) - usGPSSynContinueTime) % 60) >= 5)
		{
			usGPSSynContinueTime = GpsPara.GPS_TimeM;
			LastGPS_Latitude = (double)GpsPara.GPS_Latitude;				  
			LastGPS_Longitud = (double)GpsPara.GPS_Longitud;
		}
		else if ((((GpsPara.GPS_TimeM + 60) - usGPSSynContinueTime) % 60) >= 60)
		{
			/* ��ȡ����֮ǰ�ĺ��о��� */
			Para_Read(PARA_SAILING_DISTINCE_TYPE, ucTempBuf);               // ���μ���֮ǰ���ܺ�����
            distance = atol((char *)ucTempBuf);
			
			/* 0000.1��ת��Ϊ��*/
        	LastGPS_Latitude = LastGPS_Latitude / 600000.0;
        	LastGPS_Longitud = LastGPS_Latitude / 600000.0;
        	NowGPS_Latitude = (double)GpsPara.GPS_Latitude / 600000.0;
        	NowGPS_Longitud = (double)GpsPara.GPS_Longitud / 600000.0;
			
			/* ���㱾�κ��о��� */        	
        	distance += GetDistance(NowGPS_Latitude, NowGPS_Longitud, LastGPS_Latitude, LastGPS_Longitud);

			sprintf(ucTempBuf, "%5d", distance);
            Para_Save(PARA_SAILING_DISTINCE_TYPE, (U8 *)ucTempBuf, 5);      // ����֮����ܺ�����

			/* ����ʱ�䡢��γ�� */
			usGPSSynContinueTime = GpsPara.GPS_TimeM;
			LastGPS_Latitude = (double)GpsPara.GPS_Latitude;				  
			LastGPS_Longitud = (double)GpsPara.GPS_Longitud;
		}
    }
    else
    {
		LEDInit(LED_TYPE_GPS_SEARCH);
    }

	/*lq GPSʱ϶��λͬ�� */
    SlotSynClac();

    /*lq M4508D GPS״̬���� */
    switch(GpsCtrlPara.GpsCtrlSta)
    {
		case GPS_CTRL_STA_START:
		{
			/*lq GPSδ��λ�澯��ʱ */
			GpsCtrlPara.TimePeriod = GPS_CTRL_TIME_PERIOD_1S;
			GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
			GpsCtrlPara.Timeout = GPS_CTRL_TIME_TIMEOUT_UNFIX_ERR;
			GpsCtrlPara.TimeoutCnt = 0;

			GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIXED;

			debug_printf("$[%6d:%4d] %s : Gps ctrl state to UNFIXED!\r\n",
				currFrame, currSlot, __func__);
			break;
		}
    
		case GPS_CTRL_STA_UNFIXED:
		{
			/*lq GPS��λ��ͬ��*/
			if (GpsCtrlPara.GPSSyn_Flag == TRUE)
			{
				GpsCtrlPara.GPSSyn_Flag = FALSE;

				/*lq ��λ������λ��־*/
				SetGpsNormalFixState();
                    
				/*lq GPSδ��λ�ж���ʱ */
				GpsCtrlPara.TimePeriod = GPS_CTRL_TIME_PERIOD_1S;
				GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
				GpsCtrlPara.Timeout = GPS_CTRL_TIME_TIMEOUT_UNFIX_ASSERT;
				GpsCtrlPara.TimeoutCnt = 0;

				GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIXED;
	
				debug_printf("$[%6d:%4d] %s : Gps ctrl state to FIXED!\r\n",
					currFrame, currSlot, __func__);
			}
			else
			{
				/*lq GPSδ��λ�澯�ж� */
				if (GpsCtrlPara.TimeCnt == GetLocalClockTickProcess())
				{
					GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
				
					GpsCtrlPara.TimeoutCnt++;
					if (GpsCtrlPara.TimeoutCnt >= GpsCtrlPara.Timeout)
					{
						GpsCtrlPara.TimeoutCnt = 0;
       
						LEDInit(LED_TYPE_ERR);

						GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIXED_ERR;
      
						debug_printf("$[%6d:%4d] %s : Gps ctrl state to UNFIXED_ERR!\r\n",
							currFrame, currSlot, __func__);
					}
				}        
			}
			break;
		}
		case GPS_CTRL_STA_UNFIXED_ERR:
		{
			/*lq GPS��λ��ͬ��*/
			if (GpsCtrlPara.GPSSyn_Flag == TRUE)
			{
				GpsCtrlPara.GPSSyn_Flag = FALSE;

				/*lq ��λ������λ��־*/
				SetGpsNormalFixState();
                    
				/*lq GPSδ��λ�ж���ʱ */
				GpsCtrlPara.TimePeriod = GPS_CTRL_TIME_PERIOD_1S;
				GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
				GpsCtrlPara.Timeout = GPS_CTRL_TIME_TIMEOUT_UNFIX_ASSERT;
				GpsCtrlPara.TimeoutCnt = 0;

				LEDInit(LED_TYPE_RED_NONE);
	
					GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIXED;

				debug_printf("$[%6d:%4d] %s : Gps ctrl state to FIXED!\r\n",
					currFrame, currSlot, __func__);
			}  
			break;
		}
		case GPS_CTRL_STA_FIXED:
		{
			if (GetGpsPowerupFixState() == FALSE)
			{
				/*lq ��λGPS������λ��־ */
				SetGpsPowerupFixState();

				/*lq ��¼����ʱ�� */
				dateTime.Year = GpsPara.GPS_TimeY;
				dateTime.Month = GpsPara.GPS_TimeMon;
				dateTime.Day = GpsPara.GPS_TimeD;
				dateTime.Hour = GpsPara.GPS_TimeH;
				dateTime.Minute = GpsPara.GPS_TimeM;
				dateTime.Second = GpsPara.GPS_TimeS;
            
				TIME_DateTimeOffset28Days(&dateTime, -GetFrameNum());     //lq ��λʱ��ʱ���ȥ���������ڵ�ʱ����

				sprintf((char *)ucTemp, "%02d", (dateTime.Year) % 100);
				sprintf((char *)ucTemp1, "%02d", dateTime.Month);
				strcat((char *)ucTemp, (char *)ucTemp1);
				sprintf((char *)ucTemp1, "%02d", dateTime.Day);
				strcat((char *)ucTemp, (char *)ucTemp1);
				sprintf((char *)ucTemp1, "%02d", dateTime.Hour);
				strcat((char *)ucTemp, (char *)ucTemp1);
				sprintf((char *)ucTemp1, "%02d", dateTime.Minute);
				strcat((char *)ucTemp, (char *)ucTemp1);
				sprintf((char *)ucTemp1, "%02d", dateTime.Second);
				strcat((char *)ucTemp, (char *)ucTemp1);

				Para_OpenTimeSave(ucTemp, DevDynamicPara.OpenCloseTimeIndex);
				Para_CloseTimeSave(ucTemp, DevDynamicPara.OpenCloseTimeIndex);
				TASK_TaskTimEnable(&RecordCloseTimeTask,
                TASK_TIME_TIMEOUT_RECORD_CLOSE_TIME);
			} 

			/*lq GPS��λ��ͬ��*/
			if (GpsCtrlPara.GPSSyn_Flag == TRUE)
			{
				GpsCtrlPara.GPSSyn_Flag = FALSE;
	
				/*lq GPSδ��λ�ж����¶�ʱ */
				GpsCtrlPara.TimePeriod = GPS_CTRL_TIME_PERIOD_1S;
				GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
				GpsCtrlPara.Timeout = GPS_CTRL_TIME_TIMEOUT_UNFIX_ASSERT;
				GpsCtrlPara.TimeoutCnt = 0;
			}
			else
			{
				/*lq GPSδ��λ�ж��ж� */
				if (GpsCtrlPara.TimeCnt == GetLocalClockTickProcess())
				{
					GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
				
					GpsCtrlPara.TimeoutCnt++;
					if (GpsCtrlPara.TimeoutCnt >= GpsCtrlPara.Timeout)
					{
						GpsCtrlPara.TimeoutCnt = 0;

						/*lq �ж�Ϊδ��λ */
						ResetGpsNormalFixState();
                    
						/*lq GPSδ��λ�澯��ʱ */
						GpsCtrlPara.TimePeriod = GPS_CTRL_TIME_PERIOD_1S;
						GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
						GpsCtrlPara.Timeout = GPS_CTRL_TIME_TIMEOUT_UNFIX_ERR;
						GpsCtrlPara.TimeoutCnt = 0;

						GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIXED;
        
						debug_printf("$[%6d:%4d] %s : Gps ctrl state to UNFIXED!\r\n",
							currFrame, currSlot, __func__);
					}
				}
			}  
			break;
		}
		default:
		{
			GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_START;

			debug_printf("$[%6d:%4d] %s : Gps ctrl state to START!\r\n",
				currFrame, currSlot, __func__);    
			break;
		}		
	}
}

/*******************************************************************************
* ����  : GpsCtrlProcess
* ����  : GPS״̬������,������SARTʡ��ģʽ��GPS����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void GpsCtrlProcess_MsgTest(void)
{
    /*lq ��λ״̬��LEDָʾ*/
    if (IsGpsFix())
    {
        if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_GPS_FIX)
        {
			LEDInit(LED_TYPE_GPS_FIX);
        }
    }
    else
    {
        if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_GPS_SEARCH)
        {
			LEDInit(LED_TYPE_GPS_SEARCH);
        }                                                                      
    }

    /*lq �û�����ģʽ�£�GPS����״̬��*/
    if (GPSCtrlMsgTestPara.GpsCtrlSta == GPS_CTRL_STA_MSG_TEST_START)
    {
        if (IsGpsFix())
        {
            GPSCtrlMsgTestPara.GpsPowerupFix = TRUE;

            GPSCtrlMsgTestPara.GpsCtrlSta = GPS_CTRL_STA_MSG_TEST_POWER_DOWN;                        
		    usart_printf("[%6d:%4d]$MsgCtrlStateMsgTest : GPS_CTRL_STA_MSG_TEST_POWER_DOWN!\r\n",
                         GetFrameNum(), GetSlotNum());	                            
        }
        else
        {
            /*lq GPS1min��ʱ*/
            GPSCtrlMsgTestPara.TimeCnt = SetTimePeriodProcess(GPSCtrlMsgTestPara.TimePeriod);

            GPSCtrlMsgTestPara.GpsCtrlSta = GPS_CTRL_STA_MSG_TEST_WAIT_FIX; 
		    usart_printf("[%6d:%4d]$MsgCtrlStateMsgTest : GPS_CTRL_STA_MSG_TEST_WAIT_FIX!\r\n",
                         GetFrameNum(), GetSlotNum());	                            
        }
    }
    else if (GPSCtrlMsgTestPara.GpsCtrlSta == GPS_CTRL_STA_MSG_TEST_WAIT_FIX)
    {
        if (IsGpsFix())
        {
            /*lq ��λ�ɹ�*/
            GPSCtrlMsgTestPara.GpsPowerupFix = TRUE;

            GPSCtrlMsgTestPara.GpsCtrlSta = GPS_CTRL_STA_MSG_TEST_POWER_DOWN;                        
		    usart_printf("[%6d:%4d]$MsgCtrlStateMsgTest : GPS_CTRL_STA_MSG_TEST_POWER_DOWN!\r\n",
                         GetFrameNum(), GetSlotNum());	                            
        }
        else
        {
	         //lq �ȴ���λ                   
        }

        if (GPSCtrlMsgTestPara.TimeCnt == GetLocalClockTickProcess())
        {
            GPSCtrlMsgTestPara.TimeCnt = SetTimePeriodProcess(GPSCtrlMsgTestPara.TimePeriod);
            
            GPSCtrlMsgTestPara.Time1mCnt++;
            if (GPSCtrlMsgTestPara.Time1mCnt >= GPS_CTRL_TIME_THRES_MSG_TEST)
            {
                /*lq �û�����ģʽ��ʱ�����ر�GPS*/
                GPSCtrlMsgTestPara.GpsCtrlSta = GPS_CTRL_STA_MSG_TEST_POWER_DOWN;                        
    		    usart_printf("[%6d:%4d]$MsgCtrlStateMsgTest : GPS_CTRL_STA_MSG_TEST_POWER_DOWN!\r\n",
                             GetFrameNum(), GetSlotNum());	                            
            }
        }                
    }
    else if (GPSCtrlMsgTestPara.GpsCtrlSta == GPS_CTRL_STA_MSG_TEST_POWER_DOWN)
    {
        /*lq GPS�ر�*/
        GpsPowerDown();
        
        GPSCtrlMsgTestPara.GpsCtrlSta = GPS_CTRL_STA_MSG_TEST_IDLE;                        
	    usart_printf("[%6d:%4d]$MsgCtrlStateMsgTest : GPS_CTRL_STA_MSG_TEST_IDLE!\r\n",
                     GetFrameNum(), GetSlotNum());	                            
                                
    }
    else if (GPSCtrlMsgTestPara.GpsCtrlSta == GPS_CTRL_STA_MSG_TEST_IDLE)
    {
        //lq �ȴ��ػ�
    }
    else
    {
        GPSCtrlMsgTestPara.GpsCtrlSta = GPS_CTRL_STA_MSG_TEST_START;
        usart_printf("[%6d:%4d]$MsgCtrlStateMsgTest : GPS_CTRL_STA_MSG_TEST_START!\r\n",
                     GetFrameNum(), GetSlotNum());	                            

    }

}

/*******************************************************************************
* ����  : GetMsgTestSendFlg
* ����  : ��ȡ�û�����ģʽ��Ϣ���ͱ�־
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetMsgTestSendFlg(void)
{
    if (GPSCtrlMsgTestPara.GpsCtrlSta == GPS_CTRL_STA_MSG_TEST_IDLE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
/*******************************************************************************
* ����  : GpsPowerUp
* ����  : ��ʡ�絽����������GPS���ƣ�������Դ�����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void GpsPowerUp(void)
{
	ResetGpsCtrlStru(&GpsCtrlPara);
	ResetGpsMsgStru(&GpsPara);
    ResetGpsSynSta();
    //lq GpsSynCell.GPSSyn_FirstSynFlag = FALSE;    
	LEDInit(LED_TYPE_GPS_SEARCH);

#if !WIN32
	IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_ON);
#endif
}

/*******************************************************************************
* ����  : GpsWakeUp
* ����  : ��ʡ�絽����������GPS���ƣ���������
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void GpsWakeUp(void)
{
	ResetGpsCtrlStru(&GpsCtrlPara);
	ResetGpsMsgStru(&GpsPara);
    ResetGpsSynSta();
#if 0
    OP1_GPSOffPositionInstanceReset();
#endif
    //lq GpsSynCell.GPSSyn_FirstSynFlag = FALSE;    
	LEDInit(LED_TYPE_GPS_SEARCH);
}

/*******************************************************************************
* ����  : GetGpsSleepSta
* ����  : ��ȡGPS����״̬,ֻҪGPS�����ڶ�λ����״̬�Ϳ�������λ�״γ���״̬��
          �����MCU���ߣ�����GPS��λ����ʱ��Ҫ���б��ض�ʱ��ͬGPS��У׼
          AtoN:GPS������ͨ�����Ŀ���GPS Receiver�Ĺ���״̬������GPS����ģ�鴦��
          ATON_GPS_CTRL_STA_IDLEʱMCU�ɽ���stop����ģʽ
* ����  : ��		  
* ���  : ��
* ����  : TRUE��ʾ���Խ���ʡ�磬FALSE��ֹ����ʡ��
*******************************************************************************/
U8 GetGpsSleepSta(void)
{
#if 0
	if ((GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_FIX_LNG_OPEN)
        || (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN && !GetGpsPowerupFixState()))
	{
		return (FALSE);	
	}
	else
	{
		return (TRUE);
	}
#else
	if (GpsCtrlPara.GpsCtrlSta == ATON_GPS_CTRL_STA_IDLE)
	{
		return (TRUE);	
	}
	else
	{
		return (FALSE);
	}
#endif	
}

#if 0 //lq
/*******************************************************************************
* ����  : SlotSynClac
* ����  : ʱ϶ͬ����ʱ϶��λͬ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SlotSynClac(void)
{
	U16 uiTemp;
	U16 ChkTimeS;

//lqdb    GpsSynCell.GPSSyn_PulseEn = TRUE; 

	if (GpsSynCell.GPSSyn_PulseEn == TRUE)                                      //lq GPS��λ��Ž���ͬ��
	{                                                                           
		GpsSynCell.GPSSyn_PulseEn = FALSE;                                      //lq ÿ�������壨ÿ1s����ͬ��1��

		ChkTimeS = GpsPara.GPS_TimeS + 1;                                         //lq ��Ϊ��ͬ������GPS������֮����еģ����Լ�1��������ͬ���ı�׼ʱ�����������壩��
		if (ChkTimeS >= 60)                                                     //lq ChkTimeSΪ��׼�루GPS�룩
		{
			ChkTimeS = 0;
		}
		//ʱ϶ͬ��                                                              //lq ʱ϶ͬ������������ʱ����ͬ��
		if ((ChkTimeS & 0x01) == 1)	 //ʱ϶ͬ��ÿ���������  times�Ǵ�gps��������ȡ������
		{									//���gps_timeS���ֵ�����GPS�ı���״̬�£�����λ֮�󱣻�10�룬�����ֵΪ������
		    uiTemp = (ChkTimeS >> 1) * 75 + 37;//����ǵ�ǰ��slot��ֵ   1���slot����Ϊ75/2  �����������
			GpsSynCell.GPSSyn_SlotOffset = uiTemp - GpsSynCell.GPSSyn_PulseSlot;//��ǰ��ֵ��ȥ�����ֵ  //lq ChkTimeSΪ60sһѭ����GPSSyn_PulseSlotΪ2250slotһѭ����Ҳ��60sһѭ������
			//MinCount = gpsStoreCell.GPS_TimeM;                                                        //lq ������ARMʱ����GPS��ȫͬ���Ļ���ChkTimeS��GPSSyn_PulseSlot����ʾ��ʱ��Ӧ��ȫͬ��
			GpsSynCell.GPSSyn_SlotSynFlag = TRUE;
		}
        //lq? ż����ʱ��ʱ϶ͬ����

		//ʱ϶��λͬ����ÿ1s����
		if ((ChkTimeS & 0x01) == 0)//�����ż����                               //lq ż����ʱ���е���λͬ��
		{				
			uiTemp = (ChkTimeS >> 1) * 75;//ż�������Բ���Ҫ���Ǹ�37
			if (uiTemp > GpsSynCell.GPSSyn_PulseSlot)//������ĵ�ǰslotֵ����ͬ��ʱ�����ֵ //lq ��������ż����ʱ����ͬ�������Ա�׼��λ��һ��Ϊ0��or 1280����
			{
				GpsSynCell.GPSSyn_PhaseOffset = 1280 - GpsSynCell.GPSSyn_PulsePhase;    //lq ��ǰʱ���GPS��׼ʱ����������λ����Ϊ1280��
			}                                                                           //lq �������ط��Ὣ��λ��1280��Ϊ0��ͬʱ��ʱ϶��1
			else
			{
				GpsSynCell.GPSSyn_PhaseOffset = -GpsSynCell.GPSSyn_PulsePhase;  //lq ��ǰʱ���GPS��׼ʱ��죬����λ����Ϊ0��
			}						
			GpsSynCell.GPSSyn_PhaseSynFlag = TRUE;
		}
		else                                                                    //lq ������ʱ���е���λͬ��
		{
			GpsSynCell.GPSSyn_PhaseOffset = 640 - GpsSynCell.GPSSyn_PulsePhase;	//lq? ����ʱ��Ӧ����λ��Ϊ0 or 1280?					
			GpsSynCell.GPSSyn_PhaseSynFlag = TRUE;
		}
/*lq
		usart_printf("GpsSynCell.GPSSyn_PhaseOffset %d\r\n",GpsSynCell.GPSSyn_PhaseOffset);
		usart_printf("GpsSynCell.GPSSyn_SlotOffset %d\r\n",GpsSynCell.GPSSyn_SlotOffset);
*/
		if ((GpsSynCell.GPSSyn_PhaseOffset > -15)
		    && (GpsSynCell.GPSSyn_PhaseOffset < 15)
		    && (GpsSynCell.GPSSyn_SlotOffset == 0))
		 {
		    GpsCtrlPara.GpsSynCount++;
			usart_printf("Gps Syn Count %d\r\n",GpsCtrlPara.GpsSynCount);

            if (GpsCtrlPara.GpsSynCount >= GPS_SYN_COUNT_THRESHOLD)             //lq ����GPS_SYN_COUNT_THRESHOLD������������
			{                                                                   //lq ����Ϊͬ����ɡ�
				GpsCtrlPara.GPSSyn_Flag = TRUE;	
				GpsSynCell.GPSSyn_Flag = TRUE;
			}
		 }
         else 
         {
            GpsCtrlPara.GpsSynCount = 0;
         }
	}
}
#else
/*******************************************************************************
* ����  : SlotSynClac
* ����  : ʱ϶ͬ����ʱ϶��λͬ����ʱ϶����λͬʱͬ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SlotSynClac(void)
{
	U16 uiTemp;
	U16 ChkTimeS;
    S32 slPhaseDiffSum;

	if (GpsSynCell.GPSSyn_PulseEn == TRUE)                                      //lq GPS��λ��Ž���ͬ��
	{                                                                           
		GpsSynCell.GPSSyn_PulseEn = FALSE;                                      //lq ÿ�������壨ÿ1s����ͬ��1��

        ChkTimeS = GpsSynCell.ChkTimeS;
    	if (ChkTimeS >= 60)                                                     //lq ChkTimeSΪ��׼�루GPS�룩
		{
			ChkTimeS = 0;

#if CONFIG_GPS_SYNCH_HOUR_MIN
            GpsSynCell.ChkTimeM++;
            if (GpsSynCell.ChkTimeM >= 60)
            {
    			GpsSynCell.ChkTimeM = 0;
                GpsSynCell.ChkTimeH++;
                if (GpsSynCell.ChkTimeH >= 24)
                {
                    GpsSynCell.ChkTimeH = 0;
                }            
            }
#endif
		}

		/*lq ʱ϶��λͬ��*/
		if ((ChkTimeS & 0x01) == 1)	                                            //lq ������ͬ��
		{									
		    uiTemp = (ChkTimeS >> 1) * 75 + 37;
			
            /*lq ʱ϶ͬ��*/
            GpsSynCell.GPSSyn_SlotOffset = uiTemp - GpsSynCell.GPSSyn_PulseSlot;//lq ChkTimeSΪ60sһѭ����GPSSyn_PulseSlotΪ2250slotһѭ����Ҳ��60sһѭ������
                                                                                //lq ������ARMʱ����GPS��ȫͬ���Ļ���ChkTimeS��GPSSyn_PulseSlot����ʾ��ʱ��Ӧ��ȫͬ��
#if 1
    		/*lq �߽紦������ʱ϶�ȱ�׼ʱ϶��������׼ʱ϶Ϊ2250������Ϊ0��ʱ������ʱ϶С��2250*/
    		if (GpsSynCell.GPSSyn_SlotOffset <= -2000)//lq 2212
    		{
    			GpsSynCell.GPSSyn_SlotOffset += 2250;	
    		}
            else if (GpsSynCell.GPSSyn_SlotOffset >= 2000)
            {
    			GpsSynCell.GPSSyn_SlotOffset -= 2250;	            
            }
#endif

            /*lq ��λͬ��*/
			GpsSynCell.GPSSyn_PhaseOffset = 640 - GpsSynCell.GPSSyn_PulsePhase; //lq 1s����37.5slot����������ʱ��Ӧ����λֵΪ640��1slot=640phase��
            
            if (GpsSynCell.GPSSyn_Enable == TRUE)
            {
                GpsSynCell.ucGPSSyn_SlotPhaseSynFlag = TRUE;
#if CONFIG_GPS_SYNCH_HOUR_MIN                 
                /*lq Сʱ����ͬ��*/                
                if (GpsSynCell.GPSSyn_HourFrameSynEnable == TRUE)
                {
                    GpsSynCell.ChkTimeFrame = (GpsSynCell.ChkTimeH * 60 + GpsSynCell.ChkTimeM) % 1440;  //lq 1440=24*60
                    GpsSynCell.ucGPSSyn_HourFrameSynFlag = TRUE;                

			        debug_printf("$[%6d:%4d]GPSSyn_HourFrameSynFlag : OK!\r\n", GetFrameNum(), GetSlotNum());
                }
#endif
            }	        
    	}
        else                                                                    //lq ż����ͬ��
        {
			uiTemp = (ChkTimeS >> 1) * 75;
            
            /*lq ʱ϶ͬ��*/
            GpsSynCell.GPSSyn_SlotOffset = uiTemp - GpsSynCell.GPSSyn_PulseSlot;
#if 1
    		/*lq �߽紦������ʱ϶�ȱ�׼ʱ϶��������׼ʱ϶Ϊ2250������Ϊ0��ʱ������ʱ϶С��2250*/
    		if (GpsSynCell.GPSSyn_SlotOffset <= -2000)
    		{
    			GpsSynCell.GPSSyn_SlotOffset += 2250;	
    		}
            else if (GpsSynCell.GPSSyn_SlotOffset >= 2000)
            {
    			GpsSynCell.GPSSyn_SlotOffset -= 2250;	            
            }
#endif
            /*lq ��λͬ��*/
			GpsSynCell.GPSSyn_PhaseOffset = 0 - GpsSynCell.GPSSyn_PulsePhase;   //lq 1s����37.5slot����ż����ʱ��Ӧ����λֵΪ0

            if (GpsSynCell.GPSSyn_Enable == TRUE)
            {
                GpsSynCell.ucGPSSyn_SlotPhaseSynFlag = TRUE;
#if CONFIG_GPS_SYNCH_HOUR_MIN                
                /*lq Сʱ����ͬ��*/
                if (GpsSynCell.GPSSyn_HourFrameSynEnable == TRUE)
                {
                    GpsSynCell.ChkTimeFrame = (GpsSynCell.ChkTimeH * 60 + GpsSynCell.ChkTimeM) % 1440;  //lq 1440=24*60
                    GpsSynCell.ucGPSSyn_HourFrameSynFlag = TRUE;                

			        debug_printf("$[%6d:%4d]GPSSyn_HourFrameSynFlag : OK!\r\n", GetFrameNum(), GetSlotNum());
                }
#endif
            }
        }

		debug_printf("GPSTimeS     %d\r\n", GpsPara.GPS_TimeS);
		debug_printf("GPSTimeSLeap %d\r\n", GpsPara.GPS_TimeSLeap);
        debug_printf("ChkTimeS     %d\r\n", ChkTimeS);
    	debug_printf("GpsSynCell.GPSSyn_PulseSlot %d\r\n", GpsSynCell.GPSSyn_PulseSlot);
		debug_printf("GpsSynCell.GPSSyn_PhaseOffset %d\r\n", GpsSynCell.GPSSyn_PhaseOffset);
		debug_printf("GpsSynCell.GPSSyn_SlotOffset  %d\r\n", GpsSynCell.GPSSyn_SlotOffset);

        /*lq �ж�ͬ��ƫ��ֵ�Ƿ����趨��Χ��*/
		slPhaseDiffSum = GpsSynCell.GPSSyn_PhaseOffset + GpsSynCell.GPSSyn_SlotOffset * 1280;
        if (slPhaseDiffSum > -15 && slPhaseDiffSum < 15)
		{
		    if (GpsSynCell.ucGPSSyn_SlotPhaseSynFlag == FALSE)                  //lq ���֣��룩ͬ��У׼����ɣ�ͬ��ƫ��ֵ��0    
            {
                GpsSynCell.GPSSyn_SlotOffset = 0;
                GpsSynCell.GPSSyn_PhaseOffset = 0;		            
            }

            GpsCtrlPara.GpsSynCount++;
			if (GpsCtrlPara.GpsSynCount >= GPS_SYN_COUNT_THRESHOLD)             //lq ����GPS_SYN_COUNT_THRESHOLD������������
			{                                                                   //lq ����Ϊͬ����ɡ�
                GpsCtrlPara.GpsSynCount = GPS_SYN_COUNT_THRESHOLD;              //lq GPSһֱ����ʱ��GPSͬ����һֱ���У���ֹ�������
				GpsCtrlPara.GPSSyn_Flag = TRUE;	
				GpsSynCell.GPSSyn_Flag = TRUE;
			}

			debug_printf("Gps Syn Count %d\r\n", GpsCtrlPara.GpsSynCount);
		 }
         else 
         {
            GpsCtrlPara.GpsSynCount = 0;
         }
	}
}
#endif

/*******************************************************************************
* ����  : GetGpsSynSta
* ����  : ʱ϶ͬ����ʱ϶��λͬ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetGpsSynSta(void)
{
	if (GpsSynCell.GPSSyn_Flag == TRUE)
	{
		GpsSynCell.GPSSyn_Flag = FALSE;

		return (TRUE);
	}	

	return (FALSE);
}

/*******************************************************************************
* ����  : ResetGpsSynSta
* ����  : ʱ϶ͬ����ʱ϶��λͬ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void ResetGpsSynSta(void)
{
    GpsSynCell.GPSSyn_Flag = FALSE;
}

#if 0
/*******************************************************************************
* ����  : GetGpsMsgRequest
* ����  : GPS��Ϣ���������ѯ
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/	
U8 GetGpsMsgRequest(void)
{
	if (GpsCtrlPara.GpsMsgEnable == TRUE)
	{
		GpsCtrlPara.GpsMsgEnable = FALSE;
		return(TRUE);
	}
	else
	{
		return(FALSE);
	}			
}

/*******************************************************************************
* ����  : SetGpsMsgRequest
* ����  : GPS��Ϣ����������
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetGpsMsgRequest(void)
{
	GpsCtrlPara.GpsMsgEnable = TRUE;                                            //lq ������Э��ջ����Ϣ
}
#endif

/*******************************************************************************
* ����  : GetGpsPowerUpFixState
* ����  : ��ѯ��һ��GPS��λ��ͬ���Ƿ����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetGpsPowerupFixState(void)
{
	return (GpsCtrlPara.GpsPowerupFix);
}

/*******************************************************************************
* ����  : SetGpsPowerupFixState
* ����  : ��λ��һ��GPS��λ��ͬ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetGpsPowerupFixState(void)
{
	GpsCtrlPara.GpsPowerupFix = TRUE;
}

/*******************************************************************************
* ����  : GetGpsNormalFixState
* ����  : ��ѯ����GPS��λ��ͬ���Ƿ����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetGpsNormalFixStateRst(void)
{
	if (GpsCtrlPara.GpsNormalFix == TRUE)
	{
		GpsCtrlPara.GpsNormalFix = FALSE;
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}

/*******************************************************************************
* ����  : GetGpsNormalFixState
* ����  : ��ѯ����GPS��λ��ͬ���Ƿ����
* ����  : ��		  
* ���  : ��
* ����  : ����GPS��λ��ͬ���Ƿ����
*******************************************************************************/
U8 GetGpsNormalFixState(void)
{
    return GpsCtrlPara.GpsNormalFix;
}

/*******************************************************************************
* ����  : ResetGpsNormalFixState
* ����  : ��λGPS��λ��־
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void ResetGpsNormalFixState(void)
{
	GpsCtrlPara.GpsNormalFix = FALSE;
}

/*******************************************************************************
* ����  : SetGpsNormalFixState
* ����  : ��λGPS��λ��־
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetGpsNormalFixState(void)
{
	GpsCtrlPara.GpsNormalFix = TRUE;
}

#if 0
/*******************************************************************************
* ����  : GetGpsIdelState
* ����  : ��ѯGPS�Ƿ���Ϊ����ʱ������
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetGpsIdelState(void)
{
	if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_IDLE)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}		
}
#endif

/*******************************************************************************
* ����  : IsGpsFix
* ����  : �ж�GPS�Ƿ�λ
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 IsGpsFix(void)
{
	if (GpsPara.GPS_Stat == 'A')
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}

/*******************************************************************************
* ����  : IsGpsPowerupProtect
* ����  : GPS�Ƿ��ڿ�����λ����״̬��M4508D��Ϊ��λ״̬��,����ʱ��ƫ��ͳ�Ƽ�
          �㺯��TimAdjustProcess()�л���øú�����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 IsGpsPowerupProtect(void)
{
	if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_FIXED)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}

/*******************************************************************************
* ����  : SetGpsOnTimSlot
* ����  : ����GPS������ʱʱ϶
* ����  : value - GPS������ʱʱ϶		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetGpsOnTimSlot(S16 value)	
{
    if (value < 0)
    {
        value = 0;    
    }

    GpsCtrlPara.GpsOnTimSlot = value;
}	
	
/*******************************************************************************
* ����  : SetGpsOnTimFrame
* ����  : ����GPS������ʱ֡
* ����  : value - GPS������ʱ֡		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetGpsOnTimFrame(S32 value)	
{
    if (value < 0)
    {
        value = 0;    
    }

    GpsCtrlPara.GpsOnTimFrame = value;
}

/*******************************************************************************
* ����  : GetGpsOnTim
* ����  : ��ȡGPS��ʱ
* ����  : ��		  
* ���  : ��
* ����  : GPS��ʱ
*******************************************************************************/
S32 GetGpsOnTim(void)	
{
    return GpsCtrlPara.GpsOnTimFrame * 2250 + GpsCtrlPara.GpsOnTimSlot;
}	

/*******************************************************************************
* ����  : SetGpsOnTimEn
* ����  : GPS������ʱ��Ч��־��λ
* ����  : ��		  
* ���  : ��
* ����  : GPS��ʱ
*******************************************************************************/
void SetGpsOnTimEn(void)
{
    GpsCtrlPara.ucGpsOnTimEn = TRUE;
}

/*******************************************************************************
* ����  : ResetGpsOnTimEn
* ����  : GPS������ʱ��Ч��־��λ
* ����  : ��		  
* ���  : ��
* ����  : GPS��ʱ
*******************************************************************************/
void ResetGpsOnTimEn(void)
{
    GpsCtrlPara.ucGpsOnTimEn = FALSE;
}

/*******************************************************************************
* ����  : GetGpsOnTimEn
* ����  : ��ȡGPS������ʱ��Ч��־
* ����  : ��		  
* ���  : ��
* ����  : GPS������ʱ��Ч��־
*******************************************************************************/
U8 GetGpsOnTimEn(void)
{
    return GpsCtrlPara.ucGpsOnTimEn;
}

/*******************************************************************************
* ����  : GetGpsOnTimEnRst
* ����  : ��ȡGPS������ʱ��Ч��־������Ч��λ
* ����  : ��		  
* ���  : ��
* ����  : GPS������ʱ��Ч��־
*******************************************************************************/
U8 GetGpsOnTimEnRst(void)
{
    if (GetGpsOnTimEn() == TRUE)
    {
        ResetGpsOnTimEn();
        
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SetGpsOffTimSlot
* ����  : ����GPS�رն�ʱʱ϶
* ����  : value - GPS�رն�ʱʱ϶		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetGpsOffTimSlot(S16 value)	
{
    if (value < 0)
    {
        value = 0;    
    }

    GpsCtrlPara.GpsOffTimSlot = value;
}

/*******************************************************************************
* ����  : SetGpsOffTimFrame
* ����  : ����GPS�رն�ʱ֡
* ����  : value - GPS�رն�ʱ֡		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetGpsOffTimFrame(S32 value)	
{
    if (value < 0)
    {
        value = 0;    
    }

    GpsCtrlPara.GpsOffTimFrame = value;
}

/*******************************************************************************
* ����  : GetGpsOffTim
* ����  : ��ȡGPS��ʱ
* ����  : ��		  
* ���  : ��
* ����  : GPS��ʱ
*******************************************************************************/
S32 GetGpsOffTim(void)	
{
    return GpsCtrlPara.GpsOffTimFrame * 2250 + GpsCtrlPara.GpsOffTimSlot;
}
#if 0	
/*******************************************************************************
* ����  : SetUnfixPeriodTim
* ����  : ���ÿ���δ��λʱ1h���ڶ�ʱ
* ����  : value - ����ֵ	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetGpsUnfixPeriodTim(S32 value)	
{
    if (value < 0)
    {
        value = 0;    
    }

    GpsCtrlPara.GpsUnfixPeriodTim += value;
}

/*******************************************************************************
* ����  : ResetUnfixPeriodTim
* ����  : ��λ����δ��λʱ1h���ڶ�ʱ
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void ResetGpsUnfixPeriodTim(void)	
{
    GpsCtrlPara.GpsUnfixPeriodTim = 0;
}

/*******************************************************************************
* ����  : GetUnfixPeriodTim
* ����  : ��ȡ����δ��λʱֵ
* ����  : ��	  
* ���  : ��
* ����  : ����δ��λʱֵ
*******************************************************************************/
S32 GetGpsUnfixPeriodTim(void)	
{
    return GpsCtrlPara.GpsUnfixPeriodTim;
}
#endif
/*******************************************************************************
* ����  : GetGpsWakeupSta
* ����  : GPS���Ѷ�ʱ�Ƿ�
* ����  : ��	  
* ���  : ��
* ����  : GPS����״̬
*******************************************************************************/
U8 GetGpsWakeupSta(void)
{
    if (GetSlotSum() >= GetGpsOnTim())
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : GetGpsPowerSta
* ����  : GPS����״̬
* ����  : ��	  
* ���  : ��
* ����  : GPS����״̬
*******************************************************************************/
U8 GetGpsPowerSta(void)
{
    if (GpsCtrlPara.GpsPowerSta == GPS_CTRL_GPSPOWERUP)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : GpsPowerDown
* ����  : GPS�رմ���
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void GpsPowerDown(void)
{
    IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_OFF);
    
    GpsCtrlPara.GpsPowerSta = GPS_CTRL_GPSPOWERDOWN;
}

/*******************************************************************************
* ����  : ResetGps5minLngOpenEn
* ����  : GPS5min����ʹ�ܱ�־��λ
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void ResetGps5minLngOpenEn(void)
{
    GpsCtrlPara.ucGps5minLngOpenEn = FALSE;
}

/*******************************************************************************
* ����  : GetGps5minLngOpenEn
* ����  : ��ȡGPS5min����ʹ�ܱ�־
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetGps5minLngOpenEn(void)
{
    return GpsCtrlPara.ucGps5minLngOpenEn;
}

/*******************************************************************************
* ����  : SetGpsUnfix1hPeriodTimFrame
* ����  : GPSδ��λ1h��ʱ
* ����  : value - ��ʱֵ����λ��֡��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetGpsUnfix1hPeriodTimFrame(S32 value)
{
    if (value < 0)
    {
        value = GetFrameNum() + 60;
    }
    
    GpsCtrlPara.GpsUnfix1hPeriodTimFrame = value;
}

/*******************************************************************************
* ����  : SetGpsUnfix1hPeriodTimSlot
* ����  : GPSδ��λ1h��ʱ
* ����  : value - ��ʱֵ����λ��ʱ϶��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetGpsUnfix1hPeriodTimSlot(S16 value)
{
    if (value < 0)
    {
        value = GetSlotNum();
    }
    
    GpsCtrlPara.GpsUnfix1hPeriodTimSlot = value;
}

/*******************************************************************************
* ����  : GetGpsUnfix1hPeriodTim
* ����  : ��ȡGPSδ��λ1h��ʱ
* ����  : value - ��ʱֵ����λ��֡��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
S32 GetGpsUnfix1hPeriodTim(void)
{
    return GpsCtrlPara.GpsUnfix1hPeriodTimFrame * 2250 + GpsCtrlPara.GpsUnfix1hPeriodTimSlot;    
}

/*******************************************************************************
* ����  : SetGpsUnfixLonOpenFlg
* ����  : GPSδ��λ������־��λ
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetGpsUnfixLonOpenFlg(void)
{
    GpsCtrlPara.GpsWaitFix = TRUE;
}

/*******************************************************************************
* ����  : ResetGpsUnfixLonOpenFlg
* ����  : GPSδ��λ������־��λ
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void ResetGpsUnfixLonOpenFlg(void)
{
    GpsCtrlPara.GpsWaitFix = FALSE;
}

/*******************************************************************************
* ����  : GetGpsUnfixLonOpenFlg
* ����  : ��ȡGPSδ��λ������־
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetGpsUnfixLonOpenFlg(void)
{
    return GpsCtrlPara.GpsWaitFix;
}

/*******************************************************************************
* ����  : SetGpsUnfix1hLngOpenFlg
* ����  : GPSδ��λ1h������־��λ
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetGpsUnfix1hLngOpenFlg(void)
{
    GpsCtrlPara.ucGpsUnfix1hLngOpenFlg = TRUE;
}

/*******************************************************************************
* ����  : ResetGpsUnfix1hLngOpenFlg
* ����  : GPSδ��λ1h������־��λ
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void ResetGpsUnfix1hLngOpenFlg(void)
{
    GpsCtrlPara.ucGpsUnfix1hLngOpenFlg = FALSE;
}

/*******************************************************************************
* ����  : GetGpsUnfix1hLngOpenFlg
* ����  : ��ȡGPSδ��λ1h������־
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetGpsUnfix1hLngOpenFlg(void)
{
    return GpsCtrlPara.ucGpsUnfix1hLngOpenFlg;
}

/*******************************************************************************
* ����  : SetGpsUnfixBey1hLngOpenFlg
* ����  : GPSδ��λ����1h������־��λ
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SetGpsUnfixBey1hLngOpenFlg(void)
{
    GpsCtrlPara.ucGpsUnfixBey1hLngOpenFlg = TRUE;
}

/*******************************************************************************
* ����  : ResetGpsUnfixBey1hLngOpenFlg
* ����  : GPSδ��λ����1h������־��λ
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void ResetGpsUnfixBey1hLngOpenFlg(void)
{
    GpsCtrlPara.ucGpsUnfixBey1hLngOpenFlg = FALSE;
}

/*******************************************************************************
* ����  : GetGpsUnfixBey1hLngOpenFlg
* ����  : ��ȡGPSδ��λ����1h������־
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetGpsUnfixBey1hLngOpenFlg(void)
{
    return GpsCtrlPara.ucGpsUnfixBey1hLngOpenFlg;
}

/*******************************************************************************
* ����  : IsGpsOffTimeout
* ����  : �Ƿ�Gps�رն�ʱ��
* ����  : ��	  
* ���  : ��
* ����  : �Ƿ�Gps�رն�ʱ��
*******************************************************************************/
U8 IsGpsOffTimeout(void)
{
    if (GetSlotSum() >= GetGpsOffTim())
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : IsGpsOnTimeout
* ����  : �Ƿ�Gps������ʱ��
* ����  : ��	  
* ���  : ��
* ����  : �Ƿ�Gps������ʱ��
*******************************************************************************/
U8 IsGpsOnTimeout(void)
{
    if (GetSlotSum() >= GetGpsOnTim())
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : IsGpsUnfix1hPeriodTimeout
* ����  : �Ƿ�Gpsδ��λ1h��ʱʱ��
* ����  : ��	  
* ���  : ��
* ����  : �Ƿ�Gpsδ��λ1h��ʱʱ��
*******************************************************************************/
U8 IsGpsUnfix1hPeriodTimeout(void)
{
    if (GetSlotSum() >= GetGpsUnfix1hPeriodTim())
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }
}
#if 0
/*******************************************************************************
* Function Name  : SetGpsOnTim
* Description    : ����GPS������ʱ������ÿ������Ϣ����ʱ������GPS������ʱ
* Input          : usValue - GPS������ʱ�ͷ��䶨ʱ��ʱ϶��ֵ
* Output         : None
* Return         : None
*******************************************************************************/
void SetGpsOnTim(S16 usValue)
{
    S32 slSlotNums;

    if (usValue < 0)
    {
        usValue = SLOT_INTERVAL_GPS_ON_TO_TX;
    }
     
    /*lq GPS������ʱ��Ч*/
    if (GetGpsOnTimEn() == FALSE)
    {
        /*lq GPS�رպ�ſ�ȷ��GPS�Ķ�λ״̬������ȷ��GPS�Ļ�������*/
        if (GetGpsPowerSta() == FALSE)
        {
            /*lq GPSδ��λ1h���Ѷ�ʱ*/
            if (GetGpsUnfix1hLngOpenFlg() == TRUE)      
            {
                /*lq ��Ϣ�������ڼ���*/
                if (++GpsCtrlPara.GpsIntervalCnt > GPS_CTRL_PERIOD_UNFIX_1HOUR_LONG_OPEN)
                {
                    SetGpsOnEn();    
                    GpsCtrlPara.GpsIntervalCnt = 0;
                }
                else
                {
                    ResetGpsOnEn();    
                }
                
                if (GetGpsOnEn() == FALSE)
                {
                    /*lq ��GPSʡ���������㣬���¸����ڲ�����GPS*/
                    ResetGpsOnTimEn();                 
                }
                else
                {
                    slSlotNums = struSOTDMAPara.BurstTxTimFrame * 2250 + struSOTDMAPara.BurstTxTimSlot;
                    slSlotNums -= usValue;
                    if (slSlotNums < 0)
                    {
                        slSlotNums = 0;
                    }
                
                    SetGpsOnTimFrame(slSlotNums / 2250);
                    SetGpsOnTimSlot(slSlotNums % 2250);
    
                    /*lq GPS������ʱʹ��*/
                    SetGpsOnTimEn();        
                    SetGpsOnEn(); 

                    usart_printf("$GPS On Tim :[%6d:%4d]\r\n", GpsCtrlPara.GpsOnTimFrame, GpsCtrlPara.GpsOnTimSlot);                
                }
            }

            /*lq GPSδ��λ����1h���Ѷ�ʱ*/
            else if (GetGpsUnfixBey1hLngOpenFlg() == TRUE)
            {
                /*lq ��Ϣ�������ڼ���*/
                if (++GpsCtrlPara.GpsIntervalCnt > GPS_CTRL_PERIOD_UNFIX_BEY_1HOUR_LONG_OPEN)
                {
                    SetGpsOnEn();    
                    GpsCtrlPara.GpsIntervalCnt = 0;                    
                }
                else
                {
                    ResetGpsOnEn();    
                }
                
                if (GetGpsOnEn() == FALSE)
                {
                    /*lq ��GPSʡ���������㣬���¸����ڲ�����GPS*/
                    ResetGpsOnTimEn();                 
                }
                else
                {
                    slSlotNums = struSOTDMAPara.BurstTxTimFrame * 2250 + struSOTDMAPara.BurstTxTimSlot;
                    slSlotNums -= usValue;
                    if (slSlotNums < 0)
                    {
                        slSlotNums = 0;
                    }
                
                    SetGpsOnTimFrame(slSlotNums / 2250);
                    SetGpsOnTimSlot(slSlotNums % 2250);
    
                    /*lq GPS������ʱʹ��*/
                    SetGpsOnTimEn();        
                    SetGpsOnEn(); 

                    usart_printf("$GPS On Tim :[%6d:%4d]\r\n", GpsCtrlPara.GpsOnTimFrame, GpsCtrlPara.GpsOnTimSlot);                
                }
            }

            /*lq GPS�������Ѷ�ʱ*/
            else
            {
                /*lq GPSʡ��ģʽ�Ƿ���*/
                if (GetGpsSaveFlg() == TRUE)
                {
                    /*lq GPS�¸������Ƿ��迪��*/
                    if (GetGpsOnEn() == TRUE)
                    {
                        ResetGpsOnEn();    
                    }
                    else
                    {
                        SetGpsOnEn();    
                    }        
                }
                else
                {
                    SetGpsOnEn();    
                }

                if ((GetGpsSaveFlg() == TRUE) && (GetGpsOnEn() == FALSE))
                {
                    /*lq ��GPSʡ���������㣬���¸����ڲ�����GPS*/
                    ResetGpsOnTimEn();                 
                }
                else
                {
                    slSlotNums = struSOTDMAPara.BurstTxTimFrame * 2250 + struSOTDMAPara.BurstTxTimSlot;
                    slSlotNums -= usValue;
                    if (slSlotNums < 0)
                    {
                        slSlotNums = 0;
                    }
                
                    SetGpsOnTimFrame(slSlotNums / 2250);
                    SetGpsOnTimSlot(slSlotNums % 2250);
    
                    /*lq GPS������ʱʹ��*/
                    SetGpsOnTimEn();        
                    SetGpsOnEn(); 

                    usart_printf("$GPS On Tim :[%6d:%4d]\r\n", GpsCtrlPara.GpsOnTimFrame, GpsCtrlPara.GpsOnTimSlot);                
                }

            }
        }    
    }
}
#endif
/*******************************************************************************
* Function Name  : GetGpsOnEnRst
* Description    : ��ȡGPS����״̬,��Ϊ����λ
* Input          : None
* Output         : None
* Return         : GPS����״̬
*******************************************************************************/
U8 GetGpsOnEnRst(void)
{
    if (GpsCtrlPara.ucGpsOnEn == TRUE)
    {
        GpsCtrlPara.ucGpsOnEn = FALSE;
        
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* Function Name  : GetGpsOnEn
* Description    : ��ȡGPS����״̬
* Input          : None
* Output         : None
* Return         : GPS������־
*******************************************************************************/
U8 GetGpsOnEn(void)
{
    if (GpsCtrlPara.ucGpsOnEn == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* Function Name  : SetGpsOnEn
* Description    : ��λGPS������־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetGpsOnEn(void)
{
    GpsCtrlPara.ucGpsOnEn = TRUE;
}

/*******************************************************************************
* Function Name  : ResetGpsOnEn
* Description    : ��λGPS������־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetGpsOnEn(void)
{
    GpsCtrlPara.ucGpsOnEn = FALSE;
}

/*******************************************************************************
* Function Name  : SetGpsOffPositionSampleEn
* Description    : ��λGPSƫ�Ʋ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetGpsOffPositionSampleEn(void)
{
    GpsCtrlPara.GpsOffPositionSampleEn = TRUE;
}

/*******************************************************************************
* Function Name  : ResetGpsOffPositionSampleEn
* Description    : ��λGPSƫ�Ʋ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetGpsOffPositionSampleEn(void)
{
    GpsCtrlPara.GpsOffPositionSampleEn = FALSE;
}

/*******************************************************************************
* Function Name  : GetGpsOffPositionSampleEn
* Description    : ��ѯGPSƫ�Ʋ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
U8 GetGpsOffPositionSampleEn(void)
{
    return GpsCtrlPara.GpsOffPositionSampleEn;
}

/*******************************************************************************
* Function Name  : SetGpsOffPositionSampleEn
* Description    : ��λGPS�����µ��־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetGpsPositionSampleNewFlg(void)
{
    GpsCtrlPara.GpsPositionSampleNewFlg = TRUE;
}

/*******************************************************************************
* Function Name  : ResetGpsPositionSampleNewFlg
* Description    : ��λGPS�����µ��־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetGpsPositionSampleNewFlg(void)
{
    GpsCtrlPara.GpsPositionSampleNewFlg = FALSE;
}

/*******************************************************************************
* Function Name  : GetGpsPositionSampleNewFlgRst
* Description    : ��ѯGPS�����µ��־������λ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
U8 GetGpsPositionSampleNewFlgRst(void)
{
    if (GpsCtrlPara.GpsPositionSampleNewFlg == TRUE)
    {
        GpsCtrlPara.GpsPositionSampleNewFlg = FALSE;
        
        return TRUE;
    }
    else
    {
        return FALSE;    
    }
}

/*******************************************************************************
* Function Name  : GetGpsSaveFlgRst
* Description    : ��ȡGPSʡ���־,��Ϊ����λ
* Input          : None
* Output         : None
* Return         : GPSʡ���־
*******************************************************************************/
U8 GetGpsSaveFlgRst(void)
{
    if (GpsCtrlPara.ucGpsSaveFlg == TRUE)
    {
        GpsCtrlPara.ucGpsSaveFlg = FALSE;
        
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* Function Name  : GetGpsSaveFlg
* Description    : ��ȡGPSʡ���־
* Input          : None
* Output         : None
* Return         : GPSʡ���־
*******************************************************************************/
U8 GetGpsSaveFlg(void)
{
    if (GpsCtrlPara.ucGpsSaveFlg == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* Function Name  : SetGpsSaveFlg
* Description    : ��λGPSʡ���־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetGpsSaveFlg(void)
{
    GpsCtrlPara.ucGpsSaveFlg = TRUE;
}

/*******************************************************************************
* Function Name  : ResetGpsSaveFlg
* Description    : ��λGPSʡ���־
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetGpsSaveFlg(void)
{
    GpsCtrlPara.ucGpsSaveFlg = FALSE;
}

#if CONFIG_GPS_SAVE
/*******************************************************************************
* Function Name  : GpsSaveCondJudge
* Description    : GPSʡ�������ж�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GpsSaveCondJudge(void)
{
    if (GpsCtrlPara.GpsSaveJudgeTimeCnt == GetLocalClockTickProcess())
    {
        GpsCtrlPara.GpsSaveJudgeTimeCnt = SetTimePeriodProcess(GpsCtrlPara.GpsSaveJudgeTimePeriod);

        /*lq ����δ����*/
        if (GetGpsSaveFlg() == FALSE)
        {
            /*lq ������ʱ�������ж�*/
            if (eqEquipPara.WorkTimeSum + gslFrameCnt > GPS_SAVE_COND_WORKTIME)
            {
     	        GpsCtrlPara.GpsSaveJudgeCntWorkTime++;	    
                if (GpsCtrlPara.GpsSaveJudgeCntWorkTime > GPS_SAVE_COND_OK_TIME)
                {
      	            GpsCtrlPara.GpsSaveJudgeCntWorkTime = 0;
    
                    SetGpsSaveFlg();    //lq ����GPSʡ��ģʽ
			        usart_printf("[%6d:%4d]$Enter GPSSaveMode for WorkTime!\r\n", GetFrameNum(), GetSlotNum());	                                    
                }
            }
            else
            {
     	        GpsCtrlPara.GpsSaveJudgeCntWorkTime = 0;
            }  

            /*lq ��ص�ѹ�����ж�*/
            if (BS1_GetCurVoltValue() < GPS_SAVE_COND_BATTERYVOLT)
            {
     	        GpsCtrlPara.GpsSaveJudgeCntBatVolt++;	   
                if (GpsCtrlPara.GpsSaveJudgeCntBatVolt > GPS_SAVE_COND_OK_TIME)
                {
      	            GpsCtrlPara.GpsSaveJudgeCntBatVolt = 0;
    
                    SetGpsSaveFlg();    //lq ����GPSʡ��ģʽ
			        usart_printf("[%6d:%4d]$Enter GPSSaveMode for BatVolt!\r\n", GetFrameNum() ,GetSlotNum());
                }
            }
            else
            {
     	        GpsCtrlPara.GpsSaveJudgeCntBatVolt = 0;
            } 

            /*lq �¶������ж�*/
            if (struTemperSensorPara.fsTemper < GPS_SAVE_COND_TEMPER)   //lq? GetTemper()�ķ���ֵΪ�β��ԣ�
            {
     	        GpsCtrlPara.GpsSaveJudgeCntTemper++;	    
                if (GpsCtrlPara.GpsSaveJudgeCntTemper > GPS_SAVE_COND_OK_TIME)
                {
      	            GpsCtrlPara.GpsSaveJudgeCntTemper = 0;
    
                    SetGpsSaveFlg();    //lq ����GPSʡ��ģʽ
			        usart_printf("[%6d:%4d]$Enter GPSSaveMode for Temperature!\r\n", GetFrameNum() ,GetSlotNum());
                }
            }
            else
            {
     	        GpsCtrlPara.GpsSaveJudgeCntTemper = 0;
            }

            /*lq GPSʡ����������*/
            if (GetGpsSaveFlg() == TRUE)
            {
  	            GpsCtrlPara.GpsSaveJudgeCntWorkTime = 0;
  	            GpsCtrlPara.GpsSaveJudgeCntBatVolt = 0;
  	            GpsCtrlPara.GpsSaveJudgeCntTemper = 0;            
            }                              
        }
        else
        {
            /*lq ������ʱ�������ж�*/
            if (eqEquipPara.WorkTimeSum + gslFrameCnt <= GPS_SAVE_COND_WORKTIME)
            {
     	        GpsCtrlPara.GpsSaveJudgeCntWorkTime++;	    
                if (GpsCtrlPara.GpsSaveJudgeCntWorkTime > GPS_SAVE_COND_OK_TIME)
                {
      	            GpsCtrlPara.GpsSaveJudgeCntWorkTime = 0;
    
                    GpsCtrlPara.GpsSaveJudgeFlgWorkTime = TRUE;
			        usart_printf("[%6d:%4d]$GPSSaveMode WorkTime condition not meet!\r\n", GetFrameNum() ,GetSlotNum());
                }
            }
            else
            {
     	        GpsCtrlPara.GpsSaveJudgeCntWorkTime = 0;

                GpsCtrlPara.GpsSaveJudgeFlgWorkTime = FALSE;
            }  
            
            /*lq ��ص�ѹ�����ж�*/
            if (BS1_GetCurVoltValue() >= GPS_SAVE_COND_BATTERYVOLT)
            {
     	        GpsCtrlPara.GpsSaveJudgeCntBatVolt++;	   
                if (GpsCtrlPara.GpsSaveJudgeCntBatVolt > GPS_SAVE_COND_OK_TIME)
                {
      	            GpsCtrlPara.GpsSaveJudgeCntBatVolt = 0;
    
                    GpsCtrlPara.GpsSaveJudgeFlgBatVolt = TRUE;
			        usart_printf("[%6d:%4d]$GPSSaveMode BatVolt condition not meet!\r\n", GetFrameNum() ,GetSlotNum());
                }
            }
            else
            {
     	        GpsCtrlPara.GpsSaveJudgeCntBatVolt = 0;

                GpsCtrlPara.GpsSaveJudgeFlgBatVolt = FALSE;
            } 

            /*lq �¶������ж�*/
            if (struTemperSensorPara.fsTemper >= GPS_SAVE_COND_TEMPER)      //lq? GetTemper()�ķ���ֵΪ�β��ԣ�
            {
     	        GpsCtrlPara.GpsSaveJudgeCntTemper++;	    
                if (GpsCtrlPara.GpsSaveJudgeCntTemper > GPS_SAVE_COND_OK_TIME)
                {
      	            GpsCtrlPara.GpsSaveJudgeCntTemper = 0;
    
                    GpsCtrlPara.GpsSaveJudgeCFlgTemper = TRUE;
			        usart_printf("[%6d:%4d]$GPSSaveMode Temperature condition not meet!\r\n", GetFrameNum() ,GetSlotNum());
                }
            }
            else
            {
     	        GpsCtrlPara.GpsSaveJudgeCntTemper = 0;

                GpsCtrlPara.GpsSaveJudgeCFlgTemper = FALSE;
            }
            
            /*lq GPSʡ������������*/                     
            if (GpsCtrlPara.GpsSaveJudgeFlgWorkTime == TRUE
                && GpsCtrlPara.GpsSaveJudgeFlgBatVolt == TRUE
                && GpsCtrlPara.GpsSaveJudgeCFlgTemper == TRUE)
            {
                ResetGpsSaveFlg();  //lq �˳�GPSʡ��ģʽ

  	            GpsCtrlPara.GpsSaveJudgeCntWorkTime = 0;
  	            GpsCtrlPara.GpsSaveJudgeCntBatVolt = 0;
  	            GpsCtrlPara.GpsSaveJudgeCntTemper = 0; 
                
			    usart_printf("[%6d:%4d]$Exit GPSSaveMode!\r\n", GetFrameNum() ,GetSlotNum());	                                    
            }
        } 
    }
}
#endif

#if 0
/*******************************************************************************
* ����  : GpsCtrlProcess_BatTest
* ����  : GPS״̬������,������SART��ز���ģʽ��GPS���ơ������GpsCtrlProcess()
          ��������GPS����GPS_CTRL_STA_NORMAL_WORKING����״̬�󣬲��ٶ��������β�
          ��λ�������м����������һֱ��������ʱ��״̬������
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void GpsCtrlProcess_BatTest(void)
{
	U8  ucTemp[10];
	U8  ucTemp1[10];
    S16 ssCurrSlot;
    S32 slCurrFrame;	

    ssCurrSlot = GetSlotNum();
    slCurrFrame = GetFrameNum();

	/*lq GPSʱ϶��λͬ��*/
    SlotSynClac();
    
    /*lq GPSʡ�������ж�*/
    GpsSaveCondJudge();

    /*lq ��λ״̬��LEDָʾ������GPS������Ž���LEDָʾ*/
    if (GetGpsPowerSta() == TRUE)
    {
        if (IsGpsFix())
        {
            if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_GPS_FIX)
            {
    			LEDInit(LED_TYPE_GPS_FIX);
            }
        }
        else
        {
            if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_GPS_SEARCH)
            {
    			LEDInit(LED_TYPE_GPS_SEARCH);
            }                                                                      
        }
    }
    
    /*lq GPS�رպ�LED��GPSָʾֹͣ*/
    else
    {
        if ((LEDCtrlPara.LedGreenPara.ucType == LED_TYPE_GPS_SEARCH)
            || LEDCtrlPara.LedGreenPara.ucType == LED_TYPE_GPS_FIX) 
        {
            LEDInit(LED_TYPE_NONE);    
        }   
    }


    /*lq GPS״̬����*/
	if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_START)                	        //lq �豸����״̬
	{
        /*lq GPS��λ��ͬ��*/
        if (GpsCtrlPara.GPSSyn_Flag == TRUE)
        {
            GpsCtrlPara.GPSSyn_Flag = FALSE;

            /*lq GPS��λ�����رն�ʱ*/
            SetGpsOffTimSlot(ssCurrSlot);
            SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_FIX_LONG_OPEN);       //lq GPS��λ����5min

            /*lq GPS5min����������1*/
            GpsCtrlPara.ssGps5minLngOpenCnt++;
            if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
            {
                ResetGps5minLngOpenEn();

#ifdef GPS_CTRL_DEBUG_MSG
			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
            }
#if 0   //lq �ݲ�д��EEPROM����ֹÿ�ο������ϴμ�¼��ֵ����������
            /*lq ����GPS5min��������*/
			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
#endif
            GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIX_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
			usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif
        }
        else
        {
            /*lq GPSδ��λ�״γ����رն�ʱ*/
            SetGpsOffTimSlot(ssCurrSlot);
            SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_FIRST_LONG_OPEN);   //lq GPSδ��λ�״γ���5min

            /*lq GPSδ��λ1h��ʱ*/
            SetGpsUnfix1hPeriodTimSlot(ssCurrSlot);
            SetGpsUnfix1hPeriodTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_1_HOUR);

            /*lq GPS5min����������1*/
            GpsCtrlPara.ssGps5minLngOpenCnt++;
            if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
            {
                ResetGps5minLngOpenEn();

#ifdef GPS_CTRL_DEBUG_MSG
			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
            }
#if 0   //lq �ݲ�д��EEPROM����ֹÿ�ο������ϴμ�¼��ֵ����������
            /*lq ����GPS5min��������*/
			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
#endif
            GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
			usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif
        }
                                                                            	
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN)
    {
        /*lq GPS��λ��ͬ��*/
        if (GpsCtrlPara.GPSSyn_Flag == TRUE)
        {
            GpsCtrlPara.GPSSyn_Flag = FALSE;

            /*��λGPSδ��λ������־*/
            ResetGpsUnfixLonOpenFlg();
            ResetGpsUnfix1hLngOpenFlg();
            ResetGpsUnfixBey1hLngOpenFlg();
            
            /*lq GPS5min����ʹ��*/
            if (GetGps5minLngOpenEn() == TRUE)        
            {
                /*lq GPS��λ�����رն�ʱ*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_FIX_LONG_OPEN);   //lq GPS��λ����5min

                /*lq GPS5min����������1*/
                GpsCtrlPara.ssGps5minLngOpenCnt++;
                if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
                {
                    ResetGps5minLngOpenEn();
    
#ifdef GPS_CTRL_DEBUG_MSG
    			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
                }

                /*lq ����GPS5min��������*/
    			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
    		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
                
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIX_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                         
            }
            else
            {
                /*lq ת��GPS�ر�״̬*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif  
            }
        } 
        else
        {
            /*lq GPSδ��λ�״γ�����ʱ��*/
            if (IsGpsOffTimeout() == TRUE)
            {
                /*lq ��λGPSδ��ʱ1h������־*/
                SetGpsUnfixLonOpenFlg();
                SetGpsUnfix1hLngOpenFlg();

                /*lq ת��GPS�ر�״̬*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif 
            }
        }  
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_UNFIX_FIRST_HOUR_LNG_OPEN)
    {
        /*lq GPS��λ��ͬ��*/
        if (GpsCtrlPara.GPSSyn_Flag == TRUE)
        {
            GpsCtrlPara.GPSSyn_Flag = FALSE;
            
            /*lq ��λGPSδ��λ1h������־*/
            ResetGpsUnfixLonOpenFlg();
            ResetGpsUnfix1hLngOpenFlg();

            /*lq GPS5min����ʹ��*/
            if (GetGps5minLngOpenEn() == TRUE)        
            {
                /*lq GPS��λ�����رն�ʱ*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_FIX_LONG_OPEN);   //lq GPS��λ����5min

                /*lq GPS5min����������1*/
                GpsCtrlPara.ssGps5minLngOpenCnt++;
                if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
                {
                    ResetGps5minLngOpenEn();
    
#ifdef GPS_CTRL_DEBUG_MSG
    			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
                }

                /*lq ����GPS5min��������*/
    			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
    		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);

                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIX_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                    
            }
            else
            {
                /*lq ת��GPS�ر�״̬*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                  
            }
        }
        else
        {
            /*lq GPSδ��λ1h������ʱ��*/
            if (IsGpsOffTimeout() == TRUE)
            {
                /*lq 1h��ʱ��*/
                if (IsGpsUnfix1hPeriodTimeout() == TRUE)
                {
                    /*lq ��λGPSδ��1h������־*/
                    ResetGpsUnfix1hLngOpenFlg(); 

                    /*lq ��λGPSδ��λ����1h������־*/
                    SetGpsUnfixBey1hLngOpenFlg();
                }

                /*lq ת��GPS�ر�״̬*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                  
            }                                        
        }            
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_UNFIX_BEY_FIRST_HOUR_LNG_OPEN)
    {
        /*lq GPS��λ��ͬ��*/
        if (GpsCtrlPara.GPSSyn_Flag == TRUE)
        {
            GpsCtrlPara.GPSSyn_Flag = FALSE;

            /*lq ��λGPSδ��λ����1h������־*/
            ResetGpsUnfixLonOpenFlg();
            ResetGpsUnfixBey1hLngOpenFlg();

            /*lq GPS5min����ʹ��*/
            if (GetGps5minLngOpenEn() == TRUE)        
            {
                /*lq GPS��λ�����رն�ʱ*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_FIX_LONG_OPEN);   //lq GPS��λ����5min

                /*lq GPS5min����������1*/
                GpsCtrlPara.ssGps5minLngOpenCnt++;
                if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
                {
                    ResetGps5minLngOpenEn();
    
#ifdef GPS_CTRL_DEBUG_MSG
    			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
                }

                /*lq ����GPS5min��������*/
    			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
    		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);

                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIX_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                    
            }
            else
            {
                /*lq ת��GPS�ر�״̬*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                  
            }            
        }
        else
        {
            /*lq GPSδ��λ����1h������ʱ��*/
            if (IsGpsOffTimeout() == TRUE)
            {
                /*lq ת��GPS�ر�״̬*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                      
            }
        }                        
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_FIX_LNG_OPEN)
    {
        if (GetGpsPowerupFixState() == FALSE)
        {
            /*lq ��λGPS������λ��־*/
            SetGpsPowerupFixState();

			/*lq ��¼����ʱ��*/
            sprintf((char *)ucTemp, "%2d", GpsPara.GPS_TimeMon);
    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeD);
    		strcat((char *)ucTemp, (char *)ucTemp1);
            sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeH);
    		strcat((char *)ucTemp, (char *)ucTemp1);
    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeM);
    		strcat((char *)ucTemp, (char *)ucTemp1);
    		Para_Save(PARA_TIME_OPEN_TYPE, ucTemp, PARA_TIME_OPEN_MAXLEN);
        	Para_Save(PARA_TIME_CLOSE_TYPE, ucTemp, PARA_TIME_CLOSE_MAXLEN);                           //lq [2013/04/16]

			/*lq ����ػ�ԭ��*/
			sprintf((char *)ucTemp, "%d", 0);     		                        //lq ����ػ�ֻ����ΪĬ�Ϲػ���ʽ��ֻ�а����ػ��򳬵͵����ػ�ʱ���Ż��ٸ��Ĺػ�ԭ��
			Para_Save(PARA_CLOSE_RESION_TYPE, ucTemp, 1);
			
			/*lq ���GPS3��δ��λ����*/
			sprintf((char *)ucTemp, "%3d", 0);
		   	Para_Save(PARA_GPS_WAIT_COUNT_TYPE, ucTemp, 3);						

			/*lq ���GPS������λ����*/
			sprintf((char *)ucTemp, "%5d", 0);
		   	Para_Save(PARA_GPS_FIX_COUNT_TYPE, ucTemp, 5);
            
            /*lq ���GPS5min��������*/
			sprintf((char *)ucTemp, "%3d", 0);
		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
        } 
        
        /*lq GPS��λ������ʱ��*/ 
        if (IsGpsOffTimeout() == TRUE)
        {
            /*lq ����ʱ϶����λƫ�����ֵ����GPS������λ������ɺ���У�*/
			SetAdjustCalcNow();

            /*lq ת��GPS�ر�״̬*/            
            GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		    usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                
        }
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_NORMAL_WORKING)
    {
        /*lq ����Ϣδ��ʼ����*/
        if (GetBurstStarFlg() == FALSE)
        {
            /*lq GPS��λ��ͬ��*/
            if (GpsCtrlPara.GPSSyn_Flag == TRUE)
            {
                GpsCtrlPara.GPSSyn_Flag = FALSE;
                
                /*lq ��λ������λ��־*/
                SetGpsNormalFixState();

                /*lq ��λʧ�ܴ������㣬������λ������1*/
                GpsCtrlPara.GpsFallCount = 0;
                GpsCtrlPara.GpsFixCount++;
    			sprintf((char *)ucTemp, "%5d", GpsCtrlPara.GpsFixCount);
    		   	Para_Save(PARA_GPS_FIX_COUNT_TYPE, ucTemp, 5);//���Ϊ65536��

                /*lq ת��GPS�ر�״̬*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
			    usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                
            }
                    
        }
        else
        {
            /*lq ����Ϣ��ʼ���ͣ�GPS��λʧ��*/
            GpsCtrlPara.GpsFallCount = 0;   //lq ���Զ�λʧ�ܽ��м��������һֱ����������λ����״̬
            usart_printf("$Gps Fall Count : %d\r\n", GpsCtrlPara.GpsFallCount);	                
            
            // if (GpsCtrlPara.GpsFallCount >= eqEquipPara.GpsFixFallThres)
            if (0)  //lq ��Ϊÿ�ζ�λ���ɹ�
            {
            	/*lq GPS��λʧ�ܴ�������*/
                GpsCtrlPara.GpsFallCount = 0;
                
                /*lq GPSδ��λ����������1*/
                GpsCtrlPara.GpsWaitCount++;

                /*lq ����GPS3��δ��λ����*/
    			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.GpsWaitCount);
    		   	Para_Save(PARA_GPS_WAIT_COUNT_TYPE, ucTemp, 3);

                /*lq GPS5min����ʹ��*/
                if (GetGps5minLngOpenEn() == TRUE)        
                {
                    /*lq GPSδ��λ�״γ����رն�ʱ*/
                    SetGpsOffTimSlot(ssCurrSlot);
                    SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_FIRST_LONG_OPEN);   //lq GPSδ��λ�״γ���5min
            
                    /*lq GPSδ��λ1h��ʱ*/
                    SetGpsUnfix1hPeriodTimSlot(ssCurrSlot);
                    SetGpsUnfix1hPeriodTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_1_HOUR);

                    /*lq GPS5min����������1*/
                    GpsCtrlPara.ssGps5minLngOpenCnt++;
                    if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
                    {
                        ResetGps5minLngOpenEn();
        
#ifdef GPS_CTRL_DEBUG_MSG
        			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
                    }

                    /*lq ����GPS5min��������*/
        			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
        		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);

                    GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
			        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                       
                }
                else
                {
                    /*lq ��λδ��λ1h������־*/
                    SetGpsUnfixLonOpenFlg();
                    SetGpsUnfix1hLngOpenFlg();
                    
                    /*lq GPSδ��λ1h��ʱ*/    
                    SetGpsUnfix1hPeriodTimSlot(ssCurrSlot);
                    SetGpsUnfix1hPeriodTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_1_HOUR);

                    /*lq ת��GPS�ر�״̬*/
                    GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;           	 

#ifdef GPS_CTRL_DEBUG_MSG
			        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                       
                }
            } 
            else
            {
                /*lq ת��GPS�ر�״̬*/
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;           	 

#ifdef GPS_CTRL_DEBUG_MSG
			    usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                       
            }        
        }            
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_POWER_DOWN)
    {
        if (GetGpsPowerSta() == TRUE)
        {
            GpsPowerDown();

#ifdef GPS_CTRL_DEBUG_MSG
            UartResponseMsg("$GPS_POWER_OFF!");
#endif
        }
        
        /*lq ����Ϣ��ʼ����*/
        if (GetBurstStarFlg() == TRUE)
        {
            /*lq... GPS������ʱ*/
            SetGpsOnTim(SLOT_INTERVAL_GPS_ON_TO_TX);

            /*lq ת��GPS����״̬*/            
            GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_IDLE;

#ifdef GPS_CTRL_DEBUG_MSG
			usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_IDLE!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif      
        }     
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_IDLE)
    {
        /*lq GPS����״̬��Ч*/
        if (GetMsgGpsPowerup() == TRUE)
        {
            /*lq ����GPS��Դ������GPS������ز���*/                   
            GpsPowerUp();
#ifdef GPS_CTRL_DEBUG_MSG
            UartResponseMsg("$GPS_POWER_ON!");
#endif
            /*lq ����GPS��λ���ѡ����Ӧ��״̬*/
            if (GetGpsUnfix1hLngOpenFlg() == TRUE)                          //lq ת��GPSδ��λ1h����״̬
            {
                /*lq GPSδ��λ1h������ʱ*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_1HOUR_LONG_OPEN);   //lq GPSδ��λ1h�ڳ���1min                    

                /*lq ת��GPSδ��λ1h����״̬*/                
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIX_FIRST_HOUR_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_UNFIX_FIRST_HOUR_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif   
            }
            else if (GetGpsUnfixBey1hLngOpenFlg() == TRUE)                  //lq ת��GPSδ��λ����1h����״̬        
            {
                /*lq GPSδ��λ����1h������ʱ*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_BEY_1HOUR_LONG_OPEN);   //lq GPSδ��λ����1h����2min                    

                /*lq ת��GPSδ��λ����1h����״̬*/                
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIX_BEY_FIRST_HOUR_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_UNFIX_BEY_FIRST_HOUR_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif   
            }
            else                                                            //lq ת��GPS��������״̬ 
            {
                /*lq ת��GPS��������״̬*/                
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_NORMAL_WORKING;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_NORMAL_WORKING!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif   
            }
        }
    }
    else
    {
        /*lq ת��GPS��ʼ״̬*/                
        GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_START;

#ifdef GPS_CTRL_DEBUG_MSG
	    usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_START!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif   
    }    
}
#endif

/*******************************************************************************
* ����  : GetPPSGenFlag
* ����  : PPS���ɱ�־
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetGpsPPSGenFlag(void)
{
    return GpsPara.GPS_PPSGenFlag;
}

/*******************************************************************************
* ����  : GetGpsMsgGenFlag
* ����  : GPS��Ϣ���ɱ�־
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetGpsMsgGenFlag(void)
{
    return GpsPara.GPS_MsgGenFlag;
}

/*******************************************************************************
* ����  : GetGpsCtrlState
* ����  : GPS����״̬
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GetGpsCtrlState(void)
{
    return GpsCtrlPara.GpsCtrlSta;
}

/*******************************************************************************
* ����  : IsCoordinateOK
* ����  : ��λģʽ
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 IsCoordinateOK(void)
{
    if (GpsPara.GPS_FixMode == 3)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : IsPdopOK
* ����  : PDOP
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 IsPdopOK(void)
{
    if (GpsPara.GPS_PDOP < (GPS_DATA_PDOP_CEIL >> 1))
    {
        return TRUE;    //lq С�����ֵ��һ������ΪOK,���� too high
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : GPS_GpsSelfCheckStructInit
* ����  : ��ʼ���ṹ��
* ����  : pGpsSelfCheckStruct - GpsSelfCheckStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void GPS_GpsSelfCheckStructInit(GpsSelfCheckStruct *pGpsSelfCheckStruct)
{
	pGpsSelfCheckStruct->TimeCnt = 0;
	pGpsSelfCheckStruct->TimePeriod = GPS_SLFCHK_TIME_PERIOD;	
    pGpsSelfCheckStruct->TimeoutCnt = 0;

    pGpsSelfCheckStruct->CtrlState = GPS_SLFCHK_CTRL_STATE_IDLE;
    pGpsSelfCheckStruct->StartFlg = TRUE;
    pGpsSelfCheckStruct->CompleteFlg = FALSE;
    pGpsSelfCheckStruct->TimepulseFlg = FALSE; 
    pGpsSelfCheckStruct->DataCommFlg = FALSE; 
    pGpsSelfCheckStruct->OnDataCommFlg = FALSE; 
    pGpsSelfCheckStruct->OffDataCommFlg = FALSE; 
    pGpsSelfCheckStruct->AntennaFlg = FALSE; 
    pGpsSelfCheckStruct->ModuleFlg = TRUE; 

    pGpsSelfCheckStruct->VccCtrlOkFlg = FALSE; 
    pGpsSelfCheckStruct->TimepulseOkFlg = FALSE; 
    pGpsSelfCheckStruct->DataCommOkFlg = FALSE; 
    pGpsSelfCheckStruct->AntennaOkFlg = FALSE; 
    pGpsSelfCheckStruct->ModuleOkFlg = FALSE;           
}

/*******************************************************************************
* ����  : GPS_GpsSelfCheckReset
* ����  : �ṹ�帴λ
* ����  : pGpsSelfCheckStruct - GpsSelfCheckStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void GPS_GpsSelfCheckReset(GpsSelfCheckStruct *pGpsSelfCheckStruct)
{
    pGpsSelfCheckStruct->StartFlg = FALSE;
    pGpsSelfCheckStruct->CompleteFlg = FALSE;
    pGpsSelfCheckStruct->TimepulseFlg = FALSE; 
    pGpsSelfCheckStruct->DataCommFlg = FALSE; 
    pGpsSelfCheckStruct->OnDataCommFlg = FALSE; 
    pGpsSelfCheckStruct->OffDataCommFlg = FALSE; 
    pGpsSelfCheckStruct->AntennaFlg = FALSE; 
    pGpsSelfCheckStruct->ModuleFlg = TRUE; 

    pGpsSelfCheckStruct->VccCtrlOkFlg = FALSE; 
    pGpsSelfCheckStruct->TimepulseOkFlg = FALSE; 
    pGpsSelfCheckStruct->DataCommOkFlg = FALSE; 
    pGpsSelfCheckStruct->AntennaOkFlg = FALSE; 
    pGpsSelfCheckStruct->ModuleOkFlg = FALSE;         
}

/*******************************************************************************
* ����  : GPS_SelfCheck
* ����  : ��ʼ���ṹ��
* ����  : pGpsSelfCheckStruct - GpsSelfCheckStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void GPS_SelfCheck(GpsSelfCheckStruct *pGpsSelfCheckStruct)
{
	U32 ulTemp;

    switch (pGpsSelfCheckStruct->CtrlState)
    {
        case GPS_SLFCHK_CTRL_STATE_IDLE:
            
            if (pGpsSelfCheckStruct->StartFlg == TRUE)
            {
                GPS_GpsSelfCheckReset(pGpsSelfCheckStruct);

                /*lq GPS�ϵ� */
            	IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_OFF);      //lq �ϵ�ǰ�ȶϵ磬ȷ����GPS��Դ������������������£��ܹ���GPS�ϵ��Ŀ��������л�ȡGPS����״̬��GPSģ����Ϣ����������©GPS�ϵ��ı���
                Delay_ms(50);
                                
                /*lq GPS�ϵ� */
            	IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_ON);
        		pGpsSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pGpsSelfCheckStruct->TimePeriod);               
                pGpsSelfCheckStruct->CtrlState = GPS_SLFCHK_CTRL_STATE_POWERON;            
            }

            break; 
        case GPS_SLFCHK_CTRL_STATE_POWERON:

        	if (pGpsSelfCheckStruct->TimeCnt == GetLocalClockTickProcess())
        	{
        		pGpsSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pGpsSelfCheckStruct->TimePeriod);
                pGpsSelfCheckStruct->TimeoutCnt++;

				/* ����GPSģ���ϵ�� */
				if(pGpsSelfCheckStruct->TimeoutCnt == 2)
				{
					pGpsSelfCheckStruct->TimepulseFlg = FALSE;				
				}                
                if (pGpsSelfCheckStruct->TimeoutCnt >= GPS_SLFCHK_TIME_TIMEOUT)
                {
                    pGpsSelfCheckStruct->TimeoutCnt = 0;

                    /*lq ����ͨ�� */
                    if (pGpsSelfCheckStruct->DataCommFlg == TRUE)
                    {
                        pGpsSelfCheckStruct->DataCommOkFlg = TRUE;
                        pGpsSelfCheckStruct->OnDataCommFlg = TRUE;
                    }

                    /*lq ���� */
                    if (pGpsSelfCheckStruct->AntennaFlg == TRUE)
                    {
                        pGpsSelfCheckStruct->AntennaOkFlg = TRUE;
                    }

                    /*lq GPSģ�� */
                    if (pGpsSelfCheckStruct->ModuleFlg == TRUE)
                    {
                        pGpsSelfCheckStruct->ModuleOkFlg = TRUE;
                    }

					ulTemp = SLFCHK_GetCheckSelectValue(); 
					if((ulTemp & SLFCHK_ITEM_MASK_GPS_TIMEPULSE) != SLFCHK_ITEM_MASK_GPS_TIMEPULSE)
					{ 
						/*lq GPS�ϵ� */
	                	IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_OFF);
	                    pGpsSelfCheckStruct->DataCommFlg = FALSE;                                                  
	            		pGpsSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pGpsSelfCheckStruct->TimePeriod);

                    	pGpsSelfCheckStruct->CtrlState = GPS_SLFCHK_CTRL_STATE_POWEROFF; 
					}
					else
					{
						pGpsSelfCheckStruct->CtrlState = GPS_SLFCHK_CTRL_STATE_TIMEPULSE;
					}
                }                        
            }    

            break;
		case GPS_SLFCHK_CTRL_STATE_TIMEPULSE:
			if (pGpsSelfCheckStruct->TimeCnt == GetLocalClockTickProcess())
        	{
        		pGpsSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pGpsSelfCheckStruct->TimePeriod);
                pGpsSelfCheckStruct->TimeoutCnt++;
				if (pGpsSelfCheckStruct->TimeoutCnt >= GPS_SLFCHK_TIME_TIMEOUT_TIMEPULSE)
                {
					/*lq ������ */
                    if (pGpsSelfCheckStruct->TimepulseFlg == TRUE)
                    {
                        pGpsSelfCheckStruct->TimepulseOkFlg = TRUE;
                    }

					/*lq GPS�ϵ� */
                	IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_OFF);
                    pGpsSelfCheckStruct->DataCommFlg = FALSE;                                                  
            		pGpsSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pGpsSelfCheckStruct->TimePeriod);

                	pGpsSelfCheckStruct->CtrlState = GPS_SLFCHK_CTRL_STATE_POWEROFF;
				}
			}

			break;
        case GPS_SLFCHK_CTRL_STATE_POWEROFF:

        	if (pGpsSelfCheckStruct->TimeCnt == GetLocalClockTickProcess())
        	{
        		pGpsSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pGpsSelfCheckStruct->TimePeriod);
                pGpsSelfCheckStruct->TimeoutCnt++;                
                if (pGpsSelfCheckStruct->TimeoutCnt >= GPS_SLFCHK_TIME_TIMEOUT_POWERON)
                {
                    pGpsSelfCheckStruct->TimeoutCnt = 0;

                    /*lq ����ͨ�� */
                    if (pGpsSelfCheckStruct->DataCommFlg == TRUE)
                    {
                        pGpsSelfCheckStruct->OffDataCommFlg = TRUE;
                    }

                    /*lq GPS��Դ���� */
                    if ((pGpsSelfCheckStruct->OnDataCommFlg == TRUE)
                        && (pGpsSelfCheckStruct->OffDataCommFlg == FALSE))
                    {
                        pGpsSelfCheckStruct->VccCtrlOkFlg = TRUE;
                    }

                    pGpsSelfCheckStruct->CompleteFlg = TRUE;
                    
                    pGpsSelfCheckStruct->CtrlState = GPS_SLFCHK_CTRL_STATE_IDLE;                                                   
                }                        
            }    
            
            break;
        default :
            
            GPS_GpsSelfCheckStructInit(pGpsSelfCheckStruct);            
            break;
    }   
}
