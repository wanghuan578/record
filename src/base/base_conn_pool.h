#ifndef _BASE_CONN_POOL_H_
#define _BASE_CONN_POOL_H_

#include "basic_type.h"
#include "base/base_circle.h"
#include "mem_pool/pure_slab.h"
#include "mem_pool/p_membox.h"


using namespace p_base;

namespace p_base
{
class CBase_conn_pool
{
public:
	virtual void connect_delete(CBase_connect *con) = 0;
	virtual CBase_connect* connect_create(s32 connfd) = 0;
};


template <typename T>
class CConn_pool:public CBase_conn_pool
{
public:

	pure_slab * connect_pool;
	s32 connect_limit;
	s32 init(s32 i_c);
	
	virtual void connect_delete(CBase_connect *con);
	virtual CBase_connect* connect_create(s32 connfd);

	CConn_pool(s32 i_limit);

	static s32 conn_free(void * pd_todel,void * i_pool);
private:
		
};

template <typename T>
s32 CConn_pool<T>::conn_free(void * pd_todel,void * i_pool)
{
	pure_slab * ppool = (pure_slab *)i_pool;
	((T*)pd_todel)->~T();
	ppool->slab_free(pd_todel);
}

template <typename T>
CConn_pool<T>::CConn_pool(s32 i_limit)
{
	if(i_limit > 0)
	{
		connect_pool = new pure_slab(sizeof(T),sizeof(T)*i_limit,i_limit);
	}
	else
	{
		connect_pool = new pure_slab(sizeof(T),sizeof(T)*128);
	}
	
}


template <typename T>
void CConn_pool<T>::connect_delete(CBase_connect *con)
{
	T * pt = dynamic_cast<T *>(con);
	pt->release();

	return ;
}

template <typename T>
CBase_connect* CConn_pool<T>::connect_create(s32 connfd)
{
	void * mempt = connect_pool->slab_alloc();
	if(mempt)
	{
		T * r = new(mempt) T(connfd);
		r->m_pcp = this;
		r->set_func(conn_free,connect_pool);
		return r;
	}
	
	return NULL;
}


template <typename T>
class CMConn_pool:public CBase_conn_pool
{
public:

	membox<T> * connect_pool;
	s32 connect_limit;
	s32 init(s32 i_c);
	
	virtual void connect_delete(CBase_connect *con);
	virtual CBase_connect* connect_create(s32 connfd);

	CMConn_pool(s32 i_limit);

	static s32 conn_free(void * pd_todel,void * i_pool);
private:
		
};

template <typename T>
s32 CMConn_pool<T>::conn_free(void * pd_todel,void * i_pool)
{
	membox<T> * ppool = (membox<T> *)i_pool;
	((T*)pd_todel)->~T();
	ppool->box_free(pd_todel);
}

template <typename T>
CMConn_pool<T>::CMConn_pool(s32 i_limit)
{
	if(i_limit > 0)
	{
		connect_pool = new membox<T>(sizeof(T)*(i_limit+16));
	}
	else
	{
		connect_pool = new membox<T>(sizeof(T)*(128+16));
	}
	
}


template <typename T>
void CMConn_pool<T>::connect_delete(CBase_connect *con)
{
	T * pt = dynamic_cast<T *>(con);
	//pt->release();
	delete pt;
	return ;
}

template <typename T>
CBase_connect* CMConn_pool<T>::connect_create(s32 connfd)
{
	/*
	void * mempt = connect_pool->box_alloc();
	if(mempt)
	{
		T * r = new(mempt) T(connfd);
		r->m_pcp = this;
		r->set_func(conn_free,connect_pool);
		return r;
	}
	*/
	T * r =new T(connfd);
	r->m_pcp = this;
	r->set_func(conn_free,connect_pool);
	return r;
	
	return NULL;
}



}

#endif
