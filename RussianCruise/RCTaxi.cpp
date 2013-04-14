using namespace std;
#include "RCTaxi.h"

RCTaxi::RCTaxi()
{
    players = new TaxiPlayer[MAX_PLAYERS];
    memset(players, 0, sizeof( TaxiPlayer ) * MAX_PLAYERS );
}

RCTaxi::~RCTaxi()
{
    delete[] players;
}

int RCTaxi::init(const char *dir,void *CInSim, void *Message,void *Bank,void *RCdl, void * STreet, void *Pizza)
{
    strcpy(RootDir,dir); // Копируем путь до программы
    insim = (CInsim *)CInSim; // Присваиваем указателю область памяти
    if(!insim)
    {
        printf ("Can't struct CInsim class");    // Проверяем на существование
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

void RCTaxi::readconfig(const char *Track)
{
    cout << "RCTaxi::readconfig\n" ;
    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCTaxi\\tracks\\%s.txt",RootDir,Track);

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

        if (strlen(str) > 0)
        {
            if (strncmp(str,"/dealer",7)==0)
            {
                readf.getline(str,128);
                int count = atoi(str);
                zone.NumPoints = count;
                for ( int i = 0 ; i < count ; i++ )
                {
                    readf.getline(str,128);
                    char * X;
                    char * Y;
                    X = strtok (str,",");
                    Y = strtok (NULL,",");
                    zone.dealX[i] = atoi(X);
                    zone.dealY[i] = atoi(Y);
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
                    char * StrId;
                    X = strtok (str,",");
                    Y = strtok (NULL,",");
                    StrId = strtok (NULL,",");

                    Points[i].X = atoi(X);
                    Points[i].Y = atoi(Y);
                    Points[i].StreetId = atoi(StrId);
                }
            }
        } // if strlen > 0
    } //while readf.good()

    readf.close();

    sprintf(file,"%sdata\\RCTaxi\\dialog.txt",RootDir);

    HANDLE ff;
    // WIN32_FIND_DATA fd;
    ff = FindFirstFile(file,&fd);
    if (ff == INVALID_HANDLE_VALUE)
    {
        printf ("RCTaxi: Can't find \n%s",file);
        return;
    }
    FindClose(ff);

    ifstream read (file,ios::in);

    while (read.good())
    {
        char str[128];
        read.getline(str,128);

        if (strncmp(str,"#con",5)==0)
        {
            read.getline(str,128);
            int count = atoi(str);
            DialContCount = count;

            for (int i=0 ; i<count; i++)
            {
                read.getline(str,128);
                strcpy(Dialog_Cont[i],str);
            }
        }

        if (strncmp(str,"#obh",5)==0)
        {
            read.getline(str,128);
            int count = atoi(str);
            DialObhCount = count;


            for (int i=0 ; i<count; i++)
            {
                read.getline(str,128);
                memset(&Dialog_Obh[i],0,128);
                strcpy(Dialog_Obh[i],str);
            }
        }

        if (strncmp(str,"#dist",5)==0)
        {
            read.getline(str,128);
            int count = atoi(str);
            DialDistCount = count;

            for (int i=0 ; i<count; i++)
            {
                read.getline(str,128);
                strcpy(Dialog_Dist[i],str);
            }
        }

        if (strncmp(str,"#done",5)==0)
        {
            read.getline(str,128);
            int count = atoi(str);
            DialDoneCount = count;

            for (int i=0 ; i<count; i++)
            {
                read.getline(str,128);
                strcpy(Dialog_Done[i],str);
            }
        }

        if (strncmp(str,"#past",5)==0)
        {
            read.getline(str,128);
            int count = atoi(str);
            DialPastCount = count;

            for (int i=0 ; i<count; i++)
            {
                read.getline(str,128);
                strcpy(Dialog_Past[i],str);
            }
        }

        if (strncmp(str,"#needstop",5)==0)
        {
            read.getline(str,128);
            int count = atoi(str);
            DialStopCount = count;

            for (int i=0 ; i<count; i++)
            {
                read.getline(str,128);
                strcpy(Dialog_Stop[i],str);
            }
        }

        if (strncmp(str,"#exit",5)==0)
        {
            read.getline(str,128);
            int count = atoi(str);
            DialExitCount = count;

            for (int i=0 ; i<count; i++)
            {
                read.getline(str,128);
                strcpy(Dialog_Exit[i],str);
            }
        }

        if (strncmp(str,"#speed",5)==0)
        {
            read.getline(str,128);
            int count = atoi(str);
            DialSpeedCount = count;

            for (int i=0 ; i<count; i++)
            {
                read.getline(str,128);
                strcpy(Dialog_Speed[i],str);
            }
        }
    }
    read.close();

    /**клиенты-маршалы**/
    sprintf(file,"%sdata\\RCTaxi\\tracks\\%sclient.txt",RootDir,Track);

    HANDLE tt;
    tt = FindFirstFile(file,&fd);
    if (tt == INVALID_HANDLE_VALUE)
    {
        printf ("RCTaxi: Can't find \n%s",file);
        return;
    }
    FindClose(tt);
    ifstream readt (file,ios::in);

    int i=-1;
    while (readt.good())
    {
        char str[128];
        readt.getline(str,128);

        int c = atoi(strtok (str,", "));
        if (c!=0)
        {
            i++;
            ClientPoints[i].X = c;
            ClientPoints[i].Y = atoi(strtok (NULL,", "));
            ClientPoints[i].Z = atoi(strtok (NULL,", "));
            ClientPoints[i].Dir = atoi(strtok (NULL,", "));
            ClientPoints[i].StreetId = atoi(strtok (NULL,", "));
            //printf("%d.   %d, %d, %d, %d\n",i,ClientPoints[i].X,ClientPoints[i].Y,ClientPoints[i].Dir,ClientPoints[i].StreetId);
        }
        ClientCount=i;
    }
    readt.close();
}


void RCTaxi::accept_user()
{
    if ( accept_time >= time(&acctime)) return;
    int taxi_time = 600/(NumP+1);
    accept_time += taxi_time;
    for (int i=0; i< 32; i++)
    {
        if ((players[i].Work == 1) and (players[i].WorkNow == 1) and (players[i].WorkAccept == 0))
        {
            int DestPoint = 0;

            srand(time(NULL));
            bool ok = true;
            while (ok)
            {
                DestPoint = rand()%ClientCount;
                if (ClientPoints[DestPoint].StreetId != street->CurentStreetNum(players[i].UCID)) ok = false;
                Sleep(100);
            }

            struct streets StreetInfo;
            memset(&StreetInfo,0,sizeof(streets));

            if (street->CurentStreetInfo(&StreetInfo,players[i].UCID))
            {
                players[i].ClientID = DestPoint; //ID клиента
                players[i].WorkPointDestinaion = DestPoint;
                players[i].WorkStreetDestinaion = ClientPoints[DestPoint].StreetId; //улица назначения

                //рисую маршала
                players[i].HandUp=false; //маршал стоит с опущенной рукой


                struct IS_AXM pacAXM;
                memset(&pacAXM, 0, sizeof(struct IS_AXM));
                pacAXM.Info[0].Index=255;
                pacAXM.Info[0].Heading=ClientPoints[players[i].ClientID].Dir;
                pacAXM.Info[0].X=ClientPoints[players[i].ClientID].X/4096;
                pacAXM.Info[0].Y=ClientPoints[players[i].ClientID].Y/4096;
                pacAXM.Info[0].Zchar=ClientPoints[players[i].ClientID].Z;
                pacAXM.Info[0].Flags=133;
                pacAXM.Type=ISP_AXM;
                pacAXM.ReqI=1;
                pacAXM.NumO=1;
                pacAXM.Size=8+pacAXM.NumO*8;
                pacAXM.PMOAction = PMO_ADD_OBJECTS;
                insim->send_packet(&pacAXM);

                memset(&StreetInfo,0,sizeof(streets));
                street->CurentStreetInfoByNum(&StreetInfo,players[i].WorkStreetDestinaion);

                char Msg[96];
                sprintf(Msg,"^6|^C^7 Заберите клиента на %s ",StreetInfo.Street);
                send_mtc(players[i].UCID,Msg);
                sprintf(Msg,"^C^7Заберите клиента на %s ",StreetInfo.Street);
                btn_information(players[i].UCID,Msg);
                players[i].WorkAccept = 1;
            }
            break;
        }
    }
}

void RCTaxi::accept_user2(byte UCID)
{
    for (int i=0; i< 32; i++)
    {
        if (players[i].UCID == UCID)
        {
            srand(time(NULL)); //???

            int DestPoint = 0;
            bool ok = true;
            while (ok)
            {
                DestPoint = rand()%ClientCount;
                if (ClientPoints[DestPoint].StreetId != street->CurentStreetNum(players[i].UCID)) ok = false;
                Sleep(100);
            }

            int DestStreet = ClientPoints[DestPoint].StreetId; //улица назначения

            struct streets StreetInfo;
            memset(&StreetInfo,0,sizeof(streets));

            if (street->CurentStreetInfo(&StreetInfo,players[i].UCID))
            {
                //удаляю маршала


                struct IS_AXM pacAXM;
                memset(&pacAXM, 0, sizeof(struct IS_AXM));
                pacAXM.Info[0].Index=255;
                pacAXM.Info[0].Heading=ClientPoints[players[i].WorkPointDestinaion].Dir;
                pacAXM.Info[0].X=ClientPoints[players[i].WorkPointDestinaion].X/4096;
                pacAXM.Info[0].Y=ClientPoints[players[i].WorkPointDestinaion].Y/4096;
                pacAXM.Info[0].Zchar=ClientPoints[players[i].WorkPointDestinaion].Z;
                pacAXM.Info[0].Flags=135;
                pacAXM.Type=ISP_AXM;
                pacAXM.ReqI=1;
                pacAXM.NumO=1;
                pacAXM.Size=8+pacAXM.NumO*8;
                pacAXM.PMOAction = PMO_DEL_OBJECTS;
                insim->send_packet(&pacAXM);


                players[i].WorkPointDestinaion = DestPoint;
                players[i].WorkStreetDestinaion = DestStreet;
                memset(&StreetInfo,0,sizeof(streets));
                street->CurentStreetInfoByNum(&StreetInfo,DestStreet);

                char Msg[96];
                sprintf(Msg,"^6|^C^7 Отвези меня на %s ",StreetInfo.Street);
                send_mtc(players[i].UCID,Msg);
                sprintf(Msg,"^C^7Отвези клиента на %s ",StreetInfo.Street);
                btn_information(players[i].UCID,Msg);
                players[i].WorkAccept = 2;
                players[i].PassStress = rand()%500;
                players[i].instr=false;
            }
            break;
        }
    }
}


void RCTaxi::insim_cnl( struct IS_CNL* packet )
{
    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == packet->UCID)
        {
            //удаляю маршалов



            struct IS_AXM pacAXM;
            memset(&pacAXM, 0, sizeof(struct IS_AXM));
            pacAXM.Info[0].Index=255;
            pacAXM.Info[0].Heading=ClientPoints[players[i].WorkPointDestinaion].Dir;
            pacAXM.Info[0].X=ClientPoints[players[i].WorkPointDestinaion].X/4096;
            pacAXM.Info[0].Y=ClientPoints[players[i].WorkPointDestinaion].Y/4096;
            pacAXM.Info[0].Zchar=ClientPoints[players[i].WorkPointDestinaion].Z;
            pacAXM.Info[0].Flags=133;
            pacAXM.Type=ISP_AXM;
            pacAXM.ReqI=1;
            pacAXM.NumO=1;
            pacAXM.Size=8+pacAXM.NumO*8;
            pacAXM.PMOAction = PMO_DEL_OBJECTS;
            insim->send_packet(&pacAXM);




            pacAXM.Info[0].Flags=135;
            insim->send_packet(&pacAXM);

            save_user(&players[i]);
            memset(&players[i],0,sizeof(struct TaxiPlayer));
            NumP --;
            break;
        }
    }
}

