#include "RCMain.h"
#include "version.h"
#include "tools.h"

int ok = 1;
GlobalInfo *ginfo;

struct IS_VER pack_ver;
word	isf_flag;

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

#ifdef _RC_BANLIST_H
RCBanList banlist;
#endif

#ifdef _RC_POLICE_H
RCPolice *police;
#endif // _RC_POLICE_H

void create_classes()
{
    msg = new RCMessage();
    bank = new RCBank();

#ifdef _RC_ENERGY_H
    nrg = new RCEnergy();
#endif

#ifdef _RC_LEVEL_H
    dl = new RCDL();
#endif

#ifdef _RC_CHEAT_H
    antcht = new RCAntCheat();
#endif

#ifdef _RC_STREET_H
    street = new RCStreet();
#endif

#ifdef _RC_LIGHT_H
    lgh = new RCLight();
#endif

#ifdef _RC_PIZZA_H
    pizza = new RCPizza();
#endif

#ifdef _RC_TAXI_H
    taxi = new RCTaxi();
#endif

#ifdef _RC_BANLIST_H
    //banlist.init(RootDir, insim);
#endif

#ifdef _RC_POLICE_H
police = new RCPolice();
#endif // _RC_POLICE_H

}

void init_classes()
{
    msg->init(RootDir,insim);

    bank->init(RootDir, insim, msg, dl);

#ifdef _RC_ENERGY_H
    nrg->init(RootDir, insim, msg, bank);
#endif

#ifdef _RC_LEVEL_H
    dl->init(RootDir, insim, msg);
#endif

#ifdef _RC_CHEAT_H
    antcht->init(RootDir,insim, msg);
#endif

#ifdef _RC_STREET_H
    street->init(RootDir,insim, msg);
#endif

#ifdef _RC_LIGHT_H
    lgh->init(RootDir,insim, msg,dl);
#endif

#ifdef _RC_PIZZA_H
    pizza->init(RootDir, insim, msg, bank, nrg, dl);
#endif

#ifdef _RC_TAXI_H
    taxi->init(RootDir, insim, msg, bank, dl, street);
#endif

#ifdef _RC_BANLIST_H
    banlist.init(RootDir, insim);
#endif
#ifdef _RC_POLICE_H
	police->init(RootDir, insim, msg, bank, dl, street);
#endif // _RC_POLICE_H
}

void readconfigs()
{
#ifdef _RC_PIZZA_H
    pizza->readconfig(ginfo->Track);
#endif
    msg->readconfig(ginfo->Track);
#ifdef _RC_ENERGY_H
    nrg->readconfig(ginfo->Track);
#endif

#ifdef _RC_BANK_H
    bank->readconfig(ginfo->Track);
#endif

#ifdef _RC_CHEAT_H
    // antcht->readconfig(ginfo->Track);
#endif

#ifdef _RC_STREET_H
    street->readconfig(ginfo->Track);
#endif

#ifdef _RC_LIGHT_H
    lgh->readconfig(ginfo->Track);
#endif

#ifdef _RC_TAXI_H
    taxi->readconfig(ginfo->Track);
#endif
}

int GetCarID(char *CarName)
{
    if (strlen(CarName)!=3)
        return 0;

    for (int i=0; i< MAX_CARS; i++)
    {
        if (strcmp(CarName,ginfo->car[i].car)==0)
            return ginfo->car[i].id;
    }

    return 0;
}

int IfCop (struct player *splayer)
{
    char PlayerName[32];
    strcpy(PlayerName,splayer->PName);

    int COP = 0;

    if (
        ((strncmp("^4[^C^7ДПС^4]",PlayerName,13)==0)
         || (strncmp("^4[^C^7ДПC^4]",PlayerName,13)==0)
         || (strncmp("^4[^C^7дпс^4]",PlayerName,13)==0)
         || (strncmp("^4[^C^7дпc^4]",PlayerName,13)==0)
         || (strncmp("^4[^C^7ГАИ^4]",PlayerName,13)==0)
         || (strncmp("^4[^C^7гаи^4]",PlayerName,13)==0)
         || (strncmp("^4[^C^7ГAИ^4]",PlayerName,13)==0)
         || (strncmp("^4[^C^7гaи^4]",PlayerName,13)==0))
        && (splayer->cop != 1)
    )
    {
        COP += 1;
    }

    if ( read_cop(splayer) > 0)
    {
        COP += 2;
    }

    return COP;
}

user_car AddUserCar(const char *car = "UF1", float dist =0, int tuning = 0)
{
    user_car c;
    strcpy(c.car, car);
    c.dist = dist;
    c.tuning = tuning;
    return c;
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
    sprintf(file,"%smisc\\words.txt", RootDir);

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        out << "Can't find " << file << endl;
        return;
    }
    FindClose(fff);

    ifstream readf (file,ios::in);
    int i=0;
    while (readf.good())
    {
        char str[32];
        readf.getline(str,32);

        if (strlen(str) > 0)
        {
            strncpy(ginfo->Words[i],str,strlen(str));
            i++;
        }
    }
    ginfo->WordsCount = i;

}

void save_user_cars (struct player *splayer)
{
    char sql[128];

    if( mysql_ping( &rcMaindb ) != 0 )
        printf("Bank Error: connection with MySQL server was lost\n");

    for (int i = 0; i < MAX_CARS; i++)
    {
        if ( strlen( splayer->cars[i].car ) > 0 )
        {
            sprintf(sql,"UPDATE garage SET tuning = %d, dist = %f WHERE car = '%s' AND username = '%s';", splayer->cars[i].tuning , splayer->cars[i].dist , splayer->cars[i].car , splayer->UName );
            if( mysql_query( &rcMaindb , sql) != 0 )
                printf("Bank Error: MySQL Query Save\n");
        }
    }
}



void read_user_cars(struct player *splayer)
{
    char query[128];
    sprintf(query,"SELECT car, tuning, dist FROM garage WHERE username='%s';",splayer->UName);

    if( mysql_ping( &rcMaindb ) != 0 )
    {
        printf("Error: connection with MySQL server was lost\n");
        // произвести кик пользователя чтоль
    }

    if( mysql_query( &rcMaindb , query) != 0 )
    {
        printf("Error: MySQL Query\n");
        // произвести кик пользователя чтоль
    }

    rcMainRes = mysql_store_result(&rcMaindb);
    if(rcMainRes == NULL)
        printf("Error: can't get the result description\n");

    if( mysql_num_rows( rcMainRes ) > 0 )
    {
        int i = 0;
        while ( ( rcMainRow = mysql_fetch_row( rcMainRes ) ) )
        {
            strcpy(splayer->cars[i].car,rcMainRow[0]);
            splayer->cars[i].tuning = atoi( rcMainRow[1] );
            splayer->cars[i].dist = atof( rcMainRow[2] );
            /** map<> **/
            splayer->PLC += ginfo->carMap[ rcMainRow[0] ].PLC;
            i++;
        }

    }
    else
    {
        printf("Can't find %s\n Create user\n",splayer->UName);

        sprintf(query,"INSERT INTO garage (username, car ) VALUES ('%s' , 'UF1');",splayer->UName);

        if( mysql_ping( &rcMaindb ) != 0 )
        {
            printf("Error: connection with MySQL server was lost\n");
        }

        if( mysql_query( &rcMaindb , query) != 0 )
        {
            printf("Error: MySQL Query\n");
        }

        strcpy(splayer->cars[0].car,"UF1");
        splayer->cars[0].tuning = 0;
        splayer->cars[0].dist = 0;

        splayer->PLC += ginfo->carMap[ "UF1" ].PLC;

        save_user_cars(splayer);

    }
    mysql_free_result( rcMainRes );
    send_plc(splayer->UCID,splayer->PLC);
}

void save_car (struct player *splayer)
{

    out <<splayer->UName << " save " << splayer->CName << " info" << endl;

    for (int i=0; i<MAX_CARS; i++)
    {
        if (strcmp(splayer->CName,splayer->cars[i].car)==0)
        {
            splayer->cars[i].tuning = splayer->CTune;
            splayer->cars[i].dist = (int)splayer->Distance;
            break;
        }
    }

}



void help_cmds (struct player *splayer,int h_type)
{
    if (h_type == 1)
    {
        for ( int i = 3200 ; i <= 3218 ; i ++ )
            send_mtc(splayer->UCID,msg->GetMessage(splayer->UCID,i));
    }
    if (h_type == 2)
    {
        send_mtc(splayer->UCID,msg->GetMessage(splayer->UCID,3100));
        for ( int i=0; i<MAX_CARS; i++)
        {
            if (strlen(splayer->cars[i].car)>0)
            {
                int     tune;
                char    Text[64];

                tune = splayer->cars[i].tuning;

                char Tun[30];

                int tune2 = 45;
                if (tune&1)
                {
                    strcpy( Tun,"^2E");
                    tune2 -= 15;
                }
                else
                {
                    strcpy( Tun,"^1E");
                }

                if (tune&2)
                {
                    strcat( Tun," ^2T");
                    tune2 -= 20;
                }
                else
                {
                    strcat( Tun," ^1T");
                }

                if (tune&4)
                {
                    strcat( Tun," ^2W");
                    tune2 -= 10;
                }
                else
                {
                    strcat( Tun," ^1W");
                }

                sprintf(Text,"^1| ^C^2%s ^3%4.0f ^7Km (%s^7)(^3%d%%^7)",splayer->cars[i].car,splayer->cars[i].dist/1000,Tun,tune2);
                send_mtc(splayer->UCID,Text);
            }
        }

    }

}



/**** buttons ****/
void btn_main (struct player *splayer)
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;

    /* background */
    pack.BStyle = 16+64;
    pack.ClickID = 100;
    pack.L = 100-126/2;
    pack.T = 25;
    pack.W = 126;
    pack.H = 150;
    strcpy(pack.Text,"");
    insim->send_packet(&pack);
    /* background */

    /* Title */
    pack.BStyle = 0;
    pack.ClickID = 101;
    pack.L = 101-126/2;
    pack.T = 26;
    pack.W = 126;
    pack.H = 20;
    strcpy(pack.Text,"^7RUSSIAN CRUISE");
    insim->send_packet(&pack);
    /* Title */

    /* background #2 */
    pack.BStyle = 16+32;
    pack.ClickID = 102;
    pack.L = 101-126/2;
    pack.T = 55;
    pack.W = 124;
    pack.H = 118;
    strcpy(pack.Text,"");
    insim->send_packet(&pack);
    /* background #2 */

}

void btn_info (struct player *splayer, int b_type)
{
    btn_main(splayer);

    char info_text[5][100];
    sprintf(info_text[0],"^7RUSSIAN CRUISE v %d.%d.%d",(int)AutoVersion::RC_MAJOR,(int)AutoVersion::RC_MINOR,(int)AutoVersion::RC_BUILD);
    strncpy(info_text[1], "^C^7 Добро пожаловать, ",99);
    strncpy(info_text[2], "^C^7 Деньги: ^2",99);
    strncpy(info_text[3], "^C^7   тут жизненный статус",99);
    strncpy(info_text[4], "^C^7Доступные автомобили:",99);

    char about_text[10][100];
    sprintf(about_text[0],"^7RUSSIAN CRUISE v %d.%d.%d",(int)AutoVersion::RC_MAJOR,(int)AutoVersion::RC_MINOR,(int)AutoVersion::RC_BUILD);
    strncpy(about_text[1], "^C^7Developers: Denis Kostin, Alexandr Mochalov",99);
    strncpy(about_text[2], "^C^7Jabber: denis-kostin@jabber.ru",99);
    strncpy(about_text[3], "^C^7Jabber conference: lfs@conference.jabber.ru",99);
    strncpy(about_text[4], "^7",99);
    strncpy(about_text[5], "^C^7More information",99);
    strncpy(about_text[6], "^C^7http://vk.com/russiancruise",99);
    strncpy(about_text[7], "^7",99);
    strncpy(about_text[8], "^C^7        Thanks:",99);
    strncpy(about_text[9], "^C^3        repeat, nose, D.Matsugin, R.Ratzinger",99);

    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;

    /* Tabs */
    pack.BStyle = 32+8;
    pack.ClickID = 103;
    pack.L = 101-126/2;
    pack.T = 46;
    pack.W = 16;
    pack.H = 8;
    strcpy(pack.Text,msg->GetMessage(splayer->UCID,200));
    insim->send_packet(&pack);

    pack.BStyle = 32+8;
    pack.ClickID = 106;
    pack.L += 17;
    strcpy(pack.Text,msg->GetMessage(splayer->UCID,201));
    insim->send_packet(&pack);

    pack.BStyle = 32+8;
    pack.ClickID = 105;
    pack.L += 17;
    pack.W = 20;
    strcpy(pack.Text,msg->GetMessage(splayer->UCID,202));
    insim->send_packet(&pack);

    /* Tabs */

    if (b_type == 1)
    {
        for (int i=2; i<MAX_CARS/2; i++)
        {
            if ( ginfo->car[i].id == 0 )
                continue;

            pack.L = (101-126/2)+1;
            pack.BStyle = 16 + 64;
            pack.T = 50+6*(i-1)+1;
            pack.W = 50;
            pack.H = 6;
            pack.ClickID = 110 + i;
            sprintf(pack.Text,"^3 %d Level ^2%s ^7(^2%d^7/^3%d^7)",(i-1)*5,ginfo->car[i].car,(int)ginfo->car[i].cash,(int)ginfo->car[i].sell);
            insim->send_packet(&pack);
        }

        for (int i=MAX_CARS/2; i<MAX_CARS; i++)
        {
            if ( ginfo->car[i].id == 0 )
                continue;

            pack.L = (101-126/2)+1+51;
            pack.BStyle = 16 + 64;
            pack.T = 50+6*(i-MAX_CARS/2+1)+1;
            pack.W = 50;
            pack.H = 6;
            pack.ClickID = 110 + i;
            sprintf(pack.Text,"^3 %d Level ^2%s ^7(^2%d^7/^3%d^7)",(i-1)*5,ginfo->car[i].car,(int)ginfo->car[i].cash,(int)ginfo->car[i].sell);
            insim->send_packet(&pack);
        }

    } // if (type == 1)

    if (b_type == 2)
    {

    }

    if (b_type == 3)
    {
        pack.BStyle = 64;
        pack.L = 101-126/2;
        pack.W = 90;
        for (int g=0; g<10; g++)
        {
            pack.ClickID = 110 + g;
            pack.T += 9;
            strcpy(pack.Text,about_text[g]);
            insim->send_packet(&pack);
        }
    }
/*
    if (b_type == 4)
    {
        int fineID = 0;
        for (int i=0; i<MAX_FINES; i++)
        {
            pack.L = (101-126/2)+1;
            pack.BStyle = 16 + 64;
            if (ginfo->fines[i].id != 0)
            {
                fineID ++;
                pack.T = 56+6*(fineID-1);
                pack.W = 122;
                pack.H = 6;
                pack.ClickID = 110 + fineID;
                sprintf(pack.Text,"^7ID = %d. %s ^3(^2%d RUR.^3)",ginfo->fines[i].id,ginfo->fines[i].name,ginfo->fines[i].cash);
                insim->send_packet(&pack);
            }

        }

    }
*/
    pack.BStyle = 8;
    pack.ClickID = 149;
    pack.L = 99+126/2-8;
    pack.T = 26;
    pack.W = 8;
    pack.H = 8;
    strcpy(pack.Text,"^1[X]");
    insim->send_packet(&pack);
}



