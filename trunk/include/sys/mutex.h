#ifndef _SYS_MUTEX_H_
#define _SYS_MUTEX_H_

#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include "exception.h"
#include "define.h"

// SYS_NS_BEGIN

class CMutex
{
	friend class CCond;
public:
	// Ĭ�Ϲ���һ���ݹ黥����
	CMutex(bool recursive = true) throw (CSysCallException);
	virtual ~CMutex() throw ();
	
	// ����
	void lock() throw (CSysCallException);
	// ����
	void unlock() throw (CSysCallException);
	// ���Լ���,�����ɹ�����true,����false
	bool try_lock() throw (CSysCallException);
	// �����ָ���ĺ���ʱ���ڼ����ɹ��򷵻�true,���򷵻�false
	bool timed_lock(uint32_t millisecond) throw (CSysCallException);
	
private:
	pthread_mutexattr_t attr_;
	pthread_mutex_t mutex_;
};

class CMutexGuard
{
public:
	CMutexGuard(CMutex& lock) : lock_(lock)
	{
		lock.lock();
	}
	virtual ~CMutexGuard()
	{
		lock_.unlock();
	}
private:
	CMutex& lock_;
};

// SYS_NS_END

#endif