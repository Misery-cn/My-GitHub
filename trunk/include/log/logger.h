#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "mutex.h"
#include "log_appender.h"

class Logger;

// ��־��
class Log
{
public:
	Log();
	virtual ~Log();

	bool init();

	void do_log(log_level_t level, const char* format, ...);
	
	void set_log_level(uint32_t level);

private:
	
	// ����ʱ��־
	Logger* _run;
};


// ��־��¼��
class Logger
{
public:
	Logger();
	virtual ~Logger();

	void set_appender(Appender* appender)
	{
		_appenders->push_back(appender);
	}

	void call_appender(LogEvent* event);

	void set_log_level(log_level_t level)
	{
		_level = level;
	}

	log_level_t get_log_level()
	{
		return _level;
	}

private:
	// ��־����
	log_level_t _level;

	// ģ��ID

	// ��־Ŀ�ĵ��б�
    ArrayList<Appender*>* _appenders;

	// �����ڴ�

	// �̻߳���
	Mutex _lock;
};

#endif
