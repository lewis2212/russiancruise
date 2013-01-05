// тут уже пишется вся логика и работа методов
#include "RCQuest.h"

RCQuest::RCQuest(){}

RCQuest::~RCQuest(){}

void RCQuest::init(const char* RootDir, void *insim)
{
	strcpy(RCQuest::RootDir, RootDir);

	RCQuest::insim = (CInsim *)insim;

    if(!RCQuest::insim)
    {
        printf("RCQuest Error: Can't struct CInsim class\n");
        return;
    }

}

// обработчик события когда на сервер заходит новый пользователь
void RCQuest::insim_ncn()
{
	struct IS_NCN *packet = (struct IS_NCN*)insim->get_packet();

	if( packet->UCID == 0 )
	return;

	int i;
	for( i = 0; i < MAX_PLAYERS; i++)
	{
		if( players[i].UCID == 0 )
			break;
	}

	if( i == MAX_PLAYERS )
		return;

	players[i].UCID = packet->UCID;
	strcpy(players[i].UName, packet->UName);
	strcpy(players[i].PName, packet->PName);
}

void RCQuest::insim_npl()
{
 // тут идет обработка данных когда игрок вышел из питов
}

void RCQuest::insim_plp()
{

}

void RCQuest::insim_pll()
{

}

void RCQuest::insim_cnl()
{
	struct IS_CNL *packet = (struct IS_CNL*)insim->get_packet();

	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		if( players[i].UCID == packet->UCID )
		{
			memset(&players[i], 0, sizeof( struct QuestPlayer ) );
		}
	}
}

void RCQuest::insim_crp()
{

}

void RCQuest::insim_mso()
{
	// тут я добавилkgfdhghdfkgjhdfет потом удалить
}

void RCQuest::insim_mci()
{

}

