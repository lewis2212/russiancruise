#include "CarInfo.h"

CarInfo::CarInfo()
{
	Node = 0;
	Lap = 0;
	PLID = 0;
	Position = 0;
	Info = 0;
	Sp3 = 0;
	X = 0;
	Y = 0;
	Z = 0;
	Speed = 0;
	Direction = 0;
	Heading = 0;
	AngVel = 0;
}

CarInfo::~CarInfo()
{
	//dtor
}

CarInfo::CarInfo(const CompCar info)
{
	Node = info.Node;
	Lap = info.Lap;
	PLID = info.PLID;
	Position = info.Position;
	Info = info.Info;
	Sp3 = info.Sp3;
	X = info.X;
	Y = info.Y;
	Z = info.Z;
	Speed = info.Speed;
	Direction = info.Direction;
	Heading = info.Heading;
	AngVel = info.AngVel;
}
