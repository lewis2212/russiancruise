#ifndef _RC_AUTOSCHOOL_H
#define _RC_AUTOSCHOOL_H

#include "RCBaseClass.h"
#include "RCMessage.h"

struct SchoolPlayer:public GlobalPlayer
{
	struct	CompCar	Info;
	string	UName;
	string	PName;
	string	CName;

	string	Lesson = "shicane";

	bool	OnPlace = false;
	bool 	Started = false;
	bool	Finished = false;
	unsigned int		CheckPoint = 0;


	float			Rate = 0;           // Рейтинг Игрока
	float			TempRate = 0;       // Временный рейтинг на время выполнения упражнения
	unsigned int	LastTaskTime = 0;   // Время последного выполнения задачи
};

class RCAutoschool : public RCBaseClass
{
	private:
		map <byte, SchoolPlayer>players;          // Структура игроков

		RCMessage * msg;

		void InsimAXM( struct IS_AXM* packet );
		void InsimCNL( struct IS_CNL* packet );   // Игрок ушел с сервера
		void InsimCON( struct IS_CON* packet );
		void InsimCPR( struct IS_CPR* packet );   // Игрок переименовался
		void InsimHLV( struct IS_HLV* packet );
		void InsimMCI( struct IS_MCI* packet );   // Пакет с данными о координатах и т.д.
		void InsimMSO( struct IS_MSO* packet );   // Игрок отправил сообщение
		void InsimNCN( struct IS_NCN* packet );   // Новый игрок зашел на сервер
		void InsimNPL( struct IS_NPL* packet );   // Игрок вышел из боксов
		void InsimOBH( struct IS_OBH* packet );
		void InsimPLL( struct IS_PLL* packet );   // Игрок ушел в зрители
		void InsimPLP( struct IS_PLP* packet );   // Игрок ушел в боксы

		void ShowPanel(byte UCID);

	public:
		RCAutoschool(const char *dir);
		~RCAutoschool();

		void init(DBMySQL *db, CInsim *insim, RCMessage *msg);
		void ReadConfig(const char* Track);

};

#endif // RCAUTOSCHOOL_H
