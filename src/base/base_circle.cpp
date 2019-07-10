//#define TRACE
#include "base/base_circle.h"

using namespace p_surf;

namespace p_base
{
	#define MAX_EPOLL_FDS				4096
	#define TRY_LOCK_DELAY				200
	#define CIRCLE_DEBUG_TERM			0
	#define CIRCLE_DEBUG_ERROR			1

	CBase_circle::CBase_circle():mp_epoll(NULL), mp_timer(NULL), mp_header(NULL)//,mp_dulist(NULL)
	{
		m_sigstop  = false;
		m_valid = false;
		max_sub_conns = 4096;
	}
	
	CBase_circle::~CBase_circle()
	{
	/*
		if (mp_dulist)
			delete mp_dulist;
	*/
	
		if (mp_epoll)
			delete mp_epoll;

		if (mp_timer)
			delete mp_timer;
	}

	/*******************************************************************
	 * @brief: 函数功能说明
	 * @function_name: circle_init
	 * @description: CBase_circle类相关变量初始化
	 * @param_in: ip_svr，CBase_server类型指针，circle绑定的server地址，
	 *				对于主circle需要以server地址初始化，子circle可以为NULL
	 * @param_out: 无
	 * @returns: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(-1)
	 * @warning: 无
	********************************************************************/
	s32 CBase_circle::circle_init(CBase_server *ip_svr)
	{
		
		mp_svr = ip_svr;
		pure_lock_init(&listenfd_inepoll_lock);
	
		mp_epoll = new CPure_epoll();
		if (NULL == mp_epoll)
		{
			//debugX(CIRCLE_DEBUG_ERROR, "[%s, %d] new CPure_epoll object instance error!\n", __FILE__, __LINE__);
			return FAILURE;
		}
		
		if (FAILURE == mp_epoll->pure_epoll_init(this))
		{
			//debugX(CIRCLE_DEBUG_ERROR, "[%s, %d] function pure_epoll_init() called error!\n", __FILE__, __LINE__);
			return FAILURE;
		}
		
		if (FAILURE == mp_epoll->pure_epoll_create(MAX_EPOLL_FDS))
		{
			//debugX(CIRCLE_DEBUG_ERROR, "[%s, %d] function pure_epoll_create() called error!\n", __FILE__, __LINE__);
			return FAILURE;
		}

/*
		mp_dulist = new CPure_dulist();
		if (NULL == mp_dulist)
		{
			debugX(CIRCLE_DEBUG_ERROR, "[%s, %d] new CPure_dulist object instance error!\n", __FILE__, __LINE__);
			return FAILURE;
		}
*/

		mp_timer = new CPure_timer();
		if (NULL == mp_timer)
		{
			//debugX(CIRCLE_DEBUG_ERROR, "[%s, %d] new CPure_timer object instance error!\n", __FILE__, __LINE__);
			return FAILURE;
		}
		
		return SUCCESS;
	}

	/*******************************************************************
	 * @brief: 函数功能说明
	 * @function_name: connect_delete
	 * @description: CBase_circle类删除一个pure_connect节点。
	 * @param_in: con，CPure_connect类型指针，需要删除的connect节点地址
	 * @param_out: 无
	 * @returns: 无
	 * @warning: 无
	********************************************************************/
	/*
	void CBase_circle::connect_delete(CBase_connect *con)
	{
		if (NULL == mp_dulist->remove_element(con))
			debugX(CIRCLE_DEBUG_ERROR, "[%s, %d] function remove_element() called error!\n", __FILE__, __LINE__);
	}
*/

	/*******************************************************************
	 * @brief: 函数功能说明
	 * @function_name: connect_destroy
	 * @description: CBase_circle类销毁所有的pure_connect节点。
	 * @param_in: 无
	 * @param_out: 无
	 * @returns: 无
	 * @warning: 无
	********************************************************************/
	/*
	void CBase_circle::connect_destroy(void)
	{
		mp_dulist->destroy_list();
	}
	*/
	
	/*******************************************************************
	 * @brief: 函数功能说明
	 * @function_name: circle_register
	 * @description: 将socket地址注册到circle的epoll中
	 * @param_in: ip_socket，CBase_socket类型指针，需要注册的socket地址
	 * @param_out: 无
	 * @returns: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(-1)
	 * @warning: 无
	********************************************************************/
	s32 CBase_circle::circle_register(CBase_socket *ip_socket)
	{
		return mp_epoll->pure_epoll_sockfd_regesiter(ip_socket);
	}

	s32 CBase_circle::circle_logout(CBase_socket *ip_socket)
	{
		return mp_epoll->pure_epoll_sockfd_logout(ip_socket);
	}

	/*******************************************************************
	 * @brief: 函数功能说明
	 * @function_name: circle_add_timer_event
	 * @description: circle添加定时事件
	 * @param_in: i_timer，ul64类型，定时器的倒计时值
	 * @param_in: ip_timer_event，CPure_event类型指针，定时器事件的地址
	 * @param_out: 无
	 * @returns: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(-1)
	 * @warning: 无
	********************************************************************/
	
	s32 CBase_circle::circle_add_timer_event(ul64 i_timer, CBase_timer_event *ip_timer_event)
	{
		return mp_timer->pure_add_timer(i_timer, ip_timer_event);;
	}

