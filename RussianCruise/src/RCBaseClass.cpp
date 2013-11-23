#include "RCBaseClass.h"

RCBaseClass::RCBaseClass()
{
    //ctor
}

RCBaseClass::~RCBaseClass()
{
    //dtor
}

void RCBaseClass::next_packet()
{
    if (!insim)
        return;     //dont work if insim is NULL

    switch (insim->peek_packet())
    {
    case ISP_VER:
        InsimVER( (struct IS_VER*)insim->get_packet() );
        break;

    case ISP_TINY:
        InsimTINY( (struct IS_TINY*)insim->get_packet() );
        break;

    case ISP_SMALL:
        InsimSMALL( (struct IS_SMALL*)insim->get_packet() );
        break;

    case ISP_STA:
        InsimSTA( (struct IS_STA*)insim->get_packet() );
        break;

    case ISP_ISM:
        InsimISM( (struct IS_ISM*)insim->get_packet() );
        break;

    case ISP_MSO:
        InsimMSO( (struct IS_MSO*)insim->get_packet() );
        break;

    case ISP_III:
        InsimIII( (struct IS_III*)insim->get_packet() );
        break;

    case ISP_VTN:
        InsimVTN( (struct IS_VTN*)insim->get_packet() );
        break;

    case ISP_RST:
        InsimRST( (struct IS_RST*)insim->get_packet() );
        break;

    case ISP_NCN:
        InsimNCN(  (struct IS_NCN*)insim->get_packet()  );
        break;

    case ISP_CNL:
        InsimCNL( (struct IS_CNL*)insim->get_packet() );
        break;

    case ISP_CPR:
        InsimCPR( (struct IS_CPR*)insim->get_packet() );
        break;

    case ISP_NPL:
        InsimNPL( (struct IS_NPL*)insim->get_packet() );
        break;

    case ISP_PLP:
        InsimPLP( (struct IS_PLP*)insim->get_packet() );
        break;

    case ISP_PLL:
        InsimPLL( (struct IS_PLL*)insim->get_packet() );
        break;

    case ISP_LAP:
        InsimLAP( (struct IS_LAP*)insim->get_packet() );
        break;

    case ISP_SPX:
        InsimSPX( (struct IS_SPX*)insim->get_packet() );
        break;

    case ISP_PIT:
        InsimPIT( (struct IS_PIT*)insim->get_packet() );
        break;

    case ISP_PSF:
        InsimPSF( (struct IS_PSF*)insim->get_packet() );
        break;

    case ISP_PLA:
        InsimPLA( (struct IS_PLA*)insim->get_packet() );
        break;

    case ISP_PEN:
        InsimPEN( (struct IS_PEN*)insim->get_packet() );
        break;

    case ISP_TOC:
        InsimTOC( (struct IS_TOC*)insim->get_packet() );
        break;

    case ISP_FLG:
        InsimFLG( (struct IS_FLG*)insim->get_packet() );
        break;

    case ISP_CRS:
        InsimCRS( (struct IS_CRS*)insim->get_packet() );
        break;

    case ISP_BFN:
        InsimBFN( (struct IS_BFN*)insim->get_packet() );
        break;

    case ISP_BTC:
        InsimBTC( (struct IS_BTC*)insim->get_packet() );
        break;

    case ISP_BTT:
        InsimBTT( (struct IS_BTT*)insim->get_packet() );
        break;

    case ISP_CON:
        InsimCON( (struct IS_CON*)insim->get_packet() );
        break;

    case ISP_OBH:
        InsimOBH( (struct IS_OBH*)insim->get_packet() );
        break;

    case ISP_AXM:
        InsimAXM( (struct IS_AXM*)insim->get_packet() );
        break;

    case ISP_ACR:
        InsimACR( (struct IS_ACR*)insim->get_packet() );
        break;

    case ISP_HLV:
        InsimHLV( (struct IS_HLV*)insim->get_packet() );
        break;
    }
}

