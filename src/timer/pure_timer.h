/*
* Copyright (c) 2012,surfing
* All rights reserved.
*
* �ļ����ƣ�pure_timer.h
* ժҪ��ʵ��Epoll��ʱ��
*/

/*
* �汾��1.0.0
* ���ߣ����ǿ���������ڣ�2012��12��3��
*/

#ifndef _PURE_TIMER_H_
#define _PURE_TIMER_H_

#include <stdio.h>
//#include <windows.h>
#include <sys/timeb.h>
#include <time.h>
#include "basic_type.h"
#include "datas/pure_rbtree.h"


namespace p_base
{
	class CBase_timer_event;
	
	class CPure_timer
	{
	private:
		
		
		ul64 node_value;

		ul64 timer;
		
		pure_rbtree prbt;

	

	public:
		
		virtual ul64 pure_get_msec();
		//���Һ�����еĹ����¼�
		ul64 pure_find_timer();

		//���������еĹ����¼�
		s32 pure_timer_process();

		//������������¼�
		s32 pure_add_timer(ul64 timer, CBase_timer_event * ev);

		//���¼��Ӻ������ɾ��
		s32 pure_delete_timer(CBase_timer_event * ev);
	};
}
#endif
