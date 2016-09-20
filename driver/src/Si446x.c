/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: SI446X.c
  Author: lq       Version : V1.0.0        Date: 2015.01.15
  Description:     SI446X����
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

/* Private variables ---------------------------------------------------------*/
static SI446XStruct SI446XA;
static SI446XStruct SI446XB;
static U8 Si446xPatchCmds[] = {SI446X_PATCH_CMDS, 0x00};
#if 0
static U8 config_table[] = RADIO_CONFIGURATION_DATA_ARRAY;
#endif
U32 SI446X_FRE_BASE;
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* ����  : SI446X_SI446XStructInit
* ����  : SI446XStruct�ṹ���ʼ��
* ����  : pSI446XStruct - SI446XStruct�ṹ��ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void SI446X_SI446XStructInit(SI446XStruct *pSI446XStruct)
{
    S32 freqOffset;

    if (pSI446XStruct == &SI446XA)
    {
        freqOffset = SI446X_FREQ_OFFSET_A;
    }
    else
    {
        freqOffset = SI446X_FREQ_OFFSET_B;
    }
    pSI446XStruct->SI446XStatus = SI446X_MODE_RX;

//    pSI446XStruct->SI446X_N_Integer_Base = SI446X_CalcRfFreqInteger(
//        SI446X_FREQ_RF_BASE, SI446X_TYPE_4463);
//
//    pSI446XStruct->SI446X_N_Fractional_Base = SI446X_CalcRfFreqFractional(
//        SI446X_FREQ_RF_BASE, SI446X_TYPE_4463);
//
//    pSI446XStruct->SI446X_Freq_Dev = SI446X_CalcRfDeviation(
//        SI446X_FREQ_RF_BASE, SI446X_FREQ_DEVIATION, SI446X_TYPE_4463);
//
//    pSI446XStruct->SI446X_Freq_Offset = SI446X_CalcRfOffset(
//        SI446X_FREQ_RF_BASE, freqOffset, SI446X_TYPE_4463);
//
//    pSI446XStruct->SI446X_IF_Freq = SI446X_CalcIFFreq(
//        SI446X_FREQ_RF_BASE, SI446X_TYPE_4463);
//
//    pSI446XStruct->SI446X_Channel_Step = SI446X_CalcRfChannelStep(
//        SI446X_FREQ_RF_BASE, SI446X_FREQ_CHANNEL_STEPSIZE, SI446X_TYPE_4463);

	pSI446XStruct->SI446X_N_Integer_Base = SI446X_CalcRfFreqInteger(
        SI446X_FRE_BASE, SI446X_TYPE_4463);

    pSI446XStruct->SI446X_N_Fractional_Base = SI446X_CalcRfFreqFractional(
        SI446X_FRE_BASE, SI446X_TYPE_4463);

    pSI446XStruct->SI446X_Freq_Dev = SI446X_CalcRfDeviation(
        SI446X_FRE_BASE, SI446X_FREQ_DEVIATION, SI446X_TYPE_4463);

    pSI446XStruct->SI446X_Freq_Offset = SI446X_CalcRfOffset(
        SI446X_FRE_BASE, freqOffset, SI446X_TYPE_4463);

    pSI446XStruct->SI446X_IF_Freq = SI446X_CalcIFFreq(
        SI446X_FRE_BASE, SI446X_TYPE_4463);

    pSI446XStruct->SI446X_Channel_Step = SI446X_CalcRfChannelStep(
        SI446X_FRE_BASE, SI446X_FREQ_CHANNEL_STEPSIZE, SI446X_TYPE_4463);

    pSI446XStruct->SI446X_PaPwrLvl = SI446X_PA_PWR_LVL;
}

/*******************************************************************************
* ����  : SI446X_InitPowerPins
* ����  : ��ʼ��Si446x LDO��������
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SI446X_InitPowerPins(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_InitStructure.GPIO_Pin = SI446X_POWER_PIN;
    GPIO_Init(SI446X_POWER_PORT, &GPIO_InitStructure);

    SI446X_POWER_ON();
}

/*******************************************************************************
* ����  : SI446X_InitPins
* ����  : ��ʼ��SPIRIT1����
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SI446X_InitPins(SI446XChipEnum cs)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    if (cs == SI446X_CHIP_A)
    {
        /*lq ------------------SI446X_A ��������---------------------------*/
        /* Configure SI446X_A CE ---------------------------------*/
        GPIO_InitStructure.GPIO_Pin = SI446X_A_CE_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
        GPIO_Init(SI446X_A_CE_PORT, &GPIO_InitStructure);
#if 0
        SI446X_A_CE_OFF();                                      //lq оƬ�ر�
        Delay_ms(40);
        SI446X_A_CE_ON();                                       //lq оƬ����        
        Delay_ms(40);
#endif
        /* Configure SI446X_A SPI ---------------------------------*/
        GPIO_InitStructure.GPIO_Pin = SI446X_A_SDI_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //lq SDI
        GPIO_Init(SI446X_A_SDI_PORT, &GPIO_InitStructure);
    
        GPIO_InitStructure.GPIO_Pin = SI446X_A_SDO_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //lq SDO
        GPIO_Init(SI446X_A_SDO_PORT, &GPIO_InitStructure);
    
        GPIO_InitStructure.GPIO_Pin = SI446X_A_CS_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //lq CS
        GPIO_Init(SI446X_A_CS_PORT, &GPIO_InitStructure);
#if 0
        SI446X_A_CS_OFF();                                      //lq Ƭѡ�ر�
#endif    
        GPIO_InitStructure.GPIO_Pin = SI446X_A_SCLK_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //lq SCLK
        GPIO_Init(SI446X_A_SCLK_PORT, &GPIO_InitStructure);
    }
    else
    {
        /*lq ------------------SI446X_B ��������---------------------------*/
        /* Configure SI446X_B CE ---------------------------------*/
        GPIO_InitStructure.GPIO_Pin = SI446X_B_CE_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
        GPIO_Init(SI446X_B_CE_PORT, &GPIO_InitStructure);
#if 0
        SI446X_B_CE_OFF();                                      //lq оƬ�ر�
        Delay_ms(40);
        SI446X_B_CE_ON();                                       //lq оƬ����        
        Delay_ms(40);
#endif
        /* Configure SI446X_B SPI ---------------------------------*/
        GPIO_InitStructure.GPIO_Pin = SI446X_B_SDI_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //lq SDI
        GPIO_Init(SI446X_B_SDI_PORT, &GPIO_InitStructure);
    
        GPIO_InitStructure.GPIO_Pin = SI446X_B_SDO_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //lq SDO
        GPIO_Init(SI446X_B_SDO_PORT, &GPIO_InitStructure);
    
        GPIO_InitStructure.GPIO_Pin = SI446X_B_CS_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //lq CS
        GPIO_Init(SI446X_B_CS_PORT, &GPIO_InitStructure);
#if 0
        SI446X_B_CS_OFF();                                      //lq Ƭѡ�ر�
#endif    
        GPIO_InitStructure.GPIO_Pin = SI446X_B_SCLK_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //lq SCLK
        GPIO_Init(SI446X_B_SCLK_PORT, &GPIO_InitStructure);
    }
#if 1
    /*lq Si446x��λ */
    SI446X_Reset(cs);
#endif
}

/*******************************************************************************
* ����  : SI446X_SetSi446xPaPwrLvl
* ����  : ����SI4463�������
* ����  : pwrLvl - ����ˮƽ
          cs - Ƭѡ
* ���  : ��
* ����  : ��
*******************************************************************************/
void SI446X_SetSi446xPaPwrLvl(U8 pwrLvl, SI446XChipEnum cs)
{
    SI446XStruct *pSI446XStruct;

    if (cs == SI446X_CHIP_A)
    {
        pSI446XStruct = &SI446XA;
    }
    else
    {
        pSI446XStruct = &SI446XB;
    }

    pSI446XStruct->SI446X_PaPwrLvl = pwrLvl;
    
    SI446X_SET_PROPERTY_1(PA_PWR_LVL, pSI446XStruct->SI446X_PaPwrLvl, cs);
}

/*******************************************************************************
* ����  : SI446X_GetSi446xPaPwrLvl
* ����  : ��ȡSI4463�������
* ����  : cs - Ƭѡ
* ���  : ��
* ����  : SI446X��ǰ�������ֵ
*******************************************************************************/
U16 SI446X_GetSi446xPaPwrLvl(SI446XChipEnum cs)
{
    SI446XStruct *pSI446XStruct;

    if (cs == SI446X_CHIP_A)
    {
        pSI446XStruct = &SI446XA;
    }
    else
    {
        pSI446XStruct = &SI446XB;
    }

    return pSI446XStruct->SI446X_PaPwrLvl;
}

/*******************************************************************************
* ����  : SI446X_CalcRfFreqInteger
* ����  : SI446XоƬ��ʼ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
U32 SI446X_CalcRfFreqInteger(U32 freq, SI446XTypeEnum si446xType)
{
    U32 integer;
    U16 outDiv;

    if (si446xType == SI446X_TYPE_4464)
    {
        if ((freq >= SI446X_FREQ_OUTDIV_4464_24_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_24_UPPER))
        {
            outDiv = 24;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_16_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_16_UPPER))
        {
            outDiv = 16;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_12_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_12_UPPER))
        {
            outDiv = 12;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_8_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_8_UPPER))
        {
            outDiv = 8;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_6_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_6_UPPER))
        {
            outDiv = 6;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_4_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_4_UPPER))
        {
            outDiv = 4;
        }
        else
        {
            outDiv = SI446X_FREQ_OUTDIV_4464_DEFAULT;
        }
    }
    else
    {
        if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_24_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_24_UPPER))
        {
            outDiv = 24;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_12_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_12_UPPER))
        {
            outDiv = 12;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_8_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_8_UPPER))
        {
            outDiv = 8;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_4_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_4_UPPER))
        {
            outDiv = 4;
        }
        else
        {
            outDiv = SI446X_FREQ_OUTDIV_4460_61_63_DEFAULT;
        }
    }

    integer = (freq * outDiv) / SI446X_FREQ_XO / 2 - 1;

    return integer;
}

/*******************************************************************************
* ����  : SI446X_CalcRfFreqFractional
* ����  : SI446XоƬ��ʼ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
U32 SI446X_CalcRfFreqFractional(U32 freq, SI446XTypeEnum si446xType)
{
    U32 integer;
    U32 fractional;
    U16 outDiv;

    if (si446xType == SI446X_TYPE_4464)
    {
        if ((freq >= SI446X_FREQ_OUTDIV_4464_24_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_24_UPPER))
        {
            outDiv = 24;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_16_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_16_UPPER))
        {
            outDiv = 16;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_12_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_12_UPPER))
        {
            outDiv = 12;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_8_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_8_UPPER))
        {
            outDiv = 8;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_6_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_6_UPPER))
        {
            outDiv = 6;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_4_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_4_UPPER))
        {
            outDiv = 4;
        }
        else
        {
            outDiv = SI446X_FREQ_OUTDIV_4464_DEFAULT;
        }
    }
    else
    {
        if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_24_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_24_UPPER))
        {
            outDiv = 24;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_12_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_12_UPPER))
        {
            outDiv = 12;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_8_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_8_UPPER))
        {
            outDiv = 8;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_4_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_4_UPPER))
        {
            outDiv = 4;
        }
        else
        {
            outDiv = SI446X_FREQ_OUTDIV_4460_61_63_DEFAULT;
        }
    }

    integer = (freq * outDiv) / 2 / SI446X_FREQ_XO - 1;

    fractional = (((double)(freq * outDiv) / 2 / SI446X_FREQ_XO) - integer) * (1 << 19)
                  + 0.5;

    return fractional;
}

/*******************************************************************************
* ����  : SI446X_CalcRfDeviation
* ����  : ����SI446XоƬƵƫ�Ĵ�����ֵ
* ����  : freq - ����Ƶ��
          freqDev - Ƶƫֵ
          si446xType - SI446XоƬ�ͺ�
* ���  : ��
* ����  : SI446XоƬƵƫ�Ĵ�����ֵ
*******************************************************************************/
U32 SI446X_CalcRfDeviation(U32 freq, U32 freqDev, SI446XTypeEnum si446xType)
{
    U32 rfDev;
    U16 outDiv;

    if (si446xType == SI446X_TYPE_4464)
    {
        if ((freq >= SI446X_FREQ_OUTDIV_4464_24_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_24_UPPER))
        {
            outDiv = 24;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_16_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_16_UPPER))
        {
            outDiv = 16;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_12_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_12_UPPER))
        {
            outDiv = 12;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_8_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_8_UPPER))
        {
            outDiv = 8;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_6_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_6_UPPER))
        {
            outDiv = 6;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_4_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_4_UPPER))
        {
            outDiv = 4;
        }
        else
        {
            outDiv = SI446X_FREQ_OUTDIV_4464_DEFAULT;
        }
    }
    else
    {
        if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_24_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_24_UPPER))
        {
            outDiv = 24;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_12_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_12_UPPER))
        {
            outDiv = 12;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_8_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_8_UPPER))
        {
            outDiv = 8;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_4_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_4_UPPER))
        {
            outDiv = 4;
        }
        else
        {
            outDiv = SI446X_FREQ_OUTDIV_4460_61_63_DEFAULT;
        }
    }

    rfDev = (((double)outDiv * freqDev) / SI446X_FREQ_XO / 2) * (1 << 19) + 0.5;

    return rfDev;
}

