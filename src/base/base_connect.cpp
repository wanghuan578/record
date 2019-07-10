/*
 * base_connect.cpp
 *
 *  Created on: 2012-11-28
 *      Author: chengzhip
 */

#include "base/base_connect.h"
#include "base/base_connect_timer.h"
#include "base/base_circle.h"

#include "atomic/atom_opr.h"

#include <iostream>
using namespace std;

#define BC_TIMEOUT 30000

namespace p_base
{

	CBase_connect::CBase_connect():m_rdev(this),m_wrev(this),m_cto(this)
	{
		m_sockfd = -1; 	
		m_type = ST_UNDEFINE;		
		m_exeable = m_rdable = true;
		m_closed = m_wrable = false;

		m_pdelete = NULL;
		m_delete_data = NULL;
		
		m_refc = 1;
		memset(m_ptarray,0,sizeof(void *)*MAX_TIMER_SLOT);
		
		m_cto.set_timer_event(30000);
	}

	CBase_connect::~CBase_connect()
	{
	
	}
	/*
	s32 CBase_connect::connect_init()
	{
	
		return SUCCESS;
	}
	*/
#if 0
	s32 CBase_connect::connect_handle_init()
	{

		return SUCCESS;
	}

	/* connection对象读事件处理函数 */
	s32 CBase_connect::connect_handle_read()
	{

		return SUCCESS;
	}

	/* connection对象写事件处理函数 */
	s32 CBase_connect::connect_handle_write()
	{

		return SUCCESS;
	}

	/* connection对象连接出错处理函数 */
	s32 CBase_connect::connect_handle_error()
	{

		return SUCCESS;
	}
#endif

	s32 CBase_connect::connect_read(s8* ip_buf, u32 i_buf_len, u32 i_flag)
	{
		if (NULL == ip_buf)	
		{		
			PDEBUG("[%s, %d] argument error, point is NULL!\n", __FILE__, __LINE__);
			return FAILURE;	
		}	
		
		if (i_flag)	
		{		
			//TODO: flag unused...	
		}	

		return read(m_sockfd, ip_buf, i_buf_len);
	}

	s32 CBase_connect::connect_write(s8* ip_buf, u32 i_buf_len, u32 i_flag)
	{
		if (NULL == ip_buf)	
		{		
			PDEBUG("[%s, %d] argument error, point is NULL!\n", __FILE__, __LINE__);		
			return FAILURE;	
		}	

		if (i_flag)	
		{		
			//TODO: flag unused...	
		}	

		return write(m_sockfd, ip_buf, i_buf_len);
	}


	
	s32 CBase_connect::connect_sendfile(s32 i_fd, off_t *ip_offset, size_t i_size, u32 i_flag)
	{
		if (i_flag)	
		{		
			//TODO: flag unused...	
		}	

		return sendfile(m_sockfd, i_fd, ip_offset, i_size);
	}

#if 0
	s32 CBase_connect::create_timer_event(CPure_event* pe)
	{
		CPure_event * td = timer_head;		

		if (NULL == pe)	
		{		
			printf("[%s, %d] pure event create error!\n", __FILE__, __LINE__);
			return FAILURE;
		}	
		else	
		{		
			pe->con = this;
			if(NULL == td)		
			{			
				timer_head = pe;		
			}		
			else		
			{			
				for(; td->next; td=td->next)			
				{				
					//cout<<"->";			
				}						
				td->next = pe;			
				pe->prev = td;		
			}		

			timer_event_num++;		
			//cout<<timer_event_num<<" timer event(s)"<<endl;	
		}
		
		return SUCCESS;	
	}

	s32 CBase_connect::get_timer_event_num()
	{
		CPure_event* td = NULL;	
		s32 count = 0;	

		if (timer_head)
		{
			td = timer_head->next;
			for(; td; td=td->next)		
			{			
				count++;		
			}	
		}
		
		return count;
	}


	s32 CBase_connect::remove(CIO_event* ip_event)
	{
		if (NULL == eve)
		{
			printf("[%s, %d] argument error, point is NULL!\n", __FILE__, __LINE__);
			return FAILURE;
		}

		if(eve->prev)	
		{	
			eve->prev->next = eve->next;
		}
		
		if(eve->next)
		{
			eve->next->prev = eve->prev;
		}

		return SUCCESS;	
	}
#endif 

	s32 CBase_connect::add_ref()
	{

		return ATOMIC_ADD(&m_refc, 1);
	}

	s32 CBase_connect::release()
	{
		s32 ret = ATOMIC_SUB(&m_refc,1);
		if(ret == 0)
		{
			if(m_pdelete)
				m_pdelete(this,m_delete_data);
			else
				delete this;
			return ret;
		}
	}

	void CBase_connect::set_func(PDF i_f,void * i_d)
	{
		m_pdelete = i_f;
		m_delete_data = i_d;
	}

	s32 CBase_connect::check_timeout()
	{
		struct timeval tv;
		gettimeofday(&tv,NULL);
		ul64 now = tv.tv_sec*1000 + tv.tv_usec/1000;
		s32 diff = now - m_active_ms;
		if(diff > BC_TIMEOUT)
		{
			return FAILURE;
		}
		m_pc->circle_add_timer_event(BC_TIMEOUT,&m_cto);

		return SUCCESS;
	}

	s32 CBase_connect::update_active()
	{
		struct timeval tv;
		gettimeofday(&tv,NULL);
		m_active_ms = tv.tv_sec*1000 + tv.tv_usec/1000;
	}

	s32 CBase_connect::init_timer()
	{
		update_active();
		m_pc->circle_add_timer_event(BC_TIMEOUT,&m_cto);
	
	}

	s32 CBase_connect::connect_close()
	{
		PDEBUG("close \n");
		m_closed = true;
		remove_timer_event();
		remove_event();
		close_epoll_sock();
		m_pc->connect_remove(this);
		m_pc->conns--;
		return SUCCESS;
	}


	/*remove event from chain*/
	s32 CBase_connect::remove_event()
	{
		if(m_pc)
		{
			m_pc->remove_event(&m_rdev);
			m_pc->remove_event(&m_wrev);
		}
	}

	/*close socket fd and remove fd from epoll*/
	s32 CBase_connect::close_epoll_sock()
	{
		if(m_pc)
		{
			m_pc->circle_logout(this);
		}
		close_sockfd();
		
	}

	/*remove all timer event*/
	s32 CBase_connect::remove_timer_event()
	{
		if(m_pc)
		{
			m_pc->circle_delete_timer_event(&m_cto);
		}

		for(s32 i = 0;i<MAX_TIMER_SLOT;i++)
		{
			if(m_ptarray[i] != NULL)
			{
				delete m_ptarray[i];
				m_ptarray[i] = NULL;
			}
		}
	}
};

