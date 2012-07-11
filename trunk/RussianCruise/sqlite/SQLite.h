#ifndef _RC_SQLITE3_H_
#define _RC_SQLITE3_H_

#include <iostream>
#include "sqlite3.h"

int callback(void *classname, int columns, char **Value, char **ColName);

class SQLite3
{
    private:
    sqlite3 *db;
    char *zErrMsg;
    int rc;
    void *classname;

    public:
    int columns;
    char **Value;
    char **ColName;

    SQLite3::SQLite3();
    SQLite3::~SQLite3();
    int init(void *clname); //
    int open(char *dbname);
    int close();
    int exec(char *query);
    int query(char *query);
    int num_rows();
    char *fetch_row();
    void free();


};

#endif
