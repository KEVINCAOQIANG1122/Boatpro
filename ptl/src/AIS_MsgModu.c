/******************************************************************
Copyright(c) 2010, TJU-TCB
All right reserved
File name:		AIS_MsgModu.c
Description:	Msg related parameter initialization and Msg process
Others:			void
Author:			WT
Version:		1.0
Date:			2010.7.21
Function List:  
*****************************************************************************
*  History:               				                                    *
*      <author>   <time>    <version >     <desc>                           *
*         WT     10/07/21     1.0       build this moudle                   *
****************************************************************************/
#include "Include.h"

GPSStruct pGpsCell;

U8 GPS_valid = 0;

/*******************************************************************************
* ����  : InitGpsCell
* ����  : ��ʼ��pGpsCell
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitGpsCell(void)
{
	IniGpsMsgStru(&pGpsCell);
}

/*******************************************************************************
* ����  : bitmap_set_value_for_U8
* ����  : ��λ����ֵ����data�ĵ�data_bit_lenλ��map->array[map->index]����Чλ��
          ��data_bit_lenλ��ֵ�����ֽ�map->array[map->index]�ĸ�ֵ˳�����ȶ�MSB
          ��ֵ�����LSB��ֵ��
* ����  : map - �ṹ�������
          data - �ṩ��ֵλ���ֽ����ݣ�
          data_bit_len - data���ڸ�ֵ��λ����LSB��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 bitmap_set_value_for_U8(BITMAP* map, U8 data, U8 data_bit_len)
{
	U8 data_mask;
	U8 temp;
    U8 byteMask;

	if(data_bit_len == 0)
	{
		return 1;
	}
	else if (data_bit_len > 8)
    {
        data_bit_len = 8;
    }

	data_mask = 0xFF >> (8 - data_bit_len);
	
	if(map->offset + data_bit_len < 8)
	{
		temp = 8 - map->offset - data_bit_len;
		map->array[map->index] = (map->array[map->index] >> (8 - map->offset)) << (8 - map->offset);    //lq ����ǰ�ֽ�map->array[map->index]�ĵ�ǰ��Чλ��0(Ϊ����ġ�|��λ������׼��)
		map->array[map->index] |= ((data & data_mask) << temp);                 //lq ����ǰ�ֽ�map->array[map->index]����Чλ��ǰ��MSB��data_bit_lenλ��Ϊdata�ĵ�data_bit_lenλ    
		
		map->offset += data_bit_len;
		map->offset %= 8;
	}
	else if(map->offset + data_bit_len == 8)
	{
		temp = 8 - map->offset - data_bit_len;
		map->array[map->index] = (map->array[map->index] >> (8 - map->offset)) << (8 - map->offset);
		map->array[map->index] |= ((data & data_mask) << temp);
		
		map->offset = 0;
		map->index++;
	}                                                                           //lq ָ����һ�ֽ�
	else
	{
		if(map->index == map->array_size - 1)                                   //lq map->array����
		{
			return 1;
		}
		
		temp = 8 - map->offset;                                                 //lq ��ǰ�ֽ�map->array[map->index]���ṩ�����ڸ�ֵ����Чλλ��
		map->array[map->index] = (map->array[map->index] >> (8 - map->offset)) << (8 - map->offset);
		map->array[map->index] |= ((data & data_mask) >> (data_bit_len - temp));//lq ʹ���ֽ�����data�ĵ�data_bit_lenλ�ĸ�tempλ�Ե�ǰ�ֽ�map->array[map->index]�ĵ�tempλ��ֵ
		map->array[map->index + 1] = map->array[map->index + 1] << 8;           //lq ���ֽ�map->array[map->index + 1]��0(Ϊ����ġ�|��λ������׼��)
        map->array[map->index + 1] |= ((data & data_mask) << (8 - (data_bit_len - temp)));
		
		map->index++;
		map->offset += data_bit_len;
		map->offset %= 8;
	}

	return 0;
}

/*******************************************************************************
* ����  : bitmap_set_value
* ����  : ��λ����ֵ
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 bitmap_set_value(BITMAP* map, U32 data, U8 data_bit_len)
{
	U8 split_data[4];
	U8 split_bit_len[4];
	U8 temp;
	S8 i;

	temp = data_bit_len;
	for(i = 3; i >= 0; i--)
	{
		if(temp >= 8)
		{
			split_bit_len[i] = 8;
			temp -=8;
		}
		else
		{
			split_bit_len[i] = temp;
			temp = 0;
		}
	}

	//�޸�split_data����˳��
	split_data[3] = data & 0xFF;
	split_data[2] = (data & 0xFF00) >> 8;
	split_data[1] = (data & 0xFF0000) >> 16;
	split_data[0] = (data & 0xFF000000) >> 24;

	bitmap_set_value_for_U8(map, split_data[0], split_bit_len[0]);
	bitmap_set_value_for_U8(map, split_data[1], split_bit_len[1]);
	bitmap_set_value_for_U8(map, split_data[2], split_bit_len[2]);
	bitmap_set_value_for_U8(map, split_data[3], split_bit_len[3]);

	return 0;
}

/*******************************************************************************
* ����  : bitmap_get_value_for_U8
* ����  : ��λ��ȡֵ����map->array[map->index]����Чλ����δ��ȡ��λ���ĸ�
          data_bit_lenλ��pData��ָ���ݵ�Ԫ��ֵ��data_bit_lenλ���ݵ�MSB��LSB��
          ˳����map->array[map->index]��*pData��һ�£�
          ���ֽ�map->array[map->index]�Ķ�ȡ˳�����ȶ�ȡMSB�����ȡLSB��
* ����  : map - �ṹ�������
          pData - �洢��ȡλ���ֽ����ݣ������pData��ָ�ֽڵ�Ԫ��LSBs����
          data_bit_len - ��ȡ��λ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 bitmap_get_value_for_U8(BITMAP* map, U8* pData, U8 data_bit_len)
{
	*pData = 0;

    if(data_bit_len == 0)
	{
		return 1;
	}
	else if (data_bit_len > 8)
    {
        data_bit_len = 8;
    }
	
	if(map->offset + data_bit_len <= 8)
	{
		*pData = map->array[map->index] << map->offset; //lq ��data_bit_len���ȵı�������LSBs����ʹ��MSBsΪ0
	    *pData >>= 8 - data_bit_len;
    }
	else
	{
		if(map->index == map->array_size - 1)                                   //lq map->array����
		{
			return 1;
		}
		
		*pData = map->array[map->index] << map->offset; //lq ��data_bit_len���ȵı�������LSBs����ʹ��MSBsΪ0
	    *pData >>= map->offset;
        *pData = (*pData << (map->offset + data_bit_len - 8)) 
                 | (map->array[map->index + 1] >> (16 - map->offset - data_bit_len));
	}

	map->offset += data_bit_len;
	map->index += map->offset / 8;
	map->offset %= 8;

	return 0;
}
/*******************************************************************************
* ����  : bitmap_get_value
* ����  : ��λ��ȡֵ
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 bitmap_get_value(BITMAP* map, U32* pData, U8 data_bit_len)
{
	U8 split_data[4];
	U8 split_bit_len[4];
	U8 temp;
	S8 i;

	temp = data_bit_len;
	for(i = 3; i >= 0; i--)
	{
		if(temp >= 8)
		{
			split_bit_len[i] = 8;
			temp -=8;
		}
		else
		{
			split_bit_len[i] = temp;
			temp = 0;
		}
	}

    for (i = 0; i < 3; i++)
    {
        split_data[i] = 0;
    }

	bitmap_get_value_for_U8(map, split_data, split_bit_len[0]);
	bitmap_get_value_for_U8(map, split_data + 1, split_bit_len[1]);
	bitmap_get_value_for_U8(map, split_data + 2, split_bit_len[2]);
	bitmap_get_value_for_U8(map, split_data + 3, split_bit_len[3]);

    *pData = (split_data[0] << 24)
           | (split_data[1] << 16)
           | (split_data[2] << 8)
           | split_data[3];

	return 0;
}

/*******************************************************************************
* ����  : InitMsg1
* ����  : ��ʼ����Ϣ1
* ����  : ��		  
* ���  : commState - ͨ��״̬ 
* ����  : ��
*******************************************************************************/
void InitMsg1(AISMsgCommStateUnion commState)
{
    AISMsgTypeStruct_1_2_3 msg1;

    /*lq ��ʼ����Ϣ�����ֶε�ֵ */
    msg1.MessageID = 1;                                                         //lq 6 bits
    msg1.RepeatIndicator = 0;                                                   //lq 2 bits
    msg1.UserID = AisStaticPara.MMSI;                                           //lq 30 bits
    msg1.NavigationalStatus = AisStaticPara.NavigationStatus;                   //lq 4 bits
    msg1.RotAIS = AisDynamicPara.RotAIS;                                        //lq 8 bits
    msg1.SOG = AisDynamicPara.SOG;                                              //lq 10 bits
    msg1.PositionAccuracy = AisDynamicPara.PositionAccuracy;                    //lq 1 bit
    msg1.Longitude = AisDynamicPara.Longitude;                                  //lq 28 bits
    msg1.Latitude = AisDynamicPara.Latitude;                                    //lq 27 bits
    msg1.COG = AisDynamicPara.COG;                                              //lq 12 bits
    msg1.TrueHeading = AisDynamicPara.TrueHeading;                              //lq 9 bits
    msg1.TimeStamp = AisDynamicPara.TimeStamp;                                  //lq 6 bits
    msg1.SpecialManoeuvreIndicator = AisDynamicPara.SpecialManoeuvreIndicator;  //lq 2 bits
    //  Spare;                                                                  //lq 3 bits, all 0
    msg1.RAIMFlag = AisDynamicPara.RaimFlag;                                    //lq 1 bit
    msg1.CommunicationState.SOTDMACommState.SyncState
        = commState.SOTDMACommState.SyncState;                                  //lq 2 bits
    msg1.CommunicationState.SOTDMACommState.SlotTimeout
        = commState.SOTDMACommState.SlotTimeout;                                //lq 3 bits
    msg1.CommunicationState.SOTDMACommState.SubMessage.SubMsg
        = commState.SOTDMACommState.SubMessage.SubMsg;                          //lq 14 bits
                                                                                //lq total 168 bits��1 slot

    /*lq ������Ϣ�Ķ����Ʊ�ʾ */
    AISMSG_BuildMsg1(&msg1);
}

