using namespace std;

#include "RCTaxi.h"

RCTaxi::RCTaxi()
{

}

RCTaxi::~RCTaxi()
{

}

int RCTaxi::init(char *dir,void *CInSim, void *Message,void *Bank,void *RCdl, void * STreet)
{
    strcpy(RootDir,dir); //  опируем путь до программы


    insim = (CInsim *)CInSim; // ѕрисваиваем указателю область пам€ти
    if(!insim) // ѕровер€ем на существование
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
    if(!bank)
    {
        printf ("Can't struct RCBank class");
        return -1;
    }

    dl = (RCDL *)RCdl;
    if(!dl)
    {
        printf ("Can't struct RCDL class");
        return -1;
    }

    street = (RCStreet *)STreet;
    if(!street)
    {
        printf ("Can't struct RCStreet class");
        return -1;
    }

    accept_time = time(&acctime) + 60;

    inited = 1;

    return 0;
}


void RCTaxi::readconfig(char *Track)
{
    //cout << "RCTaxi::readconfig\n" ;
    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCTaxi\\tracks\\%s.txt",RootDir,Track);
    //cout << file << endl;


    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        printf ("RCTaxi: Can't find \n%s",file);
        return;
    }
    FindClose(fff);

    ifstream readf (file,ios::in);


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

            if (strncmp(str,"/points",5)==0)
            {
                readf.getline(str,128);
                int count = atoi(str);
                PointCount = count;

                Points = new Taxi_points[PointCount];

                for (int i=0 ; i<PointCount; i++)
                {
                    readf.getline(str,128);
                    char * X;
                    char * Y;
                    X = strtok (str,",");
                    Y = strtok (NULL,",");
                    Points[i].X = atoi(X);
                    Points[i].Y = atoi(Y);

                    printf("X= %d Y= %d\n",Points[i].X,Points[i].Y);
                }
            }



        } // if strlen > 0
    } //while readf.good()

    readf.close();
}

void RCTaxi::next_packet()
{
    switch (insim->peek_packet())
    {
    case ISP_MSO:
        taxi_mso();
        break;


    case ISP_NPL:
        taxi_npl();
        break;

    case ISP_NCN:
        taxi_ncn();
        break;

    case ISP_CNL:
        taxi_cnl();
        break;

    case ISP_PLL:
        taxi_pll();
        break;

    case ISP_PLP:
        taxi_plp();
        break;

    case ISP_CPR:
        taxi_crp();
        break;


    }
}

void RCTaxi::accept_user()
{
    //cout << accept_time - time(&acctime) << endl;
    if ( accept_time >= time(&acctime))
        return;

    printf("RCTaxi: accept function\n");

    //int r = rand()%3 + 1;
    int taxi_time = 600/(NumP+1);
    accept_time += taxi_time;

    for (int i=0; i< 32; i++)
    {
        if ((players[i].Work == 1) and (players[i].WorkNow == 1) and (players[i].WorkAccept == 0))
        {
            // accept player
            printf("RCTaxi: found %s\n",players[i].UName);

            int CurStreet = street->CurentStreetNum(players[i].UCID);
            int DestStreet = 0;
            srand(time(NULL));
            bool ok = true;
            while (ok)
            {
                DestStreet = rand()%(street->StreetCount());
                cout << DestStreet << endl;

                if (DestStreet != CurStreet)
                    ok = false;
            }

            printf("RCTaxi: street == %d\n",DestStreet);

            struct streets StreetInfo;
            memset(&StreetInfo,0,sizeof(streets));
            int DestPoint = 0;
            if (street->CurentStreetInfo(&StreetInfo,players[i].UCID))
            {
                for (int z=0;z<StreetInfo.PointCount;z++)
                {
                    printf("RCTaxi: StreetInfo.X[] = %d  StreetInfo.Y[] = %d\n",StreetInfo.StreetX[z],StreetInfo.StreetY[z]);
                }
                ok = true;
               /* while (ok)
                {
                    DestPoint = rand()%PointCount;
                    printf("RCTaxi: test dest point == %d\n",DestPoint);

                    if (Check_Pos(StreetInfo.PointCount,StreetInfo.StreetX,StreetInfo.StreetY,Points[DestPoint].X,Points[DestPoint].Y))
                        ok = false;

                    Sleep(1000);
                }*/

                for (int x=0;x<PointCount;x++)
                {
                    printf("RCTaxi: Points[].X = %d  Points[].Y = %d\n",Points[x].X,Points[x].Y);
                    if (Check_Pos(StreetInfo.PointCount,StreetInfo.StreetX,StreetInfo.StreetY,Points[x].X,Points[x].Y))
                    {
                        printf("TRUUUUUUUUUUUUUEEEEEEEEEEEEEEEE\n");
                    }
                }

                 printf("RCTaxi: dest point == %d\n",DestPoint);
                 players[i].WorkPointDestinaion = DestPoint;
                 players[i].WorkStreetDestinaion = DestStreet;

                 memset(&StreetInfo,0,sizeof(streets));
                 street->CurentStreetInfoByNum(&StreetInfo,DestStreet);

                 char Msg[96];
                 sprintf(Msg,"^C^7«аберите клиента на %s ",StreetInfo.Street);
                 send_mtc(players[i].UCID,Msg);
            }

        }
    }
}


