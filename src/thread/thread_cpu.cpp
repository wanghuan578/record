#include "thread_cpu.h"

namespace p_base
{

s32 get_cpu_num()
{
	return sysconf(_SC_NPROCESSORS_CONF);
}

/*
EFAULT A supplied memory address was invalid.

EINVAL (pthread_setaffinity_np()) The affinity bit	mask  mask	contains  no
	   processors  that are currently physically on the system and permitted
	   to the thread according to any restrictions that may  be  imposed  by
	   the "cpuset" mechanism described in cpuset(7).

EINVAL (pthread_setaffinity_np())  cpuset  specified  a CPU that was outside
	   the set supported by the kernel.  (The  kernel  configuration  option
	   CONFIG_NR_CPUS  defines	the range of the set supported by the kernel
	   data type used to represent CPU sets.)

EINVAL (pthread_getaffinity_np()) cpusetsize is smaller than the size of the
	   affinity mask used by the kernel.

ESRCH  No thread with the ID thread could be found.

*/


s32 thread_set_cpu(pthread_t thread,int i_cpun)
{
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(i_cpun, &mask);
	return pthread_setaffinity_np(thread, sizeof(mask), &mask);
}

s32 thread_get_cpu(pthread_t thread,int * o_cpun)
{
	return 0;
}

s32 proc_set_cpu(pid_t pid,int i_cpu)
{
	cpu_set_t mask;  
    CPU_ZERO(&mask);      
    CPU_SET(i_cpu, &mask);
	
	if(sched_setaffinity(pid, sizeof(mask), &mask) == -1)      //0 代表对当前线程/进程进行设置。   
    {  
        printf("set affinity failed..");  
		return -1;
    }
	
	return 0;
}

}



