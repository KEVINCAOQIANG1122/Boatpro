/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: CMX7042.c
  Author: lq       Version : V1.00        Date: 2014.05.12
  Description:     CMX7042����
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "stm32f10x_lib.h"
#include "def.h"
#include "CMX7042.h"
#include "SPI.h"
#include "wdg.h"

#if defined(CMX7042_FI_VER_1_2_4_0)
#include "7032-1.2.4.0.h"

#elif defined(CMX7042_FI_VER_1_0)
#include "EF0402_M25P10A_10.h"

#endif

CMX7042Struct CMX7042Instance;

/*******************************************************************************
* ����  : CMX7042_InitStruct
* ����  : ��ʼ���ṹ��CMX7042Struct����
* ����  : pCMX7042Struct-�ṹ��ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static CMX7042_InitStruct(CMX7042Struct* pCMX7042Struct)
{
    pCMX7042Struct->ProductIdCode = 0;
    pCMX7042Struct->FIVersionCode = 0;
    pCMX7042Struct->ActivedFlg = FALSE;
    pCMX7042Struct->StatusReg = 0x0;
    pCMX7042Struct->Status2Reg = 0x0;
    pCMX7042Struct->Status2RegOld = 0x0;
    pCMX7042Struct->IntMaskReg = 0x0;
    pCMX7042Struct->Rx1ErrFlg = FALSE;
    pCMX7042Struct->Rx2ErrFlg = FALSE;
    pCMX7042Struct->IntFlg = FALSE;
	pCMX7042Struct->SlotCnt = 0;
	pCMX7042Struct->PhaseCnt = 0;
	pCMX7042Struct->SlotCalibFlg = FALSE;
}

/*******************************************************************************
* ����  : CMX7042_InitInstance
* ����  : ��ʼ���ṹ��CMX7042Structʵ��
* ����  : ��	  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_InitInstance(void)
{
    CMX7042_InitStruct(&CMX7042Instance);
    CMX7042_SetStatusBitFlg(CMX7042_STATUS_BITMASK_ComRegFree
                            | CMX7042_STATUS_BITMASK_DataTaskComp
                            | CMX7042_STATUS_BITMASK_CnfgTaskComp
                            | CMX7042_STATUS_BITMASK_TBFree);

}

/*******************************************************************************
* ����  : CMX7042_InitPins
* ����  : ��ʼ��CMX7042����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_InitPins(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

    /*lq ------------------CMX7042 VCC ������������---------------------------*/
    /* Configure CMX7042 VCC ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = CMX7042_VCC_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(CMX7042_VCC_PORT, &GPIO_InitStructure);
    CMX7042_VCC_HIGH();        

    /*lq ------------------CMX7042 SLOTCLK ��������---------------------------*/
    /* Configure CMX7042 SLOTCLK ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = CMX7042_SLOTCLK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(CMX7042_SLOTCLK_PORT, &GPIO_InitStructure);
	CMX7042_SLOTCLK_LOW();

    /*lq ------------------CMX7042 RSSI1/RSSI2 ��������-----------------------*/
    /* Configure CMX7042 RSSI1/RSSI2 ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = CMX7042_RSSI1_PIN | CMX7042_RSSI2_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(CMX7042_RSSI_PORT, &GPIO_InitStructure);
    CMX7042_RSSI1_LOW();        
    CMX7042_RSSI2_LOW();        

    /*lq ------------------CMX7042 IRQN ��������-----------------------*/
    /* Configure CMX7042 IRQN ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = CMX7042_IRQN_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
    GPIO_Init(CMX7042_IRQN_PORT, &GPIO_InitStructure); 
    
	/* Connect EXTI Line to GPIO Pin */
	GPIO_EXTILineConfig(CMX7042_IRQN_SOURCE_PORT, CMX7042_IRQN_SOURCE_PIN);
	
	/* Configure EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = CMX7042_IRQN_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 

//lqdb
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(GPIOC, &GPIO_InitStructure); 
//lq~    
    /*lq ------------------CMX7042 C-BUS interface ��������-----------------------*/
    SPI2_Init();
}

/*******************************************************************************
* ����  : CMX7042_Init
* ����  : ��ʼ��CMX7042
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_Init(void)
{
    CMX7042_InitInstance();
    CMX7042_InitPins();
}

#if 1
/*******************************************************************************
* ����  : CMX7042_IRQS
* ����  : CMX7042�жϷ������
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_IRQS(void)
{
    U16 status = 0;
    U16 status2 = 0;

	if(EXTI_GetITStatus(CMX7042_IRQN_LINE) != RESET)
	{
#if 1
        GPIO_SetBits(GPIOC, GPIO_Pin_0);
#endif
        CMX7042Instance.IntFlg = TRUE;
#if 0
		do
		{
	        /*lq ��״̬�Ĵ��� */
	        status = CMX7042_ReadWord(CMX7042_REGR_ADDR_Status);    //lq ״̬�Ĵ�����һ�κ����е�״̬λ��������
	        if ((status & CMX7042_STATUS_BITMASK_IRQ) != 0)         //lq ȷ���жϴ���
	        {
	            CMX7042Instance.StatusReg |= status;
	
	            /*lq �жϽ���״̬�Ƿ�ı� */
                if ((status & CMX7042_STATUS_BITMASK_RxState) != 0)
	            {
                    CMX7042Instance.Status2RegOld = CMX7042Instance.Status2Reg;
	                CMX7042Instance.Status2Reg = CMX7042_ReadWord(CMX7042_REGR_ADDR_Status2);

                    /*lq �ж�RX1�Ƿ���ֽ��մ��� */
                    if ((CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_TwoLong)
                        || (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_CRC)
                        || (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_NewFrameTwoBuffFull)
                        || (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_EndFlgNotByteBound))
                    {
                        CMX7042_SetRxError(CMX7042_RX_1);                        
                    }

                    /*lq �ж�RX2�Ƿ���ֽ��մ��� */
                    if ((CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_TwoLong)
                        || (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_CRC)
                        || (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_NewFrameTwoBuffFull)
                        || (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_EndFlgNotByteBound))
                    {
                        CMX7042_SetRxError(CMX7042_RX_2);                        
                    }
	            }            
	        }
            
            if ((status & CMX7042_STATUS_BITMASK_IRQ) == 0)
            {
                usart_printf("$CMX7042 IRQ pin low without IRQ: %04X!\r\n", status);        
            }                       
		} while (GPIO_ReadInputDataBit(CMX7042_IRQN_PORT, CMX7042_IRQN_PIN) == 0);	//lq ��ֹ���������ĺ����жϼ�ⲻ��
#endif
		/* Clear the EXTI line pending bit */  
		EXTI_ClearITPendingBit(CMX7042_IRQN_LINE);             
#if 1
        GPIO_ResetBits(GPIOC, GPIO_Pin_0);
