/*
* Copyright (c) 2004,surfing
* All rights reserved.
*
* 文件名称：filename.h
* 文件标识：根据软件工程设置
* 摘要：简要描述本文件的作用和内容等
*/

/*
*
*版本：1.0.0
*作者: 潘兴浩，完成日期：2012年11月07日
*
*/

#include "pure_rbtree.h"
#include "string.h"
#include <stdio.h>

using namespace p_base;

#define BLACK 0
#define RED 1


pure_rbtree::pure_rbtree()
{
	nil = new_rbnode();
	memset(nil,0,sizeof(rb_node));
	root = nil;
	root->parent = nil;
	root->rson = nil;
	root->lson = nil;
}


void pure_rbtree::left_rotate(rb_node * x)
{
	rb_node * y = x->rson;
	x->rson = y->lson;

	if( y->lson != nil)
	{
		y->lson->parent = x;
	}
	y->parent = x->parent;

	if(x->parent == nil)
	{
		root = y;	
	}
	else if(x == x->parent->lson)
	{
		x->parent->lson = y;
	}
	else
	{
		x->parent->rson = y;
	}

	y->lson = x;
	x->parent = y;
	
	return;
	
}

void pure_rbtree::right_rotate(rb_node * x)
{
	rb_node * y = x->lson;
	x->lson = y->rson;

	if( y->rson != nil)
	{
		y->rson->parent = x;
	}
	y->parent = x->parent;

	if(x->parent == nil)
	{
		root = y;
	}
	else if(x == x->parent->lson)
	{
		x->parent->lson = y;
	}
	else
	{
		x->parent->rson = y;
	}

	y->rson = x;
	x->parent = y;

	return;
}

s8 pure_rbtree::rb_insert_key_data(ul64 i_key,void * i_data)
{
	rb_node * z = new_rbnode();
	if(z==NULL)
	{
		printf("alloc memory error!\n");
		return -1;
	}
	z->key = i_key;
	z->data = i_data;
	rb_insert(z);
	return 0;
}

void pure_rbtree::rb_insert(rb_node * z)
{
	PDEBUG("this = %x,insert z = %x,nil = %x\n",this,z,nil);
	
	rb_node * y = nil;
	rb_node * x = root;
	while(x != nil)
	{
		y = x;
		if(z->key < x->key)
		{
			x = x->lson;
		}
		else
		{
			x = x->rson;
		}
	}
	
	z->parent = y;

	if(y == nil)
	{
		root = z;
	}
	else if(z->key < y->key)
	{
		y->lson = z;
	}
	else
	{
		y->rson = z;
	}

	z->lson = nil;
	z->rson = nil;
	z->colour = RED;
	rb_insert_fixup(z);
	return;

}


void pure_rbtree::rb_insert_fixup(rb_node * z)
{
	rb_node * y = nil;
	
	while(z->parent->colour == RED)
	{
		if(z->parent == z->parent->parent->lson)
		{
			y = z->parent->parent->rson;
			if(y->colour == RED)
			{
				z->parent->colour = BLACK;
				y->colour = BLACK;
				z->parent->parent->colour = RED;
				z = z->parent->parent;
			}
			else
			{
				if(z == z->parent->rson)
				{
					z = z->parent;
					left_rotate(z);
				}
				z->parent->colour = BLACK;
				z->parent->parent->colour = RED;
				right_rotate(z->parent->parent);
			}
		}
		else
		{
			y = z->parent->parent->lson;
			if(y->colour == RED)
			{
				z->parent->colour = BLACK;
				y->colour = BLACK;
				z->parent->parent->colour = RED;
				z = z->parent->parent;
			}
			else
			{
				if(z == z->parent->lson)
				{
					z = z->parent;
					right_rotate(z);
				}
				z->parent->colour = BLACK;
				z->parent->parent->colour = RED;
				left_rotate(z->parent->parent);
			}
				
		}
		
	}
	root->colour = BLACK;
}

void pure_rbtree::rb_transplant(rb_node * u,rb_node * v)
{
	if(u->parent == nil)
	{
		root = v;
	}
	else if(u == u->parent->lson)
	{
		u->parent->lson = v;
	}
	else
	{
		u->parent->rson = v;
	}
	v->parent = u->parent;
	return;
}

rb_node * pure_rbtree::tree_minimum(rb_node * x)
{
	
	while(x->lson != nil)
	{
		x = x->lson;
	}
	return x;
}

void pure_rbtree::rb_delete(rb_node * z)
{
	PDEBUG("delete z = %x,nil = %x\n",z,nil);
	rb_node * y = z;
	rb_node * x;
	u8 y_orc = y->colour;
	if(z->lson == nil)
	{
		x = z->rson;
		rb_transplant(z,z->rson);
	}
	else if(z->rson == nil)
	{
		x = z->lson;
		rb_transplant(z,z->lson);
	}
	else
	{
		y = tree_minimum(z->rson);
		y_orc = y->colour;
		x = y->rson;
		if(y->parent == z)
		{
			x->parent = y;
		}
		else
		{
			rb_transplant(y,y->rson);
			y->rson = z->rson;
			y->rson->parent = y;
		}
		rb_transplant(z,y);
		y->lson = z->lson;
		y->lson->parent = y;
		y->colour = z->colour;
	}
	
	if(y_orc == BLACK)
	{
		rb_delete_fixup(x);
	}
	
	return;
}

