#include "RCPolice.h"

RCPolice::RCPolice()
{

}

RCPolice::~RCPolice()
{

}

int RCPolice::init(const char *dir,void *CInSim, void *Message,void *Bank,void *RCdl, void * STreet)
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

    return 0;
}

void RCPolice::insim_ncn( struct IS_NCN* packet )
{
	cout << "RCPolice ncn" << endl;
	if( packet->UCID == 0 )
		return;

	strcpy(players[ packet->UCID ].UName, packet->UName);
    strcpy(players[ packet->UCID ].PName, packet->PName);

	ReadUserFines( packet->UCID );
}

void RCPolice::insim_npl( struct IS_NPL* packet )
{
	PLIDtoUCID[ packet->PLID ] = packet->UCID;
	strcpy(players[packet->UCID].CName ,packet->CName);
}

void RCPolice::insim_plp( struct IS_PLP* packet )
{
	PLIDtoUCID.erase( packet->PLID );
}

void RCPolice::insim_pll( struct IS_PLL* packet )
{
	PLIDtoUCID.erase( packet->PLID );
}

void RCPolice::insim_cnl( struct IS_CNL* packet )
{
	SaveUserFines( packet->UCID );
	players.erase( packet->UCID );
}

void RCPolice::insim_cpr( struct IS_CPR* packet )
{
	strcpy( players[ packet->UCID ].PName, packet->PName);
}

void RCPolice::insim_mso( struct IS_MSO* packet )
{
	if (packet->UCID == 0)
        return;

	char Msg[128];
    strcpy(Msg,packet->Msg + ((unsigned char)packet->TextStart));


    if ((strncmp(Msg, "!fines", 6) == 0) or (strncmp(Msg, "!^Cштрафы", 9) == 0 ))
    {
        int j=0;
        for (int i=0; i<MAX_FINES; i++)
        {
            if (players[ packet->UCID ].fines[i].fine_id > 0 and players[ packet->UCID ].fines[i].fine_id < MAX_FINES )
            {
                char Text[64];
                // rcMainRow[0] = username
                // rcMainRow[1] = fine_id
                // rcMainRow[2] = fine_date

                int fine_id = players[ packet->UCID ].fines[i].fine_id;
                // int fine_date = atoi(rcMainRow[2));

                sprintf(Text,"^2| ^7ID = %d. %.64s ^3(^2%d RUR.^3)", fine_id , fines[fine_id].name , fines[fine_id].cash );
                send_mtc( packet->UCID ,Text);

                j++;
            }
        }

        if (j == 0)
        {
            send_mtc( packet->UCID ,msg->GetMessage( packet->UCID ,3102));;
        }
    }

     if ((strncmp(Msg, "!pay", 4) == 0 ) or (strncmp(Msg, "!^Cоплатить", 11) == 0 ))
    {

        char GetMessage2[96];
        strcpy(GetMessage2,Msg);

        char * comand;
        char * id;

        comand = strtok (GetMessage2," ");
        id = strtok (NULL," ");

        int id_i = atoi(id);

        if ((!id) or (id_i < 1))
        {
            send_mtc( packet->UCID ,msg->GetMessage( packet->UCID ,2105));
            return;
        }

        if (bank->InBank( packet->UCID ))
        {
            if (fines[id_i].cash<bank->GetCash( packet->UCID ))
            {
                int j=0;
                for (j=0; j<MAX_FINES; j++)
                {
                    if (players[ packet->UCID ].fines[j].fine_id == id_i)
                    {
                        players[ packet->UCID ].fines[j].fine_id = 0;
                        players[ packet->UCID ].fines[j].fine_date = 0;

                        bank->RemCash( packet->UCID ,fines[id_i].cash);

                       int cop = 0;
                       for ( auto& play: players)
                        {
                            if ( players[ play.first ].cop == 1)
                            {
                                if (dl->Islocked(  play.first  ))
                                {
                                    dl->Unlock(  play.first  );
                                    dl->AddSkill( play.first , 0.05);
                                    dl->Lock(  play.first  );
                                }
                                else
                                    dl->AddSkill( play.first , 0.05);

                                bank->AddCash( play.first ,(fines[id_i].cash)*0.05, true);
                                cop++;
                            }
                        }

                        bank->AddToBank((fines[id_i].cash)-((fines[id_i].cash)*0.05)*cop);
                        send_mtc( packet->UCID ,msg->GetMessage( packet->UCID ,2106));
                        dl->RemSkill( packet->UCID );
                        break;
                    }

                    if (j == MAX_FINES)
                        send_mtc( packet->UCID ,msg->GetMessage( packet->UCID ,2107));
                }
            }
            else send_mtc( packet->UCID ,"^1| ^C^7На вашем счете недостаточно средств для оплаты штрафа");
        }
        else send_mtc( packet->UCID ,"^1| ^C^7Вы находитесь не в банке");
    }


}

