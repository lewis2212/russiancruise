using namespace std;
#include "RCTaxi.h"

RCTaxi::RCTaxi()
{
    memset(ClientPoints, 0, sizeof( Taxi_clients ) * MAX_POINTS );
}

RCTaxi::~RCTaxi()
{

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
}


void RCTaxi::accept_user( byte UCID )
{
	if ((players[ UCID ].Work == 1) and (players[ UCID ].WorkNow == 1) and (players[ UCID ].WorkAccept == 0))
	{

		if ( players[ UCID ].AcceptTime >= time(&acctime) ){
			return;
		}

		int DestPoint = 0;

		srand(time(NULL));
		bool ok = true;
		while (ok)
		{
			DestPoint = rand()%ClientCount;
			if (ClientPoints[DestPoint].StreetId != street->CurentStreetNum( UCID )) ok = false;
			Sleep(100);
		}

		struct streets StreetInfo;
		memset(&StreetInfo,0,sizeof(streets));

		if (street->CurentStreetInfo(&StreetInfo, UCID ))
		{
			players[ UCID ].WorkPointDestinaion = DestPoint;
			players[ UCID ].WorkStreetDestinaion = ClientPoints[DestPoint].StreetId; //улица назначения

			//рисую маршала
			players[ UCID ].HandUp=false;
			struct IS_AXM pacAXM;
			memset(&pacAXM, 0, sizeof(struct IS_AXM));
			pacAXM.Info[0].Index=255;
			pacAXM.Info[0].Heading=ClientPoints[players[ UCID ].WorkPointDestinaion].Dir;
			pacAXM.Info[0].X=ClientPoints[players[ UCID ].WorkPointDestinaion].X/4096;
			pacAXM.Info[0].Y=ClientPoints[players[ UCID ].WorkPointDestinaion].Y/4096;
			pacAXM.Info[0].Zchar=ClientPoints[players[ UCID ].WorkPointDestinaion].Z;
			pacAXM.Info[0].Flags=133;
			pacAXM.Type=ISP_AXM;
			pacAXM.ReqI=1;
			pacAXM.NumO=1;
			pacAXM.Size=8+pacAXM.NumO*8;
			pacAXM.PMOAction = PMO_ADD_OBJECTS;
			insim->send_packet(&pacAXM);

			memset(&StreetInfo,0,sizeof(streets));
			street->CurentStreetInfoByNum(&StreetInfo,players[ UCID ].WorkStreetDestinaion);

			char Msg[96];
			sprintf(Msg,"^6|^C^7 Заберите клиента на %s ",StreetInfo.Street);
			send_mtc( UCID ,Msg);
			sprintf(Msg,"^C^7Заберите клиента на %s ",StreetInfo.Street);
			btn_information( UCID ,Msg);
			players[ UCID ].WorkAccept = 1;
		}
	}

}

void RCTaxi::accept_user2(byte UCID)
{

	srand(time(NULL)); //подготовка функции rand() к работе

	int DestPoint = 0;
	bool ok = true;
	while (ok)
	{
		DestPoint = rand()%ClientCount;
		if (ClientPoints[DestPoint].StreetId != street->CurentStreetNum( UCID )) ok = false;
		Sleep(100);
	}

	int DestStreet = ClientPoints[DestPoint].StreetId; //улица назначения

	struct streets StreetInfo;
	memset(&StreetInfo,0,sizeof(streets));

	if (street->CurentStreetInfo(&StreetInfo, UCID ))
	{
		//удаляю маршала
		delete_marshal( UCID );

		players[ UCID ].WorkPointDestinaion = DestPoint;
		players[ UCID ].WorkStreetDestinaion = DestStreet;
		memset(&StreetInfo,0,sizeof(streets));
		street->CurentStreetInfoByNum(&StreetInfo,DestStreet);

		char Msg[96];
		sprintf(Msg,"^6|^C^7 Отвези меня на %s ",StreetInfo.Street);
		send_mtc( UCID ,Msg);
		sprintf(Msg,"^C^7Отвези клиента на %s ",StreetInfo.Street);
		btn_information( UCID ,Msg);
		players[ UCID ].WorkAccept = 2;
		players[ UCID ].PassStress = rand()%500;
	}
}


