#include "RCPolice.h"
RCPolice::RCPolice(){memset( &fines, 0, sizeof( fine ) * MAX_FINES );}
RCPolice::~RCPolice(){}

int RCPolice::init(const char *dir,void *CInSim, void *Message,void *Bank,void *RCdl, void * STreet, void *Energy, void *Light)
{
    strcpy(RootDir,dir);
    insim = (CInsim *)CInSim;
    if(!insim)
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
    nrg = (RCEnergy *)Energy;
    if(!nrg)
    {
        printf ("Can't struct RCEnergy class");
        return -1;
    }
    lgh = (RCLight *)Light;
    if(!lgh)
    {
        printf ("Can't struct RCLight class");
        return -1;
    }
    return 0;
}

//done
void RCPolice::InsimNCN( struct IS_NCN* packet )
{
    if( packet->UCID == 0 )
        return;

	players[packet->UCID].UCID = packet->UCID;
    strcpy(players[packet->UCID].UName, packet->UName);
    strcpy(players[packet->UCID].PName, packet->PName);

	//читаем штрафы
    ReadUserFines(packet->UCID);
}

//done
void RCPolice::InsimNPL( struct IS_NPL* packet )
{
	PLIDtoUCID[packet->PLID] = packet->UCID;
    strcpy(players[packet->UCID].CName ,packet->CName);

	if (strncmp("^4[^C^7ДПС^4]",players[packet->UCID].PName,13)==0 || strncmp("^4[^C^7ГАИ^4]",players[packet->UCID].PName,13)==0)
	if (ReadCop(packet->UCID))
	{
        players[packet->UCID].cop = true;
    	SendMTC(packet->UCID, msg->_(packet->UCID, "CopTrue"));

		if (!((strncmp("DPS",packet->SName,3)==0)
		|| (strncmp("dps",packet->SName,3)==0)
        || (strncmp("POLICE",packet->SName,6)==0)
        || (strncmp("police",packet->SName,6)==0)))
        SendMTC(packet->UCID, "^2| ^C^7В названии скина не найдено указателей на принадлежность к ДПС");

        dl->Lock(packet->UCID);
        nrg->Lock(packet->UCID);
        lgh->SetLight3(packet->UCID, true);
    }
    else
    {
        SendMTC(packet->UCID, msg->_(packet->UCID ,"CopFalse"));

		char text[96];
		sprintf(text, "/spec %s", players[packet->UCID].UName);
        SendMST(text);
    }
}

//done
void RCPolice::InsimPLP( struct IS_PLP* packet )
{
    lgh->SetLight3(PLIDtoUCID[packet->PLID], false);
    dl->Unlock(PLIDtoUCID[packet->PLID]);
    nrg->Unlock(PLIDtoUCID[packet->PLID]);

    PLIDtoUCID.erase( packet->PLID );
}

//done
void RCPolice::InsimPLL( struct IS_PLL* packet )
{
    lgh->SetLight3(PLIDtoUCID[packet->PLID], false);
    dl->Unlock(PLIDtoUCID[packet->PLID]);
    nrg->Unlock(PLIDtoUCID[packet->PLID]);

    PLIDtoUCID.erase( packet->PLID );
}

//done
void RCPolice::InsimCNL( struct IS_CNL* packet )
{
    SaveUserFines(packet->UCID);
    players.erase(packet->UCID);
}

//done
void RCPolice::InsimCPR( struct IS_CPR* packet )
{
    strcpy(players[packet->UCID].PName, packet->PName);
    if (players[packet->UCID].cop and !ReadCop(packet->UCID))
		players[packet->UCID].cop = false;
}

