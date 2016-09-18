#ifndef _UTILS_TOKENER_H_
#define _UTILS_TOKENER_H_

#include <list>
#include <map>
#include <string>
#include <vector>
#include "string_utils.h"


// UTILS_NS_BEGIN

class CTokener
{
public:
	// ��ָ�����ַ�����Ϊ�ָ������������ַ��и�Token������һ��������
	// tokens:�洢Token����������Ϊlist��vector�ȣ�ֻҪ��֧��push_back()
	// source:���������ַ���
	// sep:Token�ָ���
	// skip_sep:�Ƿ�����������sep�������������ͬ��sep���ڣ���ֻ����һ��sep
    template <class ContainerType>
    static int split(ContainerType* tokens, const std::string& source, const std::string& sep, bool skip_sep = false)
    {
        if (sep.empty())
        {
            tokens->push_back(source);
        }
        else if (!source.empty())
        {
            std::string str = source;
            std::string::size_type pos = str.find(sep);

            for (;;)
            {
                std::string token = str.substr(0, pos);
                tokens->push_back(token);

                if (std::string::npos == pos)
                {
                    break;
                }
				
                if (skip_sep)
                {
                    bool end = false;
                    while (0 == strncmp(sep.c_str(), &str[pos + 1], sep.size()))
                    {
                        pos += sep.size();
                        if (pos >= str.size())
                        {
                            end = true;
                            tokens->push_back(std::string(""));
                            break;
                        }
                    }

                    if (end)
                    {
                        break;
                    }
                }

                str = str.substr(pos + sep.size());
                pos = str.find(sep);
            }
        }

        return static_cast<int>(tokens->size());
    }
};

// ����������ʽ���ַ��������name�ظ���������ֵ�Ḳ��ǰ����ͬname��ֵ����
// name1=value2|name2=value3|name3=value3...
class CEnhancedTokener
{
public:
    const std::map<std::string, std::string>& tokens() const
    {
        return key_value_pair_map_;
    }

    std::string operator[](const std::string& name) const
    {
        return get(name);
    }

    bool exist(const std::string& name) const
    {
        return  0 < key_value_pair_map_.count(name);
    }
    
    bool get(const std::string& name, std::string* value) const
    {
        std::map<std::string, std::string>::const_iterator iter = key_value_pair_map_.find(name);
		
        if (iter != key_value_pair_map_.end())
        {
            *value = iter->second;
			
            return true;
        }
        
        return false;
    }
    
    std::string get(const std::string& name) const
    {
        std::string value;
		
        (void)get(name, &value);
		
        return value;
    }
    
    void parse(const std::string& source, const std::string& token_sep, char name_value_sep='=')
    {
        std::vector<std::string> tokens;        
        int num_tokens = CTokener::split(&tokens, source, token_sep);
        
        for (int i = 0; i < num_tokens; ++i)
        {
            const std::string& token = tokens[i];
            std::string::size_type pos = token.find(name_value_sep);
            
            std::string name;
            std::string value;
			
            if (pos == std::string::npos)
            {
                name = token;
            }
            else
            {
                name = token.substr(0, pos);
                value = token.substr(pos + 1);
            }

            std::pair<std::map<std::string, std::string>::iterator, bool> ret =
                key_value_pair_map_.insert(std::make_pair(name, value));
			
            if (!ret.second)
            {
                // ����֮ǰ��ֵ
                ret.first->second = value;
            }
        }
    }
    
private:
    std::map<std::string, std::string> key_value_pair_map_;
};

// ����������ʽ���ַ�����name������ͬ���Ҳ��ụ���ǣ���
// name1=value2|name2=value3|name3=value3...
class CEnhancedTokenerEx
{
public:
    const std::multimap<std::string, std::string>& tokens() const
    {
        return key_value_pair_map_;
    }

    bool exist(const std::string& name) const
    {
        return 0 < key_value_pair_map_.count(name);
    }

    void parse(const std::string& source, const std::string& token_sep, char name_value_sep = '=')
    {
        std::vector<std::string> tokens;
        int num_tokens = CTokener::split(&tokens, source, token_sep);

        for (int i = 0; i < num_tokens; ++i)
        {
            const std::string& token = tokens[i];
            std::string::size_type pos = token.find(name_value_sep);

            std::string name;
            std::string value;
            if (pos == std::string::npos)
            {
                name = token;
            }
            else
            {
                name = token.substr(0, pos);
                value = token.substr(pos + 1);
            }

            key_value_pair_map_.insert(std::make_pair(name, value));
        }
    }

private:
    std::multimap<std::string, std::string> key_value_pair_map_;
};

// ����������ʽ���ַ�����
// ip1:port1#password1,ip2:port2#password2...
// ��
// username1@ip1:port1#password1,username2@ip2:port2#password2...
class CLoginTokener
{
public:
    struct LoginInfo
    {
        std::string username;
        std::string password;
        std::string ip;
        uint16_t port;
    };

    // source ���������ַ���
    // token_sep Token��ķָ�����ʾ���С�username1@ip1:port1#password1������Ϊ��һ��Token
    // ip_port_sep IP�Ͷ˿ڼ�ķָ���
    // port_passwrd_sep �˿ں������ķָ���
    // username_ip_sep �û�����IP��ķָ���
    // �����ɹ�����Token����
    // Ҫ��ip:port��������ڣ����򷵻�-1����user��password�ǿ�ѡ�ģ�
    // �����һ����Ч�Ķ˿�ֵ��Ҳ�᷵��-1
    static int parse(std::vector<struct LoginInfo>* login_infos, const std::string& source, const std::string& token_sep,
                     char ip_port_sep = ':', char port_password_sep = '#', char username_ip_sep = '@')
    {
        std::vector<std::string> tokens;
        int num_tokens = CTokener::split(&tokens, source, token_sep);

        login_infos->resize(num_tokens);
        for (int i = 0; i < num_tokens; ++i)
        {
            std::string port;
            std::string::size_type pos;
            std::string token = tokens[i];
            struct LoginInfo& login_info = (*login_infos)[i];

            // username
            pos = token.find(username_ip_sep); // @
            if (std::string::npos == pos)
            {
                login_info.username.clear();
            }
            else
            {
                // ����username
                login_info.username = token.substr(0, pos);
                token = token.substr(pos + 1);
            }

            // ip
            pos = token.find(ip_port_sep); // :
            if (std::string::npos == pos)
            {
                return -1;
            }
            else
            {
                // ����ip
                login_info.ip = token.substr(0, pos);
                token = token.substr(pos + 1);
            }

            // port
            pos = token.find(port_password_sep); // #
            if (std::string::npos == pos)
            {
                port = token;
                login_info.password.clear();
            }
            else
            {
                port = token.substr(0, pos);
                login_info.password = token.substr(pos + 1);
            }

            if (!CStringUtils::string2int(port.c_str(), login_info.port))
            {
                return -1;
            }
        }

        return num_tokens;
    }

    static void print(const std::vector<struct LoginInfo>& login_infos)
    {
        if (login_infos.empty())
        {
            printf("nothing\n");
        }
        else
        {
            for (std::vector<struct LoginInfo>::size_type i = 0; i < login_infos.size(); ++i)
            {
                const struct LoginInfo& login_info = login_infos[i];
                printf("[%d] => %s@%s:%u#%s\n",
                    (int)i, login_info.username.c_str(), login_info.ip.c_str(), login_info.port, login_info.password.c_str());
            }
        }
    }
};

// UTILS_NS_END

#endif
