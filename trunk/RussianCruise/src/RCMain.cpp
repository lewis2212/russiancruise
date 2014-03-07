#include "RCMain.h"
#include <version.h>
#include "tools.h"

GlobalInfo *ginfo;


#ifdef _RC_PIZZA_H
RCPizza *pizza;
#endif

RCMessage *msg;

#ifdef _RC_ENERGY_H
RCEnergy *nrg;
#endif

#ifdef _RC_BANK_H
RCBank *bank;
#endif

#ifdef _RC_LEVEL_H
RCDL *dl;
#endif

#ifdef _RC_CHEAT_H
RCAntCheat  *antcht;
#endif

#ifdef _RC_STREET_H
RCStreet  *street;
#endif

#ifdef _RC_LIGHT_H
RCLight *lgh;
#endif

#ifdef _RC_TAXI_H
RCTaxi  *taxi;
#endif

#ifdef _RC_POLICE_H
RCPolice *police;
#endif // _RC_POLICE_H

#ifdef _RC_ROADSIGN
RCRoadSign *RoadSign;
#endif // _RC_ROADSIGN

#ifdef _RC_QUEST_H
RCQuest	*quest;
#endif // _RC_QUEST_H

#ifdef _RC_AUTOSCHOOL_H
RCAutoschool *school;
#endif // _RC_AUTOSCHOOL_H

void CreateClasses()
{
    msg = new RCMessage(RootDir);
    bank = new RCBank(RootDir);
    classes.push_back( bank );

#ifdef _RC_ENERGY_H
    nrg = new RCEnergy(RootDir);
    classes.push_back( nrg );
#endif

#ifdef _RC_LEVEL_H
    dl = new RCDL(RootDir);
    classes.push_back( dl );
#endif

#ifdef _RC_CHEAT_H
    antcht = new RCAntCheat(RootDir);
    classes.push_back( antcht );
#endif

#ifdef _RC_STREET_H
    street = new RCStreet(RootDir);
    classes.push_back( street );
#endif

#ifdef _RC_LIGHT_H
    lgh = new RCLight(RootDir);
    classes.push_back( lgh );
#endif

#ifdef _RC_PIZZA_H
    pizza = new RCPizza(RootDir);
    classes.push_back( pizza );
#endif

#ifdef _RC_TAXI_H
    taxi = new RCTaxi(RootDir);
    classes.push_back( taxi );
#endif

#ifdef _RC_POLICE_H
    police = new RCPolice(RootDir);
    classes.push_back( police );
#endif // _RC_POLICE_H

#ifdef _RC_ROADSIGN
    RoadSign = new RCRoadSign(RootDir);
    classes.push_back( RoadSign );
#endif // _RC_ROADSIGN

#ifdef _RC_QUEST_H
    quest = new RCQuest(RootDir);
    classes.push_back( quest );
#endif // _RC_QUEST_H

#ifdef _RC_AUTOSCHOOL_H
	school = new RCAutoschool(RootDir);
	classes.push_back( school );
#endif // _RC_AUTOSCHOOL_H

}

void InitClasses()
{
    msg->init(rcMaindbConn, insim);

#ifdef _RC_BANK_H
    bank->init(rcMaindbConn, insim, msg, dl);
#endif // _RC_BANK_H

#ifdef _RC_ENERGY_H
    nrg->init(rcMaindbConn, insim, msg, bank);
#endif

#ifdef _RC_LEVEL_H
    dl->init(rcMaindbConn, insim, msg);
#endif

#ifdef _RC_CHEAT_H
    antcht->init(rcMaindbConn, insim, msg);
#endif

#ifdef _RC_STREET_H
    street->init(rcMaindbConn, insim, msg);
#endif

#ifdef _RC_LIGHT_H
    lgh->init(rcMaindbConn, insim, msg, dl);
#endif

#ifdef _RC_POLICE_H
    police->init(rcMaindbConn, insim, msg, bank, dl, street, nrg, lgh);
#endif // _RC_POLICE_H

#ifdef _RC_PIZZA_H
    pizza->init(rcMaindbConn, insim, msg, bank, nrg, dl, street);
#endif

#ifdef _RC_TAXI_H
    taxi->init(rcMaindbConn, insim, msg, bank, dl, street, police, lgh);
#endif

#ifdef _RC_ROADSIGN
    RoadSign->Init(rcMaindbConn, insim, msg, lgh);
#endif // _RC_ROADSIGN

#ifdef _RC_QUEST_H
    quest->init(rcMaindbConn, insim);
#endif // _RC_QUEST_H

#ifdef _RC_AUTOSCHOOL_H
	school->init(rcMaindbConn, insim, msg);
#endif // _RC_AUTOSCHOOL_H

}

int GetCarID(const char *CarName)
{
    if (strlen(CarName)!=3)
        return 0;

    for (int i=0; i< MAX_CARS; i++)
    {
        if (strcmp(CarName, ginfo->car[i].car)==0)
            return ginfo->car[i].id;
    }

    return 0;
}

cars AddCar(int id = 0, const char *car = "UF1", int cash =0, int sell = 0, unsigned PLC = 0)
{
    cars c;
    c.id = id;
    strcpy(c.car, car);
    c.cash = cash;
    c.sell = sell;
    c.PLC = PLC;
    return c;
}



void read_words()
{
    char file[255];
    sprintf(file, "%smisc\\words.txt", RootDir);

    FILE *fff = fopen(file, "r");
    if (fff == nullptr)
    {
        cout << "Can't find " << file << endl;
        return;
    }
    fclose(fff);

    ifstream readf (file, ios::in);
    int i=0;
    while (readf.good())
    {
        char str[32];
        readf.getline(str, 32);

        if (strlen(str) > 0)
        {
            strncpy(ginfo->Words[i], str, strlen(str));
            i++;
        }
    }
    ginfo->WordsCount = i;

}

void save_user_cars (byte UCID)
{
    char sql[128];

    if ( mysql_ping( rcMaindbConn ) != 0 )
        printf("Bank Error: connection with MySQL server was lost\n");

    for (int i = 0; i < MAX_CARS; i++)
    {
        if ( strlen( players[UCID].cars[i].car ) > 0 )
        {
            sprintf(sql, "UPDATE garage SET tuning = %d, dist = %f WHERE car = '%s' AND username = '%s';", players[UCID].cars[i].tuning , players[UCID].cars[i].dist , players[UCID].cars[i].car , players[UCID].UName );
            if ( mysql_query( rcMaindbConn , sql) != 0 )
                printf("Bank Error: MySQL Query Save\n");
        }
    }
}



void read_user_cars(byte UCID)
{
    char kickCmd[64], msg[96];
    sprintf(kickCmd, "/kick %s",players[UCID].UName);
    sprintf(msg, "^1RC:Core ERROR - BAD USER");
    char query[128];
    sprintf(query, "SELECT car, tuning, dist FROM garage WHERE username='%s';", players[UCID].UName);

    if ( mysql_ping( rcMaindbConn ) != 0 )
    {
        printf("Error: connection with MySQL server was lost\n");
        SendMST(msg);
        SendMST(kickCmd);
        return;
    }

    if ( mysql_query( rcMaindbConn , query) != 0 )
    {
        printf("Error: MySQL Query\n");
        SendMST(msg);
        SendMST(kickCmd);
        return;
    }

    rcMainRes = mysql_store_result(rcMaindbConn);
    if (rcMainRes == NULL)
    {
        printf("Error: can't get the result description\n");
        SendMST(msg);
        SendMST(kickCmd);
        return;
    }

    if ( mysql_num_rows( rcMainRes ) > 0 )
    {
        int i = 0;
        while ( ( rcMainRow = mysql_fetch_row( rcMainRes ) ) )
        {
            strcpy(players[UCID].cars[i].car, rcMainRow[0]);
            players[UCID].cars[i].tuning = atoi( rcMainRow[1] );
            players[UCID].cars[i].dist = atof( rcMainRow[2] );
            /** map<> **/
            players[UCID].PLC += carMap[ rcMainRow[0] ].PLC;
            i++;
        }
    }
    else
    {
        RCBaseClass::CCText("^7Can't find " + (string)players[UCID].UName + ", create new user");

        sprintf(query, "INSERT INTO garage (username, car ) VALUES ('%s' , 'UF1');", players[UCID].UName);

        if ( mysql_ping( rcMaindbConn ) != 0 )
            printf("Error: connection with MySQL server was lost\n");

        if ( mysql_query( rcMaindbConn , query) != 0 )
            printf("Error: MySQL Query\n");

        strcpy(players[UCID].cars[0].car, "UF1");
        players[UCID].cars[0].tuning = 0;
        players[UCID].cars[0].dist = 0;

        players[UCID].PLC += carMap[ "UF1" ].PLC;

        save_user_cars(UCID);
    }
    mysql_free_result( rcMainRes );
    SendPLC(UCID, players[UCID].PLC);
}

void save_car (byte UCID)
{

    for (int i=0; i<MAX_CARS; i++)
    {
        if (strcmp(players[UCID].CName, players[UCID].cars[i].car)==0)
        {
            players[UCID].cars[i].tuning = players[UCID].CTune;
            players[UCID].cars[i].dist = (int)players[UCID].Distance;
            break;
        }
    }

}

void Save(byte UCID)
{
    if(players.find(UCID) == players.end())
        return;

	save_car(UCID);
	save_user_cars(UCID);

	for( cl = classes.begin(); cl != classes.end(); ++cl )
	{
		(*cl)->Save( UCID );
	}

	SendMTC(UCID, msg->_( UCID, "3000" ));
}

