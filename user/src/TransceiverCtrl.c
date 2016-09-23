/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: TransceiverCtrl.c
  Author: lq       Version : V1.0.0        Date: 2014.08.06
  Description:     Transceiver����
  Version:         V1.0.0
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "stm32f10x.h"
#include "def.h"
#include "sotdma.h"
#include "RxTxCtrl.h"
#include "Include.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define ST_SIGNAL_LEN   21

/* Private macro -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
const U8 StSignal1[ST_SIGNAL_LEN] = 
{
    0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
	0x55,
};

const U8 StSignal2[ST_SIGNAL_LEN] = 
{
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,
};

const U8 StSignal3[ST_SIGNAL_LEN] = 
{
	0x48,0x99,0x07,0xad,0x40,
    0x00,0x4c,0x0b,0xa6,0x74,
    0x52,0xe4,0xdb,0x3d,0x86,
    0x32,0xac,0x00,0x1e,0x00,
    0x06,
};

TransceiverTestStruct TransceiverTest1;
TransceiverTestStruct TransceiverTest2;
TransceiverTxCtrlStruct TransceiverTxCtrlInstance;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* ����  : TRCTRL_TransceiverTxCtrlStructInit
* ����  : �ṹ���ʼ��
* ����  : TransceiverTxCtrlStruct - pTransceiverTxCtrlStruct�ṹ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void TRCTRL_TransceiverTxCtrlStructInit(TransceiverTxCtrlStruct *pTransceiverTxCtrlStruct)
{
	pTransceiverTxCtrlStruct->TimeCnt = 0;
	pTransceiverTxCtrlStruct->TimePeriod = TRCTRL_TIMEOUT_TX_DONE_WAITING;

    pTransceiverTxCtrlStruct->TxCtrlState = TRCTRL_TX_CTRL_STATE_IDLE;	
    pTransceiverTxCtrlStruct->TxFlg = FALSE;
    pTransceiverTxCtrlStruct->TxDoneFlg = FALSE;
    pTransceiverTxCtrlStruct->TxSlot = 0;
    pTransceiverTxCtrlStruct->TxChannel = ENUM_AIS_CHANNEL_A;
    pTransceiverTxCtrlStruct->TxDataCtrlState = TRCTRL_TX_DATA_CTRL_STATE_IDLE;	    
    pTransceiverTxCtrlStruct->TxDataStartBitCnt = 0;
    pTransceiverTxCtrlStruct->TxDataEndBitCnt = 0;
        
    pTransceiverTxCtrlStruct->TxBufferIndex = 0;
    pTransceiverTxCtrlStruct->TxBufferOffset = 0;            
}

/*******************************************************************************
* ����  : TRCTRL_InstTransceiverTxCtrlStructInit
* ����  : ʵ����ʼ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void TRCTRL_InstTransceiverTxCtrlStructInit(void)
{
    TRCTRL_TransceiverTxCtrlStructInit(&TransceiverTxCtrlInstance);       
}

/*******************************************************************************
* ����  : TRCTRL_TransceiverTestStructInit
* ����  : �ṹ���ʼ��
* ����  : pTransceiverTestStruct - TransceiverTestStruct�ṹ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void TRCTRL_TransceiverTestStructInit(TransceiverTestStruct *pTransceiverTestStruct)
{
    pTransceiverTestStruct->TimeCnt = 0;
    pTransceiverTestStruct->TimePeriod = TRCTRL_TIME_PERIOD;
    pTransceiverTestStruct->TimeoutTx = 0;
    pTransceiverTestStruct->TimeoutHalt = 0;
    pTransceiverTestStruct->TimeoutCntTx = 0;
    pTransceiverTestStruct->TimeoutCntHalt = 0;
    pTransceiverTestStruct->TxCtrlSta = TRCTRL_TEST_TX_CTRL_STA_IDLE;
    pTransceiverTestStruct->TxTestFlg = FALSE;
    pTransceiverTestStruct->TxChannel = ENUM_AIS_CHANNEL_A;
    pTransceiverTestStruct->TxSignalType = TRCTRL_TEST_TX_SIGNAL_NUM1; 
    pTransceiverTestStruct->TxBufferIndex = 0;
    pTransceiverTestStruct->TxBufferOffset = 0;             
}

/*******************************************************************************
* ����  : TRCTRL_InstTransceiverTestStructInit
* ����  : �ṹ���ʼ��
* ����  : cs - TransceiverоƬ����  
* ���  : ��
* ����  : ��
*******************************************************************************/
void TRCTRL_InstTransceiverTestStructInit(TransceiverTypeEnum cs)
{
    if (cs == TransceiverTypeA)
    {
        TRCTRL_TransceiverTestStructInit(&TransceiverTest1);
    }
    else if (cs == TransceiverTypeB)
    {
        TRCTRL_TransceiverTestStructInit(&TransceiverTest2);
    }            
}

