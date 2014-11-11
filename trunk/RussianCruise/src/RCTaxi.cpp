using namespace std;
#include "RCTaxi.h"

RCTaxi::RCTaxi(const char* Dir)
{
    ClassName = "RCTaxi";
    strcpy(RootDir,Dir);
    memset(ClientPoints, 0, sizeof( Taxi_clients ) * MAX_POINTS );
}

RCTaxi::~RCTaxi()
{

}

int RCTaxi::init(DBMySQL *db, CInsim *InSim, void *Message, void *Bank, void *RCdl, void * STreet, void * Police, void * Light)
{
    this->db = db;
    if (!this->db)
    {
        printf("RCTaxi: Can't sctruct MySQL Connector\n");
        return -1;
    } // �������� ���� �� ���������
    insim = InSim; // ����������� ��������� ������� ������
    if (!insim)
    {
        printf ("RCTaxi: Can't struct CInsim class");    // ��������� �� �������������
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

    CCText("^3R"+ClassName+":\t^2inited");
    return 0;
}

void RCTaxi::ReadConfig(const char *Track)
{
    char file[MAX_PATH];
    sprintf(file, "%s/data/RCTaxi/tracks/%s.txt", RootDir, Track);

    ifstream readf (file, ios::in);

     if (readf.is_open() == false)
    {
        CCText("  ^7RCTaxi     ^1ERROR: ^8file " + (string)file + " not found");
        return;
    }

    while (readf.good())
    {
        char str[128];
        readf.getline(str, 128);

        if (strlen(str) > 1)
        {
            if (strstr(str, "/dealer") != NULL)
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

                    config["dealer"]["X"].append(atoi(X));
                    config["dealer"]["Y"].append(atoi(Y));
                }
            }

            if (strstr(str, "/shop") != NULL)
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

                    config["shop"]["X"].append(atoi(X));
                    config["shop"]["Y"].append(atoi(Y));
                }
            }

            if (strstr(str, "/points") != 0)
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

    sprintf(file, "%s/data/RCTaxi/dialog.txt", RootDir);
    TaxiDialogs.empty();

    ifstream read (file, ios::in);

    if (read.is_open() == false)
    {
        CCText("  ^7RCTaxi     ^1ERROR: ^8file " + (string)file + " not found");
        return;
    }

    while (read.good())
    {
        char key[128];

        read.getline(key, 128);

        if (strstr(key, "#") != NULL )
        {
            string keyword = key + 1;

            // ��� ��� ��� �� ����������� �� ����������� ��������
            if (!keyword.empty() && keyword[keyword.length()-1] == '\r') {
                keyword.erase(keyword.length()-1);
            }

            int i = 0;
            char message[128];

            read.getline(message, 128);

            while ( strstr(message, "=") == NULL )
            {
                TaxiDialogs[ keyword ][i++] = message;
                read.getline(message, 128);
                dialogs["rus"][keyword].append( message );
            }
        }
    }
    read.close();

    /**�������-�������**/
    sprintf(file, "%s/data/RCTaxi/tracks/%sclient.txt", RootDir, Track);

    ifstream readt (file, ios::in);

    if (readt.is_open() == false)
    {
        CCText("  ^7RCTaxi     ^1ERROR: ^8file " + (string)file + " not found");
        return;
    }

    int i = 0;
    while (readt.good())
    {
        char str[128];
        readt.getline(str, 128);
        char * cc = strtok (str, ", ");
        if(cc == NULL)
            continue;

        int c = atoi(cc);
        if (c!=0)
        {
            ClientPoints[i].X = c;
            ClientPoints[i].Y = atoi(strtok (NULL, ", "));
            ClientPoints[i].Z = atoi(strtok (NULL, ", "));
            ClientPoints[i].Dir = atoi(strtok (NULL, ", "));
            ClientPoints[i].StreetId = atoi(strtok (NULL, ", "));

            Json::Value client;
            client["X"] = ClientPoints[i].X;
            client["Y"] = ClientPoints[i].Y;
            client["Z"] = ClientPoints[i].Z;
            client["Dir"] = ClientPoints[i].Dir;
            client["StreetId"] = ClientPoints[i].StreetId;

            config["clients"].append(client);
            i++;
        }
        ClientCount=i;
    }
    readt.close();

    //������� ��������
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

   /* sprintf(file, "%s/data/RCTaxi/tracks/%s.json", RootDir, Track);
    ofstream f;
    f.open(file, ios::out);
    f << configWriter.write( config );
    f.close();*/

   /* sprintf(file, "%s/data/RCTaxi/dialogs.json", RootDir);

    f.open(file, ios::out);
    f << configWriter.write( dialogs );
    f.close();*/

    CCText("  ^7RCTaxi\t^2OK");
}

