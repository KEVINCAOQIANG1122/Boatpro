/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: LightBeaconProtocol.c
  Author: lq       Version : V1.00        Date: 2014.01.21
  Description:     Protocol between AIS and light beacon
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "def.h"
#include "LightBeaconProtocol.h"
#include "gpsctrl.h"
#include "initial.h"
#include "parastorage.h"
#include "config.h"

/*lq ģ��ʵ������*/
static LBPrtCommStruct LBPrtCommPara;

/*******************************************************************************
* ����  : LB_LBPrtCommStructInit
* ����  : LBPrtCommStruct�ṹ���ʼ��
* ����  : pLBPrtCommStruct-LBPrtCommStruct����ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void LB_LBPrtCommStructInit(LBPrtCommStruct *pLBPrtCommStruct)
{
    U16 i;

    pLBPrtCommStruct->TimePeriod = LB_PRT_TIME_REPETITION_INTERVAL;
    pLBPrtCommStruct->TimeCnt = 0;

    pLBPrtCommStruct->InterrogationFlag = FALSE;
#if 0
    for (i = 0; i < LB_PRT_SEND_FIFO_DEPTH; i++)
    {
        pLBPrtCommStruct->SendFIFO[i][0] = 0;
        pLBPrtCommStruct->SendFIFO[i][1] = 0;
    }
    pLBPrtCommStruct->SendFIFOIn = 0;
    pLBPrtCommStruct->SendFIFOOut = 0;
#endif
    for (i = 0; i < LB_PRT_REPET_BUFF_LEN; i++)
    {
        pLBPrtCommStruct->RepetBuff[i] = 0;
    }

    pLBPrtCommStruct->LB_Stat = 'V';
    pLBPrtCommStruct->LB_TimeY = 0;
	pLBPrtCommStruct->LB_TimeMon = 0;
	pLBPrtCommStruct->LB_TimeD = 0;
	pLBPrtCommStruct->LB_TimeH = 24;
	pLBPrtCommStruct->LB_TimeM = 63;
	pLBPrtCommStruct->LB_TimeS = 63;
	pLBPrtCommStruct->LB_Latitude = 0x3412140;                  //lq 91��N
	pLBPrtCommStruct->LB_Longitud = 0x6791AC0;                  //lq 181��E

    //lq UartRxFIFOStructInit(&(pUBXPrtCommStruct->Uart2RxFIFO2));
}

/*******************************************************************************
* ����  : LB1_LBPrtCommInstanceInit
* ����  : LBPrtCommStruct�ṹ���ʼ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void LB1_LBPrtCommInstanceInit(void)
{
    LB_LBPrtCommStructInit(&LBPrtCommPara);
}

/*******************************************************************************
* ����  : LB_LBPrtCommStructReset
* ����  : LBPrtCommStruct�ṹ�帴λ
* ����  : pLBPrtCommStruct-LBPrtCommStruct����ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void LB_LBPrtCommStructReset(LBPrtCommStruct *pLBPrtCommStruct)
{
    U16 i;

    pLBPrtCommStruct->TimePeriod = LB_PRT_TIME_REPETITION_INTERVAL;
    pLBPrtCommStruct->TimeCnt = 0;

    pLBPrtCommStruct->InterrogationFlag = FALSE;

    for (i = 0; i < LB_PRT_REPET_BUFF_LEN; i++)
    {
        pLBPrtCommStruct->RepetBuff[i] = 0;
    }

    pLBPrtCommStruct->LB_Stat = 'V';        
    pLBPrtCommStruct->LB_TimeY = 0;
	pLBPrtCommStruct->LB_TimeMon = 0;
	pLBPrtCommStruct->LB_TimeD = 0;
	pLBPrtCommStruct->LB_TimeH = 24;
	pLBPrtCommStruct->LB_TimeM = 63;
	pLBPrtCommStruct->LB_TimeS = 63;
	pLBPrtCommStruct->LB_Latitude = 0x3412140;                  //lq 91��N
	pLBPrtCommStruct->LB_Longitud = 0x6791AC0;                  //lq 181��E
}

/*******************************************************************************
* ����  : LB_SetInterrogationFlag
* ����  : ��λInterrogationFlag
* ����  : pLBPrtCommStruct-LBPrtCommStruct����ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void LB_SetInterrogationFlag(LBPrtCommStruct *pLBPrtCommStruct)
{
    pLBPrtCommStruct->InterrogationFlag = TRUE;
}

/*******************************************************************************
* ����  : LB_SetInterrogationFlag
* ����  : ��λInterrogationFlag
* ����  : pLBPrtCommStruct-LBPrtCommStruct����ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void LB1_SetInterrogationFlag(void)
{
    LB_SetInterrogationFlag(&LBPrtCommPara);
}

/*******************************************************************************
* ����  : LB_ResetInterrogationFlag
* ����  : ��λInterrogationFlag
* ����  : pLBPrtCommStruct-LBPrtCommStruct����ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void LB_ResetInterrogationFlag(LBPrtCommStruct *pLBPrtCommStruct)
{
    pLBPrtCommStruct->InterrogationFlag = FALSE;
}

/*******************************************************************************
* ����  : LB1_ResetInterrogationFlag
* ����  : ��λInterrogationFlag
* ����  : pLBPrtCommStruct-LBPrtCommStruct����ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void LB1_ResetInterrogationFlag(void)
{
    LB_ResetInterrogationFlag(&LBPrtCommPara);
}

/*******************************************************************************
* ����  : LB1_LBPrtCommStructReset
* ����  : LBPrtCommStruct�ṹ�帴λ
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void LB1_LBPrtCommStructReset(void)
{
    LB_LBPrtCommStructReset(&LBPrtCommPara);
}

#if 0
/*******************************************************************************
* ����  : LB_SendFIFOSearch
* ����  : �ڷ��Ͷ��в���ָ������������
* ����  : pLBPrtCommStruct-LBPrtCommStruct����ָ��
          CmdType-���ҵ��������ͺ�		  
* ���  : ��
* ����  : ���ҵ������ڶ����е������ţ�-1:δ�ҵ�
*******************************************************************************/
static S16 LB_SendFIFOSearch(LBPrtCommStruct *pLBPrtCommStruct, U8 CmdType)
{
    U16 Index;

    /*lq ����Ϊ��*/
    if (pLBPrtCommStruct->SendFIFOIn == pLBPrtCommStruct->SendFIFOOut)
    {
        return -1;    
    }
    else
    {
        Index = pLBPrtCommStruct->SendFIFOOut;
        while(Index != pLBPrtCommStruct->SendFIFOIn)
        {
            if (pLBPrtCommStruct->SendFIFO[Index][0] == CmdType)
            {
                return Index;   //lq ����������
            }
            else
            {
                Index++;
                Index %= LB_PRT_SEND_FIFO_DEPTH;
            }
        }

        return -1;
    }           
}

