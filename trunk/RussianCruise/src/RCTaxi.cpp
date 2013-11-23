using namespace std;
#include "RCTaxi.h"

RCTaxi::RCTaxi(const char* Dir)
{
    strcpy(RootDir,Dir);
    memset(ClientPoints, 0, sizeof( Taxi_clients ) * MAX_POINTS );
}

RCTaxi::~RCTaxi()
{

}

int RCTaxi::init(MYSQL *conn, CInsim *InSim, void *Message, void *Bank, void *RCdl, void * STreet, void * Police, void * Light)
{
    dbconn = conn;
    if (!dbconn)
    {
        printf("RCTaxi: Can't sctruct MySQL Connector\n");
        return -1;
    } //  опируем путь до программы
    insim = InSim; // ѕрисваиваем указателю область пам€ти
    if (!insim)
    {
        printf ("RCTaxi: Can't struct CInsim class");    // ѕровер€ем на существование
        return -1;
    }

    msg = (RCMessage *)Message;
    if (!msg)
    {
        printf ("Can't struct RCMessage class");
        return -1;
    }

    bank = (RCBank *)Bank;
    if (!bank)
    {
        printf ("Can't struct RCBank class");
        return -1;
    }

    lgh = (RCLight *)Light;
    if (!lgh)
    {
        printf ("Can't struct RCLight class");
        return -1;
    }

    dl = (RCDL *)RCdl;
    if (!dl)
    {
        printf ("Can't struct RCDL class");
        return -1;
    }

    street = (RCStreet *)STreet;
    if (!street)
    {
        printf ("Can't struct RCStreet class");
        return -1;
    }

    police = (RCPolice *)Police;
    if (!police)
    {
        printf ("Can't struct RCPolice class");
        return -1;
    }

    inited = 1;
    return 0;
}

void RCTaxi::readconfig(const char *Track)
{
    char file[MAX_PATH];
    sprintf(file, "%s\\data\\RCTaxi\\tracks\\%s.txt", RootDir, Track);
    // TODO: refactoring
    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file, &fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
    	CCText("  ^7RCTaxi     ^1ERROR: ^8file " + (string)file + " not found");
        return;
    }
    FindClose(fff);

    ifstream readf (file, ios::in);

    while (readf.good())
    {
        char str[128];
        readf.getline(str, 128);

        if (strlen(str) > 0)
        {
            if (strncmp(str, "/dealer", 7)==0)
            {
                readf.getline(str, 128);
                int count = atoi(str);
                zone.NumPoints = count;

                for ( int i = 0 ; i < count ; i++ )
                {
                    readf.getline(str, 128);
                    char * X;
                    char * Y;
                    X = strtok (str, ", ");
                    Y = strtok (NULL, ", ");
                    zone.dealX[i] = atoi(X);
                    zone.dealY[i] = atoi(Y);
                }
            }

            if (strncmp(str, "/shop", 5)==0)
            {
                readf.getline(str, 128);
                int count = atoi(str);
                TrackInf.ShopCount = count;

                for (int i=0 ; i<count; i++)
                {
                    readf.getline(str, 128);
                    char * X;
                    char * Y;
                    X = strtok (str, ";");
                    Y = strtok (NULL, ";");
                    TrackInf.XShop[i] = atoi(X);
                    TrackInf.YShop[i] = atoi(Y);
                }
            }

            if (strncmp(str, "/points", 5)==0)
            {
                readf.getline(str, 128);
                int count = atoi(str);
                PointCount = count;
                Points = new Taxi_points[PointCount];

                for (int i=0 ; i<PointCount; i++)
                {
                    readf.getline(str, 128);
                    char * X;
                    char * Y;
                    char * StrId;
                    X = strtok (str, ", ");
                    Y = strtok (NULL, ", ");
                    StrId = strtok (NULL, ", ");

                    Points[i].X = atoi(X);
                    Points[i].Y = atoi(Y);
                    Points[i].StreetId = atoi(StrId);
                }
            }
        }
    }
    readf.close();

    sprintf(file, "%s\\data\\RCTaxi\\dialog.txt", RootDir);
    TaxiDialogs.empty();
    // TODO: refactoring
    HANDLE ff;
    // WIN32_FIND_DATA fd;
    ff = FindFirstFile(file, &fd);
    if (ff == INVALID_HANDLE_VALUE)
    {
    	CCText("  ^7RCTaxi     ^1ERROR: ^8file " + (string)file + " not found");
        return;
    }
    FindClose(ff);

    ifstream read (file, ios::in);

    while (read.good())
    {
        char key[128];
        char message[128];
        read.getline(key, 128);

        if (strncmp(key, "#", 1) == 0 )
        {
            int i = 0;
            read.getline(message, 128);
            while ( strncmp(message, "=", 1) != 0 )
            {
                TaxiDialogs[key + 1][i++] = message;
                read.getline(message, 128);
            }
        }
    }
    read.close();

    /**клиенты-маршалы**/
    sprintf(file, "%s\\data\\RCTaxi\\tracks\\%sclient.txt", RootDir, Track);
    // TODO: refactoring
    HANDLE tt;
    tt = FindFirstFile(file, &fd);
    if (tt == INVALID_HANDLE_VALUE)
    {
    	CCText("  ^7RCTaxi     ^1ERROR: ^8file " + (string)file + " not found");
        return;
    }
    FindClose(tt);
    ifstream readt (file, ios::in);

    int i = 0;
    while (readt.good())
    {
        char str[128];
        readt.getline(str, 128);

        int c = atoi(strtok (str, ", "));
        if (c!=0)
        {
            ClientPoints[i].X = c;
            ClientPoints[i].Y = atoi(strtok (NULL, ", "));
            ClientPoints[i].Z = atoi(strtok (NULL, ", "));
            ClientPoints[i].Dir = atoi(strtok (NULL, ", "));
            ClientPoints[i].StreetId = atoi(strtok (NULL, ", "));
            i++;
        }
        ClientCount=i;
    }
    readt.close();

    //очистка маршалов
    for (int i=0; i<ClientCount; i++)
    {
        ObjectInfo obj;
        memset(&obj, 0, sizeof(struct ObjectInfo));

		obj.Index = 255;
		obj.X = ClientPoints[i].X / 4096;
		obj.Y = ClientPoints[i].Y / 4096;
		obj.Zchar = ClientPoints[i].Z;

		delObjects.push( obj );
    }

    DelObjects();

    CCText("  ^7RCTaxi\t^2OK");
}