/*******************************************************************************
* ����  : TRCTRL_TxTestSignalInit
* ����  : ��ʼ��SPIRIT��������ź�
* ����  : pTransceiverTestStruct - TransceiverTestStruct�ṹ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void TRCTRL_TxTestSignalInit(TransceiverTestStruct *pTransceiverTestStruct)
{
    U8 *pBuff;
   
    if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_NUM1)
    {
        pBuff = (U8 *)malloc((ST_SIGNAL_LEN + 2) * sizeof(U8));
        memset((U8 *)pBuff, 0, (ST_SIGNAL_LEN + 2) * sizeof(U8));
        pBuff[0] = ST_SIGNAL_LEN;
        pBuff[1] = 0;
        memcpy(&pBuff[2], &StSignal1[0], ST_SIGNAL_LEN);                
        DLS_FrameBuildTest(&pBuff[0], pTransceiverTestStruct->TxBuffer);
        free((U8 *)pBuff);

        pTransceiverTestStruct->TimeoutTx = TRCTRL_TEST_TX_TIMEOUT_TX_1;            //lq ��ͬ���źŵķ������ںͷ���ʱ�����Էֱ����
        pTransceiverTestStruct->TimeoutHalt = TRCTRL_TEST_TX_TIMEOUT_HALT_1;                            
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_NUM2)
    {
        pBuff = (U8 *)malloc((ST_SIGNAL_LEN + 2) * sizeof(U8));
        memset((U8 *)pBuff, 0, (ST_SIGNAL_LEN + 2) * sizeof(U8));
        pBuff[0] = ST_SIGNAL_LEN;
        pBuff[1] = 0;
        memcpy(&pBuff[2], &StSignal2[0], ST_SIGNAL_LEN);                
        DLS_FrameBuildTest(&pBuff[0], pTransceiverTestStruct->TxBuffer);
        free((U8 *)pBuff);

        pTransceiverTestStruct->TimeoutTx = TRCTRL_TEST_TX_TIMEOUT_TX_2;
        pTransceiverTestStruct->TimeoutHalt = TRCTRL_TEST_TX_TIMEOUT_HALT_2;                             
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_NUM3)
    {
        pBuff = (U8 *)malloc((ST_SIGNAL_LEN + 2) * sizeof(U8));
        memset((U8 *)pBuff, 0, (ST_SIGNAL_LEN + 2) * sizeof(U8));
        pBuff[0] = ST_SIGNAL_LEN;
        pBuff[1] = 0;
        memcpy(&pBuff[2], &StSignal3[0], ST_SIGNAL_LEN);                
        DLS_FrameBuildTest(&pBuff[0], pTransceiverTestStruct->TxBuffer);
        free((U8 *)pBuff);

        pTransceiverTestStruct->TimeoutTx = TRCTRL_TEST_TX_TIMEOUT_TX_3;
        pTransceiverTestStruct->TimeoutHalt = TRCTRL_TEST_TX_TIMEOUT_HALT_3;                             
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_CW)
    {
        memset((U8 *)(pTransceiverTestStruct->TxBuffer), 0, (TRCTRL_TEST_TX_BUFFER_LEN) * sizeof(U8));        
        pTransceiverTestStruct->TimeoutTx = TRCTRL_TEST_TX_TIMEOUT_TX_4;
        pTransceiverTestStruct->TimeoutHalt = TRCTRL_TEST_TX_TIMEOUT_HALT_4;                             
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_PN9)
    {
        memset((U8 *)(pTransceiverTestStruct->TxBuffer), 0, (TRCTRL_TEST_TX_BUFFER_LEN) * sizeof(U8));        
        pTransceiverTestStruct->TimeoutTx = TRCTRL_TEST_TX_TIMEOUT_TX_5;
        pTransceiverTestStruct->TimeoutHalt = TRCTRL_TEST_TX_TIMEOUT_HALT_5;                             
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_ALL0)
    {
        memset((U8 *)(pTransceiverTestStruct->TxBuffer), 0, (TRCTRL_TEST_TX_BUFFER_LEN) * sizeof(U8));    //lq all '0'        
        pTransceiverTestStruct->TxBuffer[0] = TRCTRL_TEST_TX_BUFFER_LEN - 2;
        pTransceiverTestStruct->TxBuffer[1] = 0;
        pTransceiverTestStruct->TimeoutTx = TRCTRL_TEST_TX_TIMEOUT_TX_6;            //lq ��ͬ���źŵķ������ںͷ���ʱ�����Էֱ����
        pTransceiverTestStruct->TimeoutHalt = TRCTRL_TEST_TX_TIMEOUT_HALT_6;                            
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_ALL1)
    {
        memset((U8 *)(pTransceiverTestStruct->TxBuffer), 0xFF, (TRCTRL_TEST_TX_BUFFER_LEN) * sizeof(U8)); //lq all '1'        
        pTransceiverTestStruct->TxBuffer[0] = TRCTRL_TEST_TX_BUFFER_LEN - 2;
        pTransceiverTestStruct->TxBuffer[1] = 0;
        pTransceiverTestStruct->TimeoutTx = TRCTRL_TEST_TX_TIMEOUT_TX_7;
        pTransceiverTestStruct->TimeoutHalt = TRCTRL_TEST_TX_TIMEOUT_HALT_7;                             
    }
    pTransceiverTestStruct->TxBufferIndex = 2;
    pTransceiverTestStruct->TxBufferOffset = 0;
}

/*******************************************************************************
* ����  : TRCTRL_TxTestInitSignal
* ����  : ��ʼ��SPIRIT��������ź�
* ����  : pTransceiverTestStruct - TransceiverTestStruct�ṹ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void TRCTRL_TxTestCfgInit(TransceiverTestStruct *pTransceiverTestStruct,
                                 SI446XChipEnum cs)
{
    if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_NUM1)
    {
        SI446X_TxCfg(cs, pTransceiverTestStruct->TxChannel);
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_NUM2)
    {
        SI446X_TxCfg(cs, pTransceiverTestStruct->TxChannel);
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_NUM3)
    {
        SI446X_TxCfg(cs, pTransceiverTestStruct->TxChannel);
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_CW)
    {
        SI446X_CfgCwTx(cs, pTransceiverTestStruct->TxChannel);
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_PN9)
    {
        SI446X_CfgPN9Tx(cs, pTransceiverTestStruct->TxChannel);                            
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_ALL0)
    {
        SI446X_TxCfg(cs, pTransceiverTestStruct->TxChannel);
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_ALL1)
    {
        SI446X_TxCfg(cs, pTransceiverTestStruct->TxChannel);
    }
}

/*******************************************************************************
* ����  : TRCTRL_TxTestInit
* ����  : SPIRIT������Գ�ʼ��
* ����  : cs - TransceiverоƬ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void TRCTRL_TxTestInit(TransceiverTypeEnum cs)
{
    TransceiverTestStruct *pTransceiverTestStruct;
    SI446XChipEnum SI446X_cs;

    /*lq ѡ��ʵ�� */
    if (cs == TransceiverTypeA)
    {
        pTransceiverTestStruct = &TransceiverTest1;
        SI446X_cs = SI446X_CHIP_A;
    }
    else if (cs == TransceiverTypeB)
    {
        pTransceiverTestStruct = &TransceiverTest2;    
        SI446X_cs = SI446X_CHIP_B;
    }
    else
    {
        return ;
    }

    /*lq оƬ���ó�ʼ�� */
    TRCTRL_TxTestCfgInit(pTransceiverTestStruct, SI446X_cs);

    /*lq �����źų�ʼ�� */
    TRCTRL_TxTestSignalInit(pTransceiverTestStruct);
}