/*******************************************************************************
* ����  : LB_SendFIFOAdd
* ����  : ���Ͷ����������Ϣ
* ����  : pLBPrtCommStruct-LBPrtCommStruct����ָ��
          CmdType-����������ͺ�		  
          RepetCnt-�������Ӧ���ش�����
* ���  : ��
* ����  : ��
*******************************************************************************/
static U8 LB_SendFIFOAdd(LBPrtCommStruct *pLBPrtCommStruct, U8 CmdType, U8 RepetCnt)
{
    S16 Index;

    /*lq �Ȳ���Ҫ����������Ƿ��ڶ�����*/
    Index = LB_SendFIFOSearch(pLBPrtCommStruct, CmdType);
        
    if (Index < 0)   //lq ���ڶ�����
    {
        /*lq �������� */
        if ((pLBPrtCommStruct->SendFIFOIn + 1) % LB_PRT_SEND_FIFO_DEPTH
             == pLBPrtCommStruct->SendFIFOOut)
        {
            return FALSE;    
        }
        else
        {
            /*lq ��� */
            pLBPrtCommStruct->SendFIFO[pLBPrtCommStruct->SendFIFOIn][0] = CmdType;
            pLBPrtCommStruct->SendFIFO[pLBPrtCommStruct->SendFIFOIn][1] = RepetCnt;

            pLBPrtCommStruct->SendFIFOIn++;
            pLBPrtCommStruct->SendFIFOIn %= LB_PRT_SEND_FIFO_DEPTH; 
        }            
    }
    else
    {
        /*lq ���µ��ش������滻ԭ����ʣ����� */
        pLBPrtCommStruct->SendFIFO[Index][1] = RepetCnt;           
    }
}
#endif

