#include "exception.h"

typedef struct
{
	int32_t _fd;	// �򿪵��ļ�������,����ǵ����ߴ���Ӧ�е������Լ��ر�
	void* _addr;	// �ļ�ӳ�䵽�ڴ�ĵ�ַ
	size_t _len;	// ӳ�䵽�ڴ�Ĵ�С
} mmap_t;

class MMap
{
public:
		// ֻ����ʽӳ��
		// size:��Ҫӳ�䵽�ڴ�Ĵ�С.���Ϊ0Ĭ��ӳ�������ļ�
		// offet:ӳ��ƫ����
		// size_max:���ӳ���ֽ�
		static mmap_t* mmap_read_only(int fd, size_t size = 0, size_t offset = 0, size_t size_max = 0) throw (SysCallException);

		static mmap_t* mmap_read_only(const char* filename, size_t size_max = 0) throw (SysCallException);

		// ֻд��ʽӳ��
		static mmap_t* mmap_write_only(int fd, size_t size = 0, size_t offset = 0, size_t size_max = 0) throw (SysCallException);

		static mmap_t* mmap_write_only(const char* filename, size_t size_max = 0) throw (SysCallException);

		// �ɶ�д��ʽӳ��
		static mmap_t* mmap(int fd, size_t size = 0, size_t offset = 0, size_t size_max = 0) throw (SysCallException);

		static mmap_t* mmap(const char* filename, size_t size_max = 0) throw (SysCallException);
		// ж���ڴ�ӳ��
		static void unmap(mmap_t* ptr) throw (SysCallException);

		// ͬ����ʽˢ�ش���
		static void flush_sync(mmap_t* ptr, size_t offset = 0, size_t length = 0, bool invalid = false) throw (SysCallException);
		// �첽��ʽˢ�ش���
		static void flush_async(mmap_t* ptr, size_t offset = 0, size_t length = 0, bool invalid = false) throw (SysCallException);

private:
		static mmap_t* do_map(int prot, int fd, size_t size, size_t offset, size_t size_max, bool byfd) throw (SysCallException);
};