void RCTaxi::insim_cpr( struct IS_CPR* packet )
{
    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == packet->UCID)
        {
            strcpy(players[i].PName, packet->PName);
            break;
        }
    }
}

byte ddd=0;
void RCTaxi::insim_mci ( struct IS_MCI* pack_mci )
{
    float Dist;
    char d[96];

    for (int i = 0; i < pack_mci->NumC; i++) // прогон по всему массиву pack_mci->Info[i]
    {
        for (int j =0; j < MAX_PLAYERS; j++) // прогон по всему массиву players[j]
        {
            if (pack_mci->Info[i].PLID == players[j].PLID)
            {
                int X = pack_mci->Info[i].X/65536;
                int Y = pack_mci->Info[i].Y/65536;
                int Speed = ((int)pack_mci->Info[i].Speed*360)/(32768);
                if(Check_Pos(4,zone.dealX,zone.dealY,X,Y))
                {
                    if (players[j].InZone == 0)
                    {
                        players[j].InZone = 1;
                        send_mtc(players[j].UCID,"^6| ^3Taxi Radriges");
                        send_mtc(players[j].UCID,"^6| ^2!deal - ^CУстроиться");
                        send_mtc(players[j].UCID,"^6| ^2!undeal - ^CУволиться");
                        send_mtc(players[j].UCID,"^6| ^2!workstart - ^CНачать работу");
                        send_mtc(players[j].UCID,"^6| ^2!workend - ^CЗакончить работу");
                    }
                }
                else if (players[j].InZone == 1) players[j].InZone = 0;



                /** player drive on dest street **/
                if (players[j].WorkNow == 1 and players[j].WorkAccept != 0)
                {
                    if(players[j].WorkAccept == 2)
                    {
                        char dd[96];
                        struct streets StreetInfo;
                        memset(&StreetInfo,0,sizeof(streets));
                        street->CurentStreetInfoByNum(&StreetInfo,street->CurentStreetNum(players[j].UCID));
                        if (Speed>StreetInfo.SpeedLimit)
                        {
                            if (players[j].spd==5) players[j].spd=0;
                            if (players[j].spd==0)
                            {
                                send_mtc(players[j].UCID,Dialog_Speed[rand()%DialSpeedCount]); // превышаешь скорость
                            }
                            players[j].spd++;
                            players[j].PassStress += 10;
                        }
                    }

                    if (players[j].WorkStreetDestinaion == street->CurentStreetNum(players[j].UCID))
                    {
                        int des_X = ClientPoints[players[j].WorkPointDestinaion].X/65536;
                        int des_Y = ClientPoints[players[j].WorkPointDestinaion].Y/65536;
                        /** вычисляем растояние до точки остановки **/
                        Dist = Distance(X , Y , des_X , des_Y);
                        sprintf(d," ^7%4.0f ^Cм",Dist);
                        btn_Dist(&players[j],d);

                        if (players[j].OnStreet == false)
                        {
                            players[j].OnStreet = true;
                            char MSG[96];
                            sprintf(MSG,"^6| ^C^7Остановитесь через %3.0f метров",(Dist-(int)Dist%10));
                            send_mtc(players[j].UCID,MSG);
                        }

                        if (Dist < 30)
                        {
                            //маршал поднимает руку
                            if (!players[j].HandUp and players[j].WorkAccept == 1)
                            {
                                //направление на авто
                                int xx = (ClientPoints[players[i].WorkPointDestinaion].X - pack_mci->Info[i].X)/65536;
                                int yy = (ClientPoints[players[i].WorkPointDestinaion].Y - pack_mci->Info[i].Y)/65536;
                                int gip = sqrt(xx*xx+yy*yy);

                                float c = (float)(yy)/(float)(gip)*(-xx/abs(xx));
                                float ddd = acos(c)*180/3.14+180;
                                if (ddd>360) ddd=ddd-360;if (ddd<0) ddd=ddd+360;
                                ddd = ddd/360*256;

                                players[j].HandUp=true;


                                struct IS_AXM pacAXM;
                                memset(&pacAXM, 0, sizeof(struct IS_AXM));
                                pacAXM.Info[0].Index=255;
                                pacAXM.Info[0].Heading=(byte)ddd;
                                pacAXM.Info[0].X=ClientPoints[players[i].WorkPointDestinaion].X/4096;
                                pacAXM.Info[0].Y=ClientPoints[players[i].WorkPointDestinaion].Y/4096;
                                pacAXM.Info[0].Zchar=ClientPoints[players[i].WorkPointDestinaion].Z;
                                pacAXM.Info[0].Flags=133;
                                pacAXM.Type=ISP_AXM;
                                pacAXM.ReqI=1;
                                pacAXM.NumO=1;
                                pacAXM.Size=8+pacAXM.NumO*8;
                                pacAXM.PMOAction = PMO_DEL_OBJECTS;
                                insim->send_packet(&pacAXM);


                                pacAXM.Info[0].Flags=135;
                                pacAXM.PMOAction = PMO_ADD_OBJECTS;
                                insim->send_packet(&pacAXM);
                            }

                            if (players[j].WorkAccept == 2)
                            {
                                if (players[j].InPasZone != 1)
                                {
                                    players[j].InPasZone = 1;
                                    srand ( time(NULL) );
                                    send_mtc(players[j].UCID,Dialog_Dist[rand()%DialDistCount]); // приехали
                                }
                            }

                            if (Speed < 5 and players[j].WorkAccept == 2) taxi_done(&players[j]); //высадили
                        }
                        else
                        {
                            //маршал опускает руку
                            if (players[j].HandUp and players[j].WorkAccept == 1)
                            {
                                players[j].HandUp=false;


                                struct IS_AXM pacAXM;
                                memset(&pacAXM, 0, sizeof(struct IS_AXM));
                                pacAXM.Info[0].Index=255;
                                pacAXM.Info[0].Heading=ClientPoints[players[i].WorkPointDestinaion].Dir;
                                pacAXM.Info[0].X=ClientPoints[players[i].WorkPointDestinaion].X/4096;
                                pacAXM.Info[0].Y=ClientPoints[players[i].WorkPointDestinaion].Y/4096;
                                pacAXM.Info[0].Zchar=ClientPoints[players[i].WorkPointDestinaion].Z;
                                pacAXM.Info[0].Flags=135;
                                pacAXM.Type=ISP_AXM;
                                pacAXM.ReqI=1;
                                pacAXM.NumO=1;
                                pacAXM.Size=8+pacAXM.NumO*8;
                                pacAXM.PMOAction = PMO_DEL_OBJECTS;
                                insim->send_packet(&pacAXM);


                                pacAXM.Info[0].Flags=133;
                                pacAXM.PMOAction = PMO_ADD_OBJECTS;
                                insim->send_packet(&pacAXM);
                            }

                            if (players[j].WorkAccept == 2)
                            {
                                if (players[j].InPasZone == 1)
                                {
                                    if (!players[j].instr) players[j].instr=true;
                                    else
                                    {
                                        players[j].InPasZone = 0;
                                        players[j].PassStress += 10;
                                        srand ( time(NULL) );
                                        send_mtc(players[j].UCID,Dialog_Past[rand()%DialPastCount]); // проехали мимо
                                        players[j].instr=false;
                                    }
                                }
                            }
                        }

                        if (Dist < 5 and Speed < 5 and players[j].WorkAccept == 1) accept_user2(players[j].UCID); // посадили
                    }
                    else
                    {
                        if (players[j].OnStreet = true) send_bfn(players[j].UCID,205);
                        players[j].OnStreet = false;
                    }

                    if (players[j].WorkAccept == 2)
                    {
                        int X = pack_mci->Info[i].X/65536;
                        int Y = pack_mci->Info[i].Y/65536;
                        int Z = pack_mci->Info[i].Z/65536;
                        int D = pack_mci->Info[i].Direction/182;
                        int H = pack_mci->Info[i].Heading/182;

                        int S = ((int)pack_mci->Info[i].Speed*360)/(32768);

                        int A = pack_mci->Info[i].AngVel*360/16384;

                        int X1 = players[j].Info.X/65536;
                        int Y1 = players[j].Info.Y/65536;
                        int Z1 = players[j].Info.Z/65536;
                        int D1 = players[j].Info.Direction/182;
                        int H1 = players[j].Info.Heading/182;
                        int S1 = ((int)players[j].Info.Speed*360)/(32768);
                        int A1 = players[j].Info.AngVel*360/16384;

                        long dA = A-A1;
                        long dS = S-S1;
                        long dD = abs((int)(sin(D)*100))-abs((int)(sin(D1)*100));
                        long dH = abs((int)(sin(H)*100))-abs((int)(sin(H1)*100));

                        //int K = int(sqrt(abs((dD-dH)*(1+dA)*dS))/32);
                        int K = (int)sqrt(abs((dD-dH)*(1+dA)*dS))/8;

                        players[j].PassStress += K;

                        if (players[j].PassStress > 1000)
                            players[j].PassStress = 1000;

                        if (X1==0 and Y1==0 and Z1==0)
                        {
                            X1=X;
                            Y1=Y;
                            Z1=Z;
                        }

                        btn_stress(&players[j]);
                        sprintf(d,"^7%d m.",(int)Dist);
                        btn_Dist(&players[j],d);

                        if (players[j].PassStress > 800)
                        {
                            if (players[j].StressOverCount == 0)
                            {
                                srand ( time(NULL) );
                                send_mtc(players[j].UCID,Dialog_Stop[rand()%DialStopCount]); // пугаецца, требует остановить
                            }

                            players[j].StressOverCount ++;

                            if (players[j].StressOverCount >= 20)
                                players[j].StressOverCount = 0;

                            if (Speed < 5)
                                taxi_done(&players[j]);
                        }
                    }
                }

                if (StartPointsAdd == true)
                {
                    bool newPoint = true;
                    for (int f=0; f<2048; f++)
                    {
                        if (PointsAdd[f].Id != 0)
                        {
                            float Dist = Distance(X,Y,PointsAdd[f].X,PointsAdd[f].Y);

                            if (Dist < 100)
                                newPoint = false;
                        }
                    }

                    if (newPoint == true)
                    {
                        for (int f=0; f<2048; f++)
                        {
                            if (PointsAdd[f].Id == 0)
                            {
                                PointsAdd[f].Id = 1;
                                PointsAdd[f].StreetId = street->CurentStreetNum(players[j].UCID);
                                PointsAdd[f].X = X;
                                PointsAdd[f].Y = Y;
                                PointCount ++;
                                char MSG[64];
                                sprintf(MSG,"^7Added new point [%d]{%d,%d}",PointCount,X,Y);
                                send_mst(MSG);
                                newPoint = false;
                                break;
                            }
                        }
                    }
                }
                memcpy( &players[j].Info , &pack_mci->Info[i] , sizeof( CompCar ) );
                /** NOTE: don't use break **/
            } // if pack_mci->Info[i].PLID == players[j].PLID
        }
    }


    /** thread xD **/
    accept_user();
    /** thread **/
}

