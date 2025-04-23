#include "lobby.h"

std::string Lobbies::addLobby(const Lobby::Gamemodes &type)
{
    std::string code = GenCode();
    m_lobbies.push_back(Lobby(type, code));
    return code;
}

void Lobbies::removeLobby(const std::string &code)
{
    for (auto it = m_lobbies.begin(); it != m_lobbies.end(); ++it)
    {
        if (it->code == code)
        {
            m_lobbies.erase(it);
            return;
        }
    }
}

Lobby *Lobbies::getLobby(const std::string &code)
{
    for (auto &lobby : m_lobbies)
    {
        if (lobby.code == code)
        {
            return &lobby;
        }
    }
    return nullptr;
}

size_t Lobbies::getLobbyCount() const
{
    return m_lobbies.size();
}

std::string Lobbies::GenCode()
{
    // NOT OPTIMAL BUT WHO CARES LMAO
    // AAAAA
    // AAAAB
    // AAAAC
    // etc...

    if (AvCode != "ZZZZZ") 
    {
        for (int i = AvCode.size() - 1; i >= 0; --i)
        {
            if (AvCode[i] == 'Z')
            {
                AvCode[i] = 'A';
            }
            else
            {
                AvCode[i]++;
                break;
            }
        }
    }
    else // search for a new code
    {
        std::string code = "AAAAA";
        while (codeExists(code))
        {
            for (int i = code.size() - 1; i >= 0; --i)
            {
                if (code[i] == 'Z')
                {
                    code[i] = 'A';
                }
                else
                {
                    code[i]++;
                    break;
                }
            }
        }
    }

    return AvCode;
}

bool Lobbies::codeExists(const std::string &code)
{
    for (auto &lobby : m_lobbies)
    {
        if (lobby.code == code)
        {
            return true;
        }
    }
    return false;
}

