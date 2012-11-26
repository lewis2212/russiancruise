using namespace std;

#include "RCTaxi.h"

RCTaxi::RCTaxi()
{

}

RCTaxi::~RCTaxi()
{

}

int RCTaxi::init(const char *dir,void *CInSim, void *Message,void *Bank,void *RCdl, void * STreet, void *Pizza)
{
	strcpy(RootDir,dir); // �������� ���� �� ���������


	insim = (CInsim *)CInSim; // ����������� ��������� ������� ������
	if(!insim) // ��������� �� �������������
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


void RCTaxi::readconfig(const char *Track)
{
	cout << "RCTaxi::readconfig\n" ;
	char file[MAX_PATH];
	sprintf(file,"%sdata\\RCTaxi\\tracks\\%s.txt",RootDir,Track);
	//cout << file << endl;

   GameTrack = Track;


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
					char * StrId;
					X = strtok (str,",");
					Y = strtok (NULL,",");
					StrId = strtok (NULL,",");

					Points[i].X = atoi(X);
					Points[i].Y = atoi(Y);
					Points[i].StreetId = atoi(StrId);

					// printf("X= %d Y= %d StrId = %d\n",Points[i].X,Points[i].Y,Points[i].StreetId);
				}

			}



		} // if strlen > 0
	} //while readf.good()

	readf.close();

	//char file[MAX_PATH];
	sprintf(file,"%sdata\\RCTaxi\\dialog.txt",RootDir);
	//cout << file << endl;


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
		//cout << str << endl;

		if (strncmp(str,"#con",5)==0)
		{
			read.getline(str,128);
			int count = atoi(str);
			DialContCount = count;

			for (int i=0 ; i<count; i++)
			{
				read.getline(str,128);
				strcpy(Dialog_Cont[i],str);
				// send_mtc(255,Dialog_Cont[i]);
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
				//send_mtc(255,Dialog_Obh[i]);
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
				// cout << Dialog_Dist[i] << endl;
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
			int DestPoint = 0;

			srand(time(NULL));
			bool ok = true;

			while (ok)
			{
				DestPoint = rand()%PointCount;
				printf("RCTaxi: test dest point == %d\n",DestPoint);
				if (Points[DestPoint].StreetId != CurStreet)
					ok = false;

				Sleep(100);
			}

			DestStreet  =  Points[DestPoint].StreetId;


			printf("RCTaxi: street == %d\n",DestStreet);

			struct streets StreetInfo;
			memset(&StreetInfo,0,sizeof(streets));

			if (street->CurentStreetInfo(&StreetInfo,players[i].UCID))
			{

				printf("RCTaxi: dest point == %d\n",DestPoint);
				players[i].WorkPointDestinaion = DestPoint;
				players[i].WorkStreetDestinaion = DestStreet;

				memset(&StreetInfo,0,sizeof(streets));
				street->CurentStreetInfoByNum(&StreetInfo,DestStreet);

				char Msg[96];
				sprintf(Msg,"^6|^C^7 �������� ������� �� %s ",StreetInfo.Street);
				btn_information(players[i].UCID,Msg);
				send_mtc(players[i].UCID,Msg);
				players[i].WorkAccept = 1;
			}

			break;

		}
	}
}

void RCTaxi::accept_user2(byte UCID)
{
	//cout << accept_time - time(&acctime) << endl;




	for (int i=0; i< 32; i++)
	{
		if (players[i].UCID == UCID)
		{
			// accept player


			int CurStreet = street->CurentStreetNum(players[i].UCID);
			int DestStreet = 0;
			int DestPoint = 0;

			srand(time(NULL));
			bool ok = true;

			while (ok)
			{
				DestPoint = rand()%PointCount;
				printf("RCTaxi: test dest point == %d\n",DestPoint);
				if (Points[DestPoint].StreetId != CurStreet)
					ok = false;

				Sleep(100);
			}

			DestStreet  =  Points[DestPoint].StreetId;


			printf("RCTaxi: street == %d\n",DestStreet);

			struct streets StreetInfo;
			memset(&StreetInfo,0,sizeof(streets));

			if (street->CurentStreetInfo(&StreetInfo,players[i].UCID))
			{

				printf("RCTaxi: dest point == %d\n",DestPoint);
				players[i].WorkPointDestinaion = DestPoint;
				players[i].WorkStreetDestinaion = DestStreet;

				memset(&StreetInfo,0,sizeof(streets));
				street->CurentStreetInfoByNum(&StreetInfo,DestStreet);

				char Msg[96];
				sprintf(Msg,"^6|^C^7 ������ ���� �� %s ",StreetInfo.Street);
				send_mtc(players[i].UCID,Msg);
				btn_information(players[i].UCID,Msg);
				players[i].WorkAccept = 2;
				players[i].PassStress = rand()%500;
			}

			break;

		}
	}
}


