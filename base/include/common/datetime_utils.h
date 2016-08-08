#ifndef _DATETIME_UTILS_H_
#define _DATETIME_UTILS_H_

#include <time.h>
#include <string>
#include "define.h"

UTILS_NS_BEGIN

class CDatetimeUtils
{
public:
    // �ж��Ƿ�Ϊͬһ��
    static bool is_same_day(time_t t1, time_t t2);

    // �ж�ָ������Ƿ�Ϊ����
    static bool is_leap_year(int year);
    
    // ��time_tֵת����������20160114����������
    static uint32_t time2date(time_t t);

    // �õ���ǰ���ں�ʱ�䣬���ظ�ʽ�ɲ���format������Ĭ��Ϊ: YYYY-MM-DD HH:SS:MM
	// datetime_buffer:�����洢��ǰ���ں�ʱ��Ļ�����
	// datetime_buffer_size:datetime_buffer���ֽڴ�С����Ӧ����С��YYYY-MM-DD HH:SS:MM��
    static void get_current_datetime(char* datetime_buffer, size_t datetime_buffer_size, const char* format = "%04d-%02d-%02d %02d:%02d:%02d");
    static std::string get_current_datetime(const char* format = "%04d-%02d-%02d %02d:%02d:%02d");

	// �õ���ǰ���ڣ����ظ�ʽ�ɲ���format������Ĭ��Ϊ: YYYY-MM-DD
	// date_buffer: �����洢��ǰ���ڵĻ�����
	// date_buffer_size: date_buffer���ֽڴ�С����Ӧ����С��YYYY-MM-DD��
    static void get_current_date(char* date_buffer, size_t date_buffer_size, const char* format = "%04d-%02d-%02d");
    static std::string get_current_date(const char* format = "%04d-%02d-%02d");

	// �õ���ǰʱ�䣬���ظ�ʽ�ɲ���format������Ĭ��Ϊ: HH:SS:MM
	// time_buffer: �����洢��ǰʱ��Ļ�����
	// time_buffer_size: time_buffer���ֽڴ�С����Ӧ����С��HH:SS:MM��
    static void get_current_time(char* time_buffer, size_t time_buffer_size, const char* format = "%02d:%02d:%02d");
    static std::string get_current_time(const char* format = "%02d:%02d:%02d");

    // �õ���ǰ���ں�ʱ��ṹ
    static void get_current_datetime_struct(struct tm* current_datetime_struct);

    // ���ں�ʱ��
    static void to_current_datetime(struct tm* current_datetime_struct, char* datetime_buffer, size_t datetime_buffer_size, const char* format = "%04d-%02d-%02d %02d:%02d:%02d");
    static std::string to_current_datetime(struct tm* current_datetime_struct, const char* format = "%04d-%02d-%02d %02d:%02d:%02d");

    // ������
    static void to_current_date(struct tm* current_datetime_struct, char* date_buffer, size_t date_buffer_size, const char* format = "%04d-%02d-%02d");
    static std::string to_current_date(struct tm* current_datetime_struct, const char* format = "%04d-%02d-%02d");

    // ��ʱ��
    static void to_current_time(struct tm* current_datetime_struct, char* time_buffer, size_t time_buffer_size, const char* format = "%02d:%02d:%02d");
    static std::string to_current_time(struct tm* current_datetime_struct, const char* format = "%02d:%02d:%02d");

    // �����
    static void to_current_year(struct tm* current_datetime_struct, char* year_buffer, size_t year_buffer_size);
    static std::string to_current_year(struct tm* current_datetime_struct);

    // ���·�
    static void to_current_month(struct tm* current_datetime_struct, char* month_buffer, size_t month_buffer_size);
    static std::string to_current_month(struct tm* current_datetime_struct);

    // ����
    static void to_current_day(struct tm* current_datetime_struct, char* day_buffer, size_t day_buffer_size);
    static std::string to_current_day(struct tm* current_datetime_struct);

    // ��Сʱ
    static void to_current_hour(struct tm* current_datetime_struct, char* hour_buffer, size_t hour_buffer_size);
    static std::string to_current_hour(struct tm* current_datetime_struct);

    // ������
    static void to_current_minite(struct tm* current_datetime_struct, char* minite_buffer, size_t minite_buffer_size);
    static std::string to_current_minite(struct tm* current_datetime_struct);

    // ������
    static void to_current_second(struct tm* current_datetime_struct, char* second_buffer, size_t second_buffer_size);
    static std::string to_current_second(struct tm* current_datetime_struct);
	
	// ��һ���ַ���ת��������ʱ���ʽ
	// str:���ϡ�YYYY-MM-DD HH:MM:SS����ʽ������ʱ��
	// datetime_struct:�洢ת���������ʱ��
	// ת���ɹ�����true�����򷵻�false
    static bool datetime_struct_from_string(const char* str, struct tm* datetime_struct);
    static bool datetime_struct_from_string(const char* str, time_t* datetime);

    // ���ء�YYYY-MM-DD HH:MM:SS����ʽ������ʱ��
    static std::string to_string(time_t datetime, const char* format = "%04d-%02d-%02d %02d:%02d:%02d");

    // ���ظ�ʽ�ɲ���format������Ĭ��Ϊ��YYYY-MM-DD HH:MM:SS����ʽ������ʱ��
    static std::string to_datetime(time_t datetime, const char* format = "%04d-%02d-%02d %02d:%02d:%02d");

    // ���ظ�ʽ�ɲ���format������Ĭ��Ϊ��YYYY-MM-DD����ʽ������ʱ��
    static std::string to_date(time_t datetime, const char* format = "%04d-%02d-%02d");

    // ���ظ�ʽ�ɲ���format������Ĭ��Ϊ��HH:MM:SS����ʽ������ʱ��
    static std::string to_time(time_t datetime, const char* format = "%02d:%02d:%02d");

    // �õ���ǰ��΢��ֵ
    static int64_t get_current_microseconds();
};

// ���ؽ���
extern std::string today(const char* format = "%04d-%02d-%02d");
// ��������
extern std::string yesterday(const char* format = "%04d-%02d-%02d");
// ��������
extern std::string tomorrow(const char* format = "%04d-%02d-%02d");

// ȡ�ø�ʽ���ĵ�ǰ����ʱ�䣬
// ���with_millisecondsΪtrue���򷵻ظ�ʽΪ��YYYY-MM-DD hh:mm:ss/ms������ms�Ϊ10λ���֣�
// ���with_millisecondsΪfalse���򷵻ظ�ʽΪ��YYYY-MM-DD hh:mm:ss��
//
// ���with_millisecondsΪtrue��datetime_buffer_size�Ĵ�С����С��sizeof("YYYY-MM-DD hh:mm:ss/0123456789")��
// ���with_millisecondsΪfalse��datetime_buffer_size�Ĵ�С����С��sizeof("YYYY-MM-DD hh:mm:ss")��
extern void get_formatted_current_datetime(char* datetime_buffer, size_t datetime_buffer_size, bool with_milliseconds = true);

// ���with_millisecondsΪfalse���򷵻�ͬCDatetimeUtils::get_current_datetime()
// ���with_millisecondsΪtrue���򷵻�Ϊ��YYYY-MM-DD hh:mm:ss/milliseconds
extern std::string get_formatted_current_datetime(bool with_milliseconds = true);

UTILS_NS_END

#endif
