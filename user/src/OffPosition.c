/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: batteryscan.c
  Author: lq       Version : V1.00        Date: 2014.01.02
  Description:     ����ɨ��
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "def.h"
#include "initial.h"
#include "OffPosition.h"
#include "config.h"

/*lq ģ��ʵ������*/
GPSOffPositionStruct GPSOffPositionPara;

/*******************************************************************************
* ����  : GPSOffPositionStructInit
* ����  : ��ʼ��GPSλ��ƫ��ṹ�����
* ����  : GPSOffPositionStruct����ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void OP_GPSOffPositionStructInit(GPSOffPositionStruct *pGPSOffPositionStruct)
{
    U16 i;

    for (i = 0; i < GPS_ON_POSITION_SAMPLE_NUM + 1; i++)
    {
        pGPSOffPositionStruct->PositionNormSample[i][0] = 0;
        pGPSOffPositionStruct->PositionNormSample[i][1] = 0;
    }

    for (i = 0; i < GPS_OFF_POSITION_SAMPLE_NUM + 1; i++)
    {
        pGPSOffPositionStruct->PositionOffSample[i][0] = 0;
        pGPSOffPositionStruct->PositionOffSample[i][1] = 0;
    }

    for (i = 0; i < GPS_ON_POSITION_SAMPLE_NUM + 1; i++)
    {
        pGPSOffPositionStruct->PositionFlag[i] = TRUE;
    }
    
    pGPSOffPositionStruct->PositionOffIndexIn = 0;
    pGPSOffPositionStruct->PositionOffIndexOut = 0;
    pGPSOffPositionStruct->PositionFlagIndexIn = 0;
    pGPSOffPositionStruct->PositionFlagIndexOut = 0;

    pGPSOffPositionStruct->InsideSum = 0;

    pGPSOffPositionStruct->AvarDist = 0;
    pGPSOffPositionStruct->AvarLon = 0;
    pGPSOffPositionStruct->AvarLat = 0;
    //lq pGPSOffPositionStruct->OnPositionFlag = FALSE;
    pGPSOffPositionStruct->OffPositionFlag = FALSE;
    pGPSOffPositionStruct->OffPositionPossFlag = FALSE;
    pGPSOffPositionStruct->OffPositionValid = FALSE;
    pGPSOffPositionStruct->OffPositionPossValid = FALSE;
}

/*******************************************************************************
* ����  : GPSOffPositionStructReset
* ����  : ��λGPSλ��ƫ��ṹ�����
* ����  : GPSOffPositionStruct����ָ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void OP_GPSOffPositionStructReset(GPSOffPositionStruct *pGPSOffPositionStruct)
{
    U16 i;

    for (i = 0; i < GPS_ON_POSITION_SAMPLE_NUM + 1; i++)
    {
        pGPSOffPositionStruct->PositionNormSample[i][0] = 0;
        pGPSOffPositionStruct->PositionNormSample[i][1] = 0;
    }

    for (i = 0; i < GPS_OFF_POSITION_SAMPLE_NUM + 1; i++)
    {
        pGPSOffPositionStruct->PositionOffSample[i][0] = 0;
        pGPSOffPositionStruct->PositionOffSample[i][1] = 0;
    }

    for (i = 0; i < GPS_ON_POSITION_SAMPLE_NUM + 1; i++)
    {
        pGPSOffPositionStruct->PositionFlag[i] = TRUE;
    }
    
    pGPSOffPositionStruct->PositionOffIndexIn = 0;
    pGPSOffPositionStruct->PositionOffIndexOut = 0;
    pGPSOffPositionStruct->PositionFlagIndexIn = 0;
    pGPSOffPositionStruct->PositionFlagIndexOut = 0;

    pGPSOffPositionStruct->InsideSum = 0;

    pGPSOffPositionStruct->AvarDist = 0;
    pGPSOffPositionStruct->AvarLon = 0;
    pGPSOffPositionStruct->AvarLat = 0;
    //lq pGPSOffPositionStruct->OnPositionFlag = FALSE;    
    pGPSOffPositionStruct->OffPositionFlag = FALSE;
    pGPSOffPositionStruct->OffPositionPossFlag = FALSE;
    pGPSOffPositionStruct->OffPositionValid = FALSE;
    pGPSOffPositionStruct->OffPositionPossValid = FALSE;
}

/*******************************************************************************
* ����  : GPSOffPositionInstanceInit
* ����  : ��ʼ��GPSλ��ƫ��ṹ��ʵ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void OP1_GPSOffPositionInstanceInit(void)
{
    OP_GPSOffPositionStructInit(&GPSOffPositionPara);
}

/*******************************************************************************
* ����  : GPSOffPositionInstanceInit
* ����  : ��λGPSλ��ƫ��ṹ��ʵ��
* ����  : ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
void OP1_GPSOffPositionInstanceReset(void)
{
    OP_GPSOffPositionStructReset(&GPSOffPositionPara);
}

#if (CONFIG_OFF_POSITON_ALGORITHM == 0)
/*******************************************************************************
* ����  : GPSOffPositionAddSamplePer
* ����  : ��¼�²������λ��ƫ��״̬���ٷֱȷ�����
* ����  : State-λ��ƫ��״̬��TRUE��δƫ�룬FALSE-ƫ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void OP_GPSOffPositionAddSamplePerc(GPSOffPositionStruct *pGPSOffPositionStruct, const U8 State)
{
    /*lq �������� */      
    if (((pGPSOffPositionStruct->PositionFlagIndexIn + 1) % (GPS_OFF_POSITION_SAMPLE_NUM + 1))
        == pGPSOffPositionStruct->PositionFlagIndexOut)
    {
        /*lq �������� */      
        pGPSOffPositionStruct->PositionFlag[pGPSOffPositionStruct->PositionFlagIndexIn] = State;

        /*lq ����ͳ��Ŀǰλ��δƫ���������*/
        if (pGPSOffPositionStruct->PositionFlag[pGPSOffPositionStruct->PositionFlagIndexIn]
            != pGPSOffPositionStruct->PositionFlag[pGPSOffPositionStruct->PositionFlagIndexOut])
        {
            if (pGPSOffPositionStruct->PositionFlag[pGPSOffPositionStruct->PositionFlagIndexIn] == TRUE)
            {
                pGPSOffPositionStruct->InsideSum++;      
            }
            else
            {
                pGPSOffPositionStruct->InsideSum--;      
            }    
        }
        
        /*lq IndexIn������1*/
        pGPSOffPositionStruct->PositionFlagIndexIn++;
        pGPSOffPositionStruct->PositionFlagIndexIn %= (GPS_OFF_POSITION_SAMPLE_NUM + 1);

        /*lq IndexOut������1*/
        pGPSOffPositionStruct->PositionFlagIndexOut++;
        pGPSOffPositionStruct->PositionFlagIndexOut %= (GPS_OFF_POSITION_SAMPLE_NUM + 1);
    }
    
    /*lq ����δ�� */
    else
    {        
        /*lq �������� */      
        pGPSOffPositionStruct->PositionFlag[pGPSOffPositionStruct->PositionFlagIndexIn] = State;

        /*lq ����ͳ��Ŀǰλ��δƫ���������*/
        if (pGPSOffPositionStruct->PositionFlag[pGPSOffPositionStruct->PositionFlagIndexIn] == TRUE)
        {
            pGPSOffPositionStruct->InsideSum++;      
        }

        /*lq IndexIn������1*/
        pGPSOffPositionStruct->PositionFlagIndexIn++;
        pGPSOffPositionStruct->PositionFlagIndexIn %= (GPS_OFF_POSITION_SAMPLE_NUM + 1);
    }
}

