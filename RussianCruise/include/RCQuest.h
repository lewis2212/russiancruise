#ifndef _RC_QUEST_H
#define _RC_QUEST_H

#include "RCBaseClass.h"

// Задаем структуру игрока

struct Quests
{
    bool current;
    byte step;
    bool complete;
};

struct QuestPlayer
{
    //struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    byte    UCID;                  // Connection ID
    byte    PLID;                  // PLayer ID
    char    CName[4];              // Car Name
    map<string, Quests> quest;
};


// тут описание всех методов и свойств класса
class RCQuest:public RCBaseClass
{
public:
    RCQuest(const char* Dir);
    ~RCQuest();
    void init(MYSQL *conn, CInsim *InSim);
    bool InsimMCI( struct IS_MCI *packet );
    bool ReadConfig(const char *Track);

private:

    map <byte, QuestPlayer> players;

    bool InsimNCN( struct IS_NCN* packet );     // Новый игрок зашел на сервер
    bool InsimNPL( struct IS_NPL* packet );     // Игрок вышел из боксов
    bool InsimPLP( struct IS_PLP* packet );     // Игрок ушел в боксы
    bool InsimPLL( struct IS_PLL* packet );     // Игрок ушел в зрители
    bool InsimCNL( struct IS_CNL* packet );     // Игрок ушел с сервера
    bool InsimCPR( struct IS_CPR* packet );     // Игрок переименовался
    bool InsimMSO( struct IS_MSO* packet );     // Игрок отправил сообщение
};

#endif
