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
    char Text[96];
    sprintf(Text,"^5| ^7^C�� ��� ���� ���������: %d ^3RUR.",Cash);
    if( Show )
        send_mtc(UCID, Text);

    players[ UCID ].Cash += Cash;
    return true;
}

bool RCBank::RemCash(byte UCID, int Cash)
{
    char Text[96];
    sprintf(Text,"^5| ^7^C� ������ ����� �������: %d ^3RUR.",Cash);
    send_mtc(UCID, Text);
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

    /** ������� **/
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

    /** credit **/
    if (players[ packet->UCID ].Date_create!=0)
    {
        char Text[96];
        double razn=((double)(players[ packet->UCID ].Date_create)+30*24*3600 - (double)time(NULL))/(24*3600);

        printf("%d",razn);

        if (razn<=4 and razn>0)
        {

            sprintf(Text,"^1| ^1^C��������^7%3.0f ^1��� �� ������ �������!",razn);
            send_mtc( packet->UCID , Text);
        }
        if (razn==0)
        {
            sprintf(Text,"^1| ^1^C������� ��������� ���� ��� ��������� �������!");
            send_mtc( packet->UCID , Text);
            send_mtc( packet->UCID , Text);
            send_mtc( packet->UCID , Text);
        }
        if (razn<0) credit_penalty( packet->UCID );
    }
    /** credit **/

    return ;
}

void RCBank::credit_penalty (byte UCID)
{
    char Text[96];
    double razn=((double)(players[ UCID ].Date_create)+30*24*3600 - (double)time(NULL))/(24*3600);
    int c = (int)abs((ceil(razn)));
    sprintf(Text,"^1| ^1^C�� ���������� ������!");
    send_mtc(UCID, Text);
    sprintf(Text,"^1| ^7^C� ������ ����� ������ ������ (%d) + ����� �� ������ ����������� ����",players[ UCID ].Credit*13/10);
    send_mtc( UCID, Text);

    RemCash( UCID,( ( players[ UCID ].Credit ) * 13 / 10 + 1000 * c ) ); //�������� ����� � ������
    AddToBank( ( players[ UCID ].Credit ) * 13 / 10 + 1000 * c ); //����� � ����
    players[ UCID ].Credit = 0;
    players[ UCID ].Date_create = 0;
    bank_save( UCID ); //�����������
}

void RCBank::insim_npl( struct IS_NPL* packet )
{
    PLIDtoUCID[ packet->PLID ] = packet->UCID;

    if (players[packet->UCID].Cash > 5000000)
        players[packet->UCID].Cash = 5000000;
}

void RCBank::insim_plp( struct IS_PLP* packet)
{
    PLIDtoUCID.erase( packet->PLID );
}

void RCBank::insim_pll( struct IS_PLL* packet )
{
    PLIDtoUCID.erase( packet->PLID );
}

void RCBank::insim_cnl( struct IS_CNL* packet )
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

    /* Capital */
    sprintf(query,"UPDATE bank SET cash = %f WHERE username='_RC_Bank_Capital_'" , BankFond);

    if( mysql_ping( &rcbankDB ) != 0 )
        printf("Bank Error: connection with MySQL server was lost\n");

    if( mysql_query( &rcbankDB , query) != 0 )
        printf("Bank Error: MySQL Query Save\n");

    printf("Capital Log: Affected rows = %d\n", mysql_affected_rows( &rcbankDB ) );

}

void RCBank::insim_cpr( struct IS_CPR* packet )
{
    strcpy(players[ packet->UCID ].PName, packet->PName);
}

