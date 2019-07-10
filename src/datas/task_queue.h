/**************************************************************
* Copyright (c) 2012,surfing
* All rights reserved.
* Filename：task_queue.h
* Description：声明任务队列类的变量和方法
***************************************************************/
/**************************************************************
* Version：1.0.0
* Original_Author：chengzhip
* Date：2013-05-14
* Modified_Author：
* Modified_Description：
* Modified_Date: 
***************************************************************/

#ifndef _TASK_QUEUE_H_
#define _TASK_QUEUE_H_


#include "atomic/atom_opr.h"

using namespace p_base;


namespace p_surf
{

	#define TASK_QUEUE_DEBUG_TERM			0
	#define TASK_QUEUE_DEBUG_ERROR			1
	
	#define SINGLE						1			//单进多出
	#define MULTI							2			//多进多出
	//#define MPMC							3
	
	template <typename ELEM_T, s32 MODE>
	class CTask_queue 
	{
		
	private:
		volatile u32	m_rdindex;			//可读索引，总是指向当前可读的位置
		volatile u32	m_wrindex;			//可写索引，总是指向下一个可写的位置
		volatile u32	m_max_rdindex;		//最大可读位置，用于保证多线程安全性

		s32				m_size;				//队列的大小
		s32				m_cursize;			//当前任务个数
		ELEM_T			*mp_elems;			//任务队列
	public:
	
		CTask_queue()
		{
			m_rdindex = m_wrindex = m_max_rdindex = 0;
			m_cursize = m_size = 0;
			mp_elems = NULL;
		}

		~CTask_queue()
		{
			if (mp_elems)
				delete [] mp_elems;
		}

		/*********************************************************
		 * @brief: 函数功能简介
		 * @function_name: queue_init
		 * @description: 任务队列初始化，队列总大小为size
		 * @param[in]: size，s32类型，队列的大小
		 * @param[out]: 无
		 * @return: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(0)
		**********************************************************/
		s32 queue_init(s32 size)
		{
			m_size = size;
			mp_elems = new ELEM_T[size];
			//MYASSERT(mp_elems != NULL);
			memset(mp_elems, 0, sizeof(ELEM_T) * m_size);
			
			return SUCCESS;
		}
		

		/*********************************************************
		 * @brief: 函数功能简介
		 * @function_name: queue_resize
		 * @description: 任务队列扩容，总的大小为原来的大小加上现在要扩容的大小
		 * @param[in]: size，s32类型，扩容的大小
		 * @param[out]: 无
		 * @return: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(0)
		**********************************************************/
		s32 queue_resize(s32 size)
		{
			s32 ori_size = m_size;
			
			m_size += size;
			ELEM_T* tmp = new ELEM_T[m_size];
			//MYASSERT(tmp != NULL);
			
			memset(tmp, 0, sizeof(ELEM_T) * m_size);
			memcpy(tmp, mp_elems, ori_size * sizeof(ELEM_T));
			delete [] mp_elems;
			
			mp_elems = tmp;
			
			return SUCCESS;
		}
		

		inline bool is_empty(u32 max_rdindex, u32 rdindex) 
		{
			return (rdindex % m_size) == (max_rdindex %m_size); 
		}

		inline bool is_full(u32 wrindex, u32 rdindex) 
		{
			return ((wrindex + 1) % m_size) == (rdindex % m_size); 
		}
		
		/*********************************************************
		 * @brief: 函数功能简介
		 * @function_name: enqueue
		 * @description: 任务入队
		 * @param[in]: ele，ELEM_T类型引用，添加到任务队列的任务
		 * @param[out]: 无
		 * @return: bool类型，成功返回true，失败返回false
		**********************************************************/
		
		inline bool dequeue(ELEM_T &ele);
		
		/*********************************************************
		 * @brief: 函数功能简介
		 * @function_name: dequeue
		 * @description: 任务出队
		 * @param[in]: ele，ELEM_T类型引用，删除任务队列的任务
		 * @param[out]: 无
		 * @return: bool类型，成功返回true，失败返回false
		**********************************************************/	
		inline bool enqueue(ELEM_T &ele);
	
		
		
	private:

	};

	template <typename ELEM_T>
	class CTask_queue<ELEM_T,SINGLE>
	{
		
	private:
		volatile u32	m_rdindex;			//可读索引，总是指向当前可读的位置
		volatile u32	m_wrindex;			//可写索引，总是指向下一个可写的位置
		volatile u32	m_max_rdindex;		//最大可读位置，用于保证多线程安全性

		s32				m_size;				//队列的大小
		s32				m_cursize;			//当前任务个数
		ELEM_T			*mp_elems;			//任务队列
	public:
	
		CTask_queue()
		{
			m_rdindex = m_wrindex = m_max_rdindex = 0;
			m_cursize = m_size = 0;
			mp_elems = NULL;
		}

