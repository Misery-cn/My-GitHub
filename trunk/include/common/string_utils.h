#ifndef _STRING_UTILS_H_
#define _STRING_UTILS_H_

#include <math.h>
#include <sstream>
#include "int_types.h"
#include "utils.h"
#include "scoped_ptr.h"


// UTILS_NS_BEGIN

class StringUtils
{
public:
	// ɾ���ַ���β����ָ���ַ���ʼ������
    static void remove_last(std::string& source, char c);
	// ɾ���ַ���β����ָ���ַ�����ʼ������
    static void remove_last(std::string& source, const std::string& sep);
	
	// ���ַ����е�����Сд�ַ�ת���ɴ�д
    static void to_upper(char* source);
    static void to_upper(std::string& source);
	
	// ���ַ����е����д�д�ַ�ת����Сд
    static void to_lower(char* source);    
    static void to_lower(std::string& source);

	// �ж�ָ���ַ��Ƿ�Ϊ�ո��TAB��(\t)��س���(\r)���з�(\n)
    static bool is_space(char c);

	// ɾ���ַ�����β�ո��TAB��(\t)��س���(\r)���з�(\n)
    static void trim(char* source);
    static void trim(std::string& source);

	// ɾ���ַ���ͷ���ո��TAB��(\t)��س���(\r)���з�(\n)
    static void trim_left(char* source);
    static void trim_left(std::string& source);

	// ɾ���ַ���β���ո��TAB��(\t)��س���(\r)���з�(\n)
    static void trim_right(char* source);        
    static void trim_right(std::string& source);

	// ���ַ���ת����8λ���з�������
	// converted_length:��Ҫ��������ת�����ַ��������������ֲ������������ȡֵΪ0���������ַ���
	// ignored_zero:�Ƿ����������ַ�����0��ͷ�����Զ�����
    static bool string2int(const char* source, int8_t& result, uint8_t converted_length = 0, bool ignored_zero = false);
    static bool string2int8(const char* source, int8_t& result, uint8_t converted_length = 0, bool ignored_zero = false);    

	// ���ַ���ת����16λ���з�������
	// converted_length:��Ҫ��������ת�����ַ��������������ֲ������������ȡֵΪ0���������ַ���
	// ignored_zero:�Ƿ����������ַ�����0��ͷ�����Զ�����
    static bool string2int(const char* source, int16_t& result, uint8_t converted_length = 0, bool ignored_zero = false);
	static bool string2int16(const char* source, int16_t& result, uint8_t converted_length = 0, bool ignored_zero = false);    

	// ���ַ���ת����32λ���з�������
	// converted_length:��Ҫ��������ת�����ַ��������������ֲ������������ȡֵΪ0���������ַ���
	// ignored_zero:�Ƿ����������ַ�����0��ͷ�����Զ�����
    static bool string2int(const char* source, int32_t& result, uint8_t converted_length = 0, bool ignored_zero = false);
    static bool string2int32(const char* source, int32_t& result, uint8_t converted_length = 0, bool ignored_zero = false);    

	// ���ַ���ת����64λ���з�������
	// converted_length:��Ҫ��������ת�����ַ��������������ֲ������������ȡֵΪ0���������ַ���
	// ignored_zero:�Ƿ����������ַ�����0��ͷ�����Զ�����
    static bool string2int(const char* source, int64_t& result, uint8_t converted_length = 0, bool ignored_zero = false);
    static bool string2int64(const char* source, int64_t& result, uint8_t converted_length=0, bool ignored_zero = false);    

	// ���ַ���ת����8λ���޷�������
	// converted_length:��Ҫ��������ת�����ַ��������������ֲ������������ȡֵΪ0���������ַ���
	// ignored_zero:�Ƿ����������ַ�����0��ͷ�����Զ�����
    static bool string2int(const char* source, uint8_t& result, uint8_t converted_length = 0, bool ignored_zero = false);
    static bool string2uint8(const char* source, uint8_t& result, uint8_t converted_length = 0, bool ignored_zero = false);    

	// ���ַ���ת����16λ���޷�������
	// converted_length:��Ҫ��������ת�����ַ��������������ֲ������������ȡֵΪ0���������ַ���
	// ignored_zero:�Ƿ����������ַ�����0��ͷ�����Զ�����
    static bool string2int(const char* source, uint16_t& result, uint8_t converted_length = 0, bool ignored_zero = false);
    static bool string2uint16(const char* source, uint16_t& result, uint8_t converted_length = 0, bool ignored_zero = false);    

	// ���ַ���ת����32λ���޷�������
	// converted_length:��Ҫ��������ת�����ַ��������������ֲ������������ȡֵΪ0���������ַ���
	// ignored_zero:�Ƿ����������ַ�����0��ͷ�����Զ�����
    static bool string2int(const char* source, uint32_t& result, uint8_t converted_length = 0, bool ignored_zero = false);
    static bool string2uint32(const char* source, uint32_t& result, uint8_t converted_length = 0, bool ignored_zero = false);    

