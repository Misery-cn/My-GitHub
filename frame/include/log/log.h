#ifndef _SYS_LOG_H_
#define _SYS_LOG_H_
#include "define.h"
#include "const.h"

// ��־����С����
const int LOG_LINE_SIZE_MIN	= 256;
// ��־����󳤶�(32K),����ܳ���64K,��Ϊʹ��2�ֽ��޷��������洢��
const int LOG_LINE_SIZE_MA	= 32768;
// Ĭ�ϵĵ�����־�ļ���С(100MB)
const int DEFAULT_LOG_FILE_SIZE	= 100 * M;
// Ĭ�ϵ���־�ļ����ݸ���
const int DEFAULT_LOG_FILE_BACKUP_NUMBER = 10;

// ��־����
typedef enum
{
    LOG_LEVEL_DETAIL = 0,
    LOG_LEVEL_DEBUG  = 1,
    LOG_LEVEL_INFO   = 2,
    LOG_LEVEL_WARN   = 3,
    LOG_LEVEL_ERROR  = 4,
    LOG_LEVEL_FATAL  = 5,    
    LOG_LEVEL_STATE  = 6,
    LOG_LEVEL_TRACE  = 7
} LOG_LEVEL;

// ��־�ṹ
typedef struct
{
	// ��־���ݳ���
    uint16_t length;
	// ��־����
    char content[4];
} log_message_t;


namespace sys
{

// ��־����
class CLog
{
public:
	CLog();
	virtual ~CLog(){}
	
private:

};

// 
class CLogProber
{
public:
	CLogProber();
	virtual ~CLogProber();
};


class CLogger : public CLogProber, public CLog
{
public:
	CLogger();
	~CLogger();
};

}

#endif