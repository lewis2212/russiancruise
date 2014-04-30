

#include "tools.h"
#include <stdio.h>
#include <string.h>
using namespace std;
// с какой стороны обрезать символы - слева, справа или с обеих сторон
enum trim_type {LEFT=1, RIGHT, LEFT_AND_RIGHT};
// первый параметр - строка
// второй параметр - символ, который нужно обрезать с края строки
// третий параметр - с какой стороны обрезать символ
void trim(char *str, char c=' ', trim_type t=LEFT_AND_RIGHT)
{
    int len = strlen(str);
    int beg=0;
    if(t & RIGHT)
        while(str[len-1]==c) str[--len]=0;
    if(t & LEFT)
        while(str[beg]==c) str[beg++]=0;
    if(beg) memmove(str, str+beg, len-beg+1);
}
// первый параметр - строка
// второй параметр - строка, содержащая символы, которые нужно обрезать
// третий параметр - с какой стороны обрезать символы
void trim(char *str, char *s, trim_type t=LEFT_AND_RIGHT)
{
    int len = strlen(str), slen = strlen(s);
    int beg=0;

    if(t & RIGHT)
    {
        bool b=true;
        while(b)
        {
            b = false;
            for(int i=0; i<slen; i++)
                if(str[len-1]==s[i])
                {
                    str[--len]=0;
                    b = true;
                    break;
                }
        }
    }
    if(t & LEFT)
    {
        bool b=true;
        while(b)
        {
            b = false;
            for(int i=0; i<slen; i++)
                if(str[beg]==s[i])
                {
                    str[beg++]=0;
                    b = true;
                    break;
                }
        }
    }
    if(beg) memmove(str, str+beg, len-beg+1);
}
// вместо написания отдельных функций - пара макросов
#define ltrim(str, c) trim(str, c, LEFT)
#define rtrim(str, c) trim(str, c, RIGHT)
// =================================================

namespace tools
{
void log(const char *text)
{

    FILE *file;

    file = fopen("C:\\cruise.log","a");
    fputs(text, file);
    fputs("\n", file);
    fclose(file);
}

void read_mysql(const char *path ,mysqlConf *conf)
{
    HANDLE fff;
    WIN32_FIND_DATA fd;
    fff = FindFirstFile(path,&fd);
    if (fff == INVALID_HANDLE_VALUE)
    {
        FindClose(fff);
        return;
    }
    FindClose(fff);

    memset(conf, 0, sizeof( mysqlConf ) );

    ifstream readf;
    readf.open(path,ios::in);

    char	str[128];
    while( readf.good() )
    {
        readf.getline(str,128);

        if( strlen(str) == 0 )
            continue;

        if( strncmp(str, "Host=", 5 ) == 0 )
            strcpy( conf->host , str+5 );

        if( strncmp(str, "Port=", 5 ) == 0 )
            conf->port = atoi( str+5 );

        if( strncmp(str, "User=", 5 ) == 0 )
            strcpy( conf->user , str+5 );

        if( strncmp(str, "Pass=", 5 ) == 0 )
            strcpy( conf->password , str+5 );

        if( strncmp(str, "DBName=", 7 ) == 0 )
            strcpy( conf->database , str+7 );
    }
    readf.close();
}
}