/*******************************************************************************
* ����  : TRCTRL_TxTestResetSignal
* ����  : ��λSPIRIT��������ź�
* ����  : pTransceiverTestStruct - TransceiverTestStruct�ṹ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void TRCTRL_TxTestResetSignal(TransceiverTestStruct *pTransceiverTestStruct)
{
    pTransceiverTestStruct->TxBufferIndex = 2;
    pTransceiverTestStruct->TxBufferOffset = 0;     
}

/*******************************************************************************
* ����  : TRCTRL_TxTestGetSignalBit
* ����  : ��ȡ�����ź�λ
* ����  : pTransceiverTestStruct - TransceiverTestStruct�ṹ��	  
* ���  : ��
* ����  : �����ź�λ
*******************************************************************************/
static U8 TRCTRL_TxTestGetSignalBit(TransceiverTestStruct *pTransceiverTestStruct)
{
    U8 retValue;
    
    if ((pTransceiverTestStruct->TxBuffer[pTransceiverTestStruct->TxBufferIndex]
         & (1u << pTransceiverTestStruct->TxBufferOffset)) == 0)
    {
        retValue = 0;
    }    
    else
    {
        retValue = 1;
    }

    pTransceiverTestStruct->TxBufferOffset++;
    if (pTransceiverTestStruct->TxBufferOffset >= 8)
    {
        pTransceiverTestStruct->TxBufferOffset = 0;
        pTransceiverTestStruct->TxBufferIndex++;
        if (pTransceiverTestStruct->TxBufferIndex >= TRCTRL_TEST_TX_BUFFER_LEN)   //lq ����ж�
        {
            pTransceiverTestStruct->TxBufferIndex = 2;
        } 
    }

    /*lq ���ݽ����ж� */
    if ((pTransceiverTestStruct->TxBufferIndex >= (pTransceiverTestStruct->TxBuffer[0] + 2))
        && (pTransceiverTestStruct->TxBufferOffset >= pTransceiverTestStruct->TxBuffer[1]))
    {
        pTransceiverTestStruct->TxBufferIndex = 2;
        pTransceiverTestStruct->TxBufferOffset = 0;
    }

    return retValue;
}

