#include "RCMain.h"
#include <version.h>
#include "tools.h"

GlobalInfo *ginfo;

DBMySQL *db;


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
RCQuest *quest;
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
    msg->init(db, insim);

#ifdef _RC_BANK_H
    bank->init(db, insim, msg, dl);
#endif // _RC_BANK_H

#ifdef _RC_ENERGY_H
    nrg->init(db, insim, msg, bank);
#endif

#ifdef _RC_LEVEL_H
    dl->init(db, insim, msg);
#endif

#ifdef _RC_CHEAT_H
    antcht->init(db, insim, msg);
#endif

#ifdef _RC_STREET_H
    street->init(db, insim, msg);
#endif

#ifdef _RC_LIGHT_H
    lgh->init(db, insim, msg, dl);
#endif

#ifdef _RC_POLICE_H
    police->init(db, insim, msg, bank, dl, street, nrg, lgh);
#endif // _RC_POLICE_H

#ifdef _RC_PIZZA_H
    pizza->init(db, insim, msg, bank, nrg, dl, street);
#endif

#ifdef _RC_TAXI_H
    taxi->init(db, insim, msg, bank, dl, street, police, lgh);
#endif

#ifdef _RC_ROADSIGN
    RoadSign->init(db, insim, lgh);
#endif // _RC_ROADSIGN

#ifdef _RC_QUEST_H
    quest->init(db, insim);
#endif // _RC_QUEST_H

#ifdef _RC_AUTOSCHOOL_H
    school->init(db, insim, msg);
#endif // _RC_AUTOSCHOOL_H

}

int GetCarID(string CarName)
{
    if (CarName.size()!=3)
        return 0;

    for (int i=0; i< MAX_CARS; i++)
    {
        if (CarName == ginfo->car[i].car)
            return ginfo->car[i].id;
    }

    return 0;
}

cars AddCar(int id = 0, const char* car = "UF1", int cash =0, int sell = 0, unsigned PLC = 0)
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
    sprintf(file, "%s/misc/words.txt", RootDir);

    ifstream readf (file, ios::in);

    if(!readf.is_open())
    {
        cout << "Can't find " << file << endl;
        return;
    }

    int i=0;

    while (readf.good())
    {
        char str[32];
        readf.getline(str, 32);

        if (strlen(str) > 1)
        {
            strncpy(ginfo->Words[i], str, strlen(str));
            i++;
        }
    }

    readf.close();
    ginfo->WordsCount = i;

}

void save_user_cars (byte UCID)
{
    char sql[128];

    for (int i = 0; i < MAX_CARS; i++)
    {
        if ( strlen( players.at(UCID).cars[i].car ) > 0 )
        {
            sprintf(sql, "UPDATE garage SET tuning = %d, dist = %f WHERE car = '%s' AND username = '%s';", players.at(UCID).cars[i].tuning , players.at(UCID).cars[i].dist , players.at(UCID).cars[i].car , players.at(UCID).UName );
            if ( db->exec(sql) == false )
                printf("Bank Error: MySQL Query Save\n");
        }
    }
}



void read_user_cars(byte UCID)
{
    char kickCmd[64], msg[96];
    sprintf(kickCmd, "/kick %s",players.at(UCID).UName);
    sprintf(msg, "^1RC:Core ERROR - BAD USER");
    char query[128];
    sprintf(query, "SELECT car, tuning, dist FROM garage WHERE username='%s';", players.at(UCID).UName);

    DB_ROWS result = db->select({"car","tuning","dist"},"garage",{{"username",players.at(UCID).UName}});

    if ( result.size() > 0 )
    {
        int i = 0;
        for ( auto row: result )
        {
            strcpy(players.at(UCID).cars[i].car, row["car"].c_str());
            players.at(UCID).cars[i].tuning = atoi( row["tuning"].c_str() );
            players.at(UCID).cars[i].dist = atof( row["dist"].c_str() );
            /** map<> **/
            players.at(UCID).PLC += carMap[ row["car"] ].PLC;
            i++;
        }
    }
    else
    {
        RCBaseClass::CCText("^7Can't find " + (string)players.at(UCID).UName + ", create new user");

        sprintf(query, "INSERT INTO garage (username, car ) VALUES ('%s' , 'UF1');", players.at(UCID).UName);


        if ( db->exec(query) != 0 )
            printf("Error: MySQL Query\n");

        strcpy(players.at(UCID).cars[0].car, "UF1");
        players.at(UCID).cars[0].tuning = 0;
        players.at(UCID).cars[0].dist = 0;

        players.at(UCID).PLC += carMap[ "UF1" ].PLC;

        save_user_cars(UCID);
        btn_info(UCID,4);
    }

    insim->SendPLC(UCID, players.at(UCID).PLC);
}

void save_car (byte UCID)
{

    for (int i=0; i<MAX_CARS; i++)
    {
        if (players.at(UCID).CName == players.at(UCID).cars[i].car)
        {
            players.at(UCID).cars[i].tuning = players.at(UCID).CTune;
            players.at(UCID).cars[i].dist = (int)players.at(UCID).Distance;
            break;
        }
    }

}

void Save(byte UCID)
{
    if(UCID == 255)
        return;

    save_car(UCID);
    save_user_cars(UCID);
    if(db->debug)
        cout << "Size of classes = " << classes.size() << endl;

    for( cl = classes.begin(); cl != classes.end(); ++cl )
    {
        if(db->debug)
            cout << (*cl)->ClassName << "::Save()" << endl;

        (*cl)->Save( UCID );
    }

    insim->SendMTC(UCID, msg->_( UCID, "3000" ));
}

void SaveAll(bool SaveBonus)
{
    if(players.size() <= 0)
        return;

    for(auto& pl: players)
    {
        byte UCID = pl.first;

        Save(UCID);

        if(SaveBonus)
        {
            bonuses[ players.at(UCID).UName ]["count"] = players.at(UCID).Bonus_count;
            bonuses[ players.at(UCID).UName ]["dist"] = players.at(UCID).Bonus_dist;
        }
    }
    RCBaseClass::CCText("^2DATA SAVED");

    if(SaveBonus)
    {
        SaveBonuses();
    }
}

void help_cmds (byte UCID, int h_type)
{
    if (h_type == 1)
    {
        insim->SendMTC(UCID, msg->_(UCID, "Help1"));
        insim->SendMTC(UCID, RCBaseClass::StringFormat(msg->_(UCID, "Help2"), msg->GetLangList().c_str()));
        insim->SendMTC(UCID, msg->_(UCID, "Help3"));
        insim->SendMTC(UCID, msg->_(UCID, "Help4"));
        insim->SendMTC(UCID, msg->_(UCID, "Help5"));
        insim->SendMTC(UCID, msg->_(UCID, "Help6"));
        insim->SendMTC(UCID, msg->_(UCID, "Help7"));
        insim->SendMTC(UCID, msg->_(UCID, "Help8"));
        insim->SendMTC(UCID, msg->_(UCID, "Help9"));
        insim->SendMTC(UCID, msg->_(UCID, "Help10"));
        insim->SendMTC(UCID, msg->_(UCID, "Help11"));
        insim->SendMTC(UCID, msg->_(UCID, "Help12"));
        insim->SendMTC(UCID, msg->_(UCID, "Help13"));
    }
    if (h_type == 2)
    {
        insim->SendMTC(UCID, msg->_( UCID, "3100" ));
        for ( int i=0; i<MAX_CARS; i++)
        {
            if (strlen(players.at(UCID).cars[i].car)>0)
            {
                int     tune;
                char    Text[64];
                tune = players.at(UCID).cars[i].tuning;

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

                sprintf(Text, "^1| ^C^2%s ^3%4.0f ^7Km (%s^7)(^3%d%%^7)", players.at(UCID).cars[i].car, players.at(UCID).cars[i].dist/1000, Tun, tune2);
                insim->SendMTC(UCID, Text);
            }
        }
    }
}