/*******************************************************************************
* ����  : LB_LBPrtCommCtrlProcess
* ����  : ʹ��Light beacon Protocol ��light beaconͨ�ſ���
* ����  : pLBPrtCommStruct-LBPrtCommStruct����ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void LB_LBPrtCommCtrlProcess(LBPrtCommStruct *pLBPrtCommStruct)
{
    U8  Index;
    U8  Len;
	U8 	ucTempBuf[40];
    S32 slTemp;
    U8  ucTemp;
	char chRetBuf[100];
    U16 i;

    /*lq ����ͨ��ģʽ */
    if (pLBPrtCommStruct->InterrogationFlag == TRUE)
    {
        /*lq �������ڶ�ʱ�� */
        if (pLBPrtCommStruct->TimeCnt == GetLocalClockTickProcess())
        {
            pLBPrtCommStruct->TimeCnt = SetTimePeriodProcess(pLBPrtCommStruct->TimePeriod);

            /*lq ��Ҫ�ش�*/
            if (pLBPrtCommStruct->RepetBuff[0] > 0)
            {
                pLBPrtCommStruct->RepetBuff[0]--;                           //lq �ش�������1
                
                /*lq �ش������� */
                Len = pLBPrtCommStruct->RepetBuff[1];

                if (Len > 0)
                {
                    sprintf(chRetBuf, "$AIRDM");
                    for (Index = 2; Index < Len + 2; Index++)
                    {
                        switch (pLBPrtCommStruct->RepetBuff[Index])
                        {
                            case LB_PRT_RDM_DATE:
                            {                				
                                strcat(chRetBuf, ",");                
                                sprintf(ucTempBuf, "%02d", GpsPara.GPS_TimeD);
                                strcat(chRetBuf, (char *)ucTempBuf);
                                sprintf(ucTempBuf, "%02d", GpsPara.GPS_TimeMon);
                                strcat(chRetBuf, (char *)ucTempBuf);                                
                                sprintf(ucTempBuf, "%02d", GpsPara.GPS_TimeY);
                                strcat(chRetBuf, (char *)ucTempBuf);                                                    
                                break;
                            }
                            
                            case LB_PRT_RDM_UTC_TIME:
                			{	
                                strcat(chRetBuf, ",");                
                                sprintf(ucTempBuf, "%02d", GpsPara.GPS_TimeH);
                                strcat(chRetBuf, (char *)ucTempBuf);
                                sprintf(ucTempBuf, "%02d", GpsPara.GPS_TimeM);
                                strcat(chRetBuf, (char *)ucTempBuf);                                
                                sprintf(ucTempBuf, "%02d", GpsPara.GPS_TimeS);
                                strcat(chRetBuf, (char *)ucTempBuf);
                                strcat(chRetBuf, ".00");                    //lq ���С������Ϊ0               
                                //lq break;                                 //lq ����������ΪUBX_PRT_RDM_UTC_TIME����λ��γ��һ�𷵻�
                            }
                            
                            case LB_PRT_RDM_ANCHOR:
                			{	
                                strcat(chRetBuf, ",");
                                if (GpsPara.GPS_Stat == 'A')
                                {
                                    strcat(chRetBuf, "A");                                
                                }
                                else
                                {
                                    strcat(chRetBuf, "V");                                                                
                                } 
                                
                                slTemp = GpsPara.GPS_Latitude / 100.0;      //lq ���ȱ�Ϊ0.01��
                                if (slTemp < 0)
                                {
                                    if (ToLatFormat(-slTemp, ucTempBuf, 2))
                                    {
                                        strcat(chRetBuf, ",");                
                        				strcat(chRetBuf, (char *)ucTempBuf);
                                        strcat(chRetBuf, ",S");                
                                    }
                                    else
                                    {
                                        strcat(chRetBuf, ",,");             //lq ������Ч�����ؿ��ֶ�               
                                    }
                                }
                                else
                                {
                                    if (ToLatFormat(slTemp, ucTempBuf, 2))
                                    {
                                        strcat(chRetBuf, ",");                
                        				strcat(chRetBuf, (char *)ucTempBuf);
                                        strcat(chRetBuf, ",N");                
                                    } 
                                    else
                                    {
                                        strcat(chRetBuf, ",,");             //lq ������Ч�����ؿ��ֶ�               
                                    }                                   
                                }

                                slTemp = GpsPara.GPS_Longitud / 100.0;      //lq ���ȱ�Ϊ0.01��
                                if (slTemp < 0)
                                {
                                    if (ToLonFormat(-slTemp, ucTempBuf, 2))
                                    {
                                        strcat(chRetBuf, ",");                
                        				strcat(chRetBuf, (char *)ucTempBuf);
                                        strcat(chRetBuf, ",W");                
                                    }
                                    else
                                    {
                                        strcat(chRetBuf, ",,");             //lq ������Ч�����ؿ��ֶ�               
                                    }
                                }
                                else
                                {
                                    if (ToLonFormat(slTemp, ucTempBuf, 2))
                                    {
                                        strcat(chRetBuf, ",");                
                        				strcat(chRetBuf, (char *)ucTempBuf);
                                        strcat(chRetBuf, ",E");                
                                    } 
                                    else
                                    {
                                        strcat(chRetBuf, ",,");             //lq ������Ч�����ؿ��ֶ�               
                                    }                                   
                                }                                                                  
                                break;
                            }
                            
                            case LB_PRT_RDM_MMSI:
                			{	
                                strcat(chRetBuf, ",");                
                                sprintf(ucTempBuf, "%09d", siStaticPara.MMSI);
                                strcat(chRetBuf, (char *)ucTempBuf);                                                    
                                break;
                            }
                            
                            case LB_PRT_RDM_LIGHT_BEACON_TYPE:
                			{	
                                strcat(chRetBuf, ",");                
                                sprintf(ucTempBuf, "%0*d", PARA_ATON_TYPE_MAXLEN, siStaticPara.MMSI);
                                strcat(chRetBuf, (char *)ucTempBuf);                                                    
                                break;
                            }
                            
                            case LB_PRT_RDM_NAME:
                			{	
                                strcat(chRetBuf, ",");                
                                strcat(chRetBuf, siStaticPara.NameofAtoN);                                                    
                                break;
                            }
                            
                            case LB_PRT_RDM_AIS_STATUE:
                			{	
#if 0
                                strcat(chRetBuf, ",");
                                for (i = 0; i < 8; i++)
                                {
                                    ucTemp = siStaticPara.AtoNStatus >> (7 - i);    //lq 21����Ϣ�е�AtoN״̬
                                    ucTemp &= 0x1;                          //lq ��λ������λ 
                                    if (ucTemp == 0)
                                    {
                                        strcat(chRetBuf, "0");                                    
                                    }
                                    else
                                    {
                                        strcat(chRetBuf, "1");                                                                        
                                    }                                                                         
                                }
#else
                                strcat(chRetBuf, ",");
                                
                                /* lq RF antenna state */
                                if (RF1_GetRFAntState() == TRUE)                    
                                {
                                    strcat(chRetBuf, "0");                      //lq indicate OK                                    
                                }
                                else
                                {
                                    strcat(chRetBuf, "1");                      //lq indicate problem                                                  
                                }

                                /*lq GPS attenna state����Դ������Ϊʼ��OK*/
                                strcat(chRetBuf, "0");                          //lq indicate OK                                    

                                /*lq GPS receiver state */
                                if ((GetGpsCtrlState() != ATON_GPS_CTRL_STA_IDLE)
                                    && (GetGpsMsgGenFlag() == FALSE))
                                {
                                    strcat(chRetBuf, "1");                      //lq indicate problem
                                }
                                else
                                {
                                    strcat(chRetBuf, "0");                      //lq indicate OK
                                }
                                
                                /*lq GPS fix state*/                                    
                                if (IsGpsFix() == TRUE)
                                {
                                    strcat(chRetBuf, "0");                      //lq indicate OK                                    
                                }
                                else
                                {
                                    strcat(chRetBuf, "1");                      //lq indicate problem                                                  
                                }

                                /*lq coordinate state*/
                                if (IsCoordinateOK() == TRUE)
                                {
                                    strcat(chRetBuf, "0");                      //lq indicate OK                                    
                                }
                                else
                                {
                                    strcat(chRetBuf, "1");                      //lq indicate problem                                                  
                                }

                                /*lq PDOP */
                                if (IsPdopOK() == TRUE)
                                {
                                    strcat(chRetBuf, "0");                      //lq indicate OK                                    
                                }
                                else
                                {
                                    strcat(chRetBuf, "1");                      //lq indicate too high                                                  
                                }

                                /*lq PPS */
                                if (GetGpsPPSGenFlag() == TRUE)
                                {
                                    strcat(chRetBuf, "0");                      //lq indicate OK                                    
                                }
                                else
                                {
                                    strcat(chRetBuf, "1");                      //lq indicate too high                                                  
                                }
                                
                                /*lq Spare */
                                strcat(chRetBuf, "0");                      //lq indicate OK                                                                                                    
#endif                                               
                                break;
                            }
                            
                            case LB_PRT_RDM_USART_SETTING:
                			{	
                                strcat(chRetBuf, ",");                
                                sprintf(ucTempBuf, "%d", Uart1Config.Baud);
                                strcat(chRetBuf, (char *)ucTempBuf);
                                strcat(chRetBuf, ",");                                                                                                    
                                sprintf(ucTempBuf, "%d", Uart1Config.Data);
                                strcat(chRetBuf, (char *)ucTempBuf);
                                strcat(chRetBuf, ",");                                                                                                    
                                sprintf(ucTempBuf, "%d", Uart1Config.Parity);
                                strcat(chRetBuf, (char *)ucTempBuf);
                                strcat(chRetBuf, ",");                                                                                                    
                                sprintf(ucTempBuf, "%d", Uart1Config.Stop);
                                strcat(chRetBuf, (char *)ucTempBuf);                                                    
                                break;
                            }
                            
                            default :
                                break;
                        }   
                    } 
                    
				    /*lq ���ͱ��� */
                    UartResponseMsg(chRetBuf);		                                   
                }      
            }
        }
    }    
}

