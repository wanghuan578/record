#include "base_connect_timer.h"
#include "base_circle.h"
#include "base_connect.h"

using namespace p_base;
namespace p_base
{

s32 CBase_connect_timer::set_timer_event(s32 i_retry_time)
{

    m_retry_time = i_retry_time;
    return 0;
}

s32 CBase_connect_timer::handle_expire_timer(CPure_timer * i_pure_time)
{

	s32 ret = m_pcon->check_timeout();

	return ret;
	
}



}

