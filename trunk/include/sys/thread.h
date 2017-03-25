
#ifndef _SYS_THREAD_H_
#define _SYS_THREAD_H_

#include <pthread.h>
#include "cond.h"

// SYS_NS_BEGIN

class CThread
{
public:
    // ��ȡ��ǰ�̺߳�
    static uint32_t get_current_thread_id() throw ();

public:
	CThread() throw (CException, CSysCallException);
	virtual ~CThread() throw ();
	
	// ��������ʵ��
	// ����ֱ�ӵ���run,����ֱ�ӵ���,Ӧ�õ���start�����߳�
	virtual void run() = 0;

    // ��stop_��Ա����Ϊtrue���߳̿��Ը���_stop״̬�������Ƿ��˳��߳�      
    // wait_stop: �Ƿ�ȴ��߳̽�����ֻ�е��߳��ǿ�Joinʱ����Ч
    virtual void stop(bool wait_stop = true) throw (CException, CSysCallException);

	// Ĭ������һ����join���߳�
    // detach: �Ƿ��Կɷ���ģʽ�����߳�
	void start(bool detach = false) throw (CException, CSysCallException);

    // �����߳�ջ��С��Ӧ����start֮ǰ���ã�����������Ч�������before_start����
    void set_stack_size(uint32_t stack_size) { _stack_size = stack_size; }
    
    // �õ��߳�ջ��С�ֽ���
    size_t get_stack_size() const throw (CSysCallException);

    // �õ����̺߳�
    uint32_t get_thread_id() const { return _thread; }
    
    // �ȴ��̷߳���
    void join() throw (CSysCallException);

    // ���߳�����Ϊ�ɷ����
    void detach() throw (CSysCallException);

    // �����߳��Ƿ��join
    bool can_join() const throw (CSysCallException);

    // ����߳������ڵȴ�״̬������
    void wakeup();

	// ��ֹ
	int kill(int signal);
	
	// ��cpu��
	int set_affinity(int cpuid);

	// �Ƿ�������
	bool is_started() const { return 0 != _thread; }
	
	// �Ƿ��ǵ�ǰ�߳�
	bool am_self() const { return pthread_self() == _thread; }
	
protected:

	// �ж��߳��Ƿ�Ӧ���˳���Ĭ�Ϸ���stop_��ֵ
    virtual bool is_stop() const;

	// �߳̿��Ե���������˯��״̬�����ҿ���ͨ������do_wakeup����
    void do_sleep(int milliseconds);

private:
	// �߳�ִ�к���
	static void* thread_proc(void* thread_param);
	
	// �����߳�
    void do_wakeup(bool stop);
	
	// �߳�����ǰ�Ĳ���
	// ��������ʵ��
    virtual void before_start() {}

	// �߳̽���ǰ�Ĳ���
	// ��������ʵ��
    virtual void before_stop() {}

protected:
	// ������
    CMutex _lock;

private:
	// ��������
    CCond _cond;
	// �Ƿ�ֹͣ�̱߳�ʶ
    volatile bool _stop;
	// �߳�״̬
    volatile enum 
	{
		state_sleeping, 
		state_wakeuped, 
		state_running
	} _state;
	
	// �߳�id
    pthread_t _thread;
    pthread_attr_t _attr;
	// ջ��С
    uint32_t _stack_size;    
};

// SYS_NS_END

#endif