void RCTaxi::Event()
{
	for ( auto& play: players )
    {
        byte UCID = play.first;

        if (players[UCID].ClientType == 2 and players[UCID].WorkAccept == 2)
		{
			if (players[UCID].PassStress < MAX_PASS_STRESS)
			{
				char str[10];
				int time2 = players[UCID].WorkTime - time(NULL);

				if (time2==0 and players[UCID].cf != 3)
				{
					insim->SendMTC(UCID, TaxiDialogs["endtimer"][rand()%TaxiDialogs["endtimer"].size()].c_str());
					players[UCID].PassStress += 100;
					players[UCID].cf = 3;
					players[UCID].IsPursuit = false;
				}

                if (time2>=0)
                {
                    sprintf(str, "^1%02d:%02d", time2 / 60, time2%60);
                    insim->SendButton(255, UCID, 212, 62, 1, 10, 8, 32, str);
                }
			}
		}
    }
}

void RCTaxi::PassAccept( byte UCID )
{
    if ( players[UCID].Work == 1 and players[UCID].WorkNow == 1 and players[UCID].CanWork and
            ( players[UCID].WorkAccept == 0 or (players[UCID].WorkAccept == 2 and players[UCID].ClientType == 3)))
    {
        if (players[UCID].AcceptTime >= time(&acctime))
			return;

        // ��� �������:
        // ��� ������ �����, ��� ����� ����������� ��� ���������
        // 1 - �������, �� ���������, ������ ����� � ���
        // 2 - ���������, ������ �� ������ � �� �����, �� ������ ���������� ��������
        // 3 - ������ ��������� ��� ������ ����� (�� ������� ���� ����� ����� ���������� �� ������)
        // 4 - ������� ��� ������, ������ ������� � ������ ����� (�� ������� ���� ����� ����� ���������� �� �������)
        // 5 - ����� ������� ����, �������� � 2 ��� �������, ������� ��������� ������

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
            #ifdef CIS_LINUX
			usleep(100 * 1000);
			#else
			Sleep(100);
			#endif
		}

		players[UCID].IsPursuit = false;

        if (players[UCID].ClientType == 0)
            players[UCID].ClientType = count - (int)sqrt(rand()%(count * count));
        else
            players[UCID].ClientType = 1;

        players[UCID].WorkPointDestinaion = DestPoint;
        players[UCID].WorkStreetDestinaion = ClientPoints[DestPoint].StreetId; //����� ����������

        struct streets StreetInfo;

        //����� �������
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
        insim->SendMTC(UCID, str);

        sprintf(str, msg->_(UCID, "TaxiAccept1"), street->GetStreetName(UCID, StreetInfo.StreetID));
        ButtonInfo(UCID, str);

		players[UCID].WorkAccept = 1;
    }
}

