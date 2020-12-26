//
// Created by Terry on 2020/12/20.
//

#ifndef MAZEDATABASE_DATABASE_H
#define MAZEDATABASE_DATABASE_H

#include <mysql.h>
#include <string>
#include <vector>

#include "User.h"
#include "Record.h"

class Database {
    MYSQL* con = mysql_init(nullptr);
    std::string host;
    std::string username;
    std::string pwd;
    std::string db_name;
    unsigned int port;
    std::string charset;
public:
    constexpr static const char * const utf8 = "utf8";
    constexpr static const char * const gbk = "gbk";

    Database(const std::string &host, const std::string &username, const std::string &pwd, const std::string &dbName,
             unsigned int port_or_0_if_default, const std::string &charset);

    boolean add_user(const User &user);
    boolean add_record(const Record &record);
    std::vector<std::vector<std::string>> select_all_users();
    std::vector<std::vector<std::string>> select_all_records();
    boolean delete_user(const std::string &sno);
    boolean delete_record(const std::string &sno, const int &level, const int &index);

private:
    static void findAndReplaceAll(std::string &data, const std::string &toSearch, const std::string &replaceStr);
    static void findAndReplaceAll_reverse(std::string &data, const std::string &replaceStr, const std::string &toSearch);
    static void escape(std::string &text, bool isLike);
    static void restore(std::string &text, bool isLike);
    static std::vector<std::vector<std::string>> get_select_all_result(MYSQL *con);
};


#endif //MAZEDATABASE_DATABASE_H