/*******************************************************************************
* ����  : SI446X_CalcRfOffset
* ����  : ����SI446XоƬ����Ƶ��Ƶƫ�Ĵ�����ֵ�����㹫ʽͬSI446X_CalcRfDeviation��
* ����  : freq - ����Ƶ��
          freqOffset - Ƶƫֵ
          si446xType - SI446XоƬ�ͺ�
* ���  : ��
* ����  : SI446XоƬ����Ƶ��Ƶƫ�Ĵ�����ֵ
*******************************************************************************/
S16 SI446X_CalcRfOffset(U32 freq, S32 freqOffset, SI446XTypeEnum si446xType)
{
    S16 rfOffset;
    U16 outDiv;

    if (si446xType == SI446X_TYPE_4464)
    {
        if ((freq >= SI446X_FREQ_OUTDIV_4464_24_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_24_UPPER))
        {
            outDiv = 24;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_16_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_16_UPPER))
        {
            outDiv = 16;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_12_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_12_UPPER))
        {
            outDiv = 12;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_8_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_8_UPPER))
        {
            outDiv = 8;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_6_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_6_UPPER))
        {
            outDiv = 6;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_4_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_4_UPPER))
        {
            outDiv = 4;
        }
        else
        {
            outDiv = SI446X_FREQ_OUTDIV_4464_DEFAULT;
        }
    }
    else
    {
        if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_24_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_24_UPPER))
        {
            outDiv = 24;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_12_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_12_UPPER))
        {
            outDiv = 12;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_8_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_8_UPPER))
        {
            outDiv = 8;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_4_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_4_UPPER))
        {
            outDiv = 4;
        }
        else
        {
            outDiv = SI446X_FREQ_OUTDIV_4460_61_63_DEFAULT;
        }
    }

    rfOffset = (((double)outDiv * freqOffset) / SI446X_FREQ_XO / 2) * (1 << 19) + 0.5;

    return rfOffset;
}

/*******************************************************************************
* ����  : SI446X_CalcRfDeviation
* ����  : ����SI446XоƬ�ŵ������Ĵ�����ֵ
* ����  : freq - ����Ƶ��
          channelStep - �ŵ�����
          si446xType - SI446XоƬ�ͺ�
* ���  : ��
* ����  : SI446XоƬ�ŵ������Ĵ�����ֵ
*******************************************************************************/
U16 SI446X_CalcRfChannelStep(U32 freq, U32 channelStep, SI446XTypeEnum si446xType)
{
    U16 stepSize;
    U16 outDiv;

    if (si446xType == SI446X_TYPE_4464)
    {
        if ((freq >= SI446X_FREQ_OUTDIV_4464_24_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_24_UPPER))
        {
            outDiv = 24;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_16_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_16_UPPER))
        {
            outDiv = 16;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_12_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_12_UPPER))
        {
            outDiv = 12;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_8_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_8_UPPER))
        {
            outDiv = 8;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_6_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_6_UPPER))
        {
            outDiv = 6;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_4_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_4_UPPER))
        {
            outDiv = 4;
        }
        else
        {
            outDiv = SI446X_FREQ_OUTDIV_4464_DEFAULT;
        }
    }
    else
    {
        if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_24_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_24_UPPER))
        {
            outDiv = 24;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_12_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_12_UPPER))
        {
            outDiv = 12;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_8_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_8_UPPER))
        {
            outDiv = 8;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_4_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_4_UPPER))
        {
            outDiv = 4;
        }
        else
        {
            outDiv = SI446X_FREQ_OUTDIV_4460_61_63_DEFAULT;
        }
    }

    stepSize = (((double)outDiv * channelStep) / SI446X_FREQ_XO / 2) * (1 << 19) + 0.5;

    return stepSize;
}

/*******************************************************************************
* ����  : SI446X_CalcRfDeviation
* ����  : ����SI446XоƬƵƫ�Ĵ�����ֵ
* ����  : freq - ����Ƶ��
          si446xType - SI446XоƬ�ͺ�
* ���  : ��
* ����  : SI446XоƬIF�Ĵ�����ֵ
*******************************************************************************/
U32 SI446X_CalcIFFreq(U32 freq, SI446XTypeEnum si446xType)
{
    U32 IFFreq;
    U16 outDiv;

#if (SI446X_IF_MODE != SI446X_IF_MODE_ZERO)
    if (si446xType == SI446X_TYPE_4464)
    {
        if ((freq >= SI446X_FREQ_OUTDIV_4464_24_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_24_UPPER))
        {
            outDiv = 24;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_16_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_16_UPPER))
        {
            outDiv = 16;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_12_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_12_UPPER))
        {
            outDiv = 12;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_8_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_8_UPPER))
        {
            outDiv = 8;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_6_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_6_UPPER))
        {
            outDiv = 6;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4464_4_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4464_4_UPPER))
        {
            outDiv = 4;
        }
        else
        {
            outDiv = SI446X_FREQ_OUTDIV_4464_DEFAULT;
        }
    }
    else
    {
        if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_24_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_24_UPPER))
        {
            outDiv = 24;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_12_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_12_UPPER))
        {
            outDiv = 12;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_8_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_8_UPPER))
        {
            outDiv = 8;
        }
        else if ((freq >= SI446X_FREQ_OUTDIV_4460_61_63_4_LOWER)
            && (freq <= SI446X_FREQ_OUTDIV_4460_61_63_4_UPPER))
        {
            outDiv = 4;
        }
        else
        {
            outDiv = SI446X_FREQ_OUTDIV_4460_61_63_DEFAULT;
        }
    }

    IFFreq = outDiv << (12 - SI446X_IF_FREQ_N_EXP);  //lq modem_if_freq = (2^19 * outdiv * (freq_xo / (64 * N))) / (2 * freq_xo)
    IFFreq = ~IFFreq + 1;
#else
    IFFreq = 0;
#endif
    return IFFreq;
}

/*******************************************************************************
* ����  : SI446X_RxCfg
* ����  : SI446X��������
* ����  : cs - Ƭѡ
          channel - �ŵ�
* ���  : ��
* ����  : ��
*******************************************************************************/
void SI446X_RxCfg(SI446XChipEnum cs, AisChannelEnum channel)
{ 
    GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
    SI446XStruct *pSI446XStruct;

    if (cs == SI446X_CHIP_A)
    {
        pSI446XStruct = &SI446XA;

	    //lq SI446X_A_PAC_OFF();
        
        /*lq data pin config */
        GPIO_InitStructure.GPIO_Pin = SI446X_A_DATA_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //lq RX
        GPIO_Init(SI446X_A_DATA_PORT, &GPIO_InitStructure);
        
        /*lq clock pin config */
        /* Connect EXTI Line to GPIO Pin */
        GPIO_EXTILineConfig(SI446X_A_DCLK_SOURCE_PORT, SI446X_A_DCLK_SOURCE_PIN);   //lq DCLK
        
        /* Configure EXTI Line to generate an interrupt on rising edge */  
        EXTI_InitStructure.EXTI_Line = SI446X_A_DCLK_LINE;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //lq RxʱMCU��ʱ�������ز�������
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStructure); 
    }
    else
    {
        pSI446XStruct = &SI446XB;

	    //lq SI446X_B_PAC_OFF();
        
        /*lq data pin config */
        GPIO_InitStructure.GPIO_Pin = SI446X_B_DATA_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //lq RX
        GPIO_Init(SI446X_B_DATA_PORT, &GPIO_InitStructure);
        
        /*lq clock pin config */
        /* Connect EXTI Line to GPIO Pin */
        GPIO_EXTILineConfig(SI446X_B_DCLK_SOURCE_PORT, SI446X_B_DCLK_SOURCE_PIN);   //lq DCLK
        
        /* Configure EXTI Line to generate an interrupt on rising edge */  
        EXTI_InitStructure.EXTI_Line = SI446X_B_DCLK_LINE;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //lq RxʱMCU��ʱ�������ز�������
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStructure);
    }
#if 1
    /*lq Si446x GPIO config */
    SI446X_GpioPinCfg(SI446X_MODE_RX, cs);

	/*lq SI446X Rx config */
    if (channel == ENUM_AIS_CHANNEL_A)
    {
        SI446X_START_RX(SI446X_CHANNEL_AIS_A, 0, 0, 0, 0, 0, cs);
    }
    else
    {
        SI446X_START_RX(SI446X_CHANNEL_AIS_B, 0, 0, 0, 0, 0, cs);
    }
#endif
	pSI446XStruct->SI446XStatus = SI446X_MODE_RX;
}

/*******************************************************************************
* ����  : SI446X_TxCfg
* ����  : SI446X��������
* ����  : cs - Ƭѡ
          channel - �ŵ�
* ���  : ��
* ����  : ��
*******************************************************************************/
void SI446X_TxCfg(SI446XChipEnum cs, AisChannelEnum channel)
{ 
    GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
    SI446XStruct *pSI446XStruct;

    if (cs == SI446X_CHIP_A)
    {
        pSI446XStruct = &SI446XA;

    	//lq SI446X_A_PAC_ON();	
        
        /*lq data pin config */
        GPIO_InitStructure.GPIO_Pin = SI446X_A_DATA_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //lq TX
        GPIO_Init(SI446X_A_DATA_PORT, &GPIO_InitStructure);
    
    	/*lq clock pin config */
    	/* Connect EXTI Line to GPIO Pin */
    	GPIO_EXTILineConfig(SI446X_A_DCLK_SOURCE_PORT, SI446X_A_DCLK_SOURCE_PIN);   //lq DCLK
    	
    	/* Configure EXTI Line to generate an interrupt on falling edge */  
    	EXTI_InitStructure.EXTI_Line = SI446X_A_DCLK_LINE;
    	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //lq TxʱMCU��ʱ���½��ظ������ݣ�SI446X�������ز�����
    	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    	EXTI_Init(&EXTI_InitStructure); 
    }
    else
    {
        pSI446XStruct = &SI446XB;

    	//lq SI446X_B_PAC_ON();	
        
        /*lq data pin config */
        GPIO_InitStructure.GPIO_Pin = SI446X_B_DATA_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //lq TX
        GPIO_Init(SI446X_B_DATA_PORT, &GPIO_InitStructure);
    
    	/*lq clock pin config */
    	/* Connect EXTI Line to GPIO Pin */
    	GPIO_EXTILineConfig(SI446X_B_DCLK_SOURCE_PORT, SI446X_B_DCLK_SOURCE_PIN);   //lq DCLK
    	
    	/* Configure EXTI Line to generate an interrupt on falling edge */  
    	EXTI_InitStructure.EXTI_Line = SI446X_B_DCLK_LINE;
    	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //lq TxʱMCU��ʱ���½��ظ������ݣ�SI446X�������ز�����
    	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    	EXTI_Init(&EXTI_InitStructure); 
    }
#if 1
    /*lq Normal Tx configuration */
    SI446X_SET_PROPERTY_1(MODEM_MOD_TYPE, 0x2B, cs);        //lq TX_DIRECT_MODE_TYPE = Direct mode operates in synchronous mode, TX_DIRECT_MODE_GPIO = GPIO1, MOD_SOURCE = direct mode pin, MOD_TYPE = 2GFSK

    /*lq Si446x GPIO config */
    SI446X_GpioPinCfg(SI446X_MODE_TX, cs);

    /*lq SI446X Tx config */
    SI446X_CHANGE_STATE(SI446X_STATE_READY, cs);            //lq... Si4463��Txֱ̬��ת��Tx̬ʱ����ʧ�� 
    if (channel == ENUM_AIS_CHANNEL_A)
    {
        SI446X_START_TX(SI446X_CHANNEL_AIS_A, 0x80, 0, cs);
    }
    else
    {
        SI446X_START_TX(SI446X_CHANNEL_AIS_B, 0x80, 0, cs);
    }
#endif	
    pSI446XStruct->SI446XStatus = SI446X_MODE_TX;
}

/*******************************************************************************
* ����  : SI446X_CfgCwTx
* ����  : �����ز���������
* ����  : cs - SI446XоƬ����	  
          channel - �ŵ�
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 SI446X_CfgCwTx(SI446XChipEnum cs, AisChannelEnum channel)
{
    SI446XStruct *pSI446XStruct;

    if (cs == SI446X_CHIP_A)
    {
        pSI446XStruct = &SI446XA;

    	//lq SI446X_A_PAC_ON();	
    }
    else if (cs == SI446X_CHIP_B)
    {
        pSI446XStruct = &SI446XB;

    	//lq SI446X_B_PAC_ON();	
    }
    else
    {
        return FALSE;
    }

    /*lq CW TX config*/
    SI446X_SET_PROPERTY_1(MODEM_MOD_TYPE, 0x28, cs);        //lq TX_DIRECT_MODE_TYPE = Direct mode operates in synchronous mode, TX_DIRECT_MODE_GPIO = GPIO1, MOD_SOURCE = direct mode pin, MOD_TYPE = CW
    SI446X_CHANGE_STATE(SI446X_STATE_READY, cs);            //lq... Si4463��Txֱ̬��ת��Tx̬ʱ����ʧ�� 
    
    if (channel == ENUM_AIS_CHANNEL_A)
    {
        SI446X_START_TX(SI446X_CHANNEL_AIS_A, 0x80, 0, cs);
    }
    else
    {
        SI446X_START_TX(SI446X_CHANNEL_AIS_B, 0x80, 0, cs);
    }

    pSI446XStruct->SI446XStatus = SI446X_MODE_TX;                      

    return TRUE;
}