void RCTaxi::Event()
{
	for ( auto& play: players )
    {
        byte UCID = play.first;

        if (players[UCID].client_type == 2 and players[UCID].WorkAccept == 2)
		{
			if (players[UCID].PassStress <= 800)
			{
				char str[10];
				int time2 = players[UCID].WorkTime - time(NULL);

				if (time2==0 and players[UCID].cf != 3)
				{
					SendMTC(UCID, TaxiDialogs["endtimer"][rand()%TaxiDialogs["endtimer"].size()].c_str());
					players[UCID].PassStress += 100;
					players[UCID].cf = 3;
				}

                if (time2>=0)
                {
                    sprintf(str, "^1%02d:%02d", time2 / 60, time2%60);
                    SendButton(255, UCID, 212, 130, 1, 10, 8, 32, str);
                }
			}
		}
    }
}

void RCTaxi::accept_user( byte UCID )
{
    if ( players[UCID].Work == 1 and players[UCID].WorkNow == 1 and players[UCID].CanWork and
            ( players[UCID].WorkAccept == 0 or (players[UCID].WorkAccept == 2 and players[UCID].client_type == 3)))
    {
        if (players[UCID].AcceptTime >= time(&acctime))
			return;

        // тип клиента:
        // чем больше число, тем менше веро€тность его по€влени€
        // 1 - обычный, не торопитс€, боитс€ всего и вс€
        // 2 - торопитс€, таймер на подбор и на довоз, не боитс€ превышений скорости
        // 3 - просит подобрать еще одного друга (на подборе ищем новую точку назначени€ на подбор)
        // 4 - говорит что ошибс€, просит отвезти в другое место (на высадке ищем новую точку назначени€ на высадку)
        // 5 - везет хрупкий груз, пугаетс€ в 2 раа сильнее, большой начальный стресс

        int count = 5;

        char str[69];
        int DestPoint = 0;
        srand(time(NULL));

        bool ok = true;

		while (ok)
		{
			DestPoint = rand()%ClientCount;
			if (ClientPoints[DestPoint].StreetId != street->CurentStreetNum(UCID))
				ok = false;

			Sleep(100);
		}

        if (players[UCID].client_type == 0)
            players[UCID].client_type = count - (int)sqrt(rand()%(count * count));
        else
            players[UCID].client_type = 1;

        players[UCID].WorkPointDestinaion = DestPoint;
        players[UCID].WorkStreetDestinaion = ClientPoints[DestPoint].StreetId; //улица назначени€

        struct streets StreetInfo;

        //рисую маршала
        players[UCID].HandUp = false;

        ObjectInfo *obj = new ObjectInfo;

		obj->Index = 255;
		obj->Heading = ClientPoints[DestPoint].Dir;
		obj->X = ClientPoints[DestPoint].X / 4096;
		obj->Y = ClientPoints[DestPoint].Y / 4096;
		obj->Zchar = ClientPoints[DestPoint].Z;
		obj->Flags = 133;

		AddObject(obj);
		delete obj;

        memset(&StreetInfo, 0, sizeof(streets));
        street->CurentStreetInfoByNum(&StreetInfo, players[UCID].WorkStreetDestinaion);

        sprintf(str, msg->_(UCID, "TaxiAccept11"), street->GetStreetName(UCID, StreetInfo.StreetID));
        SendMTC(UCID, str);

        sprintf(str, msg->_(UCID, "TaxiAccept1"), street->GetStreetName(UCID, StreetInfo.StreetID));
        ButtonInfo(UCID, str);

		players[UCID].WorkAccept = 1;
    }
}