void RCTaxi::taxi_cnl ()
{
    int i;

    struct IS_CNL *pack_cnl = (struct IS_CNL*)insim->get_packet();

    // Find player and set the whole player struct he was using to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_cnl->UCID)
        {
            save_user(&players[i]);
            memset(&players[i],0,sizeof(struct TaxiPlayer));
            NumP --;
            break;
        }
    }
}

void RCTaxi::taxi_crp()
{
    int i;

    struct IS_CPR *pack_cpr = (struct IS_CPR*)insim->get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_cpr->UCID)
        {
            /** DO SOME CODE **/

            strcpy(players[i].PName, pack_cpr->PName);
            break;
        }
    }
}

void RCTaxi::taxi_mci ()
{
    //cout << "taxi_mci" << endl;

    struct IS_MCI *pack_mci = (struct IS_MCI*)insim->udp_get_packet();


    for (int i = 0; i < pack_mci->NumC; i++) // прогон по всему массиву pack_mci->Info[i]

    {
        for (int j =0; j < MAX_PLAYERS; j++) // прогон по всему массиву players[j]
        {

            if (pack_mci->Info[i].PLID == players[j].PLID) // стара€ заплатка ( and players[j].PLID != 0 and players[j].UCID != 0)
            {
                /** DO SOME CODE **/

                int X = pack_mci->Info[i].X/65536;
                int Y = pack_mci->Info[i].Y/65536;

                // ≈сли игрок находитс€ в зоне приема на работу, выводим воспомогательные сообщени€ в чат

                int TX[4] = {-465,-465,-480,-480};
                int TY[4] = {450,420,420,450};
                if (Check_Pos(4,TX,TY,X,Y))
                {
                    //cout << "in work place" << endl;
                    if (players[j].InZone == 0)
                    {
                        players[j].InZone = 1;
                        send_mtc(players[j].UCID,"^3Taxi Radriges");
                        send_mtc(players[j].UCID,"^2!deal");
                        send_mtc(players[j].UCID,"^2!undeal");
                        send_mtc(players[j].UCID,"^2!workstart");
                        send_mtc(players[j].UCID,"^2!workend");
                    }
                }
                else
                {
                    if (players[j].InZone == 1)
                        players[j].InZone = 0;
                }

                /** player drive on dest street **/
                if (players[j].WorkNow == 1)
                {
                    if (players[j].WorkStreetDestinaion == street->CurentStreetNum(players[j].UCID))
                {
                    if (players[j].OnStreet == false)
                    {
                        printf("RCTaxi: %s drove to the street\n", players[j].UName);
                        players[j].OnStreet = true;
                        /** вычисл€ем расто€ние до точки остановки **/
                        printf("RCTaxi: Calculate the distance\n");
                        int des_X = Points[players[j].WorkPointDestinaion].X;
                        int des_Y = Points[players[j].WorkPointDestinaion].Y;
                        printf("RCTaxi: Dest. Point X=%d, Y=%d\n",des_X,des_Y);

                        float Dist = sqrtf(pow(X-des_X,2)+pow(Y-des_Y,2));

                        printf("RCTaxi: Dest. Distance = %1.2f\n",Dist);

                        char MSG[96];
                        sprintf(MSG,"^C^7ќстановитесь через %3.0f метров.",Dist);
                        //send_mtc(players[j].UCID,MSG);

                    }
                }
                else
                {
                    players[j].OnStreet = false;
                }
                }

                if (players[j].Start == 1)
                {

                    bool newPoint = true;
                    for (int f=0; f<200; f++)
                    {
                        if (Points[f].Id != 0)
                        {
                            float Dist = sqrt(pow((X-Points[f].X),2)+pow((Y-Points[f].Y),2));

                            if (Dist < 100)
                                newPoint = false;
                        }
                    }

                    if (newPoint == true)
                    {
                        for (int f=0; f<200; f++)
                        {
                            if (Points[f].Id == 0)
                            {
                                Points[f].Id = 1;
                                Points[f].X = X;
                                Points[f].Y = Y;

                                send_mst("^7Added new point;");
                                break;
                            }
                        }
                    }

                }


                /** NOTE: don't use break **/
            } // if pack_mci->Info[i].PLID == players[j].PLID
        }
    }

    /** thread xD **/
    accept_user();


    /** thread **/
}

