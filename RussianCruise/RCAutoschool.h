#ifndef RCAUTOSCHOOL_H
#define RCAUTOSCHOOL_H

#include "RCBaseClass.h"

struct SchoolPlayer
{
	struct	CompCar	Info;
	string	UName;
	string	PName;
	string	CName;

	float			Rate = 0;           // Рейтинг Игрока
	float			TempRate = 0;       // Временный рейтинг на время выполнения упражнения
	unsigned int	LastTaskTime = 0;   // Время последного выполнения задачи
};

class RCAutoschool : public RCBaseClass
{
	private:
		map <byte, SchoolPlayer>players;          // Структура игроков

		void InsimNCN( struct IS_NCN* packet );   // Новый игрок зашел на сервер
		void InsimNPL( struct IS_NPL* packet );   // Игрок вышел из боксов
		void InsimPLP( struct IS_PLP* packet );   // Игрок ушел в боксы
		void InsimPLL( struct IS_PLL* packet );   // Игрок ушел в зрители
		void InsimCNL( struct IS_CNL* packet );   // Игрок ушел с сервера
		void InsimCPR( struct IS_CPR* packet );   // Игрок переименовался
		void InsimMSO( struct IS_MSO* packet );   // Игрок отправил сообщение
		void InsimCON( struct IS_CON* packet );
		void InsimOBH( struct IS_OBH* packet );
		void InsimHLV( struct IS_HLV* packet );
		void InsimAXM( struct IS_AXM* packet );

	public:
		RCAutoschool();
		~RCAutoschool();

		void InsimMCI( struct IS_MCI* packet );   // Пакет с данными о координатах и т.д.
};

#endif // RCAUTOSCHOOL_H
