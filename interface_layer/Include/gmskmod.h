/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	interfunc.h
  Author: ���      Version: V1.00       Date: 2012.03.21 
  Description:  
  				��Ҫ���ܣ� 
				1. ������غ궨��
				2. ��������
				3. ��������
				4. ����ṹ������
				
  Others:         // �������ݵ�˵��
  Function List:  // ��Ҫ�����б�ÿ����¼Ӧ���������������ܼ�Ҫ˵��
  History:        // �޸���ʷ��¼�б�ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸�
                  // �߼��޸����ݼ���  
    1. Date:
       Author:
       Modification:
*******************************************************************************/
#ifndef _GMSKMOD_H
#define _GMSKMOD_H

#include "math_rand.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define G_MOD_FILTER_SIZE           (21)
#define FSKDATAXMT_BUF_LEN			(3120)		//�������ݲ���                  //lq ��СȡֵҪ��֤��С��GMSK����ǰ����Ϣλ����5����ԭ��ȡֵΪ5120

#define FSKDATAXMT_FIFO_DEPTH		(3)
#define FSKDATAXMT_FIFODATA_LEN		(70)

#define FSKDATAXMTFIFO_DEPTH        (1) //lq (3)

#define FSKDATA_TRAN_IDLE			(0)		//����״̬
#define FSKDATA_TRAN_START			(1)		//��������״̬
#define FSKDATA_TRAN_DATA			(2)		//��������״̬
#define FSKDATA_TRAN_END			(3)		//���ݷ��ͽ���״̬
#define FSKDATA_TRAN_STARTRDY		(4)		//��������״̬

#define FSKDATA_NOWSLOT				(0x7FFF)

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/

/**
	@ingroup ���ݴ���
	@brief ���Ϳ��ƽṹ�壬�������͵�ʱ϶�����ݻ��塢���ȵ�
*/
typedef struct {
	S16 siDataXmtBuf[FSKDATAXMT_BUF_LEN];	//�������ݻ���                  //lq ��ŵ��ƺ������(Msg18[��Msg19])
	U16 uiDataXmtIndex;						//���ݷ�������                      //lq ���ڷ���ʱ�ļ���(��λ-�֣�16bit)
	U16 uiDataLen;							//�������ݳ���                      //lq Ҫ�������ݣ����ƺ�����ݳ���[��λ-�֣�16b]���ĳ���
	U16 uiTran_Sta;							//���ݷ���״̬��״̬
	U16 uiFirt_En;							//���ݵ���ʹ��
	S16 siRxTx_Slot;						//�շ��л�ʱ϶��
	S16 siTxRx_Slot;						//�����л�ʱ϶��
	S16 siPowerUp_Slot;						//��������ʱ϶
	S16 siPowerDown_Slot;					//�����½�ʱ϶
	S16 siDataStart_Slot;					//���ݿ�ʼ����ʱ϶               //lq ���ݿ�ʼ������λ
	S16 siTran_Slot;						//����֡�ţ�0��2249��Ϊ0xFFFFʱʹ�õ�ǰ֡
//lq-	S32 siTran_Slot;                        //���ƺ����Ϣ�ķ���ʱ϶��
    U16 uiModIndex;                                                             //lq ���ڵ���ʱ�ļ���(��λ-�֣�16bit)
	U16 uiServicNum;
	U32 uiXmtFreq;                          //��ǰ�����źŵķ���Ƶ��
}FSKDataXmtStruct;


typedef struct FSKDataXmtFIFO
{
    FSKDataXmtStruct  arrFIFOData[FSKDATAXMTFIFO_DEPTH];
    U8  ucFIFOIn;
    U8  ucFIFOOut;
    U8  ucFIFOLen;
}FSKDataXmtFIFOStruct;

extern FSKDataXmtStruct FSKDataXmtStru;
extern FSKDataXmtStruct *pFSKDataXmtStru2;  
extern FSKDataXmtFIFOStruct gstruFSKDataXmtFIFO;
extern struct math_rand send_seed;

/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/
extern void InitFSKDataTranPara(void);
extern U8 GMSKModMain(U8 *pInBuf, S16 uiSlotNum, U16 uiLen, U16 uiModAmp, U16 uiOffset);
extern U8 GMSKModTest(U8 *pInBuf, S16 uiSlotNum, U16 uiLen, U16 uiModAmp, U16 uiOffset, U8 ucTestMod);
extern U8 GMSKModTest2(U8 *pInBuf, S16 uiSlotNum, U16 uiLen, U16 uiModAmp, U16 uiOffset);
//extern U8 FillGMSKTranFIFO(U8 *pInBuf, U16 uiSlotNum, U16 uiLen);
extern void set_send_seed(U32 seed);

extern U8 GMSKModMain_ITDMA(U8 *pInBuf, S16 uiSlotNum, U16 uiLen, U16 uiModAmp, U16 uiOffset, 	U32 uiXmtFreq);
extern U8 FIFOInOfFSKDataXmt(void);
extern U8 FIFOOutOfFSKDataXmt(FSKDataXmtStruct **outFSKDataXmtPara);
extern U8 IsEmptyOfFSKDataXmt(void);
extern U8 IsFullOfFSKDataXmt(void);
extern void InitFSKDataXmtFIFO(FSKDataXmtFIFOStruct *pFSKDataXmtFIFO);
#endif
