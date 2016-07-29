
#ifndef _SYS_MUTEX_H_
#define _SYS_MUTEX_H_

#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include "exception.h"
#include "define.h"

namespace sys
{

class CMutex
{
	friend class CCond;
public:
	// Ĭ�Ϲ���һ���ǵݹ黥����
	CMutex(bool recursive = false) throw (sys::CSysCallException);
	virtual ~CMutex() throw ();
	
	// ����
	void lock() throw (sys::CSysCallException);
	// ����
	void unlock() throw (sys::CSysCallException);
	// ���Լ���,�����ɹ�����true,����false
	bool try_lock() throw (CSysCallException);
	// �����ָ���ĺ���ʱ���ڼ����ɹ��򷵻�true,���򷵻�false
	bool timed_lock(uint32_t millisecond) throw (sys::CSysCallException);
	
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

}

#endif