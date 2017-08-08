#ifndef _MESSENGER_H_
#define _MESSENGER_H_

#include <list>
#include <set>
#include <map>
#include "message.h"
#include "dispatcher.h"

// ͨ�Ż���
class Messenger
{
public:
	Messenger();

	Messenger(entity_name_t entityname) : _entity(), _started(false), _magic(0)
	{
		_entity._name = entityname;
	}
	
	virtual ~Messenger() {};

	struct Policy
	{
		// �������ӳ��ִ���ʱ��ɾ��
		bool _lossy;
		// ������ģʽ,��������
		bool _server;
		// �����Ӵ��ڵȴ�״̬
		bool _standby;
		// ���ӳ��������
		bool _resetcheck;


		// ���ֽڽ���
		Throttle* _throttler_bytes;
		// ����Ϣ������
		Throttle* _throttler_messages;

		Policy() : _lossy(false), _server(false), _standby(false), _resetcheck(true),
				   _throttler_bytes(NULL), _throttler_messages(NULL)
		{
			
		}
		
		Policy(bool l, bool s, bool st, bool r) : _lossy(l), _server(s), _standby(st), 
					_resetcheck(r), _throttler_bytes(NULL), _throttler_messages(NULL)
		{
			
		}

		static Policy stateful_server()
		{
			return Policy(false, true, true, true);
		}
		
		static Policy stateless_server()
		{
			return Policy(true, true, false, false);
		}
		
		static Policy lossless_peer()
		{
			return Policy(false, false, true, false);
		}
		
		static Policy lossless_peer_reuse()
		{
			return Policy(false, false, true, true);
		}
		
		static Policy lossy_client()
		{
			return Policy(true, false, false, false);
		}
		
		static Policy lossless_client()
		{
			return Policy(false, false, false, true);
		}
		
	};

	static Messenger* create(std::string type, entity_name_t name, std::string lname);

	static int get_default_crc_flags();

	// �󶨵�ַ,��������ʵ��
	virtual int bind(const entity_addr_t& bind_addr) = 0;

	// ���°󶨶˿�,���Ҳ�����֮ǰ�󶨵Ķ˿ڳ�ͻ
	virtual int rebind(const std::set<int>& avoid_ports) { return -EOPNOTSUPP; }
	// ����
	virtual int start() { _started = true; return 0; }
	// ֹͣ
	virtual int shutdown() { _started = false; return 0; }
	// �����ȴ��ر�
	virtual void wait() = 0;
	// ������Ϣ
	virtual int send_message(Message* msg, const entity_inst_t& dest) = 0;
	// �ر�socket����
	virtual void mark_down(const entity_addr_t& addr) = 0;
	// �ر�����socket����
	virtual void mark_down_all() = 0;
	// ��ȡ��Ϣת�����д�С
	virtual int get_dispatch_queue_len() = 0;
	// ��ȡ��Ϣת�����������ʱ��
	virtual double get_dispatch_queue_max_age(utime_t now) = 0;
	// ����messengerĬ�ϲ���
	virtual void set_default_policy(Policy p) = 0;
	// ��ȡmessengerĬ�ϲ���
	virtual Policy get_default_policy() = 0;
	// ����messenger����
	virtual void set_policy(int type, Policy p) = 0;
	// ��ȡmessenger����
	virtual Policy get_policy(int t) = 0;
	// ����messenger������ֵ
	virtual void set_policy_throttlers(int type, Throttle* bytes, Throttle* msgs = NULL) = 0;
	// ��ȡsocket���ӣ����û����ʼ����
	virtual Connection* get_connection(const entity_inst_t& dest) = 0;
	// ��ȡ��������
	virtual Connection* get_loopback_connection() = 0;
	// messenger׼������
	virtual void ready() {}
	// ����messenger�������ȼ�
	void set_default_send_priority(int p) { _default_send_priority = p; }
	// ��ȡ�������ȼ�
	int get_default_send_priority() { return _default_send_priority; }
	// ����socket���ȼ�IP_TOS
	void set_socket_priority(int prio) { _socket_priority = prio; }
	// ��ȡsocket���ȼ�
	int get_socket_priority() { return _socket_priority; }
	// ��ȡ����ͨ��ʵ����Ϣ
	const entity_inst_t& get_entity() { return _entity; }
	// ���ñ���ͨ��ʵ����Ϣ
	void set_entity(entity_inst_t e) { _entity = e; }
	// ħ����
	uint32_t get_magic() { return _magic; }
	// ����ħ����
	void set_magic(uint32_t magic) { _magic = magic; }
	// ��ȡ����ͨ�ŵ�ַ��Ϣ
	const entity_addr_t& get_entity_addr() { return _entity._addr; }
	// ��ȡ����ͨ��ʵ����
	const entity_name_t& get_entity_name() { return _entity._name; }
	// ���ñ���ͨ��ʵ����
	void set_entity_name(const entity_name_t m) { _entity._name = m; }

