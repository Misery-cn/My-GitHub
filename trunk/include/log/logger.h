#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "mutex.h"
#include "log_appender.h"


class CLogger;

// ��־��
class CLog
{
public:
	CLog();
	virtual ~CLog();

	bool init();

	void do_log(log_level_t level, const char* format, ...);

private:
	
	// ����ʱ��־
	CLogger* _run;
};


// ��־��¼��
class CLogger
{
public:
	CLogger();
	virtual ~CLogger();

	void set_appender(CAppender* appender)
	{
		_appenders->push_back(appender);
	}

	void call_appender(CLogEvent* event);

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
    CArrayList<CAppender*>* _appenders;

	// �����ڴ�

	// �̻߳���
	CMutex _lock;
};

#endif