void btn_info (byte UCID, int b_type)
{
    list<string> about_text = {
        RCBaseClass::StringFormat("^7RUSSIAN CRUISE  %s", IS_PRODUCT_NAME),
        "^C^7Developers: Denis Kostin, Aleksandr Mochalov",
        "^C^7Jabber: denis-kostin@jabber.ru",
        "^C^7Jabber conference: lfs@conference.jabber.ru",
        "^7",
        "^C^7More information",
        "^C^7http://vk.com/russiancruise",
        "^7",
        "^C^7Thanks:",
        "^C^3repeat, nose"
    };

    list<string> help_text = {
        RCBaseClass::StringFormat("^7RUSSIAN CRUISE  %s", IS_PRODUCT_NAME),
        msg->_(UCID,"info_help_1"),
        msg->_(UCID,"info_help_2"),
        msg->_(UCID,"info_help_3"),
        msg->_(UCID,"info_help_4"),
        msg->_(UCID,"info_help_5"),
        msg->_(UCID,"info_help_6"),
        msg->_(UCID,"info_help_7")
    };

    char Text[128];

    byte c;
    if (b_type == 1) c=MAX_CARS/2;              //количество строк для 1 вкладки
    #ifdef _RC_POLICE_H
    if (b_type == 2) c=police->GetFineCount();  //количество строк для 2 вкладки
    #endif
    if (b_type == 3) c=about_text.size();   //да, да, ты угадал
    if (b_type == 4) c=help_text.size();   //да, да, ты угадал
    byte
    id=183,             //стартовый ид кнопок
    l=100, t=90,        //центр поля
    hButton=5,          //высота одной строки
    w=100,              //ширина поля
    h=16+c*hButton;     //высота поля

    insim->SendButton(255, UCID, 176, l-w/2, t-h/2, w, h+8, 32, "");                                   //фон
    id++;
    insim->SendButton(255, UCID, 177, l-w/2, t-h/2, w, h+8, 32, "");
    id++;
    insim->SendButton(254, UCID, 178, l-7, t-h/2+h+1, 14, 6, 16+8, "^2OK");                            //закрывашка
    id++;
    insim->SendButton(255, UCID, id++, l-w/2, t-h/2, 25, 10, 3+64, "RUSSIAN CRUISE");                  //заголовок
    insim->SendButton(255, UCID, id++, l-w/2+24, t-h/2+2, 20, 3, 5+64, IS_PRODUCT_NAME);               //версия

    insim->SendButton(255, UCID, 180, l-w/2+1, t-h/2+9, 16, 6, 16+8, msg->_(UCID, "200"));  //вкладка раз
    id++;
    insim->SendButton(255, UCID, 181, l-w/2+17, t-h/2+9, 16, 6, 16+8, msg->_(UCID, "201")); //два
    id++;
    insim->SendButton(255, UCID, 182, l-w/2+33, t-h/2+9, 16, 6, 16+8, msg->_(UCID, "202")); //нутыпонел
    id++;
    insim->SendButton(255, UCID, 183, l-w/2+49, t-h/2+9, 16, 6, 16+8, msg->_(UCID, "help_tab")); //нутыпонел
    id++;

    map<byte, string>tabs = {
        pair<byte, string>{131,msg->_(UCID, "200")},
        pair<byte, string>{132,msg->_(UCID, "201")},
        pair<byte, string>{133, msg->_(UCID, "202")}
    };

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
            insim->SendButton(255, UCID, id++, l-w/2+1+plW, t-h/2+16+hButton*(i-plH), w/2-1, hButton, 16+64, Text);
        }

#ifdef _RC_POLICE_H
    if (b_type == 2)
        for (int i=0; i<c; i++)
        {
            sprintf(Text, "^7ID = %02d. %s (^2%d RUR^7)", police->fines[i+1].id, police->GetFineName(UCID, i+1), police->fines[i+1].cash);

            insim->SendButton(255, UCID, id++, l-w/2+1, t-h/2+16+hButton*i, w-2, hButton, 16+64, Text);
        }
#endif

    if (b_type == 3)
    {
        byte i = 0;
        for (auto& txt: about_text)
        {
            insim->SendButton(255, UCID, id++, l-w/2+1, t-h/2+16+hButton*i++, w-2, hButton, 0, txt);
        }

    }

     if (b_type == 4)
    {
        byte i = 0;
        for (auto& txt: help_text)
        {
            insim->SendButton(255, UCID, id++, l-w/2+1, t-h/2+16+hButton*i++, w-2, hButton, 0, txt);
        }

    }

    for (int i=id; i<239; i++)
        insim->SendBFN(UCID, i);
}

void Event ()
{
    // одометр
    for (auto& plr: players)
    {
        if (plr.first == 0 or plr.first == 255)
            continue;

        string str;
        if (players.at(plr.first).Zone== 1)
            str = msg->_(plr.first, "PitSaveGood");
        else if (players.at(plr.first).Zone== 2)
            str = msg->_( plr.first, "401" );
        else if (players.at(plr.first).Zone== 3)
            str = msg->_( plr.first, "402" );
        else if (players.at(plr.first).Zone== 4)
            str = msg->_( plr.first, "404" );
        else
            str = msg->_(plr.first, "PitSaveNotGood");
        insim->SendButton(255, plr.first, 3, 85, 1, 15, 8, 32+7, str);

        insim->SendButton(255, plr.first, 4, 70, 5, 15, 4, 32, RCBaseClass::StringFormat(msg->_(plr.first, "Dist" ), players.at(plr.first).Distance/1000));
    }
}

/****************************************/
void case_bfn ()
{
    struct IS_BFN *pack_bfn = (struct IS_BFN*)insim->get_packet();

    time_t now;
    time(&now);

    if ((now - players.at(pack_bfn->UCID).LastBFN) < 5)
        return;

    players.at(pack_bfn->UCID).LastBFN = now;
    btn_info(pack_bfn->UCID, 1);
}

void case_btc ()
{
    struct IS_BTC *pack_btc = (struct IS_BTC*)insim->get_packet();
    byte UCID = pack_btc->UCID;

    tm *timeStruct = tools::GetLocalTime();

    char log[255];
    sprintf(log, "%s/logs/shop/shop(%d.%d.%d).txt", RootDir, timeStruct->tm_year, timeStruct->tm_mon, timeStruct->tm_mday);

    /** Пользователь кликнул по другому пользователю **/
    if (pack_btc->ClickID<69)
    {
        ShowUsersList(pack_btc->UCID);

        byte L = 23, T = 187;
        string uname = RCBaseClass::StringFormat(msg->_( pack_btc->UCID, "MsgPlFor" ), players.at(pack_btc->ReqI).PName.c_str());

        if (pack_btc->ReqI == UCID)
            uname = string(msg->_(UCID, "ItsYou"));

        if (players.size()>24)
            L = 45;

        #ifdef _RC_POLICE_H
        if (police->IsCop(UCID))
        {
            uname += RCBaseClass::StringFormat(" ^8(%s^8)", players.at(pack_btc->ReqI).UName);
            if (!police->IsCop(pack_btc->ReqI))
                T = 175;
        }
        #endif

        insim->SendButton(255, UCID, 70, L, T, 24, 4, 32 + 64, uname); // на какого игрока нажали

        byte PlFlag = 3+16+8;   // BStyle активных кнопок

        if (pack_btc->ReqI == UCID)
            PlFlag = 7 + 16; // игрок не может взаимодействовать сам с собой

        insim->SendButton(pack_btc->ReqI, UCID, 71, L, T+4, 24, 4, PlFlag, msg->_( pack_btc->UCID, "1000" ), 7);   // передать деньги
        insim->SendButton(pack_btc->ReqI, UCID, 72, L, T+8, 24, 4, PlFlag, msg->_( pack_btc->UCID, "1001" ), 95);  // передать мсг

        #ifdef _RC_POLICE_H
        if ( police->IsCop(UCID) and !police-> IsCop(pack_btc->ReqI) and pack_btc->ReqI != UCID)
        {
            PlFlag = 32 + 8 + 3;
            insim->SendButton(pack_btc->ReqI, UCID, 73, L, T+12, 24, 4, PlFlag, msg->_( UCID, "FinesButton" ));   //штраф

            if (police->GetCopRank(UCID) <= 2)
                PlFlag = 32 + 7;

            insim->SendButton(pack_btc->ReqI, UCID, 74, L, T+16, 24, 4, PlFlag, msg->_( UCID, "1004" ));          //погоня вкл

            if (police->GetCopRank(UCID) == 2)
                PlFlag = 32 + 8 + 3;

            insim->SendButton(pack_btc->ReqI, UCID, 75, L, T+20, 24, 4, PlFlag, msg->_( UCID, "1005" ));          //погоня выкл
        }
        else
        {
            insim->SendBFN(UCID, 73);
            insim->SendBFN(UCID, 74);
            insim->SendBFN(UCID, 75);
        }
        #endif
    }

    /** Скрыть кнопки с пользователями **/
    if (pack_btc->ClickID==69)
    {
        for (int j=21; j<80; j++)
            insim->SendBFN(pack_btc->UCID, j);
    }

    if (pack_btc->ReqI==255)
    {
        if (pack_btc->ClickID == 180)
            btn_info(pack_btc->UCID, 1);
        if (pack_btc->ClickID == 181)
            btn_info(pack_btc->UCID, 2);
        if (pack_btc->ClickID == 182)
            btn_info(pack_btc->UCID, 3);
        if (pack_btc->ClickID == 183)
            btn_info(pack_btc->UCID, 4);
    }
}