/*******************************************************************************
* ����  : InitMsg3
* ����  : ��ʼ����Ϣ3
* ����  : ��		  
* ���  : commState - ͨ��״̬ 
* ����  : ��
*******************************************************************************/
void InitMsg3(AISMsgCommStateUnion commState)
{
    AISMsgTypeStruct_1_2_3 msg3;

    /*lq ��ʼ����Ϣ�����ֶε�ֵ */
    msg3.MessageID = 3;                                                         //lq 6 bits
    msg3.RepeatIndicator = 0;                                                   //lq 2 bits
    msg3.UserID = AisStaticPara.MMSI;                                           //lq 30 bits
    msg3.NavigationalStatus = AisStaticPara.NavigationStatus;                   //lq 4 bits
    msg3.RotAIS = AisDynamicPara.RotAIS;                                        //lq 8 bits
    msg3.SOG = AisDynamicPara.SOG;                                              //lq 10 bits
    msg3.PositionAccuracy = AisDynamicPara.PositionAccuracy;                    //lq 1 bit
    msg3.Longitude = AisDynamicPara.Longitude;                                  //lq 28 bits
    msg3.Latitude = AisDynamicPara.Latitude;                                    //lq 27 bits
    msg3.COG = AisDynamicPara.COG;                                              //lq 12 bits
    msg3.TrueHeading = AisDynamicPara.TrueHeading;                              //lq 9 bits
    msg3.TimeStamp = AisDynamicPara.TimeStamp;                                  //lq 6 bits
    msg3.SpecialManoeuvreIndicator = AisDynamicPara.SpecialManoeuvreIndicator;  //lq 2 bits
    //  Spare;                                                                  //lq 3 bits, all 0
    msg3.RAIMFlag = AisDynamicPara.RaimFlag;                                    //lq 1 bit
    msg3.CommunicationState.ITDMACommState.SyncState
        = commState.ITDMACommState.SyncState;                                   //lq 2 bits
    msg3.CommunicationState.ITDMACommState.SlotIncrement
        = commState.ITDMACommState.SlotIncrement;                               //lq 13 bits
    msg3.CommunicationState.ITDMACommState.NumberOfSlots
        = commState.ITDMACommState.NumberOfSlots;                               //lq 3 bits
    msg3.CommunicationState.ITDMACommState.KeepFlag
        = commState.ITDMACommState.KeepFlag;                                    //lq 1 bits
                                                                                //lq total 168 bits��1 slot

    /*lq ������Ϣ�Ķ����Ʊ�ʾ */
    AISMSG_BuildMsg3(&msg3);
}

/*******************************************************************************
* ����  : InitMsg5
* ����  : ��ʼ����Ϣ5
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitMsg5(void)
{
    AISMsgTypeStruct_5 msg5;

    msg5.MessageID = 5;                                                     //lq 6 bits
    msg5.RepeatIndicator = 0;                                               //lq 2 bits
    msg5.UserID = AisStaticPara.MMSI;                                       //lq 30 bits
    msg5.AISVersionIndicator = AisStaticPara.AISVersionIndicator;           //lq 2 bits
    msg5.IMONumber = AisStaticPara.IMONumber;                               //lq 30 bits
    strncpy(msg5.CallSign, AisStaticPara.CallSign, AIS_INFO_CALL_SIGN_LEN); //lq 42 bits (6-bit ASCII)
    strncpy(msg5.Name, AisStaticPara.ShipName, AIS_INFO_SHIP_NAME_LEN);     //lq 120 bits (6-bit ASCII)
    msg5.TypeOfShipAndCargo = AisStaticPara.TypeOfShip;                     //lq 8 bits
    msg5.DimensionA = AisStaticPara.ShipA;                                  //lq 9 bits
    msg5.DimensionB = AisStaticPara.ShipB;                                  //lq 9 bits
    msg5.DimensionC = AisStaticPara.ShipC;                                  //lq 6 bits
    msg5.DimensionD = AisStaticPara.ShipD;                                  //lq 6 bits
    msg5.TypeOfEPFD = AisStaticPara.TypeOfEPFS;                             //lq 4 bits
    msg5.ETAMonth = AisStaticPara.ETAMonth;                                 //lq 4 bits
    msg5.ETADay = AisStaticPara.ETADay;                                     //lq 5 bits
    msg5.ETAHour = AisStaticPara.ETAHour;                                   //lq 5 bits
    msg5.ETAMinute = AisStaticPara.ETAMinute;                               //lq 6 bits
    msg5.MaxPresentStaticDraught = AisStaticPara.MaxDraught;                //lq 8 bits
    strncpy(msg5.Destination, AisStaticPara.Destination, AIS_INFO_DESTINATION_LEN); //lq 120 bits (6-bit ASCII)
    msg5.DTE = AisStaticPara.DTE;                                           //lq 1 bits
    //  Spare;                                                              //lq 1 bit, all 0
                                                                            //lq total 424 bits��2 slots

    /*lq ������Ϣ�Ķ����Ʊ�ʾ */
    AISMSG_BuildMsg5(&msg5);
}

/*******************************************************************************
* ����  : InitMsg6
* ����  : ��ʼ����Ϣ6
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitMsg6(void)
{
    static U8 seqNum = 0;           //lq 0 - 3
    S16 i;
    AISMsgTypeStruct_6 msg6;
	BITMAP msg_bitmap;

    msg6.MessageID = 6;                                                     //lq 6 bits
    msg6.RepeatIndicator = 0;                                               //lq 2 bits
    msg6.SourceID = AisStaticPara.MMSI;                                     //lq 30 bits
    msg6.SequenceNumber = seqNum;                                           //lq 2 bits
    msg6.DestinationID = ABMPara.MmsiDest;                                  //lq 30 bits
    msg6.RetransmitFlag = 0;                                                //lq 1 bit
    //  Spare;                                                              //lq 1 bit, all 0

    /*lq Binary data�е�Application identifier��ASCIIת6-bit binary */
    msg6.AI_DAC = (AsciiToSixBinCode(ABMPara.EncapData[0]) << 4)
        || (AsciiToSixBinCode(ABMPara.EncapData[1]) >> 2);                  //lq 10 bits
    msg6.AI_FI = ((AsciiToSixBinCode(ABMPara.EncapData[1]) & 0x03) << 4)
        || (AsciiToSixBinCode(ABMPara.EncapData[2]) >> 2);                  //lq 6 bits

    /*lq Binary data�е�Application data��ASCIIת6-bit binary */
	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));
	msg_bitmap.array = msg6.ApplicationData;
  	msg_bitmap.array_size = 115;
    bitmap_set_value(&msg_bitmap, AsciiToSixBinCode(ABMPara.EncapData[2]), 2);
    for (i = 3; i < ABMPara.EncapDataLen - 1; i++)
    {
        bitmap_set_value(&msg_bitmap, AsciiToSixBinCode(ABMPara.EncapData[i]),
            6);
    }    
    bitmap_set_value(&msg_bitmap, AsciiToSixBinCode(ABMPara.EncapData[i]) >> ABMPara.FillBitNumber,
        6 - ABMPara.FillBitNumber);                                         //lq Maximum 920 bits (8-bit byte)
    msg6.AppDataBitLen = 6 * ABMPara.EncapDataLen - 16 - ABMPara.FillBitNumber;
                                                                            //lq Maximum 1008 bits��1-5 slots(B-Class mobile AIS station not exceed 2 slots)

    seqNum = (seqNum + 1) % 4;

    /*lq ������Ϣ�Ķ����Ʊ�ʾ */
    AISMSG_BuildMsg6(&msg6);
}