void    RCBaseClass::InsimMCI( struct IS_MCI* packet )
{
    if (!insim)
    {
        return;/**dont work if insim is NULL**/
    }
}

bool RCBaseClass::Check_Pos(int polySides, int polyX[], int polyY[], float x, float y)
{
    int		i, j=polySides-1 ;
    bool	oddNodes=false;
    try
    {
        for (i=0; i<polySides; i++)
        {
            if (polyY[i]<y && polyY[j]>=y ||  polyY[j]<y && polyY[i]>=y)
            {
                if (polyX[i]+(y-polyY[i])/(polyY[j]-polyY[i])*(polyX[j]-polyX[i])<x)
                {
                    oddNodes=!oddNodes;
                }
            }
            j=i;
        }
    }
    catch(...)
    {
        //tools::log("Check_Pos Exeption\n");
    }
    return oddNodes;
}

int RCBaseClass::Distance (int X, int Y, int X1, int Y1)
{
    return (int)sqrt((pow(X-X1, 2))+(pow(Y-Y1, 2)));
}

void RCBaseClass::SendMTC (byte UCID, string Msg)
{
    IS_MTC *pack = new IS_MTC;
    memset( pack, 0, sizeof( IS_MTC ) );
    pack->Size = sizeof( IS_MTC );
    pack->Type = ISP_MTC;
    pack->UCID = UCID;
    strcpy( pack->Text, Msg.c_str());
    insim->send_packet( pack );
    delete pack;
}

void RCBaseClass::SendMST (const char* Text)
{
    IS_MST *pack = new IS_MST;
    memset( pack, 0, sizeof( IS_MST));
    pack->Size = sizeof( IS_MST);
    pack->Type = ISP_MST;
    sprintf( pack->Msg, "%.63s\0", Text );
    insim->send_packet( pack );
    delete pack;
}

void RCBaseClass::SendBFN (byte UCID, byte ClickID)
{
    IS_BFN *pack = new IS_BFN;
    memset( pack, 0, sizeof( IS_BFN ) );
    pack->Size = sizeof( IS_BFN );
    pack->Type = ISP_BFN;
    pack->UCID = UCID;
    pack->ClickID = ClickID;
    insim->send_packet( pack );
    delete pack;
}

const char* RCBaseClass::GetReason(byte Reason)
{
    switch(Reason)
    {
    case 0:
        return "DISCONNECT";
        break;
    case 1:
        return "TIME_OUT";
        break;
    case 2:
        return "LOST_CONNECTION";
        break;
    case 3:
        return "KICKED";
        break;
    case 4:
        return "BANNED";
        break;
    case 5:
        return "SECURITY";
        break;
    case 6:
        return "CPW";
        break;
    case 7:
        return "OOS";
        break;
    case 8:
        return "JOOS";
        break;
    case 9:
        return "HACK";
        break;
    default:
        return "N/A";
        break;
    }
}

void RCBaseClass::SendBFNAll ( byte UCID )
{
    IS_BFN *pack = new IS_BFN;
    memset( pack, 0, sizeof( IS_BFN ) );
    pack->Size = sizeof( IS_BFN );
    pack->Type = ISP_BFN;
    pack->UCID = UCID;
    pack->SubT = BFN_CLEAR;
    insim->send_packet( pack );
    delete pack;
}

void RCBaseClass::ButtonInfo(byte UCID, const char* Message)
{
    IS_BTN *pack = new IS_BTN;
    memset( pack, 0, sizeof( IS_BTN ) );
    pack->Size = sizeof( IS_BTN );
    pack->Type = ISP_BTN;
    pack->ReqI = 1;
    pack->UCID = UCID;
    pack->ClickID = 211;
    pack->BStyle = 16;
    pack->L = 70;
    pack->T = 9;
    pack->W = 60;
    pack->H = 4;
    strcpy( pack->Text, Message );
    insim->send_packet( pack );
    delete pack;
}