void RCTaxi::dead_pass(byte UCID)
{
    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == UCID)
        {
            //printf("%d, %d, %d",players[i].Info.X,players[i].Info.Y,players[i].Info.Direction);
            if (abs(ClientPoints[players[i].WorkPointDestinaion].X/65536 - players[i].Info.X/65536)<50)
            {
                //удаляем маршала


                struct IS_AXM pacAXM;
                memset(&pacAXM, 0, sizeof(struct IS_AXM));
                pacAXM.Info[0].Index=255;
                pacAXM.Info[0].Heading=ClientPoints[players[i].WorkPointDestinaion].Dir;
                pacAXM.Info[0].X=ClientPoints[players[i].WorkPointDestinaion].X/4096;
                pacAXM.Info[0].Y=ClientPoints[players[i].WorkPointDestinaion].Y/4096;
                pacAXM.Info[0].Zchar=ClientPoints[players[i].WorkPointDestinaion].Z;
                pacAXM.Info[0].Flags=135;
                pacAXM.Type=ISP_AXM;
                pacAXM.ReqI=1;
                pacAXM.NumO=1;
                pacAXM.Size=8+pacAXM.NumO*8;
                pacAXM.PMOAction = PMO_DEL_OBJECTS;
                insim->send_packet(&pacAXM);

                players[i].WorkAccept = 0;
                players[i].WorkPointDestinaion = 0;
                players[i].WorkStreetDestinaion = 0;
                players[i].StressOverCount = 0;
                players[i].PassStress = 0;
                players[i].Work = 0;
                players[i].WorkNow = 0;
                send_mtc(players[i].UCID,"^6| ^7^CТы убил своего клиента - ^1УВОЛЕН!");
            }
        }
    }
}

