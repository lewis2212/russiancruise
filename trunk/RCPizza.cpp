using namespace std;

#include "RCPizza.h"

time_t  ptime;

pthread_t tid; // Thread ID


void *pizzathread(void *arg)  // arg == classname from RCPizza::init
{
    RCPizza *piz = (RCPizza *)arg; //struct our RCPizza class in thread
    if(!piz)
    {
        printf ("Can't start pizzathread");
        return 0;
    }

    cout << "\tthread \"Pizza\" started" << endl;
    //Sleep(10000);
    //piz->readconfig(ginfo.Track);

    //int pizza_time = time(&ptime);
    //out << pizza_time << endl;

    int ok = 1;
    piz->ShopAccepted = false;

    /** ����� ������ � �������� � ������� �� ���� ������ ������ � ���� **/

    while (ok > 0)
    {
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
                        piz->send_bfn(piz->players[i].UCID,211);

                        if (piz->players[i].WorkAccept == 3) // �� ����� �������� ��������
                            piz->ShopAccepted = false;

                        if (piz->players[i].WorkAccept != 0)
                        {
                            piz->send_mtc(piz->players[i].UCID,piz->msg->GetMessage(piz->players[i].UCID,4101));
                            piz->players[i].WorkType = WK_NULL;
                            piz->players[i].WorkAccept = 0;
                            if ( piz->players[i].WorkPlayerAccept != 0)
                            {
                                for (int j=0; j<32; j++)
                                {
                                    if (piz->players[j].UCID == piz->players[i].WorkPlayerAccept)
                                        piz->players[j].Pizza = 0;
                                    break;
                                }
                            }
                            piz->players[i].WorkPlayerAccept = 0;
                            piz->CarsInWork --;
                        }
                    }
                }

                if (piz->players[i].WorkAccept != 0)
                {
                    piz->btn_work(&piz->players[i]);
                    piz->btn_destination(&piz->players[i]);
                }



            }
        }
        /** ����� ����� ������ ������ � �������� � ������� �� ���� ������ ������ � ���� **/

        /** ��� ����� ����� ������� **/

        if ( piz->ginfo_time <= time(&ptime))
        {
            cout << "try to send pizza to player" << endl;
            //srand(time(&ptime));
            //int r = rand()%3 + 1;
            int pizza_time = 600/(piz->NumP+1);
            piz->ginfo_time += pizza_time;

            if ((piz->PStore.Muka > 5) && (piz->PStore.Voda > 5) && (piz->PStore.Ovoshi > 5) && (piz->PStore.Cheese > 5))
            {

               // int n = 0;

                for (int i = 0; i<32; i++) // ������ �� ����������
                {


                    if ( (piz->players[i].UCID !=0) and (piz->players[i].WorkType == WK_PIZZA) and (piz->players[i].WorkAccept == 0))
                    {
                       // if (n == piz->Next)
                        //{
                            //cout << piz->players[i].UName << " accepted\n";

                            /** ������ ������������� �� ������� ������ **/
                            int j = 0;
                            for (j = 0; j<32; j++) // ������ �� ����������
                            {


                                if ( (piz->players[j].UCID !=0) and (piz->players[j].UCID != piz->players[i].UCID) and (piz->players[j].Pizza == 1))
                                {

                                   // piz->Next ++; //! ������� �� ���������� ������
                                    //cout << players[i].UName << " accepted\n";
                                    piz->send_mtc(piz->players[i].UCID,piz->msg->GetMessage(piz->players[i].UCID,2201));
                                    piz->send_mtc(piz->players[i].UCID,piz->msg->GetMessage(piz->players[i].UCID,2202));
                                    piz->players[i].WorkAccept =1;
                                    piz->players[i].WorkPlayerAccept = 100 + j;
                                    piz->players[i].WorkZone =0;
                                    int worktime = time(&ptime);
                                    piz->players[i].WorkTime = worktime+60*6;

                                    piz->players[j].Pizza = 2;

                                    break; // ����� �������� ������ ������ ������

                                    // �����
                                    //char Text[64];
                                    //sprintf(Text,"%s -> %s ",piz->players[i].UName,piz->players[j].UName);
                                    //piz->send_mst(Text);
                                }

                            }
                            break;
                       // }
                        //n++;
                    }
                }




                /** ��� ����� ����� ����� **/


               // n = 0;

                for (int i = 0; i<32; i++)
                {
                    if ( (piz->players[i].UCID !=0) and (piz->players[i].WorkType == WK_PIZZA) and (piz->players[i].WorkAccept == 0))
                    {

                       // if (n == piz->Next)
                       // {
                          //  piz->Next ++; //! ������� �� ���������� ������
                            cout << piz->players[i].UName << " accepted for pizza\n";

                            piz->send_mtc(piz->players[i].UCID,piz->msg->GetMessage(piz->players[i].UCID,2201));
                            piz->send_mtc(piz->players[i].UCID,piz->msg->GetMessage(piz->players[i].UCID,2202));

                            strcpy(piz->players[i].WorkDest,piz->msg->GetMessage(piz->players[i].UCID,2201)+4); // +4 == remove slash

                            piz->players[i].WorkAccept =1;
                            piz->players[i].WorkPlayerAccept =0;
                            piz->players[i].WorkZone =0;
                            int worktime = time(&ptime);
                            piz->players[i].WorkTime = worktime+60*6;
                            break; // ����� �������� ������ ������ ������
                        //}

                       // n++;

                    }

                }
            }
            else
            {
                cout << "malo productov" << endl;
            }
        }

        /*** �������� ������ �� ������� ���������, � ���� �� ������� �� ������� ��������� ������� �������� ***/

        if ((piz->PStore.Muka < 10) || (piz->PStore.Voda < 10) || (piz->PStore.Ovoshi < 10) || (piz->PStore.Cheese < 10))
        {
            printf("try to buy products\n");
            if  (piz->ShopAccepted == false)
            {
                printf("ShopAccepted == false\n");
                for (int i = 0; i<32; i++)
                {


                    if ( (piz->players[i].UCID !=0)
                            and (piz->players[i].WorkType == WK_PIZZA)
                            and (piz->players[i].WorkAccept == 0))
                    {
                        cout << piz->players[i].UName << " accepted for shop\n";

                        piz->send_mtc(piz->players[i].UCID,"^C^3| ^7������ � ������� � �������� ��������.");
                        strcpy(piz->players[i].WorkDest,"^C^7������ � ������� � �������� ��������."); // +4 == remove slash

                        piz->players[i].WorkAccept =3;
                        piz->players[i].WorkPlayerAccept =0;
                        piz->players[i].WorkZone =0;
                        int worktime = time(&ptime);
                        piz->players[i].WorkTime = worktime+60*6;
                        piz->ShopAccepted = true;
                        break; // ����� �������� ������ ������ ������
                    }

                }
            }
        }

       // if (piz->Next > piz->CarsInWork)
        //    piz->Next=0;

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