#endif
    }
}
#else
/*******************************************************************************
* ����  : CMX7042_IRQS
* ����  : CMX7042�жϷ������
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_IRQS(void)
{
    U16 status = 0;
    U16 status2 = 0;

	if(EXTI_GetITStatus(CMX7042_IRQN_LINE) != RESET)
	{
#if 1
        GPIO_SetBits(GPIOC, GPIO_Pin_0);
#endif

		/* Clear the EXTI line pending bit */  
		EXTI_ClearITPendingBit(CMX7042_IRQN_LINE);

        /*lq ��״̬�Ĵ��� */
        status = CMX7042_ReadWord(CMX7042_REGR_ADDR_Status);    //lq ״̬�Ĵ�����һ�κ����е�״̬λ��������
        if ((status & CMX7042_STATUS_BITMASK_IRQ) != 0)         //lq ȷ���жϴ���
        {
            CMX7042Instance.StatusReg |= status;

            /*lq �жϽ���״̬�Ƿ�ı� */
            if ((status & CMX7042_STATUS_BITMASK_RxState) != 0)
            {
                CMX7042Instance.Status2RegOld = CMX7042Instance.Status2Reg;
                CMX7042Instance.Status2Reg = CMX7042_ReadWord(CMX7042_REGR_ADDR_Status2);

                /*lq �ж�RX1�Ƿ���ֽ��մ��� */
                if ((CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_TwoLong)
                    || (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_CRC)
                    || (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_NewFrameTwoBuffFull)
                    || (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_EndFlgNotByteBound))
                {
                    CMX7042_SetRxError(CMX7042_RX_1);                        
                }

                /*lq �ж�RX2�Ƿ���ֽ��մ��� */
                if ((CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_TwoLong)
                    || (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_CRC)
                    || (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_NewFrameTwoBuffFull)
                    || (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_EndFlgNotByteBound))
                {
                    CMX7042_SetRxError(CMX7042_RX_2);                        
                }
            }            
        }
        
        if ((status & CMX7042_STATUS_BITMASK_IRQ) == 0)
        {
            usart_printf("$CMX7042 IRQ pin low without IRQ: %04X!\r\n", status);        
        }                       
             
#if 1
        GPIO_ResetBits(GPIOC, GPIO_Pin_0);
#endif
    }
}
#endif

/*******************************************************************************
* ����  : CMX7042_GetStatusReg
* ����  : ��ѯCMX7042״̬�Ĵ���
* ����  : ��		  
* ���  : ��
* ����  : ��ǰ״̬
*******************************************************************************/
U16 CMX7042_GetStatusReg(void)
{
    return CMX7042Instance.StatusReg;
}

/*******************************************************************************
* ����  : CMX7042_GetStatus2Reg
* ����  : ��ѯCMX7042״̬2�Ĵ���
* ����  : ��		  
* ���  : ��
* ����  : ��ǰ״̬
*******************************************************************************/
U16 CMX7042_GetStatus2Reg(void)
{
    return CMX7042Instance.Status2Reg;
}

/*******************************************************************************
* ����  : CMX7042_SetStatusBitFlg
* ����  : ��λCMX7042״̬λ��־
* ����  : statusBit - ״̬λ��ʶ		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_SetStatusBitFlg(U16 statusBit)
{       
    CMX7042Instance.StatusReg |= statusBit;
}

/*******************************************************************************
* ����  : CMX7042_ResetStatusBitFlg
* ����  : ��λCMX7042״̬λ��־
* ����  : statusBit - ״̬λ��ʶ		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_ResetStatusBitFlg(U16 statusBit)
{       
    CMX7042Instance.StatusReg &= ~statusBit;
}

/*******************************************************************************
* ����  : CMX7042_GetStatusBitFlg
* ����  : ��ѯCMX7042״̬λ��־��״̬�Ĵ�����һ�κ����е�״̬λ��������
* ����  : statusBit - ״̬λ��ʶ		  
* ���  : ��
* ����  : ״̬λ��־�ĵ�ǰ״̬
*******************************************************************************/
U8 CMX7042_GetStatusBitFlg(U16 statusBit)
{           
    if ((CMX7042Instance.StatusReg & statusBit) != 0)
    {
        return TRUE;    
    }
    else
    {
        /*lq �ж��Ƿ�Ϊ�жϱ�־λ */        
        if ((CMX7042Instance.IntMaskReg & statusBit) != 0)
        {
            /*lq �жϱ�־λ��ѯ�����ж�ʱ�Ŷ�״̬�Ĵ��� */
            if (CMX7042_GetIntFlgRst() == TRUE)                    
            {
        	    CMX7042Instance.StatusReg |= CMX7042_ReadWord(CMX7042_REGR_ADDR_Status);    //lq ״̬�Ĵ�����һ�κ����е�״̬λ��������
                if ((CMX7042Instance.StatusReg & statusBit) != 0)
                {
                    return TRUE;    
                }
                else
                {
                    return FALSE;
                }             
            }
        }
        else
        {
            /*lq ���жϱ�־λ��ѯ��ֱ�Ӷ�״̬�Ĵ��� */
    	    CMX7042Instance.StatusReg |= CMX7042_ReadWord(CMX7042_REGR_ADDR_Status);    //lq ״̬�Ĵ�����һ�κ����е�״̬λ��������
            if ((CMX7042Instance.StatusReg & statusBit) != 0)
            {
                return TRUE;    
            }
            else
            {
                return FALSE;
            }        
        }
    }
}

/*******************************************************************************
* ����  : CMX7042_GetStatusBitFlg
* ����  : ��ѯCMX7042״̬λ��־�����Ѿ���λ���临λ
* ����  : statusBit - ״̬λ��ʶ		  
* ���  : ��
* ����  : ״̬λ��־�ĵ�ǰ״̬
*******************************************************************************/
U8 CMX7042_GetStatusBitFlgRst(U16 statusBit)
{           
    if ((CMX7042Instance.StatusReg & statusBit) != 0)
    {
        CMX7042_ResetStatusBitFlg(statusBit);        
        
        return TRUE;    
    }
    else
    {
        /*lq �ж��Ƿ�Ϊ�жϱ�־λ */        
        if ((CMX7042Instance.IntMaskReg & statusBit) != 0)
        {
            /*lq �жϱ�־λ��ѯ�����ж�ʱ�Ŷ�״̬�Ĵ��� */
            if (CMX7042_GetIntFlgRst() == TRUE)                    
            {
        	    CMX7042Instance.StatusReg |= CMX7042_ReadWord(CMX7042_REGR_ADDR_Status);    //lq ״̬�Ĵ�����һ�κ����е�״̬λ��������
                if ((CMX7042Instance.StatusReg & statusBit) != 0)
                {
                    CMX7042_ResetStatusBitFlg(statusBit);        

                    return TRUE;    
                }
                else
                {
                    return FALSE;
                }             
            }
        }
        else
        {
            /*lq ���жϱ�־λ��ѯ��ֱ�Ӷ�״̬�Ĵ��� */
    	    CMX7042Instance.StatusReg |= CMX7042_ReadWord(CMX7042_REGR_ADDR_Status);    //lq ״̬�Ĵ�����һ�κ����е�״̬λ��������
            if ((CMX7042Instance.StatusReg & statusBit) != 0)
            {
                CMX7042_ResetStatusBitFlg(statusBit);        

                return TRUE;    
            }
            else
            {
                return FALSE;
            }        
        }
    }
}

