#include "safe_log.h"

static __thread int g_thread_log_fd = -1;

void close_thread_log_fd()
{
    if (-1 != g_thread_log_fd)
    {
        if (-1 == close(g_thread_log_fd))
		{
            fprintf(stderr, "[%d:%lu] SafeLogger close error: %m\n", getpid(), pthread_self());
        }

        g_thread_log_fd = -1;
    }
}

CSafeLogger* create_safe_logger(bool enable_program_path, uint16_t log_line_size) throw (sys::CSysCallException)
{
    std::string log_dirpath = get_log_dirpath(enable_program_path);
    std::string log_filename = get_log_filename();
    CSafeLogger* logger = new CSafeLogger(log_dirpath.c_str(), log_filename.c_str(), log_line_size);

    set_log_level(logger);
    enable_screen_log(logger);
	
    return logger;
}

CSafeLogger* create_safe_logger(const std::string& log_dirpath, const std::string& cpp_filename, uint16_t log_line_size) throw (sys::CSysCallException)
{
    char* cpp_filepath = strdup(cpp_filename.c_str());
    std::string only_filename = basename(cpp_filepath);
    free(cpp_filepath);

    std::string log_filename = utils::CStringUtils::replace_suffix(only_filename, ".log");
    CSafeLogger* logger = new CSafeLogger(log_dirpath.c_str(), log_filename.c_str(), log_line_size);

    set_log_level(logger);
	
    return logger;
}

CSafeLogger::CSafeLogger(const char* log_dir, const char* log_filename, uint16_t log_line_size) throw (sys::CSysCallException)
    : auto_adddot_(false), auto_newline_(true), bin_log_enabled_(false), trace_log_enabled_(false), raw_log_enabled_(false), screen_enabled_(false), log_dir_(log_dir), log_filename_(log_filename)
{
    atomic_set(&max_bytes_, DEFAULT_LOG_FILE_SIZE);
    atomic_set(&log_level_, LOG_LEVEL_INFO);
    atomic_set(&backup_number_, DEFAULT_LOG_FILE_BACKUP_NUMBER);

    // ��֤��־����󳤶Ȳ�С��ָ��ֵ
    log_line_size_ = (log_line_size < LOG_LINE_SIZE_MIN) ? LOG_LINE_SIZE_MIN : log_line_size;
    if (LOG_LINE_SIZE_MAX < log_line_size_)
        log_line_size_ = LOG_LINE_SIZE_MAX;

    log_filepath_ = log_dir_ + std::string("/") + log_filename_;
    log_fd_ = open(log_filepath_.c_str(), O_WRONLY|O_CREAT|O_APPEND, FILE_DEFAULT_PERM);
    if (-1 == log_fd_)
    {
        fprintf(stderr, "[%d:%lu] SafeLogger open %s error: %m\n", getpid(), pthread_self(), log_filepath_.c_str());

        THROW_SYSCALL_EXCEPTION(NULL, errno, "open");
    }
}

CSafeLogger::~CSafeLogger()
{
    if (-1 != log_fd_)
    {
        close(log_fd_);
    }
}

void CSafeLogger::enable_screen(bool enabled)
{
    screen_enabled_ = enabled;
}

void CSafeLogger::enable_bin_log(bool enabled)
{
    bin_log_enabled_ = enabled;
}

void CSafeLogger::enable_trace_log(bool enabled)
{
    trace_log_enabled_ = enabled;
}

void CSafeLogger::enable_raw_log(bool enabled)
{
    raw_log_enabled_ = enabled;
}

void CSafeLogger::enable_auto_adddot(bool enabled)
{
    auto_adddot_ = enabled;
}

void CSafeLogger::enable_auto_newline(bool enabled)
{
    auto_newline_ = enabled;
}

void CSafeLogger::set_log_level(log_level_t log_level)
{
    atomic_set(&log_level_, log_level);
}

void CSafeLogger::set_single_filesize(uint32_t filesize)
{
    uint32_t max_bytes = (LOG_LINE_SIZE_MIN * 10 > filesize) ? LOG_LINE_SIZE_MIN * 10 : filesize;
	
    atomic_set(&max_bytes_, max_bytes);
}