/*******************************************************************************
* ����  : InitMsg6Test
* ����  : ��ʼ��������Ϣ6
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitMsg6Test(void)
{
    static U8 seqNum = 0;           //lq 0 - 3
    S16 i;
    AISMsgTypeStruct_6 msg6;
	BITMAP msg_bitmap;

    msg6.MessageID = 6;                                                     //lq 6 bits
    msg6.RepeatIndicator = 0;                                               //lq 2 bits
    msg6.SourceID = AisStaticPara.MMSI;                                     //lq 30 bits
    msg6.SequenceNumber = seqNum;                                           //lq 2 bits
    msg6.DestinationID = AisStaticPara.MMSI;                                //lq 30 bits
    msg6.RetransmitFlag = 0;                                                //lq 1 bit
    //  Spare;                                                              //lq 1 bit, all 0

    /*lq Binary data�е�Application identifier */
    msg6.AI_DAC = RFL_SLFCHK_MSG6_AI_DAC;                                   //lq 10 bits
    msg6.AI_FI = RFL_SLFCHK_MSG6_AI_FI;                                     //lq 6 bits

    /*lq Binary data�е�Application data */
	msg6.ApplicationData[0] = RFL_SLFCHK_MSG6_AD;                           //lq Maximum 920 bits (8-bit byte)
    msg6.AppDataBitLen = RFL_SLFCHK_MSG6_AD_BIT_LEN;
                                                                            //lq Maximum 1008 bits��1-5 slots(B-Class mobile AIS station not exceed 2 slots)
    seqNum = (seqNum + 1) % 4;

    /*lq ������Ϣ�Ķ����Ʊ�ʾ */
    AISMSG_BuildMsg6(&msg6);
}

#if 0
/*******************************************************************************
* ����  : InitMsg6
* ����  : ��ʼ��ʹ����Ϣ6��IFM0
* ����  : info - text����		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitMsg6(void)
{
	BITMAP msg_bitmap;
	U8 len_1;
//	U8 len_2;
//	U8 len_3;
	U8 len_4;
	U8 len_5;
	U8 i;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = G_Msg6IFM0;

	bitmap_set_value(&msg_bitmap, 0, 8);//ռλ                                  //lq msg_bitmap.array[0]������ݵĳ��ȣ���λbyte��

	bitmap_set_value(&msg_bitmap, 6, 6);                                        //lq Message ID
	bitmap_set_value(&msg_bitmap, 0, 2);                                        //lq Repeat indicator
	bitmap_set_value(&msg_bitmap, aiAISCell.UserID, 30);                        //lq Source ID
	bitmap_set_value(&msg_bitmap, 0, 2);                                        //lq Sequence number
	bitmap_set_value(&msg_bitmap, aiAISCell.DestMMSI, 30);                      //lq Destination ID
	bitmap_set_value(&msg_bitmap, 0, 1);                                        //lq Retransmit flag
	bitmap_set_value(&msg_bitmap, 0, 1);                                        //lq Spare
	bitmap_set_value(&msg_bitmap, Msg06_DAC, 10);                               //lq һ�����DAC=413
	bitmap_set_value(&msg_bitmap, Msg06_FI, 6);                                 //lq һ�����FI=1
	bitmap_set_value(&msg_bitmap, Msg06_AnalogueInternal, 10); 
	bitmap_set_value(&msg_bitmap, Msg06_AnalogueExternalNo1, 10);                  
	bitmap_set_value(&msg_bitmap, Msg06_AnalogueExternalNo2, 10);                  
	bitmap_set_value(&msg_bitmap, Msg06_StatusInternal, 5);                  
	bitmap_set_value(&msg_bitmap, Msg06_StatusExternal, 8);                  
	bitmap_set_value(&msg_bitmap, Msg06_OffPositionStatus, 1);                  
	bitmap_set_value(&msg_bitmap, 0, 4);                                        //lq For future use

	len_4 = msg_bitmap.index;
	len_5 = msg_bitmap.offset == 0 ? len_4 - 1 : len_4;
	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;
	
	bitmap_set_value(&msg_bitmap, len_5, 8);
}
#endif

/*******************************************************************************
* ����  : InitMsg7
* ����  : ��ʼ����Ϣ7
* ����  : ackType - ȷ������
          pDestinationID - ȷ����Ϣ��Ŀ��ID����ָ��
          pSequenceNumber - ȷ����Ϣ���������ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitMsg7(U8 ackType, U32 *pDestinationID, U8 *pSequenceNumber)
{
    AISMsgTypeStruct_7_13 msg7;

    msg7.MessageID = 7;                                 //lq 6 bits
    msg7.RepeatIndicator = 0;                           //lq 2 bits
    msg7.SourceID = AisStaticPara.MMSI;                 //lq 30 bits
    //  Spare;                                          //lq 2 bits, all 0

    if (ackType == AIS_MSG7_13_ACK_TYPE_1)
    {
        msg7.DestinationID1 = pDestinationID[0];        //lq 30 bits
        msg7.SequenceNumber1 = pSequenceNumber[0];      //lq 2 bits
        msg7.AckType = AIS_MSG7_13_ACK_TYPE_1;
    }
    else if (ackType == AIS_MSG7_13_ACK_TYPE_2)
    {
        msg7.DestinationID1 = pDestinationID[0];        //lq 30 bits
        msg7.SequenceNumber1 = pSequenceNumber[0];      //lq 2 bits
        msg7.DestinationID2 = pDestinationID[1];        //lq 30 bits
        msg7.SequenceNumber2 = pSequenceNumber[1];      //lq 2 bits
        msg7.AckType = AIS_MSG7_13_ACK_TYPE_2;
    }
    else if (ackType == AIS_MSG7_13_ACK_TYPE_3)
    {
        msg7.DestinationID1 = pDestinationID[0];        //lq 30 bits
        msg7.SequenceNumber1 = pSequenceNumber[0];      //lq 2 bits
        msg7.DestinationID2 = pDestinationID[1];        //lq 30 bits
        msg7.SequenceNumber2 = pSequenceNumber[1];      //lq 2 bits
        msg7.DestinationID3 = pDestinationID[2];        //lq 30 bits
        msg7.SequenceNumber3 = pSequenceNumber[2];      //lq 2 bits
        msg7.AckType = AIS_MSG7_13_ACK_TYPE_3;        
    }
    else if (ackType == AIS_MSG7_13_ACK_TYPE_4)
    {
        msg7.DestinationID1 = pDestinationID[0];        //lq 30 bits
        msg7.SequenceNumber1 = pSequenceNumber[0];      //lq 2 bits
        msg7.DestinationID2 = pDestinationID[1];        //lq 30 bits
        msg7.SequenceNumber2 = pSequenceNumber[1];      //lq 2 bits
        msg7.DestinationID3 = pDestinationID[2];        //lq 30 bits
        msg7.SequenceNumber3 = pSequenceNumber[2];      //lq 2 bits
        msg7.DestinationID4 = pDestinationID[3];        //lq 30 bits
        msg7.SequenceNumber4 = pSequenceNumber[3];      //lq 2 bits
        msg7.AckType = AIS_MSG7_13_ACK_TYPE_4;    
    }
    else
    {
        msg7.DestinationID1 = pDestinationID[0];        //lq 30 bits
        msg7.SequenceNumber1 = pSequenceNumber[0];      //lq 2 bits
        msg7.AckType = AIS_MSG7_13_ACK_TYPE_1;    
    }
                                                        //lq total 72-168 bits��1 slot

    /*lq ������Ϣ�Ķ����Ʊ�ʾ */
    AISMSG_BuildMsg7(&msg7);
}

/*******************************************************************************
* ����  : InitMsg8
* ����  : ��ʼ����Ϣ8
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitMsg8(void)
{
    S16 i;
    AISMsgTypeStruct_8 msg8;
	BITMAP msg_bitmap;

    msg8.MessageID = 8;                                                     //lq 6 bits
    msg8.RepeatIndicator = 0;                                               //lq 2 bits
    msg8.SourceID = AisStaticPara.MMSI;                                     //lq 30 bits
    //  Spare;                                                              //lq 2 bits, all 0

    /*lq Binary data�е�Application identifier��ASCIIת6-bit binary */
    msg8.AI_DAC = (AsciiToSixBinCode(BBMPara.EncapData[0]) << 4)
        || (AsciiToSixBinCode(BBMPara.EncapData[1]) >> 2);                  //lq 10 bits
    msg8.AI_FI = ((AsciiToSixBinCode(BBMPara.EncapData[1]) & 0x03) << 4)
        || (AsciiToSixBinCode(BBMPara.EncapData[2]) >> 2);                  //lq 6 bits

    /*lq Binary data�е�Application data��ASCIIת6-bit binary */
	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));
	msg_bitmap.array = msg8.ApplicationData;
  	msg_bitmap.array_size = 119;
    bitmap_set_value(&msg_bitmap, AsciiToSixBinCode(BBMPara.EncapData[2]), 2);
    for (i = 3; i < BBMPara.EncapDataLen - 1; i++)
    {
        bitmap_set_value(&msg_bitmap, AsciiToSixBinCode(BBMPara.EncapData[i]),
            6);
    }    
    bitmap_set_value(&msg_bitmap, AsciiToSixBinCode(BBMPara.EncapData[i]) >> BBMPara.FillBitNumber,
        6 - BBMPara.FillBitNumber);                                         //lq Maximum 920 bits (8-bit byte)
    msg8.AppDataBitLen = 6 * BBMPara.EncapDataLen - 16 - BBMPara.FillBitNumber;
                                                                            //lq Maximum 1008 bits��1-5 slots(B-Class mobile AIS station not exceed 2 slots)

    /*lq ������Ϣ�Ķ����Ʊ�ʾ */
    AISMSG_BuildMsg8(&msg8);
}

