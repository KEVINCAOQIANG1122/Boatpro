/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: main.c
  Author: ��ΰ       Version : V1.00        Date: 2011.09.23
  Description:    ��Ŀ�й���UCOS����ϵͳ
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			1����������Main
    	   �������ܣ�����ARM��������ļܹ�
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include "stm32f10x.h"
#include "def.h"
#include "ioctrl.h"
#include "LED.h"
#include "gps.h"
#include "power.h"
#include "common.h"
#include "RxTx.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private  function  --------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/

#if 0
/*******************************************************************************
* ����  : application_init
* ����  : ����Ӧ�ó�ʼ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void IO_Ctrl(U8 ucType, U8 ucVal)
{
	if (ucVal == IO_VAL_LOW)
	{
		if (ucType == IO_GREEN1_TYPE)
		{
			LED_GREEN1_LOW();	
		}
		else if (ucType == IO_RED1_TYPE)
		{
			LED_RED1_LOW();
		}
		else if (ucType == IO_GREEN2_TYPE)
		{
			LED_GREEN2_LOW();
		}
		else if (ucType == IO_RED2_TYPE)
		{
			LED_RED2_LOW();
		}
		else if (ucType == IO_POWER_TYPE)
		{
			//POWER_CRL_HIGH();
		//lq	POWER_CRL_LOW();
            POWER_CRL_LOW();
            POWER_CLK_LOW();
            Delay_ms(10);
            POWER_CLK_HIGH();
		}
		else if (ucType == IO_GPSPOWER_TYPE)
		{
            ResetGpsPulseEn();      //lq GPS����ǰʧ��GPS������		

        	GPSPOWER_LOW();
		}
#if 0
		else if (ucType == IO_RDA1846_1_TYPE)
		{
			RDA1846_Sleep(RDA1846_CS_1);	
		}
        else if (ucType == IO_RDA1846_2_TYPE)
        {
			RDA1846_Sleep(RDA1846_CS_2);	        
        }
#endif
        else if (ucType == IO_RX_EN_TYPE)
        {
            RX_EN_LOW();
        }
        else if (ucType == IO_ANT_SW_TYPE)
        {
            ANT_SW_LOW();
        }
        else if (ucType == IO_RF_VCC_TYPE)
        {
            RF_VCC_LOW();
        }
	}
	else
	{
		if (ucType == IO_GREEN1_TYPE)
		{
			LED_GREEN1_HIGH();	
		}
		else if (ucType == IO_RED1_TYPE)
		{
			LED_RED1_HIGH();
		}
		else if (ucType == IO_GREEN2_TYPE)
		{
			LED_GREEN2_HIGH();
		}
		else if (ucType == IO_RED2_TYPE)
		{
			LED_RED2_HIGH();
		}
		else if (ucType == IO_POWER_TYPE)
		{
			//POWER_CRL_LOW();	
		//lq 	POWER_CRL_HIGH();
            
            POWER_CRL_HIGH();
            POWER_CLK_LOW();
            Delay_ms(10);
            POWER_CLK_HIGH();
		}
		else if (ucType == IO_GPSPOWER_TYPE)
		{
			GPSPOWER_HIGH();

            Delay_ms(1);            //lq ȷ���Ƚ���GPS�������жϺ���λʹ��λ
            SetGpsPulseEn();        //lq GPS�ϵ��ʹ��GPS������
		}
#if 0	
    	else if (ucType == IO_RDA1846_1_TYPE)
		{
			RDA1846_Wakeup(RDA1846_CS_1);		
		}
		else if (ucType == IO_RDA1846_2_TYPE)
		{
			RDA1846_Wakeup(RDA1846_CS_2);		
		}
#endif
        else if (ucType == IO_RX_EN_TYPE)
        {
            RX_EN_HIGH();
        }
        else if (ucType == IO_ANT_SW_TYPE)
        {
            ANT_SW_HIGH();
        }
        else if (ucType == IO_RF_VCC_TYPE)
        {
            RF_VCC_HIGH();
        }
	}	
}
#else
/*******************************************************************************
* ����  : IO_Ctrl
* ����  : IO���ſ���
* ����  : ucType - ��������
          ucVal - ������ر�
* ���  : ��
* ����  : ��
*******************************************************************************/
void IO_Ctrl(U8 ucType, U8 ucVal)
{
	if (ucVal == IO_VAL_OFF)
	{
		switch(ucType)
		{
			case IO_LED_GREEN_TYPE :
			{
				LED_GREEN_OFF();
				break;	
			}
			case IO_LED_YELLOW_TYPE :
			{
				LED_YELLOW_OFF();
				break;
			}
			case IO_LED_RED_TYPE :
			{
				LED_RED_OFF();
				break;
			}
			case IO_LED_BLUE_TYPE :
			{
				LED_BLUE_OFF();
				break;
			}
			case IO_POWER_TYPE :
			{
	            POWER_CRL_OFF();
	            POWER_CLK_LOW();
	            Delay_ms(10);
	            POWER_CLK_HIGH();
				break;
			}
			case IO_GPSPOWER_TYPE :
			{
	            ResetGpsPulseEn();      //lq GPS����ǰʧ��GPS������		
	
	        	GPS_POWER_OFF();
				break;
			}
	        case IO_RX_EN_TYPE :
	        {
	            RX_EN_OFF();
				break;
	        }
	        case IO_ANT_SW_RX_TYPE :
	        {
	            ANT_SW_TX();
				break;
	        }
	        case IO_RF_VCC_TYPE :
	        {
	            RF_VCC_OFF();
				break;
	        }
		}
	}
	else
	{
		switch(ucType)
		{
			case IO_LED_GREEN_TYPE :
			{
				LED_GREEN_ON();
				break;	
			}
			case IO_LED_YELLOW_TYPE :
			{
				LED_YELLOW_ON();
				break;
			}
			case IO_LED_RED_TYPE :
			{
				LED_RED_ON();
				break;
			}
			case IO_LED_BLUE_TYPE :
			{
				LED_BLUE_ON();
				break;
			}
			case IO_POWER_TYPE :
			{
	            POWER_CRL_ON();
	            POWER_CLK_LOW();
	            Delay_ms(10);
	            POWER_CLK_HIGH();
				break;
			}
			case IO_GPSPOWER_TYPE :
			{
				GPS_POWER_ON();
	
	            Delay_ms(1);            //lq ȷ���Ƚ���GPS�������жϺ���λʹ��λ
	            SetGpsPulseEn();        //lq GPS�ϵ��ʹ��GPS������
				break;
			}
	        case IO_RX_EN_TYPE :
	        {
	            RX_EN_ON();
				break;
	        }
	        case IO_ANT_SW_RX_TYPE :
	        {
	            ANT_SW_RX();
				break;
	        }
	        case IO_RF_VCC_TYPE :
	        {
	            RF_VCC_ON();
				break;
	        }
		}
	}	
}
#endif

/*******************************************************************************
* ����  : application_init
* ����  : ����Ӧ�ó�ʼ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void IO_Write(U8 ucType, U32 ulVal)
{
#if 0
	if (ucType == IO_RDA1846_1_TYPE)
	{
		Init_RDA1846_Tx(ulVal, RDA1846_CS_1);	
	}
    else if (ucType == IO_RDA1846_2_TYPE)
    {
		Init_RDA1846_Tx(ulVal, RDA1846_CS_2);	    
    }
#endif    	
}


