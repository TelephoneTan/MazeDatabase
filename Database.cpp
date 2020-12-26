//
// Created by Terry on 2020/12/20.
//

#include "Database.h"
#include "Log.h"

Database::Database(const std::string &host, const std::string &username, const std::string &pwd,
                   const std::string &dbName, unsigned int port_or_0_if_default, const std::string &charset) : host(host),
                                                                                                               username(username),
                                                                                                               pwd(pwd),
                                                                                                               db_name(dbName),
                                                                                                               port(port_or_0_if_default),
                                                                                                               charset(charset) {
    if (con){
        Log::log("database init success");
        if (mysql_real_connect(con, host.c_str(), username.c_str(), pwd.c_str(), db_name.c_str(), port_or_0_if_default, nullptr, 0)){
            Log::log("database connected");
            // set the charset
            Log::log("set connection charset to " + charset + " ...");
            mysql_query(con, std::string("set names ").append(charset).c_str());
        } else{
            Log::log("unable to connect database");
        }
    } else{
        Log::log("database init fail");
    }
}

/// not work well for '\0'
void Database::findAndReplaceAll(std::string &data, const std::string &toSearch, const std::string &replaceStr) {
    std::size_t pos = 0 - replaceStr.size();
    while(pos = data.find(toSearch, pos + replaceStr.size()), pos != std::string::npos)
    {
        data.replace(pos, toSearch.size(), replaceStr);
    }
}

void
Database::findAndReplaceAll_reverse(std::string &data, const std::string &replaceStr, const std::string &toSearch) {
    findAndReplaceAll(data, toSearch, replaceStr);
}

/// not escape the '\0'
void Database::escape(std::string &text, bool isLike) {
    findAndReplaceAll(text, "\\", "\\\\");
    findAndReplaceAll(text, "'", "''");
    /// not necessary if use '\'' around string
    findAndReplaceAll(text, "\"", "\\\"");
    findAndReplaceAll(text, "\b", "\\b");
    findAndReplaceAll(text, "\n", "\\n");
    findAndReplaceAll(text, "\r", "\\r");
    findAndReplaceAll(text, "\t", "\\t");
    findAndReplaceAll(text, "\032", "\\Z");
    if (isLike) {
        findAndReplaceAll(text, "%", "\\%");
        findAndReplaceAll(text, "_", "\\_");
    }
}

/// not restore the '\0'
void Database::restore(std::string &text, bool isLike) {
    if (isLike) {
        findAndReplaceAll_reverse(text, "_", "\\_");
        findAndReplaceAll_reverse(text, "%", "\\%");
    }
    findAndReplaceAll_reverse(text, "\032", "\\Z");
    findAndReplaceAll_reverse(text, "\t", "\\t");
    findAndReplaceAll_reverse(text, "\r", "\\r");
    findAndReplaceAll_reverse(text, "\n", "\\n");
    findAndReplaceAll_reverse(text, "\b", "\\b");
    /// not necessary if use '\'' around string
    findAndReplaceAll_reverse(text, "\"", "\\\"");
    findAndReplaceAll_reverse(text, "'", "''");
    findAndReplaceAll_reverse(text, "\\", "\\\\");
}

std::vector<std::vector<std::string>> Database::get_select_all_result(MYSQL *con) {
    MYSQL_RES *sql_res = mysql_store_result(con);
    unsigned int column_num = mysql_num_fields(sql_res);
    unsigned int row_num = mysql_num_rows(sql_res);
    std::vector<std::vector<std::string>> res;
    res.reserve(row_num);
    for (int i = 0; i < row_num; ++i) {
        MYSQL_ROW row_pointer = mysql_fetch_row(sql_res);
        std::vector<std::string> row_str;
        row_str.reserve(column_num);
        for (int column_index = 0; column_index < column_num; ++column_index) {
            row_str.emplace_back(row_pointer[column_index]);
        }
        res.emplace_back(std::move(row_str));
    }
    mysql_free_result(sql_res);
    return std::move(res);
}

boolean Database::add_user(const User &user) {
    std::string NAME("add_user()");

    std::string sno(user.getSno());
    std::string name(user.getName());

    escape(sno, false);
    escape(name, false);

    std::string count_sql("select * from user");

    mysql_query(con, count_sql.c_str());

    MYSQL_RES *sql_res = mysql_store_result(con);
    int count_aka_next_index = mysql_num_rows(sql_res);
    mysql_free_result(sql_res);

    mysql_query(con, ("insert into user (sno, name) values ('" + sno + "', '" + name + "')").c_str());
    mysql_query(con, count_sql.c_str());

    sql_res = mysql_store_result(con);
    int count_after_insert = mysql_num_rows(sql_res);
    mysql_free_result(sql_res);

    boolean res = (count_after_insert > count_aka_next_index);

    restore(sno, false);
    restore(name, false);
    Log::log(NAME, "add user " + sno + " " + name + " " + ( (res)?("success"):("fail") ) );
    return res;
}

