using namespace std;
#include "RCTaxi.h"

RCTaxi::RCTaxi()
{
    memset(ClientPoints, 0, sizeof( Taxi_clients ) * MAX_POINTS );
}

RCTaxi::~RCTaxi()
{

}

int RCTaxi::init(const char *dir,void *CInSim, void *Message,void *Bank,void *RCdl, void * STreet, void * Police, void * Light)
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

    lgh = (RCLight *)Light;
    if(!lgh)
    {
        printf ("Can't struct RCLight class");
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

    police = (RCPolice *)Police;
    if(!police)
    {
        printf ("Can't struct RCPolice class");
        return -1;
    }

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
	TaxiDialogs.empty();

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
        char key[128];
        char message[128];
        read.getline(key,128);

        if (strncmp(key,"#",1) == 0 )
        {
        	int i = 0;
        	read.getline(message,128);
        	while( strncmp(message,"=",1) != 0 )
			{
				TaxiDialogs[key+1][i++] = message;
				read.getline(message,128);
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
        }
        ClientCount=i;
    }
    readt.close();

    //очистка маршалов
    for (int i=0;i<ClientCount;i++)
	{
		struct IS_AXM pacAXM;
		memset(&pacAXM, 0, sizeof(struct IS_AXM));
		pacAXM.Info[0].Index=255;
		pacAXM.Info[0].X=ClientPoints[i].X/4096;
		pacAXM.Info[0].Y=ClientPoints[i].Y/4096;
		pacAXM.Info[0].Zchar=ClientPoints[i].Z;
		pacAXM.Type=ISP_AXM;
		pacAXM.ReqI=1;
		pacAXM.NumO=1;
		pacAXM.Size=8+pacAXM.NumO*8;
		pacAXM.PMOAction = PMO_DEL_OBJECTS;
		insim->send_packet(&pacAXM);
	}
}

void RCTaxi::Event()
{

}

void RCTaxi::accept_user( byte UCID )
{
	if (players[UCID].Work == 1 and players[UCID].WorkNow == 1 and players[UCID].WorkAccept == 0 and players[UCID].CanWork)
	{
		if (players[UCID].AcceptTime >= time(&acctime)) return;

		int DestPoint = 0;
		srand(time(NULL));
		bool ok = true;
		while (ok)
		{
			DestPoint = rand()%ClientCount;
			if (ClientPoints[DestPoint].StreetId != street->CurentStreetNum(UCID))
				ok = false;
			#ifdef __linux__
			sleep(100);
			#else
			Sleep(100);
			#endif
		}

		struct streets StreetInfo;
		memset(&StreetInfo,0,sizeof(streets));
		if (street->CurentStreetInfo(&StreetInfo, UCID ))
		{
			players[ UCID].WorkPointDestinaion = DestPoint;
			players[UCID].WorkStreetDestinaion = ClientPoints[DestPoint].StreetId; //улица назначения

			//рисую маршала
			players[UCID].HandUp=false;
			struct IS_AXM pacAXM;
			memset(&pacAXM, 0, sizeof(struct IS_AXM));
			pacAXM.Info[0].Index=255;
			pacAXM.Info[0].Heading=ClientPoints[DestPoint].Dir;
			pacAXM.Info[0].X=ClientPoints[DestPoint].X/4096;
			pacAXM.Info[0].Y=ClientPoints[DestPoint].Y/4096;
			pacAXM.Info[0].Zchar=ClientPoints[DestPoint].Z;
			pacAXM.Info[0].Flags=133;
			pacAXM.Type=ISP_AXM;
			pacAXM.ReqI=1;
			pacAXM.NumO=1;
			pacAXM.Size=8+pacAXM.NumO*8;
			pacAXM.PMOAction = PMO_ADD_OBJECTS;
			insim->send_packet(&pacAXM);

			memset(&StreetInfo,0,sizeof(streets));
			street->CurentStreetInfoByNum(&StreetInfo,players[UCID].WorkStreetDestinaion);

			char Msg[128];
			sprintf(Msg,msg->_(UCID,"TaxiAccept11"),street->GetStreetName(UCID, StreetInfo.StreetID).c_str());
			SendMTC(UCID,Msg);
			sprintf(Msg,msg->_(UCID,"TaxiAccept1"),street->GetStreetName(UCID, StreetInfo.StreetID).c_str());
			ButtonInfo(UCID,Msg);
			players[UCID].WorkAccept = 1;
		}
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
		int X1 = ClientPoints[DestPoint].X/65536,
			Y1 = ClientPoints[DestPoint].Y/65536,
			X2 = players[UCID].Info.X/65536,
			Y2 = players[UCID].Info.Y/65536;
		if (ClientPoints[DestPoint].StreetId != street->CurentStreetNum( UCID ) and Distance(X1, Y1, X2, Y2)>300)
			ok = false;
		#ifdef __linux__
        sleep(100);
        #else
        Sleep(100);
        #endif
	}

	int DestStreet = ClientPoints[DestPoint].StreetId; //улица назначения

	struct streets StreetInfo;
	memset(&StreetInfo,0,sizeof(streets));

	if (street->CurentStreetInfo(&StreetInfo, UCID ))
	{
		//удаляю маршала
		delete_marshal( UCID );

		players[UCID].WorkPointDestinaion = DestPoint;
		players[UCID].WorkStreetDestinaion = DestStreet;
		memset(&StreetInfo,0,sizeof(streets));
		street->CurentStreetInfoByNum(&StreetInfo,DestStreet);

		char Msg[128];
		sprintf(Msg,msg->_(UCID,"TaxiAccept22"),street->GetStreetName(UCID, StreetInfo.StreetID).c_str());
		SendMTC(UCID,Msg);
		sprintf(Msg,msg->_(UCID,"TaxiAccept2"),street->GetStreetName(UCID, StreetInfo.StreetID).c_str());
		ButtonInfo(UCID,Msg);
		players[UCID].WorkAccept = 2;
		players[UCID].PassStress = rand()%500;
	}
}

