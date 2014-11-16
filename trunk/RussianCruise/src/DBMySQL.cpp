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

	bool ret = mysql_real_connect( &dbconn , host, username, password, database, port, NULL, 0);

	if(ret)
    {
        columns = collectTables(database);
    }

	return ret;
}

map<string,vector<string>>
DBMySQL::collectTables(string dbName)
{
    map<string,vector<string>> out;
    out.clear();

    MYSQL_RES *result = mysql_list_tables(&dbconn, nullptr);

    if(!result)
    {
        cout<< "DB collectTables ERROR: " + (string)mysql_error(&dbconn) << endl;
        return out;
    }

    MYSQL_ROW row;
    while(row = mysql_fetch_row(result))
    {
        out[ row[0] ] = collectColumns( row[0] );
    }

    mysql_free_result(result);

    return out;
}

vector<string>
DBMySQL::collectColumns(string tableName)
{
    vector<string> out;
    out.clear();

    string query = "SHOW COLUMNS FROM " + tableName;

    if(mysql_query(&dbconn,query.c_str()) != 0)
    {
        cout<< "DB collectColumns ERROR: " + (string)mysql_error(&dbconn) << endl;
        return out;
    }

    MYSQL_RES *result = mysql_store_result(&dbconn);

    if(!result)
    {
        cout<< "DB collectColumns ERROR: " + (string)mysql_error(&dbconn) << endl;
        return out;
    }

    MYSQL_ROW row;
    while(row = mysql_fetch_row(result))
    {
        out.push_back( row[0] );
    }

    mysql_free_result(result);

    return out;
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
DBMySQL::select(vector<string> fields, string table, DB_ROW where)
{
    DB_ROWS out;
    out.clear();

    if(fields.size() == 0)
    {
        if(columns.find(table) != columns.end())
        {
            cout << "1" << endl;
            fields = columns.at(table);
        }
        else
        {
            cout << "2" << endl;
            fields = collectColumns(table);
        }
    }

    string query = "SELECT " + xString::join(fields,",") + " FROM " + table;

    if(where.size() != 0)
    {
        bool first = true;
        for (DB_ROW::iterator it=where.begin(); it!=where.end(); ++it)
        {
            if(first)
                query += " WHERE " + it->first + "='" + it->second + "'";
            else
                query += " AND " + it->first + "=" + it->second + "'";

            first = false;
        }

    }
cout << query << endl;
    if( mysql_query(&dbconn, query.c_str() ) != 0 )
		{
			cout<< "DB SELECT ERROR: " + (string)mysql_error(&dbconn) << endl;
			return out;
		}

		MYSQL_RES *dbres = mysql_store_result(&dbconn);

		// если все гуд делаем массив 0 -> Name string colums[ mysql_num_rows() ]
		if( dbres == NULL)
		{
			cout << "^1DB SELECT ERROR: Can't store result" << endl;
			return out;
		}

        MYSQL_ROW dbrow;
		while( (dbrow = mysql_fetch_row(dbres)) != NULL )
		{
			DB_ROW row;
			for( unsigned int i = 0; i < mysql_num_fields(dbres); i++ )
			{
				row[ fields[ i ] ] = string( dbrow[ i ] );
			}
			out.push_back( row );
		}

		mysql_free_result(dbres);

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
DBMySQL::update( string table, DB_ROW fields, DB_ROW where )
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

    if(where.size() != 0)
    {
        bool first = true;
        for (DB_ROW::iterator it=where.begin(); it!=where.end(); ++it)
        {
            if(first)
                query += " WHERE " + it->first + "=" + it->second;
            else
                query += " AND " + it->first + "=" + it->second;

            first = false;
        }
    }

    return exec( query );
}

inline string
DBMySQL::Trim(const string &s)
{
   auto wsfront=find_if_not(s.begin(),s.end(),[](int c){return isspace(c);});
   auto wsback=find_if_not(s.rbegin(),s.rend(),[](int c){return isspace(c);}).base();
   return (wsback<=wsfront ? string() : string(wsfront,wsback));
}
