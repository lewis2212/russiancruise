using namespace std;

#include "RCCore.h"


RCCore::RCCore()
{

}

RCCore::~RCCore()
{

}

int RCCore::init(char *dir, void *CInSim)
{
	strcpy(RootDir, dir);

	insim = (CInsim *)CInSim;
	if (!insim)
	{
		printf ("Can't struct CInsim class");
		return -1;
	}

   /* msg = (RCMessage *)Message;
	if (!msg)
	{
		printf ("Can't struct RCMessage class");
		return -1;
	}
	bank = (RCBank *)Bank;
	if (!msg)
	{
		printf ("Can't struct RCBank class");
		return -1;
	}
	nrg = (RCEnergy *)Energy;
	if (!msg)
	{
		printf ("Can't struct RCEnergy class");
		return -1;
	}

	ginfo_time = time(&ptime)+60;
*/
	return 0;
}


void RCCore::help_cmds (struct player *splayer, int h_type)
{
	if (h_type == 1)
	{


		for ( int i = 3200 ; i <= 3214 ; i ++ )
			send_mtc(splayer->UCID, msg->message[splayer->lang_id][i]);


	}
   /** if (h_type == 2)
	{

		send_mtc(splayer->UCID, msg->message[splayer->lang_id][3100]);
		for ( int i=0; i<MAX_CARS; i++)
		{
			if (strlen(splayer->cars[i].car)>0)
			{
				//int     id;
				char    car[4];
				int     tune;
				char    dist[16];
				char    Text[64];

				strcpy(car , splayer->cars[i].car);
				tune = splayer->cars[i].tuning;
				itoa(splayer->cars[i].dist, dist, 10);


				strcpy( Text, "^1| ^C^2");
				strcat( Text, car);
				strcat( Text, " ^3");
				int dist_i = atoi(dist)/1000;
				char dist_c[12];
				itoa(dist_i, dist_c, 10);
				strcat(Text, dist_c);
				strcat( Text, "^7 km (");

				int tune2 = 45;
				if (tune&1)
				{
					strcat( Text, "^2E");
					tune2 -= 15;
				}
				else
				{
					strcat( Text, "^1E");
				}

				if (tune&2)
				{
					strcat( Text, " ^2T");
					tune2 -= 20;
				}
				else
				{
					strcat( Text, " ^1T");
				}

				if (tune&8)
					strcat( Text, " ^2A");
				else
				{
					strcat( Text, " ^1A");
				}

				strcat( Text, "^7)(^3");

				char intake[16];
				itoa(tune2, intake, 10);
				strcat( Text, intake);
				strcat( Text, "%^7)");
				send_mtc(splayer->UCID, Text);
			}
		}

	}
	if (h_type == 3)
	{

		int i=0;
		int j=0;
		for (i=0; i<MAX_FINES; i++)
		{
			if (splayer->fines[i].fine_id > 0)
			{


				char Text[64];
				// row[0] = username
				// row[1] = fine_id
				// row[2] = fine_date

				int fine_id = splayer->fines[i].fine_id;
				// int fine_date = atoi(row[2]);
				char num[3];
				itoa(fine_id, num, 10);

				char pay[5];
				itoa(ginfo.fines[fine_id].cash, pay, 10);

				strcpy(Text, "^2| ^7ID = ");
				strcat( Text, num);
				strcat( Text, ". ");
				strncat( Text, ginfo.fines[fine_id].name, 64-34);
				strcat( Text, " ^3(^2");
				strcat( Text, pay);
				strcat( Text, " RUR.^3)");
				send_mtc(splayer->UCID, Text);

				j++;
			}
		}

		if (j == 0)
		{
			send_mtc(splayer->UCID, msg->message[splayer->lang_id][3102]);
		}

	}
**/
}


/**** buttons ****/
void RCCore::btn_main (struct player *splayer)
{
	struct IS_BTN pack;
	memset(&pack, 0, sizeof(struct IS_BTN));
	pack.Size = sizeof(struct IS_BTN);
	pack.Type = ISP_BTN;
	pack.ReqI = 1;
	pack.UCID = splayer->UCID;
	pack.Inst = 0;
	pack.TypeIn = 0;

	/* background */
	pack.BStyle = 16+64;
	pack.ClickID = 100;
	pack.L = 100-126/2;
	pack.T = 25;
	pack.W = 126;
	pack.H = 150;
	strcpy(pack.Text, "");
	insim->send_packet(&pack);
	Sleep(100);
	/* background */

	/* Title */
	pack.BStyle = 0;
	pack.ClickID = 101;
	pack.L = 101-126/2;
	pack.T = 26;
	pack.W = 126;
	pack.H = 20;
	strcpy(pack.Text, "^7RUSSIAN CRUISE");
	insim->send_packet(&pack);
	Sleep(100);
	/* Title */

	/* background #2 */
	pack.BStyle = 16+32;
	pack.ClickID = 102;
	pack.L = 101-126/2;
	pack.T = 55;
	pack.W = 124;
	pack.H = 118;
	strcpy(pack.Text, "");
	insim->send_packet(&pack);
	Sleep(100);

	/* background #2 */

}
/**
void RCCore::btn_info (struct player *splayer, int b_type)
{
	btn_main(splayer);

	char info_text[5][100];
	strncpy(info_text[0], "^7RUSSIAN CRUISE", 99);
	strncpy(info_text[1], "^C^7 Добро пожаловать, ", 99);
	strncpy(info_text[2], "^C^7 Деньги: ^2", 99);
	strncpy(info_text[3], "^C^7   тут жизненный статус", 99);
	strncpy(info_text[4], "^C^7Доступные автомобили:", 99);




	char abcout_text[10][100];
	strncpy(abcout_text[0], "^7RUSSIAN CRUISE v 1.1.9", 99);
	strncpy(abcout_text[1], "^C^7Developer: Kostin Denis", 99);
	strncpy(abcout_text[2], "^C^7ICQ: 5518182", 99);
	strncpy(abcout_text[3], "^C^7Skype: denisko_leva", 99);
	strncpy(abcout_text[4], "^7", 99);
	strncpy(abcout_text[5], "^C^7More information", 99);
	strncpy(abcout_text[6], "^C^7http://russiancruise.ucoz.ru/", 99);
	strncpy(abcout_text[7], "^7", 99);
	strncpy(abcout_text[8], "^C^7        Thanks:", 99);
	strncpy(abcout_text[9], "^C^3        repeat, NOSE, etc", 99);

	struct IS_BTN pack;
	memset(&pack, 0, sizeof(struct IS_BTN));
	pack.Size = sizeof(struct IS_BTN);
	pack.Type = ISP_BTN;
	pack.ReqI = 1;
	pack.UCID = splayer->UCID;
	pack.Inst = 0;
	pack.TypeIn = 0;


	pack.BStyle = 32+8;
	pack.ClickID = 103;
	pack.L = 101-126/2;
	pack.T = 46;
	pack.W = 16;
	pack.H = 8;
	strcpy(pack.Text, msg->message[splayer->lang_id][200]);
	insim->send_packet(&pack);
	Sleep(100);

	pack.BStyle = 32+8;
	pack.ClickID = 106;
	pack.L += 17;
	strcpy(pack.Text, msg->message[splayer->lang_id][201]);
	insim->send_packet(&pack);
	Sleep(100);


	pack.BStyle = 32+8;
	pack.ClickID = 105;
	pack.L += 17;
	pack.W = 20;
	strcpy(pack.Text, msg->message[splayer->lang_id][202]);
	insim->send_packet(&pack);
	Sleep(100);




	if (b_type == 1)
	{
		for (int i=2; i<9; i++)
		{
			pack.L = (101-126/2)+1;
			pack.BStyle = 16;
			pack.T = 46+9*(i-1)+1;
			pack.W = 50;
			pack.H = 8;
			pack.ClickID = 110 + i;
			char cash[6];
			char sell[6];
			itoa(ginfo.car[i].cash, cash, 10);
			itoa(ginfo.car[i].sell, sell, 10);
			strcpy(pack.Text, "^2");
			strcat(pack.Text, ginfo.car[i].car);
			strcat(pack.Text, " ^7(^2");
			strcat(pack.Text, cash);
			strcat(pack.Text, "^7/^3");
			strcat(pack.Text, sell);
			strcat(pack.Text, "^7)");
			insim->send_packet(&pack);
		}

		for (int i=11; i<19; i++)
		{
			pack.L = (101-126/2)+1+51;
			pack.BStyle = 16;
			pack.T = 46+9*(i-10)+1;
			pack.W = 50;
			pack.H = 8;
			pack.ClickID = 110 + i;
			char cash[6];
			char sell[6];
			itoa(ginfo.car[i].cash, cash, 10);
			itoa(ginfo.car[i].sell, sell, 10);

			strcpy(pack.Text, "^2");
			strcat(pack.Text, ginfo.car[i].car);
			strcat(pack.Text, " ^7(^2");
			strcat(pack.Text, cash);
			strcat(pack.Text, "^7/^3");
			strcat(pack.Text, sell);
			strcat(pack.Text, "^7)");
			insim->send_packet(&pack);
		}

	} // if (type == 1)

	if (b_type == 2)
	{


	}

	if (b_type == 3)
	{
		pack.BStyle = 64;
		pack.L = 101-126/2;
		pack.W = 90;
		for (int g=0; g<10; g++)
		{
			pack.ClickID = 110 + g;
			pack.T += 9;
			strcpy(pack.Text, abcout_text[g]);
			insim->send_packet(&pack);
			Sleep(100);
		}
	}

	if (b_type == 4)
	{
		for (int i=1; i<=13; i++)
		{
			pack.L = (101-126/2)+1;
			pack.BStyle = 16 + 64;
			pack.T = 56+7*(i-1);
			pack.W = 122;
			pack.H = 6;
			pack.ClickID = 110 + i;

			// row[0] = username
			// row[1] = fine_id
			// row[2] = fine_date
			// int fine_date = atoi(row[2]);
			char num[3];
			itoa(ginfo.fines[i].id, num, 10);

			char pay[5];
			itoa(ginfo.fines[i].cash, pay, 10);

			strcpy(pack.Text, "^2| ^7ID = ");
			strcat( pack.Text, num);
			strcat( pack.Text, ". ");
			strncat( pack.Text, ginfo.fines[i].name, 96-34);
			strcat( pack.Text, " ^3(^2");
			strcat( pack.Text, pay);
			strcat( pack.Text, " RUR.^3)");
			insim->send_packet(&pack);
		}

	}

	pack.BStyle = 8;
	pack.ClickID = 149;
	pack.L = 99+126/2-8;
	pack.T = 26;
	pack.W = 8;
	pack.H = 8;
	strcpy(pack.Text, "^1[X]");
	insim->send_packet(&pack);
}


**/


