#include "ChatServer.hpp"
#include <iostream>
#include <functional>
#include <string>
#include "../thirdparty/json.hpp"
using json = nlohmann::json;
using namespace std;

ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameAddr)
    : _server(loop, listenAddr, nameAddr), _loop(loop)
{
    _server.setConnectionCallback(std::bind(&ChatServer::onConection, this, _1));
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    _server.setThreadNum(4);
}

// 启动服务
void ChatServer::start()
{
    _server.start();
}

// 上报链接相关信息的回调函数
void ChatServer::onConection(const TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << " status: online" << endl;
    }
    else
    {
        cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << "    status: offline" << endl;
        conn->shutdown();
    }
}

// 上报读写事件相关信息的回调函数
void ChatServer::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buffer,
                           Timestamp time)
{
    string buff = buffer->retrieveAllAsString();
    // cout << "recv buff: " << buff << "  time: " << time.toFormattedString() << endl;
    // conn->send(buff);
    json js = json::parse(buff);

    int msgid = js["msgid"].get<int>();
    msghandle handle = ChatService::getInstance()->GetHandle(msgid);

    if (handle)
    {
        handle(conn, js);
    }
    else
    {
        std::cerr << "❌ 未找到对应消息处理函数，msgid = " << msgid << std::endl;
    }
}
