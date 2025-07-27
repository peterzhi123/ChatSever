#include "../../include/ChatService.hpp"
#include "../../include/public.hpp"

ChatService *ChatService::getInstance()
{
    static ChatService service;
    return &service;
}

ChatService::ChatService()
{
    _msghandleMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2)});
    _msghandleMap.insert({REG_MSG, std::bind(&ChatService::registe, this, _1, _2)});
    _msghandleMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2)});
}

msghandle &ChatService::GetHandle(int msgType)
{
    return _msghandleMap[msgType];
}

void ChatService::login(const TcpConnectionPtr &conn, json &js)
{
    // 获取json登录信息
    int id = js["id"].get<int>();
    string passwd = js["passwd"];

    // 登录验证
    User user = _userModel.query(id);
    if (user.getId() == id && user.getPwd() == passwd)
    {
        if (user.getState() == "online")
        {
            json respond;
            respond["msqid"] = LOGIN_MSG_ACK;
            respond["error"] = 2;
            respond["msg"] = "这个账户已登录!";
            conn->send(respond.dump() + "\n");
        }
        else
        {
            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({user.getId(), conn});
            }

            user.setState("online");
            _userModel.updateState(user);

            json respond;
            respond["msqid"] = LOGIN_MSG_ACK;
            respond["error"] = 0;
            respond["msg"] = "登录成功!";
            conn->send(respond.dump() + "\n");
        }
    }
    else
    {
        json respond;
        respond["msqid"] = LOGIN_MSG_ACK;
        respond["error"] = 1;
        respond["msg"] = "登录失败。密码错误或者用户不存在!";
        conn->send(respond.dump() + "\n");
    }
}

void ChatService::registe(const TcpConnectionPtr &conn, json &js)
{
    User user;
    user.setName(js["name"]);
    user.setPwd(js["passwd"]);

    if (_userModel.insert(user))
    {
        // 注册成功
        json respond;
        respond["msgid"] = REG_MSG;
        respond["error"] = 0;
        respond["id"] = user.getId();
        conn->send(respond.dump() + "\n");
    }
    else
    {
        // 注册失败
        json respond;
        respond["msgid"] = REG_MSG;
        respond["error"] = 1;
        conn->send(respond.dump() + "\n");
    }
}

void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                // 从map表删除用户的链接信息
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    // 更新用户的状态信息
    if (user.getId() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
}

void ChatService::oneChat(const TcpConnectionPtr &conn, json &js)
{
    int toid = js["toid"].get<int>();

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end())
        {
            // toid在线，转发消息   服务器主动推送消息给toid用户
            it->second->send(js.dump() + "\n");
            return;
        }
    }

    // toid不在线，存储离线消息
}