/*******************************************************************************
* ����  : GPSPositionAnalPerc
* ����  : GPS��λλ�÷������ٷֱȷ�����
* ����  : Lon-���ȣ���λ0.0001�֣���Lat-γ�ȣ���λ0.0001�֣�
* ���  : ��
* ����  : ��
*******************************************************************************/
static void OP_GPSPositionAnalPerc(GPSOffPositionStruct *pGPSOffPositionStruct, const S32 Lon, const S32 Lat)
{
    float Lon1, Lat1, Lon2, Lat2;
    float Dist;

    /*lq ��ǰ��γ�Ȼ���Ϊ�Զ�Ϊ��λ*/
    Lon1 = Lon / 600000.0;
    Lat1 = Lat / 600000.0;

    /*lq Ŀ�ľ�γ�Ȼ���Ϊ�Զ�Ϊ��λ*/
    Lon2 = siStaticPara.NomLongitude / 600000.0;
    Lat2 = siStaticPara.NomLatitude / 600000.0;
        
    /*lq ���㵱ǰλ��������Ŀ��λ������ľ���*/        
    Dist = GetDistance(Lat1, Lon1, Lat2, Lon2);

    /*lq ���浱ǰλ�������ƫ��״̬*/
    if (Dist > siStaticPara.OffPositionThresh)
    {
        OP_GPSOffPositionAddSamplePerc(pGPSOffPositionStruct, FALSE);
    }
    else
    {
        OP_GPSOffPositionAddSamplePerc(pGPSOffPositionStruct, TRUE);
    }     

    /*lq ƫ��״̬�£�ƫ��״̬����*/
    if (((pGPSOffPositionStruct->PositionFlagIndexIn + 1) % (GPS_OFF_POSITION_SAMPLE_NUM + 1))
        == pGPSOffPositionStruct->PositionFlagIndexOut)
    {
        pGPSOffPositionStruct->OffPositionValid = TRUE;

        /*lq ����Ч��Χ�ڵĵ����Ŀ������ֵ*/
        if (pGPSOffPositionStruct->InsideSum > GPS_OFF_POSITION_PERC_THRES)
        {
            pGPSOffPositionStruct->OffPositionFlag = FALSE;            
        }
        
        /*lq ����Ч��Χ��ĵ����Ŀ������ֵ*/
        else if ((GPS_OFF_POSITION_SAMPLE_NUM - pGPSOffPositionStruct->InsideSum)
                  > GPS_OFF_POSITION_PERC_THRES)
        {
            pGPSOffPositionStruct->OffPositionFlag = TRUE;            
        }       
    }
    else
    {
        /*lq ����״̬�£�ƫ��״̬����*/
        if (pGPSOffPositionStruct->PositionFlagIndexIn >= GPS_ON_POSITION_SAMPLE_NUM)
        {
            pGPSOffPositionStruct->OffPositionPossValid = TRUE;
            
            if (pGPSOffPositionStruct->PositionFlagIndexIn != pGPSOffPositionStruct->InsideSum)
            {
                pGPSOffPositionStruct->OffPositionPossFlag = TRUE;            
            }
        }
    }

#if CONFIG_DEBUG
    usart_printf("[%6d:%4d]$GPSPositionAnalPerc Distance : %f!\r\n", 
                 GetFrameNum(), GetSlotNum(), Dist);
    
    usart_printf("[%6d:%4d]$GPSPositionAnalPerc InsideSum: %d!\r\n", 
                 GetFrameNum(), GetSlotNum() ,pGPSOffPositionStruct->InsideSum);
#endif
}