void CSafeLogger::set_backup_number(uint16_t backup_number)
{
    atomic_set(&backup_number_, backup_number);
}

bool CSafeLogger::enabled_bin()
{
    return bin_log_enabled_;
}

bool CSafeLogger::enabled_detail()
{
    return LOG_LEVEL_DETAIL >= atomic_read(&log_level_);
}

bool CSafeLogger::enabled_debug()
{
    return LOG_LEVEL_DEBUG >= atomic_read(&log_level_);
}

bool CSafeLogger::enabled_info()
{
    return LOG_LEVEL_INFO >= atomic_read(&log_level_);
}

bool CSafeLogger::enabled_warn()
{
    return LOG_LEVEL_WARN >= atomic_read(&log_level_);
}

bool CSafeLogger::enabled_error()
{
    return LOG_LEVEL_ERROR >= atomic_read(&log_level_);
}

bool CSafeLogger::enabled_fatal()
{
    return LOG_LEVEL_FATAL >= atomic_read(&log_level_);
}

bool CSafeLogger::enabled_state()
{
    return LOG_LEVEL_STATE >= atomic_read(&log_level_);
}

bool CSafeLogger::enabled_trace()
{
    return trace_log_enabled_;
}

bool CSafeLogger::enabled_raw()
{
    return raw_log_enabled_;
}

void CSafeLogger::log_detail(const char* filename, int lineno, const char* module_name, const char* format, ...)
{
    if (enabled_detail())
    {
        va_list args;
        va_start(args, format);
        VaListGuard vg(args);

        do_log(LOG_LEVEL_DETAIL, filename, lineno, module_name, format, args);
    }
}

void CSafeLogger::log_debug(const char* filename, int lineno, const char* module_name, const char* format, ...)
{
    if (enabled_debug())
    {
        va_list args;
        va_start(args, format);
        VaListGuard vg(args);

        do_log(LOG_LEVEL_DEBUG, filename, lineno, module_name, format, args);
    }
}

void CSafeLogger::log_info(const char* filename, int lineno, const char* module_name, const char* format, ...)
{
    if (enabled_info())
    {
        va_list args;
        va_start(args, format);
        VaListGuard vg(args);

        do_log(LOG_LEVEL_INFO, filename, lineno, module_name, format, args);
    }
}

void CSafeLogger::log_warn(const char* filename, int lineno, const char* module_name, const char* format, ...)
{
    if (enabled_warn())
    {
        va_list args;
        va_start(args, format);
        VaListGuard vg(args);

        do_log(LOG_LEVEL_WARN, filename, lineno, module_name, format, args);
    }
}

void CSafeLogger::log_error(const char* filename, int lineno, const char* module_name, const char* format, ...)
{
    if (enabled_error())
    {
        va_list args;
        va_start(args, format);
        VaListGuard vg(args);

        do_log(LOG_LEVEL_ERROR, filename, lineno, module_name, format, args);
    }
}

void CSafeLogger::log_fatal(const char* filename, int lineno, const char* module_name, const char* format, ...)
{
    if (enabled_fatal())
    {
        va_list args;
        va_start(args, format);
        VaListGuard vg(args);

        do_log(LOG_LEVEL_FATAL, filename, lineno, module_name, format, args);
    }
}

void CSafeLogger::log_state(const char* filename, int lineno, const char* module_name, const char* format, ...)
{
    if (enabled_state())
    {
        va_list args;
        va_start(args, format);
        VaListGuard vg(args);

        do_log(LOG_LEVEL_STATE, filename, lineno, module_name, format, args);
    }
}

void CSafeLogger::log_trace(const char* filename, int lineno, const char* module_name, const char* format, ...)
{
    if (enabled_trace())
    {
        va_list args;
        va_start(args, format);
        VaListGuard vg(args);

        do_log(LOG_LEVEL_TRACE, filename, lineno, module_name, format, args);
    }
}

void CSafeLogger::log_raw(const char* format, ...)
{
    if (enabled_raw())
    {
        va_list args;
        va_start(args, format);
        VaListGuard vg(args);
		
        do_log(LOG_LEVEL_RAW, NULL, -1, NULL, format, args);
    }
}