void btn_panel (struct player *splayer)
{
    /****************************/
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.BStyle = 1;
    pack.ClickID = 54;
    pack.BStyle = 32;
    pack.L = 85;
    pack.T = 1;
    pack.W = 15;
    pack.H = 8;

    if (splayer->Zone== 1)
        strcpy(pack.Text,msg->GetMessage(splayer->UCID,400));
    else if (splayer->Zone== 2)
        strcpy(pack.Text,msg->GetMessage(splayer->UCID,401));
    else if (splayer->Zone== 3)
        strcpy(pack.Text,msg->GetMessage(splayer->UCID,402));
    else if (splayer->Zone== 4)
        strcpy(pack.Text,msg->GetMessage(splayer->UCID,404));
    else
        strcpy(pack.Text,msg->GetMessage(splayer->UCID,403));
    insim->send_packet(&pack);

    //
    pack.ClickID = 55;
    pack.L = 70;
    pack.T = 5;
    pack.W = 15;
    pack.H = 4;

    sprintf(pack.Text,"^2%4.3f Km.",splayer->Distance/1000);


    insim->send_packet(&pack);

}





void btn_sirena(struct player *splayer)
{
    //int Heith = 30;
    //int Left = 100 - Heith/2;

    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.ClickID = 203;
    pack.BStyle = 1;
    //pack.L = 50;

    pack.T = 20;
    pack.W = 124 - (splayer->sirenaSize);
    pack.L = 100 - pack.W/2;
    pack.H = pack.W/3;

    if (pack.W <= 0)
        pack.W = 1;

    if (pack.L <= 0)
        pack.L = 1;

    if (pack.H <= 0)
        pack.H = 1;

    strcpy(pack.Text,siren);
    insim->send_packet(&pack);
}

void btn_pogonya(struct player *splayer)
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.ClickID = 204;
    pack.BStyle = 1;
    pack.L = 50;
    pack.T = 51;
    pack.W = 100;
    pack.H = 30;
    strcpy(pack.Text,splayer->PogonyaReason);
    insim->send_packet(&pack);
}

void btn_work (struct player *splayer)
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.ClickID = 210;
    pack.BStyle = 32;
    pack.L = 136;
    pack.T = 1;
    pack.W = 10;
    pack.H = 8;

    int time2 = splayer->WorkTime - time(&stime);
    int min;
    int sec;
    min = time2/60;
    sec = time2%60;
    sprintf(pack.Text,"^2%02d:%02d",min,sec);

    insim->send_packet(&pack);

}

/****************************************/
void case_bfn ()
{
    struct IS_BFN *pack_bfn = (struct IS_BFN*)insim->get_packet();

    int i;
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo->players[i].UCID == pack_bfn->UCID)
        {
            time_t now;
            time(&now);

            if((now - ginfo->players[i].LastBFN) < 10)
            {
                send_mtc(ginfo->players[i].UCID,"^1^CНельзя так часто жать кнопки");
                return;
            }

            ginfo->players[i].LastBFN = now;

            if (ginfo->players[i].bfn == 0)
            {
                ginfo->players[i].bfn = 1;
                btn_info(&ginfo->players[i],1);
            }
            else
            {
                ginfo->players[i].bfn = 0;
            }
            break;
        }
    }
}

void case_btc ()
{
    pthread_mutex_lock (&RCmutex);
    struct IS_BTC *pack_btc = (struct IS_BTC*)insim->get_packet();

    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo->players[i].UCID == pack_btc->UCID)
        {
            SYSTEMTIME sm;
            GetLocalTime(&sm);
            char log[255];
            sprintf(log,"%slogs\\shop\\shop(%d.%d.%d).txt",RootDir,sm.wYear,sm.wMonth,sm.wDay);
            /**
            Пользователь кликнул по другому пользователю
            */
            if (pack_btc->ClickID<=32)
            {
                ginfo->players[i].BID2 =  pack_btc->ClickID;

                if (ginfo->players[i].Action == 1) // send
                {
                    struct IS_BTN pack_btn;
                    memset(&pack_btn, 0, sizeof(struct IS_BTN));
                    pack_btn.Size = sizeof(struct IS_BTN);
                    pack_btn.Type = ISP_BTN;
                    pack_btn.ReqI = ginfo->players[i].UCID;              // Must be non-zero, I'll just use UCID
                    pack_btn.UCID = ginfo->players[i].UCID;              // UCID of the player that will receive the button
                    pack_btn.BStyle = 16 + ISB_CLICK;                 // Dark frame for window title
                    pack_btn.TypeIn = 16;
                    pack_btn.L = 49;
                    pack_btn.T = 175;
                    pack_btn.W = 24;
                    pack_btn.H = 4;
                    pack_btn.ClickID = 36;
                    strcpy(pack_btn.Text,msg->GetMessage(ginfo->players[i].UCID,1000));
                    insim->send_packet(&pack_btn);

                    pack_btn.TypeIn = 63;
                    pack_btn.T += 4;
                    pack_btn.ClickID = 37;
                    strcpy(pack_btn.Text, msg->GetMessage(ginfo->players[i].UCID,1001));
                    insim->send_packet(&pack_btn);

                    // cop buttons
                    if (ginfo->players[i].cop != 0)
                    {
                        pack_btn.BStyle = 32 + ISB_CLICK;
                        pack_btn.TypeIn = 2;
                        pack_btn.T += 4;
                        pack_btn.ClickID = 38;
                        strcpy(pack_btn.Text, msg->GetMessage(ginfo->players[i].UCID,1002));
                        insim->send_packet(&pack_btn);

                        pack_btn.T += 4;
                        pack_btn.ClickID = 39;
                        strcpy(pack_btn.Text, msg->GetMessage(ginfo->players[i].UCID,1003));
                        insim->send_packet(&pack_btn);

                        pack_btn.TypeIn = 0;
                        pack_btn.T += 4;
                        pack_btn.ClickID = 40;
                        strcpy(pack_btn.Text, msg->GetMessage(ginfo->players[i].UCID,1004));
                        insim->send_packet(&pack_btn);

                        pack_btn.T += 4;
                        pack_btn.ClickID = 41;
                        strcpy(pack_btn.Text, msg->GetMessage(ginfo->players[i].UCID,1005));
                        insim->send_packet(&pack_btn);
                    }
                }
            }

            /**
            Скрыть кнопки с пользователями
            */
            if (pack_btc->ClickID==34)
            {
                for (int j=0; j<50; j++)
                    send_bfn(ginfo->players[i].UCID,j);
            }

            /**
            Включаем погоню
            */
            if (pack_btc->ClickID==40)
            {
                for (int g=0; g<MAX_PLAYERS; g++)
                {
                    if  (ginfo->players[i].BID2 == ginfo->players[g].BID)
                    {
                        if (ginfo->players[g].Pogonya == 0)
                        {
                            ginfo->players[g].Pogonya = 1;
                            int worktime = time(&stime);
                            ginfo->players[g].WorkTime = worktime+60*6;
                            strcpy(ginfo->players[g].PogonyaReason,msg->GetMessage(ginfo->players[g].UCID,1006));
                            char Text[96];
                            sprintf(Text,"/msg ^2| %s %s", msg->GetMessage(ginfo->players[g].UCID,1007) , ginfo->players[g].PName );
                            send_mst(Text);
                            nrg->Lock(ginfo->players[g].UCID);

                            char fine_c[255];
                            sprintf(fine_c,"%slogs\\cop\\pursuit(%d.%d.%d).txt",RootDir,sm.wYear,sm.wMonth,sm.wDay);
                            ofstream readf (fine_c,ios::app);
                            readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  ginfo->players[i].UName << " begin pursuit to "  << ginfo->players[g].UName << endl;
                            readf.close();
                        }
                        break;
                    }
                }
                for (int g=0; g<MAX_PLAYERS; g++)
                {
                    if (ginfo->players[g].cop != 0)
                    {
                        for (int k=60; k<79; k++)
                            send_bfn(ginfo->players[g].UCID,k);
                    }
                }

            }

            /**
            Выключаем погоню
            */
            if (pack_btc->ClickID==41)
            {

                for (int g=0; g<MAX_PLAYERS; g++)
                {
                    if  (ginfo->players[i].BID2 == ginfo->players[g].BID)
                    {
                        if (ginfo->players[g].Pogonya != 0)
                        {
                            ginfo->players[g].Pogonya = 0;
                            send_bfn(ginfo->players[g].UCID,210);
                            char Text[96];
                            sprintf(Text,"/msg ^2| %s %s", msg->GetMessage(ginfo->players[g].UCID,1008) , ginfo->players[g].PName );
                            send_mst(Text);
                            nrg->Unlock(ginfo->players[g].UCID);
                        }
                        break;
                    }
                }

                for (int g=0; g<MAX_PLAYERS; g++)
                {
                    if (ginfo->players[g].cop != 0)
                    {
                        for (int k=60; k<79; k++)
                            send_bfn(ginfo->players[g].UCID,k);
                    }
                }
            }

            /**
            Информационные кнопки
            **/
            if (pack_btc->ClickID == 149)
            {
                ginfo->players[i].bfn=0;
                for (int j=159; j>0; j--)
                    send_bfn(pack_btc->UCID,j);
            }

            if (pack_btc->ClickID == 103)
            {
                for (int j=0; j<50; j++)
                    send_bfn(pack_btc->UCID,100+j);
                btn_info(&ginfo->players[i],1);
            }

            if (pack_btc->ClickID == 104)
            {
                for (int j=0; j<50; j++)
                    send_bfn(pack_btc->UCID,100+j);

                btn_info(&ginfo->players[i],2);
            }

            if (pack_btc->ClickID == 105)
            {
                for (int j=0; j<50; j++)
                    send_bfn(pack_btc->UCID,100+j);

                btn_info(&ginfo->players[i],3);
            }
            if (pack_btc->ClickID == 106)
            {
                for (int j=0; j<50; j++)
                    send_bfn(pack_btc->UCID,100+j);

                btn_info(&ginfo->players[i],4);
            }

            /**
            Не помню. Возможно на удаление
            */
            if (pack_btc->ClickID == 200)
            {
                for (int j=0; j<5; j++)
                    send_bfn(pack_btc->UCID,200+j);
            }

            break;
        } // if UCID
    }// for
    pthread_mutex_unlock (&RCmutex);
}

