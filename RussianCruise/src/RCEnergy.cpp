using namespace std;
#include "RCEnergy.h"

RCEnergy::RCEnergy(const char* Dir)
{
    strcpy(RootDir,Dir);
}

RCEnergy::~RCEnergy()
{

}

int RCEnergy::init(MYSQL *conn, CInsim *InSim, void *Message, void *Bank)
{
    dbconn = conn;
    if (!dbconn)
    {
        printf("^3RCEnergy:\t^1Can't sctruct MySQL Connector\n");
        return -1;
    }

    insim = InSim;
    if (!insim)
    {
        printf ("^3RCEnergy:\t^1Can't struct CInsim class");
        return -1;
    }

    msg = (RCMessage *)Message;
    if (!msg)
    {
        printf ("^3RCEnergy:\t^1Can't struct RCMessage class");
        return -1;
    }

    bank = (RCBank *)Bank;
    if (!msg)
    {
        printf ("^3RCEnergy:\t^1Can't struct RCBank class");
        return -1;
    }

    if( mysql_ping(dbconn) != 0)
    {
        printf("^3RCEnergy:\t^1Connection with MySQL server was lost\n");
        return -1;
    }
    CCText("^3RCEnergy:\t^2Connected to MySQL server");
    return 0;
}


void RCEnergy::readconfig(const char *Track)
{
    char file[255];
    sprintf(file, "%s\\data\\RCEnergy\\maps\\%s.txt", RootDir, Track );

    FILE *fff = fopen(file, "r");
    if (fff == nullptr)
        return;

    fclose(fff);
    ifstream readf (file, ios::in);
    while (readf.good())
    {
        char str[128];
        readf.getline(str, 128);

        if (strstr(str, "//"))
            continue;

        if (strlen(str) > 0)
        {
            if (strncmp(str, "/cafe", 5) == 0)
            {
                readf.getline(str, 128);
                int count = atoi(str);
                TrackInf.CafeCount = count;

                for (int i=0 ; i < count; i++)
                {
                    readf.getline(str, 128);
                    char * X;
                    char * Y;
                    X = strtok (str, ";");
                    Y = strtok (NULL, ";");
                    TrackInf.XCafe[i] = atoi(X);
                    TrackInf.YCafe[i] = atoi(Y);
                }
            }
        }
    }
    readf.close();

    CCText("  ^7RCEnergy\t^2OK");
}

void RCEnergy::InsimNCN( struct IS_NCN* packet )
{
    strcpy(players[packet->UCID].UName, packet->UName);
    strcpy(players[packet->UCID].PName, packet->PName);
    players[packet->UCID].Zone = 1;

    char query[128];
    sprintf(query, "SELECT energy FROM energy WHERE username='%s' LIMIT 1;", packet->UName);

    if ( mysql_ping( dbconn ) != 0 )
    {
        printf("Error: connection with MySQL server was lost\n");
    }

    if ( mysql_query( dbconn , query) != 0 )
    {
        printf("Error: MySQL Query\n");
    }

    dbres = mysql_store_result( dbconn );

    if (dbres == NULL)
    {
        printf("Error: can't get the result description\n");
    }

    if (mysql_num_rows( dbres ) > 0)
    {
        dbrow = mysql_fetch_row( dbres );
        players[ packet->UCID ].Energy = atof( dbrow[0] );
    }
    else
    {
        printf("Can't find %s\n Create user\n", packet->UName);

        sprintf(query, "INSERT INTO energy (username) VALUES ('%s');", packet->UName);

        if ( mysql_ping( dbconn ) != 0 )
        {
            printf("Error: connection with MySQL server was lost\n");
        }

        if ( mysql_query( dbconn , query) != 0 )
        {
            printf("Error: MySQL Query\n");
        }

        players[ packet->UCID ].Energy = 10000;
        energy_save( packet->UCID );
    }
    mysql_free_result( dbres );
    btn_energy( packet->UCID );
}

void RCEnergy::InsimNPL( struct IS_NPL* packet )
{
    PLIDtoUCID[packet->PLID] = packet->UCID;

    if ( players[packet->UCID].Energy < 500 )
    {
        SendMTC( packet->UCID, msg->_(  packet->UCID, "2402" ) );
        SendMTC( packet->UCID, msg->_(  packet->UCID, "2403" ) );

        char Text[64];
        sprintf(Text, "/spec %s", players[ packet->UCID ].UName);
        SendMST(Text);
        players[ packet->UCID ].Zone = 1;
        return;
    }
    players[packet->UCID].EnergyTime = time(&nrgtime);
}

void RCEnergy::InsimPLP( struct IS_PLP* packet)
{
    players[PLIDtoUCID[packet->PLID]].Zone = 1;
    //PLIDtoUCID.erase(packet->PLID);
}