void RCCore::btn_panel (struct player *splayer)
{
	struct IS_BTN pack;
	memset(&pack, 0, sizeof(struct IS_BTN));
	pack.Size = sizeof(struct IS_BTN);
	pack.Type = ISP_BTN;
	pack.ReqI = 1;
	pack.UCID = splayer->UCID;
	pack.Inst = 0;
	pack.TypeIn = 0;
	pack.BStyle = 32;
	pack.ClickID = 160;
	pack.L = 115;
	pack.T = 1;
	pack.W = 33;
	pack.H = 4;
	strcpy(pack.Text, splayer->street[splayer->StreetNum].Street);
	strcat(pack.Text, " ^7(^1");
	char speed[3];
	itoa(splayer->street[splayer->StreetNum].SpeedLimit, speed, 10);
	strcat(pack.Text, speed);
	strcat(pack.Text, " ^C^3km/h^7)");
	insim->send_packet(&pack);

	pack.ClickID = 161;
	pack.L = 100;
	pack.T = 1;
	pack.W = 15;
	pack.H = 4;

	if (splayer->Zone== 1)
		strcpy(pack.Text, msg->message[splayer->lang_id][400]);
	else if (splayer->Zone== 2)
		strcpy(pack.Text, msg->message[splayer->lang_id][401]);
	else if (splayer->Zone== 3)
		strcpy(pack.Text, msg->message[splayer->lang_id][402]);
	else
		strcpy(pack.Text, msg->message[splayer->lang_id][403]);
	insim->send_packet(&pack);

}






/****************************************/
void RCCore::case_bfn ()
{
	struct IS_BFN *pack_bfn = (struct IS_BFN*)insim->get_packet();

	int i;
	for (i=0; i < MAX_PLAYERS; i++)
	{
		if (players[i].UCID == pack_bfn->UCID)
		{
			if (players[i].bfn == 0)
			{
				players[i].bfn = 1;
			   // btn_info(&players[i], 1);
			}
			else
			{
				players[i].bfn = 0;
			}
			break;
		}
	}
}

/**
void RCCore::case_btt ()
{

	struct IS_BTT *pack_btt = (struct IS_BTT*)insim->get_packet();

	//cout << (int)pack_btt->ClickID << endl;
	char day[3];
	char month[3];
	char year[3];
	SYSTEMTIME sm;
	GetLocalTime(&sm);
	itoa(sm.wDay, day, 10);
	itoa(sm.wMonth, month, 10);
	itoa(sm.wYear, year, 10);

	char send_c[255];
	strcpy(send_c, RootDir);
	strcat(send_c, "logs\\sends\\send.txt");

	char fine_c[255];
	strcpy(fine_c, RootDir);
	strcat(fine_c, "logs\\fines\\fine");
	strcat(fine_c, "(");
	strcat(fine_c, day);
	strcat(fine_c, ".");
	strcat(fine_c, month);
	strcat(fine_c, ".");
	strcat(fine_c, year);
	strcat(fine_c, ").txt");


	int i;
	for (i=0; i < MAX_PLAYERS; i++)
	{
		if (players[i].UCID == pack_btt->UCID)
		{
			if (pack_btt->ClickID==36) // send cash
			{

				for (int g=0; g<MAX_PLAYERS; g++)
				{
					if  (players[i].BID2 == players[g].BID)
					{
						if (atoi(pack_btt->Text) > 0)
						{
							if (bank->players[i].Cash > atoi(pack_btt->Text))
							{
								cout << players[i].UName << " send " << pack_btt->Text << " to "  << players[g].UName << endl;
								bank->players[i].Cash -= atoi(pack_btt->Text);
								bank->players[g].Cash += atoi(pack_btt->Text);

								char money[96];
								itoa(atoi(pack_btt->Text), money, 10);
								char Msg[64];
								strcpy(Msg, "^1| ");
								strcat(Msg, players[i].PName);
								strcat(Msg, msg->message[players[i].lang_id][1100]);
								strcat(Msg, money);
								strcat(Msg, " RUR.");
								send_mtc(players[g].UCID, Msg);

								ofstream readf (send_c, ios::app);
								readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << ":" << sm.wMilliseconds << " " <<  players[i].UName << " send " << pack_btt->Text << " RUR. to "  << players[g].UName << endl;
								readf.close();
							}
							else
							{
								send_mtc(players[i].UCID, msg->message[players[i].lang_id][1101]);
								send_mtc(players[i].UCID, msg->message[players[i].lang_id][1102]);
							}
						} // if atoi(pack_btt->Text) > 0
						break;
					}
				}//for

			}

			else if (pack_btt->ClickID == 37) // send privat message
			{

				for (int g=0; g<MAX_PLAYERS; g++)
				{
					if  (players[i].BID2 == players[g].BID)
					{
						if (strlen(pack_btt->Text) > 0)
						{
							cout << players[i].UName << " send " << pack_btt->Text << " to "  << players[g].UName << endl;
							char Msg[64];
							strcpy(Msg, msg->message[players[g].lang_id][1103]);
							strcat(Msg, players[i].PName);
							strcat(Msg, ": ");
							strcat(Msg, pack_btt->Text);
							send_mtc(players[g].UCID, Msg);
						} // if atoi(pack_btt->Text) > 0
						break;
					}
				}//for

			}
			else if (pack_btt->ClickID==38)
			{

				for (int g=0; g<MAX_PLAYERS; g++)
				{
					if  (players[i].BID2 == players[g].BID)
					{
						if (atoi(pack_btt->Text) > 0)
						{

							cout << players[i].UName << " send fine id = " << pack_btt->Text << " to "  << players[g].UName << endl;

							char Msg[64];
							strcpy(Msg, msg->message[players[g].lang_id][1104]);
							send_mtc(players[g].UCID, Msg);
							strcpy(Msg, "^2| ^7");
							strcat(Msg, ginfo.fines[atoi(pack_btt->Text)].name);
							send_mtc(players[g].UCID, Msg);

							strcpy(Msg, msg->message[players[i].lang_id][1105]);
							send_mtc(players[i].UCID, Msg);
							send_mtc(players[i].UCID, ginfo.fines[atoi(pack_btt->Text)].name);

							strcpy(Msg, msg->message[players[i].lang_id][1106]);
							strcat(Msg, players[g].PName);
							send_mtc(players[i].UCID, Msg);


							for (int j=0; j<MAX_FINES; j++)
							{
								if (players[g].fines[j].fine_id == 0)
								{
									players[g].fines[j].fine_id = atoi(pack_btt->Text);
									players[g].fines[j].fine_date = int(time(&stime));
									break;
								}
							}



							ofstream readf (fine_c, ios::app);
							readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[i].UName << " get fine ID = " << pack_btt->Text << " to "  << players[g].UName << endl;
							readf.close();


						} // if atoi(pack_btt->Text) > 0
						break;
					}
				}//for

			}
			else if (pack_btt->ClickID==39)
			{

				for (int g=0; g<MAX_PLAYERS; g++)
				{
					if  (players[i].BID2 == players[g].BID)
					{
						if (atoi(pack_btt->Text) > 0)
						{

							cout << players[i].UName << " cancle fine id = " << pack_btt->Text << " to "  << players[g].UName << endl;

							char Msg[64];
							strcpy(Msg, msg->message[players[g].lang_id][1107]);
							send_mtc(players[g].UCID, Msg);
							strcpy(Msg, "^2| ");
							strcat(Msg, ginfo.fines[atoi(pack_btt->Text)].name);
							send_mtc(players[g].UCID, Msg);

							strcpy(Msg, msg->message[players[i].lang_id][1108]);
							send_mtc(players[i].UCID, Msg);
							send_mtc(players[i].UCID, ginfo.fines[atoi(pack_btt->Text)].name);

							strcpy(Msg, msg->message[players[g].lang_id][1106]);
							strcat(Msg, players[g].PName);
							send_mtc(players[i].UCID, Msg);


							for (int j=0; j<MAX_FINES; j++)
							{
								if (players[g].fines[j].fine_id == atoi(pack_btt->Text))
								{
									players[g].fines[j].fine_id = 0;
									players[g].fines[j].fine_date = 0;
									break;
								}
							}



							ofstream readf (fine_c, ios::app);
							readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[i].UName << " cancle fine ID = " << pack_btt->Text << " to "  << players[g].UName << endl;
							readf.close();


						} // if atoi(pack_btt->Text) > 0
						break;
					}
				}//for

			}



		}
	}
	pthread_mutex_unlock (&RCmutex);
	Sleep(100);
}
**/

