#ifndef _RC_QUEST_H
#define _RC_QUEST_H

#include "RCBaseClass.h"

struct QuestPlayer: GlobalPlayer
{
    Json::Value quest;
};


// тут описание всех методов и свойств класса
class RCQuest:public RCBaseClass
{
public:
    RCQuest(const char* Dir);
    ~RCQuest();
    void init(DBMySQL *db, CInsim *InSim);

private:
    map <byte, QuestPlayer> players;

    void    ReadConfig(const char *Track);

    void    InsimCNL( struct IS_CNL* packet );      // Игрок ушел с сервера
    void    InsimCPR( struct IS_CPR* packet );      // Игрок переименовался
    void    InsimMCI( struct IS_MCI *packet );
    void    InsimMSO( struct IS_MSO* packet );      // Игрок отправил сообщение
    void    InsimNCN( struct IS_NCN* packet );      // Новый игрок зашел на сервер
    void    InsimNPL( struct IS_NPL* packet );      // Игрок вышел из боксов
    void    InsimPLL( struct IS_PLL* packet );      // Игрок ушел в зрители
    void    InsimPLP( struct IS_PLP* packet );      // Игрок ушел в боксы
};

#endif
