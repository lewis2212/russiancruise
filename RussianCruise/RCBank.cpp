using namespace std;

#include "RCBank.h"


RCBank::RCBank()
{

}

RCBank::~RCBank()
{
    mysql_close( &rcbankDB );
}

int RCBank::GetCash(byte UCID)
{
    return (int)players[ UCID ].Cash;
}

bool RCBank::AddCash(byte UCID, int Cash, bool Show = false)
{
    char Text[128];
    sprintf(Text,"^5| ^7^CНа ваш счет поступило: %d ^3RUR^7.",Cash);
    if( Show )
        SendMTC(UCID, Text);

    players[ UCID ].Cash += Cash;
    return true;
}

bool RCBank::RemCash(byte UCID, int Cash)
{
    char Text[128];
    sprintf(Text,"^5| ^7^CС вашего счета списано: %d ^3RUR^7.",Cash);
    SendMTC(UCID, Text);
    players[ UCID ].Cash -= Cash;
    return true;
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
    return players[ UCID ].InZone;
}

int RCBank::init(const char *dir,void *CInSim, void *RCMessageClass, void *DL)
{

    strcpy(RootDir,dir);

    insim = (CInsim *)CInSim;
    if(!insim)
    {
        cout << "RCBank Error: Can't struct CInsim class" << endl;;
        return -1;
    }

    msg = (RCMessage *)RCMessageClass;
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


void RCBank::InsimNCN( struct IS_NCN* packet )
{
    int i;

    if (packet->UCID == 0)
        return;


    strcpy(players[ packet->UCID ].UName, packet->UName);
    strcpy(players[ packet->UCID ].PName, packet->PName);

    char query[128];
    sprintf(query,"SELECT cash FROM bank WHERE username='%s' LIMIT 1;",packet->UName);

    if( mysql_ping( &rcbankDB ) != 0 )
    {
        printf("Error: connection with MySQL server was lost\n");
    }
    if( mysql_query( &rcbankDB , query) != 0 )
    {
        printf("Error: MySQL Query\n");
    }

    rcbankRes = mysql_store_result( &rcbankDB );
    if(rcbankRes == NULL) printf("Error: can't get the result description\n");

    if(mysql_num_rows( rcbankRes ) > 0)
    {
        rcbankRow = mysql_fetch_row( rcbankRes );
        players[ packet->UCID ].Cash = atof( rcbankRow[0] );
    }
    else
    {
        printf("Can't find %s\n Create user\n",packet->UName);
        sprintf(query,"INSERT INTO bank (username) VALUES ('%s');",packet->UName);
        if( mysql_ping( &rcbankDB ) != 0 )
        {
            printf("Error: connection with MySQL server was lost\n");
        }
        if( mysql_query( &rcbankDB , query) != 0 )
        {
            printf("Error: MySQL Query\n");
        }

        players[ packet->UCID ].Cash = 1000;
        bank_save( packet->UCID );
    }
    mysql_free_result( rcbankRes );

    /** кредиты **/
    sprintf(query,"SELECT cash, date_create FROM bank_credits WHERE username='%s' LIMIT 1;",packet->UName);

    if( mysql_ping( &rcbankDB ) != 0 )
    {
        printf("Error credits: connection with MySQL server was lost\n");
    }
    if( mysql_query( &rcbankDB , query) != 0 )
    {
        printf("Error credits: MySQL Query\n");
    }

    rcbankRes = mysql_store_result( &rcbankDB );
    if(rcbankRes == NULL) printf("Error credits: can't get the result description\n");

    if(mysql_num_rows( rcbankRes ) > 0)
    {
        rcbankRow = mysql_fetch_row( rcbankRes );
        players[ packet->UCID ].Credit = atof( rcbankRow[0] );
        players[ packet->UCID ].Date_create = atof( rcbankRow[1] );
        //printf("Check user %s, %d, %d\n",packet->UName,players[i].Credit,players[i].Date_create);
    }
    else
    {
        sprintf(query,"INSERT INTO bank_credits (username,cash,date_create) VALUES ('%s',0,0);",packet->UName);
        if( mysql_ping( &rcbankDB ) != 0 )
        {
            printf("Error credits: connection with MySQL server was lost\n");
        }
        if( mysql_query( &rcbankDB , query) != 0 )
        {
            printf("Error credits: MySQL Query\n");
        }

        players[ packet->UCID ].Credit = 0;
        players[ packet->UCID ].Date_create = 0;
        //printf("Create user %s, %d, %d\n",packet->UName,players[i].Credit,players[i].Date_create);
    }

    mysql_free_result( rcbankRes );

    /** вклады **/
    sprintf(query,"SELECT cash, date_create FROM bank_deposits WHERE username='%s' LIMIT 1;",packet->UName);

    if( mysql_ping( &rcbankDB ) != 0 )
    {
        printf("Error deposits: connection with MySQL server was lost\n");
    }
    if( mysql_query( &rcbankDB , query) != 0 )
    {
        printf("Error deposits: MySQL Query\n");
    }

    rcbankRes = mysql_store_result( &rcbankDB );
    if(rcbankRes == NULL) printf("Error deposits: can't get the result description\n");

    if(mysql_num_rows( rcbankRes ) > 0)
    {
        rcbankRow = mysql_fetch_row( rcbankRes );
        players[ packet->UCID ].Deposit = atof( rcbankRow[0] );
        players[ packet->UCID ].Dep_Date_create = atof( rcbankRow[1] );
    }
    else
    {
        sprintf(query,"INSERT INTO bank_deposits (username,cash,date_create) VALUES ('%s',0,0);",packet->UName);
        if( mysql_ping( &rcbankDB ) != 0 )
        {
            printf("Error deposits: connection with MySQL server was lost\n");
        }
        if( mysql_query( &rcbankDB , query) != 0 )
        {
            printf("Error deposits: MySQL Query\n");
        }

        players[ packet->UCID ].Deposit = 0;
        players[ packet->UCID ].Dep_Date_create = 0;
    }
    mysql_free_result( rcbankRes );

    /** credit **/
    if (players[ packet->UCID ].Date_create!=0)
    {
        char Text[128];
        double razn=((double)(players[ packet->UCID ].Date_create)+30*24*3600 - (double)time(NULL))/(24*3600);

        printf("%d",razn);

        if (razn<=4 and razn>0)
        {

            sprintf(Text,"^1| ^1^CОсталось ^7%0.0f ^1дня до снятия кредита!",razn);
            SendMTC( packet->UCID , Text);
        }
        if (razn==0)
        {
            sprintf(Text,"^1| ^1^CСЕГОДНЯ ПОСЛЕДНИЙ ДЕНЬ ДЛЯ ПОГАШЕНИЯ КРЕДИТА!");
            SendMTC( packet->UCID , Text);
            SendMTC( packet->UCID , Text);
            SendMTC( packet->UCID , Text);
        }
        if (razn<0) credit_penalty( packet->UCID );
    }
    /** credit **/

    BtnCash( packet->UCID );

    return ;
}

void RCBank::credit_penalty (byte UCID)
{
    char Text[128];
    double razn=((double)(players[ UCID ].Date_create)+30*24*3600 - (double)time(NULL))/(24*3600);
    int c = (int)abs((ceil(razn)));
    sprintf(Text,"^1| ^1^CВЫ ПРОСРОЧИЛИ КРЕДИТ!");
    SendMTC(UCID, Text);
    sprintf(Text,"^1| ^7^CС вашего счета списан кредит (%d) + штраф за каждый пропущенный день",players[ UCID ].Credit*13/10);
    SendMTC( UCID, Text);

    RemCash( UCID,( ( players[ UCID ].Credit ) * 13 / 10 + 1000 * c ) ); //отбираем бабки у игрока
    AddToBank( ( players[ UCID ].Credit ) * 13 / 10 + 1000 * c ); //сдаем в банк
    players[ UCID ].Credit = 0;
    players[ UCID ].Date_create = 0;
    bank_save( UCID ); //сохраняемся
}

void RCBank::InsimNPL( struct IS_NPL* packet )
{
    PLIDtoUCID[ packet->PLID ] = packet->UCID;

    if (players[packet->UCID].Cash > 5000000)
        players[packet->UCID].Cash = 5000000;
}

void RCBank::InsimPLP( struct IS_PLP* packet)
{
    PLIDtoUCID.erase( packet->PLID );
}

void RCBank::InsimPLL( struct IS_PLL* packet )
{
    PLIDtoUCID.erase( packet->PLID );
}

void RCBank::InsimCNL( struct IS_CNL* packet )
{
    bank_save( packet->UCID );
    players.erase( packet->UCID );
}

void RCBank::bank_save (byte UCID)
{
    char query[128];
    sprintf(query,"UPDATE bank SET cash = %f WHERE username='%s'" , players[ UCID ].Cash, players[ UCID ].UName);
    if( mysql_ping( &rcbankDB ) != 0 )
        printf("Bank Error: connection with MySQL server was lost\n");
    if( mysql_query( &rcbankDB , query) != 0 )
        printf("Bank Error: MySQL Query Save\n");
    printf("Bank Log: Affected rows = %d\n", mysql_affected_rows( &rcbankDB ) );

    /* Credit */
    sprintf(query,"UPDATE bank_credits SET cash = %d, date_create = %d WHERE username='%s'" , players[ UCID ].Credit, players[ UCID ].Date_create, players[ UCID ].UName);
    if( mysql_ping( &rcbankDB ) != 0 )
        printf("Credit Error: connection with MySQL server was lost\n");
    if( mysql_query( &rcbankDB , query) != 0 )
        printf("Credit Error: MySQL Query Save\n");
    printf("Credit Log: Affected rows = %d\n", mysql_affected_rows( &rcbankDB ) );

    /* Deposit */
    sprintf(query,"UPDATE bank_deposits SET cash = %d, date_create = %d WHERE username='%s'" , players[ UCID ].Deposit, players[ UCID ].Dep_Date_create, players[ UCID ].UName);
    if( mysql_ping( &rcbankDB ) != 0 )
        printf("Deposit Error: connection with MySQL server was lost\n");
    if( mysql_query( &rcbankDB , query) != 0 )
        printf("Deposit Error: MySQL Query Save\n");
    printf("Deposit Log: Affected rows = %d\n", mysql_affected_rows( &rcbankDB ) );

    /* Capital */
    sprintf(query,"UPDATE bank SET cash = %f WHERE username='_RC_Bank_Capital_'" , BankFond);
    if( mysql_ping( &rcbankDB ) != 0 )
        printf("Bank Error: connection with MySQL server was lost\n");
    if( mysql_query( &rcbankDB , query) != 0 )
        printf("Bank Error: MySQL Query Save\n");
    printf("Capital Log: Affected rows = %d\n", mysql_affected_rows( &rcbankDB ) );

}

void RCBank::InsimCPR( struct IS_CPR* packet )
{
    strcpy(players[ packet->UCID ].PName, packet->PName);
}

void RCBank::InsimMSO( struct IS_MSO* packet )
{

    char Message[128];
    strcpy(Message,packet->Msg + ((unsigned char)packet->TextStart));

    if (players[ packet->UCID ].InZone)
    {
        char Text[128];

        /** Кредиты **/

        //Доступная сумма кредита:
        int cr = dl->GetLVL( packet->UCID ) * 10000;
        if (cr>500000) cr = 500000;

        if (strncmp(Message, "!credit info", strlen("!credit info")) == 0 )
        {
            if (dl->GetLVL( packet->UCID ) < 5 )
            {
                SendMTC( packet->UCID ,"^5| ^C^7Нужен уровень: ^15");
                return;
            }
            strtok (Message," ");
            strtok (NULL," ");
            int summ = atoi(strtok (NULL," "));
            if (summ <= 0 )
				summ = cr;
            else if( summ < cr / 5 or summ > cr )
            {
                sprintf(Text,"^5| ^7^CОшибка. ^7Укажите сумму от %d ^3RUR ^7до %d ^3RUR^7.",cr/5,cr);
                SendMTC( packet->UCID , Text);
                return;
            }
            SendMTC( packet->UCID ,"^5| ^CИнформация по кредиту");
            if (players[ packet->UCID ].Date_create!=0)
            {
                struct tm * timeinfo;
                char DateCreate [80];
                timeinfo = localtime (&players[ packet->UCID ].Date_create);
                strftime (DateCreate,80,"%d.%m.%Y",timeinfo);

                double razn=((double)(players[ packet->UCID ].Date_create)+30*24*3600 - (double)time(NULL))/(24*3600);

                sprintf(Text,"^5| ^7^CВы имеете кредит на сумму %d ^3RUR^7. Дата выдачи: %s",players[ packet->UCID ].Credit,DateCreate);
                SendMTC( packet->UCID , Text);
                sprintf( Text,"^5| ^7^CСумма возврата: %d ^3RUR^7.",players[ packet->UCID ].Credit * 13 / 10 );
				SendMTC( packet->UCID , Text);
                sprintf(Text,"^5| ^7^CВремя до снятия: ^2%0.0f ^7дней.",razn);
                SendMTC( packet->UCID , Text);
                return;
            }
            sprintf(Text,"^5| ^7^CВам доступен кредит на сумму от %d ^3RUR ^7до %d ^3RUR^7.",cr/5,cr);
            SendMTC( packet->UCID , Text);
            SendMTC( packet->UCID ,"^5| ^C^7Кредит выдается под ^230 %^7, на срок не более ^230^7 дней.");
            SendMTC( packet->UCID ,"^5| ^C^7При досрочном погашении будет снята полная сумма кредита + процент.");
            sprintf( Text,"^5| ^7^CЖелаемая сумма: %d ^3RUR^7.",summ );
            SendMTC( packet->UCID , Text);
            sprintf( Text,"^5| ^7^CСумма возврата: %d ^3RUR^7.",summ * 13 / 10 );
            SendMTC( packet->UCID , Text);
            return;
        }
        if (strncmp(Message, "!credit", strlen("!credit")) == 0 )
        {
        	if (players[ packet->UCID ].Dep_Date_create!=0)
            {
                SendMTC( packet->UCID ,"^5| ^C^7Вы не можете взять кредит, не закрыв вклад.");
                return;
            }
            if ( dl->GetLVL( packet->UCID ) < 5 )
            {
                SendMTC( packet->UCID ,"^5| ^C^7Нужен уровень: ^15");
                return;
            }
            if( players[ packet->UCID ].Date_create != 0 )
            {
                SendMTC( packet->UCID ,"^5| ^C^7У вас уже оформлен кредит (^3!credit info^7)");
                return;
            }
            strtok (Message," ");
            int summ = atoi(strtok (NULL," "));
            if ( summ < cr / 5 or summ > cr)
            {
                sprintf( Text ,"^5| ^7^CОшибка. ^7Укажите сумму от %d ^3RUR ^7до %d ^3RUR^7.",cr/5,cr);
                SendMTC( packet->UCID , Text);
                return;
            }
            if (players[ packet->UCID ].Cash>(summ*3/2))
            {
                SendMTC( packet->UCID ,"^5| ^C^7В выдаче кредита ^1отказано^7.");
                SendMTC( packet->UCID ,"^5| ^C^7Сумма на вашем счете превышает размер кредита более чем в половину.");
                return;
            }
            //выдаем кредит
            sprintf(Text,"^5| ^7^CВам выдан кредит на сумму %d ^3RUR^7.", summ );
            SendMTC( packet->UCID , Text );
            players[ packet->UCID ].Credit = summ;
            players[ packet->UCID ].Date_create = time(NULL);
            RemFrBank(summ); //берем бабки из банка
            AddCash( packet->UCID ,summ,false); //выдаем бабки игроку
            bank_save( packet->UCID ); //сохраняемся
        }
        if (strncmp(Message, "!repay", strlen("!repay")) == 0 )
        {
            if ( players[ packet->UCID ].Date_create == 0 )
            {
                SendMTC( packet->UCID ,"^5| ^C^7У вас нет кредитов.");
                return;
            }
            if (players[ packet->UCID ].Cash<players[ packet->UCID ].Credit*13/10)
            {
                SendMTC( packet->UCID ,"^5| ^C^7На вашем счете недостаточно средств для погашения кредита.");
                return;
            }
            SendMTC( packet->UCID , "^5| ^7^CВы погасили кредит.");
            RemCash( packet->UCID, players[ packet->UCID ].Credit * 13 / 10 ); //отбираем бабки у игрока
            AddToBank( players[ packet->UCID ].Credit * 13 / 10 ); //сдаем в банк
            players[ packet->UCID ].Credit = 0;
            players[ packet->UCID ].Date_create = 0;
            bank_save( packet->UCID ); //сохраняемся
        }

        /** Вклады **/

        //Доступная сумма вклада:
        int dr = dl->GetLVL( packet->UCID ) * 10000;
        if (dr>500000) dr = 500000;
		if (strncmp(Message, "!deposit info", strlen("!deposit info")) == 0 )
        {
            if (dl->GetLVL( packet->UCID ) < 20 )
            {
                SendMTC( packet->UCID ,"^5| ^C^7Нужен уровень: ^120");
                return;
            }
            strtok (Message," ");
            strtok (NULL," ");
            int summ = atoi(strtok (NULL," "));
            if (summ <= 0 )
				summ = dr;
            else if( summ > dr )
            {
                sprintf(Text,"^5| ^7^CОшибка. ^7Укажите сумму до %d ^3RUR.",dr);
                SendMTC( packet->UCID , Text);
                return;
            }
            SendMTC( packet->UCID ,"^5| ^CИнформация по вкладу");
            if (players[ packet->UCID ].Dep_Date_create!=0)
            {
                struct tm * timeinfo;
                char DateCreate [80];
                timeinfo = localtime (&players[ packet->UCID ].Dep_Date_create);
                strftime (DateCreate,80,"%d.%m.%Y",timeinfo);

                double razn=((double)(players[ packet->UCID ].Dep_Date_create)+30*24*3600 - (double)time(NULL))/(24*3600);
                sprintf(Text,"^5| ^7^CВы имеете вклад на сумму %d ^3RUR^7. Дата открытия: %s",players[ packet->UCID ].Deposit,DateCreate);SendMTC( packet->UCID , Text);
                if (razn<=0)
				{
					sprintf( Text,"^5| ^7^CСумма на данный момент: %d ^3RUR^7 (%d ^3RUR^7 в сутки)",players[ packet->UCID ].Deposit + players[ packet->UCID ].Deposit * 5/1000 * 30, 0 ); SendMTC( packet->UCID , Text);
					sprintf(Text,"^5| ^7^CСрок вклада истек, проценты не начисляются.");SendMTC( packet->UCID , Text);
					return;
				}
                sprintf( Text,"^5| ^7^CСумма на данный момент: %d ^3RUR^7 (%d ^3RUR^7 в сутки)",players[ packet->UCID ].Deposit + players[ packet->UCID ].Deposit * 5/1000 * (int)(30-razn), players[ packet->UCID ].Deposit * 5/1000 ); SendMTC( packet->UCID , Text);
                sprintf(Text,"^5| ^7^CДо закрытия вклада осталось: ^2%0.0f ^7дней.",razn);SendMTC( packet->UCID , Text);
                return;
            }

            sprintf(Text,"^5| ^7^CВам доступен вклад на сумму до %d ^3RUR.",dr);SendMTC( packet->UCID , Text);
            SendMTC( packet->UCID ,"^5| ^C^7Вклад открывается под ^215 %^7 в месяц, на срок не более ^230^7 дней.");
            sprintf( Text,"^5| ^7^CЖелаемая сумма вклада: %d ^3RUR^7.",summ );SendMTC( packet->UCID , Text);
            sprintf( Text,"^5| ^7^CСумма с учетом процентов через ^230 ^7дней: %d ^3RUR^7 (%d ^3RUR^7 в сутки).",summ * 115 / 100,summ * 15 / 3000 );SendMTC( packet->UCID , Text);
            sprintf( Text,"^5| ^7^CВы можете закрыть вклад не ранее чем через ^214 ^7дней с момента вклада.",summ );SendMTC( packet->UCID , Text);
            return;
        }
        if (strncmp(Message, "!deposit", strlen("!deposit")) == 0 )
        {
        	if (players[ packet->UCID ].Date_create!=0)
            {
                SendMTC( packet->UCID ,"^5| ^C^7Вы не можете открыть вклад, не погасив кредит.");
                return;
            }
            if ( dl->GetLVL( packet->UCID ) < 5 )
            {
                SendMTC( packet->UCID ,"^5| ^C^7Нужен уровень: ^120");
                return;
            }
            if( players[ packet->UCID ].Dep_Date_create != 0 )
            {
                SendMTC( packet->UCID ,"^5| ^C^7У вас уже открыт вклад (^3!deposit info^7).");
                return;
            }
            strtok (Message," ");
            int summ = atoi(strtok (NULL," "));
            if (summ > dr or summ==0)
            {
                sprintf(Text,"^5| ^7^CОшибка. ^7Укажите сумму до %d ^3RUR.",dr); SendMTC( packet->UCID , Text);
                return;
            }
            if (players[ packet->UCID ].Cash<=(summ))
            {
                SendMTC( packet->UCID ,"^5| ^C^7На вашем счете недостаточно средств для открытия вклада.");
                return;
            }
            //создаем вклад
            sprintf(Text,"^5| ^7^CВы открыли вклад на сумму %d ^3RUR^7.", summ ); SendMTC( packet->UCID , Text );
            players[ packet->UCID ].Deposit = summ;
            players[ packet->UCID ].Dep_Date_create = time(NULL);
            RemCash( packet->UCID ,summ); //снимаем у игрока
            AddToBank(summ); //кладем в банк
            bank_save( packet->UCID ); //сохраняемся
        }
        if (strncmp(Message, "!withdraw", strlen("!withdraw")) == 0 )
        {
        	if (players[ packet->UCID ].Dep_Date_create==0)
            {
                SendMTC( packet->UCID ,"^5| ^C^7У вас нет вкладов.");
                return;
            }
            strtok (Message," ");
        	double razn=((double)(players[ packet->UCID ].Dep_Date_create)+30*24*3600 - (double)time(NULL))/(24*3600);
            int dtt;
			char * s;
			s = strtok(NULL," ");
			if (s != NULL)
			{
				if (strncmp(s, "yes", strlen("yes")) == 0)
				{
					sprintf(Text,"^5| ^7^CВы закрыли вклад."); SendMTC( packet->UCID , Text );
					dtt = players[ packet->UCID ].Deposit + players[ packet->UCID ].Deposit * 5/1000 * (int)(30-razn);
					sprintf(Text,"^5| ^7^CБанк получает неустойку в размере %d ^3RUR^7.",dtt*5/100); SendMTC( packet->UCID , Text );
					dtt = dtt - dtt*5/100;
					RemFrBank(dtt); //берем из банка
					AddCash( packet->UCID ,dtt, true); //отдаем игроку
					players[ packet->UCID ].Deposit = 0;
					players[ packet->UCID ].Dep_Date_create = 0;
					bank_save( packet->UCID ); //сохраняемся
					return;
				}
			}
            if (razn>16)
			{
				sprintf(Text,"^5| ^C^1Внимание! ^7Еще не прошло 14 дней с момента вклада."); SendMTC( packet->UCID , Text );
				SendMTC( packet->UCID ,"^5| ^7^CВведите ^3!withdraw yes ^7чтобы закрыть вклад. Банк получит неустойку в размере ^25 %^7.");
				return;
			}
        	sprintf(Text,"^5| ^7^CВы закрыли вклад."); SendMTC( packet->UCID , Text );
			if (razn<=0)
				dtt = players[ packet->UCID ].Deposit + players[ packet->UCID ].Deposit * 5/1000 * 30;
			else
				dtt = players[ packet->UCID ].Deposit + players[ packet->UCID ].Deposit * 5/1000 * (int)(30-razn);
			RemFrBank(dtt); //берем из банка
			AddCash( packet->UCID ,dtt, true); //отдаем игроку
			players[ packet->UCID ].Deposit = 0;
			players[ packet->UCID ].Dep_Date_create = 0;
			bank_save( packet->UCID ); //сохраняемся
        }

    }
}

void RCBank::InsimMCI( struct IS_MCI* pack_mci )
{
    for (int i = 0; i < pack_mci->NumC; i++)
    {
		byte UCID = PLIDtoUCID[ pack_mci->Info[i].PLID ];
		//баланс игрока
		BtnCash( UCID );

		//зона банк
		int X = pack_mci->Info[i].X/65536;
		int Y = pack_mci->Info[i].Y/65536;
		if (Check_Pos(TrackInf.BankCount,TrackInf.XBank,TrackInf.YBank,X,Y))
		{
			if (!players[ UCID ].InZone)
			{
				players[ UCID ].InZone=true;

				SendMTC( UCID ,"^5| Bank");
				SendMTC( UCID ,"^5| ^7!credit info N - ^2^CИнформация по кредиту на сумму N");
				SendMTC( UCID ,"^5| ^7!credit N (!credit 50000) - ^2^CПолучить кредит на сумму N");
				SendMTC( UCID ,"^5| ^7!repay - ^2^CПогасить кредит");
				SendMTC( UCID ,"^5| ^7!deposit info N - ^2^CИнформация по вкладу на сумму N");
				SendMTC( UCID ,"^5| ^7!deposit N (!deposit 200000)- ^2^CОткрыть вклад на сумму N");
				SendMTC( UCID ,"^5| ^7!withdraw - ^2^CЗакрыть вклад");
			}
		}
		else if(players[ UCID ].InZone)
			players[ UCID ].InZone = false;
    }
}

void RCBank::BtnCash ( byte UCID )
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID =  UCID;
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
    sprintf(cash,"%d",(int)players[ UCID ].Cash);
    if (players[ UCID ].Cash > 0)
        strcpy(pack.Text,"^2");
    else
        strcpy(pack.Text,"^1");
    strcat(pack.Text,cash);
    strcat(pack.Text,"^7 RUR.");
    insim->send_packet(&pack);

    if (strcmp(players[ UCID ].UName,"denis-takumi") == 0)
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
    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        printf ("RCBank: Can't find\n%s",file);
        return;
    }
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
            char * X;
            char * Y;
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
