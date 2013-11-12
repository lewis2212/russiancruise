using namespace std;

#include "RCBank.h"

RCBank::RCBank(const char* Dir)
{
    strcpy(RootDir,Dir);
}

RCBank::~RCBank()
{
}

int RCBank::GetCash(byte UCID)
{
    return (int)players[UCID].Cash;
}

bool RCBank::AddCash(byte UCID, int Cash, bool Show = false)
{
    char Text[128];
    sprintf(Text, msg->_(UCID, "GetMoneyA"), Cash);

    if (Show)
        SendMTC(UCID, Text);

    players[UCID].Cash += Cash;
    return true;
}

bool RCBank::RemCash(byte UCID, int Cash)
{
    char Text[128];
    sprintf(Text, msg->_(UCID, "RemMoneyA"), Cash);
    SendMTC(UCID, Text);
    players[UCID].Cash -= Cash;
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
    return players[UCID].InZone;
}

int RCBank::init(MYSQL *conn, CInsim *InSim, RCMessage *RCMessageClass, RCDL *DL)
{
    dbconn = conn;
    if (!dbconn)
    {
        printf("RCBank: Can't sctruct MySQL Connector\n");
        return -1;
    }

    insim = InSim;
    if (!insim)
    {
        cout << "RCBank Error: Can't struct CInsim class" << endl;
        return -1;
    }

    msg = RCMessageClass;
    if (!msg)
    {
        cout << "RCBank Error: Can't struct RCMessage class" << endl;
        return -1;
    }

    dl = DL;
    if (!dl)
    {
        printf ("RCBank Error: Can't struct RCDL class");
        return -1;
    }

    if (mysql_ping(dbconn) != 0)
    {
        printf("RCBank Error: connection with MySQL server was lost\n");
    }

    printf("RCBank Success: Connected to MySQL server\n");
    return 0;
}


