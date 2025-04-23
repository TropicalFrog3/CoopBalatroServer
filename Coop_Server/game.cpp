#include "game.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::StartGame(std::vector<unsigned long long> IDs, unsigned int life)
{
    for (auto &i : IDs)
    {
        players.push_back(Player{i});
        players.back().lives = life;
        players.back().score = 0;
        players.back().handsLeft = 0;
        players.back().hasSpeedrun = false;
        players.back().skips = 0;
        players.back().location = "";
        players.back().isReadyBlind = false;
    }
}

void Game::SetAnte(unsigned long long ID, unsigned int Ante)
{
    // players[ID].ante = Ante;
    for(auto &p : players)
    {
        if(p.id == ID)
        {
            p.ante = Ante;
        }
    }
}

void Game::SetPlayHand(unsigned long long ID, std::unordered_map<std::string, std::string> &action)
{
    for(auto &p : players)
    {
        if(p.id == ID)
        {
            p.score = std::stoi(action["score"]);
            p.handsLeft = std::stoi(action["handsLeft"]);
            p.hasSpeedrun = action["hasSpeedrun"] == "true";
        }
    }
}

std::string Game::GetPlayHand(unsigned long long ID)
{
    std::string message = "action:enemyInfo,";
    for(auto &p : players)
    {
        if(p.id == ID)
        {
            message += "handsLeft:" + std::to_string(p.handsLeft) + ",";
            message += "score:" + std::to_string(p.score) + ",";
            message += "skips:" + std::to_string(p.skips);
        }
    }
    return message + "\n";
}

void Game::SetLocation(unsigned long long ID, std::string Location)
{
    for(auto &p : players)
    {
        if(p.id == ID)
        {
            p.location = Location;
        }
    }
}

void Game::SetSkip(unsigned long long ID, unsigned int Skips)
{
    for(auto &p : players)
    {
        if(p.id == ID)
        {
            p.skips = Skips;
        }
    }
}

std::string Game::GetLocation(unsigned long long ID)
{
    std::string message = "action:enemyLocation,";
    for(auto &p : players)
    {
        if(p.id == ID)
        {
            message += "location:" + p.location;
        }
    }
    return message + "\n";
}

unsigned int Game::GetLife(unsigned long long ID)
{
    for(auto &p : players)
    {
        if(p.id == ID)
        {
            return p.lives;
        }
    }
    return 0;
}

void Game::ReadyBlind(unsigned long long ID)
{
    for(auto &p : players)
    {
        if(p.id == ID)
        {
            p.isReadyBlind = true;
        }
    }
}

void Game::UnReadyBlind(unsigned long long ID)
{
    for(auto &p : players)
    {
        if(p.id == ID)
        {
            p.isReadyBlind = false;
        }
    }
}

bool Game::AllReadyBlind()
{
    for(auto &p : players)
    {
        if(!p.isReadyBlind)
        {
            return false;
        }
    }
    inPvp = true;
    return true;
}

bool Game::ReduceLife(unsigned long long ID)
{
    for(auto &p : players)
    {
        if(p.id == ID)
        {
            p.lives--;
            return p.lives == 0;
        }
    }
    return false;
}

unsigned long long int Game::isEndPvP()
{
    if(!inPvp)
        return -1;
    
    unsigned int highestScore = 0;
    for (const auto& p : players)
    {
        if (p.handsLeft > 0)
        {
            highestScore = std::max(highestScore, p.score);
        }
    }

    for (const auto& p : players)
    {
        if (p.handsLeft == 0 && p.score < highestScore)
        {
            return p.id;
        }
    }

    return -1;
}
