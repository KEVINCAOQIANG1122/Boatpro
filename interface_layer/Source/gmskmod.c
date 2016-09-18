/****************************************************************************
*  Copyright (C), 2008, 712.Co.,Ltd.                                        *
*  FileName: GMSKMod.C                                            			*
*  Description :															*
*  Function List:   					                                    *
*	1-- S16  GMSKModMain(U16 *pData,U16 Length)						*	
*	1-- void BitFilter(U16 Sample)										*
*****************************************************************************
*  History:               				                                    *
*    <author>  <time>    <version >   <desc>                                *
*      Lee    	 08/07/30     1.0       build this moudle                   *
****************************************************************************/
#include "def.h"
#include "FIRstruc.h"
#include "gmskmod.h" 
#include "common.h"
#include "Tim.h"
#include "math_rand.h"
#if WIN32
#include "stdio.h"
#endif

#include "testing.h"


/********************�˲�����������********************/
/* coef */
const S16 Coef_IF_GaussianFilter[G_MOD_FILTER_SIZE] = 
{       
0x0000,0x0000,0x0000,0x0001,0x000B,0x0053,0x01AB,0x05FB,
0x0EDF,0x19B2,0x1ED5,0x19B2,0x0EDF,0x05FB,0x01AB,0x0053,
0x000B,0x0001,0x0000,0x0000,0x0000,
};

FSKDataXmtStruct FSKDataXmtStru;                                                //lq ������Ϣ��������ƶ���ת����Ϣ���м����    
//FSKDataXmtStruct FSKDataXmtStru2;                                               //lq ���ڴӵ��ƶ�����ȡ������Ϣ���͵��м����
FSKDataXmtStruct *pFSKDataXmtStru2;
FSKDataXmtFIFOStruct gstruFSKDataXmtFIFO;

U8  gucDataXmt_Fifo[FSKDATAXMT_FIFO_DEPTH + 1][FSKDATAXMT_FIFODATA_LEN];	//����֡ģ��FIFO�ӿڣ���һ��Byte��Ŵ����������ŵ��ţ��ڶ���Byte��Ŵ��������ݳ��ȣ���Ϊ����
U16 gucDataXmt_FifoIn;		//FIFO����ָ��
U16 gucDataXmt_FifoOut;

/* declare */
FIRStruct FirBuf;
S16 DlineBuf[G_MOD_FILTER_SIZE * 2];

const U32 FrameSequ = 0x33333301;

/*lqdb
U8 ucTemp = 0;
U8 ucTemp1 = 0;
char arrTemp[30];
*/
/************************************************************
*  Function:       ResetFSKDataTran							*
*  Description:    ��ʼ��FSK����Filter��������				*
*  Input:          pFSKDataXmtPara	���ݷ��ͽṹ��			*	
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void ResetFSKDataTran(FSKDataXmtStruct *pFSKDataXmtPara)
{
	U16 i;

	FirInit(&FirBuf, (S16 *)Coef_IF_GaussianFilter, DlineBuf, G_MOD_FILTER_SIZE);

	pFSKDataXmtPara->siRxTx_Slot = 0;
	pFSKDataXmtPara->siTxRx_Slot = 0;
	pFSKDataXmtPara->siDataStart_Slot = 0;
	pFSKDataXmtPara->siPowerUp_Slot = 0;
	pFSKDataXmtPara->siPowerDown_Slot = 0;

	for (i = 0; i < FSKDATAXMT_BUF_LEN; i++)
	{
		pFSKDataXmtPara->siDataXmtBuf[i] = 0;
	}
	pFSKDataXmtPara->uiDataXmtIndex = 0;
	pFSKDataXmtPara->uiDataLen = 0;
	pFSKDataXmtPara->uiModIndex	= 0;
}

/************************************************************
*  Function:       InitialFSKTranFIFO						*
*  Description:    ��ʼ�����ݷ���FIFO						*
*  Input:          ��										*	
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void InitialFSKTranFIFO(void)
{
	U16 i, j;

	for (i = 0; i < FSKDATAXMT_FIFO_DEPTH + 1; i++)
	{
		for (j = 0; j < FSKDATAXMT_FIFODATA_LEN; j++)
		{
			gucDataXmt_Fifo[i][j] = 0;
		}
	}

	gucDataXmt_FifoIn = 0;
	gucDataXmt_FifoOut = 0;
}

/************************************************************
*  Function:       InitFSKDataTranPara								*
*  Description:    ��ʼ��FSK���Ʋ�������					*
*  Input:          ��			
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void InitFSKDataTranPara(void)
{
	/*��ʼ���������˲���*/
	ResetFSKDataTran(&FSKDataXmtStru);

	FSKDataXmtStru.uiTran_Sta = FSKDATA_TRAN_IDLE;
	FSKDataXmtStru.uiFirt_En = FALSE;
	
	InitialFSKTranFIFO();
}

/*****************************************************************************
*    Function:	BitFilter													 *
*    Description : This is a GMSK mod  function.			    			 *
*    Input parameter:                                                        *
*       Sample	: ������BITλ												 *
*    Output parameter:  NONE                                                 *
*    Return value:    NONE                                                   *
******************************************************************************/
void BitFilter(FSKDataXmtStruct *pFSKDataXmtPara, U16 Sample, U16 uiModAmp, U16 uiOffset)
{
#if 1
	U16 k;
	S16 siTemp, siFirTemp;
	S16 siTempOut;

	if(Sample > 0)//�����ͷ��                                                  //lq '1'����
	{
		siTemp = 400;                                                           //lq?
	}
	else                                                                        //lq '0'����
	{
		siTemp = -400;
	}
	
	//�޸Ĵ�����
	for(k = 0; k < 5; k++) 
	{
		siFirTemp = FirFilter(&FirBuf, siTemp);//����ط������ǲ��㣬���Ǹ���   //lq FIR�˲���

		/* uiModAmp�ɵ�����Ƶƫ */
		siTempOut = (S16)(((S32)siFirTemp * uiModAmp + 16) >> 5);
		siTempOut *= 5;
		pFSKDataXmtPara->siDataXmtBuf[pFSKDataXmtPara->uiModIndex++] = siTempOut + uiOffset; //lq? ��ŵ��ƺ�����ݣ�
	}                                                                                        //lq 1λ�������ƣ���Ϊ16*5λ�����ƺ�
#endif
}