/*******************************************************************************
* ����  : InitMsg12
* ����  : ��ʼ����Ϣ12
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitMsg12(void)
{
    static U8 seqNum = 0;                                                    //lq 0 - 3
    S16 i;
    AISMsgTypeStruct_12 msg12;
	BITMAP msg_bitmap;

    msg12.MessageID = 12;                                                    //lq 6 bits
    msg12.RepeatIndicator = 0;                                               //lq 2 bits
    msg12.SourceID = AisStaticPara.MMSI;                                     //lq 30 bits
    msg12.SequenceNumber = seqNum;                                           //lq 2 bits
    msg12.DestinationID = ABMPara.MmsiDest;                                  //lq 30 bits
    msg12.RetransmitFlag = 0;                                                //lq 1 bit
    //  Spare;                                                               //lq 1 bit, all 0

    /*lq Safety related text */
    for (i = 0; i < ABMPara.EncapDataLen; i++)
    {
        msg12.SafetyRelatedText[i] = AsciiToSixBinCode(ABMPara.EncapData[i]);//lq Maximum 936 bits (6-bit ASCII)    
    }
    msg12.TextLen = ABMPara.EncapDataLen;

    seqNum = (seqNum + 1) % 4;

    /*lq ������Ϣ�Ķ����Ʊ�ʾ */
    AISMSG_BuildMsg12(&msg12);
}

/*******************************************************************************
* ����  : InitMsg13
* ����  : ��ʼ����Ϣ13
* ����  : ackType - ȷ������
          pDestinationID - ȷ����Ϣ��Ŀ��ID����ָ��
          pSequenceNumber - ȷ����Ϣ���������ָ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitMsg13(U8 ackType, U32 *pDestinationID, U8 *pSequenceNumber)
{
    AISMsgTypeStruct_7_13 msg13;

    msg13.MessageID = 13;                               //lq 6 bits
    msg13.RepeatIndicator = 0;                          //lq 2 bits
    msg13.SourceID = AisStaticPara.MMSI;                //lq 30 bits
    //  Spare;                                          //lq 2 bits, all 0

    if (ackType == AIS_MSG7_13_ACK_TYPE_1)
    {
        msg13.DestinationID1 = pDestinationID[0];       //lq 30 bits
        msg13.SequenceNumber1 = pSequenceNumber[0];     //lq 2 bits
        msg13.AckType = AIS_MSG7_13_ACK_TYPE_1;
    }
    else if (ackType == AIS_MSG7_13_ACK_TYPE_2)
    {
        msg13.DestinationID1 = pDestinationID[0];       //lq 30 bits
        msg13.SequenceNumber1 = pSequenceNumber[0];     //lq 2 bits
        msg13.DestinationID2 = pDestinationID[1];       //lq 30 bits
        msg13.SequenceNumber2 = pSequenceNumber[1];     //lq 2 bits
        msg13.AckType = AIS_MSG7_13_ACK_TYPE_2;
    }
    else if (ackType == AIS_MSG7_13_ACK_TYPE_3)
    {
        msg13.DestinationID1 = pDestinationID[0];       //lq 30 bits
        msg13.SequenceNumber1 = pSequenceNumber[0];     //lq 2 bits
        msg13.DestinationID2 = pDestinationID[1];       //lq 30 bits
        msg13.SequenceNumber2 = pSequenceNumber[1];     //lq 2 bits
        msg13.DestinationID3 = pDestinationID[2];       //lq 30 bits
        msg13.SequenceNumber3 = pSequenceNumber[2];     //lq 2 bits
        msg13.AckType = AIS_MSG7_13_ACK_TYPE_3;        
    }
    else if (ackType == AIS_MSG7_13_ACK_TYPE_4)
    {
        msg13.DestinationID1 = pDestinationID[0];       //lq 30 bits
        msg13.SequenceNumber1 = pSequenceNumber[0];     //lq 2 bits
        msg13.DestinationID2 = pDestinationID[1];       //lq 30 bits
        msg13.SequenceNumber2 = pSequenceNumber[1];     //lq 2 bits
        msg13.DestinationID3 = pDestinationID[2];       //lq 30 bits
        msg13.SequenceNumber3 = pSequenceNumber[2];     //lq 2 bits
        msg13.DestinationID4 = pDestinationID[3];       //lq 30 bits
        msg13.SequenceNumber4 = pSequenceNumber[3];     //lq 2 bits
        msg13.AckType = AIS_MSG7_13_ACK_TYPE_4;    
    }
    else
    {
        msg13.DestinationID1 = pDestinationID[0];       //lq 30 bits
        msg13.SequenceNumber1 = pSequenceNumber[0];     //lq 2 bits
        msg13.AckType = AIS_MSG7_13_ACK_TYPE_1;    
    }
                                                        //lq total 72-168 bits��1 slot

    /*lq ������Ϣ�Ķ����Ʊ�ʾ */
    AISMSG_BuildMsg13(&msg13);
}

/*******************************************************************************
* ����  : InitMsg14
* ����  : ��ʼ����Ϣ14
* ����  : ��
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitMsg14(void)
{
    S16 i;
    AISMsgTypeStruct_14 msg14;
	BITMAP msg_bitmap;

    msg14.MessageID = 14;                                                    //lq 6 bits
    msg14.RepeatIndicator = 0;                                               //lq 2 bits
    msg14.SourceID = AisStaticPara.MMSI;                                     //lq 30 bits
    //  Spare;                                                               //lq 2 bits, all 0

    /*lq Safety related text */
    for (i = 0; i < BBMPara.EncapDataLen; i++)
    {
        msg14.SafetyRelatedText[i] = AsciiToSixBinCode(BBMPara.EncapData[i]);//lq Maximum 936 bits (6-bit ASCII)    
    }
    msg14.TextLen = BBMPara.EncapDataLen;
                                                                             //lq Maximum 1008 bits��1-5 slots(B-Class mobile AIS station not exceed 2 slots)

    /*lq ������Ϣ�Ķ����Ʊ�ʾ */
    AISMSG_BuildMsg14(&msg14);
}

/*******************************************************************************
* ����  : InitMsg18
* ����  : ��ʼ����Ϣ18
* ����  : ��		  
* ���  : commStateType - ͨ��״̬����
          commState - ͨ��״̬ 
* ����  : ��
*******************************************************************************/
void InitMsg18(AISMsgCommStateEnum commStateType, AISMsgCommStateUnion commState)
{
    AISMsgTypeStruct_18 msg18;

    /*lq ��ʼ����Ϣ�����ֶε�ֵ */
    msg18.MessageID = 18;                                               //lq 6 bits
    msg18.RepeatIndicator = 0;                                          //lq 2 bits
    msg18.UserID = AisStaticPara.MMSI;                                  //lq 30 bits
    //  Spare;                                                          //lq 8 bits, all 0
    msg18.SOG = AisDynamicPara.SOG;                                     //lq 10 bits
    msg18.PositionAccuracy = AisDynamicPara.PositionAccuracy;           //lq 1 bit
    msg18.Longitude = AisDynamicPara.Longitude;                         //lq 28 bits
    msg18.Latitude = AisDynamicPara.Latitude;                           //lq 27 bits
    msg18.COG = AisDynamicPara.COG;                                     //lq 12 bits
    msg18.TrueHeading = AisDynamicPara.TrueHeading;                     //lq 9 bits
    msg18.TimeStamp = AisDynamicPara.TimeStamp;                         //lq 6 bits
    //  Spare;                                                          //lq 2 bits, all 0
    msg18.ClassBUnitFlag = AisDynamicPara.ClassBUnitFlag;               //lq 1 bit
    msg18.ClassBDisplayFlag = AisDynamicPara.ClassBDisplayFlag;         //lq 1 bit
    msg18.ClassBDSCFlag = AisDynamicPara.ClassBDscFlag;                 //lq 1 bit
    msg18.ClassBBandFlag = AisDynamicPara.ClassBBandFlag;               //lq 1 bit
    msg18.ClassBMsg22Flag = AisDynamicPara.ClassBMsg22Flag;             //lq 1 bit
    msg18.ModeFlag = AisDynamicPara.ModeFlag;                           //lq 1 bit
    msg18.RAIMFlag = AisDynamicPara.RaimFlag;                           //lq 1 bit
    msg18.CommStateSelectFlag = AisDynamicPara.CommStateSelectorFlag;   //lq 1 bit
    if (commStateType == AIS_MSG_COMMSTATE_SOTDMA)
    {
        msg18.CommunicationState.SOTDMACommState.SyncState
            = commState.SOTDMACommState.SyncState;                      //lq 2 bits
        msg18.CommunicationState.SOTDMACommState.SlotTimeout
            = commState.SOTDMACommState.SlotTimeout;                    //lq 3 bits
        msg18.CommunicationState.SOTDMACommState.SubMessage.SubMsg
            = commState.SOTDMACommState.SubMessage.SubMsg;              //lq 14 bits
    }
    else
    {
        msg18.CommunicationState.ITDMACommState.SyncState
            = commState.ITDMACommState.SyncState;                       //lq 2 bits
        msg18.CommunicationState.ITDMACommState.SlotIncrement
            = commState.ITDMACommState.SlotIncrement;                   //lq 13 bits
        msg18.CommunicationState.ITDMACommState.NumberOfSlots
            = commState.ITDMACommState.NumberOfSlots;                   //lq 3 bits
        msg18.CommunicationState.ITDMACommState.KeepFlag
            = commState.ITDMACommState.KeepFlag;                        //lq 1 bit        
    }
                                                                        //lq total 168 bits��1 slot

    /*lq ������Ϣ�Ķ����Ʊ�ʾ */
    AISMSG_BuildMsg18(&msg18);
}

