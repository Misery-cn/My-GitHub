#include "thread.h"


// SYS_NS_BEGIN


CThread::CThread() throw (CException, CSysCallException)
	// ����һ���ݹ���
    : _lock(true), _stop(false), _state(state_sleeping), _stack_size(0)
{
    int r = pthread_attr_init(&_attr);
    if (0 != r)
    {
        // RUNLOG or export exception
        THROW_SYSCALL_EXCEPTION(NULL, r, "pthread_attr_init");
    }
}

CThread::~CThread() throw ()
{
	pthread_attr_destroy(&_attr);
}

void* CThread::thread_proc(void* thread_param)
{
    CThread* thread = (CThread *)thread_param;
	// go!
    thread->run();
	
    return NULL;
}

uint32_t CThread::get_current_thread_id() throw ()
{
    return pthread_self();
}

void CThread::start(bool detach) throw (CException, CSysCallException)
{
    before_start();

    int r = 0;

    // �����߳�ջ��С
    if (0 < _stack_size)
	{
		r = pthread_attr_setstacksize(&_attr, _stack_size);
		if (0 != r)
		{
			THROW_SYSCALL_EXCEPTION(NULL, r, "pthread_attr_setstacksize");
		}
	}
        
	// �����߳����з�ʽ
	r = pthread_attr_setdetachstate(&_attr, detach ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE);
	if (0 != r)
	{
		THROW_SYSCALL_EXCEPTION(NULL, r, "pthread_attr_setdetachstate");
	}
       
	// �����߳�
    r = pthread_create(&_thread, &_attr, thread_proc, this);
	if (0 != r)
	{
		THROW_SYSCALL_EXCEPTION(NULL, r, "pthread_create");
	}
}

size_t CThread::get_stack_size() const throw (CSysCallException)
{
    size_t stack_size = 0;
    int r = pthread_attr_getstacksize(&_attr, &stack_size);
    if (0 != r)
	{
		// RUNLOG or export exception
		THROW_SYSCALL_EXCEPTION(NULL, r, "pthread_attr_getstacksize");
	}

    return stack_size;
}

void CThread::join() throw (CSysCallException)
{
    // �߳��Լ����ܵ���join
    if (CThread::get_current_thread_id() != this->get_thread_id())
    {
    	if (0 < _thread)
		{
			int r = pthread_join(_thread, NULL);
	        if (0 != r)
			{
				// RUNLOG or export exception
				THROW_SYSCALL_EXCEPTION(NULL, r, "pthread_join");
			}
		}
    }
}

void CThread::detach() throw (CSysCallException)
{
    int r = pthread_detach(_thread);
    if (0 != r)
	{
		// RUNLOG or export exception
		THROW_SYSCALL_EXCEPTION(NULL, r, "pthread_detach");
	}
}

bool CThread::can_join() const throw (CSysCallException)
{
    int state;
    int r = pthread_attr_getdetachstate(&_attr, &state);
    if (0 != r)
	{
		// RUNLOG or export exception
		THROW_SYSCALL_EXCEPTION(NULL, r, "pthread_attr_getdetachstate");
	}

    return (PTHREAD_CREATE_JOINABLE == state);
}

bool CThread::is_stop() const
{
    return _stop;
}

void CThread::do_wakeup(bool stop)
{   
    // �߳���ֹ��ʶ
    if (stop)
	{
		_stop = stop;
	}
    
    // ��֤�ڻ����߳�֮ǰ���Ѿ�������״̬�޸�Ϊstate_wakeup
    if (state_sleeping == _state)
    {
        _state = state_wakeuped;
        _cond.signal();  
    }
    else
    {
        _state = state_wakeuped;
    }
}

void CThread::wakeup()
{
    CMutexGuard g(_lock);
    do_wakeup(false);
}

void CThread::stop(bool wait_stop) throw (CException, CSysCallException)
{
    if (!_stop)
    {
        _stop = true;
        before_stop();
        CMutexGuard g(_lock);
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
        CMutexGuard g(_lock);
        if (!is_stop())
        {
            if (_state != state_wakeuped)
            {        
                _state = state_sleeping;
                if (0 > milliseconds)
				{
                    _cond.wait(_lock);
				}
                else
				{
					_cond.timed_wait(_lock, milliseconds); 
				}               
            }

            // ������Ϊstate_wakeup����֤�ٴζ����Ե���do_millisleep
            _state = state_running;
        }
    }
}

// SYS_NS_END
