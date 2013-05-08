using namespace std;
#include "RCEnergy.h"

RCEnergy::RCEnergy()
{
	//players = new EnergyPlayer[MAX_PLAYERS];
	//memset(players, 0, sizeof( EnergyPlayer ) * MAX_PLAYERS );
}

RCEnergy::~RCEnergy()
{
	mysql_close( &rcNrgDB );
	//delete[] players;
}

int RCEnergy::init(const char *dir,void *CInSim, void *Message,void *Bank)
{
    strcpy(RootDir,dir);

    insim = (CInsim *)CInSim;
    if(!insim)
    {
        printf ("Can't struct CInsim class");
        return -1;
    }

    msg = (RCMessage *)Message;
    if(!msg)
    {
        printf ("Can't struct RCMessage class");
        return -1;
    }

    bank = (RCBank *)Bank;
    if(!msg)
    {
        printf ("Can't struct RCBank class");
        return -1;
    }

	if(!mysql_init( &rcNrgDB))
    {
        printf("RCEnergy Error: can't create MySQL-descriptor\n");
        return -1;
    }

    mysql_options( &rcNrgDB , MYSQL_OPT_RECONNECT, "true" ); // разрешаем переподключение

    mysqlConf conf;
	char path[MAX_PATH];
	sprintf(path,"%smisc\\mysql.cfg",RootDir);
	tools::read_mysql(path, &conf);

    while( mysql_real_connect( &rcNrgDB , conf.host , conf.user , conf.password , conf.database , conf.port , NULL, 0) == false )
    {
        printf("RCEnergy Error: can't connect to MySQL server\n");
        Sleep(60000);
    }
	printf("RCEnergy Success: Connected to MySQL server\n");

    return 0;
}


void RCEnergy::readconfig(const char *Track)
{
    char file[255];
    sprintf(file,"%sdata\\RCEnergy\\maps\\%s.txt", RootDir, Track );

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
        return;

    FindClose(fff);

    ifstream readf (file,ios::in);


    while (readf.good())
    {
        char str[128];
        readf.getline(str,128);

        if (strstr(str,"//")) {}
        if (strlen(str) > 0)
        {
            if (strncmp(str,"/cafe",5)==0)
            {
                readf.getline(str,128);
                int count = atoi(str);
                TrackInf.CafeCount = count;

                for (int i=0 ; i<count; i++)
                {
                    readf.getline(str,128);
                    char * X;
                    char * Y;
                    X = strtok (str,";");
                    Y = strtok (NULL,";");
                    TrackInf.XCafe[i] = atoi(X);
                    TrackInf.YCafe[i] = atoi(Y);
                }
            }

        } // if strlen > 0
    } //while readf.good()

    readf.close();
}



void RCEnergy::insim_ncn( struct IS_NCN* packet )
{

    if (packet->UCID == 0)
        return;

    // Copy all the player data we need into the players[] array
    strcpy(players[ packet->UCID ].UName, packet->UName);
    strcpy(players[ packet->UCID ].PName, packet->PName);
	players[  packet->UCID ].Zone = 1;

	char query[128];
    sprintf(query,"SELECT energy FROM energy WHERE username='%s' LIMIT 1;",packet->UName);

    if( mysql_ping( &rcNrgDB ) != 0 )
    {
        printf("Error: connection with MySQL server was lost\n");
    }

    if( mysql_query( &rcNrgDB , query) != 0 )
    {
        printf("Error: MySQL Query\n");
    }

    rcNrgRes = mysql_store_result( &rcNrgDB );

    if(rcNrgRes == NULL)
        printf("Error: can't get the result description\n");

    if(mysql_num_rows( rcNrgRes ) > 0)
    {
        rcNrgRow = mysql_fetch_row( rcNrgRes );
        players[ packet->UCID ].Energy = atof( rcNrgRow[0] );
    }
    else
    {
        printf("Can't find %s\n Create user\n",packet->UName);

        sprintf(query,"INSERT INTO energy (username) VALUES ('%s');",packet->UName);

        if( mysql_ping( &rcNrgDB ) != 0 )
        {
            printf("Error: connection with MySQL server was lost\n");
        }

        if( mysql_query( &rcNrgDB , query) != 0 )
        {
            printf("Error: MySQL Query\n");
        }

        players[ packet->UCID ].Energy = 10000;
        energy_save( packet->UCID );

    }

    mysql_free_result( rcNrgRes );

    btn_energy( packet->UCID );

}

