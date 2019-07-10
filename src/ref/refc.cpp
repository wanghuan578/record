#include "refc.h"
#include "atomic/atom_opr.h"

using namespace p_base;

s32 refc::add_ref()
{

	return ATOMIC_ADD(&m_refc, 1);
}

s32 refc::release()
{
	s32 ret = ATOMIC_SUB(&m_refc,1);

	if(ret == 0)
	{
		delete this;
		return ret;
	}
}

