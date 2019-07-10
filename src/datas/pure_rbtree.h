/*
* Copyright (c) 2012,surfing
* All rights reserved.
*
* 文件名称：pure_rbtree.h
* 摘要：简要描述本文件的作用和内容等
*/

/*
*版本:1.0.1
*作者：潘兴浩，完成日期：2012年11月14日
*内容：调整对外接口函数和函数结构
*
*/

/*
* 版本：1.0.0
* 作者：潘兴浩，完成日期：2012年11月9日
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
		/*删除节点时，调用的DEL*/
		inline virtual void rb_del_element(void* data);
		
		/*销毁树时，调用free*/
		inline virtual void free_element(void * data);

		
	
		void rb_insert_fixup(rb_node * node);

		rb_node * rb_search(ul64 i_key);

			
		
		/*禁止拷贝构造函数，赋值函数*/
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