using namespace std;

#include "RCBank.h"

RCBank::RCBank()
{
	//players = new BankPlayer[MAX_PLAYERS];
}

RCBank::~RCBank()
{
    mysql_close( &rcbankDB );
    //delete[] players;
}

byte RCBank::GetPlayerUCID (int i)
{
    if (players[i].UCID != 0)
        return players[i].UCID;

    return 0;
}

int RCBank::GetCash(byte UCID)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            return (int)players[i].Cash;
        }
    }
    return 0;
}

bool    RCBank::AddCash(byte UCID, int Cash)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
        	char Text[96];
        	sprintf(Text,"^5| ^7^C�� ��� ���� ���������: %d ^3RUR.",Cash);
        	send_mtc(UCID, Text);
            players[i].Cash += Cash;
            return true;
        }
    }
    return false;
}

bool    RCBank::RemCash(byte UCID, int Cash)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
        	char Text[96];
        	sprintf(Text,"^5| ^7^C� ������ ����� �������: %d ^3RUR.",Cash);
        	send_mtc(UCID, Text);
            players[i].Cash -= Cash;
            return true;
        }
    }
    return false;
}

bool RCBank::AddToBank(int Cash)
{
    BankFond += Cash;
    return true;
}

bool RCBank::RemFrBank(int Cash)
{
    BankFond -= Cash;
    return true;
}

int RCBank::init(const char *dir,void *CInSim, void *GetMessage, void *dbconn)
{

    strcpy(RootDir,dir);

    insim = (CInsim *)CInSim;
    if(!insim)
    {
        cout << "RCBank Error: Can't struct CInsim class" << endl;;
        return -1;
    }

    msg = (RCMessage *)GetMessage;
    if(!msg)
    {
        cout << "RCBank Error: Can't struct RCMessage class" << endl;
        return -1;
    }

    if(!mysql_init( &rcbankDB))
    {
        printf("RCBank Error: can't create MySQL-descriptor\n");
        return -1;
    }

    mysql_options( &rcbankDB , MYSQL_OPT_RECONNECT, "true" ); // ��������� ���������������

    mysqlConf conf;
	char path[MAX_PATH];
	sprintf(path,"%smisc\\mysql.cfg",RootDir);
	tools::read_mysql(path, &conf);

    while( mysql_real_connect( &rcbankDB , conf.host , conf.user , conf.password , conf.database , conf.port , NULL, 0) == false )
    {
        printf("RCBank Error: can't connect to MySQL server\n");
        Sleep(60000);
    }
	printf("RCBank Success: Connected to MySQL server\n");
    return 0;
}


void RCBank::insim_ncn( struct IS_NCN* packet )
{
    int i;

    if (packet->UCID == 0)
        return;

    for (i=0; i<MAX_PLAYERS; i++)
	{
		 if ( players[i].UCID == 0 )
            break;
	}

    if (i == MAX_PLAYERS)
        return;

    strcpy(players[i].UName, packet->UName);
    strcpy(players[i].PName, packet->PName);
    players[i].UCID = packet->UCID;

    char query[128];
    sprintf(query,"SELECT cash FROM bank WHERE username='%s' LIMIT 1;",packet->UName);

    if( mysql_ping( &rcbankDB ) != 0 )
    {
        printf("Error: connection with MySQL server was lost\n");
        // ���������� ��� ������������ �����
    }

    if( mysql_query( &rcbankDB , query) != 0 )
    {
        printf("Error: MySQL Query\n");
        // ���������� ��� ������������ �����
    }

    rcbankRes = mysql_store_result( &rcbankDB );
    if(rcbankRes == NULL)
        printf("Error: can't get the result description\n");

    if(mysql_num_rows( rcbankRes ) > 0)
    {
        // � ����� ���������� ��� ������
        // �������������� �������
        rcbankRow = mysql_fetch_row( rcbankRes );
        // ������� ��������� � ����������� �����
        players[i].Cash = atof( rcbankRow[0] );

    }
    else
    {
        printf("Can't find %s\n Create user\n",packet->UName);

        sprintf(query,"INSERT INTO bank (username) VALUES ('%s');",packet->UName);

        if( mysql_ping( &rcbankDB ) != 0 )
        {
            printf("Error: connection with MySQL server was lost\n");
            // ���������� ��� ������������ �����
        }

        if( mysql_query( &rcbankDB , query) != 0 )
        {
            printf("Error: MySQL Query\n");
            // ���������� ��� ������������ �����
        }

        players[i].Cash = 1000;
        bank_save(players[i].UCID);

    }

    mysql_free_result( rcbankRes );

    return ;
}



void RCBank::insim_npl( struct IS_NPL* packet )
{
    //cout << "joining race or leaving pits" << endl;
    int i;



    // Find player using UCID and update his PLID
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == packet->UCID)
        {
            players[i].PLID = packet->PLID;
            //strcpy(players[i].CName ,packet->CName);
            if (players[i].Cash > 5000000)
                players[i].Cash = 5000000;
        }
    }
}

