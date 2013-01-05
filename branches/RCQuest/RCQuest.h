#ifndef _RC_QUEST_H
#define _RC_QUEST_H

#include "RCBaseClass.h"

// ������ ��������� ������



typedef struct QuestPlayer
{
    //struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    byte    UCID;                  // Connection ID
    byte    PLID;                  // PLayer ID
    char    CName[4];              // Car Name
} _QuestPlayer;


// ��� �������� ���� ������� � ������� ������
class RCQuest:public RCBaseClass
{
    public:
        RCQuest();
        ~RCQuest();
        void init(const char* RootDir, void *insim);
        void insim_mci();

    private:

		QuestPlayer players[MAX_PLAYERS];

        void    insim_ncn();    // ����� ����� ����� �� ������
        void    insim_npl();    // ����� ����� �� ������
        void    insim_plp();    // ����� ���� � �����
        void    insim_pll();    // ����� ���� � �������
        void    insim_cnl();    // ����� ���� � �������
        void    insim_crp();    // ����� ��������������
        void    insim_mso();    // ����� �������� ���������
};

#endif