/*******************************************************************************
* ����  : InitMsg18Test
* ����  : ��ʼ��������Ϣ18
* ����  : ��		  
* ���  : commStateType - ͨ��״̬����
          commState - ͨ��״̬ 
* ����  : ��
*******************************************************************************/
void InitMsg18Test(void)
{
    AISMsgTypeStruct_18 msg18;

    /*lq ��ʼ����Ϣ�����ֶε�ֵ */
    msg18.MessageID = 18;                                               //lq 6 bits
    msg18.RepeatIndicator = 0;                                          //lq 2 bits
    msg18.UserID = AIS_INFO_MMSI_DEFAULT;                                  //lq 30 bits
    //  Spare;                                                          //lq 8 bits, all 0
    msg18.SOG = AIS_INFO_SOG_DEFAULT;                                     //lq 10 bits
    msg18.PositionAccuracy = AIS_INFO_POSITION_ACCURACY_DEFAULT;           //lq 1 bit
    msg18.Longitude = AIS_INFO_LONGITUDE_DEFAULT;                         //lq 28 bits
    msg18.Latitude = AIS_INFO_LATITUDE_DEFAULT;                           //lq 27 bits
    msg18.COG = AIS_INFO_COG_DEFAULT;                                     //lq 12 bits
    msg18.TrueHeading = AIS_INFO_TRUE_HEADING_DEFAULT;                     //lq 9 bits
    msg18.TimeStamp = AIS_INFO_TIME_STAMP_DEFAULT;                         //lq 6 bits
    //  Spare;                                                          //lq 2 bits, all 0
    msg18.ClassBUnitFlag = AIS_INFO_CLASS_B_UNIT_FLAG_DEFAULT;               //lq 1 bit
    msg18.ClassBDisplayFlag = AIS_INFO_CLASS_B_DISPLAY_FLAG_DEFAULT;         //lq 1 bit
    msg18.ClassBDSCFlag = AIS_INFO_CLASS_B_DSC_FLAG_DEFAULT;                 //lq 1 bit
    msg18.ClassBBandFlag = AIS_INFO_CLASS_B_BAND_FLAG_DEFAULT;               //lq 1 bit
    msg18.ClassBMsg22Flag = AIS_INFO_CLASS_B_MSG22_FLAG_DEFAULT;             //lq 1 bit
    msg18.ModeFlag = AIS_INFO_CLASS_B_MODE_FLAG_DEFAULT;                           //lq 1 bit
    msg18.RAIMFlag = AIS_INFO_CLASS_B_RAIM_FLAG_DEFAULT;                           //lq 1 bit
    msg18.CommStateSelectFlag = AIS_INFO_CLASS_B_COMM_STATE_FLAG_DEFAULT;   //lq 1 bit
    msg18.CommunicationState.SOTDMACommState.SyncState
        = 0;                      											//lq 2 bits
    msg18.CommunicationState.SOTDMACommState.SlotTimeout
        = 0;                   												//lq 3 bits
    msg18.CommunicationState.SOTDMACommState.SubMessage.SubMsg
            = 0;              													//lq 14 bits

    /*lq ������Ϣ�Ķ����Ʊ�ʾ */
    AISMSG_BuildMsg18(&msg18);
}

/*******************************************************************************
* ����  : InitMsg19
* ����  : ��ʼ����Ϣ19
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitMsg19(void)
{
    AISMsgTypeStruct_19 msg19;

    msg19.MessageID = 19;                                                   //lq 6 bits
    msg19.RepeatIndicator = 0;                                              //lq 2 bits
    msg19.UserID = AisStaticPara.MMSI;                                      //lq 30 bits
    //  Spare;                                                              //lq 8 bits, all 0
    msg19.SOG = AisDynamicPara.SOG;                                         //lq 10 bits
    msg19.PositionAccuracy = AisDynamicPara.PositionAccuracy;               //lq 1 bit
    msg19.Longitude = AisDynamicPara.Longitude;                             //lq 28 bits
    msg19.Latitude = AisDynamicPara.Latitude;                               //lq 27 bits
    msg19.COG = AisDynamicPara.COG;                                         //lq 12 bits
    msg19.TrueHeading = AisDynamicPara.TrueHeading;                         //lq 9 bits
    msg19.TimeStamp = AisDynamicPara.TimeStamp;                             //lq 6 bits
    //  Spare;                                                              //lq 4 bits, all 0
    strncpy(msg19.Name, AisStaticPara.ShipName, AIS_INFO_SHIP_NAME_LEN);    //lq 120 bits (6-bit ASCII)
    msg19.TypeOfShipAndCargo = AisStaticPara.TypeOfShip;                    //lq 8 bits
    msg19.DimensionA = AisStaticPara.ShipA;                                 //lq 9 bits
    msg19.DimensionB = AisStaticPara.ShipB;                                 //lq 9 bits
    msg19.DimensionC = AisStaticPara.ShipC;                                 //lq 6 bits
    msg19.DimensionD = AisStaticPara.ShipD;                                 //lq 6 bits
    msg19.TypeOfEPFD = AisStaticPara.TypeOfEPFS;                            //lq 4 bits
    msg19.RAIMFlag = AisDynamicPara.RaimFlag;                               //lq 1 bit
    msg19.DTE = AisStaticPara.DTE;                                          //lq 1 bits
    msg19.AssignedModeFlag = AisDynamicPara.ModeFlag;                       //lq 1 bit
    //  Spare;                                                              //lq 4 bits, all 0
                                                                            //lq total 312 bits��2 slots
    /*lq ������Ϣ�Ķ����Ʊ�ʾ */
    AISMSG_BuildMsg19(&msg19);
}