void SaveAll()
{
	for(auto& pl: players)
	{
		Save(pl.first);
	}
	RCBaseClass::CCText("^2DATA SAVED");
}

void help_cmds (byte UCID, int h_type)
{
    if (h_type == 1)
    {
        SendMTC(UCID, msg->_(UCID, "Help1"));
        SendMTC(UCID, RCBaseClass::StringFormat(msg->_(UCID, "Help2"), msg->GetLangList().c_str()));
        SendMTC(UCID, msg->_(UCID, "Help3"));
        SendMTC(UCID, msg->_(UCID, "Help4"));
        SendMTC(UCID, msg->_(UCID, "Help5"));
        SendMTC(UCID, msg->_(UCID, "Help6"));
        SendMTC(UCID, msg->_(UCID, "Help7"));
        SendMTC(UCID, msg->_(UCID, "Help8"));
        SendMTC(UCID, msg->_(UCID, "Help9"));
        SendMTC(UCID, msg->_(UCID, "Help10"));
        SendMTC(UCID, msg->_(UCID, "Help11"));
        SendMTC(UCID, msg->_(UCID, "Help12"));
        SendMTC(UCID, msg->_(UCID, "Help13"));
    }
    if (h_type == 2)
    {
        SendMTC(UCID, msg->_( UCID, "3100" ));
        for ( int i=0; i<MAX_CARS; i++)
        {
            if (strlen(players[UCID].cars[i].car)>0)
            {
                int     tune;
                char    Text[64];
                tune = players[UCID].cars[i].tuning;

                char Tun[30];

                int tune2 = 45;
                if (tune&1)
                {
                    strcpy( Tun, "^2E");
                    tune2 -= 15;
                }
                else
                {
                    strcpy( Tun, "^1E");
                }

                if (tune&2)
                {
                    strcat( Tun, " ^2T");
                    tune2 -= 20;
                }
                else
                {
                    strcat( Tun, " ^1T");
                }

                if (tune&4)
                {
                    strcat( Tun, " ^2W");
                    tune2 -= 10;
                }
                else
                {
                    strcat( Tun, " ^1W");
                }

                sprintf(Text, "^1| ^C^2%s ^3%4.0f ^7Km (%s^7)(^3%d%%^7)", players[UCID].cars[i].car, players[UCID].cars[i].dist/1000, Tun, tune2);
                SendMTC(UCID, Text);
            }
        }
    }
}

void btn_info (byte UCID, int b_type)
{
    char abcout_text[10][100];
    sprintf(abcout_text[0], "^7RUSSIAN CRUISE  %s", IS_PRODUCT_NAME);
    strncpy(abcout_text[1], "^C^7Developers: Denis Kostin, Aleksandr Mochalov", 99);
    strncpy(abcout_text[2], "^C^7Jabber: denis-kostin@jabber.ru", 99);
    strncpy(abcout_text[3], "^C^7Jabber conference: lfs@conference.jabber.ru", 99);
    strncpy(abcout_text[4], "^7", 99);
    strncpy(abcout_text[5], "^C^7More information", 99);
    strncpy(abcout_text[6], "^C^7http://vk.com/russiancruise", 99);
    strncpy(abcout_text[7], "^7", 99);
    strncpy(abcout_text[8], "^C^7Thanks:", 99);
    strncpy(abcout_text[9], "^C^3repeat, nose, R.Ratzinger", 99);

    char Text[128];

    byte c;
    if (b_type == 1) c=MAX_CARS/2;				//количество строк для 1 вкладки
#ifdef _RC_POLICE_H
    if (b_type == 2) c=police->GetFineCount();	//количество строк для 2 вкладки
#endif
    if (b_type == 3) c=sizeof(abcout_text)/100;	//да, да, ты угадал
    byte
    id=134, 			//стартовый ид кнопок
    l=100, t=90,		//центр поля
    hButton=5, 			//высота одной строки
    w=100, 				//ширина поля
    h=16+c*hButton; 	//высота поля

    SendButton(255, UCID, 128, l-w/2, t-h/2, w, h+8, 32, "");                                   //фон
    id++;
    SendButton(255, UCID, 129, l-w/2, t-h/2, w, h+8, 32, "");
    id++;
    SendButton(254, UCID, 130, l-7, t-h/2+h+1, 14, 6, 16+8, "^2OK");                            //закрывашка
    id++;
    SendButton(255, UCID, id++, l-w/2, t-h/2, 25, 10, 3+64, "RUSSIAN CRUISE");                  //заголовок
    SendButton(255, UCID, id++, l-w/2+24, t-h/2+2, 20, 3, 5+64, IS_PRODUCT_NAME);               //версия

    SendButton(254, UCID, 131, l-w/2+1, t-h/2+9, 16, 6, 16+8, msg->_(UCID, "200"));	//вкладка раз
    id++;
    SendButton(254, UCID, 132, l-w/2+17, t-h/2+9, 16, 6, 16+8, msg->_(UCID, "201"));	//два
    id++;
    SendButton(254, UCID, 133, l-w/2+33, t-h/2+9, 16, 6, 16+8, msg->_(UCID, "202"));	//нутыпонел
    id++;

    if (b_type == 1)
        for (int i=0; i<MAX_CARS; i++)
        {
            if (ginfo->car[i].id == 0)
                continue;

            int plW=0, plH=1;

            if (i>=MAX_CARS/2)
            {
                plW=w/2-1;
                plH=MAX_CARS/2;
            }

            if (dl->GetLVL(UCID)>=(i-1)*5)
                sprintf(Text, "^3%d lvl ^2%s ^3(^2%d^3/%d)", (i-1)*5, ginfo->car[i].car, (int)ginfo->car[i].cash, (int)ginfo->car[i].sell);
            else
                sprintf(Text, "%d lvl %s (%d/%d)", (i-1)*5, ginfo->car[i].car, (int)ginfo->car[i].cash, (int)ginfo->car[i].sell);
            SendButton(255, UCID, id++, l-w/2+1+plW, t-h/2+16+hButton*(i-plH), w/2-1, hButton, 16+64, Text);
        }

#ifdef _RC_POLICE_H
    if (b_type == 2)
        for (int i=0; i<c; i++)
        {
            sprintf(Text, "^7ID = %02d. %s (^2%d RUR^7)", police->fines[i+1].id, police->GetFineName(UCID, i+1), police->fines[i+1].cash);

            SendButton(255, UCID, id++, l-w/2+1, t-h/2+16+hButton*i, w-2, hButton, 16+64, Text);
        }
#endif

    if (b_type == 3)
        for (int i=0; i<c; i++)
            SendButton(255, UCID, id++, l-w/2+1, t-h/2+16+hButton*i, w-2, hButton, 0, abcout_text[i]);

    for (int i=id; i<165; i++)
        SendBFN(UCID, i);
}



void btn_panel (byte UCID)
{
    /****************************/
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.BStyle = 1;
    pack.ClickID = 54;
    pack.BStyle = 32;
    pack.L = 85;
    pack.T = 1;
    pack.W = 15;
    pack.H = 8;

    if (players[UCID].Zone== 1)
        strcpy(pack.Text, msg->_(UCID, "PitSaveGood"));
    else if (players[UCID].Zone== 2)
        strcpy(pack.Text, msg->_( UCID, "401" ));
    else if (players[UCID].Zone== 3)
        strcpy(pack.Text, msg->_( UCID, "402" ));
    else if (players[UCID].Zone== 4)
        strcpy(pack.Text, msg->_( UCID, "404" ));
    else
        strcpy(pack.Text, msg->_(UCID, "PitSaveNotGood"));
    insim->send_packet(&pack);

    pack.ClickID = 55;
    pack.L = 70;
    pack.T = 5;
    pack.W = 15;
    pack.H = 4;
    sprintf(pack.Text, msg->_(UCID, "Dist" ), players[UCID].Distance/1000);
    insim->send_packet(&pack);
}

/****************************************/
void case_bfn ()
{
    struct IS_BFN *pack_bfn = (struct IS_BFN*)insim->get_packet();

	time_t now;
	time(&now);

	if ((now - players[pack_bfn->UCID].LastBFN) < 5)
	{
		//SendMTC(pack_bfn->UCID, "^1^CНельзя так часто жать кнопки");
		return;
	}

	players[pack_bfn->UCID].LastBFN = now;
	btn_info(pack_bfn->UCID, 1);
}