int RCPizza::init(const char *dir,void *classname,void *CInSim, void *GetMessage,void *Bank,void *Energy,void *DrLic, void *Taxi)
{
    strcpy(RootDir,dir);

    pthread_cancel(tid);
    Sleep (1500);
    if (pthread_create(&tid,NULL,pizzathread,classname) < 0)
        return -1;

    insim = (CInsim *)CInSim;
    if(!insim)
    {
        printf ("Can't struct CInsim class");
        return -1;
    }

    msg = (RCMessage *)GetMessage;
    if(!msg)
    {
        printf ("Can't struct RCMessage class");
        return -1;
    }
    bank = (RCBank *)Bank;
    if(!bank)
    {
        printf ("Can't struct RCBank class");
        return -1;
    }
    nrg = (RCEnergy *)Energy;
    if(!nrg)
    {
        printf ("Can't struct RCEnergy class");
        return -1;
    }

    dl = (RCDL *)DrLic;
    if(!dl)
    {
        printf ("Can't struct RCDL class");
        return -1;
    }



    ginfo_time = time(&ptime)+60;

    /*** READ STORE DATA ***/
    char file[MAX_PATH];
    HANDLE fff;
    WIN32_FIND_DATA fd;
    sprintf(file,"%sdata\\RCPizza\\_Store.txt",RootDir);
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        printf ("RCPizza: Can't find _Store.txt");
        return 0;
    }
    FindClose(fff);

    ifstream ReadStore (file,ios::in);
    while (ReadStore.good())
    {
        char str[128];
        ReadStore.getline(str,128);

        //cout << str << endl;

        if (strlen(str) > 0)
        {
            if (strncmp(str,"Muka=",strlen("Muka="))==0)
            {
                PStore.Muka = atof(str+strlen("Muka="));
            }

            if (strncmp(str,"Voda=",strlen("Voda="))==0)
            {
                PStore.Voda = atof(str+strlen("Voda="));
            }

            if (strncmp(str,"Ovoshi=",strlen("Ovoshi="))==0)
            {
                PStore.Ovoshi = atof(str+strlen("Ovoshi="));
            }

            if (strncmp(str,"Cheese=",strlen("Cheese="))==0)
            {
                PStore.Cheese = atof(str+strlen("Cheese="));
            }
        }
    }
    ReadStore.close();

    sprintf(file,"%sdata\\RCPizza\\_Pizza.txt",RootDir);
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        printf ("RCPizza: Can't find _Pizza.txt");
        return 0;
    }
    FindClose(fff);

    ifstream ReadInfo (file,ios::in);
    while (ReadInfo.good())
    {
        char str[128];
        ReadInfo.getline(str,128);

        //cout << str << endl;

        if (strlen(str) > 0)
        {
            if (strncmp(str,"Capital=",strlen("Capital="))==0)
            {
                Capital = atoi(str+strlen("Capital="));
            }

            if (strncmp(str,"NumCars=",strlen("NumCars="))==0)
            {
                NumCars = atoi(str+strlen("NumCars="));
            }

        }
    }
    ReadInfo.close();

    return 0;
}

