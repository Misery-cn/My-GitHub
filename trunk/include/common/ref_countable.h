#ifndef _REF_COUNTABLE_H_
#define _REF_COUNTABLE_H_

#include "atomic.h"

// ���ü�������
class CRefCountable
{
public:
	CRefCountable() { atomic_set(&_ref, 0); }

	virtual ~CRefCountable() {}

	// ��ȡ��ǰ������
	int get_ref() const { return atomic_read(&_ref); }

	// +1
	void inc() { atomic_inc(&_ref); }

	// -1
	bool dec()
	{
		volatile bool deleted = false;
		// ��һ�������0����true
		if (atomic_dec_and_test(&_ref))
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
		inc();
		
		return this;
	}
	
private:
	atomic_t _ref;
};

#endif