/*****************************************************************************
*    Function:	BitCurrentFilter													 *
*    Description : This is a GMSK mod  function.			    			 
					���˲���������ֱ������
*    Input parameter:                                                        *
*       Sample	: ������BITλ												 *
*    Output parameter:  NONE                                                 *
*    Return value:    NONE                                                   *
******************************************************************************/
void BitCurrentFilter(FSKDataXmtStruct *pFSKDataXmtPara, U16 Sample, U16 uiModAmp, U16 uiOffset)
{
	U16 k;

	for(k = 0; k < 5; k++)
	{
		pFSKDataXmtPara->siDataXmtBuf[pFSKDataXmtPara->uiModIndex++] = uiOffset;
	}
}

/*****************************************************************************
*    Function:	GMSKModTest                                             	 *
*    Description : This is a GMSK mod Main function.			    		 *
*    Input parameter:                                                        *
*       pData	: point to a buffer used by GMSK mod					     *
*       Length		:�������ݳ���										     *
*    Output parameter:  NONE                                                 *
*    Return value:                                                           *
*       �������ݺ��γ��� used by GMSK/NRZI                                 *
******************************************************************************/
#define FSKDATAXMT_POWERUPSLOT			(10)//(10)
#define FSKDATAXMT_DATASTARTSLOT		(40)//(20)
#define FSKDATAXMT_POWERDOWNSLOTOFFSET	(40)//(5)

U16 uiTempBuf[100];

struct math_rand send_seed;
void set_send_seed(U32 seed)
{
	SetRandSeed(&send_seed, seed);
}
U16 select_slot_to_send(void)
{
#if 0	
	S16 slot;
	
	slot = GetSlotNum();
	
	return (slot + 2) % 2250;
#else
	U16 slot;
	slot = GetSlotNum() + RandData(&send_seed, 2, 9);
	if (slot >= 2250)
	{
		slot = slot - 2250;
	}
	return slot;
#endif
}

#if 0
U8 GMSKModMain(U8 *pInBuf, S16 uiSlotNum, U16 uiLen, U16 uiModAmp, U16 uiOffset)
{	   			//�źŻ���		ʱ϶		���ݳ���		���Ʒ���	Ƶ��ƫ��
	S16 i,j;
	U16 Length;
	U16 uiTemp,uiTemp1,uiLast,uiHighIndex;
	U16 CRCVal;

	if (FSKDataXmtStru.uiTran_Sta == FSKDATA_TRAN_IDLE)                         //lq ǰһ����Ϣ�����ƺ�ģ�������ɺ�Ž�����һ����Ϣ�ĵ���
	{
		
		//iniat Gussion filter 
		ResetFSKDataTran(&FSKDataXmtStru);                                      //lq �ȳ�ʼ����ŵ��ƺ����ݵĽṹ��

		FSKDataXmtStru.siTran_Slot = uiSlotNum;                                 //lq? ����ʱ϶��ʹ�õ���Ĭ��ֵ0

		if (FSKDataXmtStru.siTran_Slot != FSKDATA_NOWSLOT)                      //lq? FSKDATA_NOWSLOT?
		{
			if (FSKDataXmtStru.siTran_Slot >= 2250)
			{
				return FALSE;	
			}
		}

		Length = uiLen;

		//���ȱ�������ֹ����Խ���������
		//û��������ʧ�ܴ���
		if(Length >= 65)
		{
			Length = 64;
		}
		else if(Length <= 1)
		{
			Length = 2;
		}

		//CRC
		CRCVal = AIS_Cal_CRC16(&pInBuf[0], Length);

		for(i = 0; i < Length; i++)
		{
			uiTempBuf[i] = pInBuf[i];//���ݴ��뻺��
		}

		uiTempBuf[Length] = ((CRCVal >> 8) & 0x00ff);//Ȼ��CRCֵ���������λ    //lq CRC�ĸ�λ
		uiTempBuf[Length+1] = (CRCVal & 0x00ff);                                //lq CRC�ĵ�λ

		//֡ͷ����
		for(i = 0; i < 32; i++)
		{
			uiTemp = (U16)((FrameSequ >> (31-i)) & 0x00000001);//���֡ͷ�ǹ̶���//lq �����ݣ��֣���ÿһλ����0����1�������е��ƣ� 
			BitFilter(&FSKDataXmtStru, uiTemp, uiModAmp, uiOffset);//32λ��֡ͷ  //lq ���ƺ�����ݷ���FSKDataXmtStru��siDataXmtBuf��
		}

		uiLast = 0x0001;
		uiHighIndex = 0;

		//���ݶε���
		for(i = 0; i < Length + 2; i++)//����֡�ĵ���                           //lq ���ݶ��к���2����Ԫ��CRC��
		{// for 1
			uiTemp1 = uiTempBuf[i];	  		//��ȡ���������ֽ�

			//��ȡ��������bitֵ���ɸ�λ����λ
			for(j = 0; j < 8; j++)                                              //lq �ֽ��ڣ��ɵ�λ����λ
			{//for2
				uiTemp = (uiTemp1 >> j) & 0x0001;//��ȡbitֵ
				if(uiTemp > 0)//���ֻ������ֵ 0 1
				{
					BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
					if(++uiHighIndex >= 5)//ֻҪ��1�ͻ��                       //lq ����5����1�����1����0��
					{	//4//����4��1�󣬵����1�ͷ�ת
						uiLast = ((uiLast & 0x0001) ^ 0x0001);//������Ƿ�ת������0->1 1->0
						uiHighIndex = 0;	//^��������
						BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
					}
				}
				else
				{	//����Ҳ�������㷭ת                                        //lq NRZI(�ǹ��㷴ת����)������0����ת������1������
					uiLast = ((uiLast & 0x0001) ^ 0x0001);//����Ƿ��ŷ�ת������0->1 1->0   //lq NRZI����
					uiHighIndex = 0;//����0����� 
					BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);                 //lq ��NRZI�����ÿһλ����(GMSK?)����
				}
			}//end for 2
		}//end for 1

		//֡β
		uiTemp1 = 0x007E;//�������֡β��?
		for(j = 0; j < 8; j++)
		{
			uiTemp = (uiTemp1 >> j) & 0x0001;
			if(uiTemp > 0)
			{
				BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
			}
			else
			{
				uiLast = ((uiLast & 0x0001) ^ 0x0001);
				BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
			}			
		}

		BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
		BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);


		FSKDataXmtStru.siPowerUp_Slot = FSKDATAXMT_POWERUPSLOT;		//���ݷ���ǰ��ǰ1.5+1.25ms��������
		FSKDataXmtStru.siDataStart_Slot = FSKDATAXMT_DATASTARTSLOT;		//ʱ϶��ʼ1.25+1.25msms,�������ݴ���ʱ�䴦��������
		FSKDataXmtStru.siPowerDown_Slot = FSKDATAXMT_POWERDOWNSLOTOFFSET;		//ʱ϶��ʼ1.25+1.25msms,�������ݴ���ʱ�䴦��������

		FSKDataXmtStru.uiDataLen = FSKDataXmtStru.uiModIndex;                   //lq ���ƺ�����ݳ��ȣ���λ-�֣�16b��

		FSKDataXmtStru.siTran_Slot = select_slot_to_send();                     //lq? ѡ����ʱ϶?
		FSKDataXmtStru.uiTran_Sta = FSKDATA_TRAN_START;//ʹ�����               //lq ���루���ƺ�ģ���Ϣ�ķ���״̬�Ŀ��ƣ������ƺ������ͨ��DAC����26M����Ȼ��������RDA1846�ľ�������


		return (TRUE);
	}
	
	return(FALSE);
}
#endif