void RCCore::case_cnl ()
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
				//bank->players[i].Cash += 500;
			}

			//save_user(&players[i]);
		   // save_user_cars(&players[i]);
			//save_user_fines(&players[i]);

			cout << players[i].UName << " left server" << endl;
			memset(&players[i], 0, sizeof(struct player));
			break;
		}
	}
}

void RCCore::case_toc ()
{
	int i;

	struct IS_TOC *pack_toc = (struct IS_TOC*)insim->get_packet();

	// Find player and set the whole player struct he was using to 0
	for (i=0; i < MAX_PLAYERS; i++)
	{
		if (players[i].UCID == pack_toc->NewUCID)
		{
				char Text[64];
				strcpy(Text, "/spec ");
				strcat (Text, players[i].UName);
				send_mtc(pack_toc->NewUCID, "^1Acces Denine");
				send_mst(Text);
				break;
		}
	}
}

void RCCore::case_cpr ()
{
	int i;

	struct IS_CPR *pack_cpr = (struct IS_CPR*)insim->get_packet();

	// Find player and set his PLID to 0
	for (i=0; i < MAX_PLAYERS; i++)
	{
		if (players[i].UCID == pack_cpr->UCID)
		{
			cout << players[i].PName << " rename to " << pack_cpr->PName << endl;
			strcpy(players[i].PName, pack_cpr->PName);
			break;
		}
	}
}



/**
void RCCore::case_flg ()
{
	int i;

	struct IS_FLG *pack_flg = (struct IS_FLG*)insim->get_packet();

	// Find player and set his PLID to 0
	for (i=0; i < MAX_PLAYERS; i++)
	{
		if (players[i].PLID == pack_flg->PLID)
		{
			break;
		}
	}
}
**/
/**
void RCCore::case_lap ()
{
	cout << "IS_LAP\n";
	struct IS_LAP *pack_lap = (struct IS_LAP*)insim->get_packet();

	for (int i=0; i < MAX_PLAYERS; i++)
	{
		if (players[i].PLID == pack_lap->PLID)
		{


			int bonus = 100+(50*(pack_lap->LapsDone-1));
			bank->players[i].Cash += bonus;
			char bonus_c[64];
			strcpy(bonus_c, msg->message[players[i].lang_id][1500]);
			char bonus_ic[5];
			itoa(bonus, bonus_ic, 10);
			strcat(bonus_c, bonus_ic);
			strcat(bonus_c, " ^7RUR.");
			send_mtc(players[i].UCID, bonus_c);

			break;
		}

	}
}
**/
void RCCore::case_mci ()
{
	struct IS_MCI *pack_mci = (struct IS_MCI*)insim->udp_get_packet();


	for (int i = 0; i < pack_mci->NumC; i++)

	{
		for (int j =0; j < MAX_PLAYERS; j++)
		{

			if (pack_mci->Info[i].PLID == players[j].PLID and players[j].PLID != 0 and players[j].UCID != 0)

			{

				int Node = pack_mci->Info[i].Node;

				int X = pack_mci->Info[i].X/65536;
				int Y = pack_mci->Info[i].Y/65536;
				int Z = pack_mci->Info[i].Z/65536;

				int S = ((int)pack_mci->Info[i].Speed*360)/(32768);

				int X1 = players[j].Info.X2/65536;
				int Y1 = players[j].Info.Y2/65536;
				int Z1 = players[j].Info.Z2/65536;

				if (X1==0 and Y1==0 and Z1==0)
				{
					X1=X;
					Y1=Y;
					Z1=Z;
					players[j].Info.X2 = pack_mci->Info[i].X;
					players[j].Info.Y2 = pack_mci->Info[i].Y;
					players[j].Info.Z2 = pack_mci->Info[i].Z;
				}

				float Dist = sqrt(pow((X-X1), 2)+pow((Y-Y1), 2)+pow((Z-Z1), 2));
				if (players[j].cop != 1)
				{
					if ((abs((int)Dist) > 10) and (S>30))
					{
						players[j].Distance += abs((int)Dist);

						if (S<150)
							bank->players[j].Cash += abs((int)Dist)/10;
						//bank->BankFond -= abs((int)Dist)/10;

						players[j].Info.X2 = pack_mci->Info[i].X;
						players[j].Info.Y2 = pack_mci->Info[i].Y;
						players[j].Info.Z2 = pack_mci->Info[i].Z;
					}
				}

				/** Bonus **/

				if ( (Node <= Node_Split1+5) and (Node >= Node_Split1-5) and (Node_Split1 != 0) )
				{
					if (players[j].Bonus_s1 == 0)
					{
						players[j].Bonus_s1 = 1;
					}
				}

				else if ( (Node <= Node_Split2+5) and (Node >= Node_Split2-5) and (Node_Split2 != 0) )
				{
					if (players[j].Bonus_s2 == 0)
					{
						players[j].Bonus_s2 = 1;
					}

				}

				else if ( (Node <= Node_Split3+5) and (Node >= Node_Split3-5) and (Node_Split3 != 0) )
				{
					if (players[j].Bonus_s3 == 0)
					{
						players[j].Bonus_s3 = 1;
					}
				}


				if ((Node <= Node_Finish+5) and (Node >= Node_Finish-5) and (Node_Finish != 0))

				{
					int sum = players[j].Bonus_s1 + players[j].Bonus_s2 + players[j].Bonus_s3;
					if (sum == Splits_Count)
					{

						int bonus = 100+(50*(players[j].Bonus_laps));
						players[j].Bonus_laps +=1;
						//players[j].Bonus_key = 0;
						players[j].Bonus_s1 = 0;
						players[j].Bonus_s2 = 0;
						players[j].Bonus_s3 = 0;

						bank->players[j].Cash += bonus;
						//bank->BankFond -= bonus;

						char bonus_c[64];
						strcpy(bonus_c, msg->message[players[j].lang_id][1500]);
						char bonus_ic[5];
						itoa(bonus, bonus_ic, 10);
						strcat(bonus_c, bonus_ic);
						strcat(bonus_c, " ^7RUR.");
						send_mtc(players[j].UCID, bonus_c);
					}
					//
				}



				/** streets  **/


				for (int g=0; g<players[j].str_count; g++)
				{
					if (Node >= players[j].street[g].NodeBeg and Node <= players[j].street[g].NodeEnd )
					{
						players[j].StreetNum = g;
						//strcpy(players[j].Street, "^C^7");
						//strcat(players[j].Street, players[j].street[g].Street);
					}
				}


				/**  steets **/

				/** Zones (PitSave, shop, etc) **/


				if (Check_Pos(players[j].TrackInf.PitCount, players[j].TrackInf.XPit, players[j].TrackInf.YPit, X, Y))
				{
					players[j].Zone = 1;
				}
				else if (Check_Pos(players[j].TrackInf.ShopCount, players[j].TrackInf.XShop, players[j].TrackInf.YShop, X, Y))
				{
					players[j].Zone = 2;
				}
				else
				{
					players[j].Zone = 0;
				}

				players[j].Info.X = pack_mci->Info[i].X;
				players[j].Info.Y = pack_mci->Info[i].Y;
				players[j].Info.Node = pack_mci->Info[i].Node;
				players[j].Info.Direction = pack_mci->Info[i].Direction;
				players[j].Info.Heading = pack_mci->Info[i].Heading;
				players[j].Info.Speed = pack_mci->Info[i].Speed;
				players[j].Info.AngVel = pack_mci->Info[i].AngVel;
				//btn_svetofor3(&players[i]);

			} // if pack_mci->Info[i].PLID == players[j].PLID
		}
	}
}