	s32 CBase_circle::circle_delete_timer_event(CBase_timer_event *ip_timer_event)
	{
		return mp_timer->pure_delete_timer(ip_timer_event);
	}

	/*******************************************************************
	 * @brief: 函数功能说明
	 * @function_name: add_event
	 * @description: 向circle的事件队列中添加一个事件节点
	 * @param_in: event，CPure_event类型指针，pure_event事件节点地址
	 * @param_out: 无
	 * @returns: 无
	 * @warning: 无
	********************************************************************/
	void CBase_circle::add_event(CIO_event *event)
	{
		//ASSERT(event != NULL);
		PDEBUG("event = %x prev = %x ,next = %x ,mp_header=%x,m_next=%x,linked = %d",event,event->mp_prev,event->mp_next,mp_header,m_next,event->m_linked);
		if (!mp_header)
		{
			mp_header = event;
			event->m_linked = true;
			return;
		}
		
		event->mp_next = mp_header;
		mp_header->mp_prev = event;
		mp_header = event;
		event->m_linked = true;
		PDEBUG("event = %x prev = %x ,next = %x ,mp_header=%x,m_next=%x,linked = %d",event,event->mp_prev,event->mp_next,mp_header,m_next,event->m_linked);
		
	}


	/*******************************************************************
	 * @brief: 函数功能说明
	 * @function_name: remove_event
	 * @description: 从circle的事件队列中删除一个事件节点
	 * @param_in: event，CPure_event类型指针，pure_event事件节点地址
	 * @param_out: 无
	 * @returns: 无
	 * @warning: 无
	********************************************************************/
	void CBase_circle::remove_event(CIO_event *event)
	{
		//ASSERT(event != NULL);
		PDEBUG("event = %x prev = %x ,next = %x ,mp_header=%x,m_next=%x",event,event->mp_prev,event->mp_next,mp_header,m_next);
		if(m_next)
		{
			if(m_next == event)
			{
				//PDEBUG("m_next = %x",m_next);
				m_next= event->mp_next;
				PDEBUG("event = %x,mp_header=%x,m_next=%x",event,mp_header,m_next);
			}

		}
		if (event->mp_prev)
		{
			event->mp_prev->mp_next = event->mp_next;
		}
		else
		{
			mp_header = event->mp_next;
		}

		if (event->mp_next)
			event->mp_next->mp_prev = event->mp_prev;

		event->mp_prev = event->mp_next = NULL;
		event->m_linked = false;
	}

	/*******************************************************************
	 * @brief: 函数功能说明
	 * @function_name: traversal
	 * @description: 遍历I/O事件链表，完成I/O读写操作
	 * @param_in: 无
	 * @param_out: 无
	 * @returns: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(-1)
	 * @warning: 无
	********************************************************************/
	s32 CBase_circle::traversal()
	{
		PDEBUG("in traversal\n");
		CIO_event *node = NULL;
		CIO_event *head = mp_header;
		m_next = mp_header;
		
		s32 ret = FAILURE;

		if (!mp_header)
			return SUCCESS;

		for(node = mp_header,m_next = node->mp_next;node != NULL;)
		{
			//head = head->mp_next;
			assert(node != m_next);
			PDEBUG("i node =%x m_next = %x linked = %d",node,m_next,node->m_linked);
			ret = node->io_process();
			if (FAILURE == ret)
			{
				//TODO: delete current event node; delete connection node.
				
				node->mp_connect->connect_close();
				PDEBUG("f node =%x m_next = %x linked = %d",node,m_next,node->m_linked);
				
			}
			else if (RETRY == ret)
			{
				//TODO: keep current node
				//printf("retry\n");
				//continue;
				PDEBUG("r node =%x m_next = %x linked = %d",node,m_next,node->m_linked);
			}
			else
			{
				//TODO: delete event node, keep connection node
			
				remove_event(node);
				//node->m_linked = false;
				PDEBUG("s node =%x m_next = %x linked = %d",node,m_next,node->m_linked);
			}
			
			node = m_next;
			if(m_next == NULL)
				break;
			m_next = node->mp_next;
			
			PDEBUG("in for node =%x m_next = %x",node,m_next);
		}
		
		return SUCCESS;
	}


	s32 CBase_circle::process()
	{

		while(!m_sigstop)
		{
			ul64 now = mp_timer->pure_get_msec();
			if(!lock_held && now>next_trylock_time)
			{
				if(pure_lock_trylock(&listenfd_inepoll_lock) == 0)
				{
					/*EPOLL ADD Listen FD */
					circle_register(mp_svr);
					lock_held = 1;
					epoll_in_sign = true;
					conns_to_unlock = ((max_sub_conns - conns)/4 >= 200)?200:(max_sub_conns - conns)/4;
				}
				else
				{
					if(epoll_in_sign == true)
					{
						/*EPOLL DEL Listen FD*/
						circle_logout(mp_svr);
						epoll_in_sign = false;
						next_trylock_time = now + TRY_LOCK_DELAY;
					}
				}
			}
			
			s32 time_find = mp_timer->pure_find_timer();
			
			if(time_find == 0)
			{
				time_find = 20;
			}
			mp_epoll->pure_epoll_process(time_find);
			mp_timer->pure_timer_process();

			if(conns_to_unlock <=0)
			{
				pure_lock_unlock(&listenfd_inepoll_lock);
				lock_held = 0;
			}
			
		}

	}

};
