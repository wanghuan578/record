/*
* Copyright (c) 2012,surfing
* All rights reserved.
*
* 文件名称：pure_timer.h
* 摘要：实现Epoll定时器
*/

/*
* 版本：1.0.0
* 作者：张乔骏，完成日期：2012年12月3日
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
		//查找红黑树中的过期事件
		ul64 pure_find_timer();

		//处理红黑树中的过期事件
		s32 pure_timer_process();

		//向红黑树中添加事件
		s32 pure_add_timer(ul64 timer, CBase_timer_event * ev);

		//将事件从红黑树中删除
		s32 pure_delete_timer(CBase_timer_event * ev);
	};
}
#endif
