#include "SQLite.h"

int callback(void *classname, int columns, char **Value, char **ColName){

    SQLite3 *sql = (SQLite3 *)classname;
    if (!sql)
    {
        return -1;
    }

    return 1;
}

SQLite3::SQLite3()
{

}

SQLite3::~SQLite3()
{

}

int SQLite3::init(void *clname)
{
    classname = clname;
    return 0;
}

int SQLite3::open(char *dbname)
{
    rc = sqlite3_open(dbname, &db);
    if( rc )
    {
        printf("Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    return 0;

}

int SQLite3::close()
{
    sqlite3_close(db);
    return 0;
}

int SQLite3::exec (char *query)
{
    rc=sqlite3_exec(db, query, NULL, NULL, &zErrMsg);
    if(rc!=SQLITE_OK)
    {
        printf( "%s\n", zErrMsg);
    }
    return 0;
}

int SQLite3::query (char *query)
{
    rc=sqlite3_exec(db,query, callback, &classname, &zErrMsg);
    if(rc!=SQLITE_OK)
    {
        printf( "%s\n", zErrMsg);
    }
    return 0;
}