/*******************************************************************************
* ����  : CMX7042_SlotClkGenerator
* ����  : ����CMX7042��SlotClk���ú�����48k��ʱ�жϵ���
* ����  : slotNum - ��ǰʱ϶��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_SlotClkGenerator(S16 slotNum)
{
	if (CMX7042Instance.SlotCnt != slotNum)
	{
		/*lq ʱ϶���Ѹı� */
		CMX7042_SLOTCLK_HIGH();

		if ((CMX7042Instance.SlotCnt + 1) % 2250 != slotNum)
		{
			/*lq ʱ϶������У׼ */
			CMX7042Instance.SlotCalibFlg = TRUE;
		}
		CMX7042Instance.SlotCnt = slotNum;
		CMX7042Instance.PhaseCnt = 0;
	}
	else
	{
		if (CMX7042Instance.PhaseCnt < CMX7042_SLOTCLK_PULSEWIDTH)
		{
			CMX7042Instance.PhaseCnt++;
			if (CMX7042Instance.PhaseCnt >= CMX7042_SLOTCLK_PULSEWIDTH)
			{
				/*lq ����ʱ���� */
				CMX7042_SLOTCLK_LOW();					
			}
		}
	}
}

/*******************************************************************************
* ����  : CMX7042_GetSlotCntCalibFlgRst
* ����  : ��ѯ�Ƿ������CMX7042��ʱ϶��
* ����  : ��		  
* ���  : ��
* ����  : TRUE - �ǣ�FALSE - ��
*******************************************************************************/
U8 CMX7042_GetSlotCntCalibFlgRst(void)
{
	if (CMX7042Instance.SlotCalibFlg == TRUE)
	{
		CMX7042Instance.SlotCalibFlg = FALSE;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*******************************************************************************
* ����  : CMX7042_IsRx1Ready
* ����  : ��ѯCMX7042 Rx1�Ƿ�������
* ����  : ��		  
* ���  : ��
* ����  : TRUE - �У�FASLE - ��
*******************************************************************************/
U8 CMX7042_IsRx1Ready(void)
{
    if (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_R1BRDY) == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : CMX7042_IsRx2Ready
* ����  : ��ѯCMX7042 Rx2�Ƿ�������
* ����  : ��		  
* ���  : ��
* ����  : TRUE - �У�FASLE - ��
*******************************************************************************/
U8 CMX7042_IsRx2Ready(void)
{
    if (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_R2BRDY) == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : CMX7042_IsTxBufferFree
* ����  : ��ѯCMX7042 Tx Buffer�Ƿ����
* ����  : ��		  
* ���  : ��
* ����  : TRUE - ���ã�FASLE - ������
*******************************************************************************/
U8 CMX7042_IsTxBufferFree(void)
{
    if (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_TBFree) == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : CMX7042_IsTxDone
* ����  : ��ѯCMX7042 �����Ƿ����
* ����  : ��		  
* ���  : ��
* ����  : TRUE - ����ɣ�FASLE - δ���
*******************************************************************************/
U8 CMX7042_IsTxDone(void)
{
    U8 txStatus;

    if (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_TxDone) == TRUE)
    {
        /*lq ������ɣ���ȡ��ǰ�ķ���״̬ */
        CMX7042Instance.Status2RegOld = CMX7042Instance.Status2Reg;
        CMX7042Instance.Status2Reg = CMX7042_ReadWord(CMX7042_REGR_ADDR_Status2);

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : CMX7042_IsRxStateChange
* ����  : ��ѯCMX7042����״̬�Ƿ�ı�
* ����  : ��		  
* ���  : ��
* ����  : TRUE - �У�FASLE - ��
*******************************************************************************/
U8 CMX7042_IsRxStateChange(void)
{
    if (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_RxState) == TRUE)
    {
        CMX7042Instance.Status2RegOld = CMX7042Instance.Status2Reg;
        CMX7042Instance.Status2Reg = CMX7042_ReadWord(CMX7042_REGR_ADDR_Status2);

        /*lq �ж�RX1�Ƿ���ֽ��մ��� */
        if ((CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_TwoLong)
            || (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_CRC)
            || (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_NewFrameTwoBuffFull)
            || (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_EndFlgNotByteBound))
        {
            CMX7042_SetRxError(CMX7042_RX_1);                        
        }

        /*lq �ж�RX2�Ƿ���ֽ��մ��� */
        if ((CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_TwoLong)
            || (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_CRC)
            || (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_NewFrameTwoBuffFull)
            || (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_EndFlgNotByteBound))
        {
            CMX7042_SetRxError(CMX7042_RX_2);                        
        }
                
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : CMX7042_GetRxErrorRst
* ����  : ��ѯCMX7042 Rx�Ƿ���ճ���
* ����  : rxChannel - ���ջ�		  
* ���  : ��
* ����  : TRUE - �У�FASLE - ��
*******************************************************************************/
U8 CMX7042_GetRxErrorRst(U8 rxChannel)
{
    if (rxChannel == CMX7042_RX_1)
    {
        if (CMX7042Instance.Rx1ErrFlg == TRUE)
        {
            CMX7042Instance.Rx1ErrFlg = FALSE;
            return TRUE;    
        }
        else
        {
            return FALSE;
        }    
    } 
    else
    {
        if (CMX7042Instance.Rx2ErrFlg == TRUE)
        {
            CMX7042Instance.Rx2ErrFlg = FALSE;
            return TRUE;    
        }
        else
        {
            return FALSE;
        }    
    }      
}

/*******************************************************************************
* ����  : CMX7042_SetRxError
* ����  : ��λCMX7042 Rx���ճ����־
* ����  : rxChannel - ���ջ�		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_SetRxError(U8 rxChannel)
{
    if (rxChannel == CMX7042_RX_1)
    {
        CMX7042Instance.Rx1ErrFlg = TRUE;   
    } 
    else
    {
        CMX7042Instance.Rx2ErrFlg = TRUE;     
    }      
}

/*******************************************************************************
* ����  : CMX7042_GetRxState
* ����  : ��ѯCMX7042 rxChannel�ŵ����ջ���״̬
* ����  : rxChannel - ���ջ�		  
* ���  : ��
* ����  : state - ָ�����ջ���״̬
*******************************************************************************/
U8 CMX7042_GetRxState(U8 rxChannel)
{
    U8 state;

    if (rxChannel == CMX7042_RX_1)
    {
        state = CMX7042Instance.Status2Reg & CMX7042_STATUS2_BITMASK_Rx1State;    
    }
    else
    {
        state = (CMX7042Instance.Status2Reg & CMX7042_STATUS2_BITMASK_Rx2State) >> 4;
    }

    return state;
}

/*******************************************************************************
* ����  : CMX7042_GetRxStateOld
* ����  : ��ѯCMX7042 rxChannel�ŵ����ջ�����һ�ε�״̬
* ����  : rxChannel - ���ջ�		  
* ���  : ��
* ����  : state - ָ�����ջ���״̬
*******************************************************************************/
U8 CMX7042_GetRxStateOld(U8 rxChannel)
{
    U8 state;

    if (rxChannel == CMX7042_RX_1)
    {
        state = CMX7042Instance.Status2RegOld & CMX7042_STATUS2_BITMASK_Rx1State;    
    }
    else
    {
        state = (CMX7042Instance.Status2RegOld & CMX7042_STATUS2_BITMASK_Rx2State) >> 4;
    }

    return state;
}

/*******************************************************************************
* ����  : CMX7042_GetTxState
* ����  : ��ѯCMX7042�������״̬
* ����  : ��		  
* ���  : ��
* ����  : �������״̬
*******************************************************************************/
U8 CMX7042_GetTxState(void)
{
    return ((CMX7042Instance.Status2Reg & CMX7042_STATUS2_BITMASK_TxState) >> 8);
}

/*******************************************************************************
* ����  : CMX7042_GetIntFlgRst
* ����  : ��ѯCMX7042�жϱ�־�����Ѿ���λ���临λ
* ����  : ��		  
* ���  : ��
* ����  : �жϱ�־�ĵ�ǰ״̬
*******************************************************************************/
U8 CMX7042_GetIntFlgRst(void)
{
    if (CMX7042Instance.IntFlg == TRUE)
    {
        CMX7042Instance.IntFlg = FALSE;      

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : CMX7042_ReadWord
* ����  : ��CMX7042��ȡһ����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 CMX7042_ReadWord(U8 addr)
{
    U16 RegVal;
    
    /*lq ����SPI2 */
#if 0
    SPI_Cmd(SPI2, ENABLE);
#endif
    SPI2_NSS_LOW();             

    /*lq ���͵�ַ */
    SPI2_SendByte(addr);

    /*lq ��������*/
    RegVal = SPI2_SendByte(SPI_DUMMY_BYTE); 
    RegVal <<= 8;
    RegVal |= SPI2_SendByte(SPI_DUMMY_BYTE); 

    /*lq �ر�SPI2 */
    SPI2_NSS_HIGH();
#if 0                 
    SPI_Cmd(SPI2, DISABLE);
#endif   
    return RegVal;        
}

/*******************************************************************************
* ����  : CMX7042_WriteWord
* ����  : ��CMX7042дһ����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_WriteWord(U8 addr, U16 data)
{
    /*lq ����SPI2 */
#if 0   
    SPI_Cmd(SPI2, ENABLE);
#endif
    SPI2_NSS_LOW();             

    /*lq ���͵�ַ */
    SPI2_SendByte(addr);

    /*lq ���͸�8λ */
    SPI2_SendByte(data >> 8); 

    /*lq ���͵�8λ */
    SPI2_SendByte(data & 0xFF); 

    /*lq �ر�SPI */
    SPI2_NSS_HIGH();
#if 0                 
    SPI_Cmd(SPI2, DISABLE);
#endif
}

/*******************************************************************************
* ����  : CMX7042_WriteByte
* ����  : ��CMX7042дһ���ֽ�
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_WriteByte(U8 addr, U8 data)
{
    /*lq ����SPI2 */
#if 0   
    SPI_Cmd(SPI2, ENABLE);
#endif    
    SPI2_NSS_LOW();             

    /*lq ���͵�ַ */
    SPI2_SendByte(addr);

    /*lq �������� */
    SPI2_SendByte(data); 

    /*lq �ر�SPI */
    SPI2_NSS_HIGH(); 
#if 0                
    SPI_Cmd(SPI2, DISABLE);
#endif
}

/*******************************************************************************
* ����  : CMX7042_WriteCommand
* ����  : ��CMX7042дһ�������ֽ�
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_WriteCommand(U8 command)
{
    /*lq ����SPI2 */
#if 0
    SPI_Cmd(SPI2, ENABLE);
#endif    
    SPI2_NSS_LOW();             

    /*lq �������� */
    SPI2_SendByte(command);

    /*lq �ر�SPI */
    SPI2_NSS_HIGH();
#if 0                     
    SPI_Cmd(SPI2, DISABLE);
#endif
}

/*******************************************************************************
* ����  : CMX7042_EnInt
* ����  : ֻʹ��ָ�����жϣ�δָ�����жϱ���ԭ״̬
* ����  : interMask - �ж�ʹ������		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_EnInt(U16 intMask)
{
    CMX7042Instance.IntMaskReg |= intMask;
    CMX7042_WriteWord(CMX7042_REGW_ADDR_InterruptMask, CMX7042Instance.IntMaskReg);
}

/*******************************************************************************
* ����  : CMX7042_UnInt
* ����  : ֻʧ��ָ�����жϣ�δָ�����жϱ���ԭ״̬
* ����  : interMask - �ж�ʹ������		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_UnInt(U16 intMask)
{
    CMX7042Instance.IntMaskReg &= ~intMask;
    CMX7042_WriteWord(CMX7042_REGW_ADDR_InterruptMask, CMX7042Instance.IntMaskReg);
}

/*******************************************************************************
* ����  : CMX7042_CfgInt
* ����  : ֻʹ��ָ�����жϣ�δָ�����ж�ʧ��
* ����  : interMask - �ж�ʹ������		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_CfgInt(U16 intMask)
{
    CMX7042Instance.IntMaskReg = intMask;
    CMX7042_WriteWord(CMX7042_REGW_ADDR_InterruptMask, CMX7042Instance.IntMaskReg);
}

/*******************************************************************************
* ����  : CMX7042_DTaskDataWrite
* ����  : ִ��һ������д����������
* ����  : data - Ҫд������	
	      bitLen - ��������1-64��
          pointRst - data buffer point �Ƿ�λ
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_DTaskDataWrite(U16 data[4], U16 bitLen, U8 pointRst)
{
    U16 wordNum;
    U16 bitNum;

    /*lq ������� */
    if (bitLen > 64)    //lq CMX7042��ÿ�������������ת��4����
    {
        bitLen = 64;
    }
    else if (bitLen <= 0)
    {
        return ;
    }
    wordNum = bitLen / 16;
    bitNum = bitLen % 16;

    /*lq ����ת�� */
    if (wordNum != 0)
    {
        /*lq �����ݼĴ�����д���� */
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_DataTaskComp) == FALSE);
#endif
        if (wordNum >= 1)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite1, data[0]);                       
        }
        if (wordNum >= 2)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite2, data[1]);                             
        }
        if (wordNum >= 3)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite3, data[2]);                            
        }
        if (wordNum >= 4)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite4, data[3]); 
        }

        /*lq ִ������ת������ */    
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ComRegFree) == FALSE);           
#endif        
        if (pointRst == TRUE)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataWordReset_Tx(wordNum));                        
        }
        else
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataWordWrite_Tx(wordNum));                
        }  
    }

    /*lq ����ת�� */        
    if (bitNum != 0)
    {               
        /*lq �����ݼĴ���DataWrite1��д���� */
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_DataTaskComp) == FALSE);
#endif        
        CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite1, data[wordNum]);                

        /*lq ִ�б���ת������ */        
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ComRegFree) == FALSE);                   
#endif        
        if ((pointRst == TRUE) && wordNum == 0)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataBitReset_Tx(bitNum));                
        }
        else
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataBitWrite_Tx(bitNum));        
        }        
    }
}

