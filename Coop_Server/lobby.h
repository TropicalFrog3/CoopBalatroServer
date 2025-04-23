#ifndef LOBBY_H__
#define LOBBY_H__

#include <vector>
#include <string>
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <random>
#include "data.h"
#include "Game.h"

class Lobby
{
private:
    std::string seedCode = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int seedLength = 8;

public:
    enum Gamemodes
    {
        start_attrition,
        start_showdown,
        start_coop
    };

    struct PlayerLobby
    {
        std::string username;
        std::string modHash;
        unsigned long long id;
        int mouseX;
        int mouseY;
        int resolutionX;
        int resolutionY;

    };
    std::vector<PlayerLobby> players;
    std::string code;
    Gamemodes gamemode;

    Data data;
    Game *ThisLobbyGame;

public:
    Lobby(const Lobby::Gamemodes &Type, const std::string &Code)
    {
        gamemode = Type;
        code = Code;
        ThisLobbyGame = nullptr;

        if (gamemode == Lobby::start_attrition)
        {
            ThisLobbyGame = new Game();
        }
    }

    ~Lobby()
    {
    }

    void GenerateSeed()
    {
        if (data.custom_seed != "random")
            return;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> distrib(0, seedCode.size() - 1);
        std::string seed;

        seed.reserve(seedLength);

        for (int i = 0; i < seedLength; i++)
        {
            seed += seedCode[distrib(gen)];
        }

        data.custom_seed = seed;
    }

    void SetMousePos(unsigned long long ID, int X, int Y)
    {
        for (auto &i : players)
        {
            if (i.id == ID)
            {
                i.mouseX = X;
                i.mouseY = Y;
            }
        }
    }

    void SetResolution(unsigned long long ID, int X, int Y)
    {
        for (auto &i : players)
        {
            if (i.id == ID)
            {
                i.resolutionX = X;
                i.resolutionY = Y;
            }
        }
    }

    std::string GetResolution(unsigned long long ID)
    {
        std::string message = "action:resolution,";
        for (auto &i : players)
        {
            if (i.id == ID)
            {
                message += "X:" + std::to_string(i.resolutionX) + ",";
                message += "Y:" + std::to_string(i.resolutionY);
            }
        }
        return message + "\n";
    }
};

class Lobbies
{
public:
    Lobbies(const Lobbies &) = delete;
    Lobbies &operator=(const Lobbies &) = delete;

    static Lobbies &getInstance()
    {
        static Lobbies instance;
        return instance;
    }

    std::string addLobby(const Lobby::Gamemodes &type);
    void removeLobby(const std::string &code);
    Lobby *getLobby(const std::string &code);
    size_t getLobbyCount() const;

private:
    Lobbies() = default;
    ~Lobbies()
    {
    }

    std::string GenCode();
    bool codeExists(const std::string &code);
    std::string AvCode = "AAAAA";

    std::vector<Lobby> m_lobbies;
    std::mutex m_mutex;
};

#endif // LOBBY_H__