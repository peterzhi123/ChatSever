# include <iostream>
# include "../../include/ChatServer.hpp"
using namespace std;
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cout << "command invalid! example: ./ChatServer 127.0.0.1 6000" << endl;
        exit(-1);
    }

    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    EventLoop loop;
    InetAddress listenAddr(ip, port);
    string nameAddr = "chatServer";
    ChatServer myServer(&loop, listenAddr, nameAddr);
    myServer.start();
    loop.loop();
    return 0;
}