void RCCore::case_mci_svetofor ()
{
	struct IS_MCI *pack_mci = (struct IS_MCI*)insim->udp_get_packet();


	for (int i = 0; i < pack_mci->NumC; i++)

	{
		for (int j =0; j < MAX_PLAYERS; j++)
		{

			if (pack_mci->Info[i].PLID == players[j].PLID)

			{
				//cout << "MCI packet for " << players[j].UName << endl;

				int Node = pack_mci->Info[i].Node;
				int X = pack_mci->Info[i].X/65536;
				int Y = pack_mci->Info[i].Y/65536;
				int H = pack_mci->Info[i].Heading/182;
				//int S = ((int)pack_mci->Info[i].Speed*360)/(32768);

				int SvetKey = 0;

				if (strcmp(Track, "AS5") == 0)
				{
					//cout << ginfo.Track << endl;
					// ZONE 1 PIT 1
					// from 1 pit 2 svetofot
					int Pit1X[5] = {-605, -624, -624, -605};
					int Pit1Y[5] = {1, 1, 10, 10};
					int Pit2X[5] = {-605, -624, -624, -605};
					int Pit2Y[5] = {-510, -510, -500, -500};
					int CafeX[5] = {974, 984, 984, 974};
					int CafeY[5] = {-1027, -1027, -1039, -1039};
					int Cafe2X[6] = {974, 967, 947, 942, 962};
					int Cafe2Y[6] = {-1039, -1066, -1086, 1075, -1040};

					if ((Node > 12) and (Node < 32) and (X > -610) and (H > 140) and (H<185))
					{
						SvetKey = 1;
						players[j].Svetofor = 1;
					}
					if ((Node > 0) and (Node < 12) and (X > -610) and ((H > 350) or (H < 10)))
					{
						SvetKey = 1;
						players[j].Svetofor = 1;
					}
					if ((Check_Pos(4, Pit1X, Pit1Y, X, Y)) and (H > 260) and (H < 360))
					{
						SvetKey = 2;
						players[j].Svetofor = 2;
					}
					// !-- ZONE 1 PIT 1
					// ZONE 2 PIT 2
					if ((Node > 726) and (Node < 738) and (X > -610) and (H > 140) and (H<185))
					{
						SvetKey = 1;
						players[j].Svetofor = 1;
					}
					if ((Node > 713) and (Node < 727) and (X > -610) and ((H > 350) or (H < 10)))
					{
						SvetKey = 1;
						players[j].Svetofor = 1;
					}
					if ((Check_Pos(4, Pit2X, Pit2Y, X, Y)) and (H > 200) and (H < 360))
					{
						SvetKey = 2;
						players[j].Svetofor = 2;
					}
					// !-- ZONE 2 PIT 2
					// ZONE 3 RCPizza CAFE
					if ((Node > 377) and (Node < 390) and (Check_Pos(5, Cafe2X, Cafe2Y, X, Y)) and ((H > 300) or (H<10)))
					{
						SvetKey = 1;
						players[j].Svetofor = 1;
					}
					if ((Node > 360) and (Node < 377) and (X < 975) and (H > 140) and (H < 185))
					{
						SvetKey = 1;
						players[j].Svetofor = 1;
					}
					if ((Check_Pos(4, CafeX, CafeY, X, Y)) and (H > 0) and (H < 100))
					{
						SvetKey = 2;
						players[j].Svetofor = 2;
					}
					// !-- ZONE 3
					// ZONE 4
					if ((Node > 223) and (Node < 235) and (H > 170) and (H< 280))
					{
						SvetKey = 1;
						players[j].Svetofor = 1;
					}
					if ((Node > 238) and (Node < 262) and (H > 60) and (H< 100))
					{
						SvetKey = 2;
						players[j].Svetofor = 2;
					}
					// !-- ZONE 4

					// ZONE 5
					if ((Node > 534) and (Node < 549) and (H > 100) and (H< 170))
					{
						SvetKey = 1;
						players[j].Svetofor = 1;
					}
					if ((Node > 549) and (Node < 560) and ((H > 340) or (H< 10)))
					{
						SvetKey = 2;
						players[j].Svetofor = 2;
					}
					// !-- ZONE 5

				}// AS5

				else if (strcmp(Track, "FE4") == 0)
				{
					//cout << ginfo.Track << endl;
					// ZONE 1 PIT 1
					// from 1 pit 2 svetofot
					int Pit1X[5] = {179, 189, 189, 179};
					int Pit1Y[5] = {-15, -15, -23, -23};

					int Pit2X[5] = {173, 190, 190, 173};
					int Pit2Y[5] = {210, 210, 205, 205};

					//int CafeX[5] = {974, 984, 984, 974};
					//int CafeY[5] = {-1027, -1027, -1039, -1039};

					//int Cafe2X[6] = {974, 967, 947, 942, 962};
					//int Cafe2Y[6] = {-1039, -1066, -1086, 1075, -1040};

					if ((Node > 220) and (Node < 231) and (X < 180) and (H > 130) and (H<230))
					{
						SvetKey = 1;
						players[j].Svetofor = 1;
					}
					if ((Node > 210) and (Node < 221) and (X < 180) and ((H > 310) or (H < 50)))
					{
						SvetKey = 1;
						players[j].Svetofor = 1;
					}
					if ((Check_Pos(4, Pit1X, Pit1Y, X, Y)) and (H > 75-50) and (H < 75+50))
					{
						SvetKey = 2;
						players[j].Svetofor = 2;
					}
					// !-- ZONE 1 PIT 1
					// ZONE 2 PIT 2
					if ((Node > 246) and (Node < 260) and (X < 180) and (H > 180-50) and (H<180+50))
					{
						SvetKey = 1;
						players[j].Svetofor = 1;
					}
					if ((Node > 237) and (Node < 247) and (X < 180) and ((H > 360-50) or (H < 50)))
					{
						SvetKey = 1;
						players[j].Svetofor = 1;
					}
					if ((Check_Pos(4, Pit2X, Pit2Y, X, Y)) and (H > 90-50) and (H < 90+50))
					{
						SvetKey = 2;
						players[j].Svetofor = 2;
					}
					// !-- ZONE 2 PIT 2

					// ZONE 3 RCPizza CAFE
					if ((Node > 540) and (Node < 550) and (X > -32) and ((H > 300) or (H<10)))
					{
						SvetKey = 1;
						players[j].Svetofor = 1;
					}
					if ((Node > 550) and (Node < 560) and (X > -32) and (H > 140) and (H < 185))
					{
						SvetKey = 2;
						players[j].Svetofor = 2;
					}
					// !-- ZONE 3
					// ZONE 4

					// !-- ZONE 4

					// ZONE 5

					// !-- ZONE 5


				}// FE4

				else if (strcmp(Track, "SO4") == 0)
				{
					/**********светофоры*********/

					/***************** 1 zone ***************/

//1.1
					if ((Node>=526) or (Node>=0 and Node<=3 )and X<-140)
					{
						if (H>180 and H<220)
						{
							SvetKey = 1;
							players[j].Svetofor = 1;
						}

					}
//2.2
					if (Node >= 6 and Node <=13 )
					{
						if (H>0 and H<40)
						{
							SvetKey = 1;
							players[j].Svetofor = 1;
						}

					}
					if (Node >= 3 and Node <6)
					{
						if (X>=-151 and X<-140)
						{
							if (H>60 and H<130)
							{
								SvetKey = 2;
								players[j].Svetofor = 2;
							}
						}

					}
					/******************************************/

					/***************** 2 zone ***************/
//3.1
					if (Node>=13 and Node<24 )
					{
						if (H>160 and H<200)
						{
							SvetKey = 1;
							players[j].Svetofor = 1;
						}

					}
//4.2
					if ( Node>27 and Node<=40 )
					{
						if (H>250 and H<290)
						{
							SvetKey = 2;
							players[j].Svetofor = 2;
						}
					}




					/********************************/

					/***************** 3 zone ***************/
//5.1
					if (Node>83 and Node<=93 )
					{
						if (H>70 and H<100)
						{
							SvetKey = 1;
							players[j].Svetofor = 1;
						}

					}
//6.2
					if ( Node>=97 and Node<102 )
					{
						if (H<20 or H>300)
						{
							SvetKey = 2;
							players[j].Svetofor = 2;
						}

					}
					if ( Node>=102 and Node<106 )
					{
						if (H>260)
						{
							SvetKey = 2;
							players[j].Svetofor = 2;
						}

					}


					/********************************/

					/***************** 4 zone ***************/
//7.1
					if (Node>188 and Node<208 )
					{
						if (H>280 and H<320)
						{
							SvetKey = 1;
							players[j].Svetofor = 1;
						}

					}
//8.2
					if ( Node>=214 and Node<224 )
					{
						if ( H<20 or H>350)
						{
							SvetKey = 2;
							players[j].Svetofor = 2;
						}

					}


					/********************************/


					/***************** 5 zone ***************/
					if (Node>405 and Node<422 )
					{
						if (H<60 or H>351)
						{
							SvetKey = 1;
							players[j].Svetofor = 1;
						}

					}

					/***/

//8.2
					if ( Node>=433 and Node<440 and X>=250)
					{
						if ( H>160 and H<190)
						{
							SvetKey = 1;
							players[j].Svetofor = 1;
						}
					}

					if ( Node>=428 and Node<433 and X>=244)
					{
						if ( H>160 and H<190)
						{
							SvetKey = 1;
							players[j].Svetofor = 1;
						}

					}

					if ( Node>=425 and Node<428 and X>=240)
					{
						if ( H>160 and H<190)
						{
							SvetKey = 1;
							players[j].Svetofor = 1;
						}

					}

					if ( Node>=425 and Node<=428 and X<240)
					{
						if ( H>160 and H<190)
						{
							SvetKey = 2;
							players[j].Svetofor = 2;
						}
					}

				}// SO4

				//SvetKey = 1;

				if (SvetKey == 1)
				{
					btn_svetofor1(&players[j]);
				}
				else if (SvetKey == 2)
				{
					btn_svetofor2(&players[j]);
				}
				else
				{
					if (players[j].Svetofor != 0)
					{
						for (int f=190; f < 203; f++)
							send_bfn(players[j].UCID, f);

						players[j].Svetofor = 0;
						//cout << "clear svetofor" << endl;
					}
				}

			} // if pack_mci->Info[i].PLID == players[j].PLID
		}
	}
}



