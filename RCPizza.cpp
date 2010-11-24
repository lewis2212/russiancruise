using namespace std;

#include "RCPizza.h"

time_t  ptime;



void *pizzathread(void *arg)  // arg == classname from RCPizza::init
{
    RCPizza *piz = (RCPizza *)arg; //struct our RCPizza class in thread
    if(!piz)
    {
        printf ("Can't start pizzathread");
        return 0;
    }

    /**out << "\tthread \"Pizza\" started" << endl;**/
    //Sleep(10000);
    //piz->readconfig(ginfo.Track);

    //int pizza_time = time(&stime);
    //out << pizza_time << endl;

    int ok =1;

    while (ok > 0)
    {
        // вывод кнопки с часиками и скрытие ее если таймер пришел в ноль
        for (int i=0; i<MAX_PLAYERS; i++)
        {
            if (piz->players[i].UCID != 0)
            {
                if (piz->players[i].WorkAccept != 0)
                {
                    //cout << "piz->player[i].UCID = " << (int)piz->players[i].UCID <<endl;
                    int nowtime = time(&ptime);
                    if (piz->players[i].WorkTime <= nowtime)
                    {
                        piz->send_bfn(piz->players[i].UCID,210);
                        if (piz->players[i].WorkAccept != 0)
                        {
                            piz->send_mtc(piz->players[i].UCID,piz->msg->message[0][4101]);
                            piz->players[i].WorkType = WK_NULL;
                            piz->players[i].WorkAccept = 0;
                            piz->players[i].WorkPlayerAccept = 0;
                        }

                    }
                }

                if (piz->players[i].WorkAccept != 0)
                piz->btn_work(&piz->players[i]);


            }
        } // конец цикла вывода кнопки с часиками и скрытие ее если таймер пришел в ноль
        /*
                 if ( ginfo_time <= time(&stime))
                 {
                srand(time(&stime));
                int r = rand()%3 + 1;
                pizza_time += 600/(ginfo.NumP+1);
                //out << r << endl;
                //out << ginfo_time - time(&stime) << endl;

                for (int i = 0; i<32; i++)
                {


                if ( (piz->players[i].UCID !=0) and (piz->players[i].WorkType == WK_PIZZA) and (piz->players[i].WorkAccept == 0))
                {
                out << piz->players[i].UName << " accepted\n";
                send_mtc(piz->players[i].UCID,"^C^3| ^7Поступил заказ на пиццу");
                send_mtc(piz->players[i].UCID,"^C^3| ^7Подъедте к пиццерии и отвезите заказ");
                piz->players[i].WorkAccept =1;
                piz->players[i].WorkPlayerAccept =0;
                piz->players[i].WorkZone =0;
                int worktime = time(&stime);
                piz->players[i].WorkTime = worktime+60*6;
                 break; // чтобы оповещал только одного игрока
                }

                }
                }
                */


        Sleep(1000);
    }
    return 0;
};

RCPizza::RCPizza()
{

}

RCPizza::~RCPizza()
{

}

int RCPizza::init(void *classname,void *CInSim, void *Message,void *Bank,void *Energy)
{
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr,PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

    if (pthread_create(&tid,&attr,pizzathread,classname) < 0)
        return -1;

    insim = (CInsim *)CInSim;
    if(!insim)
    {
        printf ("Can't struct CInsim class");
        return -1;
    }

    msg = (RCMessage *)Message;
    if(!msg)
    {
        printf ("Can't struct RCMessage class");
        return -1;
    }
    bank = (RCBank *)Bank;
    if(!msg)
    {
        printf ("Can't struct RCBank class");
        return -1;
    }
    nrg = (RCEnergy *)Energy;
    if(!msg)
    {
        printf ("Can't struct RCEnergy class");
        return -1;
    }

    return 0;
}

void RCPizza::deal(struct PizzaPlayer *splayer)
{

    if (splayer->WorkType == WK_NULL)
    {

        splayer->WorkType = WK_PIZZA;
        splayer->WorkAccept = 0;
        send_mtc(splayer->UCID,msg->message[0][4000]);


    }
    else if (splayer->WorkType == WK_PIZZA)
    {
        send_mtc(splayer->UCID,msg->message[0][4001]);
    }
    else
    {
        send_mtc(splayer->UCID,msg->message[0][4002]);
    }
}

