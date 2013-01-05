#ifndef _RC_QUEST_H
#define _RC_QUEST_H

#include "RCBaseClass.h"

// Задаем структуру игрока



typedef struct QuestPlayer
{
    //struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    byte    UCID;                  // Connection ID
    byte    PLID;                  // PLayer ID
    char    CName[4];              // Car Name
} _QuestPlayer;


// тут описание всех методов и свойств класса
class RCQuest:public RCBaseClass
{
    public:
        RCQuest();
        ~RCQuest();
        void init(const char* RootDir, void *insim);
        void insim_mci();

    private:

		QuestPlayer players[MAX_PLAYERS];

        void    insim_ncn();    // Новый игрок зашел на сервер
        void    insim_npl();    // Игрок вышел из боксов
        void    insim_plp();    // Игрок ушел в боксы
        void    insim_pll();    // Игрок ушел в зрители
        void    insim_cnl();    // Игрок ушел с сервера
        void    insim_crp();    // Игрок переименовался
        void    insim_mso();    // Игрок отправил сообщение
};

#endif
