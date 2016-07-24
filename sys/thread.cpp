#include "thread.h"

CThread::CThread()
	// ����һ���ݹ���
    : lock_(true), stop_(false), state_(state_sleeping), stack_size_(0)
{
    int r = pthread_attr_init(&attr_);
    if (0 != r)
    {
        // RUNLOG or export exception
    }
}

CThread::~CThread()
{
	pthread_attr_destroy(&attr_);
}

void* CThread::thread_proc(void* thread_param)
{
    CThread* thread = (CThread *)thread_param;
	// go!
    thread->run();
	
    return NULL;
}

unsigned int CThread::get_current_thread_id()
{
    return pthread_self();
}

void CThread::start(bool detach)
{
    if (!before_start())
	{
		// RUNLOG or export exception
	}

    int r = 0;

    // �����߳�ջ��С
    if (0 < stack_size_)
	{
		r = pthread_attr_setstacksize(&attr_, stack_size_);
	}
        
    if (0 == r)
	{
		// �����߳����з�ʽ
		r = pthread_attr_setdetachstate(&attr_, detach ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE);
	}
       
    if (0 == r)
	{
		// �����߳�
        r = pthread_create(&thread_, &attr_, thread_proc, this);
	}

    if (r != 0)
    {
		// RUNLOG or export exception
    }
}

size_t CThread::get_stack_size() const
{
    size_t stack_size = 0;
    int r = pthread_attr_getstacksize(&attr_, &stack_size);
    if (0 != r)
	{
		// RUNLOG or export exception
	}

    return stack_size;
}

void CThread::join()
{
    // �߳��Լ����ܵ���join
    if (CThread::get_current_thread_id() != this->get_thread_id())
    {    
        int r = pthread_join(thread_, NULL);
        if (0 != r)
		{
			// RUNLOG or export exception
		}
    }
}

void CThread::detach()
{
    int r = pthread_detach(thread_);
    if (0 != r)
	{
		// RUNLOG or export exception
	}
}

bool CThread::can_join() const
{
    int state;
    int r = pthread_attr_getdetachstate(&attr_, &state);
    if (0 != r)
	{
		// RUNLOG or export exception
	}

    return (PTHREAD_CREATE_JOINABLE == state);
}

bool CThread::is_stop() const
{
    return stop_;
}

void CThread::do_wakeup(bool stop)
{   
    // �߳���ֹ��ʶ
    if (stop)
	{
		stop_ = stop;
	}
    
    // ��֤�ڻ����߳�֮ǰ���Ѿ�������״̬�޸�Ϊstate_wakeup
    if (state_sleeping == state_)
    {
        state_ = state_wakeuped;
        cond_.signal();  
    }
    else
    {
        state_ = state_wakeuped;
    }
}

void CThread::wakeup()
{
    CMutexGuard g(lock_);
    do_wakeup(false);
}

void CThread::stop(bool wait_stop)
{
    if (!stop_)
    {
        stop_ = true;
        before_stop();
        CMutexGuard g(lock_);
        do_wakeup(true);
    }
	
    if (wait_stop && can_join())
    {
        join();
    }
}

void CThread::do_sleep(int milliseconds)
{
    // �Ǳ��̵߳�����Ч
    if (this->get_thread_id() == CThread::get_current_thread_id())
    {    
        CMutexGuard g(lock_);
        if (!is_stop())
        {
            if (state_ != state_wakeuped)
            {        
                state_ = state_sleeping;
                if (0 > milliseconds)
				{
                    cond_.wait(lock_);
				}
                else
				{
					cond_.timed_wait(lock_, milliseconds); 
				}               
            }

            // ������Ϊstate_wakeup����֤�ٴζ����Ե���do_millisleep
            state_ = state_running;
        }
    }
}
