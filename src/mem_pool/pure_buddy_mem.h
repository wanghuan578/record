/*
* Copyright (c) 2012,surfing
* All rights reserved.
*
* �ļ����ƣ�pure_buddy_mem.cpp
* ժҪ��Ӧ�л���㷨���ڴ��������֧�ֶ���̡�
*		��Ϊpthread��
*/

/*
*�汾:1.0.1
*���ߣ����˺ƣ�������ڣ�2012��11��22��
*���ݣ���������֧�֡�
*
*/

/*
* �汾��1.0.0
* ���ߣ����˺ƣ�������ڣ�2012��11��18��
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
			/*˫������*/
			b_block * freelist;
			/*�˴�Сλͼ��*/
			u32 order;	//���С�Ľ���
			u32 pbmap[0];	
		}free_area;

		typedef struct mem_t
		{
			s32 len;	//����������

			s32 remain_len;	//ʣ�೤��
			free_area * free_start;	//free_area����Ԫ�ص�ַ 
			b_block * block_start;	//blocks �׵�ַ
			u32 align_area_size;	//�����free_area��С

		}mem;

	private:
		mem * pm;
		u32 align_block_size;	//�����blockͷ��С
	
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