void RCPolice::InsimMSO( struct IS_MSO* packet )
{
    if (packet->UCID == 0) return;
    if ( packet->UserType != MSO_PREFIX ) return;

    char Msg[128];
    strcpy(Msg,packet->Msg + ((unsigned char)packet->TextStart));

    if ((strncmp(Msg, "!fines", 6) == 0) or (strncmp(Msg, "!^Cштрафы", 9) == 0 ))
    {
        int j=0;
        for (int i=0; i<MAX_FINES; i++)
        {
            if (players[ packet->UCID ].fines[i].fine_id > 0 and players[ packet->UCID ].fines[i].fine_id < MAX_FINES )
            {
                char Text[128], Date[128];
                int fine_id = players[ packet->UCID ].fines[i].fine_id;

                sprintf(Text,"^2| ^7ID = %d. %.64s (^2%d RUR^7)", fine_id , fines[fine_id].name , fines[fine_id].cash );
                if (strlen(players[packet->UCID].fines[i].CopPName.c_str()) > 0)
				{
					strcat(Text," - ");
					strcat(Text,players[packet->UCID].fines[i].CopPName.c_str());
				}
				else if (strlen(players[packet->UCID].fines[i].CopName.c_str()) > 0)
				{
					strcat(Text," - ");
					strcat(Text,players[packet->UCID].fines[i].CopName.c_str());
				}

				struct tm * timeinfo;
				time_t t = (time_t)players[packet->UCID].fines[i].fine_date;
				timeinfo = localtime (&t);
				strftime (Date,128,", ^2%d.%m.%y",timeinfo);
				strcat(Text,Date);
                SendMTC( packet->UCID ,Text);
                j++;
            }
        }

        if (j == 0)
            SendMTC(packet->UCID ,msg->_(  packet->UCID , "NoFines" ));
    }

    if ((strncmp(Msg, "!pay", 4) == 0 ) or (strncmp(Msg, "!^Cоплатить", 11) == 0 ))
    {
        char _2[128];
        strcpy(_2,Msg);

        char * comand;
        char * id;

        comand = strtok (_2," ");
        id = strtok (NULL," ");

        int id_i = atoi(id);

        if ((!id) or (id_i < 1))
        {
            SendMTC( packet->UCID ,msg->_(  packet->UCID , "2105" ));
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

                        int cop = 0;
                        for ( auto& play: players)
                        {
                            if ( players[ play.first ].cop and packet->UCID != play.first)
                            {
                                if (dl->Islocked(  play.first  ))
                                {
                                    dl->Unlock( play.first );
                                    dl->AddSkill( play.first , 0.05);
                                    dl->Lock( play.first );
                                }
                                else
								{
									dl->AddSkill( play.first , 0.05);
								}

                                bank->AddCash( play.first ,(fines[id_i].cash)*0.05, true);
                                cop++;
                            }
                        }

						SendMTC( packet->UCID ,msg->_(  packet->UCID , "PayDone" ));
						bank->RemCash( packet->UCID ,fines[id_i].cash);
                        bank->AddToBank((fines[id_i].cash)-((fines[id_i].cash)*0.05)*cop);

						if (dl->Islocked( packet->UCID ))
						{
							dl->Unlock( packet->UCID );
							dl->RemSkill( packet->UCID );
							dl->Lock( packet->UCID );
						}
						else
							dl->RemSkill( packet->UCID );
                        break;
                    }

                    if (j == MAX_FINES)
                        SendMTC( packet->UCID, msg->_(  packet->UCID , "NoFine" ));
                }
            }
            else SendMTC(packet->UCID, msg->_(packet->UCID, "NoManyPay"));
        }
        else SendMTC(packet->UCID, msg->_(packet->UCID, "NoOnBank"));
    }

    if ((strncmp(Msg, "!sirena", 7) == 0 ) or (strncmp(Msg, "!^Cсирена", 9) == 0 ))
        if (players[packet->UCID].cop)
        {
            if (players[packet->UCID].Sirena)
            {
                SendMTC(packet->UCID, msg->_(packet->UCID, "2100"));
                players[packet->UCID].Sirena = false;
            }
            else
            {
                SendMTC(packet->UCID, msg->_(packet->UCID, "2101"));
                players[packet->UCID].Sirena = true;
            }
        }

    if ((strncmp(Msg, "!radar", 6) == 0 ) or (strncmp(Msg, "!^Cрадар", 8) == 0 ))
        if (players[packet->UCID].cop)
        {
            if (players[packet->UCID].Radar)
            {
                SendMTC(packet->UCID, msg->_(packet->UCID, "RadarOff"));
                players[packet->UCID].Radar = false;
            }
            else
            {
                SendMTC(packet->UCID, msg->_(packet->UCID, "RadarOn"));
                players[packet->UCID].Radar = true;
            }
        }

    if (strncmp(Msg, "!kick", 4) == 0 )
    {
        char user[16];
        strcpy(user,Msg+5);

        if(strlen(user)>0)
        {
            if (players[ packet->UCID ].cop )
            {
                char Kick[64];
                sprintf(Kick,"/kick %s",user);
                SendMST(Kick);

                SYSTEMTIME sm;
                GetLocalTime(&sm);
                char log[MAX_PATH];
                sprintf(log,"%slogs\\cop\\kick(%d.%d.%d).txt",RootDir,sm.wYear,sm.wMonth,sm.wDay);
                ofstream readf (log,ios::app);
                readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << ":" << sm.wMilliseconds << " " <<  players[ packet->UCID ].UName << " kick " << user << endl;
                readf.close();
            }
        }
    }

    if ( strcmp( players[ packet->UCID ].UName ,"denis-takumi") == 0 or strcmp( players[ packet->UCID ].UName ,"Lexanom") == 0 )
    {
        char file[255];
        sprintf(file,"%sdata\\RCPolice\\cops.txt",RootDir);

        char line[32];
        if (strncmp(Msg, "!cop_add", 8) == 0 )
        {
            char param[16];
            strcpy(param,Msg+9);

            if(strlen(param)>0)
            {
                ofstream wCops( file , ios::app );
                wCops << param <<  endl;
                wCops.close();
            }
        }

        if (strncmp(Msg, "!cop_del", 8) == 0 )
        {
            char param[16];
            strcpy(param,Msg+9);
            printf("%s\n",param);
            if(strlen(param)>0)
            {
                vector<string>cops;

                ifstream rCops( file , ios::in );

                while ( rCops.good() )
                {
                    memset(&line,0,32);
                    rCops.getline(line, 32);

                    if (strlen(line) >0)
                        cops.push_back(line);
                }
                rCops.close();

                ofstream wCops( file , ios::out );

                for( int i = 0; i < cops.size(); i++)
                {
                    if ( cops[i] != param )
                        wCops << cops[i].c_str() << endl;

                }
                wCops.close();

            }
        }
        if (strncmp(Msg, "!cop_list", 9) == 0 )
        {
            ifstream rCops( file , ios::in );

            while ( rCops.good() )
            {

                memset(&line,0,32);
                rCops.getline(line, 32);
                if (strlen(line) >0)
                    SendMTC(packet->UCID,line);
            }
            rCops.close();
        }
    }
}

