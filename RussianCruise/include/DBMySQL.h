#ifndef __DBMYSQL__
#define __DBMYSQL__

using namespace std;

#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <vector>
#include <cstring>
#include <string>
#include <algorithm>

#include <mysql/mysql.h>

#include "xString.h"

/**< переменная - асоциативный массив вида "название поля" -> "значение поля" */
typedef map< string, string > DB_ROW;

/**< переменная - массив состоящий из элементов DB_ROW */
typedef vector<DB_ROW> DB_ROWS;

class DBMySQL
{
	//MYSQL
    MYSQL       dbconn;
    MYSQL_RES   *dbres;

public:
	DBMySQL();
	~DBMySQL();

	bool connect(const char *host, int port, const char *username, const char *password, const char *database);

	string getError(){return mysql_error(&dbconn);}

    bool ping();
	DB_ROWS select( string query );
	bool exec( string query );
	bool exec( const char *query );

	/** @brief Добавление данных в базу
	 *
	 * @param string query - стандартная SQL команда
	 * @return unsigned int - ID последней добавленой строки, если есть PRIMARY KEY (результат запроса SELECT LAST_INSERT_ID();)
	 *
	 */
	unsigned int insert( string query );
	unsigned int insert( const char *query );
	bool update( string table, DB_ROW fields, pair<string, string> where );

	inline string Trim(const string &s);
	
};

#endif