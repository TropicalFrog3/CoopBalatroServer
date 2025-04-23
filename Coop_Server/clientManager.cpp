#include "clientManager.h"

ClientManager::ClientManager()
{
}

ClientManager::~ClientManager()
{
}

void ClientManager::handleClient(SOCKET clientSocket, struct sockaddr_in clientAddr)
{
    client = clientSocket;
    Addr = clientAddr;
    char buffer[1024];
    int bytesReceived;

    while (running)
    {
        memset(buffer, 0, sizeof(buffer));

        bytesReceived = recv(client, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0)
        {
            std::string message(buffer, bytesReceived);
            if (message != "action:keepAlive\n" && message != "action:keepAliveAck\n" && message.find("action:mouseMove") == std::string::npos)
                std::cout << client << "-Received: " << message << std::endl;

            auto tbl = tools.string_to_table(message);

            for (auto &i : tbl)
            {
                if (i.first != "action:keepAlive" && i.first != "action:keepAliveAck" && i.first != "action:mouseMove")
                {
                    std::cout << client << "-Received: " << message << std::endl;
                }
                const std::string &action = i.first;
                if (action == "action:keepAlive" || action == "action:keepAliveAck")
                {
                    sendToClient(client, message);
                }
                else if (action == "action:username")
                {
                    sendToClient(client, ActionUser(i.second));
                }
                else if (action == "action:version")
                {
                    ActionVersion(i.second);
                }
                else if (action == "action:joinLobby")
                {
                    sendToClient(client, ActionJoinLobby(i.second));
                }
                else if (action == "action:createLobby")
                {
                    sendToClient(client, ActionCreateLobby(i.second));
                }
                else if (action == "action:leaveLobby")
                {
                    ActionLeaveLobby();
                    SendUpdate(std::vector<std::string>{"stopGame", "lobbyInfo", "lobbyOptions"});
                }
                else if (action == "action:lobbyInfo")
                {
                    if (i.second.size() == 0)
                        SendUpdate(std::vector<std::string>{"lobbyInfo"});
                    else
                    {
                        SetActionLobbyInfo(i.second);
                        // SendUpdate(std::vector<std::string>{"lobbyInfo"});
                    }
                }
                else if (action == "action:lobbyOptions")
                {
                    if (i.second.size() == 0)
                        SendUpdate(std::vector<std::string>{"lobbyOptions"});
                    else
                    {
                        SetActionLobbyOptions(i.second);
                        SendUpdate(std::vector<std::string>{"lobbyOptions"});
                    }
                }
                else if (action == "action:startGame")
                {
                    // Start the game
                    CurrentLobby->ThisLobbyGame = new Game();
                    CurrentLobby->ThisLobbyGame->StartGame(GetPlayersIDs(), CurrentLobby->data.starting_lives);
                    CurrentLobby->GenerateSeed();
                    if (tools.GamemodeToString(CurrentLobby->gamemode) == "coop")
                    {
                        SendUpdate(std::vector<std::string>{"startGame", "startCoopEx"});
                    }
                    else
                    {
                        SendUpdate(std::vector<std::string>{"startGame"});
                    }
                }
                else if (action == "action:setAnte")
                {
                    CurrentLobby->ThisLobbyGame->SetAnte(client, std::stoi(i.second["ante"]));
                }
                else if (action == "action:playHand")
                {
                    CurrentLobby->ThisLobbyGame->SetPlayHand(client, i.second);
                    // do a check up if is in isPvP end
                    unsigned long long int id = CurrentLobby->ThisLobbyGame->isEndPvP();
                    if (id != -1)
                    {
                        LoseLife(id);
                        SendUpdate(std::vector<std::string>{
                            "endpvp",
                        });
                    }
                    // update on other clients with action:enemyInfo
                    SendUpdate(std::vector<std::string>{"playHand"}, false);
                }
                else if (action == "action:setLocation")
                {
                    CurrentLobby->ThisLobbyGame->SetLocation(client, i.second["location"]);
                    SendUpdate(std::vector<std::string>{"setLocation"}, false);
                }
                else if (action == "action:newRound")
                {
                    // WTF DOES THIS DO HONESTLY
                }
                else if (action == "action:skip")
                {
                    CurrentLobby->ThisLobbyGame->SetSkip(client, std::stoi(i.second["skips"]));
                }
                else if (action == "action:readyBlind")
                {
                    CurrentLobby->ThisLobbyGame->ReadyBlind(client);
                    if (CurrentLobby->ThisLobbyGame->AllReadyBlind())
                    {
                        // start fight
                        SendUpdate(std::vector<std::string>{"startBlind"});
                    }
                }
                else if (action == "action:unreadyBlind")
                {
                    CurrentLobby->ThisLobbyGame->UnReadyBlind(client);
                }
                else if (action == "action:failRound")
                {
                    LoseLife(client);
                }
                else if (action == "action:stopGame")
                {
                    // stop the game, delete current game in the lobby
                    delete CurrentLobby->ThisLobbyGame;
                    CurrentLobby->ThisLobbyGame = nullptr;
                    CurrentLobby->data.custom_seed == "random";
                    SendUpdate(std::vector<std::string>{"stopGame"});
                }
                else if (action == "action:mouseMove")
                {
                    // update mouse position
                    SetMousePos(i.second);
                    SendUpdate(std::vector<std::string>{"mouseMove"}, false);
                }
                else if (action == "action:cardClick")
                {
                    // get card id
                    std::string msg = "action:cardClick,playing:" + i.second["playing"] + "\n";
                    // std::cout << msg << std::endl;
                    // send cardClick to all clients
                    for (auto &j : CurrentLobby->players)
                    {
                        if (j.id == client)
                            continue;
                        else
                            sendToClient(j.id, msg);
                    }
                }
                else if (action == "action:resolution")
                {
                    std::cout << "Resolution: " << i.second["X"] << "x" << i.second["Y"] << std::endl;
                    CurrentLobby->SetResolution(client, std::stoi(i.second["X"]), std::stoi(i.second["Y"]));
                    SendUpdate(std::vector<std::string>{"resolution"}, false);
                }
                else if (action == "action:handPlayed")
                {
                    std::cout << "Hand Played" << std::endl;
                    SendUpdate(std::vector<std::string>{"handPlayed"}, false);
                }
                else if (action == "action:discardCards")
                {
                    std::cout << "Discard Cards" << std::endl;
                    SendUpdate(std::vector<std::string>{"discardCards"}, false);
                }
                else
                {
                    std::cerr << "Unknown action: " << i.first << std::endl;
                }
            }
        }
        else if (bytesReceived == 0 || bytesReceived == -1)
        {
            // remove client from server
            closesocket(clientSocket);
            break;
        }
        else
        {
            closesocket(clientSocket);
            break;
        }
    }

    closesocket(clientSocket);
}

