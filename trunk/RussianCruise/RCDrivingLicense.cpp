using namespace std;

#include "RCDrivingLicense.h"

RCDL::RCDL()
{

}

RCDL::~RCDL()
{

}

int     RCDL::GetLVL(byte UCID)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            return players[i].LVL;
        }
    }
    return 0;
}
int     RCDL::GetSkill(byte UCID)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            return players[i].Skill;
        }
    }
    return 0;
}

bool    RCDL::AddSkill(byte UCID)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            if (!Islocked(UCID))
            {
                players[i].Skill += 250*players[i].LVL;

                char Text[64];
                float nextlvl = (pow(players[i].LVL,2)*0.5+100)*1000;
                float skl = (players[i].LVL*250/nextlvl)*100;
                sprintf(Text,"^2 + ^3%2.3f%% ^7Skill",skl);
                send_mtc(players[i].UCID,Text);

                return true;
            }
        }
    }
    return false;
}

bool    RCDL::RemSkill(byte UCID)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            if (!Islocked(UCID))
            {
                if (players[i].Skill < 500*players[i].LVL)
                    players[i].Skill = 0;
                else
                    players[i].Skill -= 500*players[i].LVL;

                char Text[64];
                float nextlvl = (pow(players[i].LVL,2)*0.5+100)*1000;
                float skl = (players[i].LVL*500/nextlvl)*100;
                sprintf(Text,"^1 - ^3%2.3f%% ^7Skill",skl);
                send_mtc(players[i].UCID,Text);

                return true;
            }
        }
    }
    return false;
}

bool    RCDL::Lock(byte UCID)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            players[i].Lock = 1;
            return true;
        }
    }
    return false;
}

bool    RCDL::Unlock(byte UCID)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            players[i].Lock = 0;
            return true;
        }
    }
    return false;
}

bool    RCDL::Islocked(byte UCID)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            if (players[i].Lock == 1)
                return true;
            else
                return false;
        }
    }
    return false;
}

int RCDL::init(const char *dir,void *CInSim, void *GetMessage)
{
    strcpy(RootDir,dir);

    insim = (CInsim *)CInSim;
    if(!insim)
    {
        printf ("Can't struct CInsim class");
        return -1;
    }

    msg = (RCMessage *)GetMessage;
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
        Sleep(60000);
    }
	printf("RCDL Success: Connected to MySQL server\n");

    return 0;
}

void RCDL::insim_ncn()
{
    int i;

    struct IS_NCN *pack_ncn = (struct IS_NCN*)insim->get_packet();

    if (pack_ncn->UCID == 0)
        return;

    for (i=0; i<MAX_PLAYERS; i++)
        if (players[i].UCID == 0)
            break;

    if (i == MAX_PLAYERS)
        return;

    // Copy all the player data we need into the players[] array
    strcpy(players[i].UName, pack_ncn->UName);
    strcpy(players[i].PName, pack_ncn->PName);
    players[i].UCID = pack_ncn->UCID;

	char query[128];
    sprintf(query,"SELECT lvl,skill FROM dl WHERE username='%s' LIMIT 1;",pack_ncn->UName);

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
        players[i].LVL = atof( rcDLRow[0] );
        players[i].Skill = atof( rcDLRow[1] );
    }
    else
    {
        printf("Can't find %s\n Create user\n",pack_ncn->UName);

        sprintf(query,"INSERT INTO dl (username) VALUES ('%s');",pack_ncn->UName);

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

        players[i].LVL = 0;
        save(players[i].UCID);

    }

    mysql_free_result( rcDLRes );
}

void RCDL::insim_npl()
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

void RCDL::insim_plp()
{

    struct IS_PLP *pack_plp = (struct IS_PLP*)insim->get_packet();

    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == pack_plp->PLID)
        {
            players[i].PLID = 0;
            memset(&players[i].Info,0,sizeof(CompCar));
            break;
        }
    }
}

void RCDL::insim_pll()
{
    struct IS_PLL *pack_pll = (struct IS_PLL*)insim->get_packet();

    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == pack_pll->PLID)
        {
            players[i].PLID = 0;
            memset(&players[i].Info,0,sizeof(CompCar));
            break;
        }
    }
}