void case_btt ()
{
    struct IS_BTT *pack_btt = (struct IS_BTT*)insim->get_packet();

    tm *timeStruct = tools::GetLocalTime();

    char send_c[255];
    sprintf(send_c, "%s/logs/sends/send.txt", RootDir);
    char fine_c[255];
    sprintf(fine_c, "%s/logs/fines/fine(%d.%d.%d).txt", RootDir, timeStruct->tm_year, timeStruct->tm_mon, timeStruct->tm_mday);

    /**  Пользователь передает деньги   */
    if (pack_btt->ClickID==71)
    {
        if (atoi(pack_btt->Text) > 0)
        {
            if (bank->GetCash(pack_btt->UCID) > atoi(pack_btt->Text))
            {
                char Msg[126];
                sprintf(Msg, msg->_(pack_btt->ReqI, "GetMoney" ), players.at(pack_btt->UCID).PName.c_str(), atoi(pack_btt->Text));
                insim->SendMTC(pack_btt->ReqI, Msg);
                msg->AddNotify(pack_btt->ReqI, Msg);

                sprintf(Msg, msg->_(pack_btt->UCID, "SendMoney" ), players.at(pack_btt->ReqI).PName.c_str(), atoi(pack_btt->Text));
                insim->SendMTC(pack_btt->UCID, Msg);

                bank->RemCash(pack_btt->UCID, atoi(pack_btt->Text));
                bank->AddCash(pack_btt->ReqI, atoi(pack_btt->Text), false);

                sprintf(Msg, "[%02d.%02d.%d, %02d:%02d] %s => %s (%s RUR)", timeStruct->tm_mday, timeStruct->tm_mon, timeStruct->tm_year, timeStruct->tm_hour, timeStruct->tm_min, players.at(pack_btt->UCID).UName, players.at(pack_btt->ReqI).UName, pack_btt->Text);
                ofstream readf (send_c, ios::app);
                readf << Msg << endl;
                readf.close();
            }
            else
                insim->SendMTC(pack_btt->UCID, msg->_( pack_btt->UCID, "1101" ));
        } // if atoi(pack_btt->Text) > 0

    }

    /**  Пользователь передает сообщение  */
    if (pack_btt->ClickID == 72)
    {
        if (strlen(pack_btt->Text) > 0)
        {

            char Msg[128];
            sprintf(Msg, msg->_( pack_btt->ReqI, "MsgFrom" ), players.at(pack_btt->UCID).PName.c_str(), pack_btt->Text );
            insim->SendMTC(pack_btt->ReqI, Msg);
            msg->AddNotify(pack_btt->ReqI, Msg);

            RCBaseClass::CCText("^1" + (string)players.at(pack_btt->UCID).UName + " ^7передал сообщение " + (string)players.at(pack_btt->ReqI).UName + ":");
            printf("%s\n", pack_btt->Text);
        }
    }

}


void case_cnl ()
{
    struct IS_CNL *pack_cnl = (struct IS_CNL*)insim->get_packet();

    RCBaseClass::CCText("^1<< disconnected " + string(players.at(pack_cnl->UCID).UName) + " (" + string(RCBaseClass::GetReason(pack_cnl->Reason) ) + ")");

    Save( pack_cnl->UCID );
    bonuses[ players.at(pack_cnl->UCID).UName ] = Json::nullValue;
    players.erase(pack_cnl->UCID);

}

void case_toc ()
{
    struct IS_TOC *pack_toc = (struct IS_TOC*)insim->get_packet();

    insim->SendMTC(pack_toc->NewUCID, "^1Access Denine");
    insim->SendMST("/spec " + string(players.at(pack_toc->NewUCID).UName));

}

void case_cpr ()
{
    struct IS_CPR *pack_cpr = (struct IS_CPR*)insim->get_packet();
    players.at(pack_cpr->UCID).PName = pack_cpr->PName;
}

