#ifndef CARINFO_H
#define CARINFO_H

#include <CInsim.h>

class CarInfo
{
	public:
		CarInfo();
		CarInfo(CompCar info);
		~CarInfo();

		word	Node;		// current path node
		word	Lap;		// current lap
		byte	PLID;		// player's unique id
		byte	Position;	// current race position : 0 = unknown, 1 = leader, etc...
		byte	Info;		// flags and other info - see below
		byte	Sp3;
		int		X;			// X map (65536 = 1 metre)
		int		Y;			// Y map (65536 = 1 metre)
		int		Z;			// Z alt (65536 = 1 metre)
		word	Speed;		// speed (32768 = 100 m/s)
		word	Direction;	// car's motion if Speed > 0 : 0 = world y direction, 32768 = 180 deg
		word	Heading;	// direction of forward axis : 0 = world y direction, 32768 = 180 deg
		short	AngVel;
	protected:
	private:
};

#endif // CARINFO_H
