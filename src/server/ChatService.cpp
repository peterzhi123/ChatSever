#include "../../include/ChatService.hpp"
#include "../../include/public.hpp"

ChatService* ChatService::getInstance()
{
    static ChatService service;
    return &service;
}

ChatService::ChatService()
{
    _msghandleMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2)});
    _msghandleMap.insert({REGISTER_MSG, std::bind(&ChatService::registe, this, _1, _2)});
}

msghandle& ChatService::GetHandle(int msgType)
{
    return _msghandleMap[msgType];
}

void ChatService::login(const TcpConnectionPtr &conn, json &js)
{
    cout << "这是登录" << endl;
}

void ChatService::registe(const TcpConnectionPtr &conn, json &js)
{
    cout << "这是注册" << endl;
}