void case_mci ()
{
    IS_MCI *pack_mci;

    if(ginfo->UDPPORT > 0)
        pack_mci = (struct IS_MCI*)insim->udp_get_packet();
    else
        pack_mci = (struct IS_MCI*)insim->get_packet();

    for (int i = 0; i < pack_mci->NumC; i++)
    {
        if(PLIDtoUCID.find(pack_mci->Info[i].PLID) == PLIDtoUCID.end())
            continue;

        byte UCID = PLIDtoUCID[pack_mci->Info[i].PLID];

        if(players.find(UCID) == players.end())
            continue;

        struct streets StreetInfo;
        street->CurentStreetInfo(&StreetInfo, UCID);

        int X = pack_mci->Info[i].X/65536;
        int Y = pack_mci->Info[i].Y/65536;

        int S = ((int)pack_mci->Info[i].Speed*360)/(32768);

        int LastX = players.at(UCID).Info.X/65536;
        int LastY = players.at(UCID).Info.Y/65536;

        if (LastX==0 and LastY==0)
        {
            LastX=X;
            LastY=Y;
        }
        float Dist = dl->Distance(X, Y, LastX, LastY);;

        if (Dist<50)
        {
            players.at(UCID).Distance += Dist;
            /** Bonus **/
            players.at(UCID).Bonus_dist += Dist;
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

        if (players.at(UCID).Bonus_dist > 5000)
        {
            players.at(UCID).Bonus_dist -= 5000;

            int bonus = 100+(50*(players.at(UCID).Bonus_count));
            players.at(UCID).Bonus_count +=1;

            bank->AddCash(UCID, bonus, false);
            bank->RemFrBank( bonus );

            char bonus_c[64];
            sprintf(bonus_c, msg->_( UCID, "1500" ), bonus);
            insim->SendMTC(UCID, bonus_c);
        }

        /** Zones (PitSave, shop, etc) **/
        if (bank->InBank(UCID))
            players.at(UCID).Zone = 4;
        else if ( nrg->InCafe(UCID))
            players.at(UCID).Zone = 3;
        else if ( RCBaseClass::Check_Pos(ginfo->TrackInf.PitCount, ginfo->TrackInf.XPit, ginfo->TrackInf.YPit, X, Y))
            players.at(UCID).Zone = 1;
        else if ( RCBaseClass::Check_Pos(ginfo->TrackInf.ShopCount, ginfo->TrackInf.XShop, ginfo->TrackInf.YShop, X, Y))
        {
            if (players.at(UCID).Zone!=2)
            {
                players.at(UCID).Zone = 2;
                insim->SendMTC(UCID, msg->_( UCID, "ShopDialog1" ));
                insim->SendMTC(UCID, msg->_( UCID, "ShopDialog2" ));
                insim->SendMTC(UCID, msg->_( UCID, "ShopDialog3" ));
                insim->SendMTC(UCID, msg->_( UCID, "ShopDialog4" ));
                insim->SendMTC(UCID, msg->_( UCID, "ShopDialog5" ));
            }
        }
        else
            players.at(UCID).Zone = 0;

        if (players.at(UCID).Svetofor == 1)
        {
            char TEST[64];
            sprintf(TEST, "X=%d Y=%d H=%d", X, Y, pack_mci->Info[i].Heading/182);
#ifdef _RC_PIZZA_H
            pizza->ButtonInfo(UCID, TEST);
#endif
        }
        memcpy(&players.at(UCID).Info, &pack_mci->Info[i], sizeof(CompCar));
    }
}


void case_mso ()
{
    struct IS_MSO *pack_mso = (struct IS_MSO*)insim->get_packet();

    //if (pack_mso->UCID == 0) return;

    if ( pack_mso->UserType != MSO_PREFIX )
        return;


    xString Message = pack_mso->Msg + pack_mso->TextStart;
    vector<string> args = Message.split(' ',1);

    //!help
    if (Message == "!info" or Message == "!^Cинфо")
    {
        for (int j=176; j<239; j++)
            insim->SendBFN(pack_mso->UCID, j);

        btn_info(pack_mso->UCID, 1);
        return;
    }

    if (Message == "!help" or Message == "!^Cпомощь")
    {
        cout << players.at(pack_mso->UCID).UName << " send !help" << endl;
        help_cmds(pack_mso->UCID, 1);
        return;
    }
    if (Message == "!cars" or Message == "!^Cмашины")
    {
        cout << players.at(pack_mso->UCID).UName << " send !cars" << endl;
        help_cmds(pack_mso->UCID, 2);
        return;
    }

    //!save
    if (Message == "!save")
    {
        time_t now;
        time(&now);

        if ((now - players.at(pack_mso->UCID).LastSave) < 5*3600)
        {
            insim->SendMTC(pack_mso->UCID, "^1^CНельзя так часто сохраняться");
            return;
        }
        players.at(pack_mso->UCID).LastSave = now;

        Save(pack_mso->UCID);
        return;
    }

    if (Message.find("!trans") == 0 or Message.find("!^Cпередачи") == 0 )
    {

        char file[255];
        strcpy(file, RootDir);
        strcat(file, "/logs/sends/send.txt");


        ifstream readf (file, ios::in);

        if(!readf.is_open())
        {
            cout << "Can't find " << file << endl;
            return;
        }

        if (args.size() > 1 )
        {
            while (readf.good())
            {
                char str[128];
                readf.getline(str, 128);
                if (strlen(str) > 1)
                {
                    if ( strstr(str, players.at(pack_mso->UCID).UName) and strstr(str, args[1].c_str()) )
                    {
                        char Text[64];
                        strcpy(Text, "^1| ^C^7");
                        strncat(Text, str, 110);
                        insim->SendMTC(pack_mso->UCID, Text);
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
                if (strlen(str) > 1 and strstr(str, players.at(pack_mso->UCID).UName))
                    PreCount++;
            }
            readf.close();

            insim->SendMTC(pack_mso->UCID, "^1| ^3LAST TRANSFERS:");
            ifstream readf (file, ios::in);
            while (readf.good())
            {
                char str[128];
                readf.getline(str, 128);
                if (strlen(str) > 1 and strstr(str, players.at(pack_mso->UCID).UName))
                {
                    if (PostCount >= PreCount-15)
                    {
                        char Text[96];
                        strcpy(Text, "^1| ^C^7");
                        strncat(Text, str, 55);
                        insim->SendMTC(pack_mso->UCID, Text);
                    }
                    PostCount++;
                }
            }
        }
        readf.close();
        return;
    }

    //!show
    if (Message == "!show")
    {
        char Text[64];

        sprintf(Text, "/msg ^1------------------------------");
        insim->SendMST(Text);

        sprintf(Text, "/msg ^7 %s", players.at(pack_mso->UCID).PName.c_str());
        insim->SendMST(Text);

        sprintf(Text, "/msg ^7Cash: ^1%d", bank->GetCash(pack_mso->UCID));
        insim->SendMST(Text);
#ifdef _RC_LEVEL_H
        sprintf(Text, "/msg ^7Drive Level: ^1%d", dl->GetLVL(pack_mso->UCID));
        insim->SendMST(Text);

        float nextlvl = (pow(dl->GetLVL(pack_mso->UCID), 2)*0.5+100)*1000;

        int prog = int(dl->GetSkill(pack_mso->UCID)/nextlvl*100);

        sprintf(Text, "/msg ^7Drive Skill: ^1%d%s", prog, "%");
        insim->SendMST(Text);
#endif
        for ( int j=0; j<MAX_CARS; j++)
        {
            if (strlen(players.at(pack_mso->UCID).cars[j].car)>0)
            {
                int     tune;
                char    Text[64];

                tune = players.at(pack_mso->UCID).cars[j].tuning;

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

                sprintf(Text, "/msg ^2%s ^3%4.0f ^7Km (%s^7)(^3%d%%^7)", players.at(pack_mso->UCID).cars[j].car, players.at(pack_mso->UCID).cars[j].dist/1000, Tun, tune2);
                insim->SendMST(Text);
            }
        }

        sprintf(Text, "/msg ^1------------------------------");
        insim->SendMST(Text);
        return;
    }
    //!tunning

    if (Message.find("!tun")==0)
    {
        if (players.at(pack_mso->UCID).Zone != 2)
        {
            insim->SendMTC(pack_mso->UCID, msg->_( pack_mso->UCID, "2000" ));
            return;
        }

        if ( args.size() < 2)
        {
            insim->SendMTC(pack_mso->UCID, "Error");
            return;
        }

        string tun = args[1];

        if (tun == "ECU")
        {
            int needcash = 5000 + (GetCarID(players.at(pack_mso->UCID).CName)-1)*1000;
#ifdef _RC_LEVEL_H
            int needlvl = (GetCarID(players.at(pack_mso->UCID).CName)-1)*5+1;
            if (dl->GetLVL(pack_mso->UCID) < needlvl)
            {
                char msg[64];
                sprintf(msg, "^C^2|^7 Вам нужен уровень: ^1%d", needlvl);
                insim->SendMTC(pack_mso->UCID, msg);
                return;
            }
#endif
            if (players.at(pack_mso->UCID).CTune&1)
            {
                char msg[64];
                sprintf(msg, "^C^2|^7 У вас уже есть ECU");
                insim->SendMTC(pack_mso->UCID, msg);
                return;
            }

            if (bank->GetCash(pack_mso->UCID) >= needcash)
            {
                players.at(pack_mso->UCID).CTune +=1;
                char msg[64];
                sprintf(msg, "^C^2|^7 Вы купили ECU (%d RUR.)", needcash);
                insim->SendMTC(pack_mso->UCID, msg);
                bank->RemCash(pack_mso->UCID, needcash);
                bank->AddToBank(needcash);
            }
            else
            {
                char msg[64];
                sprintf(msg, "^C^2|^7 Вам нужно %d RUR.", needcash);
                insim->SendMTC(pack_mso->UCID, msg);
            }
        }
        else if (tun == "TRB")
        {
            int needcash = 10000 + (GetCarID(players.at(pack_mso->UCID).CName)-1)*10000;
#ifdef _RC_LEVEL_H
            int needlvl = (GetCarID(players.at(pack_mso->UCID).CName)-1)*5+2;
            if (dl->GetLVL(pack_mso->UCID) < needlvl)
            {
                char msg[64];
                sprintf(msg, "^C^2|^7 Вам нужен уровень: ^1%d", needlvl);
                insim->SendMTC(pack_mso->UCID, msg);
                return;
            }
#endif
            if (players.at(pack_mso->UCID).CTune&2)
            {
                char msg[64];
                sprintf(msg, "^C^2|^7 У вас уже есть Turbo");
                insim->SendMTC(pack_mso->UCID, msg);
                return;
            }

            if (bank->GetCash(pack_mso->UCID) >= needcash)
            {
                players.at(pack_mso->UCID).CTune +=2;
                char msg[64];
                sprintf(msg, "^C^2| ^7Вы купили Turbo (%d RUR.)", needcash);
                insim->SendMTC(pack_mso->UCID, msg);
                bank->RemCash(pack_mso->UCID, needcash);
                bank->AddToBank(needcash);
            }
            else
            {
                char msg[64];
                sprintf(msg, "^C^2| ^7Вам нужно %d RUR.", needcash);
                insim->SendMTC(pack_mso->UCID, msg);
            }

        }
        else if (tun == "WHT")
        {
            int needcash = 3000 + (GetCarID(players.at(pack_mso->UCID).CName)-1)*1000;
#ifdef _RC_LEVEL_H
            int needlvl = (GetCarID(players.at(pack_mso->UCID).CName)-1)*5+3;
            if (dl->GetLVL(pack_mso->UCID) < needlvl)
            {
                char msg[64];
                sprintf(msg, "^C^2| ^7Вам нужен уровень: ^1%d", needlvl);
                insim->SendMTC(pack_mso->UCID, msg);
                return;
            }
#endif
            if (players.at(pack_mso->UCID).CTune&4)
            {
                char msg[64];
                sprintf(msg, "^C^2| ^7У вас уже установлено \"Облегчение веса\"");
                insim->SendMTC(pack_mso->UCID, msg);
                return;
            }

            if (bank->GetCash(pack_mso->UCID) >= needcash)
            {
                players.at(pack_mso->UCID).CTune+=4;
                char msg[64];
                sprintf(msg, "^C^2|^7 Вы купили \"Облегчение веса\" (%d RUR.)", needcash);
                insim->SendMTC(pack_mso->UCID, msg);
                bank->RemCash(pack_mso->UCID, needcash);
                bank->AddToBank(needcash);
            }
            else
            {
                char msg[64];
                sprintf(msg, "^C^2| ^7Вам нужно %d RUR.", needcash);
                insim->SendMTC(pack_mso->UCID, msg);
            }

        }

    }


    if (Message.find("!untun")==0)
    {
        if (players.at(pack_mso->UCID).Zone != 2)
        {
            insim->SendMTC(pack_mso->UCID, msg->_( pack_mso->UCID, "2000" ));
            return;
        }


        if ( args.size() < 2)
        {
            insim->SendMTC(pack_mso->UCID, "Error");
            return;
        }

        string tun = args[1];

        if (tun == "ECU")
        {
            int sellcash = (5000 + (GetCarID(players.at(pack_mso->UCID).CName)-1)*1000)*8/10;

            if (players.at(pack_mso->UCID).CTune&1)
            {
                players.at(pack_mso->UCID).CTune -=1;
                char msg[64];
                sprintf(msg, "^C^2| ^7Вы продали ECU (%d RUR.)", sellcash);
                insim->SendMTC(pack_mso->UCID, msg);
                bank->AddCash(pack_mso->UCID, sellcash, true);
                bank->RemFrBank(sellcash);
            }
        }
        else if (tun == "TRB")
        {
            int sellcash = (10000 + (GetCarID(players.at(pack_mso->UCID).CName)-1)*10000)*8/10;

            if (players.at(pack_mso->UCID).CTune&2)
            {
                players.at(pack_mso->UCID).CTune -=2;
                char msg[64];
                sprintf(msg, "^C^2| ^7Вы продали Turbo (%d RUR.)", sellcash);
                insim->SendMTC(pack_mso->UCID, msg);
                bank->AddCash(pack_mso->UCID, sellcash, true);
                bank->RemFrBank(sellcash);
            }
        }
        else if (tun == "WHT")
        {
            int sellcash = (3000 + (GetCarID(players.at(pack_mso->UCID).CName)-1)*1000)*8/10;

            if (players.at(pack_mso->UCID).CTune&4)
            {
                players.at(pack_mso->UCID).CTune -=4;
                char msg[64];
                sprintf(msg, "^C^2| ^7Вы продали \"Облегчение веса\" (%d RUR.)", sellcash);
                insim->SendMTC(pack_mso->UCID, msg);
                bank->AddCash(pack_mso->UCID, sellcash, true);
                bank->RemFrBank(sellcash);
            }
        }
    }
    //!buy

    if (Message.find("!buy")==0)
    {
        if (players.at(pack_mso->UCID).Zone != 2)
        {
            insim->SendMTC(pack_mso->UCID, msg->_( pack_mso->UCID, "2000" ));
            return;
        }


        if ( args.size() < 2)
        {
            insim->SendMTC(pack_mso->UCID, "Error");
            return;
        }

        const char* id = args[1].c_str();
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
            insim->SendMTC(pack_mso->UCID, "^C^2| ^7У нас нет такой машины!");
            return;
        }

#ifdef _RC_LEVEL_H
        int needlvl = (CarID-1)*5;
        if (dl->GetLVL(pack_mso->UCID) < needlvl)
        {
            char msg[64];
            sprintf(msg, "^C^2| ^7Нужен уровень: ^1%d", needlvl);
            insim->SendMTC(pack_mso->UCID, msg);
            return;
        }
#endif
        if (bank->GetCash(pack_mso->UCID) < (int)ginfo->car[CarID].cash)
        {
            char msg[64];
            sprintf(msg, "^C^2| ^7Нужно ^1%d ^7RUR.", (int)ginfo->car[CarID].cash);
            insim->SendMTC(pack_mso->UCID, msg);
            return;
        }


        for ( int j=0; j<MAX_CARS; j++)
        {
            if (strcmp(id, players.at(pack_mso->UCID).cars[j].car)== 0)
            {
                char msg[64];
                sprintf(msg, "^C^2| ^7У вас уже есть такая машина");
                insim->SendMTC(pack_mso->UCID, msg);
                return;
            }
        }

        for ( int j=0; j<MAX_CARS; j++)
        {

            if (strlen(players.at(pack_mso->UCID).cars[j].car) == 0)
            {
                strcpy(players.at(pack_mso->UCID).cars[j].car, id);
                players.at(pack_mso->UCID).cars[j].tuning=0;
                players.at(pack_mso->UCID).cars[j].dist=0;
                char msg[64];
                sprintf(msg, "^C^2|^7 Вы купили %s", id);
                insim->SendMTC(pack_mso->UCID, msg);

                bank->RemCash(pack_mso->UCID, ginfo->car[CarID].cash);
                bank->AddToBank(ginfo->car[CarID].cash);

                players.at(pack_mso->UCID).PLC += ginfo->car[CarID].PLC;
                insim->SendPLC(pack_mso->UCID, players.at(pack_mso->UCID).PLC);

                tm *timeStruct = tools::GetLocalTime();

                char log[MAX_PATH];
                sprintf(log, "%s/logs/shop/shop(%d.%d.%d).txt", RootDir, timeStruct->tm_year, timeStruct->tm_mon, timeStruct->tm_mday);

                ofstream readf (log, ios::app);
                readf << timeStruct->tm_hour << ":" << timeStruct->tm_min << ":" << timeStruct->tm_sec << " " <<  players.at(pack_mso->UCID).UName << " buy car " << id << endl;
                readf.close();

                char sql[128];
                sprintf(sql, "INSERT INTO garage  ( username, car ) VALUES ( '%s' , '%s' );", players.at(pack_mso->UCID).UName , ginfo->car[CarID].car );
                db->exec(sql );

                break;
            }

        }


    }

    if (Message.find("!sell") == 0)
    {
        if (players.at(pack_mso->UCID).Zone != 2)
        {
            insim->SendMTC(pack_mso->UCID, msg->_( pack_mso->UCID, "2000" ));
            return;
        }

        if ( args.size() < 2)
        {
            insim->SendMTC(pack_mso->UCID, "Error");
            return;
        }

        const char* id = args[1].c_str();

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
        if (id == players.at(pack_mso->UCID).CName)
        {
            insim->SendMTC(pack_mso->UCID, msg->_( pack_mso->UCID, "1009" ));
            return;
        }

        for ( int k=0; k<MAX_CARS; k++)
        {
            if (strcmp(players.at(pack_mso->UCID).cars[k].car, ginfo->car[j].car) == 0)
            {
                char msg[64];
                sprintf(msg, "^C^2|^7 Вы продали %s", id);
                insim->SendMTC(pack_mso->UCID, msg);

                strcpy(players.at(pack_mso->UCID).cars[k].car, "");
                players.at(pack_mso->UCID).cars[k].tuning=0;
                players.at(pack_mso->UCID).cars[k].dist=0;

                tm *timeStruct = tools::GetLocalTime();

                char log[MAX_PATH];
                sprintf(log, "%s/logs/shop/shop(%d.%d.%d).txt", RootDir, timeStruct->tm_year, timeStruct->tm_mon, timeStruct->tm_mday);
                ofstream readf (log, ios::app);
                readf << timeStruct->tm_hour << ":" << timeStruct->tm_min << ":" << timeStruct->tm_sec << " " <<  players.at(pack_mso->UCID).UName << " sell car " << id << endl;
                readf.close();

                bank->AddCash(pack_mso->UCID, ginfo->car[j].sell, true);
                bank->RemFrBank(ginfo->car[j].sell);

                players.at(pack_mso->UCID).PLC -= ginfo->car[j].PLC;
                insim->SendPLC(pack_mso->UCID, players.at(pack_mso->UCID).PLC);

                char sql[128];
                sprintf(sql, "DELETE FROM garage WHERE  username = '%s' AND  car = '%s'", players.at(pack_mso->UCID).UName , ginfo->car[j].car );
                db->exec(sql );
                break;
            }

        }

    }

    //!EXIT
    if (
        ( Message == "!exit" or Message == "!^Cвыход" ) and
        (strcmp(players.at(pack_mso->UCID).UName, "denis-takumi") == 0 or strcmp(players.at(pack_mso->UCID).UName, "Lexanom") == 0 or pack_mso->UCID == 0))
    {
        insim->SendMTC(255, "^1| ^3Russian Cruise: ^7^CВыключение, сохранение данных");

        SaveAll(true);

        ok=0;
        return;
    }

    if (Message == "!reload" and (strcmp(players.at(pack_mso->UCID).UName, "denis-takumi") == 0 or strcmp(players.at(pack_mso->UCID).UName, "Lexanom") == 0 or pack_mso->UCID == 0))
    {
        insim->SendMTC(255, "^1| ^3Russian Cruise: ^C^7Конфиг перезагружен");
        RCBaseClass::CCText("^7Config reload");

        insim->SendTiny(TINY_RST,255);

        return;
    }

    if (Message == "!debug")
    {
        db->debug = !db->debug;

        if (players.at(pack_mso->UCID).Svetofor == 0)
            players.at(pack_mso->UCID).Svetofor = 1;
        else
        {
#ifdef _RC_PIZZA_H
            pizza->ClearButtonInfo(pack_mso->UCID);
#endif
            players.at(pack_mso->UCID).Svetofor = 0;
        }

        return;
    }

    if (Message == "!pit" or Message == "!^Cпит")
    {
        if (bank->GetCash(pack_mso->UCID)<=250)
        {
            insim->SendMTC(pack_mso->UCID, msg->_( pack_mso->UCID, "NoManyPay" ));
            return;
        }

#ifdef _RC_POLICE_H
        if ( police->InPursuite( pack_mso->UCID ) == 1 )
        {
            char Msg[64];
            sprintf(Msg, "/pitlane %s", players.at(pack_mso->UCID).UName);
            insim->SendMST(Msg);

            int pay = 10000;

            char str[96];
            sprintf(str, msg->_(pack_mso->UCID, "2700"), pay);

            insim->SendMTC(pack_mso->UCID, str);
            bank->RemCash(pack_mso->UCID, pay);
            bank->AddToBank(pay);

            if (dl->GetSkill( pack_mso->UCID ) > 10 )
                dl->RemSkill(pack_mso->UCID, 10);

            return;
        }
#endif
        char Msg[64];
        sprintf(Msg, "/pitlane %s", players.at(pack_mso->UCID).UName);
        insim->SendMST(Msg);
        players.at(pack_mso->UCID).Zone = 1;
        bank->RemCash(pack_mso->UCID, 500);
        bank->AddToBank(500);
        taxi->PassLoss(pack_mso->UCID);

        return;
    }

    //!users
    if (Message == "!users" or Message == "!^Cнарод")
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

        if (players.at(pack_mso->UCID).FloodTime == 0)
            players.at(pack_mso->UCID).FloodTime = ftime;

        int ts = ftime - players.at(pack_mso->UCID).FloodTime;
        players.at(pack_mso->UCID).FloodTime = ftime;


        if (ts < 5)
            players.at(pack_mso->UCID).FloodCount++;
        else
            players.at(pack_mso->UCID).FloodCount = 1;

        if (players.at(pack_mso->UCID).FloodCount > 4)   //max lines to tolerate
        {
            int pay = 1000;

            char str[96];
            sprintf(str, msg->_(pack_mso->UCID, "flood"), pay);
            insim->SendMTC(pack_mso->UCID, str);

            players.at(pack_mso->UCID).FloodCount = 0;
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
                insim->SendMTC(pack_mso->UCID, str);

                bank->RemCash(pack_mso->UCID, pay);
                bank->AddToBank(pay);
            }

        }
        strcpy( players.at(pack_mso->UCID).Msg, Msg);
    }
}

