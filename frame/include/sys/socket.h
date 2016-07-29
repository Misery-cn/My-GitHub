
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
#include "util.h"

namespace sys
{

class CSocket
{
public:
    CSocket();
    virtual ~CSocket();
	
	enum SOCKET_STATE
	{
		SOCKET_CLOSE = 0,
		SOCKET_OPEN = 1
	};

    // �����׽���
    int s_open();
    // �ر��׽���
    int s_close();
	// �رյ���open���ص��׽���
	int s_close(size_t fd);
    // �����˿�,MaxConnΪͬʱ��������������
    int s_listen();
    // �󶨶˿�
    int s_bind(IN char* ip, IN int port);
    // ����������
    int s_connect(IN char* ip, IN int port);
    // ��������
    int s_accept(OUT int* fd, OUT struct sockaddr_in* addr);
    // ��ȡ����
    int s_read(IN int fd, OUT char* buff, OUT int len);
    // д������
    int s_write(IN int fd, IN char* buff);
	
	inline size_t getfd() {return fd_;}
private:
    size_t fd_;
	SOCKET_STATE state_;
};

}

#endif
