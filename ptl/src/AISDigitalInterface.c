/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: AISDigitalInterface.c
  Author: lq       Version : V1.00        Date: 2014.04.25
  Description:     AIS����ͨ�Žӿ�
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include "Include.h"

AisDiSenFifoStruct AisDiSenQue;
ABMStruct ABMPara;
BBMStruct BBMPara;

/*******************************************************************************
* ����  : AISDI_InitAisDiSenFifoStruct
* ����  : �ṹ���ʼ��
* ����  : pAisDiSenFifoStruct - AisDiSenFifoStructָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void AISDI_InitAisDiSenFifoStruct(AisDiSenFifoStruct* pAisDiSenFifoStruct)
{
	pAisDiSenFifoStruct->FifoInput = 0;
	pAisDiSenFifoStruct->FifoOutput = 0;
}

/*******************************************************************************
* ����  : AISDI_InitABMStruct
* ����  : �ṹ���ʼ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void AISDI_InitABMStruct(ABMStruct *pABMStruct)
{
    pABMStruct->SentenceTotal = 0;
    pABMStruct->SentenceNumber = 0;
    pABMStruct->MessageNumber = 0;
    pABMStruct->MmsiDest = 0;
    pABMStruct->Channel = ENUM_AIS_CHANNEL_BOTH;
    pABMStruct->MsgID = 0;
    pABMStruct->EncapDataLen = 0;
    pABMStruct->FillBitNumber = 0; 	    
    pABMStruct->ValidFlg = FALSE;
}

/*******************************************************************************
* ����  : AISDI_InitBBMStruct
* ����  : �ṹ���ʼ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void AISDI_InitBBMStruct(BBMStruct *pBBMStruct)
{
    pBBMStruct->SentenceTotal = 0;
    pBBMStruct->SentenceNumber = 0;
    pBBMStruct->MessageNumber = 0;
    pBBMStruct->Channel = ENUM_AIS_CHANNEL_BOTH;
    pBBMStruct->MsgID = 0;
    pBBMStruct->EncapDataLen = 0;
    pBBMStruct->FillBitNumber = 0; 	    
    pBBMStruct->ValidFlg = FALSE;
}

/*******************************************************************************
* ����  : AISDI_InitAisDiSenFifoInstance
* ����  : �ṹ���ʼ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void AISDI_InitAisDiSenFifoInstance(void)
{
	AISDI_InitAisDiSenFifoStruct(&AisDiSenQue);
}

/*******************************************************************************
* ����  : AsciiToSixBinCode
* ����  : ASCC��תΪAIS��6λ��������
* ����  : ASCII��		  
* ���  : ��
* ����  : 6λ��������
*******************************************************************************/
U8 AsciiToSixBinCode(U8 code)
{
	U16 sum;
	
	sum = code + 0x28;
	if (sum > 0x80)
	{
	    return (sum + 0x20) & 0x3F;
	}
	else
	{
	    return (sum + 0x28) & 0x3F;            
	}
}

/*******************************************************************************
* ����  : SixBinCodeToAscii
* ����  : AIS��6λ��������תΪASCII��
* ����  : 6λ��������	  
* ���  : ��
* ����  : ASCII��
*******************************************************************************/
U8 SixBinCodeToAscii(U8 code)
{
    if (code < 0x28)
    {
        return code + 0x30;
    }
    else
    {
        return code + 0x38;
    }        
}

