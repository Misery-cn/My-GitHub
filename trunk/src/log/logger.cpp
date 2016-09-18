#include "logger.h"

CLog::CLog()
{
	init();
}

CLog::~CLog()
{
}

bool CLog::init()
{
	// ��ʼ���ļ�������
	CAppender* appender = new CFileAppender("moth");
	
	run_ = new CLogger();
	run_->set_appender(appender);

	return true;
}

void CLog::do_log(log_level_t level, const char* format, ...)
{
	if (NULL == run_)
	{
		return;
	}

	if (level <= run_->get_log_level())
	{
		CRunLogEvent event(level);
	
		va_list args;
		va_start(args, format);
		CStringUtils::fix_vsnprintf(event.content_, LOG_LINE_SIZE, format, args);
		va_end(args);

		run_->call_appender(&event);
	}
}

CLogger::CLogger()
{
	level_ = Log_Info;
	// �������б�,Ŀǰֻ��һ���ļ�������
	appenders_ = new CArrayList<CAppender*>(1);
}

CLogger::~CLogger()
{
}

void CLogger::call_appender(CLogEvent* event)
{
	if (NULL == event)
	{
		return;
	}

	lock_.lock();
	// �й�
	CMutexGuard guard(lock_);

	for (uint32_t i = 0; i < appenders_->size(); i++)
	{
		CAppender* ap = (*appenders_)[i];
		ap->do_appender(event);
	}
}


