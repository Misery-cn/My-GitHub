#include "socket.h"

// SYS_NS_BEGIN


Socket::Socket()
{
	_state = SOCKET_CLOSE;
    _fd = -1;
}

Socket::~Socket()
{
    if (0 < _fd)
    {
        (void)s_close();
		_fd = -1;
    }
}

int Socket::s_open() throw (SysCallException)
{
    // ����һ���׽���
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (0 >= _fd)
    {
        THROW_SYSCALL_EXCEPTION(NULL, errno, "socket");
    }

    return 0;
}

int Socket::s_close() throw (SysCallException)
{
    // �ر�����
    shutdown(_fd, SHUT_RDWR);
    // �����׽���
    if (0 != close(_fd))
    {
        THROW_SYSCALL_EXCEPTION(NULL, errno, "close");
    }
	
	_fd = -1;
	
    return 0;
}

int Socket::s_close(size_t fd) throw (SysCallException)
{
    // �ر�����
    shutdown(fd, SHUT_RDWR);
    // �����׽���
    if (0 != close(fd))
    {
        THROW_SYSCALL_EXCEPTION(NULL, errno, "close");
    }
	
	fd = -1;
	
    return 0;
}

int Socket::s_bind(char* ip, int port) throw (SysCallException)
{
    int	op = 1;
    struct sockaddr_in addr;

    memset((char *)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if ((NULL == ip) || (MIN_IP_LEN > strlen(ip)))
    {
        addr.sin_addr.s_addr = INADDR_ANY;
    }
    else
    {
        addr.sin_addr.s_addr = inet_addr(ip);
    }

    // ������bind�����б��ص�ַ���ظ�ʹ��
    // �ɹ�����0��ʧ�ܷ���-1
    if (0 > setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&op, sizeof(op)))
    {
        return -1;
    }

    // ���׽ӿڷ���Э���ַ,���ǰ��û�����õ�ַ���ظ�ʹ��,��Ҫ�ж�errno=EADDRINUSE,����ַ��ʹ��
    // �ɹ�����0��ʧ�ܷ���-1
    if (0 > bind(_fd, (struct sockaddr*)&addr, sizeof(addr)))
    {
        THROW_SYSCALL_EXCEPTION(NULL, errno, "bind");
    }

    return 0;
}

int Socket::s_listen() throw (SysCallException)
{
    if (0 != listen(_fd, MAX_CONN))
    {
        THROW_SYSCALL_EXCEPTION(NULL, errno, "listen");
    }

    return 0;
}

int Socket::s_accept(int* fd, struct sockaddr_in* addr) throw (SysCallException)
{
    int newfd = -1;
    int size = sizeof(struct sockaddr_in);

    newfd = accept(_fd, (struct sockaddr*)addr, (socklen_t*)&size);
    if (0 > newfd)
    {
        THROW_SYSCALL_EXCEPTION(NULL, errno, "accept");
    }

    *fd = newfd;

    return 0;
}

int Socket::s_connect(char* ip, int port) throw (SysCallException)
{
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    // CSocket����
    if (0 != connect(_fd, (struct sockaddr*)&addr, sizeof(addr)))
    {
        THROW_SYSCALL_EXCEPTION(NULL, errno, "connect");
    }

    return 0;
}

int Socket::s_read(int fd, char* buff, int len) throw (SysCallException)
{
    /*
	fd_set readfds;
    fd_set errfds;
    struct timeval tv;

    // ��ʼ��
    FD_ZERO(&readfds);
    // ��sockfd�Ŀɶ�λ
    FD_SET(fd, &readfds);

    // ��ʼ��
    FD_ZERO(&errfds);
    // ����sockfd���쳣λ
    FD_SET(fd, &errfds);

    tv.tv_sec = WAIT_TIME_OUT;
    tv.tv_usec = 0;

    // ����׽���״̬
    if (select(fd + 1, &readfds, NULL, &errfds, &tv))
    {
        // �ж�������sockfd���쳣λ�Ƿ��,��������ʾ��������
        if (FD_ISSET(fd, &errfds))
        {
            return SOCKET_READ_ERROR;
        }

        // �ж�������sockfd�Ŀɶ�λ�Ƿ��,��������ȡ����
        if (FD_ISSET(fd, &readfds))
        {
            // ������
            // ����ʵ���������ֽ���
            if (0 > read(fd, buff, len))
            {
                return SOCKET_READ_ERROR;
            }
            else
            {
				return NO_ERROR;
            }
        }
    }
	*/
	
	// ������
	// ����ʵ���������ֽ���
	if (0 > read(fd, buff, len))
	{
		if ((EAGAIN != errno) && (EWOULDBLOCK != errno))
        {
			THROW_SYSCALL_EXCEPTION(NULL, errno, "read");
        }
	}
	else
	{
		return 0;
	}
}

int Socket::s_write(int fd, char* buff) throw (SysCallException)
{
	/*
    fd_set writefds;
    struct timeval tv;

    // ��ʼ��
    FD_ZERO(&writefds);
    // ��fd�Ŀ�дλ
    FD_SET(fd, &writefds);

    tv.tv_sec = WAIT_TIME_OUT;
    tv.tv_usec = 0;

    int bufLen = strlen(buff);

    // ����׽���״̬
    if (select(fd + 1, NULL, &writefds, 0, &tv))
    {
        // �ж�������sockfd�Ŀ�дλ�Ƿ��,�������д������
        if (FD_ISSET(fd, &writefds))
        {
            // д������
            // ������д�ֽ���
            int wl = write(fd, buff, bufLen);
            if (0 > wl)
            {
                return SOCKET_WRITE_ERROR;
            }
            else
            {
                if (wl == bufLen)
                {
                    return NO_ERROR;
                }
                else
                {
                    return SOCKET_WRITE_ERROR;
                }
            }
        }
    }
	*/
	
	// д������
	// ������д�ֽ���
	int bufLen = strlen(buff);
	int wl = write(fd, buff, bufLen);
	if (0 > wl)
	{
		if ((EAGAIN != errno) && (EWOULDBLOCK != errno))
        {
			THROW_SYSCALL_EXCEPTION(NULL, errno, "write");
        }
	}
	else
	{
		if (wl == bufLen)
		{
			return 0;
		}
		else
		{
			THROW_SYSCALL_EXCEPTION(NULL, errno, "write");
		}
	}
}

// SYS_NS_END
