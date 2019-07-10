/**************************************************************
* Copyright (c) 2012,surfing
* All rights reserved.
* Filename：base_server.h
* Description：socket基础类头文件
***************************************************************/
/**************************************************************
* Version：1.0.0
* Original_Author：chengzhip
* Date：2012-11-23
* Modified_Author：
* Modified_Description：
* Modified_Date: 
***************************************************************/

#ifndef _BASE_SOCKET_H_
#define _BASE_SOCKET_H_

#include "basic_type.h"
#include "sys_headers.h"

#include "global.h"

namespace p_base
{
	class CBase_socket
	{
	protected:
		s32			m_sockfd;		//socket描述符
		SOCK_TYPE_E	m_type;			//socket类型，0表示undefined, 1表示server, 2表示connection

	public:
		
		CBase_socket():m_sockfd(INVALID_SOCKET), m_type(ST_UNDEFINE) { }

		virtual inline void set_sockfd(s32 i_sockfd) { m_sockfd = i_sockfd; }

		virtual inline s32 get_sockfd() { return m_sockfd; }

		virtual inline void set_socktype(SOCK_TYPE_E i_type) { m_type = i_type; }

		virtual inline s32 get_socktype() { return m_type; }

		
		virtual inline void close_sockfd() { close(m_sockfd);m_sockfd = NULL; }
		
		virtual ~CBase_socket() {}
	};
};

#endif /* _BASE_SOCKET_H_ */
