#ifndef _SYS_BASE_H_
#define _SYS_BASE_H_

#include <string>
#include "define.h"

// SYS_NS_BEGIN

// ������Ļ���
class CBase
{
public:
	CBase();
	virtual ~CBase();
	
	std::string description_;
};

// SYS_NS_END

#endif