void RCTaxi::PassAccept2(byte UCID)
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

		int MINDIST = 200;

        if (ClientPoints[DestPoint].StreetId != street->CurentStreetNum( UCID ) and Distance(X1, Y1, X2, Y2)>MINDIST)
            ok = false;

        #ifdef CIS_LINUX
        usleep(100 * 1000);
        #else
        Sleep(100);
        #endif
    }

    int DestStreet = ClientPoints[DestPoint].StreetId; //����� ����������

    struct streets StreetInfo;
    memset(&StreetInfo, 0, sizeof(streets));


    //������ �������
    DeleteMarshal( UCID );

    players[UCID].WorkPointDestinaion = DestPoint;
    players[UCID].WorkStreetDestinaion = DestStreet;
    memset(&StreetInfo, 0, sizeof(streets));
    street->CurentStreetInfoByNum(&StreetInfo, DestStreet);

    players[UCID].WorkAccept = 2;

    if (players[UCID].cf == 1 or players[UCID].cf == 4)
		players[UCID].PassStress -= 100;
	else if (players[UCID].ClientType == 2 or players[UCID].ClientType == 5)
        players[UCID].PassStress = rand()%300;
    else
		players[UCID].PassStress = rand()%500;

    char Msg[96];
    char Btn[96];

    sprintf(Btn, msg->_(UCID,"TaxiAccept2"), street->GetStreetName(UCID, StreetInfo.StreetID));

    if (players[UCID].ClientType == 1)
        sprintf(Msg, TaxiDialogs["client_1"][rand()%TaxiDialogs["client_1"].size()].c_str(), street->GetStreetName(UCID, StreetInfo.StreetID));

    if (players[UCID].ClientType == 2)
	{
		sprintf(Msg, TaxiDialogs["client_2"][rand()%TaxiDialogs["client_2"].size()].c_str(), street->GetStreetName(UCID, StreetInfo.StreetID));
		players[UCID].WorkTime = time(NULL) + 50 + (int)rand()%30;
	}

    if (players[UCID].ClientType == 3)
    {
        sprintf(Msg, TaxiDialogs["client_3"][rand()%TaxiDialogs["client_3"].size()].c_str());

        insim->SendMTC(UCID, Msg);
        ButtonInfo(UCID, Btn);
        players[UCID].cf = 1;
        PassAccept(UCID);
        return;
    }

    if (players[UCID].ClientType == 4)
        sprintf(Msg, TaxiDialogs["client_1"][rand()%TaxiDialogs["client_1"].size()].c_str(), street->GetStreetName(UCID, StreetInfo.StreetID));

	if (players[UCID].ClientType == 5)
	{
		players[UCID].cf = 2;
        sprintf(Msg, TaxiDialogs["client_5"][rand()%TaxiDialogs["client_5"].size()].c_str(), street->GetStreetName(UCID, StreetInfo.StreetID));
        players[UCID].PassStress = 500;
	}

	if (players[UCID].cf == 1 and players[UCID].ClientType == 1)
    {
        sprintf(Msg, TaxiDialogs["client_31"][rand()%TaxiDialogs["client_31"].size()].c_str(), street->GetStreetName(UCID, StreetInfo.StreetID));
        players[UCID].cf = 1;
    }

    insim->SendMTC(UCID, Msg);
    ButtonInfo(UCID, Btn);
}

void RCTaxi::InsimCNL( struct IS_CNL* packet )
{
    //������ ��������
    DeleteMarshal( packet->UCID );
    Save( packet->UCID );
    players.erase( packet->UCID );
    NumP = packet->Total;
}

void RCTaxi::InsimCPR( struct IS_CPR* packet )
{
    players[packet->UCID].PName = packet->PName;
}