/*******************************************************************************
* ����  : SI446X_CfgPN9Tx
* ����  : ����PN9��������
* ����  : cs - SI446XоƬ����	  
          channel - �ŵ�
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 SI446X_CfgPN9Tx(SI446XChipEnum cs, AisChannelEnum channel)
{
    SI446XStruct *pSI446XStruct;

    if (cs == SI446X_CHIP_A)
    {
        pSI446XStruct = &SI446XA;

    	//lq SI446X_A_PAC_ON();	
    }
    else if (cs == SI446X_CHIP_B)
    {
        pSI446XStruct = &SI446XB;

    	//lq SI446X_B_PAC_ON();	
    }
    else
    {
        return FALSE;
    }
    
    /*lq PN9 TX config*/
    SI446X_SET_PROPERTY_1(MODEM_MOD_TYPE, 0x33, cs);        //lq TX_DIRECT_MODE_TYPE = Direct mode operates in synchronous mode, TX_DIRECT_MODE_GPIO = GPIO1, MOD_SOURCE = pseudo-random generator, MOD_TYPE = 2GFSK
    SI446X_CHANGE_STATE(SI446X_STATE_READY, cs);            //lq... Si4463��Txֱ̬��ת��Tx̬ʱ����ʧ�� 

    if (channel == ENUM_AIS_CHANNEL_A)
    {
        SI446X_START_TX(SI446X_CHANNEL_AIS_A, 0x80, 0, cs);
    }
    else
    {
        SI446X_START_TX(SI446X_CHANNEL_AIS_B, 0x80, 0, cs);
    }

    pSI446XStruct->SI446XStatus = SI446X_MODE_TX;                      

    return TRUE;
}
#if 0
/*******************************************************************************
* ����  : SI446X_CfgCwTx
* ����  : �����ز���������
* ����  : cs - SI446XоƬ����	  
          channel - �ŵ�
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 SI446X_CfgTestRx(SI446XChipEnum cs, AisChannelEnum channel, U8 rxTestMode)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
    SI446XStruct *pSI446XStruct;

    if (cs == SI446X_CHIP_A)
    {
        pSI446XStruct = &SI446XA;

    	//lq SI446X_A_PAC_OFF();	

        /*lq data pin config */
        GPIO_InitStructure.GPIO_Pin = SI446X_A_DATA_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //lq RX
        GPIO_Init(SI446X_A_DATA_PORT, &GPIO_InitStructure);
        
        /*lq clock pin config */
        /* Connect EXTI Line to GPIO Pin */
        GPIO_EXTILineConfig(SI446X_A_DCLK_SOURCE_PORT, SI446X_A_DCLK_SOURCE_PIN);   //lq DCLK
        
        /* Configure EXTI Line to generate an interrupt on rising edge */  
        EXTI_InitStructure.EXTI_Line = SI446X_A_DCLK_LINE;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //lq RxʱMCU��ʱ�������ز�������
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStructure); 
    }
    else if (cs == SI446X_CHIP_B)
    {
        pSI446XStruct = &SI446XB;

    	//lq SI446X_B_PAC_OFF();	

        /*lq data pin config */
        GPIO_InitStructure.GPIO_Pin = SI446X_B_DATA_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //lq RX
        GPIO_Init(SI446X_B_DATA_PORT, &GPIO_InitStructure);
        
        /*lq clock pin config */
        /* Connect EXTI Line to GPIO Pin */
        GPIO_EXTILineConfig(SI446X_B_DCLK_SOURCE_PORT, SI446X_B_DCLK_SOURCE_PIN);   //lq DCLK
        
        /* Configure EXTI Line to generate an interrupt on rising edge */  
        EXTI_InitStructure.EXTI_Line = SI446X_B_DCLK_LINE;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //lq RxʱMCU��ʱ�������ز�������
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStructure); 
    }
    else
    {
        return FALSE;
    }

    if (channel == ENUM_AIS_CHANNEL_A)
    {
        //lq...
    }
    else
    {
        //lq...
    }
    pSI446XStruct->SI446XStatus = SI446X_MODE_RX;                      

    return TRUE;
}
#endif
/*******************************************************************************
* ����  : SI446X_PAOn
* ����  : SI446X���ſ���
* ����  : cs - Ƭѡ
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 SI446X_PAOn(SI446XChipEnum cs)
{
    if (cs == SI446X_CHIP_A)
    {
        //lq...
    }
    else if (cs == SI446X_CHIP_B)
    {
        //lq...
    }
    else
    {
        return FALSE;
    }

    //lq...
    return TRUE;
}

/*******************************************************************************
* ����  : SI446X_PAOff
* ����  : SI446X���Źر�
* ����  : cs - Ƭѡ
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 SI446X_PAOff(SI446XChipEnum cs)
{
    if (cs == SI446X_CHIP_A)
    {
        //lq...
    }
    else if (cs == SI446X_CHIP_B)
    {
        //lq...
    }
    else
    {
        return FALSE;
    }

    //lq...
    return TRUE;
}

/*******************************************************************************
* ����  : SI446X_GetRssi
* ����  : ��ȡSI446X���ź�ǿ��
* ����  : cs - Ƭѡ
* ���  : ��
* ����  : ��
*******************************************************************************/
S16 SI446X_GetRssi(SI446XChipEnum cs)
{
    S16 tempS16;
    U8 buffer[10];
    
    SI446X_GET_MODEM_STATUS(buffer, cs);
    tempS16 = (buffer[3] >> 1) - SI446X_RSSI_CAL;    

    return tempS16;
}

/*******************************************************************************
* ����  : SI446X_StateTo
* ����  : �л��շ���״̬
* ����  : state - ת���״̬
          cs - Ƭѡ	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SI446X_StateTo(U8 state, SI446XChipEnum cs)
{
    SI446XStruct *pSI446XStruct;

    if (cs == SI446X_CHIP_A)
    {   
        pSI446XStruct = &SI446XA;
    }
    else
    {
        pSI446XStruct = &SI446XB;
    }

    if (state == SI446X_MODE_TX)
    {
        SI446X_CHANGE_STATE(SI446X_STATE_TX, cs);
    }
    else if (state == SI446X_MODE_RX)
    {
        SI446X_CHANGE_STATE(SI446X_STATE_RX, cs);
    }
    else
    {
        SI446X_CHANGE_STATE(SI446X_STATE_READY, cs);
    }

    pSI446XStruct->SI446XStatus = state;    
}

/*******************************************************************************
* ����  : SI446X_IsTxDone
* ����  : �жϷ����Ƿ����
* ����  : cs - SI446XоƬ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 SI446X_IsTxDone(SI446XChipEnum cs)
{
    return TRUE;    //lq... ������ɱ�ʾ��ʱδ��
}

/*******************************************************************************
* ����  : SI446X_A_RxClockIRQS
* ����  : SI446X_A�շ�ʱ���ж�
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SI446X_A_TxRxClockIRQS(void)
{
    U8  currValue;
    U8  decodeValue;

	if(EXTI_GetITStatus(SI446X_A_DCLK_LINE) != RESET)
	{ 
		/* Clear the EXTI line pending bit */  
		EXTI_ClearITPendingBit(SI446X_A_DCLK_LINE);

	   #if  CONFIG_PTT_EN  //lnw  add
          if ((SI446XA.SI446XStatus == SI446X_MODE_RX)&&(PTT_OBJ.LowPowerMode_Staus==DISABLE))
	   #else		
		  if (SI446XA.SI446XStatus == SI446X_MODE_RX)
	   #endif	
        {
            /*lq ��ȡRX data GPIO���� */
            currValue = SI446X_A_DATA_IN();
#if 1
            if(PHY_InstNRZIDecode(NRZIType1, currValue, &decodeValue) == TRUE)
            {
                DLS_InstBitStreamPutBit(PacketProbeType1, decodeValue);
            }
#else
            DLS_InstBitStreamPutBit(PacketProbeType1, currValue);
#endif
        }
        else if (SI446XA.SI446XStatus == SI446X_MODE_TX)
        {
            /*lq �ṩ�������� */
            if ((DevDynamicPara.ucWorkMode == EQ_WORKMOD_TEST)
                && (DevDynamicPara.TestSta == EQ_MOD_TEST_STA_MAIN))
            {
                /*lq ����̬���� */
                if (TRCTRL_InstGetTxTestFlg(TransceiverTypeA) == TRUE)
                {
                    if (TRCTRL_InstTxTestIsTx(TransceiverTypeA) == TRUE)
                    {
                        if (TRCTRL_InstTxTestGetSignalBit(TransceiverTypeA) == 0)
                        {
                            SI446X_A_DATA_LOW(); 
                        }
                        else
                        {
                            SI446X_A_DATA_HIGH();
                        }                
                    }
                }        
            }
            else
            {
			/*zjw ��������̬��������̬���Լ��Ƿ��� */
#if 1 
                if ((DevDynamicPara.ucWorkMode == EQ_WORKMOD_NORMAL)
                    || ((DevDynamicPara.ucWorkMode == EQ_WORKMOD_TEST)
                    && (DevDynamicPara.TestSta == EQ_MOD_TEST_STA_SELF_CHECK)))
                {
                    TRCTRL_TxDataClockISR(TransceiverTypeA);
                }
#else
                /*lq ����̬���� */
                if (TRCTRL_InstGetTxTestFlg(TransceiverTypeA) == TRUE)
                {
                    if (TRCTRL_InstTxTestIsTx(TransceiverTypeA) == TRUE)
                    {
                        if (TRCTRL_InstTxTestGetSignalBit(TransceiverTypeA) == 0)
                        {
                            SI446X_A_DATA_LOW(); 
                        }
                        else
                        {
                            SI446X_A_DATA_HIGH();
                        }                
                    }
                }
#endif            
            }        
        }
	}
}

/*******************************************************************************
* ����  : SI446X_B_RxClockIRQS
* ����  : SI446X_B�շ�ʱ���ж�
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SI446X_B_TxRxClockIRQS(void)
{
    U8  currValue;
    U8  decodeValue;

	if(EXTI_GetITStatus(SI446X_B_DCLK_LINE) != RESET)
	{ 
		/* Clear the EXTI line pending bit */  
		EXTI_ClearITPendingBit(SI446X_B_DCLK_LINE);

       #if  CONFIG_PTT_EN  //lnw  add
          if((SI446XB.SI446XStatus == SI446X_MODE_RX)&&(PTT_OBJ.LowPowerMode_Staus==DISABLE))
	   #else		
		  if (SI446XB.SI446XStatus == SI446X_MODE_RX)
	   #endif	   		
        {
            /*lq ��ȡRX data GPIO���� */
            currValue = SI446X_B_DATA_IN();
#if 1
            if (PHY_InstNRZIDecode(NRZIType2, currValue, &decodeValue) == TRUE)
            {
                DLS_InstBitStreamPutBit(PacketProbeType2, decodeValue);
            }
#else
            DLS_InstBitStreamPutBit(PacketProbeType2, currValue);
#endif
        }
        else if (SI446XB.SI446XStatus == SI446X_MODE_TX)
        {
            /*lq �ṩ�������� */
            if ((DevDynamicPara.ucWorkMode == EQ_WORKMOD_TEST)
                && (DevDynamicPara.TestSta == EQ_MOD_TEST_STA_MAIN))
            {
                /*lq ����̬���� */
                if (TRCTRL_InstGetTxTestFlg(TransceiverTypeB) == TRUE)
                {
                    if (TRCTRL_InstTxTestIsTx(TransceiverTypeB) == TRUE)
                    {
                        if (TRCTRL_InstTxTestGetSignalBit(TransceiverTypeB) == 0)
                        {
                            SI446X_B_DATA_LOW(); 
                        }
                        else
                        {
                            SI446X_B_DATA_HIGH();
                        }                
                    }
                }        
            }
            else
            {
#if 1 
                if ((DevDynamicPara.ucWorkMode == EQ_WORKMOD_NORMAL)
                    || ((DevDynamicPara.ucWorkMode == EQ_WORKMOD_TEST)
                    && (DevDynamicPara.TestSta == EQ_MOD_TEST_STA_SELF_CHECK)))
                {
                    TRCTRL_TxDataClockISR(TransceiverTypeB);
                }
#else
                /*lq ����̬���� */
                if (TRCTRL_InstGetTxTestFlg(TransceiverTypeB) == TRUE)
                {
                    if (TRCTRL_InstTxTestIsTx(TransceiverTypeB) == TRUE)
                    {
                        if (TRCTRL_InstTxTestGetSignalBit(TransceiverTypeB) == 0)
                        {
                            SI446X_B_DATA_LOW(); 
                        }
                        else
                        {
                            SI446X_B_DATA_HIGH();
                        }                
                    }
                }
#endif            
            }        
        }
	}
}