/*******************************************************************************
* ����  : CMX7042_DTaskDataRead
* ����  : ִ��һ�����ݶ�����������
* ����  : data - ��Ŷ���������	
	      bitLen - ��������1-64��
          pointRst - data buffer point �Ƿ�λ
          rxBuff - ���ݻ���Rx1 or Rx2
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_DTaskDataRead(U16 data[4], U16 bitLen, U8 pointRst, U8 rxBuff)
{
    U16 wordNum;
    U16 bitNum;

    /*lq ������� */
    if (bitLen > 64)    //lq CMX7042��ÿ�������������ת��4����
    {
        bitLen = 64;
    }
    else if (bitLen <= 0)
    {
        return ;
    }
    wordNum = bitLen / 16;
    bitNum = bitLen % 16;
 

    /*lq ����ת�� */
    if (wordNum != 0)
    {
        /*lq ִ������ת������ */
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ComRegFree) == FALSE);           
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_DataTaskComp) == FALSE);         
#endif        
        if (pointRst == TRUE)
        {
            if (rxBuff == CMX7042_RX_1)
            {
                CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataWordReset_RX1(wordNum));                                    
            }
            else
            {
                CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataWordReset_RX2(wordNum));                                                
            }
        }
        else
        {
            if (rxBuff == CMX7042_RX_1)
            {
                CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataWordRead_RX1(wordNum));                                    
            }
            else
            {
                CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataWordRead_RX2(wordNum));                                                
            }
        }

        /*lq �����ݼĴ����ж����� */
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlg(CMX7042_STATUS_BITMASK_DataTaskComp) == FALSE);
#endif
        if (wordNum >= 1)
        {
            data[0] = CMX7042_ReadWord(CMX7042_REGR_ADDR_DataRead1);                
        }
        if (wordNum >= 2)
        {
            data[1] = CMX7042_ReadWord(CMX7042_REGR_ADDR_DataRead2);                
        }
        if (wordNum >= 3)
        {
            data[2] = CMX7042_ReadWord(CMX7042_REGR_ADDR_DataRead3);                
        }
        if (wordNum >= 4)
        {
            data[3] = CMX7042_ReadWord(CMX7042_REGR_ADDR_DataRead4);                
        }  
    }

    /*lq ����ת�� */        
    if (bitNum != 0)
    {               
        /*lq ִ�б���ת������ */
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ComRegFree) == FALSE);           
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_DataTaskComp) == FALSE); 
#endif
        if ((pointRst == TRUE) && wordNum == 0)
        {
            if (rxBuff == CMX7042_RX_1)
            {
                CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataBitReset_Rx1(bitNum));                            
            }
            else
            {
                CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataBitReset_Rx2(bitNum));                                        
            }
        }
        else
        {
            if (rxBuff == CMX7042_RX_1)
            {
                CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataBitRead_RX1(bitNum));                            
            }
            else
            {
                CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataBitRead_RX2(bitNum));                                        
            }
        }
        
        /*lq �����ݼĴ���DataRead1�ж����� */
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlg(CMX7042_STATUS_BITMASK_DataTaskComp) == FALSE);         
#endif        
        data[wordNum] = CMX7042_ReadWord(CMX7042_REGR_ADDR_DataRead1);                        
    }
}

