/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: DataLinkService.c
  Author: lq       Version : V1.00        Date: 2014.07.09
  Description:     AIS������·�����
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "Include.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static PacketProbeStruct PacketProbeInst1;
static PacketProbeStruct PacketProbeInst2;

static FrameRxFifoStruct FrameFifoRx1;
static FrameRxFifoStruct FrameFifoRx2;
static FrameTxFifoStruct FrameFifoTx;
 
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* ����  : DLS_BitStreamStructInit
* ����  : ��ʼ���ṹ��
* ����  : pBitStreamStruct - BitStreamStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void DLS_BitStreamStructInit(BitStreamStruct *pBitStreamStruct)
{
    pBitStreamStruct->Input = 0;
    pBitStreamStruct->Output = 0;
    pBitStreamStruct->HeadIndex = 0;
    pBitStreamStruct->TailIndex = 0;
    pBitStreamStruct->InputOffset = 0;
    pBitStreamStruct->OutputOffset = 0;
    pBitStreamStruct->HeadOffset = 0;
    pBitStreamStruct->TailOffset = 0;           
}

/*******************************************************************************
* ����  : DLS_PacketProbeStructInit
* ����  : ��ʼ���ṹ��
* ����  : pPacketProbeStruct - PacketProbeStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void DLS_PacketProbeStructInit(PacketProbeStruct *pPacketProbeStruct)
{
    DLS_BitStreamStructInit(&pPacketProbeStruct->BitStream);
    pPacketProbeStruct->State = DLS_PACKET_PROBE_STATE_HEAD_PROBING;   
}

/*******************************************************************************
* ����  : DLS_FrameRxFifoStructInit
* ����  : ��ʼ���ṹ��
* ����  : pFrameRxFifoStruct - FrameRxFifoStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void DLS_FrameRxFifoStructInit(FrameRxFifoStruct *pFrameRxFifoStruct)
{
    pFrameRxFifoStruct->Input = 0;
    pFrameRxFifoStruct->Output = 0; 
}

/*******************************************************************************
* ����  : DLS_FrameTxFifoStructInit
* ����  : ��ʼ���ṹ��
* ����  : pFrameTxFifoStruct - FrameTxFifoStruct�ṹ��ָ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void DLS_FrameTxFifoStructInit(FrameTxFifoStruct *pFrameTxFifoStruct)
{
    pFrameTxFifoStruct->Input = 0;
    pFrameTxFifoStruct->Output = 0; 
}

/*******************************************************************************
* ����  : DLS_InstFrameFifoStructInit
* ����  : ��ʼ���ṹ��
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void DLS_InstFrameFifoStructInit(void)
{
    DLS_FrameRxFifoStructInit(&FrameFifoRx1);
    DLS_FrameRxFifoStructInit(&FrameFifoRx2);
    DLS_FrameTxFifoStructInit(&FrameFifoTx);
}

/*******************************************************************************
* ����  : DLS_InstPacketProbeStructInit
* ����  : ��ʼ���ṹ��ʵ��
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void DLS_InstPacketProbeStructInit(void)
{
    DLS_PacketProbeStructInit(&PacketProbeInst1);
    DLS_PacketProbeStructInit(&PacketProbeInst2);
}

/*******************************************************************************
* ����  : DLS_BitStreamIsBuffFull
* ����  : �����������Ƿ�����
* ����  : pBitStreamStruct - BitStreamStruct�ṹ��ָ��		  
* ���  : ��
* ����  : TRUE - ������FALSE - δ��
*******************************************************************************/
static U8 DLS_BitStreamIsBuffFull(BitStreamStruct *pBitStreamStruct)
{
    if ((pBitStreamStruct->Input + 1) % DLS_BITSTREAM_BUFF_LEN
        == pBitStreamStruct->Output)
    {
        return TRUE; 

        debug_printf("$[%6d:%4d] %s : Bitstream buffer [%s]is full!\r\n",
            GetFrameNum(), GetSlotNum(), __func__,
            pBitStreamStruct == &PacketProbeInst1.BitStream ? "A" : "B");
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* ����  : DLS_BitStreamIsBuffEmpty
* ����  : �����������Ƿ�Ϊ��
* ����  : pBitStreamStruct - BitStreamStruct�ṹ��ָ��		  
* ���  : ��
* ����  : TRUE - Ϊ�գ�FALSE - �ǿ�
*******************************************************************************/
static U8 DLS_BitStreamIsBuffEmpty(BitStreamStruct *pBitStreamStruct)
{
    if ((pBitStreamStruct->Input == pBitStreamStruct->Output)
        && (pBitStreamStruct->InputOffset == pBitStreamStruct->OutputOffset))
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* ����  : DLS_BitStreamPutBit
* ����  : �򻺴��������
* ����  : pBitStreamStruct - BitStreamStruct�ṹ��ָ��
          data - ����	  
* ���  : ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
static U8 DLS_BitStreamPutBit(BitStreamStruct *pBitStreamStruct, const U8 data)
{
    U8 tempU8;
    U8 tempU8_2;

    if (DLS_BitStreamIsBuffFull(pBitStreamStruct) == FALSE)
    {
        /*lq ����λ���� */
        tempU8_2 = ~(1 << pBitStreamStruct->InputOffset);
        tempU8 = (data & 0x01) << pBitStreamStruct->InputOffset;

        /*lq ָ��λ��ֵ */
        pBitStreamStruct->DataBuff[pBitStreamStruct->Input] = 
            (pBitStreamStruct->DataBuff[pBitStreamStruct->Input] & tempU8_2)
            | tempU8;

        pBitStreamStruct->InputOffset++;
        if (pBitStreamStruct->InputOffset >= DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)
        {
            pBitStreamStruct->InputOffset = 0;
            pBitStreamStruct->Input = (pBitStreamStruct->Input + 1)
                                       % DLS_BITSTREAM_BUFF_LEN;            
        }
    
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : DLS_InstBitStreamPutBit
* ����  : �򻺴��������
* ����  : type - ʵ������
          data - ����	  
* ���  : ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
U8 DLS_InstBitStreamPutBit(const PacketProbeTypeEnum type, const U8 data)
{
    if (type == PacketProbeType1)
    {
        return DLS_BitStreamPutBit(&PacketProbeInst1.BitStream, data);    
    }
    else if (type == PacketProbeType2)
    {
        return DLS_BitStreamPutBit(&PacketProbeInst2.BitStream, data);        
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : DLS_BitStreamGetBit
* ����  : �ӻ����ȡ����
* ����  : pBitStreamStruct - BitStreamStruct�ṹ��ָ��
* ���  : pData - ��ȡ������
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
static U8 DLS_BitStreamGetBit(BitStreamStruct *pBitStreamStruct, U8 *pData)
{
    U8 tempU8;

    if (DLS_BitStreamIsBuffEmpty(pBitStreamStruct) == FALSE)
    {
        tempU8 = pBitStreamStruct->DataBuff[pBitStreamStruct->Output];
        tempU8 &= (1 << pBitStreamStruct->OutputOffset);

        pBitStreamStruct->OutputOffset++;
        if (pBitStreamStruct->OutputOffset >= DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)
        {
            pBitStreamStruct->OutputOffset = 0;
            pBitStreamStruct->Output = (pBitStreamStruct->Output + 1)
                                        % DLS_BITSTREAM_BUFF_LEN;            
        }
    
        if (tempU8 == 0)
        {
            *pData = 0;
        }
        else
        {
            *pData = 1;
        }

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : DLS_InstBitStreamGetBit
* ����  : �ӻ����ȡ����
* ����  : type - ʵ������
* ���  : pData - ��ȡ������
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
U8 DLS_InstBitStreamGetBit(const PacketProbeTypeEnum type, U8 *pData)
{
    if (type == PacketProbeType1)
    {
        return DLS_BitStreamGetBit(&PacketProbeInst1.BitStream, pData);    
    }
    else if (type == PacketProbeType2)
    {
        return DLS_BitStreamGetBit(&PacketProbeInst2.BitStream, pData);        
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : DLS_BitStreamFlushBuff
* ����  :�ﵽ��󳤶ȣ��������7��������
* ����  : pBitStreamStruct - BitStreamStruct�ṹ��ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void DLS_BitStreamFlushBuff(BitStreamStruct *pBitStreamStruct)
{
	/*pBitStreamStruct->Output = pBitStreamStruct->Input;
    pBitStreamStruct->HeadIndex = pBitStreamStruct->Input;
    pBitStreamStruct->TailIndex = pBitStreamStruct->Input;
    pBitStreamStruct->OutputOffset = pBitStreamStruct->InputOffset;
    pBitStreamStruct->HeadOffset = pBitStreamStruct->InputOffset;
    pBitStreamStruct->TailOffset = pBitStreamStruct->InputOffset;*/
	if((pBitStreamStruct->InputOffset-6)<0)
	{
		pBitStreamStruct->OutputOffset = pBitStreamStruct->InputOffset-6+DLS_FRAME_HEADER_LEN;
		if((pBitStreamStruct->Input-1)<0)
		{
			pBitStreamStruct->Output = pBitStreamStruct->Input-1+DLS_BITSTREAM_BUFF_LEN;
		}
		else
		{
			pBitStreamStruct->Output = pBitStreamStruct->Input-1;
		}
	}
	else
	{
		pBitStreamStruct->OutputOffset = pBitStreamStruct->InputOffset-6;
		pBitStreamStruct->Output = pBitStreamStruct->Input;
	}
    pBitStreamStruct->HeadIndex = pBitStreamStruct->Output;
    pBitStreamStruct->TailIndex = pBitStreamStruct->Output;
    pBitStreamStruct->HeadOffset = pBitStreamStruct->OutputOffset;
    pBitStreamStruct->TailOffset = pBitStreamStruct->OutputOffset;
}

/*******************************************************************************
* ����  : DLS_BitStreamNextFrameSetup
* ����  : ��һ֡������ã���Ϊ֡ͷ��֡β��ͬ�����Խ���ǰ��֡����β��Ϊ��һ֡��ͷ��
           ��ֹ��ǰ��֡������������һ֡ʱ��©����һ֡����ǰ��֡���Ƿ�Ϊ������һ֡��
           ���ɺ�����У�����⣩
* ����  : pBitStreamStruct - BitStreamStruct�ṹ��ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void DLS_BitStreamNextFrameSetup(BitStreamStruct *pBitStreamStruct)
{
    pBitStreamStruct->Output = pBitStreamStruct->TailIndex;
    pBitStreamStruct->OutputOffset = pBitStreamStruct->TailOffset;
    pBitStreamStruct->HeadIndex = pBitStreamStruct->TailIndex;
    pBitStreamStruct->HeadOffset = pBitStreamStruct->TailOffset;
}

/*******************************************************************************
* ����  : DLS_BitStreamGetBitQuantity
* ����  : ��ȡ����λ������֮��ı�����
* ����  : pBitStreamStruct - BitStreamStruct�ṹ��ָ��
* ���  : index1 - ��ʼ��������Ԫ������ֵ
          offset1 - ��ʼ����λƫ����
          index2 - ��ֹ��������Ԫ������ֵ
          offset2 - ��ֹ����λƫ����
* ����  : ����λ������֮��ı�����
*******************************************************************************/
static U16 DLS_BitStreamGetBitQuantity(U16 index1, U16 offset1, U16 index2,
                                       U16 offset2)
{   
    S16 tempS16;

    tempS16 = ((index2 + DLS_BITSTREAM_BUFF_LEN - index1) % DLS_BITSTREAM_BUFF_LEN) //lq ����ֵ��Ϊ�޷��������ȼӺ��
        * DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN + offset2 - offset1;

    if (tempS16 < 0)
    {
        tempS16 = 0;
    }
    
    return tempS16;
}

/*******************************************************************************
* ����  : DLS_BitStreamGetBitNum
* ����  : ��ȡ���������
* ����  : pBitStreamStruct - BitStreamStruct�ṹ��ָ��
* ���  : ��
* ����  : ���������
*******************************************************************************/
static U16 DLS_BitStreamGetBitNum(BitStreamStruct *pBitStreamStruct)
{
    return DLS_BitStreamGetBitQuantity(pBitStreamStruct->Output,
        pBitStreamStruct->OutputOffset, pBitStreamStruct->Input,
        pBitStreamStruct->InputOffset);
}

/*******************************************************************************
* ����  : DLS_BitStreamGetFrameBitNum
* ����  : ��ȡ�����е�ǰ֡�ı�����
* ����  : pBitStreamStruct - BitStreamStruct�ṹ��ָ��
* ���  : ��
* ����  : �����е�ǰ֡�ı�����
*******************************************************************************/
static U16 DLS_BitStreamGetFrameBitNum(BitStreamStruct *pBitStreamStruct)
{
    U16 tempU16;

    tempU16 = DLS_BitStreamGetBitQuantity(pBitStreamStruct->HeadIndex,
        pBitStreamStruct->HeadOffset, pBitStreamStruct->TailIndex,
        pBitStreamStruct->TailOffset);

    if (tempU16 >= DLS_FRAME_HEADER_LEN)
    {
        tempU16 -= DLS_FRAME_HEADER_LEN;
    }
    else
    {
        tempU16 = 0;
    }

    return tempU16;
}

/*******************************************************************************
* ����  : DLS_BitStreamGetFrame
* ����  : ��ȡ�����еĵ�ǰ֡�������б��صĻ���˳����֡�ķ���˳�򣬼�֡��ÿ���ֽ�
          ��LSB���յ�������1371��׼��3.3.7����
* ����  : pBitStreamStruct - BitStreamStruct�ṹ��ָ��
* ���  : pFrameBuff - ����֡�����޳����λ������֡ͷ��֡β��ֻ�������ݺ�У���룬
          pFrameBuff��ÿ���ֽڵ�LSB��MSB���յ���
* ����  : TRUE - ֡��Ч��FALSE - ֡��Ч
*******************************************************************************/
static U8 DLS_BitStreamGetFrame(BitStreamStruct *pBitStreamStruct, U8 *pFrameBuff)
{
    U16 tempU16;
    U16 outputIndex;
    U16 outputOffset;
    U16 frameBitNum;
    U16 i;
    U16 k;
    U8  highCnt;
    U8  byteData;
    U8  validBitCnt;
#if 0 //lqdb
    U8 temp[2 * DLS_FRAME_MAX_LEN_BYTE + 30];
    U8 temp2[3];
#endif
    k = 0;
    highCnt = 0;
    byteData = 0;
    validBitCnt = 0;
    frameBitNum = DLS_BitStreamGetFrameBitNum(pBitStreamStruct);
    
    tempU16 = pBitStreamStruct->HeadOffset + DLS_FRAME_HEADER_LEN;
    outputOffset = tempU16 % DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN;
    outputIndex = (pBitStreamStruct->HeadIndex + (tempU16 /
        DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)) % DLS_BITSTREAM_BUFF_LEN;

    for (i = 0; i < frameBitNum; i++, outputOffset++)
    {
        if (outputOffset >= DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)
        {
            outputOffset = 0;
            outputIndex = (outputIndex + 1) % DLS_BITSTREAM_BUFF_LEN;
        }
        if ((pBitStreamStruct->DataBuff[outputIndex] & (1 << outputOffset)) != 0)
        {
            /*lq ����1 */
            if(highCnt >= 5)
            {
                DLS_BitStreamNextFrameSetup(pBitStreamStruct);                
#if 0
                usart_printf("$PacketProbe: Frame error for continous 1!\r\n");                                                                      
#endif     
                return FALSE;                   //lq ��������£�֡�в����������6��"1"
            }
            else
            {
                byteData |= (1 << validBitCnt); //lq �˴����֡��bit��ʵ�ʷ���˳����߼�˳��ı任�����յ���bit��ΪLSB��
                validBitCnt++;
                highCnt++;                      //lq ����"1"����
            }       
        }
        else
        {
            /*lq ����0 */
            if(highCnt >= 5)
            {
                /*lq �޳����λ��������0֮ǰ��������5��"1"��ɾ���ñ���"0" */ 
            }
            else
            {
                validBitCnt++;               
            }

            highCnt = 0;
        }
        
        if (validBitCnt >= 8)
        {
            validBitCnt = 0;
            
            /*lq ���֡���ж� */
            if (k >= DLS_FRAME_MAX_LEN_BYTE)
            {
                DLS_BitStreamNextFrameSetup(pBitStreamStruct);
#if 0
                usart_printf("$PacketProbe: Frame error for too long!\r\n");                                                                      
#endif
                return FALSE;                   //lq �������֡��            
            }
            else
            {
                pFrameBuff[++k] = byteData;     //lq pFrameBuff[0] �����Ч�ֽ�����
                byteData = 0;                                     
            }
        }    
    }
#if 0 //lqdb
    sprintf(temp, "$PacketProbe FrameData: ");            
    for (i = 0; i < k; i++)
    {
        sprintf(temp2, "%02X", pFrameBuff[1 + i]);
        strcat(temp, temp2);
    }
    strcat(temp, "\r\n");
    usart_printf(temp);
#endif  
    /*lq �����ֽ��ж� */
    if (validBitCnt == 0)
    {
        if (k < 3)
        {
            DLS_BitStreamNextFrameSetup(pBitStreamStruct);
#if 0
            usart_printf("$PacketProbe: Frame error for too short!\r\n");                                                                  
#endif
            return FALSE;                       //lq ֡����Ч������Ϊ3��1byte of data and 2bytes of CRC��        
        }
        else
        {
            pFrameBuff[0] = k;                  //lq ��¼֡��
            DLS_BitStreamNextFrameSetup(pBitStreamStruct);
            return TRUE;        
        }
    }
    else
    {
        DLS_BitStreamNextFrameSetup(pBitStreamStruct);
#if 0
        usart_printf("$PacketProbe: Frame error for boundary!\r\n");                                                      
#endif     
        return FALSE;                           //lq ���������ֽ�
    }     
}

/*******************************************************************************
* ����  : DLS_BitStreamGetFrame2
* ����  : ��ȡ�����еĵ�ǰ֡�������б��صĻ���˳����֡�ķ���˳�򣬼�֡��ÿ���ֽ�
          ��LSB���յ�������1371��׼��3.3.7����
* ����  : pBitStreamStruct - BitStreamStruct�ṹ��ָ��
* ���  : pFrameBuff - ����֡�����޳����λ������֡ͷ��֡β��ֻ�������ݺ�У���룬
          pFrameBuff��ÿ���ֽڵ�MSB��LSB���յ���
* ����  : TRUE - ֡��Ч��FALSE - ֡��Ч
*******************************************************************************/
static U8 DLS_BitStreamGetFrame2(BitStreamStruct *pBitStreamStruct, U8 *pFrameBuff)
{
    U16 tempU16;
    U16 outputIndex;
    U16 outputOffset;
    U16 frameBitNum;
    U16 i;
    U16 k;
    U8  highCnt;
    U8  byteData;
    U8  validBitCnt;
#if 0 //lqdb
    U8 temp[2 * DLS_FRAME_MAX_LEN_BYTE + 30];
    U8 temp2[3];
#endif
    k = 0;
    highCnt = 0;
    byteData = 0;
    validBitCnt = 0;
    frameBitNum = DLS_BitStreamGetFrameBitNum(pBitStreamStruct);
        
    tempU16 = pBitStreamStruct->HeadOffset + DLS_FRAME_HEADER_LEN;
    outputOffset = tempU16 % DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN;
    outputIndex = (pBitStreamStruct->HeadIndex + (tempU16 /
        DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)) % DLS_BITSTREAM_BUFF_LEN;

    for (i = 0; i < frameBitNum; i++, outputOffset++)
    {
        if (outputOffset >= DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)
        {
            outputOffset = 0;
            outputIndex = (outputIndex + 1) % DLS_BITSTREAM_BUFF_LEN;
        }
        if ((pBitStreamStruct->DataBuff[outputIndex] & (1 << outputOffset)) != 0)
        {
            /*lq ����1 */
            if(highCnt >= 5)
            {
                DLS_BitStreamNextFrameSetup(pBitStreamStruct);                
#if 0
                usart_printf("$PacketProbe: Frame error for continous 1!\r\n");                                                                      
#endif     
                return FALSE;                   //lq ��������£�֡�в����������6��"1"
            }
            else
            {
                byteData <<= 1;
                byteData |= 0x01;               //lq ��¼'1'
                validBitCnt++;
                highCnt++;                      //lq ����"1"����
            }       
        }
        else
        {
            /*lq ����0 */
            if(highCnt >= 5)
            {
                /*lq �޳����λ��������0֮ǰ��������5��"1"��ɾ���ñ���"0" */ 
            }
            else
            {
                byteData <<= 1;                 //lq ��¼'0'
                validBitCnt++;               
            }

            highCnt = 0;
        }
        
        if (validBitCnt >= 8)
        {
            validBitCnt = 0;
            
            /*lq ���֡���ж� */
            if (k >= DLS_FRAME_MAX_LEN_BYTE)
            {
                DLS_BitStreamNextFrameSetup(pBitStreamStruct);
#if 0
                usart_printf("$PacketProbe: Frame error for too long!\r\n");                                                                      
#endif     
                return FALSE;                   //lq �������֡��            
            }
            else
            {
                pFrameBuff[++k] = byteData;     //lq pFrameBuff[0] �����Ч�ֽ�����
                byteData = 0;                                     
            }
        }    
    }
#if 0 //lqdb
    sprintf(temp, "$PacketProbe FrameData: ");            
    for (i = 0; i < k; i++)
    {
        sprintf(temp2, "%02X", pFrameBuff[1 + i]);
        strcat(temp, temp2);
    }
    strcat(temp, "\r\n");
    usart_printf(temp);
#endif  
    /*lq �����ֽ��ж� */
    if (validBitCnt == 0)
    {
        if (k < 3)
        {
            DLS_BitStreamNextFrameSetup(pBitStreamStruct);
#if 0
            usart_printf("$PacketProbe: Frame error for too short!\r\n");                                                                  
#endif     
            return FALSE;                       //lq ֡����Ч������Ϊ3��1byte of data and 2bytes of CRC��        
        }
        else
        {
            pFrameBuff[0] = k;                  //lq ��¼֡��
            DLS_BitStreamNextFrameSetup(pBitStreamStruct);
            return TRUE;        
        }
    }
    else
    {
        DLS_BitStreamNextFrameSetup(pBitStreamStruct);
#if 0
        usart_printf("$PacketProbe: Frame error for boundary!\r\n");                                                      
#endif     
        return FALSE;                           //lq ���������ֽ�
    }     
}

/*******************************************************************************
* ����  : DLS_BitStreamGetFrame3
* ����  : ��ȡ�����еĵ�ǰ֡�������б��صĻ���˳����֡�ķ���˳�򣬼�֡��ÿ���ֽ�
          ��LSB���յ�������1371��׼��3.3.7����
* ����  : pBitStreamStruct - BitStreamStruct�ṹ��ָ��
* ���  : pFrameBuff - ����֡��δ�޳����λ������֡ͷ��֡β��ֻ�������ݺ�У���룬
          pFrameBuff��ÿ���ֽڵ�MSB��LSB���յ���
* ����  : TRUE - ֡��Ч��FALSE - ֡��Ч
*******************************************************************************/
static U8 DLS_BitStreamGetFrame3(BitStreamStruct *pBitStreamStruct, U8 *pFrameBuff)
{
    U16 tempU16;
    U16 outputIndex;
    U16 outputOffset;
    U16 frameBitNum;
    U16 i;
    U16 k;
    U8  byteData;
    U8  validBitCnt;
#if 0 //lqdb
    U8 temp[2 * DLS_FRAME_MAX_LEN_BYTE + 30];
    U8 temp2[3];
#endif
    k = 0;
    byteData = 0;
    validBitCnt = 0;
    frameBitNum = DLS_BitStreamGetFrameBitNum(pBitStreamStruct);
    
    tempU16 = pBitStreamStruct->HeadOffset + DLS_FRAME_HEADER_LEN;
    outputOffset = tempU16 % DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN;
    outputIndex = (pBitStreamStruct->HeadIndex + (tempU16 /
        DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)) % DLS_BITSTREAM_BUFF_LEN;

    for (i = 0; i < frameBitNum; i++, outputOffset++)
    {
        if (outputOffset >= DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)
        {
            outputOffset = 0;
            outputIndex = (outputIndex + 1) % DLS_BITSTREAM_BUFF_LEN;
        }
        if ((pBitStreamStruct->DataBuff[outputIndex] & (1 << outputOffset)) != 0)
        {
            /*lq ����1 */
                byteData <<= 1;
                byteData |= 0x01;               //lq ��¼'1'
                validBitCnt++;
        }
        else
        {
            /*lq ����0 */
            byteData <<= 1;                 //lq ��¼'0'
            validBitCnt++; 
        }
        
        if (validBitCnt >= 8)
        {
            validBitCnt = 0;
            
            /*lq ���֡���ж� */
            if (k >= DLS_FRAME_MAX_LEN_BYTE)
            {
                DLS_BitStreamNextFrameSetup(pBitStreamStruct);
#if 0
                usart_printf("$PacketProbe: Frame error for too long!\r\n");                                                                      
#endif     
                return FALSE;                   //lq �������֡��            
            }
            else
            {
                pFrameBuff[++k] = byteData;     //lq pFrameBuff[0] �����Ч�ֽ�����
                byteData = 0;                                     
            }
        }    
    }
#if 0 //lqdb
    sprintf(temp, "$PacketProbe FrameData: ");            
    for (i = 0; i < k; i++)
    {
        sprintf(temp2, "%02X", pFrameBuff[1 + i]);
        strcat(temp, temp2);
    }
    strcat(temp, "\r\n");
    usart_printf(temp);
#endif  
    /*lq �����ֽ��ж� */
    if (validBitCnt == 0)
    {
        if (k < 3)
        {
            DLS_BitStreamNextFrameSetup(pBitStreamStruct);
#if 0
            usart_printf("$PacketProbe: Frame error for too short!\r\n");                                                                  
#endif     
            return FALSE;                       //lq ֡����Ч������Ϊ3��1byte of data and 2bytes of CRC��        
        }
        else
        {
            pFrameBuff[0] = k;                  //lq ��¼֡��
            DLS_BitStreamNextFrameSetup(pBitStreamStruct);
            return TRUE;        
        }
    }
    else
    {
        DLS_BitStreamNextFrameSetup(pBitStreamStruct);
#if 0
        usart_printf("$PacketProbe: Frame error for boundary!\r\n");                                                      
#endif     
        return FALSE;                           //lq ���������ֽ�
    }     
}

/*******************************************************************************
* ����  : DLS_BitStreamFindFrameHeader
* ����  : �ӻ������֡ͷ
* ����  : pBitStreamStruct - BitStreamStruct�ṹ��ָ��
* ���  : pData - ��ȡ������
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
static U8 DLS_BitStreamFindFrameHeader(BitStreamStruct *pBitStreamStruct)
{
    U32 i;
    U16 outputIndex;
    U16 outputOffset;
    U16 tempU16;
    U8  temp8;
    U8  header;

    /*lq ����bit���ﵽ֡ͷ���� */
    while (DLS_BitStreamGetBitNum(pBitStreamStruct) >= DLS_FRAME_HEADER_LEN)
    {
        header = 0;
        
        /*lq ��ȡ֡ͷ���ȵı����� */
        outputIndex = pBitStreamStruct->Output;
        outputOffset = pBitStreamStruct->OutputOffset;

        for (i = 0; i < DLS_FRAME_HEADER_LEN; i++, outputOffset++)
        {
            if (outputOffset >= DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)
            {
                outputOffset = 0;
                outputIndex = (outputIndex + 1) % DLS_BITSTREAM_BUFF_LEN; 
            }
            header <<= 1;
            if ((pBitStreamStruct->DataBuff[outputIndex] & ( 1 << outputOffset))
                != 0)
            {
                header |=  1;
            }
        }

        if (header == DLS_FRAME_HEADER)
        {
            pBitStreamStruct->HeadIndex = pBitStreamStruct->Output;
            pBitStreamStruct->HeadOffset = pBitStreamStruct->OutputOffset;
            
            tempU16 = pBitStreamStruct->HeadOffset + DLS_FRAME_HEADER_LEN;
            pBitStreamStruct->TailOffset = tempU16 % DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN;
            pBitStreamStruct->TailIndex = (pBitStreamStruct->Output
                + (tempU16 / DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN))
                % DLS_BITSTREAM_BUFF_LEN;       //lq ֡β������ʼλ��

            return TRUE;
        }
        else
        {
            /*lq ɾ��֡ͷ����λ��֮ǰ�ı��� */
            DLS_BitStreamGetBit(pBitStreamStruct, &temp8);
        }
    }

    return FALSE;
}

/*******************************************************************************
* ����  : DLS_BitStreamFindFrameTailer
* ����  : �ӻ������֡β��֡ͷ��֡β��ͬ�������
* ����  : pBitStreamStruct - BitStreamStruct�ṹ��ָ��
* ���  : pData - ��ȡ������
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
static U8 DLS_BitStreamFindFrameTailer(BitStreamStruct *pBitStreamStruct)
{
    U32 i;
    U16 tailIndex;
    U16 tailOffset;
    U16 tempU16;
    U8  tailer;
    
    /*lq ����bit��δ����2��֡ͷ���� */
    if (DLS_BitStreamGetBitNum(pBitStreamStruct) < (DLS_FRAME_HEADER_LEN << 1))
    {
        return FALSE;
    }

    /*lq ����֡β�����ҷ�ΧΪTailIndex ~ Input�� */
    while (DLS_BitStreamGetBitQuantity(pBitStreamStruct->TailIndex,
           pBitStreamStruct->TailOffset, pBitStreamStruct->Input,
           pBitStreamStruct->InputOffset) >= DLS_FRAME_HEADER_LEN) //lq TailIndex ~ Input֮��ı���������֡β
    {
        tailer = 0;
        
        /*lq ��ȡ֡ͷ���ȵı����� */
        tailIndex = pBitStreamStruct->TailIndex;
        tailOffset = pBitStreamStruct->TailOffset;

        for (i = 0; i < DLS_FRAME_HEADER_LEN; i++, tailOffset++)
        {
            if (tailOffset >= DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)
            {
                tailOffset = 0;
                tailIndex = (tailIndex + 1) % DLS_BITSTREAM_BUFF_LEN; 
            }

            tailer <<= 1;
            if ((pBitStreamStruct->DataBuff[tailIndex] & (1 << tailOffset))
                != 0)
            {
                tailer |= 1; 
            }
        }

        if (tailer == DLS_FRAME_HEADER)
        {            
            return TRUE;
        }
        else
        {
            /*lq ֡β�������� */
            pBitStreamStruct->TailOffset++;
            if (pBitStreamStruct->TailOffset >= DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)
            {
                pBitStreamStruct->TailOffset = 0;
                pBitStreamStruct->TailIndex = (pBitStreamStruct->TailIndex + 1)
                                               % DLS_BITSTREAM_BUFF_LEN;
            }
        }
    }

    return FALSE;
}

/*******************************************************************************
* ����  : DLS_FrameRxFifoIsFull
* ����  : ֡�����Ƿ�����
* ����  : pFrameRxFifoStruct - FrameRxFifoStruct�ṹ��ָ��		  
* ���  : ��
* ����  : TRUE - ������FALSE - δ��
*******************************************************************************/
static U8 DLS_FrameRxFifoIsFull(FrameRxFifoStruct *pFrameRxFifoStruct)
{
    if ((pFrameRxFifoStruct->Input + 1) % DLS_FRAME_FIFO_DEPTH
        == pFrameRxFifoStruct->Output)
    {
        return TRUE;

        debug_printf("$[%6d:%4d] %s : Frame rx fifo [%s]is full!\r\n",
            GetFrameNum(), GetSlotNum(), __func__,
            pFrameRxFifoStruct == &FrameFifoRx1 ? "A" : "B");
    }
    else
    {
        return FALSE;
    }     
}

/*******************************************************************************
* ����  : DLS_FrameTxFifoIsFull
* ����  : ֡�����Ƿ�����
* ����  : pFrameTxFifoStruct - FrameTxFifoStruct�ṹ��ָ��		  
* ���  : ��
* ����  : TRUE - ������FALSE - δ��
*******************************************************************************/
static U8 DLS_FrameTxFifoIsFull(FrameTxFifoStruct *pFrameTxFifoStruct)
{
    if ((pFrameTxFifoStruct->Input + 1) % DLS_FRAME_FIFO_DEPTH
        == pFrameTxFifoStruct->Output)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }     
}

/*******************************************************************************
* ����  : DLS_FrameRxFifoIsEmpty
* ����  : ֡�����Ƿ��Ƿ�Ϊ��
* ����  : pFrameRxFifoStruct - FrameRxFifoStruct�ṹ��ָ��		  
* ���  : ��
* ����  : TRUE - Ϊ�գ�FALSE - �ǿ�
*******************************************************************************/
static U8 DLS_FrameRxFifoIsEmpty(FrameRxFifoStruct *pFrameRxFifoStruct)
{
    if (pFrameRxFifoStruct->Input == pFrameRxFifoStruct->Output)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* ����  : DLS_FrameTxFifoIsEmpty
* ����  : ֡�����Ƿ��Ƿ�Ϊ��
* ����  : pFrameTxFifoStruct - FrameTxFifoStruct�ṹ��ָ��		  
* ���  : ��
* ����  : TRUE - Ϊ�գ�FALSE - �ǿ�
*******************************************************************************/
static U8 DLS_FrameTxFifoIsEmpty(FrameTxFifoStruct *pFrameTxFifoStruct)
{
    if (pFrameTxFifoStruct->Input == pFrameTxFifoStruct->Output)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* ����  : DLS_FrameRxFifoEnque
* ����  : ֡�������
* ����  : pFrameRxFifoStruct - FrameRxFifoStruct�ṹ��ָ��
          pFrameRxCellStruct - ָ��������ݵ�Ԫ��ָ��
* ���  : ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
static U8 DLS_FrameRxFifoEnque(FrameRxFifoStruct *pFrameRxFifoStruct, const FrameRxCellStruct *pFrameRxCellStruct)
{
    U16 i;
    FrameRxCellStruct *pCell;
       
    if (DLS_FrameRxFifoIsFull(pFrameRxFifoStruct) == FALSE)
    {      
        pCell = &(pFrameRxFifoStruct->FIFO[pFrameRxFifoStruct->Input]);
        
        pCell->DataLen = pFrameRxCellStruct->DataLen;
        for (i = 0; i < pFrameRxCellStruct->DataLen; i++)        
        {
            pCell->Data[i] = pFrameRxCellStruct->Data[i];         
        }
        pCell->RxChannel = pFrameRxCellStruct->RxChannel;
        pCell->ArrivalFrame = pFrameRxCellStruct->ArrivalFrame;
        pCell->ArrivalSlot = pFrameRxCellStruct->ArrivalSlot;
                
        pFrameRxFifoStruct->Input = (pFrameRxFifoStruct->Input + 1) % DLS_FRAME_FIFO_DEPTH;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* ����  : DLS_FrameTxFifoEnque
* ����  : ����֡�������
* ����  : pFrameTxFifoStruct - FrameTxFifoStruct�ṹ��ָ��
          pFrameTxCellStruct - ָ��������ݵ�Ԫ��ָ��
* ���  : ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
static U8 DLS_FrameTxFifoEnque(FrameTxFifoStruct *pFrameTxFifoStruct, const FrameTxCellStruct *pFrameTxCellStruct)
{
    U16 i;
    FrameTxCellStruct *pCell;
       
    if (DLS_FrameTxFifoIsFull(pFrameTxFifoStruct) == FALSE)
    {      
        pCell = &(pFrameTxFifoStruct->FIFO[pFrameTxFifoStruct->Input]);
        
        //lq pCell->DataLenBit = pFrameTxCellStruct->DataLenBit;
        for (i = 0; i < pFrameTxCellStruct->Data[0] + 3; i++)   //lq ǰ2�ֽ�+���ֽ���+�����ֽ�        
        {
            pCell->Data[i] = pFrameTxCellStruct->Data[i];         
        }
        pCell->TxChannel = pFrameTxCellStruct->TxChannel;
        pCell->TxSlot = pFrameTxCellStruct->TxSlot;
                
        pFrameTxFifoStruct->Input = (pFrameTxFifoStruct->Input + 1) % DLS_FRAME_FIFO_DEPTH;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* ����  : DLS_Inst1FrameRxFifoEnque
* ����  : ����֡�������
* ����  : pFrameTxCellStruct - ָ��������ݵ�Ԫ��ָ��      		  
* ���  : ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
U8 DLS_Inst1FrameRxFifoEnque(const FrameRxCellStruct *pFrameRxCellStruct)
{
    return DLS_FrameRxFifoEnque(&FrameFifoRx1, pFrameRxCellStruct);
}

/*******************************************************************************
* ����  : DLS_Inst2FrameRxFifoEnque
* ����  : ֡�������
* ����  : pFrameTxCellStruct - ָ��������ݵ�Ԫ��ָ��      		  
* ���  : ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
U8 DLS_Inst2FrameRxFifoEnque(const FrameRxCellStruct *pFrameRxCellStruct)
{
    return DLS_FrameRxFifoEnque(&FrameFifoRx2, pFrameRxCellStruct);
}

/*******************************************************************************
* ����  : DLS_InstFrameTxFifoEnque
* ����  : ֡�������
* ����  : pFrameTxCellStruct - ָ��������ݵ�Ԫ��ָ��       		  
* ���  : ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
U8 DLS_InstFrameTxFifoEnque(const FrameTxCellStruct *pFrameTxCellStruct)
{
    return DLS_FrameTxFifoEnque(&FrameFifoTx, pFrameTxCellStruct);
}

/*******************************************************************************
* ����  : DLS_FrameRxFifoDeque
* ����  : ֡���г���
* ����  : pFrameRxFifoStruct - FrameFifoStruct�ṹ��ָ��          
* ���  : pFrameRxCellStruct - ָ��������ݵ�Ԫ��ָ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
static U8 DLS_FrameRxFifoDeque(FrameRxFifoStruct *pFrameRxFifoStruct, FrameRxCellStruct *pFrameRxCellStruct)
{
    U16 i;
    FrameRxCellStruct *pCell;

    if (DLS_FrameRxFifoIsEmpty(pFrameRxFifoStruct) == FALSE)
    {        
        pCell = &(pFrameRxFifoStruct->FIFO[pFrameRxFifoStruct->Output]);
        
        pFrameRxCellStruct->DataLen = pCell->DataLen;
        for (i = 0; i < pCell->DataLen; i++)        
        {
            pFrameRxCellStruct->Data[i] = pCell->Data[i];         
        }
        pFrameRxCellStruct->RxChannel = pCell->RxChannel;
        pFrameRxCellStruct->ArrivalFrame = pCell->ArrivalFrame;
        pFrameRxCellStruct->ArrivalSlot = pCell->ArrivalSlot;

        pFrameRxFifoStruct->Output = (pFrameRxFifoStruct->Output + 1) % DLS_FRAME_FIFO_DEPTH;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* ����  : DLS_FrameTxFifoDeque
* ����  : ֡���г���
* ����  : pFrameTxFifoStruct - FrameFifoStruct�ṹ��ָ��          
* ���  : pFrameTxCellStruct - ָ��������ݵ�Ԫ��ָ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
static U8 DLS_FrameTxFifoDeque(FrameTxFifoStruct *pFrameTxFifoStruct, FrameTxCellStruct *pFrameTxCellStruct)
{
    U16 i;
    FrameTxCellStruct *pCell;

    if (DLS_FrameTxFifoIsEmpty(pFrameTxFifoStruct) == FALSE)
    {        
        pCell = &(pFrameTxFifoStruct->FIFO[pFrameTxFifoStruct->Output]);
        
        //lq pFrameTxCellStruct->DataLenBit = pCell->DataLenBit;
        for (i = 0; i < pCell->Data[0] + 3; i++)       //lq ǰ2�ֽ�+���ֽ���+�����ֽ�                
        {
            pFrameTxCellStruct->Data[i] = pCell->Data[i];         
        }
        pFrameTxCellStruct->TxChannel = pCell->TxChannel;
        pFrameTxCellStruct->TxSlot = pCell->TxSlot;

        pFrameTxFifoStruct->Output = (pFrameTxFifoStruct->Output + 1) % DLS_FRAME_FIFO_DEPTH;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* ����  : DLS_Inst1FrameRxFifoDeque
* ����  : ֡���г���
* ����  : ��
* ���  : pFrameTxCellStruct - ָ��������ݵ�Ԫ��ָ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
U8 DLS_Inst1FrameRxFifoDeque(FrameRxCellStruct *pFrameRxCellStruct)
{
    return DLS_FrameRxFifoDeque(&FrameFifoRx1, pFrameRxCellStruct);
}

/*******************************************************************************
* ����  : DLS_Inst2FrameRxFifoDeque
* ����  : ֡���г���
* ����  : ��
* ���  : pFrameTxCellStruct - ָ��������ݵ�Ԫ��ָ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
U8 DLS_Inst2FrameRxFifoDeque(FrameRxCellStruct *pFrameRxCellStruct)
{
    return DLS_FrameRxFifoDeque(&FrameFifoRx2, pFrameRxCellStruct);
}

/*******************************************************************************
* ����  : DLS_InstFrameRxFifoDeque
* ����  : ֡���г���
* ����  : ��
* ���  : pFrameTxCellStruct - ָ��������ݵ�Ԫ��ָ��
* ����  : TRUE - �ɹ���FALSE - ʧ��
*******************************************************************************/
U8 DLS_InstFrameTxFifoDeque(FrameTxCellStruct *pFrameTxCellStruct)
{
    return DLS_FrameTxFifoDeque(&FrameFifoTx, pFrameTxCellStruct);
}

/*******************************************************************************
* ����  : DLS_PacketProbeProcess
* ����  : ���ݰ�̽��
* ����  : pPacketProbeStruct - PacketProbeStruct�ṹ��ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
static void DLS_PacketProbeProcess(PacketProbeStruct *pPacketProbeStruct, FrameRxFifoStruct *pFrameRxFifoStruct)
{
    FrameRxCellStruct *pFrameRxCell;
    U8 *pFrame;
    U16 i;
    S32 tempS32;

    switch (pPacketProbeStruct->State)
    {
        case DLS_PACKET_PROBE_STATE_HEAD_PROBING:
            
            /*lq �ж��Ƿ���֡ͷ */
            if (DLS_BitStreamFindFrameHeader(&pPacketProbeStruct->BitStream) == TRUE)
            {
                pPacketProbeStruct->State = DLS_PACKET_PROBE_STATE_TAIL_PROBING;
                //lq usart_printf("$PacketProbe: Frame header found!\r\n");                                            
            }

            break;
        case DLS_PACKET_PROBE_STATE_TAIL_PROBING:
            
            /*lq �жϱ����������Ƿ����� */
            if (DLS_BitStreamIsBuffFull(&pPacketProbeStruct->BitStream) == TRUE)
            {
                /*lq ��ջ��� */ 
				/*zjw �ﵽ��󳤶ȣ��������7�������� */
                DLS_BitStreamFlushBuff(&pPacketProbeStruct->BitStream);
                
                /*lq ת��֡ͷ��� */
                pPacketProbeStruct->State = DLS_PACKET_PROBE_STATE_HEAD_PROBING;                    
            }
            else
            {
                /*lq �ж��Ƿ���֡β */
                if (DLS_BitStreamFindFrameTailer(&pPacketProbeStruct->BitStream) == TRUE)
                {
                    //lq usart_printf("$PacketProbe: Frame tailer found!\r\n");                                              
#if 0
                    usart_printf("$PacketProbe: RSSI %d, %d,",
                    SPIRIT1_ReadByte(SPIRIT1TypeA, SPIRIT1_REGADDR_FU_RSSI_LEVEL),
                    SPIRIT1_GetRegFieldValue(SPIRIT1TypeA, SPIRIT1_REGADDR_FU_LINK_QUALIF(1), SPIRIT1_FU_LINK_QUALIF1_BITMASK_CS));                                              
#endif
                    /*lq ��ȡ֡ */
                    pFrame = (U8 *)malloc((DLS_FRAME_MAX_LEN_BYTE + 1) * sizeof(U8));   //lq [0]���֡�����ֽ���
                    memset((U8 *)pFrame, 0, (DLS_FRAME_MAX_LEN_BYTE + 1) * sizeof(U8));
#if 1
                    if (DLS_BitStreamGetFrame(&pPacketProbeStruct->BitStream, pFrame) == TRUE)
#else   //lqdb
                    if (DLS_BitStreamGetFrame3(&pPacketProbeStruct->BitStream, pFrame) == TRUE)
#endif
                    {
                        pFrameRxCell = (FrameRxCellStruct *)malloc(sizeof(FrameRxCellStruct));
                        memset((FrameRxCellStruct *)pFrameRxCell, 0, sizeof(FrameRxCellStruct));
                                                
                        tempS32 = GetSlotSum();
                        pFrameRxCell->ArrivalFrame = tempS32 / FRAME_SLOT_SIZE;
                        pFrameRxCell->ArrivalSlot = tempS32 % FRAME_SLOT_SIZE;

                        pFrameRxCell->DataLen = pFrame[0];
                        for (i = 0; i < pFrame[0]; i++)
                        {
                            pFrameRxCell->Data[i] = pFrame[i + 1];
                        }
                        
                        if (pPacketProbeStruct == &PacketProbeInst1)
                        {
                            pFrameRxCell->RxChannel = ENUM_AIS_CHANNEL_A;
                        }
                        else
                        {
                            pFrameRxCell->RxChannel = ENUM_AIS_CHANNEL_B;
                        }
                                                                        
                        /*lq ����֡��� */
                        DLS_FrameRxFifoEnque(pFrameRxFifoStruct, pFrameRxCell);
                        free((FrameRxCellStruct *)pFrameRxCell);
#if 0
                        usart_printf("$PacketProbe: Frame extraction completed!\r\n"); 
#endif                                                                     
                    }
                    
                    free((U8 *)pFrame);

                    /*lq ת��֡ͷ��� */
                    pPacketProbeStruct->State = DLS_PACKET_PROBE_STATE_HEAD_PROBING;
                }            
            }

            break;
        default :
            
            DLS_PacketProbeStructInit(pPacketProbeStruct);
            break;
    }    
}

/*******************************************************************************
* ����  : DLS_InstPacketProbeProcess
* ����  : ���ݰ�̽��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void DLS_InstPacketProbeProcess(void)
{
    DLS_PacketProbeProcess(&PacketProbeInst1, &FrameFifoRx1);    
    DLS_PacketProbeProcess(&PacketProbeInst2, &FrameFifoRx2);    
}

#if 0
/*******************************************************************************
* ����  : DLS_FrameCrcCheck
* ����  : �������CRC������
* ����  : buff1 - ��У�����ݣ�buff1[0]-��У�����ݵ����ֽ�����buff1[1]-��У������
          �ķ����ֽڵı���������ֵӦʼ��Ϊ0��
* ���  : buff1 - ��CRC������
* ����  : ��
*******************************************************************************/
void DLS_FrameCrcCheck(U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U16 CRCVal;

    CRCVal = AIS_Cal_CRC16(&buff1[2], buff1[0]); 
	buff1[buff1[0] + 2] = ((CRCVal >> 8) & 0x00ff);     //lq CRC�ĸ�λ
	buff1[buff1[0] + 3] = (CRCVal & 0x00ff);
    buff1[0] += 2;                                      //lq �����ֽڳ��ȼ�2       
}
#else
/*******************************************************************************
* ����  : DLS_FrameCrcCheck
* ����  : �������CRC������
* ����  : buff1 - ��У�����ݣ�buff1[0]-��У�����ݵ����ֽ�����buff1[1]-��У������
          �ķ����ֽڵı���������ֵӦʼ��Ϊ0��
* ���  : buff2 - ��CRC������
* ����  : ��
*******************************************************************************/
void DLS_FrameCrcCheck(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U16 CRCVal;
    U16 index1;
    U8  byteNum;

	byteNum = buff1[0];

	memset((U8*)buff2, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    /*lq ���ֽ� */
    for (index1 = 2; index1 < byteNum + 2; index1++)
    {
        buff2[index1] = buff1[index1];
    }

    CRCVal = AIS_Cal_CRC16(&buff2[2], byteNum); 
	buff2[index1++] = ((CRCVal >> 8) & 0x00ff);                 //lq CRC�ĸ�λ
	buff2[index1++] = (CRCVal & 0x00ff);
    buff2[0] = byteNum + 2;                                     //lq �����ֽڳ��ȼ�2       
    buff2[1] = 0;
}
#endif

/*******************************************************************************
* ����  : DLS_FrameBitStuff
* ����  : ����������λ������ ��buff2��buff1�����ݵ�MSB��LSB˳��һ�£����λʱ��
          ��1�ļ����Ǵ�ÿ���ֽڵ�LSB��MSB�ķ�������ģ���֡ʵ�ʷ�����������·��˳
          ��
* ����  : buff1 - ��������λ�����ݣ�buff1[0]-��������λ�����ݵ����ֽ�����
          buff1[1]-��������λ�����ݵķ����ֽڱ���������
* ���  : buff2 - ��������λ�����ݣ�buff2[0]-��������λ�����ݵ����ֽ�����
          buff2[1]-��������λ�����ݵķ����ֽڱ���������
* ����  : ��
*******************************************************************************/
void DLS_FrameBitStuff(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U16 index1;
    U16 index2;
    U8  offset1;
    U8  offset2;
    U8  byteNum;
    U8  bitNum;
    U8  consOnes;

    index2 = 2;
    offset2 = 0;
    consOnes = 0;
	byteNum = buff1[0];
    bitNum = buff1[1];

	memset((U8*)buff2, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    /*lq ���ֽ� */
    for (index1 = 2; index1 < byteNum + 2; index1++)
    {
        for (offset1 = 0; offset1 < 8; offset1++)
        {
            if ((buff1[index1] & (1u << offset1)) != 0) //lq ��ǰλΪ��1��
            {
                buff2[index2] |= (1u << offset2++);
                if (offset2 >= 8)
                {
                    offset2 = 0;
                    index2++;
                }

                consOnes++;
                if (consOnes >= 5)                      //lq ����5��1�����һ��0
                {
                    consOnes = 0;
                    offset2++;                          //lq �����ѳ�ʼ��Ϊ0��ֻ���ƶ�����
                    if (offset2 >= 8)
                    {
                        offset2 = 0;
                        index2++;
                    }
                }
            }
            else                                        //lq ��ǰλΪ��0��
            {
                consOnes = 0;
                offset2++;                              //lq �����ѳ�ʼ��Ϊ0��ֻ���ƶ�����
                if (offset2 >= 8)
                {
                    offset2 = 0;
                    index2++;
                }            
            }
        }
    } 
    
    /*lq �����ֽڱ��� */
    for (offset1 = 0; offset1 < bitNum; offset1++)
    {
        if ((buff1[index1] & (1u << offset1)) != 0)     //lq ��ǰλΪ��1��
        {
            buff2[index2] |= (1u << offset2++);
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }

            consOnes++;
            if (consOnes >= 5)                          //lq ����5��1�����һ��0
            {
                consOnes = 0;
                offset2++;                              //lq �����ѳ�ʼ��Ϊ0��ֻ���ƶ�����
                if (offset2 >= 8)
                {
                    offset2 = 0;
                    index2++;
                }
            }
        }
        else                                            //lq ��ǰλΪ��0��
        {
            consOnes = 0;
            offset2++;                                  //lq �����ѳ�ʼ��Ϊ0��ֻ���ƶ�����
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }            
        }        
    }
    
    /*lq ��¼���ֽ����ͷ����ֽڱ����� */
    buff2[0] = index2 - 2;
    buff2[1] = offset2; 
}

/*******************************************************************************
* ����  : DLS_FrameBitDestuff
* ����  : �����޳����λ������ ��buff2��buff1�����ݵ�MSB��LSB˳��һ�£����λʱ����1
          �ļ����Ǵ�ÿ���ֽڵ�LSB��MSB�ķ�������ģ���֡ʵ�ʷ�����������·��˳��
* ����  : buff1 - ���޳����λ�����ݣ�buff1[0]-���޳����λ�����ݵ����ֽ�����
          buff1[1]-���޳����λ�����ݵķ����ֽڱ���������
* ���  : buff2 - ���޳����λ�����ݣ�buff2[0]-���޳����λ�����ݵ����ֽ�����
          buff2[1]-���޳����λ�����ݵķ����ֽڱ���������
* ����  : ��
*******************************************************************************/
void DLS_FrameBitDestuff(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U16 index1;
    U16 index2;
    U8  offset1;
    U8  offset2;
    U8  byteNum;
    U8  bitNum;
    U8  consOnes;

    index2 = 2;
    offset2 = 0;
    consOnes = 0;
	byteNum = buff1[0];
    bitNum = buff1[1];

	memset((U8*)buff2, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    /*lq ���ֽ� */
    for (index1 = 2; index1 < byteNum + 2; index1++)
    {
        for (offset1 = 0; offset1 < 8; offset1++)
        {
            if ((buff1[index1] & (1u << offset1)) != 0) //lq ��ǰλΪ��1��
            {
                buff2[index2] |= (1u << offset2++);
                if (offset2 >= 8)
                {
                    offset2 = 0;
                    index2++;
                }

                consOnes++;
            }
            else                                        //lq ��ǰλΪ��0��
            {
                if (consOnes >= 5)                      //lq ����5��1��ɾ�����������һ��0
                {
                    /*lq �޳����λ��������0֮ǰ��������5��"1"��ɾ���ñ���"0" */ 
                }
                else
                {
                    offset2++;
                    if (offset2 >= 8)
                    {
                        offset2 = 0;
                        index2++;
                    }                
                }
                
                consOnes = 0;                        
            }
        }
    } 
    
    /*lq �����ֽڱ��� */
    for (offset1 = 0; offset1 < bitNum; offset1++)
    {
        if ((buff1[index1] & (1u << offset1)) != 0) //lq ��ǰλΪ��1��
        {
            buff2[index2] |= (1u << offset2++);
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }

            consOnes++;
        }
        else                                        //lq ��ǰλΪ��0��
        {
            if (consOnes >= 5)                      //lq ����5��1��ɾ�����������һ��0
            {
                /*lq �޳����λ��������0֮ǰ��������5��"1"��ɾ���ñ���"0" */ 
            }
            else
            {
                offset2++;
                if (offset2 >= 8)
                {
                    offset2 = 0;
                    index2++;
                }                
            }
            
            consOnes = 0;                        
        }        
    }
    
    /*lq ��¼���ֽ����ͷ����ֽڱ����� */
    buff2[0] = index2 - 2;
    buff2[1] = offset2; 
}

/*******************************************************************************
* ����  : DLS_FrameDataPack
* ����  : ���ݴ����֡
* ����  : buff1 - ��������ݣ��������ݲ��ֺ�CRC�����ѽ���λ��䣩
* ���  : buff2 - �Ѵ�����ݣ�����ͬ�����С�֡ͷ�����ݡ�У�����֡β����δ����
          NRZI���룩
* ����  : ��
*******************************************************************************/
void DLS_FrameDataPack(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U16 index1;
    U16 index2;
    U8  offset1;
    U8  offset2;
    U8  byteNum;
    U8  bitNum;

    index2 = 2;
    offset2 = 0;	
    byteNum = buff1[0];
    bitNum = buff1[1];
             
	memset((U8*)buff2, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    /*lq ͬ������ */
    buff2[index2++] = 0xAA;     //lq LSB�ȷ��ͣ���ʵ�ʷ��͵�ֵΪ0x55
    buff2[index2++] = 0xAA;
    buff2[index2++] = 0xAA;

    /*lq ֡ͷ */
    buff2[index2++] = DLS_FRAME_HEADER;

    /*lq ����+CRC �����ֽڲ��֣�*/
    for (index1 = 2; index1 < byteNum + 2; index1++)
    {
        buff2[index2++] = buff1[index1];    
    }

    /*lq �����ֽڱ��� */
    for (offset1 = 0; offset1 < bitNum; offset1++)
    {
        if ((buff1[index1] & (1u << offset1)) != 0) //lq ��ǰλΪ��1��
        {
            buff2[index2] |= (1u << offset2++);
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }
        }
        else                                        //lq ��ǰλΪ��0��
        {
            offset2++;
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }                        
        }        
    }

    /*lq ֡β */     
    for (index1 = 0; index1 < 8; index1++)
    {
        if ((DLS_FRAME_TAILER & (1u << index1)) != 0)    //lq ��ǰλΪ��1��
        {
            buff2[index2] |= (1u << offset2++);
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }
        }
        else                                        //lq ��ǰλΪ��0��
        {
            offset2++;
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }                        
        }
     }

    /*lq ��¼���ֽ����ͷ����ֽڱ����� */
    buff2[0] = index2 - 2;
    buff2[1] = offset2; 
}

/*******************************************************************************
* ����  : DLS_FrameDataPack1
* ����  : ���ݴ����֡��ͬ�����С�֡ͷ��֡βʹ��NRZI�����ݺ�У���벻ʹ��NRZI��
* ����  : buff1 - ��������ݣ��������ݲ��ֺ�CRC�����ѽ���λ��䣩
* ���  : buff2 - �Ѵ�����ݣ�����ͬ�����С�֡ͷ�����ݡ�У�����֡β��ͬ�����С�
          ֡ͷ��֡βʹ��NRZI�����ݺ�У���벻ʹ��NRZI��
* ����  : ��
*******************************************************************************/
void DLS_FrameDataPack1(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U16 index1;
    U16 index2;
    U8  offset1;
    U8  offset2;
    U8  byteNum;
    U8  bitNum;
    U8  priorValue;

    index2 = 2;
    offset2 = 0;	
    byteNum = buff1[0];
    bitNum = buff1[1];
             
	memset((U8*)buff2, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    /*lq ͬ�����У�NZRI�� */
    buff2[index2++] = 0xCC;     //lq LSB�ȷ��ͣ�ʵ�ʷ��͵���01010101��NRZI����ʼֵΪ1����������
    buff2[index2++] = 0xCC;
    buff2[index2++] = 0xCC;

    /*lq ֡ͷ��NZRI�� */
    buff2[index2++] = 0x81;     //lq NRZI����ʼֵΪNRZI���ͬ�����е����һλ��

    /*lq ����+CRC �����ֽڲ��֣�*/
    for (index1 = 2; index1 < byteNum + 2; index1++)
    {
        buff2[index2++] = buff1[index1];    
    }

    /*lq �����ֽڱ��� */
    for (offset1 = 0; offset1 < bitNum; offset1++)
    {
        if ((buff1[index1] & (1u << offset1)) != 0) //lq ��ǰλΪ��1��
        {
            buff2[index2] |= (1u << offset2++);
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }
        }
        else                                        //lq ��ǰλΪ��0��
        {
            offset2++;
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }                        
        }        
    }

    /*lq ȷ�����һλ��ֵ������֡βNRZI�ĳ�ʼֵ�� */
    if (offset2 != 0)
    {
        if ((buff2[index2] & (1u << (offset2 - 1))) == 0)
        {
            priorValue = 0;
        }
        else
        {
            priorValue = 1;        
        }
    }
    else
    {
        if ((buff2[index2 - 1] & 0x80) == 0)
        {
            priorValue = 0;
        }
        else
        {
            priorValue = 1;        
        }    
    }

    /*lq ֡β��NRZI�� */
    for (offset1 = 0; offset1 < 8; offset1++)
    {
        if ((DLS_FRAME_TAILER & (1u << offset1)) != 0)  //lq ��ǰλΪ��1��, �豣��
        {                
            if (priorValue == 0)
            {
                buff2[index2] &= (~(1u << offset2));    //lq ����Ϊ��0��                
                priorValue = 0;                
            }
            else
            {
                buff2[index2] |= (1u << offset2);       //lq ����Ϊ��1��
                priorValue = 1;
            }
        }
        else                                            //lq ��ǰλΪ��0�����跭ת
        {
            if (priorValue == 0)
            {
                buff2[index2] |= (1u << offset2);       //lq ��תΪ��1��
                priorValue = 1;
            }
            else
            {
                buff2[index2] &= (~(1u << offset2));    //lq ��תΪ��0��                
                priorValue = 0;
            }
        }

        offset2++;
        if (offset2 >= 8)
        {
            offset2 = 0;
            index2++;
        }
    }

    /*lq ��¼���ֽ����ͷ����ֽڱ����� */
    buff2[0] = index2 - 2;
    buff2[1] = offset2; 
}

/*******************************************************************************
* ����  : DLS_FrameDataDepack
* ����  : ����֡��ȡ����
* ����  : buff1 - ����ȡ���ݣ�����ͬ�����С�֡ͷ�����ݡ�У�����֡β����δ����
          NRZI���룩
* ���  : buff2 - ����ȡ���ݣ��������ݲ��ֺ�CRC���Ұ���λ��䣩
* ����  : ��
*******************************************************************************/
void DLS_FrameDataDepack(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U16 index1;
    U16 index2;
    U8  offset1;
    U8  offset2;
    U8  byteNum;
    U8  bitNum;

    index2 = 2;
    offset2 = 0;	
    byteNum = buff1[0];
    bitNum = buff1[1];
             
	memset((U8*)buff2, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    /*lq ����+CRC �����ֽڲ��֣�*/
    for (index1 = 6; index1 < byteNum + 1; index1++)
    {
        buff2[index2++] = buff1[index1];    
    }

    /*lq �����ֽڱ��� */
    for (offset1 = 0; offset1 < bitNum; offset1++)
    {
        if ((buff1[index1] & (1u << offset1)) != 0) //lq ��ǰλΪ��1��
        {
            buff2[index2] |= (1u << offset2++);
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }
        }
        else                                        //lq ��ǰλΪ��0��
        {
            offset2++;
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }                        
        }        
    }

    /*lq ��¼���ֽ����ͷ����ֽڱ����� */
    buff2[0] = index2 - 2;
    buff2[1] = offset2; 
}

#if 0
/*******************************************************************************
* ����  : DLS_FrameNrziCode
* ����  : NRZI���루ÿ���ֽڵı���˳���Ǵ�LSB��MSB��������������·�ı�����˳��һ
          �£�
* ����  : buff1 - ��NRZI�������ݣ�����ͬ�����С�֡ͷ�����ݡ�У�����֡β��λ��䣩
          initValue - NRZI��ʼֵ
* ���  : buff1 - ��NRZI�������ݣ�����ͬ�����С�֡ͷ�����ݡ�У�����֡β��λ��䣩
* ����  : ��
*******************************************************************************/
void DLS_FrameNrziCode(U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], const U8 initValue)
{
    U16 index1;
    U8  offset1;
    U8  byteNum;
    U8  bitNum;
    U8  priorValue;
	
    byteNum = buff1[0];
    bitNum = buff1[1];
    priorValue = initValue;
     
    if (priorValue != 0) //lq ��Чֵ����
    {
        priorValue = 1;
    }

    /*lq ���ֽ� */
    for (index1 = 2; index1 < byteNum + 2; index1++)
    {
        for (offset1 = 0; offset1 < 8; offset1++)
        {                                  
            if ((buff1[index1] & (1u << offset1)) != 0) //lq ��ǰλΪ��1��, �豣��
            {                
                if (priorValue == 0)
                {
                    buff1[index1] &= ~(1u << offset1);  //lq ����Ϊ��0��                
                    priorValue = 0;
                }
                else
                {
                    buff1[index1] |= 1u << offset1;     //lq ����Ϊ��1��
                    priorValue = 1;
                }
            }
            else                                        //lq ��ǰλΪ��0�����跭ת
            {
                if (priorValue == 0)
                {
                    buff1[index1] |= 1u << offset1;     //lq ��תΪ��1��
                    priorValue = 1;
                }
                else
                {
                    buff1[index1] &= ~(1u << offset1);  //lq ��תΪ��0��                
                    priorValue = 0;
                }
            }
        }    
    }

    /*lq �����ֽڱ��� */
    for (offset1 = 0; offset1 < bitNum; offset1++)
    {
        if ((buff1[index1] & (1u << offset1)) != 0) //lq ��ǰλΪ��1��, �豣��
        {                
            if (priorValue == 0)
            {
                buff1[index1] &= ~(1u << offset1);  //lq ����Ϊ��0��                
                priorValue = 0;
            }
            else
            {
                buff1[index1] |= 1u << offset1;     //lq ����Ϊ��1��
                priorValue = 1;
            }
        }
        else                                        //lq ��ǰλΪ��0�����跭ת
        {
            if (priorValue == 0)
            {
                buff1[index1] |= 1u << offset1;     //lq ��תΪ��1��
                priorValue = 1;
            }
            else
            {
                buff1[index1] &= ~(1u << offset1);  //lq ��תΪ��0��                
                priorValue = 0;
            }
        }   
    }
    
    /*lq ���ֽ����ͷ����ֽڱ����������� */
}
#else
/*******************************************************************************
* ����  : DLS_FrameNrziCode
* ����  : NRZI���루ÿ���ֽڵı���˳���Ǵ�LSB��MSB��������������·�ı�����˳��һ
          �£�
* ����  : buff1 - ��NRZI�������ݣ�����ͬ�����С�֡ͷ�����ݡ�У�����֡β��λ��䣩
          initValue - NRZI��ʼֵ
* ���  : buff2 - ��NRZI�������ݣ�����ͬ�����С�֡ͷ�����ݡ�У�����֡β��λ��䣩
* ����  : ��
*******************************************************************************/
void DLS_FrameNrziCode(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2], const U8 initValue)
{
    U16 index1;
    U8  offset1;
    U8  byteNum;
    U8  bitNum;
    U8  priorValue;
	
    byteNum = buff1[0];
    bitNum = buff1[1];
    priorValue = initValue;
     
    if (priorValue != 0) //lq ��Чֵ����
    {
        priorValue = 1;
    }

	memset((U8*)buff2, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    /*lq ���ֽ� */
    for (index1 = 2; index1 < byteNum + 2; index1++)
    {
        for (offset1 = 0; offset1 < 8; offset1++)
        {                                  
            if ((buff1[index1] & (1u << offset1)) != 0)     //lq ��ǰλΪ��1��, �豣��
            {                
                if (priorValue == 0)
                {
                    buff2[index1] &= (~(1u << offset1));    //lq ����Ϊ��0��                
                    priorValue = 0;                
                }
                else
                {
                    buff2[index1] |= (1u << offset1);       //lq ����Ϊ��1��
                    priorValue = 1;
                }
            }
            else                                            //lq ��ǰλΪ��0�����跭ת
            {
                if (priorValue == 0)
                {
                    buff2[index1] |= (1u << offset1);       //lq ��תΪ��1��
                    priorValue = 1;
                }
                else
                {
                    buff2[index1] &= (~(1u << offset1));    //lq ��תΪ��0��                
                    priorValue = 0;
                }
            }
        }    
    }

    /*lq �����ֽڱ��� */
    for (offset1 = 0; offset1 < bitNum; offset1++)
    {
        if ((buff1[index1] & (1u << offset1)) != 0)     //lq ��ǰλΪ��1��, �豣��
        {                
            if (priorValue == 0)
            {
                buff2[index1] &= (~(1u << offset1));    //lq ����Ϊ��0��                
                priorValue = 0;                
            }
            else
            {
                buff2[index1] |= (1u << offset1);       //lq ����Ϊ��1��
                priorValue = 1;
            }
        }
        else                                            //lq ��ǰλΪ��0�����跭ת
        {
            if (priorValue == 0)
            {
                buff2[index1] |= (1u << offset1);       //lq ��תΪ��1��
                priorValue = 1;
            }
            else
            {
                buff2[index1] &= (~(1u << offset1));    //lq ��תΪ��0��                
                priorValue = 0;
            }
        }
    }
    
    /*lq ��¼���ֽ����ͷ����ֽڱ����� */
    buff2[0] = buff1[0];
    buff2[1] = buff1[1]; 
}
#endif

#if 0
/*******************************************************************************
* ����  : DLS_FrameNrziDecode
* ����  : NRZI���루ÿ���ֽڵĽ���˳���Ǵ�LSB��MSB����������·�����ı�����˳��һ
          �£�
* ����  : buff1 - ��NRZI�������ݣ�����ͬ�����С�֡ͷ�����ݡ�У�����֡β��λ��䣩
          initValue - NRZI��ʼֵ
* ���  : buff1 - ��NRZI�������ݣ�����ͬ�����С�֡ͷ�����ݡ�У�����֡β��λ��䣩
* ����  : ��
*******************************************************************************/
void DLS_FrameNrziDecode(U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], const U8 initValue)
{
    U16 index1;
    U8  offset1;
    U8  byteNum;
    U8  bitNum;
    U8  priorValue;
	
    byteNum = buff1[0];
    bitNum = buff1[1];
    priorValue = initValue;
     
    if (priorValue != 0) //lq ��Чֵ����
    {
        priorValue = 1;
    }

    /*lq ���ֽ� */
    for (index1 = 2; index1 < byteNum + 2; index1++)
    {
        for (offset1 = 0; offset1 < 8; offset1++)
        {                                  
            if ((buff1[index1] & (1u << offset1)) == (priorValue << offset1))   //lq ����, ��ǰλΪ��1��
            {                
                priorValue = (buff1[index1] >> offset1) & 0x01;
                
                buff1[index1] |= 1u << offset1;                                 //lq ��ǰλΪ��1��
            }
            else                                                                //lq ��ת, ��ǰλΪ��0��
            {
                priorValue = (buff1[index1] >> offset1) & 0x01;
                
                buff1[index1] &= ~(1u << offset1);                              //lq ��ǰλΪ��0��                     
            }
        }    
    }

    /*lq �����ֽڱ��� */
    for (offset1 = 0; offset1 < bitNum; offset1++)
    {                                  
        if ((buff1[index1] & (1u << offset1)) == (priorValue << offset1))   //lq ����, ��ǰλΪ��1��
        {                
            priorValue = (buff1[index1] >> offset1) & 0x01;
            
            buff1[index1] |= 1u << offset1;                                 //lq ��ǰλΪ��1��
        }
        else                                                                //lq ��ת, ��ǰλΪ��0��
        {
            priorValue = (buff1[index1] >> offset1) & 0x01;
            
            buff1[index1] &= ~(1u << offset1);                              //lq ��ǰλΪ��0��                     
        }
    }
    
    /*lq ���ֽ����ͷ����ֽڱ����������� */
}
#else
/*******************************************************************************
* ����  : DLS_FrameNrziDecode
* ����  : NRZI���루ÿ���ֽڵĽ���˳���Ǵ�LSB��MSB����������·�����ı�����˳��һ
          �£�
* ����  : buff1 - ��NRZI�������ݣ�����ͬ�����С�֡ͷ�����ݡ�У�����֡β��λ��䣩
          initValue - NRZI��ʼֵ
* ���  : buff2 - ��NRZI�������ݣ�����ͬ�����С�֡ͷ�����ݡ�У�����֡β��λ��䣩
* ����  : ��
*******************************************************************************/
void DLS_FrameNrziDecode(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2], const U8 initValue)
{
    U16 index1;
    U8  offset1;
    U8  byteNum;
    U8  bitNum;
    U8  priorValue;
	
    byteNum = buff1[0];
    bitNum = buff1[1];
    priorValue = initValue;
     
    if (priorValue != 0) //lq ��Чֵ����
    {
        priorValue = 1;
    }

	memset((U8*)buff2, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    /*lq ���ֽ� */
    for (index1 = 2; index1 < byteNum + 2; index1++)
    {
        for (offset1 = 0; offset1 < 8; offset1++)
        {                                  
            if ((buff1[index1] & (1u << offset1)) == (priorValue << offset1))   //lq ����, ��ǰλΪ��1��
            {                
                priorValue = (buff1[index1] >> offset1) & 0x01;
                
                buff2[index1] |= (1u << offset1);                               //lq ��ǰλΪ��1��
            }
            else                                                                //lq ��ת, ��ǰλΪ��0��
            {
                priorValue = (buff1[index1] >> offset1) & 0x01;
                
                buff2[index1] &= (~(1u << offset1));                            //lq ��ǰλΪ��0��                     
            }
        }    
    }

    /*lq �����ֽڱ��� */
    for (offset1 = 0; offset1 < bitNum; offset1++)
    {                                  
        if ((buff1[index1] & (1u << offset1)) == (priorValue << offset1))   //lq ����, ��ǰλΪ��1��
        {                
            priorValue = (buff1[index1] >> offset1) & 0x01;
            
            buff2[index1] |= (1u << offset1);                               //lq ��ǰλΪ��1��
        }
        else                                                                //lq ��ת, ��ǰλΪ��0��
        {
            priorValue = (buff1[index1] >> offset1) & 0x01;
            
            buff2[index1] &= (~(1u << offset1));                            //lq ��ǰλΪ��0��                     
        }
    }
        
    /*lq ��¼���ֽ����ͷ����ֽڱ����� */
    buff2[0] = buff1[0];
    buff2[1] = buff1[1]; 
}
#endif

/*******************************************************************************
* ����  : DLS_FrameBuild
* ����  : ��֡
* ����  : buff1 - ԭʼ����
* ���  : buff2 - ����֡���ݣ�����ͬ�����С�֡ͷ�����ݡ�У�����֡β��λ��䣬��
          �����������ѽ���NRZI��
* ����  : ��
*******************************************************************************/
void DLS_FrameBuild(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U8 *pBuff;

    pBuff = (U8 *)malloc((DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));
    memset((U8 *)pBuff, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    DLS_FrameCrcCheck(buff1, pBuff);
    DLS_FrameBitStuff(pBuff, buff2);
    DLS_FrameDataPack(buff2, pBuff);
    DLS_FrameNrziCode(pBuff, buff2, 1);
    
    free((U8 *)pBuff);
}

/*******************************************************************************
* ����  : DLS_FrameBuildTest
* ����  : ��֡��ͬ�����С�֡ͷ��֡βʹ��NRZI�����ݺ�У���벻ʹ��NRZI��
* ����  : buff1 - ԭʼ����
* ���  : buff2 - ����֡���ݣ�ͬ�����С�֡ͷ��֡βʹ��NRZI�����ݺ�У���벻ʹ��NRZI��
* ����  : ��
*******************************************************************************/
void DLS_FrameBuildTest(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U8 *pBuff;

    pBuff = (U8 *)malloc((DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));
    memset((U8 *)pBuff, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    DLS_FrameCrcCheck(buff1, buff2);
    DLS_FrameBitStuff(buff2, pBuff);
    DLS_FrameDataPack1(pBuff, buff2);   //lq ͬ�����С�֡ͷ��֡βʹ��NRZI�����ݺ�У���벻ʹ��NRZI
    
    free((U8 *)pBuff);
}

/*******************************************************************************
* ����  : DLS_FrameBuildTest2
* ����  : ��֡��ͬ�����С�֡ͷ��֡β�����ݺ�У���붼��ʹ��NRZI��
* ����  : buff1 - ԭʼ����
* ���  : buff2 - ����֡���ݣ�ͬ�����С�֡ͷ��֡β�����ݺ�У���붼��ʹ��NRZI��
* ����  : ��
*******************************************************************************/
void DLS_FrameBuildTest2(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U8 *pBuff;

    pBuff = (U8 *)malloc((DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));
    memset((U8 *)pBuff, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    DLS_FrameCrcCheck(buff1, buff2);
    DLS_FrameBitStuff(buff2, pBuff);
    DLS_FrameDataPack(pBuff, buff2);
    
    free((U8 *)pBuff);
}

/*******************************************************************************
* ����  : DLS_ModuleInit
* ����  : ģ���ʼ��
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void DLS_ModuleInit(void)
{
    DLS_InstFrameFifoStructInit();
    DLS_InstPacketProbeStructInit();
}