void RCEnergy::InsimPLL( struct IS_PLL* packet )
{
    players[PLIDtoUCID[packet->PLID]].Zone = 1;
    PLIDtoUCID.erase(packet->PLID);
}

void RCEnergy::InsimCNL( struct IS_CNL* packet )
{
    energy_save(packet->UCID);
    players.erase(packet->UCID);
}

void RCEnergy::energy_save (byte UCID)
{
    char query[128];
    sprintf(query, "UPDATE energy SET energy = %d WHERE username='%s'" , players[UCID].Energy, players[UCID].UName);

    if ( mysql_ping( dbconn ) != 0 )
        printf("Bank Error: connection with MySQL server was lost\n");

    if ( mysql_query( dbconn , query) != 0 )
        printf("Bank Error: MySQL Query Save\n");
}

void RCEnergy::InsimCPR( struct IS_CPR* packet )
{
    strcpy(players[packet->UCID].PName, packet->PName);
}

void RCEnergy::InsimMCI ( struct IS_MCI* pack_mci )
{
    for (int i = 0; i < pack_mci->NumC; i++)
    {
        byte UCID = PLIDtoUCID[pack_mci->Info[i].PLID];
        if (UCID == 0)
        {
            return;
        }

        int X = pack_mci->Info[i].X / 65536;
        int Y = pack_mci->Info[i].Y / 65536;
        int Z = pack_mci->Info[i].Z / 65536;
        int D = pack_mci->Info[i].Direction / 182;
        int H = pack_mci->Info[i].Heading / 182;

        if (Check_Pos(TrackInf.CafeCount, TrackInf.XCafe, TrackInf.YCafe, X, Y))
        {
            players[UCID].Zone = 3;
        }
        else if ( players[UCID].Energy < 10 )
        {
            players[UCID].Zone = 1;
            char Text[64];
            sprintf(Text, "/spec %s", players[UCID].UName);
            SendMST(Text);
        }
        else
        {
            players[UCID].Zone = 0;
        }


        int S = ((int)pack_mci->Info[i].Speed * 360) / (32768);

        int A = pack_mci->Info[i].AngVel * 360 / 16384;

        int X1 = players[UCID].Info.X / 65536;
        int Y1 = players[UCID].Info.Y / 65536;
        int Z1 = players[UCID].Info.Z / 65536;
        int D1 = players[UCID].Info.Direction / 182;
        int H1 = players[UCID].Info.Heading / 182;
        int S1 = ((int)players[UCID].Info.Speed * 360) / (32768);
        int A1 = players[UCID].Info.AngVel * 360 / 16384;

        long dA = A - A1;
        long dS = S - S1;
        long dD = abs((int)(sin(D) * 100)) - abs((int)(sin(D1) * 100));
        long dH = abs((int)(sin(H) * 100)) - abs((int)(sin(H1) * 100));

        int K = (int)sqrt(abs((dD - dH) * (1+dA) * dS)) / 8;

        if (players[UCID].Energy > 5 and S > 5)
        {
            if (!Islocked(UCID))
            {
                players[UCID].Energy -= K;
            }
        }

        if (S == 0)
        {
            int time_i = time(&nrgtime) - players[UCID].EnergyTime;
            if (time_i > 59)
            {
                if (players[UCID].Zone == 3)
                {
                    players[UCID].Energy += 400;
                }
                else
                {
                    players[UCID].Energy += 200;
                }

                players[UCID].EnergyTime = time(&nrgtime);
            }
        }

        if (players[UCID].Energy > 10000 )
        {
            players[UCID].Energy = 10000;
        }
        else if (players[UCID].Energy < 0 )
        {
            players[UCID].Energy = 0;
        }

        if (X1 == 0 and Y1 == 0 and Z1 == 0)
        {
            X1=X;
            Y1=Y;
            Z1=Z;
        }

        btn_energy(UCID);
        memcpy( &players[UCID].Info , &pack_mci->Info[i] , sizeof(struct CompCar) );
    }
}