void RCTaxi::InsimMCI ( struct IS_MCI* pack_mci )
{
    for (int i = 0; i < pack_mci->NumC; i++) // ������ �� ����� ������� pack_mci->Info[i]
    {
        byte UCID = PLIDtoUCID[ pack_mci->Info[i].PLID ];

        if (players[UCID].WorkNow != 0 and police->IsCop(UCID) and players[UCID].CanWork)
        {
            insim->SendMTC(UCID, msg->_(UCID, "1303" ));
            players[UCID].CanWork=false;
            DeleteMarshal(UCID);
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
                insim->SendMTC(UCID, msg->_(UCID, "TaxiDialog1"));
                insim->SendMTC(UCID, msg->_(UCID, "TaxiDialog2"));
                insim->SendMTC(UCID, msg->_(UCID, "TaxiDialog3"));
                insim->SendMTC(UCID, msg->_(UCID, "TaxiDialog4"));
                insim->SendMTC(UCID, msg->_(UCID, "TaxiDialog5"));
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
                if (players[UCID].ClientType != 2 and Speed>(StreetInfo.SpeedLimit + 10) and players[UCID].PassStress < MAX_PASS_STRESS)
                {
                    if (players[UCID].OverSpeedCount==5)
                        players[UCID].OverSpeedCount=0;

                    if (players[UCID].OverSpeedCount==0)
                        insim->SendMTC(UCID,  TaxiDialogs["speed"][ rand()%TaxiDialogs["speed"].size() ].c_str() ); // ���������� ��������

                    players[UCID].OverSpeedCount++;
                    players[UCID].PassStress += Speed-StreetInfo.SpeedLimit + 10;
                }

                if (players[UCID].ClientType != 2 and lgh->CheckOnRed(UCID))
                {
                    insim->SendMTC(UCID,  TaxiDialogs["redlight"][ rand()%TaxiDialogs["redlight"].size() ].c_str() ); // ������� ��������
                    players[UCID].PassStress += 200;
                    lgh->OnRedFalse(UCID);
                }
            }

            if (players[UCID].WorkStreetDestinaion == street->CurentStreetNum( UCID ))
            {
                int des_X = ClientPoints[players[UCID].WorkPointDestinaion].X / 65536;
                int des_Y = ClientPoints[players[UCID].WorkPointDestinaion].Y / 65536;

                /** ��������� ��������� �� ����� ��������� **/
                float Dist = Distance(X , Y , des_X , des_Y);

                if (players[UCID].OnStreet == false and players[UCID].PassStress < MAX_PASS_STRESS)
                {
                    players[UCID].OnStreet = true;
                    char MSG[128];
                    sprintf(MSG, msg->_(UCID, "TaxiOnStreet"), (Dist-(int)Dist%10));
                    insim->SendMTC(UCID, MSG);
                }

                if (players[UCID].WorkAccept == 2 and players[UCID].PassStress < MAX_PASS_STRESS)
                    insim->SendButton(255, UCID, 206, 45, 125, 8, 4, ISB_DARK + ISB_RIGHT, StringFormat("^7%0.0f ^C� ", Dist-(int)Dist%5));

                if (Dist <= 30)
                {
                    //������ ��������� ����
                    if (!players[UCID].HandUp and players[UCID].WorkAccept == 1)
                    {
                        //����������� �� ����
                        float xx = (ClientPoints[players[UCID].WorkPointDestinaion].X - pack_mci->Info[i].X) / 65536;
                        float yy = (ClientPoints[players[UCID].WorkPointDestinaion].Y - pack_mci->Info[i].Y) / 65536;
                        float gip = sqrt(xx * xx + yy * yy);
                        float ddd = acos(xx / gip) * (180 / 3.14);

                        if (yy>0)
                            ddd=360-ddd;

                        ddd=ddd + 90;

                        if (ddd>360)
                            ddd=ddd-360;

                        ddd=(360-ddd) / 360 * 256; //���� �������� ��� �������

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

                    if (players[UCID].WorkAccept == 2 and players[UCID].PassStress < MAX_PASS_STRESS)
                    {
                        if (players[UCID].ClientType == 4)
                        {
                            insim->SendMTC(UCID, TaxiDialogs["client_4"][rand()%TaxiDialogs["client_4"].size()].c_str());
                            players[UCID].ClientType = 1;
                            players[UCID].cf = 4;
                            PassAccept2(UCID);
                            return;
                        }

                        if (players[UCID].InPasZone != 1)
                        {
                            players[UCID].InPasZone = 1;
                            srand ( time(NULL) );
                            insim->SendMTC(UCID, TaxiDialogs["dist"][ rand()%TaxiDialogs["dist"].size() ].c_str() ); // ��������
                        }
                    }

                    if (Speed == 0 and players[UCID].WorkAccept == 2)
                        PassDone( UCID ); //��������
                }
                else
                {
                    //������ �������� ����
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

                    if ((players[UCID].WorkAccept == 2 or players[UCID].cf) and players[UCID].PassStress < MAX_PASS_STRESS)
                    {
                        if (players[UCID].InPasZone == 1)
                        {
                            players[UCID].InPasZone = 0;

                            players[UCID].PassStress += 10;
                            srand ( time(NULL) );
                            insim->SendMTC(UCID, TaxiDialogs["past"][ rand()%TaxiDialogs["past"].size() ].c_str() ); // ������� ����
                        }
                    }
                }

                if (Dist < 5 and Speed == 0 and players[UCID].WorkAccept == 1)
                    PassAccept2( UCID ); // ��������
            }
            else
            {
                if (players[UCID].OnStreet = true)
                    insim->SendBFN(UCID, 206);

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

                if (players[UCID].PassStress < MAX_PASS_STRESS)
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

				float coef = 1;
				if (players[UCID].ClientType == 2)
					coef = 0.5;
				if (players[UCID].ClientType == 5)
					coef = 5;

                players[UCID].PassStress += K * coef;

                if (players[UCID].PassStress > MAX_PASS_STRESS)
                    players[UCID].PassStress = MAX_PASS_STRESS;

                if (players[UCID].PassStress < 0)
                    players[UCID].PassStress = 0;

                if (X1==0 and Y1==0 and Z1==0)
                {
                    X1=X;
                    Y1=Y;
                    Z1=Z;
                }

                if (Speed == 0 and players[UCID].PassStress < 1000)
                {
                    if (lgh->CheckLight(UCID) == 2)
                    {
                        players[UCID].PassStress -= 3;
                        players[UCID].SpeedOff = 0;
                    }
                    if (lgh->CheckLight(UCID) == 1)
                        players[UCID].PassStress += 3;

                    if (players[UCID].SpeedOff>10 and lgh->CheckLight(UCID) != 2)
                    {
                        players[UCID].SpeedOff=0;
                        players[UCID].PassStress += 300;
                        insim->SendMTC(UCID,  TaxiDialogs["speedoff"][ rand()%TaxiDialogs["speedoff"].size() ].c_str() ); // ��������, ������� ����� � �� �������
                    }
                    players[UCID].SpeedOff++;
                }
                else if (players[UCID].SpeedOff != 0)
                    players[UCID].SpeedOff = 0;

                BtnStress( UCID );

                if (players[UCID].PassStress >= MAX_PASS_STRESS)
                {
                    insim->SendBFN(UCID, 206);
                    if (players[UCID].StressOverCount == 0)
                    {
                        srand ( time(NULL) );
                        insim->SendMTC(UCID,  TaxiDialogs["needstop"][ rand()%TaxiDialogs["needstop"].size() ].c_str() ); // ��������, ������� ����������
                        ClearButtonInfo( UCID );
                    }

                    players[UCID].StressOverCount ++;

                    if (players[UCID].StressOverCount >= 20)
                        players[UCID].StressOverCount = 0;

                    if (Speed == 0)
                        PassDone( UCID );
                }
                else if(police->IsPursuit(UCID))
                {
                    if (players[UCID].ClientType == 2)
                    {
                        if (!players[UCID].IsPursuit)
                        {
                            players[UCID].IsPursuit = true;
                            players[UCID].PassStress -= 200;
                            insim->SendMTC(UCID,  TaxiDialogs["client_21"][ rand()%TaxiDialogs["client_21"].size() ].c_str() );
                        }
                    }
                    else
                    {
                        players[UCID].PassStress = MAX_PASS_STRESS;
                        insim->SendMTC(UCID,  TaxiDialogs["pursuit"][ rand()%TaxiDialogs["pursuit"].size() ].c_str() ); // ��������, ������� ����������
                    }
                }

            }
        }

        memcpy( &players[UCID].Info , &pack_mci->Info[i] , sizeof( CompCar ) );

        /** thread xD **/
        PassAccept( UCID );
        /** thread **/
    }
}

