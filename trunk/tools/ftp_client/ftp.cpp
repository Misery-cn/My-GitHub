#include "ftp.h"
#include <utility>

#define REG_CMD(cmd, func) \
    cmds_.insert(std::make_pair(cmd, func));

FtpClient::FtpClient()
{
    sock_cmd_ = new CSocket();
	sock_data_ = new CSocket();
	param_ = new ftp_param_s();

	REG_CMD((int)MKD, &FtpClient::mkd);
	REG_CMD((int)USR, &FtpClient::usr);
	REG_CMD((int)PWD, &FtpClient::pwd);
	REG_CMD((int)TYPE, &FtpClient::type);
	REG_CMD((int)PASV, &FtpClient::pasv);
	REG_CMD((int)CWD, &FtpClient::cwd);
	REG_CMD((int)STOR, &FtpClient::stor);
	REG_CMD((int)RETR, &FtpClient::retr);
	REG_CMD((int)QUIT, &FtpClient::quit);
}

FtpClient::~FtpClient()
{
	DELETE_P(sock_cmd_);
	DELETE_P(sock_data_);
	DELETE_P(param_);
}

int FtpClient::mkd(ftp_param_s* param)
{
	return 0;
}

int FtpClient::usr(ftp_param_s* param)
{
	int r = 0;
	char* buff = new char[BUFF_SIZE + 1];
	memset(buff, 0, BUFF_SIZE + 1);
	// �йܸ���Դ
	CPointGuard<char> g(buff);

	// ����user����
   	snprintf(buff, BUFF_SIZE, "USER %s\r\n", param->usr_);


   	r = ftp_write(sock_cmd_, buff);
   	if (r)
   	{
   	    sock_cmd_->s_close();
   	    return FTP_USER_ERROR;
   	}

	// ��ȡuser�������Ϣ
	memset(buff, 0, BUFF_SIZE + 1);
	r = ftp_read(sock_cmd_, buff, BUFF_SIZE);
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	buff[BUFF_SIZE + 1] = '\0';

	// FTP�û�������
	if (FTP_USER_RIGHT != atol(buff))
	{
		sock_cmd_->s_close();
   	    return FTP_USER_ERROR;
	}

	return 0;
}

int FtpClient::pwd(ftp_param_s* param)
{
	int r = 0;
	char* buff = new char[BUFF_SIZE + 1];
	memset(buff, 0, BUFF_SIZE + 1);
	// �йܸ���Դ
	CPointGuard<char> g(buff);

	// ����pass����
   	snprintf(buff, BUFF_SIZE, "PASS %s\r\n", param->pwd_);


   	r = ftp_write(sock_cmd_, buff);
   	if (r)
   	{
   	    sock_cmd_->s_close();
   	    return FTP_USER_ERROR;
   	}

	// ��ȡpass�������Ϣ
	memset(buff, 0, BUFF_SIZE + 1);
	r = ftp_read(sock_cmd_, buff, BUFF_SIZE);
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	buff[BUFF_SIZE + 1] = '\0';

	// FTP�û��������
	if (FTP_PASS_RIGHT != atol(buff))
	{
		sock_cmd_->s_close();
   	    return FTP_PASS_ERROR;
	}

	return 0;
}

int FtpClient::type(ftp_param_s* param)
{
	int r = 0;
	char* buff = new char[BUFF_SIZE + 1];
	memset(buff, 0, BUFF_SIZE + 1);
	// �йܸ���Դ
	CPointGuard<char> g(buff);

	// ����type����
   	snprintf(buff, BUFF_SIZE, "TYPE %c\r\n", param->type_);


   	r = ftp_write(sock_cmd_, buff);
   	if (r)
   	{
   	    sock_cmd_->s_close();
   	    return FTP_USER_ERROR;
   	}

	// ��ȡtype�������Ϣ
	memset(buff, 0, BUFF_SIZE + 1);
	r = ftp_read(sock_cmd_, buff, BUFF_SIZE);
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	buff[BUFF_SIZE + 1] = '\0';

	// FTP���ô��䷽ʽ����
	if (FTP_TYPE_RIGHT != atol(buff))
	{
		sock_cmd_->s_close();
   	    return FTP_TYPE_ERROR;
	}

	return 0;
}