#if 0
/*******************************************************************************
* ����  : InitMsg21
* ����  : ��ʼ����Ϣ21
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitMsg21(void)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_4;
	U8 len_5;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = G_Msg21;

	bitmap_set_value(&msg_bitmap, 0, 8);//ռλ                                  //lq msg_bitmap.array[0]������ݵĳ��ȣ���λbyte��

	bitmap_set_value(&msg_bitmap, 21, 6);                                       // Message ID
	bitmap_set_value(&msg_bitmap, 0, 2);                                        // Repeat indicator
	bitmap_set_value(&msg_bitmap, aiAISCell.UserID, 30);                        // User ID
	bitmap_set_value(&msg_bitmap, aiAISCell.TypeofAtoN, 5);                     // Type of aids-tonavigation
	
    /*lq AtoN name*/
    Temp = strlen(aiAISCell.Name);
    if (Temp >= 20)
    {
        for(i = 0; i < 20; i++)
    	{
    		bitmap_set_value(&msg_bitmap, aiAISCell.Name[i], 6);                // Name of Aidsto-Navigation
    	}    
    }
    else
    {
        for(i = 0; i < Temp; i++)
    	{
    		bitmap_set_value(&msg_bitmap, aiAISCell.Name[i], 6);                // Name of Aidsto-Navigation
    	}
        
        /*lq ����20���ַ��á�@�����*/
        for(i = Temp; i < 20; i++)
        {
    		bitmap_set_value(&msg_bitmap, '@', 6);                                  // Name of Aidsto-Navigation        
        }                      
    }
       	
	bitmap_set_value(&msg_bitmap, Msg18_Positionaccuracy, 1);                   // Position accuracy
	bitmap_set_value(&msg_bitmap, Msg18_Longitude, 28);                         // Longitude
	bitmap_set_value(&msg_bitmap, Msg18_Latitude, 27);                          // Latitude
	bitmap_set_value(&msg_bitmap, aiAISCell.Dimension_Position[0], 9);          // Dimension A of ship/reference for position
	bitmap_set_value(&msg_bitmap, aiAISCell.Dimension_Position[1], 9);          // Dimension B of ship/reference for position 
	bitmap_set_value(&msg_bitmap, aiAISCell.Dimension_Position[2], 6);          // Dimension C of ship/reference for position
	bitmap_set_value(&msg_bitmap, aiAISCell.Dimension_Position[3], 6);          // Dimension D of ship/reference for position
    
    /*lq ����AtoN���ͺ�AtoN�����־������Ϣ��EPFS����*/
    if ((IsFixedAtoN() == TRUE) 
        || (siStaticPara.VirtualAtoNFlag == VIRTUAL_ATON_FLAG_VIRTUAL))
    {
	    bitmap_set_value(&msg_bitmap, 7, 4);                                    // Type of electronic position fixing device        
    }
    else
    {
	    bitmap_set_value(&msg_bitmap, aiAISCell.TypeofEPFS, 4);                 // Type of electronic position fixing device    
    }

	bitmap_set_value(&msg_bitmap, Msg18_Timestamp, 6);                          // Time stamp	
    bitmap_set_value(&msg_bitmap, Msg21_OffPositionIndicator, 1);               // Off-position indicator
    bitmap_set_value(&msg_bitmap, aiAISCell.AtoNStatus, 8);                     // AtoN status
	bitmap_set_value(&msg_bitmap, Msg18_RAIMflag, 1);                           // RAIM-flag
	bitmap_set_value(&msg_bitmap, aiAISCell.VirtualAtoNFlag, 1);                // Virtual AtoN flag
	bitmap_set_value(&msg_bitmap, Msg19_AssignedModeFlag, 1);                   // Assigned mode flag
	bitmap_set_value(&msg_bitmap, 0, 1);                // Spare
    
    /*lq AtoN name ��չ*/
    if (strlen(aiAISCell.Name) > 20)
    {
    	Temp = strlen(aiAISCell.Name);
        for(i = 20; i < Temp; i++)
    	{
    		bitmap_set_value(&msg_bitmap, aiAISCell.Name[i], 6);                // Name of Aidsto-Navigation
    	}

        /*lq �߽����*/
        i = i - 20;     //lq �����ַ���
        i = 6 * i;      //lq ����������
        i = i % 8;
        if ( i != 0)
        {
            i = 8 - i;  //lq �������� 
            bitmap_set_value(&msg_bitmap, 0, i);                                // Name of Aidsto-Navigation        
        }               
    }

    len_4 = msg_bitmap.index;
	len_5 = msg_bitmap.offset == 0 ? len_4 - 1 : len_4;
	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;
	
	bitmap_set_value(&msg_bitmap, len_5, 8);
}
#endif

/*******************************************************************************
* ����  : InitMsg24A
* ����  : ��ʼ����Ϣ24A
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitMsg24A(void)
{
    AISMsgTypeStruct_24A msg24A;

    msg24A.MessageID = 24;                                                  //lq 6 bits
    msg24A.RepeatIndicator = 0;                                             //lq 2 bits
    msg24A.UserID = AisStaticPara.MMSI;                                     //lq 30 bits
    msg24A.PartNumber = 0;                                                  //lq 2 bits
    strncpy(msg24A.Name, AisStaticPara.ShipName, AIS_INFO_SHIP_NAME_LEN);   //lq 120 bits (6-bit ASCII)
                                                                            //lq total 160 bits��1 slot

    /*lq ������Ϣ�Ķ����Ʊ�ʾ */
    AISMSG_BuildMsg24A(&msg24A);
}

/*******************************************************************************
* ����  : InitMsg24B
* ����  : ��ʼ����Ϣ24B
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void InitMsg24B(void)
{
    S16 i;
    AISMsgTypeStruct_24B msg24B;

    msg24B.MessageID = 24;                                                      //lq 6 bits
    msg24B.RepeatIndicator = 0;                                                 //lq 2 bits
    msg24B.UserID = AisStaticPara.MMSI;                                         //lq 30 bits
    msg24B.PartNumber = 1;                                                      //lq 2 bits
    msg24B.TypeOfShipAndCargo = AisStaticPara.TypeOfShip;                       //lq 8 bits
    for (i = 0; i < 3; i++)
    {   
        msg24B.VendorID.ManufacturerID[i] = (AisStaticPara.VendID[i] & 0x3F);   //lq 18bits,3 6-bit ASCII characters
    }
    msg24B.VendorID.UnitModelCode = (AisStaticPara.VendID[3] >> 2) & 0x0F;      //lq 4bits,0-15
    msg24B.VendorID.UnitSerialNumber = (AisStaticPara.VendID[3] & 0x03) << 18
        || ((AisStaticPara.VendID[4] & 0x3F) << 12)
        || ((AisStaticPara.VendID[5] & 0x3F) << 6)
        || ((AisStaticPara.VendID[7] & 0x3F));                                  //lq 20bits
    strncpy(msg24B.CallSign, AisStaticPara.CallSign, AIS_INFO_CALL_SIGN_LEN);   //lq 42 bits (6-bit ASCII)
    msg24B.DimensionA = AisStaticPara.ShipA;                                    //lq 9 bits
    msg24B.DimensionB = AisStaticPara.ShipB;                                    //lq 9 bits
    msg24B.DimensionC = AisStaticPara.ShipC;                                    //lq 6 bits
    msg24B.DimensionD = AisStaticPara.ShipD;                                    //lq 6 bits
    //  Spare;                                                                  //lq 6 bits, all 0
                                                                                //lq total 168 bits��1 slot

    /*lq ������Ϣ�Ķ����Ʊ�ʾ */
    AISMSG_BuildMsg24B(&msg24B);
}

#if 0
/******************************************
������InitMsgPara
����: ��ʼ����Ϣ����
����:
���:
����ֵ��
��ע��
*******************************************/
USINT InitMsgPara(void)
{
	Msg18_Reservedforregional = 0;
	Msg18_SOG = 1023;//0;//1023;
	Msg18_Positionaccuracy = 0;
	Msg18_Longitude = 0x6791AC0;//0x4310D2C;//0x6791AC0;
	Msg18_Latitude = 0x3412140;//0x1667B7C;//0x3412140;
	Msg18_COG = 3600;//0;//3600;
	Msg18_Trueheading = 511;
//lq	Msg18_Timestamp = 60;
	Msg18_Timestamp = 63;
	Msg18_Reservedforregionalapplication = 1;
	Msg18_B_unitflag = 0;
	Msg18_B_displayflag = 0;
	Msg18_B_dscflag = 0;
	Msg18_B_bandflag = 1;
	Msg18_B_22flag = 0;
	Msg18_B_modeflag = 0;
	Msg18_RAIMflag = 0;
	Msg18_Communicationstateselectorflag =1;
	Msg18_CommunicationState = 0xD86E;//ITDMA��ʽ��0x1D86E����ͬ����ʽΪUTCֱ��ͬ����offsetΪ7���ӣ�ռ����3��ʱ϶����ռ����һ֡�еĸ�ʱ϶

	Msg19_Typeofelectronicposition = siStaticPara.TypeofEPFS;
	Msg19_RAIMflag = 0;
	Msg19_DTE = 1;
    Msg19_AssignedModeFlag = 0;
    Msg21_OffPositionIndicator = 0;
#if CONFIG_MSG_6_DAC_IFM
    Msg06_DAC = 235;                    
    Msg06_FI = 10;
#else
    Msg06_DAC = 413;                    
    Msg06_FI = 1;
#endif
    Msg06_AnalogueInternal = 720;       //lq ��Ӧ36v
    Msg06_AnalogueExternalNo1 = 720;    //lq ��Ӧ36v;
    Msg06_AnalogueExternalNo2 = 720;    //lq ��Ӧ36v;
    Msg06_StatusInternal = 0x0;         //lq... �����ȷ��
    Msg06_StatusExternal = 0x0;         //lq... �����ȷ��
    Msg06_OffPositionStatus = 0;        //lq on position
    
    InitMsg21();
    InitMsg6();
	return 0;
}
#endif

#if 0 
/******************************************
������ResetMsgPara
����: �������Ѻ��ʼ����Ϣ����
����:
���:
����ֵ��
��ע��
*******************************************/
void ResetMsgPara(void)
{
    AisDynamicPara.SOG = GpsPara.GPS_Sog;
	AisDynamicPara.COG = GpsPara.GPS_Cog;
    AisDynamicPara.Longitude = GpsPara.GPS_Longitud;
	AisDynamicPara.Latitude = GpsPara.GPS_Latitude;
}
#endif

