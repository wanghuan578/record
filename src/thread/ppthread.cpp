#include "ppthread.h"

namespace p_base
{


THRD_INFO * thread_info_init(s32 low ,s32 high,bool bindsig)
{
	THRD_INFO * pinfo = (THRD_INFO *)malloc(sizeof(THRD_INFO));
	if(pinfo == NULL)
	{
		return NULL;
	}
	pinfo->cpu_nums = get_cpu_num();
	if(low  < 0 || low >= pinfo->cpu_nums)
	{
		pinfo->low_th = 0;
	}
	else
	{
		pinfo->low_th = low - 1;
	}
	
	if(high <= pinfo->low_th || high >= pinfo->cpu_nums )
	{
		pinfo->high_th = pinfo->cpu_nums -1;
	}
	else
	{
		pinfo->high_th = high;
	}
	
	pinfo->threads_on_cpu = (s32 *)malloc(sizeof(s32)*pinfo->cpu_nums); 
	memset(pinfo->threads_on_cpu,0,sizeof(s32)*pinfo->cpu_nums);
	pinfo->bindcpu = bindsig;

	return pinfo;
}

s32 spawn_thread(void * (*pf)(void *),void * arg,THRD_INFO * pinfo)
{

	pthread_t pid;

	pthread_attr_t attr; 
  
     pthread_attr_init(&attr); 
     
     pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); 
  
	s32 ret = pthread_create(&pid, &attr,pf,arg);

	u32 min = 0xffffffff;
	s32 index = 0;
	if(pinfo->bindcpu)
	{
		for(s32 i = pinfo->low_th;i<=pinfo->high_th;i++)
		{
			if(pinfo->threads_on_cpu[i] < min)
			{
				min = pinfo->threads_on_cpu[i];
				index = i;
			}
						
		}
		printf("set thread cpu pid  %d,%d,cpu amount = %d\n",pid,index,pinfo->cpu_nums);
		if(thread_set_cpu(pid,index)!=0)
		{
			printf("set cpu error");
		}
		else
		{
			pinfo->threads_on_cpu[index]++;
		}
		
	}


	return SUCCESS;
}


s32 thread_info_clear(THRD_INFO * pinfo)
{
	free(pinfo->threads_on_cpu);

}

}

