#ifndef _SYS_COND_H_
#define _SYS_COND_H_

#include "mutex.h"

// SYS_NS_BEGIN

class CCond
{
public:
    CCond() throw (CSysCallException);

	// �����׳��쳣
    ~CCond() throw ();

	// �̹߳���,�ȴ�������
    void wait(CMutex& lock) throw (CSysCallException);

	// �̹߳���,�����ѻ��߳�ʱ
    bool timed_wait(CMutex& lock, uint32_t millisecond) throw (CSysCallException);

	// ���ѹ�����߳�
    void signal() throw (CSysCallException);

	// �������еĹ����߳�
    void broadcast() throw (CSysCallException);
    
private:
	pthread_condattr_t _attr;
    pthread_cond_t _cond;
};

// SYS_NS_END

#endif