/*******************************************************************************
* ����  : UpdateDynamicReportInterval
* ����  : ���¶�̬��Ϣ����������Ŀǰ�����ݺ��ٸı䱨������ClassB"SO" ��
          "Nominal reporting interval"��Ϊ"Increased reporting interval"������
          ����ǰʱ϶��ռ����������ж�Ҳ���ڸú����н��У�
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void UpdateDynamicReportInterval(void)
{
    if (AisDynamicPara.SOG != AIS_MSG_SOG_INVALID)
    {
        if (AisDynamicPara.SOG <= TDMA_REPORT_INTERVAL_SOG_MAX_B_3M)
        {
            TDMA_InstReportIntervalSample(TDMA_REPORT_INTERVAL_SLOT_3M);
        }
        else if (AisDynamicPara.SOG <= TDMA_REPORT_INTERVAL_SOG_MAX_B_30S)
        {
            TDMA_InstReportIntervalSample(TDMA_REPORT_INTERVAL_SLOT_30S);
        }
        else if (AisDynamicPara.SOG <= TDMA_REPORT_INTERVAL_SOG_MAX_B_15S)
        {
            TDMA_InstReportIntervalSample(TDMA_REPORT_INTERVAL_SLOT_15S);
        }
        else
        {
            TDMA_InstReportIntervalSample(TDMA_REPORT_INTERVAL_SLOT_5S);
        }
    }
}

/*******************************************************************************
* ����  : UpdateAisDynamicInfo
* ����  : ����AIS��̬��Ϣ
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 UpdateAisDynamicInfo(void)
{
	U8 rc;
    float sfTemp;	

	rc = GetGpsFixSta(&pGpsCell);

	if (rc == TRUE)
	{
		AisDynamicPara.SOG = pGpsCell.GPS_Sog;
        AisDynamicPara.SOG = AisDynamicPara.SOG > 1022 ? 1022 : AisDynamicPara.SOG;

        AisDynamicPara.Longitude = pGpsCell.GPS_Longitud;
		AisDynamicPara.Latitude = pGpsCell.GPS_Latitude;

		AisDynamicPara.COG = pGpsCell.GPS_Cog;
        
        /*lq ʱ�������*/
        AisDynamicPara.TimeStamp = pGpsCell.GPS_TimeS;
    }
    else
    {               
        /*lq ʱ�������*/
        AisDynamicPara.TimeStamp = AIS_INFO_TIME_STAMP_DEFAULT;
    }

    /*lq RAIM��PA����*/
    if (pGpsCell.GPS_RAIMFlag == TRUE)
    {
        /*lq ����RAIM*/
        AisDynamicPara.RaimFlag = 1;
        
        /*lq ����EXPECTED RAIM error������PA*/
        sfTemp =  sqrt(pGpsCell.GPS_errlat * pGpsCell.GPS_errlat + pGpsCell.GPS_errlon * pGpsCell.GPS_errlon);
        if (sfTemp <= RAIM_ERR_THRES)
        {
            AisDynamicPara.PositionAccuracy = 1;
        }
        else
        {
            AisDynamicPara.PositionAccuracy = 0;        
        }   
    }
    else
    {
        /*lq ����RAIM*/ 
        AisDynamicPara.RaimFlag = 0;

        /*lq ����quality indicator������PA*/            
        if (pGpsCell.GPS_QualityIndicator == 0)
        {
            AisDynamicPara.PositionAccuracy = 0;       
        }
        else
        {
            AisDynamicPara.PositionAccuracy = 1;        
        }    
    }

#if 0
    /*lq ����AIS��Ϣ�еľ�̬�ֶ� */
    if (siStaticPara.Refresh == TRUE)
    {
        siStaticPara.Refresh = FALSE;
        AISStcStructInit();
    }
#endif	
    return 0;
}


#if 0
/*******************************************************************************
* ����  : Update_GpsData_To_Msg
* ����  : �ӵײ��ȡGPS���������ж�GPS�Ƿ�λ��Ч���Ƿ�ɲ�����Ϣ
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U16 Update_GpsData_To_Msg(void)
{
	U8 rc;
    float sfTemp;	

	rc = GetGpsFixSta(&pGpsCell);                                               //lq? Ϊ��Ҫ��������һ��GPSStruct����,����ֱ����GpsPara?

	if(rc == TRUE)                                                              //lq Ҫ����Э��ջ������Ϣ
	{
#if !CONFIG_SOG_FOR_BAT_VOLUME
#if 1
		Msg18_SOG = pGpsCell.GPS_Sog;
#else
		Msg18_SOG = GetSOG();
#endif	        
        Msg18_SOG = Msg18_SOG > 1023 ? 1022 : Msg18_SOG;
#endif		
        Msg18_Longitude = pGpsCell.GPS_Longitud;
		Msg18_Latitude = pGpsCell.GPS_Latitude;

#if !CONFIG_COG_FOR_TEMPERATURE
		Msg18_COG = pGpsCell.GPS_Cog;
#endif
        
        /*lq ʱ�������*/
        //lq Msg18_Timestamp = pGpsCell.GPS_TimeS;                              //lq ÿ����Ϣֻ��һ��ʱ������ʹ��
        Msg18_Timestamp = ((float)GetSlotNum()) / 37.5;                         //lq ��Ϣ����ʱGPS�п����Ѿ��ر�

        /*lq ����Ч��ʹ��֮ǰ��ֵ */
        if (OP1_GetGPSOffPositionValid() == TRUE)
        {
            if (OP1_GetGPSOffPositionStat() == TRUE)        
            {
                Msg21_OffPositionIndicator = 1;            
            }
            else
            {
                Msg21_OffPositionIndicator = 0;
            }
        }
    }
    else
    {
        if (GetGpsNormalFixState() == FALSE)                                    //lq �빦���Ƿ����Ŀ�������һ��
        {
            /*lq ��γ�ȸ���*/
            Msg18_Longitude = 0x6791AC0;
    		Msg18_Latitude = 0x3412140;        
        }
                
        /*lq ʱ�������*/
        Msg18_Timestamp = 63;
    }

    /*lq RAIM��PA����*/
    if (pGpsCell.GPS_RAIMFlag == TRUE)
    {
        /*lq ����RAIM*/
        Msg18_RAIMflag = 1;
        Msg19_RAIMflag = 1;
        
        /*lq ����EXPECTED RAIM error������PA*/
        sfTemp =  sqrt(pGpsCell.GPS_errlat * pGpsCell.GPS_errlat + pGpsCell.GPS_errlon * pGpsCell.GPS_errlon);
        if (sfTemp <= RAIM_ERR_THRES)
        {
            Msg18_Positionaccuracy = 1;
        }
        else
        {
            Msg18_Positionaccuracy = 0;        
        }   
    }
    else
    {
        /*lq ����RAIM*/ 
        Msg18_RAIMflag = 0;
        Msg19_RAIMflag = 0;

        /*lq ����quality indicator������PA*/            
        if (pGpsCell.GPS_QualityIndicator == 0)
        {
            Msg18_Positionaccuracy = 0;       
        }
        else
        {
            Msg18_Positionaccuracy = 1;        
        }    
    }

    /*lq ����AIS��Ϣ�еľ�̬�ֶ� */
    if (siStaticPara.Refresh == TRUE)
    {
        siStaticPara.Refresh = FALSE;
        AISStcStructInit();
    }

#if CONFIG_SOG_FOR_BAT_VOLUME
    
    /*lq ���ٸ�������ص����ٷֱ�*/
    if (!((GetBurstStarFlg() == TRUE) && (GetBurstFinsFlg() == FALSE))) //lq ��Ϣ�����ڼ䲻����SOG����ʱ��ѹֵ��׼ȷ
    {
        Msg18_SOG = (GetBatteryVolume() + 0.05) * 10;      //lq ��Ӧ0-1 
        Msg18_SOG = Msg18_SOG > 1023 ? 1022 : Msg18_SOG;
    }
#endif

#if CONFIG_COG_FOR_TEMPERATURE
    
    /*lq COG�����������¶�*/
    if (!((GetBurstStarFlg() == TRUE) && (GetBurstFinsFlg() == FALSE))) //lq ��Ϣ�����ڼ䲻����SOG����ʱ��ѹֵ��׼ȷ
    {
		Msg18_COG = GetTemperForCOG(struTemperSensorPara.fsTemper);     //lq? GetTemper()�ķ���ֵΪ�β��ԣ�
    }
#endif
	
    return 0;
}
#endif

