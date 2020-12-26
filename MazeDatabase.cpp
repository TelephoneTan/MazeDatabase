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
    // 因为执行字符集设置为UTF8，所以手动设置代码页以防止在中文 Windows 打印出乱码，如果直接使用标准输入作为数据来源，那么输入也会被转换成UTF8
    system("chcp 65001");

	std::cout << "Hello CMake." << std::endl;

	// 初始化一个数据实例，指定：数据库文件路径（必须是已经存在的文件）
    Database db("maze.db");

    /// TODO: 执行字符集不可更改，因为SQLite默认使用UTF8。如果将执行字符集指定为其他编码，可能会出现乱码

    // 增加用户
	db.add_user(User("1800301129", "谭\t凌\n风"));
	db.add_user(User("1800301128", "Telephone"));

	// 查询所有用户
    Log::log("all users", beauty(db.select_all_users()));

    // 删除用户
    db.delete_user("1800301129");

    // 增加记录
    db.add_record(Record("1800301129", 1, 20, 120));
    db.add_record(Record("1800301129", 1, 15, 150));
    db.add_record(Record("1800301128", 1, 30, 90));
    db.add_record(Record("1800301128", 1, 40, 60));

    // 查询所有记录
    Log::log("all records", beauty(db.select_all_records()));

    // 删除记录
    db.delete_record("1800301129", 1, 0);
    db.delete_record("1800301128", 1, 1);

	std::getchar();

	return 0;
}