	// ���ַ���ת����64λ���޷�������
	// converted_length:��Ҫ��������ת�����ַ��������������ֲ������������ȡֵΪ0���������ַ���
	// ignored_zero:�Ƿ����������ַ�����0��ͷ�����Զ�����
    static bool string2int(const char* source, uint64_t& result, uint8_t converted_length = 0, bool ignored_zero = false);
    static bool string2uint64(const char* source, uint64_t& result, uint8_t converted_length = 0, bool ignored_zero = false);    

	// ��strΪ����Ч��IntType��������ֵʱ������error_valueָ����ֵ
    template <typename IntType>
    static IntType string2int(const char* str, IntType error_value = 0)
    {
        IntType m = 0;
        if (!string2int(str, m))
        {
            m = error_value;
        }

        return m;
    }

    template <typename IntType>
    static IntType string2int(const std::string& str, IntType error_value = 0)
    {
        IntType m = 0;
        if (!string2int(str.c_str(), m))
        {
            m = error_value;
        }

        return m;
    }

    static std::string int_tostring(int16_t source);
    static std::string int16_tostring(int16_t source);    

    static std::string int32_tostring(int32_t source);
    static std::string int_tostring(int32_t source);

    static std::string int_tostring(int64_t source);
    static std::string int64_tostring(int64_t source);

    static std::string int_tostring(uint16_t source);
    static std::string uint16_tostring(uint16_t source);    

    static std::string int_tostring(uint32_t source);
    static std::string uint32_tostring(uint32_t source);    

    static std::string int_tostring(uint64_t source);
    static std::string uint64_tostring(uint64_t source);    

	// �����ո�
    static char* skip_spaces(char* buffer);
    static const char* skip_spaces(const char* buffer);

    static uint32_t hash(const char *str, int len);

	// ���Ǳ�֤����ʵ����strд����ֽ���,������β��,������size�Ƿ��㹻����
    static int fix_snprintf(char *str, size_t size, const char *format, ...) __attribute__((format(printf, 3, 4)));
    static int fix_vsnprintf(char *str, size_t size, const char *format, va_list ap);

	// ·��ת�����ļ���
    static std::string path2filename(const std::string& path, const std::string& join_string);

	// ��������ת������
    template <typename Any>
    static std::string any2string(Any any)
    {
        std::stringstream s;
        s << any;
        return s.str();
    }

	// ��STL����ת�����ַ���
    template <class ContainerClass>
    static std::string container2string(const ContainerClass& container, const std::string& join_string)
    {
        std::string str;
        typename ContainerClass::const_iterator iter = container.begin();

        for (; iter != container.end(); ++iter)
        {
            if (str.empty())
            {
                str = any2string(*iter);
            }
            else
            {
                str += join_string + any2string(*iter);
            }
        }

        return str;
    }

	// ��map����ת�����ַ���
    template <class MapClass>
    static std::string map2string(const MapClass& map, const std::string& join_string)
    {
        std::string str;
        typename MapClass::const_iterator iter = map.begin();

        for (; iter!=map.end(); ++iter)
        {
            if (str.empty())
            {
                str = any2string(iter->second);
            }
            else
            {
                str += join_string + any2string(iter->second);
            }
        }

        return str;
    }

	// ����һ���ַ����ַ����е�λ��
    static int chr_index(const char* str, char c);
    static int chr_rindex(const char* str, char c);
    
    static std::string extract_dirpath(const char* filepath);
    
    static std::string extract_filename(const std::string& filepath);

    static const char* extract_filename(const char* filepath);

    static std::string format_string(const char* format, ...) __attribute__((format(printf, 1, 2)));

    static bool is_numeric_string(const char* str);

    static bool is_alphabetic_string(const char* str);

    static bool is_variable_string(const char* str);
    
    static bool is_regex_string(const char* str);

    static std::string remove_suffix(const std::string& filename);
    
    static std::string replace_suffix(const std::string& filepath, const std::string& new_suffix);

    static std::string to_hex(const std::string& source, bool lowercase = true);

    static std::string encode_url(const std::string& url, bool space2plus = false);
    static std::string encode_url(const char* url, size_t url_length, bool space2plus = false);

    static std::string decode_url(const std::string& encoded_url);
    static std::string decode_url(const char* encoded_url, size_t encoded_url_length);

    static void trim_CR(char* line);
    static void trim_CR(std::string& line);

    static std::string char2hex(unsigned char c);
    static unsigned char hex2char(const std::string& hex);
};

// UTILS_NS_END

#endif
