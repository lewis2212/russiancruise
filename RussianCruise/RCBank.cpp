using namespace std;

#include "RCBank.h"

RCBank::RCBank()
{
	players = new BankPlayer[MAX_PLAYERS];
	memset(players, 0, sizeof( BankPlayer ) * MAX_PLAYERS );
}

RCBank::~RCBank()
{
    mysql_close( &rcbankDB );
    delete[] players;
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

bool    RCBank::AddCash(byte UCID, int Cash, bool Show = false)
{
    for (int i = 0; i< MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
        	char Text[96];
        	sprintf(Text,"^5| ^7^CНа ваш счет поступило: %d ^3RUR.",Cash);
        	if( Show )
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
        	sprintf(Text,"^5| ^7^CС вашего счета списано: %d ^3RUR.",Cash);
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

bool RCBank::InBank(byte UCID)
{
    for (int i = 0; i < MAX_PLAYERS; ++i)
    if ( players[i].UCID == UCID)
    return players[i].InZone;
}

int RCBank::init(const char *dir,void *CInSim, void *GetMessage, void *DL)
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

    dl = (RCDL *)DL;
    if(!dl)
    {
        printf ("RCBank Error: Can't struct RCDL class");
        return -1;
    }

    if(!mysql_init( &rcbankDB))
    {
        printf("RCBank Error: can't create MySQL-descriptor\n");
        return -1;
    }

    mysql_options( &rcbankDB , MYSQL_OPT_RECONNECT, "true" ); // разрешаем переподключение

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
        if ( players[i].UCID == 0 )
        	break;

    if (i == MAX_PLAYERS)
		return;

    strcpy(players[i].UName, packet->UName);
    strcpy(players[i].PName, packet->PName);
    players[i].UCID = packet->UCID;

    char query[128];
    sprintf(query,"SELECT cash FROM bank WHERE username='%s' LIMIT 1;",packet->UName);

    if( mysql_ping( &rcbankDB ) != 0 ){printf("Error: connection with MySQL server was lost\n");}
    if( mysql_query( &rcbankDB , query) != 0 ){printf("Error: MySQL Query\n");}

    rcbankRes = mysql_store_result( &rcbankDB );
    if(rcbankRes == NULL) printf("Error: can't get the result description\n");

    if(mysql_num_rows( rcbankRes ) > 0)
    {
        rcbankRow = mysql_fetch_row( rcbankRes );
        players[i].Cash = atof( rcbankRow[0] );
    }
    else
    {
        printf("Can't find %s\n Create user\n",packet->UName);
        sprintf(query,"INSERT INTO bank (username) VALUES ('%s');",packet->UName);
        if( mysql_ping( &rcbankDB ) != 0 ){printf("Error: connection with MySQL server was lost\n");}
        if( mysql_query( &rcbankDB , query) != 0 ){printf("Error: MySQL Query\n");}

        players[i].Cash = 1000;
        bank_save(players[i].UCID);
    }
    mysql_free_result( rcbankRes );

    /** кредиты **/
    sprintf(query,"SELECT cash, date_create FROM bank_credits WHERE username='%s' LIMIT 1;",packet->UName);

    if( mysql_ping( &rcbankDB ) != 0 ){printf("Error credits: connection with MySQL server was lost\n");}
    if( mysql_query( &rcbankDB , query) != 0 ){printf("Error credits: MySQL Query\n");}

    rcbankRes = mysql_store_result( &rcbankDB );
    if(rcbankRes == NULL) printf("Error credits: can't get the result description\n");

    if(mysql_num_rows( rcbankRes ) > 0)
    {
        rcbankRow = mysql_fetch_row( rcbankRes );
        players[i].Credit = atof( rcbankRow[0] );
        players[i].Date_create = atof( rcbankRow[1] );
        //printf("Check user %s, %d, %d\n",packet->UName,players[i].Credit,players[i].Date_create);
    }
    else
    {
        sprintf(query,"INSERT INTO bank_credits (username,cash,date_create) VALUES ('%s',0,0);",packet->UName);
        if( mysql_ping( &rcbankDB ) != 0 ){printf("Error credits: connection with MySQL server was lost\n");}
        if( mysql_query( &rcbankDB , query) != 0 ){printf("Error credits: MySQL Query\n");}

        players[i].Credit = 0;
        players[i].Date_create = 0;
        //printf("Create user %s, %d, %d\n",packet->UName,players[i].Credit,players[i].Date_create);
    }

    mysql_free_result( rcbankRes );

    /** credit **/
    if (players[i].Date_create!=0)
    {
        char Text[96];
        double razn=((double)(players[i].Date_create)+30*24*3600 - (double)time(NULL))/(24*3600);

        printf("%d",razn);

        if (razn<=4 and razn>0)
        {

            sprintf(Text,"^1| ^1^CОСТАЛОСЬ^7%3.0f ^1ДНЯ ДО СНЯТИЯ КРЕДИТА!",razn);
            send_mtc(players[i].UCID, Text);
        }
        if (razn==0)
        {
            sprintf(Text,"^1| ^1^CСЕГОДНЯ ПОСЛЕДНИЙ ДЕНЬ ДЛЯ ПОГАШЕНИЯ КРЕДИТА!");
            send_mtc(players[i].UCID, Text);
            send_mtc(players[i].UCID, Text);
            send_mtc(players[i].UCID, Text);
        }
        if (razn<0) credit_penalty(players[i].UCID);
    }
    /** credit **/

    return ;
}

void RCBank::credit_penalty (byte UCID)
{
    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {

            char Text[96];

            double razn=((double)(players[i].Date_create)+30*24*3600 - (double)time(NULL))/(24*3600);
            int c = (int)abs((ceil(razn)));
            sprintf(Text,"^1| ^1^CВЫ ПРОСРОЧИЛИ КРЕДИТ!");send_mtc(players[i].UCID, Text);
            sprintf(Text,"^1| ^7^CС вашего счета списан кредит (%d) + штраф за каждый пропущенный день",players[i].Credit*13/10);send_mtc(players[i].UCID, Text);

            RemCash(players[i].UCID,((players[i].Credit)*13/10 + 1000*c)); //отбираем бабки у игрока
            AddToBank((players[i].Credit)*13/10 + 1000*c); //сдаем в банк
            players[i].Credit = 0;
            players[i].Date_create = 0;
            bank_save(players[i].UCID); //сохраняемся

        }
    }
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

            /* Credit */
            sprintf(query,"UPDATE bank_credits SET cash = %d, date_create = %d WHERE username='%s'" , players[i].Credit, players[i].Date_create, players[i].UName);
            if( mysql_ping( &rcbankDB ) != 0 )
                printf("Credit Error: connection with MySQL server was lost\n");
            if( mysql_query( &rcbankDB , query) != 0 )
                printf("Credit Error: MySQL Query Save\n");

            printf("Credit Log: Affected rows = %d\n", mysql_affected_rows( &rcbankDB ) );

            /* Capital */
            sprintf(query,"UPDATE bank SET cash = %f WHERE username='_RC_Bank_Capital_'" , BankFond);

            if( mysql_ping( &rcbankDB ) != 0 )
                printf("Bank Error: connection with MySQL server was lost\n");

            if( mysql_query( &rcbankDB , query) != 0 )
                printf("Bank Error: MySQL Query Save\n");

            printf("Capital Log: Affected rows = %d\n", mysql_affected_rows( &rcbankDB ) );
        }
    }
}