void case_btt ()
{
    pthread_mutex_lock (&RCmutex);
    struct IS_BTT *pack_btt = (struct IS_BTT*)insim->get_packet();

    SYSTEMTIME sm;
    GetLocalTime(&sm);

    char send_c[255];
    sprintf(send_c,"%slogs\\sends\\send.txt",RootDir,sm.wYear,sm.wMonth,sm.wDay);
    char fine_c[255];
    sprintf(fine_c,"%slogs\\fines\\fine(%d.%d.%d).txt",RootDir,sm.wYear,sm.wMonth,sm.wDay);


    int i;
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo->players[i].UCID == pack_btt->UCID)
        {
            /**
            Пользователь передает деньги
            */
            if (pack_btt->ClickID==36)
            {
                for (int g=0; g<MAX_PLAYERS; g++)
                {
                    if  (ginfo->players[i].BID2 == ginfo->players[g].BID)
                    {
                        if (atoi(pack_btt->Text) > 0)
                        {
                            if (bank->GetCash(ginfo->players[i].UCID) > atoi(pack_btt->Text))
                            {
                                out << ginfo->players[i].UName << " send " << pack_btt->Text << " to "  << ginfo->players[g].UName << endl;
                                bank->RemCash(ginfo->players[i].UCID,atoi(pack_btt->Text));
                                bank->AddCash(ginfo->players[g].UCID,atoi(pack_btt->Text), true);

                                char Msg[120];
                                sprintf(Msg,"^1| %s%s%d RUR.",ginfo->players[i].PName,msg->GetMessage(ginfo->players[i].UCID,1100),atoi(pack_btt->Text));
                                send_mtc(ginfo->players[g].UCID,Msg);

                                ofstream readf (send_c,ios::app);
                                readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << ":" << sm.wMilliseconds << " " <<  ginfo->players[i].UName << " send " << pack_btt->Text << " RUR. to "  << ginfo->players[g].UName << endl;
                                readf.close();
                            }
                            else
                            {
                                send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,1101));
                                send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,1101));
                            }
                        } // if atoi(pack_btt->Text) > 0
                        break;
                    }
                }//for
            }

            /**
            Пользователь передает сообщение
            */
            if (pack_btt->ClickID == 37)
            {

                for (int g=0; g<MAX_PLAYERS; g++)
                {
                    if  (ginfo->players[i].BID2 == ginfo->players[g].BID)
                    {
                        if (strlen(pack_btt->Text) > 0)
                        {
                            out << ginfo->players[i].UName << " send " << pack_btt->Text << " to "  << ginfo->players[g].UName << endl;
                            char Msg[127];
                            strcpy(Msg,msg->GetMessage(ginfo->players[g].UCID,1103));
                            strcat(Msg,ginfo->players[i].PName);
                            strcat(Msg,": ^1");
                            int len = strlen(Msg);
                            int can = (127-len);
                            strncat(Msg,pack_btt->Text,can);
                            send_mtc(ginfo->players[g].UCID,Msg);
                        } // if atoi(pack_btt->Text) > 0
                        break;
                    }
                }//for
            }
		}
    }
    pthread_mutex_unlock (&RCmutex);
}


void case_cnl ()
{
    int i;

    struct IS_CNL *pack_cnl = (struct IS_CNL*)insim->get_packet();

    // Find player and set the whole player struct he was using to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo->players[i].UCID == pack_cnl->UCID)
        {
            save_user_cars(&ginfo->players[i]);

            #ifdef _RC_POLICE_H
			police->SaveUserFines( ginfo->players[i].UCID );
            #endif // _RC_POLICE_H


            out << ginfo->players[i].UName << " left server" << endl;
            ginfo->players[i].cars2.clear();
            memset(&ginfo->players[i],0,sizeof(struct player));
            break;
        }
    }
}

void case_toc ()
{
    int i;

    struct IS_TOC *pack_toc = (struct IS_TOC*)insim->get_packet();

    // Find player and set the whole player struct he was using to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo->players[i].UCID == pack_toc->NewUCID)
        {
            char Text[64];
            strcpy(Text, "/spec ");
            strcat (Text, ginfo->players[i].UName);
            send_mtc(pack_toc->NewUCID,"^1Acces Denine");
            send_mst(Text);
            break;
        }
    }
}

void case_cpr ()
{
    struct IS_CPR *pack_cpr = (struct IS_CPR*)insim->get_packet();

    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo->players[i].UCID == pack_cpr->UCID)
        {
            out << ginfo->players[i].PName << " rename to " << pack_cpr->PName << endl;
            strcpy(ginfo->players[i].PName, pack_cpr->PName);
            break;
        }
    }
}


void case_flg ()
{
    struct IS_FLG *pack_flg = (struct IS_FLG*)insim->get_packet();

    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo->players[i].PLID == pack_flg->PLID)
        {
            break;
        }
    }
}

void case_lap ()
{
    struct IS_LAP *pack_lap = (struct IS_LAP*)insim->get_packet();

    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo->players[i].PLID == pack_lap->PLID)
        {
            break;
        }

    }
}

void case_mci ()
{
    struct IS_MCI *pack_mci = (struct IS_MCI*)insim->udp_get_packet();


    for (int i = 0; i < pack_mci->NumC; i++)

    {
        for (int j =0; j < MAX_PLAYERS; j++)
        {

            if (pack_mci->Info[i].PLID == ginfo->players[j].PLID and ginfo->players[j].PLID != 0 and ginfo->players[j].UCID != 0)

            {
                struct streets StreetInfo;
                street->CurentStreetInfo(&StreetInfo,ginfo->players[j].UCID);

                int X = pack_mci->Info[i].X/65536;
                int Y = pack_mci->Info[i].Y/65536;

                int S = ((int)pack_mci->Info[i].Speed*360)/(32768);

                int LastX = ginfo->players[j].Info.X/65536;
                int LastY = ginfo->players[j].Info.Y/65536;

                if (LastX==0 and LastY==0)
                {
                    LastX=X;
                    LastY=Y;
                }
                float Dist = dl->Distance(X,Y,LastX,LastY);

                if ((abs((int)Dist) > 10) and (S>30))
                {
                    ginfo->players[j].Distance += Dist;
                    /** Bonus **/
                    ginfo->players[j].Bonus_dist += Dist;

                    //начисляем деньги только до 20 уровня
                    if(dl->GetLVL(ginfo->players[j].UCID) < 20)
                    {

                        if (S <= StreetInfo.SpeedLimit)
                        {
                            if ( dl->Islocked( ginfo->players[j].UCID ) )
                                dl->Unlock( ginfo->players[j].UCID );
                            bank->AddCash(ginfo->players[j].UCID,abs((int)Dist)/10, false);
                            bank->RemFrBank(abs((int)Dist)/100);
                        }
                        else
                        {
                            if ( !( dl->Islocked( ginfo->players[j].UCID ) ) )
                                dl->Lock( ginfo->players[j].UCID );
                        }

                    }
                }

                /** Bonus **/

                if (ginfo->players[j].Bonus_dist > 5000)
                {
                    ginfo->players[j].Bonus_dist -= 5000;

                    int bonus = 100+(50*(ginfo->players[j].Bonus_count));
                    ginfo->players[j].Bonus_count +=1;

                    bank->AddCash(ginfo->players[j].UCID,bonus, true);
                    //dl->AddSkill(ginfo->players[j].UCID);
                    //bank->BankFond -= bonus;

                    char bonus_c[64];
                    sprintf(bonus_c,"%s%d ^7RUR.",msg->GetMessage(ginfo->players[j].UCID,1500),bonus);
                    send_mtc(ginfo->players[j].UCID,bonus_c);

                }

                /** Zones (PitSave, shop, etc) **/
                if(bank->InBank(ginfo->players[j].UCID))
                    ginfo->players[j].Zone = 4;
                else    if (ginfo->players[j].Pitlane)
                    ginfo->players[j].Zone = 1;
                else if (dl->Check_Pos(ginfo->TrackInf.ShopCount,ginfo->TrackInf.XShop,ginfo->TrackInf.YShop,X,Y))
                    ginfo->players[j].Zone = 2;

                else
                    ginfo->players[j].Zone = 0;

                if (ginfo->players[j].Svetofor == 1)
                {
                    char TEST[64];
                    sprintf(TEST,"X=%d Y=%d H=%d",X,Y,pack_mci->Info[i].Heading/182);
                    pizza->btn_information(ginfo->players[j].UCID,TEST);
                }
                memcpy(&ginfo->players[j].Info, &pack_mci->Info[i],sizeof(CompCar));

            } // if pack_mci->Info[i].PLID == ginfo->players[j].PLID
        }
    }
}

void case_mci_cop ()
{
    struct IS_MCI *pack_mci = (struct IS_MCI*)insim->udp_get_packet();

    for (int i = 0; i < pack_mci->NumC; i++)

    {
        for (int j =0; j < MAX_PLAYERS; j++)
        {
            if (pack_mci->Info[i].PLID == ginfo->players[j].PLID and ginfo->players[j].PLID != 0 and ginfo->players[j].UCID != 0)
            {
                int S = ginfo->players[j].Info.Speed*360/32768;
                /** автоотключение радара **/
                if (S < 5)
                    ginfo->players[j].StopTime ++;
                else
                {
                    ginfo->players[j].StopTime = 0;
                    if (ginfo->players[j].radar ==1 )
                    {
                        send_mtc(ginfo->players[j].UCID,msg->GetMessage(ginfo->players[j].UCID,1700));
                        ginfo->players[j].radar = 0;
                    }
                }

                if (ginfo->players[j].cop == 1)
                {
                    for (int g =0; g < MAX_PLAYERS; g++)
                    {
                        if (ginfo->players[g].PLID !=0)
                        {
                            int X = ginfo->players[g].Info.X/65536;
                            int Y = ginfo->players[g].Info.Y/65536;
                            int X1 = ginfo->players[j].Info.X/65536;
                            int Y1 = ginfo->players[j].Info.Y/65536;

                            int Rast = dl->Distance( X, Y, X1, Y1);

                            if (ginfo->players[g].Pogonya == 1)
                            {
                                if ( (Rast < 10) and (ginfo->players[g].cop != 1))
                                {
                                    int S2 = ginfo->players[g].Info.Speed*360/32768;

                                    if ((S2 < 5) and (ginfo->players[g].StopTime > 4))
                                    {

                                        ginfo->players[g].Pogonya = 2;
                                        nrg->Unlock(ginfo->players[g].UCID);
                                        strcpy(ginfo->players[g].PogonyaReason,msg->GetMessage(ginfo->players[g].UCID,1701));

                                        char Text[96];
                                        sprintf(Text,"/msg ^2| %s%s", ginfo->players[g].PName, msg->GetMessage(ginfo->players[j].UCID,1702));
                                        send_mst(Text);

                                        send_mtc(ginfo->players[j].UCID,msg->GetMessage(ginfo->players[j].UCID,1703));
                                    }
                                }
                            } // pogonya
                            /**
                            РАДАР
                            */
                            if (ginfo->players[j].radar ==1 )
                            {
                                if ((Rast < 50 ) and (ginfo->players[g].cop != 1))
                                {
                                    int Speed = ginfo->players[g].Info.Speed*360/32768;
                                    struct streets StreetInfo;
                                    street->CurentStreetInfo(&StreetInfo,ginfo->players[g].UCID);

                                    if ((Speed > StreetInfo.SpeedLimit+10) )
                                    {
                                        char text[64];
                                        int Speed2 = Speed - StreetInfo.SpeedLimit;
                                        sprintf(text,"^2| %s%s%d%s",ginfo->players[g].PName,msg->GetMessage(ginfo->players[g].UCID,1704),Speed2,msg->GetMessage(ginfo->players[j].UCID,1705));
                                        send_mtc(ginfo->players[j].UCID,text);

                                        if (ginfo->players[g].Pogonya == 0)
                                        {
                                            ginfo->players[g].Pogonya = 1;
                                            int worktime = time(&stime);
                                            ginfo->players[g].WorkTime = worktime+60*6;
                                            strcpy(ginfo->players[g].PogonyaReason,msg->GetMessage(ginfo->players[g].UCID,1006));
                                            char Text[96];
                                            sprintf(Text,"/msg ^2| %s%s", msg->GetMessage(ginfo->players[g].UCID,1007) , ginfo->players[g].PName );
                                            send_mst(Text);
                                            nrg->Lock(ginfo->players[g].UCID);
                                        }
                                    }
                                }
                            }
                            /**
                            ЛЮСТРА
                            */
                            if (ginfo->players[j].sirena ==1)
                            {
                                if ( (Rast < 120) and (ginfo->players[g].cop != 1) )
                                {
                                    ginfo->players[g].sirenaOnOff = 1;
                                    ginfo->players[g].sirenaKey = 1;
                                    ginfo->players[g].sirenaSize = Rast;
                                }
                                else
                                {
                                    ginfo->players[g].sirenaOnOff = 0;
                                }
                                ginfo->players[j].sirenaKey = 1;
                                ginfo->players[j].sirenaOnOff = 1;

                                if ( ginfo->players[j].cop == 1 )
                                    ginfo->players[j].sirenaSize = 90;
                                else
                                    ginfo->players[j].sirenaSize = 0;
                            }
                            else
                            {
                                ginfo->players[g].sirenaOnOff = 0;
                                ginfo->players[j].sirenaOnOff = 0;
                            }

                        }
                    }
                }

                if ((ginfo->players[j].sirenaOnOff == 0) and (ginfo->players[j].sirenaKey == 1))
                {
                    ginfo->players[j].sirenaKey = 0;
                    send_bfn(ginfo->players[j].UCID,203);
                }

                if ( ginfo->players[j].sirenaOnOff == 1)
                    btn_sirena(&ginfo->players[j]);


                if ((ginfo->players[j].Pogonya == 0) and (strlen(ginfo->players[j].PogonyaReason) > 1))
                {
                    strcpy(ginfo->players[j].PogonyaReason,"");
                    send_bfn(ginfo->players[j].UCID,204);
                }

                if ( ginfo->players[j].Pogonya != 0)
                    btn_pogonya(&ginfo->players[j]);

            } // if CompCar->PLID == PLID
        }
    }
}