void RCTaxi::taxi_mso ()
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

    char Message[96];
    strcpy(Message,pack_mso->Msg + ((unsigned char)pack_mso->TextStart));


    if (strncmp(Message, "!save", strlen("!save")) == 0 )
    {
        /** DO SOME CODE **/
        save_user(&players[i]);

    }
    if (strncmp(Message, "!deal", strlen("!deal")) == 0 )
    {
        /** DO SOME CODE **/
        if (players[i].Work !=0)
        {
            send_mtc(players[i].UCID,"Ti uze rabotaew");
            return;
        }

        time_t nt;
        int nowtime = time(&nt);
        if (players[i].FiredPenalty > nowtime)
        {
            send_mtc(players[i].UCID,"uvolen do 'tut vremya'");
            return;
        }

        send_mtc(players[i].UCID,"Ti prinyat");
        players[i].Work = 1;

    }

    if (strncmp(Message, "!undeal", strlen("!undeal")) == 0 )
    {
        /** DO SOME CODE **/
        if (players[i].Work ==0)
        {
            send_mtc(players[i].UCID,"Ti uze prinyat na rabotu");
            return;
        }

        send_mtc(players[i].UCID,"dogovor rastorgnut");
        players[i].Work = 0;
    }

    if (strncmp(Message, "!workstart", strlen("!workstart")) == 0 )
    {
        /** DO SOME CODE **/
        if (players[i].Work ==0)
        {
            send_mtc(players[i].UCID,"Ti dolzen ustroet'sya");
            return;
        }
        if (players[i].WorkNow ==1)
        {
            send_mtc(players[i].UCID,"Ti uze rabotaesh");
            return;
        }
        players[i].WorkNow = 1;
        send_mtc(players[i].UCID,"Rabota na4ata");

    }

    if (strncmp(Message, "!workend", strlen("!workend")) == 0 )
    {
        /** DO SOME CODE **/
        if (players[i].Work ==0)
        {
            send_mtc(players[i].UCID,"Ti dolzen ustroet'sya");
            return;
        }
        if (players[i].WorkNow ==0)
        {
            send_mtc(players[i].UCID,"Ti uze ne rabotaesh");
            return;
        }
        players[i].WorkNow = 0;
        send_mtc(players[i].UCID,"Rabota zakon4ena");

    }
    if (strncmp(Message, "!start", strlen("!start")) == 0 )
    {
        /** DO SOME CODE **/
        if (players[i].Start ==0)
        {
            players[i].Start =1;
        }
        else
        {
            players[i].Start =0;

            ofstream readf("PoInTs.txt",ios::out);
            for (int f=0;f<200;f++)
            {
                if (Points[f].Id != 0)
                {
                    readf << Points[f].X << "," << Points[f].Y << endl;
                }
            }
        }

    }

}