void RCPizza::take(struct PizzaPlayer *splayer)
{
    if (splayer->WorkType == WK_PIZZA)
    {
        if (splayer->WorkAccept == 1)
        {
            if (splayer->WorkPlayerAccept == 0)
            {
                srand(time(&ptime));
                int place = rand()%zone.NumPoints;

                if (place == 0)
                    place ++;

                int worktime = time(&ptime);
                splayer->WorkTime = worktime+60*6;
                splayer->WorkDestinaion = place;
                splayer->WorkAccept = 2;
                send_mtc(splayer->UCID,msg->message[0][4200]);
                send_mtc(splayer->UCID,zone.point[place].Place);
            }
            else if (splayer->WorkPlayerAccept != 0)
            {
                int worktime = time(&ptime);
                splayer->WorkTime = worktime+60*6;
                splayer->WorkDestinaion = splayer->WorkPlayerAccept;
                splayer->WorkAccept = 2;
                char text[96];
                strcpy(text,msg->message[0][4201]);
                strcat(text,players[splayer->WorkPlayerAccept - 100].PName);
                send_mtc(splayer->UCID,text);
            }

        }
        else
        {
            send_mtc(splayer->UCID,msg->message[0][4202]);
        }
    }
    else
    {
        send_mtc(splayer->UCID,msg->message[0][4203]);
    }
}

void RCPizza::done(struct PizzaPlayer *splayer)
{

    //cout << "true" << endl;
    if ((splayer->WorkType == WK_PIZZA) and (splayer->WorkAccept == 2))
    {
        splayer->WorkDestinaion =0;
        splayer->WorkAccept = 0;
        splayer->WorkPlayerAccept = 0;
        //splayer->cash += 800;
// TODO (#1#): Class Bank

        splayer->WorkCountDone ++;
        send_mtc(splayer->UCID,msg->message[0][4300]);
        send_bfn(splayer->UCID,210);
        splayer->WorkZone =0;
    }

}
void RCPizza::undeal(struct PizzaPlayer *splayer, char *Reason)
{

    //cout << "true" << endl;
    if (splayer->WorkType == WK_PIZZA)
    {
        send_bfn(splayer->UCID,210);
        send_mtc(splayer->UCID,msg->message[0][4100]);
        send_mtc(splayer->UCID,Reason);
        splayer->WorkType = WK_NULL;
        splayer->WorkDestinaion =0;
        splayer->WorkAccept = 0;
    }

}

void RCPizza::readconfig(char *Track)
{
    //cout << "RCPizza::readconfig\n" ;
    char file[MAX_PATH];
    memset(&file,0,MAX_PATH);
    //strcpy(file,RootDir);
    strcat(file,"work\\");
    strcat(file,Track);
    strcat(file,"_pizza.txt");

    //cout << file << endl;


    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        printf ("Can't find ");
        return;
    }
    FindClose(fff);

    ifstream readf (file,ios::in);


    int point = 0;

    while (readf.good())
    {
        char str[128];
        readf.getline(str,128);

        //cout << str << endl;


        if (strlen(str) > 0)
        {

            if (strncmp(str,"/dealer",7)==0)
            {
                for (int i=0; i<4; i++)
                {
                    readf.getline(str,128);
                    char * X;
                    char * Y;
                    X = strtok (str,",");
                    Y = strtok (NULL,",");
                    zone.dealX[i] = atoi(X);
                    zone.dealY[i] = atoi(Y);
                    //cout << point << ". X=" << X << "; Y=" << Y << ";\n";
                }
            } // if /street

            if (strncmp(str,"point",5)==0)
            {
                readf.getline(str,64);
                strncpy(zone.point[point].Place,str,strlen(str));
                //cout << "place[" << point << "]= " << zone.point[point].Place << endl;
                for (int i=0; i<4; i++)
                {
                    readf.getline(str,128);
                    char * X;
                    char * Y;
                    X = strtok (str,",");
                    Y = strtok (NULL,",");
                    //cout << point << ". X["<<i<<"]= " << X << ", Y["<<i<<"]= " << Y << endl;
                    zone.point[point].X[i] = atoi(X);
                    zone.point[point].Y[i] = atoi(Y);
                }
                point ++;
                zone.NumPoints = point;
            } // if /street


        } // if strlen > 0
    } //while readf.good()

    readf.close();
}