	// ���dispatcher��dispatchers����ͷ��
	void add_dispatcher_head(Dispatcher* d)
	{ 
		bool first = _dispatchers.empty();
		_dispatchers.push_front(d);
		if (d->ms_can_fast_dispatch_any())
		{
			_fast_dispatchers.push_front(d);
		}
		
		// ��Ϊ��������messenger
		if (first)
		{
			ready();
		}
	}
	
	// ���dispatcher��dispatchers����β��
	void add_dispatcher_tail(Dispatcher* d)
	{ 
		bool first = _dispatchers.empty();
		_dispatchers.push_back(d);
		if (d->ms_can_fast_dispatch_any())
		{
			_fast_dispatchers.push_back(d);
		}
		
		// ��Ϊ��������messenger
		if (first)
		{
			ready();
		}
	}
	
	// ���fast_dispatchers�����Ƿ���Դ������Ϣ
	bool ms_can_fast_dispatch(Message* m)
	{
		for (std::list<Dispatcher*>::iterator iter = _fast_dispatchers.begin();
			iter != _fast_dispatchers.end(); ++iter)
		{
			if ((*iter)->ms_can_fast_dispatch(m))
			{
				return true;
			}
		}
			
		return false;
	}
	
	// �����յ�����Ϣ
	void ms_fast_dispatch(Message* m)
	{
		m->set_dispatch_stamp(clock_now());
		for (std::list<Dispatcher*>::iterator iter = _fast_dispatchers.begin();
			iter != _fast_dispatchers.end(); ++iter)
		{
			if ((*iter)->ms_can_fast_dispatch(m))
			{
				(*iter)->ms_fast_dispatch(m);
				return;
			}
		}
	}
	
	// ��ϢԤ����
	void ms_fast_preprocess(Message* m)
	{
		for (std::list<Dispatcher*>::iterator iter = _fast_dispatchers.begin();
			iter != _fast_dispatchers.end(); ++iter)
		{
			(*iter)->ms_fast_preprocess(m);
		}
	}
	
	// �·���Ϣ��dispatcher����
	void ms_deliver_dispatch(Message* m)
	{
		m->set_dispatch_stamp(clock_now());
		for (std::list<Dispatcher*>::iterator iter = _dispatchers.begin(); 
			iter != _dispatchers.end(); ++iter)
		{
			if ((*iter)->ms_dispatch(m))
			{
				return;
			}
		}
		
		m->dec();
	}

	// �·�connect�����dispatcher����
	void ms_deliver_handle_connect(Connection* con)
	{
		for (std::list<Dispatcher*>::iterator iter = _dispatchers.begin();
			iter != _dispatchers.end(); ++iter)
		{
			(*iter)->ms_handle_connect(con);
		}
	}
	
	// �·�connect�����dispatcher����
	void ms_deliver_handle_fast_connect(Connection* con)
	{
		for (std::list<Dispatcher*>::iterator iter = _fast_dispatchers.begin();
			iter != _fast_dispatchers.end(); ++iter)
		{
			(*iter)->ms_handle_fast_connect(con);
		}
	}
	
