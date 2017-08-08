#ifndef _ACCEPTER_H_
#define _ACCEPTER_H_

#include <set>
#include "thread.h"
#include "msg_types.h"

class SimpleMessenger;

class Accepter : public Thread
{
private:
	SimpleMessenger* _msgr;
	// �Ƿ�ֹͣ
	bool _done;
	// ���ؼ������ļ�������
	int _listen_fd;
	int _shutdown_rd_fd;
	int _shutdown_wr_fd;
	
	int create_socket(int* rd, int* wr);

public:
	Accepter(SimpleMessenger* r) : _msgr(r), _done(false), _listen_fd(-1),
				_shutdown_rd_fd(-1), _shutdown_wr_fd(-1)
	{}
    
	// �߳����
	void entry();
	// ֹͣ�߳�
	void stop();
	// �󶨲���ʼ����
	int bind(const entity_addr_t& bind_addr, const std::set<int>& avoid_ports);
	// ���°�
	int rebind(const std::set<int>& avoid_port);
	// �����߳�
	int start();
};

#endif