/*******************************************************************************
* ����  : LB1_LBPrtCommCtrlProcess
* ����  : ʹ��Light beacon Protocol ��light beaconͨ�ſ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void LB1_LBPrtCommCtrlProcess(void)
{
    LB_LBPrtCommCtrlProcess(&LBPrtCommPara);
}

/*******************************************************************************
* ���� ��LB_MsgDealProcess
* ���� ������Light Beacon Msg
* ���� ����
* ��� ����
* ���� ��TRUE-��Ϣ������Light Beacon Msg��FALSE-��Ϣ���Ĳ���Light Beacon Msg
*******************************************************************************/
static U8 LB_MsgDealProcess(LBPrtCommStruct *pLBPrtCommStruct, U8 *pDat, U16 uiLen)
{
	char chTemp[40];
	U16 i;
    U32 ulTemp;
    S32 slTemp;
    U8  ucTemp1, ucTemp2, ucTemp3;
	U8  ucTemp;
	U8 	ucCheck;
	U8 	ucLen;

    /*lq AIRDM����*/
    if (strncmp((char *)pDat, "AIRDM", 5) == FALSE)
    {
		/*lq ��ȡ��1�ֶ����ݣ�repetition time*/
		ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);        
        if (ucCheck == FALSE)
        {
            /*lq Ϊ�գ���ʾ��ֹ�ش� */            
            pLBPrtCommStruct->RepetBuff[0] = 0;                
        }
        else
        {
            ulTemp = atol(chTemp);
            if (ulTemp > 65534)
            {
                ulTemp = 65534;   
            }
            
            /*lq �����ش����� */
            pLBPrtCommStruct->RepetBuff[0] = ulTemp + 1;                
            
            /*lq ��ȡ�������ͺ� */
            for (i = 2; i < LB_PRT_REPET_BUFF_LEN; i++)
            {
		        /*lq ���λ�ȡ�����������ͺ� */
                ucCheck = GetSegment((char *)pDat, chTemp, i, uiLen, 30);        
                if (ucCheck != FALSE)
                {
                    /*lq ���ڱ����г��ֵ��Ⱥ�˳���¼�������ͺ� */
                    pLBPrtCommStruct->RepetBuff[i] = atol(chTemp);                
                }
                else
                {
                    break;
                }
            }

            /*lq ���������� */            
            if (i >= LB_PRT_REPET_BUFF_LEN)
            {
                pLBPrtCommStruct->RepetBuff[1] = LB_PRT_REPET_BUFF_LEN - 2;            
            }
            else
            {
                pLBPrtCommStruct->RepetBuff[1] = i - 2;            
            }
            
            /*lq ������Ӧ��ʱ*/ 
            pLBPrtCommStruct->TimeCnt = SetTimePeriodProcess(LB_PRT_TIME_RESPONSE_INTERVAL);
        }                
    }

    /*lq AIWRM����*/
    else if (strncmp((char *)pDat, "AIWRM", 5) == FALSE)
    {
		/*lq ��ȡ��1�ֶ����ݣ�cmd type*/
		ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);        
        if (ucCheck != FALSE)
        {
            ulTemp = atol(chTemp);
            switch (ulTemp)
            {
                case LB_PRT_WRM_DATE:
                {
            		/*lq ��ȡ��2�ֶ����ݣ������� */
            		ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);    				
                    if (ucCheck != FALSE)
                    {
                        pLBPrtCommStruct->LB_TimeD = (chTemp[0] - '0') * 10 + (chTemp[1] - '0');
        				pLBPrtCommStruct->LB_TimeMon = (chTemp[2] - '0') * 10 + (chTemp[3] - '0');
        				pLBPrtCommStruct->LB_TimeY = (chTemp[4] - '0') * 10 + (chTemp[5] - '0');                      
                    
                		UartResponseMsg("$AIWRM,OK");                        
                    }
                    else
                    {
                		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿
                        break;                                           
                    }
                    
                    break;                
                }    

                case LB_PRT_WRM_UTC_TIME:
                {
            		/*lq ��ȡ��2�ֶ����ݣ�ʱ���� */
            		ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);    				
                    if (ucCheck != FALSE)
                    {
        				pLBPrtCommStruct->LB_TimeH = (chTemp[0] - '0') * 10 + (chTemp[1] - '0');
        				pLBPrtCommStruct->LB_TimeM = (chTemp[2] - '0') * 10 + (chTemp[3] - '0');
        				pLBPrtCommStruct->LB_TimeS = (chTemp[4] - '0') * 10 + (chTemp[5] - '0');                     
                    }
                    else
                    {
                		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                        break;
                    }

            		/*lq ��ȡ��3�ֶ����ݣ���λ״̬ */
            		ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);    				
                    if (ucCheck != FALSE)
                    {
        				pLBPrtCommStruct->LB_Stat = chTemp[0];                     
                    }
                    else
                    {
                		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                        break;
                    }

        			/**************************Latitude******************/
        			ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
        
                    /*lq �ֶηǿ�*/
        			if (ucCheck != FALSE)
        			{
                        if (IsLatFormat(chTemp, &ulTemp, 4)== FALSE)
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                            break;                          
                        }

        			    /*lq -------N/S�ֶ�--------- */
        			    ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);
                        
                        /*lq �ֶηǿ�*/
            			if (ucCheck != FALSE)
            			{
                			/*lq ������Ч���ж�*/
                			ucLen = strlen(chTemp);
                			if (ucLen != 1)
                			{
                        		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                                break;
                			}

                            /*lq �ַ���Ч���ж�*/
                            if (chTemp[0] == 'N')
                            {
 			                    slTemp = ulTemp;                            
                            }
                            else if (chTemp[0] == 'S')
                            {
 			                    slTemp = -ulTemp;                            
                            }
                            else
                            {
                        		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                                break;                             
                            }

	                        pLBPrtCommStruct->LB_Latitude = slTemp;
                        }
                        else
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                            break;                          
                        }
        			}
                    else
                    {
        			    /*lq -------N/S�ֶ�--------- */
        			    ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);
                    
                        /*lq γ����ֵ�ֶ�Ϊ�գ���N/S�ֶηǿ�*/
        			    if (ucCheck != FALSE)
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                            break;                           
                        }
                    }

        			/**************************Longitude******************/
        			ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 30);
        
                    /*lq �ֶηǿ�*/
        			if (ucCheck != FALSE)
        			{
                        if (IsLonFormat(chTemp, &ulTemp, 4)== FALSE)
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                            break;                            
                        }

        			    /*lq -------E/W�ֶ�--------- */
                        ucCheck = GetSegment((char *)pDat, chTemp, 7, uiLen, 30);
                        
                        /*lq �ֶηǿ�*/
            			if (ucCheck != FALSE)
            			{
                			/*lq ������Ч���ж�*/
                			ucLen = strlen(chTemp);
                			if (ucLen != 1)
                			{
                        		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                                break; 
                			}

                            /*lq �ַ���Ч���ж�*/
                            if (chTemp[0] == 'E')
                            {
 			                    slTemp = ulTemp;                            
                            }
                            else if (chTemp[0] == 'W')
                            {
 			                    slTemp = -ulTemp;                            
                            }
                            else
                            {
                        		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                                break;                              
                            }

	                        pLBPrtCommStruct->LB_Longitud = slTemp;
                        }
                        else
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                            break;                       
                        }
        			}
                    else
                    {
        			    /*lq -------E/W�ֶ�--------- */
        			    ucCheck = GetSegment((char *)pDat, chTemp, 7, uiLen, 30);
                    
                        /*lq ������ֵ�ֶ�Ϊ�գ���E/W�ֶηǿ�*/
        			    if (ucCheck != FALSE)
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                            break;                       
                        }
                    }
                	
                    UartResponseMsg("$AIWRM,OK");                        
                    break;                
                } 

                case LB_PRT_WRM_ANCHOR:
                {
            		/*lq ��ȡ��2�ֶ����ݣ���λ״̬ */
            		ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);    				
                    if (ucCheck != FALSE)
                    {
        				pLBPrtCommStruct->LB_Stat = chTemp[0];                     
                    }
                    else
                    {
                		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                        break;
                    }

        			/**************************Latitude******************/
        			ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
        
                    /*lq �ֶηǿ�*/
        			if (ucCheck != FALSE)
        			{
                        if (IsLatFormat(chTemp, &ulTemp, 4)== FALSE)
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                            break;                          
                        }

        			    /*lq -------N/S�ֶ�--------- */
        			    ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
                        
                        /*lq �ֶηǿ�*/
            			if (ucCheck != FALSE)
            			{
                			/*lq ������Ч���ж�*/
                			ucLen = strlen(chTemp);
                			if (ucLen != 1)
                			{
                        		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                                break;
                			}

                            /*lq �ַ���Ч���ж�*/
                            if (chTemp[0] == 'N')
                            {
 			                    slTemp = ulTemp;                            
                            }
                            else if (chTemp[0] == 'S')
                            {
 			                    slTemp = -ulTemp;                            
                            }
                            else
                            {
                        		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                                break;                             
                            }

	                        pLBPrtCommStruct->LB_Latitude = slTemp;
                        }
                        else
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                            break;                          
                        }
        			}
                    else
                    {
        			    /*lq -------N/S�ֶ�--------- */
        			    ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
                    
                        /*lq γ����ֵ�ֶ�Ϊ�գ���N/S�ֶηǿ�*/
        			    if (ucCheck != FALSE)
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                            break;                           
                        }
                    }

        			/**************************Longitude******************/
        			ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);
        
                    /*lq �ֶηǿ�*/
        			if (ucCheck != FALSE)
        			{
                        if (IsLonFormat(chTemp, &ulTemp, 4)== FALSE)
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                            break;                            
                        }

        			    /*lq -------E/W�ֶ�--------- */
                        ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 30);
                        
                        /*lq �ֶηǿ�*/
            			if (ucCheck != FALSE)
            			{
                			/*lq ������Ч���ж�*/
                			ucLen = strlen(chTemp);
                			if (ucLen != 1)
                			{
                        		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                                break; 
                			}

                            /*lq �ַ���Ч���ж�*/
                            if (chTemp[0] == 'E')
                            {
 			                    slTemp = ulTemp;                            
                            }
                            else if (chTemp[0] == 'W')
                            {
 			                    slTemp = -ulTemp;                            
                            }
                            else
                            {
                        		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                                break;                              
                            }

	                        pLBPrtCommStruct->LB_Longitud = slTemp;
                        }
                        else
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                            break;                       
                        }
        			}
                    else
                    {
        			    /*lq -------E/W�ֶ�--------- */
        			    ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 30);
                    
                        /*lq ������ֵ�ֶ�Ϊ�գ���E/W�ֶηǿ�*/
        			    if (ucCheck != FALSE)
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿                    
                            break;                       
                        }
                    }
                	
                    UartResponseMsg("$AIWRM,OK");                        
                    break;                
                } 

                case LB_PRT_WRM_MMSI:
                {
		            /*lq ��ȡ��2�ֶ����ݣ���ȡMMSI*/
					ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
					if (ucCheck == FALSE)
					{
                		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿
                        break; 
					}

                    /*lq ������Ч���ж�*/
					ucLen = strlen(chTemp);
					if (ucLen != 9)
					{
                		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿
                        break; 
					}

                    /*lq ��ֵ��Ч���ж�*/
                    if (IsAscStringOk(chTemp) == FALSE)
                    {
                		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿
                        break;                             
                    }

					/*lq д��MMSI�� */
					Para_Save(PARA_MMSI_TYPE, (U8 *)chTemp, ucLen);
					Para_Save(PARA_MMSI_BACKUP_TYPE, (U8 *)chTemp, ucLen);//ͬʱд����

					/*lq �˴����Ӹ���Э��ջ�ӿ� */
					siStaticPara.MMSI = atol(chTemp);
					siStaticPara.Refresh = TRUE;

            		UartResponseMsg("$AIWRM,OK");
                    
                    break;                
                }
                
                case LB_PRT_WRM_LIGHT_BEACON_TYPE:
                {
       			    /***********************Type of AtoN***********************/
                    ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
        
        			/*lq �ֶηǿ�*/
                    if (ucCheck != FALSE)
        			{
            			/*lq ������Ч���ж�*/
                        ucLen = strlen(chTemp);
            			if (ucLen != 2)
            			{
                    		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿
                            break; 
            			}
                        			
                        /*lq �ַ���Ч���ж�*/
                        ucCheck = IsAscStringOk(chTemp);
            			if (ucCheck == FALSE)
            			{
                    		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿
                            break; 
            			}
                        else
                        {
                            /*lq ȡֵ��Χ��Ч���ж�*/
                            ulTemp = atol(chTemp);
                            if (ulTemp < PARA_ATON_TYPE_FLOOR
                                || ulTemp > PARA_ATON_TYPE_CEIL)
                            {
                        		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿
                                break;                
                            }            
                        }
            
            			Para_Save(PARA_ATON_TYPE_TYPE, (U8 *)chTemp, ucLen);
            			siStaticPara.TypeofAtoN = atoi(chTemp);
                        siStaticPara.Refresh = TRUE;
        			}

            		UartResponseMsg("$AIWRM,OK");
                    
                    break;                
                }

                case LB_PRT_WRM_NAME:
                {
        			/******************************Name of AtoN************************/
                    ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 40);
        
                    /*lq �ֶηǿ�*/
        			if (ucCheck != FALSE)
        			{
            			/*lq ������Ч���ж�*/
                        ucLen = strlen(chTemp);
            			if (ucLen > PARA_ATON_NAME_MAXLEN)
            			{
                    		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿
                            break;   
            			}
                        			
            			Para_Save(PARA_ATON_NAME_TYPE, (U8 *)chTemp, ucLen);
            			strcpy(siStaticPara.NameofAtoN, chTemp);
					    siStaticPara.Refresh = TRUE;
        			}

            		UartResponseMsg("$AIWRM,OK");
                    
                    break;                
                }

                case LB_PRT_WRM_ATON_STATUE:
                {
                    /***************************AtoN status bits***********************/
        			ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
        			
                    /*lq �ֶηǿ�*/
                    if (ucCheck != FALSE)
        			{
            			/*lq ������Ч���ж�*/
            			ucLen = strlen(chTemp);
            			if (ucLen != 8)
            			{
                    		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿
                            break;  
            			}
            
                        /*lq �ַ���Ч���ж�*/
                        if (IsHexStringOk(chTemp) == FALSE)
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿
                            break;     			                                   
                        }
                        else
                        {
                            ucTemp = 0;
                            for (i = 0; i < 8; i++)
                            {
                                ucTemp <<= 1;
                                ucTemp |= ((chTemp[i] - '0') & 0x1);
                            }
                            siStaticPara.AtoNStatus = ucTemp; 
					        siStaticPara.Refresh = TRUE;                                    
                        }
        			}

            		UartResponseMsg("$AIWRM,OK");
                    
                    break;                
                }

                case LB_PRT_WRM_USART_SETTING:
                {
        		    /*lq baud rate*/
                	ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
                    if (ucCheck != FALSE)
        			{
                        ulTemp = atol(chTemp);
        			}
                    else
                    {
                		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿
                        break;                     
                    }

        		    /*lq data length*/
                	ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
                    if (ucCheck != FALSE)
        			{
                        ucTemp1 = atoi(chTemp);
        			}
                    else
                    {
                		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿
                        break;                     
                    }

        		    /*lq parity*/
                	ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
                    if (ucCheck != FALSE)
        			{
                        ucTemp2 = atoi(chTemp);
        			}
                    else
                    {
                		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿
                        break;                     
                    }

        		    /*lq stop bits*/
                	ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);
                    if (ucCheck != FALSE)
        			{
                        ucTemp3 = atoi(chTemp);
        			}
                    else
                    {
                		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿
                        break;                     
                    }

                    /*lq �޸�USART1����*/
                    if (Usart1_UsartConfig(ulTemp, ucTemp1, ucTemp2, ucTemp3) == FALSE)
                    {
                 		UartResponseMsg("$AIWRM,301");      //lq?������뺬�壿
                        break;                   
                    }
                    else
                    {
            		    /*lq �޸�USART1���ýṹ������*/
                        USART1_UsartConfigStructInit(ulTemp, ucTemp1, ucTemp2, ucTemp3);

                        UartResponseMsg("$AIWRM,OK");                    
                    }
                    
                    break;                
                }
                                                                                                                
                default :
                    break;
            }       
        }   
    }

    /*lq AICDM����*/
    else if (strncmp((char *)pDat, "AICDM", 5) == FALSE)
    {
		/*lq ��ȡ��1�ֶ����ݣ�cmd type*/
		ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);        
        if (ucCheck != FALSE)
        {
            ulTemp = atol(chTemp);
            switch (ulTemp)
            {
                case LB_PRT_CDM_START_INTERROGATION:
                {               
                    /*lq ��λ��ѯ��־*/
                    pLBPrtCommStruct->InterrogationFlag = TRUE;                    
                    UartResponseMsg("$AICDM,OK");                                            
                    break;
                }
                case LB_PRT_CDM_AIS_SLEEP:
                {
                    //lq... ��stop interrogation�����Ч��һ��
                    /*lq ��λ�ṹ������ */
                    LB_LBPrtCommStructReset(pLBPrtCommStruct);
                    UartResponseMsg("$AICDM,OK");                                            
                    break;
                }
                case LB_PRT_CDM_STOP_INTERROGATION:
                {
                    /*lq ��λ�ṹ������ */
                    LB_LBPrtCommStructReset(pLBPrtCommStruct);                                        
                    UartResponseMsg("$AICDM,OK");                                                                
                    break;
                }                                                
                default :
                    break;
            }
        }
    }

    else
    {
        return FALSE;
    }

    return TRUE;
}

