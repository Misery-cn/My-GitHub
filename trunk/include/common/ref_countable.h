#ifndef _REF_COUNTABLE_H_
#define _REF_COUNTABLE_H_

#include "atomic.h"

// ���ü�������
class CRefCountable
{
public:
	CRefCountable() { atomic_set(&refcount_, 0); }

	virtual ~CRefCountable() {}

	// ��ȡ��ǰ������
	int get_refcount() const { return atomic_read(&refcount_); }

	// +1
	void inc_refcount() { atomic_inc(&refcount_); }

	// -1
	bool dec_refcount()
	{
		volatile bool deleted = false;
		// ��һ�������0����true
		if (atomic_dec_and_test(&refcount_))
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
	atomic_t refcount_;
};

#endif
