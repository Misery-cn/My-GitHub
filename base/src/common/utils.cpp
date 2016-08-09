#include <dirent.h>
#include <execinfo.h> // backtrace��backtrace_symbols����
#include <features.h> // feature_test_macros
#include <ftw.h> // ftw
#include <libgen.h> // dirname&basename
#include <sys/time.h>
#include <sys/prctl.h> // prctl
#include <sys/resource.h>
#include <time.h>
#include "utils.h"
#include "string_utils.h"


#ifndef PR_SET_NAME
#define PR_SET_NAME 15
#endif
 
#ifndef PR_GET_NAME
#define PR_GET_NAME 16
#endif

UTILS_NS_BEGIN

// ��set_program_title���
static char *g_arg_start = NULL;
static char *g_arg_end   = NULL;
static char *g_env_start = NULL;

void CUtils::millisleep(uint32_t millisecond)
{
    struct timespec ts = { millisecond / 1000, (millisecond % 1000) * 1000000 };
    while ((-1 == nanosleep(&ts, &ts)) && (EINTR == errno));
}

std::string CUtils::get_error_message(int errcode)
{
    return sys::Error::to_string(errcode);
}

std::string CUtils::get_last_error_message()
{
    return sys::Error::to_string();
}

int CUtils::get_last_error_code()
{
    return sys::Error::code();
}

int CUtils::get_current_process_id()
{
    return getpid();
}

std::string CUtils::get_program_path()
{
    char buf[1024] = {0};

    buf[0] = '\0';
    int r = readlink("/proc/self/exe", buf, sizeof(buf)-1);
    if (0 < r)
    {
        buf[r] = '\0';

#if 0 // ������ηϴ��룬���μ�deleted�Ĵ��ڣ�����������ֻȡ·�����֣����Բ��������Ĵ���
        if (!strcmp(buf + r - 10," (deleted)"))
        {
            buf[r - 10] = '\0';
        }
#else

        // ȥ���ļ�������
        char* end = strrchr(buf, '/');
        if (NULL == end)
        {
            buf[0] = 0;
        }
        else
        {
            *end = '\0';
        }
#endif
    }

    return buf;
}

std::string CUtils::get_filename(int fd)
{
	char path[PATH_MAX] = {0};
	char filename[FILENAME_MAX] = {'\0'};
	
	snprintf(path, sizeof(path), "/proc/%d/fd/%d", getpid(), fd);
	if (-1 == readlink(path, filename, sizeof(filename)))
	{
		filename[0] = '\0';
	}
    
	return filename;
}

// �⺯����char *realpath(const char *path, char *resolved_path);
//         char *canonicalize_file_name(const char *path);
std::string CUtils::get_full_directory(const char* directory)
{
    std::string full_directory;
    DIR* dir = opendir(directory);
    if (NULL != dir)
    {
        int fd = dirfd(dir);
        if (-1 != fd)
        {
            full_directory = get_filename(fd);
        }

        closedir(dir);
    }
 
    return full_directory;
}

// ��غ�����
// get_nprocs()��������sys/sysinfo.h
// sysconf(_SC_NPROCESSORS_CONF)
// sysconf(_SC_NPROCESSORS_ONLN)
uint16_t CUtils::get_cpu_number()
{
	FILE* fp = fopen("/proc/cpuinfo", "r");
	if (NULL == fp)
	{
		return 1;
	}
	
	char line[LINE_MAX] = {0};
	uint16_t cpu_number = 0;
    CloseHelper<FILE*> ch(fp);

	while (fgets(line, sizeof(line) - 1, fp))
	{ 
		char* name = line;
		char* value = strchr(line, ':');
		
		if (NULL == value)
		{
			continue;
		}

		*value++ = 0;		
		if (0 == strncmp("processor", name, sizeof("processor") - 1))
		{
			 if (!utils::CStringUtils::string2uint16(value, cpu_number))
             {
                 return 0;
             }
		}
	}

	return (cpu_number + 1);
}

bool CUtils::get_backtrace(std::string& call_stack)
{
	// ���֡����
    const int frame_number_max = 20;
	// ֡��ַ����
    void* address_array[frame_number_max];

    // real_frame_number��ֵ���ᳬ��frame_number_max�����������frame_number_max�����ʾ����֡���ض���
    int real_frame_number = backtrace(address_array, frame_number_max);

    char** symbols_strings = backtrace_symbols(address_array, real_frame_number);
    if (NULL == symbols_strings)
    {
        return false;
    }
    else if (2 > real_frame_number) 
    {
        free(symbols_strings);
        return false;
    }

	// symbols_strings[0]Ϊget_backtrace�Լ�������ʾ
    call_stack = symbols_strings[1];
    for (int i = 2; i < real_frame_number; ++i)
    {
        call_stack += std::string("\n") + symbols_strings[i];
    }

    free(symbols_strings);
    return true;
}

