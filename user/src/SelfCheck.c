/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: SelfCheck.c
  Author: lq       Version : V1.00        Date: 2014.07.17
  Description:     �豸�Լ�ģ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "def.h"
#include "SelfCheck.h"
#include "systemtick.h"
#include "24c02.h"
#include "gpsctrl.h"
#include "batteryctrl.h"
#include "rtc.h"
#include "ledctrl.h"
#include "Si446x.h"
#include "RfLinkSelfCheck.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
static SelfCheckStruct SelfCheckInst;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
 
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* ����  : SLFCHK_SelfCheckStructInit
* ����  : ��ʼ���ṹ��
* ����  : pSelfCheckStruct - SelfCheckStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SLFCHK_SelfCheckStructInit(SelfCheckStruct *pSelfCheckStruct)
{
	pSelfCheckStruct->TimeCnt = 0;
	pSelfCheckStruct->TimePeriod = SLFCHK_TIME_PERIOD;	
    pSelfCheckStruct->TimeoutCnt = 0;
        
    pSelfCheckStruct->CtrlState = SLFCHK_CTRL_STATE_IDLE;
    pSelfCheckStruct->StartFlg = FALSE;
    pSelfCheckStruct->CompleteFlg = FALSE;
    pSelfCheckStruct->CheckResult = 0;
    pSelfCheckStruct->CompleteBitFlg = 0;
    pSelfCheckStruct->CheckSelect = SLFCHK_ITEM_MASK_ALL;
/*lq
    pSelfCheckStruct->PowerSupplyVoltOkFlg = FALSE;
    pSelfCheckStruct->RtcLseOkFlg = FALSE;
    pSelfCheckStruct->GpsVccCtrlOkFlg = FALSE;
    pSelfCheckStruct->GpsTimePulseOkFlg = FALSE;
    pSelfCheckStruct->GpsCommOkFlg = FALSE;
    pSelfCheckStruct->GpsAntennaOkFlg = FALSE;
    pSelfCheckStruct->GpsSelfCheckOkFlg = FALSE;
    pSelfCheckStruct->Rda1846VccCtrlOkFlg = FALSE;
    pSelfCheckStruct->Rda1846CommOkFlg = FALSE;
    pSelfCheckStruct->EepromWriteProtectOkFlg = FALSE;
    pSelfCheckStruct->EepromCommOkFlg = FALSE;
*/
}

/*******************************************************************************
* ����  : SLFCHK_InstSelfCheckStructInit
* ����  : ��ʼ���ṹ��
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SLFCHK_InstSelfCheckStructInit(void)
{
    SLFCHK_SelfCheckStructInit(&SelfCheckInst);
}

/*******************************************************************************
* ����  : SLFCHK_GetBitNum32
* ����  : ��������ֵ����λ���
* ����  : bitMask - λ����(λ�����е�1��������)	  
* ���  : ��
* ����  : ��������Ӧ��λ��ţ���λ���������λ��1��λ��ţ�
*******************************************************************************/
U8 SLFCHK_GetBitNum32(const U32 bitMask)
{
    U8 i;
       
    for (i = 0; i < 32; i++)
    {
        if (((bitMask >> i) & 0x00000001) != 0)
        {
            break;
        }
    } 
    
    return i;   
}

/*******************************************************************************
* ����  : SLFCHK_GetCheckResultValue
* ����  : ��ȡ�Լ�����ĳһ���״̬
* ����  : pSelfCheckStruct - SelfCheckStruct�ṹ��ָ��
          bitMask - �ֶζ�Ӧ��λ����	  
* ���  : ��
* ����  : λ�����Ӧ��ֵ
*******************************************************************************/
static U8 SLFCHK_GetCheckResultValue(const SelfCheckStruct *pSelfCheckStruct, const U32 bitMask)
{
    U32 temp32;

    temp32 = pSelfCheckStruct->CheckResult & bitMask;
    temp32 >>= SLFCHK_GetBitNum32(bitMask);
    
    return temp32;  
}

