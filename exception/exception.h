#include <string>
#include <string.h>
#include <sstream>
#include "const.h"

// �����쳣�̳и���
class CException
{
public:
	CException(int errcode, const char* filename, int linenumber, const char* tips = NULL);
	virtual ~CException(){}
	
	// ��ȡ�쳣�����ļ���
    const char* get_filename() const { return filename_; }

    // ��ȡ�쳣�����к�
    int get_linenumber() const { return linenumber_; }

    // ��ȡerrno
    int get_errcode() const { return errcode_; }

	// ��ȡ������Ϣ
    std::string get_errmessage() const;

	// ��ȡ�쳣��ʾ
    const char* get_tips() const { return tips_.empty() ? NULL : tips_.c_str(); }

public:
    // ������������д
    virtual std::string to_string() const;
	
private:
	int errcode_;
	int linenumber_;
	char filename_[MAX_FILENAME];
    std::string tips_;
};

// ϵͳ�����쳣
class CSysCallException : public CException
{
public:
	CSysCallException(int errcode, const char* filename, int linenumber, const char* tips = NULL);
	virtual ~CSysCallException();
	
	virtual std::string to_string() const;
	
private:

};