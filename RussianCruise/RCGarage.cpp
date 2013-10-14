
void save_user_cars (struct player *splayer)
{
	//out <<splayer->UName << " save cars_info" << endl;

	char file[255];
	strcpy(file, RootDir);
	strcat(file, "data\\RCGarage\\");
	strcat(file, splayer->UName);
	strcat(file, ".txt");

	ofstream writef (file, ios::out);
	for (int i = 0; i < MAX_CARS; i++)
	{
		if (strlen(splayer->cars[i].car)>0)
		{
			writef << splayer->cars[i].car << ";" << splayer->cars[i].tuning << ";" << splayer->cars[i].dist << endl;
		}
	}

	writef.close();
}

void read_user_cars(struct player *splayer)
{
	char file[255];
	strcpy(file, RootDir);
	strcat(file, "data\\RCGarage\\");
	strcat(file, splayer->UName);
	strcat(file, ".txt");


	HANDLE fff;
	WIN32_FIND_DATA fd;
	fff = FindFirstFile(file, &fd);
	if (fff == INVALID_HANDLE_VALUE)
	{
		out << "Can't find " << file << endl;
		strcpy(splayer->cars[0].car, "XFG");
		splayer->cars[0].tuning = 0;
		splayer->cars[0].dist = 0;
		save_user_cars(splayer);
	}
	else
	{
		ifstream readf (file, ios::in);
		int i=0;
		while (readf.good())
		{
			char str[128];
			readf.getline(str, 128);

			if (strlen(str) > 0)
			{
				char *car;
				char *tun;
				char *dis;

				car = strtok(str, ";");
				tun = strtok(NULL, ";");
				dis = strtok(NULL, ";");

				strcpy(splayer->cars[i].car, car);
				splayer->cars[i].tuning = atoi(tun);
				splayer->cars[i].dist = atoi(dis);

				i++;
			}
		}
	}
	FindClose(fff);





}

void save_car (struct player *splayer)
{

	out <<splayer->UName << " save " << splayer->CName << " info" << endl;

	for (int i=0; i<MAX_CARS; i++)
	{
		if (strcmp(splayer->CName, splayer->cars[i].car)==0)
		{
			splayer->cars[i].tuning = splayer->CTune;
			splayer->cars[i].dist = (int)splayer->Distance;
			break;
		}
	}

}

void read_car()
{

	char file[255];
	strcpy(file, RootDir);
	//strcat(file, "misc\\mysql.cfg");
	strcat(file, "misc\\cars.txt");

	HANDLE fff;
	WIN32_FIND_DATA fd;
	fff = FindFirstFile(file, &fd);
	if (fff == INVALID_HANDLE_VALUE)
	{
		out << "Can't find " << file << endl;
		return;
	}
	FindClose(fff);

	ifstream readf (file, ios::in);

	int i = 0;
	while (readf.good())
	{
		char str[128];
		readf.getline(str, 128);
		if (strlen(str) > 0)
		{

			if (strstr(str, "//")) {}
			else
			{
				char * number;
				char * car;
				char * cash;
				number = strtok (str, ";");
				car = strtok (NULL, ";");
				cash = strtok (NULL, ";");

				i = atoi(number);
				memset(&ginfo.car[i], 0, sizeof(struct cars));
				ginfo.car[i].id = i;
				strcpy(ginfo.car[i].car, car);
				ginfo.car[i].cash =atoi(cash);
				ginfo.car[i].sell= ginfo.car[i].cash*8/10;
			}


		} // if strlen > 0
	} //while readf.good()

	readf.close();
}

