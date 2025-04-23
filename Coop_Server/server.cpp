#include "server.h"

std::atomic<bool> running(true);

Server::Server()
{
}

Server::~Server()
{
    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
    }
    WSACleanup();  // Cleanup Winsock
}


void Server::startServer()
{
    WSADATA wsaData;
    SOCKET serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed with error code: " << WSAGetLastError() << std::endl;
        return;
    }

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed with error code: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    // Setup the server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Bind failed with error code: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    // Listen for incoming connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed with error code: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    std::vector<std::thread> clientThreads;

    while (running)
    {
        // Accept incoming client connections
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Accept failed with error code: " << WSAGetLastError() << std::endl;
            break;
        }

        std::cout << "Client connected." << std::endl;
        std::string connectedMessage = "action:connected\n";
        send(clientSocket, connectedMessage.c_str(), connectedMessage.length(), 0);

        auto clientManager = std::make_shared<ClientManager>();
        std::thread clientThread(&ClientManager::handleClient, clientManager, clientSocket, clientAddr);

        clientThreads.push_back(std::move(clientThread));
    }

    for (auto& thread : clientThreads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    closesocket(serverSocket);
    WSACleanup();
}