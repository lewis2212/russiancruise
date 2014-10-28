#ifndef _RC_TOOLS_H
#define _RC_TOOLS_H

#include <math.h>
#include <windows.h>
#include <iostream>
#include <fstream>

typedef unsigned char byte;

struct square           // square of destination point
{
    int     X;       // 4 X points
    int     Y;       // 4 Y points
    char    Place[64];  // The name of Destination Point
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
void read_mysql(const char *path ,mysqlConf *conf);
}

#endif