void RCCore::case_mso ()
{
	int i;

	struct IS_MSO *pack_mso = (struct IS_MSO*)insim->get_packet();

	// The chat message is sent by the host, don't do anything
	if (pack_mso->UCID == 0)
	{
		// cout << "(Chat message by host: " << pack_mso->Msg + ((unsigned char)pack_mso->TextStart) << ")" << endl;
		return;

	}

	// Find the player that wrote in the chat
	for (i=0; i < MAX_PLAYERS; i++)
	{
		if (players[i].UCID == pack_mso->UCID)
		{
			//  cout << "Msg: " << pack_mso->Msg << endl;
			break;
		}
	}

	char Msg[96];
	strcpy(Msg, pack_mso->Msg + ((unsigned char)pack_mso->TextStart));
	printf("Msg: %s\n", Msg);

	//!help
	if ((strncmp(Msg, "!info", 5) == 0) or (strncmp(Msg, "!^Cинфо", 7) == 0))
	{
		cout << players[i].UName << " send !info" << endl;
		for (int j=159; j>0; j--)
		{
			send_bfn(players[i].UCID, j);
			Sleep(10);
		}
	   // btn_info(&players[i], 1);
	}

	if ((strncmp(Msg, "!help", 5) == 0) or (strncmp(Msg, "!^Cпомощь", 9) == 0))
	{
		cout << players[i].UName << " send !help" << endl;
		help_cmds(&players[i], 1);
	}
	if ((strncmp(Msg, "!cars", 5) == 0) or (strncmp(Msg, "!^Cмашины", 9) == 0))
	{
		cout << players[i].UName << " send !cars" << endl;
		help_cmds(&players[i], 2);
	}

	//!save
	/**if ((strncmp(Msg, "!save", 5) == 0) or (strncmp(Msg, "!^Cаминь", 8) == 0))
	{
		cout << players[i].UName << " send !save" << endl;
		save_car(&players[i]);
		Sleep(500);
		save_user_cars(&players[i]);
		save_user_fines(&players[i]);

		bank->bank_save(i);
		nrg.energy_save(i);

		Sleep(500);
		send_mtc(players[i].UCID, msg->message[players[i].lang_id][3000]);

	}**/

	if ((strncmp(Msg, "!trans", 6) == 0 ) or (strncmp(Msg, "!^Cпередачи", 11) == 0))
	{
		cout << players[i].UName << Msg << endl;


		char message[96];
		strcpy(message, Msg);

		char file[255];
		strcpy(file, RootDir);
		strcat(file, "logs\\sends\\send.txt");

		HANDLE fff;
		WIN32_FIND_DATA fd;
		fff = FindFirstFile(file, &fd);
		if (fff == INVALID_HANDLE_VALUE)
		{
			cout << "Can't find " << file << endl;
			return;
		}
		FindClose(fff);

		ifstream readf (file, ios::in);

		char * comand;
		char * user;
		comand = strtok (message, " ");
		user = strtok (NULL, " ");


		if ((user) and (strlen(user) > 0))
		{
			// username exist


			while (readf.good())
			{
				char str[128];
				readf.getline(str, 128);
				if (strlen(str) > 0)
				{
					if ((strstr(str, players[i].UName)) and (strstr(str, user)))
					{
						char Text[64];
						strcpy(Text, "^1| ^C^7");
						strncat(Text, str, 55);
						send_mtc(players[i].UCID, Text);
					}
				}
			}

		}
		else
		{
			// username not exist
			while (readf.good())
			{
				char str[128];
				readf.getline(str, 128);
				if (strlen(str) > 0)
				{
					if (strstr(str, players[i].UName))
					{
						char Text[64];
						strcpy(Text, "^1| ^C^7");
						strncat(Text, str, 55);
						send_mtc(players[i].UCID, Text);
					}
				}
			}
		}


		readf.close();

	}

	//!shop
	if ((strncmp(Msg, "!shop", 4) == 0) or (strncmp(Msg, "!^Cмагазин", 10) == 0))
	{
		cout << players[i].UName << " send !shop" << endl;
		if (players[i].Zone == 2)
		{
			for (int j=0; j<200; j++)
				send_bfn(players[i].UCID, j);

			players[i].Shop=1;
		   // btn_shop(&players[i]);
		}
		else
		{
			send_mtc(players[i].UCID, msg->message[players[i].lang_id][2000]);
		}
	}


	//!EXIT
	if (strncmp(Msg, "!exit", 5) == 0 and strcmp(players[i].UName, "denis-takumi") == 0)
	{
		cout << players[i].UName << " send !exit" << endl;
		for (int j=0; j<MAX_PLAYERS; j++)
		{

			if (players[j].UCID !=0 )
			{
			   // save_car(&players[j]);

			  // save_user_cars(&players[j]);
			   // save_user_fines(&players[j]);
				bank->bank_save(j);
				//nrg.energy_save(j);
			}
		}
		//ok=0;
	}
	if (strncmp(Msg, "!reload", 7) == 0 and strcmp(players[i].UName, "denis-takumi") == 0)
	{
		cout << players[i].UName << " send !reload" << endl;

		struct IS_TINY pack_requests;
		memset(&pack_requests, 0, sizeof(struct IS_TINY));
		pack_requests.Size = sizeof(struct IS_TINY);
		pack_requests.Type = ISP_TINY;
		pack_requests.ReqI = 1;

		pack_requests.SubT = TINY_RST;      // Request all players in-grid to know their PLID
		insim->send_packet(&pack_requests);
	}
	//!evo
	if ((strncmp(Msg, "!pit", 4) == 0) or (strncmp(Msg, "!^Cпит", 6) == 0 ))
	{

		char Msg[64];
		strcpy(Msg, "/pitlane ");
		strcat(Msg, players[i].UName);
		send_mst(Msg);
		bank->players[i].Cash -=250;
		bank->BankFond += 250;
	}
	//!users
	if ((strncmp(Msg, "!users", 6) == 0) or (strncmp(Msg, "!^Cнарод", 8) == 0 ))
	{
		players[i].Action = 1;

		for (int h=0; h<240; h++)
		{
			send_bfn(players[i].UCID, h);
		}

		struct IS_BTN pack_btn;
		memset(&pack_btn, 0, sizeof(struct IS_BTN));
		pack_btn.Size = sizeof(struct IS_BTN);
		pack_btn.Type = ISP_BTN;
		pack_btn.ReqI = players[i].UCID;              // Must be non-zero, I'll just use UCID
		pack_btn.UCID = players[i].UCID;              // UCID of the player that will receive the button
		pack_btn.BStyle = 16 + ISB_CLICK;                 // Dark frame for window title
		pack_btn.L = 162;
		pack_btn.T = 1;
		pack_btn.W = 34;
		pack_btn.H = 5;
		for (int j=0; j<MAX_PLAYERS; j++)
		{
			if (players[j].UCID!=0)
			{
				pack_btn.ClickID = players[j].BID;
				strcpy(pack_btn.Text, players[j].PName);
				insim->send_packet(&pack_btn);
				pack_btn.T += 6;
				Sleep(100);
			}

		}
		pack_btn.ClickID = 34;
		strcpy(pack_btn.Text, msg->message[players[i].lang_id][2003]);
		insim->send_packet(&pack_btn);

	}

	if ((strncmp(Msg, "!lang", 5) == 0 ) or (strncmp(Msg, "!^Cязык", 7) == 0 ))
	{
		cout << players[i].UName << " send !lang" << endl;

		char message2[96];
		strcpy(message2, Msg);

		if (strlen(message2) < 8)
		{
			send_mtc(players[i].UCID, msg->message[players[i].lang_id][2104]);
			return;
		}

		char * comand;
		char * id;

		comand = strtok (message2, " ");
		id = strtok (NULL, " ");

		if ((!id) or (strlen(id) != 3))
		{
			send_mtc(players[i].UCID, msg->message[players[i].lang_id][2105]);
			return;
		}

		if (strstr("* eng rus", id) == 0)
		{
			return;
		}

		strcpy(players[i].Lang, id);
		//read_lang(&players[i]);
		if (strcmp(players[i].Lang, "eng") == 0)
		{
			players[i].lang_id = 1;
		}
		else
		{
			players[i].lang_id = 0;
		}

		//read_track(&players[i]);

	}

	/**  FLOOD ***/
	if (pack_mso->UserType != MSO_PREFIX) // if msg not with prefix '!'
	{

		time_t timef;
		int ftime = time(&timef); // get current time

		if (players[i].FloodTime == 0)
		{
			players[i].FloodTime = ftime;
		}

		int ts = ftime - players[i].FloodTime;
		players[i].FloodTime = ftime;


		if (ts < 5)
			players[i].FloodCount++;
		else
		{
			players[i].FloodCount = 1;
		}

		if (players[i].FloodCount > 4)   //max lines to tolerate
		{
			send_mtc(players[i].UCID, msg->message[players[i].lang_id][2004]);
			players[i].FloodCount = 0;
			bank->players[i].Cash -= 500;
			bank->BankFond += 500;
		}


		/*** МАТ И Т.П. ***/
		//read_words();


/**
		char Msg2[96];
		strcpy(Msg2, strupr(Msg));

		for (int j=0; j< ginfo.WordsCount; j++)
		{
			if (strstr(Msg2, strupr(ginfo.Words[j])))
			{
				send_mtc(players[i].UCID, msg->message[players[i].lang_id][2005]);
				bank->players[i].Cash -= 1000;
				bank->BankFond += 1000;
			}

		}
		strcpy(players[i].Msg, Msg);**/
	} // if UserType != MSO_PREFIX
	/** Flood and bad words **/
}