#elif (CONFIG_OFF_POSITON_ALGORITHM == 1)
/*******************************************************************************
* ����  : GPSOffPositionAddSamplePos
* ����  : ��¼�²�����λ�����꣬ƽ��λ�÷�����
* ����  : Lon-���ȣ�Lat-γ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void OP_GPSOffPositionAddSamplePos(GPSOffPositionStruct *pGPSOffPositionStruct, const S32 Lon, const S32 Lat)
{
    float Temp1;
    float Temp2;
    
    Temp1 = (float)Lon;    
    Temp2 = (float)Lat;

    /*lq �������� */  
    if (((pGPSOffPositionStruct->PositionOffIndexIn + 1) % (GPS_OFF_POSITION_SAMPLE_NUM + 1))
        == pGPSOffPositionStruct->PositionOffIndexOut)
    {
        /*lq �����д��ԭʼֵ��GPS_OFF_POSITION_SAMPLE_NUM��֮1ֵ*/
        Temp1 = (float)Lon / GPS_OFF_POSITION_SAMPLE_NUM;    
        Temp2 = (float)Lat / GPS_OFF_POSITION_SAMPLE_NUM;

        /*lq �������GPS_OFF_POSITION_SAMPLE_NUM�����Ȳ������ƽ��ֵ*/
        pGPSOffPositionStruct->AvarLon -= pGPSOffPositionStruct->PositionOffSample[pGPSOffPositionStruct->PositionOffIndexOut][0];
        pGPSOffPositionStruct->AvarLon += Temp1;

        /*lq �������GPS_OFF_POSITION_SAMPLE_NUM��γ�Ȳ������ƽ��ֵ*/
        pGPSOffPositionStruct->AvarLat -= pGPSOffPositionStruct->PositionOffSample[pGPSOffPositionStruct->PositionOffIndexOut][1];
        pGPSOffPositionStruct->AvarLat += Temp2;

        /*lq �������� */      
        pGPSOffPositionStruct->PositionOffSample[pGPSOffPositionStruct->PositionOffIndexIn][0] = Temp1;
        pGPSOffPositionStruct->PositionOffSample[pGPSOffPositionStruct->PositionOffIndexIn][1] = Temp2;

        /*lq IndexIn������1*/
        pGPSOffPositionStruct->PositionOffIndexIn++;
        pGPSOffPositionStruct->PositionOffIndexIn %= (GPS_OFF_POSITION_SAMPLE_NUM + 1);

        /*lq IndexOut������1*/
        pGPSOffPositionStruct->PositionOffIndexOut++;
        pGPSOffPositionStruct->PositionOffIndexOut %= (GPS_OFF_POSITION_SAMPLE_NUM + 1);
    }
    
    /*lq ����δ�� */
    else
    {        
        /*lq ����ƽ������ֵ*/
    	pGPSOffPositionStruct->AvarLon += (Temp1 - pGPSOffPositionStruct->AvarLon)
                                           / (pGPSOffPositionStruct->PositionOffIndexIn + 1);   
    	pGPSOffPositionStruct->AvarLat += (Temp2 - pGPSOffPositionStruct->AvarLat)
                                           / (pGPSOffPositionStruct->PositionOffIndexIn + 1);

        /*lq �����д��ԭʼֵ��GPS_OFF_POSITION_SAMPLE_NUM��֮1ֵ*/
        Temp1 = (float)Lon / GPS_OFF_POSITION_SAMPLE_NUM;    
        Temp2 = (float)Lat / GPS_OFF_POSITION_SAMPLE_NUM;

        /*lq �������� */
        pGPSOffPositionStruct->PositionOffSample[pGPSOffPositionStruct->PositionOffIndexIn][0] = Temp1;
        pGPSOffPositionStruct->PositionOffSample[pGPSOffPositionStruct->PositionOffIndexIn][1] = Temp2;

        /*lq IndexIn������1*/
        pGPSOffPositionStruct->PositionOffIndexIn++;
        pGPSOffPositionStruct->PositionOffIndexIn %= (GPS_OFF_POSITION_SAMPLE_NUM + 1);
    }
}