void RCBank::InsimNCN(struct IS_NCN* packet)
{
    if (packet->UCID == 0)
        return;

    char kickCmd[64], msg[96];
    sprintf(kickCmd, "/kick %s",packet->UName);
    sprintf(msg, "^1RCBank error - BAD USER");

    strcpy(players[packet->UCID].UName, packet->UName);
    strcpy(players[packet->UCID].PName, packet->PName);

    char query[128];
    sprintf(query, "SELECT cash FROM bank WHERE username = '%s' LIMIT 1;", packet->UName);

    if (mysql_ping(dbconn) != 0)
    {
        printf("Error: connection with MySQL server was lost\n");
        SendMST(msg);
        SendMST(kickCmd);
        return;
    }

    if (mysql_query(dbconn, query) != 0)
    {
        printf("Error: MySQL Query\n");
        SendMST(msg);
        SendMST(kickCmd);
        return;
    }

    dbres = mysql_store_result(dbconn);
    if (dbres == NULL)
    {
        printf("Error: can't get the result description\n");
        SendMST(msg);
        SendMST(kickCmd);
        return;
    }

    if (mysql_num_rows(dbres) > 0)
    {
        dbrow = mysql_fetch_row(dbres);
        players[packet->UCID].Cash = atof(dbrow[0]);
    }
    else
    {
        printf("Can't find %s\n Create user\n", packet->UName);
        sprintf(query, "INSERT INTO bank (username) VALUES ('%s');", packet->UName);

        if (mysql_ping(dbconn) != 0)
        {
            printf("Error: connection with MySQL server was lost\n");
            SendMST(msg);
            SendMST(kickCmd);
            return;
        }

        if (mysql_query(dbconn, query) != 0)
        {
            printf("Error: MySQL Query\n");
        }

        players[packet->UCID].Cash = 1000;
        bank_save(packet->UCID);
    }
    mysql_free_result(dbres);

    /** кредиты **/
    sprintf(query, "SELECT cash, date_create FROM bank_credits WHERE username='%s' LIMIT 1;", packet->UName);

    if (mysql_ping(dbconn) != 0)
    {
        printf("Error credits: connection with MySQL server was lost\n");
        SendMST(msg);
        SendMST(kickCmd);
        return;
    }

    if (mysql_query(dbconn, query) != 0)
    {
        printf("Error credits: MySQL Query\n");
        SendMST(msg);
        SendMST(kickCmd);
        return;
    }

    dbres = mysql_store_result(dbconn);
    if (dbres == NULL)
    {
        printf("Error credits: can't get the result description\n");
        SendMST(msg);
        SendMST(kickCmd);
        return;
    }

    if (mysql_num_rows(dbres) > 0)
    {
        dbrow = mysql_fetch_row(dbres);
        players[packet->UCID].Credit = atof(dbrow[0]);
        players[packet->UCID].Date_create = atof(dbrow[1]);
    }
    else
    {
        sprintf(query, "INSERT INTO bank_credits (username, cash, date_create) VALUES ('%s', 0, 0);", packet->UName);
        if (mysql_ping(dbconn) != 0)
        {
            printf("Error credits: connection with MySQL server was lost\n");
            SendMST(msg);
            SendMST(kickCmd);
            return;
        }

        if (mysql_query(dbconn, query) != 0)
        {
            printf("Error credits: MySQL Query\n");
        }

        players[packet->UCID].Credit = 0;
        players[packet->UCID].Date_create = 0;
    }

    mysql_free_result(dbres);

    /** вклады **/
    sprintf(query, "SELECT cash, date_create FROM bank_deposits WHERE username='%s' LIMIT 1;", packet->UName);

    if (mysql_ping(dbconn) != 0)
    {
        printf("Error deposits: connection with MySQL server was lost\n");
        SendMST(msg);
        SendMST(kickCmd);
        return;
    }

    if (mysql_query(dbconn, query) != 0)
    {
        printf("Error deposits: MySQL Query\n");
        return;
    }

    dbres = mysql_store_result(dbconn);
    if (dbres == NULL)
    {
        printf("Error deposits: can't get the result description\n");
        SendMST(msg);
        SendMST(kickCmd);
        return;
    }

    if (mysql_num_rows(dbres) > 0)
    {
        dbrow = mysql_fetch_row(dbres);
        players[packet->UCID].Deposit = atof(dbrow[0]);
        players[packet->UCID].Dep_Date_create = atof(dbrow[1]);
    }
    else
    {
        sprintf(query, "INSERT INTO bank_deposits (username, cash, date_create) VALUES ('%s', 0, 0);", packet->UName);
        if (mysql_ping(dbconn) != 0)
        {
            printf("Error deposits: connection with MySQL server was lost\n");
            SendMST(msg);
            SendMST(kickCmd);
            return;
        }

        if (mysql_query(dbconn, query) != 0)
        {
            printf("Error deposits: MySQL Query\n");
        }

        players[packet->UCID].Deposit = 0;
        players[packet->UCID].Dep_Date_create = 0;
    }
    mysql_free_result(dbres);

    /** credit **/
    if (players[packet->UCID].Date_create!=0)
    {
        char Text[128];
        double razn=((double)(players[packet->UCID].Date_create)+30*24*3600 - (double)time(NULL))/(24*3600);

        printf("%d", razn);

        if ((int)razn<=4 and (int)razn>0)
        {
            sprintf(Text, this->msg->_(packet->UCID, "BankDialog9"), razn);
            SendMTC(packet->UCID, Text);
        }

        if ((int)razn==0)
        {
            sprintf(Text, this->msg->_(packet->UCID, "BankDialog10"));
            SendMTC(packet->UCID, Text);
            SendMTC(packet->UCID, Text);
            SendMTC(packet->UCID, Text);
        }

        if ((int)razn<0)
        {
            credit_penalty(packet->UCID);
        }
    }

    players[packet->UCID].ReadTrue = true;

    /** credit **/

    BtnCash(packet->UCID);

    return ;
}

