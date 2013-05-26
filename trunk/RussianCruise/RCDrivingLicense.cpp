using namespace std;

#include "RCDrivingLicense.h"

RCDL::RCDL()
{

}

RCDL::~RCDL()
{

}

int RCDL::GetLVL(byte UCID)
{
	return players[ UCID ].LVL;
}

int RCDL::GetSkill(byte UCID)
{
	return players[ UCID ].Skill;
}

bool RCDL::AddSkill(byte UCID)
{
	if ( !Islocked( UCID ) )
	{
		players[ UCID ].Skill += 250*players[ UCID ].LVL;

		char Text[64];
		float nextlvl = ( pow( players[ UCID ].LVL, 2 ) * 0.5 + 100 ) * 1000;
		float skl = ( players[ UCID ].LVL * 250 / nextlvl ) * 100;
		sprintf( Text, "^2 + ^3%2.3f%% ^7Skill", skl);
		SendMTC( UCID, Text );
		return true;
	}
    return false;
}

bool RCDL::AddSkill(byte UCID, float coef)
{
	if (coef < 0)
		return false;

	if ( !Islocked( UCID ) )
	{
		players[ UCID ].Skill += (250 * players[ UCID ].LVL ) * coef;

		char Text[64];
		float nextlvl = ( pow( players[ UCID ].LVL, 2 ) * 0.5 + 100 ) * 1000;
		float skl = ( players[ UCID ].LVL * 250 / nextlvl ) * 100 * coef;
		sprintf( Text, "^2 + ^3%2.3f%% ^7Skill", skl);
		SendMTC( UCID, Text );
		return true;
	}
    return false;
}

bool RCDL::RemSkill(byte UCID)
{
	if ( !Islocked( UCID ) )
	{
		if ( players[ UCID ].Skill < 500 * players[ UCID ].LVL ){

			if( players[ UCID ].LVL > 0 ){
				players[ UCID ].Skill = ( ( pow( players[ UCID ].LVL - 1 , 2 ) * 0.5 + 100 ) * 1000 ) - ( 500 * players[ UCID ].LVL - players[ UCID ].Skill );
				players[ UCID ].LVL --;
			}
			else
				players[ UCID ].Skill = 0;

		}
		else
			players[ UCID ].Skill -= 500 * players[ UCID ].LVL;

		char Text[64];
		float nextlvl = ( pow( players[ UCID ].LVL , 2 ) * 0.5 + 100 ) * 1000;
		float skl = ( players[ UCID ].LVL * 500 / nextlvl ) * 100;
		sprintf(Text,"^1 - ^3%2.3f%% ^7Skill", skl );
		SendMTC( UCID, Text );
		return true;
	}
    return false;
}

bool RCDL::RemSkill(byte UCID, float coef)
{
	if (coef < 0)
		return false;

	if ( !Islocked( UCID ) )
	{
		if ( players[ UCID ].Skill < 500 * players[ UCID ].LVL * coef ){

			if( players[ UCID ].LVL > 0 ){
				players[ UCID ].Skill = ( ( pow( players[ UCID ].LVL - 1 , 2 ) * 0.5 + 100 ) * 1000 ) - ( 500 * players[ UCID ].LVL * coef - players[ UCID ].Skill );
				players[ UCID ].LVL --;
			}
			else
				players[ UCID ].Skill = 0;
		}
		else
			players[ UCID ].Skill -= 500 * players[ UCID ].LVL * coef;

		char Text[64];
		float nextlvl = ( pow( players[ UCID ].LVL , 2 ) * 0.5 + 100 ) * 1000;
		float skl = ( players[ UCID ].LVL * 500 / nextlvl ) * 100;
		sprintf(Text,"^1 - ^3%2.3f%% ^7Skill", skl * coef );
		SendMTC( UCID ,Text);

		return true;
	}

    return false;
}

bool RCDL::Lock(byte UCID)
{
	players[ UCID ].Lock = 1;
	return true;
}

bool RCDL::Unlock(byte UCID)
{
	players[ UCID ].Lock = 0;
	return true;
}

