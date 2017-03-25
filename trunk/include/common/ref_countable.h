#ifndef _REF_COUNTABLE_H_
#define _REF_COUNTABLE_H_

#include "atomic.h"

// ���ü�������
class CRefCountable
{
public:
	CRefCountable() { atomic_set(&_refcount, 0); }

	virtual ~CRefCountable() {}

	// ��ȡ��ǰ������
	int get_refcount() const { return atomic_read(&_refcount); }

	// +1
	void inc_refcount() { atomic_inc(&_refcount); }

	// -1
	bool dec_refcount()
	{
		volatile bool deleted = false;
		// ��һ�������0����true
		if (atomic_dec_and_test(&_refcount))
		{
			deleted = true;
			delete this;
		}

		return deleted;
	}

	// ��ȡ��ǰ���ü���ָ��
	CRefCountable* get()
	{
		// +1
		inc_refcount();
		
		return this;
	}
	
private:
	atomic_t _refcount;
};

#endif
