/*
* Copyright (c) 2012,surfing
* All rights reserved.
*
* 文件名称：pure_timer_event.h
* 摘要：实现Epoll定时器的事件
*/

/*
* 版本：1.0.0
* 作者：张乔骏，完成日期：2012年12月3日
*/

#ifndef _PURE_TIMER_EVENT_H_
#define _PURE_TIMER_EVENT_H_

#include "basic_type.h"
#include "datas/pure_rbtree.h"
#include "pure_timer.h"
#include "sys_headers.h"

//#include "base/base_connect.h"


namespace p_base
{
	class CPure_timer;
	class CBase_connect;
	class CBase_timer_event
	{
		public:
		rb_node m_ev_node;
	   CBase_timer_event() : m_is_inrbtree(0){ m_ptt = NULL; memset(&m_ev_node,0,sizeof(rb_node));}
        virtual ~CBase_timer_event();
	public:

		virtual s32 add_to_timer(ul64 timer,  CPure_timer * ptt);
		virtual s32 remove_timer();
		virtual s32 handle_expire_timer_ex(CPure_timer *)=0;
				
	protected:
			
		CPure_timer * m_ptt;
		volatile bool m_is_inrbtree;
	};
	
	class CPure_timer_event:public CBase_timer_event
	{
	public:

	   CPure_timer_event(CBase_connect * pcon) { m_pcon = pcon; }
       virtual ~CPure_timer_event(){};
	public:

		virtual s32 handle_expire_timer_ex(CPure_timer *);
		virtual s32 handle_expire_timer(CPure_timer *)=0;

	protected:
		CBase_connect * m_pcon;
	
	};
}
#endif
