#ifndef SERVER_H__
#define SERVER_H__

#include <unordered_map>
#include <iostream>
#include <winsock2.h>
#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include <memory>
#include "clientManager.h"

#define PORT 8788
#define IP "127.0.0.1"
#define IPR "balatro.virtualized.dev"
extern std::atomic<bool> running;


class Server
{
public:
    Server();
    ~Server();
    void startServer();

private:
    SOCKET serverSocket;
    SOCKADDR_IN serverAddr;
    WSADATA wsaData;

    unsigned int id = -1;
    std::unordered_map<unsigned int, SOCKET> clients;

};

#endif // SERVER_H__