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

void RCQuest::readconfig(const char *Track)
{
    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCQuest\\%s.txt",RootDir,Track);
}


// обработчик события когда на сервер заходит новый пользователь
void RCQuest::insim_ncn( struct IS_NCN *packet )
{
	if( packet->UCID == 0 )
	return;

	strcpy(players[ packet->UCID ].UName, packet->UName);
	strcpy(players[ packet->UCID ].PName, packet->PName);
}

void RCQuest::insim_npl( struct IS_NPL *packet )
{
	PLIDtoUCID[packet->PLID] = packet->UCID;
}

void RCQuest::insim_plp( struct IS_PLP *packet )
{
	PLIDtoUCID.erase( packet->PLID );
}

void RCQuest::insim_pll( struct IS_PLL *packet )
{
	PLIDtoUCID.erase( packet->PLID );
}

void RCQuest::insim_cnl( struct IS_CNL *packet )
{
	players.erase( packet->UCID );
}

void RCQuest::insim_cpr( struct IS_CPR *packet )
{
	strcpy( players[ packet->UCID ].PName, packet->PName );
}

void RCQuest::insim_mso( struct IS_MSO *packet )
{
	byte UCID = packet->UCID;

	if ( UCID == 0 )
        return;

	if ( packet->UserType != MSO_PREFIX )
        return;

	char Msg[128];
    strcpy( Msg, packet->Msg + ((unsigned char)packet->TextStart));
}

void RCQuest::insim_mci(struct IS_MCI *packet)
{
	for (int i = 0; i < packet->NumC; i++)
    {
		byte UCID = PLIDtoUCID[ packet->Info[i].PLID ];
	}
}