/*******************************************************************************
* ����  : CMX7042_DTaskDataBulkWrite
* ����  : ִ��һ������д���ݵ���������
* ����  : data - Ҫд������	
	      bitLen - ������
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_DTaskDataBulkWrite(U8* pData, U16 bitLen)
{
    U16 taskNum;    //lq ÿ��������������д16*4=64bit
    U16 bitNum;     //lq ���һ������������д��λ��
    U16 byteNum;    //lq pData���ֽ�����
    U16 i;
    U16 data[4];
    U16 temp1, temp2;
    
    byteNum = (bitLen % 8) == 0 ? (bitLen / 8) : ((bitLen / 8) + 1);
    taskNum = bitLen / 64;
    bitNum = bitLen % 64;

    if (bitLen == 0)
    {
        return ;    
    }

    for (i = 0; i < taskNum; i++)    
    {
        data[0] = (pData[i * 8] << 8) | pData[i * 8 + 1];
        data[1] = (pData[i * 8 + 2] << 8) | pData[i * 8 + 3];
        data[2] = (pData[i * 8 + 4] << 8) | pData[i * 8 + 5];
        data[3] = (pData[i * 8 + 6] << 8) | pData[i * 8 + 7];
        
        if (i == 0)
        {
            CMX7042_DTaskDataWrite(data, 64, TRUE);
        }
        else
        {
            CMX7042_DTaskDataWrite(data, 64, FALSE);                   
        }             
    }

    if (bitNum != 0)
    {
        temp1 = byteNum - taskNum * 8;                          //lq ʣ���ֽ���
        temp2 = (temp1 % 2) == 0 ? temp1 / 2 : temp1 / 2 + 1;   //lq ʣ������

        for (i = 0; i < temp2; i++)
        {
            data[i] = pData[taskNum * 8 + i * 2] << 8;
            if ((taskNum * 8 + i * 2 + 1) < byteNum)            //lq �߽��������
            {
                data[i] |= pData[taskNum * 8 + i * 2 + 1];
            }
        }

        if (taskNum == 0)
        {
            CMX7042_DTaskDataWrite(data, bitNum, TRUE);                           
        }
        else
        {
            CMX7042_DTaskDataWrite(data, bitNum, FALSE); 
        }        
    } 
}


/*******************************************************************************
* ����  : CMX7042_DTaskDataBulkWrite
* ����  : ִ��һ������д���ݵ���������
* ����  : data - Ҫд������	
	      bitLen - ������
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_DTaskDataBulkRead(U8* pData, U16 bitLen,  U8 pointRst, U8 rxBuff)
{
    U16 taskNum;    //lq ÿ��������������д16*4=64bit
    U16 bitNum;     //lq ���һ������������д��λ��
    U16 byteNum;    //lq pData���ֽ�����
    U16 i, k;
    U16 data[4];
    U16 temp1, temp2;
    
    byteNum = (bitLen % 8) == 0 ? (bitLen / 8) : ((bitLen / 8) + 1);
    taskNum = bitLen / 64;
    bitNum = bitLen % 64;

    if (bitLen == 0)
    {
        return ;    
    }

    /*lq �������ֶ�ȡ */
    for (i = 0; i < taskNum; i++)    
    {
        if (i == 0)
        {
            CMX7042_DTaskDataRead(data, 64, pointRst, rxBuff);
        }
        else
        {
            CMX7042_DTaskDataRead(data, 64, FALSE, rxBuff);
        } 

        for (k = 0; k < 4; k++)
        {
            pData[i * 8 + k * 2] = (data[k] >> 8) & 0xFF;
            pData[i * 8 + k * 2 + 1] = data[k] & 0xFF;

        }           
    }

    /*lq ����һ���ֵĶ�ȡ */
    if (bitNum != 0)
    {
        temp1 = byteNum - taskNum * 8;                          //lq ʣ���ֽ���
        temp2 = (temp1 % 2) == 0 ? temp1 / 2 : temp1 / 2 + 1;   //lq ʣ������

        if (taskNum == 0)
        {
            CMX7042_DTaskDataRead(data, bitNum, pointRst, rxBuff);
        }
        else
        {
            CMX7042_DTaskDataRead(data, bitNum, FALSE, rxBuff);
        } 

        for (i = 0; i < temp2; i++)
        {
            pData[taskNum * 8 + i * 2] = (data[i] >> 8) & 0xFF;
            if ((taskNum * 8 + i * 2 + 1) < byteNum)            //lq �߽��������
            {
                pData[taskNum * 8 + i * 2 + 1] = data[i] & 0xFF;                    
            }
        }       
    } 
}


