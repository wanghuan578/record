#ifndef _P_MEMBOX_H_
#define _P_MEMBOX_H_

#include "basic_type.h"
#include <string.h>
#include <malloc.h>

#include "basic_type.h"
#include "atomic/atom_opr.h"

#define ALIGNMENT 8
#define PAGE_SIZE 4096
#define CACHE_LINE_SIZE 64

#define pure_align_ptr(p, a)                                                   \
		(u8 *) (((ul64) (p) + ((ul64) a - 1)) & ~((ul64) a - 1))
	
#define pure_align_u32(p, a)                                                   \
		(u32) (((u32) (p) + ((u32) a - 1)) & ~((u32) a - 1))

#define MEMBLOCKHEAD 16		//the size of memblock in front of p 

namespace p_base
{

template <typename T>
class membox
{

	struct mempool_t;
	struct memblock_empty
	{
		u32 	seq;			//0 means free 1 means used 
		mempool_t * mp;
		u8 p[0];
	};
	
	typedef struct memblock_t
	{	
		u32 	seq;			//0 means free 1 means used 
		mempool_t * mp;
		u8 p[pure_align_u32(pure_align_u32(sizeof(T),ALIGNMENT) + MEMBLOCKHEAD,CACHE_LINE_SIZE)-sizeof(memblock_empty)];
	}memblock;

	typedef struct mempool_t
	{
		mempool_t * next;
		u32 length;
		u8 off_t;	//	由于cacheline对齐产生的偏移
		u32 block_num;
		u32 free_num;
		memblock * pa;
	}mempool;
	
	
private:
	mempool * mp_mpool;
	
	u32 m_mbsize;
	u32 m_poolsize;
//	u32 m_alignedsize;	//字节对齐后，头长度

	mempool * creat_mempool(mempool * p ,u32 l)
	{
		
		mempool * mp = NULL;
	
		/*initialize */
		if(p == NULL)
		{
			void * true_mem = new_zone(l);
			if(true_mem == NULL)
			{
				printf("malloc error \n");
				return NULL;
			}
			
			mp = (mempool *)pure_align_ptr(true_mem,CACHE_LINE_SIZE);
			mp->off_t = (u8 *)mp - (u8*)true_mem;
	
			mp ->next = NULL;
			mp ->length = l-mp->off_t;
			return mp;
		}
		else
		{
			if(p->next == NULL)
			{
				void * true_mem = new_zone(l);
				if(true_mem == NULL)
				{
					printf("malloc failed %d\n",l);
					return NULL;
				}
				p->next = (mempool *)pure_align_ptr(true_mem,CACHE_LINE_SIZE);
				mp = p->next;
				mp->off_t = (u8 *)mp - (u8 *)true_mem;
				mp->next = NULL;
				mp->length = l - mp->off_t;
			}
			else
			{
				printf("p->next != NULL");
				return NULL;
			}
	
			return mp;
		}
	}
	void init_memblock(mempool *,u32);
	void * mempool_alloc(mempool* pmp);
	void destroy(mempool **);

	inline virtual void * new_zone(u32);
	inline virtual void delete_zone(void *);
public:

	membox(u32);
	~membox();
	void * box_alloc();
	void box_free(void *);

};


	template <typename T>
	membox<T>::membox(u32 pool_size)
	{

		m_mbsize = pure_align_u32(pure_align_u32(sizeof(T),ALIGNMENT) + 8,CACHE_LINE_SIZE);
		printf("size = %d\n",sizeof(memblock));
		m_poolsize = pure_align_u32(pool_size,PAGE_SIZE);

		mp_mpool = NULL;
		mp_mpool = creat_mempool(mp_mpool,m_poolsize);
	
		if(mp_mpool != NULL)
		{
			init_memblock(mp_mpool,sizeof(memblock));
		}
	
		return;
	}

#if 0
	template <typename T>
	membox<T>::mempool*
	membox::creat_mempool(mempool * p,u32 l)
	{	
		mempool * mp = NULL;
	
		/*initialize */
		if(p == NULL)
		{
			void * true_mem = new_zone(l);
			if(true_mem == NULL)
			{
				printf("malloc error \n");
				return NULL;
			}
			
			mp = (mempool *)pure_align_ptr(true_mem,CACHE_LINE_SIZE);
			mp->off_t = (u8 *)mp - (u8*)true_mem;
	
			mp ->next = NULL;
			mp ->length = l-mp->off_t;
			return mp;
		}
		else
		{
			if(p->next == NULL)
			{
				void * true_mem = new_zone(l);
				if(true_mem == NULL)
				{
					printf("malloc failed %d\n",l);
					return NULL;
				}
				p->next = (mempool *)pure_align_ptr(true_mem,CACHE_LINE_SIZE);
				mp = p->next;
				mp->off_t = (u8 *)mp - (u8 *)true_mem;
				mp->next = NULL;
				mp->length = l - mp->off_t;
			}
			else
			{
				printf("p->next != NULL");
				return NULL;
			}
	
			return mp;
		}
	}
#endif