void RCTaxi::PassDead(byte UCID)
{
    if (players[UCID].WorkAccept != 0
            and abs(ClientPoints[players[UCID].WorkPointDestinaion].X / 65536 - players[UCID].Info.X / 65536) < 5 )
    {
        //������� �������
        DeleteMarshal( UCID );
        players[UCID].WorkAccept = 0;
        players[UCID].WorkPointDestinaion = 0;
        players[UCID].WorkStreetDestinaion = 0;
        players[UCID].StressOverCount = 0;
        players[UCID].PassStress = 0;
        players[UCID].Work = 0;
        players[UCID].WorkNow = 0;
        insim->SendMTC(UCID, msg->_(UCID, "TaxiDead"));
    }
}

void RCTaxi::InsimMSO( struct IS_MSO* packet )
{
    if (packet->UCID == 0)
    {
        return;
    }

    byte UCID = packet->UCID;

    string Message;
    Message = packet->Msg + (packet->TextStart);

    if (players[packet->UCID].InZone == 1)
    {
        if ( Message == "!deal" )
        {
            if (dl->GetLVL( packet->UCID ) < 20)
            {
                insim->SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog6"));
                return;
            }

            if (players[packet->UCID].Work != 0)
            {
                insim->SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog7"));
                return;
            }

            if (players[UCID].FiredPenalty > time(NULL))
			{
				int PenaltyTime = ( players[UCID].FiredPenalty - time(NULL) ) / 60;
				insim->SendMTC(packet->UCID, StringFormat( msg->_(UCID, "TaxiFiredPenalty" ), PenaltyTime) );
                return;
			}

            insim->SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog8"));
            players[packet->UCID].Work = 1;
        }

        if ( Message == "!undeal" )
        {
            if (players[packet->UCID].Work ==0)
            {
                insim->SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog9"));
                return;
            }
            insim->SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog10"));
            //������ ��������
            DeleteMarshal( packet->UCID );

            players[packet->UCID].WorkAccept = 0;
            players[packet->UCID].WorkPointDestinaion = 0;
            players[packet->UCID].WorkStreetDestinaion = 0;
            players[packet->UCID].StressOverCount = 0;
            players[packet->UCID].PassStress = 0;
            players[packet->UCID].WorkNow = 0;
            players[packet->UCID].Work = 0;
        }

        if ( Message == "!workstart" )
        {
            if (players[packet->UCID].Work ==0)
            {
                insim->SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog11"));
                return;
            }

            if (police->IsCop(packet->UCID))
            {
                insim->SendMTC(packet->UCID, msg->_(packet->UCID, "1303" ));
                return;
            }

            if (players[packet->UCID].WorkNow ==1)
            {
                insim->SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog12"));
                return;
            }

            if (!players[packet->UCID].CanWork)
            {
                insim->SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog13"));
                return;
            }

            players[packet->UCID].AcceptTime = time(NULL) + PASSANGER_INTERVAL / (NumP + 1);
            players[packet->UCID].WorkNow = 1;
            insim->SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog14"));
        }

        if ( Message == "!workend" )
        {
            if (players[packet->UCID].Work ==0)
            {
                insim->SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog11"));
                return;
            }

            if (players[packet->UCID].WorkNow ==0)
            {
                insim->SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog15"));
                return;
            }

            insim->SendMTC(packet->UCID, msg->_(UCID, "TaxiDialog16"));

            //������ ��������
            DeleteMarshal( packet->UCID );

            players[packet->UCID].WorkNow = 0;
            players[packet->UCID].WorkAccept = 0;
            players[packet->UCID].WorkPointDestinaion = 0;
            players[packet->UCID].WorkStreetDestinaion = 0;
            players[packet->UCID].StressOverCount = 0;
            players[packet->UCID].PassStress = 0;
        }
    }

    if ( Message.find("!tstat") == 0 )
    {
        string uname = players[UCID].UName;
        if ( Message.size() > 7 and players[UCID].Admin)
            uname = Message.replace(0,7,"");

        if (uname.length() == 0)
        {
            insim->SendMTC(UCID, msg->_(packet->UCID, "TaxiDialog17"));
            return;
        }

        for ( auto& p: players )
			if ( uname == players[p.first].UName )
			{
				string str = StringFormat("^6| ^C^7���������� ������ � ����� ^8%s ^7(^9%s^7)", players[p.first].PName.c_str(), players[p.first].UName.c_str());
				insim->SendMTC(packet->UCID, str);

				if (players[p.first].Work == 0)
				{
					str ="^6| ^C^7�� ��������";
					insim->SendMTC(packet->UCID, str);
				}
				else
                {
                    if (players[p.first].WorkNow == 0)
                    {
                        str = "^6| ^C^7�� �� �����";
                        insim->SendMTC(packet->UCID, str);
                    }
                    else
                    {
                        str = StringFormat("^6| ^C^7������� ���������: %d", players[p.first].WorkAccept);
                        insim->SendMTC(packet->UCID, str);
                    }
                }

				str = StringFormat("^6| ^C^7����� ����� ����������: %d", players[p.first].PassCount);
				insim->SendMTC(packet->UCID, str);
				str = StringFormat("^6| ^C^7����� ���� ����������: %d", players[p.first].PenaltyCount);
				insim->SendMTC(packet->UCID, str);
				return;
			}
		insim->SendMTC(packet->UCID, "^6| ^C^7����� �� ������");
    }

    if ( Message == "!points" and players[UCID].Admin)
    {
        if (StartPointsAdd == 0)
        {
            StartPointsAdd = 1;
            insim->SendMTC(packet->UCID, "^3points clients ^2ON");
        }
        else if (StartPointsAdd == 1)
        {
            StartPointsAdd = 2;
            insim->SendMTC(packet->UCID, "^3points points ^2ON");
        }
        else if (StartPointsAdd == 2)
        {
            StartPointsAdd = 0;
            insim->SendMTC(packet->UCID, "^3points ^1OFF");
        }
    }
}

