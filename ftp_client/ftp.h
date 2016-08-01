#ifndef _FTP_CLIENT_H_
#define _FTP_CLIENT_H_

#include <map>
#include <string>
#include "socket.h"

// ��֪��FTP�����
enum {
	// FTP�ѽ�������
	FTP_CONNECTED = 220,
	// FTP�û�������
	FTP_USER_RIGHT = 331,
	// FTP�û�������ȷ
	FTP_PASS_RIGHT = 230,
	// FTP�����ļ����Ŀ¼����
	FTP_CWD_RIGHT = 250,
	// FTP���ñ����ʽ����
	FTP_TYPE_RIGHT = 200,
	// FTP���뱻��ģʽ����
	FTP_PASV_RIGHT = 227,
	// FTP��������ģʽ����
	FTP_PORT_RIGHT = 200,
	// FTP�ϴ�����ִ������,��ʼ����
	FTP_STOR_TRANS = 125,
	// FTP�ϴ�����ִ������
	FTP_STOR_RIGHT = 150,
	// FTP��������ִ������,��ʼ����
	FTP_RETR_TRANS = 125,
	// FTP��������ִ������
	FTP_RETR_RIGHT = 150,
	// FTP�ϴ����
	FTP_PUT_OVER = 226,
	// FTP�������
	FTP_GET_OVER = 226,
	// FTP�ļ���Ϊ���
	FTP_FILE_OVER = 250,
	// FTP�˳��ɹ�
	FTP_QUIT_SUCC = 221,
	// ��Ϊ�Զ�������
	FTP_COMMAND_NOT_FOUND = 299,
};

//�ļ�����������
#define FILE_OPEN_ERROR			-200	//���ļ�����
//����������������
#define DATA_READ_ERROR			-300	//�����ݴ���
#define DATA_WRITE_ERROR		-301	//д���ݴ���
//FTP����������
#define FTP_TIME_OUT			-1000	//FTP��ʱ
#define FTP_CONNECT_ERROR		-1001	//FTP�����쳣
#define FTP_USER_ERROR			-1002	//FTP�û�������
#define FTP_PASS_ERROR			-1003	//FTP�������
#define FTP_CWD_ERROR			-1004	//FTP�����ļ����Ŀ¼�쳣
#define FTP_TYPE_ERROR			-1005	//FTP���ñ����ʽ�쳣
#define FTP_PASV_ERROR			-1006	//FTP���뱻��ģʽ�쳣
#define FTP_PASV_IP_ERROR		-1007	//FTP���ݴ���IP��ַ��ȡ����
#define FTP_PORT_ERROR			-1008	//FTP��������ģʽ�쳣
#define FTP_STOR_ERROR			-1009	//FTP�ϴ�����ִ���쳣
#define FTP_RETR_ERROR			-1010	//FTP��������ִ���쳣
#define FTP_PUT_ERROR			-1011	//FTP�ϴ�ʧ��
#define FTP_GET_ERROR			-1012	//FTP����ʧ��
#define FTP_QUIT_ERROR			-1013	//FTP�˳�ʧ��


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

#endif
