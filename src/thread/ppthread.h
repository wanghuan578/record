#ifndef _PPTHREAD_H_
#define _PPTHREAD_H_

#include "thread_cpu.h"

namespace p_base
{

typedef struct threads_info_s
{
	s32 cpu_nums;
	s32 *threads_on_cpu;
	s32 low_th;
	s32 high_th;
	bool bindcpu;		//1 bind 2 unbind
}THRD_INFO;


THRD_INFO * thread_info_init(s32 low ,s32 high,bool bindsig);
s32 thread_info_clear(THRD_INFO *);

s32 spawn_thread(void * (*pf)(void *),void *,THRD_INFO *);


};

#endif