void btn_shop (struct player *splayer)
{
	//btn_main(splayer);
	int j;
	for (j =0 ; j< MAX_PLAYERS; j++)
	{
		if (bank->players[j].UCID == splayer->UCID)
		{
			break;
		}

	}

	struct IS_BTN pack;
	memset(&pack, 0, sizeof(struct IS_BTN));
	pack.Size = sizeof(struct IS_BTN);
	pack.Type = ISP_BTN;
	pack.ReqI = 1;
	pack.UCID = splayer->UCID;
	pack.Inst = 0;
	pack.TypeIn = 0;


	pack.BStyle = 16;
	pack.ClickID = 150;
	pack.L = 39;
	pack.T = 56;
	pack.W = 33;
	pack.H = 8;
	strcpy(pack.Text, msg.message[splayer->lang_id][600]);
	insim.send_packet(&pack);
	pack.BStyle = 16+8;
	pack.ClickID = 151;
	pack.L = 39;
	pack.T = 56+9;
	pack.W = 16;
	strcpy(pack.Text, msg.message[splayer->lang_id][602]);
	insim.send_packet(&pack);
	pack.BStyle = 16+8;
	pack.ClickID = 152;
	pack.L += 17;
	pack.W = 16;
	strcpy(pack.Text, msg.message[splayer->lang_id][603]);
	insim.send_packet(&pack);
	pack.BStyle = 16;
	pack.ClickID = 153;
	pack.L = 39+34;
	pack.T = 56;
	pack.W = 33;
	pack.H = 8;
	strcpy(pack.Text, msg.message[splayer->lang_id][601]);
	insim.send_packet(&pack);
	pack.BStyle = 16+8;
	pack.ClickID = 154;
	pack.T = 56+9;
	pack.W = 16;
	strcpy(pack.Text, msg.message[splayer->lang_id][602]);
	insim.send_packet(&pack);
	pack.BStyle = 16+8;
	pack.ClickID = 155;
	pack.L += 17;
	pack.W = 16;
	strcpy(pack.Text, msg.message[splayer->lang_id][603]);
	insim.send_packet(&pack);

	// Заплатка для магазина
	char Cars[255];
	ZeroMemory(&Cars, 255);
	for (int i=1; i<MAX_CARS; i++)
	{
		for (int j=1; j<MAX_CARS; j++)
		{
			if ((strcmp(ginfo.car[i].car, splayer->cars[j].car) == 0) and (strlen(ginfo.car[i].car) > 0))
				strcat(Cars, splayer->cars[j].car);
		}
	}
	//cout << Cars << endl;


	if (splayer->Shop == 1)
	{


		pack.L = 39;
		pack.W = 33;
		pack.H = 8;

		for (int i=2; i<9; i++)
		{
			pack.ClickID = 50 + i;
			pack.T = 65+9*(i-1);

			if ((bank->players[j].Cash > ginfo.car[i].cash) and (strstr(Cars, ginfo.car[i].car)==0))
			{
				pack.BStyle = 4+16+8;
				strcpy(pack.Text, "^2");
			}
			else
			{
				pack.BStyle = 4+32;
				strcpy(pack.Text, "^1");
			}

			strcat(pack.Text, ginfo.car[i].car);
			insim.send_packet(&pack);
		}

		if (strcmp(ginfo.Product, "S2")==0)
		{
			pack.L = 39+34;
			pack.W = 33;
			pack.T = 65+9;


			for (int i=11; i<19; i++)
			{
				pack.ClickID = 50 + i;
				pack.T = 65+9*(i-10);
				if ((bank->players[j].Cash > ginfo.car[i].cash) and (strstr(Cars, ginfo.car[i].car)==0))
				{
					pack.BStyle = 4+16+8;
					strcpy(pack.Text, "^2");
				}
				else
				{
					pack.BStyle = 4+32;
					strcpy(pack.Text, "^1");
				}
				strcat(pack.Text, ginfo.car[i].car);
				insim.send_packet(&pack);
			}
		}

	} // if b_type == 1

	if (splayer->Shop == 2)
	{

		pack.L = 39;
		pack.W = 33;
		pack.H = 8;
		pack.BStyle = 4+16+8;

		int b = 1;
		for (int i=2; i<9; i++)
		{
			// out << ginfo.car[i].car << " " << splayer->Cars << endl;
			// out << (int)strstr(splayer->Cars, ginfo.car[i].car) << endl;
			if (strstr(Cars, ginfo.car[i].car))
			{
				pack.ClickID = 50 + i;
				pack.T = 65+9*b;
				strcpy(pack.Text, "^2");
				strcat(pack.Text, ginfo.car[i].car);
				insim.send_packet(&pack);
				b++;
			}
		}

		if (strcmp(ginfo.Product, "S2")==0)
		{
			pack.L = 39+34;
			pack.W = 33;
			pack.T = 65+9;
			b=1;
			for (int i=1; i<9; i++)
			{
				// out << ginfo.car[10+i].car << " " << splayer->Cars << endl;
				// out << (int)strstr(splayer->Cars, ginfo.car[10+i].car) << endl;
				if (strstr(Cars, ginfo.car[10+i].car))
				{
					pack.ClickID = 60 + i;
					pack.T = 65+9*b;
					strcpy(pack.Text, "^2");
					strcat(pack.Text, ginfo.car[10+i].car);
					insim.send_packet(&pack);
					b++;
				}
			}
		}


	}

	if (splayer->Shop == 3)
	{


		pack.L = 39;
		pack.W = 33;
		pack.H = 8;
		pack.BStyle = 4+16+8;

		int b = 1;
		if (!(splayer->CTune&1) and bank->players[j].Cash > 5000)
		{
			pack.ClickID = 71;
			pack.T = 65+9*b;
			strcpy(pack.Text, "^2ECU");
			insim.send_packet(&pack);
			b++;
		}
		if (!(splayer->CTune&2) and (splayer->CTune&1) and bank->players[j].Cash > 10000)
		{
			pack.ClickID = 72;
			pack.T = 65+9*b;
			strcpy(pack.Text, "^2Turbo");
			insim.send_packet(&pack);
			b++;
		}

		if (!(splayer->CTune&8) and bank->players[j].Cash > 20000)
		{
			pack.ClickID = 73;
			pack.T = 65+9*b;
			strcpy(pack.Text, "^2ABS");
			insim.send_packet(&pack);
			b++;
		}

		pack.BStyle = 16+8;
		pack.ClickID = 74;
		pack.L = 39+34;
		pack.W = 33;
		pack.H = 8;
		pack.T = 65+9;
		pack.TypeIn = 16;
		strcpy(pack.Text, "^C^2Окраска(^1без ^7CAR_^2)");
		//insim.send_packet(&pack);



	}

	if (splayer->Shop == 4)
	{


		pack.L = 39;
		pack.W = 33;
		pack.H = 8;
		pack.BStyle = 4+16+8;

		int b = 1;
		if (splayer->CTune&1)
		{
			pack.ClickID = 71;
			pack.T = 65+9*b;
			strcpy(pack.Text, "^2ECU");
			insim.send_packet(&pack);
			b++;
		}
		if (splayer->CTune&2)
		{
			pack.ClickID = 72;
			pack.T = 65+9*b;
			strcpy(pack.Text, "^2Turbo");
			insim.send_packet(&pack);
			b++;
		}

		if (splayer->CTune&8)
		{
			pack.ClickID = 73;
			pack.T = 65+9*b;
			strcpy(pack.Text, "^2ABS");
			insim.send_packet(&pack);
			b++;
		}

	}

	pack.BStyle = 8 + 16;
	pack.ClickID = 149;
	pack.TypeIn =0;
	pack.L = 39 +34 +34;
	pack.T = 56;
	pack.W = 16;
	pack.H = 8;
	strcpy(pack.Text, msg.message[splayer->lang_id][604]);
	insim.send_packet(&pack);
}

