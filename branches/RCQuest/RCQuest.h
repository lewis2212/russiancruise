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
        void    insim_ncn();    // Новый игрок зашел на сервер
        void    insim_npl();    // Игрок вышел из боксов
        void    insim_plp();    // Игрок ушел в боксы
        void    insim_pll();    // Игрок ушел в зрители
        void    insim_cnl();    // Игрок ушел с сервера
        void    insim_crp();    // Игрок переименовался
        void    insim_mso();    // Игрок отправил сообщение
};

//};