void ClientManager::sendToClient(SOCKET clientSocket, const std::string &message)
{
    // if (message != "action:keepAlive\n" && message != "action:keepAliveAck\n")
    //     std::cout << client << "-Sent: " << message << std::endl;
    send(clientSocket, message.c_str(), message.length(), 0);
}

void ClientManager::SendUpdate(std::vector<std::string> category, bool UpdateThisClient)
{
    int toi = 0;
    for (auto &j : CurrentLobby->players)
    {
        if (!UpdateThisClient && j.id == client)
            continue;

        for (auto &i : category)
        {
            if (i == "lobbyInfo")
                sendToClient(j.id, GetActionLobbyInfo(j.id));
            else if (i == "lobbyOptions")
                sendToClient(j.id, GetActionLobbyOptions());
            else if (i == "startGame")
            {
                sendToClient(j.id, GetActionStartGame());
                sendToClient(j.id, GetPlayerInfo());
            }
            else if (i == "playHand")
            {
                sendToClient(j.id, CurrentLobby->ThisLobbyGame->GetPlayHand(client));
            }
            else if (i == "setLocation")
            {
                sendToClient(j.id, CurrentLobby->ThisLobbyGame->GetLocation(client));
            }
            else if (i == "startBlind")
            {
                sendToClient(j.id, "action:startBlind\n");
            }
            else if (i == "loseGame")
            {
                if (j.id == client)
                    sendToClient(j.id, "action:loseGame\n");
                else
                    sendToClient(j.id, "action:winGame\n");
            }
            else if (i == "endpvp")
            {
                sendToClient(j.id, "action:endPvP\n");
            }
            else if (i == "stopGame")
            {
                sendToClient(j.id, "action:stopGame\n");
            }
            else if (i == "startCoopEx")
            {
                sendToClient(j.id, "action:startCoopEx\n");
            }
            else if (i == "mouseMove")
            {
                sendToClient(j.id, GetMousePos(client, toi));
            }
            else if (i == "resolution")
            {
                sendToClient(j.id, CurrentLobby->GetResolution(client));
            }
            else if (i == "handPlayed")
            {
                sendToClient(j.id, "action:handPlayed\n");
            }
            else if (i == "discardCards")
            {
                sendToClient(j.id, "action:discardCards\n");
            }
        }
        toi++;
    }
}