s8 pure_rbtree::rb_delete_key(ul64 i_key,void ** out_data)
{
	rb_node * z = rb_search(i_key);
	if(z != nil)
	{
		*out_data = z->data;
		rb_delete(z);
		rb_del_element(z->data);
		delete_rbnode(z);
		
		return 0;
	}
	else
	{
		return -1;
	}
}

void pure_rbtree::rb_delete_fixup(rb_node * x)
{
	rb_node * w;
	while((x != root) && (x->colour == BLACK))
	{
		if(x == x->parent->lson)
		{
			w = x->parent->rson;
			if(w->colour == RED)
			{
				w->colour = BLACK;
				x->parent->colour = RED;
				left_rotate(x->parent);
				w = x->parent->rson;
			}
			if((w->lson->colour == BLACK) && (w->rson->colour == BLACK))
			{
				w->colour = RED;
				x = x->parent;
			}
			else 
			{
				if(w->rson->colour == BLACK)
				{
					w->lson->colour = BLACK;
					w->colour = RED;
					right_rotate(w);
					w = x->parent->rson;
				}
				w->colour = x->parent->colour;
				x->parent->colour = BLACK;
				w->rson->colour = BLACK;
				left_rotate(x->parent);
				x = root;
			}
		}
		else
		{
			w = x->parent->lson;
			if(w->colour == RED)
			{
				w->colour = BLACK;
				x->parent->colour = RED;
				right_rotate(x->parent);
				w = x->parent->lson;
			}
			if((w->lson->colour == BLACK) && (w->rson->colour == BLACK))
			{
				w->colour = RED;
				x = x->parent;
			}
			else 
			{
				if(w->lson->colour == BLACK)
				{
					w->rson->colour = BLACK;
					w->colour = RED;
					left_rotate(w);
					w = x->parent->lson;
				}
				w->colour = x->parent->colour;
				x->parent->colour = BLACK;
				w->lson->colour = BLACK;
				right_rotate(x->parent);
				x = root;
			}
		}
	}
	x->colour = BLACK;
	return;
}

void pure_rbtree::rb_del_element(void* data)
{

}

pure_rbtree::~pure_rbtree()
{
	rb_destroy();
	delete_rbnode(nil);
}

void pure_rbtree::rb_destroy()
{

	rb_node * x = root;
	rb_node * y;
	while(x != nil)
	{
		y = x->parent;
		if(x->lson != nil)
		{
			x = x->lson;
			continue;
		}
		if(x->rson != nil)
		{
			x = x->rson;
			continue;
		}
		
		if(x == x->parent->lson)
		{
			x->parent->lson = nil;
	
		}
		else
		{
			x->parent->rson = nil;
		
		}
		free_element(x->data);
		delete_rbnode(x);
		x = y;
	}
	return;
}


void pure_rbtree::inorder_rb_traversal(rb_node * x)
{
	//printf("%x,%ld,%d,parent =%x,%ld\n", x,x->key,x->colour,x->parent,x->key);
	traversal_element(x->data);

	if(x->lson != nil)
	{
		inorder_rb_traversal(x->lson);
	}
	
	if(x->rson != nil)
	{
		inorder_rb_traversal(x->rson);
	}
	return;
}

rb_node * pure_rbtree::rb_search(ul64 i_key)
{
	rb_node * x = root;
	while((x != nil)&&(i_key != x->key) )
	{
		if(i_key < x->key)
		{
			x = x->lson;
		}
		else
		{
			x= x->rson;
		}
	}
	return x;
}

void pure_rbtree::rb_traversal()
{
	inorder_rb_traversal(root);
	return;
}

void * pure_rbtree::rb_search_key(ul64 i_key)
{
	rb_node * z = rb_search(i_key);
	if(z != nil)
	{
		return z->data;
	}
	else
	{
		return NULL;
	}
}

ul64 pure_rbtree::tree_minimum_key(void ** out_data)
{
	if(root == nil)
	{
		return 0;
	}
	rb_node * min= tree_minimum(root);
	*out_data = min->data;
	return min->key;
}

void pure_rbtree::traversal_element(void * data)
{
	return;
}

void pure_rbtree::free_element(void * data)
{
	return;
}

rb_node * pure_rbtree::new_rbnode()
{
	return new rb_node;
}

void pure_rbtree::delete_rbnode(rb_node * z)
{
	delete z;
}

rb_node * pure_rbtree::tree_minimum_node()
{
	if(root == nil)
	{
		return nil;
	}
	else
	{
		return tree_minimum(root);
	}
}

/*禁止拷贝构造函数，赋值函数*/
/*
pure_rbtree::pure_rbtree(const pure_rbtree &src)
{

}

pure_rbtree & pure_rbtree::operator=(const pure_rbtree &rval)
{
	
}
*/