void RCTaxi::insim_cnl ()
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

void RCTaxi::insim_crp()
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

void RCTaxi::insim_mci ()
{
	if(!insim)return;/**dont work if insim is NULL**/
	//cout << "taxi_mci" << endl;
	float Dist;
	char d[96];
	struct IS_MCI *pack_mci = (struct IS_MCI*)insim->udp_get_packet();


	for (int i = 0; i < pack_mci->NumC; i++) // ������ �� ����� ������� pack_mci->Info[i]

	{
		for (int j =0; j < MAX_PLAYERS; j++) // ������ �� ����� ������� players[j]
		{

			if (pack_mci->Info[i].PLID == players[j].PLID) // ������ �������� ( and players[j].PLID != 0 and players[j].UCID != 0)
			{
				/** DO SOME CODE **/

				int X = pack_mci->Info[i].X/65536;
				int Y = pack_mci->Info[i].Y/65536;
				int Speed = ((int)pack_mci->Info[i].Speed*360)/(32768);

				// ���� ����� ��������� � ���� ������ �� ������, ������� ���������������� ��������� � ���
				int TX[4] = {-465,-465,-480,-480};
				int TY[4] = {450,420,420,450};
/*
				if(GameTrack == "SO4X")
				{
					TX = {-465,-465,-480,-480};
					TY = {450,420,420,450};
				}
				else if (GameTrack == "FE4X")
				{
					TX = {-20,-10,-9,0};
					TY = {469,484,472,460};
				}
				else
				{
					TX = {-465,-465,-480,-480};
					TY = {450,420,420,450};
				}
*/

				//if (Check_Pos(4,TX,TY,X,Y))

				if(Check_Pos(4,zone.dealX,zone.dealY,X,Y))
				{
					//cout << "in work place" << endl;
					if (players[j].InZone == 0)
					{
						players[j].InZone = 1;
						send_mtc(players[j].UCID,"^6| ^3Taxi Radriges");
						send_mtc(players[j].UCID,"^6| ^2!deal - ^C����������");
						send_mtc(players[j].UCID,"^6| ^2!undeal - ^C���������");
						send_mtc(players[j].UCID,"^6| ^2!workstart - ^C������ ������");
						send_mtc(players[j].UCID,"^6| ^2!workend - ^C��������� ������");
					}
				}
				else
				{
					if (players[j].InZone == 1)
						players[j].InZone = 0;
				}

				/** player drive on dest street **/
				if (players[j].WorkNow == 1 and players[j].WorkAccept != 0)
				{
					if (players[j].WorkStreetDestinaion == street->CurentStreetNum(players[j].UCID))
					{
						int des_X = Points[players[j].WorkPointDestinaion].X;
						int des_Y = Points[players[j].WorkPointDestinaion].Y;
						/** ��������� ��������� �� ����� ��������� **/
						Dist = sqrtf(pow(X-des_X,2)+pow(Y-des_Y,2));
						sprintf(d,"^7%d m.",(int)Dist);
						btn_Dist(&players[j],d);
						// ������� ������ ���������


						if (players[j].OnStreet == false)
						{
							players[j].OnStreet = true;

							char MSG[96];
							sprintf(MSG,"^6| ^C^7������������ ����� %3.0f ������.",(Dist-(int)Dist%10));
							send_mtc(players[j].UCID,MSG);

						}

						/**    **/
						if (Dist < 50)
						{
							if (players[j].WorkAccept == 2)
							{
								if (players[j].InPasZone != 1)
								{
									players[j].InPasZone = 1;
									/// NOTE: ADD PHRASE!
									srand ( time(NULL) );
									send_mtc(players[j].UCID,Dialog_Dist[rand()%DialDistCount]); // send random dialog phrase
								}
							}

							if (Speed < 5)
							{
								if (players[j].WorkAccept == 1)
									accept_user2(players[j].UCID);
								else if (players[j].WorkAccept == 2)
									taxi_done(&players[j]);
							}

						}
						else
						{
							if (players[j].WorkAccept == 2)
							{
								if (players[j].InPasZone == 1)
								{
									players[j].InPasZone = 0;
									players[j].PassStress += 10;
									/// NOTE: ADD PHRASE!
									srand ( time(NULL) );
									send_mtc(players[j].UCID,Dialog_Past[rand()%DialPastCount]); // send random dialog phrase
								}
							}
						}

					}
					else
					{
						if (players[j].OnStreet = true)
						{
							send_bfn(players[j].UCID,205);
						}
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


						players[j].Info.X = pack_mci->Info[i].X;
						players[j].Info.Y = pack_mci->Info[i].Y;
						players[j].Info.Node = pack_mci->Info[i].Node;
						players[j].Info.Direction = pack_mci->Info[i].Direction;
						players[j].Info.Heading = pack_mci->Info[i].Heading;
						players[j].Info.Speed = pack_mci->Info[i].Speed;
						players[j].Info.AngVel = pack_mci->Info[i].AngVel;

						btn_stress(&players[j]);
						sprintf(d,"^7%d m.",(int)Dist);
						btn_Dist(&players[j],d);

						if (players[j].PassStress > 800)
						{
							if (players[j].StressOverCount == 0)
							{
								srand ( time(NULL) );
								send_mtc(players[j].UCID,Dialog_Stop[rand()%DialStopCount]); // send random dialog phrase
							}

							players[j].StressOverCount ++;

							if (players[j].StressOverCount >= 20)
								players[j].StressOverCount = 0;

							if (Speed < 5)
							{
								taxi_done(&players[j]);
							}
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

void RCTaxi::insim_mso ()
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
	if (players[i].InZone == 1)
	{
		if (strncmp(Message, "!deal", strlen("!deal")) == 0 )
		{

			if (dl->GetLVL(players[i].UCID) < 20)
			{
				send_mtc(players[i].UCID,"^6| ^C^7������� �� ���. ����� ������� ���� 20.");
				return;
			}
			/** DO SOME CODE **/
			if (players[i].Work !=0)
			{
				send_mtc(players[i].UCID,"^6| ^C^7����� ��� �� ��������! �� ��� ��������� ���.");
				return;
			}

			/**time_t nt;
			int nowtime = time(&nt);
			if (players[i].FiredPenalty > nowtime)
			{
				send_mtc(players[i].UCID,"^6| ^C^7������ �� 'tut vremya'");
				return;
			}**/

			send_mtc(players[i].UCID,"^6| ^C^7�� ������");
			players[i].Work = 1;

		}

		if (strncmp(Message, "!undeal", strlen("!undeal")) == 0 )
		{
			/** DO SOME CODE **/
			if (players[i].Work ==0)
			{
				send_mtc(players[i].UCID,"^6| ^C^7������, �! �� � ��� ��� ��� �� ���������.");
				return;
			}

			send_mtc(players[i].UCID,"^6| ^C^7������� �� ���? �� � ������ ������, ������� �����.");
			players[i].Work = 0;
		}

		if (strncmp(Message, "!workstart", strlen("!workstart")) == 0 )
		{
			/** DO SOME CODE **/
			if (players[i].Work ==0)
			{
				send_mtc(players[i].UCID,"^6| ^C^7��, ��� �� ��������� ���, � ��� ������� ��������!");
				return;
			}
			if (players[i].WorkNow ==1)
			{
				send_mtc(players[i].UCID,"^6| ^C^7������ ��� �� ������, �� � ��� ��� �� �����.");
				return;
			}
			players[i].WorkNow = 1;
			send_mtc(players[i].UCID,"^6| ^C^7���, ��� ��������.");

		}

		if (strncmp(Message, "!workend", strlen("!workend")) == 0 )
		{
			/** DO SOME CODE **/
			if (players[i].Work ==0)
			{
				send_mtc(players[i].UCID,"^6| ^C^7��, ��� �� ��������� ���, � ��� ������������!");
				return;
			}
			if (players[i].WorkNow ==0)
			{
				send_mtc(players[i].UCID,"^6| ^C^7� ���� ��� �������� �����.");
				return;
			}
			players[i].WorkNow = 0;
			send_mtc(players[i].UCID,"^6| ^C^7������ ����, ����� ����.");

		}
	}

	if ((strncmp(Message, "!start_points", strlen("!start_points")) == 0 ) and (strncmp(players[i].UName, "denis-takumi", strlen("denis-takumi")) == 0 ))
	{

		if (StartPointsAdd ==0)
		{
			StartPointsAdd =1;
			PointCount = 0;
			for (int f=0; f<200; f++)
			{
				PointsAdd[f].Id = 0;
			}
		}
		else
		{
			StartPointsAdd =0;

			ofstream readf("PoInTs.txt",ios::out);
			readf << PointCount << endl;
			for (int f=0; f<200; f++)
			{
				if (PointsAdd[f].Id != 0)
				{
					readf << PointsAdd[f].X << "," << PointsAdd[f].Y << "," << PointsAdd[f].StreetId << endl;
				}
			}
		}

	}

}


void RCTaxi::insim_ncn()
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

void RCTaxi::insim_npl()
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

void RCTaxi::insim_plp()
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

void RCTaxi::insim_pll()
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



void RCTaxi::taxi_done(TaxiPlayer *splayer)
{
	//send_mtc(splayer->UCID,"^C^7������ ���������");
	send_bfn(splayer->UCID,206);
	send_bfn(splayer->UCID,205);
	//NOTE: ADD SLOVA
	if (splayer->PassStress <= 800)
	{
		splayer->PassCount++;
		srand ( time(NULL) );
		send_mtc(splayer->UCID,Dialog_Done[rand()%DialDoneCount]); // send random dialog phrase

		char Text[128];
		sprintf(Text,"^6|^C^7 �� ���������� %d ^2RUR.\n",(1000 - splayer->PassStress/2));
		send_mtc(splayer->UCID,Text);
		bank->AddCash(splayer->UCID,(1000 - splayer->PassStress/2));
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

void RCTaxi::insim_con()
{
	// printf("Car contact\n");


	struct IS_CON *pack_con = (struct IS_CON*)insim->get_packet();

	for (int i=0; i<MAX_PLAYERS; i++)
	{
		if (players[i].PLID == pack_con->A.PLID)
		{
			if (players[i].WorkAccept == 2)
			{
				players[i].PassStress += 10 * pack_con->SpClose;
				//NOTE: ADD SLOVA
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
				//NOTE: ADD SLOVA
				srand ( time(NULL) );
				send_mtc(players[j].UCID,Dialog_Cont[rand()%DialContCount]); // send random dialog phrase
			}
			break;
		}
	}



}

void RCTaxi::insim_obh()
{


	struct IS_OBH *pack_obh = (struct IS_OBH*)insim->get_packet();


	for (int i=0; i<MAX_PLAYERS; i++)
	{
		if (players[i].PLID == pack_obh->PLID)
		{
			if (players[i].WorkAccept == 2)
			{
				if((pack_obh->Index > 45 and pack_obh->Index < 125) or (pack_obh->Index > 140))
				{
					players[i].PassStress +=  pack_obh->SpClose;
					//NOTE: ADD SLOVA
					srand ( time(NULL) );
					//cout << rand()%DialObhCount << endl;
					send_mtc(players[i].UCID,Dialog_Obh[rand()%DialObhCount]); // send random dialog phrase
				}
				else
				{
					players[i].PassStress +=  pack_obh->SpClose/10;
				}

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

	//strcat(pack.Text,msg->GetMessage[0][100]);

	//
	pack.ClickID = 206;
	pack.BStyle = 32+64;
	pack.L = 1;
	pack.T = 125;
	pack.W = 48;
	pack.H = 4;
	//int life = 100;
	strcpy(pack.Text,"");

	//cout << "nrg = " << splayer->PassStress/10 << endl;

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
	for (int i=0; i<(100-splayer->PassStress/10); i++)
	{
		strcat(pack.Text,"||");
	}

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

	//strcat(pack.Text,msg->GetMessage[0][100]);

	//
	pack.ClickID = 205;
	pack.BStyle = 32+64;
	pack.L = 50;
	pack.T = 125;
	pack.W = 8;
	pack.H = 4;
	//int life = 100;
	strcpy(pack.Text,Text);

	insim->send_packet(&pack);
}

bool RCTaxi::IfWork (byte UCID)
{
	for (int i=0; i < MAX_PLAYERS; i++)
	{
		if (players[i].UCID ==UCID)
		{
			//cout << "Msg: " << pack_mso->Msg << endl;
			if (players[i].Work != 0)
				return true;

			break;
		}
	}
	return false;
}
