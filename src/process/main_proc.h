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

		//初始化
		s32 init_proc();

		//绑定cpu
		s32 bind_cpu(s32 cpuid,pid_t pid);

		//发送信号
		static s32 send_signal(const s8 * name, s32 signal_t,s32 pid);


		//生成各个子进程
		s32 spawn_one_process(const s8 * name);
		
		s32 run_work(const s8 * name);
		

		//信号注册
		s32 register_signal();
		

		//主进程循环，重启进程
		s32 run();
	};

};

#endif
