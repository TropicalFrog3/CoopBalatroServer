#ifndef GAME_H__
#define GAME_H__

#include <vector>
#include <unordered_map>
#include <string>
#include "data.h"
#include "player.h"

class Game
{
public: 
    std::vector<Player> players;
    bool inPvp = false;

public:
    Game();
    ~Game();

    void StartGame(std::vector<unsigned long long> IDs, unsigned int life);
    void SetAnte(unsigned long long ID, unsigned int Ante);
    void SetPlayHand(unsigned long long ID, std::unordered_map<std::string, std::string> &action);
    void SetLocation(unsigned long long ID, std::string Location);
    void SetSkip(unsigned long long ID, unsigned int Skips);

    std::string GetPlayHand(unsigned long long ID);
    std::string GetLocation(unsigned long long ID);
    unsigned int GetLife(unsigned long long ID);

    void ReadyBlind(unsigned long long ID);
    void UnReadyBlind(unsigned long long ID);
    bool AllReadyBlind();

    // return isdead
    bool ReduceLife(unsigned long long ID);

    // return endPvP id
    unsigned long long int isEndPvP();
   
};


#endif // GAME_H__