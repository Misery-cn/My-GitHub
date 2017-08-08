#ifndef _REF_COUNTABLE_H_
#define _REF_COUNTABLE_H_

#include "atomic.h"

// ���ü�������
class RefCountable
{
public:
	// ��ʼ��ʱ���ü�����Ϊ1
	RefCountable() { atomic_set(&_ref, 1); }

	virtual ~RefCountable() {}

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
	RefCountable* get()
	{
		// +1
		inc();
		
		return this;
	}
	
private:
	atomic_t _ref;
};

#endif
