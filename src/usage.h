#ifndef _USAGE_H_
#define _USAGE_H_

#include "basic_type.h"
#include "sys_headers.h"

#define ENDLINE "\n"

#define PROGRAM "config"
#define VER "0.2.0"

#define DEFAULT_CONF_PATH "../conf/"PROGRAM".conf"
#define DEFAULT_LICENSE_PATH "../conf/"PROGRAM".lic"
#define DEFAULT_LOG_PATH "../log/"

enum signal_type
{
	SIGNAL_STOP = 1,
	SIGNAL_RELOAD
};

extern char g_usage[];

using namespace p_base;

typedef struct argument
{

	const char * test_conf_s;
	const char * set_conf_s;

	const char * send_mod_s;
	u8     send_sig_s;
	
//	const char * test_lic_s;

	
	bool show_help;
	bool show_version;
	bool test_config;
//	bool test_license;
	bool set_config;
	bool send_signal;
	bool debug_mode;

}G_ARG_S;




int get_argument(int argc,char *const * argv,struct argument & argu_s);

#endif
