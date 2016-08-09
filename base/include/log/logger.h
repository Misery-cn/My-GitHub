#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <sys/epoll.h>
#include "atomic.h"
#include "mutex.h"
#include "cond.h"
#include "thread.h"
#include "log.h"
#include "utils.h"

class CLogger;
class CLogThread;


// �����������Logger����
const int LOGGER_NUMBER_MAX = 100;
// һ�ο�����д�����־����������ܳ���IOV_MAX
const int LOG_NUMBER_WRITED_ONCE = 10;

typedef struct
{    
    uint16_t length;	// ��־���ݳ���
    char content[4];	// ��־����
} log_message_t;


class CLogProber
{
public:
    CLogProber();
    virtual ~CLogProber();

    virtual bool execute() = 0;
    int get_fd() const { return pipe_fd_[0]; }

protected:
    void send_signal();
    void read_signal(int signal_number);

protected:
    int pipe_fd_[2];
};

class CLogger: public CLog, public CLogProber
{
public:

	// Ĭ�������ÿ����־����󳤶ȣ�����ᳬ��LOG_LINE_SIZE_MAX������ض�
    CLogger(uint16_t log_line_size = 8192);
private:
    virtual ~CLogger();
public:

	// ������־�������̰߳�ȫ��ֻ�б�һ���̵߳���һ��
 	// ���ú�Logger�����ٱ�ʹ�á�
    void destroy();

	// ��־����ʼ�������̰߳�ȫ��ֻ�б�һ���̵߳���һ��
    void create(const char* log_path, const char* log_filename, uint32_t log_queue_size = 1000);

    bool is_registered() const { return registered_; }
    void set_registered(bool registered) { registered_ = registered; }

public:
    // �Ƿ�����ͬʱ�ڱ�׼����ϴ�ӡ��־
    virtual void enable_screen(bool enabled);
    // �Ƿ������������־����������־����ͨ��������
    virtual void enable_bin_log(bool enabled);
    // �Ƿ����������־��������־����ͨ��������
    virtual void enable_trace_log(bool enabled);
   	// �Ƿ��Զ���һ�к���ӽ�β�ĵ�ţ��������Ѿ��е�ţ��򲻻������
    virtual void enable_auto_adddot(bool enabled);
    // �Ƿ��Զ���ӻ��з�������Ѿ��л��з����򲻻����Զ���ӻ��з�
    virtual void enable_auto_newline(bool enabled);    
    // ������־���𣬸�����־������ͨ����������
    virtual void set_log_level(log_level_t log_level);
    // ���õ����ļ���������С
    virtual void set_single_filesize(uint32_t filesize);
    // ���ñ�����־�ĸ��������Ϊ0���򲻱���
    virtual void set_backup_number(uint16_t backup_number);

    virtual bool enabled_bin();

    virtual bool enabled_detail();
    virtual bool enabled_debug();
    virtual bool enabled_info();
    virtual bool enabled_warn();
    virtual bool enabled_error();
    virtual bool enabled_fatal();
    virtual bool enabled_state();
    virtual bool enabled_trace();

    virtual void log_detail(const char* filename, int lineno, const char* module_name, const char* format, ...) __attribute__((format(printf, 5, 6)));
    virtual void log_debug(const char* filename, int lineno, const char* module_name, const char* format, ...) __attribute__((format(printf, 5, 6)));
    virtual void log_info(const char* filename, int lineno, const char* module_name, const char* format, ...) __attribute__((format(printf, 5, 6)));
    virtual void log_warn(const char* filename, int lineno, const char* module_name, const char* format, ...) __attribute__((format(printf, 5, 6)));
    virtual void log_error(const char* filename, int lineno, const char* module_name, const char* format, ...) __attribute__((format(printf, 5, 6)));
    virtual void log_fatal(const char* filename, int lineno, const char* module_name, const char* format, ...) __attribute__((format(printf, 5, 6)));
    virtual void log_state(const char* filename, int lineno, const char* module_name, const char* format, ...) __attribute__((format(printf, 5, 6)));
    virtual void log_trace(const char* filename, int lineno, const char* module_name, const char* format, ...) __attribute__((format(printf, 5, 6)));

    virtual void log_bin(const char* filename, int lineno, const char* module_name, const char* log, uint16_t size);

private:
    void close_logfile();
    void create_logfile(bool truncate);
    bool need_rotate_file() const { return current_bytes_ > (uint32_t)atomic_read(&max_bytes_); }
    void rotate_file();
    bool need_create_file() const;

private:
    virtual bool execute();
    void create_thread();
    bool prewrite();
    bool batch_write();
    bool single_write();
    void do_log(log_level_t log_level, const char* filename, int lineno, const char* module_name, const char* format, va_list& args);
    void push_log_message(log_message_t* log_message);
    
private:    
    int log_fd_;
    bool auto_adddot_;
    bool auto_newline_;
    uint16_t log_line_size_;
    atomic_t log_level_;
    bool bin_log_enabled_;
    bool trace_log_enabled_;

private:        
    bool registered_; // �Ƿ��Ѿ���ע�ᵽLogThread��
    bool destroying_; // ���ڱ�destroy
    bool screen_enabled_; 
    atomic_t max_bytes_;     
    atomic_t backup_number_;
    uint32_t current_bytes_;
    char log_path_[PATH_MAX];
    char log_filename_[FILENAME_MAX];
    utils::CArrayList<log_message_t*>* log_queue_;
    volatile int waiter_number_; // �ȴ�PUSH��Ϣ���̸߳���
    sys::CCond queue_cond_;
    sys::CMutex queue_lock_; // ����log_queue_����

private: // ����Logger����ͬһ��CLogThread
    static sys::CMutex thread_lock_; // ����log_thread_����
    static CLogThread* log_thread_;
};

class CLogThread : public sys::CThread, public CLogProber
{
public:
    CLogThread();
    virtual ~CLogThread() throw ();

    void remove_logger(CLogger* logger);
    void register_logger(CLogger* logger);
    void inc_log_number() { atomic_inc(&log_number_); }
    void dec_log_number(int number) { atomic_sub(number, &log_number_); }
    int get_log_number() const { return atomic_read(&log_number_); }

private:
    virtual void run();
    virtual void before_stop() throw (sys::CException, sys::CSysCallException);
    virtual void before_start() throw (sys::CException, sys::CSysCallException);
    virtual bool execute();

private:
    void remove_object(CLogProber* log_prober);
    void register_object(CLogProber* log_prober);

private:    
    int epoll_fd_; 
    atomic_t log_number_; // ��������־������
    struct epoll_event* epoll_events_;
};

#endif
