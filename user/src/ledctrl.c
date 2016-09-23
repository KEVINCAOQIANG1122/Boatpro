/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: ledctrl.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    LED����ģ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
    <author>    <time>      <version >  <desc>
    lq          2015.07.23              �޸��ļ��ṹ�����¶���LED���ƻ����LED��
                                        ʼ����ʽ                                            
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "def.h"
#include "ledctrl.h"
#include "ioctrl.h"
#include "systemtick.h"

/* Private define ------------------------------------------------------------*/
#define LED_INFINIT_LEN		0xFF

#define LED1_GREEN_OFFSET	0
#define LED1_RED_OFFSET		1
#define LED2_GREEN_OFFSET	2
#define LED2_RED_OFFSET		3
#define LED3_GREEN_OFFSET	4
#define LED3_RED_OFFSET		5
#define LED4_GREEN_OFFSET	6
#define LED4_RED_OFFSET		7
#define LED1_TIMER_OFFSET	8
#define LED2_TIMER_OFFSET	9
#define LED3_TIMER_OFFSET	10
#define LED4_TIMER_OFFSET	11

/* Private typedef -----------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/
LEDCtrlStruct LEDCtrlPara;

/* Private variables ---------------------------------------------------------*/
/*lq ����ʱ����250msΪ��λ */
/*lq LED�رջ��� */
#define LED_NONE_LEN	    1
static const U8 LED_None_Buf[LED_CTRL_BUFF_DEPTH][LED_NONE_LEN] = 
{	
	{0},		//LED1_GREEN, ���ڵ�ɫLEDֻ�趨��LED_GREEN�Ŀ��Ʒ�ʽ���ɣ�LED_RED�Ķ����LED����û��Ӱ��
	{0},		//LED1_RED
	{0},		//LED2_GREEN
	{0},		//LED2_RED
	{0},		//LED3_GREEN
	{0},		//LED3_RED
	{0},		//LED4_GREEN
	{0},		//LED4_RED
	{0},	    //LED1_TIME
	{0},		//LED2_TIME
	{0},		//LED3_TIME
	{0},	    //LED4_TIME
};

/*lq GPS�������壨ͬ��ʱָʾ�� */
#define LED_GPS_SEARCH_LEN		1
static const U8 LED_GPS_Search_Buf[LED_CTRL_BUFF_DEPTH][LED_GPS_SEARCH_LEN] = 
{	
	{0},		//LED1_GREEN, ���ڵ�ɫLEDֻ�趨��LED_GREEN�Ŀ��Ʒ�ʽ���ɣ�LED_RED�Ķ����LED����û��Ӱ��
	{0},		//LED1_RED
	{1},		//LED2_GREEN
	{1},		//LED2_RED
	{0},		//LED3_GREEN
	{0},		//LED3_RED
	{0},		//LED4_GREEN
	{0},		//LED4_RED
	{0},	    //LED1_TIME
	{1},		//LED2_TIME
	{0},		//LED3_TIME
	{0},	    //LED4_TIME
};