std::string ClientManager::ActionUser(std::unordered_map<std::string, std::string> &action)
{
    for (auto &j : action)
    {
        if (j.first == "username")
        {
            username = j.second;
            // std::cout << "Username: " << username << std::endl;
        }
        else if (j.first == "modHash")
        {
            modHash = j.second;
            // std::cout << "ModHash: " << modHash << std::endl;
        }
    }
    return std::string("action:version\n");
}

void ClientManager::ActionVersion(std::unordered_map<std::string, std::string> &action)
{
    for (auto &j : action)
    {
        if (j.first == "version")
        {
            if (j.second != COMPVERSION)
            {
                std::cerr << "Client version mismatch: " << j.second << std::endl;
            }
            version = j.second;
            // std::cout << "Version: " << j.second << std::endl;
        }
    }
}

std::string ClientManager::ActionJoinLobby(std::unordered_map<std::string, std::string> &action)
{
    for (auto &j : action)
    {
        if (j.first == "code")
        {
            Lobbies &lobbies = Lobbies::getInstance();
            CurrentLobby = lobbies.getLobby(j.second);
            if (CurrentLobby == nullptr)
            {
                return std::string("action:error,message:Lobby does not exist.\n");
            }
            else
            {
                // add player to lobby
                CurrentLobby->players.push_back(Lobby::PlayerLobby{username, modHash, client});

                return std::string("action:joinedLobby,code:" + CurrentLobby->code + ",type:" + tools.GamemodeToString(CurrentLobby->gamemode) + "\n");
            }
        }
    }
    return std::string();
}

std::string ClientManager::ActionCreateLobby(std::unordered_map<std::string, std::string> &action)
{
    for (auto &j : action)
    {
        if (j.first == "gameMode")
        {
            Lobbies &lobbies = Lobbies::getInstance();
            std::string code = lobbies.addLobby(tools.StringToGamemode(j.second));
            CurrentLobby = lobbies.getLobby(code);
            if (CurrentLobby == nullptr)
            {
                return std::string("action:error,message:Failed to create lobby.\n");
            }
            else
            {

                // create lobby info for first time
                CurrentLobby->players.push_back(Lobby::PlayerLobby{username, modHash, client});

                return std::string("action:joinedLobby,code:" + code + ",type:" + tools.GamemodeToString(CurrentLobby->gamemode) + "\n");
            }
        }
    }
}

void ClientManager::SetActionLobbyInfo(std::unordered_map<std::string, std::string> &action)
{
    for (auto &i : action)
    {
        const std::string cond = i.first;
        if (cond == "host")
        {
            CurrentLobby->players.push_back(Lobby::PlayerLobby{i.second, action["hostHash"], client});
        }
        else if (cond == "guest")
        {
            CurrentLobby->players.push_back(Lobby::PlayerLobby{i.second, action["guestHash"], client});
        }
    }
}

