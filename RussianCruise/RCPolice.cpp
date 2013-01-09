


/**
void save_user_fines (struct player *splayer)
{
	out <<splayer->UName << " save fines_info" << endl;

	char file[255];
	strcpy(file,RootDir);
	strcat(file,"data\\RCPolice\\fines\\");
	strcat(file,splayer->UName);
	strcat(file,".txt");

	ofstream writef (file,ios::out);
	for (int i = 0; i < MAX_FINES; i++)
	{
		if (splayer->fines[i].fine_id > 0)
		{
			writef << splayer->fines[i].fine_id << ";" << splayer->fines[i].fine_date <<  endl;
		}
	}

	writef.close();
}

void read_user_fines(struct player *splayer)
{
	char file[255];
	strcpy(file,RootDir);
	strcat(file,"data\\RCPolice\\fines\\");
	strcat(file,splayer->UName);
	strcat(file,".txt");

	HANDLE fff;
	WIN32_FIND_DATA fd;
	fff = FindFirstFile(file,&fd);
	if (fff == INVALID_HANDLE_VALUE)
	{
		out << "Can't find " << file << endl;
		save_user_fines(splayer);
	}
	else
	{
		ifstream readf (file,ios::in);
		int i=0;
		while (readf.good())
		{
			char str[128];
			readf.getline(str,128);

			if (strlen(str) > 0)
			{
				char *id;
				char *date;


				id = strtok(str,";");
				date = strtok(NULL,";");


				splayer->fines[i].fine_id = atoi(id);
				splayer->fines[i].fine_date = atoi(date);

				i++;
			}
		}
	}




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
	insim.send_packet(&pack);
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
	insim.send_packet(&pack);
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
	pack.L = 148;
	pack.T = 1;
	pack.W = 10;
	pack.H = 8;

	char min_c[3];
	char sec_c[3];

	int time2 = splayer->WorkTime - time(&stime);
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
	insim.send_packet(&pack);

}

void read_fines()
{

	char file[255];
	strcpy(file,RootDir);
	//strcat(file,"misc\\mysql.cfg");
	strcat(file,"misc\\fines.txt");

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

	int i = 0;
	while (readf.good())
	{
		char str[128];
		readf.getline(str,128);
		if (strlen(str) > 0)
		{

			if (strstr(str,"//")) {}
			else
			{
				char * id;
				char * name;
				char * cash;
				id = strtok (str,";");
				name = strtok (NULL,";");
				cash = strtok (NULL,";");

				i = atoi(id);
				memset(&ginfo.fines[i],0,sizeof(struct cars));
				ginfo.fines[i].id = i;
				strcpy(ginfo.fines[i].name, name);
				ginfo.fines[i].cash = atoi(cash);

			}


		} // if strlen > 0
	} //while readf.good()

	readf.close();
}

int read_cop(struct player *splayer)
{
	char file[255];
	strcpy(file,RootDir);
	strcat(file,"misc\\cops.txt");
	//strcat(file,"tracks\\.txt");

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
		if (strlen(str) > 0)
		{
			if (strncmp(splayer->UName,str,strlen(splayer->UName))==0)
			{
				cop ++;
			}
		}
	}


	readf.close();

	return cop;
}
void case_mci_cop ()
{
	struct IS_MCI *pack_mci = (struct IS_MCI*)insim.udp_get_packet();


	for (int i = 0; i < pack_mci->NumC; i++)

	{
		for (int j =0; j < MAX_PLAYERS; j++)
		{
			if (pack_mci->Info[i].PLID == ginfo.players[j].PLID and ginfo.players[j].PLID != 0 and ginfo.players[j].UCID != 0)

			{
				int S = ginfo.players[j].Info.Speed*360/32768;
				if (S < 5)
					ginfo.players[j].StopTime ++;
				else
				{
					ginfo.players[j].StopTime = 0;
					if (ginfo.players[j].radar ==1 )
					{
						send_mtc(ginfo.players[j].UCID,msg.message[ginfo.players[j].lang_id][1700] );
						ginfo.players[j].radar = 0;
					}
				}



				if (ginfo.players[j].cop == 1)
				{
					btn_svetofor3(&ginfo.players[j]);

					for (int g =0; g < MAX_PLAYERS; g++)
					{
						if (ginfo.players[g].PLID !=0)
						{
							// int dN = abs(ginfo.players[g].Info.Node - ginfo.players[j].Info.Node);
							int dX = abs((ginfo.players[g].Info.X/65536) - (ginfo.players[j].Info.X/65536));
							int dY = abs((ginfo.players[g].Info.Y/65536) - (ginfo.players[j].Info.Y/65536));

							int Rast = (int)sqrt((pow(dX,2))+(pow(dY,2)));

							//printf("dX=%d ; dY=%d \n",dX,dY);
							if (ginfo.players[g].Pogonya == 1)
							{



								//printf("COP - %s CRIM - %s dX=%d dY=%d; \n",ginfo.players[j].UName,ginfo.players[g].UName,dX,dY);

								if ( (Rast < 10) and (ginfo.players[g].cop != 1))
								{
									int S2 = ginfo.players[g].Info.Speed*360/32768;

									if ((S2 < 5) and (ginfo.players[g].StopTime > 6))
									{
										//send_mst("/rcm ^C^1ВЫ АРЕСТОВАНЫ");
										ginfo.players[g].Pogonya = 2;
										strcpy(ginfo.players[g].PogonyaReason,msg.message[ginfo.players[g].lang_id][1701]);
										char Text[96];
										strcpy(Text,"/msg ^2| ");
										strcat(Text,ginfo.players[g].PName);
										strcat(Text,msg.message[ginfo.players[j].lang_id][1702]);
										//strcat(Text,ginfo.players[g].PName);
										send_mst(Text);

										send_mtc(ginfo.players[j].UCID,msg.message[ginfo.players[j].lang_id][1703]);
									}

								}
							} // pogonya



							if (ginfo.players[j].radar ==1 )
							{
								if ((Rast < 50 ) and (ginfo.players[g].cop != 1))
								{
									//printf("COP - %s CRIM - %s dN=%d \n",ginfo.players[j].UName,ginfo.players[g].UName,dN);
									// player[g] in radar zone of player[j]
									int Speed = ginfo.players[g].Info.Speed*360/32768;
									if ((Speed > ginfo.players[j].street[ginfo.players[g].StreetNum].SpeedLimit+10) )
									{
										// speed > SpeedLimit + 10
										char text[64];
										strcpy(text,"^2| ");
										strcat(text,ginfo.players[g].PName);
										strcat(text,msg.message[ginfo.players[g].lang_id][1704]);
										char speed[3];
										int Speed2 = Speed - ginfo.players[j].street[ginfo.players[g].StreetNum].SpeedLimit;
										itoa(Speed2,speed,10);
										strcat(text,speed);
										strcat(text,msg.message[ginfo.players[j].lang_id][1705]);
										send_mtc(ginfo.players[j].UCID,text);
									}
								}
							}

							if (ginfo.players[j].sirena ==1)
							{
								if ((dX < 120) and (dY < 120 ) and (ginfo.players[g].cop != 1))
								{
									ginfo.players[g].sirenaOnOff = 1;
									ginfo.players[g].sirenaKey = 1;
									ginfo.players[g].sirenaSize = Rast;
								}
								else
								{
									ginfo.players[g].sirenaOnOff = 0;
								}
							}
							else
							{
								ginfo.players[g].sirenaOnOff = 0;
							}

						}
					}
				}


				if ((ginfo.players[j].sirenaOnOff == 0) and (ginfo.players[j].sirenaKey == 1))
				{
					ginfo.players[j].sirenaKey = 0;
					send_bfn(ginfo.players[j].UCID,203);
				}

				if ( ginfo.players[j].sirenaOnOff == 1)
					btn_sirena(&ginfo.players[j]);

				if ((ginfo.players[j].Pogonya == 0) and (strlen(ginfo.players[j].PogonyaReason) > 1))
				{
					strcpy(ginfo.players[j].PogonyaReason,"");
					send_bfn(ginfo.players[j].UCID,204);
				}

				if ( ginfo.players[j].Pogonya != 0)
					btn_pogonya(&ginfo.players[j]);


			} // if CompCar->PLID == PLID
		}
	}
}

void case_mso_cop ()
{
	int i;

	struct IS_MSO *packet = (struct IS_MSO*)insim.get_packet();

	// The chat message is sent by the host, don't do anything
	if (packet->UCID == 0)
	{
		// out << "(Chat message by host: " << packet->Msg + ((unsigned char)packet->TextStart) << ")" << endl;
		return;

	}

	// Find the player that wrote in the chat
	for (i=0; i < MAX_PLAYERS; i++)
	{
		if (ginfo.players[i].UCID == packet->UCID)
		{
			//  out << "Msg: " << packet->Msg << endl;
			break;
		}
	}
	char Msg[96];
	strcpy(Msg,packet->Msg + ((unsigned char)packet->TextStart));

	if ((strncmp(Msg, "!sirena", 7) == 0 ) or (strncmp(Msg, "!^Cсирена", 9) == 0 ))
	{
		out << ginfo.players[i].UName << " send !sirena" << endl;

		if (ginfo.players[i].cop == 1)
		{
			if (ginfo.players[i].sirena ==0 )
			{
				send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2100]);
				ginfo.players[i].sirena = 1;
			}
			else
			{
				send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2101]);
				ginfo.players[i].sirena = 0;
			}
		}
	}

	if ((strncmp(Msg, "!radar", 6) == 0 ) or (strncmp(Msg, "!^Cрадар", 8) == 0 ))
	{
		out << ginfo.players[i].UName << " send !radar" << endl;

		if (ginfo.players[i].cop == 1)
		{
			if (ginfo.players[i].radar ==0 )
			{
				send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2102]);
				ginfo.players[i].radar = 1;
			}
			else
			{
				send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2103]);
				ginfo.players[i].radar = 0;
			}
		}
	}

	if ((strncmp(Msg, "!fines", 6) == 0) or (strncmp(Msg, "!^Cштрафы", 9) == 0 ))
	{
		out << ginfo.players[i].UName << " send !fines" << endl;
		help_cmds(&ginfo.players[i],3);
	}

	if ((strncmp(Msg, "!pay", 4) == 0 ) or (strncmp(Msg, "!^Cоплатить", 11) == 0 ))
	{
		out << ginfo.players[i].UName << " send !pay" << endl;

		char message2[96];
		strcpy(message2,Msg);

		//if (strlen(message2) < 6)
		//{
		//    send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2104]);
		//    return;
		//}

		char * comand;
		char * id;

		comand = strtok (message2," ");
		id = strtok (NULL," ");


		int id_i = atoi(id);

		if ((!id) or (id_i < 1))
		{
			send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2105]);
			return;
		}
		int j=0;
		for (j=0; j<MAX_FINES; j++)
		{
			if (ginfo.players[i].fines[j].fine_id == id_i)
			{
				ginfo.players[i].fines[j].fine_id = 0;
				ginfo.players[i].fines[j].fine_date = 0;
				bank.players[i].Cash -= ginfo.fines[id_i].cash;
				bank.BankFond += ginfo.fines[id_i].cash;
				send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2106]);
				break;
			}
			if (j == MAX_FINES)
			{
				send_mtc(ginfo.players[i].UCID,msg.message[ginfo.players[i].lang_id][2107]);
			}
		}

	}

}

void RCCore::case_cpr ()
{
	int i;



	// Find player and set his PLID to 0
	for (i=0; i < MAX_PLAYERS; i++)
	{
		if (players[i].UCID == packet->UCID)
		{
			cout << players[i].PName << " rename to " << packet->PName << endl;
			strcpy(players[i].PName, packet->PName);
			// убираем ЛФС коды ^1 ^C ^L ^7 и т.д.
			char PlayerName[32];
			strcpy(PlayerName,packet->PName);
			char * pch;
			while (pch = strstr(PlayerName,"^"))
			{
				int point = strlen(pch);
				//cout << pch << endl;
				strcpy(PlayerName+strlen(PlayerName)-point,pch+2);
			}
			//cout << PlayerName << endl;
			char Text[64];
			strcpy(Text, "/spec ");
			strcat (Text, players[i].UName);
			if (players[i].PLID != 0)
			{
				if (((strncmp("[ДПС]",PlayerName,5)==0) or (strncmp("[дпс]",PlayerName,5)==0)) and (players[i].cop !=1))
				{
					if ( read_cop(&players[i]) != 1)
					{
						send_mtc(players[i].UCID,msg->message[players[i].lang_id][1300]);
						//players[i].PLID =0;
						send_mst(Text);
					}
					else
					{
						players[i].cop =1;
						send_mtc(players[i].UCID,msg->message[players[i].lang_id][1301]);
					}
				}
				else
				{
					if (players[i].cop == 1)
					{
						players[i].cop =0;
						send_mtc(players[i].UCID,msg->message[players[i].lang_id][1302]);

						int tune = 45;
						if (players[i].CTune&1)
							tune -= 15;
						if (players[i].CTune&2)
							tune -= 20;
						if (players[i].H_TRes < tune)
						{
							players[i].PLID = 0;
							players[i].Zone = 1;
							send_mst(Text);
							char QSL [8];
							itoa(tune,QSL,10);
							char Texxt[32];
							strcpy(Texxt,msg->message[players[i].lang_id][2400] );
							strcat(Texxt,QSL);
							strcat(Texxt," %");
							send_mtc(players[i].UCID,Texxt);
						}
						else if ((players[i].SetF&4) and !(players[i].CTune&8))
						{
							char Texxt[32];
							strcpy(Texxt,msg->message[players[i].lang_id][2401] );
							send_mtc(players[i].UCID,Texxt);
							players[i].Zone = 1;
							players[i].PLID = 0;
							char Text[64];
							strcpy(Text, "/spec ");
							strcat (Text, players[i].UName);
							send_mst(Text);
							return;
						}
					}
				}
			}

			break;
		}
	}
}


void RCCore::case_pen ()
{
	int i;

	struct IS_PEN *pack_pen = (struct IS_PEN*)insim->get_packet();

	// Find player and set his PLID to 0
	for (i=0; i < MAX_PLAYERS; i++)
	{
		if (players[i].PLID == pack_pen->PLID)
		{
			if((pack_pen->NewPen != 0) and (pack_pen->Reason == PENR_SPEEDING))
			{
				players[i].Penalty = 1;
				for (int j=0; j<MAX_FINES; j++)
				{
					if (players[i].fines[j].fine_id == 0)
					{
						players[i].fines[j].fine_id = 13;
						players[i].fines[j].fine_date = int(time(&stime));

						char Msg[64];
						strcpy(Msg,msg->message[players[i].lang_id][1104]);
						send_mtc(players[i].UCID,Msg);
						strcpy(Msg,"^2| ^7");
						strcat(Msg,ginfo.fines[13].name);
						send_mtc(players[i].UCID,Msg);
						break;
					}
				}
			}
			break;
		}
	}
}


void RCCore::case_pla ()
{
	int i;

	struct IS_PLA *pack_pla = (struct IS_PLA*)insim->get_packet();

	// Find player and set his PLID to 0
	for (i=0; i < MAX_PLAYERS; i++)
	{
		if (players[i].PLID == pack_pla->PLID)
		{
			if (pack_pla->Fact == PITLANE_EXIT)
			{
				if (players[i].Penalty != 0)
				{
					char Text[64];
					strcpy(Text, "/p_clear ");
					strcat (Text, players[i].UName);
					send_mst(Text);
				}
				int count = 0;
				for (int j=0; j<MAX_FINES; j++)
				{
					if (players[i].fines[j].fine_id != 0)
					{
						count++;
					}
				}

				if (count > 10)
				{
					players[i].PLID = 0;
					char Text[64];
					strcpy(Text, "/spec ");
					strcat (Text, players[i].UName);
					send_mtc(players[i].UCID,msg->message[players[i].lang_id][3400]);
					send_mst(Text);
				}

			}
			break;
		}
	}
}


**/