void RCPizza::deal(struct PizzaPlayer *splayer)
{

    if (splayer->WorkType == WK_NULL)
    {
        if (CarsInWork <= NumCars)
        {
            splayer->WorkType = WK_PIZZA;
            splayer->WorkAccept = 0;
            send_mtc(splayer->UCID,msg->GetMessage(splayer->UCID,4000));
            CarsInWork ++;
        }
        else
        {
            send_mtc(splayer->UCID,"^C^3| ^7��� ��������");
            return;
        }


    }
    else if (splayer->WorkType == WK_PIZZA)
    {
        send_mtc(splayer->UCID,msg->GetMessage(splayer->UCID,4001));
    }
    else
    {
        send_mtc(splayer->UCID,msg->GetMessage(splayer->UCID,4002));
    }
}

void RCPizza::take(struct PizzaPlayer *splayer)
{
    if (splayer->WorkType != WK_PIZZA)
    {
        send_mtc(splayer->UCID,msg->GetMessage(splayer->UCID,4002));
        return;
    }

    if (strcmp(splayer->CName,"UF1")!=0)
    {
        send_mtc(splayer->UCID, "^3^C| ^7����� ������ ^2UF1");
        return;
    }
    if (splayer->WorkAccept == 1)
    {
        /** ������� ����� **/
        /**
        *   ���� ������ ����� - 1 �� ���� 0.6 �� ����, 0.9 �������, 0.4 ����
        *   ������������� �� ������ = 3+ 3 +40 +83 = 132 ���
        *   ������������� ���� 320 ���
        *   ������ �� ������������ 188 ���
        *   ������ �� �������� 240 ���.
        **/
        PStore.Muka -= 0.25; // 3 rur
        PStore.Voda -= 0.2;    // 3
        PStore.Ovoshi -= 0.5;   //40
        PStore.Cheese -= 0.15; // 83

        if (splayer->WorkPlayerAccept == 0) // ���� ������� �����
        {
            srand(time(NULL));
            int place = rand()%zone.NumPoints;

            //if (place == 0)
               // place ++;

            int worktime = time(&ptime);
            splayer->WorkTime = worktime+60*6;
            splayer->WorkDestinaion = place;
            splayer->WorkAccept = 2;
            send_mtc(splayer->UCID,msg->GetMessage(splayer->UCID,4200));
            send_mtc(splayer->UCID,zone.point[place].Place);
            strcpy(splayer->WorkDest,zone.point[place].Place);

        }
        else if (splayer->WorkPlayerAccept != 0) // ������� �����
        {
            int worktime = time(&ptime);
            splayer->WorkTime = worktime+60*6;
            splayer->WorkDestinaion = splayer->WorkPlayerAccept;
            splayer->WorkAccept = 2;
            char text[96];
            strcpy(text,msg->GetMessage(splayer->UCID,4201));
            strcat(text,players[splayer->WorkPlayerAccept - 100].PName);

            strcpy(splayer->WorkDest,players[splayer->WorkPlayerAccept - 100].PName);

            send_mtc(splayer->UCID,text);
        }

    }
    else if (splayer->WorkAccept == 2)
    {
        send_mtc(splayer->UCID,msg->GetMessage(splayer->UCID,4202)); // ������ ������� ���� �����
    }
    else
    {
        send_mtc(splayer->UCID,msg->GetMessage(splayer->UCID,4203)); // Wait until i call you
    }

}