/*******************************************************************************
* ����  : SI446X_ModuleInit
* ����  : ģ���ʼ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void SI446X_ModuleInit(void) 
{
	U8 ucFlag;
	U8 ucTemp;
	U32 ulTemp1;
	U32 ulTemp2;
	U32 ulDeviceIDBuff[11];

//    SI446X_SI446XStructInit(&SI446XA);
//    SI446X_SI446XStructInit(&SI446XB);

    /* ��Դ���� */
    SI446X_InitPowerPins();
//	Delay_ms(1000);
//	Feed_Dog();
//	Delay_ms(1000);
//	Feed_Dog();

	/*lq �ܽ����� */
    SI446X_InitPins(SI446X_CHIP_A);
    SI446X_InitPins(SI446X_CHIP_B);

	ucFlag = 2;
	while(ucFlag > 0)
	{
		/* ���ܣ���û�׼Ƶ�� */
		ucTemp = 0;
		ReadDeviceID(ulDeviceIDBuff);
		do
		{
			ulTemp1 = ReadCry(ucTemp);
			ulTemp2 = Decode(ulTemp1, ulDeviceIDBuff, 10);
			ucTemp++;
		}while(ucTemp < 3 && ulTemp2 != 0);
	
		//���û�׼Ƶ��
		SI446X_SETFREBASE(ulTemp2);
	
		/* ��ʼ��SI446X�Ľṹ�� */
		SI446X_SI446XStructInit(&SI446XA);
	    SI446X_SI446XStructInit(&SI446XB);
	
	//	/*lq �ܽ����� */
	//    SI446X_InitPins(SI446X_CHIP_A);
	//    SI446X_InitPins(SI446X_CHIP_B);
	
	    /*lq оƬ���� */
	    SI446X_ConfigInit(SI446X_CHIP_A);
	    SI446X_ConfigInit(SI446X_CHIP_B);
		ucFlag--;
	}
    
    /*lq SI446X Rx ���� */
    SI446X_RxCfg(SI446X_CHIP_A, ENUM_AIS_CHANNEL_A);
    SI446X_RxCfg(SI446X_CHIP_B, ENUM_AIS_CHANNEL_B);

    //lq usart_printf("$SI446X: Device part number %04X!\r\n", SI446X_ReadReg(0x000001C7, SI446X_CHIP_A));
}

/*******************************************************************************
* ����  : SI446X_SPI_ExchangeByte
* ����  : ʹ��SPI1���ͺͽ���һ���ֽڵ����ݣ������ز�����SPI_CPOL = SPI_CPOL_Low��
          SPI_CPHA = SPI_CPHA_1Edge����MSB����
* ����  : sendData - �����ֽ�
          cs - Ƭѡ          
* ���  : ��
* ����  : �����ֽ�
*******************************************************************************/
U8 SI446X_SPI_ExchangeByte(U8 sendData, SI446XChipEnum cs)
{
    if (cs == SI446X_CHIP_A)
    {
        return GPIO_SPI1_ExchangeByte(sendData);
    }
    else
    {
        return GPIO_SPI2_ExchangeByte(sendData);
    }
}

/*******************************************************************************
* ����  : SI446X_GpioPinCfg
* ����  : ����Si446x GPIO
* ����  : txRxMode - �շ�ģʽ
          cs - Ƭѡ          
* ���  : ��
* ����  : ��
*******************************************************************************/
void SI446X_GpioPinCfg(U8 txRxMode, SI446XChipEnum cs)
{
    if (txRxMode == SI446X_MODE_TX)
    {
#if 0
        SI446X_GPIO_PIN_CFG(0x04, 0x50, 0x61, 0x60, 0x00, 0x00, 0x00, cs);  //lq RX: GPIO2 = 1, GPIO3 = 0; TX: GPIO2 = 0, GPIO3 = 1
#else
        SI446X_GPIO_PIN_CFG(0x50, 0x04, 0x60, 0x61, 0x00, 0x00, 0x00, cs);  //lq RX: GPIO2 = 0, GPIO3 = 1; TX: GPIO2 = 1, GPIO3 = 0
#endif
    }
    else
    {
#if 0
        SI446X_GPIO_PIN_CFG(0x54, 0x51, 0x61, 0x60, 0x00, 0x00, 0x00, cs);       
#else 
        SI446X_GPIO_PIN_CFG(0x51, 0x54, 0x60, 0x61, 0x00, 0x00, 0x00, cs);       
#endif
    }
}

/*
=================================================================================
SI446X_Cmd();
Function : Send a command to the device
INTPUT   : cmd, the buffer stores the command array
           cmdSize, the size of the command array
OUTPUT   : NONE
=================================================================================
*/
void SI446X_Cmd(U8 *cmd, U8 cmdSize, SI446XChipEnum cs)
{
    SI446X_WaitCts(cs);

    if (cs == SI446X_CHIP_A)
    {
        SI446X_A_CS_ON();
    }
    else
    {
        SI446X_B_CS_ON();
    }
    
    while(cmdSize--)
    {
        SI446X_SPI_ExchangeByte(*cmd++, cs);
    }

    if (cs == SI446X_CHIP_A)
    {
        SI446X_A_CS_OFF();
    }
    else
    {
        SI446X_B_CS_OFF();
    }
}

/*
=================================================================================
SI446X_Cmd();
Function : Send a command to the device. This function is used for the self check
           of SI446X.
INTPUT   : cmd, the buffer stores the command array
           cmdSize, the size of the command array
           csEn, whether cs is enable or not.
OUTPUT   : NONE
=================================================================================
*/
void SI446X_CmdSelfCheck(U8 *cmd, U8 cmdSize, U8 csEn, SI446XChipEnum cs)
{
    SI446X_WaitCts(cs);

    if (cs == SI446X_CHIP_A)
    {
        if (csEn == TRUE)
        {
            SI446X_A_CS_ON();
        }
        else
        {
            SI446X_A_CS_OFF();
        }
    }
    else
    {
        if (csEn == TRUE)
        {
            SI446X_B_CS_ON();
        }
        else
        {
            SI446X_B_CS_OFF();
        }
    }
    
    while(cmdSize--)
    {
        SI446X_SPI_ExchangeByte(*cmd++, cs);
    }

    if (cs == SI446X_CHIP_A)
    {
        SI446X_A_CS_OFF();
    }
    else
    {
        SI446X_B_CS_OFF();
    }
}

/*
=================================================================================
SI446X_POWER_UP();
Function : Power up the device
INTPUT   : bootOptions - Boot options
           xtalOptions - Xtal options
           f_xtal, the frequency of the external high-speed crystal
OUTPUT   : NONE
=================================================================================
*/
void SI446X_POWER_UP(U8 bootOptions, U8 xtalOptions, U32 f_xtal, SI446XChipEnum cs)
{
    U8 cmd[7];
    
    cmd[0] = POWER_UP;
    cmd[1] = bootOptions;
    cmd[2] = xtalOptions;
    cmd[3] = f_xtal>>24;
    cmd[4] = f_xtal>>16;
    cmd[5] = f_xtal>>8;
    cmd[6] = f_xtal;
    
    SI446X_Cmd(cmd, 7, cs);
}

/*
=================================================================================
SI446X_IRCAL();
Function : Power up the device
INTPUT   : arg1-����1��arg1-����2��arg1-����3��arg1-����4
           cs-Ƭѡ
OUTPUT   : NONE
=================================================================================
*/
void SI446X_IRCAL(U8 arg1, U8 arg2, U8 arg3, U8 arg4, SI446XChipEnum cs)
{
    U8 cmd[5];
    
    cmd[0] = IRCAL;
    cmd[1] = arg1;
    cmd[2] = arg2;
    cmd[3] = arg3;
    cmd[4] = arg4;
    
    SI446X_Cmd(cmd, 5, cs);
}

/*
=================================================================================
SI446X_ReadResponse();
Function : read a array of command response
INTPUT   : buffer,  a buffer, stores the data responsed
           size,    How many bytes should be read
OUTPUT   : NONE
=================================================================================
*/
void SI446X_ReadResponse(U8 *buffer, U8 size, SI446XChipEnum cs)
{
    SI446X_WaitCts(cs);

    if (cs == SI446X_CHIP_A)
    {
        SI446X_A_CS_ON();
    }
    else
    {
        SI446X_B_CS_ON();
    }

	SI446X_SPI_ExchangeByte(READ_CMD_BUFF, cs);
	while(size--)
    {
        *buffer++ = SI446X_SPI_ExchangeByte(0xFF, cs);
    }

    if (cs == SI446X_CHIP_A)
    {
        SI446X_A_CS_OFF();
    }
    else
    {
        SI446X_B_CS_OFF();
    }
}

/*
=================================================================================
SI446X_ReadResponse();
Function : read a array of command response. This function is used for the self
           check of SI446X.
INTPUT   : buffer,  a buffer, stores the data responsed
           size,    How many bytes should be read
           csEn, whether cs is enable or not.
OUTPUT   : NONE
=================================================================================
*/
void SI446X_ReadResponseSelfCheck(U8 *buffer, U8 size, U8 csEn, SI446XChipEnum cs)
{
    SI446X_WaitCts(cs);

    if (cs == SI446X_CHIP_A)
    {
        if (csEn == TRUE)
        {
            SI446X_A_CS_ON();
        }
        else
        {
            SI446X_A_CS_OFF();
        }
    }
    else
    {
        if (csEn == TRUE)
        {
            SI446X_B_CS_ON();
        }
        else
        {
            SI446X_B_CS_OFF();
        }
    }

	SI446X_SPI_ExchangeByte(READ_CMD_BUFF, cs);
	while(size--)
    {
        *buffer++ = SI446X_SPI_ExchangeByte(0xFF, cs);
    }

    if (cs == SI446X_CHIP_A)
    {
        SI446X_A_CS_OFF();
    }
    else
    {
        SI446X_B_CS_OFF();
    }
}

/*
=================================================================================
SI446X_WaitCts();
Function : wait the device ready to response a command
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
void SI446X_WaitCts(SI446XChipEnum cs)
{
    U8 cts;
    do
    {
        if (cs == SI446X_CHIP_A)
        {
            SI446X_A_CS_ON();
        }
        else
        {
            SI446X_B_CS_ON();
        }
        SI446X_SPI_ExchangeByte(READ_CMD_BUFF, cs);
        cts = SI446X_SPI_ExchangeByte(0xFF, cs);

        if (cs == SI446X_CHIP_A)
        {
            SI446X_A_CS_OFF();
        }
        else
        {
            SI446X_B_CS_OFF();
        }        
    }while(cts != 0xFF);
}
/*
=================================================================================
SI446X_NOP();
Function : NO Operation command
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
U8 SI446X_NOP(SI446XChipEnum cs)
{
    U8 cts;

    if (cs == SI446X_CHIP_A)
    {
        SI446X_A_CS_ON();
    }
    else
    {
        SI446X_B_CS_ON();
    }
    
    cts = SI446X_SPI_ExchangeByte(NOP, cs);
    
    if (cs == SI446X_CHIP_A)
    {
        SI446X_A_CS_OFF();
    }
    else
    {
        SI446X_B_CS_OFF();
    }

	return cts;
}

/*
=================================================================================
SI446X_PART_INFO();
Function : Read the PART_INFO of the device, 8 bytes needed
INTPUT   : buffer, the buffer stores the part information
OUTPUT   : NONE
=================================================================================
*/
void SI446X_PART_INFO(U8 *buffer, SI446XChipEnum cs)
{
    U8 cmd = PART_INFO;

    SI446X_Cmd(&cmd, 1, cs);
#if 0
    SI446X_ReadResponse(buffer, 8, cs);
#else
    SI446X_ReadResponse(buffer, 9, cs);
#endif
}

/*
=================================================================================
SI446X_PART_INFO();
Function : Read the PART_INFO of the device, 8 bytes needed. This function is
           used for the self check of SI446X.
INTPUT   : buffer, the buffer stores the part information
           csEn, whether cs is enable or not.
OUTPUT   : NONE
=================================================================================
*/
void SI446X_PART_INFO_SelfCheck(U8 *buffer, U8 csEn, SI446XChipEnum cs)
{
    U8 cmd = PART_INFO;

    SI446X_CmdSelfCheck(&cmd, 1, csEn, cs);
#if 0
    SI446X_ReadResponseSelfCheck(buffer, 8, csEn, cs);
#else
    SI446X_ReadResponseSelfCheck(buffer, 9, csEn, cs);
#endif
}

/*
=================================================================================
SI446X_FUNC_INFO();
Function : Read the FUNC_INFO of the device, 7 bytes needed
INTPUT   : buffer, the buffer stores the FUNC information
OUTPUT   : NONE
=================================================================================
*/
void SI446X_FUNC_INFO(U8 *buffer, SI446XChipEnum cs)
{
    U8 cmd = FUNC_INFO;

    SI446X_Cmd(&cmd, 1, cs);
    SI446X_ReadResponse(buffer, 7, cs);
}

/*
=================================================================================
SI446X_REQUEST_DEVICE_STATE();
Function : Read the FUNC_INFO of the device, 3 bytes needed
INTPUT   : buffer, the buffer stores the DEVICE_STATE information
OUTPUT   : NONE
=================================================================================
*/
void SI446X_REQUEST_DEVICE_STATE(U8 *buffer, SI446XChipEnum cs)
{
    U8 cmd = REQUEST_DEVICE_STATE;

    SI446X_Cmd(&cmd, 1, cs);
    SI446X_ReadResponse(buffer, 3, cs);
}

