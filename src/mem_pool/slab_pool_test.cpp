#include "pure_slab_pool.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>   
#include <stdio.h>   
#include <stdlib.h>   
  
#include <sys/mman.h>   
#include <fcntl.h>   
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h> 
#include<sys/types.h>
#include<sys/wait.h> 

  
#ifndef _POSIX_THREAD_PROCESS_SHARED       
#error "This platform does not support process shared mutex"       
#endif   
  


using namespace pure_baselib;

int main()
{


	int cycle_times = 20;

	pure_slab_pool slab(60,10);

	u8 * test[20];

	for(int i = 0;i<cycle_times;i++)
	{

		test[i] = (u8 *)slab.alloc(100);
		printf("alloc = %x\n",test[i]);
		memset(test[i],'s',100);
	}

	for(int i = 0;i<cycle_times;i++)
	{
		slab.free(test[i]);
		printf("alloc = %x\n",test[i]);
		memset(test[i],'s',100);
	}
	
	

	return 0;
}