void RCPolice::ShowFinesPanel( byte UCID, byte UCID2 )
{
	char Text[128];
	byte c = GetFineCount();

	byte
		id=84, 				//стартовый ид кнопок
		id2=110,
		l=100, 				//не менять
		t=90,				//не менять
		hButton=5, 			//высота строки
		w=100, 				//ширина поля
		h=10+c*hButton; 	//высота поля

	sprintf(Text,"^C^7Панель выписки штрафов (^8%s^7)", players[UCID2].PName);

	SendButton(255,UCID,81,l-w/2,t-h/2,w,h+8,32,""); 				//фон
	SendButton(255,UCID,82,l-w/2,t-h/2,w,h+8,32,"");				//фон
	SendButton(255,UCID,83,l-w/2,t-h/2,w,10,64,Text); 				//заголовок
	SendButton(254,UCID,80,l-7,t-h/2+h+1,14,6,16+ISB_CLICK,"^2OK"); //закрывашка

	for(int i=0;i<c;i++)
	{
		byte fid = fines[i+1].id; if (fid==0) fid = 255;
		sprintf(Text,"^2%02d. ^7%s", fines[i+1].id, fines[i+1].name);

		SendButton(fid,UCID,id++,l-w/2+1,t-h/2+10+hButton*i,w-12,hButton,16+64+8,Text);
		SendButton(fid,UCID,id2++,l+w/2-11,t-h/2+10+hButton*i,10,hButton,16+8,"^CОтмена");
	}
}

