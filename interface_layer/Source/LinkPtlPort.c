/****************************************************************************
*  Copyright (C), 2012, 712.Co.,Ltd.                                        *
*  FileName: LinkPtlReport.c 				                                *
*  Description:  �ײ���Э��ջ�����ӿ�ʵ��									*
*  Function List:   					                                    *
*	1--  void InitaReportPara (void);							     		*
*****************************************************************************
*  History:               				                                    *
*      <author>  <time>    <version >   <desc>                              *
*      vozhj   	 12/01/08     1.0       build this moudle		            *
****************************************************************************/

#include "def.h"
#include "LinkPtlPort.h"
#include "Ptl_Interface.h"

LinkPtlFifoStruc LinkPtlFifoPara;
//extern MessManaStr mmMessCell;
/************************************************************
*  Function:       InitaReportPara							*
*  Description:    ��ʼ��Report����							*
*  Input:          ��										*	
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void InitaLinkPtlPortPara (void)
{
	U8 i, j;

	LinkPtlFifoPara.FifoIn = 0;
	LinkPtlFifoPara.FifoOut = 0;

	for (i = 0; i < (LINKPTL_FIFOLEN + 1); i++)
	{
		LinkPtlFifoPara.LinkPtlPara[i].MessState = FALSE;                       //lq ��ʼ��������Ϣ��״̬Ϊ�ѷ���(���ɷ���)
		LinkPtlFifoPara.LinkPtlPara[i].TranSlotNum = 0;
		LinkPtlFifoPara.LinkPtlPara[i].ChannelNum = 'A';
		for (j = 0; j < LINKPTL_DATABUF_LEN; j++)                               //lq ��ʼ��Ĭ����Ϣ����ȫΪ0
		{
			LinkPtlFifoPara.LinkPtlPara[i].DataBuf[j] = 0;
		}
		LinkPtlFifoPara.LinkPtlPara[i].DataLength = 0;                          //lq ��ʼ��Ĭ����Ϣ����Ϊ0
	}
}

#if 0
/************************************************************
*  Function:       LinkPtlPort								*
*  Description:    Э��ջ������Ϣ����						*
*  Input:          ��										*	
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void LinkPtlPort (void)
{
	U16 i;

	if (mmMessCell.MessState == TRUE)
	{
		mmMessCell.MessState = FALSE;

		//FIFO�Ƿ��пռ�
		if ((LinkPtlFifoPara.FifoIn + 1) != LinkPtlFifoPara.FifoOut)
		{
			LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].MessState = TRUE;

			LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].TranSlotNum = mmMessCell.TranSlotNum;
			LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].ChannelNum = mmMessCell.ChannelNum;

			LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].DataLength = mmMessCell.DataLength;
			for (i = 0; i < LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].DataLength; i++)
			{
				LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].DataBuf[i] = (U8)mmMessCell.pData[i];
			}
			LinkPtlFifoPara.FifoIn ++;
			LinkPtlFifoPara.FifoIn &= LINKPTL_FIFOLEN;
		}
	}
}
#endif

/************************************************************
*  Function:       LinkPtlPort								*
*  Description:    Э��ջ��Ϣ��Žӿ�						*
*  Input:          MessManaStr *mmMessCell Э��ջ��Ϣ�ṹ��										*	
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void LinkPtlPort (MessManaStr *mmMessCell)
{
	U16 i;

	if (mmMessCell->MessState == TRUE)
	{
		mmMessCell->MessState = FALSE;

		//FIFO�Ƿ��пռ�
		if ((LinkPtlFifoPara.FifoIn + 1) != LinkPtlFifoPara.FifoOut)
		{
			
			LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].MessState = TRUE;   //lq ��Ǹ�����Ϣ���Է��ͣ�����δ���ͣ�

			LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].TranSlotNum = mmMessCell->TranSlotNum;
			LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].ChannelNum = mmMessCell->ChannelNum;

			LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].DataLength = mmMessCell->DataLength;
			for (i = 0; i < LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].DataLength; i++)
			{
				LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].DataBuf[i] = (U8)mmMessCell->pData[i];
			}

			LinkPtlFifoPara.FifoIn ++;
			LinkPtlFifoPara.FifoIn &= LINKPTL_FIFOLEN;
		}
	}
}

/************************************************************
*  Function:       LinkPtlPortDeque							
*  Description:    Э��ջ��Ϣ��ȡ						
*  Input:          pLinkPtlStruc - Э��ջ��Ϣ�ṹ��ָ��	
*  Output:         ��										
*  Return:         TRUE - ���ӳɹ���FALSE - ����ʧ��		
*  Others:         ��										
************************************************************/
U8 LinkPtlPortDeque(LinkPtlStruc *pLinkPtlStruc)
{
	U16 i;

	/*lq �ж��Ƿ������� */
	if (IsLinkPtlQueEmpty() == FALSE)
	{
		pLinkPtlStruc->MessState = LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].MessState;   //lq ��Ǹ�����Ϣ���Է��ͣ�����δ���ͣ�
		pLinkPtlStruc->TranSlotNum = LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].TranSlotNum;
		pLinkPtlStruc->ChannelNum = LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].ChannelNum;
		pLinkPtlStruc->DataLength = LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].DataLength;
		for (i = 0; i < LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].DataLength; i++)
		{
			pLinkPtlStruc->DataBuf[i] = LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].DataBuf[i];
		}

		LinkPtlFifoPara.FifoOut ++;
		LinkPtlFifoPara.FifoOut &= LINKPTL_FIFOLEN;

        return TRUE;
	}
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : IsLinkPtlQueEmpty
* ����  : �����Ƿ�Ϊ��
* ����  : ��
* ���  : ��
* ����  : True - Ϊ�գ�FALSE - �ǿ�
*******************************************************************************/
U8 IsLinkPtlQueEmpty(void)
{
    if (LinkPtlFifoPara.FifoIn == LinkPtlFifoPara.FifoOut)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : IsLinkPtlQueFull
* ����  : �����Ƿ�����
* ����  : ��
* ���  : ��
* ����  : True - ������FALSE - δ��
*******************************************************************************/
U8 IsLinkPtlQueFull(void)
{
    if ((LinkPtlFifoPara.FifoIn + 1) % (LINKPTL_FIFOLEN + 1)
         == LinkPtlFifoPara.FifoOut)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* ����  : ChkPtlFifoSta
* ����  : ��ѯ�ӿ����Ƿ��������
* ����  : ��		  
* ���  : ��
* ����  : �ӿ�������״̬
*******************************************************************************/
U8 ChkPtlFifoSta(void)
{
	if (LinkPtlFifoPara.FifoOut != LinkPtlFifoPara.FifoIn)
	{
		return (TRUE);	
	}
	else
	{
		return (FALSE);
	}
}

/*******************************************************************************
* ����  : GetPtlFifoSleepSta
* ����  : ��ѯ�ӿ��Ƿ�ɽ���˯��
* ����  : ��		  
* ���  : ��
* ����  : �ӿ�������״̬
*******************************************************************************/
U8 GetPtlFifoSleepSta(void)
{
	if (LinkPtlFifoPara.FifoOut == LinkPtlFifoPara.FifoIn)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}		

/************************************************************
*  Function:       GetPtlSta								*
*  Description:    ��ѯЭ��ջ����״̬						*
*  Input:          ��										*	
*  Output:         ��										*
*  Return:         ��										*
*  Others:         Э��ջ״̬								*
************************************************************/
U8 GetPtlSta (void)
{
	return ((U8)mmMessCell.CurrentOperationFlg);
}

/************************************************************
*  Function:       DCSimPtl									*
*  Description:    �ײ����Э��ջ�ӿ�						*
*  Input:          ��										*	
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
U16 gucTestPtlBuf[70];
extern S16 gsiSlotCnt;	     //2250ʱ϶����
#define DCSIM_DATA_LEN		40
void DCSimPtl (void)
{
#if 0
	U16 i;

	mmMessCell.MessState = TRUE;
	mmMessCell.ChannelNum = 'A';
	mmMessCell.TranSlotNum = gsiSlotCnt + 2;

	for (i = 0; i < DCSIM_DATA_LEN; i++)
	{
		gucTestPtlBuf[i] = 0x55;
	}
	mmMessCell.DataLength = DCSIM_DATA_LEN;
	mmMessCell.pData = &gucTestPtlBuf[0];
#endif
}
























