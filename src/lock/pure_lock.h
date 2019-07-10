/**************************************************************
* Copyright (c) 2012,surfing
* All rights reserved.
* Filename£ºpure_lock.h
* Description£º
***************************************************************/
/**************************************************************
* Version£º1.0.0
* Original_Author£ºpxh
* Date£º2012-12-23
***************************************************************/
#ifndef _PURE_LOCK_H_
#define _PURE_LOCK_H_

#include <pthread.h>
#define PTHREAD

namespace p_base
{
#ifdef PTHREAD
  typedef struct pure_lock_s
  {
      pthread_mutex_t mutex;
      pthread_mutexattr_t mutex_attr;
  }PLOCK_S;

  inline s32 pure_lock_init(PLOCK_S *plock,int range = 0)
  {
	 /* s32 ret = pthread_attr_init(plock->mutex_attr);
	  if(ret!=0)
	  {
		  perror("attr_init failure\n");
		  return -1;
	  }
  	*/
	
	int ret;
	ret = pthread_mutexattr_init(&plock->mutex_attr); /* * resetting to its default value: private */
	if(range == 0)
	{
		ret = pthread_mutexattr_setpshared(&plock->mutex_attr, PTHREAD_PROCESS_PRIVATE);
	}
	else
	{
		ret = pthread_mutexattr_setpshared(&plock->mutex_attr,PTHREAD_PROCESS_SHARED);
	}
	
	return pthread_mutex_init(&plock->mutex,&plock->mutex_attr);
  
  }
  
  inline s32 pure_lock_dolock(PLOCK_S *plock)
  {
	 return  pthread_mutex_lock(&plock->mutex);
  }
  
  
  inline s32 pure_lock_unlock(PLOCK_S *plock)
  {
	  return pthread_mutex_unlock(&plock->mutex);
  }
  
  inline s32 pure_lock_destroy(PLOCK_S *plock)
  {
  	pthread_mutexattr_destroy(&plock->mutex_attr);
	 pthread_mutex_destroy(&plock->mutex);
	  return SUCCESS;
  }

  inline s32 pure_lock_trylock(PLOCK_S *plock)
  {
  	return  pthread_mutex_trylock(&plock->mutex);
  }
#endif
}

#endif // _PURE_LOCK_H_