void RCPolice::InsimBTC( struct IS_BTC* packet )
{
	if ( packet->ClickID == 130 and packet->ReqI == 254 )
	{
		players[packet->UCID].ThisFineCount=0;
		for(int i=128;i<165;i++) SendBFN(packet->UCID,i);
		for(int j=0;j<20;j++) strcpy(players[packet->UCID].ThisFine[j],"");
	}

	if ( packet->ClickID == 80 and packet->ReqI == 254 ) //погоня офф
	{
		for(int i=80;i<165;i++)
			SendBFN(packet->UCID,i);
	}

	if ( packet->ClickID >= 84 and packet->ClickID < 110 and packet->ReqI != 254 and packet->ReqI != 255) //выписка штрафа
	{
		SYSTEMTIME sm;
		GetLocalTime(&sm);
		char fine_c[255];
		sprintf(fine_c,"%slogs\\fines\\fine(%d.%d.%d).txt",RootDir,sm.wYear,sm.wMonth,sm.wDay);

		for ( auto& play: players )
        {
            if  ( players[packet->UCID].BID2 == players[play.first].BID)
            {
                char Msg[128];
                sprintf(Msg,msg->_(play.first,"GiveFine"),players[packet->UCID].PName,fines[packet->ReqI].name);
                SendMTC( play.first, Msg);

                sprintf(Msg,msg->_(  packet->UCID , "AddFine" ),players[play.first].PName,fines[packet->ReqI].name);
                SendMTC( packet->UCID ,Msg);

                for (int j=0; j<MAX_FINES; j++)
                {
                    if (players[ play.first ].fines[j].fine_id == 0)
                    {
                        players[ play.first ].fines[j].fine_id = packet->ReqI;
                        players[ play.first ].fines[j].fine_date = int( time( NULL ) );
                        players[ play.first ].fines[j].CopName = players[ packet->UCID ].UName;
                        players[ play.first ].fines[j].CopPName = players[ packet->UCID ].PName;
                        break;
                    }
                }
                ofstream readf (fine_c,ios::app);
                readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[ packet->UCID ].UName << " get fine ID = " << packet->ReqI << " to "  << players[ play.first ].UName << endl;
                readf.close();

                if(players[play.first].ThisFineCount==0) SendBFNAll(play.first);
                for(int j=0; j<20; j++)
                    if( strlen(players[play.first].ThisFine[j]) == 0 )
                    {
                        sprintf(players[play.first].ThisFine[j],"^7%d. %s (^2ID = %d^7)   -   %s",j+1,fines[packet->ReqI].name,fines[packet->ReqI].id,players[ packet->UCID ].PName);
                        players[play.first].ThisFineCount++;
                        break;
                    }
                break;
            }
        }
	}

	if ( packet->ClickID >= 110 and packet->ClickID < 130 and packet->ReqI != 255 and packet->ReqI != 254) //выписка штрафа
	{
		SYSTEMTIME sm;
		GetLocalTime(&sm);
		char fine_c[255];
		sprintf(fine_c,"%slogs\\fines\\fine(%d.%d.%d).txt",RootDir,sm.wYear,sm.wMonth,sm.wDay);

        for ( auto& play: players )
        {
            if  (players[ packet->UCID ].BID2 == players[ play.first ].BID)
            {
                for (int j=0; j<MAX_FINES; j++)
                {
                    if ( players[ play.first ].fines[j].fine_id == packet->ReqI )
                    {
                        char Msg[128];
                        sprintf(Msg,msg->_( play.first, "DeletedFine"),players[packet->UCID].PName,fines[packet->ReqI].name);
                        SendMTC( play.first ,Msg);

                        sprintf(Msg,msg->_(packet->UCID, "DelFine"),players[ play.first ].PName,fines[packet->ReqI].name);
                        SendMTC( packet->UCID ,Msg);

                        players[ play.first ].fines[j].fine_id = 0;
                        players[ play.first ].fines[j].fine_date = 0;
                        ofstream readf (fine_c,ios::app);
                        readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[ packet->UCID ].UName << " cancle fine ID = " << packet->ReqI << " to "  << players[ play.first ].UName << endl;
                        readf.close();

                        if (players[play.first].ThisFineCount>0)
                            for(int j=0; j<20; j++)
                                if( strlen(players[play.first].ThisFine[j]) == 0 )
                                {
                                    sprintf(players[play.first].ThisFine[j],"   ^1^KЎї ^7%s (^2ID = %d^7)   -   %s",fines[packet->ReqI].name,fines[packet->ReqI].id,players[ packet->UCID ].PName);
                                    players[play.first].ThisFineCount++;
                                    break;
                                }
                        break;
                    }
                }
                break;
            }
        }
	}

    if ( packet->ClickID <= 32 )
        players[ packet->UCID ].BID2 =  packet->ClickID;

    /** Штрафы **/
    if (packet->ClickID==38 and players[packet->UCID].cop)
		ShowFinesPanel(packet->UCID, packet->ReqI);

    /** Включаем погоню **/
    if (packet->ClickID==40)
    {
        for ( auto& play: players )
        {
            if  (players[ packet->UCID ].BID2 == players[ play.first ].BID)
            {
                if (players[play.first].Pogonya == 0)
                {
                    players[play.first].Pogonya = 1;
                    int worktime = time(NULL);
                    players[play.first].WorkTime = worktime+60*5;
                    strcpy(players[ play.first ].PogonyaReason,msg->_(  play.first , "1006" ));
                    char Text[96];
                    sprintf(Text,msg->_(play.first, "PogonyaOn" ), players[ play.first ].PName, players[ packet->UCID ].PName );
                    SendMTC(255, Text);
                    nrg->Lock( play.first );
                }
            }

            if ( players[ play.first ].cop )
                for (int k=60; k<79; k++)
                    SendBFN( play.first ,k);
        }
    }

    /** Выключаем погоню **/
    if (packet->ClickID==41)
    {
        for ( auto& play: players )
        {
            if  (players[ packet->UCID ].BID2 == players[ play.first ].BID)
            {
                if (players[play.first].Pogonya != 0)
                {
                    players[ play.first ].Pogonya = 0;
                    SendBFN( play.first ,210);
                    char Text[96];
                    sprintf(Text,msg->_(play.first , "PogonyaOff" ), players[ play.first ].PName, players[ packet->UCID ].PName );
                    SendMTC(255, Text);
                    nrg->Unlock( play.first );
                }
            }

            if ( players[ play.first ].cop )
            {
                for (int k=60; k<79; k++)
                    SendBFN( play.first ,k);
            }
        }
    }
}