void RCTaxi::accept_user2(byte UCID)
{
    srand(time(NULL));
    lgh->OnRedFalse(UCID);

    int DestPoint = 0;
    bool ok = true;
    while (ok)
    {
        DestPoint = rand()%ClientCount;
        int X1 = ClientPoints[DestPoint].X / 65536,
            Y1 = ClientPoints[DestPoint].Y / 65536,
            X2 = players[UCID].Info.X / 65536,
            Y2 = players[UCID].Info.Y / 65536;

		int MINDIST = 300;

        if (ClientPoints[DestPoint].StreetId != street->CurentStreetNum( UCID ) and Distance(X1, Y1, X2, Y2)>MINDIST)
            ok = false;

        Sleep(100);
    }

    int DestStreet = ClientPoints[DestPoint].StreetId; //улица назначени€

    struct streets StreetInfo;
    memset(&StreetInfo, 0, sizeof(streets));


    //удал€ю маршала
    delete_marshal( UCID );

    players[UCID].WorkPointDestinaion = DestPoint;
    players[UCID].WorkStreetDestinaion = DestStreet;
    memset(&StreetInfo, 0, sizeof(streets));
    street->CurentStreetInfoByNum(&StreetInfo, DestStreet);

    players[UCID].WorkAccept = 2;

    if (players[UCID].cf == 1 or players[UCID].cf == 4)
		players[UCID].PassStress -= 100;
	else if (players[UCID].client_type == 2)
        players[UCID].PassStress = rand()%300;
    else
		players[UCID].PassStress = rand()%500;

    char Msg[96];
    char Btn[96];

    sprintf(Btn, msg->_(UCID,"TaxiAccept2"), street->GetStreetName(UCID, StreetInfo.StreetID));

    if (players[UCID].client_type == 1)
        sprintf(Msg, TaxiDialogs["client_1"][rand()%TaxiDialogs["client_1"].size()].c_str(), street->GetStreetName(UCID, StreetInfo.StreetID));

    if (players[UCID].client_type == 2)
	{
		sprintf(Msg, TaxiDialogs["client_2"][rand()%TaxiDialogs["client_2"].size()].c_str(), street->GetStreetName(UCID, StreetInfo.StreetID));
		players[UCID].WorkTime = time(NULL) + 70 + (int)rand()%40;
	}

    if (players[UCID].client_type == 3)
    {
        sprintf(Msg, TaxiDialogs["client_3"][rand()%TaxiDialogs["client_3"].size()].c_str());

        SendMTC(UCID, Msg);
        ButtonInfo(UCID, Btn);
        players[UCID].cf = 1;
        accept_user(UCID);
        return;
    }

    if (players[UCID].client_type == 4)
        sprintf(Msg, TaxiDialogs["client_1"][rand()%TaxiDialogs["client_1"].size()].c_str(), street->GetStreetName(UCID, StreetInfo.StreetID));

	if (players[UCID].client_type == 5)
	{
		players[UCID].cf = 2;
        sprintf(Msg, TaxiDialogs["client_5"][rand()%TaxiDialogs["client_5"].size()].c_str(), street->GetStreetName(UCID, StreetInfo.StreetID));
        players[UCID].PassStress = 500;
	}

	if (players[UCID].cf == 1 and players[UCID].client_type == 1)
    {
        sprintf(Msg, TaxiDialogs["client_31"][rand()%TaxiDialogs["client_31"].size()].c_str(), street->GetStreetName(UCID, StreetInfo.StreetID));
        players[UCID].cf = 1;
    }

    SendMTC(UCID, Msg);
    ButtonInfo(UCID, Btn);
}

void RCTaxi::InsimCNL( struct IS_CNL* packet )
{
    //удал€ю маршалов
    delete_marshal( packet->UCID );
    save_user( packet->UCID );
    players.erase( packet->UCID );
    NumP --;
}

void RCTaxi::InsimCPR( struct IS_CPR* packet )
{
    strcpy( players[packet->UCID].PName, packet->PName);
}

