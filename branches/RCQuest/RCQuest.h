//#ifndef _RCQUEST_H
#define _RCQUEST_H

#include "RCBaseClass.h"


class RCQuest:public RCBaseClass
{
    public:
        RCQuest();
        ~RCQuest();
        void init(const char* RootDir);
        void insim_mci();

    private:
        void    insim_ncn();    // ����� ����� ����� �� ������
        void    insim_npl();    // ����� ����� �� ������
        void    insim_plp();    // ����� ���� � �����
        void    insim_pll();    // ����� ���� � �������
        void    insim_cnl();    // ����� ���� � �������
        void    insim_crp();    // ����� ��������������
        void    insim_mso();    // ����� �������� ���������
};

//};