void RCTaxi::InsimCNL( struct IS_CNL* packet )
{
	//удаляю маршалов
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
			SendMTC(UCID,msg->_(UCID, "1303" ));
			players[UCID].CanWork=false;
			delete_marshal(UCID);
			players[UCID].WorkAccept = 0;
			players[UCID].WorkPointDestinaion = 0;
			players[UCID].WorkStreetDestinaion = 0;
			players[UCID].StressOverCount = 0;
			players[UCID].PassStress = 0;
		}

		int X = pack_mci->Info[i].X/65536;
		int Y = pack_mci->Info[i].Y/65536;
		int Speed = ((int)pack_mci->Info[i].Speed*360)/(32768);
		if(Check_Pos(4,zone.dealX,zone.dealY,X,Y))
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
			if(players[UCID].WorkAccept == 2)
			{

				struct streets StreetInfo;
				memset(&StreetInfo,0,sizeof(streets));
				street->CurentStreetInfoByNum(&StreetInfo,street->CurentStreetNum( UCID ));
				if (Speed>(StreetInfo.SpeedLimit+10) and players[UCID].PassStress <= 800)
				{
					if (players[UCID].spd==5) players[UCID].spd=0;
					if (players[UCID].spd==0)
						SendMTC(UCID,  TaxiDialogs["speed"][ rand()%TaxiDialogs["speed"].size() ].c_str() ); // превышаешь скорость

					players[UCID].spd++;
					players[UCID].PassStress += Speed-StreetInfo.SpeedLimit+10;
				}

				if (lgh->CheckOnRed(UCID))
				{
					SendMTC(UCID,  TaxiDialogs["redlight"][ rand()%TaxiDialogs["redlight"].size() ].c_str() ); // красный светофор
					players[UCID].PassStress += 200;
					lgh->OnRedFalse(UCID);
				}
			}

			if (players[UCID].WorkStreetDestinaion == street->CurentStreetNum( UCID ))
			{
				int des_X = ClientPoints[players[UCID].WorkPointDestinaion].X/65536;
				int des_Y = ClientPoints[players[UCID].WorkPointDestinaion].Y/65536;
				/** вычисляем растояние до точки остановки **/
				Dist = Distance(X , Y , des_X , des_Y);

				if (players[UCID].OnStreet == false and players[UCID].PassStress <= 800)
				{
					players[UCID].OnStreet = true;
					char MSG[128];
					sprintf(MSG,msg->_(UCID,"TaxiOnStreet"),(Dist-(int)Dist%10));
					SendMTC(UCID,MSG);
				}

				if (Dist <= 30)
				{
					//маршал поднимает руку
					if (!players[UCID].HandUp and players[UCID].WorkAccept == 1)
					{
						//направление на авто
						float xx = (ClientPoints[players[UCID].WorkPointDestinaion].X - pack_mci->Info[i].X)/65536;
						float yy = (ClientPoints[players[UCID].WorkPointDestinaion].Y - pack_mci->Info[i].Y)/65536;
						float gip = sqrt(xx*xx+yy*yy);
						float ddd = acos(xx/gip)*(180/3.14);
						if (yy>0)ddd=360-ddd;
						ddd=ddd+90;
						if (ddd>360) ddd=ddd-360;
						ddd=(360-ddd)/360*256; //угол поворота для маршала

						players[UCID].HandUp=true;
						struct IS_AXM pacAXM;
						memset(&pacAXM, 0, sizeof(struct IS_AXM));
						pacAXM.Info[0].Index=255;
						pacAXM.Info[0].Heading=(byte)ddd;
						pacAXM.Info[0].X=ClientPoints[players[UCID].WorkPointDestinaion].X/4096;
						pacAXM.Info[0].Y=ClientPoints[players[UCID].WorkPointDestinaion].Y/4096;
						pacAXM.Info[0].Zchar=ClientPoints[players[UCID].WorkPointDestinaion].Z;
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

					if (players[UCID].WorkAccept == 2 and players[UCID].PassStress <= 800)
					{
						if (players[UCID].InPasZone != 1)
						{
							players[UCID].InPasZone = 1;
							srand ( time(NULL) );
							SendMTC(UCID,  TaxiDialogs["dist"][ rand()%TaxiDialogs["dist"].size() ].c_str() ); // приехали
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
						players[UCID].HandUp=false;
						struct IS_AXM pacAXM;
						memset(&pacAXM, 0, sizeof(struct IS_AXM));
						pacAXM.Info[0].Index=255;
						pacAXM.Info[0].Heading=ClientPoints[players[UCID].WorkPointDestinaion].Dir;
						pacAXM.Info[0].X=ClientPoints[players[UCID].WorkPointDestinaion].X/4096;
						pacAXM.Info[0].Y=ClientPoints[players[UCID].WorkPointDestinaion].Y/4096;
						pacAXM.Info[0].Zchar=ClientPoints[players[UCID].WorkPointDestinaion].Z;
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

					if (players[UCID].WorkAccept == 2 and players[UCID].PassStress <= 800)
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
				if (players[UCID].OnStreet = true) SendBFN(UCID,206);
				players[UCID].OnStreet = false;
			}

			if (players[UCID].WorkAccept == 1)
			{
				struct streets StreetInfo;
				memset(&StreetInfo,0,sizeof(streets));
				street->CurentStreetInfoByNum(&StreetInfo,players[UCID].WorkStreetDestinaion);

				char Msg[128];
				sprintf(Msg,msg->_(UCID,"TaxiAccept1"),street->GetStreetName(UCID, StreetInfo.StreetID).c_str());
				ButtonInfo(UCID,Msg);
			}

			if (players[UCID].WorkAccept == 2)
			{
				struct streets StreetInfo;
				memset(&StreetInfo,0,sizeof(streets));
				street->CurentStreetInfoByNum(&StreetInfo,players[UCID].WorkStreetDestinaion);

				char Msg[128];
				sprintf(Msg,msg->_(UCID,"TaxiAccept2"),street->GetStreetName(UCID, StreetInfo.StreetID).c_str());
				if (players[UCID].PassStress <= 800) ButtonInfo(UCID,Msg);

				int X = pack_mci->Info[i].X/65536;
				int Y = pack_mci->Info[i].Y/65536;
				int Z = pack_mci->Info[i].Z/65536;
				int D = pack_mci->Info[i].Direction/182;
				int H = pack_mci->Info[i].Heading/182;

				int S = ((int)pack_mci->Info[i].Speed*360)/(32768);

				int A = pack_mci->Info[i].AngVel*360/16384;

				int X1 = players[UCID].Info.X/65536;
				int Y1 = players[UCID].Info.Y/65536;
				int Z1 = players[UCID].Info.Z/65536;
				int D1 = players[UCID].Info.Direction/182;
				int H1 = players[UCID].Info.Heading/182;
				int S1 = ((int)players[UCID].Info.Speed*360)/(32768);
				int A1 = players[UCID].Info.AngVel*360/16384;

				long dA = A-A1;
				long dS = S-S1;
				long dD = abs((int)(sin(D)*100))-abs((int)(sin(D1)*100));
				long dH = abs((int)(sin(H)*100))-abs((int)(sin(H1)*100));

				//int K = int(sqrt(abs((dD-dH)*(1+dA)*dS))/32);
				if (Speed<60 and abs(A)<40) dA=0;
				int K = (int)sqrt(abs((dD-dH)*(1+dA)*dS))/8;

				players[UCID].PassStress += K;

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
				sprintf(d,"^7%0.0f ^Cм ",(Dist-(int)Dist%10+10));
				if (players[UCID].OnStreet and players[UCID].PassStress <= 800)
					SendButton(255, UCID, 206, 49, 125, 8, 4, 32+128, d);

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
	if (players[UCID].WorkAccept != 0 and abs(ClientPoints[players[UCID].WorkPointDestinaion].X/65536 - players[UCID].Info.X/65536)<5)
	{
		//удаляем маршала
		delete_marshal( UCID );
		players[UCID].WorkAccept = 0;
		players[UCID].WorkPointDestinaion = 0;
		players[UCID].WorkStreetDestinaion = 0;
		players[UCID].StressOverCount = 0;
		players[UCID].PassStress = 0;
		players[UCID].Work = 0;
		players[UCID].WorkNow = 0;
		SendMTC(UCID,msg->_(UCID,"TaxiDead"));
	}
}

void RCTaxi::InsimMSO( struct IS_MSO* packet )
{
    if (packet->UCID == 0)
		return;
	byte UCID = packet->UCID;

    char Message[128];
    strcpy(Message,packet->Msg + ((unsigned char)packet->TextStart));

    if (strncmp(Message, "!save", strlen("!save")) == 0){
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
            //удаляю маршалов
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
				SendMTC(packet->UCID,msg->_(packet->UCID, "1303" ));
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
            players[packet->UCID].AcceptTime = time(NULL) + PASSANGER_INTERVAL/(NumP+1);
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

            //удаляю маршалов
            delete_marshal( packet->UCID );

            players[packet->UCID].WorkNow = 0;
            players[packet->UCID].WorkAccept = 0;
            players[packet->UCID].WorkPointDestinaion = 0;
            players[packet->UCID].WorkStreetDestinaion = 0;
            players[packet->UCID].StressOverCount = 0;
            players[packet->UCID].PassStress = 0;
        }
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
        return;

    strcpy(players[packet->UCID].UName, packet->UName);
    strcpy(players[packet->UCID].PName, packet->PName);

    read_user( packet->UCID );
    NumP ++;
}

void RCTaxi::InsimNPL( struct IS_NPL* packet )
{
	PLIDtoUCID[ packet->PLID ] = packet->UCID;
	strcpy(players[packet->UCID].CName ,packet->CName);

	if (strcmp(players[packet->UCID].CName,"MRT") == 0 or
		strcmp(players[packet->UCID].CName,"UFR") == 0 or
		strcmp(players[packet->UCID].CName,"XFR") == 0 or
		//strcmp(players[packet->UCID].CName,"FXR") == 0 or
		strcmp(players[packet->UCID].CName,"XRR") == 0 or
		strcmp(players[packet->UCID].CName,"FZR") == 0 or
		strcmp(players[packet->UCID].CName,"FBM") == 0 or
		strcmp(players[packet->UCID].CName,"FOX") == 0 or
		strcmp(players[packet->UCID].CName,"FO8") == 0 or
		strcmp(players[packet->UCID].CName,"BF1") == 0)
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

void RCTaxi::InsimPLP( struct IS_PLP* packet)
{
   //PLIDtoUCID.erase( packet->PLID );
}

void RCTaxi::InsimPLL( struct IS_PLL* packet )
{
   PLIDtoUCID.erase( packet->PLID );
}

void RCTaxi::read_user( byte UCID )
{
    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCTaxi\\users\\%s.txt",RootDir,players[UCID].UName);

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
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
        ifstream readf (file,ios::in);

        while (readf.good())
        {
            char str[128];
            readf.getline(str,128);
            if (strlen(str) > 0)
            {
                if (strncmp("Work=",str,strlen("Work="))==0)
                    players[UCID].Work = atoi(str+strlen("Work="));

                if (strncmp("FiredPenalty=",str,strlen("FiredPenalty="))==0)
                    players[UCID].FiredPenalty= atoi(str+strlen("FiredPenalty="));

                if (strncmp("PenaltyCount=",str,strlen("PenaltyCount="))==0)
                    players[UCID].PenaltyCount = atoi(str+strlen("PenaltyCount="));

                if (strncmp("PassCount=",str,strlen("PassCount="))==0)
                    players[UCID].PassCount = atoi(str+strlen("PassCount="));
            }
        }
        readf.close();
    }
    FindClose(fff);
}

void RCTaxi::delete_marshal(byte UCID)
{
   	struct IS_AXM pacAXM;
	memset(&pacAXM, 0, sizeof(struct IS_AXM));
	pacAXM.Info[0].Index=255;
	pacAXM.Info[0].Heading=ClientPoints[players[UCID].WorkPointDestinaion].Dir;
	pacAXM.Info[0].X=ClientPoints[players[UCID].WorkPointDestinaion].X/4096;
	pacAXM.Info[0].Y=ClientPoints[players[UCID].WorkPointDestinaion].Y/4096;
	pacAXM.Info[0].Zchar=ClientPoints[players[UCID].WorkPointDestinaion].Z;
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

void RCTaxi::save_user( byte UCID )
{
    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCTaxi\\users\\%s.txt",RootDir, players[UCID].UName);
    ofstream writef (file,ios::out);
    writef << "Work=" << players[UCID].Work << endl;
    writef << "FiredPenalty=" << players[UCID].FiredPenalty << endl;
    writef << "PenaltyCount=" << players[UCID].PenaltyCount << endl;
    writef << "PassCount=" << players[UCID].PassCount << endl;
    writef.close();
}

void RCTaxi::taxi_done( byte UCID )
{
    SendBFN(UCID,207);
    SendBFN(UCID,206);
    if (players[UCID].PassStress <= 800)
    {
        players[UCID].PassCount++;
        srand ( time(NULL) );
        SendMTC(UCID, TaxiDialogs["done"][ rand()%TaxiDialogs["done"].size() ].c_str() ); // send random dialog phrase

        bank->AddCash(UCID,(1000 - players[UCID].PassStress/2), true);
        dl->AddSkill( UCID );
        ClearButtonInfo( UCID );
    }
    else
    {
        srand ( time(NULL) );
        SendMTC(UCID, TaxiDialogs["exit"][ rand()%TaxiDialogs["exit"].size() ].c_str() ); // send random dialog phrase
    }
	players[UCID].AcceptTime = time(NULL) + rand()%PASSANGER_INTERVAL/(NumP+1);
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

	if (players[ UCIDA ].WorkAccept == 2 and (now - players[UCIDA].LastT) > 1)
	{
		players[UCIDA].LastT = now;
		players[ UCIDA ].PassStress += 10 * packet->SpClose;

		srand ( time(NULL) );
		SendMTC( UCIDA, TaxiDialogs["con"][ rand()%TaxiDialogs["con"].size() ].c_str() ); // send random dialog phrase
	}

	if (players[ UCIDB ].WorkAccept == 2 and (now - players[UCIDB].LastT) > 1)
	{
		players[UCIDB].LastT = now;
		players[ UCIDB ].PassStress += 10 * packet->SpClose;

		srand ( time(NULL) );
		SendMTC( UCIDB ,  TaxiDialogs["con"][ rand()%TaxiDialogs["con"].size() ].c_str() ); // send random dialog phrase
	}
}

void RCTaxi::InsimOBH( struct IS_OBH* packet )
{
	byte UCID = PLIDtoUCID[ packet->PLID ];

	time_t now = time(NULL);
	if((now - players[UCID].LastT) <= 1) return;
	players[UCID].LastT = now;

	if (players[UCID].WorkAccept == 2)
	{
		if ((packet->Index==128 or packet->Index==129) and packet->SpClose>30)
		{
			players[UCID].PassStress += packet->SpClose+20;
			srand(time(NULL));
			SendMTC(UCID, TaxiDialogs["speedhump"][ rand()%TaxiDialogs["speedhump"].size() ].c_str() ); // send random dialog phrase
		}
		else if((packet->Index > 45 and packet->Index < 125 and packet->Index!=120 and packet->Index!=121) or (packet->Index > 140))
		{
			players[UCID].PassStress +=  packet->SpClose;
			srand(time(NULL));
			SendMTC(UCID, TaxiDialogs["obh"][ rand()%TaxiDialogs["obh"].size() ].c_str() ); // send random dialog phrase
		}
		else players[UCID].PassStress +=  packet->SpClose/10;
	}
}

void RCTaxi::InsimAXM( struct IS_AXM* packet )
{
	if (packet->PMOAction != 1 or StartPointsAdd == 0)
		return;

	char text[96];

	int X = packet->Info[0].X*4096/65536;
	int Y = packet->Info[0].Y*4096/65536;
	int StreetID = -1;

	for (int g = 0; g < street->StreetNums; g++)
		if( Check_Pos(street->Street[g].PointCount,street->Street[g].StreetX,street->Street[g].StreetY,X,Y) )
		{
			StreetID = g;
			break;
		}

	if (StreetID != -1)
	if (StartPointsAdd == 1)
	{
		sprintf(text,"%d, %d, %d, %d, %d", packet->Info[0].X*4096, packet->Info[0].Y*4096, packet->Info[0].Zchar, packet->Info[0].Heading, StreetID);
		SendMTC(255,text);

		ofstream readf ("Clients.ini",ios::app);
		readf << text << endl;
		readf.close();
	}
	else
	{
		sprintf(text,"%d,%d,%d", packet->Info[0].X, packet->Info[0].Y, StreetID);
		SendMTC(255,text);

		ofstream readf ("Points.ini",ios::app);
		readf << text << endl;
		readf.close();
	}
}

void RCTaxi::InsimHLV( struct IS_HLV* packet )
{
	byte UCID = PLIDtoUCID[ packet->PLID ];

	/** столкновение со стеной **/
	if (packet->HLVC==1)
	{
		time_t now = time(NULL);
		if((now - players[UCID].LastT) < 1) return;
		players[UCID].LastT = now;

		if (players[UCID].WorkAccept == 2)
		{
			players[UCID].PassStress +=  packet->C.Speed*10;
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
    pack.BStyle = 32+64;
    pack.L = 1;
    pack.T = 125;
    pack.W = 48;
    pack.H = 4;
    strcpy(pack.Text,"");

    for (int i=1; i<=players[UCID].PassStress/10; i++) // 1000 / 10 = 100
    {
        if (i == 1)
            strcat(pack.Text,"^2");
        if (i == 40)
            strcat(pack.Text,"^3");
        if (i == 80)
            strcat(pack.Text,"^1");

        strcat(pack.Text,"||");
    }
    strcat(pack.Text,"^8");
    for (int i=0; i<(100-players[UCID].PassStress/10); i++) strcat(pack.Text,"||");
    insim->send_packet(&pack);
}

bool RCTaxi::IfWork (byte UCID)
{
	if (players[UCID].Work != 0)
		return true;
    return false;
}