void RCCore::case_ncn ()
{
	int i;

	struct IS_NCN *pack_ncn = (struct IS_NCN*)insim->get_packet();

	//cout << "ReqI = " << (int)pack_ncn->ReqI << endl;


	if (pack_ncn->UCID == 0)
	{
		// cout << "(Host connected, not adding him to array...)" << endl << endl;
		return;
	}



	for (i=0; i<MAX_PLAYERS; i++)
		if (players[i].UCID == 0)
			break;


	if (i == MAX_PLAYERS)
	{
		//  cout << "NO MORE PEOPLE ALLOWED!!!" << endl << endl;
		return;
	}

	// Copy all the player data we need into the players[] array
	strcpy(players[i].UName, pack_ncn->UName);
	strcpy(players[i].PName, pack_ncn->PName);
	players[i].UCID = pack_ncn->UCID;
	players[i].BID = i;
	players[i].Zone = 1;
	//players[i].Admin = pack_ncn->Admin;


	cout <<  "\n******************************" << endl;
	cout <<  players[i].UName << " connected" << endl;
	//read_user(&players[i]);
   // read_user_cars(&players[i]);
	//read_user_fines(&players[i]);
	//read_lang(&players[i]);
   // read_track(&players[i]);
	cout <<  "******************************" << endl;

   // help_cmds(&players[i], 1);
   // help_cmds(&players[i], 2);
}

