#ifndef _RC_LIGHT_H // ��������, ����� ���� ����������� ���� ���
#define _RC_LIGHT_H

#include <stdio.h>      //
#include <iostream>     //
#include <time.h>       //
#include <windows.h>    //

#include "CInsim.h"     // Insim
#include "RCMessage.h"  // Messages

#include "tools.h"      // Check_Pos  etc.

// ������ ��������� ������

struct LghPlayer
{
    struct  CompCar Info;
    char    UName[24];             // Username
    char    PName[24];             // Player name
    byte    UCID;                  // Connection ID
    byte    PLID;                  // PLayer ID
    byte    Light;
};

struct Lights
{
    byte    ID;
    int     Heading;
    int     PointCount;
    int     *X;
    int     *Y;
};

// �������� ������ �����
class RCLight
{
private:
    // ���������� � �������, ��������� ������ ������ ������

    char RootDir[MAX_PATH]; // ������ ���� �� ����� � ����������

    CInsim      *insim; // ����������-��������� �� ����� CInsim
    RCMessage   *msg;   // ����������-��������� �� ����� RCMessage

    struct  LghPlayer players[32];     // ��������� �������
    int     LightsCount;
    struct  Lights Light[30];              // Array of streets
    // struct  streets2 Street2[30];              // Array of streets

    // �������-����������� �������� ������� ����
    void ncn();   // ����� ����� ����� �� ������
    void npl();   // ����� ����� �� ������
    void plp();   // ����� ���� � �����
    void pll();   // ����� ���� � �������
    void cnl();   // ����� ���� � �������
    void crp();   // ����� ��������������

    void mso();   // ����� �������� ���������

    void send_bfn(byte UCID, byte ClickID);
    void send_mst (char* Text);
    void send_mtc (byte UCID,char* Msg);
    void btn_svetofor1 (struct LghPlayer *splayer);
    void btn_svetofor2 (struct LghPlayer *splayer);
    void btn_svetofor3 (struct LghPlayer *splayer);


public:
    // ���������� � �������, ��������� ��� ����

    RCLight::RCLight();   // ����������� ������ (�����������)
    RCLight::~RCLight();  // ���������� ������ (�����������)


    bool IfInited;


    // �������� ������� ������
    int init(char *dir,void *CInSim, void *Message);    // classname - ��������� �� ����� RCStreet.
    // ����� ��� ������� � ������ ������ ������
    // ��� ����� ����� ��� ���� ����� �������� ���������� ����� ���� ������ � ������ RCStreet
    void readconfig(char *Track); // ������ ������ � ������ "����� ����������"

    void mci();   // ����� � ������� � ����������� � �.�.
    void next_packet(); // ������� ��������� ���� ������

};
#endif // #define _RC_STREET_H