/*******************************************************************************
* ����  : TRCTRL_TxGetSignalBit
* ����  : ��ȡ�����ź�λ
* ����  : pTransceiverTxCtrlStruct - TransceiverTxCtrlStruct�ṹ��	  
* ���  : ��
* ����  : �����ź�λ
*******************************************************************************/
static U8 TRCTRL_TxGetSignalBit(TransceiverTxCtrlStruct *pTransceiverTxCtrlStruct)
{
    U8 retValue;

    if (pTransceiverTxCtrlStruct->TxFlg == TRUE)
    {
        if ((pTransceiverTxCtrlStruct->TxBuffer[pTransceiverTxCtrlStruct->TxBufferIndex]
             & (1u << pTransceiverTxCtrlStruct->TxBufferOffset)) == 0)
        {
            retValue = 0;
        }    
        else
        {
            retValue = 1;
        }
    
        pTransceiverTxCtrlStruct->TxBufferOffset++;
        if (pTransceiverTxCtrlStruct->TxBufferOffset >= 8)
        {
            pTransceiverTxCtrlStruct->TxBufferOffset = 0;
            pTransceiverTxCtrlStruct->TxBufferIndex++;
            if (pTransceiverTxCtrlStruct->TxBufferIndex >= TRCTRL_TEST_TX_BUFFER_LEN)   //lq ����ж�
            {
                pTransceiverTxCtrlStruct->TxBufferIndex = 2;
                
                pTransceiverTxCtrlStruct->TxFlg = FALSE;
            } 
        }
    
        /*lq ���ݽ����ж� */
        if ((pTransceiverTxCtrlStruct->TxBufferIndex >= (pTransceiverTxCtrlStruct->TxBuffer[0] + 2))
            && (pTransceiverTxCtrlStruct->TxBufferOffset >= pTransceiverTxCtrlStruct->TxBuffer[1]))
        {
            pTransceiverTxCtrlStruct->TxBufferIndex = 2;
            pTransceiverTxCtrlStruct->TxBufferOffset = 0;
            
            pTransceiverTxCtrlStruct->TxFlg = FALSE;
        }    
    }
    else
    {
        retValue = 0;
    } 
       
    return retValue;    
}

/*******************************************************************************
* ����  : TRCTRL_TxSetTxDoneFlg
* ����  : ���÷���״̬�·�����ɱ�־
* ����  : pTransceiverTxCtrlStruct - TransceiverTxCtrlStruct�ṹ��
          flag - ��־״̬	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void TRCTRL_TxSetTxDoneFlg(TransceiverTxCtrlStruct *pTransceiverTxCtrlStruct,
                                  U8 flag)
{
    pTransceiverTxCtrlStruct->TxDoneFlg = flag;
}

/*******************************************************************************
* ����  : TRCTRL_InstTxSetTxDoneFlg
* ����  : ���÷���״̬�·�����ɱ�־
* ����  : flag - ��־״̬	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void TRCTRL_InstTxSetTxDoneFlg(U8 flag)
{
    TRCTRL_TxSetTxDoneFlg(&TransceiverTxCtrlInstance, flag);
}

/*******************************************************************************
* ����  : TRCTRL_TxGetTxFlg
* ����  : ����״̬�»�ȡ�����־
* ����  : pTransceiverTxCtrlStruct - TransceiverTxCtrlStruct�ṹ��	  
* ���  : ��
* ����  : �����־
*******************************************************************************/
static U8 TRCTRL_TxGetTxFlg(TransceiverTxCtrlStruct *pTransceiverTxCtrlStruct)
{
    return pTransceiverTxCtrlStruct->TxFlg;
}

/*******************************************************************************
* ����  : TRCTRL_InstTxGetTxFlg
* ����  : ����״̬�»�ȡ�����־
* ����  : pTransceiverTxCtrlStruct - TransceiverTxCtrlStruct�ṹ��	  
* ���  : ��
* ����  : �����־
*******************************************************************************/
U8 TRCTRL_InstTxGetTxFlg(void)
{
    return TRCTRL_TxGetTxFlg(&TransceiverTxCtrlInstance);
}