/*******************************************************************************
* ����  : CMX7042_ExecCfgTask
* ����  : ִ���������񣬸�������������в���Ҳ���޲���
* ����  : cfgTask - ��������
          data - ���ò���
	      wordNum - ����������0-4          
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_ExecCfgTask(U16 cfgTask, U16 data[4], U16 wordNum)
{
    /*lq ������� */
    if (wordNum > 4)
    {
        wordNum = 4;
    }

    if (wordNum != 0)
    {
        /*lq �����ݼĴ�����д���� */
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_DataTaskComp) == FALSE);        
#endif
        if (wordNum >= 1)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite1, data[0]);                
        }
        if (wordNum >= 2)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite2, data[1]);                
        }
        if (wordNum >= 3)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite3, data[2]);                
        }
        if (wordNum >= 4)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite3, data[3]);                
        }
    
        /*lq ͬʱִ������������������� */
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ComRegFree) == FALSE);
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_CnfgTaskComp) == FALSE);                              
#endif
#if 0        
        CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, 
                          CMX7042_DTASK_DataWordWrite_Tx(wordNum) | cfgTask);
#else
        CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, 
                          CMX7042_DTASK_DataWordReset_Tx(wordNum) | cfgTask);
#endif
    }
    else
    {
        /*lq ִ���������� */
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ComRegFree) == FALSE);
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_CnfgTaskComp) == FALSE);                              
#endif
        CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, cfgTask);
    }
}

/*******************************************************************************
* ����  : CMX7042_ExecCfgTask
* ����  : ִ�в�����������������
* ����  : cfgTask - ��������
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_ExecCfgTaskNoPara(U16 cfgTask)
{
    /*lq ִ���������� */
#if CMX7042_FLAG_CHECK
    while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ComRegFree) == FALSE);
    while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_CnfgTaskComp) == FALSE);                              
#endif    
    CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, cfgTask);
}

/*******************************************************************************
* ����  : CMX7042_ExecModemTaskNoPara
* ����  : ִ�в��������ĵ��ƽ������
* ����  : task - ����
* ���  : ��
* ����  : ��
*******************************************************************************/
void CMX7042_ExecModemTaskNoPara(U16 task)
{
    /*lq ִ���������� */
#if CMX7042_FLAG_CHECK
    while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ComRegFree) == FALSE);
#endif    
    CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, task);
}

