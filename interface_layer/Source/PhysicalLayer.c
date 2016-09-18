/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: PhysicalLayer.c
  Author: lq       Version : V1.00        Date: 2014.07.11
  Description:     AIS�����
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
#include "PhysicalLayer.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static NRZIStruct NRZIDecode1;
static NRZIStruct NRZIDecode2;
 
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* ����  : PHY_NRZIStructInit
* ����  : ��ʼ���ṹ��
* ����  : pNRZIStruct - NRZIStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void PHY_NRZIStructInit(NRZIStruct *pNRZIStruct)
{
    pNRZIStruct->RunningFlg = FALSE;
    pNRZIStruct->InitialValue = 1;
    pNRZIStruct->PriorValue = 1;    
}

/*******************************************************************************
* ����  : PHY_InstNRZIStructInit
* ����  : ��ʼ���ṹ��
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void PHY_InstNRZIStructInit(void)
{
    PHY_NRZIStructInit(&NRZIDecode1);    
    PHY_NRZIStructInit(&NRZIDecode2);        
}

/*******************************************************************************
* ����  : PHY_NRZIDecodeStart
* ����  : NRZI���뿪ʼ
* ����  : pNRZIStruct - NRZIStruct�ṹ��ָ��
          initValue - ��ʼֵ	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void PHY_NRZIDecodeStart(NRZIStruct *pNRZIStruct, const U8 initValue)
{
    pNRZIStruct->RunningFlg = TRUE;
    pNRZIStruct->InitialValue = initValue & 0x01;
    pNRZIStruct->PriorValue = pNRZIStruct->InitialValue;    
}

/*******************************************************************************
* ����  : PHY_InstNRZIDecodeStart
* ����  : NRZI���뿪ʼ
* ����  : NRZIType - NRZI����
          initValue - ��ʼֵ	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void PHY_InstNRZIDecodeStart(const NRZITypeEnum NRZIType, const U8 initValue)
{
    if (NRZIType == NRZIType1)
    {
        PHY_NRZIDecodeStart(&NRZIDecode1, initValue);    
    }
    else if (NRZIType == NRZIType2) 
    {
        PHY_NRZIDecodeStart(&NRZIDecode2, initValue);
    }    
}

/*******************************************************************************
* ����  : PHY_NRZIDecodeStop
* ����  : NRZI����ֹͣ
* ����  : pNRZIStruct - NRZIStruct�ṹ��ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void PHY_NRZIDecodeStop(NRZIStruct *pNRZIStruct)
{
    pNRZIStruct->RunningFlg = FALSE;
}

/*******************************************************************************
* ����  : PHY_InstNRZIDecodeStop
* ����  : NRZI����ֹͣ
* ����  : NRZIType - NRZI����
* ���  : ��
* ����  : ��
*******************************************************************************/
void PHY_InstNRZIDecodeStop(const NRZITypeEnum NRZIType)
{
    if (NRZIType == NRZIType1)
    {
        PHY_NRZIDecodeStop(&NRZIDecode1);    
    }
    else if (NRZIType == NRZIType2) 
    {
        PHY_NRZIDecodeStop(&NRZIDecode2);
    }
}

/*******************************************************************************
* ����  : PHY_NRZIDecode
* ����  : NRZI����
* ����  : pNRZIStruct - NRZIStruct�ṹ��ָ��
          curValue - ��ǰֵ
          pDecodeValue - ָ���Ž���ֵ��ָ��	  
* ���  : ��
* ����  : TRUE - ����ֵ��Ч��FALSE - ����ֵ��Ч 
*******************************************************************************/
static U8 PHY_NRZIDecode(NRZIStruct *pNRZIStruct, const U8 curValue, U8 *pDecodeValue)
{
    if (pNRZIStruct->RunningFlg == TRUE)
    {
        if ((curValue & 0x01) == pNRZIStruct->PriorValue)
        {
            *pDecodeValue = 1;
        }
        else
        {
            *pDecodeValue = 0;
        }
    
        pNRZIStruct->PriorValue = curValue & 0x01;

        return TRUE;
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* ����  : PHY_InstNRZIDecode
* ����  : NRZI����
* ����  : NRZIType - NRZI����
          curValue - ��ǰֵ
          pDecodeValue - ָ���Ž���ֵ��ָ��	  
* ���  : ��
* ����  : TRUE - ����ֵ��Ч��FALSE - ����ֵ��Ч 
*******************************************************************************/
U8 PHY_InstNRZIDecode(const NRZITypeEnum NRZIType, const U8 curValue, U8 *pDecodeValue)
{
    if (NRZIType == NRZIType1)
    {
        return PHY_NRZIDecode(&NRZIDecode1, curValue, pDecodeValue);    
    }
    else if (NRZIType == NRZIType2) 
    {
        return PHY_NRZIDecode(&NRZIDecode2, curValue, pDecodeValue);        
    }
    else
    {
        return FALSE;    
    }
}

/*******************************************************************************
* ����  : PHY_ModuleInit
* ����  : �����ģ���ʼ��
* ����  : ��	  
* ���  : ��
* ����  : �� 
*******************************************************************************/
void PHY_ModuleInit(void)
{
    PHY_InstNRZIStructInit();
    PHY_InstNRZIDecodeStart(NRZIType1, 1);
    PHY_InstNRZIDecodeStart(NRZIType2, 1);
}