/*
=================================================================================
SI446X_REQUEST_DEVICE_STATE();
Function : Read the FUNC_INFO of the device, 3 bytes needed
INTPUT   : newState, the new state of the device
OUTPUT   : NONE
=================================================================================
*/
void SI446X_CHANGE_STATE(U8 newState, SI446XChipEnum cs)
{
    U8 cmd[2];
    
    cmd[0] = CHANGE_STATE;
    cmd[1] = newState;

    SI446X_Cmd(cmd, 2, cs);
}

/*
=================================================================================
SI446X_GET_INT_STATUS();
Function : Read the INT status of the device, 9 bytes needed
INTPUT   : buffer, the buffer stores the int status
OUTPUT   : NONE
=================================================================================
*/
void SI446X_GET_INT_STATUS(U8 *buffer, SI446XChipEnum cs)
{
    U8 cmd[4];
    
    cmd[0] = GET_INT_STATUS;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;

    SI446X_Cmd(cmd, 4, cs);
    SI446X_ReadResponse(buffer, 9, cs);
}

/*
=================================================================================
SI446X_GET_MODEM_STATUS();
Function : Read the MODEM status of the device, 9 bytes needed
INTPUT   : buffer, the buffer stores the int status
OUTPUT   : NONE
=================================================================================
*/
void SI446X_GET_MODEM_STATUS(U8 *buffer, SI446XChipEnum cs)
{
    U8 cmd;
    
    cmd = GET_MODEM_STATUS;

    SI446X_Cmd(&cmd, 4, cs);
    SI446X_ReadResponse(buffer, 9, cs);
}

