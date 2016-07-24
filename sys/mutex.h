#include <pthread.h>
#include <sys/time.h>
#include <errno.h>

class CMutex
{
	friend class CCond;
public:
	// Ĭ�Ϲ���һ���ǵݹ黥����
	CMutex(bool recursive = false);
	virtual ~CMutex();
	
	// ����
	void lock();
	// ����
	void unlock();
	// ���Լ���,�����ɹ�����true,����false
	bool try_lock();
	// �����ָ���ĺ���ʱ���ڼ����ɹ��򷵻�true,���򷵻�false
	bool timed_lock(unsigned int millisecond);
	
private:
	pthread_mutex_t mutex_;
	pthread_mutexattr_t attr_;
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