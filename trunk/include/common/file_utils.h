#ifndef _FILE_UTILS_H_
#define _FILE_UTILS_H_
#include "utils.h"

// UTILS_NS_BEGIN

class FileUtils
{
public:
	// �ļ����ƺ���,�����ļ���С
	// src_fd: �򿪵�Դ�ļ����
	// dst_fd: �򿪵�Ŀ���ļ����
    static size_t file_copy(int src_fd, int dst_fd) throw (SysCallException);
    static size_t file_copy(int src_fd, const char* dst_filename) throw (SysCallException);
    static size_t file_copy(const char* src_filename, int dst_fd) throw (SysCallException);
    static size_t file_copy(const char* src_filename, const char* dst_filename) throw (SysCallException);

	// ��ȡ�ļ��ֽ���
    static off_t get_file_size(int fd) throw (SysCallException);
    static off_t get_file_size(const char* filepath) throw (SysCallException);
    
	// ��ȡ�ļ�32λcrcֵ
	// ע�⣺crc32_file���޸Ķ�д�ļ���ƫ��ֵ
    static uint32_t crc32_file(int fd) throw (SysCallException);
    static uint32_t crc32_file(const char* filepath) throw (SysCallException);
    
	// ��ȡ�ļ�Ȩ��ģʽ
    static uint32_t get_file_mode(int fd) throw (SysCallException);

	// ɾ���ļ�
    static void remove(const char* filepath) throw (SysCallException);

	// �������ļ�
    static void rename(const char* from_filepath, const char* to_filepath) throw (SysCallException);

	static bool exist(const char* filepath);
};

// UTILS_NS_END

#endif
