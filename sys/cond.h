#include "mutex.h"

class CCond
{
public:
    CCond();
    ~CCond();

	// �̹߳���,�ȴ�������
    void wait(CMutex& lock);

	// �̹߳���,�����ѻ��߳�ʱ
    bool timed_wait(CMutex& lock, unsigned int millisecond);

	// ���ѹ�����߳�
    void signal();

	// �������еĹ����߳�
    void broadcast();
    
private:
    pthread_cond_t cond_;
};