/*
=================================================================================
SI446X_GET_PROPERTY_X();
Function : Read the PROPERTY of the device
INTPUT   : buffer, the buffer stores the PROPERTY value
           GROUP_NUM, the group and number of the parameter
           NUM_GROUP, number of the group
OUTPUT   : NONE
=================================================================================
*/
void SI446X_GET_PROPERTY_X(SI446XPropertyEnum GROUP_NUM, U8 NUM_PROPS, U8 *buffer,
                           SI446XChipEnum cs)
{
    U8 cmd[4];

    cmd[0] = GET_PROPERTY;
    cmd[1] = GROUP_NUM>>8;
    cmd[2] = NUM_PROPS;
    cmd[3] = GROUP_NUM;

    SI446X_Cmd(cmd, 4, cs);
    SI446X_ReadResponse(buffer, NUM_PROPS + 1, cs);
}
/*
=================================================================================
SI446X_SET_PROPERTY_X();
Function : Set the PROPERTY of the device
INTPUT   : GROUP_NUM, the group and the number of the parameter
           NUM_GROUP, number of the group
           PAR_BUFF, buffer stores parameters
OUTPUT   : NONE
=================================================================================
*/
void SI446X_SET_PROPERTY_X(SI446XPropertyEnum GROUP_NUM, U8 NUM_PROPS, U8 *PAR_BUFF,
                           SI446XChipEnum cs)
{
    U8 cmd[20], i = 0;
    
    if(NUM_PROPS >= 16)
    {
        return;
    }
    cmd[i++] = SET_PROPERTY;
    cmd[i++] = GROUP_NUM>>8;
    cmd[i++] = NUM_PROPS;
    cmd[i++] = GROUP_NUM;
    while(NUM_PROPS--)
    {
        cmd[i++] = *PAR_BUFF++;
    }
    
    SI446X_Cmd(cmd, i, cs);
}
/*
=================================================================================
SI446X_SET_PROPERTY_1();
Function : Set the PROPERTY of the device, only 1 byte
INTPUT   : GROUP_NUM, the group and number index
           para,  the value to be set
OUTPUT   : NONE
=================================================================================
*/
void SI446X_SET_PROPERTY_1(SI446XPropertyEnum GROUP_NUM, U8 para,
                           SI446XChipEnum cs)
{
    U8 cmd[5];

    cmd[0] = SET_PROPERTY;
    cmd[1] = GROUP_NUM>>8;
    cmd[2] = 1;
    cmd[3] = GROUP_NUM;
    cmd[4] = para;
    
    SI446X_Cmd(cmd, 5, cs);
}
/*
=================================================================================
SI446X_GET_PROPERTY_1();
Function : Get the PROPERTY of the device, only 1 byte
INTPUT   : GROUP_NUM, the group and number index
OUTPUT   : the PROPERTY value read from device
=================================================================================
*/
U8 SI446X_GET_PROPERTY_1(SI446XPropertyEnum GROUP_NUM, SI446XChipEnum cs)
{
    U8 cmd[4];

    cmd[0] = GET_PROPERTY;
    cmd[1] = GROUP_NUM>>8;
    cmd[2] = 1;
    cmd[3] = GROUP_NUM;
    
    SI446X_Cmd(cmd, 4, cs);
    SI446X_ReadResponse(cmd, 2, cs);
    
    return cmd[1];  //lq cmd[0]ΪCTS
}
/*
=================================================================================
SI446X_Reset();
Function : reset the SI446x device
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
void SI446X_Reset(SI446XChipEnum cs)
{
    U16 x = 255;

    if (cs == SI446X_CHIP_A)
    {
        SI446X_A_CE_OFF();
    }
    else
    {
        SI446X_B_CE_OFF();
    }

#if 0
    while(x--);
#else
    Delay_ms(20);
#endif

    if (cs == SI446X_CHIP_A)
    {
        SI446X_A_CE_ON();
        SI446X_A_CS_OFF();
    }
    else
    {
        SI446X_B_CE_ON();
        SI446X_B_CS_OFF();
    }

    Delay_ms(20);
}

/*
=================================================================================
SI446X_IQCalibration();
Function : Image rejection and IQ calibration��ʹ��WDS��������
            Si4463_DirectRx_161975000Hz_TCXO26MHz_RevB1_IQCalib
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
void SI446X_IQCalibration(SI446XChipEnum cs)
{
    SI446X_SET_PROPERTY_1(GLOBAL_XO_TUNE, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_MOD_TYPE, 0x2B, cs);
    SI446X_SET_PROPERTY_1(MODEM_MAP_CONTROL, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_DSM_CTRL, 0x07, cs);
    SI446X_SET_PROPERTY_1(MODEM_CLKGEN_BAND, 0x0D, cs);
    SI446X_SET_PROPERTY_1(SYNTH_PFDCP_CPFF, 0x2C, cs);
    SI446X_SET_PROPERTY_1(SYNTH_PFDCP_CPINT, 0x0E, cs);
    SI446X_SET_PROPERTY_1(SYNTH_VCO_KV, 0x0B, cs);
    SI446X_SET_PROPERTY_1(SYNTH_LPFILT3, 0x04, cs);
    SI446X_SET_PROPERTY_1(SYNTH_LPFILT2, 0x0C, cs);
    SI446X_SET_PROPERTY_1(SYNTH_LPFILT1, 0x73, cs);
    SI446X_SET_PROPERTY_1(SYNTH_LPFILT0, 0x03, cs);
    SI446X_SET_PROPERTY_1(MODEM_DATA_RATE_2, 0x02, cs);
    SI446X_SET_PROPERTY_1(MODEM_DATA_RATE_1, 0x71, cs);
    SI446X_SET_PROPERTY_1(MODEM_DATA_RATE_0, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_TX_NCO_MODE_3, 0x05, cs);
    SI446X_SET_PROPERTY_1(MODEM_TX_NCO_MODE_2, 0x8C, cs);
    SI446X_SET_PROPERTY_1(MODEM_TX_NCO_MODE_1, 0xBA, cs);
    SI446X_SET_PROPERTY_1(MODEM_TX_NCO_MODE_0, 0x80, cs);
    SI446X_SET_PROPERTY_1(MODEM_FREQ_DEV_2, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_FREQ_DEV_1, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_FREQ_DEV_0, 0xF2, cs);
    SI446X_SET_PROPERTY_1(MODEM_TX_RAMP_DELAY, 0x01, cs);
    SI446X_SET_PROPERTY_1(PA_TC, 0x3D, cs);
    SI446X_SET_PROPERTY_1(FREQ_CONTROL_INTE, 0x47, cs);
    SI446X_SET_PROPERTY_1(FREQ_CONTROL_FRAC_2, 0x0B, cs);
    SI446X_SET_PROPERTY_1(FREQ_CONTROL_FRAC_1, 0x00, cs);
    SI446X_SET_PROPERTY_1(FREQ_CONTROL_FRAC_0, 0x00, cs);
    SI446X_SET_PROPERTY_1(FREQ_CONTROL_CHANNEL_STEP_SIZE_1, 0x17, cs);
    SI446X_SET_PROPERTY_1(FREQ_CONTROL_CHANNEL_STEP_SIZE_0, 0xA1, cs);
    SI446X_SET_PROPERTY_1(FREQ_CONTROL_W_SIZE, 0x20, cs);
    SI446X_SET_PROPERTY_1(FREQ_CONTROL_VCOCNT_RX_ADJ, 0xFA, cs);
    SI446X_SET_PROPERTY_1(MODEM_MDM_CTRL, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_IF_CONTROL, 0x08, cs);
    SI446X_SET_PROPERTY_1(MODEM_IF_FREQ_2, 0x02, cs);
    SI446X_SET_PROPERTY_1(MODEM_IF_FREQ_1, 0x80, cs);
    SI446X_SET_PROPERTY_1(MODEM_IF_FREQ_0, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_DECIMATION_CFG1, 0xB0, cs);
    SI446X_SET_PROPERTY_1(MODEM_DECIMATION_CFG0, 0x10, cs);
    SI446X_SET_PROPERTY_1(MODEM_BCR_OSR_1, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_BCR_OSR_0, 0x44, cs);
    SI446X_SET_PROPERTY_1(MODEM_BCR_NCO_OFFSET_2, 0x07, cs);
    SI446X_SET_PROPERTY_1(MODEM_BCR_NCO_OFFSET_1, 0x8F, cs);
    SI446X_SET_PROPERTY_1(MODEM_BCR_NCO_OFFSET_0, 0xD5, cs);
    SI446X_SET_PROPERTY_1(MODEM_BCR_GAIN_1, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_BCR_GAIN_0, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_BCR_GEAR, 0x02, cs);
    SI446X_SET_PROPERTY_1(MODEM_BCR_MISC1, 0xC0, cs);
    SI446X_SET_PROPERTY_1(MODEM_AFC_GEAR, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_AFC_WAIT, 0x12, cs);
    SI446X_SET_PROPERTY_1(MODEM_AFC_GAIN_1, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_AFC_GAIN_0, 0x79, cs);
    SI446X_SET_PROPERTY_1(MODEM_AFC_LIMITER_1, 0x01, cs);
    SI446X_SET_PROPERTY_1(MODEM_AFC_LIMITER_0, 0x78, cs);
    SI446X_SET_PROPERTY_1(MODEM_AFC_MISC, 0xA0, cs);
    SI446X_SET_PROPERTY_1(MODEM_AGC_CONTROL, 0xE2, cs);
    SI446X_SET_PROPERTY_1(MODEM_AGC_WINDOW_SIZE, 0x11, cs);
    SI446X_SET_PROPERTY_1(MODEM_AGC_RFPD_DECAY, 0x0F, cs);
    SI446X_SET_PROPERTY_1(MODEM_AGC_IFPD_DECAY, 0x0F, cs);
    SI446X_SET_PROPERTY_1(MODEM_FSK4_GAIN1, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_FSK4_GAIN0, 0x1A, cs);
    SI446X_SET_PROPERTY_1(MODEM_FSK4_TH1, 0x20, cs);
    SI446X_SET_PROPERTY_1(MODEM_FSK4_TH0, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_FSK4_MAP, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_OOK_PDTC, 0x28, cs);
    SI446X_SET_PROPERTY_1(MODEM_OOK_CNT1, 0xA4, cs);
    SI446X_SET_PROPERTY_1(MODEM_OOK_MISC, 0x03, cs);
    SI446X_SET_PROPERTY_1(MODEM_RAW_SEARCH, 0xD6, cs);
    SI446X_SET_PROPERTY_1(MODEM_RAW_CONTROL, 0x03, cs);
    SI446X_SET_PROPERTY_1(MODEM_RAW_EYE_1, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_RAW_EYE_0, 0x8E, cs);
    SI446X_SET_PROPERTY_1(MODEM_ANT_DIV_MODE, 0x01, cs);
    SI446X_SET_PROPERTY_1(MODEM_ANT_DIV_CONTROL, 0x80, cs);
    SI446X_SET_PROPERTY_1(MODEM_RSSI_COMP, 0x22, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE13_7_0, 0xCC, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE12_7_0, 0xA1, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE11_7_0, 0x30, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE10_7_0, 0xA0, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE9_7_0, 0x21, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE8_7_0, 0xD1, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE7_7_0, 0xB9, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE6_7_0, 0xC9, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE5_7_0, 0xEA, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE4_7_0, 0x05, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE3_7_0, 0x12, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE2_7_0, 0x11, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE1_7_0, 0x0A, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE0_7_0, 0x04, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COEM0, 0x15, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COEM1, 0xFC, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COEM2, 0x03, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COEM3, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE13_7_0, 0xCC, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE12_7_0, 0xA1, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE11_7_0, 0x30, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE10_7_0, 0xA0, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE9_7_0, 0x21, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE8_7_0, 0xD1, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE7_7_0, 0xB9, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE6_7_0, 0xC9, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE5_7_0, 0xEA, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE4_7_0, 0x05, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE3_7_0, 0x12, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE2_7_0, 0x11, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE1_7_0, 0x0A, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE0_7_0, 0x04, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COEM0, 0x15, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COEM1, 0xFC, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COEM2, 0x03, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COEM3, 0x00, cs);
    SI446X_SET_PROPERTY_1(GLOBAL_CONFIG, 0x60, cs);

    SI446X_START_RX(0, 0, 0, 0, 0, 0, cs);
    SI446X_IRCAL(0x56, 0x10, 0xCA, 0xF0, cs);   //lq WDS����ֵ
#if 1
    SI446X_NOP(cs);
    SI446X_NOP(cs);
    SI446X_NOP(cs);
    SI446X_NOP(cs);
#endif
    SI446X_IRCAL(0x13, 0x10, 0xCA, 0xF0, cs);   //lq WDS����ֵ
#if 1
    SI446X_NOP(cs);
    SI446X_NOP(cs);
    SI446X_NOP(cs);
    SI446X_NOP(cs);
#endif
}

/*
=================================================================================
SI446X_PatchInit();
Function : configuration the device with patches
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
void SI446X_PatchInit(SI446XChipEnum cs)
{
    U8  len;
    U16 startIndex = 0;

    while((len = Si446xPatchCmds[startIndex]) != 0)
    {
        startIndex += 1;
        SI446X_Cmd(Si446xPatchCmds + startIndex, len, cs);
        startIndex += len;
    }
}

/*
=================================================================================
SI446X_ConfigInit();
Function : configuration the device
INTPUT   : NONE
OUTPUT   : NONE
=================================================================================
*/
void SI446X_ConfigInit(SI446XChipEnum cs)
{
    SI446XStruct *pSI446XStruct;
 //   U8  i;
//    U16 j = 0;
    U8  buffer[10];
//    U8  cmd[10];

    if (cs == SI446X_CHIP_A)
    {
        pSI446XStruct = &SI446XA;
    }
    else
    {
        pSI446XStruct = &SI446XB;
    }
#if 1
    /*lq Patch */    
    SI446X_PatchInit(cs);

    /*lq Powerup */
    SI446X_POWER_UP(0x81, 0x01, SI446X_FREQ_XO, cs);	 //����һ��26M��Ƶ��
    SI446X_GpioPinCfg(SI446X_MODE_RX, cs);				 //��4463�ڲ������Ž�������

    /*lq ----------------------Property configuration------------------------ */
    /*lq Global configuration */
    SI446X_SET_PROPERTY_1(GLOBAL_XO_TUNE, 0x00, cs);	 //�����ڲ�����ʱ��Ϊ26M
    
    /*lq Iamge rejection and IQ calibration */
    //lq SI446X_IQCalibration(cs);

    /*lq Global configuration */
    SI446X_SET_PROPERTY_1(GLOBAL_CLK_CFG, 0x00, cs);	//ʹ�ø���Si4463��26M��ϵͳʱ��
    SI446X_SET_PROPERTY_1(GLOBAL_CONFIG, 0x20, cs);		//����4463�Ľ��뷢��ģʽ�����Ķ����б�֤�ģ�
														//FIFOģʽ(TX��Rx�ǵ����ģ����ǹ���ģ�
														//��ѭһ��İ���ʽ������ IEEE802.15.4gЭ�����
														//���ڸ�����ģʽ�����ǵ͹���ģʽ
    SI446X_SET_PROPERTY_1(GLOBAL_WUT_CONFIG, 0x00, cs); //����4463�Ļ��ѻ��ƣ�ȫ��ʧ��
    
    /*lq Interrupt config*/
    SI446X_SET_PROPERTY_1(INT_CTL_ENABLE, 0x00, cs);    //lq Disable all interrupts
														//����4463�ж����ŵ�Ӳ���ж�(����оƬ�жϡ��������жϡ����ƽ�����жϣ�
														//ȫ��ʧ�ܣ�4463���ж�����û�����ӵ�MCU�ϣ�

    /*lq Fast response register configuration */
    //lq ��ǰ��ʹ��

    /*lq Preamble configuration */
    SI446X_SET_PROPERTY_1(PREAMBLE_CONFIG, 0x20, cs);       //lq Use non-standard preamble
    SI446X_SET_PROPERTY_1(PREAMBLE_CONFIG_NSTD, 0x0F, cs);  //lq PATTERN_LENGTH = 16bits  ��15+1=16��
    SI446X_SET_PROPERTY_1(PREAMBLE_TX_LENGTH, 0x00, cs);	//�����籨��ͷ�Ĵ��䣬����ͬ���ֶ�Ϊ��һ�������ֶ�
    SI446X_SET_PROPERTY_1(PREAMBLE_PATTERN_7_0, 0x33, cs);	//PREAMBLE_PATTERN�ֶ�ֻ��ʹ��non-standard preambleģʽ��ʱ�򣬲ſ��Ա�ʹ�ã�
    SI446X_SET_PROPERTY_1(PREAMBLE_PATTERN_15_8, 0x33, cs);	//

    /*lq Sync configuration ͬ������ */
    SI446X_SET_PROPERTY_1(SYNC_CONFIG, 0xC2, cs);           //ͬ���ֽڵ��ŷ��ͻ���գ���������
															//�ڷ���ģʽ�ǲ�����ͬ���ֽڣ����ǲ�Ӱ�����ģʽ
															//�ڽ���ͬ�����ؼ��ʱ���������4���صĴ���λ
															// 4(G)FSKʧ�� 	
															// ����˹�ر���ʧ��
															//ͬ��λ�ֶεĸ�24λ��ʹ��[31:8]��ֻ����3���ֽ�
    SI446X_SET_PROPERTY_1(SYNC_BITS_31_24, 0x33, cs);       //����ͬ���ֽ�
    SI446X_SET_PROPERTY_1(SYNC_BITS_23_16, 0x33, cs);
    SI446X_SET_PROPERTY_1(SYNC_BITS_15_8, 0x33, cs);

    /*lq Packet handler configuration*/
    SI446X_SET_PROPERTY_1(PKT_CONFIG1, 0x40, cs);		    //�ڽ��պͷ���ģʽʹ��һ�鹲��Ĵ洢����
															//��������ʹ�ܣ�CRC�����Լ�У�飬�ɱ䳤�ȵİ���PH_RX_DISABLE ����ֶ�ֻ�Խ���ģʽ��Ч
															//
															//

    /*lq Modem configuration */
    SI446X_SET_PROPERTY_1(MODEM_MOD_TYPE, 0x2B, cs);        //lq TX_DIRECT_MODE_TYPE = Direct mode operates in synchronous mode, 
															//TX_DIRECT_MODE_GPIO = GPIO0, MOD_SOURCE = direct mode pin, MOD_TYPE = 2GFSK
    SI446X_SET_PROPERTY_1(MODEM_MAP_CONTROL, 0x00, cs);		//��Ϊ����˹�ر�������ͬ���ֽڳ�ʱ
															//�����ý������ݱ���
															//�����÷������ݱ���
															//������Ƶ��ƫ��ļ���
    SI446X_SET_PROPERTY_1(MODEM_DSM_CTRL, 0x07, cs);        //lq ��Щ����λ��WDS���ɣ���������Ϊ�����޸�
															//DSM��ʱ��Դ����һ����Ƶ���30MHzʱ��
															//Ӧ����PLL�ϳ����ϵ�DSM������MASH 1-1-1 DSM
															//DSM����ʧ��
															//+1/0�������͵Ķ�����ѡ������DSM����ʧ�ܣ����Ը�Ϊ������Ч
															//����DSM�ķ�����DSM����һ��������״̬�����ò���������
															//DSM��LSB���ڲ�ǿ�����ߣ���Ч��Ƶ�ʵ�ͨ���ֱ����Ǳ�������
															//DSM������Ϊ3rd order, MASH 1-1-1


    SI446X_SET_PROPERTY_1(MODEM_DATA_RATE_2, (SI446X_MODEM_DATA_RATE * 40) >> 16, cs);  //lq ����ֵ��Ϊʵ�����ʵ�40��������ѡ��ĵ���ģʽ��2GFSK��
																						//������MODEM_TX_NCO_MODE�е�TXOSR��ֵΪ1����ѡ���Ϊ40������������ֵ��Ϊʵ�����ʵ�40��
    SI446X_SET_PROPERTY_1(MODEM_DATA_RATE_1, (SI446X_MODEM_DATA_RATE * 40) >> 8, cs);
    SI446X_SET_PROPERTY_1(MODEM_DATA_RATE_0, (SI446X_MODEM_DATA_RATE * 40), cs);

    SI446X_SET_PROPERTY_1(MODEM_TX_NCO_MODE_3, 0x04 | ((SI446X_FREQ_XO >> 24) & 0x03), cs); //lq ���ݹ�ʽ�ɵ�MODEM_TX_NCO_MODE == Fxtal(����Ƶ��)Hz��
																							//TXOSR��ֵΪ1����ѡ���Ϊ40������������ֵ��Ϊʵ�ʲ����ʵ�40��
																							//����TXCNO�ڲ������������ֵΪ26M
    SI446X_SET_PROPERTY_1(MODEM_TX_NCO_MODE_2, SI446X_FREQ_XO >> 16, cs);
    SI446X_SET_PROPERTY_1(MODEM_TX_NCO_MODE_1, SI446X_FREQ_XO >> 8, cs);
    SI446X_SET_PROPERTY_1(MODEM_TX_NCO_MODE_0, SI446X_FREQ_XO, cs);

    SI446X_SET_PROPERTY_1(MODEM_FREQ_DEV_2, (pSI446XStruct->SI446X_Freq_Dev) >> 16, cs);	//����Ƶƫ	  SI446X_Freq_Dev = ((24*2400)/26000000/2)*(1<<19)+0.5
    SI446X_SET_PROPERTY_1(MODEM_FREQ_DEV_1, (pSI446XStruct->SI446X_Freq_Dev) >> 8, cs);
    SI446X_SET_PROPERTY_1(MODEM_FREQ_DEV_0, (pSI446XStruct->SI446X_Freq_Dev), cs);
    SI446X_SET_PROPERTY_1(MODEM_FREQ_OFFSET_1, (pSI446XStruct->SI446X_Freq_Offset) >> 8, cs); //����Ƶ�ʲ���ֵ SI446X_Freq_Offset = ((24*2400)/26000000/2)*(1<<19)+0.5
    SI446X_SET_PROPERTY_1(MODEM_FREQ_OFFSET_0, (pSI446XStruct->SI446X_Freq_Offset), cs);

    SI446X_SET_PROPERTY_1(MODEM_TX_RAMP_DELAY, 0x01, cs);									//���÷����½���ʱ�ӣ���λΪ����/��־λ����
    SI446X_SET_PROPERTY_1(MODEM_MDM_CTRL, 0x01, cs);										//��λԴ��ѡ����λ����������λ����������

#if (SI446X_IF_MODE == SI446X_IF_MODE_FIXED)												//ִ��
    SI446X_SET_PROPERTY_1(MODEM_IF_CONTROL, 0x08, cs);										//ѡ����յ��ƽ�����Ĳ���ģʽ��Fixed-IFģʽ
#elif (SI446X_IF_MODE == SI446X_IF_MODE_SCALED)
    SI446X_SET_PROPERTY_1(MODEM_IF_CONTROL, 0x00, cs);
#else
    SI446X_SET_PROPERTY_1(MODEM_IF_CONTROL, 0x10, cs);          //lq Zero-IF
#endif
    SI446X_SET_PROPERTY_1(MODEM_IF_FREQ_2, ((pSI446XStruct->SI446X_IF_Freq) >> 16) & 0x03, cs);	   //����ѡ���ģʽΪ�̶���Ƶģʽ������SI446X_IF_Freq = ~(24<<(12-0))+1	
																								   //��ʽΪmodem_if_freq = (2^19 * outdiv * (freq_xo / (64 * N))) / (2 * freq_xo)
    SI446X_SET_PROPERTY_1(MODEM_IF_FREQ_1, (pSI446XStruct->SI446X_IF_Freq) >> 8, cs);
    SI446X_SET_PROPERTY_1(MODEM_IF_FREQ_0, (pSI446XStruct->SI446X_IF_Freq), cs);

#if (SI446X_IF_MODE == SI446X_IF_MODE_FIXED)					//�̶���Ƶģʽ
    SI446X_SET_PROPERTY_1(MODEM_DECIMATION_CFG1, 0x70, cs);     //lq ��SI446X_FREQ_XO�й�
    SI446X_SET_PROPERTY_1(MODEM_DECIMATION_CFG0, 0x20, cs);
    SI446X_SET_PROPERTY_1(MODEM_DECIMATION_CFG2, 0x00, cs);
#elif (SI446X_IF_MODE == SI446X_IF_MODE_SCALED)					//ɨ����Ƶģʽ
    SI446X_SET_PROPERTY_1(MODEM_DECIMATION_CFG1, 0x30, cs);     //lq ��SI446X_FREQ_XO�й�
    SI446X_SET_PROPERTY_1(MODEM_DECIMATION_CFG0, 0x10, cs);
    SI446X_SET_PROPERTY_1(MODEM_DECIMATION_CFG2, 0x00, cs);
#else															//����Ƶģʽ
    SI446X_SET_PROPERTY_1(MODEM_DECIMATION_CFG1, 0x30, cs);     //lq ��SI446X_FREQ_XO�й�
    SI446X_SET_PROPERTY_1(MODEM_DECIMATION_CFG0, 0x10, cs);
    SI446X_SET_PROPERTY_1(MODEM_DECIMATION_CFG2, 0x00, cs);
#endif

    SI446X_SET_PROPERTY_1(MODEM_IFPKD_THRESHOLDS, 0xE8, cs);

    SI446X_SET_PROPERTY_1(MODEM_BCR_OSR_1, 0x00, cs);           //lq ��SI446X_FREQ_XO�й� //λʱ��ͬ����������
    SI446X_SET_PROPERTY_1(MODEM_BCR_OSR_0, 0x55, cs);           
    SI446X_SET_PROPERTY_1(MODEM_BCR_NCO_OFFSET_2, 0x06, cs);    //lq ��SI446X_FREQ_XO�й�
    SI446X_SET_PROPERTY_1(MODEM_BCR_NCO_OFFSET_1, 0x0C, cs);
    SI446X_SET_PROPERTY_1(MODEM_BCR_NCO_OFFSET_0, 0xAB, cs);
    SI446X_SET_PROPERTY_1(MODEM_BCR_GAIN_1, 0x03, cs);          //lq ��SI446X_FREQ_XO�й�
    SI446X_SET_PROPERTY_1(MODEM_BCR_GAIN_0, 0x03, cs);
    SI446X_SET_PROPERTY_1(MODEM_BCR_GEAR, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_BCR_MISC0, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_BCR_MISC1, 0xC2, cs);

    SI446X_SET_PROPERTY_1(MODEM_AFC_GAIN_1, 0x00, cs);

#if (SI446X_IF_MODE == SI446X_IF_MODE_FIXED)
    SI446X_SET_PROPERTY_1(MODEM_AGC_CONTROL, 0xE0, cs);
    SI446X_SET_PROPERTY_1(MODEM_AGC_WINDOW_SIZE, 0x11, cs);
    SI446X_SET_PROPERTY_1(MODEM_AGC_RFPD_DECAY, 0x13, cs);      //lq ��SI446X_FREQ_XO�й�
    SI446X_SET_PROPERTY_1(MODEM_AGC_IFPD_DECAY, 0x13, cs);
#elif (SI446X_IF_MODE == SI446X_IF_MODE_SCALED)
    SI446X_SET_PROPERTY_1(MODEM_AGC_CONTROL, 0xE2, cs);
    SI446X_SET_PROPERTY_1(MODEM_AGC_WINDOW_SIZE, 0xFF, cs);
    SI446X_SET_PROPERTY_1(MODEM_AGC_RFPD_DECAY, 0xFF, cs);      //lq ��SI446X_FREQ_XO�й�
    SI446X_SET_PROPERTY_1(MODEM_AGC_IFPD_DECAY, 0xFF, cs);
#else
    SI446X_SET_PROPERTY_1(MODEM_AGC_CONTROL, 0xE2, cs);
    SI446X_SET_PROPERTY_1(MODEM_AGC_WINDOW_SIZE, 0x11, cs);
    SI446X_SET_PROPERTY_1(MODEM_AGC_RFPD_DECAY, 0x19, cs);      //lq ��SI446X_FREQ_XO�й�
    SI446X_SET_PROPERTY_1(MODEM_AGC_IFPD_DECAY, 0x19, cs);
#endif

    SI446X_SET_PROPERTY_1(MODEM_OOK_MISC, 0x23, cs);

    SI446X_SET_PROPERTY_1(MODEM_RAW_SEARCH, 0x96, cs);
    SI446X_SET_PROPERTY_1(MODEM_RAW_CONTROL, 0x8F, cs);
    SI446X_SET_PROPERTY_1(MODEM_RAW_EYE_1, 0x00, cs);           //lq ��SI446X_FREQ_XO�й�
    SI446X_SET_PROPERTY_1(MODEM_RAW_EYE_0, 0x7B, cs);

    SI446X_SET_PROPERTY_1(MODEM_ANT_DIV_MODE, 0x02, cs);
    SI446X_SET_PROPERTY_1(MODEM_ANT_DIV_CONTROL, 0x00, cs);

    SI446X_SET_PROPERTY_1(MODEM_RSSI_CONTROL, 0x09, cs);
    SI446X_SET_PROPERTY_1(MODEM_RSSI_CONTROL2, 0x00, cs);
    SI446X_SET_PROPERTY_1(MODEM_RSSI_MUTE, 0x00, cs);

#if (SI446X_FREQ_BAND == SI446X_FREQ_BAND_142_175)            
    SI446X_SET_PROPERTY_1(MODEM_CLKGEN_BAND, 0x0D, cs);
#elif (SI446X_FREQ_BAND == SI446X_FREQ_BAND_420_525)            
    SI446X_SET_PROPERTY_1(MODEM_CLKGEN_BAND, 0x0A, cs);
#endif

    /*lq MODEM_CHFLT configuration */
    /*lq WB=NB=20.07kHz��RX bandwidth = 20kHz�����ٵ�ѡ����ʵ����� */
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE13_7_0, 0xCC, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE12_7_0, 0xA1, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE11_7_0, 0x30, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE10_7_0, 0xA0, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE9_7_0, 0x21, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE8_7_0, 0xD1, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE7_7_0, 0xB9, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE6_7_0, 0xC9, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE5_7_0, 0xEA, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE4_7_0, 0x05, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE3_7_0, 0x12, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE2_7_0, 0x11, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE1_7_0, 0x0A, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COE0_7_0, 0x04, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COEM0, 0x15, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COEM1, 0xFC, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COEM2, 0x03, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX1_CHFLT_COEM3, 0x00, cs);
    
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE13_7_0, 0xCC, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE12_7_0, 0xA1, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE11_7_0, 0x30, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE10_7_0, 0xA0, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE9_7_0, 0x21, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE8_7_0, 0xD1, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE7_7_0, 0xB9, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE6_7_0, 0xC9, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE5_7_0, 0xEA, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE4_7_0, 0x05, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE3_7_0, 0x12, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE2_7_0, 0x11, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE1_7_0, 0x0A, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COE0_7_0, 0x04, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COEM0, 0x15, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COEM1, 0xFC, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COEM2, 0x03, cs);
    SI446X_SET_PROPERTY_1(MODEM_CHFLT_RX2_CHFLT_COEM3, 0x00, cs);

    SI446X_SET_PROPERTY_1(MODEM_SPIKE_DET, 0x03, cs);
    SI446X_SET_PROPERTY_1(MODEM_ONE_SHOT_AFC, 0x07, cs);

    SI446X_SET_PROPERTY_1(MODEM_DSA_CTRL1, 0x40, cs);
    SI446X_SET_PROPERTY_1(MODEM_DSA_CTRL2, 0x04, cs);
    SI446X_SET_PROPERTY_1(MODEM_DSA_QUAL, 0x05, cs);
    SI446X_SET_PROPERTY_1(MODEM_DSA_RSSI, 0x78, cs);
    SI446X_SET_PROPERTY_1(MODEM_DSA_MISC, 0x20, cs);

    /*lq PA configuration */
    SI446X_SET_PROPERTY_1(PA_MODE, 0x08, cs);
    SI446X_SET_PROPERTY_1(PA_PWR_LVL, pSI446XStruct->SI446X_PaPwrLvl, cs);
    SI446X_SET_PROPERTY_1(PA_BIAS_CLKDUTY, 0x00, cs);
    SI446X_SET_PROPERTY_1(PA_TC, 0x1D, cs);

    /*lq Synth configuration */
    SI446X_SET_PROPERTY_1(SYNTH_PFDCP_CPFF, 0x2C, cs);
    SI446X_SET_PROPERTY_1(SYNTH_PFDCP_CPINT, 0x0E, cs);
    SI446X_SET_PROPERTY_1(SYNTH_VCO_KV, 0x0B, cs);
    SI446X_SET_PROPERTY_1(SYNTH_LPFILT3, 0x04, cs);
    SI446X_SET_PROPERTY_1(SYNTH_LPFILT2, 0x0C, cs);
    SI446X_SET_PROPERTY_1(SYNTH_LPFILT1, 0x73, cs);
    SI446X_SET_PROPERTY_1(SYNTH_LPFILT0, 0x03, cs);

    /*lq Match configuration */
    SI446X_SET_PROPERTY_1(MATCH_CTRL_1, 0x00, cs);  //lq Disable match function

    /*lq Freq configuration */
    SI446X_SET_PROPERTY_1(FREQ_CONTROL_INTE, pSI446XStruct->SI446X_N_Integer_Base, cs);
    SI446X_SET_PROPERTY_1(FREQ_CONTROL_FRAC_2,
                          pSI446XStruct->SI446X_N_Fractional_Base >> 16,
                          cs);
    SI446X_SET_PROPERTY_1(FREQ_CONTROL_FRAC_1,
                          pSI446XStruct->SI446X_N_Fractional_Base >> 8,
                          cs);
    SI446X_SET_PROPERTY_1(FREQ_CONTROL_FRAC_0,
                          pSI446XStruct->SI446X_N_Fractional_Base,
                          cs);
    SI446X_SET_PROPERTY_1(FREQ_CONTROL_CHANNEL_STEP_SIZE_1,
                          pSI446XStruct->SI446X_Channel_Step >> 8,
                          cs);
    SI446X_SET_PROPERTY_1(FREQ_CONTROL_CHANNEL_STEP_SIZE_0,
                          pSI446XStruct->SI446X_Channel_Step,
                          cs);

    /*lq Rx hop configuration */
    SI446X_SET_PROPERTY_1(RX_HOP_CONTROL, 0x00, cs);//lq Disable RX hop function

