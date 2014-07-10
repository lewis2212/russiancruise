using namespace std;

#include "RCAntCheat.h"



RCAntCheat::RCAntCheat(const char* Dir)
{
    ClassName = "RCAntCheat";
    strcpy(RootDir,Dir);
}

RCAntCheat::~RCAntCheat()
{
    //delete[] players;
}

int RCAntCheat::init(MYSQL *conn, CInsim *InSim, void *Message)
{
    dbconn = conn;
    if (!dbconn)
    {
        printf("RCAntCheat: Can't sctruct MySQL Connector\n");
        return -1;
    } // Копируем путь до программы


    insim = InSim; // Присваиваем указателю область памяти
    if (!insim) // Проверяем на существование
    {
        printf ("Can't struct CInsim class");
        return -1;
    }

    msg = (RCMessage *)Message;
    if (!msg)
    {
        printf ("Can't struct RCMessage class");
        return -1;
    }

    CCText("^3R"+ClassName+":\t^2inited");
    return 0;
}

void RCAntCheat::InsimCNL( struct IS_CNL* packet )
{
    players.erase( packet->UCID );
}

void RCAntCheat::InsimCPR( struct IS_CPR* packet )
{
    strcpy(players[ packet->UCID ].PName, packet->PName);
}

void RCAntCheat::InsimMCI ( struct IS_MCI* pack_mci )
{
    return;
}

void RCAntCheat::InsimMSO( struct IS_MSO* packet )
{
    if (packet->UCID == 0)
    {
        return;
    }
}


void RCAntCheat::InsimNCN( struct IS_NCN* packet )
{

    if (packet->UCID == 0)
    {
        return;
    }

    strcpy(players[ packet->UCID ].UName, packet->UName);
    strcpy(players[ packet->UCID ].PName, packet->PName);
}

void RCAntCheat::InsimNPL( struct IS_NPL* packet )
{
    PLIDtoUCID[ packet->PLID ] = packet->UCID;
    players[ packet->UCID ].NPL = 1;
    strcpy(players[ packet->UCID ].CName, packet->CName);

    /*****   Hack Detect ***/
    char Text[64];
    time_t timeh;
    int htime = time(&timeh); // get current time

    if (players[ packet->UCID ].NPLTime == 0)
    {
        players[ packet->UCID ].NPLTime = htime;
    }

    int ts = htime - players[ packet->UCID ].HackTime;
    players[ packet->UCID ].HackTime = htime;


    if (ts < 10)  // 10 sec
    {
        players[ packet->UCID ].NPLHack++;
    }
    else
    {
        players[ packet->UCID ].NPLHack = 1;
    }

    if (players[ packet->UCID ].NPLHack > 4)   //max lines to tolerate
    {
        players[ packet->UCID ].NPLHack = 0;
        sprintf(Text, "/kick %s", players[ packet->UCID ].UName);
        insim->SendMTC( packet->UCID , "^1Hack detect");
        insim->SendMST(Text);
        return;

    }

    if ( strlen( packet->CName ) < 3)
    {
        sprintf(Text, "/spec %s", players[ packet->UCID ].UName);
        insim->SendMST("/msg ^1Hack detect");
        insim->SendMST(Text);
        return;
    }

    players[ packet->UCID ].ReadyForMCI = true;
}

void RCAntCheat::InsimPLP( struct IS_PLP* packet)
{
    byte UCID = PLIDtoUCID[ packet->PLID ];
    PLIDtoUCID.erase( packet->PLID );
    players[ UCID ].NPL = 0;
    memset(&players[ UCID ].Info, 0, sizeof(CompCar));
    players[ UCID ].InPit = true;
    players[ UCID ].ReadyForMCI = false;
}

void RCAntCheat::InsimPLL( struct IS_PLL* packet )
{
    byte UCID = PLIDtoUCID[ packet->PLID ];
    PLIDtoUCID.erase( packet->PLID );
    players[ UCID ].NPL = 0;
    memset(&players[ UCID ].Info, 0, sizeof(CompCar));
    players[ UCID ].InPit = true;
    players[ UCID ].ReadyForMCI = false;
}

void RCAntCheat::InsimPLA( struct IS_PLA* packet )
{
    insim->SendTiny(TINY_REO,255);
}

/*void RCAntCheat::InsimREO( struct IS_REO* packet )
{


    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID != 0)
        {
            int j = 0;
            while (j < packet->NumP)
            {
                if (players[i].PLID == packet->PLID[j] )
                    break;

                ++j;
            }

            if (j == packet->NumP)
            {
                char Text[48];
                sprintf(Text, "/spec %s", players[i].UName);
                SendMST(Text);
                sprintf(Text, "/msg %s ^1have wrong PLID", players[i].UName);
                SendMST(Text);
            }
        }
    }
}*/


void RCAntCheat::pitlane (const char* UName)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    sprintf(pack_mst.Msg, "/pitlane %s", UName);
    insim->send_packet(&pack_mst);
}