/*******************************************************************************
* ����  : SLFCHK_GetCheckSelect
* ����  : ��ȡ�Լ�ѡ����ĳһ���״̬
* ����  : pSelfCheckStruct - SelfCheckStruct�ṹ��ָ��
          bitMask - �ֶζ�Ӧ��λ����	  
* ���  : ��
* ����  : λ�����Ӧ��ֵ
*******************************************************************************/
static U8 SLFCHK_GetCheckSelect(const SelfCheckStruct *pSelfCheckStruct, const U32 bitMask)
{
    U32 temp32;

    temp32 = pSelfCheckStruct->CheckSelect & bitMask;
    temp32 >>= SLFCHK_GetBitNum32(bitMask);
    
    return temp32;  
}

/*******************************************************************************
* ����  : SLFCHK_InstGetCheckResultValue
* ����  : ��ȡ�Լ�����ĳһ���״̬
* ����  : bitMask - �ֶζ�Ӧ��λ����	  
* ���  : ��
* ����  : λ�����Ӧ��ֵ
*******************************************************************************/
U8 SLFCHK_InstGetCheckResultValue(const U32 bitMask)
{ 
    return SLFCHK_GetCheckResultValue(&SelfCheckInst, bitMask);  
}

/*******************************************************************************
* ����  : SLFCHK_SetCheckResultValue
* ����  : ��λ�Լ�����ĳһ���״̬
* ����  : pSelfCheckStruct - SelfCheckStruct�ṹ��ָ��
          bitMask - �ֶζ�Ӧ��λ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SLFCHK_SetCheckResultValue(SelfCheckStruct *pSelfCheckStruct, const U32 bitMask)
{
    pSelfCheckStruct->CheckResult |= bitMask;    
}

/*******************************************************************************
* ����  : SLFCHK_CfgCheckSelect
* ����  : ��λ�Լ�ѡ����ָ�����״̬��δָ���λ
* ����  : pSelfCheckStruct - SelfCheckStruct�ṹ��ָ��
          bitMask - �ֶζ�Ӧ��λ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SLFCHK_CfgCheckSelect(SelfCheckStruct *pSelfCheckStruct, const U32 bitMask)
{
    pSelfCheckStruct->CheckSelect = bitMask;    
}

/*******************************************************************************
* ����  : SLFCHK_SetCheckSelect
* ����  : ��λ�Լ�ѡ����ĳһ���״̬
* ����  : pSelfCheckStruct - SelfCheckStruct�ṹ��ָ��
          bitMask - �ֶζ�Ӧ��λ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SLFCHK_SetCheckSelect(SelfCheckStruct *pSelfCheckStruct, const U32 bitMask)
{
    pSelfCheckStruct->CheckSelect |= bitMask;    
}

/*******************************************************************************
* ����  : SLFCHK_ResetCheckSelect
* ����  : ��λ�Լ�ѡ����ĳһ���״̬
* ����  : pSelfCheckStruct - SelfCheckStruct�ṹ��ָ��
          bitMask - �ֶζ�Ӧ��λ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SLFCHK_ResetCheckSelect(SelfCheckStruct *pSelfCheckStruct, const U32 bitMask)
{
    pSelfCheckStruct->CheckSelect &= ~bitMask;    
}

/*******************************************************************************
* ����  : SLFCHK_CfgCheckSelect
* ����  : ��λ�Լ�ѡ����ָ�����״̬��δָ���λ
* ����  : bitMask - �ֶζ�Ӧ��λ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SLFCHK_InstCfgCheckSelect(const U32 bitMask)
{
    SLFCHK_CfgCheckSelect(&SelfCheckInst, bitMask);    
}

/*******************************************************************************
* ����  : SLFCHK_SetCheckSelect
* ����  : ��λ�Լ�ѡ����ĳһ���״̬
* ����  : bitMask - �ֶζ�Ӧ��λ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SLFCHK_InstSetCheckSelect(const U32 bitMask)
{
    SLFCHK_SetCheckSelect(&SelfCheckInst, bitMask);    
}

/*******************************************************************************
* ����  : SLFCHK_InstResetCheckSelect
* ����  : ��λ�Լ�ѡ����ĳһ���״̬
* ����  : bitMask - �ֶζ�Ӧ��λ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SLFCHK_InstResetCheckSelect(const U32 bitMask)
{
    SLFCHK_ResetCheckSelect(&SelfCheckInst, bitMask);    
}

/*******************************************************************************
* ����  : SLFCHK_SetCheckResultValue
* ����  : ��λ�Լ�����ĳһ���״̬
* ����  : pSelfCheckStruct - SelfCheckStruct�ṹ��ָ��
          bitMask - �ֶζ�Ӧ��λ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SLFCHK_InstSetCheckResultValue(const U32 bitMask)
{
    SLFCHK_SetCheckResultValue(&SelfCheckInst, bitMask);    
}

/*******************************************************************************
* ����  : SLFCHK_ResetCheckResultValue
* ����  : ��λ�Լ�����ĳһ���״̬
* ����  : pSelfCheckStruct - SelfCheckStruct�ṹ��ָ��
          bitMask - �ֶζ�Ӧ��λ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SLFCHK_ResetCheckResultValue(SelfCheckStruct *pSelfCheckStruct, const U32 bitMask)
{
    pSelfCheckStruct->CheckResult &= ~bitMask;    
}

/*******************************************************************************
* ����  : SLFCHK_InstResetCheckResultValue
* ����  : ��λ�Լ�����ĳһ���״̬
* ����  : pSelfCheckStruct - SelfCheckStruct�ṹ��ָ��
          bitMask - �ֶζ�Ӧ��λ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SLFCHK_InstResetCheckResultValue(const U32 bitMask)
{
    SLFCHK_ResetCheckResultValue(&SelfCheckInst, bitMask);    
}

/*******************************************************************************
* ����  : SLFCHK_SelfCheckStart
* ����  : �Լ�����
* ����  : pSelfCheckStruct - SelfCheckStruct�ṹ��ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SLFCHK_SelfCheckStart(SelfCheckStruct *pSelfCheckStruct)
{
    pSelfCheckStruct->StartFlg = TRUE;    
}

/*******************************************************************************
* ����  : SLFCHK_InstSelfCheckStart
* ����  : �Լ�����
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void SLFCHK_InstSelfCheckStart(void)
{
    SLFCHK_SelfCheckStart(&SelfCheckInst);    
}

/*******************************************************************************
* ����  : SLFCHK_SetCompleteBitValue
* ����  : ��λ��ɱ�־ĳһ���״̬
* ����  : pSelfCheckStruct - SelfCheckStruct�ṹ��ָ��
          bitMask - �ֶζ�Ӧ��λ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SLFCHK_SetCompleteBitValue(SelfCheckStruct *pSelfCheckStruct, const U32 bitMask)
{
    pSelfCheckStruct->CompleteBitFlg |= bitMask;    
}

/*******************************************************************************
* ����  : SLFCHK_IsSelfCheckComplete
* ����  : �Լ��Ƿ����
* ����  : pSelfCheckStruct - SelfCheckStruct�ṹ��ָ��
* ���  : ��
* ����  : TRUE - ����ɣ�FALSE - δ���
*******************************************************************************/
static U8 SLFCHK_IsSelfCheckComplete(SelfCheckStruct *pSelfCheckStruct)
{
    if ((pSelfCheckStruct->CompleteBitFlg & pSelfCheckStruct->CheckSelect)
        == pSelfCheckStruct->CheckSelect)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SLFCHK_IsSelfCheckOk
* ����  : �Լ��Ƿ�����
* ����  : pSelfCheckStruct - SelfCheckStruct�ṹ��ָ��
* ���  : ��
* ����  : TRUE - ������FALSE - ������
*******************************************************************************/
static U8 SLFCHK_IsSelfCheckOk(SelfCheckStruct *pSelfCheckStruct)
{
    if ((pSelfCheckStruct->CheckResult & pSelfCheckStruct->CheckSelect)
        == pSelfCheckStruct->CheckSelect)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : SLFCHK_SelfCheckProcess
* ����  : �Լ촦��
* ����  : pSelfCheckStruct - SelfCheckStruct�ṹ��ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void SLFCHK_SelfCheckProcess(SelfCheckStruct *pSelfCheckStruct)
{
    static EepromSelfCheckStruct EepromSelfCheckInst;
    static Si446xSelfCheckStruct Si446xSelfCheckInstA;
    static Si446xSelfCheckStruct Si446xSelfCheckInstB;
    static BatterySelfCheckStruct BatterySelfCheckInst;
    static RtcSelfCheckStruct RtcSelfCheckInst;
    static RfLinkSelfCheckStruct RfLinkSelfCheckInst;
	char chRetBuf[100];
	char chTemp[30];
     
    switch (pSelfCheckStruct->CtrlState)
    {
        case SLFCHK_CTRL_STATE_IDLE:
           
            if (pSelfCheckStruct->StartFlg == TRUE)
            {
    			LEDInit(LED_TYPE_SELF_CHECK_RUNNING);                
                pSelfCheckStruct->CtrlState = SLFCHK_CTRL_STATE_START;                                            
                pSelfCheckStruct->StartFlg = FALSE;
            }
            break;
        case SLFCHK_CTRL_STATE_START:
        
            EEPROM_EepromSelfCheckStructInit(&EepromSelfCheckInst);
            SI446X_Si446xSelfCheckStructInit(&Si446xSelfCheckInstA);
            SI446X_Si446xSelfCheckStructInit(&Si446xSelfCheckInstB);
#if CONFIG_SLFCHK_POWER_SUPPLY_VOLT_EN
            BAT_BatterySelfCheckStructInit(&BatterySelfCheckInst);
#endif
            RTC_RtcSelfCheckStructInit(&RtcSelfCheckInst);
            GPS_GpsSelfCheckStructInit(&GpsSelfCheckInst);
            RFL_RfLinkSelfCheckStructInit(&RfLinkSelfCheckInst);

            //lq pSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pSelfCheckStruct->TimePeriod);               
            pSelfCheckStruct->CtrlState = SLFCHK_CTRL_STATE_RUNNING;                                            
            
            break;
        case SLFCHK_CTRL_STATE_RUNNING:

            EEPROM_SelfCheck(&EepromSelfCheckInst);
            SI446X_SelfCheck(&Si446xSelfCheckInstA, SI446X_CHIP_A);			   //��ƵоƬ�Լ죬����PORT_INFO�����Լ�
            SI446X_SelfCheck(&Si446xSelfCheckInstB, SI446X_CHIP_B);
#if CONFIG_SLFCHK_POWER_SUPPLY_VOLT_EN
            BAT_SelfCheck(&BatterySelfCheckInst);							   //��ѹ��⣬��ȡ��ѹ��AD����ֵ
#endif
            RTC_SelfCheck(&RtcSelfCheckInst);
            GPS_SelfCheck(&GpsSelfCheckInst);								   //GPS�Լ�ģʽ,GPS���磬50ms��ʱ���ٶ�GPS�ϵ㣬���GPS�Ƿ�����Ϣ
            RFL_SelfCheck(&RfLinkSelfCheckInst);							   //�����⣬���ûػ���⣬����B�ŵ�����һ��6�Ų��ԣ�Ȼ����оƬB���н���

/*********************************************** �Լ������ **************************************************************************/
            /*lq EEPROM�Լ��� */
            if (EepromSelfCheckInst.CompleteFlg == TRUE)
            {
                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_EEPROM_WP) == TRUE)
                {
                    if (EepromSelfCheckInst.WPOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_EEPROM_WP);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_EEPROM_WP);                                   
                    }

                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_EEPROM_WP);
                    
                    /*lq ���±����������λ���Ĳ��Խ������ؼ� */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_EEPROM_WP);                                    //lq ָ����ǰ������
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_EEPROM_WP);  //lq ��ǰ������Ĳ��Խ��
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                    
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_EEPROM_DATA_COMM) == TRUE)
                {
                    if (EepromSelfCheckInst.DataCommOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_EEPROM_DATA_COMM);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_EEPROM_DATA_COMM);                                   
                    }

                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_EEPROM_DATA_COMM);
                    
                    /*lq ���±����������λ���Ĳ��Խ������ؼ� */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_EEPROM_DATA_COMM);                                    //lq ָ����ǰ������
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_EEPROM_DATA_COMM);  //lq ��ǰ������Ĳ��Խ��
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                     
                }

                EepromSelfCheckInst.CompleteFlg = FALSE;
            }
            
            /*lq SI446X_A�Լ��� */        
            if (Si446xSelfCheckInstA.CompleteFlg == TRUE)
            {
                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_PDN) == TRUE)
                {
                    if (Si446xSelfCheckInstA.PdnOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_PDN);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_PDN);                                   
                    }  
                    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_PDN);

                    /*lq ���±����������λ���Ĳ��Խ������ؼ� */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_SI446X_A_PDN);                                    //lq ָ����ǰ������
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_SI446X_A_PDN);  //lq ��ǰ������Ĳ��Խ��
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                    
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_SEN) == TRUE)
                {
                    if (Si446xSelfCheckInstA.SenOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_SEN);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_SEN);                                   
                    }
                    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_SEN);

                    /*lq ���±����������λ���Ĳ��Խ������ؼ� */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_SI446X_A_SEN);                                    //lq ָ����ǰ������
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_SI446X_A_SEN);  //lq ��ǰ������Ĳ��Խ��
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                     
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_DATA_COMM) == TRUE)
                {
                    if (Si446xSelfCheckInstA.DataCommOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_DATA_COMM);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_DATA_COMM);                                   
                    }
    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_DATA_COMM);

                    /*lq ���±����������λ���Ĳ��Խ������ؼ� */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_SI446X_A_DATA_COMM);                                    //lq ָ����ǰ������
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_SI446X_A_DATA_COMM);  //lq ��ǰ������Ĳ��Խ��
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_TYPE) == TRUE)
                {
                    if (Si446xSelfCheckInstA.TypeOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_TYPE);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_TYPE);                                   
                    }
                    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_TYPE);

                    /*lq ���±����������λ���Ĳ��Խ������ؼ� */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_SI446X_A_TYPE);                                     //lq ָ����ǰ������
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_SI446X_A_TYPE);  //lq ��ǰ������Ĳ��Խ��
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                     
                }

                Si446xSelfCheckInstA.CompleteFlg = FALSE;
            }

            /*lq SI446X_B�Լ��� */        
            if (Si446xSelfCheckInstB.CompleteFlg == TRUE)
            {
                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_PDN) == TRUE)
                {
                    if (Si446xSelfCheckInstB.PdnOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_PDN);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_PDN);                                   
                    }  
                    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_PDN);

                    /*lq ���±����������λ���Ĳ��Խ������ؼ� */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_SI446X_B_PDN);                                    //lq ָ����ǰ������
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_SI446X_B_PDN);  //lq ��ǰ������Ĳ��Խ��
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                    
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_SEN) == TRUE)
                {
                    if (Si446xSelfCheckInstB.SenOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_SEN);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_SEN);                                   
                    }
                    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_SEN);

                    /*lq ���±����������λ���Ĳ��Խ������ؼ� */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_SI446X_B_SEN);                                    //lq ָ����ǰ������
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_SI446X_B_SEN);  //lq ��ǰ������Ĳ��Խ��
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                     
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_DATA_COMM) == TRUE)
                {
                    if (Si446xSelfCheckInstB.DataCommOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_DATA_COMM);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_DATA_COMM);                                   
                    }
    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_DATA_COMM);

                    /*lq ���±����������λ���Ĳ��Խ������ؼ� */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_SI446X_B_DATA_COMM);                                    //lq ָ����ǰ������
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_SI446X_B_DATA_COMM);  //lq ��ǰ������Ĳ��Խ��
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_TYPE) == TRUE)
                {
                    if (Si446xSelfCheckInstB.TypeOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_TYPE);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_TYPE);                                   
                    }
                    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_TYPE);

                    /*lq ���±����������λ���Ĳ��Խ������ؼ� */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_SI446X_B_TYPE);                                     //lq ָ����ǰ������
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_SI446X_B_TYPE);  //lq ��ǰ������Ĳ��Խ��
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                     
                }

                Si446xSelfCheckInstB.CompleteFlg = FALSE;
            }

            /*lq Supply voltage��ѹ�Լ��� */        
            if (BatterySelfCheckInst.CompleteFlg == TRUE)
            {
                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_POWER_SUPPLY_VOLT) == TRUE)
                {
                    if (BatterySelfCheckInst.SupplyVoltOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_POWER_SUPPLY_VOLT);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_POWER_SUPPLY_VOLT);                                   
                    }
    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_POWER_SUPPLY_VOLT);

                    /*lq ���±����������λ���Ĳ��Խ������ؼ� */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_POWER_SUPPLY_VOLT);                                    //lq ָ����ǰ������
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_POWER_SUPPLY_VOLT);  //lq ��ǰ������Ĳ��Խ��
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                  
                }                

                BatterySelfCheckInst.CompleteFlg = FALSE;
            }

            /*lq RTC�Լ��� */        
            if (RtcSelfCheckInst.CompleteFlg == TRUE)
            {
                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_RTC_LSE) == TRUE)
                {
                    if (RtcSelfCheckInst.RtcLseOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_RTC_LSE);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_RTC_LSE);                                   
                    }
    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_RTC_LSE);

                    /*lq ���±����������λ���Ĳ��Խ������ؼ� */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_RTC_LSE);                                    //lq ָ����ǰ������
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_RTC_LSE);  //lq ��ǰ������Ĳ��Խ��
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                
                }

                RtcSelfCheckInst.CompleteFlg = FALSE;
            }            

            /*lq GPS�Լ��� */        
            if (GpsSelfCheckInst.CompleteFlg == TRUE)
            {
                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_VCC_CTRL) == TRUE)
                {
                    if (GpsSelfCheckInst.VccCtrlOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_VCC_CTRL);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_VCC_CTRL);                                   
                    }

                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_VCC_CTRL);

                    /*lq ���±����������λ���Ĳ��Խ������ؼ� */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_GPS_VCC_CTRL);                                    //lq ָ����ǰ������
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_GPS_VCC_CTRL);  //lq ��ǰ������Ĳ��Խ��
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                    
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_TIMEPULSE) == TRUE)
                {
                    if (GpsSelfCheckInst.TimepulseOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_TIMEPULSE);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_TIMEPULSE);                                   
                    } 

                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_TIMEPULSE);

                    /*lq ���±����������λ���Ĳ��Խ������ؼ� */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_GPS_TIMEPULSE);                                    //lq ָ����ǰ������
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_GPS_TIMEPULSE);  //lq ��ǰ������Ĳ��Խ��
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                    
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_DATA_COMM) == TRUE)
                {
                    if (GpsSelfCheckInst.DataCommOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_DATA_COMM);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_DATA_COMM);                                   
                    }
                    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_DATA_COMM);

                    /*lq ���±����������λ���Ĳ��Խ������ؼ� */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_GPS_DATA_COMM);                                    //lq ָ����ǰ������
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_GPS_DATA_COMM);  //lq ��ǰ������Ĳ��Խ��
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                    
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_ANTENNA) == TRUE)
                {
                    if (GpsSelfCheckInst.AntennaOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_ANTENNA);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_ANTENNA);                                   
                    }

                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_ANTENNA);
                    
                    /*lq ���±����������λ���Ĳ��Խ������ؼ� */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_GPS_ANTENNA);                                    //lq ָ����ǰ������
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_GPS_ANTENNA);  //lq ��ǰ������Ĳ��Խ��
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                    
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_MODULE) == TRUE)
                {
                    if (GpsSelfCheckInst.ModuleOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_MODULE);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_MODULE);                                   
                    }
                    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_MODULE);

                    /*lq ���±����������λ���Ĳ��Խ������ؼ� */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_GPS_MODULE);                                    //lq ָ����ǰ������
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_GPS_MODULE);  //lq ��ǰ������Ĳ��Խ��
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                      
                }

                GpsSelfCheckInst.CompleteFlg = FALSE;
            }

            /*lq RF link�Լ��� */        
            if (RfLinkSelfCheckInst.CompleteFlg == TRUE)
            {
                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_RF_LINK) == TRUE)
                {
                    if (RfLinkSelfCheckInst.RfLinkOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_RF_LINK);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_RF_LINK);                                   
                    }
    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_RF_LINK);

                    /*lq ���±����������λ���Ĳ��Խ������ؼ� */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_RF_LINK);                                    //lq ָ����ǰ������
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_RF_LINK);  //lq ��ǰ������Ĳ��Խ��
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                
                }

                RfLinkSelfCheckInst.CompleteFlg = FALSE;
            }            

            if (SLFCHK_IsSelfCheckComplete(pSelfCheckStruct) == TRUE)
            {
    			if (SLFCHK_IsSelfCheckOk(pSelfCheckStruct) == TRUE)
                {
                    LEDInit(LED_TYPE_SELF_CHECK_OK);                                
                }
                else
                {
                    LEDInit(LED_TYPE_SELF_CHECK_ERR);
                }

                sprintf(chRetBuf, "$P712,SLFCHK,");
                sprintf(chTemp, "%08X", pSelfCheckStruct->CheckSelect);                                    //lq ָ����ǰ������
                strcat(chRetBuf, chTemp);
                sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & pSelfCheckStruct->CheckSelect);  //lq ��ǰ������Ĳ��Խ��
                strcat(chRetBuf, chTemp);
                UartResponseMsg(chRetBuf); 

                pSelfCheckStruct->CompleteFlg = TRUE;
                
                pSelfCheckStruct->CtrlState = SLFCHK_CTRL_STATE_IDLE;                                
            }

            break;                            
        default :
            
            SLFCHK_SelfCheckStructInit(pSelfCheckStruct);
            LEDInit(LED_TYPE_SELF_CHECK_IDLE);                                
            break;       
    }    
}

/*******************************************************************************
* ����  : SLFCHK_InstSelfCheckProcess
* ����  : �Լ촦��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void SLFCHK_InstSelfCheckProcess(void)
{
    SLFCHK_SelfCheckProcess(&SelfCheckInst);
}

/*******************************************************************************
* ����  : SLFCHK_GetCheckSelect
* ����  : ��ȡ�Լ�ѡ��
* ����  : ��
* ���  : ��
* ����  : �Լ�ѡ��
*******************************************************************************/
U32 SLFCHK_GetCheckSelectValue(void)
{
	SelfCheckStruct *pSelfCheckStruct;

	pSelfCheckStruct = &SelfCheckInst;

	return	pSelfCheckStruct->CheckSelect;
}
