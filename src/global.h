/**************************************************************
* Copyright (c) 2012,surfing
* All rights reserved.
* Filename：global.h
* Description：定义调试用的宏定义
***************************************************************/
/**************************************************************
* Version：1.0.0
* Original_Author：chengzhip
* Date：2013-05-26
* Modified_Author：
* Modified_Description：
* Modified_Date:
***************************************************************/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "sys_headers.h"
#include "basic_type.h"
#include "log/pure_log.h"
#include "global_conf.h"




using namespace p_base;

#define FI_INDEXFILE "index"

namespace p_surf
{

	extern CLog* g_psyslog;
	extern CLog* g_perrlog;
	extern G_CONF_S * g_parg;
	
	extern bool g_restart_sig;
	extern bool g_reload_sig;

	/*file index*/
	//extern FITH_CT* work_tasks;
	extern int workthreads;
	extern char * g_tspath;
}


#endif /* _GLOBAL_H_ */