int FtpClient::pasv(ftp_param_s* param)
{
	int r = 0;
	char* buff = new char[BUFF_SIZE + 1];
	memset(buff, 0, BUFF_SIZE + 1);
	// �йܸ���Դ
	CPointGuard<char> g(buff);

	// ����pasv����
   	strcpy(buff, "PASV\r\n");


   	r = ftp_write(sock_cmd_, buff);
   	if (r)
   	{
   	    sock_cmd_->s_close();
   	    return FTP_USER_ERROR;
   	}

	// ��ȡpasv�������Ϣ
	memset(buff, 0, BUFF_SIZE + 1);
	r = ftp_read(sock_cmd_, buff, BUFF_SIZE);
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	buff[BUFF_SIZE + 1] = '\0';

	// FTP���ñ���ģʽ����
	if (FTP_PASV_RIGHT != atol(buff))
	{
		sock_cmd_->s_close();
   	    return FTP_PASV_ERROR;
	}
	
	return parse_pasv_result(buff);;
}

int FtpClient::parse_pasv_result(char* result)
{
	// ����ֵ���£�(127,0,0,1,28,6)
	if (result)
	{
		int ilPos = strcspn(result, "(");
		int irPos = strcspn(result, ")");
		
		// ȡ�������е�����
		char buf[30 + 1] = {0};
		strncpy(buf, result + ilPos + 1, irPos - ilPos - 1);
		
		int a=0,b=0,c=0,d=0,e=0,f=0;
		int n = sscanf(buf, "%d,%d,%d,%d,%d,%d", &a, &b, &c, &d, &e, &f);
		if (6 != n)
		{
			return FTP_PASV_IP_ERROR;
		}

		param_->pasv_port_ = e * 256 + f;
		// ip�Ͳ�������
		// sprintf(param_->pasv_ip_, "%d.%d.%d.%d", a, b, c, d);
		return 0;
	}
	
	return -1;
}

int FtpClient::cwd(ftp_param_s* param)
{
	int r = 0;
	char* buff = new char[BUFF_SIZE + 1];
	memset(buff, 0, BUFF_SIZE + 1);
	// �йܸ���Դ
	CPointGuard<char> g(buff);

	// ���ͽ��빤��Ŀ¼����
   	snprintf(buff, BUFF_SIZE, "CWD %s\r\n", param->desDir_);


   	r = ftp_write(sock_cmd_, buff);
   	if (r)
   	{
   	    sock_cmd_->s_close();
   	    return FTP_USER_ERROR;
   	}

	// ��ȡcwd�������Ϣ
	memset(buff, 0, BUFF_SIZE + 1);
	r = ftp_read(sock_cmd_, buff, BUFF_SIZE);
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	buff[BUFF_SIZE + 1] = '\0';

	// FTP���빤��Ŀ¼����
	if (FTP_CWD_RIGHT != atol(buff))
	{
		sock_cmd_->s_close();
   	    return FTP_CWD_ERROR;
	}

	return 0;
}

int FtpClient::stor(ftp_param_s* param)
{
	int r = 0;
	char* buff = new char[BUFF_SIZE + 1];
	memset(buff, 0, BUFF_SIZE + 1);
	// �йܸ���Դ
	CPointGuard<char> g(buff);

	// ����stor�ϴ��ļ�����
   	snprintf(buff, BUFF_SIZE, "STOR %s\r\n", param->filename_);


   	r = ftp_write(sock_cmd_, buff);
   	if (r)
   	{
   	    sock_cmd_->s_close();
   	    return FTP_USER_ERROR;
   	}

	// ��ȡ�ϴ��ļ��������Ϣ
	memset(buff, 0, BUFF_SIZE + 1);
	r = ftp_read(sock_cmd_, buff, BUFF_SIZE);
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	buff[BUFF_SIZE + 1] = '\0';

	// �ϴ��ļ�������
	if (FTP_STOR_TRANS != atol(buff) && FTP_STOR_RIGHT != atol(buff))
	{
		sock_cmd_->s_close();
   	    return FTP_STOR_ERROR;
	}

	return 0;
}