void case_mso ()
{
    int i;

    struct IS_MSO *pack_mso = (struct IS_MSO*)insim->get_packet();

    // The chat GetMessage is sent by the host, don't do anything
    if (pack_mso->UCID == 0)
        return;

    if ( pack_mso->UserType != MSO_PREFIX )
        return;

    // Find the player that wrote in the chat
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo->players[i].UCID == pack_mso->UCID)
        {
            break;
        }
    }

    char Msg[128];
    strcpy(Msg,pack_mso->Msg + ((unsigned char)pack_mso->TextStart));


    //!help
    if (((strncmp(Msg, "!info", 5) == 0) or (strncmp(Msg, "!^Cинфо", 7) == 0)) and !bank->InBank(ginfo->players[i].UCID))
    {
        for (int j=159; j>0; j--)
        {
            send_bfn(ginfo->players[i].UCID,j);
        }
        btn_info(&ginfo->players[i],1);
    }

    if ((strncmp(Msg, "!help", 5) == 0) or (strncmp(Msg, "!^Cпомощь", 9) == 0))
    {
        out << ginfo->players[i].UName << " send !help" << endl;
        help_cmds(&ginfo->players[i],1);
    }
    if ((strncmp(Msg, "!cars", 5) == 0) or (strncmp(Msg, "!^Cмашины", 9) == 0))
    {
        out << ginfo->players[i].UName << " send !cars" << endl;
        help_cmds(&ginfo->players[i],2);
    }

    //!save
    if (strncmp(Msg, "!save", 5) == 0)
    {
        time_t now;
        time(&now);

        if((now - ginfo->players[i].LastSave) < 5*3600)
        {
            send_mtc(ginfo->players[i].UCID,"^1^CНельзя так часто сохраняться");
            return;
        }
        ginfo->players[i].LastSave = now;
        out << ginfo->players[i].UName << " send !save" << endl;
        save_car(&ginfo->players[i]);
        save_user_cars(&ginfo->players[i]);

        #ifdef _RC_POLICE_H
		police->SaveUserFines( ginfo->players[i].UCID );
		#endif // _RC_POLICE_H

        bank->bank_save(ginfo->players[i].UCID);
        send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,3000));

    }

    if ((strncmp(Msg, "!trans", 6) == 0 ) or (strncmp(Msg, "!^Cпередачи", 11) == 0))
    {
        char GetMessage[128];
        strcpy(GetMessage,Msg);

        char file[255];
        strcpy(file,RootDir);
        strcat(file,"logs\\sends\\send.txt");

        HANDLE fff;
        WIN32_FIND_DATA fd;
        fff = FindFirstFile(file,&fd);
        if (fff == INVALID_HANDLE_VALUE)
        {
            out << "Can't find " << file << endl;
            return;
        }
        FindClose(fff);

        ifstream readf (file,ios::in);

        char * comand;
        char * user;
        comand = strtok (GetMessage," ");
        user = strtok (NULL," ");

        if ((user) and (strlen(user) > 0))
        {
            while (readf.good())
            {
                char str[128];
                readf.getline(str,128);
                if (strlen(str) > 0)
                {
                    if ((strstr(str,ginfo->players[i].UName)) and (strstr(str,user)))
                    {
                        char Text[64];
                        strcpy(Text,"^1| ^C^7");
                        strncat(Text,str,55);
                        send_mtc(ginfo->players[i].UCID,Text);
                    }
                }
            }

        }
        else
        {
            while (readf.good())
            {
                char str[128];
                readf.getline(str,128);
                if (strlen(str) > 0)
                {
                    if (strstr(str,ginfo->players[i].UName))
                    {
                        char Text[64];
                        strcpy(Text,"^1| ^C^7");
                        strncat(Text,str,55);
                        send_mtc(ginfo->players[i].UCID,Text);
                    }
                }
            }
        }
        readf.close();
    }

    //!show
    if (strncmp(Msg, "!show", 5) == 0)
    {
        char Text[64];

        sprintf(Text,"/msg ^1------------------------------");
        send_mst(Text);

        sprintf(Text,"/msg ^7 %s",ginfo->players[i].PName);
        send_mst(Text);

        sprintf(Text,"/msg ^7Cash: ^1%d",bank->GetCash(ginfo->players[i].UCID));
        send_mst(Text);
#ifdef _RC_LEVEL_H
        sprintf(Text,"/msg ^7Drive Level: ^1%d",dl->GetLVL(ginfo->players[i].UCID));
        send_mst(Text);

        float nextlvl = (pow(dl->GetLVL(ginfo->players[i].UCID),2)*0.5+100)*1000;

        int prog = int(dl->GetSkill(ginfo->players[i].UCID)/nextlvl*100);

        sprintf(Text,"/msg ^7Drive Skill: ^1%d%s",prog,"%");
        send_mst(Text);
#endif
        for ( int j=0; j<MAX_CARS; j++)
        {
            if (strlen(ginfo->players[i].cars[j].car)>0)
            {
                int     tune;
                char    Text[64];

                tune = ginfo->players[i].cars[j].tuning;

                char Tun[30];

                int tune2 = 45;
                if (tune&1)
                {
                    strcpy( Tun,"^2E");
                    tune2 -= 15;
                }
                else
                {
                    strcpy( Tun,"^1E");
                }

                if (tune&2)
                {
                    strcat( Tun," ^2T");
                    tune2 -= 20;
                }
                else
                {
                    strcat( Tun," ^1T");
                }

                if (tune&4)
                {
                    strcat( Tun," ^2W");
                    tune2 -= 10;
                }
                else
                {
                    strcat( Tun," ^1W");
                }

                sprintf(Text,"/msg ^2%s ^3%4.0f ^7Km (%s^7)(^3%d%%^7)",ginfo->players[i].cars[j].car,ginfo->players[i].cars[j].dist/1000,Tun,tune2);
                send_mst(Text);
            }
        }

        sprintf(Text,"/msg ^1------------------------------");
        send_mst(Text);
    }
    //!tunning

    if (strncmp(Msg, "!tun", 4) == 0)
    {
        if (ginfo->players[i].Zone != 2)
        {
            send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2000));
            return;
        }
        char * comand;
        char * id;

        comand = strtok (Msg," ");
        id = strtok (NULL," ");

        if ((!id) or (strlen(id) != 3))
        {
            send_mtc(ginfo->players[i].UCID,"Error");
            return;
        }


        if (strcmp(id,"ECU")==0)
        {
            int needcash = 5000 + (GetCarID(ginfo->players[i].CName)-1)*1000;
#ifdef _RC_LEVEL_H
            int needlvl = (GetCarID(ginfo->players[i].CName)-1)*5+1;
            if (dl->GetLVL(ginfo->players[i].UCID) < needlvl)
            {
                char msg[64];
                sprintf(msg,"^C^2|^7 Вам нужен уровень: ^1%d",needlvl);
                send_mtc(ginfo->players[i].UCID,msg);
                return;
            }
#endif
            if (ginfo->players[i].CTune&1)
            {
                char msg[64];
                sprintf(msg,"^C^2|^7 У вас уже есть ECU");
                send_mtc(ginfo->players[i].UCID,msg);
                return;
            }

            if (bank->GetCash(ginfo->players[i].UCID) >= needcash)
            {
                ginfo->players[i].CTune +=1;
                char msg[64];
                sprintf(msg,"^C^2|^7 Вы купили ECU (%d RUR.)",needcash);
                send_mtc(ginfo->players[i].UCID,msg);
                bank->RemCash(ginfo->players[i].UCID,needcash);
                bank->AddToBank(needcash);
            }
            else
            {
                char msg[64];
                sprintf(msg,"^C^2|^7 Вам нужно %d RUR.",needcash);
                send_mtc(ginfo->players[i].UCID,msg);
            }
        }
        else if (strcmp(id,"TRB")==0)
        {
            int needcash = 10000 + (GetCarID(ginfo->players[i].CName)-1)*10000;
#ifdef _RC_LEVEL_H
            int needlvl = (GetCarID(ginfo->players[i].CName)-1)*5+2;
            if (dl->GetLVL(ginfo->players[i].UCID) < needlvl)
            {
                char msg[64];
                sprintf(msg,"^C^2|^7 Вам нужен уровень: ^1%d",needlvl);
                send_mtc(ginfo->players[i].UCID,msg);
                return;
            }
#endif
            if (ginfo->players[i].CTune&2)
            {
                char msg[64];
                sprintf(msg,"^C^2|^7 У вас уже есть Turbo");
                send_mtc(ginfo->players[i].UCID,msg);
                return;
            }

            if (bank->GetCash(ginfo->players[i].UCID) >= needcash)
            {
                ginfo->players[i].CTune +=2;
                char msg[64];
                sprintf(msg,"^C^2| ^7Вы купили Turbo (%d RUR.)",needcash);
                send_mtc(ginfo->players[i].UCID,msg);
                bank->RemCash(ginfo->players[i].UCID,needcash);
                bank->AddToBank(needcash);
            }
            else
            {
                char msg[64];
                sprintf(msg,"^C^2| ^7Вам нужно %d RUR.",needcash);
                send_mtc(ginfo->players[i].UCID,msg);
            }

        }
        else if (strcmp(id,"WHT")==0)
        {
            int needcash = 3000 + (GetCarID(ginfo->players[i].CName)-1)*1000;
#ifdef _RC_LEVEL_H
            int needlvl = (GetCarID(ginfo->players[i].CName)-1)*5+3;
            if (dl->GetLVL(ginfo->players[i].UCID) < needlvl)
            {
                char msg[64];
                sprintf(msg,"^C^2| ^7Вам нужен уровень: ^1%d",needlvl);
                send_mtc(ginfo->players[i].UCID,msg);
                return;
            }
#endif
            if (ginfo->players[i].CTune&4)
            {
                char msg[64];
                sprintf(msg,"^C^2| ^7У вас уже установлено \"Облегчение веса\"");
                send_mtc(ginfo->players[i].UCID,msg);
                return;
            }

            if (bank->GetCash(ginfo->players[i].UCID) >= needcash)
            {
                ginfo->players[i].CTune+=4;
                char msg[64];
                sprintf(msg,"^C^2|^7 Вы купили \"Облегчение веса\" (%d RUR.)",needcash);
                send_mtc(ginfo->players[i].UCID,msg);
                bank->RemCash(ginfo->players[i].UCID,needcash);
                bank->AddToBank(needcash);
            }
            else
            {
                char msg[64];
                sprintf(msg,"^C^2| ^7Вам нужно %d RUR.",needcash);
                send_mtc(ginfo->players[i].UCID,msg);
            }

        }

    }


    if (strncmp(Msg, "!untun", 6) == 0)
    {
        if (ginfo->players[i].Zone != 2)
        {
            send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2000));
            return;
        }
        char * comand;
        char * id;

        comand = strtok (Msg," ");
        id = strtok (NULL," ");

        if ((!id) or (strlen(id) != 3))
        {
            send_mtc(ginfo->players[i].UCID,"Error");
            return;
        }

        if (strcmp(id,"ECU")==0)
        {
            int sellcash = (5000 + (GetCarID(ginfo->players[i].CName)-1)*1000)*8/10;

            if (ginfo->players[i].CTune&1)
            {
                ginfo->players[i].CTune -=1;
                char msg[64];
                sprintf(msg,"^C^2| ^7Вы продали ECU (%d RUR.)",sellcash);
                send_mtc(ginfo->players[i].UCID,msg);
                bank->AddCash(ginfo->players[i].UCID,sellcash, true);
                bank->RemFrBank(sellcash);
            }
        }

        else if (strcmp(id,"TRB")==0)
        {
            int sellcash = (10000 + (GetCarID(ginfo->players[i].CName)-1)*10000)*8/10;

            if (ginfo->players[i].CTune&2)
            {
                ginfo->players[i].CTune -=2;
                char msg[64];
                sprintf(msg,"^C^2| ^7Вы продали Turbo (%d RUR.)",sellcash);
                send_mtc(ginfo->players[i].UCID,msg);
                bank->AddCash(ginfo->players[i].UCID,sellcash, true);
                bank->RemFrBank(sellcash);
            }
        }

        else if (strcmp(id,"WHT")==0)
        {
            int sellcash = (3000 + (GetCarID(ginfo->players[i].CName)-1)*1000)*8/10;

            if (ginfo->players[i].CTune&4)
            {
                ginfo->players[i].CTune -=4;
                char msg[64];
                sprintf(msg,"^C^2| ^7Вы продали \"Облегчение веса\" (%d RUR.)",sellcash);
                send_mtc(ginfo->players[i].UCID,msg);
                bank->AddCash(ginfo->players[i].UCID,sellcash, true);
                bank->RemFrBank(sellcash);
            }
        }
    }
    //!buy

    if (strncmp(Msg, "!buy", 4) == 0)
    {
        if (ginfo->players[i].Zone != 2)
        {
            send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2000));
            return;
        }
        char * comand;
        char * id;

        comand = strtok (Msg," ");
        id = strtok (NULL," ");

        if ((!id) or (strlen(id) != 3))
        {
            send_mtc(ginfo->players[i].UCID,"Error");
            return;
        }

        int CarID;

        for (CarID = 0; CarID < MAX_CARS; CarID ++)
        {
            if (strcmp(id,ginfo->car[CarID].car)== 0)
            {
                break;
            }
        }

        if (CarID == MAX_CARS)
        {
            send_mtc(ginfo->players[i].UCID,"^C^2| ^7У нас нет такой машины!");
            return;
        }