void RCTaxi::insim_mso( struct IS_MSO* packet )
{
    int i;

    if (packet->UCID == 0)
        return;

    for (i=0; i < MAX_PLAYERS; i++)
        if (players[i].UCID == packet->UCID)
            break;

    char Message[96];
    strcpy(Message,packet->Msg + ((unsigned char)packet->TextStart));


    if (strncmp(Message, "!save", strlen("!save")) == 0 )
        save_user(&players[i]);

    if (players[i].InZone == 1)
    {
        if (strncmp(Message, "!deal", strlen("!deal")) == 0 )
        {
            if (dl->GetLVL(players[i].UCID) < 20)
            {
                send_mtc(players[i].UCID,"^6| ^C^7Маловат ты еще. Нужен уровень выше 20.");
                return;
            }
            /** DO SOME CODE **/
            if (players[i].Work !=0)
            {
                send_mtc(players[i].UCID,"^6| ^C^7Мозги мне не канифоль! Ты уже работаешь тут.");
                return;
            }

            send_mtc(players[i].UCID,"^6| ^C^7Ты принят");
            players[i].Work = 1;
        }

        if (strncmp(Message, "!undeal", strlen("!undeal")) == 0 )
        {
            if (players[i].Work ==0)
            {
                send_mtc(players[i].UCID,"^6| ^C^7Слушай, А! Ты и так уже тут не работаешь.");
                return;
            }

            send_mtc(players[i].UCID,"^6| ^C^7Уходишь от нас? Ну и ступай отсюда, другого найду.");
            players[i].WorkAccept = 0;
            players[i].WorkPointDestinaion = 0;
            players[i].WorkStreetDestinaion = 0;
            players[i].StressOverCount = 0;
            players[i].PassStress = 0;

            players[i].Work = 0;
                //удаляю маршалов


                struct IS_AXM pacAXM;
                memset(&pacAXM, 0, sizeof(struct IS_AXM));
                pacAXM.Info[0].Index=255;
                pacAXM.Info[0].Heading=ClientPoints[players[i].WorkPointDestinaion].Dir;
                pacAXM.Info[0].X=ClientPoints[players[i].WorkPointDestinaion].X/4096;
                pacAXM.Info[0].Y=ClientPoints[players[i].WorkPointDestinaion].Y/4096;
                pacAXM.Info[0].Zchar=ClientPoints[players[i].WorkPointDestinaion].Z;
                pacAXM.Info[0].Flags=133;
                pacAXM.Type=ISP_AXM;
                pacAXM.ReqI=1;
                pacAXM.NumO=1;
                pacAXM.Size=8+pacAXM.NumO*8;
                pacAXM.PMOAction = PMO_DEL_OBJECTS;
                insim->send_packet(&pacAXM);


                pacAXM.Info[0].Flags=135;
                insim->send_packet(&pacAXM);
        }

        if (strncmp(Message, "!workstart", strlen("!workstart")) == 0 )
        {
            if (players[i].Work ==0)
            {
                send_mtc(players[i].UCID,"^6| ^C^7Эй, еще не работаешь тут, а уже рвешься кататься!");
                return;
            }
            if (players[i].WorkNow ==1)
            {
                send_mtc(players[i].UCID,"^6| ^C^7Голову мне не морочь, ты и так уже на вахте.");
                return;
            }
            players[i].WorkNow = 1;
            send_mtc(players[i].UCID,"^6| ^C^7Все, иди работать.");
        }

        if (strncmp(Message, "!workend", strlen("!workend")) == 0 )
        {
            if (players[i].Work ==0)
            {
                send_mtc(players[i].UCID,"^6| ^C^7Эй, еще не работаешь тут, а уже увольняешься!");
                return;
            }
            if (players[i].WorkNow ==0)
            {
                send_mtc(players[i].UCID,"^6| ^C^7Я тебя уже отпустил домой.");
                return;
            }
            players[i].WorkNow = 0;
            send_mtc(players[i].UCID,"^6| ^C^7Сделал дело, вымой тело.");
            players[i].WorkAccept = 0;
            players[i].WorkPointDestinaion = 0;
            players[i].WorkStreetDestinaion = 0;
            players[i].StressOverCount = 0;
            players[i].PassStress = 0;

                //удаляю маршалов


                struct IS_AXM pacAXM;
                memset(&pacAXM, 0, sizeof(struct IS_AXM));
                pacAXM.Info[0].Index=255;
                pacAXM.Info[0].Heading=ClientPoints[players[i].WorkPointDestinaion].Dir;
                pacAXM.Info[0].X=ClientPoints[players[i].WorkPointDestinaion].X/4096;
                pacAXM.Info[0].Y=ClientPoints[players[i].WorkPointDestinaion].Y/4096;
                pacAXM.Info[0].Zchar=ClientPoints[players[i].WorkPointDestinaion].Z;
                pacAXM.Info[0].Flags=133;
                pacAXM.Type=ISP_AXM;
                pacAXM.ReqI=1;
                pacAXM.NumO=1;
                pacAXM.Size=8+pacAXM.NumO*8;
                pacAXM.PMOAction = PMO_DEL_OBJECTS;
                insim->send_packet(&pacAXM);


                pacAXM.Info[0].Flags=135;
                insim->send_packet(&pacAXM);
        }
    }

    if (strncmp(Message, "!test", strlen("!test")) == 0 )
    {
        //dead_pass(players[i].UCID);
        /** btn **/
        /*
        struct IS_BTN pack;
        memset(&pack, 0, sizeof(struct IS_BTN));
        pack.Size = sizeof(struct IS_BTN);
        pack.Type = ISP_BTN;
        pack.ReqI = 255;
        pack.UCID = 255;
        pack.Inst = 0;
        pack.TypeIn = 0;
        pack.ClickID = 15;
        pack.BStyle = 32+64;
        pack.L = 90;
        pack.T = 30;
        pack.W = 20;
        pack.H = 6;
        strcpy( pack.Text,"test");
        insim->send_packet(&pack);
        */

        /**axm add**/
        /*
        struct IS_AXM pacAXM;
        memset(&pacAXM, 0, sizeof(struct IS_AXM));
        pacAXM.Info[0].Index=255;
        pacAXM.Info[0].Heading=128;
        pacAXM.Info[0].X=6006;
        pacAXM.Info[0].Y=-6560;
        pacAXM.Info[0].Zchar=8;
        pacAXM.Info[0].Flags=133;
        pacAXM.Type=ISP_AXM;
        pacAXM.ReqI=1;
        pacAXM.NumO=1;
        pacAXM.Size=8+pacAXM.NumO*8;
        pacAXM.PMOAction = PMO_ADD_OBJECTS;
        insim->send_packet(&pacAXM);
        */
    }

    if ((strncmp(Message, "!start_points", strlen("!start_points")) == 0 ) and (strncmp(players[i].UName, "denis-takumi", strlen("denis-takumi")) == 0 ))
    {
        if (StartPointsAdd ==0)
        {
            StartPointsAdd =1;
            PointCount = 0;
            for (int f=0; f<2048; f++)
            {
                PointsAdd[f].Id = 0;
            }
        }
        else
        {
            StartPointsAdd =0;

            ofstream readf("PoInTs.txt",ios::out);
            readf << PointCount << endl;
            for (int f=0; f<2048; f++)
            {
                if (PointsAdd[f].Id != 0)
                {
                    readf << PointsAdd[f].X << "," << PointsAdd[f].Y << "," << PointsAdd[f].StreetId << endl;
                }
            }
        }

    }

}