/*******************************************************************************
* ����  : TRCTRL_InstTxTestGetSignalBit
* ����  : ��ȡ�����ź�λ
* ����  : cs - TransceiverоƬ����	  
* ���  : ��
* ����  : �����ź�λ
*******************************************************************************/
U8 TRCTRL_InstTxTestGetSignalBit(TransceiverTypeEnum cs)
{
    if (cs == TransceiverTypeA)
    {
        return TRCTRL_TxTestGetSignalBit(&TransceiverTest1);
    }
    else if (cs == TransceiverTypeB)
    {
        return TRCTRL_TxTestGetSignalBit(&TransceiverTest2);
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
* ����  : TRCTRL_InstTxTestGetSignalBit
* ����  : ��ȡ�����ź�λ
* ����  : cs - TransceiverоƬ����	  
* ���  : ��
* ����  : �����ź�λ
*******************************************************************************/
U8 TRCTRL_InstTxGetSignalBit(void)
{
    return TRCTRL_TxGetSignalBit(&TransceiverTxCtrlInstance);
}

/*******************************************************************************
* ����  : TRCTRL_GetTxTestFlg
* ����  : �������״̬
* ����  : pTransceiverTestStruct - TransceiverTestStruct�ṹ��	  
* ���  : ��
* ����  : �������״̬
*******************************************************************************/
static U8 TRCTRL_GetTxTestFlg(TransceiverTestStruct *pTransceiverTestStruct)
{
    if (pTransceiverTestStruct->TxTestFlg == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : TRCTRL_InstGetTxTestFlg
* ����  : �������״̬
* ����  : cs - TransceiverоƬ����	  
* ���  : ��
* ����  : �������״̬
*******************************************************************************/
U8 TRCTRL_InstGetTxTestFlg(TransceiverTypeEnum cs)
{
    if (cs == TransceiverTypeA)
    {
        return TRCTRL_GetTxTestFlg(&TransceiverTest1);
    }
    else if (cs == TransceiverTypeB)
    {
        return TRCTRL_GetTxTestFlg(&TransceiverTest2);
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : TRCTRL_SetTxTestFlg
* ����  : ��λ�������״̬
* ����  : pTransceiverTestStruct - TransceiverTestStruct�ṹ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void TRCTRL_SetTxTestFlg(TransceiverTestStruct *pTransceiverTestStruct)
{
    pTransceiverTestStruct->TxTestFlg = TRUE;
}

/*******************************************************************************
* ����  : TRCTRL_InstSetTxTestFlg
* ����  : ��λ�������״̬
* ����  : cs - TransceiverоƬ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void TRCTRL_InstSetTxTestFlg(TransceiverTypeEnum cs)
{
    if (cs == TransceiverTypeA)
    {
        TRCTRL_SetTxTestFlg(&TransceiverTest1);
    }
    else if (cs == TransceiverTypeB)
    {
        TRCTRL_SetTxTestFlg(&TransceiverTest2);
    }
}

/*******************************************************************************
* ����  : TRCTRL_TxTestSetTxSignalType
* ����  : ���÷����ź�����
* ����  : pTransceiverTestStruct - TransceiverTestStruct�ṹ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void TRCTRL_TxTestSetTxSignalType(TransceiverTestStruct *pTransceiverTestStruct, U8 type)
{
    pTransceiverTestStruct->TxSignalType = type;
}

/*******************************************************************************
* ����  : TRCTRL_InstTxTestSetTxSignalType
* ����  : ���÷����ź�����
* ����  : cs - TransceiverоƬ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void TRCTRL_InstTxTestSetTxSignalType(TransceiverTypeEnum cs, U8 type)
{
    if (cs == TransceiverTypeA)
    {
        TRCTRL_TxTestSetTxSignalType(&TransceiverTest1, type);
    }
    else if (cs == TransceiverTypeB)
    {
        TRCTRL_TxTestSetTxSignalType(&TransceiverTest2, type);
    }
}

/*******************************************************************************
* ����  : TRCTRL_TxTestSetTxChannel
* ����  : ���÷����ŵ�
* ����  : pTransceiverTestStruct - TransceiverTestStruct�ṹ��
          channel - �ŵ�	  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void TRCTRL_TxTestSetTxChannel(TransceiverTestStruct *pTransceiverTestStruct,
                                      AisChannelEnum channel)
{
    pTransceiverTestStruct->TxChannel = channel;
}

/*******************************************************************************
* ����  : TRCTRL_InstTxTestSetTxChannel
* ����  : ���÷����ŵ�
* ����  : cs - TransceiverоƬ����
          channel - �ŵ�	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void TRCTRL_InstTxTestSetTxChannel(TransceiverTypeEnum cs, AisChannelEnum channel)
{
    if (cs == TransceiverTypeA)
    {
        TRCTRL_TxTestSetTxChannel(&TransceiverTest1, channel);
    }
    else if (cs == TransceiverTypeB)
    {
        TRCTRL_TxTestSetTxChannel(&TransceiverTest2, channel);
    }
}

/*******************************************************************************
* ����  : TRCTRL_TxTestCtrl
* ����  : ������Կ���
* ����  : cs - TransceiverоƬ����	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void TRCTRL_TxTestCtrl(TransceiverTypeEnum cs)
{
    TransceiverTestStruct *pTransceiverTestStruct;

    /*lq ѡ��ʵ�� */
    if (cs == TransceiverTypeA)
    {
        pTransceiverTestStruct = &TransceiverTest1;
    }
    else if (cs == TransceiverTypeB)
    {
        pTransceiverTestStruct = &TransceiverTest2;    
    }
    else
    {
        return ;
    }

	switch (pTransceiverTestStruct->TxCtrlSta)
    {
        case TRCTRL_TEST_TX_CTRL_STA_IDLE:
            
            if (pTransceiverTestStruct->TxTestFlg == TRUE)
            {
                /*lq GPIO���ݷ������� */
                TRCTRL_TxTestInit(cs);
                RF_PowerOn();	    
                pTransceiverTestStruct->TimeCnt = SetTimePeriodProcess(pTransceiverTestStruct->TimePeriod);
                pTransceiverTestStruct->TxCtrlSta = TRCTRL_TEST_TX_CTRL_STA_TX;            
            }

            break;
        case TRCTRL_TEST_TX_CTRL_STA_TX:
            
            if (pTransceiverTestStruct->TimeCnt == GetLocalClockTickProcess())
            {
                pTransceiverTestStruct->TimeCnt = SetTimePeriodProcess(pTransceiverTestStruct->TimePeriod);                
                
                if (pTransceiverTestStruct->TimeoutTx != 0)     //lq ��ʱΪ0��ʾ�������������
                {
                    pTransceiverTestStruct->TimeoutCntTx++;
                    if (pTransceiverTestStruct->TimeoutCntTx >= pTransceiverTestStruct->TimeoutTx)
                    {
                        pTransceiverTestStruct->TimeoutCntTx = 0;
    
                        /*lq �˳��շ�״̬ */
    	                RF_PowerDown();	    
                        SI446X_StateTo(SI446X_MODE_IDLE, SI446X_CHIP_A);
                        pTransceiverTestStruct->TxCtrlSta = TRCTRL_TEST_TX_CTRL_STA_HALT;                 
                    }                 
                }
            }
            
            break;
        case TRCTRL_TEST_TX_CTRL_STA_HALT:

            if (pTransceiverTestStruct->TimeCnt == GetLocalClockTickProcess())
            {
                pTransceiverTestStruct->TimeCnt = SetTimePeriodProcess(pTransceiverTestStruct->TimePeriod);                
                pTransceiverTestStruct->TimeoutCntHalt++;
                if (pTransceiverTestStruct->TimeoutCntHalt >= pTransceiverTestStruct->TimeoutHalt)
                {
                    pTransceiverTestStruct->TimeoutCntHalt = 0;

                    /*lq ����TX̬ */
                    SI446X_StateTo(SI446X_MODE_TX, SI446X_CHIP_A);
    	            RF_PowerOn();	    
                    TRCTRL_TxTestResetSignal(pTransceiverTestStruct);                                        
                    pTransceiverTestStruct->TxCtrlSta = TRCTRL_TEST_TX_CTRL_STA_TX;                     
                }
            }
                        
            break;
        default :

            pTransceiverTestStruct->TxCtrlSta = TRCTRL_TEST_TX_CTRL_STA_IDLE;
            break;
    }
     
}

/*******************************************************************************
* ����  : TRCTRL_InstTxTestCtrl
* ����  : ������Կ���
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void TRCTRL_InstTxTestCtrl(void)
{
    TRCTRL_TxTestCtrl(TransceiverTypeA);    
    TRCTRL_TxTestCtrl(TransceiverTypeB); 
}

/*******************************************************************************
* ����  : TRCTRL_TxTestIsTx
* ����  : �ж��Ƿ�������
* ����  : pTransceiverTestStruct - TransceiverTestStruct�ṹ��	  
* ���  : ��
* ����  : TRUE - ���䣬 FALSE - ������
*******************************************************************************/
static U8 TRCTRL_TxTestIsTx(TransceiverTestStruct *pTransceiverTestStruct)
{
    if (pTransceiverTestStruct->TxCtrlSta == TRCTRL_TEST_TX_CTRL_STA_TX)
    {
        return TRUE;
    } 
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : TRCTRL_TxTestIsTx
* ����  : �ж��Ƿ�������
* ����  : cs - TransceiverоƬ����	  
* ���  : ��
* ����  : TRUE - ���䣬 FALSE - ������
*******************************************************************************/
U8 TRCTRL_InstTxTestIsTx(TransceiverTypeEnum cs)
{
    if (cs == TransceiverTypeA)
    {
        return TRCTRL_TxTestIsTx(&TransceiverTest1);
    }
    else if (cs == TransceiverTypeB)
    {
        return TRCTRL_TxTestIsTx(&TransceiverTest2);
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : TRCTRL_TransmitProcess
* ����  : �������
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void TRCTRL_TransmitProcess(void)
{
    FrameTxCellStruct *pFrameTxCell;    
    TransceiverTxCtrlStruct *pTransceiverTxCtrlStruct;
    S32 currFrame;
    S16 currSlot;
//    S16 currPhase;
    S16 slotDiff;
    U8  txSlotValidFlg = FALSE;
    U8  txDoneFlg = FALSE;

    pTransceiverTxCtrlStruct = &TransceiverTxCtrlInstance;

    currFrame = GetFrameNum();
    currSlot = GetSlotNum();
//    currPhase = GetPhaseNum();

    switch (pTransceiverTxCtrlStruct->TxCtrlState)
    {
        case TRCTRL_TX_CTRL_STATE_IDLE:

            pFrameTxCell = (FrameTxCellStruct *)malloc(sizeof(FrameTxCellStruct));
            memset((FrameTxCellStruct *)pFrameTxCell, 0,
                sizeof(FrameTxCellStruct));

            /*lq ֡���Ͷ��зǿ� */
            if (DLS_InstFrameTxFifoDeque(pFrameTxCell) == TRUE)
            {
                pTransceiverTxCtrlStruct->TxSlot = pFrameTxCell->TxSlot;
                pTransceiverTxCtrlStruct->TxChannel = pFrameTxCell->TxChannel;
                
                memcpy(&pTransceiverTxCtrlStruct->TxBuffer[0],
                    &pFrameTxCell->Data[0], pFrameTxCell->Data[0] + 3);  //lq ǰ2�ֽ�+���ֽ���+�����ֽ�                                
                pTransceiverTxCtrlStruct->TxBufferIndex = 2;
                pTransceiverTxCtrlStruct->TxBufferOffset = 0;
                pTransceiverTxCtrlStruct->TxFlg = FALSE;
                pTransceiverTxCtrlStruct->TxDoneFlg = FALSE;
                                                   
                pTransceiverTxCtrlStruct->TxCtrlState =
                    TRCTRL_TX_CTRL_STATE_TX_INIT;
                
                debug_printf("$[%6d:%4d] %s : Ctrl state to TX_INIT!\r\n",
                    currFrame, currSlot, __func__);
            } 

            free((FrameTxCellStruct *)pFrameTxCell);
                    
            break;    

        case TRCTRL_TX_CTRL_STATE_TX_INIT:

            txSlotValidFlg = TRUE;
            if (pTransceiverTxCtrlStruct->TxSlot != SLOT_FORCE_TX_VALUE)
            {
                slotDiff = (pTransceiverTxCtrlStruct->TxSlot - currSlot
                    + FRAME_SLOT_SIZE) % FRAME_SLOT_SIZE;
    
                /*lq ����ʱ϶�Ƿ���Ч�ж� */
                if (slotDiff > AIS_SLOT_INTERVAL_SEND_TO_TX)
                {
                    /*lq ʱ϶�ѹ� */
                    txSlotValidFlg = FALSE;
                }                        
            }

            if (txSlotValidFlg == TRUE)
            {
                /*lq �շ�ת�� */
                RxTxCtrl(RXTX_CTRL_TYPE_TX, 0, 0);            
                
                /*lq Transceiver�������� */
                pTransceiverTxCtrlStruct->TxDataCtrlState = 
                    TRCTRL_TX_DATA_CTRL_STATE_IDLE;     //lq �����շ�����������ǰ�����ÿ���״̬
                pTransceiverTxCtrlStruct->TxFlg = TRUE;
                SI446X_TxCfg(SI446X_CHIP_A, pTransceiverTxCtrlStruct->TxChannel);                     
    
        		/*lq ���÷��䳬ʱ��ʱ */
                pTransceiverTxCtrlStruct->TimeCnt = SetTimePeriodProcess(
                    pTransceiverTxCtrlStruct->TimePeriod);

			    /*lq ��Ϣ����LEDָʾ */
                LEDInit(LED_TYPE_RF_XMT);
    
                pTransceiverTxCtrlStruct->TxCtrlState = TRCTRL_TX_CTRL_STATE_TX_RUNNING;
                debug_printf("$[%6d:%4d] %s : Ctrl state to TX_RUNNING!\r\n",
                    currFrame, currSlot, __func__);            
            }
            else
            {
                pTransceiverTxCtrlStruct->TxCtrlState = TRCTRL_TX_CTRL_STATE_IDLE;
				if(DevDynamicPara.TestSta == EQ_MOD_TEST_STA_SELF_CHECK)
				{
					UartResponseMsg("A is not OK!");
				}
                debug_printf("$[%6d:%4d] %s : Ctrl state to TX_IDLE, Tx slot [%4d] has expired!\r\n",
                    currFrame, currSlot, __func__, pTransceiverTxCtrlStruct->TxSlot);            
            
            }

            break;

        case TRCTRL_TX_CTRL_STATE_TX_RUNNING:

            if (pTransceiverTxCtrlStruct->TxDoneFlg == TRUE)
            {            
                txDoneFlg = TRUE;
				if(DevDynamicPara.TestSta == EQ_MOD_TEST_STA_SELF_CHECK)
				{
					UartResponseMsg("A is OK!");
				}
            }
            else
            {
                /*lq ���䳬ʱ */
                if (pTransceiverTxCtrlStruct->TimeCnt == GetLocalClockTickProcess())
            	{
                    txDoneFlg = TRUE;

    			    /*lq ��ʱLEDָʾ */
                    LEDInit(LED_TYPE_TIMEOUT);
    
					if(DevDynamicPara.TestSta == EQ_MOD_TEST_STA_SELF_CHECK)
					{
						UartResponseMsg("A is not OK!");
					}
                    debug_printf("$[%6d:%4d] %s : Transceiver TX timeout!\r\n",
                        currFrame, currSlot, __func__);
            	}
            }

            /*lq ������ɴ��� */
            if (txDoneFlg == TRUE)
            {
                /*lq ������� */
                pTransceiverTxCtrlStruct->TxDoneFlg = FALSE;

                /*lq Transceiver�������� */
                SI446X_RxCfg(SI446X_CHIP_A, ENUM_AIS_CHANNEL_A);

                /*lq �շ�ת�� */
                RxTxCtrl(RXTX_CTRL_TYPE_RX, 0, 0);                                                    

			    /*lq ��ʱLEDָʾ��� */
                LEDInit(LED_TYPE_YELLOW_NONE);

                pTransceiverTxCtrlStruct->TxCtrlState = TRCTRL_TX_CTRL_STATE_IDLE;

                debug_printf("$[%6d:%4d] %s : Ctrl state to IDLE!\r\n",
                    currFrame, currSlot, __func__);
            }

            break;

        default :
            
            /*lq Ĭ�Ͻ��� */
            /*lq Transceiver�������� */
            SI446X_RxCfg(SI446X_CHIP_A, ENUM_AIS_CHANNEL_A);
            RxTxCtrl(RXTX_CTRL_TYPE_RX, 0, 0);
            pTransceiverTxCtrlStruct->TxCtrlState = TRCTRL_TX_CTRL_STATE_IDLE;                                                
            debug_printf("$[%6d:%4d] %s : Ctrl state to IDLE from default case!\r\n",
                currFrame, currSlot, __func__);
            
            break;
    }     
}

/*******************************************************************************
* ����  : TRCTRL_TxDataClockISR
* ����  : ����ģʽ��������ʱ���жϷ�������
* ����  : transceiverType - �շ�������	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void TRCTRL_TxDataClockISR(TransceiverTypeEnum transceiverType)
{
    TransceiverTxCtrlStruct *pTransceiverTxCtrlStruct;
    S16 currSlot;
    S16 currPhase;
    S16 slotDiff;
    U8  txDataEn = FALSE;

    pTransceiverTxCtrlStruct = &TransceiverTxCtrlInstance;

    currSlot = GetSlotNum();
    currPhase = GetPhaseNum();
    
    switch (pTransceiverTxCtrlStruct->TxDataCtrlState)
    {
        case TRCTRL_TX_DATA_CTRL_STATE_IDLE :

            /*lq ����ʹ���ж� */
            if (TRCTRL_TxGetTxFlg(pTransceiverTxCtrlStruct) == TRUE)
            {
                pTransceiverTxCtrlStruct->TxDataCtrlState = 
                    TRCTRL_TX_DATA_CTRL_STATE_START;
            }

            break;

        case TRCTRL_TX_DATA_CTRL_STATE_START :

            pTransceiverTxCtrlStruct->TxDataCtrlState = 
                TRCTRL_TX_DATA_CTRL_STATE_READY;

            if (pTransceiverTxCtrlStruct->TxSlot != SLOT_FORCE_TX_VALUE)
            {
                slotDiff = (pTransceiverTxCtrlStruct->TxSlot - currSlot
                    + FRAME_SLOT_SIZE) % FRAME_SLOT_SIZE;
    
                /*lq ����ʱ϶�Ƿ���Ч�ж� */
                if (slotDiff > AIS_SLOT_INTERVAL_SEND_TO_TX)
                {
                    /*lq ʱ϶�ѹ� */
                    TRCTRL_TxSetTxDoneFlg(pTransceiverTxCtrlStruct, TRUE);

                    pTransceiverTxCtrlStruct->TxDataCtrlState = 
                        TRCTRL_TX_DATA_CTRL_STATE_IDLE;
                }                        
            }

            break;
    
        case TRCTRL_TX_DATA_CTRL_STATE_READY :

            /*lq ����ʱ϶�ж� */
            if (pTransceiverTxCtrlStruct->TxSlot == SLOT_FORCE_TX_VALUE)
            {
                /*lq ������λ�ж� */
                if ((currPhase >= AIS_PHASE_INTERVAL_DATA_START)
                    && (currPhase <= AIS_PHASE_INTERVAL_DATA_START
                    + AIS_BIT_PHASE_SIZE))      //lq 1bit = 5phase������ʱ����ʱ϶��ʼ��λ���ܲ�ͬ������������Ƚ�
                {
                    txDataEn = TRUE;
                }                        
            }
            else if (currSlot == pTransceiverTxCtrlStruct->TxSlot)
            {
                /*lq ������λ�ж� */
                if (currPhase >= AIS_PHASE_INTERVAL_DATA_START)
                {
                    txDataEn = TRUE;
                }
            }

            if (txDataEn == TRUE)
            {
                /*lq ���ſ��� */    
                RF_PowerOn();	    

                pTransceiverTxCtrlStruct->TxDataCtrlState = 
                    TRCTRL_TX_DATA_CTRL_STATE_DATA;                        
            }

            break;

        case TRCTRL_TX_DATA_CTRL_STATE_DATA :

            if (TRCTRL_TxGetTxFlg(pTransceiverTxCtrlStruct) == TRUE)
            {
                if (TRCTRL_TxGetSignalBit(pTransceiverTxCtrlStruct) == 0)
                {
                    if (transceiverType == TransceiverTypeA)
                    {
                        SI446X_A_DATA_LOW(); 
                    }
                    else
                    {
                        SI446X_B_DATA_LOW(); 
                    }
                }
                else
                {
                    if (transceiverType == TransceiverTypeA)
                    {
                        SI446X_A_DATA_HIGH();
                    }
                    else
                    {
                        SI446X_B_DATA_HIGH();
                    }
                }

                /*lq ���ݷ�������ж� */
                if (TRCTRL_TxGetTxFlg(pTransceiverTxCtrlStruct) == FALSE)
                {
                    pTransceiverTxCtrlStruct->TxDataEndBitCnt = 0;

                    pTransceiverTxCtrlStruct->TxDataCtrlState = 
                        TRCTRL_TX_DATA_CTRL_STATE_END;
                }
            }

            break;

        case TRCTRL_TX_DATA_CTRL_STATE_END :

            /*lq ���ݷ�����ɼ��� */
            pTransceiverTxCtrlStruct->TxDataEndBitCnt++;
            if (pTransceiverTxCtrlStruct->TxDataEndBitCnt
                >= AIS_BIT_TIME_DATA_END)
            {
                pTransceiverTxCtrlStruct->TxDataEndBitCnt = 0;

                /*lq ���Źر� */
    	        RF_PowerDown();	                                
                
                TRCTRL_TxSetTxDoneFlg(pTransceiverTxCtrlStruct, TRUE);

                pTransceiverTxCtrlStruct->TxDataCtrlState = 
                    TRCTRL_TX_DATA_CTRL_STATE_IDLE;
            }

            break;
    }

}

/*******************************************************************************
* ����  : TRCTRL_ModuleInit
* ����  : ģ���ʼ��
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void TRCTRL_ModuleInit(void)
{
    TRCTRL_TransceiverTestStructInit(&TransceiverTest1);
    TRCTRL_TransceiverTestStructInit(&TransceiverTest2);
    TRCTRL_TransceiverTxCtrlStructInit(&TransceiverTxCtrlInstance);
}