#else
    while((i = config_table[j]) != 0)
    {
        j += 1;
        SI446X_Cmd(config_table + j, i, cs);
        j += i;
    }

#if 0
#if PACKET_LENGTH > 0           //fixed packet length
    SI446X_SET_PROPERTY_1(PKT_FIELD_1_LENGTH_7_0, PACKET_LENGTH, cs);
#else                           //variable packet length
    SI446X_SET_PROPERTY_1(PKT_CONFIG1, 0x00, cs);
    SI446X_SET_PROPERTY_1(PKT_CRC_CONFIG, 0x00, cs);
    SI446X_SET_PROPERTY_1(PKT_LEN_FIELD_SOURCE, 0x01, cs);
    SI446X_SET_PROPERTY_1(PKT_LEN, 0x2A, cs);
    SI446X_SET_PROPERTY_1(PKT_LEN_ADJUST, 0x00, cs);
    SI446X_SET_PROPERTY_1(PKT_FIELD_1_LENGTH_12_8, 0x00, cs);
    SI446X_SET_PROPERTY_1(PKT_FIELD_1_LENGTH_7_0, 0x01, cs);
    SI446X_SET_PROPERTY_1(PKT_FIELD_1_CONFIG, 0x00, cs);
    SI446X_SET_PROPERTY_1(PKT_FIELD_1_CRC_CONFIG, 0x00, cs);
    SI446X_SET_PROPERTY_1(PKT_FIELD_2_LENGTH_12_8, 0x00, cs);
    SI446X_SET_PROPERTY_1(PKT_FIELD_2_LENGTH_7_0, 0x10, cs);
    SI446X_SET_PROPERTY_1(PKT_FIELD_2_CONFIG, 0x00, cs);
    SI446X_SET_PROPERTY_1(PKT_FIELD_2_CRC_CONFIG, 0x00, cs);
#endif //PACKET_LENGTH
#endif

    //��Ҫ�� 4463��GDO2��GDO3������Ƶ���أ�  33 ,32 
    //����ʱ���룺 GDO2=0��GDO3=1
    //����ʱ���룺 GDO2=1��GDO3=0
    //���ԣ�����������4463�Ǳ���ģ�4460��4461����Ҫ��Ƶ���أ������ȥ�������
    SI446X_GPIO_PIN_CFG(0, 0, 33|0x40, 32|0x40, 0, 0, 0, cs);    //��Ҫ
    //SI446X_GPIO_PIN_CFG(0, 0, 0x43, 0x42, 0, 0, 0);