void RCPizza::done(struct PizzaPlayer *splayer)
{

    //cout << "true" << endl;
    if ((splayer->WorkType == WK_PIZZA) and (splayer->WorkAccept == 2))
    {
        int i;
        for (i=0; i < MAX_PLAYERS; i++)
        {
            if (bank->GetPlayerUCID(i) == splayer->UCID)
            {
                break;
            }
        }



        splayer->WorkDestinaion =0;
        splayer->WorkAccept = 0;
        splayer->WorkPlayerAccept = 0;
        bank->AddCash(splayer->UCID,248); // ���� �� �������� 248 ���.
        Capital += 420;
        //bank->RemFrBank(800);

        dl->AddSkill(splayer->UCID);

        splayer->WorkCountDone ++;
        send_mtc(splayer->UCID,msg->GetMessage(splayer->UCID,4300));
        send_bfn(splayer->UCID,210);
        send_bfn(splayer->UCID,211);
        splayer->WorkZone =0;
    }

}
void RCPizza::undeal(struct PizzaPlayer *splayer,const char *Reason)
{

    //cout << "true" << endl;
    if (splayer->WorkType == WK_PIZZA)
    {
        if (splayer->WorkAccept == 3)
            ShopAccepted = false;

        send_bfn(splayer->UCID,210);
        send_bfn(splayer->UCID,211);
        send_mtc(splayer->UCID,msg->GetMessage(splayer->UCID,4100));
        send_mtc(splayer->UCID,Reason);
        splayer->WorkType = WK_NULL;
        splayer->WorkDestinaion =0;
        splayer->WorkAccept = 0;
        CarsInWork --;
    }

}

void RCPizza::readconfig(const char *Track)
{
    //cout << "RCPizza::readconfig\n" ;
    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCPizza\\%s.txt",RootDir,Track);
    //cout << file << endl;


    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        printf ("RCPizza: Can't find\n%s",file);
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

            if (strncmp(str,"/shop",5)==0)
            {
                readf.getline(str,128);
                int count = atoi(str);
                TrackInf.ShopCount = count;

                for (int i=0 ; i<count; i++)
                {
                    readf.getline(str,128);
                    char * X;
                    char * Y;
                    X = strtok (str,";");
                    Y = strtok (NULL,";");
                    TrackInf.XShop[i] = atoi(X);
                    TrackInf.YShop[i] = atoi(Y);
                }
            }

            if (strncmp(str,"point",5)==0)
            {
                readf.getline(str,64);
                strncpy(zone.point[point].Place,str,strlen(str));
                //cout << "place[" << point << "]= " << zone.point[point].Place << endl;
                readf.getline(str,128);
                char * X;
                char * Y;
				X = strtok (str,",");
                Y = strtok (NULL,",");
                    //cout << point << ". X["<<i<<"]= " << X << ", Y["<<i<<"]= " << Y << endl;
                zone.point[point].X = atoi(X);
                zone.point[point].Y = atoi(Y);

                point ++;
                zone.NumPoints = point;
            } // if /street


        } // if strlen > 0
    } //while readf.good()

    readf.close();

}