void RCCore::case_npl ()
{
	// cout << "joining race or leaving pits" << endl;
	int i;
	char Text[64];
	struct IS_NPL *pack_npl = (struct IS_NPL*)insim->get_packet();

	// Find player using UCID and update his PLID
	for (i=0; i < MAX_PLAYERS; i++)
	{
		if (players[i].UCID == pack_npl->UCID)
		{

			/*****   Hack Detect ***/

			time_t timeh;
			int htime = time(&timeh); // get current time

			if (players[i].NPLTime == 0)
			{
				players[i].NPLTime = htime;
			}

			int ts = htime - players[i].HackTime;
			players[i].HackTime = htime;


			if (ts < 10)  // 10 sec
				players[i].NPLHack++;
			else
			{
				players[i].NPLHack = 1;
			}

			if (players[i].NPLHack > 4)   //max lines to tolerate
			{

				players[i].NPLHack = 0;
				strcpy(Text, "/kick ");
				strcat (Text, players[i].UName);
				send_mst("/msg ^1Hack detect");
				send_mst(Text);
				return;

			}




			if (strlen(pack_npl->CName) < 3)
			{
				strcpy(Text, "/kick ");
				strcat (Text, players[i].UName);
				send_mst("/msg ^1Hack detect");
				send_mst(Text);
				return;
			}

			/*******    ********/
			// cout << "Core.dll: find user " << players[i].UName << endl;
			if (pack_npl->PType != 6)
			{
			  /**  char PlayerName[32];
				strcpy(PlayerName, players[i].PName);
				char * pch;
				while (pch = strstr(PlayerName, "^"))
				{
					int point = strlen(pch);
					//cout << pch << endl;
					strcpy(PlayerName+strlen(PlayerName)-point, pch+2);
				}
				//cout << PlayerName << endl;

				if (
					((strncmp("[ДПС]", PlayerName, 5)==0)
					 || (strncmp("[дпс]", PlayerName, 5)==0)
					 || (strncmp("[ГАИ]", PlayerName, 5)==0)
					 || (strncmp("[гаи]", PlayerName, 5)==0))
					&& (players[i].cop !=1)
				)
				{
					if ( read_cop(&players[i]) < 1)
					{
						strcpy(Text, "/spec ");
						strcat (Text, players[i].UName);
						//players[i].PLID =0;
						send_mtc(players[i].UCID, msg->message[players[i].lang_id][1300]);
						send_mst(Text);
						return;
					}
					else
					{
						players[i].cop =1;
						send_mtc(players[i].UCID, msg->message[players[i].lang_id][1301]);
					}
				}**/
/**
				if (players[i].cop != 1)
				{

					char Text[64];
					strcpy(Text, "/spec ");
					strcat (Text, players[i].UName);

					players[i].PLID = pack_npl->PLID;
					players[i].H_TRes =  pack_npl->H_TRes;
					players[i].SetF =  pack_npl->SetF;


					// cout << "pack_npl->PType = " << (int)pack_npl->PType << endl;

					int j=0;
					for (j=0; j<MAX_CARS; j++)
					{
						if (strcmp(players[i].cars[j].car, pack_npl->CName)==0)
							break;
					}

					if ( j != MAX_CARS)
					{



						strcpy(players[i].CName , pack_npl->CName);
						players[i].CTune = players[i].cars[j].tuning;
						players[i].Distance = players[i].cars[j].dist;
						//strcpy(players[i].SName , row[2]);


						// cout << "Core.dll: free result" << endl;



						//itoa(players[i].CTune, SQL, 10);
						//send_mtc(players[i].UCID, SQL);


						int tune = 45;
						if (players[i].CTune&1)
							tune -= 15;
						if (players[i].CTune&2)
							tune -= 20;
						if (pack_npl->H_TRes < tune)
						{
							players[i].PLID = 0;
							players[i].Zone = 1;
							send_mst(Text);
							char QSL [8];
							itoa(tune, QSL, 10);
							char Texxt[32];
							strcpy(Texxt, msg->message[players[i].lang_id][2400] );
							strcat(Texxt, QSL);
							strcat(Texxt, " %");
							send_mtc(players[i].UCID, Texxt);
						}
						else if ((pack_npl->SetF&4) and !(players[i].CTune&8))
						{
							char Texxt[32];
							strcpy(Texxt, msg->message[players[i].lang_id][2401] );
							send_mtc(players[i].UCID, Texxt);
							players[i].Zone = 1;
							players[i].PLID = 0;
							char Text[64];
							strcpy(Text, "/spec ");
							strcat (Text, players[i].UName);
							send_mst(Text);
							return;
						}



					}
					else
					{
						send_mtc(players[i].UCID, msg->message[players[i].lang_id][2404]);

						help_cmds(&players[i], 2);
						players[i].Zone = 1;
						players[i].PLID = 0;
						send_mst(Text);
						return;
					}

				} // if cop !=1
				else
				{
					players[i].PLID = pack_npl->PLID;
					players[i].H_TRes =  pack_npl->H_TRes;
					players[i].SetF =  pack_npl->SetF;
					//players[i].EnergyTime = time(&stime);
				}

*/

				break;
			} //if PTupe != 6
			else
			{
				cout << "bot detecdet" << endl;
			}
		}//if UCID == UCID
	}//for



}




void RCCore::case_pll ()
{
	// cout << "player leaves race" << endl;
	int i;

	struct IS_PLL *pack_pll = (struct IS_PLL*)insim->get_packet();

	// Find player and set his PLID to 0
	for (i=0; i < MAX_PLAYERS; i++)
	{
		if (players[i].PLID == pack_pll->PLID)
		{
			players[i].PLID = 0;
			players[i].Info.X = 0;
			players[i].Info.Y = 0;
			players[i].Info.Z = 0;
			players[i].Penalty =0;

		   // save_car(&players[i]);

			if (players[i].Pogonya == 1)
			{
				send_mtc(players[i].UCID, msg->message[players[i].lang_id][2600]);
				send_mtc(players[i].UCID, msg->message[players[i].lang_id][2601]);
				bank->players[i].Cash -= 5000;
				bank->BankFond += 5000;
			}
			else
			{
				if (players[i].Zone != 1)
				{
					send_mtc(players[i].UCID, msg->message[players[i].lang_id][2602]);
					send_mtc(players[i].UCID, msg->message[players[i].lang_id][2603]);
					bank->players[i].Cash -= 500;
					bank->BankFond +=500;
				}
			}
			for (int g=0; g<200; g++)
				send_bfn(players[i].UCID, g);

			break;
		}
	}
}

void RCCore::case_plp ()
{
	// cout << "player leaves race" << endl;
	int i;

	struct IS_PLP *pack_plp = (struct IS_PLP*)insim->get_packet();

	// Find player and set his PLID to 0
	for (i=0; i < MAX_PLAYERS; i++)
	{
		if (players[i].PLID == pack_plp->PLID)
		{
			players[i].PLID = 0;
			players[i].Info.X = 0;
			players[i].Info.Y = 0;
			players[i].Info.Z = 0;
			players[i].Penalty =0;

		 //   save_car(&players[i]);

			if (players[i].Pogonya == 1)
			{
				send_mtc(players[i].UCID, msg->message[players[i].lang_id][2700]);
				send_mtc(players[i].UCID, msg->message[players[i].lang_id][2701]);
				bank->players[i].Cash -= 5000;
				bank->BankFond += 5000;
			}
			else
			{
				if (players[i].Zone != 1)
				{
					send_mtc(players[i].UCID, msg->message[players[i].lang_id][2702]);
					send_mtc(players[i].UCID, msg->message[players[i].lang_id][2703]);
					bank->players[i].Cash -= 500;
					bank->BankFond += 500;
				}
			}

			for (int g=0; g<200; g++)
				send_bfn(players[i].UCID, g);

			break;
		}
	}
}

void RCCore::case_rst ()
{
	struct IS_RST *pack_rst = (struct IS_RST*)insim->get_packet();
	cout << "Race Start Packet" << endl;

	strcpy(Track, pack_rst->Track);



	Splits_Count = 0;

	if (pack_rst->Split1 <= pack_rst->NumNodes)
	{
		Node_Split1 = pack_rst->Split1;
		Splits_Count +=1;
	}
	if (pack_rst->Split2 <= pack_rst->NumNodes)
	{
		Node_Split2 = pack_rst->Split2;
		Splits_Count +=1;
	}
	if (pack_rst->Split3 <= pack_rst->NumNodes)
	{
		Node_Split3 = pack_rst->Split3;
		Splits_Count +=1;
	}

	Node_Finish = pack_rst->Finish;

	/////////
	//read_car();
	Sleep(100);
   // read_fines();
	Sleep(100);
	//pizza.readconfig(pack_rst->Track);
	//nrg.readconfig(pack_rst->Track);
   // bank->readconfig(pack_rst->Track);
	Sleep(100);
	/////////

	// cout << pack_sta->Track << endl << ginfo.Track << endl;
};

void RCCore::case_vtn ()
{
	struct IS_VTN *pack_vtn = (struct IS_VTN *)insim->get_packet();

	struct IS_TINY pack;
	memset(&pack, 0, sizeof(struct IS_TINY));
	pack.Size = sizeof(struct IS_TINY);
	pack.Type = ISP_TINY;
	pack.ReqI = 1;
	pack.SubT = TINY_VTC;
	insim->send_packet(&pack);

	for (int i=0; i<MAX_PLAYERS; i++)
	{
		if (players[i].UCID == pack_vtn->UCID)
		{
			send_mst(msg->message[players[i].lang_id][2900]);
			break;
		}
	}




}

void RCCore::send_mtc (byte UCID, char* Msg)
{
	struct IS_MTC pack_mtc;
	memset(&pack_mtc, 0, sizeof(struct IS_MTC));
	pack_mtc.Size = sizeof(struct IS_MTC);
	pack_mtc.Type = ISP_MTC;
	pack_mtc.UCID = UCID;
	strcpy(pack_mtc.Msg, Msg);
	insim->send_packet(&pack_mtc);
};

void RCCore::send_mst (char* Text)
{
	struct IS_MST pack_mst;
	memset(&pack_mst, 0, sizeof(struct IS_MST));
	pack_mst.Size = sizeof(struct IS_MST);
	pack_mst.Type = ISP_MST;
	strcpy(pack_mst.Msg, Text);
	insim->send_packet(&pack_mst);
};


