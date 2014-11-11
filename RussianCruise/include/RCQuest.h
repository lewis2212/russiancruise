#ifndef _RC_QUEST_H
#define _RC_QUEST_H

#include "RCBaseClass.h"

struct QuestPlayer: GlobalPlayer
{
    Json::Value quest;
};


// ��� �������� ���� ������� � ������� ������
class RCQuest:public RCBaseClass
{
public:
    RCQuest(const char* Dir);
    ~RCQuest();
    void init(DBMySQL *db, CInsim *InSim);

private:
    map <byte, QuestPlayer> players;

    void    ReadConfig(const char *Track);

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
