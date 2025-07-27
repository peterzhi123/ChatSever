# include <iostream>
# include <signal.h>
# include "../../include/ChatServer.hpp"
using namespace std;

// 处理服务器ctrl+c结束后，重置user的状态信息
void resetHandler(int)
{
    ChatService::getInstance()->reset();
    exit(0);
}

int main(int argc, char **argv)
{
    char *ip;
    uint16_t port;

    if (argc < 3)
    {
        ip = (char *)"127.0.0.1"; // 默认IP
        port = atoi("6000");      // 默认端口
    }
    else
    {
        ip = argv[1]; // 从命令行读取
        port = atoi(argv[2]);
    }

    signal(SIGINT, resetHandler);
    
    EventLoop loop;
    InetAddress listenAddr(ip, port);
    string nameAddr = "chatServer";
    ChatServer myServer(&loop, listenAddr, nameAddr);
    myServer.start();
    loop.loop();
    return 0;
}