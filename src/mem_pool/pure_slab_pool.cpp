

#include "pure_slab_pool.h"
#include "sys_headers.h"

#define pure_align_ptr(p, a)                                                   \
    (u8 *) (((ul64) (p) + ((ul64) a - 1)) & ~((ul64) a - 1))

#define pure_align_u32(p, a)                                                   \
    (u32) (((u32) (p) + ((u32) a - 1)) & ~((u32) a - 1))


/*
*未按大小分别创建内存池
*分配小内存时，空间利用率低
*/
namespace pure_baselib
{

	pure_slab_pool::MP * pure_slab_pool::CreatePool(MP * p,int l)
	{	
		MP * mp = NULL;
		/*initialize */
		if(p == NULL)
		{
			mp = (MP *)malloc(l);
			if(mp == NULL)
			{
				printf("malloc error \n");
				exit(0);
			}
			memset(mp,0,l);
			poolnum++;
			mp ->next = NULL;
			mp ->len = l;
			return mp;
		}
		else
		{
			if(p->next == NULL)
			{
				p->next = (MP*)(malloc(l));
				if(p->next == 0)
				{
					printf("calloc failed %d\n",l);
					exit(0);
				}
				memset(p->next,0,l);
			}
			else
			{
				printf("p->next != NULL");
				return NULL;
			}
			
			mp = p->next;
			mp->next = NULL;
			mp->len = l;
			poolnum++;
			return mp;
		}
	}

	void pure_slab_pool::InitMemblock(MP * p,int ul)
	{


		char * pos = ((char *)p) + sizeof(MP);
		char * mbpre = NULL;
		unsigned int offs = sizeof(MP);
		p->freehead = (MB *)pos;

		while(offs <= (p->len-ul))
		{
			((MB *)pos)->pre = (MB *)mbpre;
			if(((MB *)pos)->pre != NULL)
			{
				((MB *)pos)->pre->next=(MB *)pos;
			}
		//	log->prt(3,"mb pos = %x\n",pos);
			((MB *)pos)->next = NULL;
		
			mbpre = pos;
			pos += ul;
			offs += ul;

		}
		/*
		MB * ph = p->freehead;
		while(ph!=NULL)
		{
		//	log->prt(3,"chain pos = %x\n",ph);
			ph =ph->next;
		}
		*/
		p->usedhead = NULL;
		return;
	}


	void * pure_slab_pool::MPAlloc(MP* p,int ilvl)
	{
		MB * b = NULL;
		
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

			p = CreatePool(p,poolsize[ilvl]);
			if(p == NULL)
			{
				printf("pool alloc error\n");
				return NULL;
			}
			InitMemblock(p,mbsize[ilvl]);
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
			
			return ((char *)b+sizeof(MB));
		}
	}

	pure_slab_pool::pure_slab_pool(unsigned int size,int ilvl)
	{
	//	log = new plog();
	//	log->loginit("pool.log",NULL);
	//	log->setlevel(0);
		lvl = ilvl;
		unsigned int alignedsize = pure_align_u32(size, ALIGNMENT)  ;
		PDEBUG("alignedsize %d\n",alignedsize);
		mbsize = new unsigned int[lvl+1];
		if(mbsize == NULL)
		{
			printf("new mbsize failed \n");
			exit(0);
		}
		initialsize = new unsigned int[lvl+1];
		if(initialsize== NULL)
		{
			printf("new initialsize failed \n");
			exit(0);
		}
		poolsize = new unsigned int[lvl+1];
		if(poolsize== NULL)
		{
			printf("new poolsize failed \n");
			exit(0);
		}
		int i = 0;
		for(i=0;i<=lvl;i++)
		{
			mbsize[i] = (alignedsize <<i) + sizeof(MB);
			initialsize[i] = (pure_align_u32(mbsize[i] , ALIGNMENT*2))<<6 ;
			poolsize[i] = initialsize[i];
		}
		pmpool = new MP*[lvl+1];
	//	printf("size = %d",sizeof(pmpool));
		
		if(pmpool == NULL)
		{
			printf("new pmpool failed \n");
			exit(0);
		}
	    poolnum = 0;
		for(i=0;i<=lvl;i++)
		{
	//		printf("mbsize = %u,initialize = %u\n",mbsize[i],initialsize[i]);
			pmpool[i] = NULL;
			pmpool[i] = CreatePool(pmpool[i],initialsize[i]);
		//	pmpool[i] = (MP *)malloc(initialsize[i]);
			if(pmpool[i] != NULL)
			{
				InitMemblock(pmpool[i],mbsize[i]);
			}
		}
		
	}

	pure_slab_pool::~pure_slab_pool()
	{
		int i=0;
		for(i=0;i<=lvl;i++)
		{
			Destroy(&pmpool[i]);
		}
		delete[] mbsize;
		delete[] initialsize;
		delete[] poolsize;
		delete[] pmpool;
	}

	void * pure_slab_pool::Alloc(int l)
	{
		
		MP * pmp;
		int i = ((l+sizeof(MB)-1)/mbsize[0];
		int bitp = 0;
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

		if(bitp >lvl)
		{
		    printf("alloc size too big\n");
			return NULL;
		}
		
		PDEBUG("bit %d,%x\n",bitp,i);
		PDEBUG("mbsize= %lu,i =%lu",mbsize[bitp],l);
		
		pmp = pmpool[bitp];
		return(MPAlloc(pmp,bitp)); 
		
	}



	void pure_slab_pool::Free(void * data)
	{
	//	printf("free\n");   
		MB * pmb = (MB *)((char *)data - sizeof(MB));
		MP * pmp = pmb->mp;
	//	log->prt(3,"free pos = %x\n",pmb);
		/*  be not the usedhead*/
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


	void pure_slab_pool::Destroy(MP ** p)
	{
		MP * pmp;
		MP * dpmp = *p;
		
		while(dpmp!=NULL)
		{
			pmp = dpmp;
			dpmp = dpmp->next;
			free(pmp);
		}
		*p = NULL;
		return;
	}

}


