
#ifndef _SYS_EXCEPTION_H_
#define _SYS_EXCEPTION_H_

#include <string>
#include <string.h>
#include <sstream>
#include <errno.h>
#include "const.h"

#define THROW_EXCEPTION(errmsg, errcode) \
	throw ::sys::CException(errmsg, errcode, __FILE__, __LINE__)

#define THROW_SYSCALL_EXCEPTION(errmsg, errcode, syscall) \
    throw ::sys::CSysCallException(errmsg, errcode, __FILE__, __LINE__, syscall)

namespace sys
{

// �����쳣�̳и���
class CException : public std::exception
{
public:
	CException(const char* errmsg, int errcode, const char* filename, int linenum) throw ();
	virtual ~CException() throw ();

	// ��д
	virtual const char* what() const throw ();

	virtual std::string to_string() const;
	
	// ��ȡ�쳣�����ļ���
    const char* get_filename() const throw () { return filename_.c_str(); }

    // ��ȡ�쳣�����к�
    int get_linenumber() const throw () { return linenum_; }

    // ��ȡerrcode
    int get_errcode() const throw () { return errcode_; }

	// ��ȡ������Ϣ
    std::string get_errmessage() const throw () { return errmsg_.c_str(); }
	
protected:
    std::string errmsg_;
    int errcode_;
    std::string filename_;
    int linenum_;
};



// ϵͳ�����쳣
class CSysCallException : public CException
{
public:
	CSysCallException(const char* errmsg, int errcode, const char* filename, int linenum, const char* syscall) throw ();
	virtual ~CSysCallException() throw ();
	
	virtual std::string to_string() const throw ();
	
private:
	std::string syscall_;
};

}

#endif