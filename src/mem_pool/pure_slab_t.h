/*
* Copyright (c) 2012,surfing
* All rights reserved.
*
* 文件名称：pure_slab.h
* 摘要：slab分配器，适用于频繁分配释放固定大小对象的场景。
*		当前版本只用于单进程，多进程请给申请和释放操作加锁。
*/

/*
* 版本：1.0.0
* 作者：潘兴浩，完成日期：2012年11月19日
*/
#ifndef _PURE_SLAB_T_H_
#define _PURE_SLAB_T_H_


#include "basic_type.h"
#include <string.h>
#include <malloc.h>

#include "basic_type.h"
#define ALIGNMENT 8
#define PAGE_SIZE 4096
#define CACHE_LINE_SIZE 64

#define pure_align_ptr(p, a)                                                   \
		(u8 *) (((ul64) (p) + ((ul64) a - 1)) & ~((ul64) a - 1))
	
#define pure_align_u32(p, a)                                                   \
		(u32) (((u32) (p) + ((u32) a - 1)) & ~((u32) a - 1))
	
	

namespace pure_baselib
{

template <typename T,u32 MAX_NUM = 0>
class pure_slab_t
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
		u8 off_t;	//	由于cacheline对齐产生的偏移
	}mempool;
	
	
private:
	mempool * mp_mpool;
	s32 m_max_num;
	s32 m_alloced_num;
	
	u32 m_mbsize;
	u32 m_poolsize;
	u32 m_alignedsize;	//字节对齐后，头长度

	mempool * creat_mempool(mempool *,u32);
	void init_memblock(mempool *,u32);
	T * mempool_alloc(mempool* pmp);
	void destroy(mempool **);

//	plog * log;
	inline virtual void * new_zone(u32);
	inline virtual void delete_zone(void *);
public:

	pure_slab_t(u32, u32);
	~pure_slab_t();
	T * slab_alloc();
	void slab_free(T *);

};

