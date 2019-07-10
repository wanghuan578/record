#ifndef _THREAD_CPU_H_
#define _THREAD_CPU_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

#include "basic_type.h"

namespace p_base
{

s32 get_cpu_num();

s32 thread_set_cpu(pthread_t thread,int i_cpun);

s32 thread_get_cpu(pthread_t thread,int * o_cpun);

s32 proc_set_cpu(pid_t pid,int i_cpun);

};


#endif
