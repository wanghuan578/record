/*
* Copyright (c) 2012,surfing
* All rights reserved.
*
* �ļ����ƣ�pure_rbtree.h
* ժҪ����Ҫ�������ļ������ú����ݵ�
*/

/*
*�汾:1.0.1
*���ߣ����˺ƣ�������ڣ�2012��11��14��
*���ݣ���������ӿں����ͺ����ṹ
*
*/

/*
* �汾��1.0.0
* ���ߣ����˺ƣ�������ڣ�2012��11��9��
*/
#ifndef _PURE_RBTREE_H_
#define _PURE_RBTREE_H_
#include "basic_type.h"
namespace p_base
{
	typedef struct rb_tree_node
	{
		rb_tree_node *parent;
		rb_tree_node *lson;
		rb_tree_node *rson;
		u8 colour;
		ul64 key;
		void * data;
	}rb_node;

	class pure_rbtree
	{
	private:
		//rb_node * nil;	
		//rb_node * root;	
		
		inline virtual rb_node * new_rbnode();
		inline virtual void delete_rbnode(rb_node *);

		void left_rotate(rb_node * node);
		void right_rotate(rb_node * node);
		
		void inorder_rb_traversal(rb_node * x);
		inline virtual void traversal_element(void * data);

		rb_node * tree_minimum(rb_node * x);

		void rb_transplant(rb_node * u,rb_node * v);

		void rb_delete_fixup(rb_node * node);
		/*ɾ���ڵ�ʱ�����õ�DEL*/
		inline virtual void rb_del_element(void* data);
		
		/*������ʱ������free*/
		inline virtual void free_element(void * data);

		
	
		void rb_insert_fixup(rb_node * node);

		rb_node * rb_search(ul64 i_key);

			
		
		/*��ֹ�������캯������ֵ����*/
		pure_rbtree(const pure_rbtree &src);
		pure_rbtree & operator=(const pure_rbtree &rval);

	public:

		rb_node * nil;	
		rb_node * root;
				
		pure_rbtree();
		virtual ~pure_rbtree();
		void rb_destroy();

		s8 rb_insert_key_data(ul64 i_key,void * i_data);

		s8 rb_delete_key(ul64 i_key,void ** o_data);
		
		void rb_insert(rb_node * z);
		void rb_delete(rb_node * z);

		void rb_traversal();
		void * rb_search_key(ul64 i_key);
		
		rb_node * tree_minimum_node();

		ul64 tree_minimum_key(void ** out_data);
	};


}
#endif