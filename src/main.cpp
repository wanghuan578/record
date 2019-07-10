
#include "sys_headers.h"
#include "usage.h"
#include "conf/pconf.h"
#include <vector>

#include "global_conf.h"
#include "string/p_string.h"
#include "process/main_proc.h"
#include "app/record_control.h"
#include "app/process_manager.h"
#include "app/mongo_adaptor.h"

using namespace p_base;

/*global time struct*/
namespace p_surf
{
	CLog* g_psyslog;	
	CLog* g_perrlog;	
	G_CONF_S * g_parg;

	bool g_restart_sig;
	bool g_reload_sig;
}

using namespace p_surf;

void write_pid_to_file(s8 * file)
{
	pid_t pid = getpid();
	s32 m_fd = -1;
	
	if((m_fd=open(file,O_RDWR|O_CREAT,0644))==-1) 
	{
		g_psyslog->prt(LOG_CRIT,"wrtie pid : Open %s Error:%s",file,strerror(errno));
	
		printf("wrtie pid : Open %s Error:%s\n",file,strerror(errno));
		exit(0);
	}

	s32 ret = lockf(m_fd,F_TLOCK,0);  
	
	if(ret<0)
	{
		g_psyslog->prt(LOG_CRIT,"lock file %s Error:%s\t"PROGRAM" has runned",file,strerror(errno));
	
		printf(PROGRAM" has runned\n");
		exit(0);
	}
	
	s8 buffer[64];
	
	s32 len = rn_snprintf(buffer,64,"%d\n",pid);
	
	if(write(m_fd,buffer,len) < 0)
	{
		g_psyslog->prt(LOG_CRIT,"wrtie pid : Write %s Error:%s\n",file,strerror(errno));
		printf("wrtie pid : Write %s Error:%s\n",file,strerror(errno));
		exit(0);
	}

	return;

}

s32 get_pid_from_file(s8 * file,pid_t * o_pid)
{
	s32 m_fd = -1;
	
	if((m_fd=open(file,O_RDONLY,S_IRUSR))==-1) 
	{
		printf("get pid : Open %s Error:%s\n",file,strerror(errno));
		return FAILURE;
	}
	s8 buffer[64] = {0};

	if(read(m_fd,buffer,64)<0)
	{
		printf("get pid : Read %s Error:%s\n",file,strerror(errno));
		return FAILURE;
	}

	*o_pid = atoi(buffer);
	
	return SUCCESS;

}