void RCPolice::InsimBTT( struct IS_BTT* packet )
{
	SYSTEMTIME sm;
    GetLocalTime(&sm);

    char fine_c[255];
    sprintf(fine_c,"%slogs\\fines\\fine(%d.%d.%d).txt",RootDir,sm.wYear,sm.wMonth,sm.wDay);

    /** Пользователь выписывает штраф **/
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
                            char Msg[128];
                            sprintf(Msg,msg->_(play.first,"GiveFine"),players[packet->UCID].PName,fines[atoi(packet->Text)].name);
                            SendMTC( play.first, Msg);

                            sprintf(Msg,msg->_(  packet->UCID , "AddFine" ),players[play.first].PName,fines[atoi(packet->Text)].name);
                            SendMTC( packet->UCID ,Msg);

                            for (int j=0; j<MAX_FINES; j++)
                            {
                                if (players[ play.first ].fines[j].fine_id == 0)
                                {
                                    players[ play.first ].fines[j].fine_id = atoi(packet->Text);
                                    players[ play.first ].fines[j].fine_date = int( time( NULL ) );
                                    players[ play.first ].fines[j].CopName = players[ packet->UCID ].UName;
                                    players[ play.first ].fines[j].CopPName = players[ packet->UCID ].PName;
                                    break;
                                }
                            }
                            ofstream readf (fine_c,ios::app);
                            readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[ packet->UCID ].UName << " get fine ID = " << packet->Text << " to "  << players[ play.first ].UName << endl;
                            readf.close();

							if(players[play.first].ThisFineCount==0) SendBFNAll(play.first);
                            for(int j=0;j<20;j++)
							if( strlen(players[play.first].ThisFine[j]) == 0 )
							{
								sprintf(players[play.first].ThisFine[j],"^7%d. %s (^2ID = %d^7)   -   %s",j+1,fines[atoi(packet->Text)].name,fines[atoi(packet->Text)].id,players[ packet->UCID ].PName);
								players[play.first].ThisFineCount++;
								break;
							}
                        }
                    }
                }
                break;
            }
        }
    }

    /** Пользователь отменяет штраф **/
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
                            char Msg[128];
                            sprintf(Msg,msg->_( play.first, "DeletedFine"),players[packet->UCID].PName,fines[atoi(packet->Text)].name);
                            SendMTC( play.first ,Msg);

                            sprintf(Msg,msg->_(packet->UCID, "DelFine"),players[ play.first ].PName,fines[atoi(packet->Text)].name);
                            SendMTC( packet->UCID ,Msg);

                            players[ play.first ].fines[j].fine_id = 0;
                            players[ play.first ].fines[j].fine_date = 0;
                            ofstream readf (fine_c,ios::app);
                            readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[ packet->UCID ].UName << " cancle fine ID = " << packet->Text << " to "  << players[ play.first ].UName << endl;
                            readf.close();

							if (players[play.first].ThisFineCount>0)
                            for(int j=0;j<20;j++)
							if( strlen(players[play.first].ThisFine[j]) == 0 )
							{
								sprintf(players[play.first].ThisFine[j],"   ^1^KЎї ^7%s (^2ID = %d^7)   -   %s",fines[atoi(packet->Text)].name,fines[atoi(packet->Text)].id,players[ packet->UCID ].PName);
								players[play.first].ThisFineCount++;
								break;
							}
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
}