void RCBaseClass::ClearButtonInfo(byte UCID)
{
    SendBFN( UCID, 211);
}

string RCBaseClass::NumToString (int n)
{
	char s[24];
	sprintf(s,"%d",n);
	return (string)s;
}

string RCBaseClass::NumToString (byte n)
{
	char s[24];
	sprintf(s,"%d",n);
	return (string)s;
}

void RCBaseClass::SendStringButton (byte ReqI, byte UCID, byte ClickID, byte L, byte T, byte W, byte H, byte BStyle, string Text, byte TypeIn )
{
    IS_BTN *pack = new IS_BTN;
    memset( pack, 0, sizeof( IS_BTN ) );
    pack->Size = sizeof( IS_BTN );
    pack->Type = ISP_BTN;
    pack->ReqI = ReqI;
    pack->UCID = UCID;
    pack->Inst = 0;
    pack->BStyle = BStyle;
    pack->TypeIn = TypeIn;
    pack->ClickID = ClickID;
    pack->L = L;
    pack->T = T;
    pack->W = W;
    pack->H = H;
    strcpy(pack->Text, Text.c_str());
    insim->send_packet(pack);
    delete pack;
}

void RCBaseClass::SendButton(byte ReqI, byte UCID, byte ClickID, byte L, byte T, byte W, byte H, byte BStyle, const char * Text, byte TypeIn)
{
    IS_BTN *pack = new IS_BTN;
    memset( pack, 0, sizeof( IS_BTN ) );
    pack->Size = sizeof( IS_BTN );
    pack->Type = ISP_BTN;
    pack->ReqI = ReqI;
    pack->UCID = UCID;
    pack->Inst = 0;
    pack->BStyle = BStyle;
    pack->TypeIn = TypeIn;
    pack->ClickID = ClickID;
    pack->L = L;
    pack->T = T;
    pack->W = W;
    pack->H = H;
    sprintf(pack->Text, Text);
    insim->send_packet( pack );
    delete pack;
}

list<DB_ROW>
RCBaseClass::dbSelect( string query )
{
	list<DB_ROW> out;
    out.clear();

    size_t pos = string::npos;
    string tableName;

	pos = query.find("*");

	if( pos != string::npos )
	{

		pos = query.find("FROM");

		if(pos == string::npos)
			pos = query.find("from");

		if(pos == string::npos)
		{
		   printf( "RCBaseclass::Slect - Can't find 'FROM' in query");
		   return out;
		}

		pos += 1 + strlen("from"); // shift the whitespace (продвигаемся дальше пробела)
		size_t afterTable = query.find(" ", pos );

		tableName = query.substr( pos, afterTable - pos );

		char colQuery[MAX_PATH];
		sprintf(colQuery, "SELECT COLUMN_NAME FROM information_schema.COLUMNS WHERE TABLE_NAME = '%s'", tableName.c_str());

		if( mysql_query(dbconn, colQuery) != 0 )
		{
			printf("DB ERROR: %s\n", mysql_error(dbconn));
			return out;
		}

		dbres = mysql_store_result(dbconn);

		// если все гуд делаем массив 0 -> Name string colums[ mysql_num_rows() ]
		if( dbres == NULL  || mysql_num_rows(dbres) == 0)
		{
			printf("TABLE %s NOT FOUND\n", tableName.c_str());
			return out;
		}

		string *columns = new string[ mysql_num_rows(dbres) ];

		int colNum = 0;
		while( (dbrow = mysql_fetch_row(dbres)) != NULL )
		{
			columns[ colNum ] = string( dbrow[0] );
			colNum++;
		}

		mysql_free_result(dbres);

		if( mysql_query(dbconn, query.c_str() ) != 0 )
		{
			printf("DB ERROR: %s\n", mysql_error(dbconn));
			return out;
		}

		dbres = mysql_store_result(dbconn);

		// если все гуд делаем массив 0 -> Name string colums[ mysql_num_rows() ]
		if( dbres == NULL)
		{
			printf("DB ERROR: Can't store result\n", tableName.c_str());
			return out;
		}

		while( (dbrow = mysql_fetch_row(dbres)) != NULL )
		{
			DB_ROW row;
			for( int i = 0; i < mysql_num_fields(dbres); i++ )
			{
				row[ columns[ i ] ] = string( dbrow[ i ] );
			}
			out.push_back( row );
		}

		delete [] columns;

    }
    else
    {
    	pos = query.find("FROM");

		if(pos == string::npos)
			pos = query.find("from");

		if(pos == string::npos)
		{
		   printf( "RCBaseclass::Slect - Can't find 'FROM' in query");
		   return out;
		}

    	SplitString fields = query.substr( strlen("select ") , pos  - strlen("select ") );

    	vector<string> arFields = fields.split(',', 1);

		list<string> columns;
    	for( auto& f : arFields)
		{
			columns.push_back( trim( f ) );
		}

		if( mysql_query(dbconn, query.c_str() ) != 0 )
		{
			printf("DB ERROR: %s\n", mysql_error(dbconn));
			return out;
		}

		dbres = mysql_store_result(dbconn);

		// если все гуд делаем массив 0 -> Name string colums[ mysql_num_rows() ]
		if( dbres == NULL)
		{
			printf("DB ERROR: Can't store result\n", tableName.c_str());
			return out;
		}

		while( (dbrow = mysql_fetch_row(dbres)) != NULL )
		{
			DB_ROW row;
			int i = 0;
			for( auto& col: columns)
			{
				row[ col ] = dbrow[ i ];
				i++;
			}
			out.push_back( row );
		}
    }

    return out;
}

