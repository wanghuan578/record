/**************************************************************
* Copyright (c) 2012,surfing
* All rights reserved.
* Filename£ºpure_timer_event.cpp
* Description£º
***************************************************************/
/**************************************************************
* Version£º1.0.0
* Original_Author£ºpxh
* Date£º2012-12-25
***************************************************************/

#include "pure_timer_event.h"
#include "base/base_connect.h"


namespace p_base
{
    CBase_timer_event::~CBase_timer_event()
    {
        if(m_is_inrbtree == true)
        {
            if(m_ptt!=NULL)
            {
                m_ptt->pure_delete_timer(this);
            }
        }
    }

	s32 CBase_timer_event::add_to_timer(ul64 timer,  CPure_timer * ptt)
	{
		PDEBUG("m_is_inrbtree = %d,this =%x \n",m_is_inrbtree,this);
		if(m_is_inrbtree == false)
		{
			m_ev_node.key = timer;
			m_ev_node.data = this;
			m_is_inrbtree = true;
			m_ptt = ptt;
			return SUCCESS;
		}
		else
		{
			return FAILURE;
		}
	//prbt.rb_traversal();
	}
	
	s32 CBase_timer_event::remove_timer()
	{
		PDEBUG("m_is_inrbtree = %d,this =%x \n",m_is_inrbtree,this);
		if(m_is_inrbtree == true)
		{
			m_is_inrbtree = false;
			return SUCCESS;
		}
		else
		{
			return FAILURE;
		}
	}

	s32 CPure_timer_event::handle_expire_timer_ex(CPure_timer * pt)
	{
		if(m_pcon->m_closed == true)
		{
			m_pcon->connect_close();
			
		}
		
		if(handle_expire_timer(pt) == FAILURE)
		{
			m_pcon->connect_close();
			
			
		}
		return SUCCESS;
	}
}
