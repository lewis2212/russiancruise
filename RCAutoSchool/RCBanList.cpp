#include "RCBanList.h"

RCBanList::RCBanList()
{
    //ctor
}

RCBanList::~RCBanList()
{

}

int RCBanList::init( const char* dir, void *CInSim )
{
    strcpy( RootDir, dir );

    insim = (CInsim *)CInSim; // Присваиваем указателю область памяти
    if (!insim) // Проверяем на существование
    {
        printf ("Can't struct CInsim class");
        return -1;
    }
}

void RCBanList::InsimNCN( struct IS_NCN* packet )
{

    char file[MAX_PATH];
    sprintf(file, "%smisc\\banlist.txt", RootDir);
    ifstream bans( file , ios::in );

    char line[48];
    while ( bans.good() )
    {
        memset(&line, 0, 48);
        bans.getline(line, 48);

        if ( strstr( line, packet->UName )!= NULL )
        {

            char* user;
            char * timme;
            user = strtok(line, ";");
            timme = strtok(NULL, ";");
            if ( !user || !timme )
                return;

            char buffer[80];
            time_t seconds = atoi( timme );
            time_t currenttime = time(NULL);
            if ( seconds > currenttime )
            {

                tm* timeinfo = localtime(&seconds);
                const char* format = "%d.%m.%Y %H:%M:%S";
                strftime(buffer, 80, format, timeinfo);

                char command[64];
                sprintf( command, "^1BAN BAN BAN -> %s", buffer );
                SendMTC(packet->UCID, command );
                SendMTC(packet->UCID, command );
                SendMTC(packet->UCID, command );
                SendMTC(packet->UCID, command );
                SendMTC(packet->UCID, command );
                SendMTC(packet->UCID, command );
                SendMTC(packet->UCID, command );
                SendMTC(packet->UCID, command );
                SendMTC(packet->UCID, command );
                SendMTC(packet->UCID, command );
                sprintf( command, "/kick %s", packet->UName );
                SendMST( command );
            }
            else
            {

                removeUser( packet->UName );

            }
            break;
        }
    }
    bans.close();

}

void RCBanList::removeUser( const char* username )
{
    if (strlen(username)>0)
    {
        char file[MAX_PATH];
        sprintf(file, "%smisc\\banlist.txt", RootDir);
        char line[48];
        //cout << file << endl;
        char Bans[200][32];
        memset(&Bans, 0, 200*32);

        int j = 0;

        ifstream rBans( file , ios::in );

        while ( rBans.good() )
        {
            memset(&line, 0, 32);
            rBans.getline(line, 32);

            if (strlen(line) >0)
                strncpy(Bans[j++], line, 32);
        }
        rBans.close();

        ofstream wBans( file , ios::out );

        for (j=0; j<40; j++)
        {
            if (strncmp(Bans[j], username, strlen(username)) != 0)
            {
                wBans << Bans[j] << endl;
            }
        }
        wBans.close();
    }
}

void RCBanList::addUser( const char* username, time_t timestamp )
{

    if (strlen(username)>0)
    {
        char file[MAX_PATH];
        sprintf(file, "%smisc\\banlist.txt", RootDir);
        char line[48];
        char buffer[80];
        //cout << file << endl;
        if (strlen(username)>0)
        {
            ofstream wBans( file , ios::app );
            wBans << username << ";" << timestamp << endl;
            wBans.close();
        }

        tm* timeinfo = localtime(&timestamp);
        const char* format = "%d.%m.%Y %H:%M:%S";
        strftime(buffer, 80, format, timeinfo);

        char command[64];
        sprintf( command, "^1BAN ^0%s ^7-> %s", username, buffer );
        SendMST( command );
        sprintf( command, "/kick %s", username );
        SendMST( command );

    }
}
