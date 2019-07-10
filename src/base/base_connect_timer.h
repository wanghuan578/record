#ifndef _BASE_CONNECT_TIMER_H_
#define _BASE_CONNECT_TIMER_H_


#include "basic_type.h"
#include "timer/pure_timer_event.h"
#include "timer/pure_timer.h"

using namespace p_base;

namespace p_base
{

class CBase_connect;
class CPure_timer;
class CPure_timer_event;
/*
*	used to check whether if connection is timeout.
*
*
*/

class CBase_connect_timer:public CPure_timer_event
{
public:
	CBase_connect_timer(CBase_connect * pcon):CPure_timer_event(pcon){}
	virtual s32 handle_expire_timer(CPure_timer * i_pure_time);
	s32 set_timer_event(s32 i_retry_time);

protected:

	s32 m_retry_time;


};


}





#endif
