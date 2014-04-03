#include "tools.h"

namespace tools
{
    void log(const char *text)
    {
        FILE *file;

        file = fopen("cruise.log", "a");
        fputs(text, file);
        fputs("\n", file);
        fclose(file);
    }

    bool read_mysql(const char *path , mysqlConf *conf)
    {
        memset(conf, 0, sizeof( mysqlConf ) );

        ifstream readf;
        readf.open(path, ios::in);

        if(readf.is_open() == false)
        {
            cout << "Can't open " << path << endl;
        }

        char	str[128];
        while ( readf.good() )
        {
            readf.getline(str, 128);

            if ( strlen(str) == 0 )
                continue;

            if ( strncmp(str, "Host=", 5 ) == 0 )
                strcpy( conf->host , str+5 );

            if ( strncmp(str, "Port=", 5 ) == 0 )
                conf->port = atoi( str+5 );

            if ( strncmp(str, "User=", 5 ) == 0 )
                strcpy( conf->user , str+5 );

            if ( strncmp(str, "Pass=", 5 ) == 0 )
                strcpy( conf->password , str+5 );

            if ( strncmp(str, "DBName=", 7 ) == 0 )
                strcpy( conf->database , str+7 );
        }
        readf.close();

        if( strlen(conf->host) == 0 ||
           conf->port == 0 ||
           strlen(conf->user) == 0 ||
           strlen(conf->password) == 0 ||
           strlen(conf->database) == 0)
        {
            return false;
        }

        return true;
    }
}
