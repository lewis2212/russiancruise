#include "CarInfo.h"

CarInfo::CarInfo()
{
	//ctor
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
