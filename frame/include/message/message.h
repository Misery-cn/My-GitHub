
#ifndef _MESSAGE_H_
#define _MESSAGE_H_

struct Message
{
public:
	Message();
	virtual ~Message();
	
	// �ɾ������Ϣ�Լ�ʵ�ֱ����
	virtual int encode() = 0;
	virtual int decode() = 0;
	
private:
	int len_;
};

#endif