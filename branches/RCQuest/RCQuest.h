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

			void    insim_ncn( struct IS_NCN* packet );		// ����� ����� ����� �� ������
			void    insim_npl( struct IS_NPL* packet );		// ����� ����� �� ������
			void    insim_plp( struct IS_PLP* packet );		// ����� ���� � �����
			void    insim_pll( struct IS_PLL* packet );		// ����� ���� � �������
			void    insim_cnl( struct IS_CNL* packet );		// ����� ���� � �������
			void    insim_cpr( struct IS_CPR* packet );		// ����� ��������������
			void    insim_mso( struct IS_MSO* packet );		// ����� �������� ���������
};

#endif