bool
RCBaseClass::dbExec( string query )
{
    if (mysql_ping(dbconn) != 0)
    {
        printf("DB ERROR: connection with MySQL server was lost\n");

        return false;
    }

	if( mysql_query(dbconn, query.c_str() ) != 0 )
	{
		printf("DB ERROR: %s\n", mysql_error(dbconn));
		return false;
	}
	return true;
}

bool
RCBaseClass::dbExec( const char *query )
{
    if (mysql_ping(dbconn) != 0)
    {
        printf("DB ERROR: connection with MySQL server was lost\n");

        return false;
    }

	if( mysql_query(dbconn, query ) != 0 )
	{
		printf("DB ERROR: %s\n", mysql_error(dbconn));
		return false;
	}
	return true;
}

inline string
RCBaseClass::trim(const std::string &s)
{
   auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
   auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
   return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));
}

void
RCBaseClass::AddObjects()
{
	if( addObjects.size() == 0 )
		return;

	//char debug[MAX_PATH];
	//sprintf(debug,"^6DEBUG^7: ^2addObjects queue = %d", addObjects.size());
	//CCText(debug);

	IS_AXM	*packAXM = new IS_AXM;
    memset(packAXM, 0, sizeof(struct IS_AXM));
    packAXM->Type = ISP_AXM;
    packAXM->ReqI = 1;
    packAXM->PMOAction = PMO_ADD_OBJECTS;

    int i = 0;

    while( !addObjects.empty() )
	{

		memcpy( &packAXM->Info[i], &addObjects.front(), sizeof( ObjectInfo ) );
		addObjects.pop();

		packAXM->NumO = ++i;

		if( i%30 == 0 )
			break;
	}

	//sprintf(debug,"^6DEBUG^7: ^2Add %d objects",packAXM->NumO );
	//CCText(debug);

    packAXM->Size = 8 + packAXM->NumO * 8;
    insim->send_packet(packAXM);
    delete packAXM;
}

