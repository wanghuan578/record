/*
* Copyright (c) 2012,surfing
* All rights reserved.
*
* 文件名称：pure_timer.cpp
* 摘要：Epoll定时器的方法实现
*/

/*
* 版本：1.0.0
* 作者：张乔骏，完成日期：2012年12月3日
*/

#include <sys/time.h>
#include <assert.h>
#include "timer/pure_timer.h"
#include "pure_timer_event.h"
#include "datas/pure_rbtree.h"

using namespace p_base;

ul64 CPure_timer::pure_find_timer()
{
	CPure_timer_event * min_ev;

	node_value = prbt.tree_minimum_key((void **)&min_ev);

	//printf("this = %x,node_value = %ld",this,node_value);
	ul64 cur_time = pure_get_msec();

    return (ul64) (node_value > cur_time? node_value-cur_time : 0);
}

s32 CPure_timer::pure_timer_process()
{
	CPure_timer_event * ev = NULL;
	ul64 now = pure_get_msec();
	for ( ;; )
	{
         rb_node * node = prbt.tree_minimum_node();
		ev = (CPure_timer_event *)(node->data);

		if (node == prbt.nil)
		{
			return -1;
         }

		s32 delta = node->key - now;
		//printf("dleta = %d,key = %ld , now = %ld\n",delta,node->key,now);
         if (delta <= 0)
		{
			pure_delete_timer(ev);
			ev->handle_expire_timer_ex(this);
		}
		else
		{
			return -1;
		}

	}
	//printf("out timer\n");
	return 0;
}

s32 CPure_timer::pure_add_timer(ul64 timer, CBase_timer_event * ev)
{
	
	if(ev->add_to_timer(timer + pure_get_msec(),this) == SUCCESS)
	{
		assert(ev->m_ev_node.lson == NULL );
		assert(ev->m_ev_node.rson == NULL);
		prbt.rb_insert(&ev->m_ev_node);
	}
	return 0;
}

s32 CPure_timer::pure_delete_timer(CBase_timer_event * ev)
{

	if(ev->remove_timer() == SUCCESS)
	{
		PDEBUG("delete timer \n");
		prbt.rb_delete(&ev->m_ev_node);
		memset(&ev->m_ev_node,0,sizeof(rb_node));
	}

	return 0;

}

ul64 CPure_timer::pure_get_msec()
{
	ul64 msec;
	
	/* for windows
	 struct timeb tp;
	 ftime(&tp);
	 msec = (ul64)tp.time*1000+(ul64)tp.millitm;
	*/

	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv,&tz);

	msec =(ul64)tv.tv_sec*1000+(ul64)tv.tv_usec/1000;

    return msec;
}