int FtpClient::retr(ftp_param_s* param)
{
	int r = 0;
	char* buff = new char[BUFF_SIZE + 1];
	memset(buff, 0, BUFF_SIZE + 1);
	// �йܸ���Դ
	CPointGuard<char> g(buff);

	// ���������ļ�����
   	snprintf(buff, BUFF_SIZE, "RETR %s\r\n", param->filename_);


   	r = ftp_write(sock_cmd_, buff);
   	if (r)
   	{
   	    sock_cmd_->s_close();
   	    return FTP_USER_ERROR;
   	}

	// ��ȡ�����ļ��������Ϣ
	memset(buff, 0, BUFF_SIZE + 1);
	r = ftp_read(sock_cmd_, buff, BUFF_SIZE);
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	buff[BUFF_SIZE + 1] = '\0';

	// �����ļ�������
	if (FTP_RETR_TRANS != atol(buff) && FTP_RETR_RIGHT != atol(buff))
	{
		sock_cmd_->s_close();
   	    return FTP_RETR_ERROR;
	}

	return 0;
}

int FtpClient::quit(ftp_param_s* param)
{
	int r = 0;
	char* buff = new char[BUFF_SIZE + 1];
	memset(buff, 0, BUFF_SIZE + 1);
	// �йܸ���Դ
	CPointGuard<char> g(buff);

	// �����˳�����
   	strcpy(buff, "QUIT\r\n");


   	r = ftp_write(sock_cmd_, buff);
   	if (r)
   	{
   	    sock_cmd_->s_close();
   	    return FTP_USER_ERROR;
   	}

	// ��ȡ�˳��������Ϣ
	memset(buff, 0, BUFF_SIZE + 1);
	r = ftp_read(sock_cmd_, buff, BUFF_SIZE);
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	buff[BUFF_SIZE + 1] = '\0';

	// �ͻ����˳�����
	if (FTP_QUIT_SUCC != atol(buff))
	{
		sock_cmd_->s_close();
   	    return FTP_QUIT_ERROR;
	}
	
	sock_cmd_->s_close();

	return 0;
}

int FtpClient::put_file()
{
	// �����ݴ�������
	// �˴���ip��port�Ƿ���˷��ص�
	int r = create_data_link(param_->ip_, param_->pasv_port_);
	if (r)
	{
		sock_data_->s_close();
		return r;
	}
	// ���뵽Ŀ��Ŀ¼
	r = execute_cmd(CWD);
	if (r)
	{
		sock_data_->s_close();
		return r;
	}
	
	// ׼�������ļ�
	r = execute_cmd(STOR);
	if (r)
	{
		sock_data_->s_close();
		return r;
	}
	
	return puting();
}

int FtpClient::puting()
{
	int len = 0;
	int fd = 0;
	int r = 0;
	char* buff = new char[MAX_READ_SIZE + 1];
	memset(buff, 0, MAX_READ_SIZE + 1);
	CPointGuard<char> g(buff);
	
	std::string f(param_->localDir_);
	f.append("/").append(param_->filename_);

	// �򿪱����ļ�
	fd = open(f.c_str(), O_RDONLY);
	if (-1 == fd)
	{
		return FILE_OPEN_ERROR;
	}
	
	// ��ȡ�ļ�����
	while (0 != (len = read(fd, buff, MAX_READ_SIZE)))
	{
		if (0 < len)
		{
			// д����
			char* p = buff;
			int wl = 0;
			while (0 != (wl = write(sock_data_->getfd(), p, len)))
			{
				// ȫ��д��
				if (wl == len)
				{
					break;
				}
				else if (0 < wl)
				{
					// ����
					p = buff + wl;
					len = len - wl;
				}
				else
				{
					return DATA_WRITE_ERROR;
				}
			}
		}
		else
		{
			return DATA_READ_ERROR;
		}
	}
	
	// �ر��ļ�
	close(fd);

	// �ر����ݴ�������
	sock_data_->s_close();

	// ��ȡ�ļ��ϴ���ɷ�����Ϣ
	memset(buff, 0, MAX_READ_SIZE + 1);
	r = ftp_read(sock_cmd_, buff, MAX_READ_SIZE);
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	buff[MAX_READ_SIZE + 1] = '\0';

	// FTP�û���������
	if (FTP_PUT_OVER != atol(buff) && FTP_FILE_OVER != atol(buff))
	{
		sock_cmd_->s_close();
   	    return FTP_PUT_ERROR;
	}
	
	return 0;
}