void RCBank::insim_cpr( struct IS_CPR* packet )
{
    int i;
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == packet->UCID)
        {
            strcpy(players[i].PName, packet->PName);break;
        }
    }
}

void RCBank::insim_mso( struct IS_MSO* packet )
{
    int i;

    for (i=0; i < MAX_PLAYERS; i++)
        if (players[i].UCID == packet->UCID) break;

    char Message[96];
    strcpy(Message,packet->Msg + ((unsigned char)packet->TextStart));

    if (players[i].InZone)
    {
        char Text[96];
        //Доступная сумма кредита:
        int cr = dl->GetLVL(players[i].UCID) * 10000; if (cr>500000) cr = 500000;

        if (strncmp(Message, "!info", strlen("!info")) == 0 )
        {

            send_mtc(players[i].UCID,"^5| ^CИнформация");
            if (dl->GetLVL(players[i].UCID)<5) { send_mtc(players[i].UCID,"^1| ^C^7Нужен уровень: ^15"); return;}
            //кредит
            sprintf(Text,"^5| ^7^CВам доступен кредит на сумму от %d ^3RUR ^7до %d ^3RUR^7.",cr/5,cr);send_mtc(players[i].UCID, Text);
            send_mtc(players[i].UCID,"^5| ^C^7Кредит выдается под ^230 % ^7в месяц, на срок не более ^230^7 дней.");
            send_mtc(players[i].UCID,"^5| ^C^7При досрочном погашении будет снята полная сумма кредита + процент.");
            //send_mtc(players[i].UCID,"^6| ^C^7При просрочке более чем на ^21 ^7день будет снята сумма кредита + полный процент + штраф.");
        }

        if (strncmp(Message, "!credit info", strlen("!credit info")) == 0 )
        {
            if (dl->GetLVL(players[i].UCID)<5) { send_mtc(players[i].UCID,"^1| ^C^7Нужен уровень: ^15"); return;}

            if (players[i].Date_create!=0)
            {
                struct tm * timeinfo;
                char DateCreate [80];
                timeinfo = localtime (&players[i].Date_create);
                strftime (DateCreate,80,"%d.%m.%Y",timeinfo);

                double razn=((double)(players[i].Date_create)+30*24*3600 - (double)time(NULL))/(24*3600);

                sprintf(Text,"^5| ^7^CВы имеете кредит на сумму %d ^3RUR^7. Дата выдачи: %s",players[i].Credit,DateCreate); send_mtc(players[i].UCID, Text);
                sprintf(Text,"^5| ^7^CВремя до снятия: ^2%3.0f ^7дней.",razn); send_mtc(players[i].UCID, Text);
                return;
            }

            strtok (Message," "); strtok (NULL," "); int summ = atoi(strtok (NULL," "));
            if (summ<=0) summ=cr; else if (summ<cr/5 or summ>cr) { sprintf(Text,"^1| ^7^CОшибка. ^7Укажите сумму от %d ^3RUR ^7до %d ^3RUR^7.",cr/5,cr); send_mtc(players[i].UCID, Text); return; }

            send_mtc(players[i].UCID,"^5| ^CИнформация по оформляемому кредиту");
            sprintf(Text,"^5| ^7^CЖелаемая сумма: %d ^3RUR^7.",summ);send_mtc(players[i].UCID, Text);
            sprintf(Text,"^5| ^7^CСумма возврата: %d ^3RUR^7.",summ*13/10);send_mtc(players[i].UCID, Text);
            return;
        }

        if (strncmp(Message, "!credit", strlen("!credit")) == 0 )
        {
            if (dl->GetLVL(players[i].UCID)<5){send_mtc(players[i].UCID,"^1| ^C^7Нужен уровень: ^15");return;}
            if (players[i].Date_create!=0){send_mtc(players[i].UCID,"^1| ^C^7У вас уже оформлен кредит (^3!credit info^7)");return;}

            strtok (Message," "); int summ = atoi(strtok (NULL," "));
            if (summ<cr/5 or summ>cr) { sprintf(Text,"^1| ^7^CОшибка. ^7Укажите сумму от %d ^3RUR ^7до %d ^3RUR^7.",cr/5,cr); send_mtc(players[i].UCID, Text); return; }

            if (players[i].Cash>(summ*3/2))
            {
                send_mtc(players[i].UCID,"^1| ^C^7В выдаче кредита ^1отказано^7.");
                send_mtc(players[i].UCID,"^1| ^C^7Сумма на вашем счете превышает размер кредита более чем в половину.");
                return;
            }
            //выдаем кредит
            sprintf(Text,"^5| ^7^CВам выдан кредит на сумму %d ^3RUR^7.",summ); send_mtc(players[i].UCID, Text);
            players[i].Credit = summ; players[i].Date_create = time(NULL);

            RemFrBank(summ); //берем бабки из банка
            AddCash(players[i].UCID,summ,false); //выдаем бабки игроку
            bank_save(players[i].UCID); //сохраняемся
        }

        if (strncmp(Message, "!repay", strlen("!repay")) == 0 )
        {
            if (players[i].Date_create==0) {send_mtc(players[i].UCID,"^1| ^C^7У вас нет кредитов");return;}

            if (players[i].Cash<players[i].Credit*13/10) {send_mtc(players[i].UCID,"^1| ^C^7На вашем счете недостаточно средств для погашения кредита");return;}

            send_mtc(players[i].UCID, "^5| ^7^CВы погасили кредит.");
            players[i].Credit = 0;
            players[i].Date_create = 0;

            RemCash(players[i].UCID,players[i].Credit*13/10); //отбираем бабки у игрока
            AddToBank(players[i].Credit*13/10); //сдаем в банк
            bank_save(players[i].UCID); //сохраняемся
        }
    }
}