void RCTaxi::InsimMCI ( struct IS_MCI* pack_mci )
{
    float Dist;

    for (int i = 0; i < pack_mci->NumC; i++) // прогон по всему массиву pack_mci->Info[i]
    {
        byte UCID = PLIDtoUCID[ pack_mci->Info[i].PLID ];

        if (players[UCID].WorkNow != 0 and police->IsCop(UCID) and players[UCID].CanWork)
        {
            SendMTC(UCID, msg->_(UCID, "1303" ));
            players[UCID].CanWork=false;
            delete_marshal(UCID);
            players[UCID].WorkAccept = 0;
            players[UCID].WorkPointDestinaion = 0;
            players[UCID].WorkStreetDestinaion = 0;
            players[UCID].StressOverCount = 0;
            players[UCID].PassStress = 0;
        }

        int X = pack_mci->Info[i].X / 65536;
        int Y = pack_mci->Info[i].Y / 65536;
        int Speed = ((int)pack_mci->Info[i].Speed * 360) / (32768);

        if (Check_Pos(4, zone.dealX, zone.dealY, X, Y))
        {
            if (players[UCID].InZone == 0)
            {
                players[UCID].InZone = 1;
                SendMTC(UCID, msg->_(UCID, "TaxiDialog1"));
                SendMTC(UCID, msg->_(UCID, "TaxiDialog2"));
                SendMTC(UCID, msg->_(UCID, "TaxiDialog3"));
                SendMTC(UCID, msg->_(UCID, "TaxiDialog4"));
                SendMTC(UCID, msg->_(UCID, "TaxiDialog5"));
            }
        }
        else if (players[UCID].InZone == 1)
            players[UCID].InZone = 0;

        /** player drive on dest street **/
        if (players[UCID].WorkNow == 1 and players[UCID].WorkAccept != 0)
        {
            if (players[UCID].WorkAccept == 2 or players[UCID].cf)
            {

                struct streets StreetInfo;
                memset(&StreetInfo, 0, sizeof(streets));
                street->CurentStreetInfoByNum(&StreetInfo, street->CurentStreetNum( UCID ));
                if (players[UCID].client_type != 2 and Speed>(StreetInfo.SpeedLimit + 10) and players[UCID].PassStress <= 800)
                {
                    if (players[UCID].spd==5)
                        players[UCID].spd=0;

                    if (players[UCID].spd==0)
                        SendMTC(UCID,  TaxiDialogs["speed"][ rand()%TaxiDialogs["speed"].size() ].c_str() ); // превышаешь скорость

                    players[UCID].spd++;
                    players[UCID].PassStress += Speed-StreetInfo.SpeedLimit + 10;
                }

                if (players[UCID].client_type != 2 and lgh->CheckOnRed(UCID))
                {
                    SendMTC(UCID,  TaxiDialogs["redlight"][ rand()%TaxiDialogs["redlight"].size() ].c_str() ); // красный светофор
                    players[UCID].PassStress += 200;
                    lgh->OnRedFalse(UCID);
                }
            }

            if (players[UCID].WorkStreetDestinaion == street->CurentStreetNum( UCID ))
            {
                int des_X = ClientPoints[players[UCID].WorkPointDestinaion].X / 65536;
                int des_Y = ClientPoints[players[UCID].WorkPointDestinaion].Y / 65536;
                /** вычисл€ем расто€ние до точки остановки **/
                Dist = Distance(X , Y , des_X , des_Y);

                if (players[UCID].OnStreet == false and players[UCID].PassStress <= 800)
                {
                    players[UCID].OnStreet = true;
                    char MSG[128];
                    sprintf(MSG, msg->_(UCID, "TaxiOnStreet"), (Dist-(int)Dist%10));
                    SendMTC(UCID, MSG);
                }

                if (Dist <= 30)
                {
                    //маршал поднимает руку
                    if (!players[UCID].HandUp and players[UCID].WorkAccept == 1)
                    {
                        //направление на авто
                        float xx = (ClientPoints[players[UCID].WorkPointDestinaion].X - pack_mci->Info[i].X) / 65536;
                        float yy = (ClientPoints[players[UCID].WorkPointDestinaion].Y - pack_mci->Info[i].Y) / 65536;
                        float gip = sqrt(xx * xx + yy * yy);
                        float ddd = acos(xx / gip) * (180 / 3.14);

                        if (yy>0)
                            ddd=360-ddd;

                        ddd=ddd + 90;

                        if (ddd>360)
                            ddd=ddd-360;

                        ddd=(360-ddd) / 360 * 256; //угол поворота дл€ маршала

                        players[UCID].HandUp = true;

                        ObjectInfo *obj = new ObjectInfo;
                        memset(obj, 0, sizeof( ObjectInfo ) );
						obj->Index = 255;
						obj->Heading = (byte)ddd;
						obj->X = ClientPoints[players[UCID].WorkPointDestinaion].X / 4096;
						obj->Y = ClientPoints[players[UCID].WorkPointDestinaion].Y / 4096;
						obj->Zchar = ClientPoints[players[UCID].WorkPointDestinaion].Z;
						obj->Flags = 133;

						DelObject(obj);

                        obj->Flags = 135;

						AddObject(obj);
						delete obj;
                    }

                    if (players[UCID].WorkAccept == 2 and players[UCID].PassStress <= 800)
                    {
                        if (players[UCID].client_type == 4)
                        {
                            SendMTC(UCID, TaxiDialogs["client_4"][rand()%TaxiDialogs["client_4"].size()].c_str());
                            players[UCID].client_type = 1;
                            players[UCID].cf = 4;
                            accept_user2(UCID);
                            return;
                        }

                        if (players[UCID].InPasZone != 1)
                        {
                            players[UCID].InPasZone = 1;
                            srand ( time(NULL) );
                            SendMTC(UCID, TaxiDialogs["dist"][ rand()%TaxiDialogs["dist"].size() ].c_str() ); // приехали
                        }
                    }

                    if (Speed < 5 and players[UCID].WorkAccept == 2)
                        taxi_done( UCID ); //высадили
                }
                else
                {
                    //маршал опускает руку
                    if (players[UCID].HandUp and players[UCID].WorkAccept == 1)
                    {
                        players[UCID].HandUp = false;

                        ObjectInfo *obj = new ObjectInfo;
                        memset(obj, 0, sizeof( ObjectInfo ) );
						obj->Index = 255;
						obj->Heading = ClientPoints[players[UCID].WorkPointDestinaion].Dir;
						obj->X = ClientPoints[players[UCID].WorkPointDestinaion].X / 4096;
						obj->Y = ClientPoints[players[UCID].WorkPointDestinaion].Y / 4096;
						obj->Zchar = ClientPoints[players[UCID].WorkPointDestinaion].Z;
						obj->Flags = 135;

						DelObject(obj);

                        obj->Flags = 133;

						AddObject(obj);
						delete obj;
                    }

                    if ((players[UCID].WorkAccept == 2 or players[UCID].cf) and players[UCID].PassStress <= 800)
                    {
                        if (players[UCID].InPasZone == 1)
                        {
                            players[UCID].InPasZone = 0;
                            players[UCID].PassStress += 10;
                            srand ( time(NULL) );
                            SendMTC(UCID, TaxiDialogs["past"][ rand()%TaxiDialogs["past"].size() ].c_str() ); // проехал мимо
                        }
                    }
                }

                if (Dist < 5 and Speed < 5 and players[UCID].WorkAccept == 1)
                    accept_user2( UCID ); // посадили
            }
            else
            {
                if (players[UCID].OnStreet = true)
                    SendBFN(UCID, 206);

                players[UCID].OnStreet = false;
            }

            if (players[UCID].WorkAccept == 1)
            {
                struct streets StreetInfo;
                memset(&StreetInfo, 0, sizeof(streets));
                street->CurentStreetInfoByNum(&StreetInfo, players[UCID].WorkStreetDestinaion);

                char Msg[128];
                sprintf(Msg, msg->_(UCID, "TaxiAccept1"), street->GetStreetName(UCID, StreetInfo.StreetID));
                ButtonInfo(UCID, Msg);
            }

            if (players[UCID].WorkAccept == 2 or players[UCID].cf)
            {
                struct streets StreetInfo;
                memset(&StreetInfo, 0, sizeof(streets));
                street->CurentStreetInfoByNum(&StreetInfo, players[UCID].WorkStreetDestinaion);

                char Msg[128];
                sprintf(Msg, msg->_(UCID, "TaxiAccept2"), street->GetStreetName(UCID, StreetInfo.StreetID));

                if (players[UCID].PassStress <= 800)
                    ButtonInfo(UCID, Msg);

                int X = pack_mci->Info[i].X / 65536;
                int Y = pack_mci->Info[i].Y / 65536;
                int Z = pack_mci->Info[i].Z / 65536;
                int D = pack_mci->Info[i].Direction / 182;
                int H = pack_mci->Info[i].Heading / 182;

                int S = ((int)pack_mci->Info[i].Speed * 360) / (32768);

                int A = pack_mci->Info[i].AngVel * 360 / 16384;

                int X1 = players[UCID].Info.X / 65536;
                int Y1 = players[UCID].Info.Y / 65536;
                int Z1 = players[UCID].Info.Z / 65536;
                int D1 = players[UCID].Info.Direction / 182;
                int H1 = players[UCID].Info.Heading / 182;
                int S1 = ((int)players[UCID].Info.Speed * 360) / (32768);
                int A1 = players[UCID].Info.AngVel * 360 / 16384;

                long dA = A-A1;
                long dS = S-S1;
                long dD = abs((int)(sin(D) * 100))-abs((int)(sin(D1) * 100));
                long dH = abs((int)(sin(H) * 100))-abs((int)(sin(H1) * 100));

                if (Speed<60 and abs(A)<40)
                    dA=0;

                int K = (int)sqrt(abs((dD-dH) * (1 + dA) * dS)) / 8;

				int coef = 1;
				if (players[UCID].client_type == 5)
					coef = 5;

                players[UCID].PassStress += K * coef;

                if (players[UCID].PassStress > 1000)
                    players[UCID].PassStress = 1000;

                if (X1==0 and Y1==0 and Z1==0)
                {
                    X1=X;
                    Y1=Y;
                    Z1=Z;
                }

                btn_stress( UCID );
                char d[128];
                sprintf(d, "^7%0.0f ^Cм ", (Dist-(int)Dist%5));

                if (players[UCID].OnStreet and players[UCID].PassStress <= 800)
                    SendButton(255, UCID, 206, 49, 125, 8, 4, 32 + 128, d);

                if (players[UCID].PassStress > 800)
                {
                    SendBFN(UCID, 206);
                    if (players[UCID].StressOverCount == 0)
                    {
                        srand ( time(NULL) );
                        SendMTC(UCID,  TaxiDialogs["needstop"][ rand()%TaxiDialogs["needstop"].size() ].c_str() ); // пугаецца, требует остановить
                        ClearButtonInfo( UCID );
                    }

                    players[UCID].StressOverCount ++;

                    if (players[UCID].StressOverCount >= 20)
                        players[UCID].StressOverCount = 0;

                    if (Speed < 5)
                        taxi_done( UCID );
                }
            }
        }

        memcpy( &players[UCID].Info , &pack_mci->Info[i] , sizeof( CompCar ) );

        /** thread xD **/
        accept_user( UCID );
        /** thread **/
    }
}