int main(int argc, char *const *argv)
{
	/*initialize signal variable*/
	g_restart_sig = false;
	g_reload_sig = false;
	
	/*analyze the argv*/
	G_ARG_S start_arg;

	memset(&start_arg,0,sizeof(argument));
	
	start_arg.debug_mode = false;

	if(get_argument(argc,argv,start_arg) == false)
	{
		printf("argument error\n");
		exit(0);
	}

	/*show help*/
	if(start_arg.show_help) 
	{
		printf("\n%s",g_usage);
		exit(0);
	}

	/*show version*/
	if(start_arg.show_version)
	{
		//printf("%s version: %s.%s\n",PROGRAM,VER,MAKEDATE);
		exit(0);
	}


	/*get current path*/
	g_parg = new G_CONF_S;

	assert(g_parg);

	memset(g_parg,0,sizeof(G_CONF_S));
	g_parg->argv = (s8 **)argv;
	g_parg->argc = argc;
	g_parg->os_argv = argv[0];
	get_cur_path(argv[0],g_parg->sbin_path,MAX_PATH_LENGTH);
	
	if(start_arg.test_config)
	{
		rn_snprintf(g_parg->conf_path,MAX_PATH_LENGTH,"%s/%s",g_parg->sbin_path,start_arg.test_conf_s);
		/*test configuration file*/
		pconf * g_pconf = new pconf();
		g_parg->mod_tree = pconf::new_node();
		if(g_pconf->readconf(g_parg->conf_path) < 0)
		{
			exit(0);
		}
		if(g_pconf->parse(g_parg->mod_tree) == 0)
		{
			printf("configuration success\n");
		}
		
		exit(0);
	}
	

	/*initialize environment*/
	const char * conf_p = NULL;
	if(start_arg.set_conf_s)
	{
		conf_p = start_arg.set_conf_s;
	}
	else
	{
		conf_p = DEFAULT_CONF_PATH;
	}

	rn_snprintf(g_parg->conf_path,MAX_PATH_LENGTH,"%s/%s",g_parg->sbin_path,conf_p);
	rn_snprintf(g_parg->log_path,MAX_PATH_LENGTH,"%s/%s",g_parg->sbin_path,DEFAULT_LOG_PATH);
	rn_snprintf(g_parg->pid_path,MAX_PATH_LENGTH,"%s/%s",g_parg->sbin_path,"../log/pid");
	
	pconf * ppconf = new pconf();
	g_parg->mod_tree = pconf::new_node();
	if(ppconf->readconf(g_parg->conf_path) < 0)
	{
		pconf::destroy(g_parg->mod_tree);
		delete ppconf;
		exit(0);
	}
	


	/*handle -s send signal to mods*/
	if(start_arg.send_signal)
	{
		pid_t pid;
		if(get_pid_from_file(g_parg->pid_path,&pid)!=SUCCESS)
		{
			printf("pidfile doesn't exist!\n Send signal error!\n");
			exit(0);
		}
		main_proc::send_signal(start_arg.send_mod_s,start_arg.send_sig_s,pid);
		pconf::destroy(g_parg->mod_tree);
		delete ppconf;
		exit(0);
	}

	if(ppconf->parse(g_parg->mod_tree) == 0)
	{
		printf("configuration success\n");
	}
	else
	{
		pconf::destroy(g_parg->mod_tree);
		delete ppconf;
		exit(0);
	}
	

	/*create CLOG*/
	g_psyslog = new CLog;
	assert(g_psyslog);
	g_psyslog->set_log_level(LOG_INFO);
	g_psyslog->set_log_out_by_time(0);	//one file per hour 
	g_psyslog->init(g_parg->log_path,"syslog");
	
	g_perrlog = new CLog;
	assert(g_perrlog);
	g_perrlog->set_log_level(LOG_ERROR);
	//g_psyslog->set_log_out_by_time(0);	//one file per hour 
	g_perrlog->init(g_parg->log_path,"error");
	
	CRecordCtl ctrl;
	//main_proc * pmp;
#if 1

	//string db = "rec_distributed";
	//string tb = "process_info";
  	//void *connect = MongoAdaptor::connect("mongodb://localhost:27017");
 	//void *collection = MongoAdaptor::collection(connect, db, tb);
	//void *collection, bson_t *query
	//bson_t *query = bson_new();
 	//BSON_APPEND_UTF8(query, "stream_id", "a3OILGKH_C0");
#if 0//update

	BSON_APPEND_UTF8(query, "stream_id", "B3OILGKH_C0");

	bson_t *update = bson_new();
 	bson_t *child = bson_new();

	bson_append_document_begin(update, "$set", -1, child);
	BSON_APPEND_INT64(child, "pid", 51200);
	bson_append_document_end(update, child);
#endif
    	//BSON_APPEND_UTF8(query, "stream_id", "B3OILGKH_C0");
#if 0
	BSON_APPEND_UTF8(query, "stream_id", "B3OILGKH_C0");
	BSON_APPEND_INT32(query, "pid", getpid());
	BSON_APPEND_UTF8(query, "src_url", "rtmp://localhost:1936/live/1");
	BSON_APPEND_UTF8(query, "dest_url", "rtmp://localhost:1935/live/1000");
	BSON_APPEND_UTF8(query, "type", "rtmp");
	BSON_APPEND_UTF8(query, "status", "active");
	BSON_APPEND_UTF8(query, "host", "10.200.41.89");

  	MongoAdaptor::insert(collection, query);
#else
	//MongoAdaptor::remove(collection, query);
	//MongoAdaptor::update(collection, query, update);
	//vector<string> list;
	//MongoAdaptor::find(collection, query, list);
	//printf("list size = %d\n", list.size());
 	//vector<string>::iterator iter;
	//int index = 0;
	//for(iter = list.begin(); iter != list.end(); iter++)
	//{
	//	printf("item %d ===> %s\n", index++, iter->c_str());
	//}

#endif
	ProcessManager::Instance()->initialize();
	ctrl.init(g_parg);
	ctrl.exec();
#else
    if(start_arg.debug_mode == false)
    {
		if(daemon(1,1)<0)
		{
		    perror("error daemon...\n");
		    exit(1);
		}
		//signal(SIGCHLD,sig_chld);

		/*write pid to file*/
		write_pid_to_file(g_parg->pid_path);
		
		ctrl.Init(g_parg);
		ctrl.Run();
		/*Traversal process module,*/
		//pmp = new main_proc;
		
		//initial process
		//pmp->init_proc();
		
		//pmp->spawn_one_process(PROGRAM);
		
		/*master process run*/
		//pmp->run();

		unlink(g_parg->pid_path);
		
		pconf::destroy(g_parg->mod_tree);
		delete pmp;
		delete ppconf;
		return 0;
	}
	else
	{	
		/*write pid to file*/
		write_pid_to_file(g_parg->pid_path);
			
		/*Traversal process module,*/
		pmp = new main_proc;
		pmp->init_proc();

		//pmp->spawn_one_process("dispatch");
		/*register signal*/
		//pmp->register_signal();
		
		/*master process run*/
		//pmp->run();
		
		pmp->run_work(PROGRAM);

		unlink(g_parg->pid_path);
		delete pmp;
	}
#endif
	//pconf::destroy(g_parg->mod_tree);
	unlink(g_parg->pid_path);	
	pconf::destroy(g_parg->mod_tree);
	delete ppconf;
}

//end