void RCBank::insim_mso( struct IS_MSO* packet )
{

    char Message[96];
    strcpy(Message,packet->Msg + ((unsigned char)packet->TextStart));

    if (players[ packet->UCID ].InZone)
    {
        char Text[96];
        //��������� ����� �������:
        int cr = dl->GetLVL( packet->UCID ) * 10000;
        if (cr>500000) cr = 500000;

        if (strncmp(Message, "!info", strlen("!info")) == 0 )
        {

            send_mtc( packet->UCID ,"^5| ^C����������");
            if (dl->GetLVL( packet->UCID )<5)
            {
                send_mtc( packet->UCID ,"^1| ^C^7����� �������: ^15");
                return;
            }
            //������
            sprintf(Text,"^5| ^7^C��� �������� ������ �� ����� �� %d ^3RUR ^7�� %d ^3RUR^7.",cr/5,cr);
            send_mtc( packet->UCID , Text);
            send_mtc( packet->UCID ,"^5| ^C^7������ �������� ��� ^230 %^7, �� ���� �� ����� ^230^7 ����.");
            send_mtc( packet->UCID ,"^5| ^C^7��� ��������� ��������� ����� ����� ������ ����� ������� + �������.");
            //send_mtc( packet->UCID ,"^6| ^C^7��� ��������� ����� ��� �� ^21 ^7���� ����� ����� ����� ������� + ������ ������� + �����.");
        }

        if (strncmp(Message, "!credit info", strlen("!credit info")) == 0 )
        {
            if (dl->GetLVL( packet->UCID ) < 5 )
            {
                send_mtc( packet->UCID ,"^5| ^C^7����� �������: ^15");
                return;
            }

            if (players[ packet->UCID ].Date_create!=0)
            {
                struct tm * timeinfo;
                char DateCreate [80];
                timeinfo = localtime (&players[ packet->UCID ].Date_create);
                strftime (DateCreate,80,"%d.%m.%Y",timeinfo);

                double razn=((double)(players[ packet->UCID ].Date_create)+30*24*3600 - (double)time(NULL))/(24*3600);

                sprintf(Text,"^5| ^7^C�� ������ ������ �� ����� %d ^3RUR^7. ���� ������: %s",players[ packet->UCID ].Credit,DateCreate);
                send_mtc( packet->UCID , Text);
                sprintf( Text,"^5| ^7^C����� ��������: %d ^3RUR^7.",players[ packet->UCID ].Credit * 13 / 10 );
				send_mtc( packet->UCID , Text);
                sprintf(Text,"^5| ^7^C����� �� ������: ^2%3.0f ^7����.",razn);
                send_mtc( packet->UCID , Text);
                return;
            }

            strtok (Message," ");
            strtok (NULL," ");
            int summ = atoi(strtok (NULL," "));
            if (summ <= 0 )
				summ = cr;
            else if( summ < cr / 5 or summ > cr )
            {
                sprintf(Text,"^5| ^7^C������. ^7������� ����� �� %d ^3RUR ^7�� %d ^3RUR^7.",cr/5,cr);
                send_mtc( packet->UCID , Text);
                return;
            }

            send_mtc( packet->UCID ,"^5| ^C���������� �� ������������ �������");
            sprintf( Text,"^5| ^7^C�������� �����: %d ^3RUR^7.",summ );
            send_mtc( packet->UCID , Text);
            sprintf( Text,"^5| ^7^C����� ��������: %d ^3RUR^7.",summ * 13 / 10 );
            send_mtc( packet->UCID , Text);
            return;
        }

        if (strncmp(Message, "!credit", strlen("!credit")) == 0 )
        {
            if ( dl->GetLVL( packet->UCID ) < 5 )
            {
                send_mtc( packet->UCID ,"^5| ^C^7����� �������: ^15");
                return;
            }

            if( players[ packet->UCID ].Date_create != 0 )
            {
                send_mtc( packet->UCID ,"^5| ^C^7� ��� ��� �������� ������ (^3!credit info^7)");
                return;
            }

            strtok (Message," ");
            int summ = atoi(strtok (NULL," "));

            if ( summ < cr / 5 or summ > cr)
            {
                sprintf( Text ,"^5| ^7^C������. ^7������� ����� �� %d ^3RUR ^7�� %d ^3RUR^7.",cr/5,cr);
                send_mtc( packet->UCID , Text);
                return;
            }

            if (players[ packet->UCID ].Cash>(summ*3/2))
            {
                send_mtc( packet->UCID ,"^5| ^C^7� ������ ������� ^1��������^7.");
                send_mtc( packet->UCID ,"^5| ^C^7����� �� ����� ����� ��������� ������ ������� ����� ��� � ��������.");
                return;
            }
            //������ ������
            sprintf(Text,"^5| ^7^C��� ����� ������ �� ����� %d ^3RUR^7.", summ );
            send_mtc( packet->UCID , Text );
            players[ packet->UCID ].Credit = summ;
            players[ packet->UCID ].Date_create = time(NULL);

            RemFrBank(summ); //����� ����� �� �����
            AddCash( packet->UCID ,summ,false); //������ ����� ������
            bank_save( packet->UCID ); //�����������
        }

        if (strncmp(Message, "!repay", strlen("!repay")) == 0 )
        {
            if ( players[ packet->UCID ].Date_create == 0 )
            {
                send_mtc( packet->UCID ,"^5| ^C^7� ��� ��� ��������");
                return;
            }

            if (players[ packet->UCID ].Cash<players[ packet->UCID ].Credit*13/10)
            {
                send_mtc( packet->UCID ,"^5| ^C^7�� ����� ����� ������������ ������� ��� ��������� �������");
                return;
            }

            send_mtc( packet->UCID , "^5| ^7^C�� �������� ������.");
            players[ packet->UCID ].Credit = 0;
            players[ packet->UCID ].Date_create = 0;

            RemCash( packet->UCID, players[ packet->UCID ].Credit * 13 / 10 ); //�������� ����� � ������
            AddToBank( players[ packet->UCID ].Credit * 13 / 10 ); //����� � ����
            bank_save( packet->UCID ); //�����������
        }
    }
}

void RCBank::insim_mci( struct IS_MCI* pack_mci )
{
    for (int i = 0; i < pack_mci->NumC; i++)
    {
		byte UCID = PLIDtoUCID[ pack_mci->Info[i].PLID ];
		//������ ������
		BtnCash( UCID );

		//���� ����
		int X = pack_mci->Info[i].X/65536;
		int Y = pack_mci->Info[i].Y/65536;
		if (Check_Pos(TrackInf.BankCount,TrackInf.XBank,TrackInf.YBank,X,Y))
		{
			if (!players[ UCID ].InZone)
			{
				players[ UCID ].InZone=true;

				send_mtc( UCID ,"^5| Bank");
				send_mtc( UCID ,"^5| ^7!info - ^2^C����� ����������");
				send_mtc( UCID ,"^5| ^7!credit info N - ^2^C������ ��������� �� ������� �� ����� N");
				send_mtc( UCID ,"^5| ^7!credit N (!credit 50000) - ^2^C�������� ������ �� ����� N");
				send_mtc( UCID ,"^5| ^7!repay - ^2^C�������� ������");
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


// �������-�������

int RCBank::check_pos(struct BankPlayer *splayer)
{
    int PLX = splayer->Info.X/65536;
    int PLY = splayer->Info.Y/65536;
    if (Check_Pos(4,zone.dealX,zone.dealY,PLX,PLY))
        return 1;

    return 0;
}
