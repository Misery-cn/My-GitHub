#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <vector>
#include <sys/types.h>
#include <unistd.h>
#include "const.h"
#include "exception.h"
#include "error.h"

// UTILS_NS_BEGIN


template<typename T>
class CPointGuard
{
public:
    CPointGuard(T* p, bool is_arrary = false);
    virtual ~CPointGuard();
private:
    T* _point;
	bool _is_arrary;
};

template<typename T>
CPointGuard<T>::CPointGuard(T * p, bool is_arrary)
{
	_point = p;
	_is_arrary = is_arrary;
}

template<typename T>
CPointGuard<T>::~CPointGuard()
{
	if (!_is_arrary)
	{
		delete _point;
	}
	else
	{
		delete[] _point;
	}
	
	_point = NULL;
}

// ������close���ֺ�����Ҫ������й��е�close����
template <typename T>
class CloseHelper
{
public:
    CloseHelper(T* obj) : _obj(obj)
    {
    }

    ~CloseHelper()
    {
        if (NULL != _obj)
        {
            _obj->close();
        }
    }

    T* operator->()
    {
        return _obj;
    }

    T* release()
    {
        T* obj = _obj;
        _obj = NULL;
        return obj;
    }

private:
    T* _obj;
};

template <>
class CloseHelper<int>
{
public:
    CloseHelper<int>(int fd) : _fd(fd)
    {
    }
    
    ~CloseHelper<int>()
    {
        if (-1 != _fd)
        {
            ::close(_fd);
        }
    }

    operator int() const
    {
        return _fd;
    }

    int get() const
    {
        return _fd;
    }

    int release()
    {
        int fd = _fd;
        _fd = -1;
        return fd;
    }

private:
    int _fd;
};
 

template <>
class CloseHelper<FILE*>
{
public:
    CloseHelper<FILE*>(FILE* fp) : _fd(fp)
    {
    }
    
    ~CloseHelper<FILE*>()
    {
        if (NULL != _fd)
        {
            fclose(_fd);
        }
    }

    operator FILE*() const
    {
        return _fd;
    }

    FILE* release()
    {
        FILE* fp = _fd;
        _fd = NULL;
        return fp;
    }
 
private:
    FILE* _fd;
};

template <typename T>
class ReleaseHelper
{
public:
    ReleaseHelper(T* obj) : _obj(obj)
    {
    }

    ~ReleaseHelper()
    {
        _obj->release();
    }

    T* operator->()
    {
        return _obj;
    }

private:
    T* _obj;
};


template <typename DataType>
class CArrayList
{       
public:
    CArrayList(uint32_t list_max) : _head(0), _tail(0), _list_size(0)
    {
        _list_max = list_max;
        if (0 == _list_max)
        {
            _items = NULL;
        }
        else
        {
            _items = new DataType[_list_max];        
            memset(_items, 0, _list_max);
        }
    }

    ~CArrayList()
    {
        delete []_items;
    }

	
    bool is_full() const 
    {
        return (_list_max == _list_size);
    }
    
    bool is_empty() const 
    {
        return (0 == _list_size);
    }

    DataType front() const 
    {
        return _items[_head];
    }
    
    DataType pop_front() 
    {
        DataType item = _items[_head];
        _head = (_head + 1) % _list_max;
        --_list_size;
        return item;
    }

    void push_back(DataType item) 
    {
        _items[_tail] = item;
        _tail = (_tail + 1) % _list_max; 
        ++_list_size;
    }

    uint32_t size() const 
    {
        return _list_size;
    }

	DataType& operator[](uint32_t pos)
	{
		if (_list_size < pos)
		{
			return _items[_list_size];
		}

		return _items[pos];
	}
    
    uint32_t capacity() const
    {
        return _list_max;
    }

private:
	volatile uint32_t _head;
    volatile uint32_t _tail;
    volatile uint32_t _list_size;
    uint32_t _list_max;
    DataType* _items;
};


class CUtils
{
public:
	// ���뼶sleep����
    static void millisleep(uint32_t millisecond);

    // ��ȡָ��ϵͳ���ô�������ַ���������Ϣ
    static std::string get_error_message(int errcode);

    // �õ����һ�εĳ�����Ϣ
    static std::string get_last_error_message();

    // �õ����һ�εĳ������
    static int get_last_error_code();

    // �õ���ǰ���̺�
    static int get_current_process_id();

    // �õ���ǰ����������ִ���ļ����ڵľ���·������β��������б��
    static std::string get_program_path();
    
    // �õ���ָ��fd���Ӧ���ļ���������·������
	static std::string get_filename(int fd);

    // �õ�һ��Ŀ¼�ľ���·����·���в������../��./�ȣ���һ��������ԭʼ·��
    static std::string get_full_directory(const char* directory);

