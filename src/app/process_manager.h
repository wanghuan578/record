
#ifndef __PROCESS_MANAGER_H__
#define __PROCESS_MANAGER_H__

#include <stdlib.h>
#include <string>
#include <list>
#include <queue>
#include "common.h"

using namespace std;


typedef enum message_type
{
    MSG_TYPE_SIGCHIL_NOTIFY,
}_message_type;

typedef struct message
{
    _message_type type;
    void *data;
}_message;

namespace p_surf {

typedef void (*sync_to_cache)(_process_info &param);

class ProcessManager
{
public:
    static ProcessManager *Instance()
    {
        if(NULL == _instance)
	{
	    _instance = new ProcessManager;
	}
 	
	return _instance;
    }

    static void  Destroy()
    {
	if(NULL != _instance)
	{
	    delete _instance;

	    _instance = NULL;
	}
    }

    bool initialize();
    int spawn_process(_process_info &info);
    int kill_process(_process_info &info);
    int create_m3u8(_m3u8_param &param);
    int remove_m3u8(_m3u8_param &param);
    int put_data(_process_info &param);
private:
    ProcessManager() {};
    ~ProcessManager() {pthread_mutex_destroy(&_mutex);};

public:
    int add_process_info(_process_info &info);
    _process_info *get_process_info(string sid);
    int remove_process_by_stream(string sid);
    int remove_process_by_id(int id);
    int process_reload(_process_info &info);
    //int process_reload(string sid);
    int send_message(_message *msg);
    inline bool running() { return is_running;}
    inline queue<_process_info> *get_queue() {return &_q;}

private:
    int re_create_process(_process_info &param);
    bool is_process_exist(string sid);
    bool is_process_exist(int pid, _process_info &info);
    int add_process_to_cache(_process_info &info);
    int find_orphan_process(vector<_process_info> &vec);
    int orphan_process_sync();
    bool check_system_process_exist(int pid);    
    int update_process_info(_process_info &param);
    int parser(string item, _process_info *info);
    void multi_mkdir(const char *dir);
    int create_monitor_thread();
    int create_process_queue();
    static void *process_queue(void *ptr);
    static void *monitor(void *ptr);

private:
    static ProcessManager *_instance;
    list<_process_info> process_list;
    pthread_mutex_t _mutex;
    string db_address;
    queue<_process_info> _q;
    bool is_running;
};

}
#endif
