#ifndef _SYS_THREAD_POOL_H_
#define _SYS_THREAD_POOL_H_

#include "thread.h"

// SYS_NS_BEGIN


// ģ��ʵ��,���û��ػ��Լ����߳�
template <class ThreadClass>
class CThreadPool
{
public:
    // ���캯��
    CThreadPool() : _next_thread(0), _thread_count(0), _thread_array(NULL)
    {
		
    }

    // �����̳߳�,�������̳߳��е������̣߳�
	// ���̴߳����ɹ���,������������������״̬,���Ǵ��ڵȴ�״̬,��Ҫ��������
	// thread_count:�̳߳��е��̸߳���
	// parameter:���ݸ����̵߳Ĳ���
    void create(unsigned int thread_count, void* parameter = NULL)
    {
        _thread_array = new ThreadClass*[thread_count];
        for (unsigned int i = 0; i < thread_count; ++i)
        {
            _thread_array[i] = new ThreadClass;
            _thread_array[i]->set_index(i);
            _thread_array[i]->set_parameter(parameter);
        }
        for (unsigned int i = 0; i < thread_count; ++i)
        {
            try
            {                
                _thread_array[i]->start();
                ++_thread_count;
            }
            catch (...)
            {
                destroy();
                throw;
            }
        }
    }

    // �����̳߳�,�����ȴ������߳���,Ȼ��ɾ���߳�
    void destroy()
    {
        if (NULL != _thread_array)
        {
            unsigned int thread_count = _thread_count;
            for (unsigned int i = thread_count; 0 < i; --i)
            {
                _thread_array[i-1]->stop();
                --_thread_count;
            }

            delete []_thread_array;
            _thread_array = NULL;
        }
    }

	// �����̳߳أ������е������̻߳��ѣ�
	// Ҳ���Ե������ø����̵߳�wakeup�����ǻ���
    void activate()
    {
        for (unsigned int i = 0; i < _thread_count; ++i)
		{
			_thread_array[i]->wakeup();
		}
    }

    // ��ȡ�̸߳���
    unsigned int get_thread_count() const { return _thread_count; }

    // �õ��̳߳��е��߳�����
    ThreadClass** get_thread_array() { return _thread_array; }
    ThreadClass** get_thread_array() const { return _thread_array; }

    // �����̱߳��,�õ���Ӧ���߳�
    ThreadClass* get_thread(unsigned int thread_index)
    {
        if (0 == _thread_count)
		{
			return NULL;
		}
		
        if (thread_index > _thread_count)
		{
			return NULL;
		}
		
        return _thread_array[thread_index];
    }
    
    // �����̱߳��,�õ���Ӧ���߳�
    ThreadClass* get_thread(unsigned int thread_index) const
    {
        if (0 == _thread_count)
		{
			return NULL;
		}
		
        if (thread_index > _thread_count)
		{
			return NULL;
		}
		
        return _thread_array[thread_index];
    }
    
	// �õ�ָ���¸��̵߳�ָ��,�ӵ�һ����ʼѭ������,���ս��,���ﵽ���һ��ʱ,��ָ���һ��
	// ��ҪӦ���ڲ�����ѯ��ʽ��һ�������������������߳�
    ThreadClass* get_next_thread()
    {
        if (0 == _thread_count)
		{
			return NULL;
		}
		// ���һ���߳�
        if (_next_thread >= _thread_count)
		{
			_next_thread = 0;
		}

        return _thread_array[_next_thread++];
    }

private:
	// ��һ���߳�������
    unsigned int _next_thread;
	// �߳���
    unsigned int _thread_count;
	// �߳�����
    ThreadClass** _thread_array;
};

// SYS_NS_END

#endif