void case_btc ()
{
    struct IS_BTC *pack_btc = (struct IS_BTC*)insim->get_packet();

    SYSTEMTIME sm;
    GetLocalTime(&sm);
    char log[255];
    sprintf(log, "%slogs\\shop\\shop(%d.%d.%d).txt", RootDir, sm.wYear, sm.wMonth, sm.wDay);

    /** Пользователь кликнул по другому пользователю **/
    if (pack_btc->ClickID<=48)
    {
        ShowUsersList(pack_btc->UCID);

            struct IS_BTN pack_btn;
            memset(&pack_btn, 0, sizeof(struct IS_BTN));
            pack_btn.Size = sizeof(struct IS_BTN);
            pack_btn.Type = ISP_BTN;
            pack_btn.ReqI = pack_btc->ReqI;
            pack_btn.UCID = pack_btc->UCID;
            pack_btn.L = 23;


            if (players.size()>24)
                pack_btn.L += 22;

            pack_btn.T = 187;
#ifdef _RC_POLICE_H
            if (police->IsCop(pack_btc->UCID) and !police->IsCop(pack_btc->ReqI))
                pack_btn.T = 175;
#endif
            pack_btn.W = 24;
            pack_btn.H = 4;
            pack_btn.ClickID = 35;
            pack_btn.BStyle = 32 + 64;

            if (pack_btc->ReqI == pack_btc->UCID)
            {
                sprintf(pack_btn.Text, msg->_( pack_btc->UCID, "ItsYou" ));
                insim->send_packet(&pack_btn);
                pack_btn.BStyle = 7 + 16;
            }
            else
            {
                sprintf(pack_btn.Text, msg->_( pack_btc->UCID, "MsgPlFor" ), players[pack_btc->ReqI].PName);
#ifdef _RC_POLICE_H
                if (police->IsCop(pack_btc->UCID))
                    sprintf(pack_btn.Text, "^7^CДля %s ^8(%s^8) ^7:", players[pack_btc->UCID].PName, players[pack_btc->UCID].UName);
#endif
                insim->send_packet(&pack_btn);
                pack_btn.BStyle = 3 + 16 + ISB_CLICK;
            }

            pack_btn.TypeIn = 7;
            pack_btn.T += 4;
            pack_btn.ClickID = 36;
            strcpy(pack_btn.Text, msg->_( pack_btc->UCID, "1000" ));
            insim->send_packet(&pack_btn);

            pack_btn.TypeIn = 63;
            pack_btn.T += 4;
            pack_btn.ClickID = 37;
            strcpy(pack_btn.Text, msg->_( pack_btc->UCID, "1001" ));
            insim->send_packet(&pack_btn);

#ifdef _RC_POLICE_H
            // cop buttons
            if ( police->IsCop(pack_btc->UCID) and !police-> IsCop(pack_btc->ReqI) and pack_btc->ReqI != pack_btc->UCID)
            {
                pack_btn.TypeIn = 0;
                pack_btn.BStyle = 32 + 8 + 3;

                pack_btn.T += 4;
                pack_btn.ClickID = 38;
                strcpy(pack_btn.Text, msg->_( pack_btc->UCID, "FinesButton" ));
                insim->send_packet(&pack_btn);

                if (police->GetCopRank(pack_btc->UCID) == 1)
                    pack_btn.BStyle = 32 + 7;

                if (police->GetCopRank(pack_btc->UCID) == 2)
                    pack_btn.BStyle = 32 + 7;

                pack_btn.ReqI = pack_btc->ReqI;
                pack_btn.T += 4;
                pack_btn.ClickID = 40;
                strcpy(pack_btn.Text, msg->_( pack_btc->UCID, "1004" ));
                insim->send_packet(&pack_btn);

                if (police->GetCopRank(pack_btc->UCID) == 2)
                    pack_btn.BStyle = 32 + 8 + 3;

                pack_btn.T += 4;
                pack_btn.ClickID = 41;
                strcpy(pack_btn.Text, msg->_( pack_btc->UCID, "1005" ));
                insim->send_packet(&pack_btn);
            }
            else
            {
                SendBFN(pack_btc->UCID, 38);
                SendBFN(pack_btc->UCID, 39);
                SendBFN(pack_btc->UCID, 40);
                SendBFN(pack_btc->UCID, 41);
            }
#endif
    }

    /** Скрыть кнопки с пользователями **/
    if (pack_btc->ClickID==48)
    {
        for (int j=0; j<50; j++)
            SendBFN(pack_btc->UCID, j);
    }

    /**
    Информационные кнопки
    **/
    /*if (pack_btc->ClickID == 149)
    {
        players[i].bfn=0;
        for (int j=159; j>0; j--)
            SendBFN(pack_btc->UCID, j);
    }*/

    if (pack_btc->ReqI==254)
    {
        if (pack_btc->ClickID == 131)
            btn_info(pack_btc->UCID, 1);
        if (pack_btc->ClickID == 132)
            btn_info(pack_btc->UCID, 2);
        if (pack_btc->ClickID == 133)
            btn_info(pack_btc->UCID, 3);
    }


    /**
    Не помню. Возможно на удаление
    */
    if (pack_btc->ClickID == 200)
    {
        for (int j=0; j<5; j++)
            SendBFN(pack_btc->UCID, 200+j);
    }

}

void case_btt ()
{

    struct IS_BTT *pack_btt = (struct IS_BTT*)insim->get_packet();

    SYSTEMTIME sm;
    GetLocalTime(&sm);

    char send_c[255];
    sprintf(send_c, "%slogs\\sends\\send.txt", RootDir);
    char fine_c[255];
    sprintf(fine_c, "%slogs\\fines\\fine(%d.%d.%d).txt", RootDir, sm.wYear, sm.wMonth, sm.wDay);

    /**
    Пользователь передает деньги
    */
    if (pack_btt->ClickID==36)
    {
        if (atoi(pack_btt->Text) > 0)
        {
            if (bank->GetCash(pack_btt->UCID) > atoi(pack_btt->Text))
            {
                cout << players[pack_btt->UCID].UName << " send " << pack_btt->Text << " to "  << players[pack_btt->ReqI].UName << endl;

                char Msg[126];
                sprintf(Msg, msg->_(pack_btt->ReqI, "GetMoney" ), players[pack_btt->UCID].PName, atoi(pack_btt->Text));
                SendMTC(pack_btt->ReqI, Msg);
                sprintf(Msg, msg->_(pack_btt->UCID, "SendMoney" ), players[pack_btt->ReqI].PName, atoi(pack_btt->Text));
                SendMTC(pack_btt->UCID, Msg);

                bank->RemCash(pack_btt->UCID, atoi(pack_btt->Text));
                bank->AddCash(pack_btt->ReqI, atoi(pack_btt->Text), false);

                sprintf(Msg, "[%02d.%02d.%d, %02d:%02d] %s => %s (%s RUR)", sm.wDay, sm.wMonth, sm.wYear, sm.wHour, sm.wMinute, players[pack_btt->UCID].UName, players[pack_btt->ReqI].UName, pack_btt->Text);
                ofstream readf (send_c, ios::app);
                readf << Msg << endl;
                readf.close();
            }
            else
                SendMTC(pack_btt->UCID, msg->_( pack_btt->UCID, "1101" ));
        } // if atoi(pack_btt->Text) > 0

    }

    /**
    Пользователь передает сообщение
    */
    if (pack_btt->ClickID == 37)
    {
        if (strlen(pack_btt->Text) > 0)
        {

            char Msg[128];
            sprintf(Msg, msg->_( pack_btt->ReqI, "MsgFrom" ), players[pack_btt->UCID].PName, pack_btt->Text );
            SendMTC(pack_btt->ReqI, Msg);

            RCBaseClass::CCText("^1" + (string)players[pack_btt->UCID].UName + " ^7передал сообщение " + (string)players[pack_btt->ReqI].UName + ":");
            printf("%s\n", pack_btt->Text);
        }
    }

}


void case_cnl ()
{
    struct IS_CNL *pack_cnl = (struct IS_CNL*)insim->get_packet();

	RCBaseClass::CCText("<< ^9disconnected " + (string)players[pack_cnl->UCID].UName + " (" + (string)RCBaseClass::GetReason(pack_cnl->Reason) + ")");

	Save( pack_cnl->UCID );

	players.erase(pack_cnl->UCID);

}

void case_toc ()
{
    struct IS_TOC *pack_toc = (struct IS_TOC*)insim->get_packet();

	SendMTC(pack_toc->NewUCID, "^1Access Denine");
	SendMST("/spec " + string(players[pack_toc->NewUCID].UName));

}

void case_cpr ()
{
    struct IS_CPR *pack_cpr = (struct IS_CPR*)insim->get_packet();
	strcpy(players[pack_cpr->UCID].PName, pack_cpr->PName);
}