void RCPolice::InsimPEN( struct IS_PEN* packet )
{
    byte UCID = PLIDtoUCID[ packet->PLID ];

    if (packet->Reason == PENR_WRONG_WAY )
    {
        for (int j=0; j<MAX_FINES; j++)
        {
            if (players[ UCID ].fines[j].fine_id == 0)
            {
                players[ UCID ].fines[j].fine_id = 18;
                players[ UCID ].fines[j].fine_date = int(time(NULL));

                char Msg[96];
                sprintf(Msg,msg->_( UCID , "GiveFine2" ),fines[18].name);
                SendMTC( UCID ,Msg);
                break;
            }
        }
    }
    else if (packet->Reason == PENR_SPEEDING )
    {

		if( packet->NewPen == PENALTY_30 or
			packet->NewPen == PENALTY_45 or
			packet->NewPen == PENALTY_DT or
			packet->NewPen == PENALTY_SG )
		{
			for (int j=0; j<MAX_FINES; j++)
			{
				if (players[ UCID ].fines[j].fine_id == 0)
				{
					players[ UCID ].fines[j].fine_id = 13;
					players[ UCID ].fines[j].fine_date = int(time(NULL));

					char Msg[64];
					sprintf(Msg,msg->_( UCID , "GiveFine2" ),fines[13].name);
					SendMTC( UCID ,Msg);
					break;
				}
			}
		}
    }
}

void RCPolice::InsimPLA( struct IS_PLA* packet )
{
    byte UCID = PLIDtoUCID[ packet->PLID ];

    if (packet->Fact == PITLANE_EXIT)
    {
		char Text[64];
		sprintf(Text, "/p_clear %s", players[ UCID ].UName);
		SendMST(Text);

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
            SendMTC( UCID ,msg->_(  UCID , "3400" ));
            SendMST(Text);
        }
    }
}