bool RCDL::Islocked(byte UCID)
{
	if (players[ UCID ].Lock == 1)
		return true;
	else
		return false;
}

int RCDL::init(const char *dir,void *CInSim, void *RCMessageClass)
{
    strcpy(RootDir,dir);

    insim = (CInsim *)CInSim;
    if(!insim)
    {
        printf ("Can't struct CInsim class");
        return -1;
    }

    msg = (RCMessage *)RCMessageClass;
    if(!msg)
    {
        printf ("Can't struct RCMessage class");
        return -1;
    }

    inited = 1;

    if(!mysql_init( &rcDLDB))
    {
        printf("RCDL Error: can't create MySQL-descriptor\n");
        return -1;
    }

    mysql_options( &rcDLDB , MYSQL_OPT_RECONNECT, "true" ); // разрешаем переподключение

    mysqlConf conf;
	char path[MAX_PATH];
	sprintf(path,"%smisc\\mysql.cfg",RootDir);
	tools::read_mysql(path, &conf);

    while( mysql_real_connect( &rcDLDB , conf.host , conf.user , conf.password , conf.database , conf.port , NULL, 0) == false )
    {
        printf("RCDL Error: can't connect to MySQL server\n");
		#ifdef __linux__
        sleep(60000);
        #else
        Sleep(60000);
        #endif
    }
	printf("RCDL Success: Connected to MySQL server\n");

    return 0;
}

void RCDL::InsimNCN( struct IS_NCN* packet )
{
    if (packet->UCID == 0)
        return;

    // Copy all the player data we need into the players[] array
    strcpy(players[ packet->UCID ].UName, packet->UName);
    strcpy(players[ packet->UCID ].PName, packet->PName);

	char query[128];
    sprintf(query,"SELECT lvl,skill FROM dl WHERE username='%s' LIMIT 1;",packet->UName);

    if( mysql_ping( &rcDLDB ) != 0 )
    {
        printf("Error: connection with MySQL server was lost\n");
        // произвести кик пользователя чтоль
    }

    if( mysql_query( &rcDLDB , query) != 0 )
    {
        printf("Error: MySQL Query\n");
        // произвести кик пользователя чтоль
    }

    rcDLRes = mysql_store_result( &rcDLDB );
    if(rcDLRes == NULL)
        printf("Error: can't get the result description\n");

    if(mysql_num_rows( rcDLRes ) > 0)
    {
        rcDLRow = mysql_fetch_row( rcDLRes );
        players[ packet->UCID ].LVL = atof( rcDLRow[0] );
        players[ packet->UCID ].Skill = atof( rcDLRow[1] );
    }
    else
    {
        printf("Can't find %s\n Create user\n",packet->UName);

        sprintf(query,"INSERT INTO dl (username) VALUES ('%s');",packet->UName);

        if( mysql_ping( &rcDLDB ) != 0 )
        {
            printf("Error: connection with MySQL server was lost\n");
            // произвести кик пользователя чтоль
        }

        if( mysql_query( &rcDLDB , query) != 0 )
        {
            printf("Error: MySQL Query\n");
            // произвести кик пользователя чтоль
        }

        players[ packet->UCID ].LVL = 0;
        save( packet->UCID );

    }

    mysql_free_result( rcDLRes );

    btn_dl( packet->UCID );
}

void RCDL::InsimNPL( struct IS_NPL* packet )
{
	PLIDtoUCID[ packet->PLID ] = packet->UCID;
}

void RCDL::InsimPLP( struct IS_PLP* packet)
{
	memset(&players[PLIDtoUCID[ packet->PLID ]  ].Info, 0, sizeof( CompCar ) );
	PLIDtoUCID.erase( packet->PLID );
}

void RCDL::InsimPLL( struct IS_PLL* packet )
{
	memset(&players[PLIDtoUCID[ packet->PLID ]  ].Info, 0, sizeof( CompCar ) );
	PLIDtoUCID.erase( packet->PLID );
}