/** �������-����������� �������� ������� ���� **/

void RCPizza::next_packet()
{
    switch (insim->peek_packet())
    {
    case ISP_MSO:
        pizza_mso();
        break;


    case ISP_NPL:
        pizza_npl();
        break;

    case ISP_NCN:
        pizza_ncn();
        break;

    case ISP_CNL:
        pizza_cnl();
        break;

    case ISP_PLL:
        pizza_pll();
        break;

    case ISP_PLP:
        pizza_plp();
        break;

    case ISP_CPR:
        pizza_crp();
        break;


    }
}


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
            printf("Pizza Car = %s\n",players[i].CName);
            NumP ++;
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

            NumP --;
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

            NumP --;
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

            if (players[i].WorkType == WK_PIZZA)
                CarsInWork --;

            memset(&players[i],0,sizeof(struct PizzaPlayer));
            break;
        }
    }
    /** Save Pizza Info **/

    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCPizza\\_Store.txt",RootDir);
    ofstream writef (file,ios::out);
    writef << "Muka=" << PStore.Muka << endl;
    writef << "Voda=" << PStore.Voda << endl;
    writef << "Ovoshi=" << PStore.Ovoshi << endl;
    writef << "Cheese=" << PStore.Cheese << endl;
    writef.close();

    sprintf(file,"%sdata\\RCPizza\\_Pizza.txt",RootDir);
    ofstream WriteInfo (file,ios::out);
    WriteInfo << "Capital=" << Capital << endl;
    WriteInfo << "NumCars=" << NumCars << endl;
    WriteInfo.close();


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


                // �������� ���� ���� ����� � ���� �������� � �������� ����� ����

                if (check_pos(&players[j]) == 1)
                {
                    if (players[j].Zone != 4 )
                    {
                        players[j].Zone = 4;

                        send_mtc(players[j].UCID,msg->GetMessage(players[j].UCID,1600)); // pizza u Jony
                        if (players[j].WorkType != WK_PIZZA)
                            send_mtc(players[j].UCID,msg->GetMessage(players[j].UCID,1601)); // deal
                        else
                            send_mtc(players[j].UCID,msg->GetMessage(players[j].UCID,1602)); // undeal
                        if (players[j].WorkAccept != 0)
                        {
                            send_mtc(players[j].UCID,msg->GetMessage(players[j].UCID,1603)); // take
                            take(&players[j]);
                        }

                    }
                }

                else
                {
                    players[j].Zone = 0;
                }
                /** Zones (PitSave, shop, etc) **/

                if ((Distance(X,Y,zone.point[players[j].WorkDestinaion].X,zone.point[players[j].WorkDestinaion].Y) < 10) and (S <5))
					done(&players[j]);



                int PLN = players[j].WorkPlayerAccept;
                if ((PLN >= 100 ) and (players[j].WorkAccept == 2))
                {
                    int PLNX = players[PLN-100].Info.X/65536;
                    int PLNY = players[PLN-100].Info.Y/65536;

                    if (Distance(X,Y,PLNX,PLNY) < 10) // ���� ��������� �� ������ ������ 10 ������
                    {
                    	done(&players[j]);
                        bank->RemCash(players[PLN-100].UCID,800);
                        nrg->players[PLN-100].Energy += 8000;
                        send_mtc(players[PLN-100].UCID,msg->GetMessage(players[PLN-100].UCID,1604));
                        players[PLN-100].Pizza = 0;
                    }


                }

                if ((players[j].WorkAccept ==3) && (Check_Pos(TrackInf.ShopCount,TrackInf.XShop,TrackInf.YShop,X,Y))) // ���� ������ ������� �������� �������� � �� ������� � �������
                {
                    players[j].WorkDestinaion =0;
                    players[j].WorkAccept = 0;
                    players[j].WorkPlayerAccept = 0;
                    send_bfn(players[j].UCID,210);
                    send_bfn(players[j].UCID,211);
                    send_mtc(players[j].UCID,"^C^3| ^7������� �� ����� ���������.");

                    // ��������� ����� �� ��������

                    if (PStore.Muka < 10)
                    {
                        PStore.Muka += 99;
                        Capital -= 99 * 12;
                        bank->AddToBank(99 * 12);
                    }
                    if (PStore.Voda < 10)
                    {
                        PStore.Voda += 99;
                        Capital -= 99 * 10;
                        bank->AddToBank(99 * 10);
                    }
                    if (PStore.Ovoshi < 10)
                    {
                        PStore.Ovoshi += 99;
                        Capital -= 99 * 80;
                        bank->AddToBank(99 * 80);
                    }
                    if (PStore.Cheese < 10)
                    {
                        PStore.Cheese += 99;
                        Capital -= 99 * 560;
                        bank->AddToBank(99 * 560);
                    }
                    ShopAccepted = false;

                }


            } // if pack_mci->Info[i].PLID == players[j].PLID
        }
    }
}