void RCCore::case_btc ()
{
//    pthread_mutex_lock (&RCmutex);
	struct IS_BTC *pack_btc = (struct IS_BTC*)insim->get_packet();

	//cout << (int)pack_btc->ClickID << endl;
	int i;
	for (i=0; i < MAX_PLAYERS; i++)
	{
		if (players[i].UCID == pack_btc->UCID)
		{

			char day[3];
			char month[3];
			char year[3];
			SYSTEMTIME sm;
			GetLocalTime(&sm);
			itoa(sm.wDay, day, 10);
			itoa(sm.wMonth, month, 10);
			itoa(sm.wYear, year, 10);
			char log[255];
			strcpy(log, RootDir);
			strcat(log, "logs\\shop\\shop");
			strcat(log, "(");
			strcat(log, day);
			strcat(log, ".");
			strcat(log, month);
			strcat(log, ".");
			strcat(log, year);
			strcat(log, ").txt");

			if (pack_btc->ClickID<=32)
			{
				players[i].BID2 =  pack_btc->ClickID;

				if (players[i].Action == 1) // send
				{
					struct IS_BTN pack_btn;
					memset(&pack_btn, 0, sizeof(struct IS_BTN));
					pack_btn.Size = sizeof(struct IS_BTN);
					pack_btn.Type = ISP_BTN;
					pack_btn.ReqI = players[i].UCID;              // Must be non-zero, I'll just use UCID
					pack_btn.UCID = players[i].UCID;              // UCID of the player that will receive the button
					pack_btn.BStyle = 16 + ISB_CLICK;                 // Dark frame for window title
					pack_btn.TypeIn = 16;
					pack_btn.L = 116;
					pack_btn.T = 13;
					pack_btn.W = 34;
					pack_btn.H = 5;
					pack_btn.ClickID = 36;
					strcpy(pack_btn.Text, msg->message[players[i].lang_id][1000]);
					insim->send_packet(&pack_btn);

					pack_btn.TypeIn = 64-24-14;
					pack_btn.L = 116;
					pack_btn.T += 6;
					pack_btn.W = 34;
					pack_btn.H = 5;
					pack_btn.ClickID = 37;
					strcpy(pack_btn.Text, msg->message[players[i].lang_id][1001]);
					insim->send_packet(&pack_btn);

					// cop buttons
					if (players[i].cop != 0)
					{
						pack_btn.BStyle = 32 + ISB_CLICK;
						pack_btn.TypeIn = 2;
						pack_btn.L = 116;
						pack_btn.T += 6;
						pack_btn.W = 34;
						pack_btn.H = 5;
						pack_btn.ClickID = 38;
						strcpy(pack_btn.Text, msg->message[players[i].lang_id][1002]);
						insim->send_packet(&pack_btn);

						pack_btn.BStyle = 32 + ISB_CLICK;
						pack_btn.TypeIn = 2;
						pack_btn.L = 116;
						pack_btn.T += 6;
						pack_btn.W = 34;
						pack_btn.H = 5;
						pack_btn.ClickID = 39;
						strcpy(pack_btn.Text, msg->message[players[i].lang_id][1003]);
						insim->send_packet(&pack_btn);

						pack_btn.BStyle = 32 + ISB_CLICK;
						pack_btn.TypeIn = 0;
						pack_btn.L = 116;
						pack_btn.T += 6;
						pack_btn.W = 34;
						pack_btn.H = 5;
						pack_btn.ClickID = 40;
						strcpy(pack_btn.Text, msg->message[players[i].lang_id][1004]);
						insim->send_packet(&pack_btn);

						pack_btn.BStyle = 32 + ISB_CLICK;
						pack_btn.TypeIn = 0;
						pack_btn.L = 116;
						pack_btn.T += 6;
						pack_btn.W = 34;
						pack_btn.H = 5;
						pack_btn.ClickID = 41;
						strcpy(pack_btn.Text, msg->message[players[i].lang_id][1005]);
						insim->send_packet(&pack_btn);
					}
				}
			}
			if (pack_btc->ClickID==34)
			{
				for (int j=0; j<50; j++)
					send_bfn(players[i].UCID, j);
				Sleep(100);
			}

			if (pack_btc->ClickID==40)
			{

				for (int g=0; g<MAX_PLAYERS; g++)
				{
					if  (players[i].BID2 == players[g].BID)
					{
						if (players[g].Pogonya == 0)
						{
							players[g].Pogonya = 1;
							int worktime = time(&stime);
							players[g].WorkTime = worktime+60*6;
							strcpy(players[g].PogonyaReason, msg->message[players[g].lang_id][1006]);
							char Text[96];
							strcpy(Text, "/msg ^2| ");
							//strcat(Text, players[i].PName);
							strcat(Text, msg->message[players[g].lang_id][1007]);
							strcat(Text, players[g].PName);
							send_mst(Text);
						}
						break;
					}
				}
			}
			if (pack_btc->ClickID==41)
			{

				for (int g=0; g<MAX_PLAYERS; g++)
				{
					if  (players[i].BID2 == players[g].BID)
					{
						if (players[g].Pogonya != 0)
						{
							players[g].Pogonya = 0;
							send_bfn(players[g].UCID, 210);
							char Text[96];
							strcpy(Text, "/msg ^2| ");
							//strcat(Text, players[i].PName);
							strcat(Text, msg->message[players[g].lang_id][1008]);
							strcat(Text, players[g].PName);
							send_mst(Text);
						}
						break;
					}
				}
			}


			for (int g=50; g< 70; g++)
			{
				if (g == pack_btc->ClickID)
				{
					cout << players[i].UName << "come in to shop and \n";
					if (players[i].Shop == 1)
					{
						//strcat(players[i].Cars, ginfo.car[g-50].car);
						//strcat(players[i].Cars, " ");
						bank->players[i].Cash -= ginfo.car[g-50].cash;
						bank->BankFond += ginfo.car[g-50].cash;
						cout << "buy car - " << ginfo.car[g-50].car << endl;




						ofstream readf (log, ios::app);
						readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[i].UName << " buy car " << ginfo.car[g-50].car << endl;
						readf.close();

						for ( int j=0; j<MAX_CARS; j++)
						{
							if (strlen(players[i].cars[j].car) == 0)
							{
								strcpy(players[i].cars[j].car, ginfo.car[g-50].car);
								players[i].cars[j].tuning=0;
								players[i].cars[j].dist=0;
								break;
							}

						}



						for (int j=40; j<200; j++)
							send_bfn(pack_btc->UCID, j);

						players[i].Shop = 1;
						btn_shop(&players[i]);
					}
					if (players[i].Shop == 2)
					{
						if (strcmp(players[i].CName, ginfo.car[g-50].car)!=0)
						{
							cout << "sell car - " << ginfo.car[g-50].car << endl;



							ofstream readf (log, ios::app);

							readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << ":" << sm.wMilliseconds << " " <<  players[i].UName << " sell car " << ginfo.car[g-50].car << endl;


							/*string Cars;
							Cars += players[i].Cars;
							cout << Cars << endl;
							Cars.replace(Cars.find(ginfo.car[g-50].car), 4, "");
							cout << Cars << endl;

							strcpy(players[i].Cars, Cars.c_str());*/
							// readf << "DEBAG:" << bank->players[i].Cash << endl;
						   // bank->players[i].Cash += ginfo.car[g-50].sell;
						   // bank->BankFond -= ginfo.car[g-50].sell;

							//readf << "DEBAG:" << bank->players[i].Cash << endl;
							//cout << players[i].Cars << endl;
							readf.close();
							for ( int j=0; j<MAX_CARS; j++)
							{
								if (strcmp(players[i].cars[j].car, ginfo.car[g-50].car) == 0)
								{
									strcpy(players[i].cars[j].car, "");
									players[i].cars[j].tuning=0;
									players[i].cars[j].dist=0;
									break;
								}

							}


							for (int j=40; j<200; j++)
								send_bfn(pack_btc->UCID, j);

							players[i].Shop = 2;
							btn_shop(&players[i]);
						}
						else
						{
							send_mtc(players[i].UCID, msg->message[players[i].lang_id][1009]);
						}
					}
					break;
				} // if g == clickId
			} // for g



			if (pack_btc->ClickID == 71)
			{
				if (players[i].Shop == 3)
				{

					players[i].CTune += 1;
					bank->players[i].Cash -= 5000;
					bank->BankFond += 5000;


					ofstream readf (log, ios::app);
					readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[i].UName << " buy ECU "<< endl;
					readf.close();

					for (int j=40; j<200; j++)
						send_bfn(pack_btc->UCID, j);

					players[i].Shop = 3;
					btn_shop(&players[i]);
				}
				if (players[i].Shop == 4)
				{

					players[i].CTune -= 1;
					bank->players[i].Cash += 5000*8/10;
					bank->BankFond -= 5000*8/10;

					ofstream readf (log, ios::app);
					readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[i].UName << " sell ECU "<< endl;
					readf.close();

					for (int j=40; j<200; j++)
						send_bfn(pack_btc->UCID, j);

					players[i].Shop = 4;
					btn_shop(&players[i]);
				}
			}
			if (pack_btc->ClickID == 72)
			{
				if (players[i].Shop == 3)
				{

					players[i].CTune += 2;
					bank->players[i].Cash -= 10000;
					bank->BankFond += 10000;


					ofstream readf (log, ios::app);
					readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[i].UName << " buy Turbo "<< endl;
					readf.close();

					for (int j=40; j<200; j++)
						send_bfn(pack_btc->UCID, j);

					players[i].Shop = 3;
					btn_shop(&players[i]);
				}
				if (players[i].Shop == 4)
				{

					players[i].CTune -= 2;
					bank->players[i].Cash += 10000*8/10;
					bank->BankFond -= 10000*8/10;


					ofstream readf (log, ios::app);
					readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[i].UName << " sell Turbo "<< endl;
					readf.close();

					for (int j=40; j<200; j++)
						send_bfn(pack_btc->UCID, j);

					players[i].Shop = 4;
					btn_shop(&players[i]);
				}
			}
			if (pack_btc->ClickID == 73)
			{
				if (players[i].Shop == 3)
				{

					players[i].CTune += 8;
					bank->players[i].Cash -= 20000;
					bank->BankFond += 20000;


					ofstream readf (log, ios::app);
					readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[i].UName << " buy ABS "<< endl;
					readf.close();

					for (int j=40; j<159; j++)
						send_bfn(pack_btc->UCID, j);

					players[i].Shop = 3;
					btn_shop(&players[i]);
				}
				if (players[i].Shop == 4)
				{

					players[i].CTune -= 8;
					bank->players[i].Cash += 20000*8/10;
					bank->BankFond -= 20000*8/10;


					ofstream readf (log, ios::app);
					readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[i].UName << " sell ABS "<< endl;
					readf.close();

					for (int j=40; j<159; j++)
						send_bfn(pack_btc->UCID, j);

					players[i].Shop = 4;
					btn_shop(&players[i]);
				}
			}


			if (pack_btc->ClickID == 151)
			{
				for (int j=40; j<159; j++)
					send_bfn(pack_btc->UCID, j);
				players[i].Shop = 1;
				btn_shop(&players[i]);
			}
			if (pack_btc->ClickID == 152)
			{
				for (int j=40; j<159; j++)
					send_bfn(pack_btc->UCID, j);
				players[i].Shop = 2;
				btn_shop(&players[i]);
			}

			if (pack_btc->ClickID == 154)
			{
				for (int j=40; j<159; j++)
					send_bfn(pack_btc->UCID, j);
				players[i].Shop = 3;
				btn_shop(&players[i]);
			}
			if (pack_btc->ClickID == 155)
			{
				for (int j=40; j<159; j++)
					send_bfn(pack_btc->UCID, j);
				players[i].Shop = 4;
				btn_shop(&players[i]);
			}

			if (pack_btc->ClickID == 149)
			{
				for (int j=159; j>0; j--)
				{
					send_bfn(pack_btc->UCID, j);
					Sleep(10);
				}
			}

			if (pack_btc->ClickID == 103)
			{
				for (int j=0; j<50; j++)
					send_bfn(pack_btc->UCID, 100+j);
				btn_info(&players[i], 1);
			}

			if (pack_btc->ClickID == 104)
			{
				for (int j=0; j<50; j++)
					send_bfn(pack_btc->UCID, 100+j);

				btn_info(&players[i], 2);
			}

			if (pack_btc->ClickID == 105)
			{
				for (int j=0; j<50; j++)
					send_bfn(pack_btc->UCID, 100+j);

				btn_info(&players[i], 3);
			}
			if (pack_btc->ClickID == 106)
			{
				for (int j=0; j<50; j++)
					send_bfn(pack_btc->UCID, 100+j);

				btn_info(&players[i], 4);
			}


			if (pack_btc->ClickID == 200)
			{
				for (int j=0; j<5; j++)
					send_bfn(pack_btc->UCID, 200+j);
			}

			break;
		} // if UCID
	}// for
	pthread_mutex_unlock (&RCmutex);
	Sleep(100);
}