void RCTaxi::InsimNCN( struct IS_NCN* packet )
{
    if (packet->UCID == 0)
    {
        return;
    }

    players[packet->UCID].UName = packet->UName;
    players[packet->UCID].PName = packet->PName;
    players[packet->UCID].Admin = packet->Admin;

    ReadUser( packet->UCID );
    NumP = packet->Total;
}

void RCTaxi::InsimNPL( struct IS_NPL* packet )
{
    PLIDtoUCID[ packet->PLID ] = packet->UCID;
    players[packet->UCID].CName = packet->CName;

    if ( players[packet->UCID].CName == "MRT" or
            players[packet->UCID].CName == "UFR" or
            players[packet->UCID].CName == "XFR" or
            //players[packet->UCID].CName == "FXR" or
            players[packet->UCID].CName == "XRR" or
            players[packet->UCID].CName == "FZR" or
            players[packet->UCID].CName == "FBM" or
            players[packet->UCID].CName == "FOX" or
            players[packet->UCID].CName == "FO8" or
            players[packet->UCID].CName == "BF1")
        players[packet->UCID].CanWork=false;
    else
        players[packet->UCID].CanWork=true;

    if (players[packet->UCID].WorkNow != 0 and !players[packet->UCID].CanWork)
    {
        insim->SendMTC(packet->UCID, msg->_(packet->UCID, "TaxiDialog13"));
        DeleteMarshal(packet->UCID);
        players[packet->UCID].WorkAccept = 0;
        players[packet->UCID].WorkPointDestinaion = 0;
        players[packet->UCID].WorkStreetDestinaion = 0;
        players[packet->UCID].StressOverCount = 0;
        players[packet->UCID].PassStress = 0;
    }
}