void RCBank::credit_penalty (byte UCID)
{
    char Text[128];
    double razn=((double)(players[UCID].Date_create)+30*24*3600 - (double)time(NULL))/(24*3600);
    int c = (int)abs((ceil(razn)));
    sprintf(Text, msg->_(UCID, "BankDialog11"));
    SendMTC(UCID, Text);
    sprintf(Text, msg->_(UCID, "BankDialog12"), players[UCID].Credit*13/10);
    SendMTC(UCID, Text);

    /*int ost = players[UCID].Cash - ((players[UCID].Credit) * 13 / 10 + 1000 * c);
    if (ost<0)
    {
    	//денег на счету недостаточно, забираем автомобили
    }*/

    RemCash(UCID, ((players[UCID].Credit) * 13 / 10 + 1000 * c)); //отбираем бабки у игрока
    AddToBank((players[UCID].Credit) * 13 / 10 + 1000 * c); //сдаем в банк
    players[UCID].Credit = 0;
    players[UCID].Date_create = 0;
    bank_save(UCID); //сохраняемся
}

void RCBank::InsimNPL(struct IS_NPL* packet)
{
    PLIDtoUCID[packet->PLID] = packet->UCID;

    if (players[packet->UCID].Cash > 5000000)
    {
        players[packet->UCID].Cash = 5000000;
    }
}

void RCBank::InsimPLP(struct IS_PLP* packet)
{
    PLIDtoUCID.erase(packet->PLID);
}

void RCBank::InsimPLL(struct IS_PLL* packet)
{
    PLIDtoUCID.erase(packet->PLID);
}

void RCBank::InsimCNL(struct IS_CNL* packet)
{
    if (players[packet->UCID].ReadTrue)
        bank_save(packet->UCID);
    players.erase(packet->UCID);
}

void RCBank::bank_save (byte UCID)
{
    char query[128];
    sprintf(query, "UPDATE bank SET cash = %f WHERE username='%s'", players[UCID].Cash, players[UCID].UName);

    if (mysql_ping(dbconn) != 0)
    {
        printf("Bank Error: connection with MySQL server was lost\n");
    }

    if (mysql_query(dbconn, query) != 0)
    {
        printf("Bank Error: MySQL Query Save\n");
    }

    //printf("Bank Log: Affected rows = %d\n", mysql_affected_rows(dbconn));

    /* Credit */
    sprintf(query, "UPDATE bank_credits SET cash = %d, date_create = %d WHERE username='%s'", players[UCID].Credit, players[UCID].Date_create, players[UCID].UName);

    if (mysql_ping(dbconn) != 0)
    {
        printf("Credit Error: connection with MySQL server was lost\n");
    }

    if (mysql_query(dbconn, query) != 0)
    {
        printf("Credit Error: MySQL Query Save\n");
    }

    //printf("Credit Log: Affected rows = %d\n", mysql_affected_rows(dbconn));

    /* Deposit */
    sprintf(query, "UPDATE bank_deposits SET cash = %d, date_create = %d WHERE username='%s'", players[UCID].Deposit, players[UCID].Dep_Date_create, players[UCID].UName);

    if (mysql_ping(dbconn) != 0)
    {
        printf("Deposit Error: connection with MySQL server was lost\n");
    }

    if (mysql_query(dbconn, query) != 0)
    {
        printf("Deposit Error: MySQL Query Save\n");
    }

    //printf("Deposit Log: Affected rows = %d\n", mysql_affected_rows(dbconn));

    /* Capital */
    sprintf(query, "UPDATE bank SET cash = %f WHERE username='_RC_Bank_Capital_'", BankFond);

    if (mysql_ping(dbconn) != 0)
    {
        printf("Bank Error: connection with MySQL server was lost\n");
    }

    if (mysql_query(dbconn, query) != 0)
    {
        printf("Bank Error: MySQL Query Save\n");
    }

    //printf("Capital Log: Affected rows = %d\n", mysql_affected_rows(dbconn));
}