/*******************************************************************************
* ����  : GPSPositionAnalAvarPos
* ����  : GPS��λλ�÷�����ƽ��λ�÷�����
* ����  : Lon-���ȣ�Lat-γ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void OP_GPSPositionAnalAvarPos(GPSOffPositionStruct *pGPSOffPositionStruct, const S32 Lon, const S32 Lat)
{
    float Lon1, Lat1, Lon2, Lat2;
    float Dist;

    /*lq �������㣬������λ������ƽ��ֵ*/
    OP_GPSOffPositionAddSamplePos(pGPSOffPositionStruct, Lon, Lat);

    /*lq ƽ����γ�Ȼ���Ϊ�Զ�Ϊ��λ*/
    Lon1 = pGPSOffPositionStruct->AvarLon / 600000.0;
    Lat1 = pGPSOffPositionStruct->AvarLat / 600000.0;

    /*lq Ŀ�ľ�γ�Ȼ���Ϊ�Զ�Ϊ��λ*/
    Lon2 = siStaticPara.NomLongitude / 600000.0;
    Lat2 = siStaticPara.NomLatitude / 600000.0;

    /*lq ���㵱ǰƽ��λ��������Ŀ��λ������ľ���*/
    Dist = GetDistance(Lat1, Lon1, Lat2, Lon2);
    
    /*lq ƫ��״̬�£���ǰλ��ƫ���ж�*/
    if (((pGPSOffPositionStruct->PositionOffIndexIn + 1) % (GPS_OFF_POSITION_SAMPLE_NUM + 1))
        == pGPSOffPositionStruct->PositionOffIndexOut)
    {
        pGPSOffPositionStruct->OffPositionValid = TRUE;
        
        if (Dist > siStaticPara.OffPositionThresh)
        {
            pGPSOffPositionStruct->OffPositionFlag = TRUE;    
        }
        else
        {
            pGPSOffPositionStruct->OffPositionFlag = FALSE;        
        } 
    }
    else 
    {
        /*lq ����״̬�£�ƫ��״̬����*/
        if (pGPSOffPositionStruct->PositionOffIndexIn >= GPS_ON_POSITION_SAMPLE_NUM)
        {
            pGPSOffPositionStruct->OffPositionPossValid = TRUE;

            if (Dist > siStaticPara.OffPositionThresh)
            {
                pGPSOffPositionStruct->OffPositionPossFlag = TRUE;
            }        
        }
    }
}