void CSafeLogger::log_bin(const char* filename, int lineno, const char* module_name, const char* log, uint16_t size)
{
    if (enabled_bin())
    {
        // set_log_length(log, size);
        // log_thread_->push_log(log);
    }
}

int CSafeLogger::get_thread_log_fd() const
{
    if (g_thread_log_fd != log_fd_)
    {
        if (-1 != g_thread_log_fd)
        {
            close(g_thread_log_fd);
        }

        sys::CMutexGuard guard(lock_);
		
        if (-1 == log_fd_)
        {
            g_thread_log_fd = -1;
        }
        else
        {
            g_thread_log_fd = dup(log_fd_);
        }
    }

    return g_thread_log_fd;
}

bool CSafeLogger::need_rotate(int fd) const
{
    return utils::CFileUtils::get_file_size(fd) > static_cast<off_t>(max_bytes_);
}

void CSafeLogger::do_log(log_level_t log_level, const char* filename, int lineno, const char* module_name, const char* format, va_list& args)
{
    int log_real_size = 0;
    utils::ScopedArray<char> log_line(new char[log_line_size_]);

    if (LOG_LEVEL_RAW == log_level)
    {
        // fix_vsnprintf()�ķ���ֵ�����˽�β�����ڵĳ���
        log_real_size = utils::CStringUtils::fix_vsnprintf(log_line.get(), log_line_size_, format, args);
		// ��β������Ҫд����־�ļ���
        --log_real_size;
    }
    else
    {
    	// ÿ����־��ͷ
        std::stringstream log_header;
        char datetime[sizeof("1970-00-00 00:00:00/0123456789")];
        utils::get_formatted_current_datetime(datetime, sizeof(datetime));

        // ��־ͷ���ݣ�[����][�߳�ID/����ID][��־����][ģ����][�����ļ���][�����к�]
        log_header << "[" << datetime << "]"
                   << "[" << pthread_self() << "/" << getpid() << "]"
                   << "[" << get_log_level_name(log_level) << "]";
		
        if (NULL != module_name)
        {
            log_header << "[" << module_name << "]";
        }
		
        log_header << "[" << utils::CStringUtils::extract_filename(filename) << ":" << lineno << "]";

        int m = 0;
		int n = 0;
        // fix_snprintf()�ķ���ֵ��С�����˽�β��
        m = utils::CStringUtils::fix_snprintf(log_line.get(), log_line_size_, "%s", log_header.str().c_str());
        n = utils::CStringUtils::fix_vsnprintf(log_line.get() + m - 1, log_line_size_ - m, format, args);
        log_real_size = m + n - 2;

        // �Ƿ��Զ���ӽ�β�õĵ��
        if (auto_adddot_)
        {
            // ������н�β�ĵ㣬������ӣ������ظ�
            if ('.' != log_line.get()[log_real_size - 1])
            {
                log_line.get()[log_real_size] = '.';
                ++log_real_size;
            }
        }

        // �Ƿ��Զ�����
        if (auto_newline_)
        {
            // �������һ�����з����������
            if ('\n' != log_line.get()[log_real_size - 1])
            {
                log_line.get()[log_real_size] = '\n';
                ++log_real_size;
            }
        }
    }

    // ��ӡ����Ļ��
    if (screen_enabled_)
    {
        (void)write(STDOUT_FILENO, log_line.get(), log_real_size);
    }

    // д��־�ļ�
    int thread_log_fd = get_thread_log_fd();
    if (-1 != thread_log_fd)
    {
        int bytes = write(thread_log_fd, log_line.get(), log_real_size);
        if (-1 == bytes)
        {
            fprintf(stderr, "[%d:%lu] SafeLogger[%d] write error: %m\n", getpid(), pthread_self(), thread_log_fd);
        }
        else if (0 < bytes)
        {
            try
            {
                // �ж��Ƿ���Ҫ����
                if (need_rotate(thread_log_fd))
                {
                    std::string lock_path = log_dir_ + std::string("/.") + log_filename_ + std::string(".lock");
                    utils::FileLocker file_locker(lock_path.c_str(), true);

                    // fd�����ѱ��������̻��̹߳����ˣ�����������Ҫ����openһ��
                    int log_fd = open(log_filepath_.c_str(), O_WRONLY|O_CREAT|O_APPEND, FILE_DEFAULT_PERM);
                    if (-1 == log_fd)
                    {
                        fprintf(stderr, "[%d:%lu] SafeLogger open %s error: %m\n", getpid(), pthread_self(), log_filepath_.c_str());
                    }
                    else
                    {
                        try
                        {
                            if (need_rotate(log_fd))
                            {
                                close(log_fd);
                                rotate_log();
                            }
                            else
                            {
                                // ������߳�ִ���˹�������log_fd_ֵΪ��-1����ֱ��ʹ��
                                // �������������ִ���˹����ˣ���Ӧ��ʹ��log_fd���log_fd_
                                // ���Է�����FileLocker֮�⣬��������log_fd_
                                // ����ÿ���̲߳���ֱ��ʹ��_log_fd�����Ƕ�log_fd_����dup������ֻҪ����log_fd_����
                                sys::CMutexGuard guard(lock_);

                                close(log_fd_);
                                log_fd_ = log_fd;
                            }
                        }
                        catch (sys::CSysCallException& ex)
                        {
                            fprintf(stderr, "[%d:%lu] SafeLogger[%d] rotate error: %s.\n", getpid(), pthread_self(), log_fd, ex.to_string().c_str());
                        }
                    }
                }
            }
            catch (sys::CSysCallException& ex)
            {
                fprintf(stderr, "[%d:%lu] SafeLogger[%d] rotate error: %s\n", getpid(), pthread_self(), thread_log_fd, ex.to_string().c_str());
            }
        }
    }
}