/*******************************************************************************
* ����  : AISDI_IsSenQueFull
* ����  : �������Ƿ�����
* ����  : pAisDiSenFifoStruct - AisDiSenFifoStructָ��		  
* ���  : ��
* ����  : TRUE - ������FALSE - δ��
*******************************************************************************/
U8 AISDI_IsSenQueFull(AisDiSenFifoStruct* pAisDiSenFifoStruct)
{
	if ((pAisDiSenFifoStruct->FifoInput + 1) % AIS_DI_SEN_FIFO_DEPTH
		== pAisDiSenFifoStruct->FifoOutput)
	{
        debug_printf("$[%6d:%4d] %s : Sentence queue is full!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);		 
			
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*******************************************************************************
* ����  : AISDI_IsSenQueEmpty
* ����  : �������Ƿ�Ϊ��
* ����  : pAisDiSenFifoStruct - AisDiSenFifoStructָ��		  
* ���  : ��
* ����  : TRUE - Ϊ�գ�FALSE - �ǿ�
*******************************************************************************/
U8 AISDI_IsSenQueEmpty(AisDiSenFifoStruct* pAisDiSenFifoStruct)
{
	if (pAisDiSenFifoStruct->FifoInput == pAisDiSenFifoStruct->FifoOutput)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*******************************************************************************
* ����  : AISDI_SenEnque
* ����  : ���������
* ����  : pAisDiSenFifoStruct - AisDiSenFifoStructָ��
		  pData - ָ������ָ��		  
* ���  : ��
* ����  : True - ��ӳɹ���FALSE - ���ʧ��
*******************************************************************************/
U8 AISDI_SenEnque(AisDiSenFifoStruct* pAisDiSenFifoStruct, U8* pData)
{
	if (AISDI_IsSenQueFull(pAisDiSenFifoStruct) == FALSE)
	{
		strcpy(pAisDiSenFifoStruct->Fifo[pAisDiSenFifoStruct->FifoInput], pData);
		pAisDiSenFifoStruct->FifoInput = (pAisDiSenFifoStruct->FifoInput + 1) 
										  % AIS_DI_SEN_FIFO_DEPTH;		
	
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*******************************************************************************
* ����  : AISDI_SenDeque
* ����  : �����г���
* ����  : pAisDiSenFifoStruct - AisDiSenFifoStructָ��
		  pData - ָ���ų�������ָ��		  
* ���  : ��
* ����  : True - ���ӳɹ���FALSE - ����ʧ��
*******************************************************************************/
U8 AISDI_SenDeque(AisDiSenFifoStruct* pAisDiSenFifoStruct, U8* pData)
{
	if (AISDI_IsSenQueEmpty(pAisDiSenFifoStruct) == FALSE)
	{
		strcpy(pData, pAisDiSenFifoStruct->Fifo[pAisDiSenFifoStruct->FifoOutput]);
		pAisDiSenFifoStruct->FifoOutput = (pAisDiSenFifoStruct->FifoOutput + 1) 
										  % AIS_DI_SEN_FIFO_DEPTH;		
	
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*******************************************************************************
* ����  : AISDI_BuildVDM
* ����  : �齨VDM���
* ����  : listener - ������ID
          data - ��Ϣ
          len - ��Ϣ�ֽڳ���
          channel - ������Ϣ���ŵ�����  		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void AISDI_BuildVDM(U8 listener[], U8 data[], U16 len, U8 channel)
{
	static U8 msgSeq = 0;
	U8 	senNum;
	U8	senTotal;
	U8	fillBitNum;
	U16 sixBitCodeNum;
    U16 temp16;
	AISDISenBuffArray sentence;
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 temp8;
	U8 tempBuff[10];

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));
	msg_bitmap.array = data;
	msg_bitmap.array_size = len;
	
	if ((len * 8) % 6 == 0)
	{
		sixBitCodeNum = (len * 8) / 6;
		fillBitNum = 0;	
	}
	else
	{
		sixBitCodeNum = (len * 8) / 6 + 1;
		fillBitNum = 6 - (len * 8) % 6;
	}

	if (channel != 'A' && channel != 'B')
	{
		/*lq �ŵ����ֶ�Ϊ�� */
		if (sixBitCodeNum <= 62)
		{
            /*lq ����� */
        	sprintf(sentence, "!");
        	strncat(sentence, listener, 2);
        	strcat(sentence, "VDM");			
			strcat(sentence, ",1");		//lq �������
			strcat(sentence, ",1");		//lq ������
			strcat(sentence, ",");		//lq ��Ϣ��ţ��������Ϣ����Ϣ����ֶ�Ϊ��
			strcat(sentence, ",");		//lq �ŵ�Ϊ��
			strcat(sentence, ",");		//lq ����
			for (i = 0; i < sixBitCodeNum - 1; i++)
			{
				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
				temp8 = SixBinCodeToAscii(temp8);
				strncat(sentence, &temp8, 1);
			}
			bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6 - fillBitNum);
			temp8 = SixBinCodeToAscii(temp8 << fillBitNum);
			strncat(sentence, &temp8, 1);
			sprintf(tempBuff, ",%d", fillBitNum); 
			strcat(sentence, tempBuff);	//lq λ�����

			/*lq ����������*/
			AISDI_SenEnque(&AisDiSenQue, sentence);							
		}
		else
		{
            /*lq ����� */
            if (sixBitCodeNum % 61 == 0)
            {
                senTotal = sixBitCodeNum / 61;
                temp16 = 61;
            }
            else
            {
                senTotal = sixBitCodeNum / 61 + 1;
                temp16 = sixBitCodeNum % 61;            
            }            

            /*lq ��󳤶���� */
            for (senNum = 1; senNum < senTotal; senNum++)
            {
            	sprintf(sentence, "!");
            	strncat(sentence, listener, 2);
            	strcat(sentence, "VDM");
			    sprintf(tempBuff, ",%d", senTotal); 
    			strcat(sentence, tempBuff);		//lq �������
			    sprintf(tempBuff, ",%d", senNum); 
    			strcat(sentence, tempBuff);		//lq ������
			    sprintf(tempBuff, ",%d", msgSeq); 
    			strcat(sentence, tempBuff);		//lq ��Ϣ���
    			strcat(sentence, ",");		    //lq �ŵ�Ϊ��
    			strcat(sentence, ",");		    //lq ����
    			for (i = 0; i < 61; i++)
    			{
    				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
    				temp8 = SixBinCodeToAscii(temp8);
    				strncat(sentence, &temp8, 1);
    			}
    			strcat(sentence, ",0");	        //lq λ�����
    
    			/*lq ����������*/
    			AISDI_SenEnque(&AisDiSenQue, sentence);            
            }

            /*lq ����Ϣ�����һ����� */
        	sprintf(sentence, "!");
        	strncat(sentence, listener, 2);
        	strcat(sentence, "VDM");
		    sprintf(tempBuff, ",%d", senTotal); 
			strcat(sentence, tempBuff);		//lq �������
		    sprintf(tempBuff, ",%d", senNum); 
			strcat(sentence, tempBuff);		//lq ������
		    sprintf(tempBuff, ",%d", msgSeq); 
			strcat(sentence, tempBuff);		//lq ��Ϣ���
			strcat(sentence, ",");		    //lq �ŵ�Ϊ��
			strcat(sentence, ",");		    //lq ����
			for (i = 0; i < temp16 - 1; i++)
			{
				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
				temp8 = SixBinCodeToAscii(temp8);
				strncat(sentence, &temp8, 1);
			}
			bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6 - fillBitNum);
			temp8 = SixBinCodeToAscii(temp8 << fillBitNum);
			strncat(sentence, &temp8, 1);
			sprintf(tempBuff, ",%d", fillBitNum); 
			strcat(sentence, tempBuff);	    //lq λ�����
			
            /*lq ����������*/
			AISDI_SenEnque(&AisDiSenQue, sentence);

            /*lq VDM�����Ϣ��ż�1 */
            msgSeq = (msgSeq + 1) % 10;                       
		}
	}
	else
	{
		/*lq �ŵ����ֶβ�Ϊ�� */
		if (sixBitCodeNum <= 61)
		{
            /*lq ����� */
        	sprintf(sentence, "!");
        	strncat(sentence, listener, 2);
        	strcat(sentence, "VDM");
			strcat(sentence, ",1");		//lq �������
			strcat(sentence, ",1");		//lq ������
			strcat(sentence, ",");		//lq ��Ϣ��ţ��������Ϣ����Ϣ����ֶ�Ϊ��
			strcat(sentence, ",");			
			strncat(sentence, &channel, 1);//lq �ŵ�			
			strcat(sentence, ",");		//lq ����
			for (i = 0; i < sixBitCodeNum - 1; i++)
			{
				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
				temp8 = SixBinCodeToAscii(temp8);
				strncat(sentence, &temp8, 1);
			}
			bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6 - fillBitNum);
			temp8 = SixBinCodeToAscii(temp8 << fillBitNum);
			strncat(sentence, &temp8, 1);
			sprintf(tempBuff, ",%d", fillBitNum); 
			strcat(sentence, tempBuff);	//lq λ�����

			/*lq ����������*/
			AISDI_SenEnque(&AisDiSenQue, sentence);							
		}
		else
		{
            /*lq ����� */
            if (sixBitCodeNum % 60 == 0)
            {
                senTotal = sixBitCodeNum / 60;
                temp16 = 60;
            }
            else
            {
                senTotal = sixBitCodeNum / 60 + 1;
                temp16 = sixBitCodeNum % 60;            
            }            

            /*lq ��󳤶���� */
            for (senNum = 1; senNum < senTotal; senNum++)
            {
            	sprintf(sentence, "!");
            	strncat(sentence, listener, 2);
            	strcat(sentence, "VDM");
			    sprintf(tempBuff, ",%d", senTotal); 
    			strcat(sentence, tempBuff);		//lq �������
			    sprintf(tempBuff, ",%d", senNum); 
    			strcat(sentence, tempBuff);		//lq ������
			    sprintf(tempBuff, ",%d", msgSeq); 
    			strcat(sentence, tempBuff);		//lq ��Ϣ���
    			strcat(sentence, ",");			
    			strncat(sentence, &channel, 1); //lq �ŵ�			
    			strcat(sentence, ",");		    //lq ����
    			for (i = 0; i < 60; i++)
    			{
    				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
    				temp8 = SixBinCodeToAscii(temp8);
    				strncat(sentence, &temp8, 1);
    			}
    			strcat(sentence, ",0");	        //lq λ�����
    
    			/*lq ����������*/
    			AISDI_SenEnque(&AisDiSenQue, sentence);            
            }

            /*lq ����Ϣ�����һ����� */
        	sprintf(sentence, "!");
        	strncat(sentence, listener, 2);
        	strcat(sentence, "VDM");
		    sprintf(tempBuff, ",%d", senTotal); 
			strcat(sentence, tempBuff);		//lq �������
		    sprintf(tempBuff, ",%d", senNum); 
			strcat(sentence, tempBuff);		//lq ������
		    sprintf(tempBuff, ",%d", msgSeq); 
			strcat(sentence, tempBuff);		//lq ��Ϣ���
			strcat(sentence, ",");			
			strncat(sentence, &channel, 1); //lq �ŵ�
			strcat(sentence, ",");		    //lq ����
			for (i = 0; i < temp16 - 1; i++)
			{
				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
				temp8 = SixBinCodeToAscii(temp8);
				strncat(sentence, &temp8, 1);
			}
			bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6 - fillBitNum);
			temp8 = SixBinCodeToAscii(temp8 << fillBitNum);
			strncat(sentence, &temp8, 1);
			sprintf(tempBuff, ",%d", fillBitNum); 
			strcat(sentence, tempBuff);	    //lq λ�����
			
            /*lq ����������*/
			AISDI_SenEnque(&AisDiSenQue, sentence);

            /*lq VDM�����Ϣ��ż�1 */
            msgSeq = (msgSeq + 1) % 10;                       
  		}	
	}
}

/*******************************************************************************
* ����  : AISDI_BuildVDO
* ����  : �齨VDO���
* ����  : listener - ������ID
          data - ��Ϣ
          len - ��Ϣ�ֽڳ���
          channel - ������Ϣ���ŵ�����  		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void AISDI_BuildVDO(U8 listener[], U8 data[], U16 len, U8 channel)
{
	static U8 msgSeq = 0;
	U8 	senNum;
	U8	senTotal;
	U8	fillBitNum;
	U16 sixBitCodeNum;
    U16 temp16;
	AISDISenBuffArray sentence;
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 temp8;
	U8 tempBuff[10];

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));
	msg_bitmap.array = data;
	msg_bitmap.array_size = len;
	
	if ((len * 8) % 6 == 0)
	{
		sixBitCodeNum = (len * 8) / 6;
		fillBitNum = 0;	
	}
	else
	{
		sixBitCodeNum = (len * 8) / 6 + 1;
		fillBitNum = 6 - (len * 8) % 6;
	}

	if (channel != 'A' && channel != 'B')
	{
		/*lq �ŵ����ֶ�Ϊ�� */
		if (sixBitCodeNum <= 62)
		{
            /*lq ����� */
        	sprintf(sentence, "!");
        	strncat(sentence, listener, 2);
        	strcat(sentence, "VDO");			
			strcat(sentence, ",1");		//lq �������
			strcat(sentence, ",1");		//lq ������
			strcat(sentence, ",");		//lq ��Ϣ��ţ��������Ϣ����Ϣ����ֶ�Ϊ��
			strcat(sentence, ",");		//lq �ŵ�Ϊ��
			strcat(sentence, ",");		//lq ����
			for (i = 0; i < sixBitCodeNum - 1; i++)
			{
				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
				temp8 = SixBinCodeToAscii(temp8);
				strncat(sentence, &temp8, 1);
			}
			bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6 - fillBitNum);
			temp8 = SixBinCodeToAscii(temp8 << fillBitNum);
			strncat(sentence, &temp8, 1);
			sprintf(tempBuff, ",%d", fillBitNum); 
			strcat(sentence, tempBuff);	//lq λ�����

			/*lq ����������*/
			AISDI_SenEnque(&AisDiSenQue, sentence);							
		}
		else
		{
            /*lq ����� */
            if (sixBitCodeNum % 61 == 0)
            {
                senTotal = sixBitCodeNum / 61;
                temp16 = 61;
            }
            else
            {
                senTotal = sixBitCodeNum / 61 + 1;
                temp16 = sixBitCodeNum % 61;            
            }            

            /*lq ��󳤶���� */
            for (senNum = 1; senNum < senTotal; senNum++)
            {
            	sprintf(sentence, "!");
            	strncat(sentence, listener, 2);
            	strcat(sentence, "VDO");
			    sprintf(tempBuff, ",%d", senTotal); 
    			strcat(sentence, tempBuff);		//lq �������
			    sprintf(tempBuff, ",%d", senNum); 
    			strcat(sentence, tempBuff);		//lq ������
			    sprintf(tempBuff, ",%d", msgSeq); 
    			strcat(sentence, tempBuff);		//lq ��Ϣ���
    			strcat(sentence, ",");		    //lq �ŵ�Ϊ��
    			strcat(sentence, ",");		    //lq ����
    			for (i = 0; i < 61; i++)
    			{
    				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
    				temp8 = SixBinCodeToAscii(temp8);
    				strncat(sentence, &temp8, 1);
    			}
    			strcat(sentence, ",0");	        //lq λ�����
    
    			/*lq ����������*/
    			AISDI_SenEnque(&AisDiSenQue, sentence);            
            }

            /*lq ����Ϣ�����һ����� */
        	sprintf(sentence, "!");
        	strncat(sentence, listener, 2);
        	strcat(sentence, "VDO");
		    sprintf(tempBuff, ",%d", senTotal); 
			strcat(sentence, tempBuff);		//lq �������
		    sprintf(tempBuff, ",%d", senNum); 
			strcat(sentence, tempBuff);		//lq ������
		    sprintf(tempBuff, ",%d", msgSeq); 
			strcat(sentence, tempBuff);		//lq ��Ϣ���
			strcat(sentence, ",");		    //lq �ŵ�Ϊ��
			strcat(sentence, ",");		    //lq ����
			for (i = 0; i < temp16 - 1; i++)
			{
				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
				temp8 = SixBinCodeToAscii(temp8);
				strncat(sentence, &temp8, 1);
			}
			bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6 - fillBitNum);
			temp8 = SixBinCodeToAscii(temp8 << fillBitNum);
			strncat(sentence, &temp8, 1);
			sprintf(tempBuff, ",%d", fillBitNum); 
			strcat(sentence, tempBuff);	    //lq λ�����
			
            /*lq ����������*/
			AISDI_SenEnque(&AisDiSenQue, sentence);

            /*lq VDO�����Ϣ��ż�1 */
            msgSeq = (msgSeq + 1) % 10;                       
		}
	}
	else
	{
		/*lq �ŵ����ֶβ�Ϊ�� */
		if (sixBitCodeNum <= 61)
		{
            /*lq ����� */
        	sprintf(sentence, "!");
        	strncat(sentence, listener, 2);
        	strcat(sentence, "VDO");
			strcat(sentence, ",1");		//lq �������
			strcat(sentence, ",1");		//lq ������
			strcat(sentence, ",");		//lq ��Ϣ��ţ��������Ϣ����Ϣ����ֶ�Ϊ��
			strcat(sentence, ",");			
			strncat(sentence, &channel, 1);//lq �ŵ�			
			strcat(sentence, ",");		//lq ����
			for (i = 0; i < sixBitCodeNum - 1; i++)
			{
				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
				temp8 = SixBinCodeToAscii(temp8);
				strncat(sentence, &temp8, 1);
			}
			bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6 - fillBitNum);
			temp8 = SixBinCodeToAscii(temp8 << fillBitNum);
			strncat(sentence, &temp8, 1);
			sprintf(tempBuff, ",%d", fillBitNum); 
			strcat(sentence, tempBuff);	//lq λ�����

			/*lq ����������*/
			AISDI_SenEnque(&AisDiSenQue, sentence);							
		}
		else
		{
            /*lq ����� */
            if (sixBitCodeNum % 60 == 0)
            {
                senTotal = sixBitCodeNum / 60;
                temp16 = 60;
            }
            else
            {
                senTotal = sixBitCodeNum / 60 + 1;
                temp16 = sixBitCodeNum % 60;            
            }            

            /*lq ��󳤶���� */
            for (senNum = 1; senNum < senTotal; senNum++)
            {
            	sprintf(sentence, "!");
            	strncat(sentence, listener, 2);
            	strcat(sentence, "VDO");
			    sprintf(tempBuff, ",%d", senTotal); 
    			strcat(sentence, tempBuff);		//lq �������
			    sprintf(tempBuff, ",%d", senNum); 
    			strcat(sentence, tempBuff);		//lq ������
			    sprintf(tempBuff, ",%d", msgSeq); 
    			strcat(sentence, tempBuff);		//lq ��Ϣ���
    			strcat(sentence, ",");			
    			strncat(sentence, &channel, 1); //lq �ŵ�			
    			strcat(sentence, ",");		    //lq ����
    			for (i = 0; i < 60; i++)
    			{
    				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
    				temp8 = SixBinCodeToAscii(temp8);
    				strncat(sentence, &temp8, 1);
    			}
    			strcat(sentence, ",0");	        //lq λ�����
    
    			/*lq ����������*/
    			AISDI_SenEnque(&AisDiSenQue, sentence);            
            }

            /*lq ����Ϣ�����һ����� */
        	sprintf(sentence, "!");
        	strncat(sentence, listener, 2);
        	strcat(sentence, "VDO");
		    sprintf(tempBuff, ",%d", senTotal); 
			strcat(sentence, tempBuff);		//lq �������
		    sprintf(tempBuff, ",%d", senNum); 
			strcat(sentence, tempBuff);		//lq ������
		    sprintf(tempBuff, ",%d", msgSeq); 
			strcat(sentence, tempBuff);		//lq ��Ϣ���
			strcat(sentence, ",");			
			strncat(sentence, &channel, 1); //lq �ŵ�
			strcat(sentence, ",");		    //lq ����
			for (i = 0; i < temp16 - 1; i++)
			{
				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
				temp8 = SixBinCodeToAscii(temp8);
				strncat(sentence, &temp8, 1);
			}
			bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6 - fillBitNum);
			temp8 = SixBinCodeToAscii(temp8 << fillBitNum);
			strncat(sentence, &temp8, 1);
			sprintf(tempBuff, ",%d", fillBitNum); 
			strcat(sentence, tempBuff);	    //lq λ�����
			
            /*lq ����������*/
			AISDI_SenEnque(&AisDiSenQue, sentence);

            /*lq VDO�����Ϣ��ż�1 */
            msgSeq = (msgSeq + 1) % 10;                       
  		}	
	}
}

