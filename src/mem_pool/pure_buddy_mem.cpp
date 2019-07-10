/*
* Copyright (c) 2012,surfing
* All rights reserved.
*
* 文件名称：pure_buddy_mem.cpp
* 摘要：应有伙伴算法的内存分配器，支持多进程。
*/

/*
*版本:1.0.1
*作者：潘兴浩，完成日期：2012年11月22日
*内容：调整对外接口函数和函数结构
*
*/

/*
* 版本：1.0.0
* 作者：潘兴浩，完成日期：2012年11月18日
*/
#include "pure_buddy_mem.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace pure_baselib;

#define pure_align_ptr(p, a)                                                   \
    (u8 *) (((ul64) (p) + ((ul64) a - 1)) & ~((ul64) a - 1))

#define pure_align_u32(p, a)                                                   \
    (u32) (((u32) (p) + ((u32) a - 1)) & ~((u32) a - 1))

#define pure_lock(lock) \
	pthread_mutex_lock(&lock)   

#define pure_unlock(lock) \
	pthread_mutex_unlock(&lock);   

s32 CBuddymem::init_free()
{


  	/* Set pthread_mutex_attr to process shared */ 
			
	pthread_mutexattr_t mutex_shared_attr;  
  	pthread_mutexattr_init(&mutex_shared_attr);   
  	pthread_mutexattr_setpshared(&mutex_shared_attr, PTHREAD_PROCESS_SHARED);   
  
	int block_size = 0;
	int order = MAX_ORDER;
	free_area * fa = pm->free_start;

	/*估算位图所需位数，及最大freearea大小*/
	int bitamount = ((pm->remain_len / MIN_BLOCK_SIZE + 15) >>(3+1));
	pm->align_area_size = pure_align_u32(sizeof(free_area) + bitamount,CACHE_LINE_SIZE);
	
	/*检测剩余空间是否满足初始化*/
	if(pm->align_area_size * (MAX_ORDER + 1) > pm->remain_len)
	{
		printf("no enough zone to init free_area\n");
		return -1;
	}

	/*初始化free_area空间*/
	memset(fa,0,pm->align_area_size  * (MAX_ORDER + 1));
	pm->remain_len -= (pm->align_area_size  * (MAX_ORDER + 1));
	pm->block_start = (b_block *)((u8 *)pm->free_start + pm->align_area_size * (MAX_ORDER +1));

	   
  
	/*初始化blocks*/
	fa = (free_area *)((u8 *)pm->free_start + pm->align_area_size * MAX_ORDER);
	b_block * pblock = pm->block_start;
	for(;order>=0;order--)
	{
	
		pthread_mutex_init(&(fa->lock), &mutex_shared_attr);   
		block_size = MIN_BLOCK_SIZE<<order;
		fa->order = order;
	//	memset(fa->pbmap,0xff,bitamount);
		while(pm->remain_len >= block_size)
		{
			  
			  add_block(pblock,fa);
			  u32 index = ((u8 *)pblock-(u8 *)pm->block_start)/MIN_BLOCK_SIZE;
			  set_bitmap(index,fa);
			  pm->remain_len -= block_size;
			  pblock = (b_block *)((u8 *)pblock + block_size);
		}
	
		fa = (free_area *)((u8 *)fa - pm->align_area_size);
	}
	return 0;
}

inline s32 CBuddymem::add_block(b_block * b,free_area *fa)
{
	//printf("block = %x,order = %d\n",b,fa->order);	
	b_block * tempb = fa->freelist;
	if(tempb != NULL)
	{
		tempb->pre = b;
	}
	fa->freelist = b;
	b->pre = NULL;
	b->next = tempb;
	b->free_area = fa;
}


void * CBuddymem::Alloc(u32 l)
{
	/*至少要分配的大小*/
	u32 tlen = l+align_block_size;

	u32 i = ((tlen<<2)-1)>>MIN_BLOCK_SIZE_EXP;
	s32 bitp = 0;
	if(i > 0)
	{
		asm("bsr %1, %%eax;"
		"movl %%eax, %0;"
		:"=r" (bitp)
		:"r" (i)
		:"%eax"
		);
	}
	else
	{
		bitp = 0;
	}
	
	if(bitp > MAX_ORDER)
	{
		printf("too large size to alloc\n");
		return NULL;
	}

		
	/*找到符合block大小的free_area*/
	free_area * fa = (free_area *)((u8 *)pm->free_start + pm->align_area_size*bitp);
	free_area * rfa = fa;	  
	u8 * pb = NULL;
	u32 index = 0;
	
	for(int order = bitp;order<=MAX_ORDER;order++)
	{
		pure_lock(fa->lock);
		if(fa->freelist != NULL)
		{
			/*从大到小循环处理*/
			free_area * dfa ;
			/*找到空余的内存块*/
			pb = (u8 *)fa->freelist;
			//index = (pb-(u8 *)pm->block_start)/MIN_BLOCK_SIZE;
			
			index = (pb-(u8 *)pm->block_start)>>MIN_BLOCK_SIZE_EXP;
			
			/*从freelist中去掉该节点*/
			
			/*重新检测freelist,因为有可能是空*/
			/*if(fa->freelist == NULL)
			{
				pure_unlock(fa->lock);
				fa = (free_area *)((u8 *)fa + pm->align_area_size);
				continue;
			}*/
			
			//printf("freelist = %x\n",fa->freelist);
			fa->freelist = fa->freelist->next;
			if(fa->freelist != NULL)
			{
				fa->freelist->pre = NULL;
			}
			
			/*更改位图值*/
			
			set_bitmap(index,fa);
			pure_unlock(fa->lock);
			
			/*从下一级别开始，每级别递减半*/
			u8 * degrade_pb = NULL;
			u32 degrade_index = 0; 
			s32 dorder = order-1;
			while(dorder >= bitp)
			{
	
				/*一分为二*/
				degrade_pb = pb + (MIN_BLOCK_SIZE<<dorder);
				degrade_index = (degrade_pb-(u8 *)pm->block_start)/MIN_BLOCK_SIZE;

				dfa = (free_area *)((u8 *)pm->free_start + pm->align_area_size*dorder);
				
				pure_lock(dfa->lock);
				
				add_block((b_block *)degrade_pb,dfa);
				set_bitmap(degrade_index,dfa);
				
				pure_unlock(dfa->lock);
				dorder--;
			}
			((b_block *)pb)->free_area = rfa;
			//printf("fa [%d] size = %d\n",order,MIN_BLOCK_SIZE<<order);
			
			return (void *)(pb+align_block_size);
		}
		pure_unlock(fa->lock);
		fa = (free_area *)((u8 *)fa + pm->align_area_size);
	}
	printf("not found enough large block!\n");
	return NULL;
	
}

