
#define IN
#define OUT

// �����RUNLOG
#define RUNLOG printf

#define DELETE_P(p) \
	if (p) \
	{ \
		delete p; \
		p = NULL; \
	}
	
#define WORK_DIR "WORK_DIR"
	
// socket�����
enum {
	SOCKET_ERROR = 100,
	SOCKET_CLOSE_ERROR,
	SOCKET_SET_ERROR,
	SOCKET_BIND_ERROR,
	SOCKET_LISTEN_ERROR,
	SOCKET_ACCEPT_ERROR,
	SOCKET_CONNECT_ERROR,
	SOCKET_READ_ERROR,
	SOCKET_WRITE_ERROR,
	SOCKET_TIME_OUT
};

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