/*******************************************************************************
* ����  : CMX7042_LoadFI
* ����  : ����FI
* ����  : ��		  
* ���  : ��
* ����  : TRUE-�ɹ���FALSE-ʧ��
*******************************************************************************/
U8 CMX7042_LoadFI(void)
{
    U16 i;
    U16 db1ChkHi = 0;
    U16 db1ChkLo = 0;
    U16 db2ChkHi = 0;
    U16 db2ChkLo = 0;

    //lq... ÿ������FI֮ǰ�������λCMX7042

	/*lq ι��*/
    Is_Feed_Dog = TRUE;
	Feed_Dog();

    /*lq ��CMX7042��������CMX7042����ʧ�ܣ���ᵼ�¿��Ź���λ���� */
    while((CMX7042_ReadWord(CMX7042_REGR_ADDR_Status) & 0x0001) != 1);	

    /*lq д���ݿ�1 */
    CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite3, DB1_PTR);
    CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite4, DB1_LEN);
    CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, 0x0001);
    while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ACT) == FALSE);
    for (i = 0; i < DB1_LEN; i++)
    {
        CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, db1[i]);
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ACT) == FALSE);
    }
    usart_printf("$CMX7042 Data block1: done!\r\n");

	/*lq ι��*/
    Is_Feed_Dog = TRUE;
	Feed_Dog();

    /*lq д���ݿ�2 */
    CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite3, DB2_PTR);
    CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite4, DB2_LEN);
    CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, 0x0001);   
    while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ACT) == FALSE);
    for (i = 0; i < DB2_LEN; i++)
    {
        CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, db2[i]);
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ACT) == FALSE);
    }
    usart_printf("$CMX7042 Data block2: done!\r\n");

	/*lq ι��*/
    Is_Feed_Dog = TRUE;
	Feed_Dog();
    
    /*lq д���ݿ�3 */
    CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite3, ACTIVATE_PTR);
    CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite4, ACTIVATE_LEN);
    CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, 0x0001);
    while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ACT) == FALSE);
    usart_printf("$CMX7042 Data block3: done!\r\n");
    
    /*lq ��ȡУ���� */
    db1ChkHi = CMX7042_ReadWord(CMX7042_REGR_ADDR_DataRead3);
    db1ChkLo = CMX7042_ReadWord(CMX7042_REGR_ADDR_DataRead4);
    db2ChkHi = CMX7042_ReadWord(CMX7042_REGR_ADDR_DataRead1);
    db2ChkLo = CMX7042_ReadWord(CMX7042_REGR_ADDR_DataRead2);
    usart_printf("$CMX7042 DB1_CHK_HI: %04X!\r\n", db1ChkHi);
    usart_printf("$CMX7042 DB1_CHK_LO: %04X!\r\n", db1ChkLo);
    usart_printf("$CMX7042 DB2_CHK_HI: %04X!\r\n", db2ChkHi);
    usart_printf("$CMX7042 DB2_CHK_LO: %04X!\r\n", db2ChkLo);
          
    if ((db1ChkHi == DB1_CHK_HI)
        && (db1ChkLo == DB1_CHK_LO)
        && (db2ChkHi == DB2_CHK_HI)
        && (db2ChkLo == DB2_CHK_LO))
    {
        /*lq ��ȡ�豸ID��FI�汾�� */
        CMX7042Instance.ProductIdCode = CMX7042_ReadWord(CMX7042_REGR_ADDR_Status2);
        CMX7042Instance.FIVersionCode = CMX7042_ReadWord(CMX7042_REGR_ADDR_ADCData);
        usart_printf("$CMX7042 ProductIdCode:%04X!\r\n", CMX7042Instance.ProductIdCode);
        usart_printf("$CMX7042 FIVersionCode:%04X!\r\n", CMX7042Instance.FIVersionCode);

        /*lq ���ͼ������λ */
        CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, ACTIVATE_CHK_HI);
        usart_printf("$CMX7042 Send activecodeHigh done!\r\n");
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ACT) == FALSE);
        
        /*lq ���ͼ������λ */
        CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, ACTIVATE_CHK_LO);
        usart_printf("$CMX7042 Send activecodeLow done!\r\n");
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ACT) == FALSE);
        CMX7042Instance.ActivedFlg = TRUE;
        usart_printf("$CMX7042 Actived successly!\r\n");

    	/*lq ι��*/
        Is_Feed_Dog = TRUE;
    	Feed_Dog();                             

        return TRUE;
    }
    else
    {       
        /*lq FI����ʧ�� */    
        return FALSE;
    }                                 
}

/*******************************************************************************
* ����  : CMX7042_Configuration
* ����  : CMX7042����
* ����  : ��		  
* ���  : ��
* ����  : TRUE-�ɹ���FALSE-ʧ��
*******************************************************************************/
void CMX7042_Configuration(void)
{
    U16 data[4];

    /*lq ---------------ֱ�����üĴ���---------------------- */
    /*lq Interrupt Mask Register - $CE 16-bit write */      
	CMX7042_CfgInt(CMX7042_STATUS_BITMASK_IRQ
                   //lq | CMX7042_STATUS_BITMASK_TxDone
				   | CMX7042_STATUS_BITMASK_RxState
                   | CMX7042_STATUS_BITMASK_R1BRDY
                   | CMX7042_STATUS_BITMASK_R2BRDY
                   | CMX7042_STATUS_BITMASK_TBFree);		//lq ����7042�ж�	

    /*lq System CLK 1 and 2 PLL Data Registers - $AB, $AD write */
    //lq �������ã�Ӳ��δʹ��

    /*lq System CLK 1 and 2 Ref Registers - $AC and $AE write */
    //lq �������ã�Ӳ��δʹ��

    /*lq Input/Output Gain and Routing Register - $B1 write */
    CMX7042_WriteWord(CMX7042_REGW_ADDR_IOGainAndRouting, 0xB480);//lq Input1 Gain = 16dB��Input2 Gain = 16dB��MOD2 Source: Output -> MOD2;

    /*lq Power Down Register - $C0 16-bit write */
    //lq �������ã���FI�Զ�����

    /*lq Mode Register - $C1 16-bit write: ADC Input Select, sleep EN, CSEN, CS Channel, Rx2 AIS/DSC, Rx1 AIS/DSC*/
    CMX7042_WriteWord(CMX7042_REGW_ADDR_Mode, 0x0000);      //lq ADC Input Select = No input selected;sleep EN = false;CSEN = false;CS Channel = Rx1;Rx2 AIS/DSC = AIS; Rx1 AIS/DSC = AIS;    
    
    /*lq CS-Threshold Register - $C2 16-bit write */ 
    //lq ��CSEN = false����������CS-Threshold Register

    /*lq ---------------��������ģʽ---------------------- */
    while ((CMX7042_ReadWord(CMX7042_REGR_ADDR_Status2) & CMX7042_STATUS2_BITMASK_Conf) == 0)
    {
#if 0
        CMX7042_ExecCfgTaskNoPara(CMX7042_MTASK_EnterConfigMode);
#else
        CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_MTASK_EnterConfigMode);
#endif
        Delay_ms(5);    
    }

    /*lq ---------------ʹ��Configuration Task ��������---------------------- */
    /*lq System Options */
    data[0] = 0x0001;                                               //lq 9.6MHz xtal or oscillator; enable CRC checking;
    CMX7042_ExecCfgTask(CMX7042_CTASK_SystemOptions, data, 1);      //lq ִ�и���������

    /*lq Tx 2-point or I/Q mode*/
    //lq... ʹ��Ĭ������: 2-point modulation, MOD1 and MOD2 normal polarity.

    /*lq Tx MOD levels */
    //lq... ʹ��Ĭ�����ã�MOD1 = 0dB, MOD2 = 0dB.

    /*lq RSSI look-up table */
    //lq... ʹ��Ĭ��ֵ

    /*lq RSSI window */
    //lq... ʹ��Ĭ��ֵ: start time = 60, 24bits; window length = 27, approx. 11bits;

    /*lq RSSI gain */
    //lq... ʹ��Ĭ��ֵ��Default value is $025F ( x 0.0185)

    /*lq CSTDMA gain */
    //lq... ʹ��Ĭ��ֵ��Default value is $025F ( x 0.0185)
