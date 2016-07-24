#include "util.h"


bool get_work_dir(char* buf)
{
	if (NULL == buf)
	{
		return false;
	}
	
    char* workdir = NULL;
    workdir = getenv(WORK_DIR);
    if (NULL == workdir)
    {
        return false;
    }
    else
    {
        strcpy(buf, workdir);
        unsigned int len = strlen(buf);
		// �������������Ҫ��'/'
        if (buf[len - 1] == '/')
		{
			buf[len - 1] = 0;
		}
		
        return true;
    }
}