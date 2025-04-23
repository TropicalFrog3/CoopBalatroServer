#ifndef TOOLS_H__
#define TOOLS_H__
#include <iostream>
#include <string>
#include <unordered_map>
#include "lobby.h"

class Tools
{
public:
    Lobby::Gamemodes StringToGamemode(const std::string &str)
    {
        if (str == "start_attrition" || str == "attrition")
        {
            return Lobby::start_attrition;
        }
        else if (str == "start_showdown" || str == "showdown")
        {
            return Lobby::start_showdown;
        }
        else if (str == "start_coop" || str == "coop")
        {
            return Lobby::start_coop;
        }
        else
        {
            return Lobby::start_attrition;
        }
    }

    std::string GamemodeToString(const Lobby::Gamemodes &mode)
    {
        switch (mode)
        {
        case Lobby::start_attrition:
            return "attrition";
        case Lobby::start_showdown:
            return "showdown";
        case Lobby::start_coop:
            return "coop";
        default:
            return "attrition";
        }
    }

    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> string_to_table(const std::string &str)
    {
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> tbl;
        bool actionFlag = false;

        std::string action;
        std::string currentKey;
        std::string temp;

        for (auto &c : str)
        {
            if (c == ',')
            {
                if (!actionFlag)
                {
                    // action:username
                    action = temp;
                    actionFlag = true;
                    tbl[action] = std::unordered_map<std::string, std::string>();
                }
                else
                {
                    // Guest
                    tbl[action][currentKey] = temp;
                    temp.clear();
                    currentKey.clear();
                }
                temp.clear();
            }
            else if (actionFlag && c == ':')
            {
                // username
                currentKey = temp;
                tbl[action][currentKey] = "";
                temp.clear();
            }
            else if (c == '\n')
            {
                // End of an action block
                if (!temp.empty() && !currentKey.empty())
                {
                    tbl[action][currentKey] = temp;
                }
                else if (!temp.empty() && currentKey.empty())
                {
                    tbl[temp] = std::unordered_map<std::string, std::string>();
                }
                actionFlag = false;
                temp.clear();
                currentKey.clear();
            }
            else
            {
                temp += c;
            }
        }
        return tbl;
    }
};

#endif // TOOLS_H__