void RCTaxi::dead_pass(byte UCID)
{
    if (players[UCID].WorkAccept != 0
            and abs(ClientPoints[players[UCID].WorkPointDestinaion].X / 65536 - players[UCID].Info.X / 65536) < 5 )
    {
        //удал€ем маршала
        delete_marshal( UCID );
        players[UCID].WorkAccept = 0;
        players[UCID].WorkPointDestinaion = 0;
        players[UCID].WorkStreetDestinaion = 0;
        players[UCID].StressOverCount = 0;
        players[UCID].PassStress = 0;
        players[UCID].Work = 0;
        players[UCID].WorkNow = 0;
        SendMTC(UCID, msg->_(UCID, "TaxiDead"));
    }
}

void RCTaxi::InsimMSO( struct IS_MSO* packet )
{
    if (packet->UCID == 0)
    {
        return;
    }

    byte UCID = packet->UCID;

    char Message[128];
    strcpy(Message, packet->Msg + ((unsigned char)packet->TextStart));

    if (strncmp(Message, "!save", strlen("!save")) == 0)
    {
        save_user( packet->UCID );
    }

    if (players[packet->UCID].InZone == 1)
    {
        if (strncmp(Message, "!deal", strlen("!deal")) == 0 )
        {
            if (dl->GetLVL( packet->UCID ) < 20)
            {
                SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog6"));
                return;
            }
            /** DO SOME CODE **/
            if (players[packet->UCID].Work !=0)
            {
                SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog7"));
                return;
            }

            SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog8"));
            players[packet->UCID].Work = 1;
        }

        if (strncmp(Message, "!undeal", strlen("!undeal")) == 0 )
        {
            if (players[packet->UCID].Work ==0)
            {
                SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog9"));
                return;
            }
            SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog10"));
            //удал€ю маршалов
            delete_marshal( packet->UCID );

            players[packet->UCID].WorkAccept = 0;
            players[packet->UCID].WorkPointDestinaion = 0;
            players[packet->UCID].WorkStreetDestinaion = 0;
            players[packet->UCID].StressOverCount = 0;
            players[packet->UCID].PassStress = 0;
            players[packet->UCID].WorkNow = 0;
            players[packet->UCID].Work = 0;
        }

        if (strncmp(Message, "!workstart", strlen("!workstart")) == 0 )
        {
            if (players[packet->UCID].Work ==0)
            {
                SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog11"));
                return;
            }

            if (police->IsCop(packet->UCID))
            {
                SendMTC(packet->UCID, msg->_(packet->UCID, "1303" ));
                return;
            }

            if (players[packet->UCID].WorkNow ==1)
            {
                SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog12"));
                return;
            }

            if (!players[packet->UCID].CanWork)
            {
                SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog13"));
                return;
            }
            players[packet->UCID].AcceptTime = time(NULL) + PASSANGER_INTERVAL / (NumP + 1);
            players[packet->UCID].WorkNow = 1;
            SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog14"));
        }

        if (strncmp(Message, "!workend", strlen("!workend")) == 0 )
        {
            if (players[packet->UCID].Work ==0)
            {
                SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog11"));
                return;
            }

            if (players[packet->UCID].WorkNow ==0)
            {
                SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog15"));
                return;
            }

            SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog16"));

            //удал€ю маршалов
            delete_marshal( packet->UCID );

            players[packet->UCID].WorkNow = 0;
            players[packet->UCID].WorkAccept = 0;
            players[packet->UCID].WorkPointDestinaion = 0;
            players[packet->UCID].WorkStreetDestinaion = 0;
            players[packet->UCID].StressOverCount = 0;
            players[packet->UCID].PassStress = 0;
        }
    }

    if (strncmp(Message, "!tstat", strlen("!tstat")) == 0 and (strcmp(players[UCID].UName, "denis-takumi") == 0 or strcmp(players[UCID].UName, "Lexanom") == 0))
    {
        strtok(Message, " ");
        char * uname = strtok(NULL, " ");

        if (!uname)
        {
            SendMTC(packet->UCID, msg->_(packet->UCID, "TaxiDialog17"));
            return;
        }

        for ( auto& p: players )
			if ( strcmp(uname, players[p.first].UName) == 0 )
			{
				char str[96];

				sprintf(str,"^6| ^C^7—татистика работы в такси ^8%s ^7(^9%s^7)", players[p.first].PName, players[p.first].UName);
				SendMTC(packet->UCID, str);

				if (players[p.first].Work == 0)
				{
					sprintf(str,"^6| ^C^7Ќе работает");
					SendMTC(packet->UCID, str);
				}
				else
                {
                    if (players[p.first].WorkNow == 0)
                    {
                        sprintf(str,"^6| ^C^7Ќе на вахте");
                        SendMTC(packet->UCID, str);
                    }
                    else
                    {
                        sprintf(str,"^6| ^C^7“екуща€ зан€тость: %d", players[p.first].WorkAccept);
                        SendMTC(packet->UCID, str);
                    }
                }

				sprintf(str,"^6| ^C^7¬сего отвез пассажиров: %d", players[p.first].PassCount);
				SendMTC(packet->UCID, str);
				return;
			}
		SendMTC(packet->UCID, "^6| ^C^7»грок не найден");
    }

    if (strncmp(Message, "!points", strlen("!points")) == 0 and (strcmp(players[UCID].UName, "denis-takumi") == 0 or strcmp(players[UCID].UName, "Lexanom") == 0))
    {
        if (StartPointsAdd == 0)
        {
            StartPointsAdd = 1;
            SendMTC(packet->UCID, "^3points clients ^2ON");
        }
        else if (StartPointsAdd == 1)
        {
            StartPointsAdd = 2;
            SendMTC(packet->UCID, "^3points points ^2ON");
        }
        else if (StartPointsAdd == 2)
        {
            StartPointsAdd = 0;
            SendMTC(packet->UCID, "^3points ^1OFF");
        }
    }
}