void RCTaxi::insim_cnl( struct IS_CNL* packet )
{
	//удаляю маршалов
	delete_marshal( packet->UCID );
	save_user( packet->UCID );
	players.erase( packet->UCID );
	NumP --;
}

void RCTaxi::insim_cpr( struct IS_CPR* packet )
{
	strcpy( players[ packet->UCID ].PName, packet->PName);
}

void RCTaxi::insim_mci ( struct IS_MCI* pack_mci )
{
    float Dist;
    char d[96];
    for (int i = 0; i < pack_mci->NumC; i++) // прогон по всему массиву pack_mci->Info[i]
    {
    	byte UCID = PLIDtoUCID[ pack_mci->Info[i].PLID ];

		int X = pack_mci->Info[i].X/65536;
		int Y = pack_mci->Info[i].Y/65536;
		int Speed = ((int)pack_mci->Info[i].Speed*360)/(32768);
		if(Check_Pos(4,zone.dealX,zone.dealY,X,Y))
		{
			if (players[ UCID ].InZone == 0)
			{
				players[ UCID ].InZone = 1;
				send_mtc( UCID ,"^6| ^3Taxi Radriges");
				send_mtc( UCID ,"^6| ^2!deal - ^CУстроиться");
				send_mtc( UCID ,"^6| ^2!undeal - ^CУволиться");
				send_mtc( UCID ,"^6| ^2!workstart - ^CНачать работу");
				send_mtc( UCID ,"^6| ^2!workend - ^CЗакончить работу");
			}
		}
		else if (players[ UCID ].InZone == 1) players[ UCID ].InZone = 0;

		/** player drive on dest street **/
		if (players[ UCID ].WorkNow == 1 and players[ UCID ].WorkAccept != 0)
		{
			if(players[ UCID ].WorkAccept == 2)
			{
				char dd[96];
				struct streets StreetInfo;
				memset(&StreetInfo,0,sizeof(streets));
				street->CurentStreetInfoByNum(&StreetInfo,street->CurentStreetNum( UCID ));
				if (Speed>(StreetInfo.SpeedLimit+10))
				{
					if (players[ UCID ].spd==5) players[ UCID ].spd=0;
					if (players[ UCID ].spd==0)
					{
						send_mtc( UCID ,  TaxiDialogs["speed"][ rand()%TaxiDialogs["speed"].size() ].c_str() ); // превышаешь скорость
					}
					players[ UCID ].spd++;
					players[ UCID ].PassStress += 10;
				}
			}

			if (players[ UCID ].WorkStreetDestinaion == street->CurentStreetNum( UCID ))
			{
				int des_X = ClientPoints[players[ UCID ].WorkPointDestinaion].X/65536;
				int des_Y = ClientPoints[players[ UCID ].WorkPointDestinaion].Y/65536;
				/** вычисляем растояние до точки остановки **/
				Dist = Distance(X , Y , des_X , des_Y);
				sprintf(d," ^7%4.0f ^Cм",Dist);
				btn_Dist( UCID ,d);

				if (players[ UCID ].OnStreet == false)
				{
					players[ UCID ].OnStreet = true;
					char MSG[96];
					sprintf(MSG,"^6| ^C^7Остановитесь через %3.0f метров",(Dist-(int)Dist%10));
					send_mtc( UCID ,MSG);
				}

				if (Dist <= 30)
				{
					//маршал поднимает руку
					if (!players[ UCID ].HandUp and players[ UCID ].WorkAccept == 1)
					{
						//направление на авто
						float xx = (ClientPoints[players[ UCID ].WorkPointDestinaion].X - pack_mci->Info[i].X)/65536;
						float yy = (ClientPoints[players[ UCID ].WorkPointDestinaion].Y - pack_mci->Info[i].Y)/65536;
						float gip = sqrt(xx*xx+yy*yy);
						float ddd = acos(xx/gip)*(180/3.14);
						if (yy>0)ddd=360-ddd;
						ddd=ddd+90;
						if (ddd>360) ddd=ddd-360;
						ddd=(360-ddd)/360*256; //угол поворота для маршала

						players[ UCID ].HandUp=true;
						struct IS_AXM pacAXM;
						memset(&pacAXM, 0, sizeof(struct IS_AXM));
						pacAXM.Info[0].Index=255;
						pacAXM.Info[0].Heading=(byte)ddd;
						pacAXM.Info[0].X=ClientPoints[players[ UCID ].WorkPointDestinaion].X/4096;
						pacAXM.Info[0].Y=ClientPoints[players[ UCID ].WorkPointDestinaion].Y/4096;
						pacAXM.Info[0].Zchar=ClientPoints[players[ UCID ].WorkPointDestinaion].Z;
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

					if (players[ UCID ].WorkAccept == 2 and players[ UCID ].PassStress <= 800)
					{
						if (players[ UCID ].InPasZone != 1)
						{
							players[ UCID ].InPasZone = 1;
							srand ( time(NULL) );
							send_mtc( UCID ,  TaxiDialogs["dist"][ rand()%TaxiDialogs["dist"].size() ].c_str() ); // приехали
						}
					}

					if (Speed < 5 and players[ UCID ].WorkAccept == 2)
						taxi_done( UCID ); //высадили
				}
				else
				{
					//маршал опускает руку
					if (players[ UCID ].HandUp and players[ UCID ].WorkAccept == 1)
					{
						players[ UCID ].HandUp=false;
						struct IS_AXM pacAXM;
						memset(&pacAXM, 0, sizeof(struct IS_AXM));
						pacAXM.Info[0].Index=255;
						pacAXM.Info[0].Heading=ClientPoints[players[ UCID ].WorkPointDestinaion].Dir;
						pacAXM.Info[0].X=ClientPoints[players[ UCID ].WorkPointDestinaion].X/4096;
						pacAXM.Info[0].Y=ClientPoints[players[ UCID ].WorkPointDestinaion].Y/4096;
						pacAXM.Info[0].Zchar=ClientPoints[players[ UCID ].WorkPointDestinaion].Z;
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

					if (players[ UCID ].WorkAccept == 2 and players[ UCID ].PassStress <= 800)
					{
						if (players[ UCID ].InPasZone == 1)
						{
							players[ UCID ].InPasZone = 0;
							players[ UCID ].PassStress += 10;
							srand ( time(NULL) );
							send_mtc( UCID ,  TaxiDialogs["past"][ rand()%TaxiDialogs["past"].size() ].c_str() ); // проехал мимо
						}
					}
				}

				if (Dist < 5 and Speed < 5 and players[ UCID ].WorkAccept == 1) accept_user2( UCID ); // посадили
			}
			else
			{
				if (players[ UCID ].OnStreet = true) send_bfn( UCID ,205);
				players[ UCID ].OnStreet = false;
			}

			if (players[ UCID ].WorkAccept == 2)
			{
				int X = pack_mci->Info[i].X/65536;
				int Y = pack_mci->Info[i].Y/65536;
				int Z = pack_mci->Info[i].Z/65536;
				int D = pack_mci->Info[i].Direction/182;
				int H = pack_mci->Info[i].Heading/182;

				int S = ((int)pack_mci->Info[i].Speed*360)/(32768);

				int A = pack_mci->Info[i].AngVel*360/16384;

				int X1 = players[ UCID ].Info.X/65536;
				int Y1 = players[ UCID ].Info.Y/65536;
				int Z1 = players[ UCID ].Info.Z/65536;
				int D1 = players[ UCID ].Info.Direction/182;
				int H1 = players[ UCID ].Info.Heading/182;
				int S1 = ((int)players[ UCID ].Info.Speed*360)/(32768);
				int A1 = players[ UCID ].Info.AngVel*360/16384;

				long dA = A-A1;
				long dS = S-S1;
				long dD = abs((int)(sin(D)*100))-abs((int)(sin(D1)*100));
				long dH = abs((int)(sin(H)*100))-abs((int)(sin(H1)*100));

				//int K = int(sqrt(abs((dD-dH)*(1+dA)*dS))/32);
				int K = (int)sqrt(abs((dD-dH)*(1+dA)*dS))/8;

				players[ UCID ].PassStress += K;

				if (players[ UCID ].PassStress > 1000)
					players[ UCID ].PassStress = 1000;

				if (X1==0 and Y1==0 and Z1==0)
				{
					X1=X;
					Y1=Y;
					Z1=Z;
				}

				btn_stress( UCID );
				sprintf(d," ^7%d ^Cм",(int)Dist);
				btn_Dist( UCID ,d);

				if (players[ UCID ].PassStress > 800)
				{
					if (players[ UCID ].StressOverCount == 0)
					{
						srand ( time(NULL) );
						send_mtc( UCID ,  TaxiDialogs["needstop"][ rand()%TaxiDialogs["needstop"].size() ].c_str() ); // пугаецца, требует остановить
					}

					players[ UCID ].StressOverCount ++;

					if (players[ UCID ].StressOverCount >= 20)
						players[ UCID ].StressOverCount = 0;

					if (Speed < 5)
						taxi_done( UCID );
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
						PointsAdd[f].StreetId = street->CurentStreetNum( UCID );
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
		memcpy( &players[ UCID ].Info , &pack_mci->Info[i] , sizeof( CompCar ) );

		/** thread xD **/
		accept_user( UCID );
		/** thread **/
    }
}

void RCTaxi::dead_pass(byte UCID)
{
	if (players[ UCID ].WorkAccept != 0 and abs(ClientPoints[players[ UCID ].WorkPointDestinaion].X/65536 - players[ UCID ].Info.X/65536)<5)
	{
		//удаляем маршала
		delete_marshal( UCID );
		players[ UCID ].WorkAccept = 0;
		players[ UCID ].WorkPointDestinaion = 0;
		players[ UCID ].WorkStreetDestinaion = 0;
		players[ UCID ].StressOverCount = 0;
		players[ UCID ].PassStress = 0;
		players[ UCID ].Work = 0;
		players[ UCID ].WorkNow = 0;
		send_mtc( UCID ,"^6| ^7^CТы убил своего клиента - ^1УВОЛЕН!");
	}
}

void RCTaxi::insim_mso( struct IS_MSO* packet )
{
    int i;
    if (packet->UCID == 0)
		return;

    char Message[96];
    strcpy(Message,packet->Msg + ((unsigned char)packet->TextStart));

    if (strncmp(Message, "!save", strlen("!save")) == 0){
		save_user( packet->UCID );
    }

    if (players[ packet->UCID ].InZone == 1)
    {
        if (strncmp(Message, "!deal", strlen("!deal")) == 0 )
        {
            if (dl->GetLVL( packet->UCID ) < 20)
            {
                send_mtc( packet->UCID ,"^6| ^C^7Маловат ты еще. Нужен уровень выше 20.");
                return;
            }
            /** DO SOME CODE **/
            if (players[ packet->UCID ].Work !=0)
            {
                send_mtc( packet->UCID ,"^6| ^C^7Мозги мне не канифоль! Ты уже работаешь тут.");
                return;
            }

            send_mtc( packet->UCID ,"^6| ^C^7Ты принят");
            players[ packet->UCID ].Work = 1;
        }

        if (strncmp(Message, "!undeal", strlen("!undeal")) == 0 )
        {
            if (players[ packet->UCID ].Work ==0)
            {
                send_mtc( packet->UCID ,"^6| ^C^7Слушай, А! Ты и так уже тут не работаешь.");
                return;
            }
            send_mtc( packet->UCID ,"^6| ^C^7Уходишь от нас? Ну и ступай отсюда, другого найду.");
            players[ packet->UCID ].WorkAccept = 0;
            players[ packet->UCID ].WorkPointDestinaion = 0;
            players[ packet->UCID ].WorkStreetDestinaion = 0;
            players[ packet->UCID ].StressOverCount = 0;
            players[ packet->UCID ].PassStress = 0;
            players[ packet->UCID ].WorkNow = 0;
            players[ packet->UCID ].Work = 0;
            //удаляю маршалов
            delete_marshal( packet->UCID );
        }

        if (strncmp(Message, "!workstart", strlen("!workstart")) == 0 )
        {
            if (players[ packet->UCID ].Work ==0)
            {
                send_mtc( packet->UCID ,"^6| ^C^7Эй, еще не работаешь тут, а уже рвешься кататься!");
                return;
            }
            if (players[ packet->UCID ].WorkNow ==1)
            {
                send_mtc( packet->UCID ,"^6| ^C^7Голову мне не морочь, ты и так уже на вахте.");
                return;
            }
            players[ packet->UCID ].AcceptTime = time(NULL) + PASSANGER_INTERVAL/(NumP+1);
            players[ packet->UCID ].WorkNow = 1;
            send_mtc( packet->UCID ,"^6| ^C^7Все, иди работать.");
        }

        if (strncmp(Message, "!workend", strlen("!workend")) == 0 )
        {
            if (players[ packet->UCID ].Work ==0)
            {
                send_mtc( packet->UCID ,"^6| ^C^7Эй, еще не работаешь тут, а уже увольняешься!");
                return;
            }
            if (players[ packet->UCID ].WorkNow ==0)
            {
                send_mtc( packet->UCID ,"^6| ^C^7Я тебя уже отпустил домой.");
                return;
            }
            players[ packet->UCID ].WorkNow = 0;
            send_mtc( packet->UCID ,"^6| ^C^7Сделал дело, вымой тело.");
            players[ packet->UCID ].WorkAccept = 0;
            players[ packet->UCID ].WorkPointDestinaion = 0;
            players[ packet->UCID ].WorkStreetDestinaion = 0;
            players[ packet->UCID ].StressOverCount = 0;
            players[ packet->UCID ].PassStress = 0;

            //удаляю маршалов
            delete_marshal( packet->UCID );
        }
    }

    if (strncmp(Message, "!test", strlen("!test")) == 0 )
    {

    }

    if ((strncmp(Message, "!start_points", strlen("!start_points")) == 0 ) and (strncmp(players[ packet->UCID ].UName, "denis-takumi", strlen("denis-takumi")) == 0 ))
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

    strcpy(players[ packet->UCID ].UName, packet->UName);
    strcpy(players[ packet->UCID ].PName, packet->PName);

    read_user( packet->UCID );
    NumP ++;
}

void RCTaxi::insim_npl( struct IS_NPL* packet )
{
	PLIDtoUCID[ packet->PLID ] = packet->UCID;
}

void RCTaxi::insim_plp( struct IS_PLP* packet)
{
   PLIDtoUCID.erase( packet->PLID );
}

void RCTaxi::insim_pll( struct IS_PLL* packet )
{
   PLIDtoUCID.erase( packet->PLID );
}

void RCTaxi::read_user( byte UCID )
{
    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCTaxi\\users\\%s.txt",RootDir,players[ UCID ].UName);

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        players[ UCID ].Work= 0;
        players[ UCID ].FiredPenalty= 0;
        players[ UCID ].PenaltyCount = 0;
        players[ UCID ].PassCount = 0;
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
                    players[ UCID ].Work = atoi(str+strlen("Work="));

                if (strncmp("FiredPenalty=",str,strlen("FiredPenalty="))==0)
                    players[ UCID ].FiredPenalty= atoi(str+strlen("FiredPenalty="));

                if (strncmp("PenaltyCount=",str,strlen("PenaltyCount="))==0)
                    players[ UCID ].PenaltyCount = atoi(str+strlen("PenaltyCount="));

                if (strncmp("PassCount=",str,strlen("PassCount="))==0)
                    players[ UCID ].PassCount = atoi(str+strlen("PassCount="));
            }
        }
        readf.close();
    }
    FindClose(fff);
}

void RCTaxi::delete_marshal(byte UCID)
{
    for (int i=0; i < MAX_PLAYERS; i++)
    {
        if ( UCID  == UCID)
        {
            struct IS_AXM pacAXM;
            memset(&pacAXM, 0, sizeof(struct IS_AXM));
            pacAXM.Info[0].Index=255;
            pacAXM.Info[0].Heading=ClientPoints[players[ UCID ].WorkPointDestinaion].Dir;
            pacAXM.Info[0].X=ClientPoints[players[ UCID ].WorkPointDestinaion].X/4096;
            pacAXM.Info[0].Y=ClientPoints[players[ UCID ].WorkPointDestinaion].Y/4096;
            pacAXM.Info[0].Zchar=ClientPoints[players[ UCID ].WorkPointDestinaion].Z;
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
}

void RCTaxi::save_user( byte UCID )
{
    char file[MAX_PATH];
    sprintf(file,"%sdata\\RCTaxi\\users\\%s.txt",RootDir, players[ UCID ].UName);
    ofstream writef (file,ios::out);
    writef << "Work=" << players[ UCID ].Work << endl;
    writef << "FiredPenalty=" << players[ UCID ].FiredPenalty << endl;
    writef << "PenaltyCount=" << players[ UCID ].PenaltyCount << endl;
    writef << "PassCount=" << players[ UCID ].PassCount << endl;
    writef.close();
}


void RCTaxi::taxi_done( byte UCID )
{
    send_bfn( UCID ,206);
    send_bfn( UCID ,205);
    if (players[ UCID ].PassStress <= 800)
    {
        players[ UCID ].PassCount++;
        srand ( time(NULL) );
        send_mtc( UCID , TaxiDialogs["done"][ rand()%TaxiDialogs["done"].size() ].c_str() ); // send random dialog phrase

        bank->AddCash( UCID ,(1000 - players[ UCID ].PassStress/2), true);
        dl->AddSkill( UCID );
        btn_information_clear( UCID );
    }
    else
    {
        srand ( time(NULL) );
        send_mtc( UCID , TaxiDialogs["exit"][ rand()%TaxiDialogs["exit"].size() ].c_str() ); // send random dialog phrase
    }
	players[ UCID ].AcceptTime = time(NULL) + PASSANGER_INTERVAL/(NumP+1);
    players[ UCID ].WorkAccept = 0;
    players[ UCID ].WorkPointDestinaion = 0;
    players[ UCID ].WorkStreetDestinaion = 0;
    players[ UCID ].StressOverCount = 0;
    players[ UCID ].PassStress = 0;
    players[ UCID ].InPasZone = 0;
}

void RCTaxi::insim_con( struct IS_CON* packet )
{

	byte UCIDA = PLIDtoUCID[ packet->A.PLID ];
	byte UCIDB = PLIDtoUCID[ packet->B.PLID ];

	if (players[ UCIDA ].WorkAccept == 2)
	{
		players[ UCIDA ].PassStress += 10 * packet->SpClose;

		srand ( time(NULL) );
		send_mtc( UCIDA, TaxiDialogs["con"][ rand()%TaxiDialogs["con"].size() ].c_str() ); // send random dialog phrase
	}

	if (players[ UCIDB ].WorkAccept == 2)
	{
		players[ UCIDB ].PassStress += 10 * packet->SpClose;

		srand ( time(NULL) );
		send_mtc( UCIDB ,  TaxiDialogs["con"][ rand()%TaxiDialogs["con"].size() ].c_str() ); // send random dialog phrase
	}

}


/*void RCTaxi::insim_axm( struct IS_AXM* packet )
{
    readAxm=true;
}*/


void RCTaxi::insim_obh( struct IS_OBH* packet )
{
	byte UCID = PLIDtoUCID[ packet->PLID ];
	if (players[ UCID ].WorkAccept == 2)
	{
		if((packet->Index > 45 and packet->Index < 125) or (packet->Index > 140))
		{
			players[ UCID ].PassStress +=  packet->SpClose;
			srand ( time(NULL) );
			send_mtc( UCID ,  TaxiDialogs["obh"][ rand()%TaxiDialogs["obh"].size() ].c_str() ); // send random dialog phrase
		}
		else players[ UCID ].PassStress +=  packet->SpClose/10;
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

    pack.ClickID = 206;
    pack.BStyle = 32+64;
    pack.L = 9;
    pack.T = 125;
    pack.W = 48;
    pack.H = 4;
    strcpy(pack.Text,"");

    for (int i=1; i<=players[ UCID ].PassStress/10; i++) // 1000 / 10 = 100
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
    for (int i=0; i<(100-players[ UCID ].PassStress/10); i++) strcat(pack.Text,"||");
    insim->send_packet(&pack);
}

void RCTaxi::btn_Dist( byte UCID , const char* Text)
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID =  UCID;
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
	if (players[ UCID ].Work != 0)
		return true;

    return false;
}
