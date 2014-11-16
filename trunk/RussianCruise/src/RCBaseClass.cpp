#include "RCBaseClass.h"

RCBaseClass::RCBaseClass()
{
}

RCBaseClass::~RCBaseClass()
{
}

void
RCBaseClass::next_packet()
{
    if (!insim)
        return;

    switch (insim->peek_packet())
    {
    case ISP_VER:
        InsimVER( (struct IS_VER*)insim->get_packet() );
        break;

    case ISP_TINY:
        InsimTINY( (struct IS_TINY*)insim->get_packet() );
        break;

    case ISP_SMALL:
        InsimSMALL( (struct IS_SMALL*)insim->get_packet() );
        break;

    case ISP_STA:
        InsimSTA( (struct IS_STA*)insim->get_packet() );
        break;

    case ISP_ISM:
        InsimISM( (struct IS_ISM*)insim->get_packet() );
        break;

    case ISP_MCI:
        InsimMCI( (struct IS_MCI*)insim->get_packet() );

    case ISP_MSO:
        InsimMSO( (struct IS_MSO*)insim->get_packet() );
        break;

    case ISP_III:
        InsimIII( (struct IS_III*)insim->get_packet() );
        break;

    case ISP_VTN:
        InsimVTN( (struct IS_VTN*)insim->get_packet() );
        break;

    case ISP_RST:
        InsimRST( (struct IS_RST*)insim->get_packet() );
        break;

    case ISP_NCN:
        InsimNCN(  (struct IS_NCN*)insim->get_packet()  );
        break;

    case ISP_CNL:
        InsimCNL( (struct IS_CNL*)insim->get_packet() );
        break;

    case ISP_CPR:
        InsimCPR( (struct IS_CPR*)insim->get_packet() );
        break;

    case ISP_NPL:
        InsimNPL( (struct IS_NPL*)insim->get_packet() );
        break;

    case ISP_PLP:
        InsimPLP( (struct IS_PLP*)insim->get_packet() );
        break;

    case ISP_PLL:
        InsimPLL( (struct IS_PLL*)insim->get_packet() );
        break;

    case ISP_LAP:
        InsimLAP( (struct IS_LAP*)insim->get_packet() );
        break;

    case ISP_SPX:
        InsimSPX( (struct IS_SPX*)insim->get_packet() );
        break;

    case ISP_PIT:
        InsimPIT( (struct IS_PIT*)insim->get_packet() );
        break;

    case ISP_PSF:
        InsimPSF( (struct IS_PSF*)insim->get_packet() );
        break;

    case ISP_PLA:
        InsimPLA( (struct IS_PLA*)insim->get_packet() );
        break;

    case ISP_PEN:
        InsimPEN( (struct IS_PEN*)insim->get_packet() );
        break;

    case ISP_TOC:
        InsimTOC( (struct IS_TOC*)insim->get_packet() );
        break;

    case ISP_FLG:
        InsimFLG( (struct IS_FLG*)insim->get_packet() );
        break;

    case ISP_CRS:
        InsimCRS( (struct IS_CRS*)insim->get_packet() );
        break;

    case ISP_BFN:
        InsimBFN( (struct IS_BFN*)insim->get_packet() );
        break;

    case ISP_BTC:
        InsimBTC( (struct IS_BTC*)insim->get_packet() );
        break;

    case ISP_BTT:
        InsimBTT( (struct IS_BTT*)insim->get_packet() );
        break;

    case ISP_CON:
        InsimCON( (struct IS_CON*)insim->get_packet() );
        break;

    case ISP_OBH:
        InsimOBH( (struct IS_OBH*)insim->get_packet() );
        break;

    case ISP_AXM:
        InsimAXM( (struct IS_AXM*)insim->get_packet() );
        break;

    case ISP_ACR:
        InsimACR( (struct IS_ACR*)insim->get_packet() );
        break;

    case ISP_HLV:
        InsimHLV( (struct IS_HLV*)insim->get_packet() );
        break;
    }
}

void
RCBaseClass::upd_next_packet()
{
    if (!insim)
        return;

    InsimMCI( (struct IS_MCI*)insim->udp_get_packet() );
}

