#ifndef _RC_POLICE_H
#define _RC_POLICE_H


struct fine
{
    int     id;
    char    name[64];
    int     cash;
};

struct user_fine
{
    int     fine_id;
    int     fine_date;
};

// struct  fine fines[100];                 // Array of fines (for cops)

struct player
{
    struct  CompCar2 Info;

    struct user_fine fines[MAX_FINES];
};

#endif