	// �·�accept�����dispatcher����
	void ms_deliver_handle_accept(Connection* con)
	{
		for (std::list<Dispatcher*>::iterator iter = _dispatchers.begin();
			iter != _dispatchers.end(); ++iter)
		{
			(*iter)->ms_handle_accept(con);
		}
	}
	
	// �·�accept�����dispatcher����
	void ms_deliver_handle_fast_accept(Connection* con)
	{
		for (std::list<Dispatcher*>::iterator iter = _fast_dispatchers.begin();
			iter != _fast_dispatchers.end(); ++iter)
		{
			(*iter)->ms_handle_fast_accept(con);
		}
	}

	// �·��������������dispatcher����
	void ms_deliver_handle_reset(Connection* con)
	{
		for (std::list<Dispatcher*>::iterator iter = _dispatchers.begin();
			iter != _dispatchers.end(); ++iter)
		{
			if ((*iter)->ms_handle_reset(con))
			{
				return;
			}
		}
	}

	void ms_deliver_handle_remote_reset(Connection* con)
	{
		for (std::list<Dispatcher*>::iterator iter = _dispatchers.begin();
			iter != _dispatchers.end(); ++iter)
		{
			(*iter)->ms_handle_remote_reset(con);
		}
	}
	
	// �·�connect refused��Ϣ��dispatcher����
	void ms_deliver_handle_refused(Connection* con)
	{
		for (std::list<Dispatcher*>::iterator iter = _dispatchers.begin();
			iter != _dispatchers.end(); ++iter)
		{
			if ((*iter)->ms_handle_refused(con))
			{
				return;
			}
		}
	}

protected:
	// ���ñ���ͨ��ʵ���ַ
	virtual void set_entity_addr(const entity_addr_t& a) { _entity._addr = a; }

public:
	int _crc_flag;

protected:
	// ��ǰͨ��ʵ��
	entity_inst_t _entity;
	// �Ƿ�������
	bool _started;
	// ħ����
	uint32_t _magic;
	// �������ȼ�
	int _default_send_priority;
	// socket���ȼ�
	int _socket_priority;

private:
	// ��Ϣ�ַ���
	std::list<Dispatcher*> _dispatchers;
	// ������Ϣ�ַ���
	std::list<Dispatcher*> _fast_dispatchers;
};



class PolicyMessenger : public Messenger
{
private:
	Mutex _lock;
	// messengerĬ�ϲ���
	Policy _default_policy;
	std::map<int, Policy> _policy_map;

public:
	PolicyMessenger(entity_name_t name, std::string mname) : Messenger(name)
	{
	}

	~PolicyMessenger()
	{
	}

	Policy get_policy(int t)
	{
		Mutex::Locker locker(_lock);
		std::map<int, Policy>::iterator iter = _policy_map.find(t);
		
		if (iter != _policy_map.end())
		{
			return iter->second;
		}
		else
		{
			return _default_policy;
		}
	}

	Policy get_default_policy()
	{
		Mutex::Locker locker(_lock);
		return _default_policy;
	}
	
	void set_default_policy(Policy p)
	{
		Mutex::Locker locker(_lock);
		_default_policy = p;
	}

	void set_policy(int type, Policy p)
	{
		Mutex::Locker locker(_lock);
		_policy_map[type] = p;
	}

	void set_policy_throttlers(int type, Throttle* byte_throttle, Throttle* msg_throttle)
	{
    	Mutex::Locker locker(_lock);
		std::map<int, Policy>::iterator iter = _policy_map.find(type);
		
		if (iter != _policy_map.end())
		{
			iter->second._throttler_bytes = byte_throttle;
			iter->second._throttler_messages = msg_throttle;
		}
		else
		{
			_default_policy._throttler_bytes = byte_throttle;
			_default_policy._throttler_messages = msg_throttle;
		}
	}

};

#endif