// Ŀ¼��С
static off_t dirsize;
int _du_fn(const char *fpath, const struct stat *sb, int typeflag)
{   
    if (FTW_F == typeflag)
    {
        dirsize += sb->st_size;
    }

    return 0;
}

off_t CUtils::du(const char* dirpath)
{
    dirsize = 0;
    if (0 != ftw(dirpath, _du_fn, 0))
	{
		return -1;
    }

    return dirsize;
}

int CUtils::get_page_size()
{
    // sysconf(_SC_PAGE_SIZE);
    // sysconf(_SC_PAGESIZE);
    return getpagesize();
}

int CUtils::get_fd_max()
{
    // sysconf(_SC_OPEN_MAX);
    return getdtablesize();
}

bool CUtils::is_file(int fd)
{
    struct stat buf;
    if (-1 == fstat(fd, &buf))
    {
        THROW_SYSCALL_EXCEPTION(NULL, errno, "fstat");
    }

    return S_ISREG(buf.st_mode);
}

bool CUtils::is_file(const char* path)
{
    struct stat buf;
    if (-1 == stat(path, &buf))
    {
        THROW_SYSCALL_EXCEPTION(NULL, errno, "stat");
    }

    return S_ISREG(buf.st_mode);
}

bool CUtils::is_link(int fd)
{
    struct stat buf;
    if (-1 == fstat(fd, &buf))
    {
        THROW_SYSCALL_EXCEPTION(NULL, errno, "fstat");
    }

    return S_ISLNK(buf.st_mode);
}

bool CUtils::is_link(const char* path)
{
    struct stat buf;
    if (-1 == stat(path, &buf))
    {
        THROW_SYSCALL_EXCEPTION(NULL, errno, "stat");
    }

    return S_ISLNK(buf.st_mode);
}

bool CUtils::is_directory(int fd)
{
    struct stat buf;
    if (-1 == fstat(fd, &buf))
    {
        THROW_SYSCALL_EXCEPTION(NULL, errno, "fstat");
    }

    return S_ISDIR(buf.st_mode);
}

bool CUtils::is_directory(const char* path)
{
    struct stat buf;
    if (-1 == stat(path, &buf))
    {
        THROW_SYSCALL_EXCEPTION(NULL, errno, "stat");
    }

    return S_ISDIR(buf.st_mode);
}

void CUtils::enable_core_dump(bool enabled, int core_file_size)
{    
    if (enabled)
    {
        struct rlimit rlim;
        rlim.rlim_cur = (0 > core_file_size) ? RLIM_INFINITY : core_file_size;
        rlim.rlim_max = rlim.rlim_cur;

        if (-1 == setrlimit(RLIMIT_CORE, &rlim))
        {
            THROW_SYSCALL_EXCEPTION(NULL, errno, "setrlimit");
        }
    }       
    
    if (-1 == prctl(PR_SET_DUMPABLE, enabled ? 1 : 0))
    {
        THROW_SYSCALL_EXCEPTION(NULL, errno, "prctl");
    }
}

std::string CUtils::get_program_long_name()
{
    //#define _GNU_SOURCE
    //#include <errno.h>
    return program_invocation_name;
}

// ���������set_process_title()��
// ��ͨ��program_invocation_short_name����ȡ����Ԥ�ڵ�ֵ���������ص��ǿ�
std::string CUtils::get_program_short_name()
{
    //#define _GNU_SOURCE
    //#include <errno.h>
    return program_invocation_short_name;
}

std::string CUtils::get_filename(const std::string& filepath)
{
    // basename�Ĳ����������룬Ҳ�����������������Ҫtmp_filepath
    std::string tmp_filepath(filepath);
    return basename(const_cast<char*>(tmp_filepath.c_str())); // #include <libgen.h>
}

std::string CUtils::get_dirpath(const std::string& filepath)
{
    // basename�Ĳ����������룬Ҳ�����������������Ҫtmp_filepath
    std::string tmp_filepath(filepath);
    return dirname(const_cast<char*>(tmp_filepath.c_str())); // #include <libgen.h>
}

void CUtils::set_process_name(const std::string& new_name)
{
    if (!new_name.empty())
    {
        if (-1 == prctl(PR_SET_NAME, new_name.c_str()))
        {
            THROW_SYSCALL_EXCEPTION(NULL, errno, "prctl");
        }
    }
}

