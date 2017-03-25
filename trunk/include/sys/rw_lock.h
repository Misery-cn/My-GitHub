#ifndef _SYS_RW_LOCK_H_
#define _SYS_RW_LOCK_H_

#include <pthread.h>
#include "define.h"
#include "exception.h"

// SYS_NS_BEGIN

class CRWLock
{
public:
	CRWLock() throw (CSysCallException);
	virtual ~CRWLock() throw ();

	// ����
	void unlock() throw (CSysCallException);
	// ����
	void lock_read() throw (CSysCallException);
	// д��
	void lock_write() throw (CSysCallException);
	// ���Զ���
	bool try_lock_read() throw (CSysCallException);
	// ����д��
	bool try_lock_write() throw (CSysCallException);

	bool timed_lock_read(uint32_t millisecond) throw (CSysCallException);

	bool timed_lock_write(uint32_t millisecond) throw (CSysCallException);
	
private:
	pthread_rwlockattr_t _attr;
	pthread_rwlock_t _rwlock;
};

class CRWLockGuard
{
public:
	// Ĭ�ϼӶ���
    CRWLockGuard(CRWLock& lock, bool is_read = true) : _lock(lock)
    {
    	if (is_read)
		{
        	_lock.lock_read();
    	}
		else
		{
			_lock.lock_write();
		}
    }    
    
    ~CRWLockGuard()
    {
        _lock.unlock();
    }
    
private:
    CRWLock& _lock;
};

// SYS_NS_END

#endif