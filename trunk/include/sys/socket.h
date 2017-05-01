
#ifndef _SYS_SOCKET_H_
#define _SYS_SOCKET_H_

#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "utils.h"

// SYS_NS_BEGIN

class Socket
{
public:
    Socket();
    virtual ~Socket();
	
	enum SOCKET_STATE
	{
		SOCKET_CLOSE = 0,
		SOCKET_OPEN = 1
	};

    // �����׽���
    int s_open() throw (SysCallException);
    // �ر��׽���
    int s_close() throw (SysCallException);
	// �رյ���open���ص��׽���
	int s_close(size_t fd) throw (SysCallException);
    // �����˿�,MaxConnΪͬʱ��������������
    int s_listen() throw (SysCallException);
    // �󶨶˿�
    int s_bind(IN char* ip, IN int port) throw (SysCallException);
    // ����������
    int s_connect(IN char* ip, IN int port) throw (SysCallException);
    // ��������
    int s_accept(OUT int* fd, OUT struct sockaddr_in* addr) throw (SysCallException);
    // ��ȡ����
    int s_read(IN int fd, OUT char* buff, OUT int len) throw (SysCallException);
    // д������
    int s_write(IN int fd, IN char* buff) throw (SysCallException);
	
	inline size_t getfd() {return _fd;}
private:
    size_t _fd;
	SOCKET_STATE _state;
};

// SYS_NS_END

#endif
