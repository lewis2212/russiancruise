#ifndef _RC_TOOLS_H
#define _RC_TOOLS_H

using namespace std;

#include <math.h>
#include <ctime>
//#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef __linux__
#include <iconv.h>
#endif // __linux__
#include <stdexcept>

typedef unsigned char byte;

struct square           // square of destination point
{
    int     X;       // 4 X points
    int     Y;       // 4 Y points
    char    PlaceName[64];  // The name of Destination Point
};

struct place
{
    int     dealX[10];       // 4 X points of  Place
    int     dealY[10];       // 4 Y points of  Place
    int     NumPoints;      // Count of Destination points/ Need for random
    struct  square point[40]; // Destination points.
};

struct mysqlConf
{
    char 	host[64];
    int		port;
    char 	user[64];
    char 	password[64];
    char 	database[64];
};

namespace tools
{
    void log(const char *text);
    bool read_mysql(const char *path ,mysqlConf& conf);
    string convert_encoding(const string& data, const string& from, const string& to);
    struct tm * GetLocalTime();
    bool isAdmin(const char *path, string username);
};

#endif
