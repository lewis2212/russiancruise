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

bool RCQuest::ReadConfig(const char *Track)
{
    char file[MAX_PATH];
    sprintf(file, "%s/data/RCQuest/%s.txt", RootDir, Track);

    CCText("  ^7RCQuest\t^2OK");
    return true;
}


// обработчик события когда на сервер заходит новый пользователь
bool RCQuest::InsimNCN( struct IS_NCN *packet )
{
    if ( packet->UCID == 0 )
        return true;

    strcpy(players[ packet->UCID ].UName, packet->UName);
    strcpy(players[ packet->UCID ].PName, packet->PName);
    return true;
}

bool RCQuest::InsimNPL( struct IS_NPL *packet )
{
    PLIDtoUCID[packet->PLID] = packet->UCID;
    return true;
}

bool RCQuest::InsimPLP( struct IS_PLP *packet )
{
    PLIDtoUCID.erase( packet->PLID );
    return true;
}

bool RCQuest::InsimPLL( struct IS_PLL *packet )
{
    PLIDtoUCID.erase( packet->PLID );
    return true;
}

bool RCQuest::InsimCNL( struct IS_CNL *packet )
{
    players.erase( packet->UCID );
    return true;
}

bool RCQuest::InsimCPR( struct IS_CPR *packet )
{
    strcpy( players[ packet->UCID ].PName, packet->PName );
    return true;
}

bool RCQuest::InsimMSO( struct IS_MSO *packet )
{
    byte UCID = packet->UCID;

    if ( UCID == 0 )
        return true;

    if ( packet->UserType != MSO_PREFIX )
        return true;

    char Msg[128];
    strcpy( Msg, packet->Msg + ((unsigned char)packet->TextStart));
}

bool RCQuest::InsimMCI(struct IS_MCI *packet)
{
    for (int i = 0; i < packet->NumC; i++)
    {
        //byte UCID = PLIDtoUCID[ packet->Info[i].PLID ];
    }
    return true;
}
