#ifndef _FILE_LOCKER_H_
#define _FILE_LOCKER_H_

#include <errno.h>
#include <stdio.h>
#include <string>
#include <sys/file.h>
#include <unistd.h>


// UTILS_NS_BEGIN

// ��ͨ�ļ������������������ļ�
// ֧�ֶ�ռ�͹����������ͣ��ɲ���exclusive����
// �����˳�ʱ���������е����ᱻ�Զ��ͷ�
// ע��ͬһ��FileLocker���󣬲�Ҫ���߳�ʹ��
// ͬһ��FileLocker��������ֻ�ᱻͬһ���̵߳���
class FileLocker
{
public:
	// ���������캯�����ɵ����߾�����ʱ����
    explicit FileLocker(const char* filepath) throw ()
        : _fd(-1), _filepath(filepath)
    {
    }

	// �Զ��������캯��
	// ����ʹ��SharedFileLocker��ExclusiveFileLocker������˹��캯������
    explicit FileLocker(const char* filepath, bool exclusive) throw ()
        : _fd(-1), _filepath(filepath)
    {
        lock(exclusive);
    }

    ~FileLocker() throw ()
    {
        unlock();
    }

	// �Ƿ��ռ����
    bool lock(bool exclusive) throw ()
    {
        // ��ռ���ǹ���
        int operation = exclusive ? LOCK_EX : LOCK_SH;

        _fd = open(_filepath.c_str(), O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        if (-1 != _fd)
        {
            if (-1 == flock(_fd, operation))
            {
                int errcode = errno;
                close(_fd);

                _fd = -1;
				// �ָ���Ŀ�������ϲ�����߿���ʹ��errno
                errno = errcode;
            }
        }

        return (-1 != _fd);
    }

    bool unlock() throw ()
    {
        bool ret = false;
    
        if (is_locked())
        {
            if (0 == flock(_fd, LOCK_UN))
            {
                ret = true;
            }

            close(_fd);
        }
        
        return ret;
    }

    bool is_locked() const throw ()
    {
        return (-1 != _fd);
    }

    operator bool () const
    {
        return (-1 != _fd);
    }

	// ��ȡ���ļ�·��
    const std::string& get_filepath() const throw ()
    {
        return _filepath;
    }

private:
    int _fd;
    std::string _filepath;
};


// ��ռ�ļ���
class ExclusiveFileLocker : public FileLocker
{
public:
    explicit ExclusiveFileLocker(const char* filepath)
        : FileLocker(filepath, true)
    {
    }

private:
    bool lock();
    bool unlock();
};


// �����ļ���
class SharedFileLocker : public FileLocker
{
public:
    explicit SharedFileLocker(const char* filepath)
        : FileLocker(filepath, false)
    {
    }

private:
    bool lock();
    bool unlock();
};


// ��ǿ���ļ�������ֻ���ļ�ָ���Ĳ���,һ��ExFileLocker��������ֻ��һ���̵߳���
class ExFileLocker
{
public:
	// ������ǿ���ļ�������
	// ��������Ҫ�����ļ��Ĵ򿪺͹ر�
	// fd�������Ѵ򿪵��ļ�������
	// �ұ�����O_WRONLY��O_RDWR��ʽ�򿪣���������EBADF����
	// ��������ý��̾���PRIV_LOCKRDONLYȨ�޵���ĳ�Ա�����ʹ��O_RDONLY��fd
    explicit ExFileLocker(int fd)
        : _fd(fd)
    {
    }

	// ���������������������true����������
	// �������ʧ�ܣ��򷵻�false
	// size ���Ϊ0����ʾ�������ļ���ǰƫ�ƿ�ʼ���ļ�β������
	// ���Ϊ�������ʾ�������ļ���ǰƫ�ƿ�ʼ�������СΪsize����������
	// ���Ϊ�������ʾ�������ļ���ǰƫ�ƿ�ʼ����ǰ��СΪsize����������
    bool lock(off_t size)
    {
        if (-1 != _fd)
        {
            return 0 == lockf(_fd, F_LOCK, size);
        }

        return false;
    }

	// ���Լ����������������أ����ᱻ����
	// ��������ɹ�������������true��������������false
	// size ���Ϊ0����ʾ�����������ļ���ǰƫ�ƿ�ʼ���ļ�β������
	// ���Ϊ�������ʾ�����������ļ���ǰƫ�ƿ�ʼ�������СΪsize����������
	// ���Ϊ�������ʾ�����������ļ���ǰƫ�ƿ�ʼ����ǰ��СΪsize����������
    bool try_lock(off_t size)
    {
        if (-1 != _fd)
        {
            return 0 == lockf(_fd, F_TLOCK, size);
        }

        return false;
    }

	// �����ָ�����������Ƿ�����������̵�����
	// ���������ɷ��ʣ��򷵻�true�����򷵻�false
	// size ���Ϊ0����ʾ�����ļ���ǰƫ�ƿ�ʼ���ļ�β�����������״̬
	// ���Ϊ�������ʾ�����ļ���ǰƫ�ƿ�ʼ�������СΪsize���������������״̬
	// ���Ϊ�������ʾ�����ļ���ǰƫ�ƿ�ʼ����ǰ��СΪsize���������������״̬
    bool test_lock(off_t size)
    {
        if (-1 != _fd)
        {
            return 0 == lockf(_fd, F_TEST, 0);
        }

        return false;
    }

	// ��������ɹ�������������true��������������false
	// size ���Ϊ0����ʾ�������ļ���ǰƫ�ƿ�ʼ���ļ�β������
	// ���Ϊ�������ʾ�������ļ���ǰƫ�ƿ�ʼ�������СΪsize����������
	// ���Ϊ�������ʾ�������ļ���ǰƫ�ƿ�ʼ����ǰ��СΪsize����������
    bool unlock(off_t size)
    {
        if (-1 != _fd)
        {
            return 0 == lockf(_fd, F_ULOCK, size);
        }

        return false;
    }

private:
    int _fd;
};

// UTILS_NS_END

#endif