void RCTaxi::PassLoss(byte UCID)
{
    if (players[UCID].WorkAccept == 2 or players[UCID].cf)
    {
    	insim->SendMTC(UCID,msg->_(UCID,"TaxiPll"));
        insim->SendBFN(UCID, 206);
        insim->SendBFN(UCID, 207);
        insim->SendBFN(UCID, 212);
        ClearButtonInfo(UCID);
        players[UCID].cf = 0;
        players[UCID].ClientType = 0;
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
    PassLoss(UCID);
}

void RCTaxi::InsimPLL( struct IS_PLL* packet )
{

    byte UCID = PLIDtoUCID[packet->PLID];
    PassLoss(UCID);

    PLIDtoUCID.erase( packet->PLID );
}

void RCTaxi::ReadUser( byte UCID )
{
	string query = StringFormat("SELECT * FROM taxi WHERE username = '%s'",players[UCID].UName.c_str());

	DB_ROWS res = db->select( query );

	if( res.size() > 0 )
	{
		DB_ROW row = res.front();

		players[UCID].Work = atoi( row["Work"].c_str() );
		players[UCID].FiredPenalty= atoi( row["FiredPenalty"].c_str() );
		players[UCID].PenaltyCount = atoi( row["PenaltyCount"].c_str() );
		players[UCID].PassCount = atoi( row["PassCount"].c_str() );
	}
	else
	{
		players[UCID].Work= 0;
		players[UCID].FiredPenalty= 0;
		players[UCID].PenaltyCount = 0;
		players[UCID].PassCount = 0;
		Save( UCID );
	}

}

void RCTaxi::DeleteMarshal(byte UCID)
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

void RCTaxi::Save( byte UCID )
{
	string query = StringFormat("REPLACE INTO taxi (username, Work, FiredPenalty, PenaltyCount, PassCount) VALUES ('%s',%d,%d,%d,%d)",
								players[UCID].UName.c_str(),
								players[UCID].Work,
								players[UCID].FiredPenalty,
								players[UCID].PenaltyCount,
								players[UCID].PassCount);

	db->exec( query );

}

void RCTaxi::PassDone( byte UCID )
{
    insim->SendBFN(UCID, 206);
    insim->SendBFN(UCID, 207);
    insim->SendBFN(UCID, 212);
    if (players[UCID].PassStress < MAX_PASS_STRESS)
    {
        players[UCID].PassCount++;
        srand(time(NULL));
        insim->SendMTC(UCID, TaxiDialogs["done"][ rand()%TaxiDialogs["done"].size() ].c_str() ); // send random dialog phrase

        float coef = 1, coef2 = 1;
        if (players[UCID].cf == 1 or players[UCID].cf == 4)
		{
            coef = 1.5;
            coef2 = 1.2;
		}
		if (players[UCID].cf == 2)
		{
            coef = 2;
			coef2 = 1.4;
		}
        if (players[UCID].cf == 3)
            coef = 0.5;
        if (players[UCID].ClientType == 2 and players[UCID].IsPursuit)
        {
            coef = 1.5;
			coef2 = 1.2;
		}

        bank->AddCash(UCID, (1000*coef - players[UCID].PassStress / 2), true);
        dl->AddSkill(UCID, coef2);
    }
    else
    {
    	players[UCID].PenaltyCount++;
        srand (time(NULL));
        insim->SendMTC(UCID, TaxiDialogs["exit"][ rand()%TaxiDialogs["exit"].size() ].c_str() ); // send random dialog phrase

        if( players[UCID].PenaltyCount % 3 == 0 )
		{
			players[UCID].FiredPenalty = time(NULL) + 60 * players[UCID].PenaltyCount;

			players[UCID].WorkAccept = 0;
            players[UCID].WorkPointDestinaion = 0;
            players[UCID].WorkStreetDestinaion = 0;
            players[UCID].StressOverCount = 0;
            players[UCID].PassStress = 0;
            players[UCID].WorkNow = 0;
            players[UCID].Work = 0;

            insim->SendMTC(UCID, msg->_(UCID,"TaxiDialog10") );
		}
    }

	ClearButtonInfo(UCID);
    players[UCID].cf = 0;
    players[UCID].ClientType = 0;
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
    if (players[UCIDA].ClientType == 5 or players[UCIDB].ClientType == 5)
        coef = 30;

    if ((players[ UCIDA ].WorkAccept == 2 or players[UCIDA].cf) and (now - players[UCIDA].LastT) > 1)
    {
        players[UCIDA].LastT = now;
        players[ UCIDA ].PassStress += coef * packet->SpClose;

        srand ( time(NULL) );
        insim->SendMTC( UCIDA, TaxiDialogs["con"][ rand()%TaxiDialogs["con"].size() ].c_str() ); // send random dialog phrase
    }

    if ((players[ UCIDB ].WorkAccept == 2 or players[UCIDB].cf) and (now - players[UCIDB].LastT) > 1)
    {
        players[UCIDB].LastT = now;
        players[ UCIDB ].PassStress += coef * packet->SpClose;

        srand ( time(NULL) );
        insim->SendMTC( UCIDB ,  TaxiDialogs["con"][ rand()%TaxiDialogs["con"].size() ].c_str() ); // send random dialog phrase
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
        if (players[UCID].ClientType == 5)
            coef = 5;

        if ((packet->Index==128 or packet->Index==129) and packet->SpClose>50)
        {
            players[UCID].PassStress += packet->SpClose + 20;
            srand(time(NULL));
            insim->SendMTC(UCID, TaxiDialogs["speedhump"][ rand()%TaxiDialogs["speedhump"].size() ].c_str() ); // send random dialog phrase
        }
        else if ((packet->Index > 45 and packet->Index < 125 and packet->Index!=120 and packet->Index!=121) or (packet->Index > 140))
        {
            players[UCID].PassStress +=  packet->SpClose*coef;
            srand(time(NULL));
            insim->SendMTC(UCID, TaxiDialogs["obh"][ rand()%TaxiDialogs["obh"].size() ].c_str() ); // send random dialog phrase
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
            insim->SendMTC(255, text);

            ofstream readf ("Clients.ini", ios::app);
            readf << text << endl;
            readf.close();
        }
        else
        {
            sprintf(text, "%d, %d, %d", packet->Info[0].X, packet->Info[0].Y, StreetID);
            insim->SendMTC(255, text);

            ofstream readf ("Points.ini", ios::app);
            readf << text << endl;
            readf.close();
        }
    }
}

void RCTaxi::InsimHLV( struct IS_HLV* packet )
{
    byte UCID = PLIDtoUCID[ packet->PLID ];

    /** ������������ �� ������ **/
    if (packet->HLVC==1)
    {
        time_t now = time(NULL);

        if ((now - players[UCID].LastT) < 1)
            return;

        players[UCID].LastT = now;

        int coef = 10;
        if (players[UCID].ClientType == 5)
            coef = 30;

        if (players[UCID].WorkAccept == 2 or players[UCID].cf)
        {
            players[UCID].PassStress +=  packet->C.Speed * coef;
            srand(time(NULL));
            insim->SendMTC(UCID, TaxiDialogs["obh"][ rand()%TaxiDialogs["obh"].size() ].c_str() ); // send random dialog phrase
        }
    }
}

void RCTaxi::BtnStress( byte UCID )
{
	string str = "";

	if (!players[UCID].blink)
	{
		players[UCID].blink = true;
		str = "^1";
	}
	else
		players[UCID].blink = false;

    if (players[UCID].PassStress == MAX_PASS_STRESS)
        for (int i = 1; i <= players[UCID].PassStress / 10; i++)
            str += "I";
    else
        for (int i = 1; i <= players[UCID].PassStress / 10; i++) // MAX_PASS_STRESS / 10 = 100
        {
            if (i == 1)
                str += "^2";

            if (i == 50)
                str += "^3";

            if (i == 94)
                str += "^1";

            str += "I";
        }
    str += "^8";

    for (int i = 0; i < (100 - players[UCID].PassStress / 10); i++)
        str += "I";

    insim->SendButton(255, UCID, 207, 1, 125, 44, 4, 32+2, str);
}

bool RCTaxi::IfWork (byte UCID)
{
    if (players[UCID].Work != 0)
        return true;
    return false;
}