#elif (CONFIG_OFF_POSITON_ALGORITHM == 2)
/*******************************************************************************
* ����  : GPSOffPositionAddSampleDist
* ����  : ��¼�²�����λ�þ��룬ƽ�����뷽����
* ����  : Lon-���ȣ�Lat-γ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void OP_GPSOffPositionAddSampleDist(GPSOffPositionStruct *pGPSOffPositionStruct, const float Dist)
{
    float Temp1;

    /*lq �������� */     
    if (((pGPSOffPositionStruct->PositionOffIndexIn + 1) % (GPS_OFF_POSITION_SAMPLE_NUM + 1))
        == pGPSOffPositionStruct->PositionOffIndexOut)
    {
        /*lq �����д��ԭʼֵ��GPS_OFF_POSITION_SAMPLE_NUM��֮1ֵ*/
        Temp1 = Dist / GPS_OFF_POSITION_SAMPLE_NUM;    

        /*lq �������GPS_OFF_POSITION_SAMPLE_NUM�����Ȳ���������ƽ��ֵ*/
        pGPSOffPositionStruct->AvarDist -= pGPSOffPositionStruct->PositionOffSample[pGPSOffPositionStruct->PositionOffIndexOut][0];
        pGPSOffPositionStruct->AvarDist += Temp1;

        /*lq �������� */      
        pGPSOffPositionStruct->PositionOffSample[pGPSOffPositionStruct->PositionOffIndexIn][0] = Temp1;

        /*lq IndexIn������1*/
        pGPSOffPositionStruct->PositionOffIndexIn++;
        pGPSOffPositionStruct->PositionOffIndexIn %= (GPS_OFF_POSITION_SAMPLE_NUM + 1);

        /*lq IndexOut������1*/
        pGPSOffPositionStruct->PositionOffIndexOut++;
        pGPSOffPositionStruct->PositionOffIndexOut %= (GPS_OFF_POSITION_SAMPLE_NUM + 1);
    }
    
    /*lq ����δ�� */
    else
    {        
        /*lq ����ƽ������ֵ*/
    	pGPSOffPositionStruct->AvarDist += (Dist - pGPSOffPositionStruct->AvarDist)
                                           / (pGPSOffPositionStruct->PositionOffIndexIn + 1);   

        /*lq �����д��ԭʼֵ��GPS_OFF_POSITION_SAMPLE_NUM��֮1ֵ*/
        Temp1 = Dist / GPS_OFF_POSITION_SAMPLE_NUM;    

        /*lq �������� */
        pGPSOffPositionStruct->PositionOffSample[pGPSOffPositionStruct->PositionOffIndexIn][0] = Temp1;

        /*lq IndexIn������1*/
        pGPSOffPositionStruct->PositionOffIndexIn++;
        pGPSOffPositionStruct->PositionOffIndexIn %= (GPS_OFF_POSITION_SAMPLE_NUM + 1);
    }
}

