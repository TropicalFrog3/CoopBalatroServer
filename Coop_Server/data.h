#ifndef DATA_H__
#define DATA_H__
#include <string>
#include <vector>

class Data
{
public:
    Data()
    {
    }
    ~Data()
    {

    }

    /////////////////// LOBBY OPTIONS ///////////////////
    bool different_decks = false;
    bool death_on_round_loss = true;
    std::string custom_seed = "random";
    int stake = 1;
    bool no_gold_on_round_loss = false;
    std::string sleeve = "sleeve_casl_none";
    bool different_seeds = false;
    int showdown_starting_antes = 3;
    int coop_starting_antes = 3;
    bool gold_on_life_loss = true;
    int starting_lives = 4;
    std::string back = "Red Deck";
    bool multiplayer_jokers = true;
    /////////////////////////////////////////////////////

    ////////////////// PLAYER GAME DATA /////////////////
    unsigned long long id;
    unsigned int lives;
    unsigned int gold;
    unsigned int ante;
    unsigned int round;
    unsigned int hand;
    unsigned int deck;
    unsigned int seed;
    /////////////////////////////////////////////////////

    /////////////////// CARD DATA ///////////////////////
    struct Card
    {
        unsigned int uuid;
    };
    std::vector<Card> cards;
    /////////////////////////////////////////////////////
};

#endif // DATA_H__