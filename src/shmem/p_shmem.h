#ifndef SHAREMEMORY_H_
#define SHAREMEMORY_H_
#include <fcntl.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#include <sys/shm.h>
#include <error.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "basic_type.h"


namespace p_base
{

class Sh_mem
{

public:
	Sh_mem();
	~Sh_mem();
	s32 alloc(s32 shmsize,int key);
	void set_name(const s8 * i_name);
	s32 connectshm(s32 key);
	s32 free();
	
public:
	s8  name[MIN_STRING_SIZE];
	s32 shm_id;
	s8 * shmaddr;
	s32 size;

};

}
#endif 