void RCBank::InsimCPR(struct IS_CPR* packet)
{
    strcpy(players[packet->UCID].PName, packet->PName);
}

void RCBank::InsimMSO(struct IS_MSO* packet)
{
    char Message[128], Text[128];
    strcpy(Message, packet->Msg + ((unsigned char)packet->TextStart));

    if (strncmp(Message, "!bnk", strlen("!bnk")) == 0 and (strcmp(players[packet->UCID].UName, "denis-takumi") == 0 || strcmp(players[packet->UCID].UName, "Lexanom") == 0))
    {
        sprintf(Text, "^5| ^7^CБаланс банка: %0.0f ^3RUR", BankFond);
        SendMTC(packet->UCID, Text);
    }

    if (players[packet->UCID].InZone)
    {
        /** Кредиты **/
        //Доступная сумма кредита:
        int cr = dl->GetLVL(packet->UCID) * 10000;

        if (cr>500000)
        {
            cr = 500000;
        }

        if (strncmp(Message, "!credit info", strlen("!credit info")) == 0)
        {
            if (dl->GetLVL(packet->UCID) < 5)
            {
                SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog13"));
                return;
            }
            strtok (Message, " ");
            strtok (NULL, " ");
            int summ = atoi(strtok (NULL, " "));
            if (summ <= 0)
            {
                summ = cr;
            }
            else if (summ < cr / 5 or summ > cr)
            {
                sprintf(Text, msg->_(packet->UCID, "BankDialog14"), cr/5, cr);
                SendMTC(packet->UCID, Text);
                return;
            }
            SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog15"));
            if (players[ packet->UCID ].Date_create!=0)
            {
                struct tm * timeinfo;
                char DateCreate [80];
                timeinfo = localtime (&players[packet->UCID].Date_create);
                strftime (DateCreate, 80, "%d.%m.%Y", timeinfo);

                double razn=((double)(players[packet->UCID].Date_create)+30*24*3600 - (double)time(NULL))/(24*3600);

                sprintf(Text, msg->_(packet->UCID, "BankDialog16"), players[ packet->UCID ].Credit, DateCreate);
                SendMTC( packet->UCID , Text);
                sprintf( Text, msg->_(packet->UCID, "BankDialog17"), players[ packet->UCID ].Credit * 13 / 10 );
                SendMTC( packet->UCID , Text);
                sprintf(Text, msg->_(packet->UCID, "BankDialog18"), razn);
                SendMTC(packet->UCID, Text);
                return;
            }
            sprintf(Text, msg->_(packet->UCID, "BankDialog19"), cr/5, cr);
            SendMTC( packet->UCID , Text);
            SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog20"));
            SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog21"));
            sprintf( Text, msg->_(packet->UCID, "BankDialog22"), summ );
            SendMTC( packet->UCID , Text);
            sprintf( Text, msg->_(packet->UCID, "BankDialog23"), summ * 13 / 10 );
            SendMTC( packet->UCID , Text);
            return;
        }
        if (strncmp(Message, "!credit", strlen("!credit")) == 0)
        {
            if (players[packet->UCID].Dep_Date_create!=0)
            {
                SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog24"));
                return;
            }

            if (dl->GetLVL(packet->UCID) < 5)
            {
                SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog25"));
                return;
            }

            if (players[packet->UCID].Date_create != 0)
            {
                SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog26"));
                return;
            }
            strtok (Message, " ");
            int summ = atoi(strtok (NULL, " "));
            if (summ < cr / 5 or summ > cr)
            {
                sprintf( Text , msg->_(packet->UCID, "BankDialog27"), cr/5, cr);
                SendMTC( packet->UCID , Text);
                return;
            }

            if (players[packet->UCID].Cash>(summ*3/2))
            {
                SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog28"));
                SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog29"));
                return;
            }

            if (players[packet->UCID].Cash <= -50000)
            {
                SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog30"));
                SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog31"));
                return;
            }
            //выдаем кредит
            sprintf(Text, msg->_(packet->UCID, "BankDialog32"), summ );
            SendMTC( packet->UCID , Text );
            players[ packet->UCID ].Credit = summ;
            players[ packet->UCID ].Date_create = time(NULL);
            RemFrBank(summ); //берем бабки из банка
            AddCash(packet->UCID, summ, false); //выдаем бабки игроку
            bank_save(packet->UCID); //сохраняемся
        }
        if (strncmp(Message, "!repay", strlen("!repay")) == 0)
        {
            if (players[packet->UCID].Date_create == 0)
            {
                SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog33"));
                return;
            }

            if (players[packet->UCID].Cash<players[packet->UCID].Credit*13/10)
            {
                SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog34"));
                return;
            }

            SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog35"));
            RemCash( packet->UCID, players[ packet->UCID ].Credit * 13 / 10 ); //отбираем бабки у игрока
            AddToBank( players[ packet->UCID ].Credit * 13 / 10 ); //сдаем в банк
            players[ packet->UCID ].Credit = 0;
            players[ packet->UCID ].Date_create = 0;
            bank_save( packet->UCID ); //сохраняемся
        }

        /** Вклады **/

        //Доступная сумма вклада:
        int dr = dl->GetLVL(packet->UCID) * 10000;
        if (dr>500000)
        {
            dr = 500000;
        }
        if (strncmp(Message, "!deposit info", strlen("!deposit info")) == 0)
        {
            if (dl->GetLVL(packet->UCID) < 20)
            {
                SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog36"));
                return;
            }

            strtok (Message, " ");
            strtok (NULL, " ");
            int summ = atoi(strtok (NULL, " "));

            if (summ <= 0)
            {
                summ = dr;
            }
            else if (summ > dr)
            {
                sprintf(Text, msg->_(packet->UCID, "BankDialog37"), dr);
                SendMTC(packet->UCID, Text);
                return;
            }

            SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog38"));

            if (players[packet->UCID].Dep_Date_create!=0)
            {
                struct tm * timeinfo;
                char DateCreate [80];
                timeinfo = localtime (&players[packet->UCID].Dep_Date_create);
                strftime (DateCreate, 80, "%d.%m.%Y", timeinfo);

                double razn=((double)(players[ packet->UCID ].Dep_Date_create)+30*24*3600 - (double)time(NULL))/(24*3600);
                sprintf(Text, msg->_(packet->UCID, "BankDialog39"), players[ packet->UCID ].Deposit, DateCreate);
                SendMTC( packet->UCID , Text);

                if (razn<=0)
                {
                    sprintf( Text, msg->_(packet->UCID, "BankDialog40"), players[ packet->UCID ].Deposit + players[ packet->UCID ].Deposit * 5/1000 * 30, 0 );
                    SendMTC( packet->UCID , Text);
                    sprintf(Text, msg->_(packet->UCID, "BankDialog41"));
                    SendMTC(packet->UCID, Text);
                    return;
                }

                sprintf( Text, msg->_(packet->UCID, "BankDialog42"), players[ packet->UCID ].Deposit + players[ packet->UCID ].Deposit * 5/1000 * (int)(30-razn), players[ packet->UCID ].Deposit * 5/1000 );
                SendMTC( packet->UCID , Text);
                sprintf(Text, msg->_(packet->UCID, "BankDialog43"), razn);
                SendMTC(packet->UCID, Text);
                return;
            }

            sprintf(Text, msg->_(packet->UCID, "BankDialog44"), dr);
            SendMTC( packet->UCID , Text);
            SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog45"));
            sprintf( Text, msg->_(packet->UCID, "BankDialog46"), summ );
            SendMTC( packet->UCID , Text);
            sprintf( Text, msg->_(packet->UCID, "BankDialog47"), summ * 115 / 100, summ * 15 / 3000 );
            SendMTC( packet->UCID , Text);
            sprintf( Text, msg->_(packet->UCID, "BankDialog48"), summ );
            SendMTC( packet->UCID , Text);
            return;
        }
        if (strncmp(Message, "!deposit", strlen("!deposit")) == 0)
        {
            if (players[packet->UCID].Date_create!=0)
            {
                SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog49"));
                return;
            }

            if (dl->GetLVL(packet->UCID) < 5)
            {
                SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog50"));
                return;
            }

            if (players[packet->UCID].Dep_Date_create != 0)
            {
                SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog51"));
                return;
            }

            strtok (Message, " ");
            int summ = atoi(strtok (NULL, " "));

            if (summ > dr or summ==0)
            {
                sprintf(Text, msg->_(packet->UCID, "BankDialog52"), dr);
                SendMTC(packet->UCID, Text);
                return;
            }

            if (players[packet->UCID].Cash<=(summ))
            {
                SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog53"));
                return;
            }

            //создаем вклад
            sprintf(Text, msg->_(packet->UCID, "BankDialog54"), summ );
            SendMTC( packet->UCID , Text );
            players[ packet->UCID ].Deposit = summ;
            players[ packet->UCID ].Dep_Date_create = time(NULL);
            RemCash( packet->UCID , summ); //снимаем у игрока
            AddToBank(summ); //кладем в банк
            bank_save(packet->UCID); //сохраняемся
        }
        if (strncmp(Message, "!withdraw", strlen("!withdraw")) == 0)
        {
            if (players[packet->UCID].Dep_Date_create==0)
            {
                SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog55"));
                return;
            }

            strtok (Message, " ");
            double razn=((double)(players[packet->UCID].Dep_Date_create)+30*24*3600 - (double)time(NULL))/(24*3600);
            int dtt;
            char * s;
            s = strtok(NULL, " ");

            if (s != NULL)
            {
                if (strncmp(s, "yes", strlen("yes")) == 0)
                {
                    sprintf(Text, msg->_(packet->UCID, "BankDialog56"));
                    SendMTC(packet->UCID, Text);
                    dtt = players[packet->UCID].Deposit + players[packet->UCID].Deposit * 5/1000 * (int)(30-razn);
                    sprintf(Text, msg->_(packet->UCID, "BankDialog57"), dtt*5/100);
                    SendMTC(packet->UCID, Text);
                    dtt = dtt - dtt*5/100;
                    RemFrBank(dtt); //берем из банка
                    AddCash(packet->UCID, dtt, true); //отдаем игроку
                    players[packet->UCID].Deposit = 0;
                    players[packet->UCID].Dep_Date_create = 0;
                    bank_save(packet->UCID); //сохраняемся
                    return;
                }
            }

            if (razn > 14)
            {
                SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog58"));
                SendMTC( packet->UCID , msg->_(packet->UCID, "BankDialog59"));
                return;
            }

            sprintf(Text, msg->_(packet->UCID, "BankDialog60"));
            SendMTC(packet->UCID, Text);

            if (razn<=0)
            {
                dtt = players[packet->UCID].Deposit + players[packet->UCID].Deposit * 5/1000 * 30;
            }
            else
            {
                dtt = players[packet->UCID].Deposit + players[packet->UCID].Deposit * 5/1000 * (int)(30-razn);
            }

            RemFrBank(dtt); //берем из банка
            AddCash(packet->UCID, dtt, true); //отдаем игроку
            players[packet->UCID].Deposit = 0;
            players[packet->UCID].Dep_Date_create = 0;
            bank_save(packet->UCID); //сохраняемся
        }

    }
}