void RCPolice::insim_con( struct IS_CON* packet )
{

}

void RCPolice::insim_obh( struct IS_OBH* packet )
{

}

void RCPolice::insim_btc( struct IS_BTC* packet )
{
	if ( packet->ClickID <= 32 )
	{
		players[ packet->UCID ].BID2 =  packet->ClickID;
	}
}

void RCPolice::insim_btt( struct IS_BTT* packet )
{
	/**
	Пользователь выписывает штраф
	*/
	if ( packet->ClickID == 38 )
	{
		for ( auto& play: players )
		{
			if  ( players[ packet->UCID ].BID2 == players[ play.first ].BID)
			{
				if (atoi(packet->Text) > 0)
				{

					for (int j = 0; j < MAX_FINES; j++)
					{
						if( fines[j].id == atoi(packet->Text) )
						{
							char Msg[64];
							strcpy(Msg,msg->GetMessage( play.first ,1104));
							send_mtc( play.first ,Msg);
							strcpy(Msg,"^2| ^7");
							strcat(Msg,fines[atoi(packet->Text)].name);
							send_mtc( play.first ,Msg);

							strcpy(Msg,msg->GetMessage( packet->UCID ,1105));
							send_mtc( packet->UCID ,Msg);
							send_mtc( packet->UCID ,fines[atoi(packet->Text)].name);

							strcpy(Msg,msg->GetMessage( packet->UCID ,1106));
							strcat(Msg,players[ play.first ].PName);
							send_mtc( packet->UCID ,Msg);

							for (int j=0; j<MAX_FINES; j++)
							{
								if (players[ play.first ].fines[j].fine_id == 0)
								{
									players[ play.first ].fines[j].fine_id = atoi(packet->Text);
									players[ play.first ].fines[j].fine_date = int( time( NULL ) );
									break;
								}
							}

							/*ofstream readf (fine_c,ios::app);
							readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[i].UName << " get fine ID = " << packet->Text << " to "  << players[g].UName << endl;
							readf.close();*/
						}
					}
				} // if atoi(pack_btt->Text) > 0
				break;
			}
		}//for
	}

	/**
	Пользователь отменяет штраф
	*/
	if (packet->ClickID==39)
	{
		for ( auto& play: players )
		{
			if  (players[ packet->UCID ].BID2 == players[ play.first ].BID)
			{
				if ( atoi( packet->Text ) > 0 )
				{
					for (int j=0; j<MAX_FINES; j++)
					{
						if ( players[ play.first ].fines[j].fine_id == atoi( packet->Text ) )
						{
							char Msg[64];
							strcpy(Msg,msg->GetMessage( play.first ,1107));
							send_mtc( play.first ,Msg);
							strcpy(Msg,"^2| ");
							strcat(Msg,fines[atoi(packet->Text)].name);
							send_mtc( play.first ,Msg);

							strcpy(Msg,msg->GetMessage( packet->UCID ,1108));
							send_mtc( packet->UCID ,Msg);
							send_mtc( packet->UCID ,fines[atoi(packet->Text)].name);

							strcpy(Msg,msg->GetMessage( play.first ,1106));
							strcat(Msg,players[ play.first ].PName);
							send_mtc( packet->UCID ,Msg);

							players[ play.first ].fines[j].fine_id = 0;
							players[ play.first ].fines[j].fine_date = 0;

							/*ofstream readf (fine_c,ios::app);
							readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[i].UName << " cancle fine ID = " << pack_btt->Text << " to "  << players[g].UName << endl;
							readf.close();*/

							break;
						}
					}
				} // if atoi(pack_btt->Text) > 0
				break;
			}
		}//for
	}
}

