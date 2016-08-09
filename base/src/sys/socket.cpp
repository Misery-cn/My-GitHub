#include "socket.h"

SYS_NS_BEGIN


CSocket::CSocket()
{
	state_ = SOCKET_CLOSE;
    fd_ = -1;
}

CSocket::~CSocket()
{
    if (0 < fd_)
    {
        (void)s_close();
		fd_ = -1;
    }
}

int CSocket::s_open()
{
    // ����һ���׽���
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (0 >= fd_)
    {
        RUNLOG("create CSocket failed!\n");
        return SOCKET_ERROR;
    }

    return NO_ERROR;
}

int CSocket::s_close()
{
    // �ر�����
    shutdown(fd_, SHUT_RDWR);
    // �����׽���
    if (0 != close(fd_))
    {
        return SOCKET_CLOSE_ERROR;
    }
	
	fd_ = -1;
	
    return NO_ERROR;
}

int CSocket::s_close(size_t fd)
{
    // �ر�����
    shutdown(fd, SHUT_RDWR);
    // �����׽���
    if (0 != close(fd))
    {
        return SOCKET_CLOSE_ERROR;
    }
	
	fd = -1;
	
    return NO_ERROR;
}

int CSocket::s_bind(char* ip, int port)
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
    if (0 > setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (char *)&op, sizeof(op)))
    {
        return SOCKET_SET_ERROR;
    }

    // ���׽ӿڷ���Э���ַ,���ǰ��û�����õ�ַ���ظ�ʹ��,��Ҫ�ж�errno=EADDRINUSE,����ַ��ʹ��
    // �ɹ�����0��ʧ�ܷ���-1
    if (0 > bind(fd_, (struct sockaddr*)&addr, sizeof(addr)))
    {
        return SOCKET_BIND_ERROR;
    }

    return NO_ERROR;
}

int CSocket::s_listen()
{
    if (0 != listen(fd_, MAX_CONN))
    {
        return SOCKET_LISTEN_ERROR;
    }

    return NO_ERROR;
}

int CSocket::s_accept(int* fd, struct sockaddr_in* addr)
{
    int newfd = -1;
    int size = sizeof(struct sockaddr_in);

    newfd = accept(fd_, (struct sockaddr*)addr, (socklen_t*)&size);
    if (0 > newfd)
    {
        return SOCKET_ACCEPT_ERROR;
    }

    *fd = newfd;

    return NO_ERROR;
}

int CSocket::s_connect(char* ip, int port)
{
    struct sockaddr_in addr;

    addr.sin_family    = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    // CSocket����
    if (0 != connect(fd_, (struct sockaddr*)&addr, sizeof(addr)))
    {
        RUNLOG("connect failed: %s", strerror(errno));
        return SOCKET_CONNECT_ERROR;
    }

    return NO_ERROR;
}

int CSocket::s_read(int fd, char* buff, int len)
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
		return SOCKET_READ_ERROR;
	}
	else
	{
		return NO_ERROR;
	}
}

int CSocket::s_write(int fd, char* buff)
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

SYS_NS_END