/*******************************************************************************
* ����  : GPSPositionAnalAvarDist
* ����  : GPS��λλ�÷�����ƽ�����뷽����
* ����  : Lon-���ȣ�Lat-γ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static void OP_GPSPositionAnalAvarDist(GPSOffPositionStruct *pGPSOffPositionStruct, const S32 Lon, const S32 Lat)
{
    float Lon1, Lat1, Lon2, Lat2;
    float Dist;

    /*lq ��ǰ��γ�Ȼ���Ϊ�Զ�Ϊ��λ*/
    Lon1 = Lon / 600000.0;
    Lat1 = Lat / 600000.0;

    /*lq Ŀ�ľ�γ�Ȼ���Ϊ�Զ�Ϊ��λ*/
    Lon2 = siStaticPara.NomLongitude / 600000.0;
    Lat2 = siStaticPara.NomLatitude / 600000.0;
        
    /*lq ���㵱ǰλ��������Ŀ��λ������ľ���*/        
    Dist = GetDistance(Lat1, Lon1, Lat2, Lon2);

    /*lq ���浱ǰλ������ľ��� */
    OP_GPSOffPositionAddSampleDist(pGPSOffPositionStruct, Dist);

    /*lq ƫ��״̬�£���ǰλ��ƫ���ж�*/
    if (((pGPSOffPositionStruct->PositionOffIndexIn + 1) % (GPS_OFF_POSITION_SAMPLE_NUM + 1))
        == pGPSOffPositionStruct->PositionOffIndexOut)
    {
        pGPSOffPositionStruct->OffPositionValid = TRUE;
        
        if (pGPSOffPositionStruct->AvarDist > siStaticPara.OffPositionThresh)
        {
            pGPSOffPositionStruct->OffPositionFlag = TRUE;    
        }
        else
        {
            pGPSOffPositionStruct->OffPositionFlag = FALSE;        
        } 
    }
    else 
    {
        /*lq ����״̬�£�ƫ��״̬����*/
        if (pGPSOffPositionStruct->PositionOffIndexIn >= GPS_ON_POSITION_SAMPLE_NUM)
        {
            pGPSOffPositionStruct->OffPositionPossValid = TRUE;

            if (pGPSOffPositionStruct->AvarDist > siStaticPara.OffPositionThresh)
            {
                pGPSOffPositionStruct->OffPositionPossFlag = TRUE;
            }        
        }
    }    
}
#endif

