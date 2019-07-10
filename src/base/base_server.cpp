//#define TRACE
#include "base/base_server.h"
#include "atomic/atom_opr.h"
#include "sys_headers.h"

using namespace p_base;

	static s32 pure_socket_setnonblock(s32 sock)
	{
		s32 opts;
		opts=fcntl(sock,F_GETFL);
	
		if(opts<0)
		{
//			LOG("fcntl(sock.GETFL) %s\n)",strerror(errno));
			return -1;
		}
	
		opts = opts|O_NONBLOCK;
	
		if(fcntl(sock,F_SETFL,opts)<0)
		{
//			LOG("fcntl(sock.SETFL) %s\n",strerror(errno));
			return -1;
		}
		return 0;
	}


	CBase_server::CBase_server()
	{
		mp_maincircle = NULL;
		mp_subcircles = NULL;
		m_type = ST_SERVER;
		m_ismulti = false;
		m_subthreads = m_curthreads = 0;
	}

	CBase_server::~CBase_server()
	{
		if (mp_maincircle)
			delete mp_maincircle;

		if (mp_subcircles)
			delete [] mp_subcircles;
	}

	/*******************************************************************
	 * @brief: ��������˵��
	 * @function_name: add_circle
	 * @description: server���һ�������̵߳�ַ��
	 * @param_in: ip_circle, CBase_circle����ָ�룬�����̵߳ĵ�ַ��
	 * @param_out: ��
	 * @returns: s32���ͣ��ɹ�����SUCCESS(0)��ʧ�ܷ���FAILURE(-1)
	 * @warning: ��
	********************************************************************/
	s32 CBase_server::add_circle(CBase_circle* ip_circle)
	{
		s32 cur_threads = 0; 
		
		if (NULL == ip_circle)
		{
			PDEBUG("[%s, %d] argument error, point is NULL!\n", __FILE__, __LINE__);
			return FAILURE;
		}

		if (!mp_subcircles)
		{	
			PDEBUG("[%s, %d] there is only a main circle, no sub-circles is established!\n", __FILE__, __LINE__);
			return FAILURE;
		}
		
		do
		{
			cur_threads = m_curthreads;
			if (cur_threads >= m_subthreads)
			{
				PDEBUG("[%s, %d] number of current thread is overflow!\n", __FILE__, __LINE__);
				return FAILURE;
			}
			
		} while (!CAS(&m_curthreads, m_curthreads, cur_threads + 1));

		mp_subcircles[cur_threads] = ip_circle;
		
		return SUCCESS;
	}


	/*******************************************************************
	 * @brief: ��������˵��
	 * @function_name: server_sockfd_create
	 * @description: server����socket������
	 * @param_in: ��
	 * @param_out: ��
	 * @returns: s32���ͣ��ɹ�����SUCCESS(0)��ʧ�ܷ���FAILURE(-1)
	 * @warning: ��
	********************************************************************/
	s32 CBase_server::server_sockfd_create(void)
	{
		m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (m_sockfd < 0)
		{
			PDEBUG("[%s, %d] create socket fd error, error number = %d\n" , __FILE__, __LINE__, errno);
			return FAILURE;
		}

		if (FAILURE == pure_socket_setnonblock(m_sockfd))
		{
			PDEBUG("[%s, %d] functionset_nonBlocking() called error!\n", __FILE__, __LINE__);
			return FAILURE;
		}
		
		return SUCCESS;
	}

	/*******************************************************************
	 * @brief: ��������˵��
	 * @function_name: server_sockfd_bind
	 * @description: server��socket�������󶨵�ָ���˿ں�
	 * @param_in: i_port��s32���ͣ�server��Ҫ�󶨵Ķ˿ں�
	 * @param_out: ��
	 * @returns: s32���ͣ��ɹ�����SUCCESS(0)��ʧ�ܷ���FAILURE(-1)
	 * @warning: ��
	********************************************************************/
	s32 CBase_server::server_sockfd_bind(s32 i_port)
	{
		u32 sock_opt = 0;
		struct sockaddr_in addr;
		
		/* ����socketѡ�������ö˿ڸ��� */
		sock_opt = SO_REUSEADDR;
		if (0 != setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &sock_opt, sizeof(sock_opt)))
		{
			perror("set socket option error!");
			return FAILURE;
		}
		
		memset(&addr, 0, sizeof(struct sockaddr_in));
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_family = AF_INET;
		addr.sin_port = htons(i_port);
		if (bind(m_sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		{
			perror("socket bind error!");
			return FAILURE;
		}
		
		return SUCCESS;
	}

	/*******************************************************************
	 * @brief: ��������˵��
	 * @function_name: server_sockfd_listen
	 * @description: server�����˿�
	 * @param_in: listen_num��s32���ͣ����ɼ����ĸ���
	 * @param_out: ��
	 * @returns: s32���ͣ��ɹ�����SUCCESS(0)��ʧ�ܷ���FAILURE(-1)
	 * @warning: ��
	********************************************************************/
	s32 CBase_server::server_sockfd_listen(s32 listen_num)
	{
		if (listen(m_sockfd, listen_num) < 0)
		{
			perror("socket listen error!");
			return FAILURE;
		}

		return SUCCESS;
	}

	s32 CBase_server::server_init(u32 i_subthreads , int port)
	{
			
		if(server_sockfd_create()!= SUCCESS)
		{
			return FAILURE;
		}

		if(server_sockfd_bind(port)!=SUCCESS)
		{
			return FAILURE;
		}
		
		if(server_sockfd_listen(256)!=SUCCESS)
		{
			return FAILURE;
		}
		
	}