void RCTaxi::insim_ncn( struct IS_NCN* packet )
{
    int i;

    if (packet->UCID == 0)
        return;

    for (i=0; i<MAX_PLAYERS; i++)
        if (players[i].UCID == 0)
            break;


    if (i == MAX_PLAYERS) return;
    strcpy(players[i].UName, packet->UName);
    strcpy(players[i].PName, packet->PName);
    players[i].UCID = packet->UCID;

    read_user(&players[i]);

    NumP ++;
}

void RCTaxi::insim_npl( struct IS_NPL* packet )
{
    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == packet->UCID)
        {
            players[i].PLID = packet->PLID;
            break;
        }
    }
}

void RCTaxi::insim_plp( struct IS_PLP* packet)
{
    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == packet->PLID)
        {
            players[i].PLID = 0;
            break;
        }
    }
}

void RCTaxi::insim_pll( struct IS_PLL* packet )
{
    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].PLID == packet->PLID)
        {
            players[i].PLID = 0;
            break;
        }
    }
}

void RCTaxi::read_user(struct TaxiPlayer *splayer)
{
    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCTaxi\\users\\%s.txt",RootDir,splayer->UName);

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
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
                    splayer->Work = atoi(str+strlen("Work="));

                if (strncmp("FiredPenalty=",str,strlen("FiredPenalty="))==0)
                    splayer->FiredPenalty= atoi(str+strlen("FiredPenalty="));

                if (strncmp("PenaltyCount=",str,strlen("PenaltyCount="))==0)
                    splayer->PenaltyCount = atoi(str+strlen("PenaltyCount="));

                if (strncmp("PassCount=",str,strlen("PassCount="))==0)
                    splayer->PassCount = atoi(str+strlen("PassCount="));
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

    ofstream writef (file,ios::out);
    writef << "Work=" << splayer->Work << endl;
    writef << "FiredPenalty=" << splayer->FiredPenalty << endl;
    writef << "PenaltyCount=" << splayer->PenaltyCount << endl;
    writef << "PassCount=" << splayer->PassCount << endl;
    writef.close();
}