std::string ClientManager::GetActionLobbyInfo(unsigned long long MainID)
{
    std::string message = "action:lobbyInfo,";
    // search for guests
    bool hostSkip = true;
    for (auto &i : CurrentLobby->players)
    {
        if (hostSkip)
        {
            message += "host:" + i.username + ",";
            message += "hostHash:" + i.modHash + ",";
            message += "isHost:";
            bool Hosting = i.id == MainID;
            message += Hosting ? "true" : "false";
            hostSkip = !hostSkip;
        }
        else
        {
            message += ",guest:" + i.username + ",";
            message += "guestHash:" + i.modHash;
        }
    }
    return message + "\n";
}

void ClientManager::SetActionLobbyOptions(std::unordered_map<std::string, std::string> &action)
{
    for (auto &i : action)
    {
        const std::string cond = i.first;
        if (cond == "different_decks")
        {
            CurrentLobby->data.different_decks = i.second == "true";
        }
        else if (cond == "death_on_round_loss")
        {
            CurrentLobby->data.death_on_round_loss = i.second == "true";
        }
        else if (cond == "custom_seed")
        {
            CurrentLobby->data.custom_seed = i.second;
        }
        else if (cond == "stake")
        {
            CurrentLobby->data.stake = std::stoi(i.second);
        }
        else if (cond == "no_gold_on_round_loss")
        {
            CurrentLobby->data.no_gold_on_round_loss = i.second == "true";
        }
        else if (cond == "sleeve")
        {
            CurrentLobby->data.sleeve = i.second;
        }
        else if (cond == "different_seeds")
        {
            CurrentLobby->data.different_seeds = i.second == "true";
        }
        else if (cond == "showdown_starting_antes")
        {
            CurrentLobby->data.showdown_starting_antes = std::stoi(i.second);
        }
        else if (cond == "gold_on_life_loss")
        {
            CurrentLobby->data.gold_on_life_loss = i.second == "true";
        }
        else if (cond == "starting_lives")
        {
            CurrentLobby->data.starting_lives = std::stoi(i.second);
        }
        else if (cond == "back")
        {
            CurrentLobby->data.back = i.second;
        }
        else if (cond == "multiplayer_jokers")
        {
            CurrentLobby->data.multiplayer_jokers = i.second == "true";
        }
        else if (cond == "coop_starting_antes")
        {
            CurrentLobby->data.coop_starting_antes = std::stoi(i.second);
        }
    }
}

void ClientManager::LoseLife(unsigned long long int id)
{
    if (CurrentLobby->ThisLobbyGame->ReduceLife(id))
    {
        SendUpdate(std::vector<std::string>{"loseGame"});
    }
    else
    {
        sendToClient(id, "action:playerInfo,lives:" + std::to_string(CurrentLobby->ThisLobbyGame->GetLife(id)) + "\n");
    }
}

std::string ClientManager::GetActionLobbyOptions()
{
    std::string message = "action:lobbyOptions,";
    std::string temp;
    message += "gamemode:" + tools.GamemodeToString(CurrentLobby->gamemode) + ",";
    temp = CurrentLobby->data.different_decks ? "true" : "false";
    message += "different_decks:" + temp + ",";
    temp = CurrentLobby->data.death_on_round_loss ? "true" : "false";
    message += "death_on_round_loss:" + temp + ",";
    message += "custom_seed:" + CurrentLobby->data.custom_seed + ",";
    message += "stake:" + std::to_string(CurrentLobby->data.stake) + ",";
    temp = CurrentLobby->data.no_gold_on_round_loss ? "true" : "false";
    message += "no_gold_on_round_loss:" + temp + ",";
    message += "sleeve:" + CurrentLobby->data.sleeve + ",";
    temp = CurrentLobby->data.different_seeds ? "true" : "false";
    message += "different_seeds:" + temp + ",";
    message += "showdown_starting_antes:" + std::to_string(CurrentLobby->data.showdown_starting_antes) + ",";
    message += "coop_starting_antes:" + std::to_string(CurrentLobby->data.coop_starting_antes) + ",";
    temp = CurrentLobby->data.gold_on_life_loss ? "true" : "false";
    message += "gold_on_life_loss:" + temp + ",";
    message += "starting_lives:" + std::to_string(CurrentLobby->data.starting_lives) + ",";
    message += "back:" + CurrentLobby->data.back + ",";
    temp = CurrentLobby->data.multiplayer_jokers ? "true" : "false";
    message += "multiplayer_jokers:" + temp;

    return message + "\n";
}