#ifdef _RC_LEVEL_H
        int needlvl = (CarID-1)*5;
        if (dl->GetLVL(ginfo->players[i].UCID) < needlvl)
        {
            char msg[64];
            sprintf(msg,"^C^2| ^7Нужен уровень: ^1%d",needlvl);
            send_mtc(ginfo->players[i].UCID,msg);
            return;
        }
#endif
        if (bank->GetCash(ginfo->players[i].UCID) < (int)ginfo->car[CarID].cash)
        {
            char msg[64];
            sprintf(msg,"^C^2| ^7Нужно ^1%d ^7RUR.",(int)ginfo->car[CarID].cash);
            send_mtc(ginfo->players[i].UCID,msg);
            return;
        }


        for ( int j=0; j<MAX_CARS; j++)
        {
            if (strcmp(id,ginfo->players[i].cars[j].car)== 0)
            {
                char msg[64];
                sprintf(msg,"^C^2| ^7У вас уже есть такая машина");
                send_mtc(ginfo->players[i].UCID,msg);
                return;
            }
        }

        for ( int j=0; j<MAX_CARS; j++)
        {

            if (strlen(ginfo->players[i].cars[j].car) == 0)
            {
                strcpy(ginfo->players[i].cars[j].car,id);
                ginfo->players[i].cars[j].tuning=0;
                ginfo->players[i].cars[j].dist=0;
                char msg[64];
                sprintf(msg,"^C^2|^7 Вы купили %s",id);
                send_mtc(ginfo->players[i].UCID,msg);

                bank->RemCash(ginfo->players[i].UCID,ginfo->car[CarID].cash);
                bank->AddToBank(ginfo->car[CarID].cash);

                ginfo->players[i].PLC += ginfo->car[CarID].PLC;
                send_plc(ginfo->players[i].UCID, ginfo->players[i].PLC);

                SYSTEMTIME sm;
                GetLocalTime(&sm);
                char log[MAX_PATH];
                sprintf(log,"%slogs\\shop\\shop(%d.%d.%d).txt",RootDir,sm.wYear,sm.wMonth,sm.wDay);

                ofstream readf (log,ios::app);
                readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  ginfo->players[i].UName << " buy car " << id << endl;
                readf.close();

                char sql[128];
                sprintf(sql,"INSERT INTO garage  ( username, car ) VALUES ( '%s' , '%s' );", ginfo->players[i].UName , ginfo->car[CarID].car );
                mysql_query( &rcMaindb , sql );

                break;
            }

        }


    }

    if (strncmp(Msg, "!sell", 4) == 0)
    {
        if (ginfo->players[i].Zone != 2)
        {
            send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2000));
            return;
        }
        char * comand;
        char * id;

        comand = strtok (Msg," ");
        id = strtok (NULL," ");
        // test if "id" NULL or != 3
        if ( ( !id ) or ( strlen( id ) != 3 ) )
        {
            send_mtc(ginfo->players[i].UCID,"Error");
            return;
        }

        if (strcmp(id,"UF1")==0)
            return;
        // search car in global base
        int j = 0; // global car id
        for ( j = 1 ; j < MAX_CARS ; j++)
            if (strcmp(id,ginfo->car[j].car)==0)
                break;

        if ( j == MAX_CARS )
            return;

        // if user now  on this car
        if (strcmp(id,ginfo->players[i].CName)==0)
        {
            send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,1009));
            return;
        }

        for ( int k=0; k<MAX_CARS; k++)
        {
            if (strcmp(ginfo->players[i].cars[k].car,ginfo->car[j].car) == 0)
            {
                char msg[64];
                sprintf(msg,"^C^2|^7 Вы продали %s",id);
                send_mtc(ginfo->players[i].UCID,msg);

                strcpy(ginfo->players[i].cars[k].car,"");
                ginfo->players[i].cars[k].tuning=0;
                ginfo->players[i].cars[k].dist=0;

                SYSTEMTIME sm;
                GetLocalTime(&sm);
                char log[MAX_PATH];
                sprintf(log,"%slogs\\shop\\shop(%d.%d.%d).txt",RootDir,sm.wYear,sm.wMonth,sm.wDay);
                ofstream readf (log,ios::app);
                readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << ":" << sm.wMilliseconds << " " <<  ginfo->players[i].UName << " sell car " << id << endl;
                readf.close();

                bank->AddCash(ginfo->players[i].UCID,ginfo->car[j].sell, true);
                bank->RemFrBank(ginfo->car[j].sell);

                ginfo->players[i].PLC -= ginfo->car[j].PLC;
                send_plc(ginfo->players[i].UCID, ginfo->players[i].PLC);

                char sql[128];
                sprintf(sql,"DELETE FROM garage WHERE  username = '%s' AND  car = '%s'", ginfo->players[i].UName , ginfo->car[j].car );
                mysql_query( &rcMaindb , sql );
                break;
            }

        }

    }

    //!EXIT
    if ( ( strncmp(Msg, "!exit", 5) == 0 or ( strncmp(Msg, "!^Cвыход", 8) == 0) ) and strcmp(ginfo->players[i].UName, "denis-takumi") == 0)
    {
        send_mst("/msg ^1| ^3Russian Cruise: ^7^CПодана команда на выключение");
        send_mst("/msg ^1| ^3Russian Cruise: ^7^CСохранение данных");
        for (int j=0; j<MAX_PLAYERS; j++)
        {

            if (ginfo->players[j].UCID !=0 )
            {
                save_car(&ginfo->players[j]);
                save_user_cars(&ginfo->players[j]);
#ifdef _RC_POLICE_H
				police->SaveUserFines( ginfo->players[j].UCID );
#endif // _RC_POLICE_H

                bank->bank_save(ginfo->players[j].UCID);
#ifdef _RC_ENERGY_H
                nrg->energy_save(ginfo->players[j].UCID);
#endif

#ifdef _RC_LEVEL_H
                dl->save(ginfo->players[j].UCID);
#endif
            }
        }
        ok=0;
    }

    if (strncmp(Msg, "!reload", 7) == 0 and (strcmp(ginfo->players[i].UName, "denis-takumi") == 0 or strcmp(ginfo->players[i].UName, "Lexanom") == 0))
    {
        send_mst("/msg ^1| ^3Russian Cruise: ^3^CПодана команда на чтение файлов");

        struct IS_TINY pack_requests;
        memset(&pack_requests, 0, sizeof(struct IS_TINY));
        pack_requests.Size = sizeof(struct IS_TINY);
        pack_requests.Type = ISP_TINY;
        pack_requests.ReqI = 1;
        pack_requests.SubT = TINY_RST;
        insim->send_packet(&pack_requests);

    }

    if (strncmp(Msg, "!debug", 7) == 0)
    {
        if (ginfo->players[i].Svetofor == 0)
            ginfo->players[i].Svetofor = 1;
        else
        {
            pizza->btn_information_clear(ginfo->players[i].UCID);
            ginfo->players[i].Svetofor = 0;
        }

    }

    if ((strncmp(Msg, "!pit", 4) == 0) or (strncmp(Msg, "!^Cпит", 6) == 0 ))
    {
        if (ginfo->players[i].Pogonya == 1)
        {
            char Msg[64];
            sprintf(Msg, "/pitlane %s",ginfo->players[i].UName);
            send_mst(Msg);
            send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2700));
            send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2701));
            bank->RemCash(ginfo->players[i].UCID,5000);
            bank->AddToBank(5000);
        }
        else
        {

            char Msg[64];
            sprintf(Msg, "/pitlane %s",ginfo->players[i].UName);
            send_mst(Msg);
            ginfo->players[i].Zone = 1;
            bank->RemCash(ginfo->players[i].UCID,250);
            bank->AddToBank(250);

        }
    }
    //!users
    if ((strncmp(Msg, "!users",6) == 0) or (strncmp(Msg, "!^Cнарод", 8) == 0 ))
    {
        ginfo->players[i].Action = 1;

        for(int h=0; h<50; h++)
        {
            send_bfn(ginfo->players[i].UCID,h);
        }

        struct IS_BTN pack_btn;
        memset(&pack_btn, 0, sizeof(struct IS_BTN));
        pack_btn.Size = sizeof(struct IS_BTN);
        pack_btn.Type = ISP_BTN;
        pack_btn.ReqI = ginfo->players[i].UCID;              // Must be non-zero, I'll just use UCID
        pack_btn.UCID = ginfo->players[i].UCID;              // UCID of the player that will receive the button
        pack_btn.BStyle = 16 + ISB_CLICK;                 // Dark frame for window title
        pack_btn.L = 1;
        pack_btn.T = 191;
        pack_btn.W = 24;
        pack_btn.H = 4;

        int col = 0;
        for (int j=0; j<MAX_PLAYERS; j++)
        {
            if (ginfo->players[j].UCID!=0 and ginfo->players[j].cop == 0)
            {
                if (col == 16)
                {
                    pack_btn.L += 24;
                    pack_btn.T = 191;
                }

                pack_btn.ClickID = ginfo->players[j].BID;
                strcpy(pack_btn.Text, ginfo->players[j].PName);
                insim->send_packet(&pack_btn);
                pack_btn.T -= 4;

                col++;
            }

        }
        pack_btn.ClickID = 34;
        pack_btn.T = 195;
        strcpy(pack_btn.Text,msg->GetMessage(ginfo->players[i].UCID,2003));
        insim->send_packet(&pack_btn);

    }
}