void RCPolice::InsimMCI( struct IS_MCI* packet )
{
    for (int i = 0; i < packet->NumC; i++)
    {
        byte UCID = PLIDtoUCID[packet->Info[i].PLID];
        if(UCID == 0) return;

        /** окошко со штрафами **/
		if ( players[UCID].ThisFineCount != 0 )
		{
			byte ClickID=CLICKID::CLICK_ID_128, w=90, h=10+5*players[UCID].ThisFineCount, l=100, t=90;
			SendButton(255, UCID, ClickID++,l-w/2,t-h/2,w,h+8,32,"");
			SendButton(255, UCID, ClickID++,l-w/2,t-h/2,w,h+8,32,"");									//фон
			SendButton(254, UCID, ClickID++, l-7, t-h/2+h+1, 14, 6, ISB_LIGHT + ISB_CLICK,"^2OK"); 		//закрывашка
			SendButton(255, UCID, ClickID++,l-w/2,t-h/2,w,10,3+64,msg->_(UCID,"GiveFine3")); 			//заголовок

			for(int j=0; j < players[UCID].ThisFineCount; j++)
				SendButton(255, UCID, ClickID++, l-w/2+1, t-h/2+10+5*j, w-2, 5,ISB_LEFT + ISB_LIGHT, players[UCID].ThisFine[j]);
		}

        /** автоотключение радара **/
        int S = packet->Info[i].Speed*360/32768;
        if (S > 5 and players[UCID].Radar)
		{
			SendMTC(UCID, msg->_(UCID, "RadarOff"));
			players[UCID].Radar = false;
        }

        /** сирена копа **/
        if (players[UCID].cop and players[UCID].Sirena)
			SendButton(255, UCID, 20, 90, 26, 20, 10, 32, siren.c_str());
		else
			SendBFN(UCID, 20);
		int SirenaCount = 0;

		int SDtemp=0;
		for (auto& play: players)
		{
			if(UCID == play.first or !players[play.first].cop) continue;
			int X1 = players[UCID].Info.X/65536,
				Y1 = players[UCID].Info.Y/65536,
				X2 = players[play.first].Info.X/65536,
				Y2 = players[play.first].Info.Y/65536,
				Dist = Distance(X1, Y1, X2, Y2);

			/** сирена **/
			if (players[play.first].Sirena)
			{
				if (Dist < 120)
				{
					SirenaCount++;
					if (Dist < SDtemp or SDtemp ==0)
						SDtemp = Dist;
				}
			}
			players[UCID].SirenaDist = SDtemp;

			/** радар **/
			if (players[play.first].Radar and !players[UCID].cop and players[UCID].Pogonya != 1)
			{
				if (Dist<25)
				{
					int Speed = players[UCID].Info.Speed*360/32768;
					struct streets StreetInfo;
					street->CurentStreetInfo(&StreetInfo,UCID);

					if (Speed > StreetInfo.SpeedLimit+10 and Speed > players[UCID].speed_over)
						players[UCID].speed_over=Speed;
				}
				else if (players[UCID].speed_over>0)
				{
					struct streets StreetInfo;
					street->CurentStreetInfo(&StreetInfo,UCID);

					char text[128];
					int Speed2 = players[UCID].speed_over - StreetInfo.SpeedLimit;
					sprintf(text,msg->_(UCID, "Speeding"), players[UCID].PName, Speed2, StreetInfo.Street, players[play.first].PName);
					SendMTC(255,text);

					if (players[UCID].Pogonya == 0)
					{
						players[UCID].Pogonya = 1;
						int worktime = time(NULL);
						players[UCID].WorkTime = worktime+60*5;
						strcpy(players[UCID].PogonyaReason,msg->_(UCID,"1006"));
						sprintf(text,msg->_(UCID, "PogonyaOn" ), players[UCID].PName, players[play.first].PName );
						SendMTC(255, text);
						nrg->Lock( UCID );
					}
					players[UCID].speed_over=0;
				}
			}

			/** арест **/
			if (players[UCID].Pogonya == 1)
			{
				int S2 = players[UCID].Info.Speed*360/32768;
				if (S2 < 5 and Dist < 10)
				{
					if (players[UCID].StopTime >= 3)
					{
						players[UCID].Pogonya = 2;
						nrg->Unlock(UCID);
						strcpy(players[UCID].PogonyaReason, msg->_(UCID, "1701" ));

						char Text[96];
						sprintf(Text,"^2| %s%s", players[UCID].PName, msg->_(UCID, "1702"));
						SendMTC(255,Text);
						SendMTC(UCID,msg->_(UCID, "1703"));
					}
					players[UCID].StopTime++;
				}
				else players[UCID].StopTime = 0;
			}
		}

		/** сирена у игрока **/
		if (SirenaCount > 0)
			ShowSirena(UCID);
		else if (players[UCID].SirenaDist > 0 or SirenaCount == 0)
		{
			players[UCID].SirenaDist = 0;
			HideSirena(UCID);
		}

        if (players[UCID].Pogonya == 0 and strlen(players[UCID].PogonyaReason) > 1)
        {
            strcpy(players[ UCID ].PogonyaReason,"");
            SendBFN( UCID ,204);
        }

        if (players[UCID].Pogonya != 0)
            BtnPogonya(UCID);

        memcpy(&players[UCID].Info, &packet->Info[i], sizeof(CompCar));
    }
}

bool RCPolice::ReadCop(byte UCID)
{
    char PlayerName[32];
    strcpy(PlayerName,players[UCID].PName);

    char file[255];
    sprintf(file,"%sdata\\RCPolice\\cops.txt",RootDir);
    FILE *fff = fopen(file, "r");
    if ( NULL == fff ) return -1;
    fclose(fff);
    ifstream readf (file,ios::in);

    char str[32];
    while (readf.good())
    {
        readf.getline(str,32);
        if(strlen(str) > 0 and strcmp(players[UCID].UName, str) == 0)
            if ((strncmp("^4[^C^7ДПС^4]",PlayerName,13)==0) || (strncmp("^4[^C^7ГАИ^4]",PlayerName,13)==0))
            {
				readf.close();
				return true;
            }
    }
    return false;
}

void RCPolice::SetSirenLight( string sirenWord )
{
    siren = sirenWord;
}

void RCPolice::ShowSirena(byte UCID)
{
	char txt[50];
	SendButton(255,UCID,21,0,31,200,(120-players[UCID].SirenaDist+1)/4, 0, siren.c_str());
}

void RCPolice::HideSirena(byte UCID)
{
	SendBFN(UCID, 21);
}

bool RCPolice::IsCop( byte UCID )
{
    return players[UCID].cop;
}

int RCPolice::InPursuite( byte UCID )
{
    return players[UCID].Pogonya;
}

