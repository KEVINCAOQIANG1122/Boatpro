/****************************************************************************
*  Copyright (C), 2010, 712.Co.,Ltd.                                        *
*  FileName: LinkPtlReport.h								   						*
*  Description:  �ײ���Э��ջ�����ӿں�����ر������弰��������							*
*****************************************************************************
*  History:               				                                    *
*      <author>  <time>    <version >   <desc>                              *
*      vozhj   	 10/11/25     1.0       Build this file					    *
****************************************************************************/
#ifndef _LINKPTLREPORT_H
#define _LINKPTLREPORT_H


/*****************************************************************************
***********************			�ṹ�嶨��   		***********************
****************************************************************************/

#define REPORT_TRANERR_NONE			0
#define REPORT_TRANERR_PHASEERR		1
#define REPORT_TRANERR_CHERR		2
#define REPORT_TRANERR_SLOTERR0		3
#define REPORT_TRANERR_OK			4
#define REPORT_TRANERR_SLOTERR1		5
#define REPORT_TRANERR_ATERR		6
#define REPORT_TRANERR_LENERR		7
#define REPORT_TRANERR_ENDFRAME		8
#define REPORT_TRANERR_PLLERR		9

typedef struct {
	U16 uiTranSta;			//���ͳɹ�ʧ��״̬
	U16 uiServicNum;			//��ǰ״̬ҵ���

	U16 uiATval;				//AT��־
}LinkPtlReportStruct;

extern LinkPtlReportStruct LinkPtlReportPara;
/*****************************************************************************
***********************			��������    		***********************
****************************************************************************/

extern void InitaReportPara (void);
extern void ReportToPtl (void);
#endif











