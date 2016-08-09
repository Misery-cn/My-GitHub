#ifndef _SYS_RW_LOCK_H_
#define _SYS_RW_LOCK_H_

#include <pthread.h>
#include "define.h"
#include "exception.h"

SYS_NS_BEGIN

class CRWLock
{
public:
	CRWLock() throw (sys::CSysCallException);
	virtual ~CRWLock() throw ();

	// ����
	void unlock() throw (sys::CSysCallException);
	// ����
	void lock_read() throw (sys::CSysCallException);
	// д��
	void lock_write() throw (sys::CSysCallException);
	// ���Զ���
	bool try_lock_read() throw (sys::CSysCallException);
	// ����д��
	bool try_lock_write() throw (sys::CSysCallException);

	bool timed_lock_read(uint32_t millisecond) throw (sys::CSysCallException);

	bool timed_lock_write(uint32_t millisecond) throw (sys::CSysCallException);
	
private:
	pthread_rwlockattr_t attr_;
	pthread_rwlock_t rwlock_;
};

class CRWLockGuard
{
public:
	// Ĭ�ϼӶ���
    CRWLockGuard(CRWLock& lock, bool is_read = true) : lock_(lock)
    {
    	if (is_read)
		{
        	lock_.lock_read();
    	}
		else
		{
			lock_.lock_write();
		}
    }    
    
    ~CRWLockGuard()
    {
        lock_.unlock();
    }
    
private:
    CRWLock& lock_;
};

SYS_NS_END

#endif