void RCCore::send_bfn (byte UCID, byte ClickID)
{
	struct IS_BFN pack;
	memset(&pack, 0, sizeof(struct IS_BFN));
	pack.Size = sizeof(struct IS_BFN);
	pack.Type = ISP_BFN;
	pack.UCID = UCID;
	pack.ClickID = ClickID;
	insim->send_packet(&pack);
};


/******************************************************************
*
*               SVETOFOR ODNAKO
*
*
******************************************************************/

const char* signal1 ="^0•";
const char* signal2 ="^0•";
const char* signal3 ="^0•";
const char* signal11 ="^0•";
const char* signal12 ="^0•";
const char* signal13 ="^0•";


void RCCore::btn_svetofor1 (struct player *splayer)
{

	struct IS_BTN pack;
	memset(&pack, 0, sizeof(struct IS_BTN));
	pack.Size = sizeof(struct IS_BTN);
	pack.Type = ISP_BTN;
	pack.ReqI = 1;
	pack.UCID = splayer->UCID;
	pack.Inst = 0;
	pack.BStyle = 32;
	pack.TypeIn = 0;

	pack.ClickID = 190;
	pack.L = 92;
	pack.T = 15;
	pack.W = 16;
	pack.H = 40;
	insim->send_packet(&pack);

	pack.BStyle = 16;
	pack.ClickID = 191;
	pack.L = 93;
	pack.T = 16;
	pack.W = 14;
	pack.H = 38;
	insim->send_packet(&pack);



	pack.BStyle = 1;
	pack.ClickID = 192;
	pack.L = 85;
	pack.T = 10;
	pack.W = 30;
	pack.H = 30;
	strcpy(pack.Text, signal1);
	insim->send_packet(&pack);


	pack.ClickID = 193;
	pack.T = 10+10;
	strcpy(pack.Text, signal2);
	insim->send_packet(&pack);

	pack.ClickID = 194;
	pack.T = 10+20;
	strcpy(pack.Text, signal3);
	insim->send_packet(&pack);
}

void RCCore::btn_svetofor2 (struct player *splayer)
{

	struct IS_BTN pack;
	memset(&pack, 0, sizeof(struct IS_BTN));
	pack.Size = sizeof(struct IS_BTN);
	pack.Type = ISP_BTN;
	pack.ReqI = 1;
	pack.UCID = splayer->UCID;
	pack.Inst = 0;
	pack.BStyle = 32;
	pack.TypeIn = 0;

	pack.ClickID = 190;
	pack.L = 92;
	pack.T = 15;
	pack.W = 16;
	pack.H = 40;
	insim->send_packet(&pack);

	pack.BStyle = 16;
	pack.ClickID = 191;
	pack.L = 93;
	pack.T = 16;
	pack.W = 14;
	pack.H = 38;
	insim->send_packet(&pack);



	pack.BStyle = 1;
	pack.ClickID = 192;
	pack.L = 85;
	pack.T = 10;
	pack.W = 30;
	pack.H = 30;
	strcpy(pack.Text, signal11);
	insim->send_packet(&pack);


	pack.ClickID = 193;
	pack.T = 10+10;
	strcpy(pack.Text, signal12);
	insim->send_packet(&pack);

	pack.ClickID = 194;
	pack.T = 10+20;
	strcpy(pack.Text, signal13);
	insim->send_packet(&pack);
}


void RCCore::btn_svetofor3 (struct player *splayer)
{

	struct IS_BTN pack_btn;
	memset(&pack_btn, 0, sizeof(struct IS_BTN));
	pack_btn.Size = sizeof(struct IS_BTN);
	pack_btn.Type = ISP_BTN;
	pack_btn.ReqI = 1;
	pack_btn.UCID = splayer->UCID;
	pack_btn.Inst = 0;
	pack_btn.BStyle = 32;
	pack_btn.TypeIn = 0;
	/**** telo1 **/
	pack_btn.ClickID = 195;
	pack_btn.L = 148;
	pack_btn.T = 110;
	pack_btn.W = 8;
	pack_btn.H = 20;
	strcpy(pack_btn.Text, "");
	insim->send_packet(&pack_btn);
	/**********/


	pack_btn.BStyle = 1;
	pack_btn.ClickID = 196;
	pack_btn.L = 144;
	pack_btn.T = 107;
	pack_btn.W = 16;
	pack_btn.H = 16;
	strcpy(pack_btn.Text, signal1);
	insim->send_packet(&pack_btn);


	pack_btn.ClickID = 197;
	pack_btn.T = 112;
	strcpy(pack_btn.Text, signal2);
	insim->send_packet(&pack_btn);

	pack_btn.ClickID = 198;
	pack_btn.T = 117;
	strcpy(pack_btn.Text, signal3);
	insim->send_packet(&pack_btn);
	/********************************/
	pack_btn.BStyle = 32;
	/**** telo 2 *******/
	pack_btn.ClickID = 199;
	pack_btn.L = 157;
	pack_btn.T = 110;
	pack_btn.W = 8;
	pack_btn.H = 20;
	strcpy(pack_btn.Text, "");
	insim->send_packet(&pack_btn);
	/*************************/
	pack_btn.BStyle = 1;

	pack_btn.ClickID = 200;
	pack_btn.L = 153;
	pack_btn.T = 107;
	pack_btn.W = 16;
	pack_btn.H = 16;
	strcpy(pack_btn.Text, signal11);
	insim->send_packet(&pack_btn);


	pack_btn.ClickID = 201;
	pack_btn.T = 112;
	strcpy(pack_btn.Text, signal12);
	insim->send_packet(&pack_btn);

	pack_btn.ClickID = 202;
	pack_btn.T = 117;
	strcpy(pack_btn.Text, signal13);
	insim->send_packet(&pack_btn);
};

void RCCore::clear_svetofor (struct player *splayer)
{
	struct IS_BFN pack;
	memset(&pack, 0, sizeof(struct IS_BFN));
	pack.Size = sizeof(struct IS_BFN);
	pack.Type = ISP_BFN;
	pack.ReqI = 0;
	pack.SubT = 0;
	pack.UCID = splayer->UCID;
	pack.ClickID = 190;
	insim->send_packet(&pack);
	pack.ClickID = 191;
	insim->send_packet(&pack);
	pack.ClickID = 192;
	insim->send_packet(&pack);
	pack.ClickID = 193;
	insim->send_packet(&pack);
	pack.ClickID = 194;
	insim->send_packet(&pack);
};
/*
void read_user (struct player *splayer)
{
	char file[255];
	strcpy(file, RootDir);
	strcat(file, "data\\users\\");
	strcat(file, splayer->UName);
	strcat(file, ".txt");

	ifstream readf (file, ios::in);
	char key[2];
	readf.getline(key, 2);
	if (atoi(key) > 0)
	{
		char data[128];
		readf.getline(data, 128);
		int L2 = strlen(data);
		char *cash;
		char cashf[24];
		char *energy;
		char *car;
		char *lang;

		cash = strtok(data, ";");
		energy = strtok(NULL, ";");
		car = strtok(NULL, ";");

		int L = strlen(cash) + strlen(energy) + strlen(car) + 3;
		//out << "L = " << L << "L2 = " << L2 << endl;
		if (L == L2)
			lang="rus";
		else
			lang = strtok(NULL, ";");


		//memcpy(&cashf, &cash, strlen(cash));
		strcpy(cashf, cash);
		splayer->cash = atof(cashf);
		//splayer->Energy = atoi(energy);
		strcpy(splayer->Cars, car);
		strcpy(splayer->Lang, lang);
	}
	else
	{
		splayer->cash = 1000;
		//splayer->Energy = 10000;
		strcpy(splayer->Cars, "XFG ");
		strcpy(splayer->Lang, "rus");


	   // save_user(splayer);
	   // save_user_cars(splayer);
	   // save_user_fines(splayer);
	}
	if (strcmp(splayer->Lang, "eng") == 0)
	{
		splayer->lang_id = 1;
	}
	readf.close();
}
*/
/**
void save_user (struct player *splayer)
{
	out <<splayer->UName << " save user_info" << endl;

	char file[255];
	strcpy(file, RootDir);
	strcat(file, "data\\users\\");
	strcat(file, splayer->UName);
	strcat(file, ".txt");

	ofstream writef (file, ios::out);
	writef << 1 << endl;
	writef << splayer->cash << ";"
	// << splayer->Energy << ";"
	<< splayer->Cars << ";"
	<< splayer->Lang << ";"
	<< endl;
	writef.close();

	save_user_cars(splayer);
	save_user_fines(splayer);
}
**/
