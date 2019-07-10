#include "p_signals.h"

namespace p_surf
{

signal_reg_s g_sig_reg_table[]=
{
	{LIVE_SHUTDOWN_SIGNAL,NULL,NULL},
	{LIVE_TERMINATE_SIGNAL,NULL,NULL},
	{LIVE_RECONFIGURE_SIGNAL,NULL,NULL},
	{NULL,NULL,NULL}
};

}
