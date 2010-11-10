#include "pizza.h"

void *pizzathread(void *arg)  // arg == classname from Pizza::init
{
    Pizza *piz = (Pizza *)arg; //struct our pizza class in thread
    if(!piz)
    {
        printf ("Can't start pizzathread");
        return 0;
    }

    return 0;

}

Pizza::Pizza()
{

}

Pizza::~Pizza()
{

}

int Pizza::init(void *classname,void *CInSim)
{
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr,PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

    if (pthread_create(&tid,&attr,pizzathread,classname) < 0)
    return -1;

    insim = (CInsim *)CInSim;
    if(!insim)
    {
        printf ("Can't struct cinsim class");
        return 0;
    }

    return 0;
}

void Pizza::pizza_ncn()
{
    int i;

    struct IS_NCN *pack_ncn = (struct IS_NCN*)insim->get_packet();

    //out << "ReqI = " << (int)pack_ncn->ReqI << endl;


    if (pack_ncn->UCID == 0)
    {
        // out << "(Host connected, not adding him to array...)" << endl << endl;
        return;
    }



    for (i=0; i<MAX_PLAYERS; i++)
        if (players[i].UCID == 0)
            break;


    if (i == MAX_PLAYERS)
    {
        //  out << "NO MORE PEOPLE ALLOWED!!!" << endl << endl;
        return;
    }

    // Copy all the player data we need into the players[] array
    strcpy(players[i].UName, pack_ncn->UName);
    strcpy(players[i].PName, pack_ncn->PName);
    players[i].UCID = pack_ncn->UCID;

}

void Pizza::pizza_npl()
{
    // out << "joining race or leaving pits" << endl;
    int i;

    struct IS_NPL *pack_npl = (struct IS_NPL*)insim->get_packet();

    // Find player using UCID and update his PLID
    for (i=0; i < MAX_PLAYERS; i++)
    {
        if (players[i].UCID == pack_npl->UCID)
        {
            players[i].PLID = pack_npl->PLID;
            strcpy(players[i].CName ,pack_npl->CName);
        }
    }
}
