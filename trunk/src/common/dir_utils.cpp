#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "dir_utils.h"
#include "string_utils.h"

// UTILS_NS_BEGIN

void CDirUtils::list(const std::string& dirpath, std::vector<std::string>* subdir_names, 
						std::vector<std::string>* file_names, std::vector<std::string>* link_names) throw (CSysCallException)
{
    DIR* dir = opendir(dirpath.c_str());
    if (NULL == dir)
    {
        THROW_SYSCALL_EXCEPTION(NULL, errno, "opendir");
    }

    for (;;)
    {
        errno = 0;
        struct dirent* ent = readdir(dir);
        if (NULL == ent)
        {
            if (0 != errno)
            {
                int errcode = errno;
                if (EACCES == errcode)
                {
                    // ������Ȩ�޵�
                    continue;
                }

                closedir(dir);
                THROW_SYSCALL_EXCEPTION(NULL, errcode, "readdir");
            }

            break;
        }

        // �ų���ǰĿ¼�͸�Ŀ¼
        if ((0 == strcmp(ent->d_name, "."))
			|| (0 == strcmp(ent->d_name, "..")))
        {
            continue;
        }

        if (DT_DIR == ent->d_type)
        {
            if (NULL != subdir_names)
            {
                subdir_names->push_back(ent->d_name);
            }
        }
        else if (DT_REG == ent->d_type)
        {
            if (NULL != file_names)
            {
                file_names->push_back(ent->d_name);
            }
        }
        else if (DT_LNK == ent->d_type)
        {
            if (NULL != link_names)
            {
                link_names->push_back(ent->d_name);
            }
        }
    }

    closedir(dir);
}

void CDirUtils::remove(const std::string& dirpath) throw (CSysCallException)
{
    if (-1 == rmdir(dirpath.c_str()))
    {
        THROW_SYSCALL_EXCEPTION(NULL, errno, "rmdir");
    }
}

bool CDirUtils::exist(const std::string& dirpath) throw (CSysCallException)
{
    struct stat buf;

    if (-1 == stat(dirpath.c_str(), &buf))
    {
    	// Ŀ¼������
        if (ENOENT == errno)
        {
            return false;
        }
		// ����һ��Ŀ¼
        if (ENOTDIR == errno)
        {
            return false;
        }

        THROW_SYSCALL_EXCEPTION(NULL, errno, "stat");
    }

    return S_ISDIR(buf.st_mode);
}

void CDirUtils::create_directory(const char* dirpath, mode_t permissions)
{
    if (-1 == mkdir(dirpath, permissions))
    {
        if (errno != EEXIST)
        {
            THROW_SYSCALL_EXCEPTION(NULL, errno, "mkdir");
        }
    }
}

void CDirUtils::create_directory_recursive(const char* dirpath, mode_t permissions)
{
    char* slash;
    char* pathname = strdupa(dirpath); // _GNU_SOURCE
    char* pathname_p = pathname;

    // ���˵�ͷ����б��
    while ('/' == *pathname_p)
	{
		++pathname_p;
    }

    for (;;)
    {
        slash = strchr(pathname_p, '/');
        if (NULL == slash) // Ҷ��Ŀ¼
        {
            if (0 == mkdir(pathname, permissions))
			{
				break;
            }
			
            if (EEXIST == errno)
			{
				break;
            }

            THROW_SYSCALL_EXCEPTION(NULL, errno, "mkdir");
        }

        *slash = '\0';
        if ((-1 == mkdir(pathname, permissions)) && (EEXIST != errno))
        {
            THROW_SYSCALL_EXCEPTION(NULL, errno, "mkdir");
        }

        *slash++ = '/';
        while ('/' == *slash)
		{
			++slash; // ���˵�������б��
        }
        pathname_p = slash;
    }
}

void CDirUtils::create_directory_byfilepath(const char* filepath, mode_t permissions)
{
    std::string dirpath = CStringUtils::extract_dirpath(filepath);
    create_directory_recursive(dirpath.c_str(),  permissions);
}

// UTILS_NS_END