#include <map>
#include <string>
#include "socket.h"

// ȫ�ֺ���ָ��
typedef int (*gfunc)(void);

class FtpClient
{
public:
    FtpClient();
    virtual ~FtpClient();
	
	struct ftp_param_s
	{
		// �ͻ���Ҫ���ӵ�����ip
		char* ip_;
		// �ͻ���Ҫ���ӵ������˿�
		int port_;
		// ftp�û���
		char* usr_;
		// ftp�û�������
		char* pwd_;
		// Զ��Ŀ¼
		char* desDir_;
		// ����Ŀ¼
		char* localDir_;
		// ������ļ���
		char* filename_;
		// ���䷽ʽ(A - ASCII,I - bin,Ĭ�϶�����)
		char type_;
		// ��������ģʽ�·��������ص�ip
		// char* pasv_ip_;
		// ��������ģʽ�·��������صĶ˿�
		// ������˿ڷ�������
		int pasv_port_;
		
		ftp_param_s()
		{
			ip_ = NULL;
			port_ = 0;
			usr_ = NULL;
			pwd_ = NULL;
			desDir_ = NULL;
			localDir_ = NULL;
			filename_ = NULL;
			// Binary mode
			type_ = 'I';
			// �ݲ����ǹ���(������Ҫipӳ��)
			// pasv_ip_ = new char[MAX_IP_LEN];
			pasv_port_ = 0;
		}
		
		~ftp_param_s()
		{
			// if (pasv_ip_)
			// {
			//     delete[] pasv_ip_; 
			// }
		}
	};
	
	// ����ĺ���ָ��
	typedef int (FtpClient::*func)(ftp_param_s* param);
	
    enum {
        MKD = 1,	// ����Ŀ¼
        CWD = 2,	// ���뵽����Ŀ¼
		USR = 3,	// �û���
		PWD = 4,	// �û�����
		TYPE = 5,	// ���䷽ʽ(asccii�Ͷ�����)
		PASV = 6,	// ����ģʽ(�����ͱ���)
		STOR = 7,	// �ϴ��ļ�
		RETR = 8,	// �����ļ�
		QUIT = 9	// �˳�
    };
	
private:
	int pre_transfer();
	
    // ����ftp������
	int create_cmd_link(char* ip, int port);
	int create_data_link(char* ip, int port);
	
    // ����ftp����
    int execute_cmd(int cmd);
    int mkd(ftp_param_s* param);
	int usr(ftp_param_s* param);
	int pwd(ftp_param_s* param);
	int type(ftp_param_s* param);
	int pasv(ftp_param_s* param);
	int cwd(ftp_param_s* param);
	int stor(ftp_param_s* param);
	int retr(ftp_param_s* param);
	int quit(ftp_param_s* param);
	
	int put_file();
	int puting();
	int get_file();
	int geting();
	
	int ftp_write(sys::CSocket* sock, char* buff);
	
	int ftp_read(sys::CSocket* sock, char* buff, int len);
	
	int parse_pasv_result(char* result);
    // �ݹ鴴��Ŀ¼
    int recursive_mkd();
	
public:
    // �ϴ��ļ�
    int upload(char* ip, int port, char* usr, char* pwd, char* desDir, char* localDir, char* filename);
    // �����ļ�
    int download(char* ip, int port, char* usr, char* pwd, char* desDir, char* localDir, char* filename);

private:
	// ��������
    sys::CSocket* sock_cmd_;
	// pasv���ݴ�������
	sys::CSocket* sock_data_;
	ftp_param_s* param_;
    std::map<int, func> cmds_;
};
