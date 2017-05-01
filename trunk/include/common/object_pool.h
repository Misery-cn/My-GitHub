#include "utils.h"

// Ҫʹ�ö����,�������̳��Ը���
class PoolObject
{
public:
    
    PoolObject() throw () :_in_pool(false) ,_index(0)
    {
    }

    void set_index(uint32_t index) throw ()
    {
        _index = index;
    }

    uint32_t get_index() const throw ()
    {
        return _index;
    }

    void set_in_pool(bool in_pool) throw ()
    {
        _in_pool = in_pool;
    }

	// �ж϶����Ƿ��ڶ������
    bool is_in_pool() const throw ()
    {
        return _in_pool;
    }

	void reset() = 0;

private:
	// �Ƿ��ڶ������
    bool _in_pool;
	// �ڶ�����е�����
    uint32_t _index;
};


template<class ObjectClass>
class ObjectPool
{
public:
	
	ObjectPool() throw () : _obj_num(0), _idle_num(0), _object_queue(NULL)
	{
	}

	void create(uint32_t object_number) throw ()
	{
		MutexGuard locker(_lock);
		
		_obj_num = object_number;
		_idle_num = _obj_num;

		// _object_array = new ObjectClass[_obj_num];
		_object_queue = new ArrayList<ObjectClass*>(_obj_num);

		for (uint32_t i = 0; i < _obj_num; ++i)
		{
			// ObjectClass* object = &_object_array[i];
			// �����Ҫnew���,Ҳ����һ����newһ������
			ObjectClass* object = new ObjectClass();
			object->set_index(i);
			object->set_in_pool(true);

			_object_queue->push_back(object);
		}
	}

	void destory() throw()
	{
		MutexGuard locker(_lock);
		
		DELETE_P(_object_queue);
        // DELETE_ARRAY(_object_array);
	}

	// �Ӷ�����л�ȡһ������
	ObjectClass* borrow() throw ()
    {
    	MutexGuard locker(_lock);
		
        ObjectClass* object = NULL;
        
        // ����ز�Ϊ��
        if (!_object_queue->is_empty())
        {
        	// ����һ������,���ö����Ѳ��ڳ���,���ж�������һ
			object = _object_queue->pop_front();
            object->set_in_pool(false);
            --_idle_num;
        }      

        return object;
    }

	void pay_back(ObjectClass* object) throw ()
    {
    	MutexGuard locker(_lock);
		
        // ȷ�������ǲ��ڳ��е�
        if (!object->is_in_pool())
        {
            object->reset();
            object->set_in_pool(true);

            _object_queue->push_back(object);
            ++_idle_num;
        }
    }

	uint32_t get_pool_size() const throw ()
	{
		return _idle_num;
	}

	volatile uint32_t get_idle_number() const throw ()
    {
        return _idle_num;
    }
		
private:
	// �����ܶ�����,����ʹ�ú�δʹ�õ�
	uint32_t _obj_num;
	// δʹ�õĶ�����
	uint32_t _idle_num;
	
	Mutex _lock;
	// ObjectClass* _object_array;
	ArrayList<ObjectClass*>* _object_queue;
};