		~CTask_queue()
		{
			if (mp_elems)
				delete [] mp_elems;
		}

		/*********************************************************
		 * @brief: 函数功能简介
		 * @function_name: queue_init
		 * @description: 任务队列初始化，队列总大小为size
		 * @param[in]: size，s32类型，队列的大小
		 * @param[out]: 无
		 * @return: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(0)
		**********************************************************/
		s32 queue_init(s32 size)
		{
			m_size = size;
			mp_elems = new ELEM_T[size];
			//MYASSERT(mp_elems != NULL);
			memset(mp_elems, 0, sizeof(ELEM_T) * m_size);
			
			return SUCCESS;
		}
		

		/*********************************************************
		 * @brief: 函数功能简介
		 * @function_name: queue_resize
		 * @description: 任务队列扩容，总的大小为原来的大小加上现在要扩容的大小
		 * @param[in]: size，s32类型，扩容的大小
		 * @param[out]: 无
		 * @return: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(0)
		**********************************************************/
		s32 queue_resize(s32 size)
		{
			s32 ori_size = m_size;
			
			m_size += size;
			ELEM_T* tmp = new ELEM_T[m_size];
			//MYASSERT(tmp != NULL);
			
			memset(tmp, 0, sizeof(ELEM_T) * m_size);
			memcpy(tmp, mp_elems, ori_size * sizeof(ELEM_T));
			delete [] mp_elems;
			
			mp_elems = tmp;
			
			return SUCCESS;
		}
		

		inline bool is_empty(u32 max_rdindex, u32 rdindex) 
		{
			return (rdindex % m_size) == (max_rdindex %m_size); 
		}

		inline bool is_full(u32 wrindex, u32 rdindex) 
		{
			return ((wrindex + 1) % m_size) == (rdindex % m_size); 
		}
		
		/*********************************************************
		 * @brief: 函数功能简介
		 * @function_name: enqueue
		 * @description: 任务入队
		 * @param[in]: ele，ELEM_T类型引用，添加到任务队列的任务
		 * @param[out]: 无
		 * @return: bool类型，成功返回true，失败返回false
		**********************************************************/
		
		inline bool dequeue(ELEM_T &ele);
		
		/*********************************************************
		 * @brief: 函数功能简介
		 * @function_name: dequeue
		 * @description: 任务出队
		 * @param[in]: ele，ELEM_T类型引用，删除任务队列的任务
		 * @param[out]: 无
		 * @return: bool类型，成功返回true，失败返回false
		**********************************************************/	
		inline bool enqueue(ELEM_T &ele);
	
		
		
	private:

	};

		template <typename ELEM_T>
	class CTask_queue<ELEM_T,MULTI>
	{
		
	private:
		volatile u32	m_rdindex;			//可读索引，总是指向当前可读的位置
		volatile u32	m_wrindex;			//可写索引，总是指向下一个可写的位置
		volatile u32	m_max_rdindex;		//最大可读位置，用于保证多线程安全性

		s32				m_size;				//队列的大小
		s32				m_cursize;			//当前任务个数
		ELEM_T			*mp_elems;			//任务队列
	public:
	
		CTask_queue()
		{
			m_rdindex = m_wrindex = m_max_rdindex = 0;
			m_cursize = m_size = 0;
			mp_elems = NULL;
		}

		~CTask_queue()
		{
			if (mp_elems)
				delete [] mp_elems;
		}

		/*********************************************************
		 * @brief: 函数功能简介
		 * @function_name: queue_init
		 * @description: 任务队列初始化，队列总大小为size
		 * @param[in]: size，s32类型，队列的大小
		 * @param[out]: 无
		 * @return: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(0)
		**********************************************************/
		s32 queue_init(s32 size)
		{
			m_size = size;
			mp_elems = new ELEM_T[size];
			ASSERT(mp_elems != NULL);
			memset(mp_elems, 0, sizeof(ELEM_T) * m_size);
			
			return SUCCESS;
		}
		

		/*********************************************************
		 * @brief: 函数功能简介
		 * @function_name: queue_resize
		 * @description: 任务队列扩容，总的大小为原来的大小加上现在要扩容的大小
		 * @param[in]: size，s32类型，扩容的大小
		 * @param[out]: 无
		 * @return: s32类型，成功返回SUCCESS(0)，失败返回FAILURE(0)
		**********************************************************/
		s32 queue_resize(s32 size)
		{
			s32 ori_size = m_size;
			
			m_size += size;
			ELEM_T* tmp = new ELEM_T[m_size];
			//MYASSERT(tmp != NULL);
			
			memset(tmp, 0, sizeof(ELEM_T) * m_size);
			memcpy(tmp, mp_elems, ori_size * sizeof(ELEM_T));
			delete [] mp_elems;
			
			mp_elems = tmp;
			
			return SUCCESS;
		}
		