void RCDL::InsimCNL( struct IS_CNL* packet )
{
	save( packet->UCID );
	players.erase( packet->UCID );
}

void RCDL::InsimCPR( struct IS_CPR* packet )
{
	strcpy(players[ packet->UCID ].PName, packet->PName);
}

void RCDL::InsimMSO( struct IS_MSO* packet )
{

    if (strncmp(packet->Msg + ((unsigned char)packet->TextStart), "!save", 5) == 0 )
        save( packet->UCID );

}

void RCDL::save (byte UCID)
{
	char query[128];
	sprintf(query,"UPDATE dl SET lvl = %d, skill = %d WHERE username='%s'" , players[ UCID ].LVL , players[ UCID ].Skill, players[ UCID ].UName);

	if( mysql_ping( &rcDLDB ) != 0 )
		printf("Bank Error: connection with MySQL server was lost\n");

	if( mysql_query( &rcDLDB , query) != 0 )
		printf("Bank Error: MySQL Query Save\n");

	printf("Bank Log: Affected rows = %d\n", mysql_affected_rows( &rcDLDB ) );

}

void RCDL::InsimCON( struct IS_CON* packet )
{
    struct IS_CON *pack_con = (struct IS_CON*)insim->get_packet();
    /*
    	for (int i=0; i<MAX_PLAYERS; i++)
    	{
    		if (players[i].PLID == pack_con->A.PLID)
    		{
    			if (!Islocked(players[i].UCID))
    				RemSkill(players[i].UCID);
    			break;
    		}
    	}

    	for (int i=0; i<MAX_PLAYERS; i++)
    	{
    		if (players[i].PLID == pack_con->B.PLID)
    		{
    			if (!Islocked(players[i].UCID))
    				RemSkill(players[i].UCID);
    			break;
    		}
    	}

    */

}


void RCDL::InsimMCI( struct IS_MCI* pack_mci )
{
    for (int i = 0; i < pack_mci->NumC; i++)
    {
    	byte UCID = PLIDtoUCID[ pack_mci->Info[i].PLID ];

		int X = pack_mci->Info[i].X/65536;
		int Y = pack_mci->Info[i].Y/65536;
		int S = ((int)pack_mci->Info[i].Speed*360)/(32768);
		int X1 = players[ UCID ].Info.X/65536;
		int Y1 = players[ UCID ].Info.Y/65536;

		if (X1==0 and Y1==0)
		{
			X1=X;
			Y1=Y;
			memcpy(&players[ UCID ].Info,&pack_mci->Info[i],sizeof(CompCar));
		}

		float Skill = Distance(X,Y,X1,Y1);

		if ((abs((int)Skill) > 10) and (S>30))
		{
			players[ UCID ].Skill += abs((int)(Skill*1.5f));
			memcpy(&players[ UCID ].Info,&pack_mci->Info[i],sizeof(CompCar));
		}

		/** next lvl **/

		float nextlvl = (pow(players[ UCID ].LVL,2)*0.5+100)*1000;

		if (players[ UCID ].Skill > nextlvl)
		{
			players[ UCID ].LVL ++;
			players[ UCID ].Skill = 0;
			char Msg[64];
			sprintf(Msg,"/msg ^5| ^8^C%s ^1get ^3%d ^1lvl",players[ UCID ].PName, players[ UCID ].LVL);
			if( UCID !=0 )
				SendMST(Msg);
		}

		/** buttons **/
		btn_dl( UCID );
    }
}

void RCDL::btn_dl( byte UCID )
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.ClickID = 230;
    pack.BStyle = 32+64;
    pack.L = 100;
    pack.T = 5;
    pack.W = 30;
    pack.H = 4;

    float nextlvl = ( pow( players[ UCID ].LVL, 2) * 0.5 + 100 ) * 1000;
    float skl = ( players[ UCID ].Skill / nextlvl ) * 100;

    sprintf( pack.Text,msg->_(UCID, "LvlAndSkill"),players[ UCID ].LVL,skl);
    insim->send_packet(&pack);
}