/*******************************************************************************
* ����  : AISDI_BuildABK
* ����  : �齨ABK���
* ����  : listener - ������ID
          addrMmsi - Ѱַ�豸��MMSI
          rxChannel - 7����Ϣ��13����Ϣ���յ����ŵ�
          msgId - ABK������ȷ�ϵ���Ϣ��ID
          ackType - ȷ������
* ���  : ��
* ����  : ��
*******************************************************************************/
void AISDI_BuildABK(U8 listener[], U32 addrMmsi, AisChannelEnum rxChannel,
                    U8 msgId, U8 msgSeqNum, U8 ackType)
{
	AISDISenBuffArray sentence;
	U8 tempBuff[16];
	
    /*lq �齨��� */
	sprintf(sentence, "$");
	strncat(sentence, listener, 2);
	strcat(sentence, "ABK");
	strcat(sentence, ",");
	sprintf(tempBuff, "%09d", addrMmsi);    			
	strcat(sentence, tempBuff);		        //lq MMSI of the addressed AIS unit
	strcat(sentence, ",");
	if (rxChannel == ENUM_AIS_CHANNEL_A)    //lq AIS channel of reception
    {
	    strcat(sentence, AIS_CHANNEL_NAME_A);
    }
    else
    {
	    strcat(sentence, AIS_CHANNEL_NAME_B);
    }
	strcat(sentence, ",");
	sprintf(tempBuff, "%d", msgId);    			
	strcat(sentence, tempBuff);		        //lq ITU-R M.1371Message ID
	strcat(sentence, ",");
	sprintf(tempBuff, "%d", msgSeqNum);    			
	strcat(sentence, tempBuff);		        //lq Message sequence number
	strcat(sentence, ",");
	sprintf(tempBuff, "%d", ackType);    			
	strcat(sentence, tempBuff);		        //lq Type of acknowledgement
    
    /*lq ����������*/
    AISDI_SenEnque(&AisDiSenQue, sentence);
}

