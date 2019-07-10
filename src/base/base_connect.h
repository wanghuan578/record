#ifndef _BASE_CONNECT_H_
#define _BASE_CONNECT_H_

#include "base/base_socket.h"

#include "epoll/read_event.h"
#include "epoll/write_event.h"
#include "base/base_connect_timer.h"
#include "timer/pure_timer_event.h"
#include "base/base_conn_pool.h"


#define CACHE_BUF_SIZE 8192 
#define MAX_TIMER_SLOT 8

namespace p_base
{
	
	
	typedef struct connect_buf_s 		CON_BUF_T;
	typedef s32 (*PDF)(void *,void *);
	
	struct connect_buf_s
	{
		s8 		*buf; 		//�����洢���ݵĻ�����
		u32 		last;		//�������ݵ���βָ��
		u32		end;		//buf����λ��
		u32		buf_len;	//��ǰbuf���е����ݳ���
	};

	class CBase_circle;
	class CPure_timer_event;
	class CBase_connect:public CBase_socket
	{
	public:	
		
		
		CRead_event	m_rdev;			//IO read event variable, inner contain double linked list
		CWrite_event 	m_wrev;			//IO write event variable, the same as readev

		CON_BUF_T		m_rdcache;			//���ڶ���buf���������ݻ���
		CON_BUF_T		m_wrcache;			//����д��buf���������ݻ���
		
		bool 			m_closed;		//close flag
		bool				m_registerd;	//in register
		
		bool 			m_rdable;		//read flag
		bool 			m_wrable;		//write flag
		bool 			m_exeable;		//execute flag

		CBase_connect();
		
		virtual ~CBase_connect();

		//virtual s32 connect_init(CPure_dulist *ip_dulist);

		/* connection��ʼ���Ĵ����� */
		virtual s32 connect_handle_init(CBase_circle * ) = 0;	
		
		/* connection������¼������� */
		virtual s32 connect_handle_read() = 0;	
		
		/* connection����д�¼������� */
		virtual s32 connect_handle_write() = 0;	
		
		/* connection�������ӳ������� */
		virtual s32 connect_handle_error() = 0;	
		
		/* connect����Ķ����ݲ��� */
		virtual s32 connect_read(s8* ip_buf, u32 i_buf_len, u32 i_flag);	
		
		/* connect�����д���ݲ��� */
		virtual s32 connect_write(s8* ip_buf, u32 i_buf_len, u32 i_flag);	
		
		/* connect������sock_fd�����ļ����� */
		virtual s32 connect_sendfile(s32 i_fd, off_t *ip_offset, size_t i_size, u32 i_flag);

		/*connect �ر�*/
		virtual s32 connect_close();
		
		/*���ü������ӡ�����*/
		s32 add_ref();
		s32 release();
		
		/*set delete callback*/
		void set_func(PDF i_f,void * i_d);

		/*check timeout*/
		s32 check_timeout();
		s32 update_active();
		s32 init_timer();		//������ʱ���

		CBase_circle * m_pc;
		CBase_conn_pool * m_pcp;
	private:
		
		/*remove event from chain*/
		s32 remove_event();
		/*close socket fd and remove fd from epoll*/
		s32 close_epoll_sock();
		/*remove all timer event*/
		s32 remove_timer_event();
		
		CBase_connect_timer m_cto;	//��ⳬʱ�¼���
	
		ul64 m_active_ms;				//�����Ծʱ��
		
		
		volatile u32 m_refc;	//���ü���
		PDF m_pdelete;	//ɾ���ص�����ָ��
		void * m_delete_data;		//ɾ���ص���������	
	protected:
		CPure_timer_event * m_ptarray[MAX_TIMER_SLOT];
				
	};
}
#endif /* _BASE_CONNECT_H_ */
