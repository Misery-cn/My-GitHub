#include <deque>
#include <map>
#include <set>
#include <string>

class CConfLine
{
public:
	CConfLine(const std::string& key, const std::string val,
		const std::string section, const std::string comment, int lineno);
		
	bool operator<(const CConfLine& oth) const;
	
public:
	friend std::ostream& operator<<(std::ostream& oss, const CConfLine& line);
	
public:
	// 配置项
	std::string key_;
	// 配置项的值
	std::string val_;
	// 节点名
	std::string section_;
};


class CConfSection
{
public:
	typedef std::set<CConfLine>::const_iterator const_line_iter_t;

	std::set<CConfLine> lines_;
};

class CConfFile
{
public:
	typedef std::map<std::string, CConfSection>::iterator section_iter_t;
	typedef std::map<std::string, CConfSection>::const_iterator const_section_iter_t;

	CConfFile();
	~CConfFile();
	
	// 清空
	void clear();
	// 解析文件
	int parse_file(const std::string& filename);
	// 读取配置项
	int read(const std::string& section, const std::string& key, std::string& val) const;
	
	const_section_iter_t sections_begin();
	const_section_iter_t sections_end();
	
	// 去除空字符
	static void trim_whitespace(std::string& str, bool strip_internal);

	// 格式化配置项名
	static std::string normalize_key_name(const std::string& key);
	
public:
	friend std::ostream& operator<<(std::ostream& oss, const CConfFile& cf);

private:
	// 加载配置文件
	void load_from_buffer(const char* buf, size_t sz);
	
	// 解析一行
	static CConfLine* process_line(int line_no, const char* line);

	std::map<std::string, CConfSection> sections_;
};