void RCTaxi::InsimNCN( struct IS_NCN* packet )
{
    if (packet->UCID == 0)
    {
        return;
    }

    strcpy(players[packet->UCID].UName, packet->UName);
    strcpy(players[packet->UCID].PName, packet->PName);

    read_user( packet->UCID );
    NumP ++;
}

void RCTaxi::InsimNPL( struct IS_NPL* packet )
{
    PLIDtoUCID[ packet->PLID ] = packet->UCID;
    strcpy(players[packet->UCID].CName , packet->CName);

    if (strcmp(players[packet->UCID].CName, "MRT") == 0 or
            strcmp(players[packet->UCID].CName, "UFR") == 0 or
            strcmp(players[packet->UCID].CName, "XFR") == 0 or
            //strcmp(players[packet->UCID].CName, "FXR") == 0 or
            strcmp(players[packet->UCID].CName, "XRR") == 0 or
            strcmp(players[packet->UCID].CName, "FZR") == 0 or
            strcmp(players[packet->UCID].CName, "FBM") == 0 or
            strcmp(players[packet->UCID].CName, "FOX") == 0 or
            strcmp(players[packet->UCID].CName, "FO8") == 0 or
            strcmp(players[packet->UCID].CName, "BF1") == 0)
        players[packet->UCID].CanWork=false;
    else
        players[packet->UCID].CanWork=true;

    if (players[packet->UCID].WorkNow != 0 and !players[packet->UCID].CanWork)
    {
        SendMTC(packet->UCID, msg->_(packet->UCID, "TaxiDialog13"));
        delete_marshal(packet->UCID);
        players[packet->UCID].WorkAccept = 0;
        players[packet->UCID].WorkPointDestinaion = 0;
        players[packet->UCID].WorkStreetDestinaion = 0;
        players[packet->UCID].StressOverCount = 0;
        players[packet->UCID].PassStress = 0;
    }
}