void RCEnergy::insim_npl( struct IS_NPL* packet )
{
	PLIDtoUCID[ packet->PLID ] = packet->UCID;

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

void RCEnergy::insim_plp( struct IS_PLP* packet)
{
	PLIDtoUCID.erase( packet->PLID );
}

void RCEnergy::insim_pll( struct IS_PLL* packet )
{
	PLIDtoUCID.erase( packet->PLID );
}

void RCEnergy::insim_cnl( struct IS_CNL* packet )
{
	energy_save( packet->UCID );
	players.erase(  packet->UCID );
}

void RCEnergy::energy_save (byte UCID)
{
	char query[128];
	sprintf(query,"UPDATE energy SET energy = %d WHERE username='%s'" , players[ UCID ].Energy, players[ UCID ].UName);

	if( mysql_ping( &rcNrgDB ) != 0 )
		printf("Bank Error: connection with MySQL server was lost\n");

	if( mysql_query( &rcNrgDB , query) != 0 )
		printf("Bank Error: MySQL Query Save\n");

	printf("Bank Log: Affected rows = %d\n", mysql_affected_rows( &rcNrgDB ) );

}

void RCEnergy::insim_cpr( struct IS_CPR* packet )
{
	strcpy(players[ packet->UCID ].PName, packet->PName);
}

void RCEnergy::insim_mci ( struct IS_MCI* pack_mci )
{

    for (int i = 0; i < pack_mci->NumC; i++)
    {
		byte UCID = PLIDtoUCID[ pack_mci->Info[i].PLID ];
		if( UCID == 0)
			return;

		int X = pack_mci->Info[i].X/65536;
		int Y = pack_mci->Info[i].Y/65536;
		int Z = pack_mci->Info[i].Z/65536;
		int D = pack_mci->Info[i].Direction/182;
		int H = pack_mci->Info[i].Heading/182;

		int S = ((int)pack_mci->Info[i].Speed*360)/(32768);

		int A = pack_mci->Info[i].AngVel*360/16384;

		int X1 = players[ UCID ].Info.X/65536;
		int Y1 = players[ UCID ].Info.Y/65536;
		int Z1 = players[ UCID ].Info.Z/65536;
		int D1 = players[ UCID ].Info.Direction/182;
		int H1 = players[ UCID ].Info.Heading/182;
		int S1 = ((int)players[ UCID ].Info.Speed*360)/(32768);
		int A1 = players[ UCID ].Info.AngVel*360/16384;


		long dA = A-A1;
		long dS = S-S1;
		long dD = abs((int)(sin(D)*100))-abs((int)(sin(D1)*100));
		long dH = abs((int)(sin(H)*100))-abs((int)(sin(H1)*100));

		int K = (int)sqrt(abs((dD-dH)*(1+dA)*dS))/8;


		if ((players[ UCID ].Energy > 5) and (S > 5))
		{
			if (!Islocked( UCID ))
				players[ UCID ].Energy -= K;
		}

		if (Check_Pos(TrackInf.CafeCount,TrackInf.XCafe,TrackInf.YCafe,X,Y))
			players[ UCID ].Zone = 3;
		else if ( players[ UCID ].Energy < 10 )
		{
			players[ UCID ].Zone = 1;
			char Text[64];
			sprintf(Text, "/spec %s", players[ UCID ].UName);
			SendMST(Text);
		}
		else
			players[ UCID ].Zone = 0;

		if (S == 0)
		{
			int time_i = time(&nrgtime) - players[ UCID ].EnergyTime;

			if (time_i > 59)
			{
				if (players[ UCID ].Zone == 3)
					players[ UCID ].Energy += 400;
				else
					players[ UCID ].Energy += 200;

				players[ UCID ].EnergyTime = time(&nrgtime);
			}

		}

		if (players[ UCID ].Energy > 10000 )
		{
			players[ UCID ].Energy = 10000;
		}
		else if (players[ UCID ].Energy < 0 )
		{
			players[ UCID ].Energy = 0;
		}

		if (X1==0 and Y1==0 and Z1==0)
		{
			X1=X;
			Y1=Y;
			Z1=Z;
		}

		memcpy( &players[ UCID ].Info , &pack_mci->Info[i] , sizeof(struct CompCar) );

		btn_energy( UCID );
    }
}


void RCEnergy::insim_mso( struct IS_MSO* packet )
{

    if (packet->UCID == 0)
        return;

    if (strncmp(packet->Msg + ((unsigned char)packet->TextStart), "!coffee", 7) == 0)
    {
        if (((players[ packet->UCID ].Zone == 1) and (players[ packet->UCID ].Energy < 500)) or (players[ packet->UCID ].Zone == 3))
        {
            if (bank->GetCash(packet->UCID) > 50)
            {
                players[ packet->UCID ].Energy += 500;
                bank->RemCash(packet->UCID,50);
                bank->AddToBank(50);
            }
            else
            {
                SendMTC(packet->UCID,msg->_( packet->UCID, "2001" ));
            }
        }
        else
        {
            SendMTC(packet->UCID,msg->_( packet->UCID, "2002" ));
        }
    }

    //!redbule
    if (strncmp(packet->Msg + ((unsigned char)packet->TextStart), "!redbull", 8) == 0)
    {
        if (((players[ packet->UCID ].Zone == 1) and (players[ packet->UCID ].Energy < 500)) or (players[ packet->UCID ].Zone == 3))
        {
            if (bank->GetCash(packet->UCID) > 100)
            {
                players[ packet->UCID ].Energy += 1000;
                bank->RemCash(packet->UCID,100);
                bank->AddToBank(100);
            }
            else
            {
                SendMTC(packet->UCID,msg->_( packet->UCID, "2001" ));
            }
        }
        else
        {
            SendMTC(packet->UCID,msg->_( packet->UCID, "2002" ));
        }

    }

    if (strncmp(packet->Msg + ((unsigned char)packet->TextStart), "!save", 5) == 0 )
        energy_save(packet->UCID);

}

void RCEnergy::btn_energy ( byte UCID )
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.ClickID = 207;
    pack.BStyle = 32+64;
    pack.L = 100;
    pack.T = 1;
    pack.W = 30;
    pack.H = 4;

    if (players[ UCID ].Zone == 3)
		strcpy(pack.Text,"^2");
	else
		strcpy(pack.Text,"^1");

    strcat(pack.Text,msg->_(UCID , "Energy"));

    int nrg = players[ UCID ].Energy/50 ;

    if (nrg <= 40)
        strcat(pack.Text,"^1");
    else if (nrg <= 140 and nrg > 40)
        strcat(pack.Text,"^3");
    else
        strcat(pack.Text,"^2");

    float nrg2 = (players[ UCID ].Energy)*100/10000;

    sprintf(pack.Text,"%s %0.0f^KЈҐ",pack.Text,nrg2);
    insim->send_packet(&pack);
}