void
RCBaseClass::DelObjects()
{
	if( delObjects.size() == 0 )
		return;

	//char debug[MAX_PATH];
	//sprintf(debug,"^6DEBUG^7: ^1delObjects queue = %d", delObjects.size());
	//CCText(debug);

	IS_AXM	*packAXM = new IS_AXM;
    memset(packAXM, 0, sizeof(struct IS_AXM));
    packAXM->Type = ISP_AXM;
    packAXM->ReqI = 1;
    packAXM->PMOAction = PMO_DEL_OBJECTS;

	int i = 0;
    while( !delObjects.empty() )
	{

		memcpy( &packAXM->Info[i], &delObjects.front(), sizeof( ObjectInfo ) );
		delObjects.pop();

		packAXM->NumO = ++i;

		if( i%30 == 0 )
			break;
	}

	//sprintf(debug,"^6DEBUG^7: ^1Del %d objects",packAXM->NumO );
	//CCText(debug);

    packAXM->Size = 8 + packAXM->NumO * 8;
    insim->send_packet(packAXM);
    delete packAXM;
}

void
RCBaseClass::AddObject( ObjectInfo *object )
{
	IS_AXM	*packAXM = new IS_AXM;
    memset(packAXM, 0, sizeof(struct IS_AXM));
    packAXM->Type = ISP_AXM;
    packAXM->ReqI = 1;
    packAXM->PMOAction = PMO_ADD_OBJECTS;
    packAXM->NumO = 1;
	packAXM->Size = 8 + packAXM->NumO * 8;

	memcpy( &packAXM->Info[0], object, sizeof( ObjectInfo ) );

    insim->send_packet(packAXM);
    delete packAXM;
}

void
RCBaseClass::DelObject( ObjectInfo *object )
{
	IS_AXM	*packAXM = new IS_AXM;
    memset(packAXM, 0, sizeof(struct IS_AXM));
    packAXM->Type = ISP_AXM;
    packAXM->ReqI = 1;
    packAXM->PMOAction = PMO_DEL_OBJECTS;
    packAXM->NumO = 1;
	packAXM->Size = 8 + packAXM->NumO * 8;

	memcpy( &packAXM->Info[0], object, sizeof( ObjectInfo ) );

    insim->send_packet(packAXM);
    delete packAXM;
}

// split: receives a char delimiter; returns a vector of strings
// By default ignores repeated delimiters, unless argument rep == 1.
vector<string>& SplitString::split( const char delim, int rep)
{
    if (!flds.empty()) flds.clear();  // empty vector if necessary
    string work = data();
    string buf = "";
    int i = 0;
    while (i < work.length())
    {
        if (work[i] != delim)
            buf += work[i];
        else if (rep == 1)
        {
            flds.push_back(buf);
            buf = "";
        }
        else if (buf.length() > 0)
        {
            flds.push_back(buf);
            buf = "";
        }
        i++;
    }
    if (!buf.empty())
        flds.push_back(buf);
    return flds;
}

void RCBaseClass::CCText(string Text)
{
	char str[256];
	sprintf(str,"f^8%s",Text.c_str());
	strtok(str, "^");

    while(true)
    {
    	char* NonFormatText = strtok (NULL, "^");
		char text[256];

        if (!NonFormatText)
            break;

        char ColorPref [2];
        strncpy(ColorPref, NonFormatText, 1);
        int COLOR = atoi(ColorPref);

        switch(COLOR)
        {
        	case 1: COLOR = 12; break;
        	case 2: COLOR = 10; break;
        	case 3: COLOR = 14; break;
        	case 4: COLOR = 9;  break;
        	case 5: COLOR = 13; break;
        	case 6: COLOR = 11; break;
        	case 7: COLOR = 15; break;
        	case 8: COLOR = 7;  break;
        	case 9: COLOR = 8;  break;
        }

        strncpy(text, NonFormatText+1, strlen((NonFormatText)));

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)(0 | COLOR));
        printf("%s", text);
    }
    printf("\n");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)(0 | 7));
}
