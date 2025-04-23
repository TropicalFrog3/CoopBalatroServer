#ifndef PLAYER_H__
#define PLAYER_H__

class Player
{
public:
    unsigned long long id;
    unsigned int lives;
    unsigned int ante;

    unsigned int score;
    unsigned int handsLeft;
    bool hasSpeedrun;
    unsigned int skips;

    std::string location;

    bool isReadyBlind;



};

#endif // PLAYER_H__