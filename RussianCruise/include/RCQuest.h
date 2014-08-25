#ifndef _RC_QUEST_H
#define _RC_QUEST_H

#include "RCBaseClass.h"

// ������ ��������� ������

struct Quests
{
    bool current;
    byte step;
    bool complete;
};

struct QuestPlayer
{
    //struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    byte    UCID;                  // Connection ID
    byte    PLID;                  // PLayer ID
    char    CName[4];              // Car Name
    map<string, Quests> quest;
};


// ��� �������� ���� ������� � ������� ������
class RCQuest:public RCBaseClass
{
public:
    RCQuest(const char* Dir);
    ~RCQuest();
    void init(MYSQL *conn, CInsim *InSim);
    void insim_mci();
    void ReadConfig(const char *Track);

private:

    map <byte, QuestPlayer> players;

    void    InsimCNL( struct IS_CNL* packet );      // ����� ���� � �������
    void    InsimCPR( struct IS_CPR* packet );      // ����� ��������������
    void    InsimMCI( struct IS_MCI *packet );
    void    InsimMSO( struct IS_MSO* packet );      // ����� �������� ���������
    void    InsimNCN( struct IS_NCN* packet );      // ����� ����� ����� �� ������
    void    InsimNPL( struct IS_NPL* packet );      // ����� ����� �� ������
    void    InsimPLL( struct IS_PLL* packet );      // ����� ���� � �������
    void    InsimPLP( struct IS_PLP* packet );      // ����� ���� � �����
};

#endif