void RCTaxi::taxi_done(TaxiPlayer *splayer)
{

    send_bfn(splayer->UCID,206);
    send_bfn(splayer->UCID,205);
    if (splayer->PassStress <= 800)
    {
        splayer->PassCount++;
        srand ( time(NULL) );
        send_mtc(splayer->UCID,Dialog_Done[rand()%DialDoneCount]); // send random dialog phrase

        //char Text[128];
        //sprintf(Text,"^6|^C^7 Вы заработали %d ^2RUR.\n",(1000 - splayer->PassStress/2));
        //send_mtc(splayer->UCID,Text);
        bank->AddCash(splayer->UCID,(1000 - splayer->PassStress/2), true);
        dl->AddSkill(splayer->UCID);
        btn_information_clear(splayer->UCID);
    }
    else
    {
        srand ( time(NULL) );
        send_mtc(splayer->UCID,Dialog_Exit[rand()%DialExitCount]); // send random dialog phrase
    }

    splayer->WorkAccept = 0;
    splayer->WorkPointDestinaion = 0;
    splayer->WorkStreetDestinaion = 0;
    splayer->StressOverCount = 0;
    splayer->PassStress = 0;
}

void RCTaxi::insim_con( struct IS_CON* packet )
{
    struct IS_CON *pack_con = (struct IS_CON*)insim->get_packet();

    for (int i=0; i<MAX_PLAYERS; i++)
    {
        if (players[i].PLID == pack_con->A.PLID)
        {
            if (players[i].WorkAccept == 2)
            {
                players[i].PassStress += 10 * pack_con->SpClose;

                srand ( time(NULL) );
                send_mtc(players[i].UCID,Dialog_Cont[rand()%DialContCount]); // send random dialog phrase
            }
            break;
        }
    }
    for (int j=0; j<MAX_PLAYERS; j++)
    {
        if (players[j].PLID == pack_con->B.PLID)
        {
            if (players[j].WorkAccept == 2)
            {
                players[j].PassStress += 10 * pack_con->SpClose;

                srand ( time(NULL) );
                send_mtc(players[j].UCID,Dialog_Cont[rand()%DialContCount]); // send random dialog phrase
            }
            break;
        }
    }
}


