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
		
		//CPure_dulist	*mp_dulist;		//˫���������connection�ڵ�
		CPure_epoll	*mp_epoll;		//epollָ��
		CPure_timer	*mp_timer;		//��ʱ��ָ��
		CIO_event		*mp_header;		//pure event�¼����������׵�ַ
		CIO_event		*m_next;			//pure event�¼�������һ�����ʵ�ַ
		volatile bool m_sigstop;
	public:
		
		CBase_server	*mp_svr;		/* ��һ��server��ַ�����ڶ��߳���˵��һ��server���ܻ���һ������circle�Ͷ������
										   �̵߳���circleָ�룬���﷽����ݵ�serverָ������������� */
		
		CBase_circle();
		
		virtual ~CBase_circle();

		//inline CPure_dulist* get_dulist() { return mp_dulist; }

		/*******************************************************************
		 * @brief: ��������˵��
		 * @function_name: circle_init
		 * @description: CBase_circle����ر�����ʼ��
		 * @param_in: ip_svr��CBase_server����ָ�룬circle�󶨵�server��ַ��
		 *				������circle��Ҫ��server��ַ��ʼ������circle����ΪNULL
		 * @param_out: ��
		 * @returns: s32���ͣ��ɹ�����SUCCESS(0)��ʧ�ܷ���FAILURE(-1)
		 * @warning: ��
		********************************************************************/
		virtual s32 circle_init(CBase_server *ip_svr);
		

		/*******************************************************************
		 * @brief: ��������˵��
		 * @function_name: connect_create
		 * @description: CBase_circle�ഴ��һ��pure_connect�ڵ㡣
		 * @param_in: connfd��s32���ͣ��ͻ��˵�socket������
		 * @param_out: ��
		 * @returns: s32���ͣ��ɹ�����SUCCESS(0)��ʧ�ܷ���FAILURE(-1)
		 * @warning: ��
		********************************************************************/
		virtual CBase_connect* accept_connect_create(s32 connfd) = 0;


		/*******************************************************************
		 * @brief: ��������˵��
		 * @function_name: connect_delete
		 * @description: CBase_circle��ɾ��һ��pure_connect�ڵ㡣
		 * @param_in: con��CPure_connect����ָ�룬��Ҫɾ����connect�ڵ��ַ
		 * @param_out: ��
		 * @returns: ��
		 * @warning: ��
		********************************************************************/
		virtual void connect_remove(CBase_connect *con) = 0;


		

		/*******************************************************************
		 * @brief: ��������˵��
		 * @function_name: circle_register
		 * @description: ��socket��ַע�ᵽcircle��epoll��
		 * @param_in: ip_socket��CBase_socket����ָ�룬��Ҫע���socket��ַ
		 * @param_out: ��
		 * @returns: s32���ͣ��ɹ�����SUCCESS(0)��ʧ�ܷ���FAILURE(-1)
		 * @warning: ��
		********************************************************************/
		virtual s32 circle_register(CBase_socket *ip_socket);
		
		virtual s32 circle_logout(CBase_socket *ip_socket);
		
		/*******************************************************************
		 * @brief: ��������˵��
		 * @function_name: circle_process
		 * @description: circle�����¼��Ͷ�ʱ�¼�����
		 * @param_in: ��
		 * @param_out: ��
		 * @returns: s32���ͣ��ɹ�����SUCCESS(0)��ʧ�ܷ���FAILURE(-1)
		 * @warning: ��
		********************************************************************/
		//virtual s32 circle_process() = 0;
		

		/*******************************************************************
		 * @brief: ��������˵��
		 * @function_name: circle_add_timer_event
		 * @description: circle��Ӷ�ʱ�¼�
		 * @param_in: i_timer��ul64���ͣ���ʱ���ĵ���ʱֵ
		 * @param_in: ip_timer_event��CPure_event����ָ�룬��ʱ���¼��ĵ�ַ
		 * @param_out: ��
		 * @returns: s32���ͣ��ɹ�����SUCCESS(0)��ʧ�ܷ���FAILURE(-1)
		 * @warning: ��
		********************************************************************/
		virtual s32 circle_add_timer_event(ul64 i_timer, CBase_timer_event *ip_timer_event);

		virtual s32 circle_delete_timer_event(CBase_timer_event *ip_timer_event);
		
		/*******************************************************************
		 * @brief: ��������˵��
		 * @function_name: add_event
		 * @description: ��circle���¼����������һ���¼��ڵ�
		 * @param_in: event��CPure_event����ָ�룬pure_event�¼��ڵ��ַ
		 * @param_out: ��
		 * @returns: ��
		 * @warning: ��
		********************************************************************/
		virtual void add_event(CIO_event *event);


		/*******************************************************************
		 * @brief: ��������˵��
		 * @function_name: remove_event
		 * @description: ��circle���¼�������ɾ��һ���¼��ڵ�
		 * @param_in: event��CPure_event����ָ�룬pure_event�¼��ڵ��ַ
		 * @param_out: ��
		 * @returns: ��
		 * @warning: ��
		********************************************************************/
		virtual void remove_event(CIO_event *event);
		

		/*******************************************************************
		 * @brief: ��������˵��
		 * @function_name: traversal
		 * @description: ����I/O�¼��������I/O��д����
		 * @param_in: ��
		 * @param_out: ��
		 * @returns: s32���ͣ��ɹ�����SUCCESS(0)��ʧ�ܷ���FAILURE(-1)
		 * @warning: ��
		********************************************************************/
		virtual s32 traversal();

		virtual s32 process();

		/*��ɫ ��Ч��Ч ��ͨ��*/
		volatile bool m_valid;

		PLOCK_S listenfd_inepoll_lock; //��֤epoll��listenfd������2��

		bool lock_held;
		bool epoll_in_sign;
		
		s32 conns_to_unlock;
		
		s32 conns;
		s32 max_sub_conns;
		
		ul64 next_trylock_time;
	};	
};

#endif /* _BASE_CIRCLE_H_ */
