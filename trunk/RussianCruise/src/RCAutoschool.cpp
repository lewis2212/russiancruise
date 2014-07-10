#include "RCAutoschool.h"

RCAutoschool::RCAutoschool(const char *dir)
{
    ClassName = "RCAutoschool";
	strcpy(RootDir, dir);
	players.clear();
}

RCAutoschool::~RCAutoschool()
{
	players.clear();
}

void
RCAutoschool::init(MYSQL *rcMaindbConn, CInsim *insim, RCMessage *msg)
{
	dbconn = rcMaindbConn;

	this->insim = insim;
	this->msg = msg;

    CCText("^3R"+ClassName+":\t^2inited");
}

void
RCAutoschool::ReadConfig(const char* Track)
{
	char confFile[MAX_PATH];
	sprintf(confFile, "%s/data/RCAutoschool/%s/config.json", RootDir, Track);



	ifstream file;

	file.open(confFile, ios::binary);

	if( !file.is_open() )
	{
		cout  << "Failed to open configuration file\n";
		return;
	}

	bool readed = configReader.parse( file, config, false );

	if ( !readed )
	{
		file.close();
		// report to the user the failure and their locations in the document.
		cout  << "Failed to parse configuration\n"
				   << configReader.getFormattedErrorMessages();
		return;
	}
	file.close();

	//cout << config["encoding"] << endl;

	CCText("  ^7RCSchool\t^2OK");
}

void
RCAutoschool::InsimNCN( struct IS_NCN* packet )
{
	 if (packet->UCID == 0)
    {
        return;
    }

    players[packet->UCID].UName = packet->UName;
    players[packet->UCID].PName = packet->PName;
}

void
RCAutoschool::InsimNPL( struct IS_NPL* packet )
{
	PLIDtoUCID[ packet->PLID ] = packet->UCID;
    players[packet->UCID].CName = packet->CName;
}

void
RCAutoschool::InsimPLP( struct IS_PLP* packet )
{
}

void
RCAutoschool::InsimPLL( struct IS_PLL* packet )
{
	PLIDtoUCID.erase( packet->PLID );
}

void
RCAutoschool::InsimCNL( struct IS_CNL* packet )
{
	players.erase( packet->UCID );
}

void
RCAutoschool::InsimCPR( struct IS_CPR* packet )
{
	players[packet->UCID].PName = packet->PName;
}

void
RCAutoschool::InsimMSO( struct IS_MSO* packet )
{
	if (packet->UCID == 0)
    {
        return;
    }

    //byte UCID = packet->UCID;

    string msg = packet->Msg + packet->TextStart;

    if( msg == "!saveConf" )
	{
		char confFile[MAX_PATH];
		sprintf(confFile, "%s/data/RCAutoschool/SO4X/config.temp.json", RootDir);

		ofstream f;
		f.open(confFile, ios::out);
		f << configWriter.write( config );
		f.close();
	}

/*	int X = players[ packet->UCID ].Info.X / 65536;
	int Y = players[ packet->UCID ].Info.Y / 65536;
	int H = players[ packet->UCID ].Info.Heading / 182;

	if(msg == "!start" && lesson.length() != 0 )
	{
		config["lesson"][lesson]["start"]["X"] = X;
		config["lesson"][lesson]["start"]["Y"] = Y;
		config["lesson"][lesson]["start"]["Heading"] = H;

		SendMTC(255, "Set start position for " + lesson );

	}

	if(msg == "!finish" && lesson.length() != 0 )
	{
		config["lesson"][lesson]["finish"]["X"] = X;
		config["lesson"][lesson]["finish"]["Y"] = Y;
		config["lesson"][lesson]["finish"]["Heading"] = H;

		SendMTC(255, "Set finish position for " + lesson );
	}

	if(msg == "!check" && lesson.length() != 0 )
	{
		Json::Value point;

		point["X"] = X;
		point["Y"] = Y;
		point["Heading"] = H;

		config["lesson"][lesson]["checkpoints"].append( point );

		SendMTC(255, "Add checkpoint for " + lesson );
	}

	if( msg.find("!lesson") == 0 )
	{
		SplitString p = msg;
		vector<string> params = p.split(' ', 1);

		if( params.size() > 1 )
		{
			for( auto l: config["lessons"] )
			{
				if( l.asString() == params[1] )
				{
					lesson = params[1];
					SendMTC(255, l.asString() );
					return;
				}
			}
			for( auto l: config["lessons"] )
			{
				SendMTC(255, l.asString() );
			}
		}

		else{
			SendMTC(255, "Lesson not selected");

			for( auto l: config["lessons"] )
			{
				SendMTC(255, l.asString() );
			}
		}
	}
	*/
}

void
RCAutoschool::InsimCON( struct IS_CON* packet )
{
	//byte UCIDA = PLIDtoUCID[ packet->A.PLID ];
    //byte UCIDB = PLIDtoUCID[ packet->B.PLID ];
}

void
RCAutoschool::InsimOBH( struct IS_OBH* packet )
{
	 //byte UCID = PLIDtoUCID[ packet->PLID ];
}

void
RCAutoschool::InsimHLV( struct IS_HLV* packet )
{
	 //byte UCID = PLIDtoUCID[ packet->PLID ];

    /** столкновение со стеной **/
    if (packet->HLVC==1)
    {

    }
}

