
#ifndef _SYS_COND_H_
#define _SYS_COND_H_

#include "mutex.h"

namespace sys 
{

class CCond
{
public:
    CCond() throw (sys::CSysCallException);

	// �����׳��쳣
    ~CCond() throw ();

	// �̹߳���,�ȴ�������
    void wait(CMutex& lock) throw (sys::CSysCallException);

	// �̹߳���,�����ѻ��߳�ʱ
    bool timed_wait(CMutex& lock, uint32_t millisecond) throw (sys::CSysCallException);

	// ���ѹ�����߳�
    void signal() throw (sys::CSysCallException);

	// �������еĹ����߳�
    void broadcast() throw (sys::CSysCallException);
    
private:
	pthread_condattr_t attr_;
    pthread_cond_t cond_;
};

}

#endif