    // �õ�CPU�˸���
	static uint16_t get_cpu_number();    

	// �õ���ǰ����ջ;ע������: ����Դ����ʱ����-rdynamic��-gѡ�������ܿ������Ǻ�����ַ�������Ǻ�����������
    static bool get_backtrace(std::string& call_stack);

	// �õ�ָ��Ŀ¼�ֽ�����С�����̰߳�ȫ������ͬһʱ��ֻ�ܱ�һ���̵߳���
    static off_t du(const char* dirpath);

    // �õ��ڴ�ҳ��С
    static int get_page_size();

	// �õ�һ�����̿ɳ��е�����ļ�(�����׽��ֵ�)�����
    static int get_fd_max();

	// �ж�ָ��fd��Ӧ���Ƿ�Ϊ�ļ�
    static bool is_file(int fd);
	// �ж�ָ��Path�Ƿ�Ϊһ���ļ�
    static bool is_file(const char* path);
	// �ж�ָ��fd��Ӧ���Ƿ�Ϊ������
    static bool is_link(int fd);
	// �ж�ָ��Path�Ƿ�Ϊһ��������
    static bool is_link(const char* path);
	// �ж�ָ��fd��Ӧ���Ƿ�ΪĿ¼
    static bool is_directory(int fd);
	// �ж�ָ��Path�Ƿ�Ϊһ��Ŀ¼
    static bool is_directory(const char* path);
    
	// �Ƿ�����ǰ��������coredump�ļ�
    static void enable_core_dump(bool enabled=true, int core_file_size=-1);

	// �õ���ǰ���̳�������ƣ������main������argv[0]��ͬ
    static std::string get_program_long_name();

	// �õ���ǰ���̵ĵ����ƣ�������Ŀ¼���֣��硰./abc.exe��ֵΪ��abc.exe��
	// ���������set_process_title()��
	// ��ͨ��program_invocation_short_name����ȡ����Ԥ�ڵ�ֵ���������ص��ǿ�
    static std::string get_program_short_name();


	// ȡ·�����ļ������֣����������׺����
    static std::string get_filename(const std::string& filepath);

	// ȡ·����Ŀ¼���֣��������ļ������֣�����֤���Է�б�ܽ�β
    static std::string get_dirpath(const std::string& filepath);

	// ���ý�����
    static void set_process_name(const std::string& new_name);
    static void set_process_name(const char* format, ...);

	// ���ý��̱��⣬ps������Ľ���������ȵ���init_program_title()�󣬲ſ��Ե���set_program_title()
    static void init_process_title(int argc, char *argv[]);    
    static void set_process_title(const std::string& new_title);
    static void set_process_title(const char* format, ...);

	// ͨ�õ�pipe��ȡ����
	// ��ȡ����Ϊ���ȶ�һ��4�ֽڵĳ��ȣ�Ȼ����ݳ��ȶ�ȡ����
	// fd pipe�ľ��
	// buffer �洢��pipe�ж�ȡ�����ݣ�ע�������ʹ�ú�������delete []buffer���ͷ��ڴ�
	// buffer_size �洢��pipe�ж�ȡ���������ֽ���
    static void common_pipe_read(int fd, char** buffer, int32_t* buffer_size);

	// ͨ�õ�pipeд����
	// ��ȡ����Ϊ����дһ��4�ֽڵĳ���buffer_size��Ȼ����ݳ���buffer_sizeд������
	// fd pipe�ľ��
	// buffer ��Ҫд��pipe������
	// buffer_size ��Ҫд����ֽ���
    static void common_pipe_write(int fd, const char* buffer, int32_t buffer_size);

    // ȡ�����
    template <typename T>
    static T get_random_number(unsigned int i, T max_number)
    {
        struct timeval tv;
        struct timezone *tz = NULL;

        gettimeofday(&tv, tz);
        srandom(tv.tv_usec + i); // ����i���Խ������ʱtv_usecֵ��ͬ

        // RAND_MAX ������INT_MAX
        return static_cast<T>(random() % max_number);
    }

    // ��һ��vector�����
    template <typename T>
    static void randomize_vector(std::vector<T>& vec)
    {
        unsigned int j = 0;
        std::vector<T> tmp;

        while (!vec.empty())
        {
            typename std::vector<T>::size_type max_size = vec.size();
            typename std::vector<T>::size_type random_number = CUtils::get_random_number(j, max_size);

            typename std::vector<T>::iterator iter = vec.begin() + random_number;
            tmp.push_back(*iter);
            vec.erase(iter);
            ++j;
        }

        vec.swap(tmp);
    }
};

// UTILS_NS_END

#endif
