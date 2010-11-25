#ifndef _RC_TOOLS_H
#define _RC_TOOLS_H



struct square           // square of destination point
{
    int     X[5];       // 4 X points
    int     Y[5];       // 4 Y points
    char    Place[64];  // The name of Destination Point
};

struct place
{
    int     dealX[4];       // 4 X points of  Place
    int     dealY[4];       // 4 Y points of  Place
    int     NumPoints;      // Count of Destination points/ Need for random
    struct  square point[40]; // Destination points. See Bellow
};

bool Check_Pos(int polySides,int polyX[],int polyY[],float x,float y);


#endif