void RCTaxi::taxi_loss(byte UCID)
{
    if (players[UCID].WorkAccept == 2 or players[UCID].cf)
    {
    	SendMTC(UCID,msg->_(UCID,"TaxiPll"));
        SendBFN(UCID, 206);
        SendBFN(UCID, 207);
        SendBFN(UCID, 212);
        ClearButtonInfo(UCID);
        players[UCID].cf = 0;
        players[UCID].client_type = 0;
        players[UCID].AcceptTime = time(NULL) + rand()%PASSANGER_INTERVAL / (NumP + 1);
        players[UCID].WorkAccept = 0;
        players[UCID].WorkPointDestinaion = 0;
        players[UCID].WorkStreetDestinaion = 0;
        players[UCID].StressOverCount = 0;
        players[UCID].PassStress = 0;
        players[UCID].InPasZone = 0;
    }
}

void RCTaxi::InsimPLP( struct IS_PLP* packet)
{
    byte UCID = PLIDtoUCID[packet->PLID];
    taxi_loss(UCID);
}

void RCTaxi::InsimPLL( struct IS_PLL* packet )
{

    byte UCID = PLIDtoUCID[packet->PLID];
    taxi_loss(UCID);

    PLIDtoUCID.erase( packet->PLID );
}

void RCTaxi::read_user( byte UCID )
{
    char file[MAX_PATH];
    sprintf(file, "%s\\data\\RCTaxi\\users\\%s.txt", RootDir, players[UCID].UName);
    // TODO: refactoring
    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file, &fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        players[UCID].Work= 0;
        players[UCID].FiredPenalty= 0;
        players[UCID].PenaltyCount = 0;
        players[UCID].PassCount = 0;
        save_user( UCID );
    }
    else
    {
        ifstream readf (file, ios::in);

        while (readf.good())
        {
            char str[128];
            readf.getline(str, 128);
            if (strlen(str) > 0)
            {
                if (strncmp("Work=", str, strlen("Work="))==0)
                    players[UCID].Work = atoi(str + strlen("Work="));

                if (strncmp("FiredPenalty=", str, strlen("FiredPenalty="))==0)
                    players[UCID].FiredPenalty= atoi(str + strlen("FiredPenalty="));

                if (strncmp("PenaltyCount=", str, strlen("PenaltyCount="))==0)
                    players[UCID].PenaltyCount = atoi(str + strlen("PenaltyCount="));

                if (strncmp("PassCount=", str, strlen("PassCount="))==0)
                    players[UCID].PassCount = atoi(str + strlen("PassCount="));
            }
        }
        readf.close();
    }
    FindClose(fff);
}

void RCTaxi::delete_marshal(byte UCID)
{
    ObjectInfo *obj = new ObjectInfo;
	memset(obj, 0, sizeof( ObjectInfo ) );

	obj->Index = 255;
	obj->Heading = ClientPoints[players[UCID].WorkPointDestinaion].Dir;
	obj->X = ClientPoints[players[UCID].WorkPointDestinaion].X / 4096;
	obj->Y = ClientPoints[players[UCID].WorkPointDestinaion].Y / 4096;
	obj->Zchar = ClientPoints[players[UCID].WorkPointDestinaion].Z;
	obj->Flags = 133;

	DelObject(obj);

	obj->Flags = 135;

	DelObject(obj);
	delete obj;
}

void RCTaxi::save_user( byte UCID )
{
    char file[MAX_PATH];
    sprintf(file, "%s\\data\\RCTaxi\\users\\%s.txt", RootDir, players[UCID].UName);
    ofstream writef (file, ios::out);
    writef << "Work=" << players[UCID].Work << endl;
    writef << "FiredPenalty=" << players[UCID].FiredPenalty << endl;
    writef << "PenaltyCount=" << players[UCID].PenaltyCount << endl;
    writef << "PassCount=" << players[UCID].PassCount << endl;
    writef.close();
}

void RCTaxi::taxi_done( byte UCID )
{
    SendBFN(UCID, 206);
    SendBFN(UCID, 207);
    SendBFN(UCID, 212);
    if (players[UCID].PassStress <= 800)
    {
        players[UCID].PassCount++;
        srand(time(NULL));
        SendMTC(UCID, TaxiDialogs["done"][ rand()%TaxiDialogs["done"].size() ].c_str() ); // send random dialog phrase

        float coef = 1;
        if (players[UCID].cf == 1 or players[UCID].cf == 4)
            coef = 1.5;
		if (players[UCID].cf == 2)
            coef = 2;
        if (players[UCID].cf == 3)
            coef = 0.5;

        bank->AddCash(UCID, (1000*coef - players[UCID].PassStress / 2), true);
        dl->AddSkill(UCID);
    }
    else
    {
        srand (time(NULL));
        SendMTC(UCID, TaxiDialogs["exit"][ rand()%TaxiDialogs["exit"].size() ].c_str() ); // send random dialog phrase
    }

	ClearButtonInfo(UCID);
    players[UCID].cf = 0;
    players[UCID].client_type = 0;
    players[UCID].AcceptTime = time(NULL) + rand()%PASSANGER_INTERVAL / (NumP + 1);
    players[UCID].WorkAccept = 0;
    players[UCID].WorkPointDestinaion = 0;
    players[UCID].WorkStreetDestinaion = 0;
    players[UCID].StressOverCount = 0;
    players[UCID].PassStress = 0;
    players[UCID].InPasZone = 0;
}