int RCEnergy::check_pos( byte UCID )
{
    int PLX = players[ UCID ].Info.X/65536;
    int PLY = players[ UCID ].Info.Y/65536;
    if (Check_Pos(4,zone.dealX,zone.dealY,PLX,PLY))
    {
        return 1;
    }

    return 0;
}


int RCEnergy::GetEnergy(byte UCID)
{
	return (players[ UCID ].Energy*100/10000);
}


void RCEnergy::insim_con( struct IS_CON* packet )
{
	byte UCIDA = PLIDtoUCID[ packet->A.PLID ];
	byte UCIDB = PLIDtoUCID[ packet->B.PLID ];

	if ( !Islocked( UCIDA ) )
		players[ UCIDA ].Energy -= 10 * packet->SpClose;

	if ( !Islocked( UCIDB ) )
		players[ UCIDB ].Energy -= 10 * packet->SpClose;
}

void RCEnergy::insim_obh( struct IS_OBH* packet )
{
	byte UCID = PLIDtoUCID[ packet->PLID ];
    if((packet->Index > 45 and packet->Index < 125) or (packet->Index > 140))
    {
		if ( !Islocked( UCID ) )
		players[ UCID ].Energy -=  packet->SpClose;
    }
}



bool RCEnergy::Lock(byte UCID)
{
	players[ UCID ].Lock = 1;
	return true;
}

bool RCEnergy::Unlock(byte UCID)
{
	players[ UCID ].Lock = 0;
	return true;
}

bool RCEnergy::Islocked(byte UCID)
{
	if (players[ UCID ].Lock == 1)
		return true;
	else
		return false;
}

bool RCEnergy::AddEnergy( byte UCID, int Energy)
{
	players[ UCID ].Energy += Energy;
	return true;
}

bool RCEnergy::RemoveEnergy( byte UCID, int Energy)
{
	players[ UCID ].Energy -= Energy;
	return true;
}
