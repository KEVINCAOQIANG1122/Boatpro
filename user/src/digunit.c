/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: initial.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    ��ʼ����������ȫ��ģ��Ĳ����ڴ˴����г�ʼ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include "def.h"
#include "debug.h"
#include "initial.h"
#include "LinkPtlPort.h"
#include "LinkPtlReport.h"
#include "gmskmod.h"
#include "digunit.h"
#include "ledctrl.h"
#if !WIN32
#include "ioctrl.h"
#endif
#include "gmskmod.h"
#include "tim.h"
#include "wdg.h"
#if 1 //lqdb
#include "led.h"
#endif
#include "LinkManagementEntity.h"
/*lqdb
U8 ucBefore = 0;//lqdb
U8 ucBefore2 = 0;
char arrBefore[50];//lqdb
U8 ucAfter = 0;//lqdb
U8 ucAfter2 = 0;//lqdb

char arrAfter[50];//lqdb
*/

DigUnitStru	DigUnitPara;
/*******************************************************************************
* ����  : DigDealMain
* ����  : ��ʼ����Ҫ�Ĳ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitaDigDeal(void)
{
	DigUnitPara.ModSta = DIGUNIT_MOD_IDLE;
	DigUnitPara.TranEnd = FALSE;
}

#if 0
/*******************************************************************************
* ����  : DigDealMain
* ����  : ��ʼ����Ҫ�Ĳ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void DigDealUnit(void)
{

	U8 ucChk;

	if (DigUnitPara.ModSta == DIGUNIT_MOD_IDLE)
	{
		//��ѯ�ӿڻ������Ƿ�������
		ucChk = ChkPtlFifoSta();

		if (ucChk == TRUE)
		{
			DigUnitPara.ModSta = DIGUNIT_MOD_STEP1;
		}

	}
	else if (DigUnitPara.ModSta == DIGUNIT_MOD_STEP1)
	{

		//1846���ƣ�GMSK����
		ucChk = GMSKTran();                                                     //lq ��1����ϢΪ��λ���е���

		if (ucChk == TRUE)                                                      //lq ���Ƴɹ�
		{
			DigUnitPara.ModSta = DIGUNIT_MOD_STEP2;                             
#if !WIN32
			LEDInit(LED_TYPE_RF_XMT);                                           //lq ��Ϣ����LEDָʾ
#endif
		}
		else
		{
			DigUnitPara.ModSta = DIGUNIT_MOD_IDLE;
		}

      
	}
	else if (DigUnitPara.ModSta == DIGUNIT_MOD_STEP2)
	{
		//�ȴ��������ݷ������
		if (FSKDataXmtStru.uiTran_Sta == FSKDATA_TRAN_IDLE)
		{
			//DigUnitPara.TranEnd = FALSE;

			//�ر�1846
			RDA1846Ctrl(DRIVER_1846_POWERDOWN, eqEquipPara.uiXmtFreq);          //lq 1����Ϣ(���ƺ��)������ɺ󣬱�ر�RDA1846
			DigUnitPara.ModSta = DIGUNIT_MOD_IDLE;
		}

	}
	else
	{
		DigUnitPara.ModSta = DIGUNIT_MOD_IDLE;
	}
                               
  
}
#endif

#if 0
/*******************************************************************************
* ����  : DigDealUnit_ITDMA
* ����  : ITDMA��ʽ����Ϣ��������Ϣ�Ĵ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void DigDealUnit_ITDMA(void)
{
    U8 ucCheck;

	if (DigUnitPara.ModSta == DIGUNIT_MOD_IDLE)
    {
        while (!IsFullOfFSKDataXmt() && ChkPtlFifoSta())							//lq ���ƶ��в�������Ϣ���зǿգ����Լ�������
    	{                
            ucCheck = GMSKTran_ITDMA();                                                       //lq ��Ϣ�����л���δ���Ƶ���Ϣ
    
            if (ucCheck == TRUE)
            {
                if (!GetFetchFirstFskDataEn())
                {
                    SetFetchFirstFskDataEn();
                }        
            }
        } 
        
        while (ChkPtlFifoSta())                                                     //lq ��Ϣ�����л���δ���Ƶ���Ϣ,����������
        {
            //lq... ������Ч��Ϣ�ų�����       
    		LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].MessState = FALSE; //lq ��ʶЭ��ջ�еĸ���Ϣ�ѷ���
    
            LinkPtlFifoPara.FifoOut++;
    		LinkPtlFifoPara.FifoOut &= LINKPTL_FIFOLEN;        
        }                                                                          //lq ����Ϣ�����е�������Ϣ��������󣬲ŷ��͵��ƶ����е��ƺ����Ϣ
    
        if (GetFetchFirstFskDataEn())
        {
            ResetFetchFirstFskDataEn();
            if (FIFOOutOfFSKDataXmt(&pFSKDataXmtStru2))                                  //lq+ ��ȡ���ƶ����е�һ��Ҫ���͵ĵ�����Ϣ
            {
                SetFSKDataXmtEn();
                
                /*lq ��1846*/
