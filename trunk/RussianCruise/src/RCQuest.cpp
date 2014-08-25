#include "RCQuest.h"

RCQuest::RCQuest(const char* Dir)
{
    ClassName = "RCQuest";
    strcpy(RootDir,Dir);
}

RCQuest::~RCQuest()
{

}

void RCQuest::init(MYSQL *conn, CInsim *InSim)
{
    dbconn = conn;
    if (!dbconn)
    {
        printf("RCDL: Can't sctruct MySQL Connector\n");
        return ;
    }

    insim = InSim;
    if (!insim)
    {
        printf ("Can't struct CInsim class");
        return ;
    }

    CCText("^3R"+ClassName+":\t^2inited");
}

void RCQuest::ReadConfig(const char *Track)
{
    char file[MAX_PATH];
    sprintf(file, "%s/data/RCQuest/%s.txt", RootDir, Track);

    CCText("  ^7RCQuest\t^2OK");
}


// обработчик события когда на сервер заходит новый пользователь
void RCQuest::InsimNCN( struct IS_NCN *packet )
{
    if ( packet->UCID == 0 )
        return;

    strcpy(players[ packet->UCID ].UName, packet->UName);
    strcpy(players[ packet->UCID ].PName, packet->PName);
}

void RCQuest::InsimNPL( struct IS_NPL *packet )
{
    PLIDtoUCID[packet->PLID] = packet->UCID;
}

void RCQuest::InsimPLP( struct IS_PLP *packet )
{
    PLIDtoUCID.erase( packet->PLID );
}

void RCQuest::InsimPLL( struct IS_PLL *packet )
{
    PLIDtoUCID.erase( packet->PLID );
}

void RCQuest::InsimCNL( struct IS_CNL *packet )
{
    players.erase( packet->UCID );
}

void RCQuest::InsimCPR( struct IS_CPR *packet )
{
    strcpy( players[ packet->UCID ].PName, packet->PName );
}

void RCQuest::InsimMSO( struct IS_MSO *packet )
{
    byte UCID = packet->UCID;

    if ( UCID == 0 )
        return;

    if ( packet->UserType != MSO_PREFIX )
        return;

    char Msg[128];
    strcpy( Msg, packet->Msg + ((unsigned char)packet->TextStart));
}

void RCQuest::InsimMCI(struct IS_MCI *packet)
{
    for (int i = 0; i < packet->NumC; i++)
    {
        //byte UCID = PLIDtoUCID[ packet->Info[i].PLID ];
    }
}