void case_mci ()
{
    struct IS_MCI *pack_mci = (struct IS_MCI*)insim->udp_get_packet();

    for (int i = 0; i < pack_mci->NumC; i++)
    {
        if(PLIDtoUCID.find(pack_mci->Info[i].PLID) == PLIDtoUCID.end())
            continue;

        byte UCID = PLIDtoUCID[pack_mci->Info[i].PLID];

        if(players.find(UCID) == players.end())
            continue;

        btn_panel(UCID);

        struct streets StreetInfo;
        street->CurentStreetInfo(&StreetInfo, UCID);

        int X = pack_mci->Info[i].X/65536;
        int Y = pack_mci->Info[i].Y/65536;

        int S = ((int)pack_mci->Info[i].Speed*360)/(32768);

        int LastX = players[UCID].Info.X/65536;
        int LastY = players[UCID].Info.Y/65536;

        if (LastX==0 and LastY==0)
        {
            LastX=X;
            LastY=Y;
        }
        float Dist = dl->Distance(X, Y, LastX, LastY);;

        if (Dist<50)
        {
            players[UCID].Distance += Dist;
            /** Bonus **/
            players[UCID].Bonus_dist += Dist;
        }

        if ( S > 30 and dl->GetLVL(UCID) < 20 )
        {
            if (S <= StreetInfo.SpeedLimit)
            {
                if ( dl->Islocked( UCID ) )
                    dl->Unlock( UCID );
                bank->AddCash(UCID, abs((int)Dist)/10, false);
                bank->RemFrBank(abs((int)Dist)/100);
            }
            else
            {
                if ( !( dl->Islocked( UCID ) ) )
                    dl->Lock( UCID );
            }
        }

        /** Bonus **/

        if (players[UCID].Bonus_dist > 5000)
        {
            players[UCID].Bonus_dist -= 5000;

            int bonus = 100+(50*(players[UCID].Bonus_count));
            players[UCID].Bonus_count +=1;

            bank->AddCash(UCID, bonus, false);
            bank->RemFrBank( bonus );

            char bonus_c[64];
            sprintf(bonus_c, msg->_( UCID, "1500" ), bonus);
            SendMTC(UCID, bonus_c);
        }

        /** Zones (PitSave, shop, etc) **/
        if (bank->InBank(UCID))
            players[UCID].Zone = 4;
        else if ( RCBaseClass::Check_Pos(ginfo->TrackInf.PitCount, ginfo->TrackInf.XPit, ginfo->TrackInf.YPit, X, Y))
            players[UCID].Zone = 1;
        else if ( RCBaseClass::Check_Pos(ginfo->TrackInf.ShopCount, ginfo->TrackInf.XShop, ginfo->TrackInf.YShop, X, Y))
        {
            if (players[UCID].Zone!=2)
            {
                players[UCID].Zone = 2;
                SendMTC(UCID, msg->_( UCID, "ShopDialog1" ));
                SendMTC(UCID, msg->_( UCID, "ShopDialog2" ));
                SendMTC(UCID, msg->_( UCID, "ShopDialog3" ));
                SendMTC(UCID, msg->_( UCID, "ShopDialog4" ));
                SendMTC(UCID, msg->_( UCID, "ShopDialog5" ));
            }
        }
        else
            players[UCID].Zone = 0;

        if (players[UCID].Svetofor == 1)
        {
            char TEST[64];
            sprintf(TEST, "X=%d Y=%d H=%d", X, Y, pack_mci->Info[i].Heading/182);
#ifdef _RC_PIZZA_H
            pizza->ButtonInfo(UCID, TEST);
#endif
        }
        memcpy(&players[UCID].Info, &pack_mci->Info[i], sizeof(CompCar));
    }
}


