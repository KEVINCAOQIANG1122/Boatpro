/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: math_rand.h
  Author: lq       Version : V1.00        Date: 2013.11.04
  Description:     ���������ģ��ĺꡢ�ṹ�嶨�壬��������������
  Version:         V1.00
  Function List:   // ��Ҫ�������书��
  			
  History:         // ��ʷ�޸ļ�¼
      <author>  <time>   <version >   <desc>
*******************************************************************************/

#ifndef _MATH_RAND_H
#define _MATH_RAND_H

#include "def.h"

/*������ṹ��*/
struct math_rand 
{
	U32 seed;	/*���������*/
};



extern U16 RandData(struct math_rand* _rand, U16 a, U16 b);
extern void SetRandSeed(struct math_rand* _rand, U32 seed);


#endif