void case_mso_flood ()
{
    int i;

    struct IS_MSO *pack_mso = (struct IS_MSO*)insim->get_packet();

    // The chat GetMessage is sent by the host, don't do anything
    if (pack_mso->UCID == 0)
        return;

    // Find the player that wrote in the chat
    for (i=0; i < MAX_PLAYERS; i++)
        if (ginfo->players[i].UCID == pack_mso->UCID)
            break;


    char Msg[128];
    strcpy(Msg,pack_mso->Msg + ((unsigned char)pack_mso->TextStart));

    /**  FLOOD ***/
    if (pack_mso->UserType == MSO_USER) // if msg from user
    {

        time_t timef;
        int ftime = time(&timef); // get current time

        if (ginfo->players[i].FloodTime == 0)
            ginfo->players[i].FloodTime = ftime;

        int ts = ftime - ginfo->players[i].FloodTime;
        ginfo->players[i].FloodTime = ftime;


        if (ts < 5)
            ginfo->players[i].FloodCount++;
        else
            ginfo->players[i].FloodCount = 1;

        if (ginfo->players[i].FloodCount > 4)   //max lines to tolerate
        {
            send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2004));
            ginfo->players[i].FloodCount = 0;

            bank->RemCash(ginfo->players[i].UCID,500);
            bank->AddToBank(500);
        }


        /*** МАТ И Т.П. ***/

        for (int j=0; j< ginfo->WordsCount; j++)
        {
            if (strstr(Msg,ginfo->Words[j]))
            {
                send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2005));

                bank->RemCash(ginfo->players[i].UCID,5000);
                bank->AddToBank(5000);
            }

        }
        strcpy(ginfo->players[i].Msg,Msg);
    } // if UserType != MSO_PREFIX
    /** Flood and bad words **/

}
void case_mso_cop ()
{
    int i;

    struct IS_MSO *pack_mso = (struct IS_MSO*)insim->get_packet();

    // The chat GetMessage is sent by the host, don't do anything
    if (pack_mso->UCID == 0)
        return;

    if ( pack_mso->UserType != MSO_PREFIX )
        return;

    // Find the player that wrote in the chat
    for (i=0; i < MAX_PLAYERS; i++)
        if (ginfo->players[i].UCID == pack_mso->UCID)
            break;

    char Msg[96];
    strcpy(Msg,pack_mso->Msg + ((unsigned char)pack_mso->TextStart));

    if ((strncmp(Msg, "!sirena", 7) == 0 ) or (strncmp(Msg, "!^Cсирена", 9) == 0 ))
    {
        out << ginfo->players[i].UName << " send !sirena" << endl;

        if (ginfo->players[i].cop == 1)
        {
            if (ginfo->players[i].sirena ==0 )
            {
                send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2100));
                ginfo->players[i].sirena = 1;
            }
            else
            {
                send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2101));
                ginfo->players[i].sirena = 0;
            }
        }
    }

    if ((strncmp(Msg, "!radar", 6) == 0 ) or (strncmp(Msg, "!^Cрадар", 8) == 0 ))
    {
        out << ginfo->players[i].UName << " send !radar" << endl;

        if (ginfo->players[i].cop == 1)
        {
            if (ginfo->players[i].radar ==0 )
            {
                send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2102));
                ginfo->players[i].radar = 1;
            }
            else
            {
                send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2103));
                ginfo->players[i].radar = 0;
            }
        }
    }

    if ((strncmp(Msg, "!fines", 6) == 0) or (strncmp(Msg, "!^Cштрафы", 9) == 0 ))
    {
        help_cmds(&ginfo->players[i],3);
    }

    if (strncmp(Msg, "!kick", 4) == 0 )
    {
        char user[16];
        strcpy(user,Msg+5);

        if(strlen(user)>0)
        {
            if (ginfo->players[i].cop == 1)
            {
                char Kick[64];
                sprintf(Kick,"/kick %s",user);
                send_mst(Kick);

                SYSTEMTIME sm;
                GetLocalTime(&sm);
                char log[MAX_PATH];
                sprintf(log,"%slogs\\cop\\kick(%d.%d.%d).txt",RootDir,sm.wYear,sm.wMonth,sm.wDay);
                ofstream readf (log,ios::app);
                readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << ":" << sm.wMilliseconds << " " <<  ginfo->players[i].UName << " kick " << user << endl;
                readf.close();
            }
        }
    }

    if ( strcmp( ginfo->players[i].UName ,"denis-takumi") == 0 )
    {
        char file[255];
        sprintf(file,"%smisc\\cops.txt",RootDir);

        char line[32];
        if (strncmp(Msg, "!cop_add", 8) == 0 )
        {
            char param[16];
            strcpy(param,Msg+9);

            if(strlen(param)>0)
            {
                ofstream wCops( file , ios::app );
                wCops << param <<  endl;
                wCops.close();
            }
        }

        if (strncmp(Msg, "!cop_del", 8) == 0 )
        {
            char param[16];
            strcpy(param,Msg+9);
            printf("%s\n",param);
            if(strlen(param)>0)
            {
                char cops[40][32];
                memset(&cops,0,40*32);

                int j = 0;

                ifstream rCops( file , ios::in );

                while ( rCops.good() )
                {
                    memset(&line,0,32);
                    rCops.getline(line, 32);

                    if (strlen(line) >0)
                        strncpy(cops[j++],line,32);
                }
                rCops.close();

                ofstream wCops( file , ios::out );

                for(j=0; j<40; j++)
                {
                    if (strncmp(cops[j],param,strlen(param)) != 0)
                    {
                        wCops << cops[j] << endl;
                    }
                }
                wCops.close();

            }
        }
        if (strncmp(Msg, "!cop_list", 9) == 0 )
        {
            ifstream rCops( file , ios::in );

            while ( rCops.good() )
            {

                memset(&line,0,32);
                rCops.getline(line, 32);
                if (strlen(line) >0)
                    send_mst(line);
            }
            rCops.close();
        }
//!ban#denis-takumi#1
        if (strncmp(Msg, "!ban", 4) == 0 )
        {
            cout << Msg << endl;
            char* com;
            char* user;
            char* timee;

            com = strtok(Msg, "#");
            user = strtok(NULL, "#");
            timee = strtok(NULL, "#");

            if( !com || !user || !timee )
                return;

            banlist.addUser( user, time(NULL) + atoi(timee)*24*3600 );

        }

        if (strncmp(Msg, "!unban", 6) == 0 )
        {
            char* com;
            char* user;

            com = strtok(Msg, "#");
            user = strtok(NULL, "#");

            if( !com || !user )
                return;
            banlist.removeUser( user );
        }

    }
}




void case_ncn ()
{
    int i;

    struct IS_NCN *pack_ncn = (struct IS_NCN*)insim->get_packet();

    if (pack_ncn->UCID == 0)
        return;

    for (i=0; i<MAX_PLAYERS; i++)
        if (ginfo->players[i].UCID == 0)
            break;


    if (i == MAX_PLAYERS)
        return;

    // Copy all the player data we need into the ginfo->players[] array
    strcpy(ginfo->players[i].UName, pack_ncn->UName);
    strcpy(ginfo->players[i].PName, pack_ncn->PName);
    ginfo->players[i].UCID = pack_ncn->UCID;
    ginfo->players[i].BID = i;
    ginfo->players[i].Zone = 1;

    read_user_cars(&ginfo->players[i]);

    help_cmds(&ginfo->players[i],2);
}

void case_npl ()
{
    // out << "joining race or leaving pits" << endl;
    int i;
    char specText[64];
    struct IS_NPL *pack_npl = (struct IS_NPL*)insim->get_packet();

    // Find player using UCID and update his PLID
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo->players[i].UCID == pack_npl->UCID)
        {
			sprintf(specText, "/spec %s", ginfo->players[i].UName);

            if (pack_npl->PType != 6)
            {
                msg->send_bfn_all( ginfo->players[i].UCID );

                ginfo->players[i].cop = 0;
                ginfo->players[i].Pitlane = 1;

                if (IfCop(&ginfo->players[i]) == 1)
                {
                    send_mst( specText );
                    send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,1300));
                    return;
                }
                else if (IfCop(&ginfo->players[i]) == 3)
                {
                    send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,1301));
                    ginfo->players[i].cop = 1;
                    dl->Lock(ginfo->players[i].UCID);
                    nrg->Lock(ginfo->players[i].UCID);
                    lgh->SetLight3(ginfo->players[i].UCID,true);
                }



                ginfo->players[i].PLID = pack_npl->PLID;
                ginfo->players[i].H_TRes =  pack_npl->H_TRes;
                ginfo->players[i].SetF =  pack_npl->SetF;

                int j=0;
                for (j=0; j<MAX_CARS; j++)
                {
                    if (strcmp(ginfo->players[i].cars[j].car,pack_npl->CName)==0)
                        break;
                }

                if ( j != MAX_CARS)
                {
                    strcpy(ginfo->players[i].CName ,pack_npl->CName);
                    ginfo->players[i].CTune = ginfo->players[i].cars[j].tuning;
                    ginfo->players[i].Distance = ginfo->players[i].cars[j].dist;

                    int tune = 45;
                    if (ginfo->players[i].CTune&1)
                    {
                        tune -= 15;
                    }
                    if (ginfo->players[i].CTune&2)
                    {
                        tune -= 20;
                    }
                    if (ginfo->players[i].CTune&4)
                    {
                        tune -= 10;
                    }

#ifdef _RC_LEVEL_H
                    int needlvl = (GetCarID(ginfo->players[i].CName)-1)*5;

                    if (dl->GetLVL(ginfo->players[i].UCID) < needlvl)
                    {
                        ginfo->players[i].PLID = 0;
                        ginfo->players[i].Zone = 1;
                        send_mst( specText );

                        char msg2[64];
                        sprintf(msg2,"^C^1|^7 Нужен уровень: %d",needlvl);
                        send_mtc(ginfo->players[i].UCID,msg2);
                        send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2404));

                        char Text2[64];
                        strcpy(Text2,"^1| ^2");
                        for (int k=0; k< MAX_CARS; k++)
                        {
                            if ((strlen(ginfo->players[i].cars[k].car) != 0) and ((GetCarID(ginfo->players[i].cars[k].car)-1)*5 <= dl->GetLVL(ginfo->players[i].UCID)))
                            {
                                strcat(Text2,ginfo->players[i].cars[k].car);
                                strcat(Text2," ");
                            }
                        }
                        send_mtc(ginfo->players[i].UCID,Text2);
                        return;
                    }
                    else if ((pack_npl->H_TRes < tune))
                    {
                        ginfo->players[i].PLID = 0;
                        ginfo->players[i].Zone = 1;
                        send_mst( specText );

                        char Texxt[32];
                        sprintf(Texxt,"%s %d %%",msg->GetMessage(ginfo->players[i].UCID,2400),tune);
                        send_mtc(ginfo->players[i].UCID,Texxt);
                    }
#endif
                }
                else
                {
                    send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2404));
                    help_cmds(&ginfo->players[i],2);
                    ginfo->players[i].Zone = 1;
                    ginfo->players[i].PLID = 0;
                    send_mst( specText );
                    return;
                }
                break;
            } //if PTupe != 6
        }//if UCID == UCID
    }//for
}


void case_pen ()
{
    int i;

    struct IS_PEN *pack_pen = (struct IS_PEN*)insim->get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo->players[i].PLID == pack_pen->PLID)
        {
            if (pack_pen->Reason == PENR_WRONG_WAY)
            {
				taxi->dead_pass(ginfo->players[i].UCID);
            }

            break;
        }
    }
}

void case_pla ()
{
    int i;

    struct IS_PLA *pack_pla = (struct IS_PLA*)insim->get_packet();

    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo->players[i].PLID == pack_pla->PLID)
        {
            if (pack_pla->Fact == PITLANE_EXIT)
            {
                ginfo->players[i].Pitlane = false;
                if (ginfo->players[i].Penalty != 0)
                {
                    char Text[64];
                    sprintf(Text, "/p_clear %s",ginfo->players[i].UName);
                    send_mst(Text);
                }

            }
            else
            {
                ginfo->players[i].Pitlane = true;
            }
            break;
        }
    }

}

void case_pll ()
{
    // out << "player leaves race" << endl;
    int i;

    struct IS_PLL *pack_pll = (struct IS_PLL*)insim->get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo->players[i].PLID == pack_pll->PLID)
        {
        	msg->send_bfn_all( ginfo->players[i].UCID );
            ginfo->players[i].PLID = 0;
            memset(&ginfo->players[i].Info,0,sizeof(CompCar));
            ginfo->players[i].Penalty =0;

            dl->Unlock(ginfo->players[i].UCID);
            nrg->Unlock(ginfo->players[i].UCID);
            lgh->SetLight3(ginfo->players[i].UCID,false);

            save_car(&ginfo->players[i]);

            if (ginfo->players[i].Pogonya == 1)
            {
                send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2600));
                send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2601));

                if( nrg->GetEnergy( ginfo->players[i].UCID ) > 10 )
                    dl->RemSkill(ginfo->players[i].UCID,10);

                bank->RemCash(ginfo->players[i].UCID,5000);
                bank->AddToBank(5000);
            }
            else
            {
                if (ginfo->players[i].Zone != 1)
                {
                    send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2602));
                    send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2603));

                    bank->RemCash(ginfo->players[i].UCID,500);
                    bank->AddToBank(500);
                }
            }
            for (int g=0; g<200; g++)
                send_bfn(ginfo->players[i].UCID,g);

            break;
        }
    }
}

void case_plp ()
{
    // out << "player leaves race" << endl;
    int i;

    struct IS_PLP *pack_plp = (struct IS_PLP*)insim->get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (ginfo->players[i].PLID == pack_plp->PLID)
        {
        	msg->send_bfn_all( ginfo->players[i].UCID );
            ginfo->players[i].PLID = 0;
            memset(&ginfo->players[i].Info,0,sizeof(CompCar));
            ginfo->players[i].Penalty =0;


            dl->Unlock(ginfo->players[i].UCID);
            nrg->Unlock(ginfo->players[i].UCID);
            lgh->SetLight3(ginfo->players[i].UCID,false);


            save_car(&ginfo->players[i]);

            if (ginfo->players[i].Pogonya == 1)
            {
                send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2700));
                send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2701));

                if( nrg->GetEnergy( ginfo->players[i].UCID ) > 10 )
                    dl->RemSkill(ginfo->players[i].UCID,10);

                bank->RemCash(ginfo->players[i].UCID,5000);
                bank->AddToBank(5000);
            }
            else
            {
                if (ginfo->players[i].Zone != 1)
                {
                    send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2702));
                    send_mtc(ginfo->players[i].UCID,msg->GetMessage(ginfo->players[i].UCID,2703));

                    bank->RemCash(ginfo->players[i].UCID,500);
                    bank->AddToBank(500);
                }
            }

            for (int g=0; g<200; g++)
                send_bfn(ginfo->players[i].UCID,g);

            break;
        }
    }
}

void case_rst ()
{
    struct IS_RST *pack_rst = (struct IS_RST*)insim->get_packet();

    strcpy(ginfo->Track, pack_rst->Track);

    readconfigs();

    read_car();

#ifdef _RC_POLICE_H
			police->ReadFines();
#endif // _RC_POLICE_H

    read_track();

    read_words();
};

void case_vtn ()
{
    struct IS_VTN *pack_vtn = (struct IS_VTN *)insim->get_packet();

    for (int i=0; i<MAX_PLAYERS; i++)
    {
        if (ginfo->players[i].UCID == pack_vtn->UCID)
        {
            send_mst(msg->GetMessage(ginfo->players[i].UCID,2900));
            send_mst("/cv");
            break;
        }
    }
}

