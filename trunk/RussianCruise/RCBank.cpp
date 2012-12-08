using namespace std;

#include "RCBank.h"

time_t  btime;
pthread_t bid; // Thread ID

void *bankthread (void *arg)
{
    RCBank *bnk = (RCBank *)arg; //struct our RCBank class in thread
    if(!bnk)
    {
        printf ("Can't start bankthread");
        return 0;
    }

    cout << "\tthread \"Bank\" started" << endl;

    for (;;)
    {
        for (int i=0; i<MAX_PLAYERS; i++)
        {
            if (bnk->GetPlayerUCID(i) != 0)
                bnk->btn_cash(i);
        }
        Sleep(2000);
    }
}


RCBank::RCBank()
{

}

RCBank::~RCBank()
{
    mysql_close(&cruisedb);
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

int RCBank::init(const char *dir,void *CInSim, void *GetMessage,void *Bank)
{

    pthread_cancel(bid);
    if (pthread_create(&bid,NULL,bankthread,Bank) < 0)
        return -1;

    strcpy(RootDir,dir);

    insim = (CInsim *)CInSim;
    if(!insim)
    {
        cout << "Can't struct CInsim class" << endl;;
        return -1;
    }

    msg = (RCMessage *)GetMessage;
    if(!msg)
    {
        cout << "Can't struct RCMessage class" << endl;
        return -1;
    }

    if(!mysql_init(&cruisedb))
    {
        printf("Error: can't create MySQL-descriptor\n");
        return -1;
    }

    mysql_options( &cruisedb , MYSQL_OPT_RECONNECT, "true" ); // разрешаем переподключение

    if(!mysql_real_connect( &cruisedb , "localhost", "cruise", "cruise", "cruise", 3310, NULL, 0))
    {
        printf("Error: can't connect to MySQL server\n");
        return -1;
    }
    else
    {
        printf("Success: connected to MySQL server\n");
    }

    return 0;
}


void RCBank::insim_ncn()
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

    strcpy(players[i].UName, pack_ncn->UName);
    strcpy(players[i].PName, pack_ncn->PName);
    players[i].UCID = pack_ncn->UCID;

    char query[128];
    sprintf(query,"SELECT cash FROM bank WHERE username='%s' LIMIT 1;",pack_ncn->UName);

    if( mysql_ping( &cruisedb ) != 0 )
    {
        printf("Error: connection with MySQL server was lost\n");
        // произвести кик пользователя чтоль
    }

    if( mysql_query( &cruisedb , query) != 0 )
    {
        printf("Error: MySQL Query\n");
        // произвести кик пользователя чтоль
    }

    res = mysql_store_result(&cruisedb);
    if(res == NULL)
        printf("Error: can't get the result description\n");

    if(mysql_num_rows(res) > 0)
    {
        // В цикле перебираем все записи
        // результирующей таблицы
        row = mysql_fetch_row(res);
        // Выводим результат в стандартный поток
        players[i].Cash = atof( row[0] );

    }
    else
    {
        printf("Can't find %s\n Create user\n",pack_ncn->UName);

        sprintf(query,"INSERT INTO bank (username) VALUES ('%s');",pack_ncn->UName);

        if( mysql_ping( &cruisedb ) != 0 )
        {
            printf("Error: connection with MySQL server was lost\n");
            // произвести кик пользователя чтоль
        }

        if( mysql_query( &cruisedb , query) != 0 )
        {
            printf("Error: MySQL Query\n");
            // произвести кик пользователя чтоль
        }

        players[i].Cash = 1000;
        bank_save(players[i].UCID);

    }

    return ;
}



void RCBank::insim_npl()
{
    //cout << "joining race or leaving pits" << endl;
    int i;

    struct IS_NPL *pack_npl = (struct IS_NPL*)insim->get_packet();

    // Find player using UCID and update his PLID
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_npl->UCID)
        {
            players[i].PLID = pack_npl->PLID;
            //strcpy(players[i].CName ,pack_npl->CName);
            if (players[i].Cash > 5000000)
                players[i].Cash = 5000000;
        }
    }
}

void RCBank::insim_plp()
{
    //cout << "player leaves race" << endl;
    int i;

    struct IS_PLP *pack_plp = (struct IS_PLP*)insim->get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == pack_plp->PLID)
        {
            players[i].PLID = 0;
            break;
        }
    }
}

void RCBank::insim_pll()
{
    //cout << "player leaves race" << endl;
    int i;

    struct IS_PLL *pack_pll = (struct IS_PLL*)insim->get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == pack_pll->PLID)
        {
            players[i].PLID = 0;
            break;
        }
    }
}

void RCBank::insim_cnl ()
{
    int i;

    struct IS_CNL *pack_cnl = (struct IS_CNL*)insim->get_packet();

    // Find player and set the whole player struct he was using to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_cnl->UCID)
        {
            if (pack_cnl->Reason != LEAVR_DISCO)
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
            /* User */
            sprintf(query,"UPDATE bank SET cash = %f WHERE username='%s'" , players[i].Cash, players[i].UName);

            if( mysql_ping( &cruisedb ) != 0 )
                printf("Bank Error: connection with MySQL server was lost\n");

            if( mysql_query( &cruisedb , query) != 0 )
                printf("Bank Error: MySQL Query Save\n");

            printf("Bank Log: Affected rows = %d\n", mysql_affected_rows(&cruisedb) );
            /* Capital */
            sprintf(query,"UPDATE bank SET cash = %f WHERE username='_RC_Bank_Capital_'" , BankFond);

            if( mysql_ping( &cruisedb ) != 0 )
                printf("Bank Error: connection with MySQL server was lost\n");

            if( mysql_query( &cruisedb , query) != 0 )
                printf("Bank Error: MySQL Query Save\n");

            printf("Bank Log: Affected rows = %d\n", mysql_affected_rows(&cruisedb) );

        }
    }



}

void RCBank::insim_crp()
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

    if( mysql_ping( &cruisedb ) != 0 )
    {
        printf("Error: connection with MySQL server was lost\n");
    }

    if( mysql_query( &cruisedb , query) != 0 )
    {
        printf("Error: MySQL Query\n");
    }

    res = mysql_store_result(&cruisedb);
    if(res == NULL)
        printf("Error: can't get the result description\n");

    if(mysql_num_rows(res) > 0)
    {
        row = mysql_fetch_row(res);
        BankFond = atof( row[0] );
    }
    else
    {
        BankFond = 0;
    }
}


// функции-утилиты

int RCBank::check_pos(struct BankPlayer *splayer)
{
    int PLX = splayer->Info.X/65536;
    int PLY = splayer->Info.Y/65536;

    if (Check_Pos(4,zone.dealX,zone.dealY,PLX,PLY))
        return 1;

    return 0;
}