#if 0 //lq ��׼����
    /*lq Tx Sequence */
    data[0] = 0x0000;                               //lq Sequence Start Time = 0bit
    data[1] = 0x0001;                               //lq TXENA initial state = 1
    data[2] = CMX7042_TXSEQ_EVENTID_DUMMY;          //lq CSTDMA_START event ID , ��CSδʹ�ܣ�����dummy�¼�
    data[3] = 0x0000;                               //lq timer = 0
    CMX7042_DTaskDataWrite(data, 64, TRUE);         //lq ������������ĵ�һ�������踴λ���ݻ���ָ��
    
    data[0] = CMX7042_TXSEQ_EVENTID_MODULATE_START; //lq MODULATE_START event ID
    data[1] = 0x0000;                               //lq timer = 0
    data[2] = CMX7042_TXSEQ_EVENTID_DUMMY;          //lq CSTDMA_END event ID , ��CSδʹ�ܣ�����dummy�¼�
    data[3] = 0x0000;                               //lq timer = 0
    CMX7042_DTaskDataWrite(data, 64, FALSE);
    
    data[0] = CMX7042_TXSEQ_EVENTID_Tx_en_lo;       //lq Tx_en_lo event ID
    data[1] = 0x0005;                               //lq timer = 2bit
    data[2] = CMX7042_TXSEQ_EVENTID_DUMMY;          //lq RAMDAC_UP event ID, δʹ��RAMDAC_UP����dummy�¼�ռλ
    data[3] = 0x0003;                               //lq timer ~ 1bit
    CMX7042_DTaskDataWrite(data, 64, FALSE);

    data[0] = CMX7042_TXSEQ_EVENTID_DUMMY;          //lq RAMDAC_DOWN event ID��δʹ��RAMDAC_DOWN����dummy�¼�ռλ
    data[1] = 0x0000;                               //lq timer = 0
    data[2] = CMX7042_TXSEQ_EVENTID_Tx_en_hi;       //lq Tx_en_hi event ID
    data[3] = 0x0014;                               //lq timer = 8bit
    CMX7042_DTaskDataWrite(data, 64, FALSE);

    data[0] = CMX7042_TXSEQ_EVENTID_MODULATE_END;   //lq MODULATE_END event ID
    data[1] = 0x0005;                               //lq timer = 2bit
    CMX7042_DTaskDataWrite(data, 32, FALSE);
#else
    /*lq Tx Sequence */
    data[0] = 0x0000;                               //lq Sequence Start Time = 0bit
    data[1] = 0x0001;                               //lq TXENA initial state = 1
    data[2] = CMX7042_TXSEQ_EVENTID_DUMMY;          //lq CSTDMA_START event ID , ��CSδʹ�ܣ�����dummy�¼�
    data[3] = 0x0000;                               //lq timer = 0
    CMX7042_DTaskDataWrite(data, 64, TRUE);         //lq ������������ĵ�һ�������踴λ���ݻ���ָ��
    
    data[0] = CMX7042_TXSEQ_EVENTID_MODULATE_START; //lq MODULATE_START event ID
    data[1] = 0x0000;                               //lq timer = 0
    data[2] = CMX7042_TXSEQ_EVENTID_DUMMY;          //lq CSTDMA_END event ID , ��CSδʹ�ܣ�����dummy�¼�
    data[3] = 0x0000;                               //lq timer = 0
    CMX7042_DTaskDataWrite(data, 64, FALSE);
    
    data[0] = CMX7042_TXSEQ_EVENTID_Tx_en_lo;       //lq Tx_en_lo event ID
    data[1] = 0x0005;                               //lq timer = 2bit
    data[2] = CMX7042_TXSEQ_EVENTID_RAMDAC_UP;      //lq RAMDAC_UP event ID, δʹ��RAMDAC_UP����dummy�¼�ռλ
    data[3] = 0x0008;                               //lq timer ~ 1bit
    CMX7042_DTaskDataWrite(data, 64, FALSE);

    data[0] = CMX7042_TXSEQ_EVENTID_MODULATE_END;   //lq MODULATE_END event ID
    data[1] = 0x0000;                               //lq timer = 0
    data[2] = CMX7042_TXSEQ_EVENTID_RAMDAC_DOWN;    //lq RAMDAC_DOWN event ID��δʹ��RAMDAC_DOWN����dummy�¼�ռλ
    data[3] = 0x0000;                               //lq timer = 0    
    CMX7042_DTaskDataWrite(data, 64, FALSE);

    data[0] = CMX7042_TXSEQ_EVENTID_Tx_en_hi;       //lq Tx_en_hi event ID
    data[1] = 0x0014;                               //lq timer = 8bit
    CMX7042_DTaskDataWrite(data, 32, FALSE);
#endif
#if CMX7042_FLAG_CHECK
    while (CMX7042_GetStatusBitFlg(CMX7042_STATUS_BITMASK_DataTaskComp) == FALSE);  //lq �˴����ܸ�λ����Ϊ���治ִ����������ֻ��ȷ��ǰ��ͨ�������������ṩ�Ĳ��������
    while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ComRegFree) == FALSE);
    while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_CnfgTaskComp) == FALSE);
#endif    
    CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_CTASK_Tx_sequence);    //lq ִ�и���������
    
    /*lq Sleep Timing */    
    //lq... ʹ��Ĭ��ֵ: start time = 120;end time = 625;

    /*lq RAMDAC Control and Load */
    //lq ʹ��Ĭ��ֵ��Ӳ����·��֧��
    data[0] = 0x0006;                                               //lq AuxDAC1 in RAMDAC mode; enable AuxDAC1; no ramp profile update;
    data[1] = 0x0007;                                               //lq RAMDAC CLK div1 = 8;
    data[2] = 0x001E;                                               //lq RAMDAC CLK div2 = 30;
    CMX7042_ExecCfgTask(CMX7042_CTASK_RAMDACLoad, data, 3);         //lq ִ�и���������

    /*lq Enter Deep Sleep */
    //lq �������ã���FI�Զ�����

    /*lq Leave Deep Sleep */
    //lq �������ã���FI�Զ�����

    /*lq Slot Number Initialization */
    //lq... �ݲ����ã���SlotClk�������

    /*lq Time of Arrival Compensation */
    data[0] = 0x0000;                                               //lq... ToA compensation = 0������ڲ���
    CMX7042_ExecCfgTask(CMX7042_CTASK_SetToACompensate, data, 1);   //lq ִ�и���������    
    usart_printf("$CMX7042 config finished!\r\n");
    usart_printf("$CMX7042[Status2Reg]:%04X!\r\n", CMX7042_ReadWord(CMX7042_REGR_ADDR_Status2));

	/*lq ι��*/
    Is_Feed_Dog = TRUE;
	Feed_Dog();
            
    /*lq ---------------�˳�����ģʽ---------------------- */
    while ((CMX7042_ReadWord(CMX7042_REGR_ADDR_Status2) & CMX7042_STATUS2_BITMASK_Conf) != 0)
    {
#if 1
        CMX7042_ExecCfgTaskNoPara(CMX7042_CTASK_EXIT_CONFIG);
#else
        CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_CTASK_EXIT_CONFIG);
#endif
        Delay_ms(5);    
    }

    usart_printf("$CMX7042 exit config mode!\r\n");
    usart_printf("$CMX7042[Status2Reg]:%04X!\r\n", CMX7042_ReadWord(CMX7042_REGR_ADDR_Status2));

    /*lq ������������ */
    CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Rx1_RXB);
    CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Rx2_RXB);
}
