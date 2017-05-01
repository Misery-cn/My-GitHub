#ifndef _SYS_COND_H_
#define _SYS_COND_H_

#include "mutex.h"

// SYS_NS_BEGIN

class Cond
{
public:
    Cond() throw (SysCallException);

	// �����׳��쳣
    ~Cond() throw ();

	// �̹߳���,�ȴ�������
    void wait(Mutex& lock) throw (SysCallException);

	// �̹߳���,�����ѻ��߳�ʱ
    bool timed_wait(Mutex& lock, uint32_t millisecond) throw (SysCallException);

	// ���ѹ�����߳�
    void signal() throw (SysCallException);

	// �������еĹ����߳�
    void broadcast() throw (SysCallException);
    
private:
	pthread_condattr_t _attr;
    pthread_cond_t _cond;
};

// SYS_NS_END

#endif