int core_connect(void *pack_ver)
{
    memset(pack_ver,0,sizeof(struct IS_VER));
    struct IS_VER *pack_v = (IS_VER*)pack_ver;

    if (insim->init (ginfo->IP, ginfo->TCPPORT, IS_PRODUCT_NAME,ginfo->ADMIN,pack_v, '!', isf_flag,500, ginfo->UDPPORT) < 0)
    {
        out << "\n * Error during initialization * " << endl;
        return -1;
    }

    strcpy(ginfo->Product,pack_v->Product);

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


    pack_requests.SubT = TINY_RST;      // Request all players in-grid to know their PLID
    insim->send_packet(&pack_requests);

    return 1;
}

int core_reconnect(void *pack_ver)
{
    insim->isclose();

    out << "wait 1 minute and reconnect \n";
    Sleep (60000);


    memset(pack_ver,0,sizeof(struct IS_VER));
    struct IS_VER *pack_v = (IS_VER*)pack_ver;

    if (insim->init (ginfo->IP, ginfo->TCPPORT, IS_PRODUCT_NAME,ginfo->ADMIN, pack_v, '!', isf_flag,500, ginfo->UDPPORT) < 0)
    {
        out << "\n * Error during initialization * " << endl;
        return -1;
    }

    strcpy(ginfo->Product,pack_v->Product);

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

    pack_requests.SubT = TINY_RST;      // Request all players in-grid to know their PLID
    insim->send_packet(&pack_requests);

    return 1;
}

void read_track()
{
    char file[255];
    sprintf(file,"%sdata\\RCCore\\tracks\\%s.txt" , RootDir , ginfo->Track);

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        out << "Can't find " << file << endl;
        return;
    }
    FindClose(fff);

    ifstream readf (file,ios::in);

    memset(&ginfo->TrackInf,0,sizeof(track_info));

    while (readf.good())
    {
        char str[128];
        readf.getline(str,128);

        if (strstr(str,"//")) {}
        if (strlen(str) > 0)
        {
            if (strncmp(str,"/pitzone",8)==0)
            {
                readf.getline(str,128);
                int count = atoi(str);

                ginfo->TrackInf.PitCount = count;

                for (int i=0 ; i<count; i++)
                {
                    readf.getline(str,128);
                    char * X;
                    char * Y;
                    X = strtok (str,";");
                    Y = strtok (NULL,";");
                    ginfo->TrackInf.XPit[i] = atoi(X);
                    ginfo->TrackInf.YPit[i] = atoi(Y);
                }
            }

            if (strncmp(str,"/shop",5)==0)
            {
                readf.getline(str,128);
                int count = atoi(str);
                ginfo->TrackInf.ShopCount = count;

                for (int i=0 ; i<count; i++)
                {
                    readf.getline(str,128);
                    char * X;
                    char * Y;
                    X = strtok (str,";");
                    Y = strtok (NULL,";");
                    ginfo->TrackInf.XShop[i] = atoi(X);
                    ginfo->TrackInf.YShop[i] = atoi(Y);
                }
            }
        } // if strlen > 0
    } //while readf.good()

    readf.close();

    memcpy(&antcht->TrackInf,&ginfo->TrackInf,sizeof(struct track_info));
}



void read_car()
{
    char file[255];
    sprintf(file,"%smisc\\cars.txt" , RootDir);

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        out << "Can't find " << file << endl;
        return;
    }
    FindClose(fff);

    ginfo->carMap.clear();

    ifstream readf (file,ios::in);
    int i = 0;
    while (readf.good())
    {
        char str[128];
        readf.getline(str,128);
        if (strlen(str) > 0)
        {

            if (strstr(str,"//"))
                continue;

            char * id;
            char * car;
            char * cash;
            char * PLC;
            id = strtok (str,";");
            car = strtok (NULL,";");
            cash = strtok (NULL,";");
            PLC = strtok (NULL,";");

            i = atoi(id);
            memset(&ginfo->car[i],0,sizeof(struct cars));
            ginfo->car[i].id = i;
            strcpy(ginfo->car[i].car, car);
            ginfo->car[i].cash =atoi(cash);
            ginfo->car[i].sell= ginfo->car[i].cash*8/10;
            ginfo->car[i].PLC =atoi(PLC);
            /** map<> **/
            ginfo->carMap[ car ] = AddCar(i ,car, atoi(cash), atoi(cash)*8/10 , atoi(PLC) );

        } // if strlen > 0
    } //while readf.good()

    readf.close();
}

int read_cop(struct player *splayer)
{
    char file[255];
    sprintf(file,"%smisc\\cops.txt",RootDir);

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        out << "Can't find " << file << endl;
        return -1;
    }
    FindClose(fff);

    ifstream readf (file,ios::in);

    int cop = 0;
    while (readf.good())
    {
        char str[32];
        readf.getline(str,32);
        if( strlen( str ) > 0 and strcmp( splayer->UName , str ) == 0 )
		{
			cop ++;
			break;
        }
    }

    readf.close();
    return cop;
}

void read_cfg()
{
    out << "Read Server Config" << endl;

    char file[255];
    sprintf(file,"%smisc\\%s.cfg",RootDir,ServiceName);

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        out << "Can't find " << file << endl;
        return;
    }
    FindClose(fff);

    ifstream readf (file,ios::in);

    char str[128];
    while(readf.good())
    {
        readf.getline(str,128);
        if (strlen(str) > 0)
        {
            // GET IP
            if (strncmp("Ip=",str,3)==0)
                strncpy(ginfo->IP,str+3,strlen(str+3));

            // GET TCPPORT
            if (strncmp("TcpPort=",str,8)==0)
                ginfo->TCPPORT = atoi(str+8);

            // GET UDPPORT
            if (strncmp("UdpPort=",str,8)==0)
                ginfo->UDPPORT = atoi(str+8);

            //GET ADMIN PASSWORD
            if (strncmp("Admin=",str,6)==0)
                strncpy(ginfo->ADMIN,str+6,strlen(str+6));
        }
    }
}

/*********************************************/

void *thread_mci (void *params)
{
    out << "\tthread \"Multi Car Info\" started" << endl;
    while (true)
    {
        if (insim->udp_next_packet() < 0)
            continue;

        //out << "UDP packet MCI " << endl;
        case_mci ();
        //case_mci_svetofor();
        case_mci_cop();

#ifdef _RC_BANK_H
        bank->insim_mci( (struct IS_MCI*)insim->udp_get_packet() );
#endif

#ifdef _RC_CHEAT_H
        antcht->insim_mci( (struct IS_MCI*)insim->udp_get_packet() );
#endif

#ifdef _RC_PIZZA_H
        pizza->insim_mci( (struct IS_MCI*)insim->udp_get_packet() );
#endif

#ifdef _RC_ENERGY_H
        nrg->insim_mci( (struct IS_MCI*)insim->udp_get_packet() );
#endif

#ifdef _RC_STREET_H
        if (street->IfInited)
            street->insim_mci( (struct IS_MCI*)insim->udp_get_packet() );
#endif

#ifdef _RC_LIGHT_H
        if (lgh->IfInited)
            lgh->insim_mci( (struct IS_MCI*)insim->udp_get_packet() );
#endif

#ifdef _RC_LEVEL_H
        if (dl->inited == 1)
            dl->insim_mci( (struct IS_MCI*)insim->udp_get_packet() );
#endif

#ifdef _RC_TAXI_H
        if (taxi->inited == 1)
            taxi->insim_mci( (struct IS_MCI*)insim->udp_get_packet() );
#endif


    }
    return 0;
};

void *thread_btn (void *params)
{
    while (ok > 0)
    {
        for (int i=0; i<MAX_PLAYERS; i++)
        {
            if (ginfo->players[i].UCID != 0)
            {
                btn_panel(&ginfo->players[i]);

                if (ginfo->players[i].cop == 1)
                {
                    struct IS_BTN pack;
                    memset(&pack, 0, sizeof(struct IS_BTN));
                    pack.Size = sizeof(struct IS_BTN);
                    pack.Type = ISP_BTN;
                    pack.ReqI = 1;
                    pack.UCID = ginfo->players[i].UCID;
                    pack.Inst = 0;
                    pack.TypeIn = 0;
                    pack.ClickID = 60;
                    pack.BStyle = 32;
                    pack.L = 73;
                    pack.T = 191;
                    pack.W = 50;
                    pack.H = 4;

                    streets StreetInfo;

                    for (int j=0; j<MAX_PLAYERS; j++)
                    {
                        if (ginfo->players[j].UCID != 0 and ginfo->players[j].Pogonya != 0)
                        {
                            int time2 = ginfo->players[j].WorkTime - time(&stime);
                            int min = (time2/60)%60;
                            int sec = time2%60;

                            float D = dl->Distance(ginfo->players[i].Info.X,ginfo->players[i].Info.Y,ginfo->players[j].Info.X,ginfo->players[j].Info.Y)/65536;

                            street->CurentStreetInfo(&StreetInfo,ginfo->players[j].UCID);

                            if ( ginfo->players[j].Pogonya == 1 )
                                sprintf(pack.Text,"%s %s %3.3f ^2(^1%02d:%02d^2)",ginfo->players[j].PName,StreetInfo.Street,D,min,sec);
                            else if ( ginfo->players[j].Pogonya == 2 )
                                sprintf(pack.Text,"%s %s ^1^CАРЕСТОВАН",ginfo->players[j].PName,StreetInfo.Street);

                            insim->send_packet(&pack);
                            pack.T -=4;
                            pack.ClickID ++;

                        }
                    }
                }
            }

        }
        Sleep(1000);
    }
    return 0;
};

void *thread_save (void *params)
{
    SYSTEMTIME sm; //time_t seconds;
    while (ok > 0)
    {
        GetLocalTime(&sm); //seconds = time (NULL);
        if ((sm.wMinute*60+sm.wSecond) % 600 == 0) //every 30 minute
        {
            for (int j=0; j<MAX_PLAYERS; j++)
            {
                if (ginfo->players[j].UCID !=0 )
                {
                    save_car(&ginfo->players[j]);
                    save_user_cars(&ginfo->players[j]);
#ifdef _RC_POLICE_H
					police->SaveUserFines( ginfo->players[j].UCID );
#endif // _RC_POLICE_H
#ifdef _RC_ENERGY_H
                    nrg->energy_save(ginfo->players[j].UCID);
#endif
                    bank->bank_save(ginfo->players[j].UCID);
#ifdef _RC_LEVEL_H
                    dl->save(ginfo->players[j].UCID);
#endif

                    send_mtc(ginfo->players[j].UCID,msg->GetMessage(ginfo->players[j].UCID,3000));
                }
            }

        }
        Sleep(500);
        siren = "^4||||||||||^1||||||||||";
        Sleep(500);
        siren = "^1||||||||||^4||||||||||";
    }
    return 0;
};

void *thread_work (void *params)
{
    Sleep(10000);

    while (ok > 0)
    {
        // вывод кнопки с часиками и скрытие ее если таймер пришел в ноль
        for (int i=0; i<MAX_PLAYERS; i++)
        {
            if (ginfo->players[i].UCID != 0)
            {
                if (ginfo->players[i].Pogonya == 1)
                {
                    btn_work(&ginfo->players[i]);
                    int nowtime = time(&stime);
                    if (ginfo->players[i].WorkTime <= nowtime)
                    {
                        send_bfn(ginfo->players[i].UCID,210);

                        char Text[64];
                        sprintf( Text , "/msg ^2|%s %s" , ginfo->players[i].PName , msg->GetMessage(ginfo->players[i].UCID,1706) );

                        send_mst(Text);
                        ginfo->players[i].Pogonya = 0;
                        nrg->Unlock(ginfo->players[i].UCID);
                        dl->AddSkill(ginfo->players[i].UCID);
                    }
                }

            }
        } // конец цикла вывода кнопки с часиками и скрытие ее если таймер пришел в ноль

		pizza->Event();
        Sleep(1000);
    }
    return 0;
};