		inline bool is_empty(u32 max_rdindex, u32 rdindex) 
		{
			return (rdindex % m_size) == (max_rdindex %m_size); 
		}

		inline bool is_full(u32 wrindex, u32 rdindex) 
		{
			return ((wrindex + 1) % m_size) == (rdindex % m_size); 
		}
		
		/*********************************************************
		 * @brief: 函数功能简介
		 * @function_name: enqueue
		 * @description: 任务入队
		 * @param[in]: ele，ELEM_T类型引用，添加到任务队列的任务
		 * @param[out]: 无
		 * @return: bool类型，成功返回true，失败返回false
		**********************************************************/
		
		inline bool dequeue(ELEM_T &ele);
		
		/*********************************************************
		 * @brief: 函数功能简介
		 * @function_name: dequeue
		 * @description: 任务出队
		 * @param[in]: ele，ELEM_T类型引用，删除任务队列的任务
		 * @param[out]: 无
		 * @return: bool类型，成功返回true，失败返回false
		**********************************************************/	
		inline bool enqueue(ELEM_T &ele);
	
		
		
	private:

	};

	/************************************************
		enqueue方法重载，将整形值SINGLE转换成类型
	************************************************/
	
	template <typename ELEM_T>
	bool CTask_queue<ELEM_T,SINGLE>::enqueue(ELEM_T &ele)
	{
		u32 cur_rdindex = 0;
		u32 cur_wrindex = 0;

		cur_wrindex = m_wrindex;
		cur_rdindex = m_rdindex;

		if (is_full(cur_wrindex, cur_rdindex))
		{
			return false;
		}
		
		mp_elems[cur_wrindex % m_size] = ele;
		ATOMIC_ADD(&m_wrindex, 1);

		return true;
	}

	/************************************************
		enqueue方法重载，将整形值MULTI转换成类型
	************************************************/
	template <typename ELEM_T>
	bool CTask_queue<ELEM_T,MULTI>::enqueue(ELEM_T &ele)
	{
		u32 cur_rdindex = 0;
		u32 cur_wrindex = 0;
		
		do
		{
			cur_wrindex = m_wrindex;
			cur_rdindex = m_rdindex;
			
			if (is_full(cur_wrindex, cur_rdindex))
			{
				return false;
			}
			
		} while (!CAS(&m_wrindex, cur_wrindex, (cur_wrindex + 1)));
		
		mp_elems[cur_wrindex % m_size] = ele;
		
		while (!CAS(&m_max_rdindex, cur_wrindex, (cur_wrindex + 1)))
		{
			sched_yield();
		}
		
		return true;
	}

	/************************************************
		dequeue方法重载，将整形值SINGLE转换成类型
	************************************************/
	template <typename ELEM_T>
	bool CTask_queue<ELEM_T,MULTI>::dequeue(ELEM_T &ele)
	{
		u32 cur_rdindex = 0;
		u32 cur_max_rdindex = 0;
		
		do
		{
			cur_rdindex = m_rdindex;
			cur_max_rdindex = m_wrindex;
		
			if (is_empty(cur_max_rdindex, cur_rdindex))
			{
				return false;
			}
		
			ele = mp_elems[cur_rdindex % m_size];
		
			if (CAS(&m_rdindex, cur_rdindex, (cur_rdindex + 1)))
			{
				return true;
			}
		} while(1);

		//ASSERT(false);

		return false;
	}

	template <typename ELEM_T>
	bool CTask_queue<ELEM_T,SINGLE>::dequeue(ELEM_T &ele)
	{
		u32 cur_rdindex = 0;
		u32 cur_max_rdindex = 0;
		
		do
		{
			cur_rdindex = m_rdindex;
			cur_max_rdindex = m_wrindex;
		
			if (is_empty(cur_max_rdindex, cur_rdindex))
			{
				return false;
			}
		
			ele = mp_elems[cur_rdindex % m_size];
		
			if (CAS(&m_rdindex, cur_rdindex, (cur_rdindex + 1)))
			{
				return true;
			}
		} while(1);

		//ASSERT(false);

		return false;
	}

	/************************************************
		dequeue方法重载，将整形值MULTI转换成类型
	************************************************/
/*
	template<typename ELEM_T>
	bool CTask_queue<ELEM_T,MULTI>::dequeue(ELEM_T &ele)
	{
		u32 cur_rdindex = 0;
		u32 cur_max_rdindex = 0;
		
		do
		{
			cur_rdindex = m_rdindex;
			cur_max_rdindex = m_max_rdindex;
		
			if (is_empty(cur_max_rdindex, cur_rdindex))
			{
				return false;
			}
		
			ele = mp_elems[cur_rdindex%m_size];
		
			if (CAS(&m_rdindex, cur_rdindex, (cur_rdindex + 1)))
			{
				return true;
			}
		} while(1);
		
		ASSERT(false);
		
		return false;
	}
*/
};








#endif /* _TASK_QUEUE_H_ */