/*******************************************************************************
* ����  : OP1_GPSPositionAnalPerc
* ����  : GPS��λλ�÷������ⲿ�ӿ�
* ����  : Lon-���ȣ���λ0.0001�֣���Lat-γ�ȣ���λ0.0001�֣�
* ���  : ��
* ����  : ��
*******************************************************************************/
void OP1_GPSPositionAnal(const S32 Lon, const S32 Lat)
{
#if (CONFIG_OFF_POSITON_ALGORITHM == 0)
    OP_GPSPositionAnalPerc(&GPSOffPositionPara, Lon, Lat);

#elif (CONFIG_OFF_POSITON_ALGORITHM == 1)
    OP_GPSPositionAnalAvarPos(&GPSOffPositionPara, Lon, Lat);

#elif (CONFIG_OFF_POSITON_ALGORITHM == 2)
    OP_GPSPositionAnalAvarDist(&GPSOffPositionPara, Lon, Lat);

#endif
}

/*******************************************************************************
* ����  : GetGPSOffPositionStat
* ����  : ��ѯλ��ƫ��״̬
* ����  : TRUE-ƫ�룬FALSE-δƫ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static U8 OP_GetGPSOffPositionStat(GPSOffPositionStruct *pGPSOffPositionStruct)
{
    return pGPSOffPositionStruct->OffPositionFlag;
}

/*******************************************************************************
* ����  : GetGPSOffPositionPossStat
* ����  : ��ѯ����λ��ƫ��״̬
* ����  : TRUE-����ƫ�룬FALSE-δƫ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static U8 OP_GetGPSOffPositionPossStat(GPSOffPositionStruct *pGPSOffPositionStruct)
{
    return pGPSOffPositionStruct->OffPositionPossFlag;
}

/*******************************************************************************
* ����  : OP_GetGPSOffPositionPossValid
* ����  : ��ѯ����λ��ƫ��״̬�Ƿ���Ч
* ����  : TRUE-��Ч��FALSE-��Ч		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static U8 OP_GetGPSOffPositionPossValid(GPSOffPositionStruct *pGPSOffPositionStruct)
{
    return pGPSOffPositionStruct->OffPositionPossValid;
}

/*******************************************************************************
* ����  : OP1_GetGPSOffPositionPossValid
* ����  : ��ѯ����λ��ƫ��״̬�Ƿ���Ч
* ����  : TRUE-��Ч��FALSE-��Ч		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 OP1_GetGPSOffPositionPossValid(void)
{
    return OP_GetGPSOffPositionPossValid(&GPSOffPositionPara);
}

/*******************************************************************************
* ����  : OP_GetGPSOffPositionValid
* ����  : ��ѯλ��ƫ��״̬�Ƿ���Ч
* ����  : TRUE-��Ч��FALSE-��Ч		  
* ���  : ��
* ����  : ��
*******************************************************************************/
static U8 OP_GetGPSOffPositionValid(GPSOffPositionStruct *pGPSOffPositionStruct)
{
    return pGPSOffPositionStruct->OffPositionValid;
}

/*******************************************************************************
* ����  : OP1_GetGPSOffPositionValid
* ����  : ��ѯλ��ƫ��״̬�Ƿ���Ч
* ����  : TRUE-��Ч��FALSE-��Ч		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 OP1_GetGPSOffPositionValid(void)
{
    return OP_GetGPSOffPositionValid(&GPSOffPositionPara);
}

/*******************************************************************************
* ����  : GetGPSOffPositionStat
* ����  : ��ѯλ��ƫ��״̬
* ����  : TRUE-ƫ�룬FALSE-δƫ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 OP1_GetGPSOffPositionStat(void)
{
    return OP_GetGPSOffPositionStat(&GPSOffPositionPara);
}

/*******************************************************************************
* ����  : GetGPSOffPositionPossStat
* ����  : ��ѯ����λ��ƫ��״̬
* ����  : TRUE-����ƫ�룬FALSE-δƫ��		  
* ���  : ��
* ����  : ��
*******************************************************************************/
U8 OP1_GetGPSOffPositionPossStat(void)
{
    return OP_GetGPSOffPositionPossStat(&GPSOffPositionPara);
}