/*******************************************************************************
* ����  : AISDI_SenPrint
* ����  : �������AIS���
* ����  : pAisDiSenFifoStruct - AisDiSenFifoStructָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void AISDI_SenPrint(AisDiSenFifoStruct* pAisDiSenFifoStruct)
{
    AISDISenBuffArray sentence;
#if 0    
    while (AISDI_SenDeque(pAisDiSenFifoStruct, sentence) == TRUE)
    {
        UartResponseMsg(sentence);
    }
#else
    if (AISDI_SenDeque(pAisDiSenFifoStruct, sentence) == TRUE)
    {
        UartResponseMsg(sentence);
    }    
#endif        
}

/*******************************************************************************
* ����  : AISDI_SentencePrint
* ����  : �������AIS���
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void AISDI_SentencePrint(void)
{
    AISDI_SenPrint(&AisDiSenQue);
}

/*******************************************************************************
* ����  : AISDI_SentenceParse
* ����  : AIS���ֽӿڱ�׼����������ͨ��һ��������ö������ʱ����ĳ�ֶ�Ϊ����
          ���Ը��ֶΣ�����ĳ�ֶε�ȡֵ��Чʱ����Ϊ����ʧ�ܣ�����֮ǰ�����óɹ���,
          �������ֶλ���Ϊ�ջ���ȡֵ��Ч�����óɹ���
* ����  : pDat-���ݻ���ָ�룬uiLen-���ݳ���
* ���  : ��
* ����  : TRUE-��δ����FALSE-��Ϣ�Ѵ���
*******************************************************************************/
U8 AISDI_SentenceParse(U8 *pDat, U16 uiLen)
{
	U8 chTemp[AIS_DI_SENTENCE_MAXLEN + 1];
    U8 chTemp2[AIS_DI_SENTENCE_MAXLEN + 1];
	U8 	ucCheck;
	U8 	ucLen;
	U32 ulTemp;
	U32 ulTemp1;
	U32 ulTemp2;
	U32 ulTemp3;
	U32 ulTemp4;
    U8 *pEnd;

    /*lq AIABM-AIS Broadcast binary message����*/
	if (strncmp((char *)pDat, "AIABM", 5) == FALSE)
    {
		if (DevDynamicPara.ucWorkMode == EQ_WORKMOD_NORMAL)
		{
            /**************Total number of sentences***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);
            
            /*lq �ֶηǿ�*/
            if (ucCheck != FALSE)
			{
    			/*lq ������Ч���ж�*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 1)
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq �ַ���Ч���ж�*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < AIS_DI_SEN_TOTAL_MIN
                        || ulTemp > AIS_DI_SEN_TOTAL_MAX)
                    {
        				UartResponseMsg("$DUAIR,0,ABM");
        				return (FALSE);                
                    }

                    ulTemp1 = ulTemp;
                }
            }
            else
            {
				UartResponseMsg("$DUAIR,0,ABM");
				return (FALSE);            
            }

            /**************Sentence number***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
            
            /*lq �ֶηǿ�*/
            if (ucCheck != FALSE)
			{
    			/*lq ������Ч���ж�*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 1)
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq �ַ���Ч���ж�*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < AIS_DI_SEN_NUM_MIN
                        || ulTemp > AIS_DI_SEN_NUM_MAX)
                    {
        				UartResponseMsg("$DUAIR,0,ABM");
        				return (FALSE);                
                    }

                    ulTemp2 = ulTemp;
                }
            }
            else
            {
				UartResponseMsg("$DUAIR,0,ABM");
				return (FALSE);            
            }

			if (ulTemp1 > 1)    //lq �������Ϊ1ʱ��ʡ����Ϣ����ֶ�
            {
                /**************Sequential message identifier***************/
                ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
                
                /*lq �ֶηǿ�*/
                if (ucCheck != FALSE)
    			{
        			/*lq ������Ч���ж�*/
        			ucLen = strlen(chTemp);
        			if (ucLen != 1)
        			{
        				UartResponseMsg("$DUAIR,0,ABM");
        				return (FALSE);
        			}
    
                    ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                    
                    /*lq �ַ���Ч���ж�*/
        			if ((*pEnd) != '\0')
        			{
        				UartResponseMsg("$DUAIR,0,ABM");
        				return (FALSE);
        			}
                    else
                    {
                        /*lq ȡֵ��Χ��Ч���ж�*/
                        if (ulTemp < AIS_DI_MSG_SEQ_MIN
                            || ulTemp > AIS_DI_MSG_SEQ_MAX)
                        {
            				UartResponseMsg("$DUAIR,0,ABM");
            				return (FALSE);                
                        }
    
                        ulTemp3 = ulTemp;
                    }
                }
                else
                {
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);            
                }            
            }

            /**************MMSI of the destination AIS unit***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
            
            /*lq �ֶηǿ�*/
            if (ucCheck != FALSE)
			{
    			/*lq ������Ч���ж�*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 9)
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq �ַ���Ч���ж�*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}
                else
                {
                    ABMPara.MmsiDest = ulTemp;                                
                }
            }
            else
            {
				UartResponseMsg("$DUAIR,0,ABM");
				return (FALSE);                            
            }            

            /**************channel for broadcast***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);
            
            /*lq �ֶηǿ�*/
            if (ucCheck != FALSE)
			{
    			/*lq ������Ч���ж�*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 1)
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq �ַ���Ч���ж�*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < AIS_DI_ABM_CHN_MIN
                        || ulTemp > AIS_DI_ABM_CHN_MAX)
                    {
        				UartResponseMsg("$DUAIR,0,ABM");
        				return (FALSE);                
                    }
                    
                    switch(ulTemp)
                    {
                        case 1:
                            ABMPara.Channel = ENUM_AIS_CHANNEL_A;                                
                            break;
                        case 2:
                            ABMPara.Channel = ENUM_AIS_CHANNEL_B;                                
                            break;
                        default :
                            ABMPara.Channel = ENUM_AIS_CHANNEL_BOTH;                                
                            break;
                    }
                }
            }
            else
            {
                if (ulTemp2 == 1)
                {
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);                            
                }
            }            

            /**************Message ID***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 30);
            
            /*lq �ֶηǿ�*/
            if (ucCheck != FALSE)
			{
    			/*lq ������Ч���ж�*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 2)                         //lq ABM�����BBM���ĸ��ֶγ��Ȳ�ͬ
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq �ַ���Ч���ж�*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (!((ulTemp == 6) || (ulTemp == 12)))
                    {
        				UartResponseMsg("$DUAIR,0,ABM");
        				return (FALSE);                
                    }
                    
                    ABMPara.MsgID = ulTemp;                                
                }
            }
            else
            {
                if (ulTemp2 == 1)
                {				
                    UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);            
                }
            } 

            /**************Encapsulated data***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 7, uiLen, 60);
            
            /*lq �ֶηǿ�*/
            if (ucCheck != FALSE)
			{
    			/*lq ������Ч���ж�*/
    			ucLen = strlen(chTemp);
    			if (!((ucLen >= 1) && (ucLen <= 60)))
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}
                strcpy(chTemp2, chTemp);
            }
            else
            {
                UartResponseMsg("$DUAIR,0,ABM");
				return (FALSE);            
            }

            /**************Number of fill-bits***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 8, uiLen, 30);
            
            /*lq �ֶηǿ�*/
            if (ucCheck != FALSE)
			{
    			/*lq ������Ч���ж�*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 1)
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq �ַ���Ч���ж�*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < AIS_DI_ABM_FILL_BIT_MIN
                        || ulTemp > AIS_DI_ABM_FILL_BIT_MAX)
                    {
        				UartResponseMsg("$DUAIR,0,ABM");
        				return (FALSE);                
                    }

                    ulTemp4 = ulTemp;
                }
            }
            else
            {
				UartResponseMsg("$DUAIR,0,ABM");
				return (FALSE);            
            }

        	if (ulTemp2 == 1)								                    //lq ��һ�����
        	{
                ABMPara.ValidFlg = FALSE;

        	    /*lq ��һ�����ʼ�ո���֮ǰ������ */
                strcpy(ABMPara.EncapData, chTemp2);
                ABMPara.EncapDataLen = strlen(ABMPara.EncapData);
        
        		if (ulTemp1 > 1)
        		{
        			/*lq ��¼�������Ϣ�ĵ�ǰ����״̬ */
        			ABMPara.SentenceTotal = ulTemp1;
        			ABMPara.SentenceNumber = ulTemp2;
        			ABMPara.MessageNumber = ulTemp3;
        		}
        		else
        		{
        			/*lq ������� */
        			ABMPara.FillBitNumber = ulTemp4;
                    ABMPara.ValidFlg = TRUE;
        		}
        	}
        	else if ((ulTemp3 == ABMPara.MessageNumber)
        				&& (ulTemp2 == (ABMPara.SentenceNumber + 1))
        				&& (ulTemp2 <= ABMPara.SentenceTotal))					//lq �ǵ�һ����� 
        	{
        		if (ulTemp2 < ABMPara.SentenceTotal)							//lq �������Ϣ���м����
        		{
        			ABMPara.SentenceNumber++;
        			strcat(ABMPara.EncapData, chTemp2);
                    ABMPara.EncapDataLen = strlen(ABMPara.EncapData);
        		}
        		else if (ulTemp2 == ABMPara.SentenceTotal)						//lq �������Ϣ�����һ�����
        		{       
        			/*lq ת��������Encapsulated Data */
        			strcat(ABMPara.EncapData, chTemp2);
                    ABMPara.EncapDataLen = strlen(ABMPara.EncapData);
        			ABMPara.FillBitNumber = ulTemp4;
                    ABMPara.ValidFlg = TRUE;
        		}
        	}

            UartResponseMsg("$DUAIR,1,ABM");
        }        
    }

    /*lq AIBBM-AIS Broadcast binary message����*/
	else if (strncmp((char *)pDat, "AIBBM", 5) == FALSE)
    {
		if (DevDynamicPara.ucWorkMode == EQ_WORKMOD_NORMAL)
		{
            /**************Total number of sentences***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);
            
            /*lq �ֶηǿ�*/
            if (ucCheck != FALSE)
			{
    			/*lq ������Ч���ж�*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 1)
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq �ַ���Ч���ж�*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < AIS_DI_SEN_TOTAL_MIN
                        || ulTemp > AIS_DI_SEN_TOTAL_MAX)
                    {
        				UartResponseMsg("$DUAIR,0,BBM");
        				return (FALSE);                
                    }

                    ulTemp1 = ulTemp;
                }
            }
            else
            {
				UartResponseMsg("$DUAIR,0,BBM");
				return (FALSE);            
            }

            /**************Sentence number***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
            
            /*lq �ֶηǿ�*/
            if (ucCheck != FALSE)
			{
    			/*lq ������Ч���ж�*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 1)
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq �ַ���Ч���ж�*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < AIS_DI_SEN_NUM_MIN
                        || ulTemp > AIS_DI_SEN_NUM_MAX)
                    {
        				UartResponseMsg("$DUAIR,0,BBM");
        				return (FALSE);                
                    }

                    ulTemp2 = ulTemp;
                }
            }
            else
            {
				UartResponseMsg("$DUAIR,0,BBM");
				return (FALSE);            
            }

			if (ulTemp1 > 1)    //lq �������Ϊ1ʱ��ʡ����Ϣ����ֶ�
            {
                /**************Sequential message identifier***************/
                ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
                
                /*lq �ֶηǿ�*/
                if (ucCheck != FALSE)
    			{
        			/*lq ������Ч���ж�*/
        			ucLen = strlen(chTemp);
        			if (ucLen != 1)
        			{
        				UartResponseMsg("$DUAIR,0,BBM");
        				return (FALSE);
        			}
    
                    ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                    
                    /*lq �ַ���Ч���ж�*/
        			if ((*pEnd) != '\0')
        			{
        				UartResponseMsg("$DUAIR,0,BBM");
        				return (FALSE);
        			}
                    else
                    {
                        /*lq ȡֵ��Χ��Ч���ж�*/
                        if (ulTemp < AIS_DI_MSG_SEQ_MIN
                            || ulTemp > AIS_DI_MSG_SEQ_MAX)
                        {
            				UartResponseMsg("$DUAIR,0,BBM");
            				return (FALSE);                
                        }
    
                        ulTemp3 = ulTemp;
                    }
                }
                else
                {
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);            
                }            
            }

            /**************channel for broadcast***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
            
            /*lq �ֶηǿ�*/
            if (ucCheck != FALSE)
			{
    			/*lq ������Ч���ж�*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 1)
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq �ַ���Ч���ж�*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < AIS_DI_BBM_CHN_MIN
                        || ulTemp > AIS_DI_BBM_CHN_MAX)
                    {
        				UartResponseMsg("$DUAIR,0,BBM");
        				return (FALSE);                
                    }
                    
                    switch(ulTemp)
                    {
                        case 1:
                            BBMPara.Channel = ENUM_AIS_CHANNEL_A;                                
                            break;
                        case 2:
                            BBMPara.Channel = ENUM_AIS_CHANNEL_B;                                
                            break;
                        default :
                            BBMPara.Channel = ENUM_AIS_CHANNEL_BOTH;                                
                            break;
                    }
                }
            }
            else
            {
                if (ulTemp2 == 1)
                {
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);                            
                }
            }            

            /**************Message ID***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);
            
            /*lq �ֶηǿ�*/
            if (ucCheck != FALSE)
			{
    			/*lq ������Ч���ж�*/
    			ucLen = strlen(chTemp);
    			if (!((ucLen >= 1) && (ucLen <= 2)))    //lq ABM�����BBM���ĸ��ֶγ��Ȳ�ͬ
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq �ַ���Ч���ж�*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (!((ulTemp == 8) || (ulTemp == 14)))
                    {
        				UartResponseMsg("$DUAIR,0,BBM");
        				return (FALSE);                
                    }
                    
                    BBMPara.MsgID = ulTemp;                                
                }
            }
            else
            {
                if (ulTemp2 == 1)
                {				
                    UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);            
                }
            } 

            /**************Encapsulated data***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 60);
            
            /*lq �ֶηǿ�*/
            if (ucCheck != FALSE)
			{
    			/*lq ������Ч���ж�*/
    			ucLen = strlen(chTemp);
    			if (!((ucLen >= 1) && (ucLen <= 60)))
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}
                strcpy(chTemp2, chTemp);
            }
            else
            {
                UartResponseMsg("$DUAIR,0,BBM");
				return (FALSE);            
            }

            /**************Number of fill-bits***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 7, uiLen, 30);
            
            /*lq �ֶηǿ�*/
            if (ucCheck != FALSE)
			{
    			/*lq ������Ч���ж�*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 1)
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq �ַ���Ч���ж�*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}
                else
                {
                    /*lq ȡֵ��Χ��Ч���ж�*/
                    if (ulTemp < AIS_DI_BBM_FILL_BIT_MIN
                        || ulTemp > AIS_DI_BBM_FILL_BIT_MAX)
                    {
        				UartResponseMsg("$DUAIR,0,BBM");
        				return (FALSE);                
                    }

                    ulTemp4 = ulTemp;
                }
            }
            else
            {
				UartResponseMsg("$DUAIR,0,BBM");
				return (FALSE);            
            }

        	if (ulTemp2 == 1)								                    //lq ��һ�����
        	{
                BBMPara.ValidFlg = FALSE;

        	    /*lq ��һ�����ʼ�ո���֮ǰ������ */
                strcpy(BBMPara.EncapData, chTemp2);
                BBMPara.EncapDataLen = strlen(BBMPara.EncapData);
        
        		if (ulTemp1 > 1)
        		{
        			/*lq ��¼�������Ϣ�ĵ�ǰ����״̬ */
        			BBMPara.SentenceTotal = ulTemp1;
        			BBMPara.SentenceNumber = ulTemp2;
        			BBMPara.MessageNumber = ulTemp3;
        		}
        		else
        		{
        			/*lq ������� */
        			BBMPara.FillBitNumber = ulTemp4;
                    BBMPara.ValidFlg = TRUE;
        		}
        	}
        	else if ((ulTemp3 == BBMPara.MessageNumber)
        				&& (ulTemp2 == (BBMPara.SentenceNumber + 1))
        				&& (ulTemp2 <= BBMPara.SentenceTotal))					//lq �ǵ�һ����� 
        	{
        		if (ulTemp2 < BBMPara.SentenceTotal)							//lq �������Ϣ���м����
        		{
        			BBMPara.SentenceNumber++;
        			strcat(BBMPara.EncapData, chTemp2);
                    BBMPara.EncapDataLen = strlen(BBMPara.EncapData);
        		}
        		else if (ulTemp2 == BBMPara.SentenceTotal)						//lq �������Ϣ�����һ�����
        		{       
        			/*lq ת��������Encapsulated Data */
        			strcat(BBMPara.EncapData, chTemp2);
                    BBMPara.EncapDataLen = strlen(BBMPara.EncapData);
        			BBMPara.FillBitNumber = ulTemp4;
                    BBMPara.ValidFlg = TRUE;
        		}
        	}

            UartResponseMsg("$DUAIR,1,BBM");
        }        
    }

    else
    {
        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
* ����  : AISDI_ABMIsValid
* ����  : ABM�����Ƿ���Ч
* ����  : pABMStruct - ABMStruct�ṹ��ָ��
* ���  : ��
* ����  : ABM�����Ƿ���Ч
*******************************************************************************/
U8 AISDI_ABMIsValid(ABMStruct *pABMStruct)
{
    return pABMStruct->ValidFlg;
}

/*******************************************************************************
* ����  : AISDI_BBMIsValid
* ����  : BBM�����Ƿ���Ч
* ����  : pBBMStruct - BBMStruct�ṹ��ָ��
* ���  : ��
* ����  : BBM�����Ƿ���Ч
*******************************************************************************/
U8 AISDI_BBMIsValid(BBMStruct *pBBMStruct)
{
    return pBBMStruct->ValidFlg;
}

/*******************************************************************************
* ����  : AISDI_ModuleInit
* ����  : ģ���ʼ��
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void AISDI_ModuleInit()
{
	AISDI_InitAisDiSenFifoStruct(&AisDiSenQue);
    AISDI_InitABMStruct(&ABMPara);
    AISDI_InitBBMStruct(&BBMPara);
}
