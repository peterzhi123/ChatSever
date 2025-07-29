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
# include "../include/model/friendmodel.hpp"
# include "../include/model/group.hpp"
# include "../include/model/groupmodel.hpp"

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

    // 登录业务
    void login(const TcpConnectionPtr &conn, json &js);

    // 注册业务
    void registe(const TcpConnectionPtr &conn, json &js);

    // 客户端异常退出时需要执行的逻辑
    void clientCloseException(const TcpConnectionPtr &conn);

    // 点对点聊天业务
    void oneChat(const TcpConnectionPtr &conn, json &js);

    // 服务器重启业务逻辑
    void reset();

    // 添加好友业务
    void addFriend(const TcpConnectionPtr &conn, json &js);

    // 创建群组业务
    void createGroup(const TcpConnectionPtr &conn, json &js);

    // 加入群组业务
    void addGroup(const TcpConnectionPtr &conn, json &js);

    // 群组聊天业务
    void groupChat(const TcpConnectionPtr &conn, json &js);

    // 登出业务
    void loginout(const TcpConnectionPtr &conn, json &js);

private:
    ChatService();

    unordered_map<int, msghandle> _msghandleMap;

    UserModel _userModel;
    OfflineMsgModel _offlineMsgModel;
    FriendModel _friendModel;
    GroupModel _groupModel;
    
    unordered_map<int, TcpConnectionPtr> _userConnMap;

    mutex _connMutex;
};

# endif