#include "RCPolice.h"

RCPolice::RCPolice()
{
	memset( &fines, 0, sizeof( fine ) * MAX_FINES );
}

RCPolice::~RCPolice()
{

}

int RCPolice::init(const char *dir,void *CInSim, void *Message,void *Bank,void *RCdl, void * STreet, void *Energy, void *Light)
{
    strcpy(RootDir,dir); // �������� ���� �� ���������
    insim = (CInsim *)CInSim; // ����������� ��������� ������� ������
    if(!insim)
    {
        printf ("Can't struct CInsim class");    // ��������� �� �������������
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

void RCPolice::insim_ncn( struct IS_NCN* packet )
{
    if( packet->UCID == 0 )
        return;

    strcpy(players[ packet->UCID ].UName, packet->UName);
    strcpy(players[ packet->UCID ].PName, packet->PName);

    ReadUserFines( packet->UCID );
}

void RCPolice::insim_npl( struct IS_NPL* packet )
{
    PLIDtoUCID[ packet->PLID ] = packet->UCID;
    players[ packet->UCID ].cop = false;
    strcpy(players[packet->UCID].CName ,packet->CName);

    char specText[64];
    sprintf(specText, "/spec %s", players[ packet->UCID ].UName);

    if (IfCop(  packet->UCID  ) == 1)
    {
        send_mst( specText );
        send_mtc( packet->UCID ,msg->GetMessage( packet->UCID ,1300));
        return;
    }
    else if (IfCop(  packet->UCID  ) == 3)
    {
        send_mtc( packet->UCID ,msg->GetMessage( packet->UCID ,1301));

        players[ packet->UCID ].cop = true;

        dl->Lock( packet->UCID );
        nrg->Lock( packet->UCID );
        lgh->SetLight3( packet->UCID ,true);
    }
}

void RCPolice::insim_plp( struct IS_PLP* packet )
{
    lgh->SetLight3( PLIDtoUCID[ packet->PLID ] ,false);
    dl->Unlock( PLIDtoUCID[ packet->PLID ] );
    nrg->Unlock( PLIDtoUCID[ packet->PLID ] );

    players[ PLIDtoUCID[ packet->PLID ] ].Penalty =0;
    PLIDtoUCID.erase( packet->PLID );
}

void RCPolice::insim_pll( struct IS_PLL* packet )
{
    lgh->SetLight3( PLIDtoUCID[ packet->PLID ] ,false);
    dl->Unlock( PLIDtoUCID[ packet->PLID ] );
    nrg->Unlock( PLIDtoUCID[ packet->PLID ] );

    players[ PLIDtoUCID[ packet->PLID ] ].Penalty =0;
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

    if ( packet->UserType != MSO_PREFIX )
        return;

    char Msg[128];
    strcpy(Msg,packet->Msg + ((unsigned char)packet->TextStart));


    if ((strncmp(Msg, "!fines", 6) == 0) or (strncmp(Msg, "!^C������", 9) == 0 ))
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

    if ((strncmp(Msg, "!pay", 4) == 0 ) or (strncmp(Msg, "!^C��������", 11) == 0 ))
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
                            if ( players[ play.first ].cop )
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
            else send_mtc( packet->UCID ,"^1| ^C^7�� ����� ����� ������������ ������� ��� ������ ������");
        }
        else send_mtc( packet->UCID ,"^1| ^C^7�� ���������� �� � �����");
    }

    if ((strncmp(Msg, "!sirena", 7) == 0 ) or (strncmp(Msg, "!^C������", 9) == 0 ))
    {

        if (players[ packet->UCID ].cop )
        {
            if (players[ packet->UCID ].sirena ==0 )
            {
                send_mtc( packet->UCID ,msg->GetMessage( packet->UCID ,2100));
                players[ packet->UCID ].sirena = 1;
            }
            else
            {
                send_mtc( packet->UCID ,msg->GetMessage( packet->UCID ,2101));
                players[ packet->UCID ].sirena = 0;
            }
        }
    }

    if ((strncmp(Msg, "!radar", 6) == 0 ) or (strncmp(Msg, "!^C�����", 8) == 0 ))
    {

        if (players[ packet->UCID ].cop )
        {
            if (players[ packet->UCID ].radar ==0 )
            {
                send_mtc( packet->UCID ,msg->GetMessage( packet->UCID ,2102));
                players[ packet->UCID ].radar = 1;
            }
            else
            {
                send_mtc( packet->UCID ,msg->GetMessage( packet->UCID ,2103));
                players[ packet->UCID ].radar = 0;
            }
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
                send_mst(Kick);

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

    if ( strcmp( players[ packet->UCID ].UName ,"denis-takumi") == 0 )
    {
        char file[255];
        sprintf(file,"%smisc\\cops.txt",RootDir);

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
                char cops[40][32];
                memset(&cops,0,40*32);

                int j = 0;

                ifstream rCops( file , ios::in );

                while ( rCops.good() )
                {
                    memset(&line,0,32);
                    rCops.getline(line, 32);

                    if (strlen(line) >0)
                        strncpy(cops[j++],line,32);
                }
                rCops.close();

                ofstream wCops( file , ios::out );

                for(j=0; j<40; j++)
                {
                    if (strncmp(cops[j],param,strlen(param)) != 0)
                    {
                        wCops << cops[j] << endl;
                    }
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
                    send_mst(line);
            }
            rCops.close();
        }
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

    /**
    �������� ������
    */
    if (packet->ClickID==40)
    {
        for ( auto& play: players )
        {
            if  (players[ packet->UCID ].BID2 == players[ play.first ].BID)
            {
                if (players[ play.first ].Pogonya == 0)
                {
                    players[ play.first ].Pogonya = 1;
                    int worktime = time(NULL);
                    players[ play.first ].WorkTime = worktime+60*6;
                    strcpy(players[ play.first ].PogonyaReason,msg->GetMessage( play.first ,1006));
                    char Text[96];
                    sprintf(Text,"/msg ^2| %s %s", msg->GetMessage( play.first ,1007) , players[ play.first ].PName );
                    send_mst(Text);
                    nrg->Lock( play.first );

                    /*char fine_c[255];
                    sprintf(fine_c,"%slogs\\cop\\pursuit(%d.%d.%d).txt",RootDir,sm.wYear,sm.wMonth,sm.wDay);
                    ofstream readf (fine_c,ios::app);
                    readf << sm.wHour << ":" << sm.wMinute << ":" << sm.wSecond << " " <<  players[i].UName << " begin pursuit to "  << players[g].UName << endl;
                    readf.close();*/
                }

            }

            if ( players[ play.first ].cop )
            {
                for (int k=60; k<79; k++)
                    send_bfn( play.first ,k);
            }
        }

    }

    /**
    ��������� ������
    */
    if (packet->ClickID==41)
    {
        for ( auto& play: players )
        {
            if  (players[ packet->UCID ].BID2 == players[ play.first ].BID)
            {
                if (players[ play.first ].Pogonya != 0)
                {
                    players[ play.first ].Pogonya = 0;
                    send_bfn( play.first ,210);
                    char Text[96];
                    sprintf(Text,"/msg ^2| %s %s", msg->GetMessage( play.first ,1008) , players[ play.first ].PName );
                    send_mst(Text);
                    nrg->Unlock( play.first );
                }

            }

            if ( players[ play.first ].cop )
            {
                for (int k=60; k<79; k++)
                    send_bfn( play.first ,k);
            }
        }
    }

	if (packet->ClickID == 106)
	{

		struct IS_BTN pack;
		memset(&pack, 0, sizeof(struct IS_BTN));
		pack.Size = sizeof(struct IS_BTN);
		pack.Type = ISP_BTN;
		pack.ReqI = 1;
		pack.UCID = packet->UCID;
		pack.Inst = 0;
		pack.TypeIn = 0;

		int fineID = 0;
		for (int i=0; i<MAX_FINES; i++)
		{
			pack.L = (101-126/2)+1;
			pack.BStyle = 16 + 64;
			if (fines[i].id != 0)
			{
				fineID ++;
				pack.T = 56+6*(fineID-1);
				pack.W = 122;
				pack.H = 6;
				pack.ClickID = 110 + fineID;
				sprintf(pack.Text,"^7ID = %d. %s ^3(^2%d RUR.^3)", fines[i].id, fines[i].name, fines[i].cash);
				insim->send_packet(&pack);
			}

		}
	}
}

void RCPolice::insim_btt( struct IS_BTT* packet )
{
    /**
    ������������ ���������� �����
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
    ������������ �������� �����
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
                sprintf(Msg,"^2| ^7%s", fines[18].name);
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
                sprintf(Msg,"^2| ^7%s" ,fines[13].name);
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
        if ( players[ UCID ].Penalty != 0)
        {
            char Text[64];
            sprintf(Text, "/p_clear %s", players[ UCID ].UName);
            send_mst(Text);
        }

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

void RCPolice::insim_mci( struct IS_MCI* packet )
{
    for (int i = 0; i < packet->NumC; i++)
    {
        byte UCID = PLIDtoUCID[ packet->Info[i].PLID ];
        if( UCID == 0 )
            return;

        int S = players[ UCID ].Info.Speed*360/32768;
        /** �������������� ������ **/
        if (S < 5)
            players[ UCID ].StopTime ++;
        else
        {
            players[ UCID ].StopTime = 0;
            if (players[ UCID ].radar ==1 )
            {
                send_mtc( UCID ,msg->GetMessage( UCID ,1700));
                players[ UCID ].radar = 0;
            }
        }

        if (players[ UCID ].cop )
        {
            for ( auto& play: players )
            {
                byte UCID2 = play.first;

                int X = players[ UCID2 ].Info.X/65536;
                int Y = players[ UCID2 ].Info.Y/65536;
                int X1 = players[ UCID ].Info.X/65536;
                int Y1 = players[ UCID ].Info.Y/65536;

                int Rast = dl->Distance( X, Y, X1, Y1);

                if (players[ UCID2 ].Pogonya == 1)
                {
                    cout << players[ UCID2 ].UName << endl;
                    if ( (Rast < 10) and (!players[ UCID2 ].cop))
                    {
                        int S2 = players[ UCID2 ].Info.Speed*360/32768;

                        if ((S2 < 5) and (players[ UCID2 ].StopTime > 4))
                        {

                            players[ UCID2 ].Pogonya = 2;
                            nrg->Unlock( UCID2 );
                            strcpy(players[ UCID2 ].PogonyaReason,msg->GetMessage( UCID2 ,1701));

                            char Text[96];
                            sprintf(Text,"/msg ^2| %s%s", players[ UCID2 ].PName, msg->GetMessage( UCID ,1702));
                            send_mst(Text);

                            send_mtc( UCID ,msg->GetMessage( UCID ,1703));
                        }
                    }
                } // pogonya
                /**
                �����
                */
                if (players[ UCID ].radar ==1 )
                {
                    if ((Rast < 50 ) and ( !players[ UCID2 ].cop ))
                    {
                        int Speed = players[ UCID2 ].Info.Speed*360/32768;
                        struct streets StreetInfo;
                        street->CurentStreetInfo(&StreetInfo, UCID2 );

                        if ((Speed > StreetInfo.SpeedLimit+10) )
                        {
                            char text[64];
                            int Speed2 = Speed - StreetInfo.SpeedLimit;
                            sprintf(text,"^2| %s%s%d%s",players[ UCID2 ].PName,msg->GetMessage( UCID2 ,1704),Speed2,msg->GetMessage( UCID ,1705));
                            send_mtc( UCID ,text);

                            if (players[ UCID2 ].Pogonya == 0)
                            {
                                players[ UCID2 ].Pogonya = 1;
                                int worktime = time(NULL);
                                players[ UCID2 ].WorkTime = worktime+60*6;
                                strcpy(players[ UCID2 ].PogonyaReason,msg->GetMessage( UCID2 ,1006));
                                char Text[96];
                                sprintf(Text,"/msg ^2| %s%s", msg->GetMessage( UCID2 ,1007) , players[ UCID2 ].PName );
                                send_mst(Text);
                                nrg->Lock( UCID2 );
                            }
                        }
                    }
                }
                /**
                ������
                */
                if (players[ UCID ].sirena ==1)
                {
                    if ( (Rast < 120) and ( !players[ UCID2 ].cop ) )
                    {
                        players[ UCID2 ].sirenaOnOff = 1;
                        players[ UCID2 ].sirenaKey = 1;
                        players[ UCID2 ].sirenaSize = Rast;
                    }
                    else
                    {
                        players[ UCID2 ].sirenaOnOff = 0;
                    }
                    players[ UCID ].sirenaKey = 1;
                    players[ UCID ].sirenaOnOff = 1;

                    if ( players[ UCID ].cop )
                        players[ UCID ].sirenaSize = 90;
                    else
                        players[ UCID ].sirenaSize = 0;
                }
                else
                {
                    players[ UCID2 ].sirenaOnOff = 0;
                    players[ UCID ].sirenaOnOff = 0;
                }


            }
        }

        if ((players[ UCID ].sirenaOnOff == 0) and (players[ UCID ].sirenaKey == 1))
        {
            players[ UCID ].sirenaKey = 0;
            send_bfn( UCID ,203);
        }

        if ( players[ UCID ].sirenaOnOff == 1)
            BtnSirena( UCID );


        if ((players[ UCID ].Pogonya == 0) and (strlen(players[ UCID ].PogonyaReason) > 1))
        {
            strcpy(players[ UCID ].PogonyaReason,"");
            send_bfn( UCID ,204);
        }

        if ( players[ UCID ].Pogonya != 0)
            BtnPogonya( UCID );

        memcpy(  &players[ UCID ].Info, &packet->Info[i], sizeof( CompCar ) );
    }
}

int RCPolice::IfCop ( byte UCID )
{
    char PlayerName[32];
    strcpy(PlayerName,players[ UCID ].PName);

    int COP = 0;

    if (
        ((strncmp("^4[^C^7���^4]",PlayerName,13)==0)
         || (strncmp("^4[^C^7��C^4]",PlayerName,13)==0)
         || (strncmp("^4[^C^7���^4]",PlayerName,13)==0)
         || (strncmp("^4[^C^7��c^4]",PlayerName,13)==0)
         || (strncmp("^4[^C^7���^4]",PlayerName,13)==0)
         || (strncmp("^4[^C^7���^4]",PlayerName,13)==0)
         || (strncmp("^4[^C^7�A�^4]",PlayerName,13)==0)
         || (strncmp("^4[^C^7�a�^4]",PlayerName,13)==0))
        && ( !players[ UCID ].cop )
    )
    {
        COP += 1;
    }

    char file[255];
    sprintf(file,"%smisc\\cops.txt",RootDir);

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        return -1;
    }
    FindClose(fff);

    ifstream readf (file,ios::in);

    while (readf.good())
    {
        char str[32];
        readf.getline(str,32);
        if( strlen( str ) > 0 and strcmp( players[ UCID ].UName , str ) == 0 )
        {
            COP += 2;
            break;
        }
    }

    readf.close();

    return COP;
}