std::vector<std::vector<std::string>> Database::select_all_users() {
    std::string sql("select * from user");
    mysql_query(con, sql.c_str());
    return get_select_all_result(con);
}

boolean Database::delete_user(const std::string &sno) {
    std::string NAME("delete_user()");

    std::string sno_e(sno);
    escape(sno_e, false);

    std::string count_sql = std::string("select * from user where sno='") + sno_e + "'";

    std::string sql = count_sql;

    mysql_query(con, sql.c_str());

    MYSQL_RES *sql_res;
    sql_res = mysql_store_result(con);
    unsigned int num_before = mysql_num_rows(sql_res);
    mysql_free_result(sql_res);

    sql = std::string("delete from user where sno='") + sno_e + "'";
    mysql_query(con, sql.c_str());

    sql = count_sql;
    mysql_query(con, sql.c_str());

    sql_res = mysql_store_result(con);
    unsigned int num_after = mysql_num_rows(sql_res);
    mysql_free_result(sql_res);

    boolean res = (num_before > num_after);

    Log::log(NAME, "delete user " + sno + " " + ( (res)?("success"):("fail") ) );
    return res;
}

boolean Database::add_record(const Record &record) {
    std::string NAME("add_record()");

    std::string sno(record.getSno());
    std::string level(std::to_string(record.getLevel()));
    std::string time_cost_s(std::to_string(record.getTimeCostS()));
    std::string score(std::to_string(record.getScore()));

    escape(sno, false);
    escape(level, false);
    escape(time_cost_s, false);
    escape(score, false);

    std::string count_sql = std::string("select * from record where sno='") + sno + "' and level=" + level;

    mysql_query(con, count_sql.c_str());

    MYSQL_RES *sql_res = mysql_store_result(con);
    int count_aka_next_index = mysql_num_rows(sql_res);
    mysql_free_result(sql_res);

    std::string insert_sql = "insert into record (sno, level, idx, time_cost_s, score) values ('" + sno + "', " + level + ", " + std::to_string(count_aka_next_index) + ", " + time_cost_s + ", " + score + ")";

    mysql_query(con, insert_sql.c_str());

    mysql_query(con, count_sql.c_str());

    sql_res = mysql_store_result(con);
    int count_after_insert = mysql_num_rows(sql_res);
    mysql_free_result(sql_res);

    boolean res = (count_after_insert > count_aka_next_index);

    restore(sno, false);
    restore(level, false);
    restore(time_cost_s, false);
    restore(score, false);
    Log::log(NAME, "add record { sno: " + sno + ", level: " + level + ", time_cost_s: " + time_cost_s + ", score: " + score + " } " + ( (res)?("success"):("fail") ) );
    return res;
}

std::vector<std::vector<std::string>> Database::select_all_records() {
    std::string sql("select * from record");
    mysql_query(con, sql.c_str());
    return get_select_all_result(con);
}

boolean Database::delete_record(const std::string &sno, const int &level, const int &index) {
    std::string NAME("delete_record()");

    std::string sno_e(sno);
    std::string level_e(std::to_string(level));
    std::string index_e(std::to_string(index));
    escape(sno_e, false);
    escape(level_e, false);
    escape(index_e, false);

    std::string count_sql = std::string("select * from record where sno='") + sno_e + "' and level=" + level_e + " and idx=" + index_e;

    std::string sql = count_sql;

    mysql_query(con, sql.c_str());

    MYSQL_RES *sql_res;
    sql_res = mysql_store_result(con);
    unsigned int num_before = mysql_num_rows(sql_res);
    mysql_free_result(sql_res);

    sql = std::string("delete from record where sno='") + sno_e + "' and level=" + level_e + " and idx=" + index_e;
    mysql_query(con, sql.c_str());

    sql = count_sql;
    mysql_query(con, sql.c_str());

    sql_res = mysql_store_result(con);
    unsigned int num_after = mysql_num_rows(sql_res);
    mysql_free_result(sql_res);

    boolean res = (num_before > num_after);

    Log::log(NAME, "delete record { sno: " + sno + ", level: " + std::to_string(level) + ", index: " + std::to_string(index) + " } " + ( (res)?("success"):("fail") ) );

    if (res){
        std::string update_idx_sql = "update record set idx=idx-1 where idx>" + index_e;
        mysql_query(con, update_idx_sql.c_str());
    }

    return res;
}

