#include <pthread.h>
#include "cond.h"

class CThread
{
public:
	// �߳�ִ�к���
	static void* thread_proc(void* thread_param);
    // ��ȡ��ǰ�̺߳�
    static unsigned int get_current_thread_id();

public:
	CThread();
	virtual ~CThread();
	
	// ��������ʵ��
	// ����ֱ�ӵ���run,����ֱ�ӵ���,Ӧ�õ���start�����߳�
	virtual void run() = 0;
    
	// �߳�����ǰ�Ĳ���
	// ��������ʵ��
    virtual bool before_start() { return true; }

	// �߳̽���ǰ�Ĳ���
	// ��������ʵ��
    virtual void before_stop() {}

    // ��stop_��Ա����Ϊtrue���߳̿��Ը���_stop״̬�������Ƿ��˳��߳�      
    // wait_stop: �Ƿ�ȴ��߳̽�����ֻ�е��߳��ǿ�Joinʱ����Ч
    virtual void stop(bool wait_stop = true);

	// Ĭ������һ����join���߳�
    // detach: �Ƿ��Կɷ���ģʽ�����߳�
	void start(bool detach = false);

    // �����߳�ջ��С��Ӧ����start֮ǰ���ã�����������Ч�������before_start����
    void set_stack_size(size_t stack_size) { stack_size_ = stack_size; }
    
    // �õ��߳�ջ��С�ֽ���
    size_t get_stack_size() const;

    // �õ����̺߳�
    unsigned int get_thread_id() const { return thread_; }
    
    // �ȴ��̷߳���
    void join();

    // ���߳�����Ϊ�ɷ����
    void detach();

    // �����߳��Ƿ��join
    bool can_join() const;

    // ����߳������ڵȴ�״̬������
    void wakeup();

protected:

	// �ж��߳��Ƿ�Ӧ���˳���Ĭ�Ϸ���stop_��ֵ
    virtual bool is_stop() const;

	// �߳̿��Ե���������˯��״̬�����ҿ���ͨ������do_wakeup����
    void do_sleep(int milliseconds);

private:
	// �����߳�
    void do_wakeup(bool stop);

protected:
	// ������
    CMutex lock_;

private:
	// ��������
    CCond cond_;
	// �Ƿ�ֹͣ�̱߳�ʶ
    volatile bool stop_;
	// �߳�״̬
    volatile enum 
	{
		state_sleeping, 
		state_wakeuped, 
		state_running
	} state_;
	
	// �߳�id
    pthread_t thread_;
    pthread_attr_t attr_;
	// ջ��С
    size_t stack_size_;    
};