void RCDL::insim_cnl()
{
    int i;

    struct IS_CNL *pack_cnl = (struct IS_CNL*)insim->get_packet();

    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_cnl->UCID)
        {
            save(players[i].UCID);
            memset(&players[i],0,sizeof(struct DLPlayer));
            break;
        }
    }
}

void RCDL::insim_crp()
{
    int i;

    struct IS_CPR *pack_cpr = (struct IS_CPR*)insim->get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_cpr->UCID)
        {
            strcpy(players[i].PName, pack_cpr->PName);
            break;
        }
    }
}

void RCDL::insim_mso()
{
    int i;

    struct IS_MSO *pack_mso = (struct IS_MSO*)insim->get_packet();

    // The chat GetMessage is sent by the host, don't do anything
    if (pack_mso->UCID == 0)
        return;

    // Find the player that wrote in the chat
    for (i=0; i < MAX_PLAYERS; i++)
        if (players[i].UCID == pack_mso->UCID)
            break;

    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!save", 5) == 0 )
        save(players[i].UCID);

}

void RCDL::save (byte UCID)
{
    // Find player and set the whole player struct he was using to 0

    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
        	char query[128];
            sprintf(query,"UPDATE dl SET lvl = %d, skill = %d WHERE username='%s'" , players[i].LVL , players[i].Skill, players[i].UName);

            if( mysql_ping( &rcDLDB ) != 0 )
                printf("Bank Error: connection with MySQL server was lost\n");

            if( mysql_query( &rcDLDB , query) != 0 )
                printf("Bank Error: MySQL Query Save\n");

            printf("Bank Log: Affected rows = %d\n", mysql_affected_rows( &rcDLDB ) );

            break;
        }
    }

}

void RCDL::insim_con()
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


void RCDL::insim_mci()
{
    //cout << "pizza_mci" << endl;
    struct IS_MCI *pack_mci = (struct IS_MCI*)insim->udp_get_packet();


    for (int i = 0; i < pack_mci->NumC; i++)

    {
        for (int j =0; j < MAX_PLAYERS; j++)
        {

            if (pack_mci->Info[i].PLID == players[j].PLID and players[j].PLID != 0 and players[j].UCID != 0)

            {
                int X = pack_mci->Info[i].X/65536;
                int Y = pack_mci->Info[i].Y/65536;

                int S = ((int)pack_mci->Info[i].Speed*360)/(32768);

                int X1 = players[j].Info.X/65536;
                int Y1 = players[j].Info.Y/65536;

                if (X1==0 and Y1==0)
                {
                    X1=X;
                    Y1=Y;
                    memcpy(&players[j].Info,&pack_mci->Info[i],sizeof(CompCar));
                }


                float Skill = Distance(X,Y,X1,Y1);

                if ((abs((int)Skill) > 10) and (S>30))
                {
                    players[j].Skill += abs((int)(Skill*1.5f));
                    memcpy(&players[j].Info,&pack_mci->Info[i],sizeof(CompCar));
                }

                /** next lvl **/

                float nextlvl = (pow(players[j].LVL,2)*0.5+100)*1000;

                if (players[j].Skill > nextlvl)
                {
                    players[j].LVL ++;
                    players[j].Skill = 0;
                    char Msg[64];
                    sprintf(Msg,"/msg ^5| ^8^C%s ^1Get new Level = ^3%d",players[j].PName, players[j].LVL);
                    send_mst(Msg);
                }

                /** buttons **/

                if (players[j].mcicount > 10)
                {
                    btn_dl(&players[j]);
                    players[j].mcicount = 0;
                }
                else
                    players[j].mcicount += 1;

            }
        }
    }

}

void RCDL::btn_dl(struct DLPlayer *splayer)
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.ClickID = 230;
    pack.BStyle = 32+64;
    pack.L = 100;
    pack.T = 5;
    pack.W = 35;
    pack.H = 4;

    float nextlvl = (pow(splayer->LVL,2)*0.5+100)*1000;
    float skl = (splayer->Skill/nextlvl)*100;
    sprintf(pack.Text,"^7^CDrive Level: ^2%d ^7Skill: ^2%4.2f%%", splayer->LVL,skl);
    insim->send_packet(&pack);

    char Text[64];
    sprintf(Text,"^C^4| %s ^7перешел на %d уровень",splayer->PName, splayer->LVL);

}
