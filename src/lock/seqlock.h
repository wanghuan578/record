/**************************************************************
* Copyright (c) 2012,surfing
* All rights reserved.
* Filename：seqlock.h
* Description：顺序锁头文件
***************************************************************/
/**************************************************************
* Version：1.0.0
* Original_Author：pxh
* Date：2012-08-30
* Modified_Author：chengzhip
* Modified_Description：修改了类型名，使用了pure项目的基础数据类型
* Modified_Date: 2013-05-17
***************************************************************/

#ifndef _SEQLOCK_H_
#define _SEQLOCK_H_

#include "basic_type.h"
#include "atom/atom_opr.h"
using namespace pure_baselib;



typedef struct seqlock_t
{
	volatile u32 sequence;
} SEQLOCK_T;


#define cpu_relax() __asm__ ("pause")


/*
#define mb() alternative("lock; addl $0,0(%%esp)", "mfence", X86_FEATURE_XMM2)
#define rmb() alternative("lock; addl $0,0(%%esp)", "lfence", X86_FEATURE_XMM2)
#define wmb() alternative("lock; addl $0,0(%%esp)", "sfence", X86_FEATURE_XMM)
*/

#define mb()         asm volatile("mfence":::"memory")

#define rmb()        asm volatile("lfence":::"memory")

#define wmb()        asm volatile("sfence" ::: "memory")


inline void init_seqlock(struct seqlock_t *pl)
{
    pl->sequence = 0;
}

inline void write_sequnlock(struct seqlock_t *pl)
{
	wmb();
	ATOMIC_FADD(&pl->sequence, 1);
	//spin_unlock(&pl->lock);
}

inline void write_seqlock(struct seqlock_t *pl)
{
	//spin_lock(&sl->lock);
	ATOMIC_FADD(&pl->sequence,1);
	wmb();
}

 inline u32 read_seqbegin(const struct seqlock_t *pl)
{
	u32 ret;
	
repeat:
	ret = pl->sequence;
	rmb();
	if(ret & 1)
	{
		cpu_relax();
		goto repeat;
	}
	
	return ret;
}

inline s32 read_seqretry(struct seqlock_t *pl, u32 start)
{
	rmb();
	
	return(pl->sequence != start);
}




#endif /* _SEQLOCK_H_ */
