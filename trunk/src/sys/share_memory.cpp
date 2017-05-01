#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "dir_utils.h"
#include "share_memory.h"



void ShareMemory::open(const char* path) throw (SysCallException)
{
    if (NULL == path)
    {
        THROW_SYSCALL_EXCEPTION(NULL, EINVAL, NULL);
    }

    key_t key = ftok(path, getpid());
    if (-1 == key)
    {
        THROW_SYSCALL_EXCEPTION(NULL, errno, "ftok");
    }

    _shmid = shmget(key, 1, 0);
    if (-1 == _shmid)
    {
        THROW_SYSCALL_EXCEPTION(NULL, errno, "shmget");
    }
}

void ShareMemory::close() throw (SysCallException)
{
    if (-1 != _shmid)
    {
        if (-1 == shmctl(_shmid, IPC_RMID, NULL))
        {
            THROW_SYSCALL_EXCEPTION(NULL, errno, "shmctl");
        }

        _shmid = -1;
    }
}


bool ShareMemory::create(const char* path, mode_t mode) throw (SysCallException)
{
    key_t key = IPC_PRIVATE;
    
    if (path != NULL)
    {
    	// ���Ҫȷ��key_tֵ���䣬Ҫôȷ��ftok���ļ�����ɾ����Ҫô����ftok��ָ��һ���̶���key_tֵ
    	// #define IPCKEY 0x01
    	// ����������path��proj_id�ϴ��һ��,˫���Ͷ��ܹ�ͨ������ftok�����õ�ͬһ��IPC��
        key_t key = ftok(path, getpid());
        if (-1 == key)
        {
            THROW_SYSCALL_EXCEPTION(NULL, errno, "ftok");
        }
    }

    _shmid = shmget(key, 1, IPC_CREAT | IPC_EXCL | mode);
    if (-1 == _shmid)
    {
        if (EEXIST == errno) 
		{
			return false;
        }
		
        THROW_SYSCALL_EXCEPTION(NULL, errno, "shmget");
    }
        
    return true;
}

void ShareMemory::detach() throw (SysCallException)
{
    if (NULL != _shmaddr)
    {
        shmdt(_shmaddr);
        _shmaddr = NULL;
    }
}

void* ShareMemory::attach(int flag) throw (SysCallException)
{
    if (NULL != _shmaddr)
    {
        _shmaddr = shmat(_shmid, NULL, flag);
        if ((void *)-1 == _shmaddr)
        {
            THROW_SYSCALL_EXCEPTION(NULL, errno, "shmat");
        }
    }

    return _shmaddr;
}

void* ShareMemory::get_shared_memory_address() const
{
    return _shmaddr;
}