void RCPolice::insim_pen( struct IS_PEN* packet )
{
	byte UCID = PLIDtoUCID[ packet->PLID ];

	if (packet->Reason == PENR_WRONG_WAY)
	{
		for (int j=0; j<MAX_FINES; j++)
		{
			if (players[ UCID ].fines[j].fine_id == 0)
			{
				players[ UCID ].fines[j].fine_id = 18;
				players[ UCID ].fines[j].fine_date = int(time(NULL));

				char Msg[64];
				strcpy(Msg,msg->GetMessage(UCID ,1104));
				send_mtc( UCID ,Msg);
				strcpy(Msg,"^2| ^7");
				strcat(Msg,fines[18].name);
				send_mtc( UCID ,Msg);
				break;
			}
		}
	}

	if((packet->NewPen != 0) and (packet->Reason == PENR_SPEEDING))
	{
		players[ UCID ].Penalty = 1;
		for (int j=0; j<MAX_FINES; j++)
		{
			if (players[ UCID ].fines[j].fine_id == 0)
			{
				players[ UCID ].fines[j].fine_id = 13;
				players[ UCID ].fines[j].fine_date = int(time(NULL));

				char Msg[64];
				strcpy(Msg,msg->GetMessage( UCID ,1104));
				send_mtc( UCID ,Msg);
				strcpy(Msg,"^2| ^7");
				strcat(Msg,fines[13].name);
				send_mtc( UCID ,Msg);
				break;
			}
		}
	}
}

void RCPolice::insim_pla( struct IS_PLA* packet )
{
	byte UCID = PLIDtoUCID[ packet->PLID ];

	if (packet->Fact == PITLANE_EXIT)
	{
		int count = 0;
		for (int j=0; j<MAX_FINES; j++)
		{
			if (players[ UCID ].fines[j].fine_id != 0)
				count++;
		}

		if (count > 10)
		{
			char Text[64];
			sprintf(Text, "/pitlane %s",players[ UCID ].UName);
			send_mtc( UCID ,msg->GetMessage( UCID ,3400));
			send_mst(Text);
		}
	}
}

void RCPolice::CopTurnOn( byte UCID )
{
	players[ UCID ].cop = 1;
}

void RCPolice::CopTurnOff( byte UCID )
{
	players[ UCID ].cop = 0;
}

void RCPolice::SetUserBID ( byte UCID, byte BID )
{
	players[ UCID ].BID = BID;
}

void RCPolice::SaveUserFines ( byte UCID )
{
    cout <<players[ UCID ].UName << " save fines_info" << endl;

    char file[255];
    sprintf(file,"%sdata\\RCPolice\\fines\\%s.txt", RootDir, players[ UCID ].UName);

    ofstream writef (file,ios::out);
    for (int i = 0; i < MAX_FINES; i++)
    {
        if (players[ UCID ].fines[i].fine_id > 0)
        {
            writef << players[ UCID ].fines[i].fine_id << ";" << players[ UCID ].fines[i].fine_date <<  endl;
        }
    }

    writef.close();
}

void RCPolice::ReadUserFines( byte UCID )
{
    char file[255];
    sprintf(file,"%sdata\\RCPolice\\fines\\%s.txt", RootDir, players[ UCID ].UName);

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        cout << "Can't find " << file << endl;
        SaveUserFines( UCID );
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


                players[ UCID ].fines[i].fine_id = atoi(id);
                players[ UCID ].fines[i].fine_date = atoi(date);

                i++;
            }
        }
    }
}

void RCPolice::ReadFines()
{

    char file[255];
    strcpy(file,RootDir);
    sprintf(file,"%smisc\\fines.txt" , RootDir);

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        cout << "Can't find " << file << endl;
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
            if (strstr(str,"//"))
                continue;

            char * id;
            char * name;
            char * cash;
            id = strtok (str,";");
            name = strtok (NULL,";");
            cash = strtok (NULL,";");

            i = atoi(id);
            memset(&fines[i],0,sizeof(struct fine));
            fines[i].id = i;
            strcpy(fines[i].name, name);
            fines[i].cash = atoi(cash);

        } // if strlen > 0
    } //while readf.good()

    readf.close();
}