#endif

    SI446X_PART_INFO(buffer, cs);
    debug_printf("$Si4463 CHIPREV : %02X!\r\n", buffer[1]);
    debug_printf("$Si4463 PART    : %02X%02X!\r\n", buffer[2], buffer[3]);
    debug_printf("$Si4463 PBUILD  : %02X!\r\n", buffer[4]);
    debug_printf("$Si4463 ID      : %02X%02X!\r\n", buffer[5], buffer[6]);
    debug_printf("$Si4463 CUSTOMER: %02X!\r\n", buffer[7]);
    debug_printf("$Si4463 ROMID   : %02X!\r\n", buffer[8]);

    SI446X_FUNC_INFO(buffer, cs);
    debug_printf("$Si4463 REVEXT   : %02X!\r\n", buffer[1]);
    debug_printf("$Si4463 REVBRANCH: %02X!\r\n", buffer[2]);
    debug_printf("$Si4463 REVINT   : %02X!\r\n", buffer[3]);
    debug_printf("$Si4463 PATCH    : %02X%02X!\r\n", buffer[4], buffer[5]);
    debug_printf("$Si4463 FUNC     : %02X!\r\n", buffer[6]);
}
/*
=================================================================================
SI446X_WRITE_TX_FIFO();
Function : write data to TX fifo
INTPUT   : txbuffer, a buffer stores TX array
           size,  how many bytes should be written
OUTPUT   : NONE
=================================================================================
*/
void SI446X_WRITE_TX_FIFO(U8 *txbuffer, U8 size, SI446XChipEnum cs)
{
    if (cs == SI446X_CHIP_A)
    {
        SI446X_A_CS_ON();
    }
    else
    {
        SI446X_B_CS_ON();
    }

    SI446X_SPI_ExchangeByte(WRITE_TX_FIFO, cs);
    while(size --)
    {
        SI446X_SPI_ExchangeByte(*txbuffer++, cs);
    }
    
    if (cs == SI446X_CHIP_A)
    {
        SI446X_A_CS_OFF();
    }
    else
    {
        SI446X_B_CS_OFF();
    }
}
/*
=================================================================================
SI446X_SendPacket();
Function : send a packet
INTPUT   : txbuffer, a buffer stores TX array
           size,  how many bytes should be written
           channel, tx channel
           condition, tx condition
OUTPUT   : NONE
=================================================================================
*/
void SI446X_SendPacket(U8 *txbuffer, U8 size, U8 channel, U8 condition,
                       SI446XChipEnum cs)
{
    U8 cmd[5];
    U8 tx_len = size;

    SI446X_TX_FIFO_RESET(cs);

    if (cs == SI446X_CHIP_A)
    {
        SI446X_A_CS_ON();
    }
    else
    {
        SI446X_B_CS_ON();
    }

    SI446X_SPI_ExchangeByte(WRITE_TX_FIFO, cs);
#if PACKET_LENGTH == 0
    tx_len ++;
    SI446X_SPI_ExchangeByte(size, cs);
#endif
    while(size--)
    {
        SI446X_SPI_ExchangeByte(*txbuffer++, cs);
    }

    if (cs == SI446X_CHIP_A)
    {
        SI446X_A_CS_OFF();
    }
    else
    {
        SI446X_B_CS_OFF();
    }

    cmd[0] = START_TX;
    cmd[1] = channel;
    cmd[2] = condition;
    cmd[3] = 0;
    cmd[4] = tx_len;
    SI446X_Cmd(cmd, 5, cs);
}
/*
=================================================================================
SI446X_START_TX();
Function : start TX command
INTPUT   : channel, tx channel
           condition, tx condition
           tx_len, how many bytes to be sent
OUTPUT   : NONE
=================================================================================
*/
void SI446X_START_TX(U8 channel, U8 condition, U16 tx_len, SI446XChipEnum cs)
{
    U8 cmd[5];

    cmd[0] = START_TX;
    cmd[1] = channel;
    cmd[2] = condition;
    cmd[3] = tx_len>>8;
    cmd[4] = tx_len;

    SI446X_Cmd(cmd, 5, cs);
}
/*
=================================================================================
SI446X_ReadPacket();
Function : read RX fifo
INTPUT   : buffer, a buffer to store data read
OUTPUT   : received bytes
=================================================================================
*/
U8 SI446X_ReadPacket(U8 *buffer, SI446XChipEnum cs)
{
    U8 length, i;
    
    SI446X_WaitCts(cs);
    
    if (cs == SI446X_CHIP_A)
    {
        SI446X_A_CS_ON();
    }
    else
    {
        SI446X_B_CS_ON();
    }

    SI446X_SPI_ExchangeByte(READ_RX_FIFO, cs);
#if PACKET_LENGTH == 0
    length = SI446X_SPI_ExchangeByte(0xFF, cs);
#else
    length = PACKET_LENGTH;
#endif
    i = length;
    while(length--)
    {
        *buffer++ = SI446X_SPI_ExchangeByte(0xFF, cs);
    }

    if (cs == SI446X_CHIP_A)
    {
        SI446X_A_CS_OFF();
    }
    else
    {
        SI446X_B_CS_OFF();
    }
    return i;
}
/*
=================================================================================
SI446X_START_RX();
Function : start RX state
INTPUT   : channel, receive channel
           condition, receive condition
           rx_len, how many bytes should be read
           n_state1, next state 1
           n_state2, next state 2
           n_state3, next state 3
OUTPUT   : NONE
=================================================================================
*/
void SI446X_START_RX(U8 channel, U8 condition, U16 rx_len, U8 n_state1,
                     U8 n_state2, U8 n_state3, SI446XChipEnum cs)
{
    U8 cmd[8];

    SI446X_RX_FIFO_RESET(cs);
    SI446X_TX_FIFO_RESET(cs);
    
    cmd[0] = START_RX;
    cmd[1] = channel;
    cmd[2] = condition;
    cmd[3] = rx_len>>8;
    cmd[4] = rx_len;
    cmd[5] = n_state1;
    cmd[6] = n_state2;
    cmd[7] = n_state3;
    
    SI446X_Cmd(cmd, 8, cs);
}
/*
=================================================================================
SI446X_RX_FIFO_RESET();
Function : reset the RX FIFO of the device
INTPUT   : None
OUTPUT   : NONE
=================================================================================
*/
void SI446X_RX_FIFO_RESET(SI446XChipEnum cs)
{
    U8 cmd[2];

    cmd[0] = FIFO_INFO;
    cmd[1] = 0x02;

    SI446X_Cmd(cmd, 2, cs);
}
/*
=================================================================================
SI446X_TX_FIFO_RESET();
Function : reset the TX FIFO of the device
INTPUT   : None
OUTPUT   : NONE
=================================================================================
*/
void SI446X_TX_FIFO_RESET(SI446XChipEnum cs)
{
    U8 cmd[2];

    cmd[0] = FIFO_INFO;
    cmd[1] = 0x01;

    SI446X_Cmd(cmd, 2, cs);
}
/*
=================================================================================
SI446X_PACKET_INFO();
Function : read packet information
INTPUT   : buffer, stores the read information
           FIELD, feild mask
           length, the packet length
           diff_len, diffrence packet length
OUTPUT   : NONE
=================================================================================
*/
void SI446X_PACKET_INFO(U8 *buffer, U8 FIELD, U16 length, U16 diff_len,
                        SI446XChipEnum cs)
{
    U8 cmd[6];

    cmd[0] = PACKET_INFO;
    cmd[1] = FIELD;
    cmd[2] = length >> 8;
    cmd[3] = length;
    cmd[4] = diff_len >> 8;
    cmd[5] = diff_len;

    SI446X_Cmd(cmd, 6, cs);
    SI446X_ReadResponse(buffer, 3, cs);
}
/*
=================================================================================
SI446X_FIFO_INFO();
Function : read fifo information
INTPUT   : buffer, stores the read information
OUTPUT   : NONE
=================================================================================
*/
void SI446X_FIFO_INFO(U8 *buffer, SI446XChipEnum cs)
{
    U8 cmd[2];

    cmd[0] = FIFO_INFO;
    cmd[1] = 0x03;

    SI446X_Cmd(cmd, 2, cs);
    SI446X_ReadResponse(buffer, 3, cs);
}
/*
=================================================================================
SI446X_GPIO_PIN_CFG();
Function : config the GPIOs, IRQ, sdo
INTPUT   :
OUTPUT   : NONE
=================================================================================
*/
void SI446X_GPIO_PIN_CFG(U8 G0, U8 G1, U8 G2, U8 G3, U8 IRQ, U8 sdo,
                         U8 GEN_CONFIG, SI446XChipEnum cs)
{
    U8 cmd[10];

    cmd[0] = GPIO_PIN_CFG;
    cmd[1] = G0;
    cmd[2] = G1;
    cmd[3] = G2;
    cmd[4] = G3;
    cmd[5] = IRQ;
    cmd[6] = sdo;
    cmd[7] = GEN_CONFIG;

    SI446X_Cmd(cmd, 8, cs);
    SI446X_ReadResponse(cmd, 8, cs);
}

/*******************************************************************************
* Function Name  : SI446X_SelfCheck
* Description    : SI446X�Լ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SI446X_SelfCheck(Si446xSelfCheckStruct *pSi446xSelfCheckStruct,
                      SI446XChipEnum cs)
{
    U8  buffer[10];
  	U16 regVal;
    U16 targetVal = 0x4460; //lq ����4463/4468

    if (pSi446xSelfCheckStruct->StartFlg == TRUE)
    {
        /*lq SDNʹ�� */
        if (cs == SI446X_CHIP_A)
        {
            SI446X_A_CE_ON();
        }
        else
        {
            SI446X_B_CE_ON();
        }
        Delay_ms(50);
    
        /*lq CSʹ�� */
        SI446X_PART_INFO_SelfCheck(buffer, TRUE, cs);
        regVal = ((U16)buffer[2] << 8) | ((U16)buffer[3]);
    
        if ((regVal & 0xFFF0) == targetVal)
        {
            pSi446xSelfCheckStruct->DataCommOkFlg = TRUE;

            /*lq оƬ�ͺ� */
            if (regVal == SI446X_TYPE_ID_4463)
            {
                pSi446xSelfCheckStruct->Si446xType = SI446X_TYPE_4463;
                pSi446xSelfCheckStruct->TypeOkFlg = TRUE;
            }
            else
            {
                pSi446xSelfCheckStruct->Si446xType = SI446X_TYPE_OTHER;
                pSi446xSelfCheckStruct->TypeOkFlg = FALSE;
            }
    
            /*lq CSʧ�� */
            memset(buffer, 0, sizeof(buffer));
            SI446X_PART_INFO_SelfCheck(buffer, FALSE, cs);
            regVal = ((U16)buffer[2] << 8) | ((U16)buffer[3]);
            if ((regVal & 0xFFF0) == targetVal)
            {
                pSi446xSelfCheckStruct->SenOkFlg = FALSE;           
            }
            else
            {
                pSi446xSelfCheckStruct->SenOkFlg = TRUE;
            }

            /*lq SDNʧ�� */
            if (cs == SI446X_CHIP_A)
            {
                SI446X_A_CE_OFF();
            }
            else
            {
                SI446X_B_CE_OFF();
            }
            Delay_ms(50);
            
            /*lq CSʹ�� */
            SI446X_PART_INFO_SelfCheck(buffer, TRUE, cs);
            regVal = ((U16)buffer[2] << 8) | ((U16)buffer[3]);
            if ((regVal & 0xFFF0) == targetVal)
            {
                pSi446xSelfCheckStruct->PdnOkFlg = FALSE;           
            }
            else
            {
                pSi446xSelfCheckStruct->PdnOkFlg = TRUE;
            }

        }
        else
        {
            /*lq �޷���һ��ȷ�������׼ȷԭ�� */
            pSi446xSelfCheckStruct->PdnOkFlg = FALSE;
            pSi446xSelfCheckStruct->SenOkFlg = FALSE;
            pSi446xSelfCheckStruct->DataCommOkFlg = FALSE;
            pSi446xSelfCheckStruct->TypeOkFlg = FALSE;
        }
               
        pSi446xSelfCheckStruct->CompleteFlg = TRUE;               
        pSi446xSelfCheckStruct->StartFlg = FALSE;
    }        
}

/*******************************************************************************
* ����  : SI446X_Si446xSelfCheckStructInit
* ����  : ��ʼ���ṹ��
* ����  : pEepromSelfCheckStruct - EepromSelfCheckStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SI446X_Si446xSelfCheckStructInit(Si446xSelfCheckStruct *pSi446xSelfCheckStruct)
{
    pSi446xSelfCheckStruct->PdnOkFlg = FALSE;
    pSi446xSelfCheckStruct->SenOkFlg = FALSE;
    pSi446xSelfCheckStruct->DataCommOkFlg = FALSE;
    pSi446xSelfCheckStruct->TypeOkFlg = FALSE;
    pSi446xSelfCheckStruct->Si446xType = SI446X_TYPE_OTHER;
    pSi446xSelfCheckStruct->StartFlg = TRUE;       
    pSi446xSelfCheckStruct->CompleteFlg = FALSE;       
}

/*******************************************************************************
* ����  : SI446X_SETFREBASE
* ����  : ���û�׼Ƶ��
* ����  : ulFreBase ��׼Ƶ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void SI446X_SETFREBASE(U32 ulFreBase)
{
	SI446X_FRE_BASE = ulFreBase;
}


#if  CONFIG_PTT_EN      // lnw  add

/*******************************************************************************
* ����  : SI446X_RXSTTUS_Return
* ����  :  lnw   Use  for  PTT Module
* ����  : ulFreBase ��׼Ƶ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/

U8 SI446X_RXSTTUS_Return(void)
{
   if((SI446XA.SI446XStatus == SI446X_MODE_RX)&&(SI446XB.SI446XStatus == SI446X_MODE_RX))
   	 return 1;
   else
   	 return 0;

}
#endif
/*
=================================================================================
------------------------------------End of FILE----------------------------------
=================================================================================
*/