void case_mso ()
{
    struct IS_MSO *pack_mso = (struct IS_MSO*)insim->get_packet();

    //if (pack_mso->UCID == 0) return;

    if ( pack_mso->UserType != MSO_PREFIX )
        return;


    char Msg[128];
    strcpy(Msg, pack_mso->Msg + ((unsigned char)pack_mso->TextStart));


    //!help
    if ((strncmp(Msg, "!info", 5) == 0) or (strncmp(Msg, "!^Cинфо", 7) == 0))
    {
        for (int j=159; j>0; j--)
        {
            SendBFN(pack_mso->UCID, j);
        }
        btn_info(pack_mso->UCID, 1);
        return;
    }

    if ((strncmp(Msg, "!help", 5) == 0) or (strncmp(Msg, "!^Cпомощь", 9) == 0))
    {
        cout << players[pack_mso->UCID].UName << " send !help" << endl;
        help_cmds(pack_mso->UCID, 1);
        return;
    }
    if ((strncmp(Msg, "!cars", 5) == 0) or (strncmp(Msg, "!^Cмашины", 9) == 0))
    {
        cout << players[pack_mso->UCID].UName << " send !cars" << endl;
        help_cmds(pack_mso->UCID, 2);
        return;
    }

    //!save
    if (strncmp(Msg, "!save", 5) == 0)
    {
        time_t now;
        time(&now);

        if ((now - players[pack_mso->UCID].LastSave) < 5*3600)
        {
            SendMTC(pack_mso->UCID, "^1^CНельзя так часто сохраняться");
            return;
        }
        players[pack_mso->UCID].LastSave = now;

        cout << players[pack_mso->UCID].UName << " send !save" << endl;

        Save(pack_mso->UCID);
        return;
    }

    if ((strncmp(Msg, "!trans", 6) == 0 ) or (strncmp(Msg, "!^Cпередачи", 11) == 0))
    {
        char _[128];
        strcpy(_, Msg);

        char file[255];
        strcpy(file, RootDir);
        strcat(file, "logs\\sends\\send.txt");

        FILE *fff = fopen(file, "r");
        if (fff == nullptr)
        {
            cout << "Can't find " << file << endl;
            return;
        }
        fclose(fff);

        ifstream readf (file, ios::in);

        char * comand;
        char * user;
        comand = strtok(_, " ");
        user = strtok(NULL, " ");

        if (user and strlen(user) > 0)
        {
            while (readf.good())
            {
                char str[128];
                readf.getline(str, 128);
                if (strlen(str) > 0)
                {
                    if ((strstr(str, players[pack_mso->UCID].UName)) and (strstr(str, user)))
                    {
                        char Text[64];
                        strcpy(Text, "^1| ^C^7");
                        strncat(Text, str, 110);
                        SendMTC(pack_mso->UCID, Text);
                    }
                }
            }
        }
        else
        {
            int PreCount=0, PostCount=0;
            while (readf.good())
            {
                char str[128];
                readf.getline(str, 128);
                if (strlen(str) > 0 and strstr(str, players[pack_mso->UCID].UName))
                    PreCount++;
            }
            readf.close();

            SendMTC(pack_mso->UCID, "^1| ^3LAST TRANSFERS:");
            ifstream readf (file, ios::in);
            while (readf.good())
            {
                char str[128];
                readf.getline(str, 128);
                if (strlen(str) > 0 and strstr(str, players[pack_mso->UCID].UName))
                {
                    if (PostCount >= PreCount-15)
                    {
                        char Text[96];
                        strcpy(Text, "^1| ^C^7");
                        strncat(Text, str, 55);
                        SendMTC(pack_mso->UCID, Text);
                    }
                    PostCount++;
                }
            }
        }
        readf.close();
        return;
    }

    //!show
    if (strncmp(Msg, "!show", 5) == 0)
    {
        char Text[64];

        sprintf(Text, "/msg ^1------------------------------");
        SendMST(Text);

        sprintf(Text, "/msg ^7 %s", players[pack_mso->UCID].PName);
        SendMST(Text);

        sprintf(Text, "/msg ^7Cash: ^1%d", bank->GetCash(pack_mso->UCID));
        SendMST(Text);
#ifdef _RC_LEVEL_H
        sprintf(Text, "/msg ^7Drive Level: ^1%d", dl->GetLVL(pack_mso->UCID));
        SendMST(Text);

        float nextlvl = (pow(dl->GetLVL(pack_mso->UCID), 2)*0.5+100)*1000;

        int prog = int(dl->GetSkill(pack_mso->UCID)/nextlvl*100);

        sprintf(Text, "/msg ^7Drive Skill: ^1%d%s", prog, "%");
        SendMST(Text);
#endif
        for ( int j=0; j<MAX_CARS; j++)
        {
            if (strlen(players[pack_mso->UCID].cars[j].car)>0)
            {
                int     tune;
                char    Text[64];

                tune = players[pack_mso->UCID].cars[j].tuning;

                char Tun[30];

                int tune2 = 45;
                if (tune&1)
                {
                    strcpy( Tun, "^2E");
                    tune2 -= 15;
                }
                else
                {
                    strcpy( Tun, "^1E");
                }

                if (tune&2)
                {
                    strcat( Tun, " ^2T");
                    tune2 -= 20;
                }
                else
                {
                    strcat( Tun, " ^1T");
                }

                if (tune&4)
                {
                    strcat( Tun, " ^2W");
                    tune2 -= 10;
                }
                else
                {
                    strcat( Tun, " ^1W");
                }

                sprintf(Text, "/msg ^2%s ^3%4.0f ^7Km (%s^7)(^3%d%%^7)", players[pack_mso->UCID].cars[j].car, players[pack_mso->UCID].cars[j].dist/1000, Tun, tune2);
                SendMST(Text);
            }
        }

        sprintf(Text, "/msg ^1------------------------------");
        SendMST(Text);
        return;
    }
    //!tunning

    if (strncmp(Msg, "!tun", 4) == 0)
    {
        if (players[pack_mso->UCID].Zone != 2)
        {
            SendMTC(pack_mso->UCID, msg->_( pack_mso->UCID, "2000" ));
            return;
        }
        char * comand;
        char * id;

        comand = strtok (Msg, " ");
        id = strtok (NULL, " ");

        if ((!id) or (strlen(id) != 3))
        {
            SendMTC(pack_mso->UCID, "Error");
            return;
        }


        if (strcmp(id, "ECU")==0)
        {
            int needcash = 5000 + (GetCarID(players[pack_mso->UCID].CName)-1)*1000;
#ifdef _RC_LEVEL_H
            int needlvl = (GetCarID(players[pack_mso->UCID].CName)-1)*5+1;
            if (dl->GetLVL(pack_mso->UCID) < needlvl)
            {
                char msg[64];
                sprintf(msg, "^C^2|^7 Вам нужен уровень: ^1%d", needlvl);
                SendMTC(pack_mso->UCID, msg);
                return;
            }
#endif
            if (players[pack_mso->UCID].CTune&1)
            {
                char msg[64];
                sprintf(msg, "^C^2|^7 У вас уже есть ECU");
                SendMTC(pack_mso->UCID, msg);
                return;
            }

            if (bank->GetCash(pack_mso->UCID) >= needcash)
            {
                players[pack_mso->UCID].CTune +=1;
                char msg[64];
                sprintf(msg, "^C^2|^7 Вы купили ECU (%d RUR.)", needcash);
                SendMTC(pack_mso->UCID, msg);
                bank->RemCash(pack_mso->UCID, needcash);
                bank->AddToBank(needcash);
            }
            else
            {
                char msg[64];
                sprintf(msg, "^C^2|^7 Вам нужно %d RUR.", needcash);
                SendMTC(pack_mso->UCID, msg);
            }
        }
        else if (strcmp(id, "TRB")==0)
        {
            int needcash = 10000 + (GetCarID(players[pack_mso->UCID].CName)-1)*10000;
#ifdef _RC_LEVEL_H
            int needlvl = (GetCarID(players[pack_mso->UCID].CName)-1)*5+2;
            if (dl->GetLVL(pack_mso->UCID) < needlvl)
            {
                char msg[64];
                sprintf(msg, "^C^2|^7 Вам нужен уровень: ^1%d", needlvl);
                SendMTC(pack_mso->UCID, msg);
                return;
            }
#endif
            if (players[pack_mso->UCID].CTune&2)
            {
                char msg[64];
                sprintf(msg, "^C^2|^7 У вас уже есть Turbo");
                SendMTC(pack_mso->UCID, msg);
                return;
            }

            if (bank->GetCash(pack_mso->UCID) >= needcash)
            {
                players[pack_mso->UCID].CTune +=2;
                char msg[64];
                sprintf(msg, "^C^2| ^7Вы купили Turbo (%d RUR.)", needcash);
                SendMTC(pack_mso->UCID, msg);
                bank->RemCash(pack_mso->UCID, needcash);
                bank->AddToBank(needcash);
            }
            else
            {
                char msg[64];
                sprintf(msg, "^C^2| ^7Вам нужно %d RUR.", needcash);
                SendMTC(pack_mso->UCID, msg);
            }

        }
        else if (strcmp(id, "WHT")==0)
        {
            int needcash = 3000 + (GetCarID(players[pack_mso->UCID].CName)-1)*1000;
#ifdef _RC_LEVEL_H
            int needlvl = (GetCarID(players[pack_mso->UCID].CName)-1)*5+3;
            if (dl->GetLVL(pack_mso->UCID) < needlvl)
            {
                char msg[64];
                sprintf(msg, "^C^2| ^7Вам нужен уровень: ^1%d", needlvl);
                SendMTC(pack_mso->UCID, msg);
                return;
            }
#endif
            if (players[pack_mso->UCID].CTune&4)
            {
                char msg[64];
                sprintf(msg, "^C^2| ^7У вас уже установлено \"Облегчение веса\"");
                SendMTC(pack_mso->UCID, msg);
                return;
            }

            if (bank->GetCash(pack_mso->UCID) >= needcash)
            {
                players[pack_mso->UCID].CTune+=4;
                char msg[64];
                sprintf(msg, "^C^2|^7 Вы купили \"Облегчение веса\" (%d RUR.)", needcash);
                SendMTC(pack_mso->UCID, msg);
                bank->RemCash(pack_mso->UCID, needcash);
                bank->AddToBank(needcash);
            }
            else
            {
                char msg[64];
                sprintf(msg, "^C^2| ^7Вам нужно %d RUR.", needcash);
                SendMTC(pack_mso->UCID, msg);
            }

        }

    }


    if (strncmp(Msg, "!untun", 6) == 0)
    {
        if (players[pack_mso->UCID].Zone != 2)
        {
            SendMTC(pack_mso->UCID, msg->_( pack_mso->UCID, "2000" ));
            return;
        }
        char * comand;
        char * id;

        comand = strtok (Msg, " ");
        id = strtok (NULL, " ");

        if ((!id) or (strlen(id) != 3))
        {
            SendMTC(pack_mso->UCID, "Error");
            return;
        }

        if (strcmp(id, "ECU")==0)
        {
            int sellcash = (5000 + (GetCarID(players[pack_mso->UCID].CName)-1)*1000)*8/10;

            if (players[pack_mso->UCID].CTune&1)
            {
                players[pack_mso->UCID].CTune -=1;
                char msg[64];
                sprintf(msg, "^C^2| ^7Вы продали ECU (%d RUR.)", sellcash);
                SendMTC(pack_mso->UCID, msg);
                bank->AddCash(pack_mso->UCID, sellcash, true);
                bank->RemFrBank(sellcash);
            }
        }

        else if (strcmp(id, "TRB")==0)
        {
            int sellcash = (10000 + (GetCarID(players[pack_mso->UCID].CName)-1)*10000)*8/10;

            if (players[pack_mso->UCID].CTune&2)
            {
                players[pack_mso->UCID].CTune -=2;
                char msg[64];
                sprintf(msg, "^C^2| ^7Вы продали Turbo (%d RUR.)", sellcash);
                SendMTC(pack_mso->UCID, msg);
                bank->AddCash(pack_mso->UCID, sellcash, true);
                bank->RemFrBank(sellcash);
            }
        }

        else if (strcmp(id, "WHT")==0)
        {
            int sellcash = (3000 + (GetCarID(players[pack_mso->UCID].CName)-1)*1000)*8/10;

            if (players[pack_mso->UCID].CTune&4)
            {
                players[pack_mso->UCID].CTune -=4;
                char msg[64];
                sprintf(msg, "^C^2| ^7Вы продали \"Облегчение веса\" (%d RUR.)", sellcash);
                SendMTC(pack_mso->UCID, msg);
                bank->AddCash(pack_mso->UCID, sellcash, true);
                bank->RemFrBank(sellcash);
            }
        }
    }
    //!buy

    if (strncmp(Msg, "!buy", 4) == 0)
    {
        if (players[pack_mso->UCID].Zone != 2)
        {
            SendMTC(pack_mso->UCID, msg->_( pack_mso->UCID, "2000" ));
            return;
        }
        char * comand;
        char * id;

        comand = strtok (Msg, " ");
        id = strtok (NULL, " ");

        if ((!id) or (strlen(id) != 3))
        {
            SendMTC(pack_mso->UCID, "Error");
            return;
        }

        int CarID;

        for (CarID = 0; CarID < MAX_CARS; CarID ++)
        {
            if (strcmp(id, ginfo->car[CarID].car)== 0)
            {
                break;
            }
        }

        if (CarID == MAX_CARS)
        {
            SendMTC(pack_mso->UCID, "^C^2| ^7У нас нет такой машины!");
            return;
        }

#ifdef _RC_LEVEL_H
        int needlvl = (CarID-1)*5;
        if (dl->GetLVL(pack_mso->UCID) < needlvl)
        {
            char msg[64];
            sprintf(msg, "^C^2| ^7Нужен уровень: ^1%d", needlvl);
            SendMTC(pack_mso->UCID, msg);
            return;
        }
#endif
        if (bank->GetCash(pack_mso->UCID) < (int)ginfo->car[CarID].cash)
        {
            char msg[64];
            sprintf(msg, "^C^2| ^7Нужно ^1%d ^7RUR.", (int)ginfo->car[CarID].cash);
            SendMTC(pack_mso->UCID, msg);
            return;
        }


        for ( int j=0; j<MAX_CARS; j++)
        {
            if (strcmp(id, players[pack_mso->UCID].cars[j].car)== 0)
            {
                char msg[64];
                sprintf(msg, "^C^2| ^7У вас уже есть такая машина");
                SendMTC(pack_mso->UCID, msg);
                return;
            }
        }

        for ( int j=0; j<MAX_CARS; j++)
        {

            if (strlen(players[pack_mso->UCID].cars[j].car) == 0)
            {
                strcpy(players[pack_mso->UCID].cars[j].car, id);
                players[pack_mso->UCID].cars[j].tuning=0;
                players[pack_mso->UCID].cars[j].dist=0;
                char msg[64];
                sprintf(msg, "^C^2|^7 Вы купили %s", id);
                SendMTC(pack_mso->UCID, msg);

                bank->RemCash(pack_mso->UCID, ginfo->car[CarID].cash);
                bank->AddToBank(ginfo->car[CarID].cash);

                players[pack_mso->UCID].PLC += ginfo->car[CarID].PLC;
                SendPLC(pack_mso->UCID, players[pack_mso->UCID].PLC);

                SYSTEMTIME sm;
                GetLocalTime(&sm);
                char log[MAX_PATH];
                sprintf(log, "%slogs\\shop\\shop(%d.%d.%d).txt", RootDir, sm.wYear, sm.wMonth, sm.wDay);

                ofstream readf (log, ios::app);
                readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[pack_mso->UCID].UName << " buy car " << id << endl;
                readf.close();

                char sql[128];
                sprintf(sql, "INSERT INTO garage  ( username, car ) VALUES ( '%s' , '%s' );", players[pack_mso->UCID].UName , ginfo->car[CarID].car );
                mysql_query( rcMaindbConn , sql );

                break;
            }

        }


    }

    if (strncmp(Msg, "!sell", 4) == 0)
    {
        if (players[pack_mso->UCID].Zone != 2)
        {
            SendMTC(pack_mso->UCID, msg->_( pack_mso->UCID, "2000" ));
            return;
        }
        char * comand;
        char * id;

        comand = strtok (Msg, " ");
        id = strtok (NULL, " ");
        // test if "id" NULL or != 3
        if ( ( !id ) or ( strlen( id ) != 3 ) )
        {
            SendMTC(pack_mso->UCID, "Error");
            return;
        }

        if (strcmp(id, "UF1")==0)
            return;
        // search car in global base
        int j = 0; // global car id
        for ( j = 1 ; j < MAX_CARS ; j++)
            if (strcmp(id, ginfo->car[j].car)==0)
                break;

        if ( j == MAX_CARS )
            return;

        // if user now  on this car
        if (strcmp(id, players[pack_mso->UCID].CName)==0)
        {
            SendMTC(pack_mso->UCID, msg->_( pack_mso->UCID, "1009" ));
            return;
        }

        for ( int k=0; k<MAX_CARS; k++)
        {
            if (strcmp(players[pack_mso->UCID].cars[k].car, ginfo->car[j].car) == 0)
            {
                char msg[64];
                sprintf(msg, "^C^2|^7 Вы продали %s", id);
                SendMTC(pack_mso->UCID, msg);

                strcpy(players[pack_mso->UCID].cars[k].car, "");
                players[pack_mso->UCID].cars[k].tuning=0;
                players[pack_mso->UCID].cars[k].dist=0;

                SYSTEMTIME sm;
                GetLocalTime(&sm);
                char log[MAX_PATH];
                sprintf(log, "%slogs\\shop\\shop(%d.%d.%d).txt", RootDir, sm.wYear, sm.wMonth, sm.wDay);
                ofstream readf (log, ios::app);
                readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << ":" << sm.wMilliseconds << " " <<  players[pack_mso->UCID].UName << " sell car " << id << endl;
                readf.close();

                bank->AddCash(pack_mso->UCID, ginfo->car[j].sell, true);
                bank->RemFrBank(ginfo->car[j].sell);

                players[pack_mso->UCID].PLC -= ginfo->car[j].PLC;
                SendPLC(pack_mso->UCID, players[pack_mso->UCID].PLC);

                char sql[128];
                sprintf(sql, "DELETE FROM garage WHERE  username = '%s' AND  car = '%s'", players[pack_mso->UCID].UName , ginfo->car[j].car );
                mysql_query( rcMaindbConn , sql );
                break;
            }

        }

    }

    //!EXIT
    if (
        ( strncmp(Msg, "!exit", 5) == 0 or ( strncmp(Msg, "!^Cвыход", 8) == 0) ) and
        (strcmp(players[pack_mso->UCID].UName, "denis-takumi") == 0 or strcmp(players[pack_mso->UCID].UName, "Lexanom") == 0 || pack_mso->UCID == 0))
    {
        SendMST("/msg ^1| ^3Russian Cruise: ^7^CПодана команда на выключение");
        SendMST("/msg ^1| ^3Russian Cruise: ^7^CСохранение данных");

		SaveAll();

        ok=0;
        return;
    }

    if (strncmp(Msg, "!reload", 7) == 0 and (strcmp(players[pack_mso->UCID].UName, "denis-takumi") == 0 or strcmp(players[pack_mso->UCID].UName, "Lexanom") == 0))
    {
        SendMST("/msg ^1| ^3Russian Cruise: ^7Config reload");
        RCBaseClass::CCText("^7Config reload");

        IS_TINY *pack_requests = new IS_TINY;
        memset(pack_requests, 0, sizeof(struct IS_TINY));
        pack_requests->Size = sizeof(struct IS_TINY);
        pack_requests->Type = ISP_TINY;
        pack_requests->ReqI = 255;
        pack_requests->SubT = TINY_RST;
        insim->send_packet(pack_requests);
        delete pack_requests;

        return;
    }

    if (strncmp(Msg, "!debug", 7) == 0)
    {
        if (players[pack_mso->UCID].Svetofor == 0)
            players[pack_mso->UCID].Svetofor = 1;
        else
        {
#ifdef _RC_PIZZA_H
            pizza->ClearButtonInfo(pack_mso->UCID);
#endif
            players[pack_mso->UCID].Svetofor = 0;
        }

        return;
    }

    if ((strncmp(Msg, "!pit", 4) == 0) or (strncmp(Msg, "!^Cпит", 6) == 0 ))
    {
    	if (bank->GetCash(pack_mso->UCID)<=250)
		{
			SendMTC(pack_mso->UCID, msg->_( pack_mso->UCID, "NoManyPay" ));
			return;
		}

#ifdef _RC_POLICE_H
        if ( police->InPursuite( pack_mso->UCID ) == 1 )
        {
            char Msg[64];
            sprintf(Msg, "/pitlane %s", players[pack_mso->UCID].UName);
            SendMST(Msg);

            int pay = 10000;

            char str[96];
            sprintf(str, msg->_(pack_mso->UCID, "2700"), pay);

            SendMTC(pack_mso->UCID, str);
            bank->RemCash(pack_mso->UCID, pay);
            bank->AddToBank(pay);

            if (dl->GetSkill( pack_mso->UCID ) > 10 )
                dl->RemSkill(pack_mso->UCID, 10);

            return;
        }
#endif
        char Msg[64];
        sprintf(Msg, "/pitlane %s", players[pack_mso->UCID].UName);
        SendMST(Msg);
        players[pack_mso->UCID].Zone = 1;
        bank->RemCash(pack_mso->UCID, 500);
        bank->AddToBank(500);
        taxi->PassLoss(pack_mso->UCID);

        return;
    }
    if ((strncmp(Msg, "!test", 5) == 0))
    {
    	return;
    }

    //!users
    if ((strncmp(Msg, "!users", 6) == 0) or (strncmp(Msg, "!^Cнарод", 8) == 0 ))
	{
		ShowUsersList(pack_mso->UCID);
		return;
	}


}

