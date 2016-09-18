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
	CLogger* run_;
};


// ��־��¼��
class CLogger
{
public:
	CLogger();
	virtual ~CLogger();

	void set_appender(CAppender* appender)
	{
		appenders_->push_back(appender);
	}

	void call_appender(CLogEvent* event);

	void set_log_level(log_level_t level)
	{
		level_ = level;
	}

	log_level_t get_log_level()
	{
		return level_;
	}

private:
	// ��־����
	log_level_t level_;

	// ģ��ID

	// ��־Ŀ�ĵ��б�
    CArrayList<CAppender*>* appenders_;

	// �����ڴ�

	// �̻߳���
	CMutex lock_;
};

#endif
