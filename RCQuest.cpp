// тут уже пишется вся логика и работа методов
#include "RCQuest.h"
#include "RCBaseClass.h"
#include <iostream>
RCQuest::RCQuest() {}
RCQuest::~RCQuest() {}
void RCQuest::init(const char* RootDir, void *insim)
{
    strcpy(RCQuest::RootDir, RootDir);

    RCQuest::insim = (CInsim *)insim;

    if(!RCQuest::insim)
    {
        printf("RCQuest Error: Can't struct CInsim class\n");
        return;
    }

}// привет


void RCQuest::send_mst (const char* Text)
{
    struct IS_MST pack_mst;
    memset(&pack_mst, 0, sizeof(struct IS_MST));
    pack_mst.Size = sizeof(struct IS_MST);
    pack_mst.Type = ISP_MST;
    sprintf( pack_mst.Msg, "%.63s\0", Text );
    insim->send_packet(&pack_mst);
};

// обработчик события когда на сервер заходит новый пользователь
void RCQuest::insim_ncn( struct IS_NCN *packet )
{
    if( packet->UCID == 0 )
        return;
    strcpy(players[ packet->UCID ].UName, packet->UName);
    strcpy(players[ packet->UCID ].PName, packet->PName);
}

void RCQuest::insim_npl( struct IS_NPL *packet )
{
    PtoU[packet->PLID] = packet->UCID;
}

void RCQuest::insim_plp( struct IS_PLP *packet )
{
   PtoU.erase( packet->PLID );
}

void RCQuest::insim_pll( struct IS_PLL *packet )
{
   PtoU.erase( packet->PLID );
}

void RCQuest::insim_cnl( struct IS_CNL *packet )
{
   players.erase(packet->UCID);
}

void RCQuest::insim_cpr( struct IS_CPR *packet )
{
         strcpy( players[ packet->UCID ].PName, packet->PName );
}

 void RCQuest::insim_mso( struct IS_MSO *packet )     // TyT ya ne ponyal chto tu delal , po etomy prosto ctrl+c -> ctrl+v
{
     byte UCID = packet->UCID;

    if ( UCID == 0 )
        return;

    if ( packet->UserType != MSO_PREFIX )
        return;

    char Msg[128];
    strcpy( Msg, packet->Msg + ((unsigned char)packet->TextStart));
}


void RCQuest::insim_mci(struct IS_MCI *packet)
{

}

 void RCQuest::insim_obh( struct IS_OBH* packet )
 {

   std::cout << players[PtoU[packet->PLID]].UName;
   char str[24];
   strcpy(str , players[PtoU[packet->PLID]].PName);
   send_mst(str);
 }



void RCQuest::insim_hlv( struct IS_HLV* packet )
{

	std::cout << (int)(packet->HLVC) << std::endl;
}

