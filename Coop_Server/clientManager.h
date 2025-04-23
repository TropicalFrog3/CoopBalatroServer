#ifndef CLIENTMANAGER_H__
#define CLIENTMANAGER_H__
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include "lobby.h"
#include "server.h"
#include "tools.h"
#include "Game.h"

#define COMPVERSION "0.1.8.3-MULTIPLAYER"

class ClientManager
{
public:
    ClientManager();
    ~ClientManager();
    void handleClient(SOCKET clientSocket, struct sockaddr_in clientAddr);
    void sendToClient(SOCKET clientSocket, const std::string &message);

private:
    void ActionLeaveLobby();
    void SendUpdate(std::vector<std::string> category, bool UpdateThisClient = true);
    void ActionVersion(std::unordered_map<std::string, std::string> &action);
    void SetActionLobbyInfo(std::unordered_map<std::string, std::string> &action);
    void SetActionLobbyOptions(std::unordered_map<std::string, std::string> &action);
    void LoseLife(unsigned long long int id);
    std::string ActionUser(std::unordered_map<std::string, std::string> &action);
    std::string ActionJoinLobby(std::unordered_map<std::string, std::string> &action);
    std::string ActionCreateLobby(std::unordered_map<std::string, std::string> &action);
    std::string GetActionLobbyInfo(unsigned long long MainID);
    std::string GetActionLobbyOptions();
    std::string GetActionStartGame();
    std::string GetPlayerInfo();
    std::vector<unsigned long long int> GetPlayersIDs();

    void SetMousePos(std::unordered_map<std::string, std::string> &action);
    std::string GetMousePos(unsigned long long int id, int to);

private:
    Tools tools;
    SOCKET client;
    struct sockaddr_in Addr;
    std::string username;
    std::string modHash;
    std::string version;

    Lobby *CurrentLobby;
};

#endif // CLIENTMANAGER_H__