bool
RCBaseClass::Check_Pos(int polySides, int polyX[], int polyY[], float x, float y)
{
    int		i, j = polySides - 1;
    bool	oddNodes = false;

    for (i=0; i<polySides; i++)
    {
        if (polyY[i]<y && polyY[j]>=y ||  polyY[j]<y && polyY[i]>=y)
        {
            if (polyX[i]+(y-polyY[i])/(polyY[j]-polyY[i])*(polyX[j]-polyX[i])<x)
            {
                oddNodes=!oddNodes;
            }
        }
        j=i;
    }

    return oddNodes;
}

int
RCBaseClass::Distance (int X, int Y, int X1, int Y1)
{
    return (int)sqrt((pow(X-X1, 2))+(pow(Y-Y1, 2)));
}

/** @brief Получение причины отключения пользователя с сервера
 *
 * @param  byte Reason - Числовой индетификатор кода
 * @return static const char* - Буквенное значение кода
 *
 */
const char*
RCBaseClass::GetReason(byte Reason)
{
    switch(Reason)
    {
    case 0:
        return "DISCONNECT";
        break;
    case 1:
        return "TIME OUT";
        break;
    case 2:
        return "LOST CONNECTION";
        break;
    case 3:
        return "KICKED";
        break;
    case 4:
        return "BANNED";
        break;
    case 5:
        return "SECURITY";
        break;
    case 6:
        return "CPW";
        break;
    case 7:
        return "OOS";
        break;
    case 8:
        return "JOOS";
        break;
    case 9:
        return "HACK";
        break;
    default:
        return "N/A";
        break;
    }
}

string
RCBaseClass::packetType(byte type)
{
    switch(type)
    {
        case ISP_VER:
            return "ISP_VER";
            break;

        case ISP_TINY:
            return "ISP_TINY";
            break;

        case ISP_SMALL:
            return "ISP_SMALL";
            break;

        case ISP_STA:
            return "ISP_STA";
            break;

        case ISP_ISM:
            return "ISP_ISM";
            break;

        case ISP_MSO:
            return "ISP_MSO";
            break;

        case ISP_III:
            return "ISP_III";
            break;

        case ISP_VTN:
            return "ISP_VTN";
            break;

        case ISP_RST:
            return "ISP_RST";
            break;

        case ISP_NCN:
            return "ISP_NCN";
            break;

        case ISP_CNL:
            return "ISP_CNL";
            break;

        case ISP_CPR:
            return "ISP_CPR";
            break;

        case ISP_NPL:
            return "ISP_NPL";
            break;

        case ISP_PLP:
            return "ISP_PLP";
            break;

        case ISP_PLL:
            return "ISP_PLL";
            break;

        case ISP_LAP:
            return "ISP_LAP";
            break;

        case ISP_SPX:
            return "ISP_SPX";
            break;

        case ISP_PIT:
            return "ISP_PIT";
            break;

        case ISP_PSF:
            return "ISP_PSF";
            break;

        case ISP_PLA:
            return "ISP_PLA";
            break;

        case ISP_PEN:
            return "ISP_PEN";
            break;

        case ISP_TOC:
            return "ISP_TOC";
            break;

        case ISP_FLG:
            return "ISP_FLG";
            break;

        case ISP_CRS:
            return "ISP_CRS";
            break;

        case ISP_BFN:
            return "ISP_BFN";
            break;

        case ISP_BTC:
            return "ISP_BTC";
            break;

        case ISP_BTT:
            return "ISP_BTT";
            break;

        case ISP_CON:
            return "ISP_CON";
            break;

        case ISP_OBH:
            return "ISP_OBH";
            break;

        case ISP_AXM:
            return "ISP_AXM";
            break;

        case ISP_ACR:
            return "ISP_ACR";
            break;

        case ISP_HLV:
            return "ISP_HLV";
            break;
    }
}

void
RCBaseClass::ButtonInfo(byte UCID, const char* Message)
{
    return insim->SendButton(1, UCID, 20, 70, 9, 60, 4, ISB_DARK, Message);
}

void
RCBaseClass::ClearButtonInfo(byte UCID)
{
    insim->SendBFN( UCID, 20);
}

