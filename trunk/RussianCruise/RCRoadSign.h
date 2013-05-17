#ifndef _RC_ROADSIGN
#define _RC_ROADSIGN

#include "RCBaseClass.h"

struct RoadPlayer
{
    struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
};

class RCRoadSign : public RCBaseClass
{
	public:
		RCRoadSign();
		~RCRoadSign();
		int Init(const char *dir,void *CInSim, void *Message, void *RCDLic);
		void ReadConfig(const char *Track);
	protected:
	private:

		map< byte, RoadPlayer > players;

		void InsimNCN( struct IS_NCN* packet );   // Новый игрок зашел на сервер
		void InsimNPL( struct IS_NPL* packet );   // Игрок вышел из боксов
		void InsimPLP( struct IS_PLP* packet );   // Игрок ушел в боксы
		void InsimPLL( struct IS_PLL* packet );   // Игрок ушел в зрители
		void InsimCNL( struct IS_CNL* packet );   // Игрок ушел с сервера
		void InsimCPR( struct IS_CPR* packet );   // Игрок переименовался
		void InsimMSO( struct IS_MSO* packet );   // Игрок отправил сообщение
};

#endif // _RC_ROADSIGN
