#ifndef _SYS_THREAD_POOL_H_
#define _SYS_THREAD_POOL_H_

#include <list>
#include <vector>
#include <set>
#include "thread.h"

// SYS_NS_BEGIN

// �̳߳���
class ThreadPool
{
public:

    ThreadPool(uint32_t thread_num);

    virtual ~ThreadPool();
	
	// ������л���
    class BaseWorkQueue
    {
    public:
        BaseWorkQueue() {}

        virtual ~BaseWorkQueue() {}

        virtual void _clear() = 0;

        virtual bool _empty() = 0;

        virtual void* _void_dequeue() = 0;

        virtual void _void_process(void*) = 0;

        virtual void _void_process_finish(void*) = 0;
    };

    // ��ȡ�߳���
    uint32_t get_num_threads()
    {
        Mutex::Locker locker(_lock);
        return _num_threads;
    }

    // ����������
    void add_work_queue(BaseWorkQueue* wq)
    {
        Mutex::Locker locker(_lock);
        _work_queues.push_back(wq);
    }

	// ɾ���������
    void remove_work_queue(BaseWorkQueue* wq)
    {
        Mutex::Locker locker(_lock);

        uint32_t i = 0;
        while (_work_queues[i] != wq)
        {
            i++;
        }

        for (i++; i < _work_queues.size(); i++)
        {
            _work_queues[i - 1] = _work_queues[i];
        }

        _work_queues.resize(i - 1);
    }

    void lock()
    {
        _lock.lock();
    }

    void unlock()
    {
        _lock.unlock();
    }

    void wakeup()
    {
        _cond.signal();
    }

    void wait()
    {
        _cond.wait(_lock);
    }

	// �����̳߳�
    void start();

	// ֹͣ�̳߳�
    void stop(bool clear_after=true);

	// ��ͣ�̳߳��������߳�
    void pause();
	// �ָ�����ͣ���߳�
    void unpause();
	// �ȴ��������
    void drain(BaseWorkQueue* wq = NULL);

private:
	// �����߳���
    class WorkThread : public Thread
    {
	public:
	
        WorkThread(ThreadPool* p) : _pool(p) {}
        // �����߳�������,������pool��worker����
        void entry()
        {
            _pool->worker(this);
        }
		
	private:	
		ThreadPool* _pool;
    };

	// �����̳߳��е��߳�
    void start_threads();

    // join�߳�
    void join_old_threads();

	// �̳߳ع�������
    void worker(WorkThread* wt);

public:
    template<class T>
    class BatchWorkQueue : public BaseWorkQueue
    {
    public:
		// ���������е��̳߳�
        BatchWorkQueue(ThreadPool* p) : _pool(p)
        {
            _pool->add_work_queue(this);
        }

		
        virtual ~BatchWorkQueue()
        {
            _pool->remove_work_queue(this);
        }

        bool queue(T* item)
        {
        	Mutex::Locker locker(_pool->_lock);
			
            bool r = _enqueue(item);
			
			// ���ѹ����߳�
            _pool->_cond.signal();
			
            return r;
        }

        void dequeue(T* item)
        {
            Mutex::Locker locker(_pool->_lock);
			
            _dequeue(item);
        }

        void clear()
        {
            Mutex::Locker locker(_pool->_lock);
			
            _clear();
        }

        void lock()
        {
            _pool->lock();
        }
        void unlock()
        {
            _pool->unlock();
        }

        void wakeup()
        {
            _pool->wakeup();
        }

        void drain()
        {
            _pool->drain(this);
        }

	protected:
        virtual void _process(const std::list<T*>& items) = 0;

	private:
        virtual bool _enqueue(T*) = 0;

        virtual void _dequeue(T*) = 0;

        virtual void _dequeue(std::list<T*>*) = 0;

        virtual void _process_finish(const std::list<T*>& ) {}

		// һ�η�����������
        void* _void_dequeue()
        {
            std::list<T*>* out(new std::list<T*>);
            _dequeue(out);
            if (!out->empty())
            {
                return (void*)out;
            }
            else
            {
                delete out;
                return NULL;
            }
        }

        void _void_process(void* p)
        {
            _process(*((std::list<T*>*)p));
        }

        void _void_process_finish(void* p)
        {
            _process_finish(*(std::list<T*>*)p);
            delete (std::list<T*>*)p;
        }
		
	private:
		// �̳߳�
		ThreadPool* _pool;
    };

	template<class T>
	class WorkQueue : public BaseWorkQueue
	{
	public:
		WorkQueue(ThreadPool* p) : _pool(p)
		{
			_pool->add_work_queue(this);
		}
		
		virtual ~WorkQueue()
		{
			_pool->remove_work_queue(this);
		}

		// ���к���,��Ҫ����������ʵ�ֵ�_enqueue
		bool queue(T* item)
		{
      		Mutex::Locker locker(_pool->_lock);
			bool r = _enqueue(item);
			_pool->_cond.signal();
			return r;
		}

		// ���к���,��Ҫ����������ʵ�ֵ�_dequeue
		void dequeue(T *item)
		{
			Mutex::Locker locker(_pool->_lock);
			_dequeue(item);
		}
		
		void clear()
		{
			Mutex::Locker locker(_pool->_lock);
			_clear();
		}

		void lock()
		{
			_pool->lock();
		}
		
		void unlock()
		{
			_pool->unlock();
		}
		
		void wakeup()
		{
			_pool->wakeup();
		}
		
		void _wait()
		{
			_pool->wait();
		}
		
		void drain()
		{
			_pool->drain(this);
		}

	protected:

		virtual void _process(T* t) = 0;

	private:
		// ����
	    virtual bool _enqueue(T*) = 0;
		// ����
	    virtual void _dequeue(T*) = 0;
		// ����
	    virtual T* _dequeue() = 0;
		
	    virtual void _process_finish(T*) {}

	    void* _void_dequeue()
		{
			return (void *)_dequeue();
	    }
		
	    void _void_process(void* p)
		{
			_process(static_cast<T*>(p));
	    }
		
	    void _void_process_finish(void* p)
		{
			_process_finish(static_cast<T*>(p));
	    }

	private:
		ThreadPool* _pool;
	};

private:
    Mutex _lock;
    Cond _cond;
    // �ȴ�����ִ���̵߳���������
    Cond _wait_cond;
    // �̳߳��Ƿ�ֹͣ
    bool _stop;
	uint32_t _pause;
	uint32_t _draining;
    // �߳���
    uint32_t _num_threads;
    // ��������
    std::vector<BaseWorkQueue*> _work_queues;
    // ��һ�ι�������������
    uint32_t _last_work_index;
    // �����߳�
    std::set<WorkThread*> _threads;
    // �ȴ�join���߳�
    std::list<WorkThread*> _old_threads;
    // ���ڹ������߳���
    uint32_t _processing;
};

// SYS_NS_END

#endif