template <typename T>
	class pure_slab_t<T, 0>
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
			u8 off_t;	//	由于cacheline对齐产生的偏移
		}mempool;
		
		
	private:
		mempool * mp_mpool;
		
		u32 m_mbsize;
		u32 m_poolsize;
		u32 m_alignedsize;	//字节对齐后，头长度
	
		mempool * creat_mempool(mempool *,u32);
		void init_memblock(mempool *,u32);
		T * mempool_alloc(mempool* pmp);
		void destroy(mempool **);
	
	//	plog * log;
		inline virtual void * new_zone(u32);
		inline virtual void delete_zone(void *);
	public:
	
		pure_slab_t(u32, u32);
		~pure_slab_t();
		T * slab_alloc();
		void slab_free(T *);
	
	};


	template <typename T,u32 MAX_NUM>
	pure_slab_t<T,MAX_NUM>::mempool * pure_slab_t::creat_mempool(mempool * p,u32 l)
	{	
		mempool * mp = NULL;
	
		/*initialize */
		if(p == 0)
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
				if(true_mem == 0)
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

	template <typename T,u32 MAX_NUM>
	void pure_slab_t<T,MAX_NUM>::init_memblock(mempool * p,u32 ul)
	{

		u32 mpalignsize = pure_align_u32(sizeof(mempool), ALIGNMENT);
		char * pos = ((char *)p) + mpalignsize;
		char * mbpre = NULL;
		unsigned int offs = mpalignsize;
		p->freehead = (memblock *)pos;
	
		while(offs <= (p->length-ul))
		{
			((memblock *)pos)->pre = (memblock *)mbpre;
			if(((memblock *)pos)->pre != NULL)
			{
				((memblock *)pos)->pre->next=(memblock *)pos;
			}
		//	log->prt(3,"mb pos = %x\n",pos);
			((memblock *)pos)->next = 0;
		
			mbpre = pos;
			pos += ul;
			offs += ul;
	
		}
		memblock * ph = p->freehead;
		while(ph!=NULL)
		{
		//	log->prt(3,"chain pos = %x\n",ph);
			ph =ph->next;
		}
		p->usedhead = NULL;
		return;
	}
	
	template <typename T,u32 MAX_NUM>
	void * pure_slab_t<T,MAX_NUM>::mempool_alloc(mempool* p)
	{
		if(m_alloced_num >= m_max_num)
		{
			return NULL;
		}
		
		memblock * b = NULL;
		
		/*search freehead whick is not NULL*/
		while(p->next !=NULL)
		{
	
			if(p->freehead == NULL)
			{
				p = p->next;
			}
			else
			{
				break;
			}
		}
	
		/*if no freehead is not NULL ,Create new pool*/
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
	
		/* find the free node ,return  data point*/
		if((p!=NULL)&&((p->freehead != NULL)))
		{
	
			/*remove from freehead*/
			b = p->freehead;
			
			//log->prt(3,"alloc freehead = %x\n",(void *)p->freehead);
			p->freehead = b->next;
			//	log->prt(3,"alloced freehead = %x\n",(void *)p->freehead);
			if(p->freehead != NULL)
			{
				p->freehead->pre = NULL;
			}
			/*add node to usedhead*/
	
			b->mp = p;
	
			if(p->usedhead != NULL)
			{
				p ->usedhead->pre = b;
			}
			b->next = p->usedhead;
			p->usedhead = b;
			p->usedhead->pre = NULL;
	
			//log->prt(3,"alloc pos = %x,poolnum = %d\n",b,poolnum);
			m_alloced_num ++;
			return ((char *)b + m_alignedsize);
		}
	}

	template <typename T>
	void * pure_slab_t<T,0>::mempool_alloc(mempool* p)
	{
		memblock * b = NULL;
		
		/*search freehead whick is not NULL*/
		while(p->next !=NULL)
		{
	
			if(p->freehead == NULL)
			{
				p = p->next;
			}
			else
			{
				break;
			}
		}
	
		/*if no freehead is not NULL ,Create new pool*/
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
	
		/* find the free node ,return  data point*/
		if((p!=NULL)&&((p->freehead != NULL)))
		{
	
			/*remove from freehead*/
			b = p->freehead;
			
			//log->prt(3,"alloc freehead = %x\n",(void *)p->freehead);
			p->freehead = b->next;
			//	log->prt(3,"alloced freehead = %x\n",(void *)p->freehead);
			if(p->freehead != NULL)
			{
				p->freehead->pre = NULL;
			}
			/*add node to usedhead*/
	
			b->mp = p;
	
			if(p->usedhead != NULL)
			{
				p ->usedhead->pre = b;
			}
			b->next = p->usedhead;
			p->usedhead = b;
			p->usedhead->pre = NULL;
	
			//log->prt(3,"alloc pos = %x,poolnum = %d\n",b,poolnum);
			
			return ((char *)b + m_alignedsize);
		}
	}
		
	template <typename T>
	pure_slab_t<T,0>::pure_slab_t(u32 block_size,u32 pool_size)
	{
	
		m_alignedsize = pure_align_u32(sizeof(memblock), ALIGNMENT);
	
		m_mbsize = pure_align_u32(block_size,ALIGNMENT) + m_alignedsize;
		m_poolsize = pure_align_u32(pool_size,PAGE_SIZE);
	
		mp_mpool = NULL;
		mp_mpool = creat_mempool(mp_mpool,m_poolsize);
		if(mp_mpool != NULL)
		{
			init_memblock(mp_mpool,m_mbsize);
		}
		return;
	}

	template <typename T,u32 MAX_NUM>
	pure_slab_t<T,MAX_NUM>::pure_slab_t(u32 block_size,u32 pool_size)
	{
	//	log = new plog();
	//	log->loginit("pool.log",NULL);
	//	log->setlevel(0);
	//	lvl = ilvl;
		m_max_num = MAX_NUM;
		m_alloced_num = 0;
		m_alignedsize = pure_align_u32(sizeof(memblock), ALIGNMENT);
	
		m_mbsize = pure_align_u32(block_size,ALIGNMENT) + m_alignedsize;
		
		m_poolsize = pure_align_u32(pool_size,PAGE_SIZE);
	
		mp_mpool = NULL;
		mp_mpool = creat_mempool(mp_mpool,m_poolsize);
		if(mp_mpool != NULL)
		{
			init_memblock(mp_mpool,m_mbsize);
		}
		return;
	}
	
	template <typename T,u32 MAX_NUM>
	pure_slab_t<T,MAX_NUM>::~pure_slab()
	{
		destroy(&mp_mpool);
	}

	template <typename T,u32 MAX_NUM>
	void * pure_slab_t<T,MAX_NUM>::slab_alloc()
	{
		return(mempool_alloc(mp_mpool));
	}
	
	
	template <typename T>
	void pure_slab_t<T,0>::slab_free(void * data)
	{
	  
		memblock * pmb = (memblock *)((u8 *)data - m_alignedsize);
		mempool * pmp = pmb->mp;
	//	log->prt(3,"free pos = %x\n",pmb);
		/*	be not the usedhead*/
		if(pmb->pre != NULL)
		{
			/*remove from used*/
			pmb->pre->next = pmb->next;
			if(pmb->next != NULL)
			{
				pmb->next->pre = pmb->pre;
			}
		}
		else
		{
			/*be the usedhead*/
			pmp->usedhead = pmb->next;
			if(pmp->usedhead != NULL)
			{
				pmp->usedhead->pre = NULL;
			}
		}
	
		/*add node to freelist*/
		if(pmp->freehead != NULL)
		{
			pmp->freehead->pre = pmb;
		}
		pmb->pre = NULL;
		pmb->next = pmp->freehead;
		pmp->freehead = pmb;
	
		return;
	}

	template <typename T,u32 MAX_NUM>
	void pure_slab_t<T,MAX_NUM>::slab_free(void * data)
	{
	  
		memblock * pmb = (memblock *)((u8 *)data - m_alignedsize);
		mempool * pmp = pmb->mp;
	//	log->prt(3,"free pos = %x\n",pmb);
		/*	be not the usedhead*/
		if(pmb->pre != NULL)
		{
			/*remove from used*/
			pmb->pre->next = pmb->next;
			if(pmb->next != NULL)
			{
				pmb->next->pre = pmb->pre;
			}
		}
		else
		{
			/*be the usedhead*/
			pmp->usedhead = pmb->next;
			if(pmp->usedhead != NULL)
			{
				pmp->usedhead->pre = NULL;
			}
		}
	
		/*add node to freelist*/
		if(pmp->freehead != NULL)
		{
			pmp->freehead->pre = pmb;
		}
		pmb->pre = NULL;
		pmb->next = pmp->freehead;
		pmp->freehead = pmb;

		m_alloced_num--;
		return;
	}
	
	template <typename T,u32 MAX_NUM>
	void pure_slab_t<T,MAX_NUM>::destroy(mempool ** p)
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


	template <typename T,u32 MAX_NUM>
	inline void * pure_slab_t<T,MAX_NUM>::new_zone(u32 l)
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

	template <typename T,u32 MAX_NUM>
	inline void pure_slab_t<T,MAX_NUM>::delete_zone(void * p)
	{
		free(p);
		return;
	}

};

#endif