void RCBank::insim_plp( struct IS_PLP* packet)
{
    //cout << "player leaves race" << endl;
    int i;



    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == packet->PLID)
        {
            players[i].PLID = 0;
            break;
        }
    }
}

void RCBank::insim_pll( struct IS_PLL* packet )
{
    //cout << "player leaves race" << endl;
    int i;



    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == packet->PLID)
        {
            players[i].PLID = 0;
            break;
        }
    }
}

void RCBank::insim_cnl( struct IS_CNL* packet )
{
    int i;



    // Find player and set the whole player struct he was using to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == packet->UCID)
        {
            if (packet->Reason != LEAVR_DISCO)
            {
                //players[i].cash += 500;
            }
            bank_save(players[i].UCID);
            memset(&players[i],0,sizeof(struct BankPlayer));
            break;
        }
    }
}

void RCBank::bank_save (byte UCID)
{
    // Find player and set the whole player struct he was using to 0

    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {

            char query[128];
            sprintf(query,"UPDATE bank SET cash = %f WHERE username='%s'" , players[i].Cash, players[i].UName);

            if( mysql_ping( &rcbankDB ) != 0 )
                printf("Bank Error: connection with MySQL server was lost\n");

            if( mysql_query( &rcbankDB , query) != 0 )
                printf("Bank Error: MySQL Query Save\n");

            printf("Bank Log: Affected rows = %d\n", mysql_affected_rows( &rcbankDB ) );
            /* Capital */
            sprintf(query,"UPDATE bank SET cash = %f WHERE username='_RC_Bank_Capital_'" , BankFond);

            if( mysql_ping( &rcbankDB ) != 0 )
                printf("Bank Error: connection with MySQL server was lost\n");

            if( mysql_query( &rcbankDB , query) != 0 )
                printf("Bank Error: MySQL Query Save\n");

            printf("Bank Log: Affected rows = %d\n", mysql_affected_rows( &rcbankDB ) );

        }
    }



}

void RCBank::insim_cpr( struct IS_CPR* packet )
{
    int i;



    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == packet->UCID)
        {

            strcpy(players[i].PName, packet->PName);
            break;
        }
    }
}

void RCBank::insim_mci()
{
	struct IS_MCI *pack_mci = (struct IS_MCI*)insim->udp_get_packet();

	for (int i = 0; i < pack_mci->NumC; i++)
    {
        for (int j =0; j < MAX_PLAYERS; j++)        {

            if (pack_mci->Info[i].PLID == players[j].PLID and players[j].PLID != 0 and players[j].UCID != 0)
            {
            	btn_cash(j);
            }
        }
    }

}

void RCBank::btn_cash (int i)
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = players[i].UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.BStyle = 32;
    //
    pack.ClickID = 162;
    pack.L = 70;
    pack.T = 1;
    pack.W = 15;
    pack.H = 4;

    char cash[10];
    sprintf(cash,"%d",(int)players[i].Cash);
    if (players[i].Cash > 0)
        strcpy(pack.Text,"^2");
    else
        strcpy(pack.Text,"^1");
    strcat(pack.Text,cash);
    strcat(pack.Text,"^7 RUR.");
    insim->send_packet(&pack);

    if (strcmp(players[i].UName,"denis-takumi") == 0)
    {
        pack.ClickID = 163;
        pack.L = 50;
        pack.T = 1;
        pack.W = 20;
        pack.H = 4;

        char cash[10];
        sprintf(cash,"%.0f",BankFond);
        //itoa((int)BankFond,cash,10);
        if (BankFond > 0)
            strcpy(pack.Text,"^2");
        else
            strcpy(pack.Text,"^1");
        strcat(pack.Text,cash);
        strcat(pack.Text,"^7 RUR.");
        insim->send_packet(&pack);
    }
}

void RCBank::readconfig(const char *Track)
{
    char query[128];
    sprintf(query,"SELECT cash FROM bank WHERE username='_RC_Bank_Capital_' LIMIT 1;");

    if( mysql_ping( &rcbankDB ) != 0 )
    {
        printf("Error: connection with MySQL server was lost\n");
    }

    if( mysql_query( &rcbankDB , query) != 0 )
    {
        printf("Error: MySQL Query\n");
    }

    rcbankRes = mysql_store_result( &rcbankDB );
    if(rcbankRes == NULL)
        printf("Error: can't get the result description\n");

    if(mysql_num_rows( rcbankRes ) > 0)
    {
        rcbankRow = mysql_fetch_row( rcbankRes );
        BankFond = atof( rcbankRow[0] );
    }
    else
    {
        BankFond = 0;
    }

	mysql_free_result( rcbankRes );
}


// �������-�������

int RCBank::check_pos(struct BankPlayer *splayer)
{
    int PLX = splayer->Info.X/65536;
    int PLY = splayer->Info.Y/65536;
    if (Check_Pos(4,zone.dealX,zone.dealY,PLX,PLY))
        return 1;

    return 0;
}