DWORD WINAPI ThreadMain(void *CmdLine)
{

    if(!mysql_init(&rcMaindb))
    {
        printf("RCMain Error: can't create MySQL-descriptor\n");
        return -1;
    }

    mysql_options( &rcMaindb , MYSQL_OPT_RECONNECT, "true" ); // разрешаем переподключение

    mysqlConf conf;
    char path[MAX_PATH];
    sprintf(path,"%smisc\\mysql.cfg",RootDir);
    tools::read_mysql(path, &conf);

    while( mysql_real_connect( &rcMaindb , conf.host , conf.user , conf.password , conf.database , conf.port , NULL, 0) == false )
    {
        printf("RCMain Error: can't connect to MySQL server\n");
        Sleep(60000);
    }
    printf("RCMain Success: Connected to MySQL server\n");
    // TODO (#1#): Uncoment in Release
    //Sleep(2*60*1000);
    sprintf(IS_PRODUCT_NAME,"RC-%d.%d.%d:%s\0",AutoVersion::RC_MAJOR,AutoVersion::RC_MINOR,AutoVersion::RC_BUILD,AutoVersion::RC_SVN_REVISION);

    pthread_mutex_init(&RCmutex, NULL);

    if (strlen(ServiceName) == 0)
    {
        out << "Не задан файл конфигурации\n"   ;
        service_status.dwCurrentState = SERVICE_STOPPED;
        // изменить состояние сервиса
        SetServiceStatus(hServiceStatus, &service_status);
        return 0;

    }
    // read_mysql();

    int error_ch;

    insim = new CInsim();
    ginfo = new GlobalInfo();

    read_cfg();

    if ( core_connect(&pack_ver) < 0 )
    {
        for (;;)
        {
            if ( core_reconnect(&pack_ver) > 0 )
                break;
        }
    }

    if (pack_ver.InSimVer != 5)
    {
        cout << "INSIM VER != 5" << endl;
        out << "INSIM VER != 5" << endl;
        //return 0;
    }


    pthread_t mci_tid; // Thread ID
    pthread_t btn_tid; // Thread ID
    pthread_t work_tid; // Thread ID
    pthread_t save_tid; // Thread ID

    out << "Cruise started" << endl;
    out << "Start threads :" << endl;
    if (pthread_create(&btn_tid,NULL,thread_btn,NULL) < 0)
    {
        printf("Can't start `thread_btn` Thread\n");
        return 0;
    }
    Sleep(1000);
    if (pthread_create(&work_tid,NULL,thread_work,NULL) < 0)
    {
        printf("Can't start `thread_work` Thread\n");
        return 0;
    }
    Sleep(1000);
    if (pthread_create(&save_tid,NULL,thread_save,NULL) < 0)
    {
        printf("Can't start `thread_save` Thread\n");
        return 0;
    }
    Sleep(1000);

    create_classes();
    init_classes();


    if (pthread_create(&mci_tid,NULL,thread_mci,NULL) < 0)
    {
        printf("Can't start `thread_mci` Thread\n");
        return 0;
    }
    Sleep(1000);

    out << "All threads started" << endl;

    while (ok > 0)
    {
        if (error_ch = insim->next_packet() < 0)
        {
            for (;;)
            {
                if ( core_reconnect(&pack_ver) > 0 )
                    break;
            }

        }

        msg->next_packet(); // обрабатывается первым, из-за того что потом е выводятся сообщения. приоритет емае
        msg->next_packet(); // обрабатывается первым, из-за того что потом е выводятся сообщения. приоритет емае

        switch (insim->peek_packet())
        {
        case ISP_MSO:
            case_mso ();
            case_mso_cop();
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

        case ISP_FLG:
            case_flg ();
            break;

        case ISP_PEN:
            case_pen ();
            break;

        case  ISP_PLA:
            case_pla();
            break;

        case ISP_VTN:
            case_vtn();
            break;

        case ISP_TOC:
            case_toc();
            break;
        }

#ifdef _RC_ENERGY_H
        nrg->next_packet();
#endif
#ifdef _RC_PIZZA_H
        pizza->next_packet();
#endif
        bank->next_packet();
#ifdef _RC_LEVEL_H
        dl->next_packet();
#endif

#ifdef _RC_CHEAT_H
        antcht->next_packet();
#endif

#ifdef _RC_STREET_H
        street->next_packet();
#endif

#ifdef _RC_LIGHT_H
        lgh->next_packet();
#endif

#ifdef _RC_TAXI_H
        taxi->next_packet();
#endif

#ifdef _RC_BANLIST_H
        banlist.next_packet();
#endif
    }

    if (insim->isclose() < 0)
        return 0;

    delete insim;
    delete ginfo;

    service_status.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(hServiceStatus, &service_status);

    pthread_mutex_destroy(&RCmutex);

    return 0;
}


int  nCount;     // счетчик


int core_install_service(char* param[])
{
    hServiceControlManager = OpenSCManager(
                                 NULL,      // локальная машина
                                 NULL,      // активная база данных сервисов
                                 SC_MANAGER_CREATE_SERVICE  // возможно создание сервиса
                             );
    if (hServiceControlManager == NULL)
    {
        printf("Open service control manager failed.\n");
        return 0;
    }

    printf("Service control manager is opened.\n");
    char dir[255];
    sprintf(dir,"%s %s", param[0] , param[1]);

    // устанавливаем новый сервис
    hService = CreateService(
                   hServiceControlManager,      // дескриптор менеджера сервисов
                   ServiceName,                 // внутреннее имя сервиса, используемое SCM
                   ServiceName,                 // внешнее имя сервиса в панели управления
                   SERVICE_ALL_ACCESS,          // полный контроль над сервисом
                   SERVICE_WIN32_OWN_PROCESS,   // сервис является процессом
                   SERVICE_AUTO_START,        	// запускается сервис по требованию
                   SERVICE_ERROR_NORMAL,        // обработка ошибок нормальная
                   dir,                         // путь к сервису
                   NULL,                        // сервис не принадлежит к группе
                   NULL,                        // тэг группы не изменяется
                   NULL,                        // сервис не зависит от других сервисов
                   NULL,                        // имя совпадает с текущим именем учетной записи
                   NULL                         // пароля нет
               );
    if (hService == NULL)
    {
        printf("Create service failed.");
        // закрываем дескриптор менеджера сервисов
        CloseServiceHandle(hServiceControlManager);
        return 0;
    }

    printf("Service is installed.");
    // закрываем дескрипторы
    CloseServiceHandle(hService);
    CloseServiceHandle(hServiceControlManager);
    return 2;
}

int core_uninstall_service()
{
    hServiceControlManager = OpenSCManager(
                                 NULL,      // локальная машина
                                 NULL,      // активная база данных сервисов
                                 SC_MANAGER_CONNECT   // соединение с менеджером сервисов
                             );
    if (hServiceControlManager == NULL)
    {
        printf("Open service control manager failed.");

        return 0;
    }

    printf("Service control manager is opened.");

    // открываем сервис
    hService = OpenService(
                   hServiceControlManager,  // дескриптор менеджера сервисов
                   ServiceName,            // имя сервиса
                   SERVICE_ALL_ACCESS | DELETE  // любой доступ к сервису
                   // и удаление из базы данных
               );
    if (hService == NULL)
    {
        printf("Open service failed.");
        // закрываем дескриптор менеджера сервисов
        CloseServiceHandle(hServiceControlManager);

        return 0;
    }

    printf("Service is opened.");


    // получаем состояние сервиса
    if (!QueryServiceStatus(hService, &service_status))
    {
        printf("Query service status failed.");
        // закрываем дескрипторы
        CloseServiceHandle(hServiceControlManager);
        CloseServiceHandle(hService);

        return 0;
    }

    // если сервис работает, то останавливаем его
    if (service_status.dwCurrentState != SERVICE_STOPPED)
    {
        printf("Service is working. It will be stoped");
        if (!ControlService(hService, SERVICE_CONTROL_STOP, &service_status))
        {
            printf("Control service failed.");
            // закрываем дескрипторы
            CloseServiceHandle(hServiceControlManager);
            CloseServiceHandle(hService);

            return 0;
        }
        // ждем, пока сервис остановится
        Sleep(5000);
    }

    // удаляем сервис
    if (!DeleteService(hService))
    {
        printf("Delete service failed.");
        // закрываем дескрипторы
        CloseServiceHandle(hServiceControlManager);
        CloseServiceHandle(hService);

        return 0;
    }

    printf("The service is deleted.");

    // закрываем дескрипторы
    CloseServiceHandle(hServiceControlManager);
    CloseServiceHandle(hService);

    return 2;
}
// главная функция приложения
int main(int argc, char* argv[])
{

    isf_flag = ISF_MCI + ISF_MSO_COLS + ISF_CON + ISF_OBH + ISF_HLV + ISF_AXM_EDIT + ISF_AXM_LOAD;


    int need = 92;
    int d;

    for (d = strlen(argv[0]); d > 0; d--)
    {
        if ( int(argv[0][d]) == need )
            break;
    }

    strncpy(RootDir,argv[0],d+1);
    strcpy(ServiceName,argv[1]);
    if (strcmp(argv[argc-1],"install") == 0 )
    {
        core_install_service(argv);
        return 0;
    }
    else if (strcmp(argv[argc-1],"uninstall") == 0 )
    {
        core_uninstall_service();
        return 0;
    }
    else if (strcmp(argv[argc-1],"console") == 0 )
    {

        SYSTEMTIME sm;
        GetLocalTime(&sm);

        char log[MAX_PATH];
        sprintf(log,"%slogs\\%s(%d.%d.%d).log",RootDir,ServiceName,sm.wDay,sm.wMonth,sm.wYear);

        out.open(log);

        CreateThread(NULL,0,ThreadMain,0,0,NULL);

        // рабочий цикл сервиса
        while (ok)
        {
            Sleep(1000);
        }
        out.close();

        Sleep(10000);
        return 0;
    }

    //  инициализируем структуру сервисов
    SERVICE_TABLE_ENTRY  service_table[] =
    {
        {ServiceName, ServiceMain},   // имя сервиса и функция сервиса
        { NULL, NULL }                 // больше сервисов нет
    };

    // запускаем диспетчер сервиса
    if (!StartServiceCtrlDispatcher(service_table))
        return 0;

    return 0;
}

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
    SYSTEMTIME sm;
    GetLocalTime(&sm);

    char log[MAX_PATH];
    sprintf(log,"%slogs\\%s(%d.%d.%d).log",RootDir,ServiceName,sm.wDay,sm.wMonth,sm.wYear);

    out.open(log);

    // регистрируем обработчик управляющих команд для сервиса
    hServiceStatus = RegisterServiceCtrlHandler(
                         ServiceName,        // имя сервиса
                         ServiceCtrlHandler   // обработчик управляющих команд
                     );
    if (!hServiceStatus)
    {
        out << "Register service control handler failed.";
        return;
    }

    // инициализируем структуру состояния сервиса
    service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    service_status.dwCurrentState = SERVICE_START_PENDING;
    service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP |
                                        SERVICE_ACCEPT_SHUTDOWN;
    service_status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
    service_status.dwServiceSpecificExitCode = 0;
    service_status.dwCheckPoint = 0;
    service_status.dwWaitHint = 5000;

    // устанавливаем состояние сервиса
    if (!SetServiceStatus(hServiceStatus, &service_status))
    {
        out << "Set service status 'SERVICE_START_PENDING' failed.";
        return;
    }

    // определяем сервис как работающий
    service_status.dwCurrentState = SERVICE_RUNNING;
    // нет ошибок
    service_status.dwWin32ExitCode = NO_ERROR;
    // устанавливаем новое состояние сервиса
    if (!SetServiceStatus(hServiceStatus, &service_status))
    {
        out << "Set service status 'START_PENDING' failed.";
        return;
    }

    // открываем файл протокола работы сервиса

    out << "The service is started." << endl << flush;
    out << "Main Thead started. Wait 2 minuts while all services are started.\n"   ;
    CreateThread(NULL,0,ThreadMain,0,0,NULL);

    // рабочий цикл сервиса
    while (service_status.dwCurrentState == SERVICE_RUNNING)
    {
        ++nCount;
        Sleep(3000);
    }

    out << "" << endl;
    out << "\t-----------------------------" << endl;
    out << "" << endl;
    out.close();
}

VOID WINAPI ServiceCtrlHandler(DWORD dwControl)
{
    switch (dwControl)
    {
    case SERVICE_CONTROL_STOP:     // остановить сервис
        send_mst("/msg ^1| ^3Russian Cruise: ^7^CПодана команда на выключение");
        send_mst("/msg ^1| ^3Russian Cruise: ^7^CСохранение данных");
        // save all users
        for (int j=0; j<MAX_PLAYERS; j++)
        {
            if (ginfo->players[j].UCID !=0 )
            {
                save_car(&ginfo->players[j]);
                save_user_cars(&ginfo->players[j]);
#ifdef _RC_POLICE_H
			police->SaveUserFines( ginfo->players[j].UCID );
#endif // _RC_POLICE_H
#ifdef _RC_ENERGY_H
                nrg->energy_save(ginfo->players[j].UCID);
#endif
                bank->bank_save(ginfo->players[j].UCID);
#ifdef _RC_LEVEL_H
                dl->save(ginfo->players[j].UCID);
#endif
                Sleep(500);
            }
        }
        ok=0;
        // записываем конечное значение счетчика
        //out << "Count = " << nCount << endl;
        tools::log( "The service is finished." );
        // закрываем файл

        // устанавливаем состояние остановки
        service_status.dwCurrentState = SERVICE_STOPPED;
        // изменить состояние сервиса
        SetServiceStatus(hServiceStatus, &service_status);
        break;

    case SERVICE_CONTROL_SHUTDOWN:     // завершить сервис
        service_status.dwCurrentState = SERVICE_STOPPED;
        // изменить состояние сервиса
        SetServiceStatus(hServiceStatus, &service_status);
        break;

    default:
        // увеличиваем значение контрольной точки
        ++service_status.dwCheckPoint;
        // оставляем состояние сервиса без изменения
        SetServiceStatus(hServiceStatus, &service_status);
        break;
    }
    return;
}