void RCBank::insim_mci( struct IS_MCI* pack_mci )
{
	for (int i = 0; i < pack_mci->NumC; i++){for (int j =0; j < MAX_PLAYERS; j++)
        {
            if (pack_mci->Info[i].PLID == players[j].PLID and players[j].PLID != 0 and players[j].UCID != 0)
            {
                //баланс игрока
            	btn_cash(j);

                //зона банк
                int X = pack_mci->Info[i].X/65536; int Y = pack_mci->Info[i].Y/65536;
                if (Check_Pos(TrackInf.BankCount,TrackInf.XBank,TrackInf.YBank,X,Y))
                {
                    if (!players[j].InZone)
                    {
                        players[j].InZone=true;

                        send_mtc(players[j].UCID,"^5| Bank");
                        send_mtc(players[j].UCID,"^5| ^7!info - ^2^CОбщая информация");
                        send_mtc(players[j].UCID,"^5| ^7!credit info N - ^2^CРасчет процентов по кредиту на сумму N");
                        send_mtc(players[j].UCID,"^5| ^7!credit N (!credit 50000) - ^2^CПолучить кредит на сумму N");
                        send_mtc(players[j].UCID,"^5| ^7!repay - ^2^CПогасить кредит");
                    }
                }
                else if(players[j].InZone) players[j].InZone=false;
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
    cout << "RCBank::readconfig\n" ;
    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCBank\\tracks\\%s.txt",RootDir,Track);
    HANDLE fff;WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE){ printf ("RCBank: Can't find\n%s",file); return; }
    FindClose(fff);
    ifstream readf (file,ios::in);
    while (readf.good())
    {
        char str[128];
        readf.getline(str,128);
        TrackInf.BankCount = atoi(str);
        //printf ("%d\n",TrackInf.BankCount);
        for (int i=0; i<TrackInf.BankCount; i++)
        {
            readf.getline(str,128);
            char * X; char * Y;
            X = strtok (str,";");
            Y = strtok (NULL,";");
            TrackInf.XBank[i] = atoi(X);
            TrackInf.YBank[i] = atoi(Y);
            //printf ("%d;%d\n",TrackInf.XBank[i],TrackInf.YBank[i]);
        }
    } //while readf.good()
    readf.close();

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


// функции-утилиты

int RCBank::check_pos(struct BankPlayer *splayer)
{
    int PLX = splayer->Info.X/65536;
    int PLY = splayer->Info.Y/65536;
    if (Check_Pos(4,zone.dealX,zone.dealY,PLX,PLY))
        return 1;

    return 0;
}