int FtpClient::get_file()
{
	// �����ݴ�������
	// �˴���ip��port�Ƿ���˷��ص�
	int r = create_data_link(param_->ip_, param_->pasv_port_);
	if (r)
	{
		sock_data_->s_close();
		return r;
	}
	// ���뵽Ŀ��Ŀ¼
	r = execute_cmd(CWD);
	if (r)
	{
		sock_data_->s_close();
		return r;
	}
	
	// ׼�������ļ�
	r = execute_cmd(RETR);
	if (r)
	{
		sock_data_->s_close();
		return r;
	}
	
	return geting();
}

int FtpClient::geting()
{
	int len = 0;
	int fd = 0;
	int r = 0;
	char* buff = new char[MAX_READ_SIZE + 1];
	memset(buff, 0, MAX_READ_SIZE + 1);
	CPointGuard<char> g(buff);
	
	std::string f(param_->localDir_);
	f.append("/").append(param_->filename_);

	// �򿪱����ļ�
	fd = open(f.c_str(), O_RDWR|O_CREAT|O_TRUNC, 0666);
	if (-1 == fd)
	{
		return FILE_OPEN_ERROR;
	}
	
	// ��ȡ�ļ�����
	while (0 != (len = read(sock_data_->getfd(), buff, MAX_READ_SIZE)))
	{
		if (0 < len)
		{
			// д����
			char* p = buff;
			int wl = 0;
			while (0 != (wl = write(fd, p, len)))
			{
				// ȫ��д��
				if (wl == len)
				{
					break;
				}
				else if (0 < wl)
				{
					// ����
					p = buff + wl;
					len = len - wl;
				}
				else
				{
					return DATA_WRITE_ERROR;
				}
			}
		}
		else
		{
			return DATA_READ_ERROR;
		}
	}
	
	// �ر��ļ�
	close(fd);

	// �ر����ݴ�������
	sock_data_->s_close();

	// ��ȡ�ļ��ϴ���ɷ�����Ϣ
	memset(buff, 0, MAX_READ_SIZE + 1);
	r = ftp_read(sock_cmd_, buff, MAX_READ_SIZE);
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	buff[MAX_READ_SIZE + 1] = '\0';

	// FTP�û���������
	if (FTP_GET_OVER != atol(buff) && FTP_FILE_OVER != atol(buff))
	{
		sock_cmd_->s_close();
   	    return FTP_GET_ERROR;
	}
	
	return 0;
}

int FtpClient::ftp_write(CSocket* sock, char* buff)
{
	fd_set writefds;
    struct timeval tv;

    // ��ʼ��
    FD_ZERO(&writefds);
    // ��fd�Ŀ�дλ
    FD_SET(sock->getfd(), &writefds);

    tv.tv_sec = WAIT_TIME_OUT;
    tv.tv_usec = 0;

    // ����׽���״̬
    if (select(sock->getfd() + 1, NULL, &writefds, 0, &tv))
    {
        // �ж�������sockfd�Ŀ�дλ�Ƿ��,�������д������
        if (FD_ISSET(sock->getfd(), &writefds))
        {
            // д������
            // ������д�ֽ���
            return sock->s_write(sock->getfd(), buff);
        }
    }
	
	return FTP_TIME_OUT;
}

int FtpClient::ftp_read(CSocket* sock, char* buff, int len)
{
	fd_set readfds;
    fd_set errfds;
    struct timeval tv;

    // ��ʼ��
    FD_ZERO(&readfds);
    // ��sockfd�Ŀɶ�λ
    FD_SET(sock->getfd(), &readfds);

    // ��ʼ��
    FD_ZERO(&errfds);
    // ����sockfd���쳣λ
    FD_SET(sock->getfd(), &errfds);

    tv.tv_sec = WAIT_TIME_OUT;
    tv.tv_usec = 0;

    // ����׽���״̬
    if (select(sock->getfd() + 1, &readfds, NULL, &errfds, &tv))
    {
        // �ж�������sockfd���쳣λ�Ƿ��,��������ʾ��������
        if (FD_ISSET(sock->getfd(), &errfds))
        {
            return DATA_READ_ERROR;
        }

        // �ж�������sockfd�Ŀɶ�λ�Ƿ��,��������ȡ����
        if (FD_ISSET(sock->getfd(), &readfds))
        {
            // ������
            // ����ʵ���������ֽ���
            return sock->s_read(sock->getfd(), buff, len);
        }
    }
	
	return FTP_TIME_OUT;
}

