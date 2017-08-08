#include "logger.h"

Log::Log()
{
	init();
}

Log::~Log()
{
}

bool Log::init()
{
	// ��ʼ���ļ�������
	Appender* appender = new FileAppender("moth");
	
	_run = new Logger();
	
	_run->set_appender(appender);

	return true;
}

void Log::set_log_level(uint32_t level)
{
	if (Log_Off <= level && Log_Debug >= level)
	{
		_run->set_log_level((log_level_t)level);
	}
	else
	{
		// Ĭ��info�������־
		_run->set_log_level(Log_Info);
	}
}

void Log::do_log(log_level_t level, const char* format, ...)
{
	if (NULL == _run)
	{
		return;
	}

	if (level <= _run->get_log_level())
	{
		RunLogEvent event(level);
	
		va_list args;
		va_start(args, format);
		StringUtils::fix_vsnprintf(event._content, LOG_LINE_SIZE, format, args);
		va_end(args);

		_run->call_appender(&event);
	}
}

Logger::Logger()
{
	_level = Log_Info;
	// �������б�,Ŀǰֻ��һ���ļ�������
	_appenders = new ArrayList<Appender*>();
}

Logger::~Logger()
{
}

void Logger::call_appender(LogEvent* event)
{
	if (NULL == event)
	{
		return;
	}

	// �й�
	MutexGuard guard(_lock);

	for (ArrayList<Appender*>::iterator it = _appenders->begin(); !it.end(); ++it)
	{
		(*it)->do_appender(event);
	}
}


