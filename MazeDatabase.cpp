// MazeDatabase.cpp: 定义应用程序的入口点。
//

#include <iostream>
#include <windows.h>

#include "Database.h"
#include "Log.h"

std::string beauty(const std::vector<std::vector<std::string>> &vector){
    std::string res("[");
    boolean first = true;
    for (const auto &row : vector) {
        if (!first){
            res.append(",");
        } else{
            first = false;
        }
        std::string row_str;
        for (const auto &field : row){
            row_str.append(field).append(" ");
        }
        res.append("{").append(row_str).append("}");
    }
    res.append("]");
    return std::move(res);
}

int main()
{
    // 因为执行字符集设置为UTF8，所以手动设置代码页以防止在中文 Windows 打印出乱码，如果直接使用标准输入作为数据来源，那么也有好处
    system("chcp 65001");

	std::cout << "Hello CMake." << std::endl;

	// 初始化一个数据实例，指定：域名（或IP地址）、用户名、密码、使用的数据库名、连接字符集
    // 这里的字符编码应该和CMake配置中的执行字符集保持一致
    Database db("localhost", "root", "******", "maze", 0, Database::utf8);

    /// TODO: 所有涉及到字符串的函数调用，字符串的编码必须与连接字符集一致

    // 增加用户
	db.add_user(User("1800301129", "谭\t凌\n风"));
	db.add_user(User("1800301128", "Telephone"));

	// 查询所有用户
    Log::log("all users", beauty(db.select_all_users()));

    // 删除用户
    db.delete_user("1800301129");

    // 增加记录
    db.add_record(Record("1800301129", 1, 20, 120));
    db.add_record(Record("1800301128", 1, 30, 90));

    // 查询所有记录
    Log::log("all records", beauty(db.select_all_records()));

    // 删除记录
    db.delete_record("1800301129", 1, 0);

	std::getchar();

	return 0;
}