void ClientManager::ActionLeaveLobby()
{
    // remove player from lobby
    for (auto it = CurrentLobby->players.begin(); it != CurrentLobby->players.end(); ++it)
    {
        if (it->id == client)
        {
            unsigned int size = CurrentLobby->players.size();
            CurrentLobby->players.erase(it);
            if (size == 1)
            {
                CurrentLobby->players.clear();
                CurrentLobby->code = "";
                Lobbies &lobbies = Lobbies::getInstance();
                lobbies.removeLobby(CurrentLobby->code);
                return;
            }

            return;
        }
    }
    return;
}

std::string ClientManager::GetActionStartGame()
{
    // startGame to client: {"deck":"c_multiplayer_1","seed":"57V35"}
    std::string message = "action:startGame,";
    message += "deck:c_multiplayer_1,"; // why the fuck is c_multipler_1 each time lmao
    message += "seed:" + CurrentLobby->data.custom_seed;
    return message + "\n";
}

std::string ClientManager::GetPlayerInfo()
{
    std::cout << CurrentLobby->data.starting_lives << std::endl;
    std::string message = "action:playerInfo,";
    message += "lives:";
    message += std::to_string(CurrentLobby->data.starting_lives);
    return message + "\n";
}

std::vector<unsigned long long int> ClientManager::GetPlayersIDs()
{
    std::vector<unsigned long long int> playerIDs;
    for (auto &p : CurrentLobby->players)
    {
        playerIDs.push_back(p.id);
    }
    return playerIDs;
}

void ClientManager::SetMousePos(std::unordered_map<std::string, std::string> &action)
{
    int MouseX;
    int MouseY;
    for (auto &i : action)
    {
        const std::string cond = i.first;
        if (cond == "X")
        {
            MouseX = std::stoi(i.second);
        }
        if (cond == "Y")
        {
            MouseY = std::stoi(i.second);
        }
    }
    CurrentLobby->SetMousePos(client, MouseX, MouseY);
}

std::string ClientManager::GetMousePos(unsigned long long int id, int to)
{
    std::string message = "action:mouseMove,";
    for (auto &i : CurrentLobby->players)
    {
        if (i.id == id)
        {
            if (i.resolutionX == 0 || i.resolutionY == 0)
            {
                message += "X:" + std::to_string(i.mouseX) + ",";
                message += "Y:" + std::to_string(i.mouseY);
            }
            else
            {
                message += "X:" + std::to_string((i.mouseX * CurrentLobby->players[to].resolutionX) / i.resolutionX) + ",";
                message += "Y:" + std::to_string(i.mouseY * CurrentLobby->players[to].resolutionY / i.resolutionY);
            }
        }
    }
    return message + "\n";
}

// TODO:

// - make seed random           unverified
// - stopGame                   unverified

// - sendPhantom                ??? TF IS THAT
// - removePhantom
// - speedrun                   THE JOKER CARD I THINK
// - asteroid                   meh lazy

/////// LOBBY OPTIONS EFFECTS ///////
/*
- Give comeback $ on life loss
- dont get blind rewards on round loss
- lose a life on non-pvp round loss
- player have different decks
- player have different seeds
- enable multiplayer card (didnt implement multiplayer card)
*/