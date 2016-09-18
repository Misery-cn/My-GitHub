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
        : fd_(-1), filepath_(filepath)
    {
    }

	// �Զ��������캯��
	// ����ʹ��SharedFileLocker��ExclusiveFileLocker������˹��캯������
    explicit FileLocker(const char* filepath, bool exclusive) throw ()
        : fd_(-1), filepath_(filepath)
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

        fd_ = open(filepath_.c_str(), O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        if (-1 != fd_)
        {
            if (-1 == flock(fd_, operation))
            {
                int errcode = errno;
                close(fd_);

                fd_ = -1;
				// �ָ���Ŀ�������ϲ�����߿���ʹ��errno
                errno = errcode;
            }
        }

        return (-1 != fd_);
    }

    bool unlock() throw ()
    {
        bool ret = false;
    
        if (is_locked())
        {
            if (0 == flock(fd_, LOCK_UN))
            {
                ret = true;
            }

            close(fd_);
        }
        
        return ret;
    }

    bool is_locked() const throw ()
    {
        return (-1 != fd_);
    }

    operator bool () const
    {
        return (-1 != fd_);
    }

	// ��ȡ���ļ�·��
    const std::string& get_filepath() const throw ()
    {
        return filepath_;
    }

private:
    int fd_;
    std::string filepath_;
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
        : fd_(fd)
    {
    }

	// ���������������������true����������
	// �������ʧ�ܣ��򷵻�false
	// size ���Ϊ0����ʾ�������ļ���ǰƫ�ƿ�ʼ���ļ�β������
	// ���Ϊ�������ʾ�������ļ���ǰƫ�ƿ�ʼ�������СΪsize����������
	// ���Ϊ�������ʾ�������ļ���ǰƫ�ƿ�ʼ����ǰ��СΪsize����������
    bool lock(off_t size)
    {
        if (-1 != fd_)
        {
            return 0 == lockf(fd_, F_LOCK, size);
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
        if (-1 != fd_)
        {
            return 0 == lockf(fd_, F_TLOCK, size);
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
        if (-1 != fd_)
        {
            return 0 == lockf(fd_, F_TEST, 0);
        }

        return false;
    }

	// ��������ɹ�������������true��������������false
	// size ���Ϊ0����ʾ�������ļ���ǰƫ�ƿ�ʼ���ļ�β������
	// ���Ϊ�������ʾ�������ļ���ǰƫ�ƿ�ʼ�������СΪsize����������
	// ���Ϊ�������ʾ�������ļ���ǰƫ�ƿ�ʼ����ǰ��СΪsize����������
    bool unlock(off_t size)
    {
        if (-1 != fd_)
        {
            return 0 == lockf(fd_, F_ULOCK, size);
        }

        return false;
    }

private:
    int fd_;
};

// UTILS_NS_END

#endif
