# ifndef CHATSERVICE_H
# define CHATSERVICE_H

# include <iostream>
# include <functional>
# include <unordered_map>
# include <mutex>
# include "../thirdparty/json.hpp"
# include "../model/userModel.hpp"
# include "../include/model/userModel.hpp"
# include "../include/model/offlinemessagemodel.hpp"

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

    void clientCloseException(const TcpConnectionPtr &conn);

    void oneChat(const TcpConnectionPtr &conn, json &js);

    void reset();

private:
    ChatService();

    unordered_map<int, msghandle> _msghandleMap;

    UserModel _userModel;
    OfflineMsgModel _offlineMsgModel;
    
    unordered_map<int, TcpConnectionPtr> _userConnMap;

    mutex _connMutex;
};

# endif