#if 0
                RDA1846Ctrl(DRIVER_1846_XMT, pFSKDataXmtStru2->uiXmtFreq + eqEquipPara.slXmtFreqComp, RDA1846_CS_2);//lq ����1846���͵�������ǰ������RDA1846
#endif                
                DigUnitPara.ModSta = DIGUNIT_MOD_STEP1;        
            }
            else
            {
        //lq-        ResetFSKDataXmtEn();
            }        
        }        
           
    }
    else if (DigUnitPara.ModSta == DIGUNIT_MOD_STEP1)
    {
		//�ȴ��������ݷ������
		if (GetFSKDataXmtEn() == FALSE)
		{
#if 0
			//�ر�1846
			RDA1846Ctrl(DRIVER_1846_POWERDOWN, eqEquipPara.uiXmtFreq, RDA1846_CS_2);          //lq 1����Ϣ(���ƺ��)������ɺ󣬱�ر�RDA1846
#endif	
    		DigUnitPara.ModSta = DIGUNIT_MOD_IDLE;
		}    
    }
    else
    {
        DigUnitPara.ModSta = DIGUNIT_MOD_IDLE;                                  
    }
}
#endif

/*******************************************************************************
* ����  : DigDealUnit
* ����  : ����Ϣ��������ȡ��Ϣ����BinMsg�ķ��Ͷ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void DigDealUnit(void)
{
    BinMsgTxCellStruct *pBinMsgTxCell;        
    LinkPtlStruc linkPtlCell;
    S16 i;

    /*lq ����Ϣ������ȡ��Ϣ */
    if (LinkPtlPortDeque(&linkPtlCell) == TRUE)
    {
        pBinMsgTxCell = (BinMsgTxCellStruct *)malloc(sizeof(BinMsgTxCellStruct));
        memset((BinMsgTxCellStruct *)pBinMsgTxCell, 0, sizeof(BinMsgTxCellStruct));
        
        pBinMsgTxCell->TxChannel = linkPtlCell.ChannelNum;
        pBinMsgTxCell->TxSlot = linkPtlCell.TranSlotNum;
        pBinMsgTxCell->DataLen = linkPtlCell.DataLength;
        for (i = 0; i < pBinMsgTxCell->DataLen; i++)
        {
            pBinMsgTxCell->Data[i] = linkPtlCell.DataBuf[i];
        }

        LME_InstBinMsgTxFifoEnque(pBinMsgTxCell);

        free((BinMsgTxCellStruct *)pBinMsgTxCell);
    }

}
/*******************************************************************************
* ����  : GetDigUnitSleepSta
* ����  : ��ȡ���ݴ���ģ��״̬
* ����  : ��		  
* ���  : ��
* ����  : TRUE��ʾ���Խ���ʡ�磬FALSE��ֹ����ʡ��
*******************************************************************************/
U8 GetDigUnitSleepSta(void)
{
	if (DigUnitPara.ModSta == DIGUNIT_MOD_IDLE)
	{
		return (TRUE);
	}		
	else
	{			   
		return (FALSE);
	}
}
/*******************************************************************************
* ����  : DigDealMain
* ����  : ��ʼ����Ҫ�Ĳ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void DigDealMain(void)
{
	Ptl_Main();                                                                 //lq �ӽ������GPS��Ϣ����ȡ��Ϣ����x����Ϣ��������������Ϣ����Э��ջ

	DigDealUnit();
    
	ReportToPtl(); //�˴����뷴������ģ��

}

/*******************************************************************************
* ����  : GetDigUnitSta
* ����  : ����DigUnit�Ĺ���״̬��������ģ��ʹ��
* ����  : ��		  
* ���  : ��
* ����  : TRUE:�����������FALSE�������������
*******************************************************************************/
U8 GetDigUnitSta(void)
{
	if (FSKDataXmtStru.uiTran_Sta == FSKDATA_TRAN_IDLE)
	{
		return (FALSE);
	}
	else
	{
		return (TRUE);
	}
}