void CSafeLogger::rotate_log()
{
	// ��������ļ�·��������Ŀ¼���ļ���
    std::string new_path;
	// ����ǰ���ļ�·��������Ŀ¼���ļ���
    std::string old_path;

    // ��ʷ����
    for (uint8_t i = backup_number_ - 1; 1 < i; --i)
    {
        new_path = log_dir_ + std::string("/") + log_filename_ + std::string(".") + utils::CStringUtils::any2string(static_cast<int>(i));
        old_path = log_dir_ + std::string("/") + log_filename_ + std::string(".") + utils::CStringUtils::any2string(static_cast<int>(i - 1));

        if (0 == access(old_path.c_str(), F_OK))
        {
            if (-1 == rename(old_path.c_str(), new_path.c_str()))
            {
                fprintf(stderr, "[%d:%lu] SafeLogger rename %s to %s error: %m.\n", getpid(), pthread_self(), old_path.c_str(), new_path.c_str());
            }
        }
        else
        {
            if (errno != ENOENT)
            {
                fprintf(stderr, "[%d:%lu] SafeLogger access %s error: %m.\n", getpid(), pthread_self(), old_path.c_str());
            }
        }
    }

    if (0 < backup_number_)
    {
        // ��ǰ����
        new_path = log_dir_ + std::string("/") + log_filename_ + std::string(".1");
        if (0 == access(log_filepath_.c_str(), F_OK))
        {
            if (-1 == rename(log_filepath_.c_str(), new_path.c_str()))
            {
                fprintf(stderr, "[%d:%lu] SafeLogger rename %s to %s error: %m\n", getpid(), pthread_self(), log_filepath_.c_str(), new_path.c_str());
            }
        }
        else
        {
            if (errno != ENOENT)
            {
                fprintf(stderr, "[%d:%lu] SafeLogger access %s error: %m\n", getpid(), pthread_self(), log_filepath_.c_str());
            }
        }
    }

    // ���´���
    printf("[%d:%lu] SafeLogger create %s\n", getpid(), pthread_self(), log_filepath_.c_str());
    int log_fd = open(log_filepath_.c_str(), O_WRONLY|O_CREAT|O_EXCL, FILE_DEFAULT_PERM);
    if (-1 == log_fd)
    {
        fprintf(stderr, "[%d:%lu] SafeLogger create %s error: %m\n", getpid(), pthread_self(), log_filepath_.c_str());
    }

    sys::CMutexGuard guard(lock_);
    if (-1 == close(log_fd_))
    {
        fprintf(stderr, "[%d:%lu] SafeLogger close %s error: %m.\n", getpid(), pthread_self(), log_filepath_.c_str());
    }
    log_fd_ = log_fd;
}