void RCPizza::pizza_mso ()
{
    int i;

    struct IS_MSO *pack_mso = (struct IS_MSO*)insim->get_packet();

    // The chat GetMessage is sent by the host, don't do anything
    if (pack_mso->UCID == 0)
    {
        //cout << "(Chat GetMessage by host: " << pack_mso->Msg + ((unsigned char)pack_mso->TextStart) << ")" << endl;
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

    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!pstat", 6) == 0 )
    {
        //cout << players[i].UName << " send !deal" << endl;
        char Text[64];
        sprintf(Text,"^C^3| ^7������� �������� ���������� %d ���.",Capital);
        send_mtc(players[i].UCID,Text);
        sprintf(Text,"^C^3| ^7�������� �� ������.");
        send_mtc(players[i].UCID,Text);
        sprintf(Text,"^C^3| ^7����: %1.2f ��.",PStore.Muka);
        send_mtc(players[i].UCID,Text);
        sprintf(Text,"^C^3| ^7����: %1.2f ��.",PStore.Voda);
        send_mtc(players[i].UCID,Text);
        sprintf(Text,"^C^3| ^7�����: %1.2f ��.",PStore.Ovoshi);
        send_mtc(players[i].UCID,Text);
        sprintf(Text,"^C^3| ^7���: %1.2f ��.",PStore.Cheese);
        send_mtc(players[i].UCID,Text);
        sprintf(Text,"^C^3| ^7���������: %d/%d.",CarsInWork,NumCars);
        send_mtc(players[i].UCID,Text);

        for (int j=0; j<32; j++)
        {
            if (players[j].WorkType == WK_PIZZA)
            {
                sprintf(Text,"%s",players[j].PName);
                send_mtc(players[i].UCID,Text);
            }

        }

    }

    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!deal", 5) == 0 )
    {


        if ((check_pos(&players[i]) == 1) and (players[i].WorkType == 0))
        {
        	//cout << players[i].UName << " send !deal" << endl;
			if (dl->GetLVL(players[i].UCID) > 19)
			{
				send_mtc(players[i].UCID,"^6| ^C^7������� ������� ������� ������. ����� ������� ���� 20.");
				return;
			}
            if (strcmp(players[i].CName,"UF1") == 0 )
            {
                deal(&players[i]);
            }
            else
            {
                send_mtc(players[i].UCID, "^3^C| ^7����� ������ ^2UF1");
            }
        }


    }

    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!undeal", 7) == 0 )
    {
        //cout << players[i].UName << " send !undeal" << endl;

        if ((check_pos(&players[i]) == 1) and (players[i].WorkType == WK_PIZZA))
        {
            undeal(&players[i],msg->GetMessage(players[i].UCID,2200));
        }

    }

    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!take", 5) == 0)
    {


        if (check_pos(&players[i]) == 1)
        {
            //cout << players[i].UName << " send !take" << endl;
            if (players[i].WorkType == WK_PIZZA)
            {
                //if (players[i].WorkAccept == 0)
                // players[i].WorkAccept =1;
            }
            take(&players[i]);

        }

    }

    //!pizza
    if (strncmp(pack_mso->Msg + ((unsigned char)pack_mso->TextStart), "!tofu", 5) == 0)
    {
        if (nrg->GetEnergy(players[i].UCID) > 80)
        {
            send_mtc(players[i].UCID,"Your power must be less than 80%"); // ����� ������
            return;
        }


        if (players[i].WorkType == WK_PIZZA)
        {
            send_mtc(players[i].UCID,"You can't order a tofu because you work here."); // ����� ������
            return;
        }

        if (players[i].Pizza == 0)
        {
            players[i].Pizza = 1;
            send_mtc(players[i].UCID,msg->GetMessage(players[i].UCID,2204)); // ����� ������
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
    pack.L = 136;
    pack.T = 1;
    pack.W = 10;
    pack.H = 8;

    char min_c[3];
    char sec_c[3];

    int time2 = splayer->WorkTime - time(&ptime);
    int min;
    int sec;

    min = time2/60;
    sec = time2%60;

    sprintf(min_c,"%d",min);
    sprintf(sec_c,"%d",sec);

    strcpy(pack.Text,"^2");
    strcat(pack.Text,min_c);
    strcat(pack.Text,":");

    if (sec < 10)
        strcat(pack.Text,"0");

    strcat(pack.Text,sec_c);
    insim->send_packet(&pack,errmsg);

}

void RCPizza::btn_destination (struct PizzaPlayer *splayer)
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.ClickID = 211;
    pack.BStyle = 32;
    pack.L = 85;
    pack.T = 15;
    pack.W = 50;
    pack.H = 4;

    strcpy(pack.Text,splayer->WorkDest);


    insim->send_packet(&pack,errmsg);

}
// �������-�������

int RCPizza::check_pos(struct PizzaPlayer *splayer)
{
    int PLX = splayer->Info.X/65536;
    int PLY = splayer->Info.Y/65536;

    if (Check_Pos(4,zone.dealX,zone.dealY,PLX,PLY))
    {
        return 1;
    }

    /*if (Check_Pos(4,zone.point[splayer->WorkDestinaion].X,zone.point[splayer->WorkDestinaion].Y,PLX,PLY))
    {
        return 2;
    }*/

    return 0;
}

void RCPizza::send_mtc (byte UCID,const char* Msg)
{
    char errmsg[64];
    ZeroMemory(&errmsg,64);
    struct IS_MTC pack_mtc;
    memset(&pack_mtc, 0, sizeof(struct IS_MTC));
    pack_mtc.Size = sizeof(struct IS_MTC);
    pack_mtc.Type = ISP_MTC;
    pack_mtc.UCID = UCID;
    strncpy(pack_mtc.Text, Msg,strlen(Msg));
    if (!insim->send_packet(&pack_mtc,errmsg))
        cout << errmsg << endl;
};

void RCPizza::send_mst (const char* Text)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    strcpy(pack_mst.Msg,Text);
    insim->send_packet(&pack_mst,errmsg);
};


void RCPizza::send_bfn (byte UCID, byte ClickID)
{
    struct IS_BFN pack;
    memset(&pack, 0, sizeof(struct IS_BFN));
    pack.Size = sizeof(struct IS_BFN);
    pack.Type = ISP_BFN;
    pack.UCID = UCID;
    pack.ClickID = ClickID;
    insim->send_packet(&pack,errmsg);
};

bool RCPizza::IfWork (byte UCID)
{
	for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID ==UCID)
        {
            //cout << "Msg: " << pack_mso->Msg << endl;
            if (players[i].WorkType != 0)
            return true;

            break;
        }
    }
    return false;
}


