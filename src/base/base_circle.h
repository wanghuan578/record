#ifndef _BASE_CIRCLE_H_
#define _BASE_CIRCLE_H_

//#include "epoll/pure_dulist.h"
#include "epoll/pure_epoll.h"
#include "timer/pure_timer.h"
#include "base/base_connect.h"
#include "lock/pure_lock.h"

namespace p_base
{
	#define TIMER_MIN_INTERVAL		30

	class CBase_server;
	class CPure_epoll;
	
	class CBase_circle
	{
	protected:
		
		//CPure_dulist	*mp_dulist;		//双向链表管理connection节点
		CPure_epoll	*mp_epoll;		//epoll指针
		CPure_timer	*mp_timer;		//定时器指针
		CIO_event		*mp_header;		//pure event事件队列链表首地址
		CIO_event		*m_next;			//pure event事件队列下一个访问地址
		volatile bool m_sigstop;
	public:
		
		CBase_server	*mp_svr;		/* 上一级server地址，对于多线程来说，一个server可能会有一个主的circle和多个其他
										   线程的子circle指针，这里方便回溯到server指针操作其他变量 */
		
		CBase_circle();
		
		virtual ~CBase_circle();

		//inline CPure_dulist* get_dulist() { return mp_dulist; }

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
		virtual s32 circle_init(CBase_server *ip_svr);
		

		/*******************************************************************
		 * @brief: 函数功能说明
		 * @function_name: connect_create
		 * @description: CBase_circle类创建一个pure_connect节点。
		 * @param_in: connfd，s32类型，客户端的socket描述符
		 * @param_out: 无
		 * @returns: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(-1)
		 * @warning: 无
		********************************************************************/
		virtual CBase_connect* accept_connect_create(s32 connfd) = 0;


		/*******************************************************************
		 * @brief: 函数功能说明
		 * @function_name: connect_delete
		 * @description: CBase_circle类删除一个pure_connect节点。
		 * @param_in: con，CPure_connect类型指针，需要删除的connect节点地址
		 * @param_out: 无
		 * @returns: 无
		 * @warning: 无
		********************************************************************/
		virtual void connect_remove(CBase_connect *con) = 0;


		

		/*******************************************************************
		 * @brief: 函数功能说明
		 * @function_name: circle_register
		 * @description: 将socket地址注册到circle的epoll中
		 * @param_in: ip_socket，CBase_socket类型指针，需要注册的socket地址
		 * @param_out: 无
		 * @returns: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(-1)
		 * @warning: 无
		********************************************************************/
		virtual s32 circle_register(CBase_socket *ip_socket);
		
		virtual s32 circle_logout(CBase_socket *ip_socket);
		
		/*******************************************************************
		 * @brief: 函数功能说明
		 * @function_name: circle_process
		 * @description: circle网络事件和定时事件处理
		 * @param_in: 无
		 * @param_out: 无
		 * @returns: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(-1)
		 * @warning: 无
		********************************************************************/
		//virtual s32 circle_process() = 0;
		

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
		virtual s32 circle_add_timer_event(ul64 i_timer, CBase_timer_event *ip_timer_event);

		virtual s32 circle_delete_timer_event(CBase_timer_event *ip_timer_event);
		
		/*******************************************************************
		 * @brief: 函数功能说明
		 * @function_name: add_event
		 * @description: 向circle的事件队列中添加一个事件节点
		 * @param_in: event，CPure_event类型指针，pure_event事件节点地址
		 * @param_out: 无
		 * @returns: 无
		 * @warning: 无
		********************************************************************/
		virtual void add_event(CIO_event *event);


		/*******************************************************************
		 * @brief: 函数功能说明
		 * @function_name: remove_event
		 * @description: 从circle的事件队列中删除一个事件节点
		 * @param_in: event，CPure_event类型指针，pure_event事件节点地址
		 * @param_out: 无
		 * @returns: 无
		 * @warning: 无
		********************************************************************/
		virtual void remove_event(CIO_event *event);
		

		/*******************************************************************
		 * @brief: 函数功能说明
		 * @function_name: traversal
		 * @description: 遍历I/O事件链表，完成I/O读写操作
		 * @param_in: 无
		 * @param_out: 无
		 * @returns: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(-1)
		 * @warning: 无
		********************************************************************/
		virtual s32 traversal();

		virtual s32 process();

		/*角色 有效无效 非通用*/
		volatile bool m_valid;

		PLOCK_S listenfd_inepoll_lock; //保证epoll中listenfd不多于2个

		bool lock_held;
		bool epoll_in_sign;
		
		s32 conns_to_unlock;
		
		s32 conns;
		s32 max_sub_conns;
		
		ul64 next_trylock_time;
	};	
};

#endif /* _BASE_CIRCLE_H_ */