void case_mso_flood ()
{
    struct IS_MSO *pack_mso = (struct IS_MSO*)insim->get_packet();

    if (pack_mso->UCID == 0)
        return;


    char Msg[128];
    strcpy(Msg, pack_mso->Msg + ((unsigned char)pack_mso->TextStart));

    /**  FLOOD ***/
    if (pack_mso->UserType == MSO_USER) // if msg from user
    {

        time_t timef;
        int ftime = time(&timef); // get current time

        if (players[pack_mso->UCID].FloodTime == 0)
            players[pack_mso->UCID].FloodTime = ftime;

        int ts = ftime - players[pack_mso->UCID].FloodTime;
        players[pack_mso->UCID].FloodTime = ftime;


        if (ts < 5)
            players[pack_mso->UCID].FloodCount++;
        else
            players[pack_mso->UCID].FloodCount = 1;

        if (players[pack_mso->UCID].FloodCount > 4)   //max lines to tolerate
        {
            int pay = 1000;

            char str[96];
            sprintf(str, msg->_(pack_mso->UCID, "flood"), pay);
            SendMTC(pack_mso->UCID, str);

            players[pack_mso->UCID].FloodCount = 0;
            bank->RemCash(pack_mso->UCID, pay);
            bank->AddToBank(pay);
        }

        /*** МАТ И Т.П. ***/
        for (int j=0; j< ginfo->WordsCount; j++)
        {
            if (strstr(Msg, ginfo->Words[j]))
            {
                int pay = 5000;

                char str[96];
                sprintf(str, msg->_(pack_mso->UCID, "swear"), pay);
                SendMTC(pack_mso->UCID, str);

                bank->RemCash(pack_mso->UCID, pay);
                bank->AddToBank(pay);
            }

        }
        strcpy( players[pack_mso->UCID].Msg, Msg);
    }
}

void case_ncn ()
{
    struct IS_NCN *pack_ncn = (struct IS_NCN*)insim->get_packet();

    if (pack_ncn->UCID == 0)
        return;

    if (pack_ncn->ReqI == 0)
        RCBaseClass::CCText(">> connected " + (string)pack_ncn->UName);

    // Copy all the player data we need into the players[] array
    strcpy(players[pack_ncn->UCID].UName, pack_ncn->UName);
    strcpy(players[pack_ncn->UCID].PName, pack_ncn->PName);

    players[pack_ncn->UCID].Zone = 1;

    read_user_cars(pack_ncn->UCID);

    //help_cmds(&players[pack_ncn->UCID], 1);
    SendMTC(pack_ncn->UCID, msg->_( pack_ncn->UCID, "Help1" ));
    SendMTC(pack_ncn->UCID, msg->_( pack_ncn->UCID, "Help14" ));

    btn_panel(pack_ncn->UCID);
}

