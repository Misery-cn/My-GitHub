#ifndef _SYS_BASE_H_
#define _SYS_BASE_H_

#include <string>
#include "define.h"

// SYS_NS_BEGIN

// ������Ļ���
class Base
{
public:
	Base();
	virtual ~Base();
	
	std::string description_;
};

// SYS_NS_END

#endif