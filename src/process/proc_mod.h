#ifndef _PROC_MOD_H_
#define _PROC_MOD_H_
#include "conf/pconf.h"
#include "global_conf.h"

#include <map>
#include <string>


using namespace p_base;
using namespace p_surf;

#define DECLARE_DYNBASE(process)   \
		public:\
		\
		typedef process* (*proc_creator)();\
		\
		static process * create(const std::string & name);\
		static std::map<const std::string,proc_creator>& get_class_set()\
		{\
			static std::map<const std::string,proc_creator> class_set; \
			return class_set;\
		}\
		\
		struct _auto_register\
		{\
			_auto_register(const std::string& name, process::proc_creator creator)\
			{\
				process::get_class_set().insert(std::pair<const std::string,process::proc_creator>(name,creator));\
			}\
		};


#define IMPLEMENT_DYNBASE(process) \
	    process* process::create(const std::string& class_name)\
	    {\
		    std::map<const std::string,process::proc_creator>::iterator it; \
		    it = get_class_set().find(class_name); \
		    if(it != get_class_set().end())\
			{ \
				    return (it->second)();\
			}\
			return NULL;\
		}


#define DECLARE_DYNCREATE(derived,base) \
public: \
    static base* create(){ \
        return (base*)(new derived); \
       }
  
#define IMPLEMENT_DYNCREATE(derived) \
    static derived::_auto_register _register_##derived(#derived,derived::create);   


namespace p_surf
{

class process  
{  
	DECLARE_DYNBASE(process)			// 声明动态创建基类  
	
	//DECLARE_DYNCREATE(process,process)	 // 基类也可以动态创建	 
public:

	process(){};
	virtual ~process(){};

		
	pid_t  pid;
	s32    channel;	//socket
	CN_S*  local_conf;	
	s32    status;

	//fork前，初始化
	virtual s32 init_process(G_CONF_S * i_gconf) = 0;
	
	//fork后，初始化配置
	virtual s32 init_conf(G_CONF_S * i_gconf) = 0;
	
	virtual s32 run() = 0;
	virtual s32 destroy() = 0;
	
};

	
}



#endif
