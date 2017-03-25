#ifndef _LOG_APPENDER_H_
#define _LOG_APPENDER_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "thread.h"
#include "string_utils.h"
#include "dir_utils.h"
#include "file_utils.h"
#include "datetime_utils.h"

typedef enum
{
    Log_Off = 0,
    Log_Serious = 1,
    Log_Error = 2,
    Log_Info = 3,
    Log_Debug = 4
} log_level_t;

// 
const int LOG_LINE_SIZE = 512;
// �����־�ļ�10M��С
const int MAX_LOG_FILE_SIZE = 10 * M;
// ���10����־�ļ�
const int MAX_LOG_FILE_SEQ = 10;


class CLogEvent
{
public:
	CLogEvent();
	virtual ~CLogEvent();

	virtual std::string format() = 0;

public:
	log_level_t _level;
	
	char _content[LOG_LINE_SIZE];
};

// ����ʱ��־
class CRunLogEvent : public CLogEvent
{
public:
	CRunLogEvent(log_level_t level);
	virtual ~CRunLogEvent();

	std::string format();
};


class CAppender
{
public:
	CAppender();
	virtual ~CAppender();

	virtual void do_appender(CLogEvent* logevent) = 0;
	
protected:
	// ������

};


class CFileAppender : public CAppender
{
public:
	CFileAppender(const char* filename);
	virtual ~CFileAppender();

	void init();

	// ��鵱ǰ��־�ļ�,�Ƿ���Ҫ���´���
	void check_log_file();

	void create_log_file();

	// д�ļ�
	void do_appender(CLogEvent* logevent);

private:
	int _log_fd;
	// ��ǰ�ļ����
	int _log_file_seq;
	// ��־�ļ���
	char _log_file_name[FILENAME_MAX];
	// ��ǰ��־�ļ���
	char _current_file_name[FILENAME_MAX];
	// ��־·��
	char _log_path[PATH_MAX];
};

#endif