string
RCBaseClass::ToString (int i)
{
	char s[24];
	sprintf(s,"%d",i);
	return string(s);
}

string
RCBaseClass::ToString (unsigned int i)
{
	char s[24];
	sprintf(s,"%d",i);
	return string(s);
}

string
RCBaseClass::ToString (byte b)
{
	char s[4];
	sprintf(s,"%d",b);
	return string(s);
}

string
RCBaseClass::ToString (bool b)
{
	if(b)
		return "true";
	else
		return "false";
}


 /** @brief Добавление объектов на карту.
 *			Перед вызовом метода, добавляемые объекты необходимо добавить в очередь addObjects,
 *			используя метод addObjects.push( ObjectInfo object )
 *
 * @return void - ничего
 *
 */
void
RCBaseClass::AddObjects()
{
	if( addObjects.size() == 0 )
		return;

	IS_AXM	*packAXM = new IS_AXM;
    memset(packAXM, 0, sizeof(struct IS_AXM));
    packAXM->Type = ISP_AXM;
    packAXM->ReqI = 1;
    packAXM->PMOAction = PMO_ADD_OBJECTS;

    int i = 0;

    while( !addObjects.empty() )
	{

		memcpy( &packAXM->Info[i], &addObjects.front(), sizeof( ObjectInfo ) );
		addObjects.pop();

		packAXM->NumO = ++i;

		if( i%30 == 0 )
			break;
	}

    packAXM->Size = 8 + packAXM->NumO * 8;
    insim->send_packet(packAXM);
    delete packAXM;
}

/** @brief Удаление объектов на карте.
 *			Перед вызовом метода, удаляемые объекты необходимо добавить в очередь delObjects,
 *			используя метод delObjects.push( ObjectInfo object )
 *
 * @return void - ничего
 *
 */
void
RCBaseClass::DelObjects()
{
	if( delObjects.size() == 0 )
		return;

	IS_AXM	*packAXM = new IS_AXM;
    memset(packAXM, 0, sizeof(struct IS_AXM));
    packAXM->Type = ISP_AXM;
    packAXM->ReqI = 1;
    packAXM->PMOAction = PMO_DEL_OBJECTS;

	int i = 0;
    while( !delObjects.empty() )
	{

		memcpy( &packAXM->Info[i], &delObjects.front(), sizeof( ObjectInfo ) );
		delObjects.pop();

		packAXM->NumO = ++i;

		if( i%30 == 0 )
			break;
	}

    packAXM->Size = 8 + packAXM->NumO * 8;
    insim->send_packet(packAXM);
    delete packAXM;
}

/** @brief Добавление объекта на карту.
 *
 * @param  ObjectInfo *object - указатель на структуру объекта
 *
 * @return void - ничего
 *
 */
void
RCBaseClass::AddObject( ObjectInfo *object )
{
	IS_AXM	*packAXM = new IS_AXM;
    memset(packAXM, 0, sizeof(struct IS_AXM));
    packAXM->Type = ISP_AXM;
    packAXM->ReqI = 1;
    packAXM->PMOAction = PMO_ADD_OBJECTS;
    packAXM->NumO = 1;
	packAXM->Size = 8 + packAXM->NumO * 8;

	memcpy( &packAXM->Info[0], object, sizeof( ObjectInfo ) );

    insim->send_packet(packAXM);
    delete packAXM;
}

/** @brief Удаление объекта с карты
 *
 * @param  ObjectInfo *object - указатель на структуру объекта
 *
 * @return void - ничего
 *
 */
void
RCBaseClass::DelObject( ObjectInfo *object )
{
	IS_AXM	*packAXM = new IS_AXM;
    memset(packAXM, 0, sizeof(struct IS_AXM));
    packAXM->Type = ISP_AXM;
    packAXM->ReqI = 1;
    packAXM->PMOAction = PMO_DEL_OBJECTS;
    packAXM->NumO = 1;
	packAXM->Size = 8 + packAXM->NumO * 8;

	memcpy( &packAXM->Info[0], object, sizeof( ObjectInfo ) );

    insim->send_packet(packAXM);
    delete packAXM;
}