void RCBank::InsimMCI(struct IS_MCI* pack_mci)
{
    for (int i = 0; i < pack_mci->NumC; i++)
    {
        byte UCID = PLIDtoUCID[pack_mci->Info[i].PLID];

        //баланс игрока
        BtnCash(UCID);

        //зона банк
        int X = pack_mci->Info[i].X/65536;
        int Y = pack_mci->Info[i].Y/65536;
        if (Check_Pos(TrackInf.BankCount, TrackInf.XBank, TrackInf.YBank, X, Y))
        {
            if (!players[UCID].InZone)
            {
                players[UCID].InZone=true;

                SendMTC(UCID, msg->_(UCID, "BankDialog1"));
                SendMTC(UCID, msg->_(UCID, "BankDialog2"));
                SendMTC(UCID, msg->_(UCID, "BankDialog3"));
                SendMTC(UCID, msg->_(UCID, "BankDialog4"));
                SendMTC(UCID, msg->_(UCID, "BankDialog5"));
                SendMTC(UCID, msg->_(UCID, "BankDialog6"));
                SendMTC(UCID, msg->_(UCID, "BankDialog7"));
                SendMTC(UCID, msg->_(UCID, "BankDialog8"));
            }
        }
        else if (players[UCID].InZone)
            players[UCID].InZone = false;
    }
}