/** функции-повторители основных фунцкий ядра **/


void RCPizza::pizza_ncn()
{
    //printf("New player connect\n");
    int i;

    struct IS_NCN *pack_ncn = (struct IS_NCN*)insim->get_packet();

    //cout << "ReqI = " << (int)pack_ncn->ReqI << endl;


    if (pack_ncn->UCID == 0)
    {
        //cout << "(Host connected, not adding him to array...)" << endl << endl;
        return;
    }



    for (i=0; i<MAX_PLAYERS; i++)
        if (players[i].UCID == 0)
            break;


    if (i == MAX_PLAYERS)
    {
        //cout << "NO MORE PEOPLE ALLOWED!!!" << endl << endl;
        return;
    }

    // Copy all the player data we need into the players[] array
    strcpy(players[i].UName, pack_ncn->UName);
    strcpy(players[i].PName, pack_ncn->PName);
    players[i].UCID = pack_ncn->UCID;

    //cout << players[i].UName << endl;
    //cout << players[i].PName << endl;
    //cout << (int)players[i].UCID << endl;

}

void RCPizza::pizza_npl()
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
            strcpy(players[i].CName ,pack_npl->CName);
        }
    }
}

void RCPizza::pizza_plp()
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

void RCPizza::pizza_pll()
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

void RCPizza::pizza_cnl ()
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

            memset(&players[i],0,sizeof(struct PizzaPlayer));
            break;
        }
    }
}

void RCPizza::pizza_crp()
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

void RCPizza::pizza_mci ()
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

                players[j].Info.X = pack_mci->Info[i].X;
                players[j].Info.Y = pack_mci->Info[i].Y;
                players[j].Info.Node = pack_mci->Info[i].Node;
                players[j].Info.Direction = pack_mci->Info[i].Direction;
                players[j].Info.Heading = pack_mci->Info[i].Heading;
                players[j].Info.Speed = pack_mci->Info[i].Speed;
                players[j].Info.AngVel = pack_mci->Info[i].AngVel;


                //printf ("players[%d] (%s)\n",j,players[j].UName);
                //printf ("Pozition on map X:%d Y:%d\n",players[j].Info.X/65536,players[j].Info.Y/65536);


                // проверка если юзер попал в зону доставки и скорость равна нулю

                if (check_pos(&players[j]) == 1)
                {
                    if (players[j].Zone != 4 )
                    {
                        players[j].Zone = 4;
                          send_mtc(players[j].UCID,msg->message[0][1600]);
                          send_mtc(players[j].UCID,msg->message[0][1601]);
                          send_mtc(players[j].UCID,msg->message[0][1602]);
                          send_mtc(players[j].UCID,msg->message[0][1603]);

                    }
                }

                else
                {
                    players[j].Zone = 0;
                }
                /** Zones (PitSave, shop, etc) **/

                if ((check_pos(&players[j]) == 2) and (S<5))
                {
                    done(&players[j]);
                }

                int PLN = players[j].WorkPlayerAccept;
                if ((PLN >= 100 ) and (players[j].WorkAccept == 2))
                {
                    int PLNX = players[PLN-100].Info.X/65536;
                    int PLNY = players[PLN-100].Info.Y/65536;
                    int PLX[4] = {PLNX+5,PLNX-5,PLNX-5,PLNX+5};
                    int PLY[4] = {PLNY+5,PLNY+5,PLNY-5,PLNY-5};

                    if ( Check_Pos(4,PLX,PLY,X,Y))
                    {
                        done(&players[j]);
                        /** TODO: THINK ABOUT THIS**/
                        bank->players[PLN-100].Cash -= 800;
                        nrg->players[PLN-100].Energy += 800;

                        send_mtc(players[PLN-100].UCID,msg->message[0][1604]);



                    }
                }


            } // if pack_mci->Info[i].PLID == players[j].PLID
        }
    }
}

