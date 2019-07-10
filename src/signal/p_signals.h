#ifndef _P_SIGNALS_H_
#define _P_SIGNALS_H_

#include "basic_type.h"
#include "sys_headers.h"

using namespace p_base;

#define LIVE_SHUTDOWN_SIGNAL      SIGQUIT
#define LIVE_TERMINATE_SIGNAL     SIGTERM
#define LIVE_RECONFIGURE_SIGNAL   SIGHUP

namespace p_surf
{

typedef void (*PF_SIG)(int signo,void * data) ;

typedef struct signal_reg_s
{
	int signo;
	PF_SIG pf;
	void * userdata;
}SR_S;

extern SR_S  g_sig_reg_table[];


}



#endif


