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
    CThreadPool() : next_thread_(0), thread_count_(0), thread_array_(NULL)
    {
		
    }

    // �����̳߳�,�������̳߳��е������̣߳�
	// ���̴߳����ɹ���,������������������״̬,���Ǵ��ڵȴ�״̬,��Ҫ��������
	// thread_count:�̳߳��е��̸߳���
	// parameter:���ݸ����̵߳Ĳ���
    void create(unsigned int thread_count, void* parameter = NULL)
    {
        thread_array_ = new ThreadClass*[thread_count];
        for (unsigned int i = 0; i < thread_count; ++i)
        {
            thread_array_[i] = new ThreadClass;
            thread_array_[i]->set_index(i);
            thread_array_[i]->set_parameter(parameter);
        }
        for (unsigned int i = 0; i < thread_count; ++i)
        {
            try
            {                
                thread_array_[i]->start();
                ++thread_count_;
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
        if (NULL != thread_array_)
        {
            unsigned int thread_count = thread_count_;
            for (unsigned int i = thread_count; 0 < i; --i)
            {
                thread_array_[i-1]->stop();
                --thread_count_;
            }

            delete []thread_array_;
            thread_array_ = NULL;
        }
    }

	// �����̳߳أ������е������̻߳��ѣ�
	// Ҳ���Ե������ø����̵߳�wakeup�����ǻ���
    void activate()
    {
        for (unsigned int i = 0; i < thread_count_; ++i)
		{
			thread_array_[i]->wakeup();
		}
    }

    // ��ȡ�̸߳���
    unsigned int get_thread_count() const { return thread_count_; }

    // �õ��̳߳��е��߳�����
    ThreadClass** get_thread_array() { return thread_array_; }
    ThreadClass** get_thread_array() const { return thread_array_; }

    // �����̱߳��,�õ���Ӧ���߳�
    ThreadClass* get_thread(unsigned int thread_index)
    {
        if (0 == thread_count_)
		{
			return NULL;
		}
		
        if (thread_index > thread_count_)
		{
			return NULL;
		}
		
        return thread_array_[thread_index];
    }
    
    // �����̱߳��,�õ���Ӧ���߳�
    ThreadClass* get_thread(unsigned int thread_index) const
    {
        if (0 == thread_count_)
		{
			return NULL;
		}
		
        if (thread_index > thread_count_)
		{
			return NULL;
		}
		
        return thread_array_[thread_index];
    }
    
	// �õ�ָ���¸��̵߳�ָ��,�ӵ�һ����ʼѭ������,���ս��,���ﵽ���һ��ʱ,��ָ���һ��
	// ��ҪӦ���ڲ�����ѯ��ʽ��һ�������������������߳�
    ThreadClass* get_next_thread()
    {
        if (0 == thread_count_)
		{
			return NULL;
		}
		// ���һ���߳�
        if (next_thread_ >= thread_count_)
		{
			next_thread_ = 0;
		}

        return thread_array_[next_thread_++];
    }

private:
	// ��һ���߳�������
    unsigned int next_thread_;
	// �߳���
    unsigned int thread_count_;
	// �߳�����
    ThreadClass** thread_array_;
};

// SYS_NS_END

#endif
