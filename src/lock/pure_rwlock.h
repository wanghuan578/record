/**************************************************************
* Copyright (c) 2012,surfing
* All rights reserved.
* Filename£ºpure_rwlock.h
* Description£º
***************************************************************/
/**************************************************************
* Version£º1.0.0
* Original_Author£ºpxh
* Date£º2012-12-24
***************************************************************/

#ifndef _PURE_RWLOCK_H_
#define _PURE_RWLOCK_H_

#include "basic_type.h"

#include "sys_headers.h"
#include <pthread.h>
#define PTHREAD

namespace pure_baselib
{

#ifdef PTHREAD
  typedef struct pure_rwlock_s
  {
      pthread_rwlock_t  rwlock;
      pthread_rwlockattr_t  rwlock_attr;
  }PRWLOCK_S;
/*
  extern  s32 pure_rwlock_init(PRWLOCK_S *prwlock);

  extern  s32 pure_rwlock_rdlock(PRWLOCK_S *prwlock);

  extern  s32 pure_rwlock_wrlock(PRWLOCK_S *prwlock);

  extern  s32 pure_rwlock_unlock(PRWLOCK_S *prwlock);

  extern  s32 pure_rwlock_destroy(PRWLOCK_S *prwlock);
*/
inline s32 pure_rwlock_init(PRWLOCK_S *prwlock)
{
	return pthread_rwlock_init(&(prwlock->rwlock), NULL);
}


inline s32 pure_rwlock_rdlock(PRWLOCK_S *prwlock)
{
	s32 ret = 0;
	while(ret = pthread_rwlock_rdlock(&(prwlock->rwlock)))
	{
		PDEBUG("in lock\n");
		if(ret != 0)
		{
			if(errno == EAGAIN)
				continue;
			else
				perror("wrlock");
		}
		return ret;
	}

}

inline s32 pure_rwlock_wrlock(PRWLOCK_S *prwlock)
{
	s32 ret = 0;
	while(pthread_rwlock_wrlock(&(prwlock->rwlock)))
	{
		if(ret != 0)
		{
			if(errno == EAGAIN)
				continue;
			else
				perror("wrlock");
		}
		return ret;
	}
}


inline s32 pure_rwlock_unlock(PRWLOCK_S *prwlock)
{
	return pthread_rwlock_unlock(&(prwlock->rwlock));
}

 inline s32 pure_rwlock_destroy(PRWLOCK_S *prwlock)
 {
	 return pthread_rwlock_destroy(&(prwlock->rwlock));
 }

#endif

}



#endif // _PURE_RWLOCK_H_
