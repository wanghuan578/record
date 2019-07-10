#ifndef _T_DLIST_H_
#define _T_DLIST_H_

#include "sys_headers.h"
#include "basic_type.h"

namespace pure_baselib
{

template <class T> class TDList;
template <class S>
class TDList_node
{
    friend class TDList <S>;

    public:
        TDList_node( S &element )
        {
			// the node takes ownership of "element"
			m_element = element;
			m_next = NULL;
			m_prev = NULL;
        }
		
        virtual ~TDList_node()
        {
			#if _TDLIST_DEBUG_
//			Assert( m_prev == NULL && m_next == NULL );
			#endif

		}
        
        S m_element;

    protected:
        TDList_node *m_next;
        TDList_node *m_prev;
};


template <class T> class TDList
{
    
    public:
        TDList()
        {
            m_head = NULL;
            m_tail = NULL;
            m_num_nodes = 0;
        }
        
        virtual ~TDList()
		{
			clear_list();
		}
        
        #if _TDLIST_DEBUG_
        
        void	validate_links()
		{
			TDList_node<T> * nextnode;
			
			//Assert( m_head == NULL || m_head->m_prev == NULL );
			//Assert( m_tail == NULL || m_tail->m_next == NULL );
			
			
			if ( m_tail == m_head && m_tail != NULL )
			{
				//Assert(  m_tail->m_prev == NULL && m_tail->m_next == NULL );
			}
			
			if ( m_head  )
			{
				//Assert(  m_tail != NULL  );
			}

			if ( m_tail  )
			{
				//Assert(  m_head != NULL  );
			}

			
			if ( m_tail && m_tail->m_prev )
			{
				//Assert( m_tail->m_prev->m_next == m_tail  );
			}

			if ( m_head && m_head->m_next )
			{
				//Assert( m_head->m_next->m_prev == m_head  );
			}
			
				
			nextnode = m_head;
			
			while ( nextnode )
			{   
				//Assert( m_head == nextnode || nextnode->m_prev->m_next == nextnode );
				//Assert( m_tail == nextnode || nextnode->m_next->m_prev == nextnode );
				
				if ( !nextnode->m_next )
				{
					//Assert( m_tail == nextnode );
				}
				
				nextnode = nextnode->m_next;
			}

			nextnode = m_tail;
			
			while ( nextnode )
			{   
				//Assert( m_head == nextnode || nextnode->m_prev->m_next == nextnode );
				//Assert( m_tail == nextnode || nextnode->m_next->m_prev == nextnode );
				
				if ( !nextnode->m_prev )
				{
						//Assert( m_head == nextnode );
				}
				
				nextnode = nextnode->m_prev;
			}
		}
        #endif // _TDLIST_DEBUG_
        
        TDList_node<T>   * get_first() { return m_head; };
        
        void add_node_to_tail(TDList_node<T> *node)
        {
			#if _TDLIST_DEBUG_
			// must not be associated with another list
			//Assert( node->m_prev == NULL && node->m_next == NULL );
			#endif
			
			
			if ( m_tail )
				m_tail->m_next = node;

			
			node->m_prev = m_tail;
			node->m_next = NULL;
			
			m_tail = node;
				
			if ( !m_head )
				m_head = node;

			m_num_nodes++;
			
			#if _TDLIST_DEBUG_
			validate_links();
			#endif
		}       

		void add_node(TDList_node<T> *node )
		{
			#if _TDLIST_DEBUG_
			// must not be associated with another list
			//Assert( node->m_frev == NULL && node->m_next == NULL );
			#endif
			
			if ( m_head )
			{
				m_head->m_prev = node;
			}
			
			node->m_prev = NULL;
			node->m_next = m_head;
			
			m_head = node;
				
			if ( !m_tail )
				m_tail = node;

			m_num_nodes++;
			
			#if __PLDoubleLinkedListDEBUG__
			validate_links();
			#endif
			
		}

		void remove_node(TDList_node<T> *node)
		{
			
			#if _TDLIST_DEBUG_
			// must be associated with this list    
			//Assert( m_head == node || node->m_prev->m_next == node );
			
			// must be associated with this list
			//Assert( m_tail == node || node->m_next->m_prev == node );
			#endif
			
			
			if ( m_head == node)
				m_head = node->m_next;
			else
				node->m_prev->m_next = node->m_next;

			if ( m_tail == node)
				m_tail = node->m_prev;
			else
				node->m_next->m_prev = node->m_prev;

			
			node->m_prev = NULL;
			node->m_next = NULL;

			m_num_nodes--;
			
			#if _TDLIST_DEBUG_
			validate_links();
			#endif
			
		}
                                
        TDList_node<T>   *for_each_until( bool (*do_func)( TDList_node<T> *node,  void * userdata), void *userdata )
		{
			TDList_node<T> *next_element, *cur_element;
			bool stop_iteration = false;
			
			cur_element = m_head;
			
			while ( cur_element && !stop_iteration )
			{
				next_element = cur_element->m_next;
			
				stop_iteration = (*do_func)( cur_element, userdata);

				if ( !stop_iteration )
					cur_element = next_element;
			}

			return cur_element;
		}

        void for_each( void (*do_func)( TDList_node<T> *node,  void *userdata), void *userdata )
		{
			TDList_node<T> *next_element, *cur_element;
			
			cur_element = m_head;
			
			while ( cur_element )
			{
				next_element = cur_element->m_next;    
			
				(*do_func)( cur_element, userdata);
				
				cur_element = next_element;
			}

		}


        void clear_list()
		{
			for_each( do_clear_list, this );
		}


        TDList_node<T>   *get_nth_node( s32 node_index )
		{
            return for_each_until( compare_index_to_zero, &node_index );
		}
		
        u32 get_num_nodes() { return m_num_nodes; }
        
    protected:
        static bool compare_index_to_zero( TDList_node<T> * node, void * node_index ) // (node, nodeIndex)
		{
			int val = *(int*)node_index;
			
			if ( val == 0  )
				return true;
			
			*(int*)node_index = val -1;
			
			return false;
		}       
                        
        static void do_clear_list( TDList_node<T> *node, void * list_ptr )
		{
			TDList<T> *list = (TDList<T> *)list_ptr;
			
			list->remove_node( node );
			
			delete node;
			
		}
		
        TDList_node<T> *m_head;
        TDList_node<T> *m_tail;
        u32	m_num_nodes;
        
    

};

}

#endif