void RCEnergy::InsimMSO( struct IS_MSO* packet )
{
    if (packet->UCID == 0)
    {
        return;
    }

    if (strncmp(packet->Msg + ((unsigned char)packet->TextStart), "!coffee", 7) == 0)
    {
        if ( players[ packet->UCID ].Energy > 9900 )
        {
            SendMTC(packet->UCID, msg->_( packet->UCID, "EnergyFull" ));
            return;
        }

        if (bank->GetCash( packet->UCID ) < 50)
        {
            SendMTC(packet->UCID, msg->_( packet->UCID, "2001" ));
            return;
        }

        if ( !( players[ packet->UCID ].Zone == 1 and players[ packet->UCID ].Energy < 500 ) and ( players[ packet->UCID ].Zone != 3 ) )
        {
            SendMTC(packet->UCID, msg->_( packet->UCID, "2002" ));
            return;
        }

        players[ packet->UCID ].Energy += 500;
        bank->RemCash(packet->UCID, 50);
        bank->AddToBank(50);
    }

    //!redbule
    if (strncmp(packet->Msg + ((unsigned char)packet->TextStart), "!redbull", 8) == 0)
    {
        if ( players[ packet->UCID ].Energy > 9900 )
        {
            SendMTC(packet->UCID, msg->_( packet->UCID, "EnergyFull" ));
            return;
        }

        if (bank->GetCash( packet->UCID ) < 100)
        {
            SendMTC(packet->UCID, msg->_( packet->UCID, "2001" ));
            return;
        }

        if ( !(players[packet->UCID].Zone == 1 and players[packet->UCID].Energy < 500) and (players[packet->UCID].Zone != 3 ) )
        {
            SendMTC(packet->UCID, msg->_( packet->UCID, "2002" ));
            return;
        }
        players[ packet->UCID ].Energy += 1000;
        bank->RemCash(packet->UCID, 100);
        bank->AddToBank(100);
    }

    if (strncmp(packet->Msg + ((unsigned char)packet->TextStart), "!save", 5) == 0 )
    {
        energy_save(packet->UCID);
    }
}

void RCEnergy::btn_energy ( byte UCID )
{
    char str[64];
    int nrg = players[UCID].Energy / 100;
    int color = 2;

    if (nrg <= 10)
    {
        if ( players[UCID].EnergyAlarm )
        {
            players[UCID].EnergyAlarm = false;
            color = 0;
        }
        else
        {
            players[UCID].EnergyAlarm = true;
            color = 1;
        }
    }
    else if (nrg <= 50)
        color = 3;

    sprintf(str, "^%d %s: %d^K£¥", color, msg->_(UCID , "Energy"), nrg);

    if (players[UCID].Zone == 3)
        sprintf(str, "%s ^K¡è", str);

    SendButton(255, UCID, 208, 100, 1, 30, 4, 32+64, str);
}

int RCEnergy::check_pos( byte UCID )
{
    int PLX = players[UCID].Info.X / 65536;
    int PLY = players[UCID].Info.Y / 65536;

    if (Check_Pos(4, zone.dealX, zone.dealY, PLX, PLY))
    {
        return 1;
    }

    return 0;
}


int RCEnergy::GetEnergy(byte UCID)
{
    return (players[UCID].Energy * 100 / 10000);
}

void RCEnergy::InsimCON( struct IS_CON* packet )
{
    byte UCIDA = PLIDtoUCID[ packet->A.PLID ];
    byte UCIDB = PLIDtoUCID[ packet->B.PLID ];

    if ( !Islocked( UCIDA ) )
    {
        players[ UCIDA ].Energy -= 10 * packet->SpClose;
    }

    if ( !Islocked( UCIDB ) )
    {
        players[ UCIDB ].Energy -= 10 * packet->SpClose;
    }
}

void RCEnergy::InsimOBH( struct IS_OBH* packet )
{
    byte UCID = PLIDtoUCID[packet->PLID];
    if ((packet->Index > 45 and packet->Index < 125 and packet->Index != 120 and packet->Index != 121) or (packet->Index > 140))
    {
        time_t now = time(NULL);
        if ((now - players[UCID].LastT) < 1)
        {
            return;
        }
        players[UCID].LastT = now;

        if (!Islocked( UCID ))
        {
            players[UCID].Energy -=  packet->SpClose;
        }
    }
}

void RCEnergy::InsimHLV( struct IS_HLV* packet )
{
    byte UCID = PLIDtoUCID[packet->PLID];
    if (packet->HLVC == 1)
    {
        time_t now = time(NULL);
        if ((now - players[UCID].LastT) < 1)
        {
            return;
        }
        players[UCID].LastT = now;

        if (!Islocked( UCID ))
        {
            players[UCID].Energy -=  5 * packet->C.Speed;
        }
    }
}

bool RCEnergy::Lock(byte UCID)
{
    players[UCID].Lock = 1;
    return true;
}

bool RCEnergy::Unlock(byte UCID)
{
    players[UCID].Lock = 0;
    return true;
}

bool RCEnergy::Islocked(byte UCID)
{
    if (players[UCID].Lock == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool RCEnergy::AddEnergy( byte UCID, int Energy)
{
    players[UCID].Energy += Energy;
    return true;
}

bool RCEnergy::RemoveEnergy( byte UCID, int Energy)
{
    players[UCID].Energy -= Energy;
    return true;
}
