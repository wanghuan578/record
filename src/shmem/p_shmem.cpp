/*
 * sharememory.cpp
 *
 *  Created on: 2012-12-6
 *      Author: li
 */

#include "p_shmem.h"
#include "string/p_string.h"

namespace p_base
{

Sh_mem::Sh_mem()
{
	memset(name,0,sizeof(name));
	shmaddr = NULL;
	size = 0;
	shm_id = 0;

}

Sh_mem::~Sh_mem()
{
	if (shmdt(shmaddr) == -1) 
	{
		perror("shmdt error! error:");
	}
	shm_id =  0;
	shmaddr = NULL;
}


//******************************************************************
//创建匿名共享内存
//*******************************************************************
s32 Sh_mem::alloc(int shmsize,int key)
{
	size = shmsize;
	
	if(key == 0)
	{
		shm_id = shmget(IPC_PRIVATE, size, (0666|IPC_CREAT) ) ;
	}
	else
	{
		shm_id = shmget(key, size, (0666|IPC_CREAT) );
	}
	
	printf("shm_id = %d\n",shm_id);
	if ( shm_id < 0 )
	{
		perror("get shm ipc_id error") ;
	    return FAILURE;
	}

	shmaddr = (char *)shmat( shm_id, NULL, 0 );
	if ( shmaddr == (void *)-1 )
	{
		perror("shmat addr error") ;
	}


    return (shmaddr == (void *) -1) ? FAILURE : SUCCESS;

}



//date:2012-12-7
//*******************************************************************
s32 Sh_mem::free()
{

	
	if (shmctl(shm_id, IPC_RMID, NULL) == -1) 
	{
		perror("shmctl IPC_RMID error");
	}


	return 0;


}

s32 Sh_mem::connectshm(int key)
{

	shm_id = shmget(key, 0, (SHM_R|SHM_W|IPC_CREAT) ) ;
	if ( shm_id < 0 )
	{
		perror("get shm ipc_id error") ;
	    return FAILURE;
	}

	shmaddr = (char *)shmat( shm_id, NULL, 0 );
	if ( shmaddr == (void *)-1 )
	{
		perror("shmat addr error") ;
	}

	
	return (shmaddr == (void *) -1) ? FAILURE : SUCCESS;
	
}

void Sh_mem::set_name(const s8 * i_name)
{

	rc_strncpy(name,MIN_STRING_SIZE,i_name,strlen(i_name) + 1);
	return;
	
}



} 
