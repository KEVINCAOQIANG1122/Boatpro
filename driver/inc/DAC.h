/****************************************************************************
*  Copyright (C), 2011, Institute Mod. Opt. NKU                             *
*  FileName: Dac.h	 	                        	          	            *
*  Description:  DAC�������Ƴ�ʼ����������									*
*****************************************************************************
*  History:               				                                    *
*      <author>  <time>    <version >   <desc>                              *
*      vozhj   	 10/12/02     1.0       build this moudle		            *
****************************************************************************/

#ifndef __DAC_H
#define __DAC_H

#define DA_VALUE_01V    15  //lq 0.01v��Ӧ�ı仯ֵ

extern void DAC_RegInit(void);
extern void SetDAC1Val(S16 siDACVal);
extern void SetDAC2Val(S16 siDACVal);

#endif
