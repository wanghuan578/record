#ifndef _PURE_POOL_H_
#define _PURE_POOL_H_


#include "basic_type.h"
//#include "plog.h"
#define ALIGNMENT 8
#define PAGE_SIZE 4096


namespace pure_baselib
{



class pure_slab_pool
{

	struct mempool;
	typedef struct memblock
	{	
		memblock * pre;
		memblock * next;
		mempool * mp;
		
	}MB;

	typedef struct mempool
	{
		mempool * next;
		s32 len;
		memblock * usedhead;
		memblock * freehead;
		
	}MP;

	
private:
	MP ** pmpool;
	u32 * mbsize;
	s32 lvl;
	s32 poolnum;
	u32 * poolsize;
	u32 * initialsize;
	
	MP * CreatePool(MP *,s32);
	void InitMemblock(MP *,s32);
	void * MPAlloc(MP* pmp,s32 l);
	void Destroy(MP **);
//	plog * log;
public:
	
	pure_slab_pool(u32, s32 );
	~pure_slab_pool();
	void * Alloc(s32);
	void Free(void *);

};

}
#endif