void CUtils::set_process_name(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    VaListGuard guard(args);

    char name[NAME_MAX] = {0};
    vsnprintf(name, sizeof(name), format, args);
    set_process_name(std::string(name));
}

// �ο���http://www.stamhe.com/?p=416
void CUtils::init_process_title(int argc, char *argv[])
{
    g_arg_start = argv[0];
    g_arg_end = argv[argc - 1] + strlen(argv[argc - 1]) + 1;
    g_env_start = environ[0];
}

void CUtils::set_process_title(const std::string &new_title)
{
    if (!new_title.empty())
    {
        size_t new_title_len = new_title.length();

        // �µ�title���ϵĳ�
        if ((static_cast<size_t>(g_arg_end-g_arg_start) < new_title_len)
			&& (g_env_start == g_arg_end))
        {
            char* env_end = g_env_start;
            for (int i = 0; environ[i]; ++i)
            {
                if (env_end != environ[i])
                {
                    break;
                }

                env_end = environ[i] + strlen(environ[i]) + 1;
                environ[i] = strdup(environ[i]);
            }

            g_arg_end = env_end;
            g_env_start = NULL;
        }

        size_t len = g_arg_end - g_arg_start;
        if (len == new_title_len)
        {
             strcpy(g_arg_start, new_title.c_str());
        }
        else if(new_title_len < len)
        {
            strcpy(g_arg_start, new_title.c_str());
            memset(g_arg_start+new_title_len, 0, len-new_title_len);

#if 0 // ��ʵ�ʵĲ�����������������������䣬�����ps u�����COMMANDһ��Ϊ��
            // ���µ�title��ԭtitle��ʱ��
            // ���argv[0]�ֶ�ʱ����Ϊ���argv[0]���ĺ�Σ�ǰ�����0
            memset(g_arg_start, 0, len);
            strcpy(g_arg_start+(len - new_title_len), new_title.c_str());
#endif
        }
        else
        {
            *(char *)mempcpy(g_arg_start, new_title.c_str(), len-1) = '\0';
        }

        if (NULL != g_env_start)
        {
            char *p = strchr(g_arg_start, ' ');
            if (NULL != p)
            {
                *p = '\0';
            }
        }
    }
}

void CUtils::set_process_title(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    VaListGuard guard(args);

    char title[PATH_MAX] = {0};
    vsnprintf(title, sizeof(title), format, args);
    set_process_title(std::string(title));
}

void CUtils::common_pipe_read(int fd, char** buffer, int32_t* buffer_size)
{
	int ret = 0;
	int32_t size = 0;

	// ��һ��whileѭ����ȡ��С
	while (true)
	{
		ret = read(fd, &size, sizeof(size));
		if ((-1 == ret) && (EINTR == errno))
		{
			continue;
		}
		
		if (-1 == ret)
		{
		    THROW_SYSCALL_EXCEPTION(NULL, errno, "read");
		}

		break;
	}

	*buffer_size = size;
	*buffer = new char[size];
	char* bufferp = *buffer;

	// �ڶ���whileѭ�����ݴ�С��ȡ����
	while (0 < size)
	{
		ret = read(fd, bufferp, size);
		if ((0 == ret) || (ret == size))
		{
			break;
		}
		
		if ((-1 == ret) && (EINTR == errno))
		{
			continue;
		}
		
		if (-1 == ret)
		{
			delete *buffer;
			THROW_SYSCALL_EXCEPTION(NULL, errno, "read");
		}

		bufferp += ret;
		size -= ret;
	}
}

void CUtils::common_pipe_write(int fd, const char* buffer, int32_t buffer_size)
{
	int ret = 0;
	int32_t size = buffer_size;

	// ��һ��whileѭ��д���С
	while (true)
	{
		ret = write(fd, &size, sizeof(size));
		if ((-1 == ret) && (EINTR == errno))
		{
			continue;
		}
		
		if (-1 == ret)
		{
		    THROW_SYSCALL_EXCEPTION(NULL, errno, "write");
		}

		break;
	}

	const char* bufferp = buffer;

	// �ڶ���whileѭ�����ݴ�Сд������
	while (0 < size)
	{
		ret = write(fd, bufferp, size);
		if ((-1 == ret) && (EINTR == errno))
		{
			continue;
		}
		
		if (-1 == ret)
		{
		    THROW_SYSCALL_EXCEPTION(NULL, errno, "write");
		}

		size -= ret;
		bufferp += ret;
	}
}

UTILS_NS_END