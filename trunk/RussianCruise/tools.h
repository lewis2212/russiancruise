#ifndef _RC_TOOLS_H
#define _RC_TOOLS_H

#include <math.h>

typedef unsigned char byte;

struct square           // square of destination point
{
    int     X;       // 4 X points
    int     Y;       // 4 Y points
    char    Place[64];  // The name of Destination Point
};

struct place
{
    int     dealX[4];       // 4 X points of  Place
    int     dealY[4];       // 4 Y points of  Place
    int     NumPoints;      // Count of Destination points/ Need for random
    struct  square point[40]; // Destination points. See Bellow
};

namespace tools {

    bool Check_Pos(int polySides,int polyX[],int polyY[],float x,float y);
    float Distance (int X, int Y, int X1, int Y1);

}
#endif
