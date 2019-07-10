/**************************************************************
* Copyright (c) 2012,surfing
* All rights reserved.
* Filename：base_server.h
* Description：epoll模型server类头文件
***************************************************************/
/**************************************************************
* Version：1.0.0
* Original_Author：chengzhip
* Date：2012-12-25
* Modified_Author：
* Modified_Description：
* Modified_Date: 
***************************************************************/

#ifndef _BASE_SERVER_H_
#define _BASE_SERVER_H_

#include "base/base_socket.h"
#include "base/base_circle.h"

namespace p_base
{	
	class CBase_circle;
	
	class CBase_server:public CBase_socket
	{
	protected:
		
		CBase_circle		**mp_subcircles;	//子circle对象指针数组
		bool				m_ismulti;			//多线程标志位
		s32					m_subthreads;		//可创建的子线程个数
		s32					m_curthreads;		//当前已创建的子线程个数
		
	public:

		
		CBase_circle		*mp_maincircle;		//主circle对象指针
		
		CBase_server();

		virtual ~CBase_server();

		s32 get_subthreads_num() { return m_curthreads; }

		inline CBase_circle* get_maincircle() { return mp_maincircle; }

		inline CBase_circle** get_subcircles() { return mp_subcircles; }

		inline void set_multi(bool i_ismulti) { m_ismulti = i_ismulti; }
		
		inline bool get_multi(void) { return m_ismulti; }
		
		/*******************************************************************
		 * @brief: 函数功能说明
		 * @function_name: server_init
		 * @description: server类相关变量初始化
		 * @param_in: i_subthreads, s32类型，工作线程的个数。
		 *				0: 表示只有主工作线程，无其他工作线程
		 *				else: 表示工作线程个数 
		 * @param_out: 无
		 * @returns: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(-1)
		 * @warning: 无
		********************************************************************/
		virtual s32 server_init(u32 i_subthreads , s32 port);
		
		
		/*******************************************************************
		 * @brief: 函数功能说明
		 * @function_name: add_circle
		 * @description: server添加一个工作线程地址。
		 * @param_in: ip_circle, CBase_circle类型指针，工作线程的地址。
		 * @param_out: 无
		 * @returns: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(-1)
		 * @warning: 无
		 * @notify: 该方法使用原子操作，是线程安全的。
		********************************************************************/
		virtual s32 add_circle(CBase_circle* ip_circle);

	
		/*******************************************************************
		 * @brief: 函数功能说明
		 * @function_name: server_sockfd_create
		 * @description: server创建socket描述符
		 * @param_in: 无
		 * @param_out: 无
		 * @returns: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(-1)
		 * @warning: 无
		********************************************************************/
		virtual s32 server_sockfd_create(void);


		/*******************************************************************
		 * @brief: 函数功能说明
		 * @function_name: server_sockfd_bind
		 * @description: server将socket描述符绑定到指定端口号
		 * @param_in: i_port，s32类型，server需要绑定的端口号
		 * @param_out: 无
		 * @returns: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(-1)
		 * @warning: 无
		********************************************************************/
		virtual s32 server_sockfd_bind(s32 i_port);


		/*******************************************************************
		 * @brief: 函数功能说明
		 * @function_name: server_sockfd_listen
		 * @description: server监听端口
		 * @param_in: listen_num，s32类型，最大可监听的个数
		 * @param_out: 无
		 * @returns: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(-1)
		 * @warning: 无
		********************************************************************/
		virtual s32 server_sockfd_listen(s32 listen_num);

		/*******************************************************************
		 * @brief: 函数功能说明
		 * @function_name: server_process
		 * @description: server网络连接处理
		 * @param_in: 无
		 * @param_out: 无
		 * @returns: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(-1)
		 * @warning: 无
		********************************************************************/
		virtual s32 server_process() = 0;
	};
};

#endif /* _BASE_SERVER_H_ */