/*******************************************************************************
* ���� ��LB_MsgDealProcess
* ���� ������Light Beacon Msg
* ���� ����
* ��� ����
* ���� ��TRUE-��Ϣ������Light Beacon Msg��FALSE-��Ϣ���Ĳ���Light Beacon Msg
*******************************************************************************/
U8 LB1_MsgDealProcess(U8 *pDat, U16 uiLen)
{
    return LB_MsgDealProcess(&LBPrtCommPara, pDat, uiLen);    
}

/*******************************************************************************
* ���� ��LB_GetLBCommSleepSta
* ���� ����ѯlight beaconͨ��ģ����������
* ���� ��pLBPrtCommStruct��LBPrtCommStruct����ָ��
* ��� ����
* ���� ����
*******************************************************************************/
static U8 LB_GetLBCommSleepSta(LBPrtCommStruct *pLBPrtCommStruct)
{
    if (pLBPrtCommStruct->InterrogationFlag == FALSE)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ���� ��LB1_GetLBCommSleepSta
* ���� ����ѯlight beaconͨ��ģ����������
* ���� ����
* ��� ����
* ���� ����
*******************************************************************************/
U8 LB1_GetLBCommSleepSta(void)
{
    return LB_GetLBCommSleepSta(&LBPrtCommPara);    
}