U8 GMSKModMain_ITDMA(U8 *pInBuf, S16 uiSlotNum, U16 uiLen, U16 uiModAmp, U16 uiOffset, 	U32 uiXmtFreq)
{	   			//�źŻ���		ʱ϶		���ݳ���		���Ʒ���	Ƶ��ƫ��         ����Ƶ��
	S16 i,j;
	U16 Length;
	U16 uiTemp,uiTemp1,uiLast,uiHighIndex;
	U16 CRCVal;
	
    /*lqdb
    ucTemp++;
    */	

	if (!GetFSKDataXmtEn())                         //lq ǰһ����Ϣ�����ƺ�ģ�������ɺ�Ž�����һ����Ϣ�ĵ���
	{
    	//iniat Gussion filter 
    	ResetFSKDataTran(&FSKDataXmtStru);                                      //lq �ȳ�ʼ����ŵ��ƺ����ݵĽṹ��
    
    	FSKDataXmtStru.siTran_Slot = uiSlotNum;                                 //lq? ����ʱ϶��ʹ�õ���Ĭ��ֵ0
        FSKDataXmtStru.uiXmtFreq = uiXmtFreq;                                   //lq ���淢��Ƶ��
    /*
    	if (FSKDataXmtStru.siTran_Slot != FSKDATA_NOWSLOT)                      //lq? FSKDATA_NOWSLOT?
    	{
    		if (FSKDataXmtStru.siTran_Slot >= 2250)
    		{
    			return FALSE;	
    		}
    	}
    */
    	Length = uiLen;
    
    	//���ȱ�������ֹ����Խ���������
    	//û��������ʧ�ܴ���
    	if(Length >= 65)                                                            //lq �������Ϣ��󳤶�ΪLINKPTL_DATABUF_LEN = 65
    	{
    		Length = 64;
    	}
    	else if(Length <= 1)
    	{
    		Length = 2;
    	}
    
    	//CRC
    	CRCVal = AIS_Cal_CRC16(&pInBuf[0], Length);
    
    	for(i = 0; i < Length; i++)
    	{
    		uiTempBuf[i] = pInBuf[i];//���ݴ��뻺��
    	}
    
    	uiTempBuf[Length] = ((CRCVal >> 8) & 0x00ff);//Ȼ��CRCֵ���������λ    //lq CRC�ĸ�λ
    	uiTempBuf[Length+1] = (CRCVal & 0x00ff);                                //lq CRC�ĵ�λ
    
    	//֡ͷ����
    	for(i = 0; i < 32; i++)
    	{
    		uiTemp = (U16)((FrameSequ >> (31-i)) & 0x00000001);//���֡ͷ�ǹ̶���//lq �����ݣ��֣���ÿһλ����0����1�������е��ƣ� 
    		BitFilter(&FSKDataXmtStru, uiTemp, uiModAmp, uiOffset);//32λ��֡ͷ  //lq ���ƺ�����ݷ���FSKDataXmtStru��siDataXmtBuf��
    	}
    
    	uiLast = 0x0001;
    	uiHighIndex = 0;
    
    	//���ݶε���
    	for(i = 0; i < Length + 2; i++)//����֡�ĵ���                           //lq ���ݶ��к���2����Ԫ��CRC��
    	{// for 1
    		uiTemp1 = uiTempBuf[i];	  		//��ȡ���������ֽ�
    
    		//��ȡ��������bitֵ���ɸ�λ����λ
    		for(j = 0; j < 8; j++)                                              //lq �ֽ��ڣ��ɵ�λ����λ
    		{//for2
    			uiTemp = (uiTemp1 >> j) & 0x0001;//��ȡbitֵ
    			if(uiTemp > 0)//���ֻ������ֵ 0 1
    			{
    				BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
    				if(++uiHighIndex >= 5)//ֻҪ��1�ͻ��                       //lq ����5����1�����1����0��
    				{	//4//����4��1�󣬵����1�ͷ�ת
    					uiLast = ((uiLast & 0x0001) ^ 0x0001);//������Ƿ�ת������0->1 1->0
    					uiHighIndex = 0;	//^��������
    					BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
    				}
    			}
    			else
    			{	//����Ҳ�������㷭ת                                        //lq NRZI(�ǹ��㷴ת����)������0����ת������1������
    				uiLast = ((uiLast & 0x0001) ^ 0x0001);//����Ƿ��ŷ�ת������0->1 1->0   //lq NRZI����
    				uiHighIndex = 0;//����0����� 
    				BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);                 //lq ��NRZI�����ÿһλ����(GMSK?)����
    			}
    		}//end for 2
    	/*lqdb
        if (ucTemp > 1)
        {
            sprintf(arrTemp,"arrLen=%d",++ucTemp1); 
            UartResponseMsg(arrTemp);
        }
        */
        }//end for 1
    
    
    	//֡β
    	uiTemp1 = 0x007E;//�������֡β��?
    	for(j = 0; j < 8; j++)
    	{
    		uiTemp = (uiTemp1 >> j) & 0x0001;
    		if(uiTemp > 0)
    		{
    			BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
    		}
    		else
    		{
    			uiLast = ((uiLast & 0x0001) ^ 0x0001);
    			BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
    		}			
    	}
    
    	BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
    	BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
    
    
    	FSKDataXmtStru.siPowerUp_Slot = FSKDATAXMT_POWERUPSLOT;		//���ݷ���ǰ��ǰ1.5+1.25ms��������
    	FSKDataXmtStru.siDataStart_Slot = FSKDATAXMT_DATASTARTSLOT;		//ʱ϶��ʼ1.25+1.25msms,�������ݴ���ʱ�䴦��������
    	FSKDataXmtStru.siPowerDown_Slot = FSKDATAXMT_POWERDOWNSLOTOFFSET;		//ʱ϶��ʼ1.25+1.25msms,�������ݴ���ʱ�䴦��������
    
    	FSKDataXmtStru.uiDataLen = FSKDataXmtStru.uiModIndex;                   //lq ���ƺ�����ݳ��ȣ���λ-�֣�16b��
    
    //lq-	FSKDataXmtStru.siTran_Slot = select_slot_to_send();                     //lq? ѡ����ʱ϶?
    	FSKDataXmtStru.uiTran_Sta = FSKDATA_TRAN_START;//ʹ�����               //lq ���루���ƺ�ģ���Ϣ�ķ���״̬�Ŀ��ƣ������ƺ������ͨ��DAC����26M����Ȼ��������RDA1846�ľ�������
    
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************
*    Function:	GMSKModTest                                             	 *
*    Description : This is a GMSK mod Main function.			    		 *
*    Input parameter:                                                        *
*       pData	: point to a buffer used by GMSK mod					     *
*       Length		:�������ݳ���										     *
*    Output parameter:  NONE                                                 *
*    Return value:                                                           *
*       �������ݺ��γ��� used by GMSK/NRZI                                 *
******************************************************************************/
U8 GMSKModTest(U8 *pInBuf, S16 uiSlotNum, U16 uiLen, U16 uiModAmp, U16 uiOffset, U8 ucTestMod)
{
	S16 i,j;
	U16 Length;
	U16 uiTemp,uiTemp1,uiLast;
	
//lq-	if (FSKDataXmtStru.uiTran_Sta == FSKDATA_TRAN_IDLE)
	if (!GetFSKDataXmtEn())
    {		
		if (TestCtrlPara.FirstGenFlag == TRUE)//Ӧ���²����ź�					//lq ������ı䣬�������²���������ĵ����źţ�	
		{																		//lq �����ò�������״η���ʱ�����źţ��ò�����ĺ������������ٽ��е���
			TestCtrlPara.FirstGenFlag = FALSE;	
			
			//iniat Gussion filter 
			ResetFSKDataTran(&FSKDataXmtStru);
	
			FSKDataXmtStru.siTran_Slot = FSKDATA_NOWSLOT;
	
			Length = uiLen;
	
			//���ȱ�������ֹ����Խ���������
			//û��������ʧ�ܴ���
			if(Length >= 65)
			{
				Length = 64;
			}
			else if(Length <= 1)
			{
				Length = 2;
			}
	
			for(i = 0; i < Length; i++)
			{
				uiTempBuf[i] = pInBuf[i];
			}
	
			//���ݶε���
			for(i=0; i<Length; i++)
			{// for 1
				uiTemp1 = uiTempBuf[i];	  		//��ȡ���������ֽ�
	
				//��ȡ��������bitֵ���ɸ�λ����λ
				for(j = 0; j < 8; j++)
				{//for2
					uiTemp = (uiTemp1 >> (7 - j))&0x0001;
					if(uiTemp > 0)
					{
						uiLast = 0x0001;
	
						if (ucTestMod == 0)
						{
							BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
						}
						else
						{
							BitCurrentFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
						}
						
					}
					else
					{
						uiLast = 0x0000;
						if (ucTestMod == 0)
						{
							BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
						}
						else
						{
							BitCurrentFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
						}
					}
				}//end for 2
			}//end for 1
	
			if (ucTestMod == 0)
			{
				BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
				BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
			}
			else
			{
				BitCurrentFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
				BitCurrentFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
			}

		}

		FSKDataXmtStru.siPowerUp_Slot = FSKDATAXMT_POWERUPSLOT;		//���ݷ���ǰ��ǰ1.5+1.25ms��������
		FSKDataXmtStru.siDataStart_Slot = FSKDATAXMT_DATASTARTSLOT;		//ʱ϶��ʼ1.25+1.25msms,�������ݴ���ʱ�䴦��������
		FSKDataXmtStru.siPowerDown_Slot = FSKDATAXMT_POWERDOWNSLOTOFFSET;		//ʱ϶��ʼ1.25+1.25msms,�������ݴ���ʱ�䴦��������
		FSKDataXmtStru.siTran_Slot = FSKDATA_NOWSLOT;

		FSKDataXmtStru.uiDataLen = FSKDataXmtStru.uiModIndex;

		FSKDataXmtStru.uiTran_Sta = FSKDATA_TRAN_START;

		return (TRUE);
	}
	
	return(FALSE);
}

/*******************************************************************************
* ����  : GMSKModTest2
* ����  : ����֡ͷ֡β�Ĳ���̬�źŵ��ƺ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GMSKModTest2(U8 *pInBuf, S16 uiSlotNum, U16 uiLen, U16 uiModAmp, U16 uiOffset)
{	   			//�źŻ���		ʱ϶		���ݳ���		���Ʒ���	Ƶ��ƫ��
	S16 i,j;
	U16 Length;
	U16 uiTemp,uiTemp1,uiLast,uiHighIndex;
	U16 CRCVal;

//lq-	if (FSKDataXmtStru.uiTran_Sta == FSKDATA_TRAN_IDLE)
	if (!GetFSKDataXmtEn())
	{
			
		if (TestCtrlPara.FirstGenFlag == TRUE)//Ӧ���²����ź�
		{
			TestCtrlPara.FirstGenFlag = FALSE;	
		
			//iniat Gussion filter 
			ResetFSKDataTran(&FSKDataXmtStru);
	
			FSKDataXmtStru.siTran_Slot = uiSlotNum;
	
			if (FSKDataXmtStru.siTran_Slot != FSKDATA_NOWSLOT)
			{
				if (FSKDataXmtStru.siTran_Slot >= 2250)
				{
					return FALSE;	
				}
			}
	
			Length = uiLen;
	
			//���ȱ�������ֹ����Խ���������
			//û��������ʧ�ܴ���
			if(Length >= 65)
			{
				Length = 64;
			}
			else if(Length <= 1)
			{
				Length = 2;
			}
	
			//CRC
			CRCVal = AIS_Cal_CRC16(&pInBuf[0], Length);
	
			for(i = 0; i < Length; i++)
			{
				uiTempBuf[i] = pInBuf[i];//���ݴ��뻺��
			}
	
			uiTempBuf[Length] = ((CRCVal >> 8) & 0x00ff);//Ȼ��CRCֵ���������λ
			uiTempBuf[Length+1] = (CRCVal & 0x00ff);
	
			//֡ͷ����
			for(i = 0; i < 32; i++)
			{
				uiTemp = (U16)((FrameSequ >> (31-i)) & 0x00000001);//���֡ͷ�ǹ̶���
				BitFilter(&FSKDataXmtStru, uiTemp, uiModAmp, uiOffset);//32λ��֡ͷ
			}
	
			uiLast = 0x0001;
			uiHighIndex = 0;
	
			//���ݶε���
			for(i = 0; i < Length + 2; i++)//����֡�ĵ���
			{// for 1
				uiTemp1 = uiTempBuf[i];	  		//��ȡ���������ֽ�
	
				//��ȡ��������bitֵ���ɸ�λ����λ
				for(j = 0; j < 8; j++)
				{//for2
					uiTemp = (uiTemp1 >> j) & 0x0001;//��ȡbitֵ
					if(uiTemp > 0)//���ֻ������ֵ 0 1
					{
						BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
						if(++uiHighIndex >= 5)//ֻҪ��1�ͻ��
						{	//4//����4��1�󣬵����1�ͷ�ת
							uiLast = ((uiLast & 0x0001) ^ 0x0001);//������Ƿ�ת������0->1 1->0
							uiHighIndex = 0;	//^��������
							BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
						}
					}
					else
					{	//����Ҳ�������㷭ת
						uiLast = ((uiLast & 0x0001) ^ 0x0001);//����Ƿ��ŷ�ת������0->1 1->0
						uiHighIndex = 0;//����0����� 
						BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
					}
				}//end for 2
			}//end for 1
	
			//֡β
			uiTemp1 = 0x007E;//�������֡β��?
			for(j = 0; j < 8; j++)
			{
				uiTemp = (uiTemp1 >> j) & 0x0001;
				if(uiTemp > 0)
				{
					BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
				}
				else
				{
					uiLast = ((uiLast & 0x0001) ^ 0x0001);
					BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
				}			
			}
	
			BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
			BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);

		}

		FSKDataXmtStru.siPowerUp_Slot = FSKDATAXMT_POWERUPSLOT;		//���ݷ���ǰ��ǰ1.5+1.25ms��������
		FSKDataXmtStru.siDataStart_Slot = FSKDATAXMT_DATASTARTSLOT;		//ʱ϶��ʼ1.25+1.25msms,�������ݴ���ʱ�䴦��������
		FSKDataXmtStru.siPowerDown_Slot = FSKDATAXMT_POWERDOWNSLOTOFFSET;		//ʱ϶��ʼ1.25+1.25msms,�������ݴ���ʱ�䴦��������

		FSKDataXmtStru.siTran_Slot = FSKDATA_NOWSLOT;

		FSKDataXmtStru.uiDataLen = FSKDataXmtStru.uiModIndex;
		
		FSKDataXmtStru.uiTran_Sta = FSKDATA_TRAN_START;//ʹ�����


		return (TRUE);
	}
	
	return(FALSE);
}

/*******************************************************************************
* ����  : GMSKModTest2
* ����  : ����֡ͷ֡β����������NRZI����֡�����ݺ�У����Ĳ���̬�źŵ��ƺ���
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 GMSKModTest3(U8 *pInBuf, S16 uiSlotNum, U16 uiLen, U16 uiModAmp, U16 uiOffset)
{	   			//�źŻ���		ʱ϶		���ݳ���		���Ʒ���	Ƶ��ƫ��
	S16 i,j;
	U16 Length;
	U16 uiTemp,uiTemp1,uiLast,uiHighIndex;
	U16 CRCVal;

//lq-	if (FSKDataXmtStru.uiTran_Sta == FSKDATA_TRAN_IDLE)
	if (!GetFSKDataXmtEn())
	{
		if (TestCtrlPara.FirstGenFlag == TRUE)//Ӧ���²����ź�
		{
			TestCtrlPara.FirstGenFlag = FALSE;	
		
			//iniat Gussion filter 
			ResetFSKDataTran(&FSKDataXmtStru);
	
			FSKDataXmtStru.siTran_Slot = uiSlotNum;
	
			if (FSKDataXmtStru.siTran_Slot != FSKDATA_NOWSLOT)
			{
				if (FSKDataXmtStru.siTran_Slot >= 2250)
				{
					return FALSE;	
				}
			}
	
			Length = uiLen;
	
			//���ȱ�������ֹ����Խ���������
			//û��������ʧ�ܴ���
			if(Length >= 65)
			{
				Length = 64;
			}
			else if(Length <= 1)
			{
				Length = 2;
			}
	
			//CRC
			CRCVal = AIS_Cal_CRC16(&pInBuf[0], Length);
	
			for(i = 0; i < Length; i++)
			{
				uiTempBuf[i] = pInBuf[i];//���ݴ��뻺��
			}
	
			uiTempBuf[Length] = ((CRCVal >> 8) & 0x00ff);//Ȼ��CRCֵ���������λ
			uiTempBuf[Length+1] = (CRCVal & 0x00ff);
	
			//֡ͷ����
			for(i = 0; i < 32; i++)
			{
				uiTemp = (U16)((FrameSequ >> (31-i)) & 0x00000001);//���֡ͷ�ǹ̶���

				BitFilter(&FSKDataXmtStru, uiTemp, uiModAmp, uiOffset);//32λ��֡ͷ
			}
	
			uiLast = 0x0001;
			uiHighIndex = 0;
	
			//���ݶε���
			for(i = 0; i < Length + 2; i++)//����֡�ĵ���
			{// for 1
				uiTemp1 = uiTempBuf[i];	  		//��ȡ���������ֽ�
	
				//��ȡ��������bitֵ���ɸ�λ����λ
				for(j = 0; j < 8; j++)
				{//for2
					uiTemp = (uiTemp1 >> j) & 0x0001;//��ȡbitֵ
					if(uiTemp > 0)//���ֻ������ֵ 0 1
					{
//lq						BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
						BitFilter(&FSKDataXmtStru, 1, uiModAmp, uiOffset);
                       
                        uiLast = 1;
					
                    	if(++uiHighIndex >= 5)//ֻҪ��1�ͻ��
						{	//4//����4��1�󣬵����1�ͷ�ת
//lq							uiLast = ((uiLast & 0x0001) ^ 0x0001);//������Ƿ�ת������0->1 1->0
							uiHighIndex = 0;	//^��������
//lq							BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
							BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
                            
                            uiLast = 0;						
                        }
					}
					else
					{	//����Ҳ�������㷭ת
//lq						uiLast = ((uiLast & 0x0001) ^ 0x0001);//����Ƿ��ŷ�ת������0->1 1->0
						uiHighIndex = 0;//����0����� 
//lq						BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
						BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
					
                        uiLast = 0;
                    }
				}//end for 2
			}//end for 1
	
			//֡β
			uiTemp1 = 0x007E;//�������֡β��?
			for(j = 0; j < 8; j++)
			{
				uiTemp = (uiTemp1 >> j) & 0x0001;
				if(uiTemp > 0)
				{
					BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
				}
				else
				{
					uiLast = ((uiLast & 0x0001) ^ 0x0001);
					BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
				}			
			}
	
			BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
			BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
		}

		FSKDataXmtStru.siPowerUp_Slot = FSKDATAXMT_POWERUPSLOT;		//���ݷ���ǰ��ǰ1.5+1.25ms��������
		FSKDataXmtStru.siDataStart_Slot = FSKDATAXMT_DATASTARTSLOT;		//ʱ϶��ʼ1.25+1.25msms,�������ݴ���ʱ�䴦��������
		FSKDataXmtStru.siPowerDown_Slot = FSKDATAXMT_POWERDOWNSLOTOFFSET;		//ʱ϶��ʼ1.25+1.25msms,�������ݴ���ʱ�䴦��������

		FSKDataXmtStru.siTran_Slot = FSKDATA_NOWSLOT;

		FSKDataXmtStru.uiDataLen = FSKDataXmtStru.uiModIndex;
		
		FSKDataXmtStru.uiTran_Sta = FSKDATA_TRAN_START;//ʹ�����


		return (TRUE);
	}
	
	return(FALSE);
}

#if 0
/*****************************************************************************
*    Function:	GMSKModMain                                             	 *
*    Description : This is a GMSK mod Main function.			    		 *
*    Input parameter:                                                        *
*       pData	: point to a buffer used by GMSK mod					     *
*       Length		:�������ݳ���										     *
*    Output parameter:  NONE                                                 *
*    Return value:                                                           *
*       �������ݺ��γ��� used by GMSK/NRZI                                 *
******************************************************************************/
#define FSKDATAXMT_POWERUPSLOT		(30)
#define FSKDATAXMT_DATASTARTSLOT	(40)

U16 uiTempBuf[100];

U16 GMSKModMain(U16 uiModAmp)
{
	S16 i,j;
	U16 Length;
	U16 uiTemp,uiTemp1,uiLast,uiHighIndex;
	U16 CRCVal;

	if (gucDataXmt_FifoIn != gucDataXmt_FifoOut)
	{//if 0
		if (FSKDataXmtStru.uiTran_Sta == FSKDATA_TRAN_IDLE)			//��һ֡���ݵ�����ɣ����������ݵ���
		{//if 1
			//iniat Gussion filter 
			ResetFSKDataTran(&FSKDataXmtStru);                                  

			FSKDataXmtStru.siTran_Slot = ((gucDataXmt_Fifo[gucDataXmt_FifoOut][0] & 0xFF) << 8) 
										| (gucDataXmt_Fifo[gucDataXmt_FifoOut][1] & 0xFF);
			
			if (FSKDataXmtStru.siTran_Slot != FSKDATA_NOWSLOT)
			{
				if (FSKDataXmtStru.siTran_Slot >= 2250)
				{
					FSKDataXmtStru.siTran_Slot -= 2250;	
				}
			}
					
			Length = gucDataXmt_Fifo[gucDataXmt_FifoOut][2];

			//���ȱ�������ֹ����Խ���������
			//û��������ʧ�ܴ���
			if(Length >= 65)
			{
				Length = 64;
			}
			else if(Length <= 1)
			{
				Length = 2;
			}
		
			//CRC
			CRCVal = AIS_Cal_CRC16(&gucDataXmt_Fifo[gucDataXmt_FifoOut][3], Length);
		
			for(i = 0; i < Length; i++)
			{
				uiTempBuf[i] = gucDataXmt_Fifo[gucDataXmt_FifoOut][3 + i];
			}
			uiTempBuf[Length] = ((CRCVal >> 8) & 0x00ff);
			uiTempBuf[Length+1] = (CRCVal & 0x00ff);
		
			//֡ͷ����
			for(i = 0; i < 32; i++)
			{
				uiTemp = (U16)((FrameSequ >> (31-i)) & 0x00000001);
				BitFilter(&FSKDataXmtStru, uiTemp, uiModAmp);
			}
		
			uiLast = 0x0001;
			uiHighIndex = 0;
		
			//���ݶε���
			for(i = 0; i < Length + 2; i++)
			{// for 1
				uiTemp1 = uiTempBuf[i];	  		//��ȡ���������ֽ�
		
				//��ȡ��������bitֵ���ɸ�λ����λ
				for(j = 0; j < 8; j++)
				{//for2
					uiTemp = (uiTemp1 >> j) & 0x0001;
					if(uiTemp > 0)
					{
						BitFilter(&FSKDataXmtStru, uiLast, uiModAmp);
						if(++uiHighIndex >= 5)
						{
							uiLast = ((uiLast & 0x0001) ^ 0x0001);
							uiHighIndex = 0;
							BitFilter(&FSKDataXmtStru, uiLast, uiModAmp);
						}
					}
					else
					{
						uiLast = ((uiLast & 0x0001) ^ 0x0001);
						uiHighIndex = 0;
						BitFilter(&FSKDataXmtStru, uiLast, uiModAmp);
					}
				}//end for 2
			}//end for 1
			
			//֡β
			uiTemp1 = 0x007E;
			for(j = 0; j < 11; j++)
			{
				uiTemp = (uiTemp1 >> j) & 0x0001;
				if(uiTemp > 0)
				{
					BitFilter(&FSKDataXmtStru, uiLast, uiModAmp);
				}
				else
				{
					uiLast = ((uiLast & 0x0001) ^ 0x0001);
					BitFilter(&FSKDataXmtStru, uiLast, uiModAmp);
				}			
			}

			FSKDataXmtStru.siDataXmtBuf[FSKDataXmtStru.uiModIndex++] = 0;
			FSKDataXmtStru.siDataXmtBuf[FSKDataXmtStru.uiModIndex++] = 0;
			FSKDataXmtStru.siDataXmtBuf[FSKDataXmtStru.uiModIndex++] = 0;
			FSKDataXmtStru.siDataXmtBuf[FSKDataXmtStru.uiModIndex++] = 0;

			FSKDataXmtStru.siPowerUp_Slot = FSKDATAXMT_POWERUPSLOT;		//���ݷ���ǰ��ǰ1.5+1.25ms��������
			FSKDataXmtStru.siDataStart_Slot = FSKDATAXMT_DATASTARTSLOT;		//ʱ϶��ʼ1.25+1.25msms,�������ݴ���ʱ�䴦��������
			FSKDataXmtStru.siPowerDown_Slot = FSKDATAXMT_POWERDOWNSLOTOFFSET;		//ʱ϶��ʼ1.25+1.25msms,�������ݴ���ʱ�䴦��������

			FSKDataXmtStru.uiDataLen = FSKDataXmtStru.uiModIndex;
			FSKDataXmtStru.uiTran_Sta = FSKDATA_TRAN_START;

			gucDataXmt_FifoOut++;
 			gucDataXmt_FifoOut &= FSKDATAXMT_FIFO_DEPTH;
		}//end if 1
	}//end if 0
	 
	return(TRUE);
}


/*****************************************************************************
*    Function:	GMSKModTest                                             	 *
*    Description : This is a GMSK mod Main function.			    		 *
*    Input parameter:                                                        *
*       pData	: point to a buffer used by GMSK mod					     *
*       Length		:�������ݳ���										     *
*    Output parameter:  NONE                                                 *
*    Return value:                                                           *
*       �������ݺ��γ��� used by GMSK/NRZI                                 *
******************************************************************************/
U16 GMSKModTest(U16 uiModAmp)
{
	S16 i,j;
	U16 Length;
	U16 uiTemp,uiTemp1,uiLast;

	if (gucDataXmt_FifoIn != gucDataXmt_FifoOut)
	{//if 0
		if (FSKDataXmtStru.uiTran_Sta == FSKDATA_TRAN_IDLE)			//��һ֡���ݵ�����ɣ����������ݵ���
		{//if 1
			//iniat Gussion filter 
			ResetFSKDataTran(&FSKDataXmtStru);

			FSKDataXmtStru.siTran_Slot = FSKDATA_NOWSLOT;
					
			Length = gucDataXmt_Fifo[gucDataXmt_FifoOut][2];

			//���ȱ�������ֹ����Խ���������
			//û��������ʧ�ܴ���
			if(Length >= 65)
			{
				Length = 64;
			}
			else if(Length <= 1)
			{
				Length = 2;
			}
					
			for(i = 0; i < Length; i++)
			{
				uiTempBuf[i] = gucDataXmt_Fifo[gucDataXmt_FifoOut][3 + i];
			}
		
			//���ݶε���
			for(i=0; i<Length; i++)
			{// for 1
				uiTemp1 = uiTempBuf[i];	  		//��ȡ���������ֽ�
		
				//��ȡ��������bitֵ���ɸ�λ����λ
				for(j = 0; j < 8; j++)
				{//for2
					uiTemp = (uiTemp1 >> j)&0x0001;
					if(uiTemp > 0)
					{
						uiLast = 0x0001;
						BitFilter(&FSKDataXmtStru, uiLast, uiModAmp);
					}
					else
					{
						uiLast = 0x0000;
						BitFilter(&FSKDataXmtStru, uiLast, uiModAmp);
					}
				}//end for 2
			}//end for 1

			FSKDataXmtStru.siDataXmtBuf[FSKDataXmtStru.uiModIndex++] = 0;
			FSKDataXmtStru.siDataXmtBuf[FSKDataXmtStru.uiModIndex++] = 0;
			FSKDataXmtStru.siDataXmtBuf[FSKDataXmtStru.uiModIndex++] = 0;
			FSKDataXmtStru.siDataXmtBuf[FSKDataXmtStru.uiModIndex++] = 0;

			FSKDataXmtStru.siPowerUp_Slot = FSKDATAXMT_POWERUPSLOT;		//���ݷ���ǰ��ǰ1.5+1.25ms��������
			FSKDataXmtStru.siDataStart_Slot = FSKDATAXMT_DATASTARTSLOT;		//ʱ϶��ʼ1.25+1.25msms,�������ݴ���ʱ�䴦��������
			FSKDataXmtStru.siPowerDown_Slot = FSKDATAXMT_POWERDOWNSLOTOFFSET;		//ʱ϶��ʼ1.25+1.25msms,�������ݴ���ʱ�䴦��������

			FSKDataXmtStru.uiDataLen = FSKDataXmtStru.uiModIndex;
			FSKDataXmtStru.uiTran_Sta = FSKDATA_TRAN_START;

			gucDataXmt_FifoOut++;
 			gucDataXmt_FifoOut &= FSKDATAXMT_FIFO_DEPTH;
		}//end if 1
	}//end if 0
	 
	return(TRUE);
}

/************************************************************
*  Function:       FillFSKTranFIFO							*
*  Description:    ��ʱ϶�����ز�							*
*  Input:          pFSKDataXmtPara	���ݷ��ͽṹ��			*	
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
U8 FillGMSKTranFIFO(U8 *pInBuf, U16 uiSlotNum, U16 uiLen)
{
	U16 i, uiIndex;
	
	uiIndex = 0;

	if (((gucDataXmt_FifoIn + 1) & FSKDATAXMT_FIFO_DEPTH) != gucDataXmt_FifoOut)		//FIFO����
	{
		gucDataXmt_Fifo[gucDataXmt_FifoIn][uiIndex++] =	(U8)(uiSlotNum >> 8);
		gucDataXmt_Fifo[gucDataXmt_FifoIn][uiIndex++] =	(U8)(uiSlotNum & 0xFF);
		gucDataXmt_Fifo[gucDataXmt_FifoIn][uiIndex++] =	(U8)uiLen;

		for (i = 0; i < uiLen; i++)
		{
			gucDataXmt_Fifo[gucDataXmt_FifoIn][uiIndex++] = pInBuf[i];
		}

		gucDataXmt_FifoIn++;
		gucDataXmt_FifoIn &= FSKDATAXMT_FIFO_DEPTH;
		
		return TRUE;
	}

	return FALSE;
}
#endif

#if 0
/************************************************************
*  Function:       FIFOInOfFSKDataXmt						*
*  Description:    ���ƺ����Ϣ�ķ��Ͷ���-���				*
*  Input:          inFSKDataXmtPara	���ݷ��ͽṹ��			*	
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
U8 FIFOInOfFSKDataXmt(FSKDataXmtStruct inFSKDataXmtPara)
{
   if (((gstruFSKDataXmtFIFO.ucFIFOIn + 1) % FSKDATAXMTFIFO_DEPTH) != gstruFSKDataXmtFIFO.ucFIFOOut) //lq ��Ϣ������в���
   { 
        gstruFSKDataXmtFIFO.arrFIFOData[gstruFSKDataXmtFIFO.ucFIFOIn++] = inFSKDataXmtPara;
        
        gstruFSKDataXmtFIFO.ucFIFOIn %= FSKDATAXMTFIFO_DEPTH;

        return TRUE;
   }
   else
   {
        return FALSE;
   }
}
#endif

/************************************************************
*  Function:       FIFOInOfFSKDataXmt						*
*  Description:    ���ƺ����Ϣ�ķ��Ͷ���-���				*
*  Input:          ��                           			*	
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
U8 FIFOInOfFSKDataXmt(void)
{
   if (((gstruFSKDataXmtFIFO.ucFIFOIn + 1) % FSKDATAXMTFIFO_DEPTH) != gstruFSKDataXmtFIFO.ucFIFOOut) //lq ��Ϣ������в���
   { 
        gstruFSKDataXmtFIFO.arrFIFOData[gstruFSKDataXmtFIFO.ucFIFOIn++] = FSKDataXmtStru;
        
        gstruFSKDataXmtFIFO.ucFIFOIn %= FSKDATAXMTFIFO_DEPTH;

        return TRUE;
   }
   else
   {
        return FALSE;
   }
}

/************************************************************
*  Function:       FIFOOutOfFSKDataXmt						*
*  Description:    ���ƺ����Ϣ�ķ��Ͷ���-����				*
*  Input:          ��			*	
*  Output:         ��										*
*  Return:         ���ݷ��ͽṹ��							*
*  Others:         ��										*
************************************************************/
U8 FIFOOutOfFSKDataXmt(FSKDataXmtStruct **outFSKDataXmtPara)
{
   if (gstruFSKDataXmtFIFO.ucFIFOOut != gstruFSKDataXmtFIFO.ucFIFOIn)           //lq ��Ϣ������зǿ� 
   {
        *outFSKDataXmtPara = &gstruFSKDataXmtFIFO.arrFIFOData[gstruFSKDataXmtFIFO.ucFIFOOut++];

        gstruFSKDataXmtFIFO.ucFIFOOut %= FSKDATAXMTFIFO_DEPTH;

        return TRUE; 
   } 
   else
   {
        return FALSE;
   }   
}

/************************************************************
*  Function:       IsEmptyOfFSKDataXmt						*
*  Description:    �������ݶ����Ƿ�Ϊ��				        *
*  Input:          ��			                            *	
*  Output:         ��										*
*  Return:         �Ƿ�Ϊ��     							*
*  Others:         ��										*
************************************************************/
U8 IsEmptyOfFSKDataXmt(void)
{
   if (gstruFSKDataXmtFIFO.ucFIFOOut == gstruFSKDataXmtFIFO.ucFIFOIn)           //lq ��Ϣ������п� 
   {
        return TRUE; 
   } 
   else
   {
        return FALSE;
   }   
}

/************************************************************
*  Function:       IsFullOfFSKDataXmt						*
*  Description:    �������ݶ����Ƿ�����				        *
*  Input:          ��			                            *	
*  Output:         ��										*
*  Return:         �Ƿ�����     							*
*  Others:         ��										*
************************************************************/
U8 IsFullOfFSKDataXmt(void)
{
   if (gstruFSKDataXmtFIFO.ucFIFOOut == (gstruFSKDataXmtFIFO.ucFIFOIn + 1) % FSKDATAXMTFIFO_DEPTH)           //lq ��Ϣ���������
   {
        return TRUE; 
   } 
   else
   {
        return FALSE;
   }   
}

/************************************************************
*  Function:       InitFSKDataXmtFIFO						*
*  Description:    ��ʼ����Ϣ�������				        *
*  Input:          ��Ϣ�������			                    *	
*  Output:         ��										*
*  Return:         ��     							        *
*  Others:         ��										*
************************************************************/
void InitFSKDataXmtFIFO(FSKDataXmtFIFOStruct *pFSKDataXmtFIFO)
{
    U16 i;
    U16 j;
    pFSKDataXmtFIFO->ucFIFOIn = 0;
    pFSKDataXmtFIFO->ucFIFOOut = 0;
    pFSKDataXmtFIFO->ucFIFOLen = 0;

    for (i = 0; i < FSKDATAXMTFIFO_DEPTH; i++)
    {
        pFSKDataXmtFIFO->arrFIFOData[i].siRxTx_Slot = 0;
        pFSKDataXmtFIFO->arrFIFOData[i].siTxRx_Slot = 0;
        pFSKDataXmtFIFO->arrFIFOData[i].siDataStart_Slot = 0;
        pFSKDataXmtFIFO->arrFIFOData[i].siPowerUp_Slot = 0;
        pFSKDataXmtFIFO->arrFIFOData[i].siPowerDown_Slot = 0;
    	for (j = 0; j < FSKDATAXMT_BUF_LEN; j++)
    	{
            pFSKDataXmtFIFO->arrFIFOData[i].siDataXmtBuf[j] = 0;
    	}
        pFSKDataXmtFIFO->arrFIFOData[i].uiDataXmtIndex = 0;
        pFSKDataXmtFIFO->arrFIFOData[i].uiDataLen = 0;
        pFSKDataXmtFIFO->arrFIFOData[i].uiModIndex	= 0;
    }



}
