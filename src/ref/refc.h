#ifndef _REFC_H_
#define _REFC_H_
#include "basic_type.h"

namespace p_base
{


class refc
{
private:	
	
public:
	volatile u32 m_refc;
	refc(){ m_refc = 1;}
	s32 add_ref();
	s32 release();
	virtual ~refc(){};
protected:

};

}
#endif