/*lq GPS��λ���壨ѭ����Blue LED ��250ms����250ms�� */
#define LED_GPS_FIX_LEN		2
static const U8 LED_GPS_Fix_Buf[LED_CTRL_BUFF_DEPTH][LED_GPS_FIX_LEN] = 
{	
	{0, 0},		//LED1_GREEN, ���ڵ�ɫLEDֻ�趨��LED_GREEN�Ŀ��Ʒ�ʽ���ɣ�LED_RED�Ķ����LED����û��Ӱ��
	{0, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{0, 0},		//LED3_GREEN
	{0, 0},		//LED3_RED
	{1, 0},		//LED4_GREEN
	{1, 0},		//LED4_RED
	{0, 0},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{0, 0},		//LED3_TIME
	{1, 1},	    //LED4_TIME
};

/*lq ��������ģʽ���仺�壨���Σ�Red LED ��1000ms����250ms�� */
#define LED_RF_XMT_LEN		2
static const U8 LED_RF_Xmt_Buf[LED_CTRL_BUFF_DEPTH][LED_RF_XMT_LEN] = 
{	
	{0, 0},		//LED1_GREEN, ���ڵ�ɫLEDֻ�趨��LED_GREEN�Ŀ��Ʒ�ʽ���ɣ�LED_RED�Ķ����LED����û��Ӱ��
	{0, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{1, 0},		//LED3_GREEN
	{1, 0},		//LED3_RED
	{0, 0},		//LED4_GREEN
	{0, 0},		//LED4_RED
	{0, 0},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{4, 1},		//LED3_TIME
	{0, 0},	    //LED4_TIME
};

/*lq ��ʱָʾ���壨ѭ����YELLOW LED ������ */
#define LED_TIMEOUT_LEN	    1
static const U8 LED_Timeout_Buf[LED_CTRL_BUFF_DEPTH][LED_TIMEOUT_LEN] = 
{	
	{0},		//LED1_GREEN, ���ڵ�ɫLEDֻ�趨��LED_GREEN�Ŀ��Ʒ�ʽ���ɣ�LED_RED�Ķ����LED����û��Ӱ��
	{0},		//LED1_RED
	{1},		//LED2_GREEN
	{1},		//LED2_RED
	{0},		//LED3_GREEN
	{0},		//LED3_RED
	{0},		//LED4_GREEN
	{0},		//LED4_RED
	{0},	    //LED1_TIME
	{1},		//LED2_TIME
	{0},		//LED3_TIME
	{0},	    //LED4_TIME
};

/*lq ����ָʾ���壨ѭ����Red LED ������ */
#define LED_ERR_LEN	    1
static const U8 LED_Err_Buf[LED_CTRL_BUFF_DEPTH][LED_ERR_LEN] = 
{	
	{0},		//LED1_GREEN, ���ڵ�ɫLEDֻ�趨��LED_GREEN�Ŀ��Ʒ�ʽ���ɣ�LED_RED�Ķ����LED����û��Ӱ��
	{0},		//LED1_RED
	{0},		//LED2_GREEN
	{0},		//LED2_RED
	{1},		//LED3_GREEN
	{1},		//LED3_RED
	{0},		//LED4_GREEN
	{0},		//LED4_RED
	{0},	    //LED1_TIME
	{0},		//LED2_TIME
	{1},		//LED3_TIME
	{0},	    //LED4_TIME
};

/*lq ��������ģʽ��Դָʾ���壨ѭ����Green LED ��250ms����250ms�� */
#define LED_POWER_NORMAL_LEN		2
static const U8 LED_PowerNormal_Buf[LED_CTRL_BUFF_DEPTH][LED_POWER_NORMAL_LEN] = 
{	
	{1, 0},		//LED1_GREEN, ���ڵ�ɫLEDֻ�趨��LED_GREEN�Ŀ��Ʒ�ʽ���ɣ�LED_RED�Ķ����LED����û��Ӱ��
	{1, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{0, 0},		//LED3_GREEN
	{0, 0},		//LED3_RED
	{0, 0},		//LED4_GREEN
	{0, 0},		//LED4_RED
	{1, 1},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{0, 0},		//LED3_TIME
	{0, 0},	    //LED4_TIME
};

/*lq ���Թ���ģʽ��Դָʾ���壨ѭ����Green LED ��500ms����500ms�� */
#define LED_POWER_TEST_LEN		2
static const U8 LED_PowerTest_Buf[LED_CTRL_BUFF_DEPTH][LED_POWER_TEST_LEN] = 
{	
	{1, 0},		//LED1_GREEN, ���ڵ�ɫLEDֻ�趨��LED_GREEN�Ŀ��Ʒ�ʽ���ɣ�LED_RED�Ķ����LED����û��Ӱ��
	{1, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{0, 0},		//LED3_GREEN
	{0, 0},		//LED3_RED
	{0, 0},		//LED4_GREEN
	{0, 0},		//LED4_RED
	{2, 2},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{0, 0},		//LED3_TIME
	{0, 0},	    //LED4_TIME
};

/*lq ������Ե�1��壨ѭ����Red LED ��250ms����250ms�� */
#define LED_TEST_TEST1_LEN		2
static const U8 LED_Test_Test1_Buf[LED_CTRL_BUFF_DEPTH][LED_TEST_TEST1_LEN] = 
{	
	{0, 0},		//LED1_GREEN, ���ڵ�ɫLEDֻ�趨��LED_GREEN�Ŀ��Ʒ�ʽ���ɣ�LED_RED�Ķ����LED����û��Ӱ��
	{0, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{1, 0},		//LED3_GREEN
	{1, 0},		//LED3_RED
	{0, 0},		//LED4_GREEN
	{0, 0},		//LED4_RED
	{0, 0},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{1, 1},		//LED3_TIME
	{0, 0},	    //LED4_TIME
};

/*lq ������Ե�2��壨ѭ����Red LED ��250ms����750ms�� */
#define LED_TEST_TEST2_LEN		2
static const U8 LED_Test_Test2_Buf[LED_CTRL_BUFF_DEPTH][LED_TEST_TEST2_LEN] = 
{	
	{0, 0},		//LED1_GREEN, ���ڵ�ɫLEDֻ�趨��LED_GREEN�Ŀ��Ʒ�ʽ���ɣ�LED_RED�Ķ����LED����û��Ӱ��
	{0, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{1, 0},		//LED3_GREEN
	{1, 0},		//LED3_RED
	{0, 0},		//LED4_GREEN
	{0, 0},		//LED4_RED
	{0, 0},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{1, 3},		//LED3_TIME
	{0, 0},	    //LED4_TIME
};

/*lq ������Ե�3��壨ѭ����Red LED ��250ms����1250ms�� */
#define LED_TEST_TEST3_LEN		2
static const U8 LED_Test_Test3_Buf[LED_CTRL_BUFF_DEPTH][LED_TEST_TEST3_LEN] = 
{	
	{0, 0},		//LED1_GREEN, ���ڵ�ɫLEDֻ�趨��LED_GREEN�Ŀ��Ʒ�ʽ���ɣ�LED_RED�Ķ����LED����û��Ӱ��
	{0, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{1, 0},		//LED3_GREEN
	{1, 0},		//LED3_RED
	{0, 0},		//LED4_GREEN
	{0, 0},		//LED4_RED
	{0, 0},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{1, 5},		//LED3_TIME
	{0, 0},	    //LED4_TIME
};

/*lq ������Ե�4��壨ѭ����Red LED ��250ms����1750ms�� */
#define LED_TEST_TEST4_LEN		2
static const U8 LED_Test_Test4_Buf[LED_CTRL_BUFF_DEPTH][LED_TEST_TEST4_LEN] = 
{	
	{0, 0},		//LED1_GREEN, ���ڵ�ɫLEDֻ�趨��LED_GREEN�Ŀ��Ʒ�ʽ���ɣ�LED_RED�Ķ����LED����û��Ӱ��
	{0, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{1, 0},		//LED3_GREEN
	{1, 0},		//LED3_RED
	{0, 0},		//LED4_GREEN
	{0, 0},		//LED4_RED
	{0, 0},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{1, 7},		//LED3_TIME
	{0, 0},	    //LED4_TIME
};

/*lq ���ղ��Ի��壨ѭ����Yellow LED ��500ms����500ms�� */
#define LED_TEST_RX_LEN		2
static const U8 LED_TEST_RX_Buf[LED_CTRL_BUFF_DEPTH][LED_TEST_RX_LEN] = 
{	
	{0, 0},		//LED1_GREEN, ���ڵ�ɫLEDֻ�趨��LED_GREEN�Ŀ��Ʒ�ʽ���ɣ�LED_RED�Ķ����LED����û��Ӱ��
	{0, 0},		//LED1_RED
	{1, 0},		//LED2_GREEN
	{1, 0},		//LED2_RED
	{0, 0},		//LED3_GREEN
	{0, 0},		//LED3_RED
	{0, 0},		//LED4_GREEN
	{0, 0},		//LED4_RED
	{0, 0},	    //LED1_TIME
	{2, 2},		//LED2_TIME
	{0, 0},		//LED3_TIME
	{0, 0},	    //LED4_TIME
};

/*lq �豸�Լ���л��壨ѭ����Blue LED ��250ms����4750ms�� */
#define LED_SELF_CHECK_IDLE_LEN		2
static const U8 LED_SELF_CHECK_IDLE_Buf[LED_CTRL_BUFF_DEPTH][LED_SELF_CHECK_IDLE_LEN] = 
{	
	{0, 0},		//LED1_GREEN, ���ڵ�ɫLEDֻ�趨��LED_GREEN�Ŀ��Ʒ�ʽ���ɣ�LED_RED�Ķ����LED����û��Ӱ��
	{0, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{0, 0},		//LED3_GREEN
	{0, 0},		//LED3_RED
	{1, 0},		//LED4_GREEN
	{1, 0},		//LED4_RED
	{0, 0},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{0, 0},		//LED3_TIME
	{1, 19},	//LED4_TIME
};

/*lq �豸�Լ��л��壨ѭ����Blue LED ��250ms����250ms�� */
#define LED_SELF_CHECK_RUNNING_LEN		2
static const U8 LED_SELF_CHECK_RUNNING_Buf[LED_CTRL_BUFF_DEPTH][LED_SELF_CHECK_RUNNING_LEN] = 
{	
	{0, 0},		//LED1_GREEN, ���ڵ�ɫLEDֻ�趨��LED_GREEN�Ŀ��Ʒ�ʽ���ɣ�LED_RED�Ķ����LED����û��Ӱ��
	{0, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{0, 0},		//LED3_GREEN
	{0, 0},		//LED3_RED
	{1, 0},		//LED4_GREEN
	{1, 0},		//LED4_RED
	{0, 0},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{0, 0},		//LED3_TIME
	{1, 1},	    //LED4_TIME
};

/*lq �豸�Լ��������壨ѭ����Blue LED ������ */
#define LED_SELF_CHECK_OK_LEN		1
static const U8 LED_SELF_CHECK_OK_Buf[LED_CTRL_BUFF_DEPTH][LED_SELF_CHECK_OK_LEN] = 
{	
	{0},		//LED1_GREEN, ���ڵ�ɫLEDֻ�趨��LED_GREEN�Ŀ��Ʒ�ʽ���ɣ�LED_RED�Ķ����LED����û��Ӱ��
	{0},		//LED1_RED
	{0},		//LED2_GREEN
	{0},		//LED2_RED
	{0},		//LED3_GREEN
	{0},		//LED3_RED
	{1},		//LED4_GREEN
	{1},		//LED4_RED
	{0},	    //LED1_TIME
	{0},		//LED2_TIME
	{0},		//LED3_TIME
	{1},	    //LED4_TIME
};

/*lq �豸�Լ���󻺳壨ѭ����Red LED ������ */
#define LED_SELF_CHECK_ERR_LEN		1
static const U8 LED_SELF_CHECK_ERR_Buf[LED_CTRL_BUFF_DEPTH][LED_SELF_CHECK_ERR_LEN] = 
{	
	{0},		//LED1_GREEN, ���ڵ�ɫLEDֻ�趨��LED_GREEN�Ŀ��Ʒ�ʽ���ɣ�LED_RED�Ķ����LED����û��Ӱ��
	{0},		//LED1_RED
	{0},		//LED2_GREEN
	{0},		//LED2_RED
	{1},		//LED3_GREEN
	{1},		//LED3_RED
	{0},		//LED4_GREEN
	{0},		//LED4_RED
	{0},	    //LED1_TIME
	{0},		//LED2_TIME
	{1},		//LED3_TIME
	{0},	    //LED4_TIME
};

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* ����  : InitaLEDPara
* ����  : ��ʼ����LED���Ʋ���
* ����  : pLEDPara��LED��������
          pBuf�����ƻ���
          ucLen�����峤��
          ReCycle���Ƿ�ѭ�����
          priority: ���ȼ�
          ucType������
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitaLEDPara(LEDStruct *pLEDPara, U8 *pBuf, U8 ucLen, U8 ReCycle,
                  U8 priority, U8 ucType)
{
	U8  ucGreenOffset;
	U8  ucRedOffset;
	U8  ucTimerOffset;

	if (pLEDPara == &LEDCtrlPara.LedGreenPara)
	{
		ucGreenOffset = LED1_GREEN_OFFSET * ucLen;
		ucRedOffset = LED1_RED_OFFSET * ucLen;
		ucTimerOffset = LED1_TIMER_OFFSET * ucLen;
	}
	else if (pLEDPara == &LEDCtrlPara.LedYellowPara)
	{
		ucGreenOffset = LED2_GREEN_OFFSET * ucLen;
		ucRedOffset = LED2_RED_OFFSET * ucLen;
		ucTimerOffset = LED2_TIMER_OFFSET * ucLen;
	}
	else if (pLEDPara == &LEDCtrlPara.LedRedPara)
	{
		ucGreenOffset = LED3_GREEN_OFFSET * ucLen;
		ucRedOffset = LED3_RED_OFFSET * ucLen;
		ucTimerOffset = LED3_TIMER_OFFSET * ucLen;
	}
	else
	{
		ucGreenOffset = LED4_GREEN_OFFSET * ucLen;
		ucRedOffset = LED4_RED_OFFSET * ucLen;
		ucTimerOffset = LED4_TIMER_OFFSET * ucLen;
	}

	pLEDPara->LEDPriority = priority;
	pLEDPara->LEDReCycleFlag = ReCycle;
	pLEDPara->pLEDGreen = &pBuf[ucGreenOffset];
	pLEDPara->pLEDRed = &pBuf[ucRedOffset];
	pLEDPara->pLEDTimer = &pBuf[ucTimerOffset];
	pLEDPara->ucType = ucType;
	pLEDPara->ucBufLen = ucLen;
	pLEDPara->ucLedFragmentTimer = 0;
	pLEDPara->ucLedFragment = 0;

	if (ucType == LED_TYPE_NONE)
	{
		pLEDPara->LEDEn = FALSE;
	}
	else
	{
		pLEDPara->LEDEn = TRUE;
	}
}

/*******************************************************************************
* ����  : InitaLEDPara
* ����  : ��ʼ����LED���Ʋ���
* ����  : pLEDPara��LED����		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitaLEDCtrlPara(void)
{
	LEDCtrlPara.TimeCnt = 0;
	LEDCtrlPara.TimePeriod = 50;

	LEDCtrlPara.TimeCnt = SetTimePeriodProcess(LEDCtrlPara.TimePeriod);
	
	InitaLEDPara(&LEDCtrlPara.LedGreenPara, 
				(U8 *)LED_None_Buf, 
				LED_NONE_LEN, 
				LED_RECYCLE_NONE,
                LED_CTRL_PRIORITY_L, 
				LED_TYPE_NONE);
	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
				(U8 *)LED_None_Buf, 
				LED_NONE_LEN, 
				LED_RECYCLE_NONE, 
                LED_CTRL_PRIORITY_L, 
				LED_TYPE_NONE);
	InitaLEDPara(&LEDCtrlPara.LedRedPara, 
				(U8 *)LED_None_Buf, 
				LED_NONE_LEN, 
				LED_RECYCLE_NONE, 
                LED_CTRL_PRIORITY_L, 
				LED_TYPE_NONE);
	InitaLEDPara(&LEDCtrlPara.LedBluePara, 
				(U8 *)LED_None_Buf, 
				LED_NONE_LEN, 
				LED_RECYCLE_NONE, 
                LED_CTRL_PRIORITY_L, 
				LED_TYPE_NONE);
}

/*******************************************************************************
* ����  : LEDInit
* ����  : ��ʼ��LED���Ʊ�Ҫ�Ĳ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void LEDInit(U8 ucType)
{
    
	switch(ucType)
	{
		/*lq ---------------------- LED1 ---------------------------*/
		case LED_TYPE_GREEN_NONE :
		{
			/* ������ִ�� */
	    	IO_Ctrl(IO_LED_GREEN_TYPE, IO_VAL_OFF);
	    	InitaLEDPara(&LEDCtrlPara.LedGreenPara, 
	    				(U8 *)LED_None_Buf, 
	    				LED_NONE_LEN, 
	    				LED_RECYCLE_NONE,
	                    LED_CTRL_PRIORITY_L, 
	    				ucType);
			break;
		}	
	    case LED_TYPE_POWER_NORMAL :
		{
			/*lq 
	         * ֮ǰ���е����ȼ����ڻ���ڵ�ǰ���ȼ����ҿ���������֮ǰ�Ĳ�ͬ��
	         * ������ִ�У���������
	         */
			if ((LEDCtrlPara.LedGreenPara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_POWER_NORMAL))
			{
	        	IO_Ctrl(IO_LED_GREEN_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedGreenPara, 
							(U8 *)LED_PowerNormal_Buf, 
							LED_POWER_NORMAL_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
		case LED_TYPE_POWER_TEST :
		{
			/*lq 
	         * ֮ǰ���е����ȼ����ڻ���ڵ�ǰ���ȼ����ҿ���������֮ǰ�Ĳ�ͬ��
	         * ������ִ�У���������
	         */
			if ((LEDCtrlPara.LedGreenPara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_POWER_TEST))
			{
	        	IO_Ctrl(IO_LED_GREEN_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedGreenPara, 
							(U8 *)LED_PowerTest_Buf, 
							LED_POWER_TEST_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
	    
	    /*lq ---------------------- LED2 ---------------------------*/
		case LED_TYPE_YELLOW_NONE :
		{
			/* ������ִ�� */
	    	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	    	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	    				(U8 *)LED_None_Buf, 
	    				LED_NONE_LEN, 
	    				LED_RECYCLE_NONE,
	                    LED_CTRL_PRIORITY_L, 
	    				ucType);
			break;
		}	
		case LED_TYPE_GPS_SEARCH :
		{
			/*lq 
	         * ֮ǰ���е����ȼ����ڻ���ڵ�ǰ���ȼ����ҿ���������֮ǰ�Ĳ�ͬ��
	         * ������ִ�У���������
	         */
			if ((LEDCtrlPara.LedYellowPara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedYellowPara.ucType != LED_TYPE_GPS_SEARCH))                
			{
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
							(U8 *)LED_GPS_Search_Buf, 
							LED_GPS_SEARCH_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3, 
							ucType);
			
	            if (LEDCtrlPara.LedBluePara.ucType == LED_TYPE_GPS_FIX)
	            {
	            	/*lq �ر�GPS��λָʾ */
	                IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
	            	InitaLEDPara(&LEDCtrlPara.LedBluePara, 
	            				(U8 *)LED_None_Buf, 
	            				LED_NONE_LEN, 
	            				LED_RECYCLE_NONE,
	                            LED_CTRL_PRIORITY_L, 
	            				ucType);
	            }
	        }
			break;
		}
		case LED_TYPE_TIMEOUT :
		{
			/*lq 
	         * ֮ǰ���е����ȼ����ڻ���ڵ�ǰ���ȼ����ҿ���������֮ǰ�Ĳ�ͬ��
	         * ������ִ�У���������
	         */
			if ((LEDCtrlPara.LedYellowPara.LEDPriority >= LED_CTRL_PRIORITY0)
	            && (LEDCtrlPara.LedYellowPara.ucType != LED_TYPE_TIMEOUT))
			{
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
							(U8 *)LED_Timeout_Buf, 
							LED_TIMEOUT_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY0,
							ucType);
			}
			break;
		}
		case LED_TYPE_TEST_RX :
		{
			/*lq 
	         * ֮ǰ���е����ȼ����ڻ���ڵ�ǰ���ȼ����ҿ���������֮ǰ�Ĳ�ͬ��
	         * ������ִ�У���������
	         */
			if ((LEDCtrlPara.LedYellowPara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedYellowPara.ucType != LED_TYPE_TEST_RX))
			{       	
	    		/* LED3�ر� */
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedRedPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	    		/* LED4�ر� */
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedBluePara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
							(U8 *)LED_TEST_RX_Buf, 
							LED_TEST_RX_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
	    
	    /*lq ---------------------- LED3 ---------------------------*/
		case LED_TYPE_RED_NONE :
		{
			/* ������ִ�� */
	    	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
	    	InitaLEDPara(&LEDCtrlPara.LedRedPara, 
	    				(U8 *)LED_None_Buf, 
	    				LED_NONE_LEN, 
	    				LED_RECYCLE_NONE,
	                    LED_CTRL_PRIORITY_L, 
	    				ucType);
			break;
		}	
		case LED_TYPE_RF_XMT :
		{
			/*lq 
	         * ���Σ�֮ǰ���е����ȼ����ڻ���ڵ�ǰ���ȼ���������ִ�У���������
	         */
			if (LEDCtrlPara.LedRedPara.LEDPriority >= LED_CTRL_PRIORITY0)
			{
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedRedPara, 
							(U8 *)LED_RF_Xmt_Buf, 
							LED_RF_XMT_LEN, 
							LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY0,
							ucType);
			}
			break;
		}
		case LED_TYPE_ERR :
		{
			/*lq 
	         * ֮ǰ���е����ȼ����ڻ���ڵ�ǰ���ȼ����ҿ���������֮ǰ�Ĳ�ͬ��
	         * ������ִ�У���������
	         */
			if ((LEDCtrlPara.LedRedPara.LEDPriority >= LED_CTRL_PRIORITY0)
	            && (LEDCtrlPara.LedRedPara.ucType != LED_TYPE_ERR))
			{
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedRedPara, 
							(U8 *)LED_Err_Buf, 
							LED_ERR_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY0,
							ucType);
			}
			break;
		}
		case LED_TYPE_TEST_TEST1 :
		{
			/*lq 
	         * ֮ǰ���е����ȼ����ڻ���ڵ�ǰ���ȼ����ҿ���������֮ǰ�Ĳ�ͬ��
	         * ������ִ�У���������
	         */
			if ((LEDCtrlPara.LedRedPara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedRedPara.ucType != LED_TYPE_TEST_TEST1))
			{
	    		/*lq LED2�ر� */
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	    		/*lq LED4�ر� */
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedBluePara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedRedPara, 
							(U8 *)LED_Test_Test1_Buf, 
							LED_TEST_TEST1_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
		case LED_TYPE_TEST_TEST2 :
		{
			/*lq 
	         * ֮ǰ���е����ȼ����ڻ���ڵ�ǰ���ȼ����ҿ���������֮ǰ�Ĳ�ͬ��
	         * ������ִ�У���������
	         */
			if ((LEDCtrlPara.LedRedPara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedRedPara.ucType != LED_TYPE_TEST_TEST2))
			{
	    		/*lq LED2�ر� */
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	    		/*lq LED4�ر� */
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedBluePara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedRedPara, 
							(U8 *)LED_Test_Test2_Buf, 
							LED_TEST_TEST2_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
		case LED_TYPE_TEST_TEST3 :
		{
			/*lq 
	         * ֮ǰ���е����ȼ����ڻ���ڵ�ǰ���ȼ����ҿ���������֮ǰ�Ĳ�ͬ��
	         * ������ִ�У���������
	         */
			if ((LEDCtrlPara.LedRedPara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedRedPara.ucType != LED_TYPE_TEST_TEST3))
			{
	    		/*lq LED2�ر� */
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	    		/*lq LED4�ر� */
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedBluePara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedRedPara, 
							(U8 *)LED_Test_Test3_Buf, 
							LED_TEST_TEST3_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
		case LED_TYPE_TEST_TEST4 :
		{
			/*lq 
	         * ֮ǰ���е����ȼ����ڻ���ڵ�ǰ���ȼ����ҿ���������֮ǰ�Ĳ�ͬ��
	         * ������ִ�У���������
	         */
			if ((LEDCtrlPara.LedRedPara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedRedPara.ucType != LED_TYPE_TEST_TEST4))
			{
	    		/*lq LED2�ر� */
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	    		/*lq LED4�ر� */
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedBluePara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedRedPara, 
							(U8 *)LED_Test_Test4_Buf, 
							LED_TEST_TEST4_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
		case LED_TYPE_SELF_CHECK_ERR :
		{
			/*lq 
	         * ֮ǰ���е����ȼ����ڻ���ڵ�ǰ���ȼ����ҿ���������֮ǰ�Ĳ�ͬ��
	         * ������ִ�У���������
	         */
			if ((LEDCtrlPara.LedRedPara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedRedPara.ucType != LED_TYPE_SELF_CHECK_ERR))
			{
	    		/*lq LED2�ر� */
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	    		/*lq LED4�ر� */
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedBluePara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedRedPara, 
							(U8 *)LED_SELF_CHECK_ERR_Buf, 
							LED_SELF_CHECK_ERR_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
	    
	    /*lq ---------------------- LED4 ---------------------------*/
		case LED_TYPE_BLUE_NONE :
		{
			/* ������ִ�� */
	    	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
	    	InitaLEDPara(&LEDCtrlPara.LedBluePara, 
	    				(U8 *)LED_None_Buf, 
	    				LED_NONE_LEN, 
	    				LED_RECYCLE_NONE,
	                    LED_CTRL_PRIORITY_L, 
	    				ucType);
			break;
		}	
		case LED_TYPE_GPS_FIX :
		{
			/*lq 
	         * ֮ǰ���е����ȼ����ڻ���ڵ�ǰ���ȼ����ҿ���������֮ǰ�Ĳ�ͬ��
	         * ������ִ�У���������
	         */
			if ((LEDCtrlPara.LedBluePara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedBluePara.ucType != LED_TYPE_GPS_FIX))
			{
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedBluePara, 
							(U8 *)LED_GPS_Fix_Buf, 
							LED_GPS_FIX_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3, 
							ucType);
			
	            if (LEDCtrlPara.LedYellowPara.ucType == LED_TYPE_GPS_SEARCH)
	            {
	            	/*lq �ر�LED����ָʾ */
	                IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	            	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	            				(U8 *)LED_None_Buf, 
	            				LED_NONE_LEN, 
	            				LED_RECYCLE_NONE,
	                            LED_CTRL_PRIORITY_L, 
	            				ucType);           
	            }
	        }
			break;
		}
		case LED_TYPE_SELF_CHECK_IDLE :
		{
			/*lq 
	         * ֮ǰ���е����ȼ����ڻ���ڵ�ǰ���ȼ����ҿ���������֮ǰ�Ĳ�ͬ��
	         * ������ִ�У���������
	         */
			if ((LEDCtrlPara.LedBluePara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedBluePara.ucType != LED_TYPE_SELF_CHECK_IDLE))
			{
	    		/*lq LED2�ر� */
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	    		/*lq LED3�ر� */
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedRedPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedBluePara, 
							(U8 *)LED_SELF_CHECK_IDLE_Buf, 
							LED_SELF_CHECK_IDLE_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
		case LED_TYPE_SELF_CHECK_RUNNING :
		{
			/*lq 
	         * ֮ǰ���е����ȼ����ڻ���ڵ�ǰ���ȼ����ҿ���������֮ǰ�Ĳ�ͬ��
	         * ������ִ�У���������
	         */
			if ((LEDCtrlPara.LedBluePara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedBluePara.ucType != LED_TYPE_SELF_CHECK_RUNNING))
			{
	    		/*lq LED2�ر� */
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	    		/*lq LED3�ر� */
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedRedPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedBluePara, 
							(U8 *)LED_SELF_CHECK_RUNNING_Buf, 
							LED_SELF_CHECK_RUNNING_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
		case LED_TYPE_SELF_CHECK_OK :
		{
			/*lq 
	         * ֮ǰ���е����ȼ����ڻ���ڵ�ǰ���ȼ����ҿ���������֮ǰ�Ĳ�ͬ��
	         * ������ִ�У���������
	         */
			if ((LEDCtrlPara.LedBluePara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedBluePara.ucType != LED_TYPE_SELF_CHECK_OK))
			{
	    		/*lq LED2�ر� */
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	    		/*lq LED3�ر� */
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedRedPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedBluePara, 
							(U8 *)LED_SELF_CHECK_OK_Buf, 
							LED_SELF_CHECK_OK_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
	
	    /*lq --------------- LED1/LED2/LED3/LED4 -------------------*/
		default:
		{
			/* ������ִ�� */
			InitaLEDCtrlPara();
			LEDAllDown();
			break;
		}
	}
}

/*******************************************************************************
* ����  : LEDAllLight
* ����  : LEDȫ������
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void LEDAllLight(void)
{
#if !WIN32
	IO_Ctrl(IO_LED_GREEN_TYPE, IO_VAL_ON);
	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_ON);
	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_ON);
	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_ON);
#endif
}

/*******************************************************************************
* ����  : LEDAllDown
* ����  : LEDȫ���ر�
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void LEDAllDown(void)
{
#if !WIN32
	IO_Ctrl(IO_LED_GREEN_TYPE, IO_VAL_OFF);
	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
#endif
}

/*******************************************************************************
* ����  : LedCtrl
* ����  : ��ʼ����LED���Ʋ���
* ����  : pLEDPara��LED��������
* ���  : ��
* ����  : ��
*******************************************************************************/
void LedCtrl(LEDStruct *pLEDPara)
{
	U8 ucTempGreen;
//	U8 ucTempRed;

	if (pLEDPara->LEDEn == TRUE)
	{
		if (pLEDPara->pLEDGreen[pLEDPara->ucLedFragment] > 0)
		{
			ucTempGreen = IO_VAL_ON;
		}
		else
		{
			ucTempGreen = IO_VAL_OFF;
		}

		if (pLEDPara->pLEDRed[pLEDPara->ucLedFragment] > 0)
		{
			;//ucTempRed = IO_VAL_ON;
		}
		else
		{
			//;ucTempRed = IO_VAL_OFF;
		}

		pLEDPara->ucLedFragmentTimer++;

		if (pLEDPara->ucLedFragmentTimer >= pLEDPara->pLEDTimer[pLEDPara->ucLedFragment])
		{
			pLEDPara->ucLedFragmentTimer = 0;
			pLEDPara->ucLedFragment++;

			if (pLEDPara->ucLedFragment >= pLEDPara->ucBufLen)
			{
				pLEDPara->ucLedFragment = 0;

				if (pLEDPara->LEDReCycleFlag == LED_RECYCLE_NONE)
				{
					pLEDPara->LEDEn = FALSE;
                    pLEDPara->LEDPriority = LED_CTRL_PRIORITY_L;
				}
			}
		}
#if !WIN32
		if (pLEDPara == &LEDCtrlPara.LedGreenPara)
		{
			IO_Ctrl(IO_LED_GREEN_TYPE, ucTempGreen);    //lq ���ڵ�ɫLEDֻ�����LED_GREEN�Ķ��巽ʽ���п��Ƽ���
		}
		else if (pLEDPara == &LEDCtrlPara.LedYellowPara)
		{
			IO_Ctrl(IO_LED_YELLOW_TYPE, ucTempGreen);
		}
		else if (pLEDPara == &LEDCtrlPara.LedRedPara)
		{
			IO_Ctrl(IO_LED_RED_TYPE, ucTempGreen);
		}
		else
		{
			IO_Ctrl(IO_LED_BLUE_TYPE, ucTempGreen);
		}		
#endif			
	}
}
/*******************************************************************************
* ����  : LEDCtrlProcess
* ����  : LED����������
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void LEDCtrlProcess(void)
{
    /* 250ms��ʱ */
	if (LEDCtrlPara.TimeCnt == GetLocalClockTickProcess())
	{
		LEDCtrlPara.TimeCnt = SetTimePeriodProcess(LEDCtrlPara.TimePeriod);

		/* LED���� */
		LedCtrl(&LEDCtrlPara.LedGreenPara);
		LedCtrl(&LEDCtrlPara.LedYellowPara);
		LedCtrl(&LEDCtrlPara.LedRedPara);
		LedCtrl(&LEDCtrlPara.LedBluePara);
	}
}
#if 0
/*******************************************************************************
* ����  : LEDCtrlTest
* ����  : LED���Ժ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 LEDTestFlag = 0;
void LEDCtrlTest(void)
{
	if (LEDTestFlag == 1)
	{
		LEDTestFlag = 0;
		LEDInit(LED_TYPE_GPS_SEARCH);	
	}
	else if (LEDTestFlag == 2)
	{
		LEDTestFlag = 0;
		LEDInit(LED_TYPE_GPS_FIX);	
	}
	else if (LEDTestFlag == 3)
	{
		LEDTestFlag = 0;
		LEDInit(LED_TYPE_RF_XMT);	
	}
	else if (LEDTestFlag == 7)
	{
		LEDTestFlag = 0;
		LEDInit(LED_TYPE_TEST_TEST1);	
	}
	else if (LEDTestFlag == 8)
	{
		LEDTestFlag = 0;
		LEDInit(LED_TYPE_TEST_TEST2);	
	}
}
#endif

/*******************************************************************************
* ����  : LEDReadFileProcess
* ����  : ��ȡSD���ļ�LED������
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void LEDReadFileProcess(void)
{
	/* 250ms��ʱ */
	if (LEDCtrlPara.TimeCnt == GetLocalClockTickProcess())
	{
		LEDCtrlPara.TimeCnt = SetTimePeriodProcess(LEDCtrlPara.TimePeriod);

		/* LED����,ֻ���̵ƽ��п��� */
		LedCtrl(&LEDCtrlPara.LedGreenPara);
	}
}