void CBuddymem::Free(void * p)
{
	b_block * free_block = (b_block *)((u8 *)p - align_block_size);

	free_area * fa = free_block->free_area;

	/*当前block索引值*/
	u32 index = ((u8 *)free_block-(u8 *)pm->block_start)/MIN_BLOCK_SIZE;
	u32 buddy_index;		//伙伴索引值
	u32 upgrade_index;	//高级索引值
	u8 ret =0;
	while((ret==0)&& ((u8 *)fa < (u8 *)pm->block_start))
	{
		
		buddy_index = ((index>>fa->order) ^ 1)<<fa->order;
		b_block * buddy = (b_block *)((u8 *)pm->block_start + MIN_BLOCK_SIZE*buddy_index);
		upgrade_index = (index>>(fa->order+1))<<(fa->order+1);
		
		pure_lock(fa->lock);

		ret = set_bitmap(index,fa);
		if(ret == 0 && fa->order<MAX_ORDER)
		{
			/*将伙伴节点删除*/
			del_block(buddy,fa);
			free_block = (b_block *)((u8 *)pm->block_start + MIN_BLOCK_SIZE*upgrade_index);
	
		}
		else
		{
			add_block(free_block,fa);
		}
		
		pure_unlock(fa->lock);
		fa = (free_area *)((u8 *)fa + pm->align_area_size);
	}
	
	return;
}

inline s32 CBuddymem::del_block(b_block * b,free_area *fa)
{
	//printf("block = %x,order = %d\n",b,fa->order);
	b_block * t = fa->freelist;
	while(t!=NULL)
	{
		if(t == b)
		{
			/*删除节点*/
			if(t == fa->freelist)
			{
				fa->freelist = t->next;
			}

			if(t->pre!= NULL)
			{
				t->pre->next = t->next;
			}
			
			if(t->next != NULL)
			{
				t->next->pre = t->pre;
			}
			return 0;
		}
		else
		{
			t = t->next;
		}
		
	}
	printf("the node is not in the list\n");
	return -1;
}

s32 CBuddymem::init_buddy_mem(void * b,u32 l)
{
	pm = (mem *) pure_align_ptr((u8 *)b,CACHE_LINE_SIZE);
	if(l <sizeof(mem))
	{
		printf("the buffer size is too small to init！ please alloc memory at least %d\n",sizeof(mem));
		return -1;
	}

	pm->len = l-((u8 *)pm-(u8 *)b);
	pm->free_start = (free_area *)pure_align_ptr((u8 *)pm+sizeof(mem),CACHE_LINE_SIZE);
	
	//align_area_size = pure_align_u32(sizeof(free_area),CACHE_LINE_SIZE);
	align_block_size = pure_align_u32(sizeof(b_block),ALIGNMENT);
	
	//pm->block_start = (b_block *)((u8 *)pm->free_start + align_area_size * (MAX_ORDER +1));
	pm->remain_len = l -((u8 *)pm-(u8 *)b) - ((u8 *)pm->free_start - (u8 *)pm);
	if(pm->remain_len < MIN_BLOCK_SIZE)
	{
		printf("the buffer size is too small to init block! %d bytes memory will be used store info\n",sizeof(mem));
		return -1;
	}
	return init_free();
}


void CBuddymem::print_buddy_status()
{
	free_area * fa = (free_area *)((u8 *)pm->free_start);
	b_block * pblock = NULL;
	u8 * pb = NULL;
	u32 index = 0;
	
	for(int order = 0;order<=MAX_ORDER;order++)
	{
		index = 0;
		pblock = fa->freelist;
		while(pblock != NULL)
		{
			pblock = pblock->next;
			index++;
		}
		//printf("fa [%d] size = %d,free block = %d,fa_size = %d\n",order,MIN_BLOCK_SIZE<<order,index,pm->align_area_size);
		fa = (free_area *)((u8 *)fa + pm->align_area_size);
	}
}

inline u8 CBuddymem::set_bitmap(u32 index,free_area *fa)
{
	u8 * bitmap = (u8 *)fa->pbmap;
	int bitpos = index>>(fa->order+1)&0x7;
	int bytepos = index>>(fa->order+1+3);
	bitmap[bytepos] ^= (1<<bitpos);
	return (bitmap[bytepos]>>bitpos)& 0x1;
}	

CBuddymem::CBuddymem()
{

	
}
	
CBuddymem::~CBuddymem()
{

}