/******************************************
������GetTemperForCOG
����: �������¶�ת��ΪCOG�������¶ȵ�Ĭ�Ϸ�Χ
      Ϊ-99.9~+99.9�������˷�Χ���¶���COG��ʾ
      ��Ч
����:
���:
����ֵ��ת��ΪCOG��Ļ����¶�
��ע��
*******************************************/
U16 GetTemperForCOG(float Temper)
{
    U16 TemperForCOG;

    if (Temper >= 0.0)
    {
        /*lq �¶�Ϊ��*/
        Temper += 0.05;                         //lq ����
        TemperForCOG = (U16)(Temper * 10);      //lq ת��ΪCOG
    }
    else
    {
        /*lq �¶�Ϊ��*/
        Temper = 100.05 - Temper;               //lq �����������ƣ�ͬʱ����λ��1��־�¶�Ϊ��
        TemperForCOG = (U16)(Temper * 10);      //lq ת��ΪCOG
    }

    /*lq COG��Чֵ�ж�*/
    if (TemperForCOG > 3600)
    {
        TemperForCOG = 3600;
    }

    return TemperForCOG;
}

#if CONFIG_BAT_MONITOR 
/******************************************
������GetBatteryVolume
����: �����ص���
����:
���:
����ֵ��BatVolumePerct:��ص����ٷֱ�
��ע��
*******************************************/
float GetBatteryVolume(void)
{
    float BatVolumePerct;
    
    BatVolumePerct = (float)(BS1_GetCurVoltValue() - BATTERY_VOLUME_LEAST_VOLT) 
            / (float)(BATTERY_VOLUME_MOST_VOLT - BATTERY_VOLUME_LEAST_VOLT);

    /*lq ���ָʾ0.1*/
    if (BatVolumePerct < 0.1)
    {
        BatVolumePerct = 0.1;
    }
    else if (BatVolumePerct > 1.0)
    {
        BatVolumePerct = 1.0;
    }

    return BatVolumePerct;
}
#endif
/*******************************************************************************
* ����  : GetSOG
* ����  : ���ݾ�γ�ȼ��㺽��
* ����  : ��		  
* ���  : ��
* ����  : ��ǰ����
*******************************************************************************/
U16 GetSOG(void)
{
    S16 ssSDiff;
	S16 ssMDiff;
	S16 ssHDiff;
	S16 ssDDiff;
    S16 ssSecDiffSum;
	U8 ucValidFlg;
    U8 ucSpanFlg;
	U16 usSOG;
	double latitudeLast_D;
	double longitudLast_D;
	double latitude_D;
	double longitud_D;
	double distance;
	double speed;

    if (pGpsCell.GPS_FirstLLFlg == TRUE)                                           //lq ���ǿ�����һ��
    {
        if (GpsPara.GPS_LonLatFilterFlg == TRUE)
        {
        	/*lq ���㵱ǰʱ�����һ��֮��*/
            ssSDiff = pGpsCell.GPS_TimeS - pGpsCell.GPS_TimeS_Last;
            ssMDiff = pGpsCell.GPS_TimeM - pGpsCell.GPS_TimeM_Last;
        	ssHDiff = pGpsCell.GPS_TimeH - pGpsCell.GPS_TimeH_Last;
        	ssDDiff = pGpsCell.GPS_TimeD - pGpsCell.GPS_TimeD_Last;
            
            if (ssSDiff < 0)
            {
                ssSDiff += 60;
                ssMDiff--;
            }
        
            if (ssMDiff < 0)
            {
                ssMDiff += 60;
                ssHDiff--;
            }
        
            if (ssHDiff < 0)
            {
                ssHDiff += 24;
                ssDDiff--;
            }
        
            if (ssDDiff < 0)                                                        //lq ���£�������    
            {
                ucValidFlg = FALSE;
            }
            else
            {            
                ucValidFlg = TRUE;
    
                ssSecDiffSum = ssSDiff + ssMDiff * 60 + ssHDiff * 3600;                
    		}
            
            if (ucValidFlg && ssSecDiffSum >= 46 && ssSecDiffSum <= 1800)           //lq 46s~30min    
            {
                ucSpanFlg = TRUE;
            }
            else
            {
                ucSpanFlg = FALSE;
            }
        
            /*lq ���ݾ�γ�ȼ��㺽��*/
            if (ucSpanFlg)
        	{
        		/*lq 0.0001��ת��Ϊ��*/
        		latitudeLast_D = (double)pGpsCell.GPS_Latitude_Last / 600000.0;
        		longitudLast_D = (double)pGpsCell.GPS_Longitud_Last / 600000.0;
        	 	latitude_D = (double)pGpsCell.GPS_Latitude / 600000.0;
        		longitud_D = (double)pGpsCell.GPS_Longitud / 600000.0;
        		
        		distance = GetDistance(latitude_D, longitud_D, latitudeLast_D, longitudLast_D);
        		distance /= 1852.0;													//lq ����ɺ���
    
                speed = distance / (double)ssSecDiffSum * 3600.0;					//lq ����knot                		
                usSOG = (U16)(speed * 10 + 0.5);									//lq Msg18_SOG�ĵ�λΪ0.1knot
                pGpsCell.GPS_Sog_Last = usSOG;
    
            #if 1            
                usart_printf("distance: %lf\r\n", distance);//lqdb
                usart_printf("ssSecDiffSum: %d\r\n", ssSecDiffSum);//lqdb
    
                usart_printf("SOG_GPS: %ld\r\n", pGpsCell.GPS_Sog);//lqdb
                usart_printf("SOG_CAL: %ld\r\n", usSOG);//lqdb    
            #endif 		        
            }
        	else                                                                    //lq ʹ��GPS���ĵ��ٶ�    
        	{
        	    //lq usSOG = pGpsCell.GPS_Sog;
                usSOG = pGpsCell.GPS_Sog_Last;                                  //lq ʹ���ϴμ�����ٶ�
        	}
        
            /*lq ��¼��ǰ��γ�Ⱥ�ʱ��*/
            if (!(ucValidFlg && ssSecDiffSum < 46))
            {
        		pGpsCell.GPS_Latitude_Last = pGpsCell.GPS_Latitude;
        		pGpsCell.GPS_Longitud_Last = pGpsCell.GPS_Longitud;
        		pGpsCell.GPS_TimeD_Last = pGpsCell.GPS_TimeD;
        		pGpsCell.GPS_TimeH_Last = pGpsCell.GPS_TimeH;
        		pGpsCell.GPS_TimeM_Last = pGpsCell.GPS_TimeM;		    
        		pGpsCell.GPS_TimeS_Last = pGpsCell.GPS_TimeS;		      
            }         
        }
        else
        {
            usSOG = pGpsCell.GPS_Sog;//lqdb                		            
        }
    }
    else
    {
        if (GpsPara.GPS_LonLatFilterFlg == TRUE)
        {
    		pGpsCell.GPS_Latitude_Last = pGpsCell.GPS_Latitude;
    		pGpsCell.GPS_Longitud_Last = pGpsCell.GPS_Longitud;
    		pGpsCell.GPS_TimeD_Last = pGpsCell.GPS_TimeD;
    		pGpsCell.GPS_TimeH_Last = pGpsCell.GPS_TimeH;
    		pGpsCell.GPS_TimeM_Last = pGpsCell.GPS_TimeM;		    
    		pGpsCell.GPS_TimeS_Last = pGpsCell.GPS_TimeS;    	
            pGpsCell.GPS_Sog_Last = pGpsCell.GPS_Sog;
            
            pGpsCell.GPS_FirstLLFlg = TRUE; 		    	
        }
      
        usSOG = pGpsCell.GPS_Sog;//lqdb                		    
    }

	return usSOG;
}

#if 1
const double EARTH_RADIUS = 6378.137;
const double PI = 3.1415926;
/*******************************************************************************
* ����  : rad
* ����  : ��ת��Ϊ����
* ����  : ��		  
* ���  : ��
* ����  : ����
*******************************************************************************/
static double rad(double d)
{
   return d * PI / 180.0;
}

#if 0
/*******************************************************************************
* ����  : GetDistance
* ����  : ��������ľ�γ�ȼ��������ľ���
* ����  : ����ľ�γ��		  
* ���  : ��
* ����  : �����ľ���
*******************************************************************************/ 
double GetDistance(double lat1, double lng1, double lat2, double lng2)
{
   double radLat1 = rad(lat1);
   
   double radLat2 = rad(lat2);
   
   double a = radLat1 - radLat2;
   
   double b = rad(lng1) - rad(lng2);
   
   double s = 2 * asin(sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2)));
   
   s = s * EARTH_RADIUS;
   
//   s = round(s * 10000) / 10000;		  										//lq ��ȷ��1����
   
   return s;
}
#else
/*******************************************************************************
* ����  : GetDistance
* ����  : ��������ľ�γ�ȼ��������ľ���
* ����  : ����ľ�γ��		  
* ���  : ��
* ����  : �����ľ��루��λ���ף�
*******************************************************************************/ 
S32 GetDistance(double lat1, double lng1, double lat2, double lng2)
{
   double radLat1 = rad(lat1);
   
   double radLat2 = rad(lat2);
   
   double a = radLat1 - radLat2;
   
   double b = rad(lng1) - rad(lng2);
   
   double s = 2 * asin(sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2)));
   
   s = s * EARTH_RADIUS;
   
   s = s * 1000 + 0.5;		  										//lq ��ȷ��1��
   
   return (S32)s;
}
#endif
#endif
