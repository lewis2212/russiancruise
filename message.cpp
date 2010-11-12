using namespace std;

#include "message.h"

RCMessage::RCMessage()
{

}

RCMessage::~RCMessage()
{

}

int RCMessage::init()
{
    char file[255];
    //strcpy(file,RootDir);
    //strcat(file,"misc\\mysql.cfg");
    strcpy(file,"lang\\rus.txt");

    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        cout << "Can't find " << file << endl;
        return -1;
    }
    FindClose(fff);

    ifstream readf (file,ios::in);
    while (readf.good())
    {
        char str[128];
        readf.getline(str,128);
        if (strlen(str) > 6)
        {
            char * id;
            char * mesage;

            id = strtok (str,"\"");
            mesage = strtok (NULL,"\"");

            //cout << "id = " << id << "message = " << mesage << endl;

            //memset(&message[atoi(id)],0,sizeof(message));
            strcpy(message[0][atoi(id)], mesage);

        }
    }
    readf.close();

    //strcpy(file,RootDir);
    //strcat(file,"misc\\mysql.cfg");
    strcpy(file,"lang\\eng.txt");

//    HANDLE fff;
//    WIN32_FIND_DATA fd;
    fff = FindFirstFile(file,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        cout << "Can't find " << file << endl;
        return -1;
    }
    FindClose(fff);


    ifstream readf2 (file,ios::in);
    while (readf2.good())
    {
        char str[128];
        readf2.getline(str,128);
        if (strlen(str) > 6)
        {
            char * id;
            char * mesage;

            id = strtok (str,"\"");
            mesage = strtok (NULL,"\"");

            //cout << "id = " << id << "message = " << mesage << endl;

            //memset(&message[atoi(id)],0,sizeof(message));
            strcpy(message[1][atoi(id)], mesage);

        }
    }
    readf2.close();
    return 0;
}