void RCTaxi::taxi_ncn()
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

    read_user(&players[i]);

    NumP ++;

    /** DO SOME CODE **/


}

void RCTaxi::taxi_npl()
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
            /** DO SOME CODE **/
            break;
        }
    }
}

void RCTaxi::taxi_plp()
{
    //cout << "player leaves race" << endl;
    int i;

    struct IS_PLP *pack_plp = (struct IS_PLP*)insim->get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == pack_plp->PLID)
        {
            /** DO SOME CODE **/
            players[i].PLID = 0;
            /** DO SOME CODE **/
            break;
        }
    }
}

void RCTaxi::taxi_pll()
{
    //cout << "player leaves race" << endl;
    int i;

    struct IS_PLL *pack_pll = (struct IS_PLL*)insim->get_packet();

    // Find player and set his PLID to 0
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == pack_pll->PLID)
        {
            /** DO SOME CODE **/
            players[i].PLID = 0;
            /** DO SOME CODE **/
            break;
        }
    }
}

void RCTaxi::read_user(struct TaxiPlayer *splayer)
{
    /** read file **/

    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCTaxi\\users\\%s.txt",RootDir,splayer->UName);
    //strcat(file,"tracks\\.txt");

    // Read Data From File

    // Try Find New File
    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        printf("RCTaxi: Can't find %s\nRCTaxi: Create File for user %s",file,splayer->UName);
        // create
        splayer->Work= 0;
        splayer->FiredPenalty= 0;
        splayer->PenaltyCount = 0;
        splayer->PassCount = 0;
        save_user(splayer);
    }
    else
    {
        ifstream readf (file,ios::in);

        while (readf.good())
        {
            char str[128];
            readf.getline(str,128);
            if (strlen(str) > 0)
            {

                if (strncmp("Work=",str,strlen("Work="))==0)
                {
                    splayer->Work = atoi(str+strlen("Work="));
                }

                if (strncmp("FiredPenalty=",str,strlen("FiredPenalty="))==0)
                {
                    splayer->FiredPenalty= atoi(str+strlen("FiredPenalty="));
                }

                if (strncmp("PenaltyCount=",str,strlen("PenaltyCount="))==0)
                {
                    splayer->PenaltyCount = atoi(str+strlen("PenaltyCount="));
                }

                if (strncmp("PassCount=",str,strlen("PassCount="))==0)
                {
                    splayer->PassCount = atoi(str+strlen("PassCount="));
                }


            }
        }



        readf.close();
    }
    FindClose(fff);
}

void RCTaxi::save_user(struct TaxiPlayer *splayer)
{
    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCTaxi\\users\\%s.txt",RootDir,splayer->UName);
    //printf(file);

    ofstream writef (file,ios::out);
    writef << "Work=" << splayer->Work << endl;
    writef << "FiredPenalty=" << splayer->FiredPenalty << endl;
    writef << "PenaltyCount=" << splayer->PenaltyCount << endl;
    writef << "PassCount=" << splayer->PassCount << endl;
    writef.close();
}


void RCTaxi::send_mtc (byte UCID,char* Msg)
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
}

void RCTaxi::send_mst (char* Text)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    strcpy(pack_mst.Msg,Text);
    insim->send_packet(&pack_mst,errmsg);
}

void RCTaxi::send_bfn (byte UCID, byte ClickID)
{
    struct IS_BFN pack;
    memset(&pack, 0, sizeof(struct IS_BFN));
    pack.Size = sizeof(struct IS_BFN);
    pack.Type = ISP_BFN;
    pack.UCID = UCID;
    pack.ClickID = ClickID;
    insim->send_packet(&pack,errmsg);
}
