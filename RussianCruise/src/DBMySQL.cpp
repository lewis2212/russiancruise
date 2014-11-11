#include "DBMySQL.h"

DBMySQL::DBMySQL(){}
DBMySQL::~DBMySQL(){}

bool 
DBMySQL::connect(const char *host, int port, const char *username, const char *password, const char *database)
{
 	if (!mysql_init(&dbconn))
    {
        printf("Can't create MySQL-descriptor\n");
        return false;
    }

    my_bool reconnect = 1;
    mysql_options( &dbconn , MYSQL_OPT_RECONNECT, &reconnect ); // разрешаем переподключение

	return mysql_real_connect( &dbconn , host, username, password, database, port, NULL, 0);
}

bool
DBMySQL::ping()
{
    return mysql_ping( &dbconn );    
}

/** @brief Выборка данных из базы данных
 *
 * @param string query - стандартная SQL команда
 * @return DB_ROWS - массив элементов DB_ROW
 *
 */
DB_ROWS
DBMySQL::select( string query )
{
	MYSQL_ROW   dbrow;
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
		   cout<< "DB::Select - Can't find 'FROM' in query" << endl;
		   return out;
		}

		pos += 1 + strlen("from"); // shift the whitespace (продвигаемся дальше пробела)
		size_t afterTable = query.find(" ", pos );

		tableName = query.substr( pos, afterTable - pos );

		string colQuery = "SELECT COLUMN_NAME FROM information_schema.COLUMNS WHERE TABLE_NAME = '"+tableName+"'";

		if( mysql_query(&dbconn, colQuery.c_str()) != 0 )
		{
			cout<< "DB SELECT ERROR: " + (string)mysql_error(&dbconn) << endl;
			return out;
		}

		dbres = mysql_store_result(&dbconn);

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

		if( mysql_query(&dbconn, query.c_str() ) != 0 )
		{
			cout<< "DB SELECT ERROR: " + (string)mysql_error(&dbconn) << endl;
			return out;
		}

		dbres = mysql_store_result(&dbconn);

		// если все гуд делаем массив 0 -> Name string colums[ mysql_num_rows() ]
		if( dbres == NULL)
		{
			cout << "^1DB SELECT ERROR: Can't store result" << endl;
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
		   cout << "DB::Select - Can't find 'FROM' in query" << endl;
		   return out;
		}

    	xString fields = query.substr( strlen("select ") , pos  - strlen("select ") );

    	vector<string> arFields = fields.split(',', 1);

		list<string> columns;
    	for( auto f : arFields)
		{
			columns.push_back( Trim( f ) );
		}

		if( mysql_query(&dbconn, query.c_str() ) != 0 )
		{
			cout << "^1DB SELECT ERROR: Can't store result" << endl;
			return out;
		}

		dbres = mysql_store_result(&dbconn);

		// если все гуд делаем массив 0 -> Name string colums[ mysql_num_rows() ]
		if( dbres == NULL)
		{
			cout << "^1DB SELECT ERROR: Can't store result" << endl;
			return out;
		}

		while( (dbrow = mysql_fetch_row(dbres)) != NULL )
		{
			DB_ROW row;
			int i = 0;
			for( auto col: columns)
			{
				row[ col ] = dbrow[ i++ ];
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
DBMySQL::exec( string query )
{
	return exec( query.c_str() );
}

bool
DBMySQL::exec( const char *query )
{
	if( mysql_query(&dbconn, query ) != 0 )
	{
	    cout<< "DB EXEC ERROR: " + (string)mysql_error(&dbconn) << endl;
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
DBMySQL::update( string table, DB_ROW fields, pair<string, string> where )
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

    return exec( query );
}

inline string 
DBMySQL::Trim(const string &s)
{
   auto wsfront=find_if_not(s.begin(),s.end(),[](int c){return isspace(c);});
   auto wsback=find_if_not(s.rbegin(),s.rend(),[](int c){return isspace(c);}).base();
   return (wsback<=wsfront ? string() : string(wsfront,wsback));
}