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
    bool InsimMCI( struct IS_MCI *packet );
    bool ReadConfig(const char *Track);

private:

    map <byte, QuestPlayer> players;

    bool InsimNCN( struct IS_NCN* packet );     // ����� ����� ����� �� ������
    bool InsimNPL( struct IS_NPL* packet );     // ����� ����� �� ������
    bool InsimPLP( struct IS_PLP* packet );     // ����� ���� � �����
    bool InsimPLL( struct IS_PLL* packet );     // ����� ���� � �������
    bool InsimCNL( struct IS_CNL* packet );     // ����� ���� � �������
    bool InsimCPR( struct IS_CPR* packet );     // ����� ��������������
    bool InsimMSO( struct IS_MSO* packet );     // ����� �������� ���������
};

#endif