void case_ncn ()
{
    struct IS_NCN *pack_ncn = (struct IS_NCN*)insim->get_packet();

    if (pack_ncn->UCID == 0)
        return;

    if (pack_ncn->ReqI == 0)
        RCBaseClass::CCText("^4>> connected " + (string)pack_ncn->UName);

    // Copy all the player data we need into the players.at(] array
    strcpy(players[pack_ncn->UCID].UName, pack_ncn->UName);
    players[pack_ncn->UCID].PName = pack_ncn->PName;

    players[pack_ncn->UCID].Zone = 1;

    read_user_cars(pack_ncn->UCID);

    //help_cmds(&players.at(pack_ncn->UCID), 1);
    insim->SendMTC(pack_ncn->UCID, msg->_( pack_ncn->UCID, "Help1" ));
    insim->SendMTC(pack_ncn->UCID, msg->_( pack_ncn->UCID, "Help14" ));
}

void case_npl ()
{
    char specText[64];
    struct IS_NPL *pack_npl = (struct IS_NPL*)insim->get_packet();

    PLIDtoUCID[pack_npl->PLID] = pack_npl->UCID;

    sprintf(specText, "/spec %s", players.at(pack_npl->UCID).UName);

    if (pack_npl->PType != 6)
    {
       insim->SendBFNAll( pack_npl->UCID );

        players.at(pack_npl->UCID).H_TRes =  pack_npl->H_TRes;
        players.at(pack_npl->UCID).SetF =  pack_npl->SetF;

        int j=0;
        for (j=0; j<MAX_CARS; j++)
        {
            if (strcmp(players.at(pack_npl->UCID).cars[j].car, pack_npl->CName)==0)
                break;
        }

        if ( j != MAX_CARS)
        {
            players.at(pack_npl->UCID).CName = pack_npl->CName;
            players.at(pack_npl->UCID).CTune = players.at(pack_npl->UCID).cars[j].tuning;
            players.at(pack_npl->UCID).Distance = players.at(pack_npl->UCID).cars[j].dist;

            int tune = 45;
            if (players.at(pack_npl->UCID).CTune&1)
            {
                tune -= 15;
            }
            if (players.at(pack_npl->UCID).CTune&2)
            {
                tune -= 20;
            }
            if (players.at(pack_npl->UCID).CTune&4)
            {
                tune -= 10;
            }

#ifdef _RC_LEVEL_H
            int needlvl = (GetCarID(players.at(pack_npl->UCID).CName)-1)*5;

            if (dl->GetLVL(pack_npl->UCID) < needlvl)
            {
                players.at(pack_npl->UCID).Zone = 1;
                insim->SendMST( specText );

                char msg2[64];
                sprintf(msg2, "^C^1|^7 Нужен уровень: %d", needlvl);
                insim->SendMTC(pack_npl->UCID, msg2);
                insim->SendMTC(pack_npl->UCID, msg->_( pack_npl->UCID, "2404" ));

                char Text2[64];
                strcpy(Text2, "^1| ^2");
                for (int k=0; k< MAX_CARS; k++)
                {
                    if ((strlen(players.at(pack_npl->UCID).cars[k].car) != 0) and ((GetCarID(players.at(pack_npl->UCID).cars[k].car)-1)*5 <= dl->GetLVL(pack_npl->UCID)))
                    {
                        strcat(Text2, players.at(pack_npl->UCID).cars[k].car);
                        strcat(Text2, " ");
                    }
                }
                insim->SendMTC(pack_npl->UCID, Text2);
                return;
            }
            else if (pack_npl->H_TRes < tune)
            {
                players.at(pack_npl->UCID).Zone = 1;
                insim->SendMST( specText );

                char Texxt[32];
                sprintf(Texxt, "%s %d %%", msg->_( pack_npl->UCID, "2400" ), tune);
                insim->SendMTC(pack_npl->UCID, Texxt);
            }
#endif
        }
        else
        {
            insim->SendMTC(pack_npl->UCID, msg->_( pack_npl->UCID, "2404" ));
            help_cmds(pack_npl->UCID, 2);
            players.at(pack_npl->UCID).Zone = 1;
            insim->SendMST( specText );
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

    memset(&players.at(UCID).Info, 0, sizeof(CompCar));

    save_car(UCID);
#ifdef _RC_POLICE_H
    if ( police->InPursuite( UCID ) == 1 )
    {
        int pay = 10000;

        char str[96];
        sprintf(str, msg->_(UCID, "2600"), pay);
        insim->SendMTC(UCID, str);


        bank->RemCash(UCID, pay);
        bank->AddToBank(pay);

        if ( dl->GetSkill( UCID ) > 10 )
            dl->RemSkill( UCID, 10);
    }
    else
    {
#endif
        if (players.at(UCID).Zone != 1)
        {
            int pay = 1000;

            char str[96];
            sprintf(str, msg->_(UCID, "2602"), pay);
            insim->SendMTC(UCID, str);

            bank->RemCash(UCID, pay);
            bank->AddToBank(pay);
        }
#ifdef _RC_POLICE_H
    }
#endif
    players.at(UCID).Zone = 1;
    PLIDtoUCID.erase(pack_pll->PLID);
}

void case_plp ()
{
     struct IS_PLP *pack_plp = (struct IS_PLP*)insim->get_packet();

    // Find player and set his PLID to 0
    byte UCID = PLIDtoUCID[pack_plp->PLID];

    memset(&players.at(UCID).Info, 0, sizeof(CompCar));

    save_car(UCID);
#ifdef _RC_POLICE_H
    if ( police->InPursuite( UCID ) == 1 )
    {
        int pay = 10000;

        char str[96];
        sprintf(str, msg->_(UCID, "2700"), pay);
        insim->SendMTC(UCID, str);

        bank->RemCash(UCID, pay);
        bank->AddToBank(pay);

        if ( dl->GetSkill( UCID ) > 10 )
            dl->RemSkill(UCID, 10);
    }
    else
    {
#endif
        if (players.at(UCID).Zone != 1)
        {
            int pay = 1000;

            char str[96];
            sprintf(str, msg->_(UCID, "2702"), pay);
            insim->SendMTC(UCID, str);

            bank->RemCash(UCID, pay);
            bank->AddToBank(pay);
        }
#ifdef _RC_POLICE_H
    }
#endif
    players.at(UCID).Zone = 1;
    PLIDtoUCID.erase(pack_plp->PLID);
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

    insim->SendMST("/cv");
}

void ShowUsersList(byte UCID)
{
    for (int i=21; i<80; i++)
        insim->SendBFN(UCID, i);

    byte count = 0, L = 0, T = 0;
    for (auto& plr: players)
    {
        // Viva La Костыль
        if(plr.first == 0 || plr.second.PName.empty())
            continue;

        if (count == 24)
        {
            L += 22;
            T = 0;
        }

        insim->SendButton(plr.first, UCID, 21+count++, 1 + L, 191 - 4*T++, 22, 4, 16 + 8, players.at(plr.first).PName);
    }
    insim->SendButton(255, UCID, 69, 1, 195, count > 24 ? 44 : 22, 4, 16 + 8, msg->_( UCID, "604" ));
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

    insim->SendTiny(TINY_RST,255);
    insim->SendTiny(TINY_NCN,255);
    insim->SendTiny(TINY_NPL,255);

    return 1;
}

int core_reconnect(void *pack_ver)
{
    insim->isclose();

    cout << "wait 1 minute and reconnect \n";
    #ifdef CIS_LINUX
    sleep(60);
    #else
    Sleep(60000);
    #endif

    memset(pack_ver, 0, sizeof(struct IS_VER));
    struct IS_VER *pack_v = (IS_VER*)pack_ver;

    if (insim->init (ginfo->IP, ginfo->TCPPORT, IS_PRODUCT_NAME, ginfo->ADMIN, pack_v, '!', isf_flag, 500, ginfo->UDPPORT) < 0)
    {
        cout << "\n * Error during initialization * " << endl;
        return -1;
    }

    strcpy(ginfo->Product, pack_v->Product);

    insim->SendTiny(TINY_RST,255);
    insim->SendTiny(TINY_NCN,255);
    insim->SendTiny(TINY_NPL,255);

    return 1;
}

void read_track()
{
    char file[255];
    sprintf(file, "%s/data/RCCore/tracks/%s.txt" , RootDir , ginfo->Track);

    ifstream readf (file, ios::in);

    if(!readf.is_open())
    {
        cout << "Can't find " << file << endl;
        return;
    }

    memset(&ginfo->TrackInf, 0, sizeof(track_info));

    while (readf.good())
    {
        char str[128];
        readf.getline(str, 128);

        if (strstr(str, "//")) {}
        if (strlen(str) > 1)
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
    sprintf(file, "%s/misc/cars.txt" , RootDir);

    carMap.clear();

    ifstream readf (file, ios::in);

    if(!readf.is_open())
    {
        cout << "Can't find " << file << endl;
        return;
    }

    int i = 0;
    while (readf.good())
    {
        char str[128];
        readf.getline(str, 128);
        if (strlen(str) > 1)
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
    sprintf(file, "%s/misc/%s.cfg", RootDir, ServiceName);

    ifstream readf (file, ios::in);

    if(!readf.is_open())
    {
        cout << "Can't find " << file << endl;
        return;
    }

    char str[128];
    while (readf.good())
    {
        readf.getline(str, 128);
        if (strlen(str) > 1)
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

void ReadBonuses()
{
    RCBaseClass::CCText("^3Restore bonus");

    string filename = string(RootDir) + "/bonuses" + ServiceName + ".json";

    if(!RCBaseClass::FileExists(filename))
    {
        RCBaseClass::CCText("^1Bonus file not found " + filename);
        return;
    }

    ifstream file;

    file.open(filename, ios::binary);

    if( !file.is_open() )
    {
        RCBaseClass::CCText("^1Failed to open bonuses file");
        return;
    }

    bool readed = bonusesReader.parse( file, bonuses, false );

    if ( !readed )
    {
        file.close();
        // report to the user the failure and their locations in the document.
        cout << "Failed to parse bonuses\n" << bonusesReader.getFormattedErrorMessages();
        return;
    }
    file.close();

    for(auto& pl:players)
    {
        byte UCID = pl.first;

        Json::Value bonus = bonuses[players.at(UCID).UName];

        if( bonus.isObject() )
        {
            players.at(UCID).Bonus_count = bonus["count"].asInt();
            players.at(UCID).Bonus_dist = bonus["dist"].asFloat();
        }

        if (players.at(UCID).Bonus_count > 0)
            insim->SendMTC(UCID, msg->_(UCID, "> ^1BONUS RESTORED"));
    }

    remove(filename.c_str());
    bonuses.clear();
    return;
}

void SaveBonuses()
{
    string filename = string(RootDir) + "/bonuses" + ServiceName + ".json";

    ofstream f;
    f.open(filename, ios::out);
    f << bonusesWriter.write( bonuses );
    f.close();
}

/*********************************************/

void *ThreadMci (void *params)
{
    while (true)
    {
        if (insim->udp_next_packet() < 0)
            continue;

        try
        {
            case_mci ();

            for( cl = classes.begin(); cl != classes.end(); ++cl )
            {
                (*cl)->upd_next_packet();
            }
        }
        catch(const logic_error& lerror)
        {
            RCBaseClass::CCText(string("^1MCI ERROR: ") + lerror.what() );
        }


    }
    return 0;
};

void *ThreadSave (void *params)
{
    time_t seconds;

    int my_reconnect = 0;

    while (ok > 0)
    {
        seconds = time (NULL);
        if (seconds % 600 == 0) //every 10 minute
        {
            try
            {
                SaveAll(true);
            }
            catch(const logic_error& lerror)
            {
                RCBaseClass::CCText(string("^1SAVE ERROR: ") + lerror.what() );
            }


            int my_ping = 0;
            if( my_ping = db->ping() != 0 )
            {
                RCBaseClass::CCText( db->getError() );

                if (my_reconnect > 2)
                {
                    if ( db->connect( conf.host, conf.port, conf.user, conf.password, conf.database) == false )
                    {
                        RCBaseClass::CCText("^3RCMain: ^1Can't connect to MySQL server");
                        RCBaseClass::CCText( db->getError() );

                    }
                    else
                    {
                        RCBaseClass::CCText("^3RCMain:\t^2Connected to MySQL server");
                        my_reconnect = 0;
                    }
                }
            }
            else
                my_reconnect = 0;
        }
        #ifdef CIS_LINUX
        usleep(500 * 1000); // 0.5 sec
        #else
        Sleep(500);
        #endif

#ifdef _RC_POLICE_H
        police->SetSirenLight("^4||||||||||^7|^1||||||||||");
#endif
        #ifdef CIS_LINUX
        usleep(500 * 1000); // 0.5 sec
        #else
        Sleep(500);
        #endif


#ifdef _RC_POLICE_H
        police->SetSirenLight( "^1||||||||||^7|^4||||||||||" );
#endif

    }
    return 0;
};

void *ThreadEvent (void *params)
{
    #ifdef CIS_LINUX
    sleep(5);
    #else
    Sleep(5000);
    #endif
    ReadBonuses();

    while (ok > 0)
    {
        try
        {
            msg->Event();

            for( cl = classes.begin(); cl != classes.end(); ++cl )
                (*cl)->Event();

            Event();
        }
        catch(const logic_error& lerror)
        {
            RCBaseClass::CCText(string("^1EVENT ERROR: ") + lerror.what() );
        }
        #ifdef __linux__
        /* Выводим список пользователей и машину на которой они катаются */
        printf("\e[s\e[0;0H");
        RCBaseClass::CCText(string("^6") + IS_PRODUCT_NAME + string("\e[K"));
        RCBaseClass::CCText("^1Players: " + RCBaseClass::ToString( players.size() ) + "\e[K",false);

        RCBaseClass::CCText("|================================|================================|\e[K",false);
        for (int i = 0; i <= 24; ++i)
        {
            RCBaseClass::CCText("|                                |                                |\e[K",false);
        }
        RCBaseClass::CCText("|================================|================================|\e[K",false);

        int start_line = 3;
        int line = 0;

        printf("\e[%d;2H",start_line);
        for(auto pl:players)
        {
            if( ++line > 24 )
            {
                printf("\e[%d;34H",start_line + line - 24);
            }
            else
            {
                printf("\e[%d;2H",start_line + line);
            }
            RCBaseClass::CCText( string(players.at(pl.first).UName) + " (" + string(players.at(pl.first).CName) + ")",false);
                // ") Zone: " + RCBaseClass::ToString(players.at(pl.first).Zone) +
                // " X: " + RCBaseClass::ToString(players.at(pl.first).Info.X/65536) +
                // " Y: " + RCBaseClass::ToString(players.at(pl.first).Info.Y/65536));
        }

        printf("\e[u");
        #endif // __linux__
        #ifdef CIS_LINUX
        usleep(500 * 1000); // 0.5 sec
        #else
        Sleep(500);
        #endif
    }
    return 0;
};

void signal_handler(int sig)
{
    RCBaseClass::CCText("^1| ^3Russian Cruise: ^7Exit with code: " + RCBaseClass::ToString(sig));
    insim->SendMTC(255, "^1| ^3Russian Cruise: ^7^CВыключение, сохранение данных");

    SaveAll(true);
    ok=0;

    string extMsg = "curl -d 'text=Russian Cruise: Exit with code - " + RCBaseClass::ToString(sig) + "' http://sms.ru/sms/send\\?api_id=284979d3-59e5-2a24-a934-1955fdd046a9\\&to=79035530332";
    system( extMsg.c_str() );
    exit(sig);
}

int main(int argc, char* argv[])
{
    #ifdef CIS_LINUX
   signal(SIGINT,signal_handler); // ctrl+c
   signal(SIGKILL,signal_handler); // kill
   signal(SIGQUIT,signal_handler); // ctrl + \
   signal(SIGTERM,signal_handler); // kill
   signal(SIGTSTP,signal_handler); // ctrl + z
   #endif // CIS_LINUX

    if( argc < 2 )
    {
        cout << "ERROR!!! Need params" << endl;
        cout << "Use like RCMod CruiseS2" << endl;
        return -1;
    }

    isf_flag = ISF_MCI + ISF_CON + ISF_OBH + ISF_HLV + ISF_AXM_EDIT + ISF_AXM_LOAD;

    string p = argv[0];
    size_t pos = p.rfind('/');

    strcpy(RootDir, p.substr(0,pos).c_str());
    strcpy(ServiceName, argv[1]);

    if (strlen(ServiceName) == 0)
    {
        RCBaseClass::CCText("^3RCMain:\t^C^1Не задан файл конфигурации");
        return 0;
    }




    char path[MAX_PATH];
    memset(&path,0,MAX_PATH);
    sprintf(path, "%s/misc/%s_mysql.cfg", RootDir, ServiceName);
    tools::read_mysql(path, conf);

    db = new DBMySQL();

    while ( db->connect(conf.host, conf.port, conf.user, conf.password, conf.database) == false )
    {
        RCBaseClass::CCText("^3RCMain: ^1Can't connect to MySQL server");
        RCBaseClass::CCText(db->getError());
        #ifdef CIS_LINUX
        sleep(5); // 5 sec
        #else
        Sleep(5000);
        #endif
    }
    RCBaseClass::CCText("^3RCMain:\t^2Connected to MySQL server");

    sprintf(IS_PRODUCT_NAME, "RC-%.15s", AutoVersion::RC_UBUNTU_VERSION_STYLE);

    insim = new CInsim();
    ginfo = new GlobalInfo();

    read_cfg();

    IS_VER *pack_ver = new IS_VER();

    if ( core_connect(pack_ver) < 0 )
        for (;;)
            if ( core_reconnect(pack_ver) > 0 )
                break;

    if (pack_ver->InSimVer != 5)
    {
        RCBaseClass::CCText("^3RCMain:\t^1INSIM VER != 5");
        return -1;
    }

    players.clear();

    CreateClasses();
    InitClasses();

    pthread_t mci_tid; // Thread ID
    pthread_t event_tid; // Thread ID
    pthread_t save_tid; // Thread ID

    if (pthread_create(&event_tid, NULL, ThreadEvent, NULL) < 0)
    {
        printf("^3RCMain:\t\tCan't start `thread_work` Thread\n");
        return 0;
    }

    if (pthread_create(&save_tid, NULL, ThreadSave, NULL) < 0)
    {
        RCBaseClass::CCText("^3RCMain:\t\t^1Can't start `thread_save` Thread\n");
        return 0;
    }

    if(ginfo->UDPPORT > 0)
    {
        if (pthread_create(&mci_tid, NULL, ThreadMci, NULL) < 0)
        {
            RCBaseClass::CCText("^3RCMain:\t\t^1Can't start `thread_mci` Thread\n");
            return 0;
        }
    }

    RCBaseClass::CCText("^7Cruise started");

    while (ok > 0)
    {
        if (insim->next_packet() < 0)
            for (;;)
                if ( core_reconnect(pack_ver) > 0 )
                    break;

        msg->next_packet(); // обрабатывается первым, из-за того что потом е выводятся сообщения. приоритет емае

        try
        {
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

            case ISP_MCI:
                case_mci();
                break;
            }

           for( cl = classes.begin(); cl != classes.end(); ++cl )
           {
                (*cl)->next_packet();
           }
        }
        catch(const logic_error& lerror)
        {
            RCBaseClass::CCText(string("^1 ERROR: ") + lerror.what() );
        }

    }

    if (insim->isclose() < 0)
        return 0;

    delete insim;
    delete ginfo;

    return 0;
}