void case_npl ()
{
    char specText[64];
    struct IS_NPL *pack_npl = (struct IS_NPL*)insim->get_packet();

    PLIDtoUCID[pack_npl->PLID] = pack_npl->UCID;

    sprintf(specText, "/spec %s", players[pack_npl->UCID].UName);

    if (pack_npl->PType != 6)
    {
        msg->SendBFNAll( pack_npl->UCID );

        //players[i].Pitlane = 1;

        //players[i].PLID = pack_npl->PLID;
        players[pack_npl->UCID].H_TRes =  pack_npl->H_TRes;
        players[pack_npl->UCID].SetF =  pack_npl->SetF;

        int j=0;
        for (j=0; j<MAX_CARS; j++)
        {
            if (strcmp(players[pack_npl->UCID].cars[j].car, pack_npl->CName)==0)
                break;
        }

        if ( j != MAX_CARS)
        {
            strcpy(players[pack_npl->UCID].CName , pack_npl->CName);
            players[pack_npl->UCID].CTune = players[pack_npl->UCID].cars[j].tuning;
            players[pack_npl->UCID].Distance = players[pack_npl->UCID].cars[j].dist;

            int tune = 45;
            if (players[pack_npl->UCID].CTune&1)
            {
                tune -= 15;
            }
            if (players[pack_npl->UCID].CTune&2)
            {
                tune -= 20;
            }
            if (players[pack_npl->UCID].CTune&4)
            {
                tune -= 10;
            }

#ifdef _RC_LEVEL_H
            int needlvl = (GetCarID(players[pack_npl->UCID].CName)-1)*5;

            if (dl->GetLVL(pack_npl->UCID) < needlvl)
            {
                players[pack_npl->UCID].Zone = 1;
                SendMST( specText );

                char msg2[64];
                sprintf(msg2, "^C^1|^7 Нужен уровень: %d", needlvl);
                SendMTC(pack_npl->UCID, msg2);
                SendMTC(pack_npl->UCID, msg->_( pack_npl->UCID, "2404" ));

                char Text2[64];
                strcpy(Text2, "^1| ^2");
                for (int k=0; k< MAX_CARS; k++)
                {
                    if ((strlen(players[pack_npl->UCID].cars[k].car) != 0) and ((GetCarID(players[pack_npl->UCID].cars[k].car)-1)*5 <= dl->GetLVL(pack_npl->UCID)))
                    {
                        strcat(Text2, players[pack_npl->UCID].cars[k].car);
                        strcat(Text2, " ");
                    }
                }
                SendMTC(pack_npl->UCID, Text2);
                return;
            }
            else if ((pack_npl->H_TRes < tune))
            {
                players[pack_npl->UCID].Zone = 1;
                SendMST( specText );

                char Texxt[32];
                sprintf(Texxt, "%s %d %%", msg->_( pack_npl->UCID, "2400" ), tune);
                SendMTC(pack_npl->UCID, Texxt);
            }
#endif
        }
        else
        {
            SendMTC(pack_npl->UCID, msg->_( pack_npl->UCID, "2404" ));
            help_cmds(pack_npl->UCID, 2);
            players[pack_npl->UCID].Zone = 1;
            SendMST( specText );
            return;
        }
    } //if PTupe != 6

}


void case_pen ()
{
    struct IS_PEN *pack_pen = (struct IS_PEN*)insim->get_packet();

    byte UCID = PLIDtoUCID[pack_pen->PLID];

    if (pack_pen->Reason == PENR_WRONG_WAY)
    {
#ifdef _RC_TAXI_H
        taxi->PassDead(UCID);
#endif
    }
}

void case_pll ()
{
    struct IS_PLL *pack_pll = (struct IS_PLL*)insim->get_packet();

    byte UCID = PLIDtoUCID[pack_pll->PLID];

    memset(&players[UCID].Info, 0, sizeof(CompCar));

    save_car(UCID);
#ifdef _RC_POLICE_H
    if ( police->InPursuite( UCID ) == 1 )
    {
        int pay = 10000;

        char str[96];
        sprintf(str, msg->_(UCID, "2600"), pay);
        SendMTC(UCID, str);


        bank->RemCash(UCID, pay);
        bank->AddToBank(pay);

        if ( dl->GetSkill( UCID ) > 10 )
            dl->RemSkill( UCID, 10);
    }
    else
    {
#endif
        if (players[UCID].Zone != 1)
        {
            int pay = 1000;

            char str[96];
            sprintf(str, msg->_(UCID, "2602"), pay);
            SendMTC(UCID, str);

            bank->RemCash(UCID, pay);
            bank->AddToBank(pay);
        }
#ifdef _RC_POLICE_H
    }
#endif
    players[UCID].Zone = 1;
    PLIDtoUCID.erase(pack_pll->PLID);
}

void case_plp ()
{
     struct IS_PLP *pack_plp = (struct IS_PLP*)insim->get_packet();

    // Find player and set his PLID to 0
    byte UCID = PLIDtoUCID[pack_plp->PLID];

    memset(&players[UCID].Info, 0, sizeof(CompCar));

    save_car(UCID);
#ifdef _RC_POLICE_H
    if ( police->InPursuite( UCID ) == 1 )
    {
        int pay = 10000;

        char str[96];
        sprintf(str, msg->_(UCID, "2700"), pay);
        SendMTC(UCID, str);

        bank->RemCash(UCID, pay);
        bank->AddToBank(pay);

        if ( dl->GetSkill( UCID ) > 10 )
            dl->RemSkill(UCID, 10);
    }
    else
    {
#endif
        if (players[UCID].Zone != 1)
        {
            int pay = 1000;

            char str[96];
            sprintf(str, msg->_(UCID, "2702"), pay);
            SendMTC(UCID, str);

            bank->RemCash(UCID, pay);
            bank->AddToBank(pay);
        }
#ifdef _RC_POLICE_H
    }
#endif
    players[UCID].Zone = 1;

}

void case_rst ()
{
    struct IS_RST *pack_rst = (struct IS_RST*)insim->get_packet();

    strcpy(ginfo->Track, pack_rst->Track);

    read_car();

    read_track();

    read_words();
};

void case_vtn ()
{
    struct IS_VTN *pack_vtn = (struct IS_VTN *)insim->get_packet();

    if (pack_vtn->UCID == 0) return;

	SendMST("/cv");
}

void ShowUsersList(byte UCID)
{
    for (int i=0; i<50; i++)
        SendBFN(UCID, i);

    byte count = 0, L = 0, T = 0;
    for (auto& plr: players)
    {
        // Viva La Костыль
        if(plr.first == 0 || strlen(plr.second.PName) == 0)
            continue;

        if (count == 24)
        {
            L += 22;
            T = 0;
        }

        SendButton(plr.first, UCID, count++, 1 + L, 191 - 4*T++, 22, 4, 16 + 8, players[plr.first].PName);
    }
    SendButton(255, UCID, 48, 1, 195, count > 24 ? 44 : 22, 4, 16 + 8, msg->_( UCID, "604" ));
}

int core_connect(void *pack_ver)
{
    memset(pack_ver, 0, sizeof(struct IS_VER));
    struct IS_VER *pack_v = (IS_VER*)pack_ver;

    if (insim->init (ginfo->IP, ginfo->TCPPORT, IS_PRODUCT_NAME, ginfo->ADMIN, pack_v, '!', isf_flag, 500, ginfo->UDPPORT) < 0)
    {
        cout << "\n * Error during initialization * " << endl;
        return -1;
    }

    strcpy(ginfo->Product, pack_v->Product);

    struct IS_TINY pack_requests;
    memset(&pack_requests, 0, sizeof(struct IS_TINY));
    pack_requests.Size = sizeof(struct IS_TINY);
    pack_requests.Type = ISP_TINY;
    pack_requests.ReqI = 255;

    pack_requests.SubT = TINY_RST;
    insim->send_packet(&pack_requests);

    pack_requests.SubT = TINY_NCN;      // Request all connections to store their user data
    insim->send_packet(&pack_requests);

    pack_requests.SubT = TINY_NPL;      // Request all players in-grid to know their PLID
    insim->send_packet(&pack_requests);

    return 1;
}

int core_reconnect(void *pack_ver)
{
    insim->isclose();

    cout << "wait 1 minute and reconnect \n";

    Sleep(60000);

    memset(pack_ver, 0, sizeof(struct IS_VER));
    struct IS_VER *pack_v = (IS_VER*)pack_ver;

    if (insim->init (ginfo->IP, ginfo->TCPPORT, IS_PRODUCT_NAME, ginfo->ADMIN, pack_v, '!', isf_flag, 500, ginfo->UDPPORT) < 0)
    {
        cout << "\n * Error during initialization * " << endl;
        return -1;
    }

    strcpy(ginfo->Product, pack_v->Product);

    struct IS_TINY pack_requests;
    memset(&pack_requests, 0, sizeof(struct IS_TINY));
    pack_requests.Size = sizeof(struct IS_TINY);
    pack_requests.Type = ISP_TINY;
    pack_requests.ReqI = 1;

    pack_requests.SubT = TINY_RST;      // Request all players in-grid to know their PLID
    insim->send_packet(&pack_requests);

    pack_requests.SubT = TINY_NCN;      // Request all connections to store their user data
    insim->send_packet(&pack_requests);

    pack_requests.SubT = TINY_NPL;      // Request all players in-grid to know their PLID
    insim->send_packet(&pack_requests);

    return 1;
}

void read_track()
{
    char file[255];
    sprintf(file, "%s\\data\\RCCore\\tracks\\%s.txt" , RootDir , ginfo->Track);

    FILE *fff = fopen(file, "r");
    if (fff == nullptr)
    {
        cout << "Can't find " << file << endl;
        return;
    }
    fclose(fff);

    ifstream readf (file, ios::in);

    memset(&ginfo->TrackInf, 0, sizeof(track_info));

    while (readf.good())
    {
        char str[128];
        readf.getline(str, 128);

        if (strstr(str, "//")) {}
        if (strlen(str) > 0)
        {
            if (strncmp(str, "/pitzone", 8)==0)
            {
                readf.getline(str, 128);
                int count = atoi(str);

                ginfo->TrackInf.PitCount = count;

                if ( ginfo->TrackInf.XPit != NULL )
                    delete[] ginfo->TrackInf.XPit;

                if ( ginfo->TrackInf.YPit != NULL )
                    delete[] ginfo->TrackInf.YPit;

                ginfo->TrackInf.XPit = new int[count];
                ginfo->TrackInf.YPit = new int[count];

                for (int i=0 ; i<count; i++)
                {
                    readf.getline(str, 128);
                    char * X;
                    char * Y;
                    X = strtok (str, ";");
                    Y = strtok (NULL, ";");
                    ginfo->TrackInf.XPit[i] = atoi(X);
                    ginfo->TrackInf.YPit[i] = atoi(Y);
                }
            }

            if (strncmp(str, "/shop", 5)==0)
            {
                readf.getline(str, 128);
                int count = atoi(str);
                ginfo->TrackInf.ShopCount = count;

                if ( ginfo->TrackInf.XShop != NULL )
                    delete[] ginfo->TrackInf.XShop;

                if ( ginfo->TrackInf.YShop != NULL )
                    delete[] ginfo->TrackInf.YShop;

                ginfo->TrackInf.XShop = new int[count];
                ginfo->TrackInf.YShop = new int[count];

                for (int i=0 ; i<count; i++)
                {
                    readf.getline(str, 128);
                    char * X;
                    char * Y;
                    X = strtok (str, ";");
                    Y = strtok (NULL, ";");
                    ginfo->TrackInf.XShop[i] = atoi(X);
                    ginfo->TrackInf.YShop[i] = atoi(Y);
                }
            }
        } // if strlen > 0
    } //while readf.good()

    readf.close();

}