	template <typename T>
	inline void * membox<T>::new_zone(u32 l)
	{
		void * p = malloc(l);
		if(p != NULL)
		{
			memset(p,0,l);
			return p;
		}
		else
		{
			return NULL;
		}
	}

	template <typename T>
	inline void membox<T>::delete_zone(void * p)
	{
		free(p);
		return;
	}

	template <typename T>
	void * membox<T>::mempool_alloc(mempool* p)
	{
		memblock * b = NULL;
		if(p->free_num == 0)
		{
			printf("membox pool is full!\n");
			return NULL;			
		}
		//int startpos = p->free_num;
		int startpos = p->block_num - p->free_num;
		
		for(int i = 0;i<p->block_num;i++)
		{
			if(p->pa[(startpos+i)%p->block_num].seq == 0)
			{
				if(CAS(&(p->pa[(startpos+i)%p->block_num].seq),0,1))
				{
					ATOMIC_SUB(&p->free_num,1);
					p->pa[(startpos+i)%p->block_num].mp=p;
					printf("repeat times = %d\n",i);
					return p->pa[(startpos+i)%p->block_num].p;
				}
			}
						
		}

		printf("membox pool is full!\n");
		return NULL;			
		
		
		/*if no freehead is not NULL ,Create new pool*/
		/*
		if(p->freehead == NULL && p->next == NULL)
		{
	
			p = creat_mempool(p,m_poolsize);
			if(p == NULL)
			{
				printf("pool alloc error\n");
				return NULL;
			}
			init_memblock(p,m_mbsize);
		}
		*/
		/* find the free node ,return  data point*/
		
	}
	
	template <typename T>
	membox<T>::~membox()
	{
		destroy(&mp_mpool);
	}

	template <typename T>
	void membox<T>::destroy(mempool ** p)
	{
		mempool * pmp;
		mempool * dpmp = *p;
		
		while(dpmp!=NULL)
		{
			pmp = dpmp;
			dpmp = dpmp->next;
			delete_zone((u8 *)pmp-pmp->off_t);
		}
		*p = NULL;
		return;
	}

	template <typename T>
	void membox<T>::init_memblock(mempool * p,u32 ul)
	{

		u32 mpalignsize = pure_align_u32(sizeof(mempool), CACHE_LINE_SIZE);
		char * pos = ((char *)p) + mpalignsize;		//CACHE align
		char * mbpre = NULL;
		
		u32 content_len = p->length-ul-mpalignsize;	// memblock occupy max lenth
		p->pa = (memblock *)pos;
		p->block_num = content_len/sizeof(memblock);
		p->free_num = p->block_num;
		printf("p->free_num = %d\n",p->free_num);
		return;
	}

	template <typename T>
	void * membox<T>::box_alloc()
	{
		return(mempool_alloc(mp_mpool));
	}
	
	
	template <typename T>
	void membox<T>::box_free(void * data)
	{
	  
		memblock * pmb = (memblock *)((u8 *)data - MEMBLOCKHEAD);
		if(CAS(&(pmb->seq),1,0))
		{
			ATOMIC_ADD(&pmb->mp->free_num,1);
			return;
		}
		else
		{
			printf("free error,seq != 1\n");
		}
		return;
	}
}

#endif

