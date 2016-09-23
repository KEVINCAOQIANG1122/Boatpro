/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: PhysicalLayer.h
  Author: lq       Version : V1.00        Date: 2014.07.11
  Description:     AIS�����
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _PHYSICAL_LAYER_H
#define _PHYSICAL_LAYER_H

/* Include ------------------------------------------------------------------*/

/* Public define ------------------------------------------------------------*/

/* Public typedef -----------------------------------------------------------*/
typedef enum NRZITypeEnum
{
    NRZIType1,
    NRZIType2
}NRZITypeEnum;

typedef struct NRZIStruct
{
    U8  RunningFlg;
    U8  InitialValue;
    U8  PriorValue;
}NRZIStruct;

/* Public variables ---------------------------------------------------------*/

/* Public function ----------------------------------------------------------*/

//lnw add
extern void PHY_ModuleInit(void);
extern U8 PHY_InstNRZIDecode(const NRZITypeEnum NRZIType, const U8 curValue, U8 *pDecodeValue);

#endif