int RCPolice::GetFineCount()
{
	int c=0;
	for (int i=0; i<MAX_FINES; i++)
	{
		if (fines[i].id != 0)
		c++;
	}
    return c;
}

void RCPolice::BtnPogonya( byte UCID )
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.ClickID = 204;
    pack.BStyle = 1;
    pack.L = 50;
    pack.T = 51;
    pack.W = 100;
    pack.H = 30;
    strcpy(pack.Text,players[ UCID ].PogonyaReason);
    insim->send_packet(&pack);
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
            writef << players[ UCID ].fines[i].fine_id << ";" << players[ UCID ].fines[i].fine_date << ";" << players[ UCID ].fines[i].CopName << ";" << players[ UCID ].fines[i].CopPName <<  endl;
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
                char *CopName;
                char *CopPName;

                id = strtok(str,";");
                date = strtok(NULL,";");
                CopName = strtok(NULL,";");
                CopPName = strtok(NULL,";");

                players[ UCID ].fines[i].fine_id = atoi(id);
                if( date )
					players[ UCID ].fines[i].fine_date = atoi(date);

				if( CopName )
					players[ UCID ].fines[i].CopName = CopName;

				if( CopPName )
					players[ UCID ].fines[i].CopPName = CopPName;

                i++;
            }
        }
    }
}

void RCPolice::ReadFines()
{
    char file[255];
    strcpy(file,RootDir);
    sprintf(file,"%sdata\\RCPolice\\fines.txt" , RootDir);

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
            strncpy(fines[i].name, name, 64);
            fines[i].cash = atoi(cash);

        } // if strlen > 0
    } //while readf.good()

    readf.close();
}

void RCPolice::ButtonClock( byte UCID )
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.ClickID = 210;
    pack.BStyle = 32;
    pack.L = 136;
    pack.T = 1;
    pack.W = 10;
    pack.H = 8;

    int time2 = players[ UCID ].WorkTime - time(NULL);
    int min;
    int sec;
    min = time2/60;
    sec = time2%60;
    sprintf(pack.Text,"^2%02d:%02d",min,sec);

    insim->send_packet(&pack);
}

void RCPolice::Event()
{
    for( auto& play: players )
    {
        byte UCID = play.first;
        auto playr = play.second;

        if ( playr.Pogonya == 1 )
        {
            ButtonClock( UCID );
            int nowtime = time( NULL );
            if ( playr.WorkTime <= nowtime)
            {
                SendBFN( UCID ,210);

                char Text[64];
                sprintf( Text , "/msg ^2| %s %s" , playr.PName , msg->_(  UCID , "1706" ) );

                SendMST(Text);
                players[ UCID ].Pogonya = 0;
                nrg->Unlock( UCID );
                dl->AddSkill( UCID );
            }
        }

        if ( playr.cop )
        {
            struct IS_BTN pack;
            memset(&pack, 0, sizeof(struct IS_BTN));
            pack.Size = sizeof(struct IS_BTN);
            pack.Type = ISP_BTN;
            pack.ReqI = 1;
            pack.UCID =  UCID ;
            pack.Inst = 0;
            pack.TypeIn = 0;
            pack.ClickID = CLICKID::CLICK_ID_60;
            pack.BStyle = 32;
            pack.L = 73;
            pack.T = 191;
            pack.W = 50;
            pack.H = 4;

            streets StreetInfo;

            for ( auto& play2: players)
            {
                byte UCID2 = play2.first;
                auto playr2 = play2.second;
                if ( playr2.Pogonya != 0 )
                {
                    int time2 = playr2.WorkTime - time(NULL);
                    int min = (time2/60)%60;
                    int sec = time2%60;

                    float D = Distance( playr.Info.X, playr.Info.Y, playr2.Info.X, playr2.Info.Y)/65536;

                    street->CurentStreetInfo(&StreetInfo,UCID2);

                    if (playr2.Pogonya == 1)
                        sprintf(pack.Text,"%s^7 - %s, %0.0f ^Cм ^2(^1%02d:%02d^2)", playr2.PName, StreetInfo.Street, D, min, sec );
                    else if (playr2.Pogonya == 2)
                        sprintf(pack.Text,"%s ^7 - %s, ^1^Cарестован", playr2.PName,StreetInfo.Street);

                    insim->send_packet(&pack);
                    pack.T -=4;
                    pack.ClickID ++;

                }
            }
        }
    }
}
