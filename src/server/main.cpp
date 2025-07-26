# include <iostream>
# include "../../include/ChatServer.hpp"
using namespace std;
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

    EventLoop loop;
    InetAddress listenAddr(ip, port);
    string nameAddr = "chatServer";
    ChatServer myServer(&loop, listenAddr, nameAddr);
    myServer.start();
    loop.loop();
    return 0;
}