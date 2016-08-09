#ifndef _FILE_UTILS_H_
#define _FILE_UTILS_H_
#include "utils.h"

UTILS_NS_BEGIN

class CFileUtils
{
public:
	// �ļ����ƺ���,�����ļ���С
	// src_fd: �򿪵�Դ�ļ����
	// dst_fd: �򿪵�Ŀ���ļ����
    static size_t file_copy(int src_fd, int dst_fd) throw (sys::CSysCallException);
    static size_t file_copy(int src_fd, const char* dst_filename) throw (sys::CSysCallException);
    static size_t file_copy(const char* src_filename, int dst_fd) throw (sys::CSysCallException);
    static size_t file_copy(const char* src_filename, const char* dst_filename) throw (sys::CSysCallException);

	// ��ȡ�ļ��ֽ���
    static off_t get_file_size(int fd) throw (sys::CSysCallException);
    static off_t get_file_size(const char* filepath) throw (sys::CSysCallException);
    
	// ��ȡ�ļ�32λcrcֵ
	// ע�⣺crc32_file���޸Ķ�д�ļ���ƫ��ֵ
    static uint32_t crc32_file(int fd) throw (sys::CSysCallException);
    static uint32_t crc32_file(const char* filepath) throw (sys::CSysCallException);
    
	// ��ȡ�ļ�Ȩ��ģʽ
    static uint32_t get_file_mode(int fd) throw (sys::CSysCallException);

	// ɾ���ļ�
    static void remove(const char* filepath) throw (sys::CSysCallException);

	// �������ļ�
    static void rename(const char* from_filepath, const char* to_filepath) throw (sys::CSysCallException);
};

UTILS_NS_END

#endif
