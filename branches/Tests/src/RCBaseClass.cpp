#include "RCBaseClass.h"

RCBaseClass::RCBaseClass()
{
}

RCBaseClass::~RCBaseClass()
{
}

void RCBaseClass::next_packet()
{
    if (!insim)
        return;

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

void
RCBaseClass::InsimMCI( struct IS_MCI* packet )
{
    if (!insim)
        return;
}

bool RCBaseClass::Check_Pos(int polySides, int polyX[], int polyY[], float x, float y)
{
    int		i, j = polySides - 1;
    bool	oddNodes = false;
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

/** @brief Получение причины отключения пользователя с сервера
 *
 * @param  byte Reason - Числовой индетификатор кода
 * @return static const char* - Буквенное значение кода
 *
 */
const char* RCBaseClass::GetReason(byte Reason)
{
    switch(Reason)
    {
    case 0:
        return "DISCONNECT";
        break;
    case 1:
        return "TIME OUT";
        break;
    case 2:
        return "LOST CONNECTION";
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

void RCBaseClass::ButtonInfo(byte UCID, const char* Message)
{
    return insim->SendButton(1, UCID, 20, 70, 9, 60, 4, ISB_DARK, Message);
}

void RCBaseClass::ClearButtonInfo(byte UCID)
{
    insim->SendBFN( UCID, 20);
}

string RCBaseClass::ToString (int i)
{
	char s[24];
	sprintf(s,"%d",i);
	return string(s);
}

string RCBaseClass::ToString (unsigned int i)
{
	char s[24];
	sprintf(s,"%d",i);
	return string(s);
}

string RCBaseClass::ToString (byte b)
{
	char s[4];
	sprintf(s,"%d",b);
	return string(s);
}

string RCBaseClass::ToString (bool b)
{
	if(b)
		return "true";
	else
		return "false";
}

bool
RCBaseClass::dbPing( string query )
{
    return true;
    //CCText( "^3Ping From " + ClassName );

    int cnt = 0;
    while ( mysql_ping( dbconn ) != 0  && cnt++ < 10)
    {
        CCText("^3" + ClassName + ": Connection with MySQL server was lost");
        CCText( mysql_error(dbconn) );

        tools::log(query.c_str());
        /*system("pause");
        mysql_close(dbconn);

        mysqlConf conf;
        char path[MAX_PATH];
        memset(&path,0,MAX_PATH);
        sprintf(path, "%smisc/mysql.cfg", RootDir);
        CCText( "^3" + ClassName + string( path ) );

        while (!tools::read_mysql(path, &conf) || !mysql_real_connect( dbconn , conf.host , conf.user , conf.password , conf.database , conf.port , NULL, 0) )
        {
            CCText("^3" + ClassName + ": ^1Can't connect to MySQL server");
            CCText( mysql_error(dbconn) );

            sleep(1);

            memset(&path,0,MAX_PATH);
            sprintf(path, "%smisc/mysql.cfg", RootDir);
        }*/

        //CCText("^3" + ClassName + "::\t\t^2Connected to MySQL server");
    }

    if(cnt >= 10)
        return false;

    return true;
}

/** @brief Выборка данных из базы данных
 *
 * @param string query - стандартная SQL команда
 * @return DB_ROWS - массив элементов DB_ROW
 *
 */
DB_ROWS
RCBaseClass::dbSelect( string query )
{
	dbPing(query);

	DB_ROWS out;
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
		   CCText( ClassName + "::Select - Can't find 'FROM' in query");
		   return out;
		}

		pos += 1 + strlen("from"); // shift the whitespace (продвигаемся дальше пробела)
		size_t afterTable = query.find(" ", pos );

		tableName = query.substr( pos, afterTable - pos );

		char colQuery[MAX_PATH];
		sprintf(colQuery, "SELECT COLUMN_NAME FROM information_schema.COLUMNS WHERE TABLE_NAME = '%s'", tableName.c_str());

		if( mysql_query(dbconn, colQuery) != 0 )
		{
			CCText("^1DB SELECT ERROR: " + (string)mysql_error(dbconn));
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
			CCText("^1DB SELECT ERROR: " + (string)mysql_error(dbconn));
			return out;
		}

		dbres = mysql_store_result(dbconn);

		// если все гуд делаем массив 0 -> Name string colums[ mysql_num_rows() ]
		if( dbres == NULL)
		{
			CCText("^1DB SELECT ERROR: Can't store result");
			return out;
		}

		while( (dbrow = mysql_fetch_row(dbres)) != NULL )
		{
			DB_ROW row;
			for( unsigned int i = 0; i < mysql_num_fields(dbres); i++ )
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
		   CCText( ClassName + "::Select - Can't find 'FROM' in query");
		   return out;
		}

    	xString fields = query.substr( strlen("select ") , pos  - strlen("select ") );

    	vector<string> arFields = fields.split(',', 1);

		list<string> columns;
    	for( auto& f : arFields)
		{
			columns.push_back( Trim( f ) );
		}

		if( mysql_query(dbconn, query.c_str() ) != 0 )
		{
			CCText("^1DB SELECT ERROR: " + (string)mysql_error(dbconn));
			return out;
		}

		dbres = mysql_store_result(dbconn);

		// если все гуд делаем массив 0 -> Name string colums[ mysql_num_rows() ]
		if( dbres == NULL)
		{
			CCText("^1DB SELECT ERROR: Can't store result");
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
		mysql_free_result(dbres);
    }

    return out;
}

/** @brief Выполнение SQL запроса, который не возвращает данные (обновление строк, служебные запросы)
 *
 * @param string query - стандартная SQL команда
 * @return bool - результат функции mysql_query()
 *
 */
bool
RCBaseClass::dbExec( string query )
{
	return dbExec( query.c_str() );
}

bool
RCBaseClass::dbExec( const char *query )
{

    dbPing(query);

	if( mysql_query(dbconn, query ) != 0 )
	{
	    CCText("^1DB EXEC ERROR: " + (string)mysql_error(dbconn));
		return false;
	}

	return true;
}

/** @brief Обновления данных в базе
 *
 * @param string table - обновляемая таблица
 * @param DB_ROW fields - ассоциативный массив с обновляемыми данными
 * @param pair<string, string> where - пара {"ключ","значение"} фильтр по которому будет происходить обновление
 * @return bool - результат работы mysql_exec
 *
 */
bool
RCBaseClass::dbUpdate( string table, DB_ROW fields, pair<string, string> where )
{
	string query = "UPDATE " + table + " SET ";

    bool first = true;
    for( auto row: fields )
    {
        if( !first )
            query += ", ";

        first = false;

        query += row.first + "='" + row.second + "'";
    }

    query += " WHERE " +where.first +"='"+ where.second+"'";

    return dbExec( query );
}

inline string
RCBaseClass::Trim(const string &s)
{
   auto wsfront=find_if_not(s.begin(),s.end(),[](int c){return isspace(c);});
   auto wsback=find_if_not(s.rbegin(),s.rend(),[](int c){return isspace(c);}).base();
   return (wsback<=wsfront ? string() : string(wsfront,wsback));
}

 /** @brief Добавление объектов на карту.
 *			Перед вызовом метода, добавляемые объекты необходимо добавить в очередь addObjects,
 *			используя метод addObjects.push( ObjectInfo object )
 *
 * @return void - ничего
 *
 */
void
RCBaseClass::AddObjects()
{
	if( addObjects.size() == 0 )
		return;

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

    packAXM->Size = 8 + packAXM->NumO * 8;
    insim->send_packet(packAXM);
    delete packAXM;
}

/** @brief Удаление объектов на карте.
 *			Перед вызовом метода, удаляемые объекты необходимо добавить в очередь delObjects,
 *			используя метод delObjects.push( ObjectInfo object )
 *
 * @return void - ничего
 *
 */
void
RCBaseClass::DelObjects()
{
	if( delObjects.size() == 0 )
		return;

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

    packAXM->Size = 8 + packAXM->NumO * 8;
    insim->send_packet(packAXM);
    delete packAXM;
}

/** @brief Добавление объекта на карту.
 *
 * @param  ObjectInfo *object - указатель на структуру объекта
 *
 * @return void - ничего
 *
 */
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

/** @brief Удаление объекта с карты
 *
 * @param  ObjectInfo *object - указатель на структуру объекта
 *
 * @return void - ничего
 *
 */
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

void RCBaseClass::CCText(string Text)
{
    // cout << Text << endl;
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
            case 0: COLOR = 30; break;
        	case 1: COLOR = 31; break;
        	case 2: COLOR = 32; break;
        	case 3: COLOR = 33; break;
        	case 4: COLOR = 34;  break;
        	case 5: COLOR = 35; break;
        	case 6: COLOR = 36; break;
        	case 7: COLOR = 37; break;
        	case 8: COLOR = 0;  break;
        	case 9: COLOR = 0;  break;
        }

        strncpy(text, NonFormatText+1, strlen((NonFormatText)));

        printf("\033[1;%dm%s", COLOR, text);
    }
    printf("\033[0m\n");
}


string
RCBaseClass::StringFormat(const string fmt_str, ...)
{
    int final_n, n = fmt_str.size() * 2; /* reserve 2 times as much as the length of the fmt_str */
    string str;
    unique_ptr<char[]> formatted;
    va_list ap;
    while(1) {
        formatted.reset(new char[n]); /* wrap the plain char array into the unique_ptr */
        strcpy(&formatted[0], fmt_str.c_str());
        va_start(ap, fmt_str);
        final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
        va_end(ap);
        if (final_n < 0 || final_n >= n)
            n += abs(final_n - n + 1);
        else
            break;
    }
    return string(formatted.get());
}

void
RCBaseClass::ShowPanel(byte UCID, string Caption, list<string>Text)
{
    byte Left = 100;
    byte Top = 90;								//центр поля
    byte hButton = 5;							//высота одной строки
    byte Width = 100;							//ширина поля
    byte Height = 16 + Text.size() * hButton;	//высота поля


	insim->SendButton(255, UCID, 176, Left - Width/2, Top - Height/2, Width, Height + 8, ISB_DARK, "");		//фон
    insim->SendButton(255, UCID, 177, Left - Width/2, Top - Height/2, Width, Height + 8, ISB_DARK, "");

    if(Caption.size() > 0)
    insim->SendButton(255, UCID, 179, Left - Width/2, Top - Height/2, Width, 10, ISB_LEFT, Caption); 		//заголовок

    insim->SendButton(254, UCID, 178, Left-7, Top-Height/2+Height+1, 14, 6, ISB_CLICK + ISB_LIGHT, "^2OK");	//закрывашка

    if(!Text.empty())
	{
		byte ClickId = 180;
		byte cnt = 0;
		for(auto& text: Text)
		{
			insim->SendButton(254, UCID, ClickId++, Left - Width/2 + 1, Top - Height/2 + 10 + hButton * cnt++, Width - 1, hButton, ISB_LIGHT + ISB_LEFT, text);
			if(cnt > 30)
				break;
		}
	}
}

// split: receives a char delimiter; returns a vector of strings
// By default ignores repeated delimiters, unless argument rep == 1.
vector<string>&
xString::split( const char delim, int rep)
{
    if (!flds.empty()) flds.clear();  // empty vector if necessary
    string work = data();
    string buf = "";
    unsigned int i = 0;
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
