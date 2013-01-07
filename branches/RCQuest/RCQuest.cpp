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
    struct IS_NPL *pack_npl = (struct IS_NPL*)insim->get_packet();
    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_npl->UCID)
        {
            players[i].PLID = pack_npl->PLID;
            break;
        }
    }
}
void RCQuest::insim_plp( struct IS_PLP *packet )
{
    struct IS_PLP *pack_plp = (struct IS_PLP*)insim->get_packet();
    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == pack_plp->PLID)
        {
            players[i].PLID = 0;
            break;
        }
    }
}

void RCQuest::insim_pll( struct IS_PLL *packet )
{
    struct IS_PLL *pack_pll = (struct IS_PLL*)insim->get_packet();
    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == pack_pll->PLID)
        {
            players[i].PLID = 0;
            break;
        }
    }
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
    struct IS_CPR *pack_cpr = (struct IS_CPR*)insim->get_packet();
    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_cpr->UCID)
        {
            strcpy(players[i].PName, pack_cpr->PName);
            break;
        }
    }
}
void RCQuest::insim_mso( struct IS_MSO *packet )
{
    int i;
    struct IS_MSO *pack_mso = (struct IS_MSO*)insim->get_packet();
    if (pack_mso->UCID == 0)
        return;
    for (i=0; i < MAX_PLAYERS; i++)
        if (players[i].UCID == pack_mso->UCID)
            break;
}
void RCQuest::insim_mci(struct IS_MCI *packet)
{

    struct IS_MCI *pack_mci = (struct IS_MCI*)insim->udp_get_packet();

    for (int i = 0; i < packet->NumC; i++)
        for (int j =0; j < MAX_PLAYERS; j++)
		break;
}