void read_car()
{
    char file[255];
    sprintf(file, "%smisc\\cars.txt" , RootDir);

    FILE *fff = fopen(file, "r");
    if (fff == nullptr)
    {
        cout << "Can't find " << file << endl;
        return;
    }
    fclose(fff);

    carMap.clear();

    ifstream readf (file, ios::in);
    int i = 0;
    while (readf.good())
    {
        char str[128];
        readf.getline(str, 128);
        if (strlen(str) > 0)
        {

            if (strstr(str, "//"))
                continue;

            char * id;
            char * car;
            char * cash;
            char * PLC;
            id = strtok (str, ";");
            car = strtok (NULL, ";");
            cash = strtok (NULL, ";");
            PLC = strtok (NULL, ";");

            i = atoi(id);
            memset(&ginfo->car[i], 0, sizeof(struct cars));
            ginfo->car[i].id = i;
            strcpy(ginfo->car[i].car, car);
            ginfo->car[i].cash =atoi(cash);
            ginfo->car[i].sell= ginfo->car[i].cash*8/10;
            ginfo->car[i].PLC =atoi(PLC);
            /** map<> **/
            carMap[ car ] = AddCar(i , car, atoi(cash), atoi(cash)*8/10 , atoi(PLC) );

        } // if strlen > 0
    } //while readf.good()

    readf.close();
}

void read_cfg()
{
    char file[255];
    sprintf(file, "%smisc\\%s.cfg", RootDir, ServiceName);

    FILE *fff = fopen(file, "r");
    if (fff == nullptr)
    {
        cout << "Can't find " << file << endl;
        return;
    }
    fclose(fff);

    ifstream readf (file, ios::in);

    char str[128];
    while (readf.good())
    {
        readf.getline(str, 128);
        if (strlen(str) > 0)
        {
            // GET IP
            if (strncmp("Ip=", str, 3)==0)
                strncpy(ginfo->IP, str+3, strlen(str+3));

            // GET TCPPORT
            if (strncmp("TcpPort=", str, 8)==0)
                ginfo->TCPPORT = atoi(str+8);

            // GET UDPPORT
            if (strncmp("UdpPort=", str, 8)==0)
                ginfo->UDPPORT = atoi(str+8);

            //GET ADMIN PASSWORD
            if (strncmp("Admin=", str, 6)==0)
                strncpy(ginfo->ADMIN, str+6, strlen(str+6));
        }
    }
}

/*********************************************/

void *ThreadMci (void *params)
{
    while (true)
    {
        if (insim->udp_next_packet() < 0)
            continue;

        case_mci ();
		for( cl = classes.begin(); cl != classes.end(); ++cl )
		{
			(*cl)->InsimMCI( (struct IS_MCI*)insim->udp_get_packet() );
		}

    }
    return 0;
};

void *ThreadSave (void *params)
{
    SYSTEMTIME sm; //time_t seconds;
    while (ok > 0)
    {
        GetLocalTime(&sm); //seconds = time (NULL);
        if ((sm.wMinute*60+sm.wSecond) % 600 == 0) //every 30 minute
        {
			SaveAll();
        }

        Sleep(500);

#ifdef _RC_POLICE_H
        police->SetSirenLight("^4||||||||||^7|^1||||||||||");
#endif

        Sleep(500);


#ifdef _RC_POLICE_H
        police->SetSirenLight( "^1||||||||||^7|^4||||||||||" );
#endif

    }
    return 0;
};

void *ThreadWork (void *params)
{
    Sleep(10000);

    while (ok > 0)
    {
#ifdef _RC_POLICE_H
        police->Event();
#endif
#ifdef _RC_PIZZA_H
        pizza->Event();
#endif
#ifdef _RC_TAXI_H
        taxi->Event();
#endif // _RC_TAXI_H
        lgh->Event();

        Sleep(500);

    }
    return 0;
};

int main(int argc, char* argv[])
{

    isf_flag = ISF_MCI + ISF_CON + ISF_OBH + ISF_HLV + ISF_AXM_EDIT + ISF_AXM_LOAD;

    int need = 92;
    int d;

    for (d = strlen(argv[0]); d > 0; d--)
    {
        if ( int(argv[0][d]) == need )
            break;
    }

    strncpy(RootDir, argv[0], d+1);
    strcpy(ServiceName, argv[1]);

    //tools::rootDir = RootDir;

    if (strlen(ServiceName) == 0)
    {
		RCBaseClass::CCText("^3RCMain:\t^C^1Не задан файл конфигурации");
		return 0;
    }

	if (!mysql_init(&rcMaindb))
    {
        RCBaseClass::CCText("^3RCMain: ^1Can't create MySQL-descriptor");
        return 0;
    }
    my_bool reconnect = 1;
    mysql_options( &rcMaindb , MYSQL_OPT_RECONNECT, &reconnect ); // разрешаем переподключение

    mysqlConf conf;
    char path[MAX_PATH];
    memset(&path,0,MAX_PATH);
    sprintf(path, "%smisc\\mysql.cfg", RootDir);
    tools::read_mysql(path, &conf);

    while ( (rcMaindbConn = mysql_real_connect( &rcMaindb , conf.host , conf.user , conf.password , conf.database , conf.port , NULL, 0)) == false )
    {
        RCBaseClass::CCText("^3RCMain: ^1Can't connect to MySQL server");
        Sleep(60000);
    }
    RCBaseClass::CCText("^3RCMain:\t\t^2Connected to MySQL server");

    sprintf(IS_PRODUCT_NAME, "RC-%s\0", AutoVersion::RC_UBUNTU_VERSION_STYLE);

    insim = new CInsim();
    ginfo = new GlobalInfo();

    read_cfg();

    if ( core_connect(&pack_ver) < 0 )
        for (;;)
            if ( core_reconnect(&pack_ver) > 0 )
                break;

    if (pack_ver.InSimVer != 5)
    {
        RCBaseClass::CCText("^3RCMain:\t^1INSIM VER != 5");
        return -1;
    }

    players.clear();

    CreateClasses();
    InitClasses();

    pthread_t mci_tid; // Thread ID
    pthread_t work_tid; // Thread ID
    pthread_t save_tid; // Thread ID

    if (pthread_create(&work_tid, NULL, ThreadWork, NULL) < 0)
    {
        printf("^3RCMain:\t\tCan't start `thread_work` Thread\n");
        return 0;
    }

    Sleep(100);

    if (pthread_create(&save_tid, NULL, ThreadSave, NULL) < 0)
    {
        RCBaseClass::CCText("^3RCMain:\t\t^1Can't start `thread_save` Thread\n");
        return 0;
    }

    Sleep(100);

    if (pthread_create(&mci_tid, NULL, ThreadMci, NULL) < 0)
    {
        RCBaseClass::CCText("^3RCMain:\t\t^1Can't start `thread_mci` Thread\n");
        return 0;
    }

    Sleep(100);

    RCBaseClass::CCText("^7Cruise started");

    while (ok > 0)
    {
        if (insim->next_packet() < 0)
            for (;;)
                if ( core_reconnect(&pack_ver) > 0 )
                    break;

        msg->next_packet(); // обрабатывается первым, из-за того что потом е выводятся сообщения. приоритет емае

        switch (insim->peek_packet())
        {
        case ISP_MSO:
            case_mso();
            case_mso_flood();
            break;

        case ISP_NPL:
            case_npl ();
            break;

        case ISP_NCN:
            case_ncn ();
            break;

        case ISP_CNL:
            case_cnl ();
            break;

        case ISP_PLL:
            case_pll ();
            break;

        case ISP_PLP:
            case_plp ();
            break;

        case ISP_BTC:
            case_btc ();
            break;

        case ISP_BTT:
            case_btt ();
            break;

        case ISP_BFN:
            case_bfn();
            break;

        case ISP_CPR:
            case_cpr ();
            break;

        case ISP_RST:
            case_rst ();
            break;

        case ISP_REO:
            break;

        case ISP_NLP:
            break;

       case ISP_PEN:
            case_pen ();
            break;

        case ISP_VTN:
            case_vtn();
            break;

        case ISP_TOC:
            case_toc();
            break;
        }

	   for( cl = classes.begin(); cl != classes.end(); ++cl )
	   {
			(*cl)->next_packet();
	   }

    }

    if (insim->isclose() < 0)
        return 0;

    delete insim;
    delete ginfo;

    return 0;
}