#if 0
/*******************************************************************************
* ����  : GetDigUnitSta
* ����  : ����DigUnit�Ĺ���״̬��������ģ��ʹ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GMSKTran(void)
{
	U8 ucChk;

	if (LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].MessState == TRUE)	//lq? Ϊ�ζԴ��ж�
	{
		LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].MessState = FALSE; //lq ��ʶЭ��ջ�еĸ���Ϣ�ѷ���

		//�˴�����1846���ƺ����������ŵ��÷���Ƶ��
		if (LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].ChannelNum == 'A')
		{
			//��161.975Ƶ��
			eqEquipPara.uiXmtFreq = EQ_XMTFREQ_CHA;
		}
		else
		{
			//��162.025Ƶ��
			eqEquipPara.uiXmtFreq = EQ_XMTFREQ_CHB;
		}
		
		//lq RDA1846Ctrl(DRIVER_1846_XMT, eqEquipPara.uiXmtFreq);
		RDA1846Ctrl(DRIVER_1846_XMT, eqEquipPara.uiXmtFreq + eqEquipPara.slXmtFreqComp);//lq �����ݵ���ǰ������RDA1846

		//GMSK����
		ucChk = GMSKModMain(&LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].DataBuf[0], 
						LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].TranSlotNum, 
						LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].DataLength, 
						eqEquipPara.uiModAmp, 
						eqEquipPara.uiFreqOffset);
		
		LinkPtlFifoPara.FifoOut++;
		LinkPtlFifoPara.FifoOut &= LINKPTL_FIFOLEN;

		if (ucChk == TRUE)
		{
			return (TRUE);
		}
		else
		{
			RDA1846Ctrl(DRIVER_1846_POWERDOWN, eqEquipPara.uiXmtFreq);
			return (FALSE);
		}
		
	}

	return (FALSE);
}
#endif

/*******************************************************************************
* ����  : GMSKTran_ITDMA
* ����  : ITDMA��ʽ����Ϣ��������Ϣ�ĵ���,��Ϣ�����е���Ϣ��������ɺ��ٷ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GMSKTran_ITDMA(void)
{
	U8 ucChk;

	if (LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].MessState == TRUE)	//lq? Ϊ�ζԴ��ж�
	{
//lq		LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].MessState = FALSE; //lq ��ʶЭ��ջ�еĸ���Ϣ�ѷ���

		//�˴�����1846���ƺ����������ŵ��÷���Ƶ��
		if (LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].ChannelNum == 'A')
		{
			//��161.975Ƶ��
			eqEquipPara.uiXmtFreq = EQ_XMTFREQ_CHA;
		}
		else
		{
			//��162.025Ƶ��
			eqEquipPara.uiXmtFreq = EQ_XMTFREQ_CHB;
		}
		
		//lq RDA1846Ctrl(DRIVER_1846_XMT, eqEquipPara.uiXmtFreq);
//lq-			RDA1846Ctrl(DRIVER_1846_XMT, eqEquipPara.uiXmtFreq + eqEquipPara.slXmtFreqComp);//lq �����ݵ���ǰ������RDA1846

		//GMSK����
        #if 1
/*lqdb
        sprintf(arrBefore,"arrBefore2=%d",++ucBefore2);
		UartResponseMsg(arrBefore);
*/        
 
    	ucChk = GMSKModMain_ITDMA(&LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].DataBuf[0], 
        							LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].TranSlotNum, 
        							LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].DataLength, 
        							eqEquipPara.uiModAmp, 
        							eqEquipPara.uiFreqOffset,
                                    eqEquipPara.uiXmtFreq);
             