bool RCPolice::IsCop( byte UCID )
{
    return players[ UCID ].cop;
}

void RCPolice::SetSirenLight( string sirenWord )
{
    siren = sirenWord;
}

int RCPolice::InPursuite( byte UCID )
{
    return players[ UCID ].Pogonya != 0;
}

void RCPolice::BtnSirena( byte UCID )
{
    struct IS_BTN pack;
    memset(&pack, 0, sizeof(struct IS_BTN));
    pack.Size = sizeof(struct IS_BTN);
    pack.Type = ISP_BTN;
    pack.ReqI = 1;
    pack.UCID = UCID;
    pack.Inst = 0;
    pack.TypeIn = 0;
    pack.ClickID = 203;
    pack.BStyle = 1;
    //pack.L = 50;

    pack.T = 20;
    pack.W = 124 - (players[ UCID ].sirenaSize);
    pack.L = 100 - pack.W/2;
    pack.H = pack.W/3;

    if (pack.W <= 0)
        pack.W = 1;

    if (pack.L <= 0)
        pack.L = 1;

    if (pack.H <= 0)
        pack.H = 1;

    strcpy(pack.Text,siren.c_str());
    insim->send_packet(&pack);
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


void RCPolice::CopTurnOn( byte UCID )
{
    players[ UCID ].cop = true;
}

void RCPolice::CopTurnOff( byte UCID )
{
    players[ UCID ].cop = false;
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
                send_bfn( UCID ,210);

                char Text[64];
                sprintf( Text , "/msg ^2|%s %s" , playr.PName , msg->GetMessage( UCID ,1706) );

                send_mst(Text);
                playr.Pogonya = 0;
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
            pack.ClickID = 60;
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

                    if (playr2.Pogonya == 1 )
                        sprintf(pack.Text,"%s %s %3.3f ^2(^1%02d:%02d^2)", playr2.PName, StreetInfo.Street, D, min, sec );
                    else if ( playr2.Pogonya == 2 )
                        sprintf(pack.Text,"%s %s ^1^C���������", playr2.PName,StreetInfo.Street);

                    insim->send_packet(&pack);
                    pack.T -=4;
                    pack.ClickID ++;

                }
            }
        }
    }
}