void RCTaxi::InsimCON( struct IS_CON* packet )
{

    byte UCIDA = PLIDtoUCID[ packet->A.PLID ];
    byte UCIDB = PLIDtoUCID[ packet->B.PLID ];

    time_t now = time(NULL);

    int coef = 10;
    if (players[UCIDA].client_type == 5 or players[UCIDB].client_type == 5)
        coef = 30;

    if ((players[ UCIDA ].WorkAccept == 2 or players[UCIDA].cf) and (now - players[UCIDA].LastT) > 1)
    {
        players[UCIDA].LastT = now;
        players[ UCIDA ].PassStress += coef * packet->SpClose;

        srand ( time(NULL) );
        SendMTC( UCIDA, TaxiDialogs["con"][ rand()%TaxiDialogs["con"].size() ].c_str() ); // send random dialog phrase
    }

    if ((players[ UCIDB ].WorkAccept == 2 or players[UCIDB].cf) and (now - players[UCIDB].LastT) > 1)
    {
        players[UCIDB].LastT = now;
        players[ UCIDB ].PassStress += coef * packet->SpClose;

        srand ( time(NULL) );
        SendMTC( UCIDB ,  TaxiDialogs["con"][ rand()%TaxiDialogs["con"].size() ].c_str() ); // send random dialog phrase
    }
}

void RCTaxi::InsimOBH( struct IS_OBH* packet )
{
    byte UCID = PLIDtoUCID[ packet->PLID ];

    time_t now = time(NULL);

    if ((now - players[UCID].LastT) <= 1)
    {
        return;
    }

    players[UCID].LastT = now;

    if (players[UCID].WorkAccept == 2 or players[UCID].cf)
    {
        int coef = 1;
        if (players[UCID].client_type == 5)
            coef = 5;

        if ((packet->Index==128 or packet->Index==129) and packet->SpClose>50)
        {
            players[UCID].PassStress += packet->SpClose + 20;
            srand(time(NULL));
            SendMTC(UCID, TaxiDialogs["speedhump"][ rand()%TaxiDialogs["speedhump"].size() ].c_str() ); // send random dialog phrase
        }
        else if ((packet->Index > 45 and packet->Index < 125 and packet->Index!=120 and packet->Index!=121) or (packet->Index > 140))
        {
            players[UCID].PassStress +=  packet->SpClose*coef;
            srand(time(NULL));
            SendMTC(UCID, TaxiDialogs["obh"][ rand()%TaxiDialogs["obh"].size() ].c_str() ); // send random dialog phrase
        }
        else
        {
            players[UCID].PassStress +=  packet->SpClose / 10;
        }
    }
}

void RCTaxi::InsimAXM( struct IS_AXM* packet )
{

	if( packet->UCID == 0)
	{
		AddObjects();
		DelObjects();
	}

    if (StartPointsAdd == 0 or packet->UCID == 0 or packet->PMOAction != PMO_ADD_OBJECTS)
    {
        return;
    }

    char text[96];

    int X = packet->Info[0].X * 4096 / 65536;
    int Y = packet->Info[0].Y * 4096 / 65536;
    int StreetID = -1;

    for (int g = 0; g < street->StreetNums; g++)
    {
        if ( Check_Pos(street->Street[g].PointCount, street->Street[g].StreetX, street->Street[g].StreetY, X, Y) )
        {
            StreetID = g;
            break;
        }
    }

    if (StreetID != -1)
    {
        if (StartPointsAdd == 1)
        {
            sprintf(text, "%d, %d, %d, %d, %d", packet->Info[0].X * 4096, packet->Info[0].Y * 4096, packet->Info[0].Zchar, packet->Info[0].Heading, StreetID);
            SendMTC(255, text);

            ofstream readf ("Clients.ini", ios::app);
            readf << text << endl;
            readf.close();
        }
        else
        {
            sprintf(text, "%d, %d, %d", packet->Info[0].X, packet->Info[0].Y, StreetID);
            SendMTC(255, text);

            ofstream readf ("Points.ini", ios::app);
            readf << text << endl;
            readf.close();
        }
    }
}

void RCTaxi::InsimHLV( struct IS_HLV* packet )
{
    byte UCID = PLIDtoUCID[ packet->PLID ];

    /** столкновение со стеной **/
    if (packet->HLVC==1)
    {
        time_t now = time(NULL);

        if ((now - players[UCID].LastT) < 1)
            return;

        players[UCID].LastT = now;

        int coef = 10;
        if (players[UCID].client_type == 5)
            coef = 30;

        if (players[UCID].WorkAccept == 2 or players[UCID].cf)
        {
            players[UCID].PassStress +=  packet->C.Speed * coef;
            srand(time(NULL));
            SendMTC(UCID, TaxiDialogs["obh"][ rand()%TaxiDialogs["obh"].size() ].c_str() ); // send random dialog phrase
        }
    }
}

void RCTaxi::btn_stress( byte UCID )
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID =  UCID ;
    pack.Inst = 0;
    pack.TypeIn = 0;

    pack.ClickID = 207;
    pack.BStyle = 32 + 64;
    pack.L = 1;
    pack.T = 125;
    pack.W = 48;
    pack.H = 4;
    strcpy(pack.Text, "");

    for (int i=1; i<=players[UCID].PassStress / 10; i++) // 1000 / 10 = 100
    {
        if (i == 1)
            strcat(pack.Text, "^2");

        if (i == 40)
            strcat(pack.Text, "^3");

        if (i == 80)
            strcat(pack.Text, "^1");

        strcat(pack.Text, "||");
    }
    strcat(pack.Text, "^8");

    for (int i=0; i<(100-players[UCID].PassStress / 10); i++)
        strcat(pack.Text, "||");

    insim->send_packet(&pack);
}

bool RCTaxi::IfWork (byte UCID)
{
    if (players[UCID].Work != 0)
        return true;
    return false;
}
