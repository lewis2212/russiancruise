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
		RCQuest();
		~RCQuest();
		void init(const char* RootDir, void *insim);
		void insim_mci();
		void readconfig(const char *Track);

	private:

		map <byte, QuestPlayer> players;

		void    insim_ncn( struct IS_NCN* packet );		// Новый игрок зашел на сервер
		void    insim_npl( struct IS_NPL* packet );		// Игрок вышел из боксов
		void    insim_plp( struct IS_PLP* packet );		// Игрок ушел в боксы
		void    insim_pll( struct IS_PLL* packet );		// Игрок ушел в зрители
		void    insim_cnl( struct IS_CNL* packet );		// Игрок ушел с сервера
		void    insim_cpr( struct IS_CPR* packet );		// Игрок переименовался
		void    insim_mso( struct IS_MSO* packet );		// Игрок отправил сообщение
		void    insim_mci( struct IS_MCI *packet );
};

#endif
