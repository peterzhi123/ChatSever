// 数据库配置信息
# ifndef MYSQL_H
# define MYSQL_H

# include <string>
# include <mysql/mysql.h>
using namespace std;

// 数据库操作类
class MySQL
{
public:
    // 初始化数据库连接
    MySQL();

    // 释放数据库连接资源
    ~MySQL();

    // 连接数据库
    bool connect();

    // 更新操作
    bool update(string sql);

    // 查询操作
    MYSQL_RES *query(string sql);

    MYSQL* getConnection();
    

private:
    MYSQL *_conn;
};

# endif 