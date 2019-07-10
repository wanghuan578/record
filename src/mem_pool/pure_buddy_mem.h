/*
* Copyright (c) 2012,surfing
* All rights reserved.
*
* 文件名称：pure_buddy_mem.cpp
* 摘要：应有伙伴算法的内存分配器，支持多进程。
*		锁为pthread。
*/

/*
*版本:1.0.1
*作者：潘兴浩，完成日期：2012年11月22日
*内容：加入多进程支持。
*
*/

/*
* 版本：1.0.0
* 作者：潘兴浩，完成日期：2012年11月18日
*/

#ifndef _PURE_BUDDY_MEM_H_
#define _PURE_BUDDY_MEM_H_
#include "basic_type.h"
#include <pthread.h>

#define CACHE_LINE_SIZE 64
#define MIN_BLOCK_SIZE 64
#define MIN_BLOCK_SIZE_EXP 6
#define ALIGNMENT 8
#define MAX_ORDER 9



namespace pure_baselib
{

	class CBuddymem
	{

		struct free_area_t;
		
		typedef  pthread_mutex_t pure_lock_t; 

		typedef struct buddy_block
		{
			buddy_block * pre;
			buddy_block * next;
			free_area_t * free_area;
			
		}b_block;

		typedef struct free_area_t
		{
			/*lock*/
			pure_lock_t lock;
			/*双向链表*/
			b_block * freelist;
			/*此大小位图首*/
			u32 order;	//块大小的阶数
			u32 pbmap[0];	
		}free_area;

		typedef struct mem_t
		{
			s32 len;	//缓冲区长度

			s32 remain_len;	//剩余长度
			free_area * free_start;	//free_area，首元素地址 
			b_block * block_start;	//blocks 首地址
			u32 align_area_size;	//对齐后free_area大小

		}mem;

	private:
		mem * pm;
		u32 align_block_size;	//对齐后block头大小
	
		s32 init_free();

		inline u8 set_bitmap(u32 index,free_area *fa);

		inline s32 add_block(b_block *,free_area *);
		inline s32 del_block(b_block * b,free_area *fa);
	public:
		
		CBuddymem();
		~CBuddymem();

		s32 init_buddy_mem(void *,u32);

		void * Alloc(u32 l);
		void Free(void *);

		void print_buddy_status();
	};
}


#endif