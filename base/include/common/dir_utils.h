#ifndef _DIR_UTILS_H_
#define _DIR_UTILS_H_

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "define.h"
#include "exception.h"

// �´����ļ���Ĭ��Ȩ��
#define FILE_DEFAULT_PERM (S_IRUSR|S_IWUSR | S_IRGRP | S_IROTH)
// �´���Ŀ¼��Ĭ��Ȩ��
#define DIRECTORY_DEFAULT_PERM (S_IRWXU | S_IXGRP | S_IXOTH)
// �´�����IPC(����shm��sem��)Ĭ��Ȩ��
#define IPC_DEFAULT_PERM  (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)


UTILS_NS_BEGIN

class CDirUtils
{
public:
    static void list(const std::string& dirpath, std::vector<std::string>* subdir_names, std::vector<std::string>* file_names, std::vector<std::string>* link_names = NULL) throw (sys::CSysCallException);

    static void remove(const std::string& dirpath) throw (sys::CSysCallException);

    static bool exist(const std::string& dirpath) throw (sys::CSysCallException);

    static void create_directory(const char* dirpath, mode_t permissions = DIRECTORY_DEFAULT_PERM);

    static void create_directory_recursive(const char* dirpath, mode_t permissions = DIRECTORY_DEFAULT_PERM);

    static void create_directory_byfilepath(const char* filepath, mode_t permissions = DIRECTORY_DEFAULT_PERM);
};

UTILS_NS_END

#endif
