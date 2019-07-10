
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <dirent.h>  
#include <unistd.h>
#include <memory.h>

#include "process_manager.h"
#include "mongo_adaptor.h"
#include "json/json.h"
#include <boost/shared_ptr.hpp>

namespace p_surf {

static void sig_reap_chld(int signo)
{
    pid_t pid;
    int stat;

    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    {
 	printf("reap child pid = %d\n", pid);
  	
	_message *msg = new _message;
	msg->type = MSG_TYPE_SIGCHIL_NOTIFY;
	msg->data = (void*)(&pid);

	printf("wanghuan ------------------------------------ old pid = %d\n", pid);

	ProcessManager::Instance()->send_message(msg);
    }
}

ProcessManager *ProcessManager::_instance = NULL;

bool ProcessManager::initialize()
{
    signal(SIGCHLD, sig_reap_chld);
    
    pthread_mutex_init(&_mutex, NULL);

    MongoAdaptor::init();

    db_address = "mongodb://localhost:27017";

    create_process_queue();

    create_monitor_thread();
 
    return REC_ERROR_OK;
}

int ProcessManager::send_message(_message *msg)
{
    int ret = REC_ERROR_OK;

    if(NULL == msg) {

	return REC_ERROR_INTERNAL; 
    }

    boost::shared_ptr<_message> m(msg);

    switch(m->type)
    {
    case MSG_TYPE_SIGCHIL_NOTIFY:
    {
   	int pid = *(static_cast<int*>(m->data));
	
	_process_info info;

	if(is_process_exist(pid, info)) {

            process_reload(info);
    	}
    }
	break;

    default:
	break;
    }

    return ret;
}

int ProcessManager::process_reload(_process_info &info)
{
    int ret = REC_ERROR_OK;
   
    printf("process_reload\n");
 
    re_create_process(info);

    return ret;
}

void ProcessManager::multi_mkdir(const char *dir)   
{  
    int i,len;  
    char str[512] = {0};      
    strncpy(str, dir, 512);  
    len=strlen(str);  
    for( i=0; i<len; i++ )  
    {  
        if( str[i]=='/' )  
        {  
            str[i] = '\0';  
            if( access(str,0)!=0 )  
            {  
                mkdir(str, 0777);  
            }  
            str[i]='/';  
        }  
    }  
    if( len>0 && access(str,0)!=0 )  
    {  
        mkdir(str, 0777);  
    }  
    return;  
}

int ProcessManager::create_m3u8(_m3u8_param &param)
{
    int fd;

    printf("create_m3u8 path = %s\n", param.path.c_str());

    if(NULL == opendir(param.path.c_str())) {

  	printf("file path location not exist, create.\n");

 	multi_mkdir(param.path.c_str());
    }

    time_t t = time(NULL);
    char tmp[256] = {0};
    snprintf(tmp, sizeof(tmp), "%s%s_%d.m3u8", param.path.c_str(), param.sid.c_str(), t);

    printf("create_m3u8 create file path = %s\n", tmp);

    //if((access(tmp, F_OK)) != -1)
    //{
    //    printf("file exist\n");

    //	return REC_ERROR_M3U8_FILE_EXIST;
    //}   
    //else
    //{
    fd = open(tmp, O_RDWR|O_CREAT);

    if (fd < 0) {
 		
	printf("create file failed\n");

	return REC_ERROR_CREATE_M3U8_FILE;
    }

    chmod(tmp, 0777);

    close(fd);

    param.file_path = tmp;
    //}

    return REC_ERROR_OK;
}

int ProcessManager::remove_m3u8(_m3u8_param &param)
{
    return REC_ERROR_OK;
}

int ProcessManager::update_process_info(_process_info &param)
{
    int ret = REC_ERROR_OK;
    string db = "rec_distribute";
    string tb = "process_info";
    void *connect = MongoAdaptor::connect(db_address.c_str());
    void *collection = MongoAdaptor::collection(connect, db, tb);
    bson_t *query = bson_new();

    BSON_APPEND_UTF8(query, "stream_id", param.sid.c_str());

    bson_t *update = bson_new();
    bson_t *child = bson_new();

    bson_append_document_begin(update, "$set", -1, child);
    BSON_APPEND_INT32(child, "pid", param.pid);

    printf("wanghuan ------------------------------new pid = %d\n", param.pid);

    bson_append_document_end(update, child);

    printf("new pid = %d\n", param.pid);

    if(!MongoAdaptor::update(collection, query, update)) {

        ret = REC_ERROR_MONGODB;
    }

    bson_destroy(child);
    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    return ret;
}

int ProcessManager::re_create_process(_process_info &param)
{
    pid_t pid;
    pid = fork();

    printf("re_create_process\n");

    if(pid < 0)
    {
        printf("create parent process error. ret=-1\n");

        return REC_ERROR_FORK_FAILED;
    }
    else if(pid > 0)
    {
        printf("child process pid = %d\n", pid);

        param.pid = pid;

        update_process_info(param);
    }
    else if (pid == 0)
    {
        char cmd[1024] = {0};

        snprintf(cmd, sizeof(cmd), "/home/develop/tools/tmp/ffmpeg_3.3.3/bin/ffmpeg -i %s -vcodec copy -acodec copy -map 0 -f hls  -hls_list_size 0 -hls_wrap 0 -hls_time 3 %s", param.src_url.c_str(), param.m3u8_path.c_str());

        printf("wanghuan -- cmd = %s\n", cmd);

        int ret = execl("/bin/sh", "sh", "-c", cmd, (char *)0);

        printf("wanghuan -- fork ffmpeg to pull and injection stream %s failed, pid=%d\n", param.sid.c_str(), pid);

        if (ret < 0)
        {
            printf("ffmpeg launch failed\n");
        }
    }

    return REC_ERROR_OK;
}

int ProcessManager::put_data(_process_info &param)
{
    pthread_mutex_lock(&_mutex);
    _q.push(param);
    pthread_mutex_unlock(&_mutex);

    return REC_ERROR_OK;
}

void *ProcessManager::monitor(void *ptr)
{
    ProcessManager *p = (ProcessManager*)ptr;

    while(p->running())
    {
        printf("monitor detection\n");

	p->orphan_process_sync();

        sleep(5);
    }

    return NULL;
}

bool ProcessManager::check_system_process_exist(int pid)
{
    int ret = kill(pid, 0);

    printf("check_system_process_exist ret = %d\n", ret);

    if(0 == ret) {

	return true;	
    }

    return false;
}

int ProcessManager::orphan_process_sync()
{
    int ret = REC_ERROR_OK;

    vector<_process_info> orphan;

    ret = find_orphan_process(orphan);

    printf("find orphan size = %d\n", orphan.size());

    vector<_process_info>::iterator iter;
    for(iter = orphan.begin(); iter != orphan.end(); iter++) 
    {	
	if(!check_system_process_exist(iter->pid)) 
	{
 	    printf("system process pid = %d not exist\n", iter->pid);

            _process_info info;

            if(is_process_exist(iter->pid, info)) {

                process_reload(info);
            }
    	}
	else
	{
	    //printf("system process pid = %d exist\n", iter->pid);
	}

    }

    return ret;
}

void *ProcessManager::process_queue(void *ptr)
{
    _process_info e;
 
    ProcessManager *p = (ProcessManager*)ptr;

    printf("process_queue running\n");

    while(p->running())
    {
	queue<_process_info> *queue = p->get_queue();

	while(!queue->empty())
	{
	    e = queue->front();
	    
	    printf("e.sid = %s\n", e.sid.c_str());

	    p->spawn_process(e);
 
            queue->pop();

	}
	
	usleep(10);
    }

    return NULL;
}

int ProcessManager::create_monitor_thread()
{
    pthread_t id;
    
    int ret = pthread_create(&id, NULL, monitor, this);
    
    if(ret) {

        printf("create monitor error\n");
        
	return 1;
    }

    return REC_ERROR_OK;
}

int ProcessManager::create_process_queue()
{
    pthread_t id;

    is_running = true;

    int ret = pthread_create(&id, NULL, process_queue, this);

    if(ret) {

        printf("create process queue error\n");

        return 1;
    }

    return REC_ERROR_OK;
}

int ProcessManager::spawn_process(_process_info &param)
{
    int ret;
    pid_t pid;

    if(is_process_exist(param.sid)) {

	return REC_ERROR_STREAM_EXIST;
    }

    _m3u8_param m3u8_param;
    m3u8_param.sid = param.sid;
    m3u8_param.path = param.m3u8_path;

    ret = create_m3u8(m3u8_param);

    if(REC_ERROR_OK != ret) {

	return ret;
    }
    
    pid = fork();

    if(pid < 0)
    {
        printf("wanghuan - create parent process error. ret=-1\n");

        return REC_ERROR_FORK_FAILED;
    }
    else if(pid > 0)
    {
        printf("wanghuan -- child process pid = %d\n", pid);
	
	param.pid = pid;
	param.m3u8_path = m3u8_param.file_path;

	printf("#########################param.m3u8_path: %s\n", param.m3u8_path.c_str());

	add_process_to_cache(param);
    }
    else if (pid == 0)
    {
	char cmd[1024] = {0};

        snprintf(cmd, sizeof(cmd), "/home/develop/tools/tmp/ffmpeg_3.3.3/bin/ffmpeg -i %s -vcodec copy -acodec copy -map 0 -f hls -hls_list_size 0 -hls_wrap 0 -hls_time 3 %s", param.src_url.c_str(), m3u8_param.file_path.c_str());

        printf("wanghuan 999999999999999999 -- cmd = %s\n", cmd);

        ret = execl("/bin/sh", "sh", "-c", cmd, (char *)0);

        printf("wanghuan -- fork ffmpeg to pull and injection stream %s failed, pid=%d\n", param.sid.c_str(), pid);

        if (ret < 0)
        {
            printf("ffmpeg launch failed\n");
        }
    }
	
    return REC_ERROR_OK;
}

int ProcessManager::kill_process(_process_info &info)
{
    _process_info *p = get_process_info(info.sid);

    if(NULL == p) {

	printf("kill sid %s not found\n", info.sid.c_str());

	return REC_ERROR_STREAM_NOT_EXIST;
    }

    boost::shared_ptr<_process_info> process(p);

    
    printf("wanghuan --- kill pid %d\n", process->pid);

    remove_process_by_stream(process->sid);

    kill(process->pid, SIGINT);
	
    return REC_ERROR_OK;
}

int ProcessManager::add_process_to_cache(_process_info &info)
{
    int ret = REC_ERROR_OK;
    string db = "rec_distribute";
    string tb = "process_info";
    void *connect = MongoAdaptor::connect(db_address.c_str());
    void *collection = MongoAdaptor::collection(connect, db, tb);
    bson_t *query = bson_new();

    BSON_APPEND_UTF8(query, "stream_id", info.sid.c_str());
    BSON_APPEND_INT32(query, "pid", info.pid);
    BSON_APPEND_INT32(query, "ppid", getpid());
    BSON_APPEND_UTF8(query, "src_url", info.src_url.c_str());
    BSON_APPEND_UTF8(query, "m3u8_path", info.m3u8_path.c_str());
    BSON_APPEND_UTF8(query, "type", info.type.c_str());
    BSON_APPEND_UTF8(query, "status", "active");
    BSON_APPEND_UTF8(query, "host", info.host.c_str());

    if(!MongoAdaptor::insert(collection, query)) {

	ret = REC_ERROR_MONGODB;
    }
    
    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    return ret;
}

int ProcessManager::find_orphan_process(vector<_process_info> &process_vec)
{
    int ret = REC_ERROR_OK;

    string db = "rec_distribute";
    string tb = "process_info";
    void *connect = MongoAdaptor::connect(db_address.c_str());
    void *collection = MongoAdaptor::collection(connect, db, tb);
    bson_t *query = bson_new();
    bson_t *child = bson_new();

    int pid = getpid();
    printf("getpid = %d\n", pid);
    bson_append_document_begin(query, "ppid", -1, child);
    BSON_APPEND_INT32(child, "$ne", pid);
    bson_append_document_end(query, child);

    vector<string> vec;
    ret = MongoAdaptor::find(collection, query, vec);
    //printf("wanghuan 22222222222222222222222 --- MongoAdaptor::find pid = %d vec size = %d\n", pid, vec.size());
    
    _process_info item;
    vector<string>::iterator iter;
    for(iter = vec.begin(); iter != vec.end(); iter++) {

    	parser(*iter, &item);

	if (item.ppid != pid) {
	    printf("wanghuan --- orphan process pid = %d find, ppid = %d !!!!!!!!!!!\n", item.pid, item.ppid); 
	    process_vec.push_back(item);
	}
    }

    bson_destroy(query);
    bson_destroy(child);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    return ret;
}

bool ProcessManager::is_process_exist(int pid, _process_info &info)
{
    int ret = REC_ERROR_OK;
    string db = "rec_distribute";
    string tb = "process_info";
    void *connect = MongoAdaptor::connect(db_address.c_str());
    void *collection = MongoAdaptor::collection(connect, db, tb);
    bson_t *query = bson_new();
    BSON_APPEND_INT32(query, "pid", pid);

    string item;
    ret = MongoAdaptor::findOne(collection, query, item);

    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    if(REC_ERROR_OK == ret) {

        parser(item, &info);

        return true;
    }
    else
    {
        return false;
    }
}

bool ProcessManager::is_process_exist(string sid)
{
    int ret = REC_ERROR_OK;
    string db = "rec_distribute";
    string tb = "process_info";
    void *connect = MongoAdaptor::connect(db_address.c_str());
    void *collection = MongoAdaptor::collection(connect, db, tb);
    bson_t *query = bson_new();
    BSON_APPEND_UTF8(query, "stream_id", sid.c_str());

    string item;
    ret = MongoAdaptor::findOne(collection, query, item);

    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    if(REC_ERROR_OK == ret) {

	return true;
    }
    else
    {
	return false;
    }
}

int ProcessManager::parser(string item, _process_info *info)
{
    try
    {
    	Json::Reader reader;
    	Json::Value root;

    	if (!reader.parse(item.c_str(), root, false)) {

	    return REC_ERROR_JSON_PARSER;
    	}

    	info->src_url = root["src_url"].asString();
    	//printf("src_url %s\n", info->src_url.c_str());
    	info->m3u8_path = root["m3u8_path"].asString();
    	//printf("m3u8_path %s\n", info->m3u8_path.c_str());
    	info->sid = root["stream_id"].asString();
    	//printf("sid %s\n", info->sid.c_str());
    	info->type = root["type"].asString();
    	//printf("type %s\n", info->type.c_str());
    	info->status = root["status"].asString();
    	info->host = root["host"].asString();
    	info->pid = root["pid"].asInt();
    	info->ppid = root["ppid"].asInt();
    	//printf("ppid = %d\n", info->ppid);
    }
    catch(std::exception &ex)
    {
	printf("json parser exception detail = %s\n", ex.what());

	return REC_ERROR_JSON_PARSER_EXCEPTION; 
    }

    return REC_ERROR_OK;
}

_process_info *ProcessManager::get_process_info(string sid)
{
    int ret = REC_ERROR_OK;

    string db = "rec_distribute";
    string tb = "process_info";
    void *connect = MongoAdaptor::connect(db_address.c_str());
    void *collection = MongoAdaptor::collection(connect, db, tb);
    bson_t *query = bson_new();

    BSON_APPEND_UTF8(query, "stream_id", sid.c_str());

    string item;
    ret = MongoAdaptor::findOne(collection, query, item);

    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    if(REC_ERROR_OK == ret) {

	_process_info *info = new _process_info;

	parser(item, info);

	return info;
    }
    else
    {
 	return NULL;
    }
}

int ProcessManager::remove_process_by_stream(string sid)
{
    int ret = REC_ERROR_OK;
    string db = "rec_distribute";
    string tb = "process_info";
    void *connect = MongoAdaptor::connect(db_address.c_str());
    void *collection = MongoAdaptor::collection(connect, db, tb);
    bson_t *query = bson_new();

    BSON_APPEND_UTF8(query, "stream_id", sid.c_str());
   
    printf("remove_process_by_stream sid = %s\n", sid.c_str());
 
    if(!MongoAdaptor::remove(collection, query)) {

	printf("MongoAdaptor::remove error\n");

	ret = REC_ERROR_MONGODB;
    }

    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    return ret;
}

int ProcessManager::remove_process_by_id(int id)
{
    int ret = REC_ERROR_OK;
    string db = "rec_distribute";
    string tb = "process_info";
    void *connect = MongoAdaptor::connect(db_address.c_str());
    void *collection = MongoAdaptor::collection(connect, db, tb);
    bson_t *query = bson_new();

    BSON_APPEND_INT32(query, "pid", id);

    if(!MongoAdaptor::remove(collection, query)) {

        ret = REC_ERROR_MONGODB;
    }

    bson_destroy(query);
    MongoAdaptor::collection_release(collection);
    MongoAdaptor::connect_close(connect);

    return ret;
}

}
