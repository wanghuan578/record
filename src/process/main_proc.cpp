#include "main_proc.h"
#include "string/p_string.h"
#include "usage.h"
#include "global.h"


using namespace p_base;
using namespace p_surf;

static void sig_chld(int signo)
{
	pid_t   pid;
	int     stat;
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
	{
		g_restart_sig = true;
		//g_restart_pid_a.push_back(pid);
	}
	return;
}


static void handle_quit(int sig,void * data)
{
	main_proc * p = (main_proc *)data;
	p->stop = true;

}

static void handle_reload(int sig,void * data)
{
	main_proc * p = (main_proc *)data;
	p->reload = true;
}

static void sig_handle(int sig)
{
	SR_S * ps ;
	for(ps = g_sig_reg_table;ps->signo != 0;ps++)
	{
		if(ps->signo == sig)
		{
			if(ps->pf)
			{
				return ps->pf(sig,ps->userdata);
			}
			return;
		}		
	}
}


main_proc::main_proc()
{
	reload = false;
	stop = false;
	work_proc = NULL;
}

main_proc::~main_proc()
{
	
}


/*private:
	process * sub_proc_array;
	CN_S * g_conf;
*/	
	//³õÊ¼»¯
s32 main_proc::init_proc()
{
	
	init_process_title(g_parg);

	g_psyslog->prt(LOG_INFO,"Main process init successful\n");
	return SUCCESS;
}

//static ·¢ËÍĞÅºÅ
s32 main_proc::send_signal(const s8 * name, s32 signal_t,s32 pid)
{

//	if(STRMATCH(name,"all"))
//	{
	
		if(signal_t == 2)
		{
	 		if (kill(pid, LIVE_RECONFIGURE_SIGNAL) != -1)
			{
	        		return 0;
	 		}
	 	}
		else if(signal_t == 1)
		{
			if (kill(pid, LIVE_SHUTDOWN_SIGNAL) != -1)
			{
	            return 0;
	     	}
		}
	
//	}
}


//ĞÅºÅ×¢²á
s32 main_proc::register_signal()
{
	SR_S * ps ;
	
	for(ps = g_sig_reg_table;ps->signo != 0;ps++)
	{
		if(ps->signo == LIVE_SHUTDOWN_SIGNAL)
		{
			ps->pf = handle_quit;
			ps->userdata = this;
		}
		
		if(ps->signo == LIVE_TERMINATE_SIGNAL)
		{
			ps->pf = handle_quit;
			ps->userdata = this;
		}
		
		if(ps->signo == LIVE_RECONFIGURE_SIGNAL)
		{
			ps->pf = handle_reload;
			ps->userdata = this;
		}
		
	}
	
	signal(LIVE_SHUTDOWN_SIGNAL,sig_handle);
	
	signal(LIVE_RECONFIGURE_SIGNAL,sig_handle);

	signal(SIGPIPE,SIG_IGN);
	
	g_psyslog->prt(LOG_INFO,"Signal register success!\n");
}


s32 main_proc::spawn_one_process(const s8 * name)
{
		if(work_proc != NULL)
		{
			delete work_proc;
			work_proc = NULL;
		}
		work_proc = process::create(name);
		if(work_proc != NULL)
		{
			work_proc->init_process(g_parg);
		}
		
		s32 socket_t[2];

		if (socketpair(AF_UNIX, SOCK_STREAM, 0, socket_t) == -1)
		{
		    g_psyslog->prt(LOG_CRIT,"socketpair() failed while spawning processes!");
		 	exit(0);
		}
	
		pid_t pid = fork();
		if(pid == -1)
		{
			g_psyslog->prt(LOG_CRIT,"fork error:%s!,15s restart\n",work_proc->local_conf->modn);
			close(socket_t[0]);
			close(socket_t[1]);
			exit(0);
		}

		if(pid == 0)
		{
		
			close(socket_t[0]);
			work_proc->channel = socket_t[1];
			work_proc->local_conf = g_parg->mod_tree;
			work_proc->init_conf(g_parg);
			g_psyslog->prt(LOG_INFO,"Process start\n");
			work_proc->run();

		}

		if(pid > 0)
		{
			close(socket_t[1]);
			work_proc->pid = pid;
			
			work_proc->channel = socket_t[0];
			
			g_psyslog->prt(LOG_INFO,"Create Work process:%s",name);
			
		}
		
}


s32 main_proc::run()
{
	
	set_process_titile(PROGRAM":master",g_parg);
	
	register_signal();

	signal(SIGCHLD,sig_chld);
	
	while(!stop)
	{
		if(g_restart_sig)
		{
			g_psyslog->prt(LOG_INFO,"restart "PROGRAM);
			spawn_one_process(PROGRAM);
			g_restart_sig = false;
		}
	
		
		if(g_reload_sig)
		{
			g_psyslog->prt(LOG_INFO,"reload configuration");
			kill(work_proc->pid,LIVE_RECONFIGURE_SIGNAL);
			g_reload_sig = false;
		}

		sleep(10);
	}
	

	kill(work_proc->pid, LIVE_TERMINATE_SIGNAL);
	
	g_psyslog->prt(LOG_INFO,"Program master quit");
	
}

s32 main_proc::run_work(const s8 * name)
{
	signal(SIGPIPE,SIG_IGN);
		
	if(work_proc != NULL)
	{
		delete work_proc;
		work_proc = NULL;
	}

	work_proc = process::create(name);
	if(work_proc != NULL)
	{
		work_proc->init_process(g_parg);
	}
	
	work_proc->local_conf = g_parg->mod_tree;
	work_proc->init_conf(g_parg);
	g_psyslog->prt(LOG_INFO,"Process start");
	work_proc->run();
}

