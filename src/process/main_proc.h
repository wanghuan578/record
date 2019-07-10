#ifndef _MAIN_PROC_H_
#define _MAIN_PROC_H_

#include "proc_mod.h"
#include <list>
#include "log/pure_log.h"
#include "signal/p_signals.h"

using namespace p_base;
namespace p_surf
{

	class main_proc
	{	
	public:
	
		main_proc();  
		virtual ~main_proc();
		
		bool reload;
		bool stop;

		process * work_proc;
	private:

	public:

		//��ʼ��
		s32 init_proc();

		//��cpu
		s32 bind_cpu(s32 cpuid,pid_t pid);

		//�����ź�
		static s32 send_signal(const s8 * name, s32 signal_t,s32 pid);


		//���ɸ����ӽ���
		s32 spawn_one_process(const s8 * name);
		
		s32 run_work(const s8 * name);
		

		//�ź�ע��
		s32 register_signal();
		

		//������ѭ������������
		s32 run();
	};

};

#endif
