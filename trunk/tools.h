#ifndef _RC_TOOLS_H
#define _RC_TOOLS_H

typedef unsigned char byte;

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

struct PTH
{
    char    LFSPTH[6];              // do not read file if no match
    byte    version;             // 0 - do not read file if > 0
    byte    revision;            // 0 - do not read file if > 0
    int     num_nodes;           // number
    int     finish_line;         // number
};

struct PTH_NODES
{
    int     centre_X;            // fp
    int     centre_Y;            // fp
    int     centre_Z;            // fp
    float   dir_X;               // float
    float   dir_Y;               // float
    float   dir_Z;               // float
    float   limit_left;          // outer limit
    float   limit_right;         // outer limit
    float   drive_left;          // road limit
    float   drive_right;         // road limit
};
#endif
