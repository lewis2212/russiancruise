#include "tools.h"



bool Check_Pos(int polySides,int polyX[],int polyY[],float x,float y)
{

    int      i, j=polySides-1 ;
    bool  oddNodes=false     ;

    for (i=0; i<polySides; i++)
    {
        if (polyY[i]<y && polyY[j]>=y ||  polyY[j]<y && polyY[i]>=y)
        {
            if (polyX[i]+(y-polyY[i])/(polyY[j]-polyY[i])*(polyX[j]-polyX[i])<x)
            {
                oddNodes=!oddNodes;
            }
        }
        j=i;
    }
    return oddNodes;
}

int Distance (int X, int Y, int X1, int Y1)
{
	return (int)sqrt((pow(X-X1,2))+(pow(Y-Y1,2)));
}
