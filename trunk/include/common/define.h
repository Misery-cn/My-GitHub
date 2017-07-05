#ifndef _DEFINE_H_
#define _DEFINE_H_

#include <stdarg.h>
#include "acconfig.h"

#define IN
#define OUT

// sys�����ռ��
#define SYS_NS_BEGIN namespace sys {
#define SYS_NS_END }

// util�����ռ��
#define UTILS_NS_BEGIN namespace utils {
#define UTILS_NS_END	}

#define DELETE_P(p) \
	do { \
		if (p) \
		{ \
			delete p; \
			p = NULL; \
		} \
	} while(0);

/*static inline void 
DELETE_P(void* p)
{
	if (p)
	{
	    delete p;
		p = NULL;
	}
}*/

#define DELETE_ARRAY(array) \
	do { \
		if (array) \
		{ \
			delete[] array; \
			array = NULL; \
		} \
	} while(0);



// �����Ա�ڽṹ���е�ƫ����
#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif


#define get_struct_head_address(struct_type, member_name, member_address) \
                ((struct_type *)((char *)(member_address) - offsetof(struct_type, member_name)))


#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(expression) ({     \
	__typeof(expression) __result;              \
	do {                                        \
		__result = (expression);                  \
	} while (__result == -1 && errno == EINTR); \
	__result; })
#endif


#ifdef __cplusplus
# define VOID_TEMP_FAILURE_RETRY(expression) \
   static_cast<void>(TEMP_FAILURE_RETRY(expression))
#else
# define VOID_TEMP_FAILURE_RETRY(expression) \
   do { (void)TEMP_FAILURE_RETRY(expression); } while (0)
#endif


// �ļ�ȫ������ֽ���
#ifdef FILENAME_MAX
#undef FILENAME_MAX
#endif
#define FILENAME_MAX 256
		
// Ŀ¼����ֽ���
#ifdef PATH_MAX
#undef PATH_MAX
#endif		
#define PATH_MAX 256

#ifndef	LINE_MAX
#define	LINE_MAX 1024
#endif


// IO����ͨ�û�������С
#ifdef IO_BUFFER_MAX
#undef IO_BUFFER_MAX
#endif
#define IO_BUFFER_MAX 4096

#define WORK_DIR "WORK_DIR"

#endif