void RCPizza::pizza_mso ()
{
    int i;

    struct IS_MSO *pack_mso = (struct IS_MSO*)insim->get_packet();

    // The chat message is sent by the host, don't do anything
    if (pack_mso->UCID == 0)
    {
        //cout << "(Chat message by host: " << pack_mso->Msg + ((unsigned char)pack_mso->TextStart) << ")" << endl;
        return;

    }

    // Find the player that wrote in the chat
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_mso->UCID)
        {
            //cout << "Msg: " << pack_mso->Msg << endl;
            break;
        }
    }



    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!deal", 5) == 0 )
    {
        //cout << players[i].UName << " send !deal" << endl;

        if ((check_pos(&players[i]) == 1) and (players[i].WorkType == 0))
        {
            deal(&players[i]);
        }

    }

    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!undeal", 7) == 0 )
    {
        //cout << players[i].UName << " send !undeal" << endl;

        if ((check_pos(&players[i]) == 1) and (players[i].WorkType == WK_PIZZA))
        {
            undeal(&players[i],msg->message[0][2200]);
        }

    }

    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!take", 5) == 0)
    {
        //cout << players[i].UName << " send !take" << endl;

        if (check_pos(&players[i]) == 1)
        {
            if (players[i].WorkType == WK_PIZZA)
            {
                if (players[i].WorkAccept == 0)
                    players[i].WorkAccept =1;
            }
            take(&players[i]);
        }
    }

    //!pizza
    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!pizza", 6) == 0)
    {
        int j = 0;
        for (j = 0; j<32; j++)
        {


            if ( (players[j].UCID !=0) and (players[j].UCID != players[i].UCID) and (players[j].WorkType == WK_PIZZA) and (players[j].WorkAccept == 0))
            {
                //cout << players[i].UName << " accepted\n";
                 send_mtc(players[j].UCID,msg->message[0][2201]);
                 send_mtc(players[j].UCID,msg->message[0][2202]);
                players[j].WorkAccept =1;
                players[j].WorkPlayerAccept = 100 + i;
                players[j].WorkZone =0;
                int worktime = time(&ptime);
                players[j].WorkTime = worktime+60*6;
                break; // чтобы оповещал только одного игрока
            }

        }
        cout << "j= " << j << endl;
        if (j >= 32)
        {
            send_mtc(players[i].UCID,msg->message[0][2203]);
        }
        else
        {
            send_mtc(players[i].UCID,msg->message[0][2204]);
        }

    }

}

void RCPizza::btn_work (struct PizzaPlayer *splayer)
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
    pack.L = 151;
    pack.T = 1;
    pack.W = 10;
    pack.H = 10;

    char min_c[3];
    char sec_c[3];

    int time2 = splayer->WorkTime - time(&ptime);
    int min;
    int sec;

    min = time2/60;
    sec = time2%60;

    itoa(min,min_c,10);
    itoa(sec,sec_c,10);

    strcpy(pack.Text,"^2");
    strcat(pack.Text,min_c);
    strcat(pack.Text,":");

    if (sec < 10)
        strcat(pack.Text,"0");

    strcat(pack.Text,sec_c);
    insim->send_packet(&pack);

}
// функции-утилиты

int RCPizza::check_pos(struct PizzaPlayer *splayer)
{
    int PLX = splayer->Info.X/65536;
    int PLY = splayer->Info.Y/65536;

    if (Check_Pos(4,zone.dealX,zone.dealY,PLX,PLY))
    {
        return 1;
    }

    if (Check_Pos(4,zone.point[splayer->WorkDestinaion].X,zone.point[splayer->WorkDestinaion].Y,PLX,PLY))
    {
        return 2;
    }

    return 0;
}

void RCPizza::send_mtc (byte UCID,char* Msg)
{
    struct IS_MTC pack_mtc;
    memset(&pack_mtc, 0, sizeof(struct IS_MTC));
    pack_mtc.Size = sizeof(struct IS_MTC);
    pack_mtc.Type = ISP_MTC;
    pack_mtc.UCID = UCID;
    strcpy(pack_mtc.Msg, Msg);
    insim->send_packet(&pack_mtc);
};

void RCPizza::send_mst (char* Text)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    strcpy(pack_mst.Msg,Text);
    insim->send_packet(&pack_mst);
};


void RCPizza::send_bfn (byte UCID, byte ClickID)
{
    struct IS_BFN pack;
    memset(&pack, 0, sizeof(struct IS_BFN));
    pack.Size = sizeof(struct IS_BFN);
    pack.Type = ISP_BFN;
    pack.UCID = UCID;
    pack.ClickID = ClickID;
    insim->send_packet(&pack);
};