void
RCAutoschool::InsimAXM( struct IS_AXM* packet )
{
	if( packet->UCID == 0 )
		return;

	if( packet->PMOAction != PMO_ADD_OBJECTS )
		return

	CCText("^3RCAutoschool^1::^2InsimAXM");

}

void
RCAutoschool::InsimMCI ( struct IS_MCI* packet )
{
    for (int i = 0; i < packet->NumC; i++) // прогон по всему массиву packet->Info[i]
    {
        byte UCID = PLIDtoUCID[ packet->Info[i].PLID ];

        int X = packet->Info[i].X/65536;
        int Y = packet->Info[i].Y/65536;
        int Speed = ((int)packet->Info[i].Speed*360)/(32768);

        Json::Value lesson = config["lesson"][ players[UCID].Lesson ];

        int dStartHeading, dFinishHeading;

        dStartHeading = ( lesson["start"]["Heading"].asInt() - (int)(players[UCID].Info.Heading/182) );
		dStartHeading = ( dStartHeading < 0 ) ? dStartHeading + 360 : dStartHeading;
		dStartHeading = ( dStartHeading > 180 ) ? 360 - dStartHeading : dStartHeading;

		dFinishHeading = ( lesson["finish"]["Heading"].asInt() - (int)(players[UCID].Info.Heading/182) );
		dFinishHeading = ( dFinishHeading < 0 ) ? dFinishHeading + 360 : dFinishHeading;
		dFinishHeading = ( dFinishHeading > 180 ) ? 360 - dFinishHeading : dFinishHeading;

        int arX[ lesson["position"].size() ];
        int arY[ lesson["position"].size() ];

        for( unsigned int i = 0; i < lesson["position"].size(); ++i )
		{
			arX[i] = lesson["position"][i]["X"].asInt();
			arY[i] = lesson["position"][i]["Y"].asInt();
		}

		int StartDist = Distance(X, Y, lesson["start"]["X"].asInt(), lesson["start"]["Y"].asInt() ),
		PointDist = 0,
		FinishDist = Distance(X, Y, lesson["finish"]["X"].asInt(), lesson["finish"]["Y"].asInt() );

		if( Check_Pos(lesson["position"].size(),arX,arY,X,Y) )
		{
			if( !players[UCID].OnPlace )
				insim->SendMTC(UCID, msg->_(UCID,"Please go to start and stop!") );

			players[UCID].OnPlace = true;

			if( StartDist < 15 && dStartHeading < 15  && Speed == 0 && players[UCID].CheckPoint == 0)
			{
				if( !players[UCID].Started )
					insim->SendMTC(UCID, msg->_(UCID,"Let's do an exercise!") );

				players[UCID].Started = true;
			}


			if( players[UCID].Started && players[UCID].CheckPoint < lesson["checkpoints"].size() )
			{
				Json::Value point = lesson["checkpoints"][ players[UCID].CheckPoint ];

				PointDist = Distance(X, Y, point["X"].asInt(), point["Y"].asInt() );

				if( PointDist < 2 )
					players[UCID].CheckPoint++;

			}

			if( players[UCID].CheckPoint == lesson["checkpoints"].size() )
			{
				if( FinishDist < 15 && dFinishHeading < 15  && Speed == 0)
				{
					if( !players[UCID].Finished )
						insim->SendMTC(UCID, msg->_(UCID,"Exellent!") );

					players[UCID].Finished = true;
				}
			}
		}
		else
		{
			players[UCID].OnPlace = false;
			players[UCID].Finished = false;
			players[UCID].Started = false;
			players[UCID].CheckPoint = 0;
		}

	byte ClickID = CLICK_ID_130;

	byte Left = 100;
	byte Top = 10;
	byte Width = 50;
	byte Height = 4;


	insim->SendButton(255,UCID,ClickID++,Left,Top += 5,Width,Height,ISB_DARK,"^7" + players[UCID].Lesson );
	insim->SendButton(255,UCID,ClickID++,Left,Top += 5,Width,Height,ISB_DARK,"^7OnPlace: " + ToString( players[UCID].OnPlace ) );
	insim->SendButton(255,UCID,ClickID++,Left,Top += 5,Width,Height,ISB_DARK,"^7OnStart: " + ToString( players[UCID].Started ) );
	insim->SendButton(255,UCID,ClickID++,Left,Top += 5,Width,Height,ISB_DARK,"^7OnFinish: " + ToString( players[UCID].Finished ) );
	insim->SendButton(255,UCID,ClickID++,Left,Top += 5,Width,Height,ISB_DARK,"^7CheckPoint: " + ToString( players[UCID].CheckPoint ) );
	insim->SendButton(255,UCID,ClickID++,Left,Top += 5,Width,Height,ISB_DARK,"^2Point Dist: " + ToString( PointDist ) );
	insim->SendButton(255,UCID,ClickID++,Left,Top += 5,Width,Height,ISB_DARK,"^3Finish Dist: " + ToString( FinishDist ) + " ^1(X: " + ToString( lesson["finish"]["X"].asInt() ) + " Y: " + ToString( lesson["finish"]["Y"].asInt() ) +")" );
	insim->SendButton(255,UCID,ClickID++,Left,Top += 5,Width,Height,ISB_DARK,"^3Finish Head: " + ToString( dFinishHeading ) );

        ShowPanel(UCID);
        players[UCID].Info = packet->Info[i];
    }
}

void
RCAutoschool::ShowPanel(byte UCID)
{

}
