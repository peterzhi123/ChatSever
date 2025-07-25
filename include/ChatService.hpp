# ifndef CHATSERVICE_H
# define CHATSERVICE_H

# include <iostream>
# include <functional>
# include <unordered_map>
# include "../thirdparty/json.hpp"

#include <muduo/net/TcpServer.h>
using namespace muduo;
using namespace muduo::net;

using json = nlohmann::json;
using namespace std;
using msghandle = std::function<void(const TcpConnectionPtr &conn, json &js)>;

class ChatService
{
public:
    static ChatService* getInstance();

    msghandle& GetHandle(int msgType);

    void login(const TcpConnectionPtr &conn, json &js);

    void registe(const TcpConnectionPtr &conn, json &js);

private:
    ChatService();

    unordered_map<int, msghandle> _msghandleMap;
};

# endif