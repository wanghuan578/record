/**************************************************************
* Copyright (c) 2012,surfing
* All rights reserved.
* Filename��base_server.h
* Description��epollģ��server��ͷ�ļ�
***************************************************************/
/**************************************************************
* Version��1.0.0
* Original_Author��chengzhip
* Date��2012-12-25
* Modified_Author��
* Modified_Description��
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
		
		CBase_circle		**mp_subcircles;	//��circle����ָ������
		bool				m_ismulti;			//���̱߳�־λ
		s32					m_subthreads;		//�ɴ��������̸߳���
		s32					m_curthreads;		//��ǰ�Ѵ��������̸߳���
		
	public:

		
		CBase_circle		*mp_maincircle;		//��circle����ָ��
		
		CBase_server();

		virtual ~CBase_server();

		s32 get_subthreads_num() { return m_curthreads; }

		inline CBase_circle* get_maincircle() { return mp_maincircle; }

		inline CBase_circle** get_subcircles() { return mp_subcircles; }

		inline void set_multi(bool i_ismulti) { m_ismulti = i_ismulti; }
		
		inline bool get_multi(void) { return m_ismulti; }
		
		/*******************************************************************
		 * @brief: ��������˵��
		 * @function_name: server_init
		 * @description: server����ر�����ʼ��
		 * @param_in: i_subthreads, s32���ͣ������̵߳ĸ�����
		 *				0: ��ʾֻ���������̣߳������������߳�
		 *				else: ��ʾ�����̸߳��� 
		 * @param_out: ��
		 * @returns: s32���ͣ��ɹ�����SUCCESS(0)��ʧ�ܷ���FAILURE(-1)
		 * @warning: ��
		********************************************************************/
		virtual s32 server_init(u32 i_subthreads , s32 port);
		
		
		/*******************************************************************
		 * @brief: ��������˵��
		 * @function_name: add_circle
		 * @description: server���һ�������̵߳�ַ��
		 * @param_in: ip_circle, CBase_circle����ָ�룬�����̵߳ĵ�ַ��
		 * @param_out: ��
		 * @returns: s32���ͣ��ɹ�����SUCCESS(0)��ʧ�ܷ���FAILURE(-1)
		 * @warning: ��
		 * @notify: �÷���ʹ��ԭ�Ӳ��������̰߳�ȫ�ġ�
		********************************************************************/
		virtual s32 add_circle(CBase_circle* ip_circle);

	
		/*******************************************************************
		 * @brief: ��������˵��
		 * @function_name: server_sockfd_create
		 * @description: server����socket������
		 * @param_in: ��
		 * @param_out: ��
		 * @returns: s32���ͣ��ɹ�����SUCCESS(0)��ʧ�ܷ���FAILURE(-1)
		 * @warning: ��
		********************************************************************/
		virtual s32 server_sockfd_create(void);


		/*******************************************************************
		 * @brief: ��������˵��
		 * @function_name: server_sockfd_bind
		 * @description: server��socket�������󶨵�ָ���˿ں�
		 * @param_in: i_port��s32���ͣ�server��Ҫ�󶨵Ķ˿ں�
		 * @param_out: ��
		 * @returns: s32���ͣ��ɹ�����SUCCESS(0)��ʧ�ܷ���FAILURE(-1)
		 * @warning: ��
		********************************************************************/
		virtual s32 server_sockfd_bind(s32 i_port);


		/*******************************************************************
		 * @brief: ��������˵��
		 * @function_name: server_sockfd_listen
		 * @description: server�����˿�
		 * @param_in: listen_num��s32���ͣ����ɼ����ĸ���
		 * @param_out: ��
		 * @returns: s32���ͣ��ɹ�����SUCCESS(0)��ʧ�ܷ���FAILURE(-1)
		 * @warning: ��
		********************************************************************/
		virtual s32 server_sockfd_listen(s32 listen_num);

		/*******************************************************************
		 * @brief: ��������˵��
		 * @function_name: server_process
		 * @description: server�������Ӵ���
		 * @param_in: ��
		 * @param_out: ��
		 * @returns: s32���ͣ��ɹ�����SUCCESS(0)��ʧ�ܷ���FAILURE(-1)
		 * @warning: ��
		********************************************************************/
		virtual s32 server_process() = 0;
	};
};

#endif /* _BASE_SERVER_H_ */
