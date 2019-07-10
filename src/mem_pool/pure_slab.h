/*
* Copyright (c) 2012,surfing
* All rights reserved.
*
* �ļ����ƣ�pure_slab.h
* ժҪ��slab��������������Ƶ�������ͷŹ̶���С����ĳ�����
*		��ǰ�汾ֻ���ڵ����̣���������������ͷŲ���������
*/

/*
* �汾��1.0.0
* ���ߣ����˺ƣ�������ڣ�2012��11��19��
*/
#ifndef _PURE_SLAB_H_
#define _PURE_SLAB_H_


#include "basic_type.h"
#include <string.h>
#include <malloc.h>

#define ALIGNMENT 8
#define PAGE_SIZE 4096
#define CACHE_LINE_SIZE 64

#define pure_align_ptr(p, a)                                                   \
		(u8 *) (((ul64) (p) + ((ul64) a - 1)) & ~((ul64) a - 1))
	
#define pure_align_u32(p, a)                                                   \
		(u32) (((u32) (p) + ((u32) a - 1)) & ~((u32) a - 1))
	
	

namespace p_base
{

class pure_slab
{
	struct mempool_t;
	typedef struct memblock_t
	{	
		memblock_t * pre;
		memblock_t * next;
		mempool_t * mp;

	}memblock;

	typedef struct mempool_t
	{
		mempool_t * next;
		memblock * usedhead;
		memblock * freehead;
		u32 length;
		u8 off_t;	//	����cacheline���������ƫ��
	}mempool;
	
	
private:
	mempool * mp_mpool;
	s32 m_max_num;
	s32 m_alloced_num;
	
	u32 m_mbsize;
	u32 m_poolsize;
	u32 m_alignedsize;	//�ֽڶ����ͷ����

	mempool * creat_mempool(mempool *,u32);
	void init_memblock(mempool *,u32);
	void * mempool_alloc(mempool* pmp);
	void destroy(mempool **);

//	plog * log;
	inline virtual void * new_zone(u32);
	inline virtual void delete_zone(void *);
public:

	pure_slab(u32, u32,u32);
	pure_slab(u32, u32);
	~pure_slab();
	void * slab_alloc();
	void slab_free(void *);

};

}
#endif
