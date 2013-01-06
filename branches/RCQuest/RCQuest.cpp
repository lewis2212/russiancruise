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

}// привет


// обработчик события когда на сервер заходит новый пользователь
void RCQuest::insim_ncn( struct IS_NCN *packet )
{

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
printf("UCID = %d UName = %s\n", packet->UCID, packet->UName);
	players[i].UCID = packet->UCID;
	strcpy(players[i].UName, packet->UName);
	strcpy(players[i].PName, packet->PName);
}

void RCQuest::insim_npl( struct IS_NPL *packet )
{
 // тут идет обработка данных когда игрок вышел из питов
}

void RCQuest::insim_plp( struct IS_PLP *packet )
{

}

void RCQuest::insim_pll( struct IS_PLL *packet )
{

}

void RCQuest::insim_cnl( struct IS_CNL *packet )
{
	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		if( players[i].UCID == packet->UCID )
		{
			memset(&players[i], 0, sizeof( struct QuestPlayer ) );
		}
	}
}

void RCQuest::insim_cpr( struct IS_CPR *packet )
{

}

void RCQuest::insim_mso( struct IS_MSO *packet )
{
	// тут я добавилkgfdhghdfkgjhdfет потом удалить
}

void RCQuest::insim_mci()
{

}