/*void RCTaxi::insim_axm( struct IS_AXM* packet )
{
    readAxm=true;
}*/


void RCTaxi::insim_obh( struct IS_OBH* packet )
{
    for (int i=0; i<MAX_PLAYERS; i++)
    {
        if (players[i].PLID == packet->PLID)
        {
            if (players[i].WorkAccept == 2)
            {
                if((packet->Index > 45 and packet->Index < 125) or (packet->Index > 140))
                {
                    players[i].PassStress +=  packet->SpClose;
                    srand ( time(NULL) );
                    send_mtc(players[i].UCID,Dialog_Obh[rand()%DialObhCount]); // send random dialog phrase
                }
                else players[i].PassStress +=  packet->SpClose/10;
            }
            break;
        }
    }
}

void RCTaxi::btn_stress(struct TaxiPlayer *splayer)
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;

    pack.ClickID = 206;
    pack.BStyle = 32+64;
    pack.L = 9;
    pack.T = 125;
    pack.W = 48;
    pack.H = 4;
    strcpy(pack.Text,"");

    for (int i=1; i<=splayer->PassStress/10; i++) // 1000 / 10 = 100
    {
        if (i == 1)
            strcat(pack.Text,"^2");
        if (i == 30)
            strcat(pack.Text,"^3");
        if (i == 80)
            strcat(pack.Text,"^1");

        strcat(pack.Text,"||");
    }
    strcat(pack.Text,"^8");
    for (int i=0; i<(100-splayer->PassStress/10); i++) strcat(pack.Text,"||");
    insim->send_packet(&pack);
}

void RCTaxi::btn_Dist(struct TaxiPlayer *splayer, const char* Text)
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = splayer->UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;

    pack.ClickID = 205;
    pack.BStyle = 32+64;
    pack.L = 1;
    pack.T = 125;
    pack.W = 8;
    pack.H = 4;

    strcpy(pack.Text,Text);

    insim->send_packet(&pack);
}

bool RCTaxi::IfWork (byte UCID)
{
    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID ==UCID)
        {
            if (players[i].Work != 0)
                return true;

            break;
        }
    }
    return false;
}