/*lqdb        
        sprintf(arrBefore,"arrAfter2=%d",++ucAfter2);
		UartResponseMsg(arrBefore);
*/
		#endif
//lqdb        FIFOInOfFSKDataXmt(FSKDataXmtStru);										//lq ����ǰ�������ݼ�����ƶ���
                                                                                //lq �˺��������и���bug����Ӱ��Uart1RxFIFO�ĳ�Ա������ֵ why?
        if (ucChk == TRUE)
        {
            FIFOInOfFSKDataXmt();
		    
            LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].MessState = FALSE; //lq ��ʶЭ��ջ�еĸ���Ϣ�ѷ���
            LinkPtlFifoPara.FifoOut++;                                              
    		LinkPtlFifoPara.FifoOut &= LINKPTL_FIFOLEN;
            
    		UartResponseMsg("$MessState_TRUE!");//lqdb 
            
            return TRUE;
        }
        else
        {
        }
	}
    else
    {
        //lq... ������Ч��Ϣ�ų�����       
		LinkPtlFifoPara.FifoOut++;
		LinkPtlFifoPara.FifoOut &= LINKPTL_FIFOLEN;
 
        UartResponseMsg("$MessState_FALSE!");//lqdb
    }

	return FALSE;
}

#if 0
/*******************************************************************************
* ����  : RDA1846Ctrl
* ����  : RDA1846����
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void RDA1846Ctrl(U8 Sta, U32 ulFreq, U8 cs)
{
	if (Sta == DRIVER_1846_XMT)
	{
		//����1846Ϊ����״̬
#if !WIN32
        if (cs == RDA1846_CS_1)
        {
    		//�˴����뻽��1846
    		IO_Ctrl(IO_RDA1846_1_TYPE, IO_VAL_HIGH);                                  //lq ����RDA1846
    	
    		//�˴����뻽��1846
    		IO_Write(IO_RDA1846_1_TYPE, ulFreq);                                      //lq ����RDA1846�ķ���Ƶ��      
        }
        else
        {
    		//�˴����뻽��1846
    		IO_Ctrl(IO_RDA1846_2_TYPE, IO_VAL_HIGH);                                  //lq ����RDA1846
    	
    		//�˴����뻽��1846
    		IO_Write(IO_RDA1846_2_TYPE, ulFreq);                                      //lq ����RDA1846�ķ���Ƶ��      
        
        }
#endif
	}
	else
	{
		//����1846Ϊʡ��״̬
#if !WIN32
        if (cs == RDA1846_CS_1)
        {
            //�˴���������1846����ʡ��״̬
    		IO_Ctrl(IO_RDA1846_1_TYPE, IO_VAL_LOW);        
        }
        else
        {
    		IO_Ctrl(IO_RDA1846_2_TYPE, IO_VAL_LOW);                
        }		
#endif
	}
}
#endif