void RCBaseClass::CCText(string Text, bool useTime)
{
    tm *timeStruct = tools::GetLocalTime();

    if(useTime)
    {
        string time = StringFormat("^5[%d-%02d-%02d %02d:%02d:%02d]:^8 ",
            timeStruct->tm_year,
            timeStruct->tm_mon,
            timeStruct->tm_mday,
            timeStruct->tm_hour,
            timeStruct->tm_min,
            timeStruct->tm_sec);

            Text = time + Text;
    }

	char str[256];
	sprintf(str,"f^8%s",Text.c_str());
	strtok(str, "^");

    while(true)
    {
    	char* NonFormatText = strtok (NULL, "^");
		char text[256];

        if (!NonFormatText)
            break;

        char ColorPref [2];
        strncpy(ColorPref, NonFormatText, 1);
        int COLOR = atoi(ColorPref);
        #ifdef __linux__
        switch(COLOR)
        {
            case 0: COLOR = 30; break;
        	case 1: COLOR = 31; break;
        	case 2: COLOR = 32; break;
        	case 3: COLOR = 33; break;
        	case 4: COLOR = 34;  break;
        	case 5: COLOR = 35; break;
        	case 6: COLOR = 36; break;
        	case 7: COLOR = 37; break;
        	case 8: COLOR = 0;  break;
        	case 9: COLOR = 0;  break;
        }

        strncpy(text, NonFormatText+1, strlen((NonFormatText)));

        printf("\e[1;%dm%s", COLOR, text);
        #else
        switch(COLOR)
        {
        	case 1: COLOR = 12; break;
        	case 2: COLOR = 10; break;
        	case 3: COLOR = 14; break;
        	case 4: COLOR = 9;  break;
        	case 5: COLOR = 13; break;
        	case 6: COLOR = 11; break;
        	case 7: COLOR = 15; break;
        	case 8: COLOR = 7;  break;
        	case 9: COLOR = 8;  break;
        }

        strncpy(text, NonFormatText+1, strlen((NonFormatText)));

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)(0 | COLOR));
        printf("%s", text);
        #endif
    }
    #ifdef __linux__
    printf("\e[0m\n");
    #else
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)(0 | 7));
    printf("\n");
    #endif

}


string
RCBaseClass::StringFormat(const string fmt_str, ...)
{
    int final_n, n = fmt_str.size() * 2; /* reserve 2 times as much as the length of the fmt_str */
    string str;
    unique_ptr<char[]> formatted;
    va_list ap;
    while(1) {
        formatted.reset(new char[n]); /* wrap the plain char array into the unique_ptr */
        strcpy(&formatted[0], fmt_str.c_str());
        va_start(ap, fmt_str);
        final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
        va_end(ap);
        if (final_n < 0 || final_n >= n)
            n += abs(final_n - n + 1);
        else
            break;
    }
    return string(formatted.get());
}

void
RCBaseClass::ShowPanel(byte UCID, string Caption, list<string>Text)
{
    byte Left = 100;
    byte Top = 90;								//центр поля
    byte hButton = 5;							//высота одной строки
    byte Width = 100;							//ширина поля
    byte Height = 16 + Text.size() * hButton;	//высота поля


	insim->SendButton(255, UCID, 176, Left - Width/2, Top - Height/2, Width, Height + 8, ISB_DARK, "");		//фон
    insim->SendButton(255, UCID, 177, Left - Width/2, Top - Height/2, Width, Height + 8, ISB_DARK, "");

    if(Caption.size() > 0)
    insim->SendButton(255, UCID, 179, Left - Width/2, Top - Height/2, Width, 10, ISB_LEFT, Caption); 		//заголовок

    insim->SendButton(254, UCID, 178, Left-7, Top-Height/2+Height+1, 14, 6, ISB_CLICK + ISB_LIGHT, "^2OK");	//закрывашка

    if(!Text.empty())
	{
		byte ClickId = 180;
		byte cnt = 0;
		for(auto& text: Text)
		{
			insim->SendButton(254, UCID, ClickId++, Left - Width/2 + 1, Top - Height/2 + 10 + hButton * cnt++, Width - 1, hButton, ISB_LIGHT + ISB_LEFT, text);
			if(cnt > 30)
				break;
		}
	}
}