int FtpClient::execute_cmd(int cmd)
{
	std::map<int, func>::iterator i = cmds_.find(cmd);
    if (i == cmds_.end()) 
	{
		// not found
        return FTP_COMMAND_NOT_FOUND;
    } 
	else 
	{
		return (this->*(i->second))(param_);
    }
}

int FtpClient::create_cmd_link(char* ip, int port)
{
	if (sock_cmd_)
	{
		int r = 0;
		char* buff = new char[BUFF_SIZE + 1];
		memset(buff, 0, BUFF_SIZE + 1);
		// �йܸ���Դ
		CPointGuard<char> g(buff);
		
		// ����һ���׽���
		r = sock_cmd_->s_open();
		if (r)
		{
			sock_cmd_->s_close();
			return r;
		}

		// ���ӷ����
		r = sock_cmd_->s_connect(ip, port);
		if (r)
		{
			sock_cmd_->s_close();
			return r;
		}
		
		r = ftp_read(sock_cmd_, buff, BUFF_SIZE);
		if (r)
		{
			sock_cmd_->s_close();
			return r;
		}
		
		buff[BUFF_SIZE + 1] = '\0';
		
		// ����ʧ�ܷ��ش�����Ϣ
		if (FTP_CONNECTED != atol(buff))
		{
			sock_cmd_->s_close();
			return FTP_CONNECT_ERROR;
		}
		
		return 0;
	}
	
	return -1;
}

int FtpClient::create_data_link(char* ip, int port)
{
	// �������Ӳ�������Ӧ��ֱ�ӷ���
	if (sock_data_)
	{
		int r = 0;
		// ����һ���׽���
		r = sock_data_->s_open();
		if (r)
		{
			sock_data_->s_close();
			return r;
		}

		// ���ӷ����
		r = sock_data_->s_connect(ip, port);
		if (r)
		{
			sock_data_->s_close();
			return r;
		}
		
		return 0;
	}
	
	return -1;
}

int FtpClient::pre_transfer()
{
	// ����������
	int r = create_cmd_link(param_->ip_, param_->port_);
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	// �����û���¼
	r = execute_cmd(USR);
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	// �����û�����
	r = execute_cmd(PWD);
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	// ���ö����ƴ��䷽ʽ
	r = execute_cmd(TYPE);
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	// ���ñ���������ʽ
	r = execute_cmd(PASV);
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	return 0;
}

int FtpClient::upload(char* ip, int port, char* usr, char* pwd, char* desDir, char* localDir, char* filename)
{
	param_->ip_ = ip;
	param_->port_ = port;
	param_->usr_ = usr;
	param_->pwd_ = pwd;
	param_->desDir_ = desDir;
	param_->localDir_ = localDir;
	param_->filename_ = filename;
	
	int r = 0;
	// ��¼�����ô��䷽ʽ��
	r = pre_transfer();
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	r = put_file();
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	// �˳�
	r = execute_cmd(QUIT);
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	return 0;
}

int FtpClient::download(char* ip, int port, char* usr, char* pwd, char* desDir, char* localDir, char* filename)
{
	param_->ip_ = ip;
	param_->port_ = port;
	param_->usr_ = usr;
	param_->pwd_ = pwd;
	param_->desDir_ = desDir;
	param_->localDir_ = localDir;
	param_->filename_ = filename;
	
	int r = 0;
	// ��¼�����ô��䷽ʽ��
	r = pre_transfer();
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	r = get_file();
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	// �˳�
	r = execute_cmd(QUIT);
	if (r)
	{
		sock_cmd_->s_close();
		return r;
	}
	
	return 0;
}

/*
int main()
{
	FtpClient* ftpclient = new FtpClient();
	// ftpclient->upload("127.0.0.1", 21, "misery", "880622", "/home/misery/", "/home/misery/MyProject/MyCpp", "main.cpp");
	ftpclient->download("127.0.0.1", 21, "misery", "880622", "/home/misery/MyProject/MyCpp", "/home/misery/", "main.cpp");
	return 0;
}
*/

