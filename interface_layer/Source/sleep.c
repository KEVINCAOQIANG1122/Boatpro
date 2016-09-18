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
#include "config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define OS_INIT_TASK_STACK_SIZE		128			/* ��ʼ�������ջ��С 	*/ 
/* Private variables ---------------------------------------------------------*/
OS_STK	InitTaskStk[OS_INIT_TASK_STACK_SIZE];	/* ��ʼ�������ջ 		*/
/* Private  function  --------------------------------------------------------*/
extern void  application_init (void);
extern void  init_task_core(void *pdata);
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/

/*******************************************************************************
* ����  : application_init
* ����  : ����Ӧ�ó�ʼ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void application_init(void)
{
	DCB_init();	/* ע����ʼ�� */
	
	//********�м��ģ��������ʼ��********
	//�������ݴ���ģ���ʼ�� 
	comm1_init();
	comm1_handle = DCB_open(COMM1_ID, 0);

	//����IO�������ģ���ʼ��
	IO_set_init();
	IO_set_handle = DCB_open(IO_SET_ID,0);

	//Һ����ʾģ���ʼ��
	lcd_interface_init();
	LCD_handle = DCB_open(LCD_ID,0);

	//���������Լ�ģ���ʼ��
	mach_dete_init();
	mach_dete_handle = DCB_open(MACH_DETE_ID, 0);

	//��������ģ���ʼ��
	bell_init();
	bell_handle = DCB_open(BELL_ID, 0);

	//SCT3252������ͨ��ģ���ʼ��
	SCT3252_init();
	SCT3252_handle = DCB_open(SCT3252_ID, 0);

	//Flash����ģ��
	SpiFlash_Init();
	Flash_handle = DCB_open(FLASH_ID, 0);

	//¼������ģ��
	record_interface_init();
	Record_handle = DCB_open(RECORD_ID, 0);

	//������
	moto_interface_init();
	Moto_handle = DCB_open(MOTO_ID, 0);
}


/*******************************************************************************
* ����  : main
* ����  : ��ڳ���
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
int main(void)
{
	#if (OS_TASK_NAME_SIZE >= 16) 
		INT8U err;
	#endif

	//��ʼ��C��
	setlocale(LC_ALL,"C");

  	Target_Init();			/* Ŀ�������ʼ�� */

  	OSInit();				/* ��ʼ��OS 	  */

  	Tmr_TickInit();			/* ��ʼ��OS Tick  */
  
  	OSTaskCreateExt(init_task_core, (void *)0, (OS_STK *)&InitTaskStk[OS_INIT_TASK_STACK_SIZE - 1],
					OS_TASK_INIT_PRIO,
					OS_TASK_INIT_PRIO,
					(OS_STK *)&InitTaskStk[0],
					OS_INIT_TASK_STACK_SIZE,
					(void *)0,
					OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);	
					
	#if (OS_TASK_NAME_SIZE >= 16)
		OSTaskNameSet(OS_TASK_IDLE_PRIO, (INT8U *)"Idle task", &err);
		OSTaskNameSet(OS_TASK_INIT_PRIO, (INT8U *)"Init task", &err);
	#endif

	OSStart();				/* ���������񻷾� */
	 
  	return(0);
}

/***************************************************************************************
** ��������: init_task_core
** ��������: ��ʼ��������ĺ�����(1) ��ʼ��Ӧ��  (2) ��ʼ������ϵͳ���
** ��    ��: *pdata
** �� �� ֵ: None       
** ����  ��:  
** ��  ����:  
**--------------------------------------------------------------------------------------
** �� �� ��: 
** �ա�  ��: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
static void init_task_core(void *pdata)
{
	pdata = pdata;				/* ��ֹ���������� 				*/
	 
	//create_os_task(); 			/* ����Ӧ���еĴ�������� 		*/

	create_power_on_task();		/* ����Ӧ���еĿ������� 		*/

	create_task_status();		/* ���ó�ʼ��״̬��Ҫ��������� */
	
	create_os_semphore();		/* ����Ӧ���еĴ�����ź��� 	*/

	create_os_mutex();			/* ����Ӧ���еĴ���������� 	*/

	create_os_mailbox();		/* ����Ӧ���еĴ����������Ϣ 	*/

	create_os_queue();			/* ����Ӧ���еĴ����������Ϣ 	*/

	create_os_timer();			/* ����Ӧ���еĴ�ඨʱ�� 		*/
	
	create_os_group();			/* ����Ӧ���еĴ��ͨ���¼��� 	*/

	mem_init();					/* ����Ӧ���еĴ�����ڴ���� 	*/	
	 		
	application_init();			/* ��ʼ��Ӧ���в���				*/
//	Wdg_Init();					/* ���ڿ��Ź���ʼ�� 			*/		
		
	while(1)
	{
		UartDebugMsg(ARM_DEBUG_MSG, 0, "/----------------------------------------");	
		UartDebugMsg(ARM_DEBUG_MSG, 0, "TM208 version : V%d.%d.%.2d", 
					(ARM_STATE&0xF000)>>24,
					(ARM_STATE&0x0F00)>>16,
					(ARM_STATE&0x00FF));

		OSTaskSuspend(OS_PRIO_SELF);	/* �����ʼ������ 		*/
	}	
}