void RCBank::BtnCash (byte UCID)
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
    pack.ClickID = 66;
    pack.L = 70;
    pack.T = 1;
    pack.W = 15;
    pack.H = 4;

    char cash[10];
    sprintf(cash, "%d", (int)players[UCID].Cash);

    if (players[UCID].Cash > 0)
        strcpy(pack.Text, "^2");
    else
        strcpy(pack.Text, "^1");

    strcat(pack.Text, cash);
    strcat(pack.Text, "^7 RUR");
    insim->send_packet(&pack);

    if (strcmp(players[UCID].UName, "denis-takumi") == 0)
    {
        pack.ClickID = 167;
        pack.L = 50;
        pack.T = 1;
        pack.W = 20;
        pack.H = 4;

        char cash[10];
        sprintf(cash, "%.0f", BankFond);

        if (BankFond > 0)
            strcpy(pack.Text, "^2");
        else
            strcpy(pack.Text, "^1");

        strcat(pack.Text, cash);
        strcat(pack.Text, "^7 RUR");
        insim->send_packet(&pack);
    }
}

void RCBank::readconfig(const char *Track)
{
    cout << "RCBank::readconfig\n" ;
    char file[MAX_PATH];
    sprintf(file, "%s\\data\\RCBank\\tracks\\%s.txt", RootDir, Track);
    FILE *fff = fopen(file, "r");
    if (fff == nullptr)
    {
        printf ("RCBank: Can't find\n%s", file);
        return;
    }

    fclose(fff);
    ifstream readf (file, ios::in);

    while (readf.good())
    {
        char str[128];
        readf.getline(str, 128);
        TrackInf.BankCount = atoi(str);

        for (int i=0; i<TrackInf.BankCount; i++)
        {
            readf.getline(str, 128);
            char * X;
            char * Y;
            X = strtok (str, ";");
            Y = strtok (NULL, ";");
            TrackInf.XBank[i] = atoi(X);
            TrackInf.YBank[i] = atoi(Y);
        }
    } //while readf.good()
    readf.close();

    char query[128];
    sprintf(query, "SELECT cash FROM bank WHERE username='_RC_Bank_Capital_' LIMIT 1;");

    if (mysql_ping(dbconn) != 0)
    {
        printf("Error: connection with MySQL server was lost\n");
    }

    if (mysql_query(dbconn, query) != 0)
    {
        printf("Error: MySQL Query\n");
    }

    dbres = mysql_store_result(dbconn);
    if (dbres == NULL)
    {
        printf("Error: can't get the result description\n");
    }

    if (mysql_num_rows(dbres) > 0)
    {
        dbrow = mysql_fetch_row(dbres);
        BankFond = atof(dbrow[0]);
    }
    else
    {
        BankFond = 0;
    }

    mysql_free_result(dbres);
}
