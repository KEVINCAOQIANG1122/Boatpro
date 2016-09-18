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
#include "LinkPtlReport.h"

LinkPtlReportStruct LinkPtlReportPara;

/************************************************************
*  Function:       InitaReportPara							*
*  Description:    ��ʼ��Report����							*
*  Input:          ��										*	
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void InitaReportPara (void)
{
	LinkPtlReportPara.uiATval = 0;

	LinkPtlReportPara.uiServicNum = 0;
	LinkPtlReportPara.uiTranSta = REPORT_TRANERR_NONE;
}

/************************************************************
*  Function:       ReportToPtl								*
*  Description:    ��Э��ջ���ͱ���							*
*  Input:          ��										*	
*  Output:         ��										*
*  Return:         ��										*
*  Others:         ��										*
************************************************************/
void ReportToPtl (void)
{
	//�б�������
	if (LinkPtlReportPara.uiTranSta != REPORT_TRANERR_NONE)
	{
		

		LinkPtlReportPara.uiTranSta = REPORT_TRANERR_NONE;

	}
	
}

























